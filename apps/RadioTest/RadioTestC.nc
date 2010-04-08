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
  #define _ASSERT_(cond) if(!(cond || dbgLINE)){ dbgLINE = __LINE__; }
#else
  #define str(s) s
  #define _ASSERT_(cond) str()
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
  uint8_t state, sendlock;
  // BS communication variables
  int8_t  reqidx,reqtype;

  message_t pkt,bpkt;
  stat_t  stats[MAX_EDGE_COUNT];
  setup_t config;
  edge_t* problem;

  // Bitmask specifying edges on which we should send on timer ticks
  pending_t tTickSendMask;
  // Bitmask specifying edges with pending send requests
  pending_t pending;

#ifdef _DEBUG_MODE_
  uint32_t dbgLINE;
  pending_t dbgNOTMYEDGES;
#endif

  // Needed for sendPending
  uint8_t  eidx = 0x0;
  uint8_t  pidx = 0x1;

  event void Boot.booted() {
    SET_STATE( STATE_INVALID )
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

  event void TestEndTimer.fired() {
    uint8_t i = 0;
    _ASSERT_( state == STATE_RUNNING || state == STATE_LASTCHANCE )

    if ( state == STATE_RUNNING ) {
      // need to stop the timer to ensure that it only triggers in STATE_RUNNING
      call TriggerTimer.stop();
      call LowPowerListening.setLocalWakeupInterval(0);
      call LowPowerListening.setRemoteWakeupInterval(&pkt,0);

      SET_STATE( STATE_LASTCHANCE )      
      call TestEndTimer.startOneShot(config.lastchance_msec);

    } else if ( state == STATE_LASTCHANCE ) {
      SET_STATE( STATE_FINISHED )

      _ASSERT_( sendlock == UNLOCKED )

      // compute the remained statistic
      while ( pending ) {
        if ( pending & 0x1 )
          ++(stats[i].remainedCount);
        ++i;
        pending >>= 1;
      }
    }  
  }

  /* Task : sendPending
   * Processes the 'pending' bitmask and tries to send only the next waiting message.
   */
  task void sendPending() {
    testmsg_t*      msg;
    am_addr_t       address;
    // local pending variable: needed for avoiding critical sections
    // and un-fair sending service which occured when interrupted by setPendingOrBacklog
    pending_t       lpending = pending;

    _ASSERT_( sendlock == UNLOCKED );

    // In case we have any chance to send
    if ( state == STATE_RUNNING && lpending ) {

      // find the LSB and set pidx,eidx accordingly
      while ( !(lpending & pidx) ) {
        ++eidx;
        pidx <<= 1;
        if ( pidx == 0x0 ) {
          eidx = 0x0;
          pidx = 0x1;
        }
      }
      
      _ASSERT_( (1 << eidx) == pidx );
      _ASSERT_( problem[eidx].sender == TOS_NODE_ID );
      _ASSERT_( pending == lpending )
      _ASSERT_( sendlock == UNLOCKED )
        
      // Compose the new message
      msg = (testmsg_t*)(call Packet.getPayload(&pkt,sizeof(testmsg_t)));
      msg->edgeid = eidx;
      msg->msgid = problem[eidx].nextmsgid;
      address = ( config.flags & USE_DADDR ) ? problem[eidx].receiver : AM_BROADCAST_ADDR;

      call LowPowerListening.setRemoteWakeupInterval(&pkt,config.lplwakeupintval);
      // Send out
      switch ( call TxTest.send( address, &pkt, sizeof(testmsg_t)) ) {
        case SUCCESS :
          ++(stats[eidx].sendSuccessCount);
          sendlock = LOCKED;
          break;
        case FAIL :
          ++(stats[eidx].sendFailCount);
          ++(stats[eidx].resendCount);
          break;
        default :
          _ASSERT_( 0 );
          break;
      }
      ++(stats[eidx].sendCount);
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
    
    _ASSERT_( (dbgNOTMYEDGES & sbitmask) == 0x0 )
  
    // if nothing to set or state is not STATE_RUNNING, return
    if ( sbitmask == 0 || state != STATE_RUNNING )
      return;

    atomic {
      // Check which edges need to be backlogged
      blogd = pending & sbitmask;

      // Other edges are cleared to set the pending bit
      pending |= sbitmask;

      _ASSERT_( (dbgNOTMYEDGES & pending) == 0x0 )
    }

    // Count backlog values
    while ( blogd ) {
      if ( blogd & 0x1 )
        ++(stats[i].backlogCount);
      ++i;
      blogd >>=1;
    }
    // Count trigger values
    i = 0;
    while ( sbitmask ) {
      if ( sbitmask & 0x1 )
        ++(stats[i].triggerCount);
      ++i;
      sbitmask >>=1;
    }
    // Since pending is now not zero for sure, post sendPending
    if ( sendlock == UNLOCKED )
      post sendPending();
  }

  // ------------------------------------------------------------------------------------
  // BASE STATION COMMUNICATION
  // ------------------------------------------------------------------------------------

  task void sendResponse() {
    responsemsg_t* msg = (responsemsg_t*)(call Packet.getPayload(&bpkt,sizeof(responsemsg_t)));

    // RESPONSE the setup acknowledgement if applicable
    if ( reqtype == CTRL_SETUP_SYN ) {
      msg->type = (state == STATE_SETUP_RCVD || state == STATE_CONFIGURED )
                  ? RESP_SETUP_ACK
                  : RESP_SETUP_NOACK;
  
    // RESPONSE statistics and final edge states
    } else if ( reqtype == CTRL_DATA_REQ ) {

      msg->type = ( reqidx >= MAX_EDGE_COUNT || problem[reqidx].sender == INVALID_NODE )
                    ? RESP_DATA_NEXISTS
                    : RESP_DATA_OK;
      msg->respidx = reqidx;
      if ( msg->type == RESP_DATA_OK )
        msg->payload.stat = stats[reqidx];

    // RESPONSE debug information
    } else if ( reqtype == CTRL_DBG_REQ ) {

      msg->type = ( reqidx >= MAX_EDGE_COUNT || problem[reqidx].sender == INVALID_NODE )
                    ? RESP_DATA_NEXISTS
                    : RESP_DBG_OK;
      msg->respidx = reqidx;
      if ( msg->type == RESP_DBG_OK ) {
        msg->payload.debug.endtype = (problem[reqidx].sender == TOS_NODE_ID) ? 1
                                    : ( (problem[reqidx].receiver == TOS_NODE_ID) ? 2 : 3 );
        msg->payload.debug.nextmsgid = problem[reqidx].nextmsgid;
#ifdef _DEBUG_MODE_
        msg->payload.debug.dbgLINE = dbgLINE;
#else
        msg->payload.debug.dbgLINE = 0;
#endif
      }
    }

    // send the response message
    _ASSERT_( sendlock == UNLOCKED )
    if ( SUCCESS == call TxBase.send(AM_BROADCAST_ADDR, &bpkt, sizeof(responsemsg_t)) )
      sendlock = LOCKED;
  }


  event void TxBase.sendDone(message_t* bufPtr, error_t error) {
    _ASSERT_( sendlock == LOCKED )
    sendlock = UNLOCKED;
    if ( SUCCESS == error && state == STATE_SETUP_RCVD )
        SET_STATE( STATE_CONFIGURED )
  }

  event message_t* RxBase.receive(message_t* bufPtr, void* payload, uint8_t len) {
 
    ctrlmsg_t* msg = (ctrlmsg_t*)payload;
    uint8_t idx, k;
    uint8_t ctype = msg->type;

    // BaseStation RESETs this mote
    // ----------------------------------------------------------------------------------
    if ( ctype == CTRL_RESET ) {

      problem = (edge_t*)NULL;
      memset(stats,0,sizeof(stat_t)*MAX_EDGE_COUNT);
      memset(&config,0,sizeof(setup_t));

      reqidx = reqtype = -1;
      tTickSendMask = pending = 0x0;

      eidx = 0x0;
      pidx = 0x1;
      sendlock = UNLOCKED;

#ifdef _DEBUG_MODE_
      dbgLINE = dbgNOTMYEDGES = 0;
#endif

      call TriggerTimer.stop();
      call TestEndTimer.stop();
      
      call LowPowerListening.setLocalWakeupInterval(0);
      call LowPowerListening.setRemoteWakeupInterval(&pkt,0);

      SET_STATE ( STATE_IDLE )
    
    // BaseStation SETUPs this mote
    // ----------------------------------------------------------------------------------
    } else if ( ctype == CTRL_SETUP ) {
        
      // Save the configuration
      config = msg->config;

      // Check config consistency
      _ASSERT_( state == STATE_IDLE )
      _ASSERT_( msg->config.problem_idx < PROBLEMSET_COUNT );
      _ASSERT_( msg->config.runtime_msec != 0 );
      _ASSERT_( config.problem_idx < PROBLEMSET_COUNT );
      _ASSERT_( config.runtime_msec > 0 );
      _ASSERT_( tTickSendMask == 0 );
      _ASSERT_( pending == 0 );
      _ASSERT_( sendlock == UNLOCKED );
      _ASSERT_( dbgNOTMYEDGES == 0 );

      problem = problemSet[config.problem_idx];
      for( idx = 0, k = 1; problem[idx].sender != INVALID_NODE; ++idx, k<<=1 )
      {
        // Make sure these values are OK
        problem[idx].nextmsgid = 1;

        if( problem[idx].sender != TOS_NODE_ID )
#ifdef _DEBUG_MODE_
          { (dbgNOTMYEDGES |= k); continue; }
#else
          continue;
#endif

        // Set the pending bits if we need to send at start        
        if ( problem[idx].flags & SEND_ON_INIT ) {
          pending |= k;
          ++(stats[idx].triggerCount);
        }

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
    } else if ( ctype == CTRL_START ) {

      _ASSERT_( state == STATE_CONFIGURED )
      _ASSERT_( ( !(config.flags & USE_LPL) && config.lplwakeupintval == 0 ) ||
                ( (config.flags & USE_LPL) && config.lplwakeupintval > 0 )     )

      SET_STATE( STATE_RUNNING )

      // Setup the LPL feature if wanted
      call LowPowerListening.setLocalWakeupInterval(config.lplwakeupintval);
      call LowPowerListening.setRemoteWakeupInterval(&pkt,config.lplwakeupintval);

      if ( config.timer_msec > 0 && tTickSendMask )
        call TriggerTimer.startPeriodic(config.timer_msec);

      call TestEndTimer.startOneShot(config.runtime_msec);

      // If we send initial message
      if ( pending )
        post sendPending();

    // BaseStation wants SETUP SYNchronization
    // ----------------------------------------------------------------------------------
    } else if ( ctype == CTRL_SETUP_SYN ) {
      _ASSERT_( state == STATE_SETUP_RCVD || state == STATE_CONFIGURED )

      reqtype = ctype;
      post sendResponse();

    // BaseStation REQUESTs statistics
    // ----------------------------------------------------------------------------------
    } else if ( ctype == CTRL_DATA_REQ ) {
      _ASSERT_( state == STATE_FINISHED )
        
      reqtype = ctype;
      reqidx = msg->reqidx;
      post sendResponse();

    // BaseStation REQUESTs debug information
    // ----------------------------------------------------------------------------------
    } else if ( ctype == CTRL_DBG_REQ ) {
      _ASSERT_( state == STATE_FINISHED )

      reqtype = ctype;
      reqidx = msg->reqidx;
      post sendResponse();
    }
    return bufPtr;
  }

  // ------------------------------------------------------------------------------------
  // RADIO TEST COMMUNICATION
  // ------------------------------------------------------------------------------------

  event void TriggerTimer.fired() {
    setPendingOrBacklog( tTickSendMask );
  }

  event message_t* RxTest.receive(message_t* bufPtr, void* payload, uint8_t len) {

    testmsg_t* msg = (testmsg_t*)payload;
    _ASSERT_( state == STATE_RUNNING || state == STATE_LASTCHANCE )

    // In case the message is sent to this mote
    if (  ( state == STATE_RUNNING || 
            state == STATE_LASTCHANCE ) && 
            problem[msg->edgeid].receiver == TOS_NODE_ID ) {

      _ASSERT_( problem[msg->edgeid].receiver == TOS_NODE_ID )

      ++(stats[msg->edgeid].receiveCount);

      // If the message id is ok
      if ( msg->msgid == problem[msg->edgeid].nextmsgid ) {
        ++(stats[msg->edgeid].expectedCount);

      } else {
        ++(stats[msg->edgeid].wrongCount);
        
        // If we got a message with a lower id than expected -> duplicate
        if ( msg->msgid < problem[msg->edgeid].nextmsgid )
          ++(stats[msg->edgeid].duplicateCount);
        // If we got a message with a higher id than expected -> we have missed messages
        else {
          ++(stats[msg->edgeid].forwardCount);
          stats[msg->edgeid].missedCount += msg->msgid - problem[msg->edgeid].nextmsgid;
        }
      }

      // Set the next expected message id
      problem[msg->edgeid].nextmsgid = msg->msgid + 1;

      // Register new requests in ping-pong case
      setPendingOrBacklog( problem[msg->edgeid].pongs );
    }
    return bufPtr;
  }


  event void TxTest.sendDone(message_t* bufPtr, error_t error) {

    testmsg_t* msg = (testmsg_t*)(call Packet.getPayload(bufPtr,sizeof(testmsg_t)));
    bool clearPending = TRUE, isResend = FALSE;
    sendlock = UNLOCKED;

    _ASSERT_( state == STATE_RUNNING || state == STATE_LASTCHANCE )

    if ( state == STATE_RUNNING || state == STATE_LASTCHANCE ) {

      _ASSERT_( problem[msg->edgeid].sender == TOS_NODE_ID )
      _ASSERT_( pending & (1 << msg->edgeid) )

      ++(stats[msg->edgeid].sendDoneCount);

      if ( error == SUCCESS ) {
        ++(stats[msg->edgeid].sendDoneSuccessCount);

        // If ACK is not requested
        if ( (config.flags & USE_ACK) == 0x0 ) {
          ++(problem[msg->edgeid].nextmsgid);

        // If ACK is requested and received
        } else if ( call PAck.wasAcked(bufPtr) ) {
          ++(problem[msg->edgeid].nextmsgid);
          ++(stats[msg->edgeid].wasAckedCount);

        // Else the message is not considered to be sent
        } else {
          ++(stats[msg->edgeid].notAckedCount);
          ++(stats[msg->edgeid].resendCount);
          clearPending = FALSE;
          isResend = TRUE;
        }

      } else {
        ++(stats[msg->edgeid].sendDoneFailCount);
        ++(stats[msg->edgeid].resendCount);
        clearPending = FALSE;
        isResend = TRUE;
      }

      // Check whether we have to send message on sendDone
      if ( problem[msg->edgeid].flags & SEND_ON_SDONE ) {
        if ( !isResend )
          ++(stats[msg->edgeid].triggerCount);
        clearPending = FALSE;
      }
      // Remove the pending bit if applicable
      if ( clearPending )
        atomic { pending &= ~ (1 << msg->edgeid ); }

      // Re-post the sendPending
      _ASSERT_ ( sendlock == UNLOCKED )
      post sendPending();
    }
  } // end TxTest.sendDone


}
