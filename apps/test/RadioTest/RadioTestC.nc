 
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
    interface RadioSend;
    interface RadioPacket;
  }
}
implementation {

#define IS_TX (TOS_NODE_ID == 1)
#define PKLEN 10  

  uint8_t sw = 0;
  message_t pkt;
  bool busy = FALSE;
  
  event void Boot.booted() {
    uint8_t i;
    uint8_t* pl = (uint8_t*) ( ((void*)&pkt) + call RadioPacket.headerLength(&pkt));
        
    if ( IS_TX ) {
        call RadioPacket.setPayloadLength(&pkt,PKLEN);
        for( i = 0; i< PKLEN; ++i)
            *(pl++) = i*2;
        call RadioState.standby();
    }
    else
        call RadioState.turnOn();
  }

  event void MilliTimer.fired() {
    if ( ! busy  && call RadioSend.send(&pkt) == SUCCESS )
        busy = TRUE;
  }

  tasklet_async event void RadioState.done() {
    call Leds.led0Toggle();
    if ( IS_TX )
        call MilliTimer.startPeriodic(1000);
  }
 
  tasklet_async event void RadioSend.sendDone(error_t error) {
    call Leds.led1Toggle();
    if ( error == SUCCESS ) {
        busy = FALSE;
    }
  }
  
  tasklet_async event void RadioSend.ready() { }

}




