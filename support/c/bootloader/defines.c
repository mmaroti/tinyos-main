//#include "util/delay.h"
#include "defines.h"

// number of CPU cycles per 1/512 sec
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
	for(wraps = 255; wraps != 0; --wraps)
	{
		while( TCNT2 != 0 )
			;
		now = TCNT1;

		while( TCNT2 != 64 )	// wait 64/32768 = 1/512 sec
			;

		cycles = TCNT1;

	}
	cycles-=now;

	/* Reset to boot state */
	TCCR1B = 0;
	ASSR = 0;
	TCCR2B = 0;
}

int baudrateRegister(uint32_t baudrate) {
	
	init();

	// we use the fast setting: (cycles * 512) / (8 * baudrate) - 1
	return (((uint32_t)cycles) << 6) / ((uint32_t)baudrate) - 1;
}

