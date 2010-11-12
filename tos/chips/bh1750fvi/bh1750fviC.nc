configuration bh1750fviC {
  provides interface Read<uint16_t> as Light;
  provides interface SplitControl;
}
implementation {
  components bh1750fviP;
  components new TimerMilliC();

  Light = bh1750fviP.Light;
  bh1750fviP.Timer -> TimerMilliC;

  components Hplbh1750C;
  bh1750fviP.I2CPacket -> Hplbh1750C;
  bh1750fviP.I2CResource -> Hplbh1750C.Resource;

  components DiagMsgC;
  bh1750fviP.DiagMsg -> DiagMsgC;

  SplitControl = bh1750fviP;

  components LedsC;
  bh1750fviP.Leds -> LedsC;
}
