#include "Timer.h";

module HplSensirionSht21P {
  provides interface SplitControl;    //interface for power management
  uses interface Timer<TMilli>;       //for timing in millisec scale
  uses interface GeneralIO as PWR;    //for powering mgmnt
}
implementation {
  task void stopTask();

  command error_t SplitControl.start() {
    call PWR.makeOutput();
    call PWR.set();
    call Timer.startOneShot( 15 );   //max reset time 15 ms
    return SUCCESS;
  }
  
  event void Timer.fired() {
    signal SplitControl.startDone( SUCCESS );
  }

  command error_t SplitControl.stop() {
    call PWR.clr();
    post stopTask();
    return SUCCESS;
  }

  task void stopTask() {
    signal SplitControl.stopDone( SUCCESS );
  }
}

