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

module PlatformTestP {
  uses {
    interface Boot;
    interface Leds;
    interface AMSend;
    interface Read<uint16_t> as Light;
    interface Read<uint16_t> as ShtTemp;
    interface Read<uint16_t> as ShtHum;
    interface Read<int16_t> as MsTemp;
    interface Read<uint32_t> as MsPress;
    interface SplitControl as Radio;
    //interface SplitControl as Bus;
    interface GeneralIO as Power;
    interface SplitControl as ShtSplit;
    interface SplitControl as BhSplit;
    interface SplitControl as MsSplit;
    interface DiagMsg;
  }
}
implementation {
  bool Ss,Bs,Ms;
  uint16_t ShtT, ShtH, L;
  int16_t MsT;
  message_t message;

  event void Boot.booted() {
    Ss=Bs=Ms=FALSE;
    call Radio.start();
  }

  event void Radio.startDone(error_t err) {
    if(err == SUCCESS) {
     // call Bus.start();
      call Power.makeOutput();
      call Power.set();
      call ShtSplit.start();
    }
    else {
      call Radio.start();
    }
  }

  event void ShtSplit.startDone(error_t err) {
    call ShtTemp.read();
  }

  event void ShtSplit.stopDone(error_t err) {}

  event void BhSplit.startDone(error_t err) {
    Bs = TRUE;
    call Light.read();
  }

  event void BhSplit.stopDone(error_t err) {}

  event void MsSplit.startDone(error_t err) {
    Ms = TRUE;
    call MsTemp.read();
  }

  event void MsSplit.stopDone(error_t err) {}

  /*event void Bus.startDone(error_t berr) {
    if(call DiagMsg.record()) {
      call DiagMsg.str("BusDone");
      call DiagMsg.uint8(berr);
      call DiagMsg.send();
    }
    call ShtTemp.read();
  }*/

  event void ShtTemp.readDone(error_t err, uint16_t dataTemp) {
    if(call DiagMsg.record()) {
      call DiagMsg.str("TempDone");
      call DiagMsg.uint8(err);
      call DiagMsg.uint16(dataTemp);
      call DiagMsg.send();
    }
    ShtT = dataTemp;
    call ShtHum.read();//call Leds.led3On();
  }

  event void ShtHum.readDone(error_t err, uint16_t dataHum) {
    if(call DiagMsg.record()) {
      call DiagMsg.str("HumDone");
      call DiagMsg.uint8(err);
      call DiagMsg.uint16(dataHum);
      call DiagMsg.send();
    }
    ShtH = dataHum;
    //call Light.read();
    if(!Bs)
      call BhSplit.start();
    else
      call Light.read();
  }

  event void Light.readDone(error_t err, uint16_t dataLight) {
    if(call DiagMsg.record()) {
      call DiagMsg.str("LightDone");
      call DiagMsg.uint8(err);
      call DiagMsg.uint16(dataLight);
      call DiagMsg.send();
    }
    L = dataLight;
    //call MsTemp.read();
    if(!Ms)
      call MsSplit.start();
    else
      call MsTemp.read();
  }

  event void MsTemp.readDone(error_t err, int16_t dataM) {
    if(call DiagMsg.record()) {
      call DiagMsg.str("MsTempDone");
      call DiagMsg.uint8(err);
      call DiagMsg.int16(dataM);
      call DiagMsg.send();
    }
    MsT = dataM;
    call MsPress.read();
  }

  event void MsPress.readDone(error_t err, uint32_t data) {
    datamsg_t* packet = (datamsg_t*)(call AMSend.getPayload(&message, sizeof(datamsg_t) ));
    packet -> ShtTemp_data = ShtT;
    packet -> ShtHum_data = ShtH;
    packet -> Light_data = L;
    packet -> MsTemp_data = MsT;
    packet -> MsPress_data = data;
    if(call DiagMsg.record()) {
      call DiagMsg.str("MSPressDone");
      call DiagMsg.uint8(err);
      call DiagMsg.uint32(data);
      call DiagMsg.send();
    }
    call AMSend.send(AM_BROADCAST_ADDR, &message, sizeof(datamsg_t));   
  }

  event void AMSend.sendDone(message_t* bufPtr, error_t error) {
    call ShtTemp.read();
    call Leds.led1Toggle();
  }

  event void Radio.stopDone(error_t err) {}
 // event void Bus.stopDone(error_t err) {}
}
