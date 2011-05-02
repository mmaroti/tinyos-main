#include "Bh1750fvi.h" 

module Bh1750fviP {
  provides interface Read<uint16_t> as Light;
  provides interface SplitControl;
  
  uses interface I2CPacket<TI2CBasicAddr>;
  uses interface Timer<TMilli>;
  uses interface Resource as I2CResource;

  uses interface DiagMsg;
  uses interface Leds;
}
implementation {
  uint16_t mesrslt=0;
  uint8_t  res[2];
  uint8_t cmd;
  enum {
    S_OFF = 0,
    S_STARTING,
    S_STOPPING,
    S_IDLE,
    S_BUSY,
    S_RESET,
  };
  
  norace uint8_t state = S_OFF;
  bool on=0;
  bool stopRequested = FALSE;

  command error_t SplitControl.start() {
    if(state == S_STARTING) return EBUSY;
    if(state != S_OFF) return EALREADY;
      
    call Timer.startOneShot(11);
    
    return SUCCESS;
  }

  task void signalStartDone() {
    signal SplitControl.startDone(SUCCESS);
  }

  task void signalStopDone() {
    signal SplitControl.stopDone(SUCCESS);
  }
  
  command error_t SplitControl.stop() {
    if(state == S_STOPPING) return EBUSY;
    if(state == S_OFF) return EALREADY;
    if(state == S_IDLE) {
      atomic state = S_OFF;
      post signalStopDone();
    } else {
      stopRequested = TRUE;
    }
    return SUCCESS;
  }  

  command error_t Light.read() {
    if(state == S_OFF) return EOFF;
    if(state != S_IDLE) return EBUSY;

    atomic state = S_RESET;//S_BUSY;   
    call I2CResource.request();
    return SUCCESS;
  }

  event void Timer.fired() {
    if(state == S_OFF) {
      atomic state = S_STARTING;
      call I2CResource.request();
    } else if(state == S_BUSY) {
       error_t err;
        call Leds.led0On();
        err=call I2CPacket.read(I2C_START | I2C_STOP, READ_ADDRESS, 2, res);

        if(stopRequested) {
          atomic state = S_IDLE;
          call SplitControl.stop();
        }
    }/* else if(state == S_STARTING) {
        atomic state = S_IDLE;
        call I2CResource.release();
        signal SplitControl.startDone(SUCCESS); 
    }*/ else if(state == S_IDLE) {
         call I2CResource.release();
    }
  }
 
  task void signalReadDone() {
    atomic {state= S_IDLE;
    signal Light.readDone(SUCCESS, mesrslt);}
  }

  async event void I2CPacket.readDone(error_t error, uint16_t addr, uint8_t length, uint8_t* data) {
    mesrslt = data[0]<<8;
    mesrslt |= data[1];
    call Leds.led1On();
    call I2CResource.release();
   
    post signalReadDone();
  }
  
  task void startTimeout() {
    if(state == S_IDLE) call Timer.startOneShot(TIMEOUT_H_RES);
    //else if(state == S_STARTING) call Timer.startOneShot(11);
    else if(state == S_BUSY) call Timer.startOneShot(TIMEOUT_H_RES);
  }

  async event void I2CPacket.writeDone(error_t error, uint16_t addr, uint8_t length, uint8_t* data) {
    if (state==S_IDLE)call Leds.led2On();
    if ((state == S_STARTING) && (error == SUCCESS)) {
      state = S_IDLE;
      //post signalStartDone();
      signal SplitControl.startDone(SUCCESS);
      call I2CResource.release();
    }
    if (state == S_RESET) {
      state = S_BUSY;
      call I2CResource.release();
      return (void)call I2CResource.request();
    }
    post startTimeout();
  }

  event void I2CResource.granted() {
    if(state == S_STARTING || state == S_RESET) {
      error_t err;
      cmd=POWER_ON;
      err=call I2CPacket.write(I2C_START | I2C_STOP, WRITE_ADDRESS, 1, &cmd);
    } else if(state == S_BUSY) {
      cmd=ONE_SHOT_H_RES;
      call I2CPacket.write(I2C_START | I2C_STOP, WRITE_ADDRESS, 1, &cmd);
    }
  }

  default event void Light.readDone(error_t error, uint16_t val) { }
  default event void SplitControl.startDone(error_t error) { }
  default event void SplitControl.stopDone(error_t error) { }
}
