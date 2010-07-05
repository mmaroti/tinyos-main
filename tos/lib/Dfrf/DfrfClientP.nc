/*
 * Copyright (c) 2009, Vanderbilt University
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the author appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE VANDERBILT UNIVERSITY BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE VANDERBILT
 * UNIVERSITY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE VANDERBILT UNIVERSITY SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE VANDERBILT UNIVERSITY HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Author: Janos Sallai
 */

#include "Dfrf.h"

generic module DfrfClientP(uint8_t payloadlength, uint8_t uniqueLength, uint16_t bufferSize) {
  provides {
    interface StdControl;
    interface DfrfSend;
    interface DfrfReceive;
  }
  uses {
    interface DfrfControl as SubDfrfControl;
    interface DfrfSend as SubDfrfSend;
    interface DfrfReceive as SubDfrfReceive;
  }
} implementation {

  uint8_t routingBuffer[sizeof(dfrf_desc_t) + bufferSize * (payloadlength + sizeof(dfrf_block_t))];

  command error_t StdControl.start() {
    return call SubDfrfControl.init(payloadlength, uniqueLength, routingBuffer, sizeof(routingBuffer));
  }

  command error_t StdControl.stop() {
    call SubDfrfControl.stop();
    return SUCCESS;
  }

  command error_t DfrfSend.send(void* data) {
    return call SubDfrfSend.send((uint8_t*)data);
  }

  event bool SubDfrfReceive.receive(void *data) {
    return signal DfrfReceive.receive(data);
  }

  default event bool DfrfReceive.receive(void* data) { return TRUE; }

}

