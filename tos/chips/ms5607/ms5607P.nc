#include "ms5607.h" 

module ms5607P  {
  provides interface Read<uint32_t> as Pressure;
  provides interface SplitControl;
  
  uses interface I2CPacket<TI2CBasicAddr>;
  uses interface Timer<TMilli>;
  uses interface Resource as I2CResource;
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
    if(state == S_OFF) return EOFF;
    if(state != S_ON) return EBUSY;
/*i2c */
    state = S_BUSY;
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
    }
  }

  async event void I2CPacket.readDone(error_t error, uint16_t addr, uint8_t length, uint8_t* data) {
  }

  async event void I2CPacket.writeDone(error_t error, uint16_t addr, uint8_t length, uint8_t* data) {
  }

  event void I2CResource.granted() {
  }

  default event void Pressure.readDone(error_t error, uint32_t val) { }
  default event void SplitControl.startDone(error_t error) { }
  default event void SplitControl.stopDone(error_t error) { }
}
