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

		interface Get<uint16_t*> as LastBuffer;
		interface Get<echorange_t*> as LastRange;
	}

	uses
	{
		interface Boot;
		interface Leds;
		interface GeneralIO as SounderPin;
		interface ReadStream<uint16_t> as MicRead;
		interface MicSetting;
		interface Alarm<TMicro, uint16_t> as Alarm;
		interface LocalTime<TMicro>;
		interface Read<uint16_t> as ReadTemp;
	}
}

implementation
{
	event void Boot.booted()
	{
		call SounderPin.clr();
		call MicSetting.gainAdjust(0xff);
	}

	uint16_t buffer[ECHORANGER_BUFFER];
	echorange_t range;

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
			call MicRead.postBuffer(buffer + ECHORANGER_BUFFER - 2, 2);
			call MicRead.postBuffer(buffer, ECHORANGER_BUFFER);
			call MicRead.read(ECHORANGER_SAMPLING);

			range.temperature = 0xFFFF;
			call ReadTemp.read();

			return SUCCESS;
		}
		else
			return FAIL;
	}

	event void ReadTemp.readDone(error_t result, uint16_t value)
	{
		if( state == STATE_WARMUP && result == SUCCESS )
			range.temperature = value;
	}

 	event void MicRead.bufferDone(error_t result, uint16_t* bufPtr, uint16_t count)
	{
		if( state == STATE_WARMUP )
		{
			state = STATE_LISTEN;

			call Alarm.start(ECHORANGER_BEEP);
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

	void calcAverage()
	{
		uint32_t sum = 0;
		uint16_t i;

		for(i = 0; i < ECHORANGER_BUFFER; ++i)
			sum += buffer[i];

		range.average = sum / ECHORANGER_BUFFER;
	}

	// must be 16 samples after start and 16 samples before end
	int16_t getScore(uint16_t start)
	{
		int16_t score = 0;
		int16_t s;
		uint16_t i;
		
		for(i = start - 16; i < start; ++i)
		{
			s = buffer[i] - range.average;
			if( s >= 0 )
				score -= s;
			else
				score += s;
		}

		score <<= 2;
		
		// filter coefficients: { 1, 0, -3, 0, 4, 0, -3, 2, 3, -3, -2, 2, 3, -1, -3, 1 }
		// filter indices:        0, 1,  2, 3, 4, 5,  6, 7, 8,  9, 10,11,12, 13, 14,15
		
		// coef 1
		s = buffer[start] - buffer[start+13] + buffer[start+15];
		score += s;

		// coef 2
		s = buffer[start+7] - buffer[start+10] + buffer[start+11];
		score += s << 1;

		// coef 3
		s = - buffer[start+2] - buffer[start+6] + buffer[start+8] - buffer[start+9] + buffer[start+12] - buffer[start+14];
		score += s + (s << 1);

		// coef 4
		s = buffer[start+4];
		score += s << 2;

		// compensate for the average value
		score -= range.average * (1-1+1+2-2+2-3-3+3-3+3-3+4);

		return score;
	}

	bool overlaps(uint16_t index, uint16_t r)
	{
		int16_t s = index - r;
		return -16 <= s && s <= 16;
	}

	void findBestScore(uint8_t scan)
	{
		uint16_t i;
		uint16_t r = 0;
		int16_t m = -32767;
		int16_t a;

		for(i = 16; i <= ECHORANGER_BUFFER - 16; ++i)
		{
			if( scan >= 1 && overlaps(i, range.range1) )
				continue;
			else if( scan >= 2 && overlaps(i, range.range2) )
				continue;

			a = getScore(i);
			if( m < a )
			{
				m = a;
				r = i;
			}
		}

		if( scan == 0 )
		{
			range.range1 = r;
			range.score1 = m;
		}
		else if( scan == 1 )
		{
			range.range2 = r;
			range.score2 = m;
		}
		else
		{
			range.range3 = r;
			range.score3 = m;
		}
	}

	task void process()
	{
		range.seqno += 1;
		range.timestamp = call LocalTime.get();
		calcAverage();

		findBestScore(0);
		findBestScore(1);
		findBestScore(2);

		state = STATE_READY;
		signal EchoRanger.readDone(SUCCESS, &range);
	}

	command uint16_t* LastBuffer.get()
	{
		return buffer;
	}

	command echorange_t* LastRange.get()
	{
		return &range;
	}
}
