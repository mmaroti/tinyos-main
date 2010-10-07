configuration HplSensirionSht11C {
  provides interface Resource[ uint8_t id ];
  provides interface GeneralIO as DATA;
  provides interface GeneralIO as SCK;
  provides interface GpioInterrupt as InterruptDATA;
}
implementation {
  components MicaBusC;

  DATA = MicaBusC.Int3;
  SCK  = MicaBusC.PW3;

  components HplSensirionSht11P;
  
  HplSensirionSht11P.DATA -> MicaBusC.Int3;	
  HplSensirionSht11P.SCK -> MicaBusC.PW3;

  components new TimerMilliC();
  HplSensirionSht11P.Timer -> TimerMilliC;

  InterruptDATA = MicaBusC.Int3_Interrupt;

  components new FcfsArbiterC( "Sht11.Resource" ) as Arbiter;
  Resource = Arbiter;

  components new SplitControlPowerManagerC();
  SplitControlPowerManagerC.SplitControl -> HplSensirionSht11P;
  SplitControlPowerManagerC.ArbiterInfo -> Arbiter.ArbiterInfo;
  SplitControlPowerManagerC.ResourceDefaultOwner -> Arbiter.ResourceDefaultOwner;
}
