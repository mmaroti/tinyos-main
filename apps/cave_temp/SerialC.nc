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
* Author: Csepe Zoltan
*/

#include <Timer.h>
#include "TempStorage.h"

module SerialC {
	uses interface Boot;
	uses interface Leds;
	uses interface SplitControl as AMControl;
	uses interface Receive;
	uses interface AMSend as SerialAMSend;
	uses interface Receive as SerialReceive; 
	uses interface SplitControl as SerialControl;
}
implementation {

	bool busy = FALSE;
	message_t freeMsg;
	message_t *freeMsgPtr=&freeMsg;
	 
	event void Boot.booted() {
		call SerialControl.start();
	}

	event void SerialControl.startDone(error_t err) {
		if (err == SUCCESS) {
			call AMControl.start();
		}
		else {
			call SerialControl.start();
		}
	}
	
	event void AMControl.startDone(error_t err) {
		if (err != SUCCESS) {
			call AMControl.start();	
		}
	}
	
	event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len) {
		call Leds.led0Toggle();
		if (busy)
			return msg;
		if (call SerialAMSend.send(AM_BROADCAST_ADDR, msg, sizeof(BlinkToRadioMsg))!= SUCCESS){
			return msg;}
			
		busy = TRUE;
		return freeMsgPtr;
	}
	
	event void SerialAMSend.sendDone(message_t* msg, error_t error) {
		freeMsgPtr=msg;
		busy = FALSE;
	}
	
	event message_t* SerialReceive.receive(message_t* msg, void* payload, uint8_t len) {
		if(len==sizeof(BlinkToRadioMsg)){
			BlinkToRadioMsg* btrpkt = (BlinkToRadioMsg*)payload;
			}
	call Leds.led1Toggle();
	return msg;
	} 
	
	event void SerialControl.stopDone(error_t err) {}
	event void AMControl.stopDone(error_t err) {}
}





