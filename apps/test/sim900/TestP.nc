#include "gsm.h"

module TestP{
	uses interface Boot;
	uses interface AMSend as GSMSend;
	uses interface Receive as GSMReceive;
	uses interface SplitControl as GSMSplitControl;
	uses interface SplitControl as RadioSplitControl;
	uses interface Leds;
	uses interface DiagMsg;
		uses interface Receive as RadioReceive;
	uses interface AMSend as RadioSend;
}
implementation{

	message_t test;


	event void Boot.booted(){
		
		if(call RadioSplitControl.start()!=SUCCESS)
			call RadioSplitControl.start();
	
	}	
	
	event void RadioSplitControl.startDone(error_t error){
		
		if (error != SUCCESS)
			call RadioSplitControl.start();
		else{			
			if(call GSMSplitControl.start()!=SUCCESS)
				call GSMSplitControl.start();

		}
	}	

	event void GSMSplitControl.startDone(error_t error){
		if(error!=SUCCESS){
			call GSMSplitControl.start();
		}else{
			if(call DiagMsg.record()){
				call DiagMsg.str("startDone! ");
				call DiagMsg.uint8(error);
				call DiagMsg.send();
			}
			call Leds.led0On();
		}	
	
	}
	
	event message_t * RadioReceive.receive(message_t *msg, void *payload, uint8_t len){
			
		if(call GSMSend.send(TOS_BCAST_ADDR,msg,len)==SUCCESS){
			call Leds.led1Toggle();
		}
		return msg;
	}
	
	event void GSMSend.sendDone(message_t *msg, error_t error){	
		if(error==SUCCESS){
			call Leds.led1Toggle();
		}
		
	}
	
	event message_t * GSMReceive.receive(message_t *msg, void *payload, uint8_t len){
			
		if(call RadioSend.send(TOS_BCAST_ADDR,msg,len)==SUCCESS){
			call Leds.led2Toggle();
		}
	
	return msg;
	}

	event void RadioSend.sendDone(message_t *msg, error_t error){
			call Leds.led2Toggle();	
			
			if(call GSMSplitControl.stop()!=SUCCESS)
				call GSMSplitControl.stop();	
				
				
	}
		
	event void GSMSplitControl.stopDone(error_t error){
		if (error==SUCCESS){
			call Leds.led0Off();
			if(call RadioSplitControl.stop()!=SUCCESS){
				call RadioSplitControl.stop();	
			}
		}else{
				if(call GSMSplitControl.stop()!=SUCCESS){
					call GSMSplitControl.stop();	
				}			
		}
		if(call DiagMsg.record()){
			call DiagMsg.str("stopdone!");
			call DiagMsg.uint8(error);
			call DiagMsg.send();
		}
	}


	
	event void RadioSplitControl.stopDone(error_t error){}

}
