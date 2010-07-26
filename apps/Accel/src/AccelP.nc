#include "CtrlMsg.h"

module AccelP {
   uses interface Boot;
   uses interface Leds;
   uses interface SplitControl as AMControl;
   uses interface Receive;
   uses interface SimpleFile as SF;
   uses interface SplitControl as SFCtrl;
}
implementation{
   
   event void Boot.booted() {
		// TODO
		error_t error;
		
		call Leds.set(6);
		
		error = call SFCtrl.start();
		
		if (error)
			call Leds.led0On();

		error = call AMControl.start();

		if (error)
			call Leds.led0On();

   }

	event void SFCtrl.startDone(error_t error){
		if (error == SUCCESS)
			call Leds.led2Off();			
		else
			call Leds.led0On();
	}
	
	event void SF.formatDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void SF.appendDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void SF.seekDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void SF.readDone(error_t error, uint16_t length){
		// TODO Auto-generated method stub
	}

	event void SFCtrl.stopDone(error_t error){
		// TODO Auto-generated method stub
	}

	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len){
		if (len == sizeof(CtrlMsg)) { // TODO Enough?
			CtrlMsg* pkt = (CtrlMsg*)payload;
			call Leds.set(pkt->cmd); // TODO Check if meaningful?
		}
		return msg;
	}

	event void AMControl.stopDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void AMControl.startDone(error_t error){
		if (error == SUCCESS)
			call Leds.led1Off();			
		else
			call Leds.led0On();
	}
}
