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
* Author: Andras Biro
*/

#include "TimeSyncPoints.h"

module TimeSyncPointsP
{
  provides
  {
    interface TimeSyncPoints;
    interface StdControl;
    interface Set<uint16_t> as SetInterval;
    interface Set<uint8_t> as SetSavePoints;
    interface Get<uint8_t> as GetLastRSSI;
    interface Get<uint8_t> as GetLastLQI;    
  }
  uses
  {	
    interface Receive as TimeSyncReceive;
    interface TimeSyncPacket<TMilli, uint32_t> as TimeSyncPacketMilli;
    interface PacketTimeStamp<TMilli, uint32_t> as PacketTimeStampMilli;
    interface TimeSyncAMSend<TMilli, uint32_t> as TimeSyncAMSendMilli;
    interface AMPacket;
    interface Timer<TMilli>;
    interface LocalTime<TMilli>;
    
    interface Get<uint16_t> as getBootCount;
    interface PacketField<uint8_t> as PacketRSSI;
    interface PacketField<uint8_t> as PacketLinkQuality;
  }
}
implementation
{

  message_t syncMessage;
  uint16_t period=TIMESYNCPOINTS_PERIOD;
  bool running=FALSE;
  uint8_t lastRSSI,lastLQI;
  uint8_t pointsToSave=0xff;
  uint16_t pointsReceived=0;
  uint16_t pointSaveOffset=0;
  
  
  command error_t StdControl.start()
  {
      if(!running)
      {
          running=TRUE;
          if(period!=0)
            call Timer.startPeriodic(((uint32_t)period)<<10);
      }
      return SUCCESS;
  }
  
  command error_t StdControl.stop()
  {
      if (running)
      {
          running=FALSE;
          if(period!=0)
            call Timer.stop();
      }
      return SUCCESS;
  }
  
  command void SetInterval.set(uint16_t value){
    period=value;
    if(running&&period!=0)
    {
      call Timer.startPeriodic(((uint32_t)period)<<10);
    }
  }
  
  command void SetSavePoints.set(uint8_t value){  
    pointsToSave=value;
  }
  
  inline void sendPoint(){
    timeSyncPointsMsg_t* packet = (timeSyncPointsMsg_t*)(call TimeSyncAMSendMilli.getPayload(&syncMessage, sizeof(timeSyncPointsMsg_t)));
    packet->timeStamp = call LocalTime.get();
    packet->bootCount = call getBootCount.get();
    call TimeSyncAMSendMilli.send(AM_BROADCAST_ADDR, &syncMessage, sizeof(timeSyncPointsMsg_t),packet->timeStamp);
    pointSaveOffset+=pointsToSave;
    if(pointSaveOffset>=pointsReceived)
      pointSaveOffset=0;
    if(pointSaveOffset>(pointsReceived-pointsToSave)){
      pointSaveOffset=pointsReceived-pointsToSave;
    }
    pointsReceived=0;    
  }

  event void Timer.fired()
  {
    sendPoint();
  }
  
  event message_t* TimeSyncReceive.receive(message_t* msg,void* payload, uint8_t len)
  {
    pointsReceived++;
    if ((pointsReceived>pointSaveOffset&&pointsReceived<=pointSaveOffset+pointsToSave)&&running&&call TimeSyncPacketMilli.isValid(msg)){
      timeSyncPointsMsg_t* syncMsg = (timeSyncPointsMsg_t*)payload;
      lastRSSI=call PacketRSSI.get(msg);
      lastLQI=call PacketLinkQuality.get(msg);
      signal TimeSyncPoints.syncPoint(call TimeSyncPacketMilli.eventTime(msg), call AMPacket.source(msg), syncMsg->timeStamp, syncMsg->bootCount);
    }
    return msg;
  }
  
  command uint8_t GetLastRSSI.get(){
    return lastRSSI;
  }
  
  command uint8_t GetLastLQI.get(){
    return lastLQI;
  }
  
  event void TimeSyncAMSendMilli.sendDone(message_t* Bufptr, error_t err){}
  default event void TimeSyncPoints.syncPoint(uint32_t local, am_addr_t nodeId, uint32_t remote, uint16_t bootCount){}
  
  command void TimeSyncPoints.sendNow(){
    sendPoint();
  }
}

