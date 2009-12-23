/*
* Copyright (c) 2009, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Miklos Maroti
*/

#include "Adc.h"
#include "Assert.h"

module AdcStreamP
{
	provides
	{
		interface ReadStream<uint16_t>[uint8_t client];
	}

	uses
	{
		interface Atm128AdcMultiple;
		interface Atm128AdcConfig[uint8_t client];
		interface Atm128Calibrate;
		interface DiagMsg;
	}
}

#ifdef ADC_DEBUG
#define ADC_ASSERT ASSERT
#else
#define ADC_ASSERT for(;0;)
#endif

implementation 
{
	enum
	{
		STATE_READY = 0,

		STATE_20 = 2,		// 2 buffers to be filled, 0 to be reported
		STATE_11 = 3,		// 1 buffer to be filled, 1 to be reported
		STATE_02 = 4,		// 0 buffer to be filled, 2 to be reported
		STATE_10 = 7,		// 1 buffer to be filled, 0 to be reported
		STATE_01 = 8,		// 0 buffer to be filled, 1 to be reported
		STATE_00 = 12,		// error reporting

		SAMPLING_STEP = 1,	// state increment after sampling
		REPORTING_STEP = 4,	// state increment after reporting
		SAMPLING_MASK = 0x03,
	};

	norace uint8_t state;

	uint16_t * firstStart;
	uint16_t firstLength;

	norace uint16_t * secondStart;
	norace uint16_t secondLength;

	// ------- Fast path

	norace uint16_t * currentPtr;
	norace uint16_t * currentEnd;

	task void bufferDone();

	async event bool Atm128AdcMultiple.dataReady(uint16_t data, bool precise, uint8_t channel,
		uint8_t *newChannel, uint8_t *newRefVoltage)
	{
		uint8_t s;

		// this should not be here, but a new interrupt can come
		// before the ADC is stopped, so we have to ignore that
		atomic
		{
			if( (state & SAMPLING_MASK) == 0 )
				return FAIL;
		}

		ADC_ASSERT( currentPtr != NULL && currentPtr < currentEnd );
//		ADC_ASSERT( state == STATE_20 || state == STATE_11 || state == STATE_10 );

		atomic
		{
			*(currentPtr++) = data;

			if( currentPtr != currentEnd )
				return TRUE;

			currentPtr = secondStart;
			currentEnd = currentPtr + secondLength;
			s = (state += SAMPLING_STEP);
		}

		post bufferDone();

		return s == STATE_11;
	}

	// ------- Slow path

	enum {
		CLIENTS = uniqueCount(UQ_ADC_READSTREAM),
	};

	uint8_t client;
	uint16_t actualPeriod;

	typedef struct free_buffer_t
	{
		uint16_t count;
		struct free_buffer_t * next;
	} free_buffer_t;

	free_buffer_t * freeBuffers[CLIENTS];

	task void bufferDone()
	{
		uint8_t s;

		uint16_t * reportStart = firstStart;
		uint16_t reportLength = firstLength;

		ADC_ASSERT( state == STATE_11 || state == STATE_02 || state == STATE_01 || state == STATE_00 );
/*
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("done");
			call DiagMsg.uint8(state);
			call DiagMsg.uint8(freeBuffers[client] != 0);
			call DiagMsg.send();
		}
*/
		firstStart = secondStart;
		firstLength = secondLength;

		atomic
		{
			s = state;

			if( s == STATE_11 && freeBuffers[client] != NULL )
			{
				secondStart = (uint16_t *)freeBuffers[client];
				secondLength = freeBuffers[client]->count;
				freeBuffers[client] = freeBuffers[client]->next;

				state = STATE_20;
			}
			else if( s != STATE_00 )
				state = s + REPORTING_STEP;
		}

		if( s != STATE_00 || freeBuffers[client] != NULL )
		{
			if( s == STATE_00 )
			{
				reportStart = (uint16_t *)freeBuffers[client];
				reportLength = freeBuffers[client]->count;
				freeBuffers[client] = freeBuffers[client]->next;
			}

			signal ReadStream.bufferDone[client](s != STATE_00 ? SUCCESS : FAIL, reportStart, reportLength);
		}

		if( freeBuffers[client] == NULL && (s == STATE_00 || s == STATE_01) )
		{
			signal ReadStream.readDone[client](s == STATE_01 ? SUCCESS : FAIL, actualPeriod); 
			state = STATE_READY;
		}
		else if( s != STATE_11 )
			post bufferDone();
	}

	command error_t ReadStream.postBuffer[uint8_t c](uint16_t *buffer, uint16_t count)
	{
		free_buffer_t * * last;

		if( count < (sizeof(free_buffer_t) + 1) >> 1 )
			return ESIZE;
/*
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("post");
			call DiagMsg.uint8(state);
			call DiagMsg.uint8(freeBuffers[c] != 0);
			call DiagMsg.send();
		}
*/
		atomic
		{
			if( state == STATE_10 )
			{
				secondStart = buffer;
				secondLength = count;

				state = STATE_20;
				return SUCCESS;
			}
		}

		last = & freeBuffers[c];

		// TODO: setup next buffer
		
		while( *last != NULL )
			last = &((*last)->next);
	
		*last = (free_buffer_t *)buffer;
		(*last)->count = count;
		(*last)->next = NULL;

		return SUCCESS;
	}

#define PERIOD(prescaler) (uint16_t)(13 * prescaler / PLATFORM_MHZ)

	command error_t ReadStream.read[uint8_t c](uint32_t period)
	{
		uint8_t prescaler;

		if( state != STATE_READY )
			return EBUSY;

		if( freeBuffers[c] == NULL )
			return FAIL;

		firstStart = (uint16_t *)freeBuffers[c];
		firstLength = freeBuffers[c]->count;
		freeBuffers[c] = freeBuffers[c]->next;

		currentPtr = firstStart;
		currentEnd = firstStart + firstLength;

		if( freeBuffers[c] == NULL )
			state = STATE_10;
		else
		{
			secondStart = (uint16_t *)freeBuffers[c];
			secondLength = freeBuffers[c]->count;
			freeBuffers[c] = freeBuffers[c]->next;

			state = STATE_20;
		}

		if( period > PERIOD(96) )
		{
			prescaler = ATM128_ADC_PRESCALE_128;
			period = PERIOD(128);
		}
		else if( period > PERIOD(48) )
		{
			prescaler = ATM128_ADC_PRESCALE_64;
			period = PERIOD(64);
		}
		else if( period > PERIOD(24) )
		{
			prescaler = ATM128_ADC_PRESCALE_32;
			period = PERIOD(32);
		}
		else if( period > PERIOD(12) )
		{
			prescaler = ATM128_ADC_PRESCALE_16;
			period = PERIOD(16);
		}
		else if( period > PERIOD(6) )
		{
			prescaler = ATM128_ADC_PRESCALE_8;
			period = PERIOD(8);
		}
		else if( period > PERIOD(3) )
		{
			prescaler = ATM128_ADC_PRESCALE_4;
			period = PERIOD(4);
		}
		else
		{
			prescaler = ATM128_ADC_PRESCALE_2;
			period = PERIOD(2);
		}

		client = c;
		actualPeriod = period;	// TODO: correct for MHZ differences

		call Atm128AdcMultiple.getData(call Atm128AdcConfig.getChannel[client](), 
			call Atm128AdcConfig.getRefVoltage[client](), FALSE, prescaler);

		return SUCCESS;
	}

// -------  Configuration defaults (Read ground fast!)

	default async command uint8_t Atm128AdcConfig.getChannel[uint8_t c]() {
		return ATM128_ADC_SNGL_GND;
	}

	default async command uint8_t Atm128AdcConfig.getRefVoltage[uint8_t c]() {
		return ATM128_ADC_VREF_OFF;
	}

	default async command uint8_t Atm128AdcConfig.getPrescaler[uint8_t c]() {
		return ATM128_ADC_PRESCALE_2;
	}
}
