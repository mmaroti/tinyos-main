configuration ms5607C {
  provides interface Read<uint32_t> as Pressure;
  provides interface SplitControl;
}
implementation {
  components ms5607P;
  components new TimerMilliC();

  Pressure = ms5607P.Pressure;
  ms5607P.Timer -> TimerMilliC;

  components HplMs5607C;
  ms5607P.I2CPacket -> HplMs5607C;
  ms5607P.I2CResource -> HplMs5607C.Resource;

  SplitControl = ms5607P;
}
