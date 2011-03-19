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
module StreamUploaderSerialP{
	uses {
		interface Receive;
	    interface AMSend;
	    interface Packet;
  		interface AMPacket;
  		interface StreamStorage;    
  		interface SplitControl as SerialControl;
  		interface SplitControl as StreamControl;
  		interface Timer<TMilli> as WaitTimer;
  		interface Timer<TMilli> as StorageWaitTimer;
  		interface Boot;
  		interface Leds;
	}
}

implementation{
	uint32_t minaddress,maxaddress;
	uint8_t status=OFF;
	uint8_t buffer[MESSAGE_SIZE];
	message_t message;
	
	event void Boot.booted(){
		call StreamControl.start();	
	}
	
	event void StreamControl.startDone(error_t error){
		if(error!=SUCCESS){
			call StreamControl.start();
		} else {
			status=WAIT_FOR_BS;
			if(call StreamStorage.getMinAddress()==EBUSY)
				call StorageWaitTimer.startOneShot(10);
		}
	}
	
	event void StreamStorage.getMinAddressDone(uint32_t addr){
		ctrl_msg* msg=call Packet.getPayload(&message, sizeof(ctrl_msg));
		msg->min_address=addr;
		msg->max_address=call StreamStorage.getMaxAddress();
		if(call SerialControl.start()==EALREADY)
			call AMSend.send(BS_ADDR, &message, sizeof(ctrl_msg)+4);		
	}
	
	event void StorageWaitTimer.fired(){
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
			case ERASE:{
				if(call StreamStorage.erase()==EBUSY)
					call StorageWaitTimer.startOneShot(10);
			}break;
			
		}
	}
	
	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len){
		call Leds.led1Toggle();
		if((status==WAIT_FOR_BS||status==WAIT_FOR_REQ)&&len==sizeof(ctrl_msg)){
			ctrl_msg *rec=(ctrl_msg*)payload;
			call Leds.led0Toggle();
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
				status=ERASE;
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
			call WaitTimer.startOneShot((uint32_t)(uint32_t)RADIO_SHORT);
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
				call Leds.led2Toggle();
				readNext();
			}
		}
	}

	event void AMSend.sendDone(message_t *msg, error_t error){
		if(status==WAIT_FOR_BS){
			call WaitTimer.startOneShot(RADIO_SHORT);
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
				call SerialControl.stop();	
			}break;
		}
	}


	event void SerialControl.startDone(error_t error){
		if(error==SUCCESS){
			call AMSend.send(BS_ADDR, &message, sizeof(ctrl_msg)+4); 
		}else
			call SerialControl.start();
	}

	event void SerialControl.stopDone(error_t error){
		if(error!=SUCCESS)
			call SerialControl.stop();
		else{
			if(status!=OFF){
				status=WAIT_FOR_BS;
				call WaitTimer.startOneShot(RADIO_SHORT);
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

	event void StreamStorage.appendDoneWithID(void *buf, uint16_t len, error_t error){
		// TODO Auto-generated method stub
	}


	event void StreamStorage.appendDone(void *buf, uint16_t len, error_t error){
		// TODO Auto-generated method stub
	}

	event void StreamControl.stopDone(error_t error){
		// TODO Auto-generated method stub
	}
}