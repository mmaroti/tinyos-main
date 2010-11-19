#include "SerialAdder.h"

configuration SerialAdderAppC {}
implementation {
  components SerialAdderC as Application, MainC;
  components SerialActiveMessageC as AM;

  Application.Boot -> MainC;
  Application.Control -> AM;
  Application.Receive -> AM.Receive[AM_TEST_SERIAL_MSG];
  Application.AMSend -> AM.AMSend[AM_TEST_SERIAL_MSG];
  Application.Packet -> AM;
  
}
