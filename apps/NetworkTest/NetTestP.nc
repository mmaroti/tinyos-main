module NetTestP{
	uses{
		interface Boot;
		interface TimeSyncPoints;
		interface StdControl;
		interface Leds;
		interface AMSend[am_id_t id];
		interface Receive[am_id_t id];
		interface Receive as Snoop[am_id_t id];
	}
}
implementation{

	event void Boot.booted(){
		call StdControl.start();
	}

	event void TimeSyncPoints.syncPoint(uint32_t localTime, am_addr_t nodeID, uint32_t remoteTime){
		
	}

	event void AMSend.sendDone[am_id_t id](message_t *msg, error_t error){
		// TODO Auto-generated method stub
	}

	event message_t * Receive.receive[am_id_t id](message_t *msg, void *payload, uint8_t len){
		return msg;
	}

	event message_t * Snoop.receive[am_id_t id](message_t *msg, void *payload, uint8_t len){
		return msg;
	}
}