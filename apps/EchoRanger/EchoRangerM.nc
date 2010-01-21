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

module EchoRangerM
{
	uses
	{
		interface Boot;
		interface Leds;
		interface Timer<TMilli> as TimerMilli;
		interface SplitControl as AMControl;
		interface AMSend;
		interface GeneralIO as SounderPin;
		interface ReadStream<uint16_t> as MicRead;
		interface Alarm<TMicro, uint16_t> as Alarm;
	}
}

implementation
{
	enum
	{
		SAMPLING = 56,		// in microsec (17723 Hz)
		BUFFER = 100,		// must be at least 4
		BEEP = 2000,		// in microsec
	};

	event void Boot.booted()
	{
		call SounderPin.clr();
		call AMControl.start();
	}

	event void AMControl.startDone(error_t err)
	{
		if( err == SUCCESS )
			call TimerMilli.startPeriodic(2000);
		else
			call AMControl.start();
	}

	event void AMControl.stopDone(error_t err)
	{
	}

	uint16_t buffer[BUFFER];

	uint8_t state;
	enum
	{
		STATE_READY = 0,
		STATE_WARMUP = 1,
		STATE_LISTEN = 2,
		STATE_REPORT = 3,
	};

	event void TimerMilli.fired()
	{
		if( state == STATE_READY )
		{
			call Leds.led0Toggle();

			state = STATE_WARMUP;
			call MicRead.postBuffer(buffer + BUFFER - 2, 2);
			call MicRead.postBuffer(buffer, BUFFER);
			call MicRead.read(SAMPLING);
		}
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

	typedef struct report_t
	{
		uint8_t index;
		uint16_t data[10];
	} report_t;

	uint8_t reportIndex;
	message_t reportMsg;

	task void reportTask()
	{
		report_t* report;
		uint8_t i;

		if( reportIndex < BUFFER )
		{
			report = (call AMSend.getPayload(&reportMsg, sizeof(report_t)));

			report->index = reportIndex;
			for(i = 0; i < 10; ++i)
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

	event void MicRead.readDone(error_t result, uint32_t usActualPeriod)
	{
		if( state == STATE_LISTEN )
		{
			state = STATE_REPORT;
			reportIndex = 0;
			post reportTask();
		}
	}
}
