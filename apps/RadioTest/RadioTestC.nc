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
    interface Timer<TMilli> as Timer;
    interface Timer<TMilli> as TriggerTimer;
  }
}

implementation {

  uint8_t state;
  message_t pkt;
  stat_t  stats[MAX_EDGE_COUNT];
  setup_t config;

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
    state = STATE_RADIOOFF; call Leds.set(state);
  }

  /* Task : sendPending
   * 
   * Processes the owned message queue in a round-robin way, and tries to send the
   * messages. According to success or failure, it also collects different statistics.
   */
  task void sendPending() {
    static pending_t  pidx = 0x1;
    static uint8_t    eidx = 0x0;
    static testmsg_t* msg;
    static am_addr_t  address;

    while ( state == STATE_RUNNING && pending ) {

      // In case we need to send a message on the current edge
      if ( pending & pidx ) {
        
        msg = (testmsg_t*)(call Packet.getPayload(&pkt,sizeof(testmsg_t)));
        msg->edgeid = eidx;
        msg->msgid = problem[eidx].nextmsgid;

        // Check for resending      
        if ( problem[eidx].lastmsgid == problem[eidx].nextmsgid )
          ++(stats[eidx].resendCount);

        // Set the destination
        address = ( config.flags & USE_DIRECT_ADDR ) ? problem[eidx].receiver : AM_BROADCAST_ADDR;
        call AMPacket.setDestination(&pkt, address); 

        // SUCCESSFULL Send
        if ( call TxTest.send( address, &pkt, sizeof(testmsg_t)) == SUCCESS ) {
          ++(stats[eidx].sendSuccessCount);
#ifdef USE_TOSSIM
          dbg("Debug","Message sent (edgeid,msgid) = (%d,%d)\n",msg->edgeid,msg->msgid);
#endif
          // Remove the pending bit
          pending &= (~pidx);
          return;

        // UNSUCCESSFULL Send
        } else {
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

  /* Simulation END */
  event void Timer.fired() { 
    state = STATE_FINISHED; call Leds.set(state);
  }

  event void TriggerTimer.fired() {
    if ( state == STATE_RUNNING && tTickSendMask ) {
#ifdef USE_TOSSIM
        dbg("Debug","Trigger!\n");
#endif
        setPendingOrBacklog( tTickSendMask );
    }
  }

  void sendStat(uint8_t statidx) {
    ctrlmsg_t* msg;
    if ( state != STATE_UPLOADING )
      return;

    // Indicate the end of stats
    if ( statidx >= MAX_EDGE_COUNT ) {
      msg = (ctrlmsg_t*)(call Packet.getPayload(&pkt,sizeof(ctrlmsg_t)));
      msg->type = CTRL_UPL_END;
      call TxBase.send(AM_BROADCAST_ADDR, &pkt, sizeof(ctrlmsg_t));
    }
    // If that edge doesn't belong to us try the next one
    if ( problem[statidx].sender != TOS_NODE_ID &&
         problem[statidx].receiver != TOS_NODE_ID )
      sendStat(statidx+1);
    // Else send the stat
    else {
#ifdef USE_TOSSIM
      dbg("Debug","Uploading stat : %d\n", statidx);
#endif
      msg = (ctrlmsg_t*)(call Packet.getPayload(&pkt,sizeof(ctrlmsg_t)));
      msg->type = CTRL_UPL_STAT;
      msg->idx = statidx;
      msg->data.stat = stats[statidx];
      if ( SUCCESS != call TxBase.send(AM_BROADCAST_ADDR, &pkt, sizeof(ctrlmsg_t) ) )
        sendStat(statidx);
    }
  }

  event void TxBase.sendDone(message_t* bufPtr, error_t error) { 
    ctrlmsg_t* msg = (ctrlmsg_t*)(call Packet.getPayload(bufPtr,sizeof(ctrlmsg_t)));
    if ( error == SUCCESS && msg->type != CTRL_UPL_END )
      sendStat(++msg->idx);
    else
      sendStat(msg->idx);
  }

  event message_t* RxBase.receive(message_t* bufPtr, void* payload, uint8_t len) {
 
    ctrlmsg_t* msg = (ctrlmsg_t*)payload;
    uint8_t i = 0, k = 1;
    uint8_t ctype = msg->type;

    // BaseStation RESETs this mote
    if ( ctype == CTRL_RESET ) {
#ifdef USE_TOSSIM
      edgecount = 0;
#else
      problem = 0;
#endif
      memset(stats,0,sizeof(stat_t)*MAX_EDGE_COUNT);
      state = STATE_IDLE; call Leds.set(state);

    // BaseStation REQUESTS statistics
    } else if ( ( state == STATE_FINISHED || state == STATE_UPLOADING ) && ctype == CTRL_REQ_STAT ) {
      state = STATE_UPLOADING; call Leds.set(state);

#ifdef USE_TOSSIM
      dbg("Debug","Upload request received : %d\n", msg->idx);
#endif
      call PAck.noAck(&pkt);
      sendStat(msg->idx);
            
    // BaseStation SETUPs this mote
    } else if ( (state == STATE_IDLE || state == STATE_INVALID ) && ctype == CTRL_SETUP ) {
#ifdef USE_TOSSIM
      dbg("Debug","Config received.\n");
#endif
      config = msg->data.config;
      if ( config.problem_idx > PROBLEMSET_COUNT ) {
#ifdef USE_TOSSIM
        dbg("Debug","Invalid problem idx received : %d\n",config.problem_idx);
#endif
        state = STATE_INVALID; call Leds.set(state);
        return bufPtr;   
      }

      // Enable the ACK feature on the message if wanted
      if ( config.flags & USE_ACK )
        call PAck.requestAck(&pkt);

#ifdef USE_TOSSIM
      call AMPacket.setSource(&pkt,TOS_NODE_ID);

      // Dump the current problem to a separate memory location
      while ( problemSet[config.problem_idx][i].sender != MAX_NODE_COUNT+1 ) {
        problem[i] = problemSet[config.problem_idx][i];
        ++i;
      }
      problem[i] = problemSet[config.problem_idx][i];
#else
      problem = problemSet[config.problem_idx];
#endif    

      for( i = 0; problem[i].sender <= MAX_NODE_COUNT; ++i, k<<=1 )
      {
        if( problem[i].sender != TOS_NODE_ID )
          continue;
     
        // Set the pending bits if we need to send at start        
        if ( problem[i].flags & SEND_ON_INIT )
          pending |= k;

        // Set the tTickSendMask if we need to send on timer tick
        if ( problem[i].flags & SEND_ON_TTICK )
          tTickSendMask |= k;
      }
#ifdef USE_TOSSIM
      edgecount = i;
#endif

      // Now we are running
      state = STATE_RUNNING; call Leds.set(state);
      call Timer.startOneShot(config.runtime_msec);

      if ( config.sendtrig_msec > 0 && tTickSendMask > 0 )
        call TriggerTimer.startPeriodic(config.sendtrig_msec);

      post sendPending();
    }
    return bufPtr;
  }

  event message_t* RxTest.receive(message_t* bufPtr, void* payload, uint8_t len) {

    testmsg_t* msg = (testmsg_t*)payload;

    // In case the message is sent to this mote
    if (  state == STATE_RUNNING && 
          problem[msg->edgeid].receiver == TOS_NODE_ID ) {
#ifdef USE_TOSSIM
      dbg("Debug","Message received (edgeid,msgid) = (%d,%d)\n",msg->edgeid,msg->msgid);
#endif      
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
      if ( problem[msg->edgeid].pongs )
        setPendingOrBacklog( problem[msg->edgeid].pongs );

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
        setPendingOrBacklog( powf(2,msg->edgeid) );
      }

    } else {
      ++(stats[msg->edgeid].sendDoneFailCount);
    }

    // Check whether we have to send message on sendDone
    if ( problem[msg->edgeid].flags & SEND_ON_SDONE )
      setPendingOrBacklog( powf(2,msg->edgeid) );

    post sendPending();
  }
}
