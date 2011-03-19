/*
* Copyright (c) 2010, University of Szeged
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
#include "Storage.h"
#include "StorageVolumes.h"
#include "FlashDumper_at45db.h"
module FlashDumper_at45dbP{
	uses {
		interface At45db;
		interface At45dbVolume[uint8_t logID];
		interface Resource;
		interface Boot;
		interface Receive;
		interface AMSend;
		interface SplitControl;
		interface Packet;
		interface AMPacket;
		interface Leds;		
	}
}
implementation{

	uint8_t page[PAGE_SIZE];
	
	uint16_t startpage=1,endpage=0, offset;
	message_t message;
	
	event void Boot.booted(){
		call SplitControl.start();
	}
	
	event void SplitControl.startDone(error_t error){
		if(error==SUCCESS)
			call Leds.set(7);
		else
			call SplitControl.start();
	}
	
	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len){
		if((len==sizeof(ctrl_msg))&&(startpage>endpage)){
			ctrl_msg *rec=(ctrl_msg*)payload;
			startpage=rec->startpage;
			endpage=rec->endpage;
			call Resource.request();
		}
		return msg;
	}

	event void Resource.granted(){
		call At45db.read(startpage, 0, page, PAGE_SIZE);
	}
	
	event void At45db.readDone(error_t error){
		call Resource.release();
		if(error==SUCCESS){
			data_msg* send;
			offset=0;
			send=call Packet.getPayload(&message, sizeof(data_msg));
			send->page=startpage;
			send->offset=offset;
			memcpy(send->data,(page+offset),DATA_SIZE);
			call AMSend.send(AM_BROADCAST_ADDR, &message, sizeof(data_msg));
		} else {
			data_msg* send;
			send=call Packet.getPayload(&message, sizeof(data_msg));
			offset=0xffff;
			send->page=startpage;
			send->offset=offset;
			call AMSend.send(AM_BROADCAST_ADDR, &message, sizeof(data_msg));
		}
			
	}
	
	event void AMSend.sendDone(message_t *msg, error_t error){
		if(offset==0xffff){
			startpage++;
			if(startpage<=endpage){
				call Resource.request();
			}
			return;
		}
		offset+=DATA_SIZE;
		if(offset>=PAGE_SIZE){	
			ctrl_msg* send=call Packet.getPayload(&message, sizeof(ctrl_msg));
			offset=0xffff;	
			send->startpage=startpage;
			send->endpage=PAGE_SIZE;
			call AMSend.send(AM_BROADCAST_ADDR, &message, sizeof(ctrl_msg));
		} else if(offset+DATA_SIZE>=PAGE_SIZE){
			data_msg* send=call Packet.getPayload(&message, sizeof(data_msg));
			send->page=startpage;
			send->offset=offset;
			memcpy(send->data,(page+offset),PAGE_SIZE-offset);
			call AMSend.send(AM_BROADCAST_ADDR, &message, sizeof(data_msg));
		} else{
			data_msg* send=call Packet.getPayload(&message, sizeof(data_msg));
			send->page=startpage;
			send->offset=offset;
			memcpy(send->data,(page+offset),DATA_SIZE);
			call AMSend.send(AM_BROADCAST_ADDR, &message, sizeof(data_msg));
		}
	}
	
	event void At45db.eraseDone(error_t error){
	}
	event void At45db.computeCrcDone(error_t error, uint16_t crc){
	}
	event void At45db.syncDone(error_t error){
	}
	event void At45db.writeDone(error_t error){
	}
	event void At45db.flushDone(error_t error){
	}
	event void At45db.copyPageDone(error_t error){
	}
	event void SplitControl.stopDone(error_t error){
	}
}