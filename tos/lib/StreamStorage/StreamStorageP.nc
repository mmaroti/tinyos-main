/*
* Copyright (c) 2009, University of Szeged
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
* Author:Andras Biro
*/

#define DEBUG
#ifdef DEBUG
	#include "printf.h"
#endif
module StreamStorageP{
	provides {
		interface StreamStorage;
		interface SplitControl;
	}
	uses {
		interface LogRead;
		interface LogWrite;
		interface Leds;
	}
}
implementation{
	
	enum {
		UNINIT=0,
		NORMAL,
		INIT_START,
		INIT,
		INIT_STEP,
		READ_PENDING_SEEK,
		READ_PENDING_SEEK_STEP_B,
		READ_PENDING_SEEK_STEP_F,
		READ_PENDING_DATA1,
		READ_PENDING_DATA2,
		WRITE_PENDING_ID,
		WRITE_PENDING_DATA1,
		WRITE_PENDING_DATA2,
		WRITE_PENDING_METADATA,
		ERASE_PENDING,
		ERASE_PENDING_UNINIT,
		#ifdef AT45DB_H
			PAGE_SIZE=254,
			FIRST_DATA=254,
			PAGES=2048,
		#else
			PAGE_SIZE=256,
			FIRST_DATA=0,
			PAGES=2048,
		#endif
	};
	
	void *writebuffer, *readbuffer;
	uint8_t writelength, firstwritelength, readlength, readfirstlength;
	nx_uint8_t write_id;
	uint32_t buffer;
	/*
	 * current_addr: We already written current_addr bytes into the flash (with id, but without metadata)
	 * this should be always correct (even after sync or reset)
	 * we write this number to the first bytes of every page (on the first page, it's 0)
	 */
	nx_uint32_t current_addr; 
	uint8_t status=UNINIT;
	uint32_t readaddress;
	uint8_t current_page;
	

//Start/Stop
	
	command error_t SplitControl.start(){
		if(status!=UNINIT)
			return EALREADY;
		#ifdef DEBUG
			printf("Start; ");
			printfflush();
		#endif
		status=INIT_START;
	

		call LogRead.seek(SEEK_BEGINNING);
		return SUCCESS;	
	}

	command error_t SplitControl.stop(){
		status=UNINIT;
		signal SplitControl.stopDone(SUCCESS);
		return SUCCESS;
	}
		
//Erease

	command error_t StreamStorage.erase(){
		if(status!=NORMAL&&status!=UNINIT){
			return EBUSY;
		} else {
			if(status==NORMAL)
				status=ERASE_PENDING;
			else
				status=ERASE_PENDING_UNINIT;
			call LogWrite.erase();	
			return SUCCESS;
		}
	}
	
	event void LogWrite.eraseDone(error_t error){
		#ifdef DEBUG
			printf("Erase done\n");
			printfflush();
		#endif	
		if(error==SUCCESS){
			current_addr=0;
		}
		if(status==ERASE_PENDING)
			status=NORMAL;
		else
			status=UNINIT;	
		signal StreamStorage.eraseDone(error);
	}

//Write
	command error_t StreamStorage.append(void *buf, uint8_t len){
//		#ifdef DEBUG
//			printf("Start append %d long, status=%d\n", len,status);
//			printfflush();
//		#endif	
		if(status!=NORMAL){
			if(status==UNINIT)
				return EOFF;
			else
				return EBUSY;
		} else if(len>PAGE_SIZE-5){
			return EINVAL;
		}else{
			writebuffer=buf;
			writelength=len;
			write_id=0;
//			#ifdef DEBUG
//				printf("@%ld#%ld:%ld\n",call LogWrite.currentOffset(),(call LogWrite.currentOffset())/PAGE_SIZE,(call LogWrite.currentOffset()+len)/PAGE_SIZE);
//				printfflush();
//			#endif	
			if((((call LogWrite.currentOffset())/PAGE_SIZE)<((call LogWrite.currentOffset()+len-1)/PAGE_SIZE))||((call LogWrite.currentOffset())%PAGE_SIZE)==0){//data is overlapping to the next page, or we're on the first byte of the page
				firstwritelength=(PAGE_SIZE-((call LogWrite.currentOffset())%PAGE_SIZE))%PAGE_SIZE;
				if(firstwritelength>0){//if we had any space on this page, fill it
					status=WRITE_PENDING_DATA1;				
					call LogWrite.append(writebuffer, firstwritelength);
				} else {//otherwise we start with the metadata on the next page
					status=WRITE_PENDING_METADATA;				
					call LogWrite.append(&current_addr, sizeof(current_addr));
					
				}
			} else {
				firstwritelength=0;
				status=WRITE_PENDING_DATA2;
				call LogWrite.append(writebuffer, writelength);
			}
			return SUCCESS;
		}
	}
	command error_t StreamStorage.appendWithID(nx_uint8_t id, void *buf, uint8_t  len){
//		#ifdef DEBUG
//			printf("Start append %d long, status=%d\n", len,status);
//			printfflush();
//		#endif	
		if(status!=NORMAL){
			if(status==UNINIT)
				return EOFF;
			else
				return EBUSY;
		} else if(len>PAGE_SIZE-5){
			return EINVAL;
		}else{
			writebuffer=buf;
			writelength=len;
			write_id=id;
//			#ifdef DEBUG
//				printf("@%ld#%ld:%ld\n",call LogWrite.currentOffset(),(call LogWrite.currentOffset())/PAGE_SIZE,(call LogWrite.currentOffset()+len)/PAGE_SIZE);
//				printfflush();
//			#endif	
			if((((call LogWrite.currentOffset())/PAGE_SIZE)<((call LogWrite.currentOffset()+len)/PAGE_SIZE))||((call LogWrite.currentOffset())%PAGE_SIZE)==0){//data is overlapping to the next page, or we're on the first byte of the page
				firstwritelength=(PAGE_SIZE-((call LogWrite.currentOffset())%PAGE_SIZE))%PAGE_SIZE;
				
				if(firstwritelength>0){//if we had any space on this page, fill it
					status=WRITE_PENDING_DATA1;				
					call LogWrite.append(writebuffer, firstwritelength);
				} else {//otherwise we start with the metadata on the next page
					status=WRITE_PENDING_METADATA;				
					call LogWrite.append(&current_addr, sizeof(current_addr));
					
				}
			} else {
				firstwritelength=0;
				status=WRITE_PENDING_DATA2;
				call LogWrite.append(writebuffer, writelength);
			}
			return SUCCESS;
		}
	}
	
	//TODO: what should we do with half written data?
	event void LogWrite.appendDone(void *buf, storage_len_t len, bool recordsLost, error_t error){
//		#ifdef DEBUG
//			uint8_t i;
//			for(i=0;i<len;i++){
//				printf("%ld@%ld: %d\n",current_addr,(call LogWrite.currentOffset()-len+i+1),*((uint8_t*)buf+i));
//			}
//			printf("\n");
//			printfflush();
//		#endif
		if(status!=WRITE_PENDING_METADATA)
			current_addr+=len;
		if(error!=SUCCESS){
			status=NORMAL;
			if(write_id>0)
				signal StreamStorage.appendDoneWithID(writebuffer, writelength, error);
			else
				signal StreamStorage.appendDone(writebuffer, writelength, error);
			return;
		} 
		switch(status){
			case WRITE_PENDING_DATA1:{//we're done with the first page, now we write the pagestarter metadata
				status=WRITE_PENDING_METADATA;
				call LogWrite.append(&current_addr, sizeof(current_addr));
			}break;
			case WRITE_PENDING_METADATA:{//we're done with the metadata, now we write the rest of the data, or the ID	
				if(writelength==firstwritelength&&write_id>0)
				{
					status=NORMAL;
					signal StreamStorage.appendDone(writebuffer, writelength, SUCCESS);
				}else {
					status=WRITE_PENDING_DATA2;
					call LogWrite.append(writebuffer+firstwritelength, writelength-firstwritelength);
				}
				printfflush();
			}break;
			case WRITE_PENDING_DATA2:{//we're done with the data, now we write the ID
				if(write_id>0){
					status=WRITE_PENDING_ID;
					call LogWrite.append(&write_id, sizeof(write_id));
				} else {
					status=NORMAL;
					signal StreamStorage.appendDone(writebuffer, writelength, SUCCESS);
				}
			}break;
			case WRITE_PENDING_ID:{//we wrote everything
				status=NORMAL;
				signal StreamStorage.appendDoneWithID(writebuffer, writelength, SUCCESS);
			}break;
		}
	}	
//Read


	command error_t StreamStorage.read(uint32_t addr, void *buf, uint8_t len){
		if(status!=NORMAL)
			if(status==UNINIT)
				return EOFF;
			else
				return EBUSY;
		else {
			readaddress=addr;		//add the size of the metadata
			status=READ_PENDING_SEEK;
			readbuffer=buf;
			readlength=len;
			//we will read the first metadata, to know where should we search
			call LogRead.seek(SEEK_BEGINNING);
			return SUCCESS;
		}	
	}
	

	
	event void LogRead.readDone(void *buf, storage_len_t len, error_t error){
		nx_uint32_t *metadata=(nx_uint32_t*)buf;//unfortunately, we need this, because the endiannes is unpredictable in the buffer
		if(error!=SUCCESS){
			switch(status){
				case INIT_STEP:{
					#ifdef DEBUG
							printf("Found end of data, %ld\n",current_addr);
							printfflush();
					#endif	
					status=NORMAL;
					signal SplitControl.startDone(SUCCESS);
				}break;
				case READ_PENDING_DATA1:{//maybe because the page was synced. try the next page
					#ifdef DEBUG
							printf("Read error, go to %d\n",(current_page+1)*PAGE_SIZE);
					#endif
					status=READ_PENDING_SEEK_STEP_F;
					call LogRead.seek(((storage_cookie_t)current_page+1)*PAGE_SIZE);	
				}break;
				case READ_PENDING_DATA2:{//maybe because the page was synced. try the next page
					#ifdef DEBUG
							printf("Read error, go to %d\n",(current_page+1)*PAGE_SIZE);
					#endif
					status=READ_PENDING_SEEK_STEP_F;
					call LogRead.seek(((storage_cookie_t)current_page+1)*PAGE_SIZE);	
				}break;
				default:{
					status=NORMAL;
					#ifdef DEBUG
							printf("Read error\n");
							printfflush();
					#endif	
					signal StreamStorage.readDone(readbuffer, readlength, error);
					return;
				}break;
			}
		}else{ 
			switch(status){
				case READ_PENDING_SEEK:{
					storage_cookie_t startaddr_phy=call LogRead.currentOffset()-len;
					#ifdef DEBUG
						printf("Metadata at %ld: %ld\n",call LogRead.currentOffset()-len,*metadata);
					#endif
					current_page=(uint8_t)((call LogRead.currentOffset())/PAGE_SIZE);
					if((readaddress-*metadata<=(PAGE_SIZE-sizeof(current_addr)))&&(readaddress-*metadata>=0)){
						#ifdef DEBUG
							printf("Data is on this page\n");
						#endif
						startaddr_phy+=(storage_cookie_t)(readaddress-*metadata+len);//jump to the data
						if((startaddr_phy%PAGE_SIZE)>((startaddr_phy+readlength)%PAGE_SIZE)){ //the data was cut half with metadata
							readfirstlength=PAGE_SIZE-(startaddr_phy%PAGE_SIZE);
							status=READ_PENDING_DATA1;
						} else {
							readfirstlength=0;
							status=READ_PENDING_DATA2;
						}	
					}else{
						if(readaddress-*metadata>0){//forward
							status=READ_PENDING_SEEK_STEP_F;
							//													 	kulombseg		  +				metadata									/PAGE_SIZE egeszresz				
							startaddr_phy=startaddr_phy+(storage_cookie_t)((readaddress-*metadata+((readaddress-*metadata)/PAGE_SIZE)*sizeof(current_addr))/PAGE_SIZE)*PAGE_SIZE;
							#ifdef DEBUG
								printf("Forward\n");
							#endif
						}else{//backward	//TODO testing
							status=READ_PENDING_SEEK_STEP_B;
							startaddr_phy=startaddr_phy-(storage_cookie_t)((*metadata-readaddress+((*metadata-readaddress)/PAGE_SIZE)*sizeof(current_addr))/PAGE_SIZE+1)*PAGE_SIZE;					
							#ifdef DEBUG
								printf("Backward\n");
							#endif
						}				
						if(startaddr_phy>=(uint32_t)PAGE_SIZE*PAGES){//invalid address, maybe current_addr overflow
							//TODO overflow handling
							#ifdef DEBUG
								printf("Overflow detected\n");
							#endif
						}
					}
					#ifdef DEBUG
						printf("Jump to %ld\n",startaddr_phy);
						printfflush();
					#endif
					call LogRead.seek(startaddr_phy);
				}break;
				case READ_PENDING_SEEK_STEP_F:{
					storage_cookie_t startaddr_phy=call LogRead.currentOffset()-len;
					current_page=(uint8_t)((call LogRead.currentOffset())/PAGE_SIZE);
					#ifdef DEBUG
						printf("Metadata at %ld: %ld\n",call LogRead.currentOffset()-len,*metadata);
						printfflush();
					#endif
					if((readaddress-*metadata)>=(PAGE_SIZE-len)){//the data will be somewhere on the next pages
						#ifdef DEBUG
							printf("Next page\n");
							printfflush();
						#endif
						startaddr_phy+=PAGE_SIZE;
						call LogRead.seek(startaddr_phy);
					} else if(readaddress-*metadata<0)//seems like the data is somewhere before us (which should be impossible), or not in the flash (overwritten?)
						signal StreamStorage.readDone(readbuffer, readlength, FAIL);
					else{//the data is on this page
						#ifdef DEBUG
							printf("Data is on this page\n");
							printfflush();
						#endif
						startaddr_phy+=(storage_cookie_t)(readaddress-*metadata+len);//jump to the data
						if((startaddr_phy%PAGE_SIZE)>((startaddr_phy+readlength)%PAGE_SIZE)){ //the data was cut half with metadata
							readfirstlength=PAGE_SIZE-(startaddr_phy%PAGE_SIZE);
							status=READ_PENDING_DATA1;
						} else {
							readfirstlength=0;
							status=READ_PENDING_DATA2;
						}
						#ifdef DEBUG
							printf("Jump to %ld\n",startaddr_phy);
							printfflush();
						#endif
						call LogRead.seek(startaddr_phy);
					}
											
				}break;
				case READ_PENDING_SEEK_STEP_B:{
					storage_cookie_t startaddr_phy=call LogRead.currentOffset()-len;
					current_page=(uint8_t)((call LogRead.currentOffset())/PAGE_SIZE);
					#ifdef DEBUG
						printf("Metadata at %ld: %ld\n",call LogRead.currentOffset()-len,*metadata);
					#endif
					if(readaddress-*metadata<0){//the data will be somewhere on the previous pages
						#ifdef DEBUG
							printf("Prev page\n");
							printfflush();
						#endif
						startaddr_phy-=PAGE_SIZE;
						call LogRead.seek(startaddr_phy);
					} else if(readaddress-*metadata>=PAGE_SIZE+len)//seems like the data is somewhere ahead us (which should be impossible), or not in the flash (overwritten?) 
						signal StreamStorage.readDone(readbuffer, readlength, FAIL);
					else{//the data is on this page
						#ifdef DEBUG
							printf("Data is on this page\n");
							printfflush();
						#endif
						startaddr_phy+=(storage_cookie_t)(readaddress-*metadata+len);//jump to the data
						if((startaddr_phy%PAGE_SIZE)>((startaddr_phy+readlength)%PAGE_SIZE)){ //the data was cut half with metadata
							readfirstlength=PAGE_SIZE-(startaddr_phy%PAGE_SIZE);
							status=READ_PENDING_DATA1;
						} else {
							readfirstlength=0;
							status=READ_PENDING_DATA2;
						}
						call LogRead.seek(startaddr_phy);
					}
											
				}break;
				case READ_PENDING_DATA1:{//we read the first half of the data, now we jump over the metadata
					status=READ_PENDING_DATA2;
					call LogRead.seek(call LogRead.currentOffset()+sizeof(current_addr));
				}break;
				case READ_PENDING_DATA2:{//we're done
					status=NORMAL;
					signal StreamStorage.readDone(readbuffer, readlength, SUCCESS);
				}break;
				case INIT_START:{//we read the first metadata
					#ifdef DEBUG
						printf("Current addr: %ld; Buffer: %ld\n",call LogRead.currentOffset()-sizeof(buffer),*metadata);
						printfflush();
					#endif	
					current_addr=*metadata;
					current_page=(uint8_t)((call LogRead.currentOffset())/PAGE_SIZE);
					status=INIT;
					call LogRead.seek((current_page+1)*PAGE_SIZE);				
				}break;
				case INIT:{//we read all of the metadata, searching for the last page
	//				#ifdef DEBUG
	//					printf("Current addr: %ld; Buffer: %ld\n",call LogRead.currentOffset()-sizeof(buffer),*metadata);
	//					printfflush();
	//				#endif	
					if(*metadata==0){//then it's empty, so we should start on this page
						status=NORMAL;
						signal SplitControl.startDone(SUCCESS);
					}
					//unfortunatly we can read the first byte of every page, but if it's unwritten, the buffer doesn't change (in this case: current_addr==*metadata)
					if(((*metadata-current_addr)<=PAGE_SIZE)&&(current_addr!=*metadata)){//than it's a correct page. we should check the next one
						current_page=(uint8_t)((call LogRead.currentOffset())/PAGE_SIZE);
						current_addr=*metadata;
						call LogRead.seek((current_page+1)*PAGE_SIZE);	
					} else {
						#ifdef DEBUG
							printf("Unexpected metadata seek to %d\n",current_page*PAGE_SIZE);
							printfflush();
						#endif
						status=INIT_STEP;
						call LogRead.seek(current_page*PAGE_SIZE);
					}
				}break;
				case INIT_STEP:{
					if(((current_page+1)%256==(uint8_t)(call LogRead.currentOffset()%256))&&(call LogRead.currentOffset()%254!=0)){//the address increased by one (and didn't jump to the next page), and it's not the end of the page
						current_page++;
						current_addr++;
	//					#ifdef DEBUG
	//								printf("Found valid data at %ld\n",call LogRead.currentOffset());
	//								printfflush();
	//					#endif	
						call LogRead.read(&buffer,1);
					} else{
						if(call LogRead.currentOffset()%PAGE_SIZE!=0)
							current_addr--; //the last byte of a synced page was alway false. TODO NEED FURTHER TESTING!
						
						#ifdef DEBUG
								printf("Found end of data at %ld\n",current_addr);
								printfflush();
						#endif
						if(current_addr<2){//the flash is empty
							#ifdef DEBUG
								printf("The flash is empty\n");
								printfflush();
							#endif		
							current_addr=0;
							status=NORMAL;
							signal SplitControl.startDone(SUCCESS);
							//call LogWrite.append(&current_addr, sizeof(current_addr));//we write the first metadata			
						} else{
							current_addr-=4;
							status=NORMAL;
							signal SplitControl.startDone(SUCCESS);
						}	
					}
				}break;
			}
		}
	}

	event void LogRead.seekDone(error_t error){
		if(error!=SUCCESS){
			switch(status){
				case INIT_START:{
					#ifdef DEBUG
						printf("seek to beginning failed\n");
						printfflush();
					#endif	
					signal SplitControl.startDone(FAIL);
				}break;
				case INIT:{				//probably becouse it's unwritten
					printf("seek error (%d), seek to %d\n",error,current_page*PAGE_SIZE);
					status=INIT_STEP;
					call LogRead.seek(current_page*PAGE_SIZE);
				}break;
				case INIT_STEP:{
					#ifdef DEBUG
							printf("Found end of data, %ld\n",current_addr);
							printfflush();
					#endif
					status=NORMAL;
					signal SplitControl.startDone(SUCCESS);	
				}break;
				default:{
					status=NORMAL;
					signal StreamStorage.readDone(readbuffer, readlength, error);
				}
			}	
		} else {
			switch(status){
				case READ_PENDING_DATA2:{
					call LogRead.read(readbuffer+readfirstlength, readlength-readfirstlength);
				}break;
				case READ_PENDING_DATA1:{
					call LogRead.read(readbuffer, readfirstlength);
				}break;
				case INIT_STEP:{
					current_page=(uint8_t)(call LogRead.currentOffset()%256);//reusing variable
					call LogRead.read(&buffer,1);
				}break;
				default:{
					call LogRead.read(&buffer, sizeof(buffer));
				}break;
			}				
		} 
	}

	command uint32_t StreamStorage.getMaxBlockId(){
		return current_addr;
	}

	command uint32_t StreamStorage.getMinBlockId(){
		return 0;
	}
	

	event void LogWrite.syncDone(error_t error){
		signal StreamStorage.syncDone(error);	
	}

	command error_t StreamStorage.sync(){
		if(status!=NORMAL)
			if(status==UNINIT)
				return EOFF;
			else
				return EBUSY;
		else {
			return call LogWrite.sync();
		}
	}
}
