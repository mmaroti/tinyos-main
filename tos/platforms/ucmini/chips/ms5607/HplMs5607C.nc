configuration HplMs5607C {
  provides interface I2CPacket<TI2CBasicAddr> ; 
}
implementation {
  
  components ms5607P;
  components new Atm128I2CMasterC() as I2CBus;
  
  I2CPacket = I2CBus;
}
