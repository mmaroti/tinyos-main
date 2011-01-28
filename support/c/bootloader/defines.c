//#include "util/delay.h"
#include "defines.h"
#define MATCHES 50
#define MAXERROR 1
//SERIAL
// number of CPU cycles per 1/512 sec
uint16_t cycles;

/* Measure clock cycles per Jiffy (1/32768s) */
void init(void) {
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
	//cycles=31250;
	// we use the fast setting: (cycles * 512) / (8 * baudrate) - 1
	return cycles / (baudrate >> 6) - 1;
}

//LEDS

void led0On(void) {
  #if PLATFORM == IRIS
  PORTA&=~_BV(PA0);
  #elif PLATFORM == UCMINI049
  PORTE|=_BV(PE3);
  #else
  PORTE|=_BV(PE4);
  #endif
}

void led1On(void) {
  #if PLATFORM == IRIS
  PORTA&=~_BV(PA1);
  #else
  PORTE|=_BV(PE5);
  #endif
}

void led2On(void) {
  #if PLATFORM == IRIS
  PORTA&=~_BV(PA2);
  #else
  PORTE|=_BV(PE6);
  #endif
}

void led3On(void) {
  #if PLATFORM == IRIS
  return;
  #else
  PORTE|=_BV(PE7);
  #endif
}

void led0Off(void) {
  #if PLATFORM == IRIS
  PORTA|=_BV(PA0);
  #elif PLATFORM == UCMINI049
  PORTE&=~_BV(PE3);
  #else
  PORTE&=~_BV(PE4);
  #endif
}

void led1Off(void) {
  #if PLATFORM == IRIS
  PORTA|=_BV(PA1);
  #else
  PORTE&=~_BV(PE5);
  #endif
}

void led2Off(void) {
  #if PLATFORM == IRIS
  PORTA|=_BV(PA2);
  #else
  PORTE&=~_BV(PE6);
  #endif
}

void led3Off(void) {
  #if PLATFORM == IRIS
  return;
  #else
  PORTE&=~_BV(PE7);
  #endif
}

void led0Toggle(void){
  #if PLATFORM == IRIS
  if( ~(PORTA&_BV(PA0)) )
  #elif PLATFORM == UCMINI049
  if( PORTE&_BV(PE3) )
  #else
  if( PORTE&_BV(PE4) )
  #endif
  {
	led0Off();
  } else
	led0On();
}

void led1Toggle(void){
  #if PLATFORM == IRIS
  if( ~(PORTA&_BV(PA1)) )
  #else
  if( PORTE&_BV(PE5) )
  #endif
  {
	led1Off();
  } else
	led1On();
}

void led2Toggle(void){
  #if PLATFORM == IRIS
  if( ~(PORTA&_BV(PA2)) )
  #else
  if( PORTE&_BV(PE6) )
  #endif
  {
	led2Off();
  } else
	led2On();
}

void led3Toggle(void){
  #if PLATFORM == IRIS
  return;
  #else
  if( PORTE&_BV(PE7) )
  	led3Off();
  else 
	led3On();
  #endif
}

void ledSet(uint8_t val){
  if (val & 1) {
	led0On();
  } else {
	led0Off();
  }
  if (val & 2) {
	led1On();
  }
  else {
	led1Off();
  }
  if (val & 4) {
	led2On();
  }
  else {
	led2Off();
  }
  if (val & 8) {
	led3On();
  }
  else {
	led3Off();
  }
}



