#include "StreamUploader.h"
module StreamUploaderP{
	uses{
		interface DfrfSend as CtrlSend;
		interface DfrfSend as DataSend;
		interface DfrfReceive as CtrlReceive;
		interface DfrfReceive as DataReceive;		
		interface DfrfReceive as GetReceive;
		interface DfrfReceive as CommandReceive;
//		interface Packet;
//		interface AMPacket;
		interface StreamStorageRead;
		interface StreamStorageErase;
		interface Resource; 
		interface Timer<TMilli>;
		interface Leds;
		interface Boot;
		interface Init as ConvergecastInit;
	}
}
implementation{
	enum{
		STREAM_GETMIN=0x10,
		STREAM_GETMIN_READ,
		STREAM_READ,
		STREAM_ERASE=0x20,
		STREAM_NULL=0,
		BS_ADDRESS=0,
	};
	
	uint8_t streamcommand=STREAM_NULL;
	uint32_t readaddress;
	uint32_t lastaddress;
	uint8_t buffer[MESSAGE_SIZE];
	uint8_t downloadSeq=0;
	uint8_t seq_num=0;
	
	ctrl_msg ctrlsend;
	data_msg datasend;
	void *datacached;

	event bool CommandReceive.receive(void *payload){
		if(streamcommand==STREAM_NULL){
			command_msg *rec=(command_msg*)payload;
			if(call Resource.request()==SUCCESS){
				streamcommand=rec->cmd;
			}
		}
		return TRUE;
	}

	event bool GetReceive.receive(void *payload){
		get_msg *rec=(get_msg*)payload;
		if(streamcommand==STREAM_NULL&&rec->nodeid==TOS_NODE_ID&&rec->min_address<rec->max_address){
			call Leds.led1Toggle();
			readaddress=rec->min_address;
			lastaddress=rec->max_address;
			downloadSeq=rec->seq_num;
			if(readaddress<lastaddress&&lastaddress<=call StreamStorageRead.getMaxAddress()){
				streamcommand=STREAM_GETMIN_READ;
				if(call Resource.request()!=SUCCESS)
					call Timer.startOneShot(10);
			} else if(rec->min_address==rec->max_address&&(rec->nodeid==TOS_NODE_ID||rec->nodeid==TOS_BCAST_ADDR)){
				downloadSeq=rec->seq_num;
				if(call Resource.request()==SUCCESS){
					streamcommand=STREAM_ERASE;
				}
			}
		}
		return TRUE;
	}
	
	event void Timer.fired(){
		if(call Resource.request()!=SUCCESS)
			call Timer.startOneShot(10);
	}
	
	event void DataSend.sendDone(void *data){
		if(datacached==data){
			if(readaddress<lastaddress){
				if(call Resource.request()!=SUCCESS)
					call Timer.startOneShot(10);
			} else{
				streamcommand=STREAM_GETMIN;
				if(call Resource.request()!=SUCCESS)
					call Timer.startOneShot(10);
			}
		}
	}

	event void StreamStorageRead.readDone(void *buf, uint8_t len, error_t error){
		call Resource.release();
		datasend.source=TOS_NODE_ID;
		datasend.address=readaddress;
		datasend.length=len;
		memcpy(&(datasend.payload[0]),buf,len);
		readaddress+=len;
		if(call DataSend.send(&datasend,&datacached)!=SUCCESS){
			if(readaddress<lastaddress){
				if(call Resource.request()!=SUCCESS)
					call Timer.startOneShot(10);
			} else{
				streamcommand=STREAM_GETMIN;
				if(call Resource.request()!=SUCCESS)
					call Timer.startOneShot(10);
			}
		}
			
	}
	

	event void StreamStorageRead.getMinAddressDone(uint32_t addr, error_t error){
		if(error==SUCCESS){
			void * ctrlcached;
			if(streamcommand==STREAM_GETMIN){
				ctrlsend.max_address=call StreamStorageRead.getMaxAddress();
				call Resource.release();
				ctrlsend.min_address=addr;
				ctrlsend.source=TOS_NODE_ID;
				if(downloadSeq!=0){
					ctrlsend.seq_num=downloadSeq;
					downloadSeq=0;
				} else
					ctrlsend.seq_num=seq_num++;
				streamcommand=STREAM_NULL;
				call Leds.led0Toggle();
				call CtrlSend.send(&ctrlsend, &ctrlcached);
			} else {
				if(readaddress<addr)
					readaddress=addr;
				if(readaddress<lastaddress){
					streamcommand=STREAM_READ;
					call StreamStorageRead.read(readaddress, buffer, (lastaddress-readaddress)<MESSAGE_SIZE?(lastaddress-readaddress):MESSAGE_SIZE);
				} else {
					ctrlsend.max_address=call StreamStorageRead.getMaxAddress();
					call Resource.release();
					ctrlsend.min_address=addr;
					ctrlsend.source=TOS_NODE_ID;
					if(downloadSeq!=0){
						ctrlsend.seq_num=downloadSeq;
						downloadSeq=0;
					} else
						ctrlsend.seq_num=seq_num++;
					streamcommand=STREAM_NULL;
					call CtrlSend.send(&ctrlsend,&ctrlcached);
					call Leds.led2Toggle();
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
	
	event void CtrlSend.sendDone(void *data){
		call Leds.led2Toggle();
	}


	event void Boot.booted(){
		call ConvergecastInit.init();
	}

	event bool CtrlReceive.receive(void *data){
		return TRUE;
	}

	event bool DataReceive.receive(void *data){
		return TRUE;
	}
}