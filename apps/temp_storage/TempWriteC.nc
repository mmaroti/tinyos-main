
#include <Timer.h>
#include "TempStorage.h"

module TempWriteC {
	uses interface Boot;
	uses interface Leds;
	uses interface Timer<TMilli> as Timer0;
	uses interface Read<uint16_t>;
    	uses interface LogWrite;
	
}
implementation {
	 
	typedef nx_struct logentry_t {
	    nx_uint16_t nodeID;
	    nx_uint16_t time;
	    nx_uint16_t temp;
	  } logentry_t;
	bool m_busy = TRUE;
	nx_uint16_t counter;
  	logentry_t m_entry;


	event void Boot.booted() {
		call Timer0.startPeriodic(TIMER_PERIOD_MILLI_WRITE);
	}

	
	event void Timer0.fired() {
		call Read.read();
	}
	
	event void Read.readDone(error_t result, uint16_t data) {
		counter++;
		m_entry.nodeID=TOS_NODE_ID;
		m_entry.time=counter;
		m_entry.temp=data;
		call LogWrite.append(&m_entry, sizeof(logentry_t));
	}

	event void LogWrite.appendDone(void* buf, storage_len_t len, bool recordsLost, error_t err) {
    	if (err==SUCCESS)call Leds.led1Toggle();
  	}
	
	event void LogWrite.eraseDone(error_t err) {}
	event void LogWrite.syncDone(error_t err) {}

}





