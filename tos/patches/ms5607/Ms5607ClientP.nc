generic module Ms5607ClientP(){
  provides interface Read<uint32_t>[uint8_t client];
  uses interface Read<uint32_t> as ActualRead;
}
implementation{
  uint8_t client;
  
  command error_t Read.read[uint8_t cl](){
    client=cl;
    return call ActualRead.read();
  }
  
  event void ActualRead.readDone(error_t result, uint32_t value) {
    signal Read.readDone[client](result, value);
  }
}