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

module RomReaderP {
  provides interface Read<uint32_t> as RawTemperature;
  provides interface Read<uint32_t> as RawPressure;

  provides interface Calibration as Cal;
  uses interface Timer<TMilli>;

  uses interface I2CPacket<TI2CBasicAddr>;
  uses interface Resource as I2CResource;
}
implementation {
  enum {
    S_OFF = 0,
    S_STARTING,
    S_STOPPING,
    S_ON,
 //   S_BUSY,
 //   S_READ_ADC,
 //   S_READ,
    S_READ_TEMP,
    S_READ_PRESS,
  };
  bool accessingROM = FALSE;
  bool readingADC = FALSE;
  uint8_t num;
  uint8_t reg[2];
  uint16_t mesres[7] ;
  uint8_t rawres[3];
  uint32_t rawret;
  uint8_t state = S_ON;
  uint8_t cmd;

  command error_t RawTemperature.read() {
    state = S_READ_TEMP;
    call I2CResource.request();
    return SUCCESS;
  }

  command error_t RawPressure.read() {
    state = S_READ_PRESS;
    call I2CResource.request();
    return SUCCESS;
  }

  event void Timer.fired() {
    if((state == S_READ_TEMP) || (state == S_READ_PRESS)) {
      if(readingADC) {
        call I2CPacket.read(I2C_START | I2C_STOP, ADDRESS, 3, rawres);
      } else {
       call I2CPacket.write(I2C_START | I2C_STOP, ADDRESS, 1, 0x00);
      }
    }
  }

  task void signalReadDone() {
    //sigReadDone(num);
    if(accessingROM) {
      if(num == 6) { 
        accessingROM = FALSE;
        call I2CResource.release();
        signal Cal.dataReady(SUCCESS, mesres);
      }
      num++;
    } else if(readingADC) {
      readingADC = FALSE;
      state = S_ON;
      call I2CResource.release();
      switch(state) {
        case S_READ_TEMP:
          signal RawTemperature.readDone(SUCCESS, rawret);
          break;
        case S_READ_PRESS:
          signal RawPressure.readDone(SUCCESS, rawret);
          break;
      }
    }
  }

  async event void I2CPacket.readDone(error_t error, uint16_t addr, uint8_t length, uint8_t *data) {
    if(accessingROM) {
      mesres[num] = data[0] << 8;
      mesres[num] = mesres[num] | data[1];
      //if(num == 6)
      //post signalReadDone();
    } else if(readingADC) {
      rawret = (uint32_t)data[0] << 16;
      rawret = rawret | (data[1] << 8);
      rawret = rawret | data[2];
    }
    post signalReadDone();
  }

  task void readTask() {
    if(accessingROM)
      call I2CPacket.read(I2C_START | I2C_STOP, ADDRESS, 2, reg);
    if(readingADC) {
      call Timer.startOneShot(2);
    } else
    if(state == S_READ_TEMP) {
      readingADC = TRUE;
      call Timer.startOneShot(2);
    }
    if(state == S_READ_PRESS) {
      readingADC = TRUE;
      call Timer.startOneShot(2);
    }
  }

  async event void I2CPacket.writeDone(error_t error, uint16_t addr, uint8_t length, uint8_t *data) {
    post readTask();
  }

  event void I2CResource.granted() {
    if((num <=6) && accessingROM) {
      call I2CPacket.write(I2C_START | I2C_STOP, ADDRESS, 1, (0xa0 | (num << 1) ));
    } else if(state == S_READ_TEMP) {
      cmd = 0x50;
      call I2CPacket.write(I2C_START | I2C_STOP, ADDRESS, 1, &cmd);
    } else if(state == S_READ_PRESS) {
      cmd = 0x40;
      call I2CPacket.write(I2C_START | I2C_STOP, ADDRESS, 1, &cmd);
    }    
  }

  command error_t Cal.getData() {
    if(!accessingROM) {
    accessingROM = TRUE;
    num=0;
    call I2CResource.request();
    } else return FAIL;
    return SUCCESS;
  }
 
  default event void RawTemperature.readDone(error_t error, uint32_t val) { }
  default event void RawPressure.readDone(error_t error, uint32_t val) { } 
}
