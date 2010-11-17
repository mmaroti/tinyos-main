
#include "message.h"
#include "Assert.h"

enum {
	
	AM_ID = 0x44
};

module SyncMsgSenderP {
	
	provides interface SyncMsgSender;
	
	uses {
   		interface LocalTime<TMilli>;
   		interface TimeSyncAMSend<TMilli, uint32_t>;
	}
}

implementation {
	
	bool busy = FALSE;
	
	message_t pkt; // Guarded by busy
	
	task void sendSyncMsg() {
		
		error_t error = FAIL;
		
		if (busy) {
			signal SyncMsgSender.sendDone(EBUSY);	
			return;	
		}
		
		error = call TimeSyncAMSend.send(0, &pkt, 0, call LocalTime.get());
		
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

	command error_t SyncMsgSender.send() {
		
		return post sendSyncMsg();
	}
}
