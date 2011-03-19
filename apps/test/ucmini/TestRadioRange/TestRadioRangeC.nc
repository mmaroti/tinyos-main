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

#include "TestRadioRange.h"

module TestRadioRangeC @safe() {
  uses {
    interface Leds;
    interface Boot;
    interface Receive;
    interface AMSend;
    interface Receive as SReceive;
    interface AMSend as SAMSend;
    interface Timer<TMilli> as MilliTimer;
    interface SplitControl as AMControl;
    interface SplitControl as SerControl;
    interface Packet;
    interface PacketField<uint8_t> as PacketRSSI;
    interface PacketField<uint8_t> as PacketLinkQuality;
    interface LogWrite;
    interface LogRead;
  }
}
implementation {


  

  message_t packet;
  test_msg_t logpacket;

  bool locked;
  uint16_t counter = 0;
  
  inline uint16_t getRssi(message_t *msg){
    if(call PacketRSSI.isSet(msg))
      return (uint16_t) call PacketRSSI.get(msg);
    else
      return 0xFFFF;
  }
  
  inline uint16_t getLqi(message_t *msg){
    if(call PacketLinkQuality.isSet(msg))
      return (uint16_t) call PacketLinkQuality.get(msg);
    else
      return 0xFFFF;
  }
  
  event void Boot.booted() {
    if(TOS_NODE_ID==9)
      call LogWrite.erase();
		else
			call SerControl.start();
  }
  
  event void SerControl.startDone(error_t err) {
    if (err != SUCCESS) {
      call SerControl.start();
    } else {
      call AMControl.start();
    }
  }

  event void AMControl.startDone(error_t err) {
    if (err != SUCCESS) {
      call AMControl.start();
    }
  }
  
  event message_t* SReceive.receive(message_t* bufPtr, void* payload, uint8_t len) {
    if(len==sizeof(ser_msg_t)&&TOS_NODE_ID==0){
			ser_msg_t* cmd= (ser_msg_t*)payload;
			test_msg_t* send = (test_msg_t*)call Packet.getPayload(&packet,sizeof(test_msg_t));
			if(cmd->distance==CMD_DOWNLOAD)
				call LogRead.seek(SEEK_BEGINNING);
			else if(cmd->distance==CMD_ERASE){
				call Leds.set(0xff);
				call LogWrite.erase();
			}else {
				send->distance=cmd->distance;
				memcpy(&(send->etc),&(cmd->etc),ETC_LENGTH);
				counter=0;
				call MilliTimer.startPeriodic(300);
				call Leds.led2On();
			}
		} else if(TOS_NODE_ID==3){
			call LogRead.seek(SEEK_BEGINNING);
    }
    return bufPtr;
  }
  
  event void MilliTimer.fired() {
    if(counter<20){
      counter++;
      call AMSend.send(1, &packet, sizeof(test_msg_t));
    }else{
      call MilliTimer.stop();
      call LogWrite.sync();
    }
  }

  event message_t* Receive.receive(message_t* bufPtr, void* payload, uint8_t len) {
    if (len != sizeof(test_msg_t)) {return bufPtr;}
    call Leds.led0Toggle();      
    if(TOS_NODE_ID!=0){
      test_msg_t* rcm = (test_msg_t*)payload;
      call Leds.led2Toggle();//for better visibility
      rcm->slave_rssi=getRssi(bufPtr);
      rcm->slave_lqi=getLqi(bufPtr);
      memcpy(&(packet.data),&(bufPtr->data),TOSH_DATA_LENGTH);
      call AMSend.send(0, &packet, sizeof(test_msg_t));
    } else {
      test_msg_t* rcm = (test_msg_t*)payload;
      rcm->master_rssi=getRssi(bufPtr);
      rcm->master_lqi=getLqi(bufPtr);
      memcpy(&logpacket, payload, len);
      call LogWrite.append(&logpacket, len);
    }
    
    return bufPtr;
  }
  
  event void LogWrite.syncDone(error_t err){
    call Leds.led2Off();
    //end of measure
  }

  event void AMSend.sendDone(message_t* bufPtr, error_t error) {
    call Leds.led1Toggle();      
  }
  
  //only for TOS_NODE_ID==3

  event void SAMSend.sendDone(message_t* bufPtr, error_t error) {
    call LogRead.read(&logpacket, sizeof(test_msg_t));
  }
  
  event void LogRead.seekDone(error_t err){
    if(err==SUCCESS)
      call LogRead.read(&logpacket, sizeof(test_msg_t));
  }
  event void LogRead.readDone(void* buf, storage_len_t len, error_t err) {
    if(err==SUCCESS&&len!=0){
	test_msg_t* send = (test_msg_t*)call Packet.getPayload(&packet,sizeof(test_msg_t));
	test_msg_t* read = (test_msg_t*)buf;
	memcpy(send,read,len);
	call SAMSend.send(0, &packet, len);
    }
  }
  
  //only for TOS_NODE_ID==9
  event void LogWrite.eraseDone(error_t err){call Leds.set(0);}
  
  event void AMControl.stopDone(error_t err) {}
  event void SerControl.stopDone(error_t err) {}
  event void LogWrite.appendDone(void* buf, storage_len_t len, bool recordsLost, error_t err){}
}




