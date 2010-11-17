
#include "Timer.h"
#include "Assert.h"
#include "SyncMsg.h"

module SyncMsgReceiverP {
	
	uses {
		interface Receive;
		interface AMPacket;
		interface TimeSyncPacket<TMilli, uint32_t>;
		interface Leds;
	}
}

implementation{
	
	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len) {
				
		ASSERT(len==PAYLOAD_LENGTH);
		
		call Leds.led2Toggle();
		
		if (call TimeSyncPacket.isValid(msg)) {
						
			uint32_t local_time = call TimeSyncPacket.eventTime(msg);
			
			SyncMsg* data = (SyncMsg*) payload;
			
			uint32_t remote_time = data->event_time;
			
			uint32_t first_block = data->first_block;
					
			uint16_t source = call AMPacket.source(msg);

		}
		
		return msg;
	}
}
