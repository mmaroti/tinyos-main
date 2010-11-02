configuration HplSht21C {
  provides interface I2CPacket<TI2CBasicAddr> ; 
}
implementation {
  
  components Sht21P;
  components new Atm128I2CMasterC() as I2CBus;
  
  I2CPacket = I2CBus;
}
