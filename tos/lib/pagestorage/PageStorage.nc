 
interface PageStorage {

  command error_t read(uint16_t pageNum, uint8_t *buffer);
  
  command error_t write(uint16_t pageNum, uint8_t *buffer);
  
  command error_t erase(uint16_t pageNum, bool realErase);
 
  event void readDone(uint16_t pageNum, uint8_t *buffer, error_t error);
  
  event void writeDone(uint16_t pageNum, uint8_t *buffer, error_t error);
  
  event void eraseDone(uint16_t pageNum, bool realErase, error_t error);
}

