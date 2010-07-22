#ifndef TEMPSTORAGE_H
#define TEMPSTORAGE_H

enum{
	AM_BLINKTORADIOMSG = 15,
	AM_CONTROLMSG = 20,
	TIMER_PERIOD_MILLI_READ=100,
	TIMER_PERIOD_MILLI_WRITE=600,
};

typedef nx_struct BlinkToRadioMsg {
	nx_uint16_t nodeID;
	nx_uint32_t time;
	nx_uint16_t temperature;
} BlinkToRadioMsg;

typedef nx_struct ControlMsg{
	nx_uint16_t control;
} ControlMsg;

#endif

