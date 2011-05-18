#include "Bma180.h"
module TestSbSP
{
  provides interface Read<bma180_data_t>;
	uses interface Leds;
	//uses interface Boot;
	uses interface Timer<TMilli>;
  uses interface DiagMsg;
  uses interface SpiByte;
  uses interface FastSpiByte;
  uses interface SpiPacket;
  uses interface Resource;
  uses interface GeneralIO as CSN;
}

implementation
{
  uint8_t rxBuf[2] = {0,0};
  uint8_t txBuf[2] = {0x81, 0};
  bma180_data_t s_res;
  uint16_t x,y,z;
  uint8_t temp;
	/*void powerOn()
	{
		DDRF |= (1 << PF0);
		PORTF |= (1 << PF0);
	}*/

	/*void chipSelect()
	{
		DDRB |= (1 << PB6);
		PORTB &= ~(1 << PB6);
	}

	void chipDeselect()
	{
		DDRB |= (1 << PB6);
		PORTB |= (1 << PB6);
	}*/


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

  command error_t Read.read() {
    call CSN.set();
    call CSN.makeOutput();
    call Resource.request();
    return SUCCESS;
  }
  
	event void Timer.fired()
	{
		//uint8_t data;
		call Leds.led3Toggle();

	  //chipSelect();
    call CSN.clr();

		// read register 1
		
    call SpiByte.write(0x80 | 0x02);
    x = call SpiByte.write(0x00);//x
    x |= (call SpiByte.write(0) << 8);
    y = call SpiByte.write(0);//y
    y |= (call SpiByte.write(0) << 8);
    z = call SpiByte.write(0);//z
    z |= (call SpiByte.write(0) << 8);
    temp = call SpiByte.write(0);
    //call SpiPacket.send(txBuf, rxBuf, 2);

    call CSN.set();
		//chipDeselect();
    s_res.bma180_accel_x = x;
    s_res.bma180_accel_y = y;
    s_res.bma180_accel_z = z;
    s_res.bma180_temperature = temp;
    call Resource.release();
    signal Read.readDone(SUCCESS, s_res);
		setLeds(/*rxBuf[1]*/x);
	}

  async event void SpiPacket.sendDone(uint8_t* txcBuf, uint8_t* rxcBuf, uint16_t len, error_t error) {} 

  event void Resource.granted() {
    call Timer.startOneShot(576);
  }

	/*event void Boot.booted()
	{
    //powerOn();
    //call CSN.set();
    //call CSN.makeOutput();
    //call Resource.request();
	}*/
}
