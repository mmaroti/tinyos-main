/** Copyright (c) 2010, University of Szeged
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
* Author: Csepe Zoltan
*/

#include <Timer.h>
#include "TempStorage.h"

module TempWriteC {
	uses interface Boot;
	uses interface Leds;
	uses interface Timer<TMilli> as Timer0;
	uses interface Read<uint16_t>;
    	uses interface LogWrite;
	uses interface GlobalTime<TMilli>;
        uses interface TimeSyncInfo;
	uses interface TimeSyncNotify;
	uses interface Receive;
        uses interface AMSend;
        uses interface Packet;
	uses interface LogRead;
	uses interface SplitControl as RadioControl;
	uses interface LowPowerListening as LPL;
	
}
implementation {
	 
	typedef nx_struct logentry_t {
	    nx_uint16_t nodeID;
	    nx_uint32_t time;
	    nx_uint16_t temp;
	  } logentry_t;
	bool m_busy = TRUE;
	uint32_t ts;
	uint16_t c=1;
	uint16_t set=0;
	uint16_t set2=1;
  	logentry_t m_entry;
	message_t pkt;
	bool busy=FALSE;


	event void Boot.booted() {
		call RadioControl.start();
	}

	event void RadioControl.startDone(error_t err){
		if (err == SUCCESS){
			call Timer0.startPeriodic(TIMER_PERIOD_MILLI_WRITE);	
		}
		else {
			call RadioControl.start();
		}
	}
	
	event void Timer0.fired() {
		if (c==1) {if (set2==0) {call Timer0.startPeriodic(TIMER_PERIOD_MILLI_WRITE); set2=1;}
				call Read.read();
				set=0;}
		else if (c==2) { if (set==0) {call Timer0.startPeriodic(TIMER_PERIOD_MILLI_READ); set=1;}
				call LogRead.read(&m_entry, sizeof(logentry_t));
				set2=0;
				}
		else if (c==3) {
				if (call LogWrite.erase() == SUCCESS) {
				call Leds.led0On();
				set=0;
				set2=0;
		      		}
		}
		else if (c==4){call Leds.led2Toggle(); set=0; set2=0;}
	}
	
	event void Read.readDone(error_t result, uint16_t data) {
		m_entry.nodeID=TOS_NODE_ID;
		if(call GlobalTime.getGlobalTime(&ts)==SUCCESS)m_entry.time=ts;
		else m_entry.time=0;
		m_entry.temp=data;
		call LogWrite.append(&m_entry, sizeof(logentry_t));
	}

	event void LogWrite.appendDone(void* buf, storage_len_t len, bool recordsLost, error_t err) {
    	if (err==SUCCESS)call Leds.led1Toggle();
  	}
	
	event message_t* Receive.receive(message_t* msgPtr, void* payload, uint8_t len){
		call Leds.set(0);
		if(len==sizeof(ControlMsg)){
			ControlMsg* btrpkt = (ControlMsg*)payload;
			c=btrpkt->control;
			call Leds.led1On();
			call Timer0.startPeriodic(TIMER_PERIOD_MILLI_DEFAULT);
			call Leds.led1Off();
		}
	return msgPtr;
	}

	event void LogRead.readDone(void* buf, storage_len_t len, error_t err) {
		
		if(err==SUCCESS){
			call Leds.led0Toggle();
			if (!busy) {
			BlinkToRadioMsg* btrpkt = (BlinkToRadioMsg*)(call Packet.getPayload(&pkt, sizeof(BlinkToRadioMsg)));
			btrpkt->temperature = m_entry.temp;
			btrpkt->time = m_entry.time;
			btrpkt->nodeID=m_entry.nodeID;
			if (call AMSend.send(AM_BROADCAST_ADDR, &pkt, sizeof(BlinkToRadioMsg)) == SUCCESS) {
				busy = TRUE;
				}
			}
		}
		
	}

	event void AMSend.sendDone(message_t* msg, error_t error) {
	if (error == SUCCESS) {
			busy = FALSE;
			call Leds.led1Toggle();
		}
	}

	event void LogWrite.eraseDone(error_t err) {
	if (err==SUCCESS) call Leds.led0Off();	
	c=4;
	}
	event void LogWrite.syncDone(error_t err) {}
	event void TimeSyncNotify.msg_received() {
		call Leds.led2Toggle();
	}
	event void TimeSyncNotify.msg_sent() {}
	event void RadioControl.stopDone(error_t err){}
	event void LogRead.seekDone(error_t err) {}

}





