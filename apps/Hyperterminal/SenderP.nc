#include "DataMsg.h"

module SenderP {
	uses interface Boot;
	uses interface Leds;
	uses interface SplitControl;
	uses interface Receive; 
	uses interface AMSend; 
	uses interface StdControl;
	uses interface Timer<TMilli>;
	uses interface UartByte;
	uses interface UartStream;
	uses interface GeneralIO as RTS;
	uses interface GeneralIO as DTR;
	uses interface GpioInterrupt as CTS;
	uses interface GpioInterrupt as DSR;
	uses interface DiagMsg;

	
}
implementation {
	
	norace uint8_t recAnswer[26];
	norace uint16_t byteNum,length;
	message_t radioMsg;
			
	task void send(){
		uint8_t i;
		datamsg_t* answerToSend=(datamsg_t*)(call AMSend.getPayload(&radioMsg,sizeof(datamsg_t)));
		for(i=0;i<length;++i){
			answerToSend->cmd[i]=recAnswer[i];
			answerToSend->length=length;
		}
		call AMSend.send(AM_BROADCAST_ADDR,&radioMsg,sizeof(datamsg_t));
	}
	
	event void Boot.booted() {
		if(call StdControl.start()==SUCCESS){
			call SplitControl.start();
		}
		call UartStream.enableReceiveInterrupt();
		call RTS.makeOutput();
		call DTR.makeOutput();
		call CTS.enableRisingEdge();
		call DSR.enableRisingEdge();
	}
	
	event void SplitControl.startDone(error_t err){
		if(err!=SUCCESS){
			call SplitControl.start();
		}
		call RTS.clr();
		call DTR.clr();
	}
	
	event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len){
		datamsg_t* cmdToSend=(datamsg_t*)payload;
//		call RTS.set();
		if(call UartStream.send((uint8_t*)cmdToSend->cmd, cmdToSend->length )==SUCCESS){
			call Leds.led0Toggle();
		}
		return msg;
	}
	
	async event void UartStream.receivedByte( uint8_t byte ){
		call Timer.startOneShot(500);
		recAnswer[byteNum++]=byte;
		atomic{
			if(byteNum==26){
				call Timer.stop();
				length=byteNum;
				post send();
				byteNum=0;
			}
		}
	}
	
	event void Timer.fired(){
		length=byteNum;
		post send();
		byteNum=0;
	}
		
	async event void UartStream.sendDone( uint8_t* buf, uint16_t len, error_t error ){
//		call RTS.clr();
	}
	
	async event void CTS.fired(){
		call Leds.led1Toggle();
	}
	
	async event void DSR.fired(){
		call Leds.led2Toggle();
	}
	
	async event void UartStream.receiveDone( uint8_t* buf, uint16_t len, error_t error) {}
	event void SplitControl.stopDone(error_t err){}
	event void AMSend.sendDone(message_t* msg,error_t err){}
}





