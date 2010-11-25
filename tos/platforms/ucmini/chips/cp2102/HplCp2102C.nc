configuration HplCp2102C {
  provides interface GpioPCInterrupt as Vdd;

}
implementation {
  components HplAtm128InterruptC;
  
  Vdd=HplAtm128InterruptC.PCInt7;
}