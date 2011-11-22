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
  uses interface DiagMsg;
  uses interface Leds;
  uses interface Timer<TMilli>;
  uses interface SplitControl;
  uses interface PageStorage;
}
implementation
{
  enum{
    buflen=1024,
  };
  uint8_t buffer[buflen];
  uint8_t currentPage=0;
  error_t err;
  
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
  
  inline void fillbuffer(uint8_t *buf, uint8_t start){
    uint16_t i;
    for(i=0;i<buflen;i++)
      *(buf+i)=i+start;
  }
  
  inline void fillbuffer2(uint8_t *buf, uint8_t val){
    uint16_t i;
    for(i=0;i<buflen;i++)
      *(buf+i)=val;
  }
  
  event void Boot.booted() {
    fillbuffer2(buffer,10);
    call Leds.set(0xff);
    call SplitControl.start();
  }
  
  event void SplitControl.startDone(error_t error){
    call Timer.startOneShot(500);
  }
  
  event void Timer.fired(){
    if(call DiagMsg.record()){
      call DiagMsg.chr('S');
      call DiagMsg.send();
    }
    call Leds.set(0);
    //err=call PageStorage.erase(0, FALSE);
    err=call PageStorage.read(0,buffer);
    if(call DiagMsg.record()){
      call DiagMsg.chr('r');
      call DiagMsg.uint8(err);
      call DiagMsg.send();
    }
  }
  
  event void PageStorage.eraseDone(uint16_t pageNum, bool realErase, error_t error){
    if(call DiagMsg.record()){
      call DiagMsg.chr('E');
      call DiagMsg.uint8(error);
      call DiagMsg.send();
    }
    //err=call PageStorage.write(0, buffer);
    err=call PageStorage.read(0,buffer);
    if(call DiagMsg.record()){
      call DiagMsg.chr('w');
      call DiagMsg.uint8(err);
//       call DiagMsg.uint16((uint16_t)buffer);
      call DiagMsg.send();
    }
  }
  
  event void PageStorage.writeDone(uint16_t pageNum, uint8_t *buff, error_t error){
    if(call DiagMsg.record()){
      call DiagMsg.chr('W');
      call DiagMsg.uint8(error);
      call DiagMsg.send();
    }
    fillbuffer2(buffer,33);
    err=call PageStorage.read(0, buffer);
    if(call DiagMsg.record()){
      call DiagMsg.chr('r');
      call DiagMsg.uint8(err);
//       call DiagMsg.uint16((uint16_t)buffer);
      call DiagMsg.send();
    }    
  }
  
  event void PageStorage.readDone(uint16_t pageNum, uint8_t *buff, error_t error){
    if(call DiagMsg.record()){
      call DiagMsg.chr('R');
      call DiagMsg.uint8(error);
      call DiagMsg.send();
    }
    printbuffer(buff, 300);
  }
  
  event void SplitControl.stopDone(error_t error){}
}

