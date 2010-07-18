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

module PacketSnifferP @safe() {
  uses {
    interface Boot;
    interface Leds;
    
    interface SplitControl as SerialControl;
    interface SplitControl as RadioControl;

//  Serial communication    
    interface Send    as UartSend;
    interface Receive as UartReceive;

//  Radio message detection
    interface Receive as RadioReceive;

//  Packet info
    interface Packet as RadioPacket;
    interface SnifferData;
  }
}

implementation
{

  message_t  uartQueueBufs[UART_QUEUE_LEN];
  message_t  * ONE_NOK uartQueue[UART_QUEUE_LEN];
  uint8_t    uartPayloadLength[UART_QUEUE_LEN];
  
  uint8_t    uartIn, uartOut;
  bool       uartBusy, uartFull;

  task void uartSendTask();

  /**  BRING UP SERIAL AND RADIO  **/
  task void serialPowerUp() {
    if ( SUCCESS != call SerialControl.start() )
      post serialPowerUp();
  }

  task void radioPowerUp() {
    if ( SUCCESS != call RadioControl.start() )
      post radioPowerUp();
  }

  event void RadioControl.startDone(error_t error) {
    if ( error == SUCCESS )
      call Leds.led0On();
    else
      post radioPowerUp();
  }

  event void RadioControl.stopDone(error_t error) {
    if ( error == SUCCESS ) {
      call Leds.led0Off();
      post radioPowerUp();
    }
  }

  event void Boot.booted() {
    uint16_t i;
    for (i = 0; i < UART_QUEUE_LEN; i++) {
        uartQueue[i] = &uartQueueBufs[i];
        uartPayloadLength[i] = 0;
    }
    uartIn = uartOut = 0;
    uartBusy = FALSE;
    uartFull = TRUE;

    post serialPowerUp();
  }

  event void SerialControl.startDone(error_t error) {
    if ( SUCCESS != error )
      post serialPowerUp();
    else {
      uartFull = FALSE;
      post radioPowerUp();
    }
  }

  event void SerialControl.stopDone(error_t error) {
    if ( error == SUCCESS )
      post serialPowerUp();
  }

  /** MESSAGE RECEPTION **/
  event message_t* ONE RadioReceive.receive(message_t* ONE msg, void* COUNT(len) payload, uint8_t len) {

    message_t *ret = msg;  
    sniffer_data_t sd;     
    sniffer_data_t *psd = (sniffer_data_t*)(payload + len);

    call Leds.led1Toggle();
    
    // Retrieve sniffer information
    sd.rssi = call SnifferData.getPacketRSSI(msg);
    sd.lqi  = call SnifferData.getPacketLQI(msg);
    sd.timestamp = call SnifferData.getPacketTimestamp(msg);
    
    // Overwrite footer/metadata section
    *psd = sd;
    
    atomic {
      if (!uartFull){
        ret = uartQueue[uartIn];
        uartQueue[uartIn] = msg;
        uartPayloadLength[uartIn] = len;
        
        uartIn = (uartIn + 1) % UART_QUEUE_LEN;
  
        if (uartIn == uartOut)
          uartFull = TRUE;

        if (!uartBusy) {
          post uartSendTask();
          uartBusy = TRUE;
        }
      }
    }
    
    return ret;
  }

  /** SERIAL COMMUNICATION **/
  task void uartSendTask() {

    message_t* msg;

    atomic {
      if (uartIn == uartOut && !uartFull) {
        uartBusy = FALSE;
        return;
      }
    }    
    msg = uartQueue[uartOut];
    
    call RadioPacket.clear(msg);

    if (call UartSend.send(uartQueue[uartOut], uartPayloadLength[uartOut]) != SUCCESS)
      post uartSendTask();

  }

  event void UartSend.sendDone(message_t* msg, error_t error) {
    if ( error == SUCCESS )
      call Leds.led2Toggle();
      atomic {
        if (msg == uartQueue[uartOut]) {
          if (++uartOut >= UART_QUEUE_LEN)
            uartOut = 0;
          if (uartFull)
            uartFull = FALSE;
        }
      }
      post uartSendTask();
  }

  event message_t* UartReceive.receive(message_t* msg, void* payload, uint8_t len) {
    return msg;
  }

}  
