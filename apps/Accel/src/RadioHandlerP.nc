#include "CtrlMsg.h"
#include "ReportMsg.h"

module RadioHandlerP{

   uses {
		interface SplitControl as AMControl;
		interface Receive;
		interface AMSend;
		interface LedHandler;
		interface Timer<TMilli> as WatchDog;
		interface Timer<TMilli> as ShortPeriod;
   }
   
   provides {
		interface StdControl;
   	}
}

implementation{
	
	enum {
		SLEEP,
		AWAKE,
	};
	
	enum {
		ALTERING,
		CONTINUOUS
	};
	
	bool state = SLEEP;
	bool mode  = ALTERING;
	
	bool sending = FALSE;
	message_t report;
	
	error_t broadcast() {

		error_t error;
		
		if (sending) {
			error =  EBUSY;
		}
		else {
			// TODO Explain why
    		// Note that we could have avoided using the Packet interface, as it's 
    		// getPayload command is repeated within AMSend.
    		ReportMsg* pkt = (ReportMsg*)(call AMSend.getPayload(&report, NULL));
    		pkt->id = TOS_NODE_ID;
    		pkt->mode = mode;
 			error = call AMSend.send(AM_BROADCAST_ADDR, &report, sizeof(ReportMsg));
    		if (error == SUCCESS) {
      			sending = TRUE;
    		}
    	}
    	return error;
	}
	
	event void AMSend.sendDone(message_t *msg, error_t error){
		sending = FALSE;
		// FIXME Resend if failed?
	}
	

	command error_t StdControl.stop(){
		// TODO Finish impl of stop
		return FAIL;
	}

	// FIXME StdControl is not an appropriate interface
	command error_t StdControl.start(){
		// FIXME Finish impl of start

		return call AMControl.start();
		
	}
	

	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len){
		
		if (len == sizeof(CtrlMsg)) {  // TODO Enough?
		
			CtrlMsg* pkt = (CtrlMsg*)payload;
			
			uint8_t newState = pkt->cmd;
			
			call LedHandler.msgReceived();

			if      (newState == ALTERING)   {
				mode = ALTERING;
				call ShortPeriod.startOneShot(50);
			}
			else if (newState == CONTINUOUS) {
				mode = CONTINUOUS;
			}
			else {// FIXME Unknown mode received
				call LedHandler.error();
			}
		}

		return msg;
	}

	event void AMControl.stopDone(error_t error) {

		if (error == SUCCESS) {
			state = SLEEP;
			call LedHandler.radioOff();
		}		
		else
			call LedHandler.error();
	}

	event void AMControl.startDone(error_t error) {
		
		if (error == SUCCESS) {
			
			state = AWAKE;
			call LedHandler.radioOn();
			
			broadcast();
			
			call ShortPeriod.startOneShot(50);
			if (! call WatchDog.isRunning()) {
				call WatchDog.startPeriodic(1000);
			}
		}		
		else
			call LedHandler.error();
	}

	event void WatchDog.fired(){

		error_t error = SUCCESS;

		// S A -> start
		// S C -> start
		// A A -> stop
		// A C -> nothing, stay awake

		if      (state == SLEEP) {
			error = call AMControl.start();
		}
		else {
			broadcast();
			if (mode == ALTERING)
				call ShortPeriod.startOneShot(200);
		}

		if (error != SUCCESS)
			call LedHandler.error();
	}
	
	event void ShortPeriod.fired() {

		if (state == AWAKE && mode == ALTERING) {

			if (call AMControl.stop() != SUCCESS)
				call LedHandler.error();
		}
	}
}