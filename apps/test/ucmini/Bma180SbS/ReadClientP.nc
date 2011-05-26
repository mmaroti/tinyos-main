module ReadClientP{
  provides interface Read<uint16_t>[uint8_t client];
  uses interface Read as ActualRead<uint16_t>;
}
implementation{
  uint8_t client;
  
  command error_t Read.read[uint8_t cl](){
    client=cl;
    return call ActualRead.read();
  }
  
  event void ActualRead.readDone(error_t result, uint16_t val) {
    signal Read.readDone[client](err, value);
  }
}