#include "SensirionSht21.h"

module Sht21P {
  provides interface Read<uint16_t> as Temperature;
  provides interface Read<uint16_t> as Humidity;
  
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
    S_READ_TEMP,
    S_READ_HUMIDITY,
  };

  uint8_t state = S_OFF;
  bool stopRequested = FALSE;
  bool otherSensorRequested=FALSE;    
  command error_t SplitControl.start() {
    if(state == S_STARTING) return EBUSY;
    if(state != S_OFF) return EALREADY;

    call Timer.startOneShot(15);
    return SUCCESS;
  }

  task void signalStopDone(){
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

  command error_t Temperature.read() {
    if(state==S_OFF) return EOFF;
    if(state==S_READ_HUMIDITY){
      otherSensorRequested=TRUE;
      return SUCCESS;    
    }    
    if(state!=S_ON) return EBUSY;

    state = S_READ_TEMP;
    call I2CPacket.write(0x03, WRITE_ADDRESS, 1, (uint8_t*)TRIGGER_T_MEASUREMENT_NO_HOLD_MASTER);
    //call I2CPacket.read(I2C_START | I2C_STOP, READ_ADDRESS, 2, result);
    //call Timer.startPeriodic(TIMEOUT_14BIT);

    return SUCCESS;
  }

  command error_t Humidity.read() {
    if(state==S_OFF) return EOFF;
    if(state==S_READ_TEMP){
      otherSensorRequested=TRUE;
      return SUCCESS;
    }
    if(state!=S_ON) return EBUSY;

    state = S_READ_HUMIDITY;
    call I2CPacket.write(0x03, WRITE_ADDRESS, 1, (uint8_t*)TRIGGER_RH_MEASUREMENT_NO_HOLD_MASTER);
    return SUCCESS;
  }

  event void Timer.fired() {
    if(state==S_STARTING){
      state = S_ON;
      signal SplitControl.startDone(SUCCESS);
    } else if(state==S_READ_HUMIDITY){
        //uint16_t result;
        uint8_t res[2];
        call I2CPacket.read(0x03, READ_ADDRESS, 2, res);
            
        if(otherSensorRequested){
          state=S_ON;
          call Temperature.read();
        } else if(stopRequested){
          state=S_ON;
          call SplitControl.stop();
        }
        //signal Humidity.readDone(error, result);
    } else if(state==S_READ_TEMP){
        //uint16_t result;
        uint8_t res[2];
        call I2CPacket.read(0x03, READ_ADDRESS, 2, res);

        if(otherSensorRequested){
          state=S_ON;
          call Humidity.read();
        } else if(stopRequested){
          state=S_ON;
          call SplitControl.stop();
        }
        //signal Temperature.readDone(error, result);
    }
  }

  async event void I2CPacket.readDone(error_t error, uint16_t addr, uint8_t length, uint8_t* data) {
    if(state == S_READ_TEMP) signal Temperature.readDone(SUCCESS, *data);
    if(state == S_READ_HUMIDITY) signal Humidity.readDone(SUCCESS, *data);
  }
  
  async event void I2CPacket.writeDone(error_t error, uint16_t addr, uint8_t length, uint8_t* data) {
    if(state == S_READ_TEMP) call Timer.startOneShot(TIMEOUT_14BIT);
    if(state == S_READ_HUMIDITY) call Timer.startOneShot(TIMEOUT_8BIT);
  }

  default event void Temperature.readDone(error_t error, uint16_t val) {}
  default event void Humidity.readDone(error_t error, uint16_t val) {}
  default event void SplitControl.startDone(error_t error) { }
  default event void SplitControl.stopDone(error_t error) { }
}
