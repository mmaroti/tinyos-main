configuration bh1750fviC {
  provides interface Read<uint16_t> as Light;
}
implementation {
  components bh1750fviP;
  components new TimerMilliC();

  Light = bh1750fviP.Light;
  bh1750fviP.Timer -> TimerMilliC;

  components Hplbh1750C;
  bh1750fviP.I2CPacket -> Hplbh1750C;
}
