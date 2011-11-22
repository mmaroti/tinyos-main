 
interface PageStorage {

  command error_t read(uint32_t pageNum, void *buffer);
  
  command error_t write(uint32_t pageNum, void *buffer);
  
  command error_t erase(uint32_t sectorNum, bool realErase);
 
  event void readDone(uint32_t pageNum, void *buffer, error_t error);
  
  event void writeDone(uint32_t pageNum, void *buffer, error_t error);
  
  event void eraseDone(uint32_t sectorNum, bool realErase, error_t error);
}