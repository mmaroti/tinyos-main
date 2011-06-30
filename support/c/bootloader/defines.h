/* definitions generated by preprocessor, copy into defines.h */

#ifndef	PPINC
//#define	_ATMEGA1281	// device select: _ATMEGAxxxx
#define	_B4096	// boot size select: _Bxxxx (words), powers of two only
#include	<avr/io.h>
#ifndef TIMEOUT
#define TIMEOUT 10 //in seconds (error: -1/128s..0)
#endif
#ifndef SERIAL_WAIT
#define SERIAL_WAIT 255 // must be less then 256. 1 = 1/128 s
#endif

//timeout in seconds=TIMEOUT*SERIAL_WAIT/128 (error: -1/128s..0)
#define TIMEOUT_CYCLES TIMEOUT*128/SERIAL_WAIT

#ifdef _ATMEGA1281
  #define F_CPU 8000000
#else
  #define F_CPU 16000000
#endif

/* definitions for UART control */
#ifdef _ATMEGA1281
#define BAUD_RATE_HIGH_REG	UBRR0H
#define	BAUD_RATE_LOW_REG	UBRR0L
#define	UART_CONTROL_REG	UCSR0B
#define	ENABLE_TRANSMITTER_BIT	TXEN0
#define	ENABLE_RECEIVER_BIT	RXEN0
#define	UART_STATUS_REG	UCSR0A
#define UART_DOUBLE_SPEED_BIT	U2X0
#define	TRANSMIT_COMPLETE_BIT	TXC0
#define	RECEIVE_COMPLETE_BIT	RXC0
#define	UART_DATA_REG	UDR0
#else
#define BAUD_RATE_HIGH_REG	UBRR1H
#define	BAUD_RATE_LOW_REG	UBRR1L
#define	UART_CONTROL_REG	UCSR1B
#define	ENABLE_TRANSMITTER_BIT	TXEN1
#define	ENABLE_RECEIVER_BIT	RXEN1
#define	UART_STATUS_REG	UCSR1A
#define UART_DOUBLE_SPEED_BIT	U2X1
#define	TRANSMIT_COMPLETE_BIT	TXC1
#define	RECEIVE_COMPLETE_BIT	RXC1
#define	UART_DATA_REG	UDR1

#endif

/* definitions for SPM control */
#define	SPMCR_REG	SPMCSR
#define	PAGESIZE	128
#define	APP_END		0xefff
#define	LARGE_MEMORY

/* definitions for device recognition */
#define	PARTCODE
#ifdef _ATMEGA1281	
#define	SIGNATURE_BYTE_1	0x1E
#define	SIGNATURE_BYTE_2	0x97
#define	SIGNATURE_BYTE_3	0x04
#else
#define SIGNATURE_BYTE_1	0x1E
#define SIGNATURE_BYTE_2	0xA7
#define SIGNATURE_BYTE_3	0x01
#endif

/* indicate that preprocessor result is included */
#define	PPINC
#endif

//LEDS

#if PLATFORM == IRIS
  #define INVERTPOWER
  #define LED0PORT PORTA
  #define LED0DDR DDRA
  #define LED0NUM 0
  #define LED1PORT PORTA
  #define LED1DDR DDRA
  #define LED1NUM 1
  #define LED2PORT PORTA
  #define LED2DDR DDRA
  #define LED2NUM 2
#elif PLATFORM==UCDUAL
  #define INVERTPOWER
  #define LED0PORT PORTD
  #define LED0DDR DDRD
  #define LED0NUM 7
  #define LED1PORT PORTD
  #define LED1DDR DDRD
  #define LED1NUM 6
  #define LED2PORT PORTE
  #define LED2DDR DDRE
  #define LED2NUM 2
  #define LED3PORT PORTE
  #define LED3DDR DDRE
  #define LED3NUM 3
#elif PLATFORM==UCMINI049
  #define LED0PORT PORTE
  #define LED0DDR DDRE
  #define LED0NUM 3
  #define LED1PORT PORTE
  #define LED1DDR DDRE
  #define LED1NUM 5
  #define LED2PORT PORTE
  #define LED2DDR DDRE
  #define LED2NUM 6
  #define LED3PORT PORTE
  #define LED3DDR DDRE
  #define LED3NUM 7
//other ucmini
#else
  #define LED0PORT PORTE
  #define LED0DDR DDRE
  #define LED0NUM 4
  #define LED1PORT PORTE
  #define LED1DDR DDRE
  #define LED1NUM 5
  #define LED2PORT PORTE
  #define LED2DDR DDRE
  #define LED2NUM 6
  #define LED3PORT PORTE
  #define LED3DDR DDRE
  #define LED3NUM 7
#endif

int BRREG_VALUE;
int blinker;
int isWriting;

void init(void);

int baudrateRegister(uint32_t baudrate);

void led0On(void);
void led1On(void);
void led2On(void);
void led3On(void);

void led0Off(void);
void led1Off(void);
void led2Off(void);
void led3Off(void);

void led0Toggle(void);
void led1Toggle(void);
void led2Toggle(void);
void led3Toggle(void);

void ledSet(uint8_t val);

void ledInit(void);

void status(uint16_t time_out);
