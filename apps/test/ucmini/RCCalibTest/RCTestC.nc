configuration RCTestC {
}
implementation {
  components MainC, RCTestP, RCCalibrateP;

  RCTestP.Boot -> MainC;
  RCTestP.RCCalibrate -> RCCalibrateP;
}
