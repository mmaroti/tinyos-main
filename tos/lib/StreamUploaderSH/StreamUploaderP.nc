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
module StreamUploaderP{
	provides interface StdControl;
	provides interface Command;
	uses {
		interface Receive;
		interface AMSend;
		interface Packet;
  		interface AMPacket;
  		interface StreamStorageRead;    
		interface StreamStorageErase;    
  		interface SplitControl;
  		interface PacketAcknowledgements;
  		interface Timer<TMilli> as WaitTimer;
		interface Leds;
  		
		interface TimeSyncAMSend<TMilli, uint32_t> as TimeSyncAMSendMilli;
		interface Resource;
		interface LocalTime<TMilli>;
		interface DiagMsg;
	}
}

implementation{
	uint32_t minaddress,maxaddress;
	uint8_t status=OFF;
	uint8_t buffer[MESSAGE_SIZE];
	uint8_t bs_lost;
	message_t message;
	
	
	task void SCStop(){
		error_t err=call SplitControl.stop();
		if(err!=SUCCESS&&err!=EALREADY){
			post SCStop();
		}
	}
	
	task void SCStart(){
		error_t err=call SplitControl.start();
		if(err!=SUCCESS&&err!=EALREADY){
			post SCStart();
		}
	}
	
	inline void readNext(){
		minaddress+=MESSAGE_SIZE;
		if(minaddress>=maxaddress){
			status=WAIT_FOR_BS;
		}
		call Resource.request();
	}
	
	event void Resource.granted(){
	    error_t error=SUCCESS;
	    switch(status){
		case WAIT_FOR_BS:{
			error=call StreamStorageRead.getMinAddress();
		}break;
		case SEND:{
			if(minaddress+MESSAGE_SIZE>maxaddress)
			    minaddress=maxaddress-MESSAGE_SIZE+1;
			error=call StreamStorageRead.read(minaddress, buffer, MESSAGE_SIZE);
		}break;
		case ERASE:{
			error=call StreamStorageErase.erase();
		}break;
	    }
	    if(error!=SUCCESS){
	    	call Resource.release();
			switch(status){
			  case WAIT_FOR_BS:{
			  	if(error==EOFF){
				    ctrl_msg* msg=call Packet.getPayload(&message, sizeof(ctrl_msg));
				    error_t err;
				    status=WAIT_FOR_REQ;
				    call Packet.clear(&message);
				    msg->min_address=0xffffffff;
				    msg->max_address=0xffffffff;
				    msg->localtime=call LocalTime.get();
				    call PacketAcknowledgements.requestAck(&message);
				    err=call SplitControl.start();
				    if(err==EALREADY){
					if(call TimeSyncAMSendMilli.send(BS_ADDR, &message, sizeof(ctrl_msg),msg->localtime)!=SUCCESS){
						post SCStop();
					}
				    } else if(err!=SUCCESS){
						post SCStart();
					}
			    } else {
			    	post SCStop();
			    }
			  }break;
			  case SEND:{
			  	readNext();
			  }break;
			  case ERASE:{
			  	status=WAIT_FOR_BS;
			  	post SCStop();
			  }break;
			}
	    }
	}
	
	event void StreamStorageRead.getMinAddressDone(uint32_t addr,error_t error){
		if(error==SUCCESS){
			ctrl_msg* msg=call Packet.getPayload(&message, sizeof(ctrl_msg));
			error_t err;
			call Packet.clear(&message);
			msg->min_address=addr;
			msg->max_address=call StreamStorageRead.getMaxAddress();
			call Resource.release();
			msg->localtime=call LocalTime.get();
			call PacketAcknowledgements.requestAck(&message);
			err=call SplitControl.start();
			if(err==EALREADY){
				if(call TimeSyncAMSendMilli.send(BS_ADDR, &message, sizeof(ctrl_msg),msg->localtime)!=SUCCESS){
					post SCStop();
				}
			}else if(err!=SUCCESS){
				post SCStart();
			}
		} else{
			call Resource.release();
			post SCStop();
		}
	}
	
	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len){
		if(call DiagMsg.record()){
			call DiagMsg.str("rec");
			call DiagMsg.uint8(status);
			call DiagMsg.send();
		}
		if((status==WAIT_FOR_REQ)&&len==sizeof(ctrl_msg)){
			ctrl_msg *rec=(ctrl_msg*)payload;
			call WaitTimer.stop();
			if(rec->min_address!=rec->max_address){
				status=SEND;
				minaddress=rec->min_address;
				maxaddress=rec->max_address;
				call Resource.request();
			} else if(rec->min_address==0) {
				status=ERASE;
				call Resource.request();
			} else {
				status=COMMAND_PENDING;
				call WaitTimer.startOneShot(RADIO_MIDDLE);
				signal Command.newCommand(rec->min_address);
				
			}
			
		}
		return msg;
	}
	
	command error_t Command.sendData(uint32_t data){
		if(status==COMMAND_PENDING){
		    ctrl_msg* msg=call Packet.getPayload(&message, sizeof(ctrl_msg));
		    error_t err;
		    call Packet.clear(&message);
		    msg->min_address=data;
		    msg->max_address=data;
		    msg->localtime=call LocalTime.get();
		    err=call SplitControl.start();
		    if(err==EALREADY){
			    if(call TimeSyncAMSendMilli.send(BS_ADDR, &message, sizeof(ctrl_msg),msg->localtime)!=SUCCESS){
				    post SCStop();
			    }
		    }else if(err!=SUCCESS){
			    post SCStart();
		    }
		    return SUCCESS;
		} else
		      return FAIL;
	}

	event void StreamStorageRead.readDone(void *buf, uint8_t len, error_t error){
		if(status==SEND){
			if(error==SUCCESS){
				data_msg* msg=call Packet.getPayload(&message, sizeof(data_msg));
				memcpy(&(msg->data),buf,len);
				call Resource.release();
				msg->address=minaddress;
				if(call AMSend.send(BS_ADDR, &message, sizeof(data_msg))!=SUCCESS){
					readNext();
				}
			} else{
				call Resource.release();
				readNext();
			}
		}
	}

	event void AMSend.sendDone(message_t *msg, error_t error){
		readNext();
	}

	event void TimeSyncAMSendMilli.sendDone(message_t *msg, error_t error){
		if(status==WAIT_FOR_BS){
			if(call PacketAcknowledgements.wasAcked(msg)){
				call Leds.led2Toggle();
				status=WAIT_FOR_REQ;
				bs_lost=BS_OK;
				call WaitTimer.startOneShot(RADIO_SHORT);
			} else {
				if(bs_lost!=NO_BS){
					bs_lost--;	
				}
				post SCStop();		
			}
		}  else if(status==COMMAND_PENDING){
			post SCStop();
		}
		call Packet.clear(&message);
	}

	event void WaitTimer.fired(){
		switch(status){
			case WAIT_FOR_REQ:
			case COMMAND_PENDING:{
				post SCStop();	
			}break;
			case WAIT_FOR_BS:{
				call Resource.request();
			}break;
		}		
	}

	command error_t StdControl.stop(){
		status=OFF;
		call WaitTimer.stop();
		post SCStop();
		return SUCCESS;
	}

	command error_t StdControl.start(){
		status=WAIT_FOR_BS;
		bs_lost=NO_BS;
		call Resource.request();
		return SUCCESS;
	}

	event void SplitControl.startDone(error_t error){
		if(error==SUCCESS){
			ctrl_msg* msg=call Packet.getPayload(&message, sizeof(ctrl_msg));
			call Leds.led0On();
			msg->localtime=call LocalTime.get();
			if(call TimeSyncAMSendMilli.send(BS_ADDR, &message, sizeof(ctrl_msg),msg->localtime)!=SUCCESS){
				post SCStop();
			}
		}else{
			post SCStart();
		}
	}

	event void SplitControl.stopDone(error_t error){
		if(error!=SUCCESS){
			post SCStop();
		}else{		
			call Leds.led0Off();
			if(status!=OFF){
				status=WAIT_FOR_BS;
				if(bs_lost==NO_BS){//if BS_OK, than it doesn't want any data, so we can sleep longer
					call WaitTimer.startOneShot((uint32_t)RADIO_LONG*1000);
				}else{
					call WaitTimer.startOneShot(RADIO_MIDDLE);
				}
			}
		}
	}
	

	event void StreamStorageErase.eraseDone(error_t error){
		if(status!=OFF){
			status=WAIT_FOR_BS;
			if(call StreamStorageRead.getMinAddress()!=SUCCESS){
				call Resource.release();
				post SCStop();
			}
		}
	}
	
	default event void Command.newCommand(uint32_t id){}
}