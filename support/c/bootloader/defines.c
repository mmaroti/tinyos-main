//#include "util/delay.h"
#include "defines.h"
#define MATCHES 50
#define MAXERROR 2
//SERIAL
// number of CPU cycles per 1/512 sec
uint16_t cycles;
uint8_t count=0;
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

void led0On(void) {
  #ifdef INVERTPOWER
    LED0PORT&=~_BV(LED0NUM);
  #else
    LED0PORT|=_BV(LED0NUM);
  #endif
}

void led1On(void) {
  #ifdef INVERTPOWER
    LED1PORT&=~_BV(LED1NUM);
  #else
    LED1PORT|=_BV(LED1NUM);
  #endif
}

void led2On(void) {
  #ifdef INVERTPOWER
    LED2PORT&=~_BV(LED2NUM);
  #else
    LED2PORT|=_BV(LED2NUM);
  #endif
}

void led3On(void) {
  #ifdef LED3PORT
    #ifdef INVERTPOWER
      LED3PORT&=~_BV(LED3NUM);
    #else
      LED3PORT|=_BV(LED3NUM);
    #endif
  #endif
}

void led0Off(void) {
  #ifdef INVERTPOWER
    LED0PORT|=_BV(LED0NUM);
  #else
    LED0PORT&=~_BV(LED0NUM);
  #endif
}

void led1Off(void) {
  #ifdef INVERTPOWER
    LED1PORT|=_BV(LED1NUM);
  #else
    LED1PORT&=~_BV(LED1NUM);
  #endif
}

void led2Off(void) {
  #ifdef INVERTPOWER
    LED2PORT|=_BV(LED2NUM);
  #else
    LED2PORT&=~_BV(LED2NUM);
  #endif
}

void led3Off(void) {
  #ifdef LED3PORT
    #ifdef INVERTPOWER
      LED3PORT|=_BV(LED3NUM);
    #else
      LED3PORT&=~_BV(LED3NUM);
    #endif
  #endif
}

void led0Toggle(void){
  #ifdef INVERTPOWER
  if(~(LED0PORT&_BV(LED0NUM)))
  #else
  if(LED0PORT&_BV(LED0NUM))
  #endif
  {
	led0Off();
  } else
	led0On();
}

void led1Toggle(void){
  #ifdef INVERTPOWER
  if(~(LED1PORT&_BV(LED1NUM)))
  #else
  if(LED1PORT&_BV(LED1NUM))
  #endif
  {
	led1Off();
  } else
	led1On();
}

void led2Toggle(void){
  #ifdef INVERTPOWER
  if(~(LED2PORT&_BV(LED2NUM)))
  #else
  if(LED2PORT&_BV(LED2NUM))
  #endif
  {
	led2Off();
  } else
	led2On();
}

void led3Toggle(void){
  #ifdef LED3PORT
    #ifdef INVERTPOWER
    if(~(LED3PORT&_BV(LED3NUM)))
    #else
    if(LED3PORT&_BV(LED3NUM))
    #endif
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

void ledInit(void){
  LED0DDR|=_BV(LED0NUM);
  LED1DDR|=_BV(LED1NUM);
  LED2DDR|=_BV(LED2NUM);
  #ifdef LED3DDR
  LED3DDR|=_BV(LED3NUM);
  #endif
  ledSet(0);
}

void status(uint16_t time_out)

{
//     ledSet((++count)/1000);
  #ifndef LED3PORT
  if(time_out>2*TIMEOUT_CYCLES/3){
    ledSet(7);
  }else if(time_out>TIMEOUT_CYCLES/3){
    ledSet(3);
  }else{
    ledSet(1);
  }
  #else
  if(time_out>3*TIMEOUT_CYCLES/4){
    ledSet(15);
  }else if(time_out>2*TIMEOUT_CYCLES/4){
    ledSet(7);
  }else if(time_out>TIMEOUT_CYCLES/4){
    ledSet(3);
  }else{
    ledSet(1);
  }
  #endif
}


//these functions returns 0, if the bootloader should be started
int checkBattery(void){
  #if PLATFORM==UCMINI049 || PLATFORM==UCMINI
    return 0;//TODO
  #else
    return 0;
  #endif
}

int checkUsb(void){
  #if PLATFORM==UCMINI049 || PLATFORM==UCMINI
    DDRB&=~(1<<7);
    if (PINB&(1<<7))
      return 0;
    else
      return 1;
  #else
    return 0;
  #endif
}


