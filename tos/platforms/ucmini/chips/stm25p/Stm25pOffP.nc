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
  uses interface Stm25pSpi;
  uses interface Resource;
}
implementation {
  command error_t Stm25pOff.init() {
    if(!uniqueCount("Stm25pOn")) {
      while( (call Resource.immediateRequest()) != SUCCESS) ;
    }
  return SUCCESS;
  }

  event void Resource.granted() {
    call Stm25pSpi.powerDown();
    call Resource.release();
  }

  async event void Stm25pSpi.sectorEraseDone( uint8_t sector, error_t error ) {}
  async event void Stm25pSpi.readDone( stm25p_addr_t addr, uint8_t* buf, stm25p_len_t len, error_t error ) {}
  async event void Stm25pSpi.pageProgramDone( stm25p_addr_t addr, uint8_t* buf, stm25p_len_t len, error_t error ) {}
  async event void Stm25pSpi.computeCrcDone( uint16_t crc, stm25p_addr_t addr, stm25p_len_t len, error_t error ) {}
  async event void Stm25pSpi.bulkEraseDone( error_t error ) {}
}
