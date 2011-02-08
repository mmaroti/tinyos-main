/*
 * Copyright (c) 2000-2005 The Regents of the University  of California.  
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
 * - Neither the name of the University of California nor the names of
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
 */
/*
 * Copyright (c) 2002-2005 Intel Corporation
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached INTEL-LICENSE     
 * file. If you do not find these files, copies can be found by writing to
 * Intel Research Berkeley, 2150 Shattuck Avenue, Suite 1300, Berkeley, CA, 
 * 94704.  Attention:  Intel License Inquiry.
 */

/**
 * Null is an empty skeleton application.  It is useful to test that the
 * build environment is functional in its most minimal sense, i.e., you
 * can correctly compile an application. It is also useful to test the
 * minimum power consumption of a node when it has absolutely no 
 * interrupts or resources active.
 *
 * @author Cory Sharp <cssharp@eecs.berkeley.edu>
 * @date February 4, 2006
 */
#include <util/twi.h>

module NullC @safe()
{
  uses{
	interface Boot;
	interface BusyWait<TMicro, uint16_t>;
	interface Leds;
	interface McuPowerState;
  }
  provides interface McuPowerOverride;
}
implementation
{
  
  
  enum{
	I2C_WR_ADDRESS =  128,
	I2C_RD_ADDRESS = I2C_WR_ADDRESS+1,
	I2C_COMMAND = 0xf3,
	I2C_DATA_LENGTH=3,
	I2C_MEAS_WAIT=85000UL,
  };
  
  uint8_t res[I2C_DATA_LENGTH];
  uint8_t currentChr=0;
  bool command_written=FALSE;
  bool timer_meas=FALSE;

  async command mcu_power_t McuPowerOverride.lowestState() {
    return ATM128_POWER_DOWN;
  }
  
  void TWI_action(char comm)
  {	
	  TWCR = (comm|(1<<TWINT)|(1<<TWEN)|(1<<TWIE));
  }
  
  event void Boot.booted() {
		call McuPowerState.update();
		PORTD = _BV(PD0) | _BV(PD1);
		call BusyWait.wait(15000U);
		
		TWBR=255;
		TWSR=3&0x03;

		TWI_action(1<<TWSTA);
		call Leds.led0On();

  }
  
  
  task void waitForMeas(){
	uint32_t duration=I2C_MEAS_WAIT;
	while(duration>0){
	  uint16_t dur_part;
	  if(duration>65535U)
		dur_part=65535U;
	  else
		dur_part=duration;
	  call BusyWait.wait(dur_part);
	  duration-=dur_part;
	}
	TWI_action(1<<TWSTA);
  }
  
  task void nextMeas(){
	call Leds.led0Off();
	call Leds.led2Toggle();
	call BusyWait.wait(50000U);
	TWI_action(1<<TWSTA);
	call Leds.led0On();
  }
  
  AVR_ATOMIC_HANDLER(TWI_vect) {
	switch(TW_STATUS){
	  case TW_START:{
		TWDR=I2C_WR_ADDRESS;
		TWI_action(0);
	  }break;
	  case TW_REP_START:{
		TWDR=I2C_RD_ADDRESS;
		TWI_action(0);
		TWI_action(1<<TWEA);
	  }break;
	  case TW_MT_SLA_ACK:{
		if(!command_written){
		  command_written=TRUE;
		  TWDR=I2C_COMMAND;
		  TWI_action(0);
		} else{
		  command_written=FALSE;
		  post waitForMeas();
		}
	  }break;
	  case TW_MT_DATA_ACK:{
		res[currentChr]=TWDR;
		if(++currentChr<I2C_DATA_LENGTH)
		  TWI_action(1<<TWEA);
		else
		  TWI_action(0);
	  }break;
	  case TW_MT_DATA_NACK:{
		TWCR = ((1<<TWINT)|(1<<TWSTO)|(1<<TWEN));
		currentChr=0;
		TWCR = 0;
		post nextMeas();
	  }break;
	  case TW_MT_SLA_NACK:break;
	  case TW_MT_ARB_LOST:break;
	  case TW_MR_SLA_ACK:break;
	  case TW_MR_SLA_NACK:break;
	  case TW_MR_DATA_ACK:break;
	  case TW_MR_DATA_NACK:break;
	  case TW_BUS_ERROR:break;
	  default:break;
	}
  }
  
}



