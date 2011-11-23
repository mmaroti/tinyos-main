#include "testmsg.h"
#include "gsm.h"

module TestP{
	uses interface Boot;
	uses interface AMSend;
	uses interface Receive;
	uses interface SplitControl as GSMSplitControl;
	uses interface SplitControl as RadioSplitControl;
	uses interface Leds;
	uses interface DiagMsg;
	uses interface BusyWait<TMicro, uint16_t>;
}
implementation{

	message_t test;
	testmsg_t* message;

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
		message=(testmsg_t*)(call AMSend.getPayload(&test,sizeof(testmsg_t)));
		message->cmd[0]=6;
		message->cmd[1]=7;
		message->cmd[2]=8;
		message->cmd[3]=9;
		message->cmd[4]=10;
		message->cmd[5]=11;
		call AMSend.send(TOS_BCAST_ADDR,&test,sizeof(testmsg_t));
	}
	
	event void AMSend.sendDone(message_t* msg, error_t error) {
		if(error==SUCCESS){
			call Leds.led1On();
			if(call GSMSplitControl.stop()!=SUCCESS){
				call GSMSplitControl.stop();	
			}
		}
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
	
	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len){
		testmsg_t* receivedMsg=(testmsg_t*)payload;
	
		if(call DiagMsg.record()){
			call DiagMsg.str("Rec.rec");
			call DiagMsg.uint8s((uint8_t*)receivedMsg->cmd,len);
			call DiagMsg.send();
		}
	
	return msg;	
	}
	
	event void RadioSplitControl.stopDone(error_t error){}
}