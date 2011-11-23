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
  };
  
  typedef struct client_data{
    uint8_t status;
    uint16_t pageNum;
    uint8_t* buffer;
  } client_data;
  
  client_data clients[N];
  
  uint8_t currentClient;
  uint8_t fakeEraseClient;

  inline void signalEvents(uint8_t client, uint8_t prevstatus, error_t error){
    call Resource.release[client]();//currentClient can be changed
    clients[client].status=S_IDLE;
    switch(prevstatus){
      case S_READ:{
        signal PageStorage.readDone[client](clients[client].pageNum, clients[client].buffer, error);
      }break;
      case S_WRITE:{
        signal PageStorage.writeDone[client](clients[client].pageNum, clients[client].buffer, error);
      }break;
      case S_REAL_ERASE:{
        signal PageStorage.eraseDone[client](clients[client].pageNum, TRUE, error);
      }break;
      case S_ERASE:{
        signal PageStorage.eraseDone[client](clients[client].pageNum, FALSE, error);
      }break;
    }
  }
  
  inline error_t newRequest(uint8_t id, uint8_t cmd, uint16_t pageNum, void *buffer){
    clients[id].status=cmd;
    clients[id].pageNum=pageNum;
    clients[id].buffer=buffer;
    return call Resource.request[id]();
  }
  
  command error_t PageStorage.read[uint8_t id](uint32_t pageNum, void *buffer){
    if(pageNum >= call At45dbVolume.volumeSize[id]())
      return EINVAL;
    if(clients[id].status==S_READ)
      return EALREADY;
    else if(clients[id].status!=S_IDLE)
      return EBUSY;
    return newRequest(id, S_READ, pageNum, buffer);
  }
  
  command error_t PageStorage.write[uint8_t id](uint32_t pageNum, void *buffer){
    if(pageNum >= call At45dbVolume.volumeSize[id]())
      return EINVAL;
    if(clients[id].status==S_WRITE)
      return EALREADY;
    else if(clients[id].status!=S_IDLE)
      return EBUSY;
    return newRequest(id, S_WRITE, pageNum, buffer);
  }
  
  task void signalFakeErase(){
    clients[fakeEraseClient].status=S_IDLE;
    signal PageStorage.eraseDone[fakeEraseClient](clients[fakeEraseClient].pageNum, FALSE, SUCCESS);
  }
  
  command error_t PageStorage.erase[uint8_t id](uint32_t sectorNum, bool realErase){
    if(sectorNum >= call At45dbVolume.volumeSize[id]())
      return EINVAL;
    if(clients[id].status==S_ERASE||clients[id].status==S_REAL_ERASE)
      return EALREADY;
    else if(clients[id].status!=S_IDLE)
      return EBUSY;
    if(!realErase){
      clients[id].status=S_ERASE;
      clients[id].pageNum=sectorNum;
      fakeEraseClient=id;
      post signalFakeErase();
      return SUCCESS;
    }
    return newRequest(id, S_REAL_ERASE, sectorNum, NULL);
  }
  
  event void Resource.granted[uint8_t id](){
    currentClient=id;
    switch(clients[currentClient].status){
      case S_READ:{
        call At45db.read(call At45dbVolume.remap[currentClient](clients[currentClient].pageNum), 0, clients[currentClient].buffer, AT45_PAGE_SIZE);
      }break;
      case S_WRITE:{
        call At45db.write(call At45dbVolume.remap[currentClient](clients[currentClient].pageNum), 0, clients[currentClient].buffer, AT45_PAGE_SIZE);
      }break;
      case S_REAL_ERASE:{
        call At45db.erase(call At45dbVolume.remap[currentClient](clients[currentClient].pageNum), AT45_ERASE);
      }break;
    }
  }
  
  event void At45db.readDone(error_t error){
    signalEvents(currentClient, S_READ, error);
  }
  
  event void At45db.writeDone(error_t error){
    if(error!=SUCCESS){
      signalEvents(currentClient, S_WRITE, error);
    } else {
      call At45db.flush(clients[currentClient].pageNum);
    }
  }
  
  event void At45db.flushDone(error_t error){
    signalEvents(currentClient, S_WRITE, error);
  }
  
  event void At45db.eraseDone(error_t error){
    signalEvents(currentClient, S_ERASE, error);
  }
  
  command uint16_t PageStorage.getPageSize[uint8_t id](){
    return AT45_PAGE_SIZE;
  }
  
  command uint8_t PageStorage.getPageSizeLog2[uint8_t id](){
    return AT45_PAGE_SIZE_LOG2;// For those who want to ignore the last 8 bytes
  }
  
  command uint32_t PageStorage.getSectorSize[uint8_t id](){
    return AT45_PAGE_SIZE;
  }
  
  command uint8_t PageStorage.getSectorSizeLog2[uint8_t id](){
    return AT45_PAGE_SIZE_LOG2;// For those who want to ignore the last 8 bytes
  }
  
  command uint32_t PageStorage.getNumPages[uint8_t id](){
    return call At45dbVolume.volumeSize[id]();
  }
  
  command uint32_t PageStorage.getNumSectors[uint8_t id](){
    return call At45dbVolume.volumeSize[id]();
  }
  
  event void At45db.syncDone(error_t error){}
  event void At45db.copyPageDone(error_t error){}
  event void At45db.computeCrcDone(error_t error, uint16_t crc){}
  
  default event void PageStorage.readDone[uint8_t id](uint32_t pageNum, void *buffer, error_t error){}
  default event void PageStorage.writeDone[uint8_t id](uint32_t pageNum, void *buffer, error_t error){}
  default event void PageStorage.eraseDone[uint8_t id](uint32_t pageNum, bool realErase, error_t error){}
  default command at45page_t At45dbVolume.remap[uint8_t id](at45page_t volumePage) {return 0;}
  default command at45page_t At45dbVolume.volumeSize[uint8_t id]() { return 0; }
  default async command error_t Resource.request[uint8_t id]() { return FAIL; }
  default async command error_t Resource.release[uint8_t id]() { return FAIL; }
}
