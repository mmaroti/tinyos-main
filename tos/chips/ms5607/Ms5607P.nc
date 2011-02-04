#include "Ms5607.h" 

/*
* Copyright (c) 2011, University of Szeged
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


module Ms5607P  {
  provides interface Read<uint32_t> as Pressure;
  provides interface Read<uint16_t> as Temperature;
  provides interface Read<uint32_t> as PressureSlow;
  provides interface Read<uint16_t> as TemperatureSlow;
  provides interface SplitControl;
  
  uses interface Timer<TMilli>;
  uses interface Read<uint32_t> as RawTemp;
  uses interface Read<uint32_t> as RawPress;
  uses interface Calibration as Cal;

  uses interface DiagMsg;
  uses interface Leds;
}
implementation {
  enum {
    S_OFF = 0,
    S_STARTING,
    S_STOPPING,
    S_ON,
    S_READ_TEMP,
    S_READ_PRESS,
  };

  uint8_t res[3];
  uint32_t mesres;
  uint8_t state = S_OFF;
  uint16_t c1,c2,c3,c4,c5,c6,OFF,SENS;
  int32_t dT,P;
  int16_t TEMP;
  bool stopRequested = FALSE;
  bool otherSensorRequested = FALSE;
  bool setup = TRUE;


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

  event void Cal.dataReady(error_t error, uint16_t* calibration) {
    c1 = calibration[1];
    c2 = calibration[2];
    c3 = calibration[3];
    c4 = calibration[4];
    c5 = calibration[5];
    c6 = calibration[6];
  }

  command error_t Pressure.read() {
    if(state == S_OFF) return EOFF;
    if(state == S_READ_TEMP) {
      otherSensorRequested = TRUE;
      return SUCCESS;
    }
    if(state != S_ON) return EBUSY;
/*i2c */
    state = S_READ_PRESS;
    //call I2CResource.request();
    call RawPress.read();
    return SUCCESS;
  }

  command error_t Temperature.read() {
    if(state == S_OFF) return EOFF;
    if(state == S_READ_PRESS) {
      otherSensorRequested = TRUE;
      return SUCCESS;
    }
    if(state != S_ON) return EBUSY;

    state = S_READ_TEMP;
    //call I2CResource.request();
    call RawTemp.read();
    return SUCCESS;
  }

  command error_t PressureSlow.read() {
    return SUCCESS;
  }
  
  command error_t TemperatureSlow.read() {
    return SUCCESS;
  }

  event void Timer.fired() {
    if(state == S_OFF) {
      state = S_ON;
      signal SplitControl.startDone(SUCCESS);
    }  
  }

  task void signalReadDone() {
    signal Pressure.readDone(SUCCESS, mesres);
  }

  event void RawTemp.readDone(error_t error, uint32_t val) {
    if(error == SUCCESS) {
      dT = val - (c5 << 8);  // <<8
      TEMP = 2000 + (dT * (uint32_t)c6 >> 23); // >>23
    }
    signal Temperature.readDone(error, TEMP);
  }

  event void RawPress.readDone(error_t error, uint32_t val) {
    int64_t offset, sensitivity;
    offset = (uint32_t)c2 << 17 + ((int64_t)c4 * dT) >> 6; // <<17     >>6
    sensitivity = (uint32_t)c1 << 128 + ((int64_t)c3 * dT) >> 7;// <<16   >>7
    P = (int64_t)(val * sensitivity >> 21 - offset) >> 15;// >>21    >>15

    signal Pressure.readDone(error, P);   
  }

  default event void Pressure.readDone(error_t error, uint32_t val) { }
  default event void Temperature.readDone(error_t error, uint16_t val) { }
  default event void PressureSlow.readDone(error_t error, uint32_t val) { }
  default event void TemperatureSlow.readDone(error_t error, uint16_t val) { }
  default event void SplitControl.startDone(error_t error) { }
  default event void SplitControl.stopDone(error_t error) { }
}
