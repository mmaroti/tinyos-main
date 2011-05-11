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
  uses interface GeneralIO as NSEL;
  uses interface Resource;
  uses interface FastSpiByte;
  uses interface Leds;
  uses interface Timer<TMilli>;
  uses interface GeneralIO as FlashEn;
  uses interface GpioCapture as IRQ;
  uses interface GeneralIO as GIO;
}
implementation
{
  uint8_t count=0;
  bool intxx = FALSE;
  
  inline uint8_t readRegister(uint8_t reg)
  {
      call NSEL.clr();
      call FastSpiByte.splitWrite(reg&0x7f);
      call FastSpiByte.splitReadWrite(0);
      reg = call FastSpiByte.splitRead();
      call NSEL.set();

      return reg;
  }
  
  inline void writeRegister(uint8_t reg, uint8_t value)
  {
      call NSEL.clr();
      call FastSpiByte.splitWrite(0x80 + (reg&0x7f));
      call FastSpiByte.splitReadWrite(value);
      call FastSpiByte.splitRead();
      call NSEL.set();
  }
  
  event void Boot.booted() {
    call NSEL.makeOutput();
    call NSEL.set();
    call FlashEn.makeOutput();
    call FlashEn.set();
    
    call Timer.startPeriodic(1000);
    call IRQ.captureFallingEdge();
    //DDRB|=1<<4;
  }
  
  event void Timer.fired(){
    call Resource.request();
  }
  
  event void Resource.granted(){
    call Leds.set(0);    
    count++;
    if(intxx){
      uint8_t rr=readRegister(4);
      call Leds.set(rr);
      intxx = FALSE;
    }
      
    writeRegister(6,0xff);
    if(count==3){
      writeRegister(7,0x00);     
      //writeRegister(0x0d,0x1d);
    } else if(count==5){
      writeRegister(7,0x04);     
      //writeRegister(0x0d,0x1e);
    }
//     if(call GIO.get())
//       call Leds.led0On();
//     else
//       call Leds.led0Off();
    call Resource.release();
  }
  
  async event void IRQ.captured(uint16_t time){
    //call Leds.set(0xff);
    intxx = TRUE;
  }
}

