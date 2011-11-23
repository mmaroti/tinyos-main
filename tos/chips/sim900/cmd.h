#define STARTTIME 1000
#define STOPTIME 1000
#define ANSTIMEOUT 5000
#define CONNECT_TIMEOUT 20000
#define DELAY_T 3000

#define UDP_TCP "TCP"
#define APN "NET"
#define USER ""
#define PWD ""
#define IP "WSN.MATH.U-SZEGED.HU"
#define SERVERPORT "9001"


#ifndef functions
#define functions
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

char* toString(uint8_t* toConvert,uint16_t len){
		static char intString[(TOSH_DATA_LENGTH*2)+9];
		char intChar[3];
		uint8_t i;
		
		for(i=0;i<len;i++){
			intToString(intChar,toConvert[i]);
			strcat(intString,intChar);
		}	
		strcat(intString,"\r");
		return intString;	
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
#endif