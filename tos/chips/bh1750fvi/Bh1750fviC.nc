configuration Bh1750fviC {
  provides interface Read<uint16_t> as Light;
  provides interface SplitControl;
}
implementation {
  components Bh1750fviP;
  components new TimerMilliC();

  Light = Bh1750fviP.Light;
  Bh1750fviP.Timer -> TimerMilliC;

  components Hplbh1750C;
  Bh1750fviP.I2CPacket -> Hplbh1750C;
  Bh1750fviP.I2CResource -> Hplbh1750C.Resource;

  components DiagMsgC;
  Bh1750fviP.DiagMsg -> DiagMsgC;

  SplitControl = Bh1750fviP;

  components LedsC;
  Bh1750fviP.Leds -> LedsC;
}
