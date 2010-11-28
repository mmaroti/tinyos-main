/** Copyright (c) 2009, University of Szeged
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

#include "HplAtmRfa1Timer.h"

module AppGetTracesP
{
	uses
	{
		interface Boot;
		interface Leds;
		interface AtmegaCounter<uint32_t> as Counter;
		interface DiagMsg;
		interface SplitControl;
	}
}

implementation
{
	enum
	{
		TRACE_COUNT = 10,
		TRACE_SIZE = 6,
	};

	uint8_t traces[TRACE_COUNT][TRACE_SIZE];
	uint8_t traceCount = 0;

	task void reportTask()
	{
		uint8_t i;

		call Leds.led2Toggle();

		if( call DiagMsg.record() )
		{
			call DiagMsg.str("trace");
			call DiagMsg.uint8(traceCount);
			call DiagMsg.send();
		}

		for(i = 0; i < traceCount; ++i)
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.chr('t');
				call DiagMsg.uint8(i);
				call DiagMsg.uint8s(traces[i], TRACE_SIZE);
				call DiagMsg.send();
			}
		}
	}

	uint8_t trace[TRACE_SIZE];

	void insertTrace()
	{
		uint8_t i, j;

		call Leds.led1Toggle();

		// no more room
		if( traceCount >= TRACE_COUNT )
			return;

		for(i = 0; i < traceCount; ++i)
		{
			for(j = 0; j < TRACE_SIZE; ++j)
			{
				if( trace[j] != traces[i][j] )
					break;
			}

			// already there
			if( j == TRACE_SIZE )
				return;
		}

		for(j = 0; j < TRACE_SIZE; ++j)
			traces[traceCount][j] = trace[j];

		++traceCount;
		post reportTask();
	}

	uint8_t random;

	task void randomizer()
	{
		uint8_t a, b;

		for(a = 0; a < random; ++a)
			b = call Counter.get();

		++random;

		post randomizer();
	}

	task void testTimer()
	{
		atomic
		{
			trace[0] = call Counter.get();
			trace[1] = call Counter.test();
			trace[2] = call Counter.get();
			trace[3] = call Counter.test();
			trace[4] = call Counter.get();
			trace[5] = call Counter.test();
		}

		if( trace[2] == 0 )
			insertTrace();

		post testTimer();
	}

	event void Boot.booted()
	{
		call SplitControl.start();
	}

	event void SplitControl.startDone(error_t result)
	{
		call Leds.led0On();
		post randomizer();

		call Counter.setMode(ATMRFA1_CLKSC_RTC);
		call Counter.start();

		post testTimer();
	}

	event void SplitControl.stopDone(error_t result) { }

	async event void Counter.overflow()
	{
	}
}
