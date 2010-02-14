/*
* Copyright (c) 2009, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author:Andras Biro
*/
#include "StreamUploader.h"
//#include "printf.h"
module StreamUploaderP{
	provides interface StdControl;
	uses {
		interface Receive;
	    interface AMSend;
	    interface Packet;
  		interface AMPacket;
  		interface StreamStorage;    
  		interface SplitControl;
  		interface PacketAcknowledgements;
  		interface Timer<TMilli> as WaitTimer;
  		interface Timer<TMilli> as StorageWaitTimer;
	}
}

implementation{
	uint32_t minaddress,maxaddress;
	uint8_t status=OFF;
	uint8_t buffer[MESSAGE_SIZE];
	uint8_t bs_lost=NO_BS;
	message_t message;
	
	event void StreamStorage.getMinAddressDone(uint32_t addr){
		ctrl_msg* msg=call Packet.getPayload(&message, sizeof(ctrl_msg));
		msg->min_address=addr;
		msg->max_address=call StreamStorage.getMaxAddress();
//		printf("%ld, %ld",msg->min_address,msg->max_address);
//		printfflush();
		call PacketAcknowledgements.requestAck(&message);
		if(call SplitControl.start()==EALREADY)
			call AMSend.send(BS_ADDR, &message, sizeof(ctrl_msg));		
	}
	
	event void StorageWaitTimer.fired(){
//		printf("wait\n");
//		printfflush();
		switch(status){
			case WAIT_FOR_BS:{
				if(call StreamStorage.getMinAddress()==EBUSY)
					call StorageWaitTimer.startOneShot(10);
			}break;
			case SEND:{
				if(minaddress+MESSAGE_SIZE<=maxaddress){
					if(call StreamStorage.read(minaddress, buffer, MESSAGE_SIZE)==EBUSY)
						call StorageWaitTimer.startOneShot(10);
				}else{
					if(call StreamStorage.read(minaddress, buffer, maxaddress-minaddress)==EBUSY)
						call StorageWaitTimer.startOneShot(10);
				}
			}break;
			case WAIT_FOR_REQ:{
				if(call StreamStorage.erase()==EBUSY)
					call StorageWaitTimer.startOneShot(10);
			}break;
			
		}
	}
	
	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len){
		
		if((status==WAIT_FOR_REQ)&&len==sizeof(ctrl_msg)){
			ctrl_msg *rec=(ctrl_msg*)payload;
			if(rec->min_address!=rec->max_address){
				status=SEND;
				minaddress=rec->min_address;
				maxaddress=rec->max_address;
				if(minaddress+MESSAGE_SIZE<=maxaddress){
					if(call StreamStorage.read(minaddress, buffer, MESSAGE_SIZE)==EBUSY)
						call StorageWaitTimer.startOneShot(10);
				}else{
					if(call StreamStorage.read(minaddress, buffer, maxaddress-minaddress)==EBUSY)
						call StorageWaitTimer.startOneShot(10);
				}
			} else {
				if(call StreamStorage.erase()==EBUSY)
					call StorageWaitTimer.startOneShot(10);
			}
			
		}
		return msg;
	}
	
	inline void readNext(){
		minaddress+=MESSAGE_SIZE;
		if(minaddress+MESSAGE_SIZE<=maxaddress){
			if(call StreamStorage.read(minaddress, buffer, MESSAGE_SIZE)==EBUSY)
				call StorageWaitTimer.startOneShot(10);
		}else if(minaddress<maxaddress){
			if(call StreamStorage.read(minaddress, buffer, maxaddress-minaddress)==EBUSY)
				call StorageWaitTimer.startOneShot(10);
		}else{
			status=WAIT_FOR_BS;
			call WaitTimer.startOneShot((uint32_t)(uint32_t)SHORT_TIME);
		}
	}

	event void StreamStorage.readDone(void *buf, uint8_t len, error_t error){
		if(status==SEND){
			if(error==SUCCESS){
				data_msg* msg=call Packet.getPayload(&message, sizeof(data_msg));
				msg->length=len;
				msg->address=minaddress;
				memcpy(&(msg->data),buf,len);
				call AMSend.send(BS_ADDR, &message, sizeof(data_msg));
			} else{
				readNext();
			}
		}
	}

	event void AMSend.sendDone(message_t *msg, error_t error){
		if(status==WAIT_FOR_BS){
			if(call PacketAcknowledgements.wasAcked(msg)){
				bs_lost=BS_OK;
				status=WAIT_FOR_REQ;
				call WaitTimer.startOneShot(SHORT_TIME);
			} else {
				bs_lost--;
				call SplitControl.stop();			
			}
		} else { //data sending
			readNext();
		}	
	}

	event void WaitTimer.fired(){
		switch(status){
			case WAIT_FOR_BS:{
				if(call StreamStorage.getMinAddress()==EBUSY)
					call StorageWaitTimer.startOneShot(10);
			}break;
			case WAIT_FOR_REQ:{
				call SplitControl.stop();	
			}break;
		}
	}

	command error_t StdControl.stop(){
		status=OFF;
		call StorageWaitTimer.stop();
		call WaitTimer.stop();
		call SplitControl.stop();
		return SUCCESS;
	}

	command error_t StdControl.start(){
		status=WAIT_FOR_BS;
		if(call StreamStorage.getMinAddress()==EBUSY)
			call StorageWaitTimer.startOneShot(10);
		return SUCCESS;
	}

	event void SplitControl.startDone(error_t error){
		if(error==SUCCESS){
			call AMSend.send(BS_ADDR, &message, sizeof(ctrl_msg)); 
		}else
			call SplitControl.start();
	}

	event void SplitControl.stopDone(error_t error){
		if(error!=SUCCESS)
			call SplitControl.stop();
		else{
			if(status!=OFF){
				status=WAIT_FOR_BS;
				if(bs_lost==NO_BS||bs_lost==BS_OK)//if BS_OK, than it doesn't want any data, so we can sleep longer
					call WaitTimer.startOneShot((uint32_t)LONG_TIME*1000);
				else
					call WaitTimer.startOneShot(SHORT_TIME);
			}
		}
	}
	

	event void StreamStorage.eraseDone(error_t error){
		if(status!=OFF){
			status=WAIT_FOR_BS;
			if(call StreamStorage.getMinAddress()==EBUSY)
				call StorageWaitTimer.startOneShot(10);
		}
	}


	event void StreamStorage.syncDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void StreamStorage.appendDoneWithID(void *buf, uint8_t len, error_t error){
		// TODO Auto-generated method stub
	}


	event void StreamStorage.appendDone(void *buf, uint8_t len, error_t error){
		// TODO Auto-generated method stub
	}
}