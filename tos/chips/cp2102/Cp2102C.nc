configuration Cp2102C {
  provides interface UsbState;
}
implementation {
  components Cp2102P, HplCp2102C, RealMainP;
  
  Cp2102P.Vdd->HplCp2102C.Vdd;
  RealMainP.PlatformInit -> Cp2102P.PlatformInit;
  
  UsbState = Cp2102P;
}