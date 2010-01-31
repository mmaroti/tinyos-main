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

//#define  USE_TOSSIM  1

#include "RadioTestCases.h"
#ifdef PLATFORM_IRIS
#define POWER2(E) pow(2,(E))
#else
#define POWER2(E) powf(2,(E))
#endif

module RadioTestC @safe() {
  uses {
    interface Boot;
    interface Leds;
    interface Receive as RxBase;
    interface AMSend as TxBase;
    interface Receive as RxTest;
    interface AMSend as TxTest;
    interface SplitControl as AMControl;
    interface Packet;
    interface PacketAcknowledgements as PAck;
    interface AMPacket;
    interface Timer<TMilli> as TestEndTimer;
    interface Timer<TMilli> as TriggerTimer;
  }
}

implementation {

  uint8_t state;
  message_t pkt;
  stat_t  stats[MAX_EDGE_COUNT];
  setup_t config;
  uint8_t statidx;

#ifdef USE_TOSSIM
  uint8_t edgecount;
  edge_t problem[MAX_EDGE_COUNT+1];
#else
  edge_t* problem;
#endif

  // bitmask that specifies on which edges we should send a message
  // when a trigger timer is fired. Must be initialized based on edge
  // descriptions.
  pending_t tTickSendMask;
  pending_t pending;

  event void Boot.booted() {
    call AMControl.start();
  }

  event void AMControl.startDone(error_t err) {
    if (err != SUCCESS)
      call AMControl.start();
    else
      state = STATE_IDLE; call Leds.set(state);
  }

  event void AMControl.stopDone(error_t err) {
    state = STATE_INVALID; call Leds.set(state);
  }

  /* Task : sendPending
   * 
   * Processes the message queue in a round-robin way, and tries to send the next waiting
   * message. According to success or failure, it also collects different statistics.
   */
  task void sendPending() {
    static pending_t  pidx = 0x1;
    static uint8_t    eidx = 0x0;
    testmsg_t* msg;
    am_addr_t  address;
    bool sendsuccess = FALSE;

    while ( state == STATE_RUNNING && pending && !sendsuccess ) {

      // In case we need to send a message on the current edge
      if ( pending & pidx ) {
        
        msg = (testmsg_t*)(call Packet.getPayload(&pkt,sizeof(testmsg_t)));
        msg->edgeid = eidx;
        msg->msgid = problem[eidx].nextmsgid;

        // Check for resending      
        if ( problem[eidx].lastmsgid == problem[eidx].nextmsgid )
          ++(stats[eidx].resendCount);

        address = ( config.flags & USE_DIRECT_ADDR ) ? problem[eidx].receiver : AM_BROADCAST_ADDR;
        switch ( call TxTest.send( address, &pkt, sizeof(testmsg_t)) ) {
          case SUCCESS :
            dbg("Debug","Message sent (edgeid,msgid) = (%d,%d)\n",msg->edgeid,msg->msgid);

            ++(stats[eidx].sendSuccessCount);
            // Remove the pending bit
            pending &= (~pidx);

            // This condition guarantees that in case of a successfull send, the round-robin edge
            // incrementation takes place yet this function returns.
            sendsuccess = TRUE;
            break;
          case EBUSY :
            // In this case, the round-robin edge-incrementation DOES NOT take place!
            return;
          case FAIL :
            ++(stats[eidx].sendFailCount);
            break;
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

  /* Function : setPendingOrBacklog
   * 
   * Modifies the message queue as requested. If identical unsent messages detected,
   * the message is backlogged. It only affects the statistics.
   */
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

  event void TestEndTimer.fired() { 
    state = STATE_FINISHED; call Leds.set(state);
  }

  event void TriggerTimer.fired() {
    if ( state == STATE_RUNNING && tTickSendMask ) {
        dbg("Debug","Trigger!\n");
        setPendingOrBacklog( tTickSendMask );
    }
  }

  event void TxBase.sendDone(message_t* bufPtr, error_t error) { }

  task void sendSetupAck() {
    ctrlmsg_t* msg = (ctrlmsg_t*)(call Packet.getPayload(&pkt,sizeof(ctrlmsg_t)));
    msg->type = CTRL_SETUP_ACK;
    if (  state == STATE_SETUP_RCVD && 
          call TxBase.send(AM_BROADCAST_ADDR, &pkt, sizeof(ctrlmsg_t)) == SUCCESS ) {
      state = STATE_CONFIGURED; call Leds.set(state);
    }
  }

  task void sendStat() {
    ctrlmsg_t* msg = (ctrlmsg_t*)(call Packet.getPayload(&pkt,sizeof(ctrlmsg_t)));
    state = STATE_UPLOADING; call Leds.set(state);
    msg->type = ( statidx >= MAX_EDGE_COUNT || 
                    problem[statidx].sender == 0 ) ? CTRL_STAT_NEXISTS : CTRL_STAT_OK;
    msg->data.stat.statidx = statidx;
    if ( msg->type == CTRL_STAT_OK )
      msg->data.stat.statpayload = stats[statidx];

    dbg("Debug","%s\n", ( statidx >= MAX_EDGE_COUNT || problem[statidx].sender == 0 ) ? "CTRL_STAT_NEXISTS" : "CTRL_STAT_OK"); dbgstat(msg->data.stat.statpayload);

    call TxBase.send(AM_BROADCAST_ADDR, &pkt, sizeof(ctrlmsg_t) );
    state = STATE_FINISHED; call Leds.set(state);
  }

  event message_t* RxBase.receive(message_t* bufPtr, void* payload, uint8_t len) {
 
    ctrlmsg_t* msg = (ctrlmsg_t*)payload;
    uint8_t idx = 0, k = 1;
    uint8_t ctype = msg->type;

    // BaseStation RESETs this mote
    // ----------------------------------------------------------------------------------
    if ( ( state != STATE_RUNNING && state != STATE_UPLOADING ) && ctype == CTRL_RESET ) {
      dbg("Debug","CTRL_RESET received! Resetting...\n");
#ifdef USE_TOSSIM
      edgecount = 0;
#else
      problem = 0;
#endif
      memset(stats,0,sizeof(stat_t)*MAX_EDGE_COUNT);
      state = STATE_IDLE; call Leds.set(state);
     
    // BaseStation SETUPs this mote
    // ----------------------------------------------------------------------------------
    } else if ( ( state == STATE_INVALID || 
                  state == STATE_IDLE || 
                  state == STATE_SETUP_RCVD ||
                  state == STATE_CONFIGURED ) && ctype == CTRL_SETUP ) {
      dbg("Debug","CTRL_SETUP received.\n");
      config = msg->data.config;
      
      // Check config consistency
      if ( config.problem_idx > PROBLEMSET_COUNT ) {
        dbg("Debug","Invalid problem idx received : %d\n",config.problem_idx);
        state = STATE_INVALID; call Leds.set(state);
        return bufPtr;   
      }

      // Enable the ACK feature if wanted
      if ( config.flags & USE_ACK )
        call PAck.requestAck(&pkt);
      else
        call PAck.noAck(&pkt);

      // Setup the LPL feature if wanted
      // TODO!

#ifdef USE_TOSSIM
      call AMPacket.setSource(&pkt,TOS_NODE_ID);

      // Dump the current problem to a separate memory location
      while ( problemSet[config.problem_idx][idx].sender != 0 ) {
        problem[idx] = problemSet[config.problem_idx][idx];
        ++idx;
      }
      problem[idx] = problemSet[config.problem_idx][idx];
#else
      problem = problemSet[config.problem_idx];
#endif    

      for( idx = 0; problem[idx].sender > 0; ++idx, k<<=1 )
      {
        if( problem[idx].sender != TOS_NODE_ID )
          continue;
     
        // Set the pending bits if we need to send at start        
        if ( problem[idx].flags & SEND_ON_INIT )
          pending |= k;

        // Set the tTickSendMask if we need to send on timer tick
        if ( problem[idx].flags & SEND_ON_TTICK )
          tTickSendMask |= k;
      }
#ifdef USE_TOSSIM
      edgecount = idx;
      dbgpset(problem);
#endif
      state = STATE_SETUP_RCVD; call Leds.set(state);

    // BaseStation wants SETUP SYNchronization
    // ----------------------------------------------------------------------------------
    } else if ( state == STATE_SETUP_RCVD && ctype == CTRL_SETUP_SYN ) {
      dbg("Debug","CTRL_SETUP_SYN received.\n");
      post sendSetupAck();

    // BaseStation wants to START the test
    // ----------------------------------------------------------------------------------
    } else if ( state == STATE_CONFIGURED && ctype == CTRL_START ) {
      dbg("Debug","CTRL_START received.\n");
      state = STATE_RUNNING; call Leds.set(state);
      call TestEndTimer.startOneShot(config.runtime_msec);

      if ( config.sendtrig_msec > 0 && tTickSendMask > 0 )
        call TriggerTimer.startPeriodic(config.sendtrig_msec);

      post sendPending();

    // BaseStation REQUESTs statistics
    // ----------------------------------------------------------------------------------
    } else if ( state == STATE_FINISHED && ctype == CTRL_STAT_REQ ) {
      dbg("Debug","CTRL_REQ_STAT received for stat : %d\n", msg->data.stat.statidx);
      statidx = msg->data.stat.statidx;
      post sendStat();
    }
    return bufPtr;
  }

  event message_t* RxTest.receive(message_t* bufPtr, void* payload, uint8_t len) {

    testmsg_t* msg = (testmsg_t*)payload;

    // In case the message is sent to this mote
    if (  state == STATE_RUNNING && 
          problem[msg->edgeid].receiver == TOS_NODE_ID ) {
      dbg("Debug","Message received (edgeid,msgid) = (%d,%d)\n",msg->edgeid,msg->msgid);
      ++(stats[msg->edgeid].receiveCount);

      // If we got a message with a lower id than expected -> duplicate
      if ( msg->msgid < problem[msg->edgeid].nextmsgid )
        ++(stats[msg->edgeid].duplicateReceiveCount);
      
      // If we got a message with a higher id than expected -> we have missed messages
      else if ( msg->msgid > problem[msg->edgeid].nextmsgid ) {
        stats[msg->edgeid].missedCount += msg->msgid - problem[msg->edgeid].nextmsgid;
        problem[msg->edgeid].nextmsgid = msg->msgid+1;
      }
      // Else everything is OK
      else
        ++(problem[msg->edgeid].nextmsgid);
      
      // Check whether we have to send message on receive ( PONG_ON_PING case )
      if ( problem[msg->edgeid].pongs ) {
        setPendingOrBacklog( problem[msg->edgeid].pongs );
      }
    }
    return bufPtr;
  }


  event void TxTest.sendDone(message_t* bufPtr, error_t error) {

    testmsg_t* msg = (testmsg_t*)(call Packet.getPayload(bufPtr,sizeof(testmsg_t)));
    bool  acked = 0;

    if ( state != STATE_RUNNING )
      return;

    if ( error == SUCCESS ) {
      
      ++(stats[msg->edgeid].sendDoneSuccessCount);
      problem[msg->edgeid].lastmsgid = msg->msgid;

#ifdef USE_TOSSIM
      dbg("Debug","Message sent SUCCESSFULLY (edgeid,msgid) = (%d,%d)\n",msg->edgeid,msg->msgid);
      // If using TOSSIM, acknowledgements doesn't work, we must bypass it with a random value.
      acked = (rand()%10 < 3);
      if ( config.flags & USE_ACK )
        dbg("Debug","Message acked : %s\n",acked ? "yes" : "no");
#else
      acked = call PAck.wasAcked(bufPtr);
#endif
           
      // Message is COMPLETELY sent
      if ( acked || !( config.flags & USE_ACK ) ) {
        problem[msg->edgeid].nextmsgid = msg->msgid+1;

        // If message is ACKed, update the stats
        if ( acked && ( config.flags & USE_ACK ) )
          ++(stats[msg->edgeid].wasAckedCount);
  
      // If message is NOT considered COMPLETE resend it.
      } else {
        setPendingOrBacklog( POWER2(msg->edgeid) );
      }

    } else {
      ++(stats[msg->edgeid].sendDoneFailCount);
    }

    // Check whether we have to send message on sendDone
    if ( problem[msg->edgeid].flags & SEND_ON_SDONE )
      setPendingOrBacklog( POWER2(msg->edgeid) );

    post sendPending();
  }
}
