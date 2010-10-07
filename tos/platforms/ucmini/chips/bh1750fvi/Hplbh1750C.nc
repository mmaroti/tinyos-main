configuration Hplbh1750C {
  provides interface I2CPacket<TI2CBasicAddr>;
}
implementation {
  components bh1750fviP;
  components new Atm128I2CMasterC() as I2CBus;

  bh1750fviP.I2CPacket -> I2CBus;
}
