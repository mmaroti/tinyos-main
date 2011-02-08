/*
* Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Zsolt Szabo
*/

/*
 * Bug at the I2CResource.granted event, need waiting so thus the Diagmsg part included
 * probably problem occurs in Atm128 I2C lib during seizure of Resource
 */

#include "SensirionSht21.h"

module Sht21P {
  provides interface Read<uint16_t> as Temperature;
  provides interface Read<uint16_t> as Humidity;
  
  provides interface SplitControl;
  uses interface I2CPacket<TI2CBasicAddr>;
  uses interface Timer<TMilli>;
  uses interface Resource as I2CResource;

  uses interface Leds;
}
implementation {
  uint16_t mesrslt=0;
  uint8_t res[2];
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
  
  bool stopRequested = FALSE;
  bool otherSensorRequested=FALSE;    
  command error_t SplitControl.start() {
    if(state == S_STARTING) return EBUSY;
    if(state != S_OFF) return EALREADY;
    state=S_STARTING;
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
   
    if(state==S_OFF) return EOFF;
    if(state==S_READ_HUMIDITY){
      otherSensorRequested=TRUE;
      return SUCCESS;    
    }    
    if(state!=S_ON) return EBUSY;

    atomic state = S_READ_TEMP;
    
    call I2CResource.request();

    return SUCCESS;
  }

  command error_t Humidity.read() {
    if (!on) atomic state = S_ON;
    on++;
    if(state==S_OFF) return EOFF;
    if(state==S_READ_TEMP){
      otherSensorRequested=TRUE;
      return SUCCESS;
    }
    if(state!=S_ON) return EBUSY;

    atomic state = S_READ_HUMIDITY;
    
    call I2CResource.request();
    return SUCCESS;
  }

  event void Timer.fired() {
    if(state==S_OFF) {
      atomic state=S_ON;
      signal SplitControl.startDone(SUCCESS);
    }
    else if(state==S_STARTING){
      atomic state = S_ON;
      signal SplitControl.startDone(SUCCESS);
    } else if(state==S_READ_HUMIDITY){
        call I2CPacket.read(I2C_START | I2C_STOP, I2C_ADDRESS, 2, res);
        if(otherSensorRequested){
          atomic state=S_ON;
          call Temperature.read();
        } else if(stopRequested){
          atomic state=S_ON;
          call SplitControl.stop();
        }
       
    } else if(state==S_READ_TEMP){       
		call Leds.led0On();
        call I2CPacket.read(I2C_START | I2C_STOP, I2C_ADDRESS, 2, res);
   
        if(otherSensorRequested){
          atomic state=S_ON;
          call Humidity.read();
        } else if(stopRequested){
          atomic state=S_ON;
          call SplitControl.stop();
        }

    }
  }

  task void signalReadDone()
  {
  if(state == S_READ_TEMP) atomic {state= S_ON; signal Temperature.readDone(SUCCESS, mesrslt);}
  if(state == S_READ_HUMIDITY) atomic {state= S_ON; signal Humidity.readDone(SUCCESS, mesrslt);}
  
  }

  async event void I2CPacket.readDone(error_t error, uint16_t addr, uint8_t length, uint8_t* data) {
    mesrslt = data[0]<<8;
    mesrslt |= (data[1]&0xfc);
    call I2CResource.release();
	call Leds.led3On();
    post signalReadDone();
  }
  
  task void startTimeout()
  {
   if(state == S_READ_TEMP) call Timer.startOneShot(TIMEOUT_14BIT);
    if(state == S_READ_HUMIDITY) call Timer.startOneShot(TIMEOUT_12BIT);
  }
  

  async event void I2CPacket.writeDone(error_t error, uint16_t addr, uint8_t length, uint8_t* data) {
    post startTimeout(); 
  }

  event void I2CResource.granted() {
    if(state == S_READ_TEMP) {
      uint8_t data=0xf3;
      /*error_t err=*/call I2CPacket.write(I2C_START, I2C_ADDRESS, 1, &data);

   } else if (state == S_READ_HUMIDITY) {
     uint8_t data=0xf5;
     
     /*error_t err=*/call I2CPacket.write(I2C_START, I2C_ADDRESS, 1, &data);

   }
             
  }

  default event void Temperature.readDone(error_t error, uint16_t val) {}
  default event void Humidity.readDone(error_t error, uint16_t val) {}
  default event void SplitControl.startDone(error_t error) { }
  default event void SplitControl.stopDone(error_t error) { }
}
