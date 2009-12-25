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

#define MAX_EDGE_COUNT 8
#define MAX_NODE_COUNT 3

#if MAX_EDGE_COUNT <= 8
  typedef nx_uint8_t nx_pending_t;
  typedef uint8_t pending_t;
#elif MAX_EDGE_COUNT <= 16
  typedef nx_uint16_t nx_pending_t;
  typedef uint16_t pending_t;
#elif MAX_EDGE_COUNT <= 32
  typedef nx_uint32_t nx_pending_t;
  typedef uint32_t pending_t;
#elif MAX_EDGE_COUNT <= 64
  typedef nx_uint64_t nx_pending_t;
  typedef uint64_t pending_t;
#else
  #error "MAX_EDGE_COUNT is set too high! The current limit is 64!"
#endif

enum {
  // Mote states
  STATE_RADIOOFF = 0x0,
  STATE_INVALID = 0x1,
  STATE_IDLE = 0x2,
  STATE_RUNNING = 0x4,
  STATE_FINISHED = 0x6,
  STATE_UPLOADING = 0x7,

  // Policy flags
  USE_ACK = 0x01,
  USE_LPL = 0x02,
  USE_DIRECT_ADDR = 0x03,

  // AM Type identifiers
  AM_CTRLMSG_T = 101,
  AM_TESTMSG_T = 102,
  AM_SETUP_T   = 103,
  AM_STAT_T    = 104,

  // Control message types
  CTRL_SETUP = 0,
  CTRL_REQ_STAT = 1,
  CTRL_UPL_STAT = 2,
  CTRL_UPL_END = 3,
  CTRL_RESET = 4
};

// Edge type
typedef struct edge_t {
  uint8_t    sender;       // Sender end of the edge
  uint8_t    receiver;     // Receiver end of the edge
  uint8_t    flags;        // Sending policies
  uint32_t   nextmsgid;    // The message id to send ( on send side ) / expected to receive ( on receive side )
  uint32_t   lastmsgid;    // The last message id sent ( on send side ). On receive side it is unused.
  pending_t  pongs;        // In ping-pong policy the edge bitmask on which we should reply on receive
} edge_t;


// Setup type
typedef nx_struct setup_t {
  nx_uint8_t  problem_idx;    // The problem we should test
  nx_uint32_t runtime_msec;   // How long should we run the test?
  nx_uint32_t sendtrig_msec;  // How often we should send a message when sending on timer ticks ?
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

  stat_base_t receiveCount;
  stat_base_t duplicateReceiveCount;
  stat_base_t missedCount;

  stat_base_t wouldBacklogCount;
} stat_t;


typedef nx_struct testmsg_t {
  nx_uint8_t  edgeid;       // On which edge this message is intended to propagate through
  nx_uint32_t msgid;        // The auto-increment id of the message on the preset edge
} testmsg_t;

typedef nx_struct ctrlmsg_t {
  nx_uint8_t  type;         // Control type
  nx_uint8_t  idx;          // The index of the sent stat ( when a mote provides data to the basestation )
  nx_union {
    setup_t     config;     // The config ( basestation->mote )
    stat_t      stat;       // The stat ( mote->basestation )
  } data;
} ctrlmsg_t;

#ifdef USE_TOSSIM
void dbgbin(pending_t data)
{
  pending_t bit = 1 << (sizeof(pending_t)*8-1);
  char c[sizeof(pending_t)*8+1];
  uint8_t i = 0;
  while( bit ) {
    c[i++] = ( data & bit ) ? '1' : '0';
    bit >>= 1;
  }
  c[sizeof(pending_t)*8] = '\0';
  dbg("Debug","%s\n",c);
}

void dbgstat(stat_t s) {
  dbg("Debug","stat.sendSuccessCount     : %d\n",s.sendSuccessCount);
  dbg("Debug","stat.sendFailCount        : %d\n",s.sendFailCount);
  dbg("Debug","stat.sendDoneSuccessCount : %d\n",s.sendDoneSuccessCount);
  dbg("Debug","stat.sendDoneFailCount    : %d\n",s.sendDoneFailCount);
  dbg("Debug","stat.wasAckedCount        : %d\n",s.wasAckedCount);
  dbg("Debug","stat.resendCount          : %d\n",s.resendCount);
  dbg("Debug","stat.receiveCount         : %d\n",s.receiveCount);
  dbg("Debug","stat.duplicateReceiveCount: %d\n",s.duplicateReceiveCount);
  dbg("Debug","stat.missedCount          : %d\n",s.missedCount);
  dbg("Debug","stat.wouldBacklogCount    : %d\n",s.wouldBacklogCount);
  dbg("Debug","---------------------------------------------\n");
}
#endif

#endif
