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
* Author: Zsolt Szabo
*/

#include "DataMsg.h"

configuration PlatformTestC {
}
implementation {
  components MainC;
  components LedsC;
  components Sht21C;
  #ifndef SHT_ONLY
  components Bh1750fviC;
  components new Ms5607C(FALSE);
  #endif
  components ActiveMessageC;
  components new AMSenderC(AM_DATAMSG);
  components PlatformTestP as App;
  //components I2CBusC;
  components DiagMsgC;
  components AtmegaGeneralIOC as IO;

  App.Boot -> MainC;
  App.Leds -> LedsC;
  App.AMSend -> AMSenderC;

  App.Power -> IO.PortF1;
  App.ShtSplit -> Sht21C.SplitControl;
  #ifndef SHT_ONLY
  App.BhSplit -> Bh1750fviC.SplitControl;
  App.MsSplit -> Ms5607C.SplitControl;
  #endif
  
  App.ShtTemp -> Sht21C.Temperature;
  App.ShtHum -> Sht21C.Humidity;
  #ifndef SHT_ONLY
  App.Light -> Bh1750fviC.Light;
  App.MsTemp -> Ms5607C.Temperature;
  App.MsPress -> Ms5607C.Pressure;
  #endif
  App.Radio -> ActiveMessageC;
  //App.Bus -> I2CBusC.BusControl;
  App.DiagMsg -> DiagMsgC;
}
