//#include "util/delay.h"
#include "defines.h"
#define MATCHES 100
#define MAXERROR 1
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

/**
 * channel: see datasheet page 429
 * ref: 0:AREF, 1: AVDD (1.8V), 2: internal 1.5V (no ext. capacitor), 3: internal 1.6V (no ext. capacitor)
 * prescaler: 0-1: 2; 2: 4; 3: 8; 4: 16; 5: 32; 6: 64; 7: 128;
 */
uint16_t ReadADC(uint8_t channel, uint8_t ref, uint8_t prescaler)
{
  uint16_t readValue;
  DDRF|=1;
  PORTF&=~(1);
  //setting up the adc
  ADMUX |= (channel&0x0f);
  if(channel&0x10)
    ADCSRB |= (1 << MUX5);
  ADMUX |= ref<<6; 
  ADCSRC = 0;
  //turning on
  ADCSRA = (1<<ADEN) | (prescaler&0x07); 
  //conversion
  ADCSRA |= (1<<ADSC);
  while (ADCSRA & (1<<ADSC));
  readValue = (ADCL | (ADCH<<8));
  //turning off
  ADCSRA &= ~(1<<ADEN);
  return readValue;
}

//these functions returns 0, if the bootloader should be started

//0:ok 1:charge needed 2: charge recommended
int checkBattery(void){
  #if PLATFORM==UCMINI
    uint16_t battery=ReadADC(2,2,7);
    if(battery<140)
      return 0;//~0.2->battery switch is in no charge mode or no battery
    else if(battery<342)
      return 1;//less than 3V, battery is dead
    else if(battery<410)
      return 2;//less then 3.6V, recommend charging
    else
      return 0;
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


