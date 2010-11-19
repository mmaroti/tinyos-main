#include "ms5607.h" 

module ms5607P  {
  provides interface Read<uint32_t> as Pressure;
  provides interface SplitControl;
  
  uses interface I2CPacket<TI2CBasicAddr>;
  uses interface Timer<TMilli>;
  uses interface Resource as I2CResource;

  uses interface DiagMsg;
  uses interface Leds;
}
implementation {
  enum {
    S_OFF = 0,
    S_STARTING,
    S_STOPPING,
    S_ON,
    S_BUSY,
    S_WRITE,
    S_READ,
  };

  uint8_t res[3];
  uint32_t mesres;
  uint8_t state = S_OFF;
  bool stopRequested = FALSE;

  command error_t SplitControl.start() {
    if(state == S_STARTING) return EBUSY;
    if(state != S_OFF) return EALREADY;
    
    call Timer.startOneShot(300);

    return SUCCESS;
  }
  
  task void signalStopDone() {
    signal SplitControl.stopDone(SUCCESS);
  }

  command error_t SplitControl.stop() {
    if(state == S_STOPPING) return EBUSY;
    if(state == S_OFF) return EALREADY;
    if(state == S_ON) {
      state = S_OFF;
      post signalStopDone();
    } else {
      stopRequested = TRUE;
    }
    return SUCCESS;
  }  

  command error_t Pressure.read() {
  if(call DiagMsg.record()){
	    call DiagMsg.str("ms.PRread");
	    //call DiagMsg.uint8(I2C_ADDRESS);
	    call DiagMsg.uint8(state);
	    call DiagMsg.send();
      }
    if(state == S_OFF) return EOFF;
    if(state != S_ON) return EBUSY;
/*i2c */
    state = S_BUSY;
    call I2CResource.request();
    return SUCCESS;
  }

  event void Timer.fired() {

    if(state == S_OFF) {
      state = S_ON;
      signal SplitControl.startDone(SUCCESS);
    } else if(state == S_BUSY) {
        uint16_t result;
        if(stopRequested) {
          state = S_ON;
          call SplitControl.stop();
        }
      signal Pressure.readDone(SUCCESS, 0);
    } else if (state == S_WRITE) {
        call I2CPacket.write(I2C_START | I2C_STOP, ADDRESS, 1, 0x00);
    }
    
  }

  task void signalReadDone() {
    signal Pressure.readDone(SUCCESS, mesres);
  }

  async event void I2CPacket.readDone(error_t error, uint16_t addr, uint8_t length, uint8_t* data) {
    call Leds.led2On();
    mesres = (uint32_t)data[0] << 16;
    mesres = (uint32_t)mesres | (data[1]<<8);
    mesres = (uint32_t)mesres | data[2];

    if(call DiagMsg.record()){
	    call DiagMsg.str("ms5607.readdone");
	    call DiagMsg.uint8(data[0]);
            call DiagMsg.uint8(data[1]);
            call DiagMsg.uint8(data[2]);
	    call DiagMsg.uint8(state);
	    call DiagMsg.send();
      }
    atomic {state = S_ON;
    call I2CResource.release();
    post signalReadDone();
    }
  }

  task void writeTask() {
    if(state == S_WRITE) call Timer.startOneShot(20);
    if(state == S_READ) {call I2CPacket.read(I2C_START | I2C_STOP, ADDRESS, 3, res); call Leds.led1On();}
  }  

  async event void I2CPacket.writeDone(error_t error, uint16_t addr, uint8_t length, uint8_t* data) {
    if(state == S_BUSY) {
      state = S_WRITE;
      post writeTask();
    } else if (state == S_WRITE) {
      state = S_READ;
      call Leds.led0On();
      post writeTask();
    }
    
  }

  event void I2CResource.granted() {
    if(state == S_BUSY) {
      uint8_t cmd=0x50;
      call I2CPacket.write(I2C_START | I2C_STOP, ADDRESS, 1, &cmd);
    }
  }

  default event void Pressure.readDone(error_t error, uint32_t val) { }
  default event void SplitControl.startDone(error_t error) { }
  default event void SplitControl.stopDone(error_t error) { }
}
