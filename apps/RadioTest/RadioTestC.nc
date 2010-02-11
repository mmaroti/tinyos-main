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

#define  _DEBUG_MODE_

#include "RadioTestCases.h"

#define SET_STATE(s) { state = s; call Leds.set(state); }

#if defined(_DEBUG_MODE_)
  #define P_ENSURE(cond) if(!(cond)){ dbgLINE = __LINE__; }
#else
  #define str(s) s
  #define P_ENSURE(cond) str()
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
    interface LowPowerListening;
  }
}

implementation {

  // State-machine variable
  uint8_t state;
  // BS communication variables
  int8_t  reqidx,reqtype;

  message_t pkt;

  stat_t  stats[MAX_EDGE_COUNT];
  setup_t config;
  edge_t* problem;

  // Bitmask specifying edges on which we should send on timer ticks
  pending_t tTickSendMask;
  // Bitmask specifying edges with pending sending requests
  pending_t pending;

#ifdef _DEBUG_MODE_
  uint32_t dbgLINE;
  stat_t  nullstat;
  pending_t dbgNOTMYEDGES;
#endif

  event void Boot.booted() {
    call AMControl.start();
  }

  event void AMControl.startDone(error_t err) {
    if (err != SUCCESS)
      call AMControl.start();
    else
      SET_STATE( STATE_IDLE )
  }

  event void AMControl.stopDone(error_t err) {
    SET_STATE( STATE_INVALID )
  }

  /* Task : sendPending
   * Processes the 'pending' bitmask and tries to send only the next waiting message.
   */
  task void sendPending() {
    static pending_t  pidx = 0x1;
    static uint8_t    eidx = 0x0;
    testmsg_t* msg;
    am_addr_t  address;
    bool abortSending = FALSE;

    while ( state == STATE_RUNNING && pending && !abortSending ) {
      
      P_ENSURE( (1 << eidx) == pidx )

      // In case we need to send a message on the current edge
      if ( pending & pidx ) {
        
        P_ENSURE( problem[eidx].sender == TOS_NODE_ID )
        
        // Check for resending      
        if ( problem[eidx].lastmsgid == problem[eidx].nextmsgid )
          ++(stats[eidx].resendCount);

        // Compose the new message
        msg = (testmsg_t*)(call Packet.getPayload(&pkt,sizeof(testmsg_t)));
        msg->edgeid = eidx;
        msg->msgid = problem[eidx].nextmsgid;
        address = ( config.flags & USE_DADDR ) ? problem[eidx].receiver : AM_BROADCAST_ADDR;

        // Send out
        switch ( call TxTest.send( address, &pkt, sizeof(testmsg_t)) ) {
          case SUCCESS :
            ++(stats[eidx].sendSuccessCount);
            // This condition guarantees that the round-robin edge
            // incrementation takes place yet this function returns.
            abortSending = TRUE;
            break;
          case EBUSY :
            ++(stats[eidx].sendBusyCount);
            abortSending = TRUE;
            break;
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

    P_ENSURE( (dbgNOTMYEDGES & sbitmask) == 0x0 )

    // Check which edges need to be backlogged
    blogd = pending & sbitmask;

    // Other edges are cleared to set the pending bit
    pending |= sbitmask;

    P_ENSURE( (dbgNOTMYEDGES & pending) == 0x0 )

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
    if ( state == STATE_RUNNING ) {
      SET_STATE( STATE_LASTCHANCE )
      call TestEndTimer.startOneShot(config.lastchance_msec);

    } else if ( state == STATE_LASTCHANCE )
      SET_STATE( STATE_FINISHED )

    call LowPowerListening.setLocalWakeupInterval(0);
  }

  event void TriggerTimer.fired() {
    if ( state == STATE_RUNNING && tTickSendMask )
        setPendingOrBacklog( tTickSendMask );
  }

  event void TxBase.sendDone(message_t* bufPtr, error_t error) { }

  task void sendResponse() {
    responsemsg_t* msg = (responsemsg_t*)(call Packet.getPayload(&pkt,sizeof(responsemsg_t)));

    // RESPONSE the setup acknowledgement if applicable
    if ( reqtype == CTRL_SETUP_SYN ) {
      msg->type = RESP_SETUP_ACK;
      if (state == STATE_SETUP_RCVD &&
          SUCCESS == call TxBase.send(AM_BROADCAST_ADDR, &pkt, sizeof(responsemsg_t)) )
        SET_STATE( STATE_CONFIGURED )
  
    // RESPONSE statistics and final edge states
    } else if ( reqtype == CTRL_DATA_REQ ) {
      SET_STATE( STATE_UPLOADING )

      msg->type = ( reqidx >= MAX_EDGE_COUNT || problem[reqidx].sender == INVALID_NODE )
                    ? RESP_DATA_NEXISTS
                    : RESP_DATA_OK;
      msg->respidx = reqidx;
      if ( msg->type == RESP_DATA_OK )
        msg->payload.stat = stats[reqidx];
      call TxBase.send(AM_BROADCAST_ADDR, &pkt, sizeof(responsemsg_t) );
      SET_STATE( STATE_FINISHED )

    // RESPONSE debug information
    } else if ( reqtype == CTRL_DBG_REQ ) {
      SET_STATE( STATE_UPLOADING )

      msg->type = ( reqidx >= MAX_EDGE_COUNT || problem[reqidx].sender == INVALID_NODE )
                    ? RESP_DATA_NEXISTS
                    : RESP_DBG_OK;
      msg->respidx = reqidx;
      if ( msg->type == RESP_DBG_OK ) {
        msg->payload.debug.endtype = (problem[reqidx].sender == TOS_NODE_ID) ? 1 
                                    : ( (problem[reqidx].receiver == TOS_NODE_ID) ? 2 : 3 );
        msg->payload.debug.nextmsgid = problem[reqidx].nextmsgid;
        msg->payload.debug.lastmsgid = problem[reqidx].lastmsgid;
#ifdef _DEBUG_MODE_
        msg->payload.debug.dbgLINE = dbgLINE;
#else
        msg->payload.debug.dbgLINE = 0;
#endif
      }
      call TxBase.send(AM_BROADCAST_ADDR, &pkt, sizeof(responsemsg_t) );
      SET_STATE( STATE_FINISHED )
    }
  }

  event message_t* RxBase.receive(message_t* bufPtr, void* payload, uint8_t len) {
 
    ctrlmsg_t* msg = (ctrlmsg_t*)payload;
    uint8_t idx, k;
    uint8_t ctype = msg->type;

    // BaseStation RESETs this mote
    // ----------------------------------------------------------------------------------
    if ( ctype == CTRL_RESET && ( state != STATE_RUNNING && 
                                  state != STATE_LASTCHANCE ) ) {

      problem = (edge_t*)NULL;
      memset(stats,0,sizeof(stat_t)*MAX_EDGE_COUNT);
      memset(&config,0,sizeof(setup_t));

      reqidx = reqtype = -1;
      tTickSendMask = pending = 0x0;

#ifdef _DEBUG_MODE_
      dbgLINE = dbgNOTMYEDGES = 0;
#endif

      SET_STATE ( STATE_IDLE )
    
    // BaseStation SETUPs this mote
    // ----------------------------------------------------------------------------------
    } else if ( ctype == CTRL_SETUP && (  state == STATE_INVALID || 
                                          state == STATE_IDLE ) ) {

      // Save the configuration
      config = msg->config;

      // Check config consistency
      P_ENSURE( msg->config.problem_idx < PROBLEMSET_COUNT )
      P_ENSURE( msg->config.runtime_msec != 0 )
      P_ENSURE( config.problem_idx < PROBLEMSET_COUNT )
      P_ENSURE( config.runtime_msec > 0 )
      P_ENSURE( tTickSendMask == 0 )
      P_ENSURE( pending == 0 )
      P_ENSURE( dbgNOTMYEDGES == 0 )

      problem = problemSet[config.problem_idx];
      for( idx = 0, k = 1; problem[idx].sender != INVALID_NODE; ++idx, k<<=1 )
      {
        // Make sure these values are OK
        problem[idx].nextmsgid = 1;
        problem[idx].lastmsgid = 0;

        if( problem[idx].sender != TOS_NODE_ID )
#ifdef _DEBUG_MODE_
          { (dbgNOTMYEDGES |= k); continue; }
#else
          continue;
#endif

        // Set the pending bits if we need to send at start        
        if ( problem[idx].flags & SEND_ON_INIT )
          pending |= k;

        // Set the tTickSendMask if we need to send on timer tick
        if ( problem[idx].flags & SEND_ON_TTICK )
          tTickSendMask |= k;
      }

      // Enable the ACK feature if wanted
      if ( config.flags & USE_ACK )
        call PAck.requestAck(&pkt);
      else
        call PAck.noAck(&pkt);

      // LPL feature MUST NOT be set up here, otherwise there is a risk
      // of missing after-setup acks and START message!

      SET_STATE( STATE_SETUP_RCVD )

    // BaseStation wants to START the test
    // ----------------------------------------------------------------------------------
    } else if ( ctype == CTRL_START && state == STATE_CONFIGURED ) {

      SET_STATE( STATE_RUNNING )
      call TestEndTimer.startOneShot(config.runtime_msec);

      P_ENSURE( ( !(config.flags & USE_LPL) && config.lplwakeupintval == 0 ) ||
                ( (config.flags & USE_LPL) && config.lplwakeupintval > 0 )     )

      // Setup the LPL feature if wanted
      call LowPowerListening.setLocalWakeupInterval(config.lplwakeupintval);

      if ( config.sendtrig_msec > 0 && tTickSendMask )
        call TriggerTimer.startPeriodic(config.sendtrig_msec);

      post sendPending();

    // BaseStation wants SETUP SYNchronization
    // ----------------------------------------------------------------------------------
    } else if ( ctype == CTRL_SETUP_SYN && ( state == STATE_SETUP_RCVD ||
                                             state == STATE_CONFIGURED ) ) {
      reqtype = CTRL_SETUP_SYN;
      post sendResponse();

    // BaseStation REQUESTs statistics
    // ----------------------------------------------------------------------------------
    } else if ( ctype == CTRL_DATA_REQ && ( state == STATE_FINISHED ||
                                            state == STATE_UPLOADING ||
                                            state == STATE_INVALID ) ) {
      reqtype = CTRL_DATA_REQ;
      reqidx = msg->reqidx;
      post sendResponse();

    // BaseStation REQUESTs debug information
    // ----------------------------------------------------------------------------------
    } else if ( ctype == CTRL_DBG_REQ &&  ( state == STATE_FINISHED  ||
                                            state == STATE_UPLOADING ||
                                            state == STATE_INVALID ) ) {
      reqtype = CTRL_DBG_REQ;
      reqidx = msg->reqidx;
      post sendResponse();
    }
    return bufPtr;
  }

  event message_t* RxTest.receive(message_t* bufPtr, void* payload, uint8_t len) {

    testmsg_t* msg = (testmsg_t*)payload;

    // In case the message is sent to this mote
    if (  ( state == STATE_RUNNING || 
            state == STATE_LASTCHANCE ) && 
          problem[msg->edgeid].receiver == TOS_NODE_ID ) {

      P_ENSURE( problem[msg->edgeid].receiver == TOS_NODE_ID )
      P_ENSURE( problem[msg->edgeid].lastmsgid == 0 )

      ++(stats[msg->edgeid].receiveCount);

      // If we got a message with a lower id than expected -> duplicate
      if ( msg->msgid < problem[msg->edgeid].nextmsgid )
        ++(stats[msg->edgeid].duplicateCount);
      
      // If we got a message with a higher id than expected -> we have missed messages
      else if ( msg->msgid > problem[msg->edgeid].nextmsgid ) {
        stats[msg->edgeid].missedCount += msg->msgid - problem[msg->edgeid].nextmsgid;
        problem[msg->edgeid].nextmsgid = msg->msgid+1;
      }
      // Else everything is OK
      else
        ++(problem[msg->edgeid].nextmsgid);
      
      // Check whether we have to send message on receive ( ping-pong case )
      if ( problem[msg->edgeid].pongs ) {
        setPendingOrBacklog( problem[msg->edgeid].pongs );
      }
    }
    return bufPtr;
  }


  event void TxTest.sendDone(message_t* bufPtr, error_t error) {

    testmsg_t* msg = (testmsg_t*)(call Packet.getPayload(bufPtr,sizeof(testmsg_t)));
    // Check whether we have to send message on sendDone
    bool keepPending = (problem[msg->edgeid].flags & SEND_ON_SDONE) != 0;

    if ( state == STATE_RUNNING || state == STATE_LASTCHANCE ) {

      P_ENSURE( problem[msg->edgeid].sender == TOS_NODE_ID )

      if ( error == SUCCESS ) {

        P_ENSURE( pending & (1 << msg->edgeid) )
        P_ENSURE(problem[msg->edgeid].sender == TOS_NODE_ID )
        P_ENSURE( (problem[msg->edgeid].nextmsgid - problem[msg->edgeid].lastmsgid == 1 ) ||
                  (problem[msg->edgeid].nextmsgid - problem[msg->edgeid].lastmsgid == 0 )    )

        ++(stats[msg->edgeid].sendDoneSuccessCount);
        problem[msg->edgeid].lastmsgid = msg->msgid;

        // If ACK is not requested
        if ( (config.flags & USE_ACK) == 0x0 )
          ++(problem[msg->edgeid].nextmsgid);

        // If ACK is requested and received
        else if ( call PAck.wasAcked(bufPtr) ) {
          ++(problem[msg->edgeid].nextmsgid);
          ++(stats[msg->edgeid].wasAckedCount);

        // Else the message is not considered to be sent
        } else {
          ++(stats[msg->edgeid].notAckedCount);
          keepPending = TRUE;
        }

        // Remove the pending bit
        if ( !keepPending )
          atomic { pending &= ~ (1 << msg->edgeid ); }

      } else {
        ++(stats[msg->edgeid].sendDoneFailCount);
      }
      post sendPending();
    }
  }


}
