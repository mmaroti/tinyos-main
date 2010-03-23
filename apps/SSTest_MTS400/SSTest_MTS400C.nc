/*
* Copyright (c) 2009, University of Szeged
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
module SSTest_MTS400C{
	uses {
		interface SplitControl;
		interface StreamStorage;
		interface Boot;
		interface Leds;
		interface StdControl;
		interface LocalTime<TMilli>;
		interface Timer<TMilli> as SensorTimer; 
		interface Read<uint16_t> as TRead; 
		interface Read<uint16_t> as HRead;
		interface Read<uint16_t> as LRead;
	}
}
implementation{
	typedef struct asdf{
		uint16_t temp;
		uint16_t humi;
		uint16_t light;
		uint32_t time;
	} one_meas;
	one_meas fresh_data;

	event void Boot.booted(){
		call Leds.set(7);
		//call StreamStorage.erase();
		call SplitControl.start();	
	}
	
	event void StreamStorage.eraseDone(error_t error){
		call Leds.set(0);
		call StdControl.start();
		call SensorTimer.startPeriodicAt(1000,6000L);
	}
	
	event void SplitControl.startDone(error_t error){
		if(error==FAIL)
			call StreamStorage.erase();
		else {
			call Leds.set(0);
			call StdControl.start();
			call SensorTimer.startPeriodicAt(1000,600L);
		}
	}
	
	event void SensorTimer.fired(){
		call TRead.read();
	}
	
	event void TRead.readDone(error_t result, uint16_t val){
		if(result==SUCCESS)
			fresh_data.temp=val;
		else
			fresh_data.temp=0xffff;
		call HRead.read();
	}

	event void HRead.readDone(error_t result, uint16_t val){
		if(result==SUCCESS)
			fresh_data.humi=val;
		else
			fresh_data.humi=0xffff;
		call LRead.read();
	}
	
	event void LRead.readDone(error_t result, uint16_t val){
		if(result==SUCCESS)
			fresh_data.light=val;
		else
			fresh_data.light=0xffff;
		fresh_data.time=call LocalTime.get();
		call StreamStorage.append(&fresh_data, sizeof(fresh_data));
	}
	

	event void StreamStorage.appendDone(void* buf, uint16_t  len, error_t error){
		call Leds.led0Toggle();
	}
	
	event void StreamStorage.appendDoneWithID(void* buf, uint16_t  len, error_t error){
		// TODO Auto-generated method stub
	}	
	
	event void StreamStorage.syncDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void SplitControl.stopDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void StreamStorage.getMinAddressDone(uint32_t addr){
		// TODO Auto-generated method stub
	}

	event void StreamStorage.readDone(void *buf, uint8_t len, error_t error){
		// TODO Auto-generated method stub
	}

	

	
}