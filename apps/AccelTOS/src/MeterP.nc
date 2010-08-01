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

module MeterP
{

	provides {
		interface Meter;
		interface StdControl;
	}
	
	uses
	{
		interface Timer<TMilli>;
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
	uint8_t channels[] = 
	{ 
		SHIMMER_ADC_ACCEL_X, 
		SHIMMER_ADC_ACCEL_Y,
		SHIMMER_ADC_ACCEL_Z,
//		SHIMMER_ADC_GYRO_X, 
//		SHIMMER_ADC_GYRO_Y,
//		SHIMMER_ADC_GYRO_Z,
		SHIMMER_ADC_BATTERY,
		SHIMMER_ADC_TEMP,
	};

	enum
	{
		CHANNEL_COUNT = 5, // FIXME Make automatic?
	};
	
	uint8_t tracker = 0;
	
	void dump(char* msg) {
		if( call DiagMsg.record() ) {
			call DiagMsg.str(msg);
			call DiagMsg.send();
		}	
	}

	command error_t StdControl.stop(){
		// FIXME Implement shut-down
		call LedHandler.error();
		return FAIL;
	}

	command error_t StdControl.start(){
		
		error_t error = SUCCESS;
		
		call LedHandler.set(tracker++);		
		
		error = call AccelInit.init();
		
		if (error) {
			call LedHandler.error();
			dump("AccelInitFail");
		}
		else {
			call Accel.setSensitivity(RANGE_4_0G);
			call Accel.wake(TRUE);
			error = call ShimmerAdc.setChannels(channels, CHANNEL_COUNT);
					
			if(error) {
				call LedHandler.error();
				dump("SetChFail");
			}
			else {
				dump("InitOK");
				call LedHandler.set(tracker++);	
			}
		}
		
		return error;
	}

	//message_t msgBuffer; FIXME Unused?

	event void Timer.fired()
	{
		if( call ShimmerAdc.sample() != SUCCESS ) {
			call LedHandler.errorToggle();// FIXME Why does it fail?
			dump("Sample fail");
			//call LedHandler.error(); 
		}
		else {
			dump("SamplingStarted");
		}

	}

	event void ShimmerAdc.sampleDone(uint32_t timestamp, uint16_t* data)
	{
		error_t error = SUCCESS;
		
		call LedHandler.sampling();

		dump("samplingDone");

		error = call BufferedFlash.send(data - 2, 4 + CHANNEL_COUNT*2); // FIXME Magic numbers
		
		if (error)
		  call LedHandler.errorToggle();
		/*
		if (error)
			call LedHandler.set(2);
		else
			call LedHandler.set(0);

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

	command error_t Meter.stopRecording(){
		
		error_t error = SUCCESS;
		
		if (call Timer.isRunning()) {
			call Timer.stop();
		}
		else {
			error = EALREADY;
		}
		
		return error;		
	}

	command error_t Meter.startRecording(){

		error_t error = SUCCESS;
		
		dump("startRecord");
		
		if (!call Timer.isRunning()) {
			call Timer.startPeriodic(125); // FIXME Nothing happens for dt ms!
		}
		else {
			error = EALREADY;
		}
		
		return error;
	}

}
