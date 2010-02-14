/*
* Copyright (c) 2010, University of Szeged
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

#include <message.h>
#include <Mma7260.h>
#include "ShimmerAdc.h"

module TestShimmerP
{
	uses
	{
		interface Boot;
		interface SplitControl;
		interface Timer<TMilli>;
		interface ShimmerAdc;

		interface Init as AccelInit;
		interface Mma7260 as Accel;

		interface Leds;
		interface DiagMsg;
		interface BufferedSend;

		interface StdControl as Gyro;
	}
}

implementation
{
	event void Boot.booted()
	{
		call SplitControl.start();
	}

	uint8_t channels[] = 
	{ 
		SHIMMER_ADC_ACCEL_X, 
		SHIMMER_ADC_ACCEL_Y,
		SHIMMER_ADC_ACCEL_Z,
		SHIMMER_ADC_GYRO_X, 
		SHIMMER_ADC_GYRO_Y,
		SHIMMER_ADC_GYRO_Z,
		SHIMMER_ADC_BATTERY,
		SHIMMER_ADC_TEMP,
	};

	enum
	{
		CHANNEL_COUNT = 8,
	};

	event void SplitControl.startDone(error_t error)
	{
		if( error == SUCCESS )
		{
			call Leds.led2On();

			call AccelInit.init();
			call Accel.setSensitivity(RANGE_4_0G);
			call Accel.wake(TRUE);

			call Gyro.start();

			if( call ShimmerAdc.setChannels(channels, CHANNEL_COUNT) != SUCCESS )
				call Leds.led1On();

			call Timer.startPeriodic(5);
		}
		else 
			call SplitControl.start();

	}

	event void SplitControl.stopDone(error_t err)
	{
	} 

	message_t msgBuffer;

	event void Timer.fired()
	{
		if( call ShimmerAdc.sample() != SUCCESS )
			call Leds.led1On();
	}

	event void ShimmerAdc.sampleDone(uint32_t timestamp, uint16_t* data)
	{
		call Leds.led0Toggle();

		call BufferedSend.send(data - 2, 4 + CHANNEL_COUNT*2);
/*
		if( call DiagMsg.record() )
		{
			call DiagMsg.uint32(timestamp);
			call DiagMsg.uint16(data[0]);
			call DiagMsg.uint16(data[1]);
			call DiagMsg.uint16(data[2]);
			call DiagMsg.uint16(data[3]);
			call DiagMsg.send();
		}
*/
	}
}
