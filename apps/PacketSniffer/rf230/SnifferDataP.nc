/*
* Copyright (c) 2010, University of Szeged
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


module SnifferDataP @safe() {
  provides interface SnifferData;
  uses {
    interface PacketField<uint8_t> as PacketRSSI;
    interface PacketField<uint8_t> as PacketLQI;
    interface PacketTimeStamp<TRadio, uint32_t> as Timestamp;
  }
}

implementation {
  command uint8_t SnifferData.getPacketRSSI(message_t* msg) {
    return ( call PacketRSSI.isSet(msg) ) ? call PacketRSSI.get(msg) : (uint8_t)0;
  }
 
  command uint8_t SnifferData.getPacketLQI(message_t* msg) {
    return ( call PacketLQI.isSet(msg) ) ? call PacketLQI.get(msg) : (uint8_t)0;
  }
  
  command uint32_t SnifferData.getPacketTimestamp(message_t* msg) {
    return ( call Timestamp.isValid(msg) ) ? call Timestamp.timestamp(msg) : (uint32_t)0;
  }
}
