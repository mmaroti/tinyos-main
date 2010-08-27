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
		interface Leds;
	}
}
implementation{
	enum{
		STREAM_GETMIN=0x10,
		STREAM_GETMIN_READ,
		STREAM_READ,
		STREAM_ERASE=0x20,
		STREAM_NULL=0,
	};
	
	uint8_t streamcommand=STREAM_NULL;
	uint32_t readaddress;
	uint32_t lastaddress;
	uint8_t buffer[MESSAGE_SIZE];
	
	message_t ctrlmsg,datamsg;

	event message_t * CommandReceive.receive(message_t *msg, void *payload, uint8_t len){
		if(streamcommand==STREAM_NULL){
			command_msg *rec=(command_msg*)payload;
			//call Leds.led0Toggle();
			if(call Resource.request()==SUCCESS){
				streamcommand=rec->cmd;
			}
		}
		return msg;
	}

	event message_t * GetReceive.receive(message_t *msg, void *payload, uint8_t len){
		get_msg *rec=(get_msg*)payload;
		if(streamcommand==STREAM_NULL&&rec->nodeid==TOS_NODE_ID&&rec->min_address<rec->max_address){
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
		} else{
			streamcommand=STREAM_GETMIN;
			call Leds.led0Toggle();
			if(call Resource.request()!=SUCCESS)
				call Timer.startOneShot(10);
		}
	}
	
	event void Timer.fired(){
		if(call Resource.request()!=SUCCESS)
			call Timer.startOneShot(10);
	}

	event void StreamStorageRead.readDone(void *buf, uint8_t len, error_t error){
		data_msg *send=(data_msg*)call Packet.getPayload(&datamsg, sizeof(data_msg));
		call Resource.release();
		send->address=readaddress;
		send->length=len;
		memcpy(&(send->payload[0]),buf,len);
		readaddress+=len;
		call DataSend.send(TOS_BCAST_ADDR, &datamsg, sizeof(data_msg));
	}

	event void StreamStorageRead.getMinAddressDone(uint32_t addr, error_t error){
		if(error==SUCCESS){
			if(streamcommand==STREAM_GETMIN){
				ctrl_msg *send=(ctrl_msg*)call Packet.getPayload(&ctrlmsg, sizeof(ctrl_msg));
				send->max_address=call StreamStorageRead.getMaxAddress();
				call Resource.release();
				send->min_address=addr;
				streamcommand=STREAM_NULL;
				call CtrlSend.send(TOS_BCAST_ADDR, &ctrlmsg, sizeof(ctrl_msg));
			} else {
				if(readaddress<addr)
					readaddress=addr;
				if(readaddress<lastaddress){
					streamcommand=STREAM_READ;
					call StreamStorageRead.read(readaddress, buffer, (lastaddress-readaddress)<MESSAGE_SIZE?(lastaddress-readaddress):MESSAGE_SIZE);
				} else {
					ctrl_msg *send=(ctrl_msg*)call Packet.getPayload(&ctrlmsg, sizeof(ctrl_msg));
					send->max_address=call StreamStorageRead.getMaxAddress();
					call Resource.release();
					send->min_address=addr;
					streamcommand=STREAM_NULL;
					call CtrlSend.send(TOS_BCAST_ADDR, &ctrlmsg, sizeof(ctrl_msg));
				}
			}
		} else {
			call Resource.release();
		}
	}
	
	event void StreamStorageErase.eraseDone(error_t error){
		streamcommand=STREAM_GETMIN;
		call StreamStorageRead.getMinAddress();
	}
	

	event void Resource.granted(){
		error_t error=SUCCESS;	
		if(streamcommand==STREAM_GETMIN||streamcommand==STREAM_GETMIN_READ){
			call Leds.led1Toggle();
			error=call StreamStorageRead.getMinAddress();
		}else if(streamcommand==STREAM_READ){
			error=call StreamStorageRead.read(readaddress, buffer, (lastaddress-readaddress)<MESSAGE_SIZE?(lastaddress-readaddress):MESSAGE_SIZE);
		}else if(streamcommand==STREAM_ERASE)
			error=call StreamStorageErase.erase();	
		if(error!=SUCCESS){
			streamcommand=STREAM_NULL;
			call Resource.release();
		}
	}
	
	event void CtrlSend.sendDone(message_t *msg, error_t error){
		call Leds.led2Toggle();
	}


}