configuration HplUSBDetectC {
  provides interface GpioPCInterrupt as NSuspend;

}
implementation {
  components HplAtm128InterruptC;
  NSuspend=HplAtm128InterruptC.PCInt5;
}