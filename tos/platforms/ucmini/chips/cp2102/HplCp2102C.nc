configuration HplCp2102C {
  provides interface AtmegaPinChange as Vdd;
  #if (UCMINI_REV != 49)
    provides interface AtmegaPinChange as NSuspend;
  #endif

}
implementation {
  components AtmegaPinChange0C;
  
  Vdd=AtmegaPinChange0C.AtmegaPinChange[7];
  #if (UCMINI_REV != 49)
    NSuspend=AtmegaPinChange0C.AtmegaPinChange[5];
  #endif
}