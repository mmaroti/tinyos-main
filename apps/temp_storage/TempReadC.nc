
#include <Timer.h>
#include "TempStorage.h"

module TempReadC {
	uses interface Boot;
	uses interface Leds;
	uses interface Timer<TMilli> as Timer0;
	uses interface Packet;
	uses interface AMSend;
	uses interface SplitControl as AMControl;
	uses interface LogRead;	
}
implementation {
	bool busy = FALSE;
	message_t pkt;
	 
	typedef nx_struct logentry_t {
	    nx_uint16_t nodeID;
	    nx_uint16_t time;
	    nx_uint16_t temp;
	  } logentry_t;
	bool m_busy = TRUE;
  	logentry_t m_entry;


	event void Boot.booted() {
		call AMControl.start();
	}

	event void AMControl.startDone(error_t err) {
		if (err == SUCCESS) {
			call Timer0.startPeriodic(TIMER_PERIOD_MILLI_READ);
		}
		else {
			call AMControl.start();
		}
	}
	
	event void Timer0.fired() {
		call LogRead.read(&m_entry, sizeof(logentry_t));
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
	
	event void AMControl.stopDone(error_t err) {}
	event void LogRead.seekDone(error_t err) {}

}





