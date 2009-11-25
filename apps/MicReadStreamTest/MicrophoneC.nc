/** Copyright (c) 2009, University of Szeged
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
* Author: Zoltan Kincses
*/

#include "MicReadAdc.h"

module MicrophoneC
{
  provides interface SplitControl;
  uses {
    interface Timer<TMilli>;
    interface GeneralIO as MicPower;  // Power control pin
    interface GeneralIO as MicMuxSel; // Microphone / tone detector selection pin
    interface I2CPacket<TI2CBasicAddr>;
    interface Resource as I2CResource;
    interface Leds ;
  }
}
implementation
{
//  enum {
//    MIC_POT_ADDR = 0x5A,
//    MIC_POT_SUBADDR = 0,
//    MIC_GAIN = 64
//  };


  uint8_t gainPacket[2];

  task void gainOk(), gainFail(), stopDone();

  command error_t SplitControl.start() {
    // Powerup the microphone
    call MicPower.makeOutput();
    call MicPower.set();
    // Select raw microphone output
    call MicMuxSel.makeOutput();    
    call MicMuxSel.set(); 

    // Request the I2C bus to adjust gain
    call I2CResource.request();

    return SUCCESS;
  }

  event void I2CResource.granted() {
    error_t ok;
//    call Leds.led0On();

    //Send gain-control packet over I2C bus
    
    //gainPacket[0] = MIC_POT_SUBADDR;
    //gainPacket[1] = MIC_GAIN;
    //ok = call I2CPacket.write(I2C_START | I2C_STOP, MIC_POT_ADDR,sizeof gainPacket, gainPacket);
    gainPacket[0]=0;
    gainPacket[1]=64;
    ok=call I2CPacket.write(0x3,0x5A, 2, gainPacket);
    
    if (ok != SUCCESS)
    {
      signal SplitControl.startDone(FAIL);
//      call Leds.led1On();
    }
  }

  async event void I2CPacket.writeDone(error_t error, uint16_t addr,
				       uint8_t length, uint8_t* data) {
    // Release I2C bus and wait for microphone to warmup (report failure
    // in case of error)
    call I2CResource.release();
    if (error == SUCCESS)
    {
      post gainOk();
//      call Leds.led2On();
    }
    else
    {
      post gainFail();
    }
  }

  task void gainOk() {
    call Timer.startOneShot(MICROPHONE_WARMUP); 
  }

  task void gainFail() {
    signal SplitControl.startDone(FAIL);
  }

  event void Timer.fired() {
    // Microphone warmed up. Signal completion of startup.
    signal SplitControl.startDone(SUCCESS);
  }

  command error_t SplitControl.stop() {
    // Power off microphone
    call MicPower.clr();
    call MicPower.makeInput();

    // And let our caller know we're done - post a task as one should not
    // signal events directly from commands
    post stopDone();
    return SUCCESS;
  }

  task void stopDone() {
    signal SplitControl.stopDone(SUCCESS);
  }

  async event void I2CPacket.readDone(error_t error, uint16_t addr, uint8_t length, uint8_t* data) {
  }
}
