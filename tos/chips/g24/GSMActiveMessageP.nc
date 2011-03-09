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

#include<stdio.h>

module GSMActiveMessageP{
	provides interface SplitControl;
	provides interface AMSend; 
	uses interface GsmControl;
	uses interface DiagMsg;
}
implementation{
	
	#define UDP_TCP "0"
	#define OPERATION "1"
	#define APN "internet"
	
	#define USER ""
	#define PWD ""
	#define SOCKET "1"
	#define GSMPORT "9001"
	#define IP "wsn.math.u-szeged.hu"
	#define SERVERPORT "9001"
	
	message_t message;
	bool busy=FALSE;
	
	char *convertToHex(uint8_t *toConvert, uint8_t len);
	
	command error_t SplitControl.start(){
		error_t err=call GsmControl.onGsmModule();
		if(err==SUCCESS){
			return err;
		}else if(err==EALREADY){
			if((err=call GsmControl.callProvider(OPERATION,APN,USER,PWD))==SUCCESS){
				return err;
			}else if(err==EALREADY){
				err=call GsmControl.openSocket(SOCKET,GSMPORT,IP,SERVERPORT,UDP_TCP);
				return err;
			}
		}
		return err;
	}

	event void GsmControl.onGsmModuleDone(error_t err){
		if(err==SUCCESS){
			if((err=call GsmControl.callProvider(OPERATION,APN,USER,PWD))==SUCCESS){
			return;
			}
		}
		signal SplitControl.startDone(err);
	}
	
	event void GsmControl.callProviderDone(error_t err){
		if(err==SUCCESS){
			if((err=call GsmControl.openSocket(SOCKET,GSMPORT,IP,SERVERPORT,UDP_TCP))==SUCCESS){
				return;
			}
		}
		signal SplitControl.startDone(err);
	}
	
	event void GsmControl.openSocketDone(error_t err){
		signal SplitControl.startDone(err);
	}
		
	command error_t SplitControl.stop(){
		error_t err=call GsmControl.closeSocket(SOCKET);
		if(err==SUCCESS){
			return err;
		}else if(err==EALREADY){
			err=call GsmControl.offGsmModule();
			return err;
		}	
		return err;
	}
	
	event void GsmControl.closeSocketDone(error_t err){
		if(err==SUCCESS){
			if((err=call GsmControl.offGsmModule())==SUCCESS){
				return;
			}
		}
		signal SplitControl.stopDone(err);
	}
	
	event void GsmControl.offGsmModuleDone(error_t err){
		signal SplitControl.stopDone(err);
	}
	
	command error_t AMSend.send(am_addr_t addr, message_t *msg, uint8_t len){
		error_t err=call GsmControl.mipSend(SOCKET,convertToHex((uint8_t*)msg->data,len));
		if(err==SUCCESS){
			busy=TRUE;
		}else{
			busy=FALSE;
		}
		return err;
	}
	
	event void GsmControl.mipSendDone(error_t err){
		if(err==SUCCESS){
			if((err=call GsmControl.mipPush(SOCKET))==SUCCESS){
			return;
			}
		}
		busy=FALSE;
		signal AMSend.sendDone(&message, err);
	}
	
	event void GsmControl.mipPushDone(error_t err){
		busy=FALSE;
		signal AMSend.sendDone(&message, err);
	}

	command error_t AMSend.cancel(message_t *msg){
		return SUCCESS;
	}

	command void * AMSend.getPayload(message_t *msg, uint8_t len){
		return ((void*)msg)+sizeof(message_header_t);
	}

	command uint8_t AMSend.maxPayloadLength(){
		return 0;
	}
	
	char* convertToHex(uint8_t *toConvert, uint8_t len){
		static char hexString[(TOSH_DATA_LENGTH*2)+16];
		char hexChar[3];
		uint8_t i;
		
		*(hexString)='\0';
////////// HEADER //////////////////////////////////////////
		strcat(hexString,"5041434B4554");
		if(len<=15){
			hexChar[0]='0';
			sprintf(hexChar+1,"%X",len);
			strcat(hexString,hexChar);
		}else{
			sprintf(hexChar,"%X",len);
			strcat(hexString,hexChar);
		}
////////// HEADER END //////////////////////////////////////
////////// DATA       //////////////////////////////////////
		for(i=0;i<len;i++){
			if(toConvert[i]<=15){
				hexChar[0]='0';
				sprintf(hexChar+1,"%X",toConvert[i]);
				strcat(hexString,hexChar);
			}else{
				sprintf(hexChar,"%X",toConvert[i]);
				strcat(hexString,hexChar);
			}
		}	
		strcat(hexString,"0D");

////////// DATA END    //////////////////////////////////////
		return hexString;
	}
}