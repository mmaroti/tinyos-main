#include "StreamUploader.h"
module StreamUploaderP{
	uses{
		interface AMSend as CtrlSend;
		interface AMSend as DataSend;
		interface Receive as GetReceive;
		interface Receive as CommandReceive;
		interface Packet;
		interface AMPacket;
		interface StreamStorageRead;
		interface StreamStorageErase;
		interface Resource; 
		interface Timer<TMilli>;
	}
}
implementation{
	enum{
		STREAM_GETMIN,
		STREAM_GETMIN_READ,
		STREAM_READ,
		STREAM_ERASE,
		STREAM_NULL=0,
	};
	
	uint8_t streamcommand=STREAM_NULL;
	uint32_t readaddress;
	uint32_t lastaddress;
	uint8_t buffer[MESSAGE_SIZE];

	event message_t * CommandReceive.receive(message_t *msg, void *payload, uint8_t len){
		command_msg *rec=(command_msg*)payload;
		switch(rec->cmd){
			case 0x10:{
				if(call Resource.request()==SUCCESS)
					streamcommand=STREAM_GETMIN;
			}break;
			case 0x20:{
				if(call Resource.request()==SUCCESS)
					streamcommand=STREAM_ERASE;
			}break;
		}		
		return msg;
	}

	event message_t * GetReceive.receive(message_t *msg, void *payload, uint8_t len){
		get_msg *rec=(get_msg*)payload;
		if(rec->nodeid==TOS_NODE_ID){
			readaddress=rec->min_address;
			lastaddress=rec->max_address;
			if(readaddress<lastaddress&&lastaddress<=call StreamStorageRead.getMaxAddress()){
				streamcommand=STREAM_GETMIN_READ;
				if(call Resource.request()!=SUCCESS)
					call Timer.startOneShot(10);
			}
		}
		return msg;
	}


	event void DataSend.sendDone(message_t *msg, error_t error){
		if(readaddress<lastaddress){
			if(call Resource.request()!=SUCCESS)
				call Timer.startOneShot(10);
		} else
			streamcommand=STREAM_NULL;
	}
	
	event void Timer.fired(){
		if(call Resource.request()!=SUCCESS)
			call Timer.startOneShot(10);
	}

	event void StreamStorageRead.readDone(void *buf, uint8_t len, error_t error){
		message_t msg;
		data_msg *send=(data_msg*)call Packet.getPayload(&msg, sizeof(data_msg));
		call Resource.release();
		send->address=readaddress;
		send->length=len;
		memcpy(&(send->data),buf,len);
		readaddress+=len;
		call DataSend.send(TOS_BCAST_ADDR, &msg, sizeof(data_msg));
	}

	event void StreamStorageRead.getMinAddressDone(uint32_t addr, error_t error){
		if(streamcommand==STREAM_GETMIN){
			message_t msg;
			ctrl_msg *send=(ctrl_msg*)call Packet.getPayload(&msg, sizeof(ctrl_msg));
			call Resource.release();
			send->min_address=addr;
			send->max_address=call StreamStorageRead.getMaxAddress();
			streamcommand=STREAM_NULL;
			call CtrlSend.send(TOS_BCAST_ADDR, &msg, sizeof(ctrl_msg));
		} else {
			if(readaddress<addr)
				readaddress=addr;
			streamcommand=STREAM_READ;
			if(readaddress<lastaddress)
				call StreamStorageRead.read(readaddress, buffer, (lastaddress-readaddress)<MESSAGE_SIZE?(lastaddress-readaddress):MESSAGE_SIZE);
			else{
				streamcommand=STREAM_NULL;
				call Resource.release();
			}
		}
	}
	
	event void StreamStorageErase.eraseDone(error_t error){
		streamcommand=STREAM_GETMIN;
		call StreamStorageRead.getMinAddress();
	}
	

	event void Resource.granted(){
		if(streamcommand==STREAM_GETMIN||streamcommand==STREAM_GETMIN_READ)
			call StreamStorageRead.getMinAddress();
		else if(streamcommand==STREAM_READ)
			;//call StreamStorageRead.read(uint32_t addr, void *buf, uint8_t len);
		else if(streamcommand==STREAM_ERASE)
			call StreamStorageErase.erase();
	}
	
	event void CtrlSend.sendDone(message_t *msg, error_t error){}


}