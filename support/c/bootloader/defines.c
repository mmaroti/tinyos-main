//#include "util/delay.h"
#include "defines.h"
#define MATCHES 10
#define MAXERROR 2
// number of CPU cycles per 1/512 sec
uint16_t cycles;

/* Measure clock cycles per Jiffy (1/32768s) */
void init() {
	uint8_t wraps_ok=0;
	uint8_t wraps=255;
	uint16_t now;
	uint16_t prev_cycles_min=0xffff;
	uint16_t prev_cycles_max=0;
	/* Setup timer2 to at 32768 Hz, and timer1 cpu cycles */
	
	TCCR1B = 1 << CS10;
	ASSR = 1 << AS2;
	TCCR2B = 1 << CS20;

	// one wrap is 256/32768 = 1/128 sec
	while( wraps_ok<MATCHES && --wraps != 255)
	{
		while( TCNT2 != 0 )
			;
		now = TCNT1;

		while( TCNT2 != 64 )	// wait 64/32768 = 1/512 sec
			;

		cycles = TCNT1 - now;
		
		
		if(prev_cycles_min<cycles)
		  prev_cycles_min=cycles;
		if(prev_cycles_max>cycles)
		  prev_cycles_max=cycles;
		
		if(prev_cycles_max-prev_cycles_min<=MAXERROR){
		  wraps_ok++;
		} else{
		  wraps_ok=0;
		  prev_cycles_min=0xffff;
		  prev_cycles_max=0;
		}

	}

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

