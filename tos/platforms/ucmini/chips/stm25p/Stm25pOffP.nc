/*
 * Copyright (c) 2010, University of Szeged
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
 * - Neither the name of the copyright holder nor the names of
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
 *
 * Author: Zsolt Szabo
 */

module Stm25pOffP {
  provides interface Init as Stm25pOff;
  uses interface Resource as SpiResource;
  uses interface GeneralIO as CSN;
  uses interface GeneralIO as Hold;
  uses interface SpiByte;
}
implementation {
  enum {
    S_DEEP_SLEEP = 0xb9,
  };

  bool m_init = FALSE;

  uint8_t sendCmd( uint8_t cmd, uint8_t len ) {
    uint8_t tmp = 0;
    int i;

    call CSN.clr();
    for ( i = 0; i < len; i++ )
      tmp = call SpiByte.write( cmd );
    call CSN.set();

    return tmp;
  }

  command error_t Stm25pOff.init() {
    call CSN.makeOutput();
    if(!uniqueCount("Stm25pOn")) {
      call Hold.makeOutput();
      call CSN.set();
      call Hold.set();
      if(call SpiResource.immediateRequest()==SUCCESS){
        sendCmd(S_DEEP_SLEEP, 1);
      } else if(call SpiResource.request()==SUCCESS)
        m_init=TRUE;//otherwise we can't put the chip to deep sleep
    }
    call CSN.set();
    return SUCCESS;
  }

  event void SpiResource.granted() {
    if (m_init){
      m_init=FALSE;
      sendCmd(S_DEEP_SLEEP, 1);
      call SpiResource.release();
    }
    sendCmd(S_DEEP_SLEEP, 1);
    call SpiResource.release();
  }
}
