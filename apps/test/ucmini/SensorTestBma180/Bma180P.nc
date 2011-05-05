module Bma180P {
  provides interface Read<uint8_t>;
  provides interface SplitControl;
  uses interface FastSpiByte;
  uses interface AsyncStdControl;
  uses interface Leds;
}
implementation {
  uint8_t retv = 0;

  command error_t SplitControl.start() {
    call AsyncStdControl.start();
    signal SplitControl.startDone(SUCCESS);
    return SUCCESS;
  }

  command error_t SplitControl.stop() {
    call AsyncStdControl.stop();
    return SUCCESS;
  }

  command error_t Read.read() {
    //UsartSpi.chipSelect();
    //call Leds.led2Toggle();
    call FastSpiByte.write(0x80);// call Leds.led1On();
    retv = call FastSpiByte.write(0);
    signal Read.readDone(SUCCESS, retv);
    //UsartSpi.chipDeselect();
    return SUCCESS;
  }
}
