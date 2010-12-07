#define F_CPU 16000000UL

#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <avr/sleep.h>
#include <avr/power.h>

volatile uint8_t ison=0;
volatile uint16_t timer = 0;

void init(){
	DDRB = 0xff;
	PORTB= 0;
	DDRD = 0xff;
	PORTD= 0;
	DDRE = 0xff;
	PORTE= 0;
	DDRF = 0xff;
	PORTF= 0;
}

void spi_init() {
	DDRB |= 0x76;
	
	SPCR = (1<<SPE)|(1<<MSTR);
}

void SPI_MasterTransmit(char cData)

{

/* Start transmission */

SPDR = cData;

/* Wait for transmission complete */

while(!(SPSR & (1<<SPIF)));

}

/*ISR(TIMER2_OVF_vect) {
	ison = ~ison;
}*/

void next(void) {
	++timer;
	SCOCR1HH = (timer >>8);
	SCOCR1HL = timer;
	SCOCR1LH = 0;
	SCOCR1LL = 0;
}

ISR(SCNT_CMP1_vect) {
	DDRE |= _BV(PE6);
	PORTE |= _BV(PE6);
	next();
	ison = ~ison;
}

int main() {
	uint16_t i;
	
	DDRE |= _BV(PE3);
	//init();
	spi_init();
	
	MCUCR |= (1 << JTD);
	asm volatile("nop"::);
	MCUCR |= (1 << JTD);
	
	DDRF |= (_BV(PF2) | _BV(PF1));
	PORTF = 0;
	
	DDRE = 0xff;
	PORTE = 0;
	
	DDRD = 0xff;
	PORTD = 0;
	
	//DDRF |= _BV(PF0);
	PORTF |= _BV(PF0); //vbatt drigen high
	
	SPI_MasterTransmit(0xB9); //send deep-power down cmd
	PORTB |= _BV(PB4); //drivr ~cs high to initiate deep power-down
	
	DDRB |= (0x1F);  
	PORTB = 0x10; //all 0 except ~flash cs for detaching flash chip from spi bus
	
	DRTRAM0 = (1 << ENDRT);
	DRTRAM1 = (1 << ENDRT);
	DRTRAM2 = (1 << ENDRT);
	DRTRAM3 = (1 << ENDRT);
	
//	TRXPR = 1 << SLPTR;
	//PRR0 = (1 << PRTWI) | /* (1 << PRTIM2) |*/ (1 << PRTIM0) | (1 << PRPGA) | (1 << PRTIM1) | (1 << PRSPI) | (1 << PRUSART0) | (1 << PRADC);
	//PRR1 = (1 << PRTRX24) | (1 << PRTIM5) | (1 << PRTIM4) | (1 << PRTIM3) | (1 << PRUSART1);
	//PRR2 = (1 << PRRAM3) | (1 << PRRAM2) | (1<< PRRAM1) | (1 << PRRAM0);
	
	/*TCCR2B = (1 << CS20) | (1<< CS21) | (1<< CS22);
	TIMSK2 = 1 << TOIE2;*/
	ASSR   = 1 << AS2;
	SCIRQS   = (1 << IRQSCP1); //MANUALLY CLEAR symbol compare match irq flag
	SCIRQM   = (1 << IRQMCP1); //enabling symbol cnt compare match
	
	next();
	
	SCCR0 = (1 << SCEN) | (1 << SCCKSEL);
	
	DDRE |= _BV(PE5);
	for(i = 0; i < 10000; ++i)
		PORTE = _BV(PE5);
	
	sei();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	for(;;) {
		//TRXPR = 1 << SLPTR;
		sleep_enable();
		sleep_cpu();
		sleep_disable();
		//OCR2B = ison;
		
		if(!ison) {
			PORTE |= (1<<PE3);
		}
		else{
			PORTE &= ~(1<<PE3);
		}
		
	  //while(SCSR &(1<<SCBSY) !=0) {}
		//while ((ASSR & (1<<OCR2BUB)) != 0) {}	
	}
}
