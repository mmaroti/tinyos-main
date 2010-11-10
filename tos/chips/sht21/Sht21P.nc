#include "SensirionSht21.h"

module Sht21P {
  provides interface Read<uint16_t> as Temperature;
  provides interface Read<uint16_t> as Humidity;
  
  provides interface SplitControl;
  uses interface I2CPacket<TI2CBasicAddr>;
  uses interface Timer<TMilli>;
  uses interface Resource as I2CResource;

  uses interface DiagMsg;
}
implementation {
  uint16_t mesrslt;
  enum {
    S_OFF = 0,
    S_STARTING,
    S_STOPPING,
    S_ON,
    S_READ_TEMP,
    S_READ_HUMIDITY,
  };

  uint8_t state = S_OFF;
  bool on=0;
  //uint8_t res[2];
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
      atomic state = S_OFF;
      post signalStopDone();
    } else {
      stopRequested = TRUE;
    }
    return SUCCESS;
  }

  command error_t Temperature.read() {
    if (!on) atomic state = S_ON;
    on++;
    if(call DiagMsg.record()){
			call DiagMsg.str("sht21p.tempread");
			call DiagMsg.uint8(state);
			call DiagMsg.send();
		}
    if(state==S_OFF) return EOFF;
    if(state==S_READ_HUMIDITY){
      otherSensorRequested=TRUE;
      return SUCCESS;    
    }    
    if(state!=S_ON) return EBUSY;

    atomic state = S_READ_TEMP;
    /*if(call DiagMsg.record()){
			call DiagMsg.str("before i2cwrite");
			call DiagMsg.uint8(state);
			call DiagMsg.send();
		}*/
    
    call I2CResource.request();
    //call I2CPacket.write(0x03, WRITE_ADDRESS, 1, (uint8_t*)TRIGGER_T_MEASUREMENT_NO_HOLD_MASTER);
    //call I2CPacket.read(I2C_START | I2C_STOP, READ_ADDRESS, 2, result);
    //call Timer.startPeriodic(TIMEOUT_14BIT);
    /*if(call DiagMsg.record()){
			call DiagMsg.str("sht2afteri2cwrite");
			call DiagMsg.uint8(state);
			call DiagMsg.send();
		}*/

    return SUCCESS;
  }

  command error_t Humidity.read() {
    if(state==S_OFF) return EOFF;
    if(state==S_READ_TEMP){
      otherSensorRequested=TRUE;
      return SUCCESS;
    }
    if(state!=S_ON) return EBUSY;

    atomic state = S_READ_HUMIDITY;
    call I2CPacket.write(0x03, WRITE_ADDRESS, 1, (uint8_t*) TRIGGER_RH_MEASUREMENT_NO_HOLD_MASTER);
    return SUCCESS;
  }

  event void Timer.fired() {
    if(state==S_STARTING){
      atomic state = S_ON;
      signal SplitControl.startDone(SUCCESS);
    } else if(state==S_READ_HUMIDITY){
        //uint16_t result;
        uint8_t res[2];
        call I2CPacket.read(I2C_START, READ_ADDRESS, 2, res);
            
        if(otherSensorRequested){
          atomic state=S_ON;
          call Temperature.read();
        } else if(stopRequested){
          atomic state=S_ON;
          call SplitControl.stop();
        }
        //signal Humidity.readDone(error, result);
    } else if(state==S_READ_TEMP){



        //uint16_t result;
        uint8_t res[2]= {0,0};

if(call DiagMsg.record()){
			call DiagMsg.str("sht21p.timer fired temp");
			call DiagMsg.uint8(state);
			call DiagMsg.send();
		}
        call I2CPacket.read(0x03, WRITE_ADDRESS, 2, res);
   if(call DiagMsg.record()){
			call DiagMsg.str("data");
			call DiagMsg.uint8(res[0]);
                        call DiagMsg.uint8(res[1]);
			call DiagMsg.send();
		}

        if(otherSensorRequested){
          atomic state=S_ON;
          call Humidity.read();
        } else if(stopRequested){
          atomic state=S_ON;
          call SplitControl.stop();
        }
        //result = (res[0]<<8);
        //result |= res[1];
        //signal Temperature.readDone(SUCCESS, result);
    }
  }

  task void signalReadDone()
  {
  if(state == S_READ_TEMP) atomic signal Temperature.readDone(SUCCESS, mesrslt);
  if(state == S_READ_HUMIDITY) atomic signal Humidity.readDone(SUCCESS, mesrslt);
  
  }

  async event void I2CPacket.readDone(error_t error, uint16_t addr, uint8_t length, uint8_t* data) {
    mesrslt = data[0]<<8;
    mesrslt |= data[1]; 
     if(call DiagMsg.record()){
			call DiagMsg.str("I2CMP.rdDn");
			call DiagMsg.uint8(state);
			call DiagMsg.uint16(mesrslt);
			call DiagMsg.send();
		}
   call I2CResource.release();
   post signalReadDone();
  }
  
  task void startTimeout()
  {
   if(state == S_READ_TEMP) call Timer.startOneShot(TIMEOUT_14BIT);
    if(state == S_READ_HUMIDITY) call Timer.startOneShot(TIMEOUT_8BIT);
  }
  

  async event void I2CPacket.writeDone(error_t error, uint16_t addr, uint8_t length, uint8_t* data) {
  if(call DiagMsg.record()){
			call DiagMsg.str("i2cwritedone");
			call DiagMsg.uint8(state);
			call DiagMsg.send();
		}
   post startTimeout();
   
  }

  event void I2CResource.granted() {
  /*if(call DiagMsg.record()){
			call DiagMsg.str("sht21p. Granted");
			call DiagMsg.uint8((uint8_t*)TRIGGER_T_MEASUREMENT_NO_HOLD_MASTER);
			call DiagMsg.send();
		}*/

    uint8_t data=0xf3;
    error_t err=call I2CPacket.write(I2C_START, WRITE_ADDRESS, 1, &data);

    if(call DiagMsg.record()){
	    call DiagMsg.str("sht21.writeSUCC");
	    call DiagMsg.uint8(WRITE_ADDRESS);
	    call DiagMsg.uint8(err);
	    call DiagMsg.send();
    }
    
               
  }

  default event void Temperature.readDone(error_t error, uint16_t val) {}
  default event void Humidity.readDone(error_t error, uint16_t val) {}
  default event void SplitControl.startDone(error_t error) { }
  default event void SplitControl.stopDone(error_t error) { }
}
