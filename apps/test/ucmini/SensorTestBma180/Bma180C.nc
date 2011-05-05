configuration Bma180C {
  provides interface Read<uint8_t>; 
  provides interface SplitControl;
}
implementation {
  components Bma180P, HplBma180C;
  Read = Bma180P;
  SplitControl = Bma180P;
  Bma180P.FastSpiByte -> HplBma180C;
  Bma180P.AsyncStdControl -> HplBma180C;

  components LedsC;
  Bma180P.Leds -> LedsC; 
}
