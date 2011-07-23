 
#include "Timer.h"
#include "RadioTest.h"
#include "message.h"
#include "Tasklet.h"

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

  bool sw = TRUE;

  event void Boot.booted() {
    call Leds.led0On();
   // call MilliTimer.startPeriodic(1000);
  }

  event void MilliTimer.fired() {
    if (sw)
        call RadioState.turnOn();
    else
        call RadioState.turnOff();
    sw = !sw;
  }

  tasklet_async event void RadioState.done() {
  }

}




