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

#ifndef RADIO_TEST_CASES_H
#define RADIO_TEST_CASES_H

#include "RadioTest.h"

#define RT_PROBLEM_NEXT },{
#define RT_NULL_EDGE {((MAX_NODE_COUNT)+1),((MAX_NODE_COUNT)+1),0,0,0,0}

// Sending flags
enum {
  SEND_OFF  = 0x0,
  SEND_ON_INIT = 0x01,
  SEND_ON_SDONE = 0x02,
  SEND_ON_TTICK = 0x04
};

edge_t problemSet[][(MAX_EDGE_COUNT+1)] = { {

/** TRIGGER PROBLEMS -------------------------------------- */

/* 0. 
* Type          : Send from TriggerTimer
* Mote Count    : 2
* Sending Motes : 1->2
*/
  { 1, 2, SEND_ON_TTICK, 1, 0, 0 }, 
  RT_NULL_EDGE 

/* 1.
* Type          : Send from TriggerTimer
* Mote Count    : 2
* Sending Motes : 1->2,2->1
*/
RT_PROBLEM_NEXT
  {1, 2, SEND_ON_TTICK, 1, 0, 0 },
  {2, 1, SEND_ON_TTICK, 1, 0, 0 },
  RT_NULL_EDGE

/* 2.
* Type          : Send from TriggerTimer
* Mote Count    : 3
* Sending Motes : 1->2,2->3,3->1
*
*/
RT_PROBLEM_NEXT
  {1, 2, SEND_ON_TTICK, 1, 0, 0 },
  {2, 3, SEND_ON_TTICK, 1, 0, 0 },
  {3, 1, SEND_ON_TTICK, 1, 0, 0 },
  RT_NULL_EDGE

/* 3.
* Type          : Send from TriggerTimer
* Mote Count    : 3
* Sending Motes : 1->2,1->3,2->1,2->3,3->1,3->2
*
*/
RT_PROBLEM_NEXT
  {1, 2, SEND_ON_TTICK, 1, 0, 0 },
  {1, 3, SEND_ON_TTICK, 1, 0, 0 },
  {2, 1, SEND_ON_TTICK, 1, 0, 0 },
  {2, 3, SEND_ON_TTICK, 1, 0, 0 },
  {3, 1, SEND_ON_TTICK, 1, 0, 0 },
  {3, 2, SEND_ON_TTICK, 1, 0, 0 },
  RT_NULL_EDGE

/** MAXIMAL THROUGHPUT PROBLEMS -------------------------------------- */

/* 4. 
* Type          : Send from sendDone
* Mote Count    : 2
* Sending Motes : 1->2
*/
RT_PROBLEM_NEXT
  {1, 2, SEND_ON_SDONE | SEND_ON_INIT, 1, 0, 0 },
  RT_NULL_EDGE

/* 5.
* Type          : Send from sendDone
* Mote Count    : 2
* Sending Motes : 1->2,2->1
*/
RT_PROBLEM_NEXT
  {1, 2, SEND_ON_SDONE | SEND_ON_INIT, 1, 0, 0 },
  {2, 1, SEND_ON_SDONE | SEND_ON_INIT, 1, 0, 0 },
  RT_NULL_EDGE

/* 6.
* Type          : Send from sendDone
* Mote Count    : 3
* Sending Motes : 1->2,2->3,3->1
*/
RT_PROBLEM_NEXT
  {1, 2, SEND_ON_SDONE | SEND_ON_INIT, 1, 0, 0 },
  {2, 3, SEND_ON_SDONE | SEND_ON_INIT, 1, 0, 0 },
  {3, 1, SEND_ON_SDONE | SEND_ON_INIT, 1, 0, 0 },
  RT_NULL_EDGE

/* 7.
* Type          : Send from sendDone
* Mote Count    : 3
* Sending Motes : 1->2,1->3,2->1,2->3,3->1,3->2
*/
RT_PROBLEM_NEXT
  {1, 2, SEND_ON_SDONE | SEND_ON_INIT, 1, 0, 0 },
  {1, 3, SEND_ON_SDONE | SEND_ON_INIT, 1, 0, 0 },
  {2, 1, SEND_ON_SDONE | SEND_ON_INIT, 1, 0, 0 },
  {2, 3, SEND_ON_SDONE | SEND_ON_INIT, 1, 0, 0 },
  {3, 1, SEND_ON_SDONE | SEND_ON_INIT, 1, 0, 0 },
  {3, 2, SEND_ON_SDONE | SEND_ON_INIT, 1, 0, 0 },
  RT_NULL_EDGE

/** PING-PONG PROBLEMS -------------------------------------- */
/* Note :
  The 'pongs' flag of an edge say e1 defines the edgeset E on which 
  we should send message upon receiving on e1.
  The E set must conform to the edge descriptions. A zeroed 'pongs' is
  considered as 'do Nothing'. The high bits of pongs are parsed and E 
  is set accordingly. If the ith bit is high, E must contain the (i-1)th
  edge.

  If pongs = 0x1, than E = { 0 }
     pongs = 0x17 than E = { 0, 4 }
     pongs = 0x7  than E = { 0, 1 ,2 }
  ...
*/


/* 8. 
* Type          : Send from Receive.receive
* Mote Count    : 2
* Sending Motes : 1->2,2->1
* Description   : A 2 element ping-pong, the initiator is Mote1.
*/
RT_PROBLEM_NEXT
  {1, 2, SEND_ON_INIT , 0x2, 1, 0 },
  {2, 1, SEND_OFF     , 0x1, 1, 0 },
  RT_NULL_EDGE

/* 9. 
* Type          : Send from Receive.receive
* Mote Count    : 3
* Sending Motes : 1->2,2->3,3->1
* Description   : A 3 element circle ping-pong. Mote1 initiates to Mote2,
*                 then Mote2 passes the "ball" to Mote3,
*                 Mote3 passes back to Mote1, and continued.
*/
RT_PROBLEM_NEXT
  {1, 2, SEND_ON_INIT , 0x2, 1, 0 },
  {2, 3, SEND_OFF     , 0x4, 1, 0 },
  {3, 1, SEND_OFF     , 0x1, 1, 0 },
  RT_NULL_EDGE

/* 10. 
* Type          : Send from Receive.receive
* Mote Count    : 3
* Sending Motes : 1->2,1->3,2->1,2->3,3->1,3->2
* Description   : A 3 element simultaneous pairwise ping-pong using 3 "balls".
*/
RT_PROBLEM_NEXT
  {1, 2, SEND_ON_INIT , 0x16, 1, 0 },
  {2, 3, SEND_ON_INIT , 0x32, 1, 0 },
  {3, 1, SEND_ON_INIT , 0x8 , 1, 0 },
  {1, 3, SEND_OFF     , 0x4 , 1, 0 },
  {2, 1, SEND_OFF     , 0x1 , 1, 0 },
  {3, 2, SEND_OFF     , 0x2 , 1, 0 },
  RT_NULL_EDGE

}}; // problemSet END

#endif
