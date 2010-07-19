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
* Author:Andras Biro
*/

#include "EchoRanger.h"
#ifndef SAMP_T
	#define SAMP_T 60000U
#endif

module ApplicationM{
	uses {
		interface SplitControl;
		interface StreamStorage;
		interface Boot;
		interface Leds;
		interface StdControl;
		interface LocalTime<TMilli>;
		interface Timer<TMilli> as SensorTimer; 
		interface Read<echorange_t*>; 
		interface Get<uint16_t*> as LastBuffer;
		interface Get<echorange_t*> as LastRange;
	}
}

implementation{
	
	uint8_t counter=0;

	event void Boot.booted(){
		call Leds.set(7);
		call SplitControl.start();	
	}
	
	event void StreamStorage.eraseDone(error_t error){
		call Leds.set(0);
		call StdControl.start();
		call SensorTimer.startPeriodic(SAMP_T);
	}
	
	event void SplitControl.startDone(error_t error){
		if(error==FAIL){
			call StreamStorage.erase();
		}else {
			call Leds.set(0);
			call StdControl.start();
			call SensorTimer.startPeriodic(SAMP_T);
		}
	}
	
	event void SensorTimer.fired(){
		call Read.read();
	}
	
	event void Read.readDone(error_t result, echorange_t* range){
		if(result==SUCCESS)
			call StreamStorage.appendWithID(0x00,range, sizeof(echorange_t));
			//call StreamStorage.append(range, sizeof(echorange_t));
	}
	
	event void StreamStorage.appendDoneWithID(void* buf, uint16_t  len, error_t error){
		counter++;
		if(counter>4)
			counter=0;
		else if(counter==4){
			uint16_t* buffer=call LastBuffer.get();
			call StreamStorage.appendWithID(0x11,buffer, sizeof(uint16_t)*ECHORANGER_BUFFER);
		}
	}	
	
	event void StreamStorage.appendDone(void* buf, uint16_t  len, error_t error){
	}

	event void StreamStorage.syncDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void SplitControl.stopDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void StreamStorage.getMinAddressDone(uint32_t addr, error_t error){
		// TODO Auto-generated method stub
	}

	event void StreamStorage.readDone(void *buf, uint8_t len, error_t error){
		// TODO Auto-generated method stub
	}
}
