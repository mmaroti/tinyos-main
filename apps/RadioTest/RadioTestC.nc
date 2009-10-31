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

#include "RadioTestCases.h"
#include <assert.h>

// Only for testing purposes, will be controlled by the basestation
// in the near future.
#define  PROBLEM_IDX 5

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

  uint8_t state;
  setup_t config;
  message_t pkt;

  stat_t  stats[MAX_EDGE_COUNT];
  uint8_t edgecount;

  // bitmask that specifies on which edges we should send a message
  // when a trigger timer is fired. Must be initialized at Setup based on edge
  // descriptions.
  pending_t tTickSendMask;
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
        msg->msgid = problemSet[PROBLEM_IDX][eidx].nextmsgid;

        // Check for resending      
        if ( problemSet[PROBLEM_IDX][eidx].lastmsgid == problemSet[PROBLEM_IDX][eidx].nextmsgid )
          ++(stats[eidx].resendCount);

        if ( call AMSend.send( AM_BROADCAST_ADDR, &pkt, sizeof(testmsg_t)) == SUCCESS ) {
          dbg("Debug","sendPending() Send SUCCESS on edge %d with msgid : %d\n",msg->edgeid,msg->msgid);
          ++(stats[eidx].sendSuccessCount);

          // Remove the pending bit if backlog is zero, or decrement the backlog
          pending &= (~pidx);

          // Quit because only one message is allowed to be on air
          // Consequent messages would surely fail, don't include them in the statistics
          return;
        } else {
          dbg("Debug","sendPending() Send FAIL\n");
          ++(stats[eidx].sendFailCount);
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

    // Setup the pending bits
    // We can have different outgoing edges with different flags
    for( i = 0; problemSet[PROBLEM_IDX][i].sender <= MAX_NODE_COUNT; ++i, k<<=1 )
    {
      /*dbg("Debug","(%d,%d,%d,%d,%d,%d)\n",
        problemSet[PROBLEM_IDX][i].sender,
        problemSet[PROBLEM_IDX][i].receiver,
        problemSet[PROBLEM_IDX][i].flags,
        problemSet[PROBLEM_IDX][i].nextmsgid,
        problemSet[PROBLEM_IDX][i].lastmsgid,
        problemSet[PROBLEM_IDX][i].pongs);
      */

      if( problemSet[PROBLEM_IDX][i].sender != TOS_NODE_ID )
        continue;
     
      // Set the pending bits if we need to send at start        
      if ( problemSet[PROBLEM_IDX][i].flags & SEND_ON_INIT )
        pending |= k;

      // Set the tTickSendMask if we need to send on timer tick
      if ( problemSet[PROBLEM_IDX][i].flags & SEND_ON_TTICK )
        tTickSendMask |= k;
    }
    edgecount = i;

    dbg("Debug","%d's initial pending : ",TOS_NODE_ID); dbgbin(pending);
    dbg("Debug","%d's initial tTickSendMask : ",TOS_NODE_ID); dbgbin(tTickSendMask);

    if ( config.sendtrig_msec > 0 && tTickSendMask > 0 )
      call TriggerTimer.startPeriodic(config.sendtrig_msec);

    // Now we are running
    state = STATE_RUNNING;
    call Timer.startOneShot(config.runtime_msec);
    post sendPending();
  }
 
  void setPendingOrBacklog(pending_t sbitmask) {
    uint8_t i = 0;    
    pending_t blogd;

    // Check which edges need to be backlogged
    blogd = pending & sbitmask;

    // Other edges are cleared to set the pending bit
    pending |= sbitmask;

    // Backlog the previously selected edges -> only update stats
    while ( blogd ) {
      if ( blogd & 0x1 )
        ++(stats[i].wouldBacklogCount);
      ++i;
      blogd >>=1;
    }
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
  }

  event void Timer.fired() { 
    uint8_t i;
    state = STATE_IDLE;
    for ( i = 0; i < edgecount; ++i )
      dbgstat(stats[i]);
  }

  event void TriggerTimer.fired() {
    // Check whether we have to send message on timer tick
    if ( state == STATE_RUNNING && tTickSendMask ) {
        dbg("Debug","TriggerTimer fired --> ");
        setPendingOrBacklog( tTickSendMask );
    }
  }

  event message_t* Receive.receive(message_t* bufPtr, void* payload, uint8_t len) {

    testmsg_t* msg = (testmsg_t*)payload;

    // In case the message is sent to this mote
    if (  state == STATE_RUNNING && 
          problemSet[PROBLEM_IDX][msg->edgeid].receiver == TOS_NODE_ID ) {

      dbg("Debug","Message received on edge %d with msgid : %d. Expected id is %d\n",msg->edgeid,msg->msgid,problemSet[PROBLEM_IDX][msg->edgeid].nextmsgid);
      
      // If we got a message with a lower id than expected -> duplicate
      if ( msg->msgid < problemSet[PROBLEM_IDX][msg->edgeid].nextmsgid )
        ++(stats[msg->edgeid].duplicateReceiveCount);
      
      // If we got a message with a higher id than expected -> we have missed messages
      else if ( msg->msgid > problemSet[PROBLEM_IDX][msg->edgeid].nextmsgid ) {
        stats[msg->edgeid].missedCount += msg->msgid - problemSet[PROBLEM_IDX][msg->edgeid].nextmsgid;
        problemSet[PROBLEM_IDX][msg->edgeid].nextmsgid = msg->msgid+1;
      }
      // Else everything is OK
      else {
        ++(problemSet[PROBLEM_IDX][msg->edgeid].nextmsgid);
        ++(stats[msg->edgeid].receiveCount);
      }
      
      // Check whether we have to send message on receive ( PONG_ON_PING case )
      if ( problemSet[PROBLEM_IDX][msg->edgeid].pongs )
        setPendingOrBacklog( problemSet[PROBLEM_IDX][msg->edgeid].pongs );

    }
    return bufPtr;
  }

  event void AMSend.sendDone(message_t* bufPtr, error_t error) {

    testmsg_t* msg = (testmsg_t*)(call Packet.getPayload(bufPtr,sizeof(testmsg_t)));
    bool  acked = 0;

    if ( state != STATE_RUNNING )
      return;

    // In case of successfull send
    if ( error == SUCCESS ) {
      ++(stats[msg->edgeid].sendDoneSuccessCount);
      problemSet[PROBLEM_IDX][msg->edgeid].lastmsgid = msg->msgid;

      // If message is considered COMPLETELY sent according to config.flags ( BCAST, ACK ),
      // increment the message counter
      acked = call PAck.wasAcked(bufPtr);
      if ( acked || !( config.flags & USE_ACK ) ) {
        dbg("Debug","Message sent on edge %d with msgid %d is COMPLETE!\n",msg->edgeid,msg->msgid);
        problemSet[PROBLEM_IDX][msg->edgeid].nextmsgid = msg->msgid+1;

        // If message is ACKed, update the stats
        if ( acked )
          ++(stats[msg->edgeid].wasAckedCount);
  
      // If message is NOT considered COMPLETE resend it.
      } else {
        // Resend is done by setting a new pending "job", and not incrementing the message counter.
        // This way when configuring the packet to be sent with AMSend.send, we can detect that the
        // message counter is not incremented, and update the resend count statistics.
        dbg("Debug","Message sent on edge %d with msgid %d is INCOMPLETE!\n",msg->edgeid,msg->msgid);
        setPendingOrBacklog( pow(2,msg->edgeid) );
      }

    // In case of unsuccessfull send
    } else {
      dbg("Debug","Message sending failed on edge %d with msgid %d!\n",msg->edgeid,msg->msgid);
      ++(stats[msg->edgeid].sendDoneFailCount);
    }

    // Check whether we have to send message on sendDone
    if ( problemSet[PROBLEM_IDX][msg->edgeid].flags & SEND_ON_SDONE )
     setPendingOrBacklog( pow(2,msg->edgeid) );

    // Re-post the queue processing task
    post sendPending();
  }
}




