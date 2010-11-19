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
module I2CBusP {
  provides interface SplitControl;
  uses interface SplitControl as TemphumSplit;
  uses interface SplitControl as LightSplit;
  uses interface SplitControl as PressureSplit;
  uses interface GeneralIO as POWER;
}
implementation {
  uint8_t cnt=0;
  command error_t SplitControl.start() {
    call POWER.makeOutput();
    call POWER.set();

    call TemphumSplit.start();
    //call LightSplit.start();
    //call PressureSplit.start();
    return SUCCESS;
  }

  command error_t SplitControl.stop() {
    call POWER.clr();
    signal SplitControl.stopDone(SUCCESS);
    return SUCCESS;
  }
  
  event void TemphumSplit.startDone(error_t error) {
    cnt |= 1;
    if(cnt == 7)
      signal SplitControl.startDone(SUCCESS);
  }
  
  event void LightSplit.startDone(error_t error) {
    cnt |= 2;
    if(cnt == 7)
      signal SplitControl.startDone(SUCCESS);
  }
  
  event void PressureSplit.startDone(error_t error) {
    cnt |= 4;
    if(cnt == 7)
      signal SplitControl.startDone(SUCCESS);
  }
  
  event void TemphumSplit.stopDone(error_t error) {
    cnt &= 6;
    if(cnt == 0)
      signal SplitControl.stopDone(SUCCESS);
  }

  event void LightSplit.stopDone(error_t error) {
    cnt &= 4;
    if(cnt == 0)
      signal SplitControl.stopDone(SUCCESS);
  }

  event void PressureSplit.stopDone(error_t error) {
    cnt &= 3;
    if(cnt == 0)
      signal SplitControl.stopDone(SUCCESS);
  }
  
  default event void SplitControl.startDone(error_t error) { }
  default event void SplitControl.stopDone(error_t error) { }
}
