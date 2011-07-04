#include "Storage.h"

module At45dbPageStorageP {
  provides interface PageStorage[uint8_t id];
  
  uses {
    interface At45db;
    interface At45dbVolume[uint8_t id];
    interface Resource[uint8_t id];
  }
}

implementation {
  enum{
    S_IDLE,
    S_READ,
    S_WRITE,
    S_ERASE,
    S_REAL_ERASE,
  };  
  enum {
    N = uniqueCount("at45db.page"),
    //TODO NO_CLIENT = 0xff,
  };
  
  typedef struct client_data{
    uint8_t status;
    uint16_t pageNum;
    uint8_t* buffer;
  } client_data;
  
  client_data clients[N];
  
  uint8_t currentClient;
  error_t currentError;  
  
  uint16_t physicalAddr(uint8_t id, uint16_t pageNum){
    call At45dbVolume.remap[id](pageNum);
  }

  task void signalEvents(){
    uint8_t prevstatus=clients[currentClient].status;
    clients[currentClient].status=S_IDLE;
    switch(prevstatus){
      case S_READ:{
        signal PageStorage.readDone[currentClient](clients[currentClient].pageNum, clients[currentClient].buffer, currentError);
      }break;
      case S_WRITE:{
        signal PageStorage.writeDone[currentClient](clients[currentClient].pageNum, clients[currentClient].buffer, currentError);
      }break;
      case S_REAL_ERASE:{
        signal PageStorage.eraseDone[currentClient](clients[currentClient].pageNum, TRUE, currentError);
      }break;
      case S_ERASE:{
        signal PageStorage.eraseDone[currentClient](clients[currentClient].pageNum, FALSE, currentError);
      }break;
    }
  }
  
  inline void newRequest(uint8_t id, uint8_t cmd, uint16_t pageNum, void *buffer){
    clients[id].status=cmd;
    clients[id].pageNum=physicalAddr(id, pageNum);
    clients[id].buffer=buffer;
    call Resource.request[id]();
  }
  
  command error_t PageStorage.read[uint8_t id](uint16_t pageNum, void *buffer){
    if(clients[id].status==S_READ)
      return EALREADY;
    else if(clients[id].status!=S_IDLE)
      return EBUSY;
    newRequest(id, S_READ, physicalAddr(id, pageNum), buffer);
    return SUCCESS;
  }
  
  command error_t PageStorage.write[uint8_t id](uint16_t pageNum, void *buffer){
    if(clients[id].status==S_WRITE)
      return EALREADY;
    else if(clients[id].status!=S_IDLE)
      return EBUSY;
    newRequest(id, S_WRITE, physicalAddr(id, pageNum), buffer);
    return SUCCESS;
  }
  
  command error_t PageStorage.erase[uint8_t id](uint16_t pageNum, bool realErase){
    if(clients[id].status==S_ERASE||clients[id].status==S_REAL_ERASE)
      return EALREADY;
    else if(clients[id].status!=S_IDLE)
      return EBUSY;
    if(!realErase){
      signal PageStorage.eraseDone[id](pageNum, FALSE, SUCCESS);//FIXME possible infinite loop 
      return SUCCESS;
    }
    newRequest(id, S_REAL_ERASE, physicalAddr(id, pageNum), NULL);
    return SUCCESS;
  }
  
  event void Resource.granted[uint8_t id](){
    currentClient=id;
    switch(clients[currentClient].status){
      case S_READ:{
        call At45db.read(clients[currentClient].pageNum, 0, clients[currentClient].buffer, AT45_PAGE_SIZE);
      }break;
      case S_WRITE:{
        call At45db.read(clients[currentClient].pageNum, 0, clients[currentClient].buffer, AT45_PAGE_SIZE);
      }break;
      case S_REAL_ERASE:{
        call At45db.erase(clients[currentClient].pageNum, AT45_ERASE);
      }break;
    }
  }
  
  event void At45db.readDone(error_t error){
    call Resource.release[currentClient]();
    currentError=error;
    post signalEvents();
  }
  
  event void At45db.writeDone(error_t error){
    if(error!=SUCCESS){
      call Resource.release[currentClient]();
      currentError=error;
      post signalEvents(); 
    } else
      call At45db.flush(clients[currentClient].pageNum);
  }
  
  event void At45db.flushDone(error_t error){
    call Resource.release[currentClient]();
    currentError=error;
    post signalEvents();
  }
  
  event void At45db.eraseDone(error_t error){
    call Resource.release[currentClient]();
    currentError=error;
    post signalEvents();
  }
  
  event void At45db.syncDone(error_t error){}
  event void At45db.copyPageDone(error_t error){}
  event void At45db.computeCrcDone(error_t error, uint16_t crc){}
  
  default async command error_t Resource.release[uint8_t id](){return SUCCESS;}
  default event void PageStorage.readDone[uint8_t id](uint16_t pageNum, void *buffer, error_t error){}
  default event void PageStorage.writeDone[uint8_t id](uint16_t pageNum, void *buffer, error_t error){}
  default event void PageStorage.eraseDone[uint8_t id](uint16_t pageNum, bool realErase, error_t error){}
}
