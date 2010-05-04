#ifndef TEMPSTORAGE_H
#define TEMPSTORAGE_H

enum{
	AM_BLINKTORADIOMSG = 15,
	TIMER_PERIOD_MILLI_READ=100,
	TIMER_PERIOD_MILLI_WRITE=60000,
};

typedef nx_struct BlinkToRadioMsg {
	nx_uint16_t nodeID;
	nx_uint16_t time;
	nx_uint16_t temperature;
} BlinkToRadioMsg;

#endif

