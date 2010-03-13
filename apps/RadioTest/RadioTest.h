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

#define MAX_EDGE_COUNT 6
#define MAX_NODE_COUNT 3

#if MAX_EDGE_COUNT <= 8
  typedef uint8_t pending_t;
#elif MAX_EDGE_COUNT <= 16
  typedef uint16_t pending_t;
#elif MAX_EDGE_COUNT <= 32
  typedef uint32_t pending_t;
#elif MAX_EDGE_COUNT <= 64
  typedef uint64_t pending_t;
#else
  #error "MAX_EDGE_COUNT is set too high! The current limit is 64!"
#endif

enum {
  // Mote states
  STATE_INVALID     = 0x0,
  STATE_IDLE        = 0x1,
  STATE_SETUP_RCVD  = 0x2,
  STATE_CONFIGURED  = 0x3,
  STATE_RUNNING     = 0x4,
  STATE_LASTCHANCE  = 0x5,
  STATE_FINISHED    = 0x6,
  STATE_UPLOADING   = 0x7,

  // Policy flags
  USE_ACK           = 0x1,
  USE_DADDR         = 0x2,
  USE_LPL           = 0x4,

  // AM Type identifiers
  AM_CTRLMSG_T      = 101,
  AM_TESTMSG_T      = 102,
  AM_RESPONSEMSG_T  = 103,

  // Control / Response message types
  CTRL_SETUP        = 0,
  CTRL_SETUP_SYN    = 1,
  RESP_SETUP_ACK    = 2,

  CTRL_START        = 10,
  CTRL_RESET        = 20,

  CTRL_DATA_REQ     = 30,
  RESP_DATA_OK      = 31,

  CTRL_DBG_REQ      = 40,
  RESP_DBG_OK       = 41,

  RESP_DATA_NEXISTS  = 50,

  UNLOCKED          = 0,
  LOCKED            = 1
};

// Edge type
typedef struct edge_t {
  uint8_t       sender;           // Sender end of the edge
  uint8_t       receiver;         // Receiver end of the edge
  uint8_t       flags;            // Sending policies
  uint16_t      nextmsgid;        // The message id to send (on send side)/expected to receive (on receive side)
  pending_t     pongs;            // In ping-pong policy the edge bitmask on which we should reply on receive
} edge_t;


// Setup type
typedef nx_struct setup_t {
  nx_uint8_t    problem_idx;      // The problem we should test
  nx_uint32_t   runtime_msec;     // How long should we run the test?
  nx_uint32_t   timer_msec;       // How often we should send a message when sending on timer ticks ?
  nx_uint32_t   lastchance_msec;  // How long should we wait for reception after the running time has elapsed ?
  nx_uint8_t    flags;            // Global flags ( such as ACK, LPL, ... )
  nx_uint16_t   lplwakeupintval;  // Low Power Listening wakeup interval
} setup_t;


// Base type
typedef nx_uint16_t stat_base_t;

// Stats type
typedef nx_struct stat_t {
  stat_base_t   triggerCount;
  stat_base_t   backlogCount;
  stat_base_t   resendCount;

  stat_base_t   sendCount;
  stat_base_t   sendSuccessCount;
  stat_base_t   sendFailCount;

  stat_base_t   sendDoneCount;
  stat_base_t   sendDoneSuccessCount;
  stat_base_t   sendDoneFailCount;

  stat_base_t   wasAckedCount;
  stat_base_t   notAckedCount;

  stat_base_t   receiveCount;
  stat_base_t   expectedCount;
  stat_base_t   duplicateCount;
  stat_base_t   forwardCount;
  stat_base_t   missedCount;
  stat_base_t   wrongCount;

  nx_uint8_t    remainedCount;
} stat_t;


typedef nx_struct testmsg_t {
  nx_uint8_t    edgeid;           // On which edge this message is intended to propagate through
  nx_uint16_t   msgid;            // The auto-increment id of the message on the preset edge
} testmsg_t;

typedef nx_struct ctrlmsg_t {
  nx_uint8_t    type;             // Control type
  nx_uint8_t    reqidx;           // The requested stat-edge pair index in the requesting stage
  setup_t       config;           // The config in the setup stage
} ctrlmsg_t;

typedef nx_struct responsemsg_t {
  nx_uint8_t    type;             // Response type
  nx_uint8_t    respidx;          // The requested stat-edge pair index in the requesting stage
  nx_union {
    stat_t      stat;             // The requested stat structure in the requesting stage
    nx_struct {
      nx_uint8_t    endtype;      //  1 : Sender / 2 : Receiver / 3: Nothing
      nx_uint16_t   nextmsgid;    // The final value of nextmsgid on the requested edge.
      nx_uint16_t   dbgLINE;      // Assertion LINE value if any assertion failed.
    }           debug;
  } payload;
} responsemsg_t;

#endif
