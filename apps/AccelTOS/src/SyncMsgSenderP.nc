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
* Author: Ali Baharev
*/

#include "message.h"
#include "SyncMsg.h"
#include "Assert.h"

enum {
	UINT32_MAX = 0xffffffff
};

module SyncMsgSenderP {
	
	provides {
		interface StdControl;
	}
	
	uses {
		interface Timer<TMilli>;
   		interface LocalTime<TMilli>;
   		interface TimeSyncAMSend<TMilli, uint32_t>;
   		interface Packet;
   		interface LedHandler;
   		interface SimpleFile;
	}
}

implementation {
	
	bool busy = FALSE;
	
	message_t pkt; // Guarded by busy
	
	uint32_t first_block = UINT32_MAX;
	
	task void sendSyncMsg();

	event void SimpleFile.booted(uint32_t starting_at_block) {
		
		ASSERT(first_block==UINT32_MAX);
		
		first_block = starting_at_block;
	}

	command error_t StdControl.start(){
		
		call Timer.startPeriodic(2048);
		
		return SUCCESS;
	}

	event void Timer.fired() {
		
		ASSERT(first_block!=UINT32_MAX);
		
		post sendSyncMsg();
	}

	command error_t StdControl.stop(){
		
		call Timer.stop();
		
		return SUCCESS;
	}
	
	task void sendSyncMsg() {
		
		error_t error = FAIL;
		SyncMsg* payload = NULL;
		
		if (busy) {
			return;	
		}
		
		payload = call Packet.getPayload(&pkt, PAYLOAD_LENGTH);
		ASSERT(payload != NULL);
		payload->first_block = first_block;
		payload->event_time = call LocalTime.get();
		
		error = call TimeSyncAMSend.send(TOS_BCAST_ADDR, &pkt, PAYLOAD_LENGTH, payload->event_time);
		
		if (error==SUCCESS) {
			busy = TRUE;
			call LedHandler.led1On();
		}
		
		ASSERT(!error);
	}
	
	event void TimeSyncAMSend.sendDone(message_t *msg, error_t error) {
		
		busy = FALSE;
		call LedHandler.led1Off();
		ASSERT(&pkt==msg);
		
		ASSERT(!error);
	}

	event void SimpleFile.readDone(error_t error, uint16_t length){
		// TODO Auto-generated method stub
	}

	event void SimpleFile.seekDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void SimpleFile.formatDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void SimpleFile.appendDone(error_t error){
		// TODO Auto-generated method stub
	}

}
