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

#ifndef RADIO_TEST_H
#define RADIO_TEST_H

#define EDGE_COUNT 2

#if EDGE_COUNT <= 8
  typedef nx_uint8_t nx_pending_t;
  typedef uint8_t pending_t;
  #define BITSIZE 8
#elif EDGE_COUNT <= 16
  typedef nx_uint16_t nx_pending_t;
  typedef uint16_t pending_t;
  #define BITSIZE 16
#elif EDGE_COUNT <= 32
  typedef nx_uint32_t nx_pending_t;
  typedef uint32_t pending_t;
  #define BITSIZE 32
#elif EDGE_COUNT <= 64
  typedef nx_uint64_t nx_pending_t;
  typedef uint64_t pending_t;
  #define BITSIZE 64
#else
  #error "EDGE_COUNT is set too high! The current limit is 64!"
#endif


#ifdef BITSIZE
void dbgbin(pending_t data)
{
  pending_t bit = 1 << (BITSIZE-1);
  char c[BITSIZE+1];
  uint8_t i = 0;
  while( bit ) {
    c[i++] = ( data & bit ) ? '1' : '0';
    bit >>= 1;
  }
  c[BITSIZE] = '\0';
  dbg("Debug","%s\n",c);
}
#endif



// Flags
enum {
  AM_RTEST = 101,

  USE_ACK = 0x01,
  USE_LPL = 0x02,

  SEND_ON_INIT = 0x01,
  SEND_ON_SDONE = 0x02,
  SEND_ON_TTICK = 0x04,
  PONG_ON_PING = 0x08

};

enum {
  STATE_INVALID = 0,
  STATE_IDLE,
  STATE_SETUP,
  STATE_SETUP_IDLE,
  STATE_SETUP_RDY,
  STATE_RUNNING,
  STATE_FINISHED,
  STATE_DPROVIDE
};

// Setup type
typedef nx_struct setup_t {
  nx_uint32_t runtime_msec;   // How long should we run the test?
  nx_uint32_t sendtrig_msec;  // How often we should send a message when sending on timer ticks ?
  nx_uint8_t  usebcast;       // Should we use broadcasting ?
  nx_uint8_t  flags;          // Global flags ( such as ACK, LPL, ... )
} setup_t;

// Stats base type
typedef nx_uint16_t stat_base_t;

// Stats type
typedef nx_struct stat_t {
  stat_base_t sendSuccessCount;
  stat_base_t sendFailCount;
  stat_base_t sendDoneSuccessCount;
  stat_base_t sendDoneFailCount;
  stat_base_t wasAckedCount;
  stat_base_t resendCount;
  stat_base_t alreadyPendingCount;
  stat_base_t receiveCount;
  stat_base_t duplicateReceiveCount;
  stat_base_t missedCount;
} stat_t;

// Edge type
typedef nx_struct edge_t {
  nx_uint8_t    sender;       // Sender end of the edge
  nx_uint8_t    receiver;     // Receiver end of the edge
  nx_uint8_t    flags;        // Sending policies
  nx_pending_t  pongs;        // In ping-pong policy the edge bitmask on which we should reply on receive
  nx_uint32_t   lastmsgid;    // The last message id sent ( on send side ) / received ( on receive side )
  nx_uint8_t    msgsize;      // The message size we want to set on this edge
  stat_t        stats;        // The statistics collected on this particular edge
} edge_t;

typedef nx_struct testmsg_t {
  nx_uint8_t  edgeid;       // On which edge this message is intended to propagate through
  nx_uint32_t msgid;        // The auto-increment id of the message on the preset edge
} testmsg_t;

#endif
