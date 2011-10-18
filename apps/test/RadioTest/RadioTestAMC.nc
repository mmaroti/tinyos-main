#include "Timer.h"
#include "RadioTest.h"
#include "message.h"
#include "Tasklet.h"

module RadioTestAMC @safe() {
	uses {
		interface Leds;
		interface Boot;
		interface Timer<TMilli> as MilliTimer;
		interface DiagMsg;
		
		interface Packet;
		interface SplitControl;
		interface AMSend;
	}
}

implementation {

#warning "RadioTest is in AM mode"
#define PKTSIZE 10

	norace bool accept;
	message_t pkt;
	norace bool busy = FALSE;

	bool makePacket() {
		uint8_t i;
		uint8_t* pl;
		
		call Packet.clear(&pkt);
      pl = (uint8_t*)(call Packet.getPayload(&pkt,PKTSIZE));
		for( i = 0; i< PKTSIZE; ++i)
			*(pl++) = TOS_NODE_ID + i;

		return TRUE;
	}

	event void Boot.booted() {
		makePacket();
		while ( call SplitControl.start() != SUCCESS ) ;
	}
	
	event void SplitControl.stopDone(error_t error) {}
	event void SplitControl.startDone(error_t error) {
		if ( error != SUCCESS ) {
			call Leds.set(0x7);
			return;
		}
		
		call MilliTimer.startPeriodic(100);
		call Leds.led0On();
	}

	event void MilliTimer.fired() {
		if ( ! busy && call AMSend.send(AM_BROADCAST_ADDR, &pkt, PKTSIZE) == SUCCESS ) {
			call Leds.led1On();
			busy = TRUE;
		}
	}
	event void AMSend.sendDone(message_t* msg, error_t error) {
		call Leds.led1Off();
		busy = FALSE;
	}

}
