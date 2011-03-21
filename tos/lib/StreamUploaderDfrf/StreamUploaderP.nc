#include "StreamUploader.h"
module StreamUploaderP{
    uses{
        interface DfrfSend as CtrlSend;
        interface DfrfSend as DataSend;
        interface DfrfReceive as CtrlReceive;
        interface DfrfReceive as DataReceive;		
        interface DfrfReceive as GetReceive;
        interface DfrfReceive as CommandReceive;
        interface StreamStorageRead;
        interface StreamStorageErase;
        interface Resource; 
        interface Leds;
        interface Boot;
        interface Init as ConvergecastInit;
        interface Timer<TMilli>;
    }
    provides interface Command;
}
implementation{
    enum{
        STREAM_GETMIN=CMD_GETMIN,
        STREAM_ERASE=CMD_ERASE,
        STREAM_GETMIN_READ,
        STREAM_EXTERNAL_COMMAND,
        STREAM_READ,
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
    void *datacached=NULL;
    
    inline void reset(){
      if(call Resource.isOwner)
        call Resource.release();
      streamcommand=STREAM_NULL;
    }

    event bool CommandReceive.receive(void *payload){
      if(streamcommand==STREAM_NULL){
        command_msg *rec=(command_msg*)payload;
        call Leds.led1Toggle();
        if(rec->cmd>=0x10){
          streamcommand=STREAM_EXTERNAL_COMMAND;
          signal Command.newCommand(rec->cmd);
        } else if(call Resource.request()==SUCCESS){
          streamcommand=rec->cmd;
        } else
          reset();
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
                if(call Resource.request()==SUCCESS)
                  streamcommand=STREAM_GETMIN_READ;
                else
                  reset();
            } 
        } else if(rec->min_address==rec->max_address&&(rec->nodeid==TOS_NODE_ID||rec->nodeid==TOS_BCAST_ADDR)){
            downloadSeq=rec->seq_num;
            if(rec->min_address>=0x10)
              signal Command.newCommand(rec->min_address);
            else if(call Resource.request()==SUCCESS){
                streamcommand=rec->min_address;
            } else
              reset();
        }
        return TRUE;
    }
    
    event void DataSend.sendDone(void *data){
        if(datacached==data){
            error_t e;
            datacached=NULL;
            if(readaddress<lastaddress){
                e=call Resource.request();
            } else{
                streamcommand=STREAM_GETMIN;
                e=call Resource.request();
            }
            if(e!=SUCCESS)
              reset();
        }
    }
    
    event void Timer.fired(){
      datacached=call DataSend.send(&datasend);
      if(datacached==NULL){
        call Timer.startOneShot(10);
      }
    }

    event void StreamStorageRead.readDone(void *buf, uint8_t len, error_t error){
        call Resource.release();
        datasend.source=TOS_NODE_ID;
        datasend.address=readaddress;
        datasend.seqnum=seq_num++;
        memcpy(&(datasend.payload[0]),buf,len);
        readaddress+=len;
        datacached=call DataSend.send(&datasend);
        if(datacached==NULL){
          call Timer.startOneShot(10);
        }		
    }
    

    event void StreamStorageRead.getMinAddressDone(uint32_t addr, error_t error){
        if(error==SUCCESS){
            void * ctrlcached;
            if(streamcommand==STREAM_GETMIN){
                ctrlsend.max_address=call StreamStorageRead.getMaxAddress();
                call Resource.release();//!
                ctrlsend.min_address=addr;
                ctrlsend.source=TOS_NODE_ID;
                if(downloadSeq!=0){
                    ctrlsend.seq_num=downloadSeq;
                    downloadSeq=0;
                } else
                    ctrlsend.seq_num=seq_num++;
                streamcommand=STREAM_NULL;
                call Leds.led2Toggle();
                ctrlcached=call CtrlSend.send(&ctrlsend);
            } else {
                if(readaddress<addr)
                    readaddress=addr;
                if(readaddress<lastaddress){
                    streamcommand=STREAM_READ;
                    if(lastaddress-readaddress<MESSAGE_SIZE)
                        readaddress=lastaddress-MESSAGE_SIZE;
                    if(call StreamStorageRead.read(readaddress, buffer, MESSAGE_SIZE)!=SUCCESS){
                        call Resource.release();//!
                        streamcommand=STREAM_NULL;		
                    }				
                } else {
                    ctrlsend.max_address=call StreamStorageRead.getMaxAddress();
                    call Resource.release();//!
                    ctrlsend.min_address=addr;
                    ctrlsend.source=TOS_NODE_ID;
                    if(downloadSeq!=0){
                        ctrlsend.seq_num=downloadSeq;
                        downloadSeq=0;
                    } else
                        ctrlsend.seq_num=seq_num++;
                    streamcommand=STREAM_NULL;
                    ctrlcached=call CtrlSend.send(&ctrlsend);
                }
            }
        } else {
            call Resource.release();//!
        }
    }
    
    event void StreamStorageErase.eraseDone(error_t error){
        call Command.sendData(0);
        streamcommand=STREAM_NULL;
        call Resource.release();
    }
    

    event void Resource.granted(){
        error_t error=SUCCESS;	
        call Leds.led0Toggle();
        if(streamcommand==STREAM_GETMIN||streamcommand==STREAM_GETMIN_READ){
            error=call StreamStorageRead.getMinAddress();
        }else if(streamcommand==STREAM_READ){
            if(lastaddress-readaddress<MESSAGE_SIZE)
                readaddress=lastaddress-MESSAGE_SIZE;
            error=call StreamStorageRead.read(readaddress, buffer, MESSAGE_SIZE);	
        }else if(streamcommand==STREAM_ERASE)
            error=call StreamStorageErase.erase();	
        else
            error=FAIL;
        if(error!=SUCCESS){
            streamcommand=STREAM_NULL;
            call Resource.release();
            if(error==EOFF){
                call Command.sendData(0xffffffff);
            }
        }
    }
    
    command error_t Command.sendData(uint32_t data){
        void* ctrlcached=NULL;
        ctrlsend.max_address=data;
        ctrlsend.min_address=data;
        ctrlsend.source=TOS_NODE_ID;
        if(downloadSeq!=0){
            ctrlsend.seq_num=downloadSeq;
            downloadSeq=0;
        } else
            ctrlsend.seq_num=seq_num++;
        ctrlcached=call CtrlSend.send(&ctrlsend);
        if(streamcommand==STREAM_EXTERNAL_COMMAND)
          streamcommand=STREAM_NULL;
        call Leds.led2Toggle();
        return TRUE;
    }
    
    event void CtrlSend.sendDone(void *data){
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
    
    default event void Command.newCommand(uint32_t id){}
}