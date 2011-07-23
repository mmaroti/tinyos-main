
#include "RadioTest.h"
#include "RadioAssert.h"

configuration RadioTestAppC {}
implementation {
  components MainC, RadioTestC as App, RadioStackC as Radio, LedsC, AssertC;
  components new TimerMilliC();
  components DiagMsgC;
  
  App.Boot -> MainC.Boot;
  App.Leds -> LedsC;
  App.MilliTimer -> TimerMilliC;
  App.DiagMsg -> DiagMsgC;
  App.RadioState -> Radio;
  
  Radio.Boot -> MainC.Boot;
}


