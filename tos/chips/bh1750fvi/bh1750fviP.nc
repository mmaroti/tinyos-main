#include "bh1750fvi.h" 

module bh1750fviP {
  provides interface Read<uint16_t> as Light;
  provides interface SplitControl;
  
  uses interface I2CPacket<TI2CBasicAddr>;
  uses interface Timer<TMilli>;
}
implementation {
  enum {
    S_OFF = 0,
    S_STARTING,
    S_STOPPING,
    S_ON,
    S_BUSY,
  };
  
  uint8_t state = S_OFF;
  bool stopRequested = false;

  command error_t Splitcontrol.start() {
    if(state == S_STARTING) return EBUSY;
    if(state != S_OFF) return EALREADY;
      
    call Timer.startOneShot(11);
    
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
      stopRequested = true;
    }
    return SUCCESS;
  }  

  command error_t Light.read() {
    if(state == S_OFF) return EOFF;
    if(state != S_ON) return EBUSY;
 /*I2C komm*/
     state == S_BUSY;   
    I2CPacket.write(I2C_START | I2C_STOP, WRITE_ADDRESS, 1, ONE_SHOT_H_RES);
    //call Timer.startOneShot(TIMEOUT_H_RES);
    return SUCCESS;
  }

  event void Timer.fired() {
    if(state == S_STARTING) {
      state = S_ON;
      call I2CPacket.write(I2C_START | I2C_STOP, WRITE_ADDRESS, 1, POWER_ON);
     // signal SplitControl.startDone(SUCCESS);
    } else if(state == S_BUSY) {
        uint16_t result;
        uint8_t res[2];
        
        I2CPacket.read(I2C_START | I2C_STOP, READ_ADDRESS, 2, res);

        if(stopRequested) {
          state = S_ON;
          call SplitControl.stop();
        }
      //signal Light.readDone(error, result);
    }
  }

  event void I2CPacket.readDone(error_t error, uint16_t addr, uint8_t length, uint8_t* data) {
    signal Light.readDone(SUCCESS, data);
  }
  
  event void I2CPacket.writeDone(error_t error, uint16_t addr, uint8_t length, uint8_t* data) {
    if(state == S_ON) signal SplitControl.startDone(SUCCESS);
    if(state == S_BUSY) call Timer.startOneShot(TIMEOUT_H_RES);
  }

  default event void Light.readDone(error_t error, uint16_t val) { }
  default event void Splitcontrol.startDone(error_t error) { }
  default event void SplitControl.stopDone(error_t error) { }
}
