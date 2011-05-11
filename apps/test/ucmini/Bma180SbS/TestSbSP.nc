
module TestSbSP
{
	uses interface Leds;
	uses interface Boot;
	uses interface Timer<TMilli>;
  uses interface DiagMsg;
  uses interface SpiByte;
  uses interface Resource;
}

implementation
{
	/*void powerOn()
	{
		DDRF |= (1 << PF0);
		PORTF |= (1 << PF0);
	}*/

	void chipSelect()
	{
		DDRB |= (1 << PB6);
		PORTB &= ~(1 << PB6);
	}

	void chipDeselect()
	{
		DDRB |= (1 << PB6);
		PORTB |= (1 << PB6);
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

	  chipSelect();

		// read register 1
		
    call SpiByte.write(0x80 | 0x00);
    data = call SpiByte.write(0x00);

		chipDeselect();

		setLeds(data);
	}

  event void Resource.granted() {
    call Timer.startPeriodic(1024);
  }

	event void Boot.booted()
	{
    //powerOn();
    call Resource.request();
	}
}
