#include "Assert.h"

module MainAppP {

   uses {
   		interface Boot;
   		interface SplitControl as TimeSyncMsg;
   		interface Timer<TMilli>;
   		interface Leds;
   		interface SyncMsgSender;
   }
}

implementation {
	
	task void startTimeSyncMsg();

	event void Boot.booted() {
		
		post startTimeSyncMsg();		
	}

	task void startTimeSyncMsg() {
		
		if (call TimeSyncMsg.start() != SUCCESS)
			post startTimeSyncMsg();
	}
	
	event void TimeSyncMsg.startDone(error_t error){
		
		if (error) {
			post startTimeSyncMsg();
		}
		else {
			call SyncMsgSender.setFirstBlock(4096);
			call Timer.startPeriodic(1024);
		}
	}
	
	event void Timer.fired(){
		
		call Leds.led1Toggle();
		call SyncMsgSender.send();
	}
	
	event void SyncMsgSender.sendDone(error_t error){
		
		ASSERT(!error);
	}

	event void TimeSyncMsg.stopDone(error_t error){
		
		ASSERT(!error);
	}
}
