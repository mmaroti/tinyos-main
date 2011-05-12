
#define F_CPU 16000000UL // rendszer orajel: 16 MHz
#define USART_BAUDRATE 9600  // soros kommunikacio sebessege: 9600 bps
#define UBRR_ERTEK ((F_CPU / (USART_BAUDRATE * 16UL)) - 1) // UBRR
       
#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
 
volatile int Z = 0;
volatile int Y = 0;
volatile int X = 0;
 
void Konfig10bitADC(uint8_t channel, uint8_t ref, uint8_t prescaler)        // ADC konfiguralas (beallitas)
{
	ADMUX |= (channel&0x0f);
    if(channel&0x10)
      ADCSRB |= (1 << MUX5);
	ADMUX |= ref<<6; 
	ADCSRC = 0;
	ADCSRA = (1<<ADEN) | (prescaler&0x07); // ADC engedelyezese, ADC eloosztas = 64
}
 
unsigned int Beolvas10bitADC()
{
	ADCSRA |= (1<<ADSC);    // elso ADC konverzio elinditasa
	while (ADCSRA & (1<<ADSC));        // varas az atalakitasra
	return (ADCL | (ADCH<<8));        // ADC ertek kiolvasasa
}
 
void KonfigUART() // UART beallitasa
{
	// 9600 bps soros kommunikacio sebesseg beallitasa
	UBRR1L = UBRR_ERTEK;    // UBRR_ERTEK also 8 bitjenek betoltese az UBRRL regiszterbe
	UBRR1H = (UBRR_ERTEK>>8);   // UBRR_ERTEK felso 8 bitjenek betoltese az UBRRH regiszterbe
	// Aszinkron mod, 8 Adat Bit, Nincs Paritas Bit, 1 Stop Bit
	UCSR1C |=  (1 << UCSZ10) | (1 << UCSZ11);
	//Ado es Vevo aramkorok bekapcsolasa + az RX interrupt engedelyezese
	UCSR1B |= (1 << RXEN1) | (1 << TXEN1);   //
}
 
char UARTAdatFogad() // Ez a fuggveny a beerkezo adatokat kiolvassa az UDR regiszter bejovo pufferebol
{
	while(!(UCSR1A & (1<<RXC1)))  // Varakozas amig nincs uj bejovo adat
	{
	//Varakozas
	}
	//Most mar van beerkezett adat, amit kiolvasunk a pufferbol
	return UDR1;
}
 
 
void UARTAdatKuld(char data) // Ez a fuggveny a kuldendo adatot beirja az UDR regiszter kimeno pufferjebe
{
	while(!(UCSR1A & (1<<UDRE1)))  // Varakozas amig az Ado kesz nem lesz az adatkuldesre
	{
	//Varakozas
	}
	// Az Ado mar kesz az adatkuldesre, a kuldendo adatot a kimeno pufferjebe irjuk
	UDR1=data;
}
 
int main(void)  // Foprogram
{
	//char data;   
	KonfigUART();   // UART Konfiguralasa
	Konfig10bitADC(29,3,6);    // ADC beallitas lefuttatasa
 
	while(1) {
		X = Beolvas10bitADC();

		UARTAdatKuld('0'+(X/100) % 10);  // Szazasok
		UARTAdatKuld('0'+(X/10) % 10);   // Tizesek
		UARTAdatKuld('0'+X % 10);
		UARTAdatKuld('\n');
		  
		_delay_ms(1000);
	}
}
