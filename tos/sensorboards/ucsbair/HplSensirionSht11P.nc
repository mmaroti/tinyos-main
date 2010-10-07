#include "Timer.h"

module HplSensirionSht11P {
  provides interface SplitControl;
  uses interface Timer<TMilli>;
  uses interface GeneralIO as DATA;
  uses interface GeneralIO as SCK;
}
implementation {
  task void stopTask();

  command error_t SplitControl.start() {
    call Timer.startOneShot( 11 );
    return SUCCESS;
  }
  
  event void Timer.fired() {
    signal SplitControl.startDone( SUCCESS );
  }

  command error_t SplitControl.stop() {
    call SCK.makeInput();
    call SCK.clr();
    call DATA.makeInput();
    call DATA.clr();
    
    post stopTask();
    return SUCCESS;
  }

  task void stopTask() {
    signal SplitControl.stopDone( SUCCESS );
  }
}

