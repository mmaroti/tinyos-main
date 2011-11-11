module GSMActiveMessageP{
	provides interface SplitControl;
	provides interface AMSend; 
	uses interface GsmControl;
	uses interface DiagMsg;
}
implementation{
	char *conToString1(uint8_t *toConvert, uint8_t link_source_addr, uint16_t len, uint8_t group_ID, uint8_t handler_ID, uint16_t source_addr, uint16_t counter);
	message_t message;
	bool busy=FALSE;
	
	
	void intToString(char *array,int x);
	
	//uint8_t len1;	

	command error_t SplitControl.start(){
		error_t err=call GsmControl.connectToGSM();
		return err;
	}

	event void GsmControl.connectToGSMDone(error_t err){
		signal SplitControl.startDone(err);
	}
		
	command error_t SplitControl.stop(){
		error_t err=call GsmControl.disconnectFromGSM();
		return err;
	}
	
	event void GsmControl.disconnectFromGSMDone(error_t err){
		signal SplitControl.stopDone(err);
	}
	
	command error_t AMSend.send(am_addr_t addr, message_t *msg, uint8_t len){
		
		error_t err=call GsmControl.SendToGSM(conToString1((uint8_t*)msg->data,TOS_NODE_ID,len,1,2,3,4));
		if(err==SUCCESS){
			busy=TRUE;
		}else{
			busy=FALSE;
			err=FAIL;
		}
		return err;
	}
	
	event void GsmControl.SendToGSMDone(error_t err){
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
	char* conToString1(uint8_t *toConvert, uint8_t link_source_addr, uint16_t len, uint8_t group_ID, uint8_t handler_ID, uint16_t source_addr, uint16_t counter){
		static char intString[(TOSH_DATA_LENGTH*2)+9];
		char intChar[3];
		uint8_t i;
				
		*(intString)='\0';
		intToString(intChar,(len+8)); // packet length
		strcat(intString,intChar);
		intToString(intChar,0); // packet id
		strcat(intString,intChar);
//		intToString(intChar,dest_addr); // destination address lower byte
//		strcat(intString,intChar);
//		intToString(intChar,dest_addr>>8); // destination address upper byte
		strcat(intString,intChar);
		intToString(intChar,link_source_addr); // linksoure address lower byte
		strcat(intString,intChar);
		intToString(intChar,link_source_addr>>8); // linksource address upper byte
		strcat(intString,intChar);
		intToString(intChar,len); // payload length
		strcat(intString,intChar);
		intToString(intChar,group_ID); // gorup ID
		strcat(intString,intChar);
		intToString(intChar,handler_ID); // handler ID
		strcat(intString,intChar);
////////// DATA       //////////////////////////////////////
		for(i=0;i<len;i++){
			intToString(intChar,toConvert[i]);
			strcat(intString,intChar);
		}	
		strcat(intString,"\r");
////////// DATA END    //////////////////////////////////////
		return intString;
		}
	
		void intToString(char *array,int x){

		char BaseDigits[10]={'0','1','2','3','4','5','6','7','8','9'};

		if(x<10){
			array[0]=BaseDigits[x];
			array[1]='\0';
		}else if(x<100){
			array[0]=BaseDigits[x/10];
			array[1]=BaseDigits[x%10];
			array[2]='\0';
		}

	}
	
	
}
