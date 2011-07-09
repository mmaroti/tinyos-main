 
#include "Timer.h"
#include "RadioTest.h"
#include "message.h"

module RadioTestC @safe() {
  uses {
    interface Leds;
    interface Boot;
    interface Timer<TMilli> as MilliTimer;
    interface DiagMsg;
    interface RadioState;
  }
}
implementation {

  bool sw = FALSE;

  event void Boot.booted() {
    call Leds.led0On();
    call MilliTimer.startPeriodic(1000);
  }

  event void MilliTimer.fired() {
    if (sw)
        call RadioState.turnOn();
    else
        call RadioState.standby();
    sw = !sw;
  }

  async event void RadioState.done() {
  }

}




