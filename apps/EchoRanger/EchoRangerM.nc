/** Copyright (c) 2010, University of Szeged
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

#include "message.h"
#include "EchoRanger.h"

module EchoRangerM
{
	provides
	{
		interface Read<echorange_t*> as EchoRanger;
	}

	uses
	{
		interface Boot;
		interface Leds;
		interface AMSend;
		interface GeneralIO as SounderPin;
		interface ReadStream<uint16_t> as MicRead;
		interface MicSetting;
		interface Alarm<TMicro, uint16_t> as Alarm;
		interface LocalTime<TMicro>;
	}
}

implementation
{
	enum
	{
		SAMPLING = 56,		// sampling rate in microsec (17723 Hz)
		BUFFER = 1024,		// size of buffer, must be at least 4
		BEEP = 500,		// the length of beep in microsec
		SILENCE = 16,		// number of samples before echo
		MATCH = 16,		// number of samples matching the sine wave
		SENDSIZE = 50,		// number of samples in a single message
	};

	event void Boot.booted()
	{
		call SounderPin.clr();
		call MicSetting.gainAdjust(0xff);
	}

	uint16_t buffer[BUFFER];

	uint8_t state;
	enum
	{
		STATE_READY = 0,
		STATE_WARMUP = 1,
		STATE_LISTEN = 2,
		STATE_PROCESS = 3,
	};

	command error_t EchoRanger.read()
	{
		if( state == STATE_READY )
		{
			call Leds.led0On();

			state = STATE_WARMUP;
			call MicRead.postBuffer(buffer + BUFFER - 2, 2);
			call MicRead.postBuffer(buffer, BUFFER);
			call MicRead.read(SAMPLING);

			return SUCCESS;
		}
		else
			return FAIL;
	}

 	event void MicRead.bufferDone(error_t result, uint16_t* bufPtr, uint16_t count)
	{
		if( state == STATE_WARMUP )
		{
			state = STATE_LISTEN;

			call Alarm.start(BEEP);
			call Leds.led1On();
			call SounderPin.set();
		}
	}

	async event void Alarm.fired()
	{
		call Leds.led1Off();
		call SounderPin.clr();
	}

	task void process();

	event void MicRead.readDone(error_t result, uint32_t usActualPeriod)
	{
		if( state == STATE_LISTEN )
		{
			state = STATE_PROCESS;
			post process();
		}
	}

	async event error_t MicSetting.toneDetected()
	{
		return SUCCESS;
	}

	echorange_t range;

	uint16_t getAverage()
	{
		uint32_t sum = 0;
		uint16_t i;

		for(i = 0; i < BUFFER; ++i)
			sum += buffer[i];

		return (uint16_t)(sum / BUFFER);
	}

	uint16_t getVolume(uint16_t start, uint8_t length)
	{
		uint16_t volume = 0;
		int16_t sample;

		while( --length >= 0 )
		{
			sample = buffer[start++] - range.average;

			if( sample < 0 )
				sample = -sample;

			volume += sample;
		}

		return volume;
	}

	int16_t matchTable[MATCH] = {70, 56, -323, -57, 500, -59, -447, 258, 407, -411, -283, 227, 329, -154, -402, 114};

	uint16_t getMatch(uint16_t start)
	{
		int16_t sample;
		int16_t theory;
		uint16_t index;
		uint16_t match;
		uint16_t volume;

		match = getVolume(start - SILENCE, SILENCE);

		volume = getVolume(start, 16);

		for(index = start; index < start + 16; ++index)
		{
			sample = buffer[index] - range.average;
			theory = ((uint32_t)(matchTable[index - start]) * volume) >> 8;

			if( sample > theory )
				match += sample - theory;
			else
				match += theory - sample;
		}

		return match;		
	}

	task void process()
	{
		range.sequence += 1;
		range.timestamp = call LocalTime.get();
		range.average = getAverage();

		state = STATE_READY;
		signal EchoRanger.readDone(SUCCESS, &range);
	}

// -----------------------

	typedef struct report_t
	{
		uint16_t index;
		uint16_t data[SENDSIZE];
	} report_t;

	uint16_t reportIndex;
	message_t reportMsg;

	task void reportTask()
	{
		report_t* report;
		uint8_t i;

		if( reportIndex <= BUFFER - SENDSIZE )
		{
			report = (call AMSend.getPayload(&reportMsg, sizeof(report_t)));

			report->index = reportIndex;
			for(i = 0; i < SENDSIZE; ++i)
				report->data[i] = buffer[reportIndex++];

			if( call AMSend.send(0xFFFF, &reportMsg, sizeof(report_t)) != SUCCESS )
				state = STATE_READY;
		}
		else
			state = STATE_READY;
	}

 	event void AMSend.sendDone(message_t* msg, error_t error)
	{
		if( error == SUCCESS )
			call Leds.led2Toggle();

		post reportTask();
  	}

}
