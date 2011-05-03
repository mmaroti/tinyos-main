
module TestP
{
	uses interface Leds;
	uses interface Boot;
	uses interface Timer<TMilli>;
}

implementation
{
	void powerOn()
	{
		DDRE |= (1 << PE3);
		PORTE |= (1 << PE3);
	}

	void usartInit()
	{
		UBRR0 = 0;
		DDRE |= (1 << PE2);
		UCSR0C = (1 << UMSEL01) | (1 << UMSEL00) | (0 << UCPHA0) | (0 << UCPOL0);
		UCSR0B = (1 << RXEN0) | (1 << TXEN0);
		UBRR0 = 100;	// baudrate
	}

	void chipSelect()
	{
		DDRD |= (1 << PD7);
		PORTD &= ~(1 << PD7);
	}

	void chipDeselect()
	{
		DDRD |= (1 << PD7);
		PORTD |= (1 << PD7);
	}

	uint8_t writeData(uint8_t data)
	{
		while ( !( UCSR0A & (1<<UDRE0)) )
			;

		UDR0 = data;

		while ( !(UCSR0A & (1<<RXC0)) )
			;

		return UDR0;
	}

	void setLeds(uint8_t data)
	{
		if( (data & 0x01) != 0 )
			call Leds.led0On();
		else
			call Leds.led0Off();

		if( (data & 0x02) != 0 )
			call Leds.led1On();
		else
			call Leds.led1Off();

		if( (data & 0x04) != 0 )
			call Leds.led2On();
		else
			call Leds.led2Off();
	}

	event void Timer.fired()
	{
		uint8_t data;
		call Leds.led3Toggle();

		powerOn();
		usartInit();

		chipSelect();

		// read register 1
		writeData(0x80 | 0x00);
		data = writeData(0x00);

		chipDeselect();

		setLeds(data);
	}

	event void Boot.booted()
	{
		call Timer.startPeriodic(1024);
	}
}
