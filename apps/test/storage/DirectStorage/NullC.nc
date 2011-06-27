/*
* Copyright (c) 2000-2005 The Regents of the University  of California.  
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer in the
*   documentation and/or other materials provided with the
*   distribution.
* - Neither the name of the University of California nor the names of
*   its contributors may be used to endorse or promote products derived
*   from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
* THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*
* Copyright (c) 2002-2005 Intel Corporation
* All rights reserved.
*
* This file is distributed under the terms in the attached INTEL-LICENSE     
* file. If you do not find these files, copies can be found by writing to
* Intel Research Berkeley, 2150 Shattuck Avenue, Suite 1300, Berkeley, CA, 
* 94704.  Attention:  Intel License Inquiry.
*/

/**
* Null is an empty skeleton application.  It is useful to test that the
* build environment is functional in its most minimal sense, i.e., you
* can correctly compile an application. It is also useful to test the
* minimum power consumption of a node when it has absolutely no 
* interrupts or resources active.
*
* @author Cory Sharp <cssharp@eecs.berkeley.edu>
* @date February 4, 2006
*/
module NullC @safe()
{
  uses interface Boot;
  uses interface VolumeSettings;
  uses interface DirectStorage;
  uses interface DiagMsg;
  uses interface Leds;
  uses interface Timer<TMilli>;
  uses interface SplitControl;
}
implementation
{
  enum{
    buflen=1024,
  };
  uint8_t buffer[buflen];
    uint8_t currentPage=0;
  
  inline void printbuffer(void *buf, uint16_t len){
    uint16_t i;
    for(i=0;i<len;i+=16){
      if(call DiagMsg.record()){
        call DiagMsg.hex16(i);
        if(len-i>=16)
          call DiagMsg.hex8s(buf+i, 16);
        else
          call DiagMsg.hex8s(buf+i, len-i);
        call DiagMsg.send();
      }
    }
  }
  
  inline void fillbuffer(void *buf, uint8_t start){
    uint16_t i;
    for(i=0;i<buflen;i++)
      buffer[i]=i+start;
  }
  
  inline void fillbuffer2(void *buf, uint8_t val){
    uint16_t i;
    for(i=0;i<buflen;i++)
      buffer[i]=val;
  }
  
  event void Boot.booted() {
    fillbuffer(buffer,10);
    call Leds.set(0xff);
    call SplitControl.start();
  }
  
  event void SplitControl.startDone(error_t err){
    call Timer.startOneShot(5000);
  }
  
  event void Timer.fired(){
    call Leds.set(0);
    if(call DiagMsg.record()){
      call DiagMsg.chr('S');
      call DiagMsg.uint32(call VolumeSettings.getVolumeSize());
      call DiagMsg.chr('U');
      call DiagMsg.uint32(call VolumeSettings.getTotalEraseUnits());
      call DiagMsg.chr('u');
      call DiagMsg.uint32(call VolumeSettings.getEraseUnitSize());
      call DiagMsg.send();
    }
    if(call DiagMsg.record()){
      call DiagMsg.chr('W');
      call DiagMsg.uint32(call VolumeSettings.getTotalWriteUnits());
      call DiagMsg.chr('w');
      call DiagMsg.uint32(call VolumeSettings.getWriteUnitSize());
      call DiagMsg.chr('F');
      call DiagMsg.uint8(call VolumeSettings.getFillByte());
      call DiagMsg.send();
    }
    //call DirectStorage.read(0, buffer, call VolumeSettings.getWriteUnitSize()*3);
    //fillbuffer(buffer,0);
    call DirectStorage.erase(currentPage);
    //call DirectStorage.read(0, buffer, buflen);
  }
  
  event void DirectStorage.eraseDone(uint16_t eraseUnitIndex, error_t error){
    if(call DiagMsg.record()){
      call DiagMsg.str("ED");
      call DiagMsg.uint16(eraseUnitIndex);
      call DiagMsg.uint8(error);
      call DiagMsg.send();
    }
    call DirectStorage.write(5,buffer,buflen-10);
/*    if(currentPage<call VolumeSettings.getTotalEraseUnits())
      call DirectStorage.write(currentPage*call VolumeSettings.getWriteUnitSize(),buffer,call VolumeSettings.getWriteUnitSize());
    else 
      call DirectStorage.read(0, buffer, call VolumeSettings.getWriteUnitSize()*3);*/
  }
  
    event void DirectStorage.writeDone(uint32_t addr, void *buf, uint32_t len, error_t error){
    if(call DiagMsg.record()){
      call DiagMsg.str("WD");
      call DiagMsg.uint32(addr);
      call DiagMsg.uint32(len);
      call DiagMsg.uint8(error);
      call DiagMsg.send();
    }
//     currentPage++;
//     if(currentPage<call VolumeSettings.getTotalEraseUnits())
//       call DirectStorage.erase(currentPage);
//     else {
//       //printbuffer(buf,len);
//       fillbuffer2(fillbuffer,1);
//       call DirectStorage.erase(1);
//     }
    currentPage++;
    if(currentPage<2)
      call DirectStorage.write(0,buffer,5);
    else
      call DirectStorage.read(0, buffer, buflen);
  }
  
  event void DirectStorage.readDone(uint32_t addr, void *buf, uint32_t len, error_t error){
    if(call DiagMsg.record()){
      call DiagMsg.str("RD");
      call DiagMsg.uint32(addr);
      call DiagMsg.uint32(len);
      call DiagMsg.uint8(error);
      call DiagMsg.send();
    }
    printbuffer(buf,len);
  }
  
  event void DirectStorage.flushDone(error_t error){
    if(call DiagMsg.record()){
      call DiagMsg.str("FD");
      call DiagMsg.uint8(error);
      call DiagMsg.send();
    }
  }
  
  event void DirectStorage.crcDone(uint16_t calculatedCrc, uint32_t addr, uint32_t len, error_t error){
  }
  
  event void SplitControl.stopDone(error_t err){}
}

