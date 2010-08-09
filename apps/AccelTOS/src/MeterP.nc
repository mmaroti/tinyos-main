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
#include "Assert.h"

module MeterP
{

	provides {
		interface StdControl;
		interface StdControl as Sampling;
	}
	
	uses
	{
		interface ShimmerAdc;

		interface Init as AccelInit;
		interface Mma_Accel as Accel;

		interface LedHandler;
		interface BufferedFlash;

		interface DiagMsg;
	}
}

implementation
{
	enum {
		CHANNEL_COUNT = 7 // FIXME Should be automatic
	};
	
	uint8_t channels[] = 
	{ 
		//SHIMMER_ADC_ZERO,
		SHIMMER_ADC_TIME,
		SHIMMER_ADC_COUNTER,
		SHIMMER_ADC_ACCEL_X, 
		SHIMMER_ADC_ACCEL_Y,
		SHIMMER_ADC_ACCEL_Z,
		SHIMMER_ADC_BATTERY,
		SHIMMER_ADC_TEMP,
	};

	void dump(char* msg) {
		if( call DiagMsg.record() ) {
			call DiagMsg.str(msg);
			call DiagMsg.send();
		}	
	}

	command error_t StdControl.stop(){
		// FIXME Implement shut-down
		ASSERT(FALSE);
		return FAIL;
	}

	command error_t StdControl.start(){
		
		error_t error = SUCCESS;
		
		error = call AccelInit.init();
		
		if (error) {
			ASSERT(FALSE);
		}
		else {
			call Accel.setSensitivity(RANGE_4_0G);
			call Accel.wake(TRUE);
			ASSERT(CHANNEL_COUNT == sizeof(channels));
			error = call ShimmerAdc.setChannels(channels, CHANNEL_COUNT);
					
			if(error) {
				ASSERT(FALSE);
			}
			else {
				dump("InitOK");
			}
		}
		
		return error;
	}

	//message_t msgBuffer; FIXME Unused in TestShimmer?

	event void ShimmerAdc.sampleDone(uint16_t* data, uint8_t length)
	{
		error_t error = SUCCESS;
		
		call LedHandler.sampling();

		//dump("samplingDone");

		error = call BufferedFlash.send(data, 2*length);
		
		ASSERT(error==SUCCESS);
	}

	command error_t Sampling.stop(){
		// FIXME What if sampleDone is pending?
		
		return call ShimmerAdc.stopSampling();		
	}

	command error_t Sampling.start(){

		dump("startRecord");
		
		return call ShimmerAdc.sample(160);
	}

}
