#include "Timer.h"
#include "RadioTest.h"
#include "message.h"
#include "Tasklet.h"

module RadioTestParallelC @safe() {
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

#warning "RadioTest is in Parallel mode"

	message_t pkt;
	norace bool busy = FALSE;

	void makePacket() {
		uint8_t i;
		uint8_t* pl = (uint8_t*) ( ((void*)&pkt) + call RadioPacket.headerLength(&pkt));
	
		call RadioPacket.setPayloadLength(&pkt,20);
		for( i = 0; i< 20; ++i)
			*(pl++) = TOS_NODE_ID + i;
	}

	event void Boot.booted() {
		while ( SUCCESS != call RadioState.turnOn() ) ;
		makePacket();
		call MilliTimer.startPeriodic(TOS_NODE_ID * 100);
	}

	tasklet_async event void RadioState.done() {
		call Leds.led0On();
	}

	event void MilliTimer.fired() {
		if ( ! busy && call RadioSend.send(&pkt) == SUCCESS ) {
			call Leds.led1On();
			busy = TRUE;
		}
	}

	tasklet_async event void RadioSend.ready() { 
	}

	tasklet_async event void RadioSend.sendDone(error_t error) {
		call Leds.led1Off();
		busy = FALSE;
	}

	tasklet_async event bool RadioReceive.header(message_t* msg)
	{
		return TRUE;
	}

	tasklet_async event message_t* RadioReceive.receive(message_t* msg)
	{

		uint8_t i,l;
		uint8_t* data = ((void*)msg);
		l = call RadioPacket.payloadLength(msg) + call RadioPacket.headerLength(msg);
		call Leds.led2On();

		// print the whole packet
		for ( i = 0; i < l/15; ++i	) {
			if( call DiagMsg.record() ) {
				call DiagMsg.hex8s(data+i*15,15);
				call DiagMsg.send();
			}
		}
		if( l%15 != 0 && call DiagMsg.record() ) {
			call DiagMsg.hex8s(data+i*15,l%15);
			call DiagMsg.send();
		}

			if( call DiagMsg.record() ) {
		call DiagMsg.str("packet");
		call DiagMsg.hex8(l);
		call DiagMsg.send();
	 	}
		call Leds.led2Off();
		return msg;
	}

}

