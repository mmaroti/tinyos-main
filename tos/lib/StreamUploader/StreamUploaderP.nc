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
	uses {
		interface Receive;
	    interface AMSend;
	    interface Packet;
  		interface AMPacket;
  		interface StreamStorage;    
	}
}

implementation{
	uint32_t minaddress,maxaddress;
	uint16_t nodeid;
	uint8_t buffer[MESSAGE_SIZE];
	message_t message;

	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len){
		if(len==sizeof(hello_msg)){
			hello_msg *rec=(hello_msg*)payload;
			nodeid=rec->nodeid;
			call StreamStorage.getMinAddress(); 
		} else if(len==sizeof(resp_msg)){
			resp_msg *rec=(resp_msg*)payload;
			if(nodeid==rec->src){
				minaddress=rec->min_address;
				maxaddress=rec->max_address;
			}
			call StreamStorage.read(minaddress, buffer, MESSAGE_SIZE);
			
		}
		return msg;
	}
	
	event void StreamStorage.getMinAddressDone(uint32_t addr){
		resp_msg* msg=call Packet.getPayload(&message, sizeof(resp_msg));
		msg->dest=nodeid;
		msg->src=TOS_NODE_ID;
		msg->min_address=addr;
		msg->max_address=call StreamStorage.getMaxAddress();
		call AMSend.send(nodeid, &message, sizeof(resp_msg));
	}

	event void StreamStorage.readDone(void *buf, uint8_t len, error_t error){
		if(error==SUCCESS){
			if(len=MESSAGE_SIZE){
				data_msg* msg=call Packet.getPayload(&message, sizeof(data_msg));
				msg->address=(uint8_t)(minaddress%256);
				memcpy(&(msg->data),buf,len);
				call AMSend.send(nodeid, &message, sizeof(data_msg));
			} else {
				last_data_msg* msg=call Packet.getPayload(&message, sizeof(last_data_msg));
				msg->len=len;
				memcpy(&(msg->data),buf,len);
				call AMSend.send(nodeid, &message, sizeof(data_msg));
			}
		}
	}

	event void AMSend.sendDone(message_t *msg, error_t error){
		minaddress+=MESSAGE_SIZE;
		if(minaddress+MESSAGE_SIZE<=maxaddress)
			call StreamStorage.read(minaddress, buffer, MESSAGE_SIZE);
		else if(minaddress<maxaddress)
			call StreamStorage.read(minaddress, maxaddress-minaddress, MESSAGE_SIZE);
		else if(minaddress==maxaddress){
			last_data_msg* msg=call Packet.getPayload(&message, sizeof(last_data_msg));
			msg->len=0;
			call AMSend.send(nodeid, &message, sizeof(data_msg));
		}
	}




	
	event void StreamStorage.syncDone(error_t error){
	}

	event void StreamStorage.eraseDone(error_t error){
	}

	event void StreamStorage.appendDone(void *buf, uint8_t len, error_t error){
	}
	
	event void StreamStorage.appendDoneWithID(void *buf, uint8_t len, error_t error){
	}
}