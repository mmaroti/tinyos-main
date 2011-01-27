module RCTestP {
  uses interface Boot;
  uses interface RCCalibrate;
}
implementation {
  task void calib() { 
   call RCCalibrate.calibrateInternalRC(16000000);
  }

  event void Boot.booted() {
    post calib();
  }
}
