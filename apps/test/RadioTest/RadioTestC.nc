 
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

  uint8_t sw = 0;
  message_t pkt;
  bool busy = FALSE;
  
  uint8_t starter = 0;
  uint8_t pktlen = 0;
  uint8_t pktlen_count = 0;
  
  bool makePacket() {
    uint8_t i;
    uint8_t* pl = (uint8_t*) ( ((void*)&pkt) + call RadioPacket.headerLength(&pkt));
  
    if ( ++pktlen_count %2 == 0 )
        ++pktlen;  
        
    call RadioPacket.setPayloadLength(&pkt,pktlen%125+1);
    for( i = 0; i< pktlen%125+1; ++i)
        *(pl++) = starter + i;
        
    ++starter;
    return TRUE;
  }
  
  event void Boot.booted() {
    if ( IS_TX )
        call RadioState.standby();
    else
        call RadioState.turnOn();
  }

  event void MilliTimer.fired() {
    if ( ! busy && makePacket() && call RadioSend.send(&pkt) == SUCCESS )
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




