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

#include "Mts300SensorMsg.h"

configuration Mts300SensorTesterAppC 
{ 
} 
implementation { 
  
  components Mts300SensorTesterC as App;
  components MainC;
  components LedsC;
  components new TimerMilliC();
  components new AMSenderC(AM_DATAMSG) as DataSender;
  components new AMReceiverC(AM_CONTROLMSG) as ControlReceiver;
  components new SensorMts300C() as Sensor;
  components new MicReadStreamC();
  components ActiveMessageC;

  App.Boot -> MainC;
  App.Leds -> LedsC;
  App.TimerMilli -> TimerMilliC;
  App.AMControl -> ActiveMessageC;
  App.DataSend -> DataSender.AMSend;
  App.DataPacket -> DataSender.Packet;
  App.ControlReceive -> ControlReceiver.Receive;
  App.Beeper -> Sensor.Sounder;
  App.PhotoRead -> Sensor.Light;
  App.MicRead ->  MicReadStreamC;
  App.VrefRead -> Sensor.Vref; 
  App.TempRead -> Sensor.Temp; 
}

