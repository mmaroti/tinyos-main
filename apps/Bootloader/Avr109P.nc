/*
 * Copyright (c) 2012, Unicomp Kft.
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
 * Author: Andras Biro <bbandi86@gmail.com>
 */

#include <avr/boot.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

module Avr109P{
 uses interface UartStream;
 uses interface UartByte;
 uses interface StdControl;
 uses interface Leds;
 uses interface BusyWait<TMicro, uint16_t>;
 provides interface Init;
}
implementation{
  
  uint16_t address;
  uint16_t temp;
  
  void boot_program_page (uint32_t page, uint8_t *buf)
  {
      uint16_t i;
      uint8_t sreg;

      // Disable interrupts.

      sreg = SREG;
      cli();
  
      eeprom_busy_wait ();

      boot_page_erase (page);
      boot_spm_busy_wait ();      // Wait until the memory is erased.

      for (i=0; i<SPM_PAGESIZE; i+=2)
      {
          // Set up little-endian word.

          uint16_t w = *buf++;
          w += (*buf++) << 8;
      
          boot_page_fill (page + i, w);
      }

      boot_page_write (page);     // Store buffer in flash page.
      boot_spm_busy_wait();       // Wait until the memory is written.

      // Reenable RWW-section again. We need this if we want to jump back
      // to the application after bootloading.

      boot_rww_enable ();

      // Re-enable interrupts (if they were ever enabled).

      SREG = sreg;
  }
  
 inline void exitbl(bool silent){
   void (*funcptr)(void)=0x0000;
   if(!silent){
     uint8_t i;
     call Leds.set(0xff);
     for(i=0;i<3;i++){
       call BusyWait.wait(100);
       call Leds.set(0);
       call BusyWait.wait(100);
       call Leds.set(0xff);
     }
     call BusyWait.wait(100);
   }
   call Leds.set(0);
   funcptr();
 }
 

  
  command error_t Init.init(){
    error_t err = call StdControl.start();
    return ecombine(err, call UartStream.enableReceiveInterrupt());
  }
  
  async event void UartStream.sendDone( uint8_t* buf, uint16_t len, error_t error ){}

  async event void UartStream.receivedByte( uint8_t byte ){
    uint8_t buf[7];
    switch(byte){
      case 'a':{//auto increment address
        buf[0]='Y';
        call UartStream.send(buf,1);
      }break;
      case 'A':{//set address
        call UartByte.receive((uint8_t*)(&address+1),255);
        call UartByte.receive((uint8_t*)(&address),255);
        buf[0]='\r';
        call UartStream.send(buf,1);
      }break;
#define REMOVE_FLASH_BYTE_SUPPORT
#ifndef REMOVE_FLASH_BYTE_SUPPORT
      case 'c':{//write program memory, low byte
      }break;
      case 'C':{//write program memory, high byte
      }break;
      case 'm':{//issue page page write
      }break;
      case 'R':{//read program memory
      }break;
#endif
#define REMOVE_EEPROM_BYTE_SUPPORT
#ifndef REMOVE_EEPROM_BYTE_SUPPORT
      case 'd':{//read data memory
      }break;
      case 'D':{//write data memory
      }break;
#endif
      case 'e':{//chip erase
      }break;
#define REMOVE_FUSE_AND_LOCK_BIT_SUPPORT
#ifndef REMOVE_FUSE_AND_LOCK_BIT_SUPPORT
      case 'l':{//write lock bits
      }break;
#if defined(_GET_LOCK_BITS)
      case 'r':{//read lock bits
      }break;
      case 'F':{//read (low) fuse bits
      }break;
      case 'N':{//read high fuse bits
      }break;
      case 'Q':{//read extended fuse bits
      }break;
#endif /*defined(_GET_LOCK_BITS)*/
#endif /*REMOVE_FUSE_AND_LOCK_BIT_SUPPORT*/
//#define REMOVE_AVRPROG_SUPPORT
#ifndef REMOVE_AVRPROG_SUPPORT
      case 'P'://enter programming mode
      case 'L':{//leave programming mode
        buf[0]='\r';//we're in programming mode while we're in the bootloader
        call UartStream.send(buf,1);
      }break;
      case 'E':{//exit bootloader
        exitbl(FALSE);
      }break;
      case 'p':{//return programmer type
        buf[0]='S';
        call UartStream.send(buf,1);
      }break;
      case 't':{//return supported device codes
        buf[0]=' ';//TODO: what the heck is this
        buf[1]='0';
        call UartStream.send(buf,2);
      }break;
      case 'x':{//set led
        call UartByte.receive(buf,255);
        call Leds.led3On();
        buf[0]='\r';
        call UartStream.send(buf,1);
      }break;
      case 'y':{//clear led
        call UartByte.receive(buf,255);
        call Leds.led3Off();
        buf[0]='\r';
        call UartStream.send(buf,1);
      }break;
      case 'T':{//select device type
        call UartByte.receive(buf,255);
        buf[0]='\r';
        call UartStream.send(buf,1);
      }break;
#endif
#define REMOVE_BLOCK_SUPPORT
#ifndef REMOVE_BLOCK_SUPPORT
      case 'b':{//check block support
      }break;
      case 'B':{//start block flash/eeprom load
      }break;
      case 'g':{//start block flash/eeprom read
      }break;
#endif
      case 's':{//read signature bytes
         buf[0]=0x1e;//TODO read this from the mcu
         buf[1]=0xa7;
         buf[2]=0x01;
         call UartStream.send(buf,3);
      }break;
      case 'S':{//return software identifier
        buf[0]='A';
        buf[1]='V';
        buf[2]='R';
        buf[3]='B';
        buf[4]='O';
        buf[5]='O';
        buf[6]='T';
        call UartStream.send( buf, 7 );
      }
      case 'V':{//return software version
        buf[0]='1';
        buf[1]='5';
        call UartStream.send(buf,2);
      }break;
      default:{//unknown command
        buf[0]='?';
        call UartStream.send(buf,1);
      }
    }
  }

  async event void UartStream.receiveDone( uint8_t* buf, uint16_t len, error_t error ){}
  
}