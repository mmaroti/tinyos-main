/*
* Copyright (c) 2011, University of Szeged
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

configuration GSMActiveMessageC{
	provides interface SplitControl;
	provides interface AMSend;
}
implementation{
	components GSMActiveMessageP,GSMDriverP,HplG24LiteC,BusyWaitMicroC,MainC;
	components new TimerMilliC();
	components DiagMsgC;
	
	
	SplitControl=GSMActiveMessageP.SplitControl;
	AMSend=GSMActiveMessageP.AMSend;
	GSMDriverP.ON->HplG24LiteC.ON_N;
//	GSMDriverP.DTR->HplG24LiteC.DTR_N;
//	GSMDriverP.RTS->HplG24LiteC.RTS_N;
	GSMDriverP.RESET->HplG24LiteC.RESET_N;
	GSMDriverP.UartStream->HplG24LiteC.UartStream;
	GSMDriverP.StdControl->HplG24LiteC.SerialStdControl;
	GSMDriverP.Timer->TimerMilliC;
	GSMDriverP.BusyWait -> BusyWaitMicroC;
	GSMActiveMessageP.GsmControl->GSMDriverP.GsmControl; 
	MainC.SoftwareInit -> GSMDriverP.SoftwareInit;
	GSMDriverP.DiagMsg->DiagMsgC;
	GSMActiveMessageP.DiagMsg->DiagMsgC;
}