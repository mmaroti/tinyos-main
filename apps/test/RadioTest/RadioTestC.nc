 
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
    interface RadioReceive;
  }
}
implementation {

#define IS_TX (TOS_NODE_ID == 1)

  bool accept;
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
    if ( IS_TX ) {
        call Leds.led0Off();
        call RadioState.standby();
    } else {
        accept = TRUE;
        call Leds.led0On();
        call RadioState.turnOn();
    }
  }

  event void MilliTimer.fired() {
    call Leds.led1On();
    if ( ! busy && makePacket() && call RadioSend.send(&pkt) == SUCCESS ) {
        busy = TRUE;
    }
  }

  tasklet_async event void RadioState.done() {
    call Leds.led0Toggle();
    if ( IS_TX )
        call MilliTimer.startPeriodic(1000);
  }
 
  tasklet_async event void RadioSend.ready() { }
 
  tasklet_async event void RadioSend.sendDone(error_t error) {
    call Leds.led1Off();
    busy = FALSE;
  }
  
  tasklet_async event bool RadioReceive.header(message_t* msg)
  {
    accept = !accept;
    return accept;
  }

  tasklet_async event message_t* RadioReceive.receive(message_t* msg)
  {

    uint8_t i,l;
    uint8_t* data = ((void*)msg)+ call RadioPacket.headerLength(msg);
  	l = call RadioPacket.payloadLength(msg);    
    call Leds.led2On();
            
    // print the whole packet
  	if( call DiagMsg.record() ) {
      call DiagMsg.str("packet");
      call DiagMsg.hex8(l);
      call DiagMsg.send();
   	}

    for ( i = 0; i < l/15; ++i  ) {
	    if( call DiagMsg.record() ) {
	    call DiagMsg.hex8s(data+i*15,15);
	    call DiagMsg.send();
		}
	}
	if( l%15 != 0 && call DiagMsg.record() ) {
        call DiagMsg.hex8s(data+i*15,l%15);
        call DiagMsg.send();
	}

    call Leds.led2Off();
    return msg;
  }


}




