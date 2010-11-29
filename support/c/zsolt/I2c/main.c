#define F_CPU 8000000UL
//#define F_CPU 16000000UL

#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/*enum {
		IDLE = 0,
		STARTCOND,
		STOPCOND,
		WRITEBYTE,
		R_ACK,
		R_NACK,
		TWCR_def= (1 << TWEN) | (1 << TWINT) | (1 << TWEA),
	}	;
	
	
	uint8_t state = IDLE;*/
	uint8_t ret[2]={0,0};
	// number of CPU cycles per 1/1024 sec
uint16_t cycles;
int BRREG_VALUE;

	
	void led0On();void led0Off();
	void led1On();void led1Off();
	void led2On();void led2Off();
	void led3On();void led3Off();
	void ledOff();
	
	void ledSet(uint8_t);
	void initbootuart(void);
	
  
  void led0On() {
		PORTE |= _BV(PE5);
	}
	
	void led1On() {
		PORTE |= _BV(PE6);
	}
	
	void led2On() {
		PORTE |= _BV(PE7);
	}
	
	void led3On() {
		PORTE |= _BV(PE3);
	}
	
	void led0Off() {
		PORTE &= ~_BV(PE5);
	}
	
	void led1Off() {
		PORTE &= ~_BV(PE6);
	}
	
	void led2Off() {
		PORTE &= ~_BV(PE7);
	}
	
	void led3Off() {
		PORTE &= ~_BV(PE3);
	}
	
	void ledOff() {
		led0Off(); led1Off(); led2Off(); led3Off();
	}
	
	void ledSet(uint8_t val) {
		ledOff();
		if(val & 0x1) {
			led0On();
		}
		if(val & 0x2) {
			led1On();
		}
		if(val & 0x4) {
			led2On();
		}
		if(val & 0x8) {
			led3On();
		}
	}
  
  void Init() {
		DDRF |= _BV(PF2);
		PORTF |= _BV(PF2);
		
		PORTD = _BV(PD0) | _BV(PD1);
		
		DDRE = _BV(PE7) | _BV(PE6) | _BV(PE5) | _BV(PE3);
		ledOff();
		_delay_ms(1000);
	}
	

void TWI_init(char bitrate, char prescaler);
char TWI_action(char command);
char TWI_start(void);
void TWI_stop(void);
char TWI_write_data(char data);
char TWI_read_data(char put_ack);
void TWI_wait(char slave);
char EE_write_byte(const uint8_t address, const char data);
char EE_read_byte(const uint8_t address, uint8_t len, uint8_t* res);

void TWI_init(char bitrate, char prescaler)
//sets bitrate and prescaler
{
	TWBR = bitrate;
	//mask off the high prescaler bits (we only need the lower three bits)
	TWSR = prescaler & 0x03;
}

char TWI_action(char command)
//starts any TWI operation (not stop), waits for completion and returns the status code
//TWINT and TWEN are set by this function, so for a simple data transfer just use TWI_action(0);
{	//make sure command is good
	TWCR = (command|(1<<TWINT)|(1<<TWEN));
	//wait for TWINT to be set after operation has been completed
	while(!(TWCR & (1<<TWINT)));
	//return status code with prescaler bits masked to zero
	return (TWSR & 0xF8);
}

char TWI_start(void)
//uses TWI_action to generate a start condition, returns status code
{	//TWI_action writes the following command to TWCR: (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)
	return TWI_action(1<<TWSTA);
	//return values should be 0x08 (start) or 0x10 (repeated start)
}

/******************************************************************************/
void TWI_stop(void)
//generates stop condition
{	//as TWINT is not set after a stop condition, we can't use TWI_action here!
	TWCR = ((1<<TWINT)|(1<<TWSTO)|(1<<TWEN));
	//status code returned is 0xF8 (no specific operation)
}

char TWI_write_data(char data)
//loads data into TWDR and transfers it. Works for slave addresses and normal data
//waits for completion and returns the status code.
{	//just write data to TWDR and transmit it
	TWDR = data;
	//we don't need any special bits in TWCR, just TWINT and TWEN. These are set by TWI_action.
	return TWI_action(0);
	//status code returned should be:
	//0x18 (slave ACKed address)
	//0x20 (no ACK after address)
	//0x28 (data ACKed by slave)
	//0x30 (no ACK after data transfer)
	//0x38 (lost arbitration)
}

char TWI_read_data(char put_ack)
{	//if an ACK is to returned to the transmitting device, set the TWEA bit
	if(put_ack)
		return(TWI_action(1<<TWEA));
	//if no ACK (a NACK) has to be returned, just receive the data
	else
		return(TWI_action(0));
	//status codes returned:
	//0x38 (lost arbitration)
	//0x40 (slave ACKed address)
	//0x48 (no ACK after slave address)
	//0x50 (AVR ACKed data)
	//0x58 (no ACK after data transfer)
}

void TWI_wait(char slave)
{
	
	do {
		TWI_start();
	} while(TWI_write_data(slave) != 0x18);
	TWI_stop();
}

char EE_write_byte(const uint8_t address, const char data)
{
	char dummy;
	//we need this for the first if()
	dummy = TWI_start();
	//if the start was successful, continue, otherwise return 1
	if((dummy != 0x08) && (dummy != 0x10))
		return TWSR;
	
	if(TWI_write_data(address) != 0x18)
		return TWSR;
	//now send the word address byte
	//if(TWI_write_data((char)(address)) != 0x28)
	//	return TWSR;
	//now send the data byte
	if(TWI_write_data(data) != 0x28)
		return TWSR;
	//if everything was OK, return zero.
	return 0;
}

