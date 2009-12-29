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
/*
	at 7372800 Hz MCU clock and 13 ADC clock per samples

	prescaler = 2,	freq = 283569 Hz, period = 3 us
	prescaler = 4,	freq = 141784 Hz, period = 7 us
	prescaler = 8,	freq = 70892 Hz,  period = 14 us
	prescaler = 16, freq = 35446 Hz,  period = 28 us
	prescaler = 32, freq = 17723 Hz,  period = 56 us
	prescaler = 64, freq = 8861 Hz,   period = 112 us
	prescaler = 128, freq = 4430 Hz,  period = 225 us

	maxWorkCounter = 46881 (when not sampling)
*/


	enum
	{
		BUFFER_COUNT = 3,
		BUFFER_SIZE = 1000,
		SAMPLING = 14,
	};

	uint16_t buffers[BUFFER_COUNT][BUFFER_SIZE];

	uint32_t sampleCount;	// number of samples per second
	uint32_t workCounter;	// number of empty task executions per second

	event void ReadStream.bufferDone(error_t result, uint16_t* buf, uint16_t count)
	{
		if( result == SUCCESS )
		{
			sampleCount += BUFFER_SIZE;

			result = call ReadStream.postBuffer(buf, count);
		}

		if( result != SUCCESS )
			call Leds.led0Toggle();
		else
			call Leds.led1Toggle();
	}

	event void Timer.fired()
	{
		if( call DiagMsg.record() )
		{
			call DiagMsg.uint32(sampleCount);
			call DiagMsg.uint32(workCounter);
			call DiagMsg.uint16((uint16_t)(100 - workCounter / 469));	// sampling overhead in %
			call DiagMsg.send();
		}

		sampleCount = 0;
		workCounter = 0;
	}

	event void ReadStream.readDone(error_t result, uint32_t usActualPeriod)
	{
		call Leds.led2Toggle();
	}

	event void Boot.booted()
	{
		call SplitControl.start();
	}
	
	task void workTask()
	{
		++workCounter;
		post workTask();
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
		post workTask();
	}
	
	event void SplitControl.stopDone(error_t err)
	{
		ASSERT(FALSE);
	} 
}

