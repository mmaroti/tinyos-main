configuration HplSensirionSht21C {
  provides interface Resource[ uint8_t id ];   
}
implementation {
  components HplAtm128GeneralIOC;

  components new HplAtm128GeneralIOPinP() as PWRM;  //pin selection
  PWRM -> HplAtm128GeneralIOC.PortF2;               //pin definition

  components HplSensirionSht21P;
  HplSensirionSht21.PWR -> PWRM;

  components new TimerMilliC();
  HplSensirionSht21P.Timer -> TimerMilliC;

  components new FcfsArbiterC( "Sht21.Resource" ) as Arbiter;
  Resource = Arbiter;
  
  components new SplitControlPowerManagerC();
  SplitControlPowerManagerC.SplitControl -> HplSensirionSht11P;
  SplitControlPowerManagerC.ArbiterInfo -> Arbiter.ArbiterInfo;
  SplitControlPowerManagerC.ResourceDefaultOwner -> Arbiter.ResourceDefaultOwner;
}
