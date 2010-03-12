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
* Author: Veress Krisztian
*         veresskrisztian@gmail.com
*/

#include "GIDTest.h"
 
module GIDTestC @safe() {
  uses {
    interface Leds;
    interface Boot;
    interface Receive;
    interface AMSend;
    interface Timer<TMilli> as MilliTimer;
    interface SplitControl as AMControl;
    interface AMPacket;
    interface Packet;
  }
}
implementation {

  message_t packet;
  bool locked;
  uint8_t group;
   
  event void Boot.booted() {
    call AMControl.start();
  }

  event void AMControl.startDone(error_t err) {
    if (err != SUCCESS )
      call AMControl.start();

    if (TOS_NODE_ID == SENDER_ID) {
      call MilliTimer.startPeriodic(1000);
      group = 0;
      locked = FALSE;
    }
  }

  event void AMControl.stopDone(error_t err) {  }
  
  event void MilliTimer.fired() {
    if (locked) {
      return;
    }
    else {
      gidtest_msg* msg = (gidtest_msg*)call Packet.getPayload(&packet, sizeof(gidtest_msg));
      msg->cnt = (group++) % 7 + 1;
      call AMPacket.setGroup(&packet,msg->cnt);
      if (call AMSend.send(AM_BROADCAST_ADDR, &packet, sizeof(gidtest_msg)) == SUCCESS) {
      	locked = TRUE;
        call Leds.led0On();
      }
    }
  }

  event message_t* Receive.receive(message_t* bufPtr, void* payload, uint8_t len) {
    if (len != sizeof(gidtest_msg)) {return bufPtr;}
    else {
      if ( TOS_NODE_ID == GIDDISPLAY_ID )
        call Leds.set(call AMPacket.group(bufPtr));
      else if ( TOS_NODE_ID == CNTDISPLAY_ID )
        call Leds.set(((gidtest_msg*)payload)->cnt);

      return bufPtr;
    }
  }

  event void AMSend.sendDone(message_t* bufPtr, error_t error) { 
    if ( &packet == bufPtr ) {
      call Leds.led0Off();
      locked = FALSE;
    }
  }

}




