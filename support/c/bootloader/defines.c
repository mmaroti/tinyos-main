
#include "defines.h"
void init() {
    /* Measure clock cycles per Jiffy (1/32768s) */
    /* This code doesn't use the HPL to avoid timing issues when compiling
       with debugging on */
      
	uint8_t now, wraps;
	uint16_t start;

	/* Setup timer2 to count 32 jiffies, and timer1 cpu cycles */
	TCCR1B = 1 << CS10;
	ASSR = 1 << AS2;
	TCCR2B = 1 << CS21 | 1 << CS20;

	/* Wait for 1s for counter to stablilize after power-up (yes, it
	   really does take that long). That's 122 wrap arounds of timer 1
	   at 8MHz. */
	start = TCNT1;
	for (wraps = MAGIC / 2; wraps; )
	  {
	    uint16_t next = TCNT1;

	    if (next < start)
	      wraps--;
	    start = next;
	  }

	/* Wait for a TCNT0 change */
	now = TCNT2;
	while (TCNT2 == now) ;

	/* Read cpu cycles and wait for next TCNT2 change */
	start = TCNT1;
	now = TCNT2;
	while (TCNT2 == now) ;
	cycles = TCNT1;

	cycles = (cycles - start + 16) >> 5;

	/* Reset to boot state */
	ASSR = TCCR1B = TCCR2B = 0;
	TCNT2 = 0;
	TCNT1 = 0;
	TIFR1 = TIFR2 = 0xff;
	while (ASSR & (1 << TCN2UB | 1 << OCR2BUB | 1 << TCR2BUB))
	  ;
  }

int baudrateRegister(uint32_t baudrate) {
    // value is (cycles*32768) / (8*baudrate) - 1
    init();
    return ((uint32_t)cycles << 11) / baudrate - 1;
  }
