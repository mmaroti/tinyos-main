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

module StreamUploaderTestC{
	uses {
		interface SplitControl;
		interface StreamStorage;
		interface Boot;
		interface Leds;
		interface SplitControl as AMControl;
	}
}
implementation{
	uint16_t buffer;
	uint32_t counter=0;
	uint8_t readbuf[100],j=2;
	
	event void Boot.booted(){
		buffer=0;
		call StreamStorage.erase();	
	}
	
	event void SplitControl.startDone(error_t error){
		
		call Leds.set(3);
		//call StreamStorage.read(770,&readbuf,20);
		call StreamStorage.appendWithID(15,&buffer, sizeof(buffer));
	}

	event void StreamStorage.appendDone(void* buf, uint8_t  len, error_t error){
		buffer++;
		if(buffer<300){
			call StreamStorage.append(&buffer, sizeof(buffer));
		}else{
			call StreamStorage.sync();
		}
	}
	
	event void StreamStorage.appendDoneWithID(void* buf, uint8_t  len, error_t error){
		buffer++;
		counter++;
		if(counter<10000){
			call StreamStorage.appendWithID(15,&buffer, sizeof(buffer));
		}else{
			call StreamStorage.sync();
		}
	}

	event void StreamStorage.readDone(void* buf, uint8_t  len, error_t error){

	}
	
	event void StreamStorage.eraseDone(error_t error){
		//call Leds.set(0);
		//call StreamStorage.append(UINT16_T, &buffer, sizeof(buffer));
		call SplitControl.start();
	}
	
	event void StreamStorage.syncDone(error_t error){
		call AMControl.start();
	}

	event void AMControl.startDone(error_t error){
		call Leds.set(7);
	}

	event void SplitControl.stopDone(error_t error){
	}
	
	event void StreamStorage.getMinAddressDone(uint32_t minaddr){
	}



	event void AMControl.stopDone(error_t error){
	}


}