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

#include "Timer.h"
#include "Assert.h"
#include "SyncMsg.h"
#include "TimeSyncInfo.h"

module SyncMsgReceiverP {
	
	uses {
		interface Receive;
		interface AMPacket;
		interface TimeSyncPacket<TMilli, uint32_t>;
		interface Leds;
		interface BufferedFlash;
	}
}

implementation {
	
	timesync_info_t timesync_info;
	
	task void informBufferedFlash() {
		
		call BufferedFlash.updateTimeSyncInfo(&timesync_info);
	}
	
	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len) {
		
		SyncMsg* data = 0;
		
		ASSERT(len==PAYLOAD_LENGTH);
		
		call Leds.led2Toggle();
		
		if (call TimeSyncPacket.isValid(msg)) {
						
			timesync_info.local_time = call TimeSyncPacket.eventTime(msg);
			
			data = (SyncMsg*) payload;
			
			timesync_info.remote_time = data->event_time;
			
			timesync_info.remote_start = data->first_block;
					
			timesync_info.remote_id = call AMPacket.source(msg);
			
			post informBufferedFlash();
		}
		
		return msg;
	}
}