char EE_read_byte(const uint8_t address, uint8_t len, uint8_t* res)
{
	char dummy;
	uint8_t i;
	//we need this for the first if()
	dummy = TWI_start();
	//as in EE_write_byte, first send the page address and the word address
	if((dummy != 0x08) && (dummy != 0x10))
		return TWSR;
	if(TWI_write_data(address) != 0x40)
		return TWSR;
	
	for(i=0; i<len; i++) {
		if(i < len-1) {
			if(TWI_read_data(1) != 0x50)
				return TWSR;
			res[i] = TWDR;
		}
		else {
			if(TWI_read_data(0) != 0x58)
				return TWSR;
			res[i] = TWDR;
		}
	}
	//if everything was OK, return zero
	return 0;
}

void flash() {
	ledOff();
	 _delay_ms(300);
	 led0On();
	 _delay_ms(500);
	 ledOff();
	 _delay_ms(300);
}

//-----defines



/* Measure clock cycles per Jiffy (1/32768s) */
void Serial_init() {
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

		while( TCNT2 != 32 )	// wait 32/32768 = 1/1024 sec
			;

		cycles = TCNT1;
	}

	cycles -= now;

	/* Reset to boot state */
	TCCR1B = 0;
	ASSR = 0;
	TCCR2B = 0;
}

int baudrateRegister(uint32_t baudrate) {
	Serial_init();

	// we use the fast setting: (cycles * 1024) / (8 * baudrate) - 1
	return (((uint32_t)cycles) << 7) / ((uint32_t)baudrate) - 1;
}

//---defines
void initbootuart(void)
{
  BRREG_VALUE = baudrateRegister(57600);

  UBRR1L = BRREG_VALUE;
  UBRR1H= (BRREG_VALUE << 8);
  UCSR1A = (1<<U2X1);
  UCSR1B = (1 << RXEN1) |
                     (1 << TXEN1); // enable receive and transmit
  
  UCSR1C = (0 << USBS1) | (3 << UCSZ10); // setting uart frame format 8 data bits; 1 stop bit
  
}


void sendchar(unsigned char c)
{
  UDR1 = c;                                   // prepare transmission
  while (!(UCSR1A & (1 << TXC1)));// wait until byte sendt
  UCSR1A |= (1 << TXC1);          // delete TXCflag
}

int main() {
	uint8_t eredm[3]= {0,0,0};

   Init();
	 initbootuart();
	 
	 //Sht21
	 
	 /*
	TWI_init(32, 0);
	ret[0] = EE_write_byte(128, 0xf3);
	//measurement delay
	 _delay_ms(85);
	ret[1] = EE_read_byte(129, 2, eredm);

	TWI_stop();
	*/
	 
	 //bh1750fvi
	 
	 /*
	 TWI_init(32, 0);
	 ret[0] = EE_write_byte(70, 0x1);
	 TWI_stop();
	 
	 ret[0] = EE_write_byte(70, 0x20);
	 TWI_stop();
	 
	 _delay_ms(200);
	 //TWI_wait(70);
	 ret[0] = EE_write_byte(70, 0x1);
	 TWI_stop();
	 
	 ret[1] = EE_read_byte(71, 2, eredm);
	 TWI_stop();
	 */
	 
	 //ms5607
	 /*TWI_init(32,0);
	 ret[0] = EE_write_byte(238, 0x40);
	 TWI_stop();
	 _delay_ms(100);
	 //TWI_wait(238);
	 ret[1] = EE_read_byte(239, 3, eredm);
	 TWI_stop();
	 */
	 
	 /*
	 
	 I2CWrite(128, 0xf5, 0);
	 _delay_ms(100);
	 I2CRead(129, 2);*/
	 //led0On();
	 //ledSet(3);
	 flash();flash();
	 _delay_ms(1000);
	 //ledSet(10);
	 //led2On();
	 ledSet(eredm[0]>>4);
	 _delay_ms(2000);
	 
	 /*ledOff();
	 _delay_ms(300);
	 led0On();
	 _delay_ms(300);*/
	 flash();
	 
	 
	 ledSet(eredm[0]);
	 _delay_ms(2000);
	 
	 flash();
	 
	 ledSet(eredm[1]>>4);
	 _delay_ms(2000);
	 
	 flash();
	 
	 ledSet(eredm[1]);
	 _delay_ms(4000);
	 
	 
	 flash();
	 
	 ledSet(eredm[2]>>4);
	 _delay_ms(2000);
	 
	 flash();
	 
	 ledSet(eredm[2]);
	 _delay_ms(5000);
	 
	 ledOff();
	 flash();flash();flash();
	 
	 ledSet(ret[1]>>4);
	 _delay_ms(2000);
	 
	 flash();
	 
	 ledSet(ret[1]);
	 _delay_ms(2000);
	 ledOff();
	 
	 while(1) {
	 TWI_init(32, 0);
	ret[0] = EE_write_byte(128, 0xf3);
	//measurement delay
	 _delay_ms(85);
	ret[1] = EE_read_byte(129, 2, eredm);

	TWI_stop();
	 
	 sendchar(eredm[0]);
	 _delay_ms(50);
	 sendchar(eredm[1]);
	 _delay_ms(50);
	 sendchar(eredm[2]);
	 _delay_ms(50);
	 
	 sendchar(ret[0]);
	 sendchar(ret[1]);
	 }
}
