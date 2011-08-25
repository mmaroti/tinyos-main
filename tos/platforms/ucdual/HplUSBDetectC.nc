configuration HplUSBDetectC {
  provides interface AtmegaPinChange as NSuspend;

}
implementation {
  components AtmegaPinChange0C;
  NSuspend=AtmegaPinChange0C.AtmegaPinChange[5];
}