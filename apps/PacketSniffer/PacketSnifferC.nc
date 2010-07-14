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

configuration PacketSnifferC {}

implementation {

  // just to avoid a timer compilation bug
  components new TimerMilliC();
  components MainC, LedsC;
  components SerialPacketSnifferC as Serial;
 
#if defined(PLATFORM_IRIS) || defined(PLATFORM_MULLE)
  components RF230SnifferStackC as Radio;
#elif defined(PLATFORM_MICA2) || defined(PLATFORM_MICA2DOT)
  #error "** THIS PLATFORM IS NOT SUPPORTED YET ! **"
#elif defined(PLATFORM_MICAZ) || defined(PLATFORM_TELOSB) || defined(PLATFORM_SHIMMER) || defined(PLATFORM_SHIMMER2) || defined(PLATFORM_INTELMOTE2) || defined(PLATFORM_TELOSA)
  #error "** THIS PLATFORM IS NOT SUPPORTED YET ! **"
#elif defined(PLATFORM_EYESIFXV1) || defined(PLATFORM_EYESIFXV2)
  #error "** THIS PLATFORM IS NOT SUPPORTED YET ! **"
#else
  #error "This platform is not supported!"
#endif
  
  components PacketSnifferP;
  
  PacketSnifferP.Boot -> MainC;
  PacketSnifferP.Leds -> LedsC;

  PacketSnifferP.SerialControl  -> Serial;
  PacketSnifferP.UartSend       -> Serial;
  PacketSnifferP.UartReceive    -> Serial;

  PacketSnifferP.RadioControl   -> Radio;
  PacketSnifferP.RadioPacket    -> Radio;
  PacketSnifferP.RadioReceive   -> Radio;

}
