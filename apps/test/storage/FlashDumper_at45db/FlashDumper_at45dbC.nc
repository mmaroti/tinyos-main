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
* Author:Andras Biro
*/
#include "Storage.h"
#include "StorageVolumes.h"
#include "FlashDumper_at45db.h"
configuration FlashDumper_at45dbC {
 
}
implementation {
  enum {
    LOG_ID = unique(UQ_LOG_STORAGE),
    RESOURCE_ID = unique(UQ_AT45DB)
  };
    
  components FlashDumper_at45dbP, At45dbStorageManagerC, At45dbC, MainC, LedsC;
  components SerialActiveMessageC, new SerialAMSenderC(AM_CTRL_MSG_T), new SerialAMReceiverC(AM_CTRL_MSG_T);

  FlashDumper_at45dbP.At45db -> At45dbC;
  FlashDumper_at45dbP.At45dbVolume[LOG_ID] -> At45dbStorageManagerC.At45dbVolume[VOLUME_FLASH];
  FlashDumper_at45dbP.Resource -> At45dbC.Resource[RESOURCE_ID];
  
  FlashDumper_at45dbP.Boot-> MainC;
  FlashDumper_at45dbP.Leds->LedsC;
  
  FlashDumper_at45dbP.SplitControl -> SerialActiveMessageC;
  FlashDumper_at45dbP.AMSend -> SerialAMSenderC;
  FlashDumper_at45dbP.Receive -> SerialAMReceiverC;
  FlashDumper_at45dbP.Packet -> SerialAMSenderC;
  FlashDumper_at45dbP.AMPacket -> SerialAMSenderC;
}
  