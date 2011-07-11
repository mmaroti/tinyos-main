#include "StorageVolumes.h"
module Stm25pPageStorageP {
  provides interface PageStorage[uint8_t id];
  provides interface Stm25pVolume as Volume[ uint8_t id ];
  uses interface Stm25pSpi;
  uses interface Resource;
  uses interface DiagMsg;
}

implementation{
  enum{
    S_IDLE,
    S_READ,
    S_WRITE,
    S_ERASE,
    S_REAL_ERASE,
  };
  enum{
    N=uniqueCount("Stm25p.Page"),
    NOCLIENT=0xff,
  };
  
  typedef struct client_data{
    uint8_t status;
    uint16_t pageNum;
    uint8_t* buffer;
  } client_data;
  
  client_data clients[N];
  
  norace uint8_t currentClient=NOCLIENT;
  norace error_t currentError;
  
  uint8_t getVolumeId( uint8_t client ) {
    return signal Volume.getVolumeId[ client ]();
  }  
  
  uint16_t physicalAddr(uint8_t id, uint16_t page) {
    return page+(STM25P_VMAP[getVolumeId(id)].base<<(STM25P_SECTOR_SIZE_LOG2-STM25P_PAGE_SIZE_LOG2));
  }

  task void runRequests(){
    if(currentClient==NOCLIENT){
      uint8_t i;
      for(i=0;i<N;i++){
        if(clients[i].status!=S_IDLE)
          break;
      }
      if(i>=N){//nothing to do
        currentClient=NOCLIENT;
      } else {
        currentClient=i;
      }
    }
    if(currentClient!=NOCLIENT){
      call Resource.request();
    }
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
    currentClient=NOCLIENT;
    post runRequests();
  }
  
  
  command error_t PageStorage.read[uint8_t id](uint16_t pageNum, void *buffer){
    if(clients[id].status==S_READ)
      return EALREADY;
    else if(clients[id].status!=S_IDLE)
      return EBUSY;
    clients[id].status=S_READ;
    clients[id].pageNum=physicalAddr(id, pageNum);
    clients[id].buffer=buffer;
    if(currentClient==NOCLIENT){
      currentClient=id;
      post runRequests();
    }
    return SUCCESS;
  }
  
  command error_t PageStorage.write[uint8_t id](uint16_t pageNum, void *buffer){
    if(clients[id].status==S_WRITE)
      return EALREADY;
    else if(clients[id].status!=S_IDLE)
      return EBUSY;
    clients[id].status=S_WRITE;
    clients[id].pageNum=physicalAddr(id, pageNum);
    clients[id].buffer=buffer;
    if(currentClient==NOCLIENT){
      currentClient=id;
      post runRequests();
    }
    return SUCCESS;
  }
  
  command error_t PageStorage.erase[uint8_t id](uint16_t pageNum, bool realErase){
    if(clients[id].status==S_ERASE||clients[id].status==S_REAL_ERASE)
      return EALREADY;
    else if(clients[id].status!=S_IDLE)
      return EBUSY;
    clients[id].pageNum=physicalAddr(id, pageNum);
    if(clients[id].pageNum%(STM25P_SECTOR_SIZE>>STM25P_PAGE_SIZE_LOG2))
      return EINVAL;
    if(realErase)
      clients[id].status=S_REAL_ERASE;
    else
      clients[id].status=S_ERASE;
    if(currentClient==NOCLIENT){
      currentClient=id;
      post runRequests();
    }
    return SUCCESS;
  }
  
  inline void SpiDone(error_t err){
    call Stm25pSpi.powerDown();
    call Resource.release();
    currentError=err;
    post signalEvents();
  }
  
  event void Resource.granted(){
    error_t ret=call Stm25pSpi.powerUp();
    if(ret!=SUCCESS)
      SpiDone(ret);
    switch(clients[currentClient].status){
      case S_READ:{
        ret=call Stm25pSpi.read(((uint32_t)clients[currentClient].pageNum<<STM25P_PAGE_SIZE_LOG2),clients[currentClient].buffer,STM25P_PAGE_SIZE);
      }break;
      case S_WRITE:{
        ret=call Stm25pSpi.pageProgram(((uint32_t)clients[currentClient].pageNum<<STM25P_PAGE_SIZE_LOG2),clients[currentClient].buffer,STM25P_PAGE_SIZE);
      }break;
      case S_REAL_ERASE://no difference on this chip
      case S_ERASE:{
        ret=call Stm25pSpi.sectorErase(clients[currentClient].pageNum>>(STM25P_SECTOR_SIZE_LOG2-STM25P_PAGE_SIZE_LOG2));
      }break;
    }
    if(ret!=SUCCESS)
      SpiDone(ret);
  }
 
  async event void Stm25pSpi.readDone( stm25p_addr_t addr, uint8_t* buf, stm25p_len_t len, error_t error ){
    SpiDone(error);
  }

  async event void Stm25pSpi.pageProgramDone( stm25p_addr_t addr, uint8_t* buf, stm25p_len_t len, error_t error ){
    SpiDone(error);
  }

  async event void Stm25pSpi.sectorEraseDone( uint8_t sector, error_t error ){
    SpiDone(error);
  }
  
  


  async event void Stm25pSpi.bulkEraseDone( error_t error ){}
  async event void Stm25pSpi.computeCrcDone( uint16_t crc, stm25p_addr_t addr, stm25p_len_t len, error_t error ){}
 
  default event void PageStorage.readDone[uint8_t id](uint16_t pageNum, void *buffer, error_t error){}
  default event void PageStorage.writeDone[uint8_t id](uint16_t pageNum, void *buffer, error_t error){}
  default event void PageStorage.eraseDone[uint8_t id](uint16_t pageNum, bool realErase, error_t error){}
  default async event volume_id_t Volume.getVolumeId[ uint8_t id ]() { return 0xff; }
}