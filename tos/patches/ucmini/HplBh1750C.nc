configuration HplBh1750C {
  provides interface I2CPacket<TI2CBasicAddr>;
  provides interface Resource;
}
implementation {

  components new Atm128I2CMasterC() as I2CBus, Bh1750fviC, HplI2CBusControlC;

  I2CPacket = I2CBus.I2CPacket;
  Resource  = I2CBus.Resource;
  
  Bh1750fviC.BusControl -> HplI2CBusControlC;
}
