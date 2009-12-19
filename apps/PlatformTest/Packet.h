/** Copyright (c) 2009, University of Szeged
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
* Author: Zoltan Kincses
*/

#ifndef PACKET_H
#define PACKET_H
 
 enum {
   AM_CONTROLPACKET = 1,
   AM_DATAINT8=2,
   AM_DATAUINT8=3,
   AM_DATAUINT16=4,
 };

 typedef nx_struct controlpacket{
	nx_uint8_t nodeID;
	nx_uint8_t instr[2];
 }controlpacket_t;

 typedef nx_struct dataInt8{
	nx_uint8_t dataType;
	nx_uint8_t senderNodeID;
	nx_uint8_t receiverNodeID;
	nx_uint32_t sampleCnt;
    nx_int8_t min;
	nx_int8_t max;
	nx_int32_t sum_a;
	nx_int32_t sum_e;
}dataInt8_t;

typedef nx_struct dataUint8{
	nx_uint8_t dataType;
	nx_uint8_t senderNodeID;
	nx_uint8_t receiverNodeID;
	nx_uint32_t sampleCnt;
    nx_uint8_t min;
	nx_uint8_t max;
	nx_uint32_t sum_a;
	nx_uint32_t sum_e;
}dataUint8_t;

typedef nx_struct dataUint16{
	nx_uint8_t dataType;
	nx_uint8_t senderNodeID;
	nx_uint8_t receiverNodeID;
	nx_uint32_t sampleCnt;
    nx_uint16_t min;
	nx_uint16_t max;
	nx_uint32_t sum_a;
	nx_uint32_t sum_e;
}dataUint16_t;

#endif

