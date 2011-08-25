/*
 * Copyright (c) 2011, University of Szeged
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
 * Author: Andras Biro
 */

generic module AtmegaPinChangeP(){
  uses interface HplAtmegaPinChange;
  provides interface AtmegaPinChange[uint8_t pin];
}
implementation{
  /* Enables the interrupt */
  async command void AtmegaPinChange.enable[uint8_t pin](){
    call HplAtmegaPinChange.setMask(call HplAtmegaPinChange.getMask() | (1<<pin));
    call HplAtmegaPinChange.enable();
  }

  /* Disables the interrupt */
  async command void AtmegaPinChange.disable[uint8_t pin](){
    uint8_t mask = call HplAtmegaPinChange.getMask() & ~(1<<pin);
    call HplAtmegaPinChange.setMask(mask);
    if(mask==0)
      call HplAtmegaPinChange.disable();
  }

  /* Checks if the interrupt is enabled */
  async command bool AtmegaPinChange.isEnabled[uint8_t pin](){
    return call HplAtmegaPinChange.getMask() & (1<<pin);
  }

  /* Reads the current pin values */
  async command bool AtmegaPinChange.get[uint8_t pin](){
    return call HplAtmegaPinChange.getPins() & (1<<pin);
  }
  
  /* Signalled when any of the enabled pins changed */
  async event void HplAtmegaPinChange.fired(){
    uint8_t mask=call HplAtmegaPinChange.getMask();
    uint8_t pins=call HplAtmegaPinChange.getPins();
    if( mask & (1<<0) )
      //if this were plain c, we could return pins&(1<<0), but tinyos' TRUE is defined as 1
      signal AtmegaPinChange.fired[0]( (pins & (1<<0)) ? TRUE : FALSE );
    if( mask & (1<<1) )
      signal AtmegaPinChange.fired[1]( (pins & (1<<1)) ? TRUE : FALSE );
    if( mask & (1<<2) )
      signal AtmegaPinChange.fired[2]( (pins & (1<<2)) ? TRUE : FALSE );
    if( mask & (1<<3) )
      signal AtmegaPinChange.fired[3]( (pins & (1<<3)) ? TRUE : FALSE );
    if( mask & (1<<4) )
      signal AtmegaPinChange.fired[4]( (pins & (1<<4)) ? TRUE : FALSE );
    if( mask & (1<<5) )
      signal AtmegaPinChange.fired[5]( (pins & (1<<5)) ? TRUE : FALSE );
    if( mask & (1<<6) )
      signal AtmegaPinChange.fired[6]( (pins & (1<<6)) ? TRUE : FALSE );
    if( mask & (1<<7) )
      signal AtmegaPinChange.fired[7]( (pins & (1<<7)) ? TRUE : FALSE );
  }
  
  default async event void AtmegaPinChange.fired[uint8_t pin](bool state) {}
}
