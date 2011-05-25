#include "Bma180.h"
module TestSbSP
{
  provides interface Read<bma180_data_t>;
  provides interface SplitControl as BmaControl;
	uses interface Leds;
	//uses interface Boot;
	uses interface Timer<TMilli>;
  uses interface DiagMsg;
  uses interface SpiByte;
  uses interface FastSpiByte;
  uses interface SpiPacket;
  uses interface Resource;
  uses interface GeneralIO as CSN;
  uses interface GeneralIO as PWR;
}

implementation
{
  enum{
    S_OFF = 0,
    S_STARTING,
    S_CONFIG,
    S_IDLE,
  };

  bma180_data_t s_res;
  uint16_t x,y,z;
  uint8_t temp, state=S_OFF;
	
  command error_t BmaControl.start() {
    if(state == S_STARTING) return EBUSY;
    if(state != S_OFF) return EALREADY;

    state = S_STARTING;
    call PWR.makeOutput();
    call PWR.set();
    if(call DiagMsg.record()) {
        call DiagMsg.str("start");
        call DiagMsg.uint8(state);
        call DiagMsg.send();
      }
     state = S_CONFIG;
    signal BmaControl.startDone(SUCCESS);
    return SUCCESS;
  }

  command error_t BmaControl.stop() {
    call PWR.makeOutput();
    call PWR.clr();
    return SUCCESS;
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

  command error_t Read.read() {
    call CSN.set();
    call CSN.makeOutput();
    call Resource.request();
    return SUCCESS;
  }
  
	event void Timer.fired()
	{
    if(state==S_CONFIG){
      call CSN.clr();
      call SpiByte.write(0x80 | 0xD);
      temp = call SpiByte.write(0);  // ctrl_reg0
      call CSN.set();
      temp |= 0x10;                  // enable ee_write
      call CSN.clr();
      call SpiByte.write(0x7F | 0xD);
      call SpiByte.write(temp);      // ctrl_reg0 altered
      call CSN.set();

      call CSN.clr();
      call SpiByte.write(0x80 | 0x35); //offset_lsb1
      temp = call SpiByte.write(0);
      call CSN.set();
      temp &= 0xF1;                  // clear range bits
      temp |= (BMA_RANGE<<1);
      call CSN.clr();
      call SpiByte.write(0x7F | 0x35);
      call SpiByte.write(temp);
      call CSN.set();

      call CSN.clr();
      call SpiByte.write(0x80 | 0x30); //tco_z
      temp = call SpiByte.write(0);
      call CSN.set();
      temp &= 0xFC;                 // clear mode bits
      temp |= BMA_MODE;
      call CSN.clr();
      call SpiByte.write(0x7F | 0x30);
      call SpiByte.write(temp);
      call CSN.set();

      call CSN.clr();
      call SpiByte.write(0x80 | 0x20); // bw_tcs
      temp = call SpiByte.write(0);
      call CSN.set();
      temp &= 0x0F;
      temp |= (BMA_BW<<4);
      call CSN.clr();
      call SpiByte.write(0x7F & 0x20);
      call SpiByte.write(temp);
      call CSN.set(); 

      state = S_IDLE;
      if(call DiagMsg.record()) {
        call DiagMsg.str("CONFIG");
        call DiagMsg.send();
      }
    }

		//uint8_t data;
		call Leds.led3Toggle();

	  //chipSelect();
    call CSN.clr();

		// read register 1
		
    call SpiByte.write(0x80 | 0x00);
    s_res.chip_id = call SpiByte.write(0);
    call SpiByte.write(0);
    x = call SpiByte.write(0x00);//x
    x |= (call SpiByte.write(0) << 8);
    y = call SpiByte.write(0);//y
    y |= (call SpiByte.write(0) << 8);
    z = call SpiByte.write(0);//z
    z |= (call SpiByte.write(0) << 8);
    s_res.bma180_temperature = call SpiByte.write(0);

    call CSN.set();
		//chipDeselect();
    s_res.bma180_accel_x = x;
    s_res.bma180_accel_y = y;
    s_res.bma180_accel_z = z;
    //s_res.bma180_temperature = 0;//temp;
    call Resource.release();
    signal Read.readDone(SUCCESS, s_res);
		setLeds(x);
	}

  async event void SpiPacket.sendDone(uint8_t* txcBuf, uint8_t* rxcBuf, uint16_t len, error_t error) {} 

  event void Resource.granted() {
    call Timer.startOneShot(BMA_SAMPLING_TIME_MS);
  }

}
