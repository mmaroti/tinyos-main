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

module GSMDriverP{
	provides interface GsmControl;
	provides interface Init as SoftwareInit;
	uses
	{
		interface GeneralIO as ON;
//		interface GeneralIO as DTR;
//		interface GeneralIO as RTS;
		interface GeneralIO as RESET;
		interface Timer<TMilli>;
		interface UartStream;
		interface BusyWait<TMicro, uint16_t>;
		interface StdControl;
		interface DiagMsg;
	}
}
implementation{

	#define BUFFERSIZE 100
	#define STARTTIME 500
	#define STOPTIME 2000
	#define ANSTIMEOUT 5000
	
	enum{
		OFFED=0,
		OFFING,
		ONING,
		ONED,
		CALLING,
		CALLED,
		OPENING,
		OPENED,
		CLOSEING,
		CLOSED,
		SENDING,
		SENT,
		PUSHING,
		};
	
	norace error_t err;
	norace uint8_t state=OFFED;
	char ans[105],cmd[105],ipAddress[15];
	
	task void callTask(){
		signal GsmControl.callProviderDone(err);
	}
	
	task void openTask(){
		signal GsmControl.openSocketDone(err);
	}
	
	task void closeTask(){
		signal GsmControl.closeSocketDone(err);
	}
	
	task void sendTask(){
		signal GsmControl.mipSendDone(err);
	}
	
	task void pushTask(){
		signal GsmControl.mipPushDone(err);
	}
	
	task void stopTimer(){
		call Timer.stop();
	}

	command error_t SoftwareInit.init(){
		uint8_t waitTime;
		call ON.makeOutput();
		call RESET.makeInput();
		for(waitTime=0;waitTime<=27;waitTime++) // wait 800 ms to power on the module
			call BusyWait.wait(30000);
		call ON.set();
		for(waitTime=0;waitTime<=27;waitTime++) // wait again 800 ms before start to power off the module
			call BusyWait.wait(30000);
		call ON.clr();
		for(waitTime=0;waitTime<=67;waitTime++) // wait 2000 ms to power off the module
				call BusyWait.wait(30000);
		call ON.set();
		for(waitTime=0;waitTime<=84;waitTime++)	// wait 2500 ms to reset signal goes low which indicates the module is powered off
				call BusyWait.wait(30000);
		if(call RESET.get()==0)
			return SUCCESS;
		else
			return FAIL;
	}
	
	command error_t GsmControl.onGsmModule(){
		error_t error;
		if(state==ONING){
			error=SUCCESS;
		}else if((state==ONED) | (state==CLOSED) | (state==OPENED) | (state==CALLED)){
			error=EALREADY;
		}else if((state==OFFING) | (state==CALLING) | (state==OPENING) | (state==CLOSEING)){
			error=EBUSY;
		}else if((state==SENDING) | (state==SENT) | (state==PUSHING)){
			error=FAIL;
		}else{
			state=ONING;
			call ON.clr();
			call Timer.startOneShot(STARTTIME);
			error=SUCCESS;
		}
		return error;
	}

	event void Timer.fired(){ 
		error_t error;
		uint8_t waitForReset;
		call ON.set();
		if(state==ONING){
			for(waitForReset=0;waitForReset<=10;waitForReset++) // wait 300 ms to reset signal goes high which indicates the module is powered on
				call BusyWait.wait(30000);
			if(call RESET.get()==1 && call StdControl.start()==SUCCESS && call UartStream.enableReceiveInterrupt()==SUCCESS ){
				error=SUCCESS;
				state=ONED;
			}else{
				error=FAIL;
				state=OFFED;
			}
			signal GsmControl.onGsmModuleDone(error);
		}else if (state==OFFING){
			for(waitForReset=0;waitForReset<=84;waitForReset++) // wait 2500 ms to reset signal goes low which indicates the module is powered off
				call BusyWait.wait(30000);
			if(call RESET.get()==0 && call StdControl.stop()==SUCCESS && call UartStream.disableReceiveInterrupt()==SUCCESS){
				error=SUCCESS;
				state=OFFED;
			}else{
				error=FAIL;
				state=ONED;
			}
			signal GsmControl.offGsmModuleDone(error);
		}else if (state==CALLING){
			state=ONED;
			signal GsmControl.callProviderDone(FAIL);
		}else if(state==OPENING){
			state=CALLED;	
			signal GsmControl.openSocketDone(FAIL);
		}else if (state==CLOSEING){
			state=OPENED;
			signal GsmControl.closeSocketDone(FAIL);
		}else if(state==SENDING){
			state=OPENED;
			signal GsmControl.mipSendDone(FAIL);
		}else{
			state=SENT;
			signal GsmControl.mipPushDone(FAIL);
		}
	}

	command error_t GsmControl.offGsmModule(){
		error_t error;
		if(state==OFFING){
			error=SUCCESS;
		}else if(state==OFFED){
			error=EALREADY;
		}else if((state==ONING) | (state==CALLING) | (state==OPENING) | (state==CLOSEING)){
			error=EBUSY;
		}else if ( (state==OPENED) ){
			error=FAIL;
		}else{
			state=OFFING;
			call ON.clr();
			call Timer.startOneShot(STOPTIME);
			error=SUCCESS;
		}
		return error;
	}

	command error_t GsmControl.callProvider(char* operation, char* apn, char* user, char* password){
		error_t error;
		if(state==CALLING){
			error=SUCCESS;
		}else if((state==CALLED) | (state==OPENED)){
			error=EALREADY;
		}else if((state==ONING) | (state==OFFING) | (state==OPENING) | (state==CLOSEING)){
			error=EBUSY;
		}else if ((state==OFFED) | (state==CLOSED)){
			error=FAIL;
		}else{
			state=CALLING;
			strcpy(cmd,"at+mipcall=");
			strcat(cmd,operation);
			strcat(cmd,",");
			strcat(cmd,"\"");
			strcat(cmd,apn);
			strcat(cmd,"\"");
			strcat(cmd,",");
			strcat(cmd,"\"");
			strcat(cmd,user);
			strcat(cmd,"\"");
			strcat(cmd,",");
			strcat(cmd,"\"");
			strcat(cmd,password);
			strcat(cmd,"\"");
			strcat(cmd,"\r");
			strcat(cmd,"\r");
			strcat(cmd,"\n");
			call Timer.startOneShot(ANSTIMEOUT);
			error=call UartStream.send((uint8_t*)cmd,strlen(cmd)-2);	
		}
		return error;
	}
	
	command error_t GsmControl.openSocket(char* socket, char* gsmPort, char* ip, char* serverPort, char* udp_tcp){
		error_t error;
		if(state==OPENING){
			error=SUCCESS;
		}else if(state==OPENED){
			error=EALREADY;
		}else if((state==ONING) | (state==OFFING) | (state==CALLING) | (state==CLOSEING) ){
			error=EBUSY;
		}else if((state==ONED) | (state==OFFED)){
			error=FAIL;
		}else{	
			state=OPENING; 
			strcpy(cmd,"at+mipopen=");
			strcat(cmd,socket);
			strcat(cmd,",");
			strcat(cmd,gsmPort);
			strcat(cmd,",");
			strcat(cmd,"\"");
			strcat(cmd,ip);
			strcat(cmd,"\"");
			strcat(cmd,",");
			strcat(cmd,serverPort);
			strcat(cmd,",");
			strcat(cmd,udp_tcp);
			strcat(cmd,"\r");
			strcat(cmd,"\r");
			strcat(cmd,"\n");
			call Timer.startOneShot(ANSTIMEOUT);
			error=call UartStream.send((uint8_t*)cmd,strlen(cmd)-2);
		}
		return error;
	}
	
	command error_t GsmControl.closeSocket(char* socket){
		error_t error;
		if(state==CLOSEING){
			error=SUCCESS;
		}else if((state==CLOSED) | (state==OFFED) | (state==CALLED) | (state==ONED)){
			error=EALREADY;
		}else if((state==OPENING) | (state==CALLING) | (state==OFFING) | (state==ONING)){
			error=EBUSY;
		}else if((state==SENDING) | (state==SENT) | (state==PUSHING)){
			error=FAIL;
		}else{
			state=CLOSEING;
			strcpy(cmd,"at+mipclose=");
			strcat(cmd,socket);
			strcat(cmd,"\r");
			strcat(cmd,"\r");
			strcat(cmd,"\n");
			call Timer.startOneShot(ANSTIMEOUT);
			error=call UartStream.send((uint8_t*)cmd,strlen(cmd)-2);
		}
		return error;	
	}
	
	command error_t GsmControl.mipSend(char* socket,char* msg){
	error_t error;
		if (state==SENDING){
			error=EBUSY;
		}else if(state!=OPENED){
			error=FAIL;
		}else{
			state=SENDING;
			strcpy(cmd,"at+mipsend=");
			strcat(cmd,socket);
			strcat(cmd,",");
			strcat(cmd,"\"");
			strcat(cmd,msg);
			strcat(cmd,"\"");
			strcat(cmd,"\r");
			strcat(cmd,"\r");
			strcat(cmd,"\n");
			call Timer.startOneShot(ANSTIMEOUT);
			error=call UartStream.send((uint8_t*)cmd,strlen(cmd)-2);
		}
		return error;
	}
		
	command error_t GsmControl.mipPush(char* socket){
		error_t error;
		if (state==PUSHING){
			error=EBUSY;
		}else if(state!=SENT){
			error=FAIL;
		}else{
			state=PUSHING;
			strcpy(cmd,"at+mippush=");
			strcat(cmd,socket);
			strcat(cmd,"\r");
			strcat(cmd,"\r");
			strcat(cmd,"\n");
			call Timer.startOneShot(ANSTIMEOUT);
			error=call UartStream.send((uint8_t*)cmd,strlen(cmd)-2);
		}
		return error; 
	}
	
	async event void UartStream.receivedByte( uint8_t byte ){
		static uint8_t byteCounter=0,pos=0,cmp=0;
		
		ans[byteCounter++]=byte;
		if(byte==10){
			if(state==CALLING){
				switch(pos++){
				case(0):	
					post stopTimer();
					cmp=strncmp(ans,cmd,byteCounter-2);
					byteCounter=0;
					break;
				case(1):
					cmp=strncmp(ans,"OK\r\n",4);
					byteCounter=0;
					break;
				case(2):
					byteCounter=0;
					break;
				case(3):
					cmp=strncmp(ans,"+MIPCALL:",9);
					memcpy (ipAddress,ans+10,15);
					byteCounter=0;
					break;
				}
				if(cmp!=0){
					state=ONED;
					err=FAIL;
					pos=0;
					post callTask();
				}else if (pos==4){
					state=CALLED;
					err=SUCCESS;
					pos=0;
					post callTask();
				}
			}else if(state==OPENING){
				switch(pos++){
				case(0):
					post stopTimer();
					cmp=strncmp(ans,cmd,byteCounter-2);
					byteCounter=0;
					break;
				case(1):
					cmp=strncmp(ans,"OK\r\n",byteCounter-2);
					byteCounter=0;
					break;
				case(2):
					byteCounter=0;
					break;
				case(3):
					cmp=strncmp(ans,"+MIPOPEN: ",10);
					byteCounter=0;
					break;
				}
				if(cmp!=0){
					state=CALLED;
					err=FAIL;
					pos=0;
					post openTask();
				}else if (pos==4){
					state=OPENED;
					err=SUCCESS;
					pos=0;
					post openTask();
				}
			}else if(state==CLOSEING){
				switch(pos++){
				case(0):
					post stopTimer();
					cmp=strncmp(ans,cmd,byteCounter-2);
					byteCounter=0;
					break;
				case(1):
					cmp=strncmp(ans,"OK\r\n",byteCounter-2);
					byteCounter=0;
					break;
				case(2):
					byteCounter=0;
					break;
				case(3):
					byteCounter=0;
					cmp=strncmp(ans,"+MIPCLOSE: ",10);
					break;
				}
				if(cmp!=0){
					state=OPENED;
					err=FAIL;
					pos=0;
					post closeTask();
				}else if (pos==4){
					state=CLOSED;
					err=SUCCESS;
					pos=0;
					post closeTask();
				}
			}else if(state==SENDING){
				switch(pos++){
				case(0):
					post stopTimer();
					cmp=strncmp(ans,cmd,byteCounter-2);
					byteCounter=0;
					break;
				case(1):
					cmp=strncmp(ans,"+MIPSEND:",9);
					byteCounter=0;
					break;
				case(2):
					byteCounter=0;
					break;
				case(3):
					cmp=strncmp(ans,"OK\r\n",byteCounter-2);
					byteCounter=0;
					break;
				}
				if(cmp!=0){
					state=OPENED;
					err=FAIL;
					pos=0;
					post sendTask();
				}else if (pos==4){
					state=SENT;
					err=SUCCESS;
					pos=0;
					post sendTask();
				}
			}else if(state==PUSHING){
				switch(pos++){
				case(0):
					post stopTimer();
					cmp=strncmp(ans,cmd,byteCounter-2);
					byteCounter=0;
					break;
				case(1):
					cmp=strncmp(ans,"+MIPPUSH:",9);
					byteCounter=0;
					break;
				case(2):
					byteCounter=0;
					break;
				case(3):
					cmp=strncmp(ans,"OK\r\n",byteCounter-2);
					byteCounter=0;
					break;
				}
				if(cmp!=0){
					state=SENT;
					err=FAIL;
					pos=0;
					post pushTask();
				}else if (pos==4){
					state=OPENED;
					err=SUCCESS;
					pos=0;
					post pushTask();
				}
			} else {
				byteCounter=0;
			}
		}
			
	}
	
	async event void UartStream.sendDone( uint8_t* buf, uint16_t len, error_t error ){
		err=error;
		if(err!=SUCCESS){
			if(state==CALLING){
				state=ONED;
				post callTask();
			}else if(state==OPENING){
				state=CALLED;
				post openTask();
			}else if(state==CLOSEING){
				state=OPENED;
				post closeTask();
			}else if(state==SENDING){
				state=OPENED;
				post sendTask();
			}else if(state==PUSHING){
				state=SENT;
				post pushTask();
			}
		}
	}
	
	async event void UartStream.receiveDone( uint8_t* buf, uint16_t len, error_t error ){}
	
}