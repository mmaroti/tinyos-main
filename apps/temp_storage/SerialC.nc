
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





