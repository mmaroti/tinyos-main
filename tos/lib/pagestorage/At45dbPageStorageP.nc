#include "Storage.h"

module At45dbDirectStorageP {
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
  
  uint8_t currentClient=N;
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
      //TODO signal event
      return SUCCESS;
    }
    newRequest(id, S_REAL_ERASE, physicalAddr(id, pageNum), NULL);
    return SUCCESS;
  }
  
  event void Resource.granted[uint8_t id](){
    error_t ret=SUCCESS;
    switch(clients[currentClient].status){
      case S_READ:{
      }break;
      case S_WRITE:{
      }break;
      case S_REAL_ERASE:{
      }break;
    }
    if(ret!=SUCCESS)
      //TODO signal
  }
}
