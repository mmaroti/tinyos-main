/*
* Copyright (c) 2009, University of Szeged
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

#include "RadioTest.h"
#include <assert.h>

module RadioTestC @safe() {
  uses {
    interface Boot;
    interface Receive;
    interface AMSend;
    interface SplitControl as AMControl;
    interface Packet;
    interface PacketAcknowledgements as PAck;
    interface AMPacket;
    interface Timer<TMilli> as Timer;
    interface Timer<TMilli> as TriggerTimer;
  }
}

implementation {

  // the current state of the mote
  uint8_t state;
  
  // radio test config and message packet
  setup_t config;
  message_t pkt;

  // edge descriptions
  edge_t  edges[EDGE_COUNT];

  // bitmask that specifies on which edges we should send a message
  // when a trigger timer is fired. Must be initialized at Setup based on edge
  // descriptions.
  pending_t tTickSendMask;

  // backlog array and pending bitmask
  uint8_t backlog[EDGE_COUNT];  
  pending_t pending;

  task void sendPending() {
    static pending_t  pidx = 0x1;
    static uint8_t    eidx = 0x0;

    while ( state == STATE_RUNNING && pending ) {
      assert(pidx);
      // In case we need to send a message on the current edge
      if ( pending & pidx ) {

        testmsg_t* msg = (testmsg_t*)(call Packet.getPayload(&pkt,sizeof(testmsg_t)));
        msg->edgeid = eidx;
        msg->msgid = edges[eidx].lastmsgid+1;
  
        if ( call AMSend.send( AM_BROADCAST_ADDR, &pkt, sizeof(testmsg_t)) == SUCCESS ) {
          dbg("Debug","sendPending() Send SUCCESS\n");
          ++(edges[eidx].stats.sendSuccessCount);

          // Remove the pending bit if backlog is zero, or decrement the backlog
          atomic {
            if ( backlog[eidx] > 0 )
              --backlog[eidx];
            else
              pending &= (!pidx);
          }

        } else {
          dbg("Debug","sendPending() Send FAIL\n");
          ++(edges[eidx].stats.sendFailCount);
        }
      }
      // Move to the next edge
      pidx <<= 1;
      ++eidx;
      if ( pidx == 0x0 ) {
        pidx = 0x1;
        eidx = 0x0;
      }
    }
  }

  task void doSetup() {
    uint8_t i = 0, k = 1;

    // Setup config variable
    config.runtime_msec = 1000;
    config.usebcast = TRUE;
    config.flags = 0;
    config.sendtrig_msec = 100;

    // Needed for TOSSIM simulation
    call AMPacket.setSource(&pkt,TOS_NODE_ID);

    // Setup the graph
    edges[0].sender = 1;
    edges[0].receiver = 2;
    edges[0].flags = SEND_ON_INIT | SEND_ON_SDONE;

    edges[1].sender = 2;
    edges[1].receiver = 1;
    edges[1].flags = SEND_ON_INIT | SEND_ON_SDONE;

   
    // Setup the pending bits
    // We can have different outgoing edges with different flags
    for ( i = 0; i < EDGE_COUNT; ++i, k <<= 1 )
    {
      if( edges[i].sender != TOS_NODE_ID )
        continue;

      // Set the pending bits if we need to send at start        
      if ( edges[i].flags & SEND_ON_INIT )
        pending |= k;

      // Set the tTickSendMask if we need to send on timer tick
      if ( edges[i].flags & SEND_ON_TTICK )
        tTickSendMask |= k;
    }

    dbg("Debug","%d's initial pending : ",TOS_NODE_ID); dbgbin(pending);
    dbg("Debug","%d's initial tTickSendMask : ",TOS_NODE_ID); dbgbin(tTickSendMask);

    if ( config.sendtrig_msec > 0 && tTickSendMask > 0 )
      call TriggerTimer.startPeriodic(config.sendtrig_msec);

    // Now we are running
    state = STATE_RUNNING;
    call Timer.startOneShot(config.runtime_msec);
 //   dbg("Debug","Started\n");
    post sendPending();
  }
 
  void setPendingOrBacklog(pending_t sbitmask) {
    uint8_t i = 0;    
    pending_t blogd;

    dbg("Debug","setPendingOrBacklog() : "); dbgbin(sbitmask);

    // The following is in atomic to prevent race conditions
    atomic {
      // Check which edges need to be backlogged
      blogd = pending & sbitmask;

      // Other edges are cleared to set the pending bit
      pending |= (pending ^ sbitmask);
    }

    // Backlog the previously selected edges
    while ( blogd ) {
      if ( blogd & 0x1 )
        ++(backlog[i]);
      ++i;
      blogd >>=1;
    }
    dbg("Debug","setPendingOrBacklog() pending: "); dbgbin(pending);
    post sendPending();
  }



  event void Boot.booted() {
    call AMControl.start();
  }

  event void AMControl.startDone(error_t err) {
    if (err == SUCCESS)
      post doSetup();
    else
      call AMControl.start();
  }

  event void AMControl.stopDone(error_t err) {
    state = STATE_INVALID;
    call AMControl.start();
  }

  event void Timer.fired() { state = STATE_IDLE; }

  event void TriggerTimer.fired() {
    // Check whether we have to send message on timer tick
    if ( state == STATE_RUNNING ) {
        dbg("Debug","TriggerTimer fired --> ");
        setPendingOrBacklog( tTickSendMask );
    }
  }

  event message_t* Receive.receive(message_t* bufPtr, void* payload, uint8_t len) {

    testmsg_t* msg = (testmsg_t*)payload;

    // In case the message is sent to this mote
    if ( state == STATE_RUNNING && edges[msg->edgeid].receiver == TOS_NODE_ID ) {

      dbg("Debug","Message received.\n");
      
      // If we got a message with a lower id than expected -> duplicate
      if ( msg->msgid <= edges[msg->edgeid].lastmsgid )
        ++(edges[msg->edgeid].stats.duplicateReceiveCount);
      
      // If we got a message with a higher id than expected -> we have missed messages
      else if ( msg->msgid > edges[msg->edgeid].lastmsgid+1 )
        ++(edges[msg->edgeid].stats.missedCount);
      
      // Else everything is OK
      else {
        edges[msg->edgeid].lastmsgid = msg->msgid;
        ++(edges[msg->edgeid].stats.receiveCount);
      }
      
      // Check whether we have to send message on receive
      if ( edges[msg->edgeid].flags & PONG_ON_PING )
        setPendingOrBacklog( edges[msg->edgeid].pongs );

    }
    return bufPtr;
  }

  event void AMSend.sendDone(message_t* bufPtr, error_t error) {

    testmsg_t* msg = (testmsg_t*)(call Packet.getPayload(bufPtr,sizeof(testmsg_t)));
    if ( state != STATE_RUNNING )
      return;

    // In case of successfull send
    if ( error == SUCCESS ) {
      ++(edges[msg->edgeid].stats.sendDoneSuccessCount);

      // If message is ACKed, or we don't need ACK, update the stats and increment the message counter
      if ( call PAck.wasAcked(bufPtr) || !( config.flags & USE_ACK ) ) {
        dbg("Debug","Message sent successfully!\n");

        ++(edges[msg->edgeid].stats.wasAckedCount);
        ++(edges[msg->edgeid].lastmsgid); 
        
        // Check whether we have to send message on sendDone
        if ( edges[msg->edgeid].flags & SEND_ON_SDONE )
          setPendingOrBacklog( pow(2,msg->edgeid) );

      // If Message NOT ACKed, resend it.
      } else {
        // Resend is done by setting a new pending "job", and not incrementing the message counter.
        // This way the next sent message's id remains the actual one, so the next sent message is a resent one.
        // That also allows the receive end to check only the messageid, and no need to maintain an array.
        dbg("Debug","Message sent is not ACKed!\n");

        setPendingOrBacklog( pow(2,msg->edgeid) );
        ++(edges[msg->edgeid].stats.resendCount);
      }

    // In case of unsuccessfull send
    } else {
      dbg("Debug","Message sent UNsuccessfully!\n");
      ++(edges[msg->edgeid].stats.sendDoneFailCount);
    }
  }
}




