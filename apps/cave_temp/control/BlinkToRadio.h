#ifndef BLINKTORADIO_H
#define BLINKTORADIO_H

enum{
	AM_BLINKTORADIOMSG = 6,
	TIMER_PERIOD_MILLI=250,
	AM_CONTROLMSG = 20,
};

typedef nx_struct BlinkToRadioMsg {
	nx_uint16_t nodeID;
	nx_uint16_t counter;
	nx_uint16_t counter2;
} BlinkToRadioMsg;

typedef nx_struct ControlMsg{
	nx_uint16_t control;
}ControlMsg;

#endif


