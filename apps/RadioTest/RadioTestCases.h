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

// Sending flags
enum {
  SEND_ON_INIT = 0x01,
  SEND_ON_SDONE = 0x02,
  SEND_ON_TTICK = 0x04
};

enum {
  USE_ACK = 0x01,
  USE_LPL = 0x02
};

/* 1. 
* Type          : Send from TriggerTimer
* Mote Count    : 2
* Sending Motes : 1->2
*/

/* 2.
* Type          : Send from TriggerTimer
* Mote Count    : 2
* Sending Motes : 1->2,2->1
*
*/

/* 3.
* Type          : Send from TriggerTimer
* Mote Count    : 3
* Sending Motes : 1->2,2->3,3->1
*
*/

/* 3.
* Type          : Send from TriggerTimer
* Mote Count    : 3
* Sending Motes : 1->2,1->3,2->
*
*/
/* 2.
* Type          : Send from sendDone
* Mote Count    : 2
* Sending Motes : 1
*
*/

#endif
