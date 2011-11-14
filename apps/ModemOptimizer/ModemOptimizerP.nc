/*
* Copyright (c) 2011, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Krisztian Veress
*         veresskrisztian@gmail.com
*/

#include "Timer.h"
#include "message.h"
#include "Tasklet.h"
#include "ModemOptimizer.h"

module ModemOptimizerP @safe() {
	uses {
		interface Leds;
		interface Boot;
		interface Timer<TMilli> as MilliTimer;
	
		interface DiagMsg;
		interface ModemOptimizer;
		
		interface RadioState;
		interface RadioSend;
		interface RadioPacket;
		interface RadioReceive;
	}
}

implementation {

	enum {
		STATE_STOPPED = 0,
		STATE_RUNNING,
		
		LOCKED,
		UNLOCKED,
	};
	
	uint8_t 	state = STATE_STOPPED;
	uint8_t 	sendlock = UNLOCKED;
	
	moptimizer_t 					bmark;
	message_t						pkt;
	tasklet_norace uint32_t 	currentid = 0;
	
	void signalEnd() { call Leds.set(0x7); }

	void displayData() {
		DIAGMSG_S("MOPT displayData!");
		call Leds.led2On();
		// Sender
		if ( TOS_NODE_ID == 1 ) {
			atomic {
				if ( call DiagMsg.record() ) {
					call DiagMsg.str("MOs");
					call DiagMsg.uint32(currentid);
					call DiagMsg.uint16(bmark.srequest);
					call DiagMsg.uint16(bmark.sbusy);
					call DiagMsg.uint16(bmark.saccept);
					call DiagMsg.uint16(bmark.serror);
					call DiagMsg.uint16(bmark.ssuccess);
					call DiagMsg.send();
	
					call Leds.led2Off();
				}
			}
		// Receiver
		} else {
			atomic {
				if ( call DiagMsg.record() ) {
					call DiagMsg.str("MOr");
					call DiagMsg.uint32(currentid);
					call DiagMsg.uint16(bmark.rsync);
					call DiagMsg.uint16(bmark.rerror);
					call DiagMsg.uint16(bmark.rcrc);
					call DiagMsg.uint16(bmark.rsuccess);
					call DiagMsg.send();

					call Leds.led2Off();
				}
			}
		}
	}	
	
	void makePacket(uint8_t pktsize, uint32_t bmarkid) {
		uint8_t* pl8 = (uint8_t*) ( ((void*)&pkt) + call RadioPacket.headerLength(&pkt));
		call RadioPacket.setPayloadLength(&pkt,pktsize);
		
		// insert the benchmark id
		*((uint32_t*)pl8) = bmarkid;
		pl8 += 4;
		pktsize-= 4;
		
		// fill the rest payload with an arithmetic sequence
		while ( pktsize-- ) *(pl8 + pktsize) = pktsize;
	}
	
	void startNextBenchmark() {
		DIAGMSG_S("MOPT startNext!");
		
		// init the new benchmark
		atomic memset(&bmark,0,sizeof(moptimizer_t));
		
		if ( TOS_NODE_ID == 1 )
			// make the packet with the bechmark id
			makePacket( MOPT_PACKET_SIZE, currentid );	
	
		// setup the driver
		call ModemOptimizer.reset();
		call ModemOptimizer.configure(currentid);
	}
			
	task void timerTask() {
		call MilliTimer.startOneShot(MOPT_RUNTIME_MSEC);
	}
	
	task void sendTask() {
		uint8_t lstate, llock;
		atomic {
			lstate = state;
      	llock = sendlock;	
      	sendlock = LOCKED;		
		}		
		
		if ( lstate == STATE_RUNNING ) {
			if ( llock != LOCKED && call RadioSend.send(&pkt) == SUCCESS ) {
				call Leds.led1On();
			} else
				post sendTask();
		}
	}

	event void Boot.booted() {
		DIAGMSG_S("MOPT booted!");
		call RadioState.turnOn();
		call Leds.led0On();
		startNextBenchmark();
	}
	
	tasklet_async event void ModemOptimizer.configured() {
		DIAGMSG_S("MOPT configured!");
		if ( TOS_NODE_ID == 1 ) {
			atomic state = STATE_RUNNING;		
			post sendTask();
		}
		post timerTask();
	}

	event void MilliTimer.fired() {
		DIAGMSG_S("MOPT fired!");
		atomic state = STATE_STOPPED;
		displayData();
		if ( ++currentid < call ModemOptimizer.configCount() )
			startNextBenchmark();
		else
			signalEnd();
	}
	
	tasklet_async event void RadioSend.sendDone(error_t error) {
		call Leds.led1Off();
		atomic {
			sendlock = UNLOCKED;
			if ( state == STATE_RUNNING )
				post sendTask();
		}
	}
	
	tasklet_async event message_t* RadioReceive.receive(message_t* msg)
	{
		// get the bmark id from the message
		uint32_t id = *((uint32_t*) ( ((void*)msg) + call RadioPacket.headerLength(msg)));
				
		call Leds.led1Toggle();	
		// if new benchmark id seen
		if ( id != currentid )
			DIAGMSG_U32("MOPT RX fw",id);

		return msg;
	}

	tasklet_async event void RadioState.done() { }
	tasklet_async event void RadioSend.ready() { }
	tasklet_async event bool RadioReceive.header(message_t* msg) { return TRUE; }
	
	tasklet_async event void ModemOptimizer.sendRequest()			{ atomic ++bmark.srequest; }
	tasklet_async event void ModemOptimizer.sendBusy()				{ atomic ++bmark.sbusy; }
	tasklet_async event void ModemOptimizer.sendAccept()			{ atomic ++bmark.saccept; }
	tasklet_async event void ModemOptimizer.sendError()			{ atomic ++bmark.serror; }
	tasklet_async event void ModemOptimizer.sendSuccess()			{ atomic ++bmark.ssuccess; }
	
	
	tasklet_async event void ModemOptimizer.receiveSync()			{ atomic ++bmark.rsync; }
	tasklet_async event void ModemOptimizer.receiveError()		{ atomic ++bmark.rerror; }
	tasklet_async event void ModemOptimizer.receiveCRC()			{ atomic ++bmark.rcrc; }
	tasklet_async event void ModemOptimizer.receiveSuccess()		{ atomic ++bmark.rsuccess; }
}

