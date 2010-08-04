/** Copyright (c) 2010, University of Szeged
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
* Author: Ali Baharev
*/

#include "CtrlMsg.h"
#include "ReportMsg.h"

configuration RadioHandlerC {

}

implementation{
	
	enum {
		AM_SAMPLEMSG = 0x37
	};
	
	components MainC;
	components RadioHandlerP;
	components ActiveMessageC;
	components AssertC;

	components new AMReceiverC(AM_CTRLMSG) as AMRec;
	components new AMSenderC(AM_REPORTMSG) as Report;
	components new AMSenderC(AM_SAMPLEMSG) as Samples;

	components BufferedSendP;

	components new TimerMilliC() as Timer1;
	components new TimerMilliC() as Timer2;
	components new TimerMilliC() as Timer3;

	components SimpleFileC;
	components LedHandlerC;
	components MeterC;
	components RadioDiagMsgC;

	RadioHandlerP.Boot -> MainC;
	RadioHandlerP.AMControl -> ActiveMessageC;
	RadioHandlerP.Receive -> AMRec;
	RadioHandlerP.AMReportMsg -> Report;
	
	BufferedSendP.AMSend -> Samples; // FIXME It should go to its own component
	BufferedSendP.Packet -> Samples;
	BufferedSendP.DiagMsg -> RadioDiagMsgC;
	RadioHandlerP.BufferedSend -> BufferedSendP;
	
	RadioHandlerP.Disk -> SimpleFileC;
	RadioHandlerP.DiskCtrl -> SimpleFileC;
	
	RadioHandlerP.WatchDog -> Timer1;
	RadioHandlerP.ShortPeriod -> Timer2;
	RadioHandlerP.Download -> Timer3;
	RadioHandlerP.LedHandler -> LedHandlerC;
	RadioHandlerP.Sampling -> MeterC.Sampling;
	RadioHandlerP.MeterCtrl -> MeterC.StdControl;
	RadioHandlerP.DiagMsg -> RadioDiagMsgC;
}