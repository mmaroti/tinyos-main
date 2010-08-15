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

configuration HplG24LiteC {
	provides {
    
	//Serial interface RTS pin
	interface GeneralIO as RTS_N;
	//Serial interface DTR pin
	interface GeneralIO as DTR_N;
	//Control pin to wake up G24 from sleep
	interface GeneralIO as WKUPI_N;
	//Control pin to switch on/off G24
	interface GeneralIO as ON_N;
    
	//Serial interface CTS pin
	interface GpioInterrupt as CTS_N;
	//Serial Interface DSR pin
	interface GpioInterrupt as DSR_N;
	//Control pin signals when G24 is woken on
	interface GpioInterrupt as WKUPO_N;
	//Control pin signals when G24 is on/off
	interface GpioInterrupt as RESET_N;
  
	//Serial interfaces to send, receive and switch on/off UART
	interface UartByte;
	interface UartStream;
	interface StdControl as SerialStdControl;
  }
}
implementation {

	components HplAtm128GeneralIOC as Pins;
	
	RTS_N = Pins.PortC0;
	DTR_N = Pins.PortC1;
	ON_N = Pins.PortC3;
	WKUPI_N = Pins.PortC5;
	
	components HplAtm128InterruptC;
	components new Atm128GpioInterruptC() as Atm128GpioInterrupt0C;
	Atm128GpioInterrupt0C.Atm128Interrupt->HplAtm128InterruptC.Int4;
	RESET_N=Atm128GpioInterrupt0C.Interrupt;

	
	components new Atm128GpioInterruptC() as Atm128GpioInterrupt1C;
	Atm128GpioInterrupt1C.Atm128Interrupt->HplAtm128InterruptC.Int5;
	DSR_N=Atm128GpioInterrupt1C.Interrupt;

	components new Atm128GpioInterruptC() as Atm128GpioInterrupt2C;
	Atm128GpioInterrupt2C.Atm128Interrupt->HplAtm128InterruptC.Int6;
	WKUPO_N=Atm128GpioInterrupt2C.Interrupt;

	components new Atm128GpioInterruptC() as Atm128GpioInterrupt3C;
	Atm128GpioInterrupt3C.Atm128Interrupt->HplAtm128InterruptC.Int7;
	CTS_N=Atm128GpioInterrupt3C.Interrupt;
	
	components CounterMicro32C;
	components Atm128Uart0C;
	Atm128Uart0C.Counter -> CounterMicro32C;
	UartByte=Atm128Uart0C.UartByte;
	UartStream=Atm128Uart0C.UartStream;
	SerialStdControl=Atm128Uart0C.StdControl;
}
