
#include "defines.h"

// number of CPU cycles per 1/1024 sec
uint16_t cycles;

/* Measure clock cycles per Jiffy (1/32768s) */
void init() {
	uint8_t wraps;
	uint16_t now;

	/* Setup timer2 to at 32768 Hz, and timer1 cpu cycles */
	TCCR1B = 1 << CS10;
	ASSR = 1 << AS2;
	TCCR2B = 1 << CS20;

	// one wrap is 256/32768 = 1/128 sec
	for(wraps = 128; wraps != 0; --wraps)
	{
		while( TCNT1 != 0 )
			;

		now = TCNT2;

		while( TCNT1 != 32 )	// wait 32/32768 = 1/1024 sec
			;

		cycles = TCNT2;
	}

	cycles -= now;

	/* Reset to boot state */
	TCCR1B = 0;
	ASSR = 0;
	TCCR2B = 0;
}

int baudrateRegister(uint32_t baudrate) {
	init();

	// we use the fast setting: (cycles * 1024) / (8 * baudrate) - 1
	return (((uint32_t)cycles) << 7) / baudrate - 1;
}

