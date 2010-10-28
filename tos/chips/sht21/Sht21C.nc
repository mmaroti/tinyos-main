configuration Sht21C {
  provides interface Read<uint16_t> as Temperature;
  provides interface Read<uint16_t> as Humidity;
}
implementation {
  components Sht21P;
  components new TimerMilliC();
  
  Temperature = Sht21P.Temperature;
  Humidity    = Sht21P.Humidity;

  Sht21P.Timer -> TimerMilliC;
  
  components HplSht21C;
  Sht21P.I2CPacket -> HplSht21C;
}
