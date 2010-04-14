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
#include "printf.h"
module NetTestP{
	uses{
		interface Boot;
		interface TimeSyncPoints;
		interface StdControl;
		interface Leds;
		interface AMSend;
		interface Receive;
		interface Receive as Snoop;
	}
}
implementation{
	message_t mess;
	uint8_t init=0;
	
	event void Boot.booted(){
		call StdControl.start();
	}

	event void TimeSyncPoints.syncPoint(uint32_t localTime, am_addr_t nodeID, uint32_t remoteTime){
		if(TOS_NODE_ID==1&&init==10){	
			mess.data[0]=0;
			init++;
			call AMSend.send(TOS_BCAST_ADDR, &mess, 1);
		} else
			init++;
		if(init==250)
			init=11;
	}

	event void AMSend.sendDone(message_t *msg, error_t error){
		if(TOS_NODE_ID==1){
			mess.data[0]++;
			call AMSend.send(2, &mess, 1);
//			printf("send done\n");
			printfflush();
		}
	}

	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len){
//		if(TOS_NODE_ID!=1)
//			call AMSend.send(TOS_BCAST_ADDR, msg, len);
		return msg;
	}

	event message_t * Snoop.receive(message_t *msg, void *payload, uint8_t len){
		return msg;
	}
}