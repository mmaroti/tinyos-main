configuration HplMs5607C {
  provides interface I2CPacket<TI2CBasicAddr> ; 
}
implementation {
  
  components ms5607P;
  components new Atm128I2CMasterC() as I2CBus;
  
  ms5607P.I2CPacket -> I2CBus;
  
}
