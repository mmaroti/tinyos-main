#include "Timer.h"
#include "CtrlMsg.h"

module RadioHandlerP{

   uses {
		interface SplitControl as AMControl;
		interface Receive;
		interface LedHandler;
		interface Timer<TMilli> as TimerRadio;
   }
   
   provides {
		interface SplitControl;
   	}
}

implementation{
	
	// State
	bool SLEEP = TRUE;
	// Mode
	// TODO Sleep-awake-continuous

	command error_t SplitControl.stop(){
		// TODO Finish impl of stop
		// FIXME signal ?
		return call AMControl.stop();
	}

	command error_t SplitControl.start(){
		// FIXME Finish impl of start
		// FIXME signal ?
		return call AMControl.start();
	}
	

	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len){
		if (len == sizeof(CtrlMsg)) { // TODO Enough?
			CtrlMsg* pkt = (CtrlMsg*)payload;
			//call Leds.set(pkt->cmd); // TODO Check if meaningful?
		}
		return msg;
	}

	event void AMControl.stopDone(error_t error){
		if (error == SUCCESS) {
			SLEEP = TRUE;
			call LedHandler.radioOff();
			call TimerRadio.startOneShot(10000);
		}		
		else
			call LedHandler.error();
	}

	event void AMControl.startDone(error_t error){
		if (error == SUCCESS) {
			SLEEP = FALSE;
			call LedHandler.radioOn();
			call TimerRadio.startOneShot(50);
		}		
		else
			call LedHandler.error();
	}

	event void TimerRadio.fired(){

		error_t error;

		if (SLEEP)
			error = call AMControl.start();
		else
			error = call AMControl.stop();
			
		if (error)
			call LedHandler.error();
	}
}