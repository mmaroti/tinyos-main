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

#include "Assert.h"

module TestFastAdcM
{
	uses
	{
		interface ReadStream<uint16_t>;
		interface Boot;
		interface SplitControl;
		interface Timer<TMilli>;

		interface Leds;
		interface DiagMsg;
	}
}

implementation
{
	enum
	{
		BUFFER_COUNT = 3,
		BUFFER_SIZE = 50,
		SAMPLING = 50,
	};

	uint16_t buffers[BUFFER_COUNT][BUFFER_SIZE];

	uint16_t lastSample = -BUFFER_SIZE;
	
	uint16_t sampleCount;
	uint16_t missedCount;

	event void ReadStream.bufferDone(error_t result, uint16_t* buf, uint16_t count)
	{
		if( result == SUCCESS )
		{
			atomic
			{
				sampleCount += BUFFER_SIZE;
				missedCount += *buf - lastSample - BUFFER_SIZE;
			}

			lastSample = *buf;

			result = call ReadStream.postBuffer(buf, count);
		}

		if( result != SUCCESS )
			call Leds.led0Toggle();
		else
			call Leds.led1Toggle();
	}

	event void Timer.fired()
	{
		uint16_t s, m;

		atomic
		{
			s = sampleCount;
			m = missedCount;
			sampleCount = 0;
			missedCount = 0;
		}

		if( call DiagMsg.record() )
		{
			call DiagMsg.uint16(s);
			call DiagMsg.uint16(m);
			call DiagMsg.send();
		}
	}

	event void ReadStream.readDone(error_t result, uint32_t usActualPeriod)
	{
		call Leds.led2Toggle();
	}

	event void Boot.booted()
	{
		call SplitControl.start();
	}
	
	event void SplitControl.startDone(error_t error)
	{
		uint8_t i;

		ASSERT( error == SUCCESS );

		for(i = 0; i < BUFFER_COUNT; ++i)
		{
			error = call ReadStream.postBuffer(buffers[i], BUFFER_SIZE);
			ASSERT( error == SUCCESS );
		}

		error = call ReadStream.read(SAMPLING);
		ASSERT( error == SUCCESS );

		call Timer.startPeriodic(1024);
	}
	
	event void SplitControl.stopDone(error_t err)
	{
		ASSERT(FALSE);
	} 
}

