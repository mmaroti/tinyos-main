#include "bh1750fvi.h" 

module bh1750fviP {
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
  enum {
    S_OFF = 0,
    S_STARTING,
    S_STOPPING,
    S_ON,
    S_BUSY,
  };
  
  uint8_t state = S_OFF;
  bool on=0;
  bool stopRequested = FALSE;

  command error_t SplitControl.start() {
    if(state == S_STARTING) return EBUSY;
    if(state != S_OFF) return EALREADY;
      
    call Timer.startOneShot(11);
    if(call DiagMsg.record()){
	    call DiagMsg.str("P.splitsta");
            call DiagMsg.uint8(state);
	    //call DiagMsg.uint8(err);
	    call DiagMsg.send();
      }
    return SUCCESS;
  }

  task void signalStopDone() {
    signal SplitControl.stopDone(SUCCESS);
  }
  
  command error_t SplitControl.stop() {
    if(state == S_STOPPING) return EBUSY;
    if(state == S_OFF) return EALREADY;
    if(state == S_ON) {
      atomic state = S_OFF;
      post signalStopDone();
    } else {
      stopRequested = TRUE;
    }
    return SUCCESS;
  }  

  command error_t Light.read() {
  //if (!on) atomic state = S_ON;
    //on++; //for testing purposes
  if(call DiagMsg.record()){
	    call DiagMsg.str("P.read");
            call DiagMsg.uint8(state);
	    //call DiagMsg.uint8(err);
	    call DiagMsg.send();
      }
    if(state == S_OFF) return EOFF;
    if(state != S_ON) return EBUSY;
 /*I2C komm*/
    atomic state = S_BUSY;   
    //call I2CPacket.write(I2C_START | I2C_STOP, WRITE_ADDRESS, 1, ONE_SHOT_H_RES);
    //call Timer.startOneShot(TIMEOUT_H_RES);
    call I2CResource.request();
    return SUCCESS;
  }

  event void Timer.fired() {
    if(state == S_OFF) {
      atomic state = S_STARTING;
      call I2CResource.request();
      //call I2CPacket.write(I2C_START | I2C_STOP, WRITE_ADDRESS, 1, POWER_ON);
     // signal SplitControl.startDone(SUCCESS);
    } else if(state == S_BUSY) {
       error_t err;
        call Leds.led0On();
        err=call I2CPacket.read(I2C_START | I2C_STOP, READ_ADDRESS, 2, res);
        if(call DiagMsg.record()){
	    call DiagMsg.str("P.firBUS");
            call DiagMsg.uint8(state);
	    call DiagMsg.uint8(err);
	    call DiagMsg.send();
      }

        if(stopRequested) {
          atomic state = S_ON;
          call SplitControl.stop();
        }
      //signal Light.readDone(error, result);
    } else if(state == S_STARTING) {
        uint8_t cmd=0x10;
        error_t err;
      atomic state= S_ON;
        err=call I2CPacket.write(I2C_START | I2C_STOP, WRITE_ADDRESS, 1, &cmd);
      //call I2CResource.release();
      if(call DiagMsg.record()){
	    call DiagMsg.str("P.firSTA");
            call DiagMsg.uint8(state);
	    call DiagMsg.uint8(err);
	    call DiagMsg.send();
      }
      signal SplitControl.startDone(SUCCESS);
    } else if(state == S_ON) {
         call I2CResource.release();
    }
  }
 
  task void signalReadDone() {
    atomic {state= S_ON;
    signal Light.readDone(SUCCESS, mesrslt);}
  }

  async event void I2CPacket.readDone(error_t error, uint16_t addr, uint8_t length, uint8_t* data) {
    mesrslt = data[0]<<8;
    mesrslt |= data[1];
    call Leds.led1On();
    if(call DiagMsg.record()){
	    call DiagMsg.str("P.readDone");
            call DiagMsg.uint8(state);
            call DiagMsg.uint8(data[0]);
            call DiagMsg.uint8(data[1]);
	    //call DiagMsg.uint8(err);
	    call DiagMsg.send();
      }
    call I2CResource.release();
   
    post signalReadDone();
    //signal Light.readDone(SUCCESS, data);
  }
  
  task void startTimeout() {
    if(state == S_ON) call Timer.startOneShot(TIMEOUT_H_RES);//signal SplitControl.startDone(SUCCESS);
    else if(state == S_STARTING) call Timer.startOneShot(11);
    //if(state == S_BUSY) call Timer.startOneShot(TIMEOUT_H_RES);
  }

  async event void I2CPacket.writeDone(error_t error, uint16_t addr, uint8_t length, uint8_t* data) {
    if (state==S_ON)call Leds.led2On();
    post startTimeout();
  }

  event void I2CResource.granted() {
    if(state == S_STARTING) {
      error_t err;
      uint8_t cmd=0x01;
      err=call I2CPacket.write(I2C_START | I2C_STOP, WRITE_ADDRESS, 1, &cmd);
      if(call DiagMsg.record()){
	    call DiagMsg.str("P.grantPWRON");
            call DiagMsg.uint8(state);
	    call DiagMsg.uint8(err);
	    call DiagMsg.send();
      }
    } else if(state == S_BUSY) {
 error_t err;
      //uint8_t cmd=0x10;
      //call I2CPacket.write(I2C_START | I2C_STOP, WRITE_ADDRESS, 1, &cmd);
      //call I2CPacket.read(I2C_START | I2C_STOP, WRITE_ADDRESS, 2, res);
      
call Timer.startOneShot(TIMEOUT_H_RES);
/*err=call I2CPacket.read(I2C_START | I2C_STOP, READ_ADDRESS, 2, res);
      if(call DiagMsg.record()){
	    call DiagMsg.str("P.granted");
            call DiagMsg.uint8(state);
            call DiagMsg.uint8(err);
	    call DiagMsg.send();
      }*/
    }
  }

  default event void Light.readDone(error_t error, uint16_t val) { }
  default event void SplitControl.startDone(error_t error) { }
  default event void SplitControl.stopDone(error_t error) { }
}
