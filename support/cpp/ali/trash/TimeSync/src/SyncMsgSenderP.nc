
#include "message.h"
#include "SyncMsg.h"
#include "Assert.h"

enum {
	UINT32_MAX = 0xffffffff
};

module SyncMsgSenderP {
	
	provides interface SyncMsgSender;
	
	uses {
   		interface LocalTime<TMilli>;
   		interface TimeSyncAMSend<TMilli, uint32_t>;
   		interface Packet;
	}
}

implementation {
	
	bool busy = FALSE;
	
	message_t pkt; // Guarded by busy
	
	uint32_t first_block = UINT32_MAX;
	
	task void sendSyncMsg();

	command void SyncMsgSender.setFirstBlock(uint32_t position){
		
		ASSERT(first_block==UINT32_MAX);
		
		first_block = position;
	}

	command error_t SyncMsgSender.send() {
		
		ASSERT(first_block!=UINT32_MAX);
		
		return post sendSyncMsg();
	}
	
	task void sendSyncMsg() {
		
		error_t error = FAIL;
		uint32_t* payload = NULL;
		
		if (busy) {
			signal SyncMsgSender.sendDone(EBUSY);	
			return;	
		}
		
		payload = call Packet.getPayload(&pkt, PAYLOAD_LENGTH);
		ASSERT(payload != NULL);
		*payload = first_block;
		
		error = call TimeSyncAMSend.send(TOS_BCAST_ADDR, &pkt, PAYLOAD_LENGTH, call LocalTime.get());
		
		if (error==SUCCESS) {
			busy = TRUE;
		}
		else {
			signal SyncMsgSender.sendDone(error);	
		}
	}
	
	event void TimeSyncAMSend.sendDone(message_t *msg, error_t error) {
		
		busy = FALSE;
		
		ASSERT(&pkt==msg);
		
		signal SyncMsgSender.sendDone(error);
	}

}
