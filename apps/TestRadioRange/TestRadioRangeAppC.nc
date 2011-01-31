/*
 * Copyright (c) 2010, University of Szeged
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 * - Neither the name of the copyright holder nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Andras Biro
 */

#include "StorageVolumes.h"
#include "TestRadioRange.h"

configuration TestRadioRangeAppC {}
implementation {
  components MainC, TestRadioRangeC as App, LedsC;
  components new AMSenderC(AM_TEST_MSG);
  components new AMReceiverC(AM_TEST_MSG);
  components new SerialAMSenderC(AM_SER_MSG);
  components new SerialAMReceiverC(AM_SER_MSG);
  components new TimerMilliC();
  components RFA1ActiveMessageC;
  components SerialActiveMessageC;

  components new LogStorageC(VOLUME_LOGTEST, FALSE);
  
  App.Boot -> MainC.Boot;
  
  App.Receive -> AMReceiverC;
  App.AMSend -> AMSenderC;
  App.SReceive -> SerialAMReceiverC;
  App.SAMSend -> SerialAMSenderC;
  App.AMControl -> RFA1ActiveMessageC;
  App.SerControl -> SerialActiveMessageC;
  App.Leds -> LedsC;
  App.MilliTimer -> TimerMilliC;
  App.Packet -> AMSenderC;
  App.PacketRSSI -> RFA1ActiveMessageC.PacketRSSI;
  App.PacketLinkQuality -> RFA1ActiveMessageC.PacketLinkQuality;
  App.LogWrite -> LogStorageC;
  App.LogRead -> LogStorageC;

}


