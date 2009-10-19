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
		NORMAL=0,
		UNINIT,
		INIT_START,
		INIT,
		FIRST_INIT,
		READ_PENDING_SEEK,
		READ_PENDING_DATA1,
		READ_PENDING_DATA2,
		WRITE_PENDING_ID,
		WRITE_PENDING_DATA1,
		WRITE_PENDING_DATA2,
		WRITE_PENDING_METADATA,
		EREASE_PENDING,
	};
	
	void *writebuffer, *readbuffer;
	uint8_t writelength, firstwritelength, readlength, readfirstlength;
	nx_uint8_t write_id;
	uint32_t buffer;
	nx_uint32_t current_addr;
	uint32_t min_addr,first_addr;
	storage_cookie_t base_offset;
	uint8_t status=UNINIT;
	uint32_t startaddr;
	

//Start/Stop
	
	command error_t SplitControl.start(){
		if(status!=UNINIT)
			return EALREADY;
		status=INIT_START;
		current_addr=call LogWrite.currentOffset();
		#ifdef AT45DB_H
		if(current_addr==256)
		#else
		if(current_addr==0)
		#endif
		{//The flash is empty
			current_addr=1;
			min_addr=0;
			#ifdef AT45DB_H
				base_offset=256;
			#else
				base_offset=0;
			#endif
			status=FIRST_INIT;
			call LogWrite.append(&current_addr, sizeof(current_addr));
		} else{
			call LogRead.seek(SEEK_BEGINNING);
		}
		return SUCCESS;	
	}

	command error_t SplitControl.stop(){
		status=UNINIT;
		signal SplitControl.stopDone(SUCCESS);
		return SUCCESS;
	}
		
//Erease

	command error_t StreamStorage.erease(){
		if(status!=NORMAL){
			if(status==UNINIT)
				return EOFF;
			else
				return EBUSY;
		} else {
			status=EREASE_PENDING;
			call LogWrite.erase();	
			return SUCCESS;
		}
	}
	
	event void LogWrite.eraseDone(error_t error){
		status=NORMAL;
		current_addr=1;
		min_addr=0;
		#ifdef AT45DB_H
			base_offset=256;
		#else
			base_offset=0;
		#endif
		call LogWrite.append(&current_addr, sizeof(current_addr));
	}

//Write
	
	command error_t StreamStorage.append(nx_uint8_t id, void *buf, uint8_t  len){
		if(status!=NORMAL){
			if(status==UNINIT)
				return EOFF;
			else
				return EBUSY;
		} else if(len>16){
			return EINVAL;
		}else{
			writebuffer=buf;
			writelength=len;
			write_id=id+(current_addr+len+1)&256;
			if((call LogWrite.currentOffset())&256>(call LogWrite.currentOffset()+len)&256){
				firstwritelength=256-((call LogWrite.currentOffset())&256);
				if(firstwritelength>0){
					status=WRITE_PENDING_DATA1;				
					call LogWrite.append(&writebuffer, firstwritelength);
				} else {
					status=WRITE_PENDING_METADATA;				
					call LogWrite.append(&current_addr, sizeof(current_addr));
					
				}
			} else {
				firstwritelength=0;
				status=WRITE_PENDING_DATA2;
				call LogWrite.append(&writebuffer, writelength);
			}
			return SUCCESS;
		}
	}
	
	//TODO: hibakezelés, különös tekintettel a félig kiírt adatokra
	event void LogWrite.appendDone(void *buf, storage_len_t len, bool recordsLost, error_t error){
		if(current_addr+len<=current_addr)
			current_addr++;
		current_addr+=len;
		if(error!=SUCCESS){
			if(status!=FIRST_INIT){
				status=NORMAL;
				signal StreamStorage.appendDone(writebuffer, writelength, error);
			} else {
				status=NORMAL;
				signal SplitControl.startDone(FAIL);
			}
			return;
		} 
		switch(status){
			case WRITE_PENDING_DATA1:{
				status=WRITE_PENDING_METADATA;
				call LogWrite.append(&current_addr, sizeof(current_addr));
			}break;
			case WRITE_PENDING_METADATA:{
				status=WRITE_PENDING_DATA2;
				call LogWrite.append(&writebuffer+firstwritelength, writelength-firstwritelength);
			}break;
			case WRITE_PENDING_DATA2:{
				status=WRITE_PENDING_ID;
				call LogWrite.append(&write_id, sizeof(write_id));
			}break;
			case WRITE_PENDING_ID:{
				status=NORMAL;
				signal StreamStorage.appendDone(writebuffer, writelength, SUCCESS);
			}break;
			case FIRST_INIT:{
				status=NORMAL;
				signal SplitControl.startDone(SUCCESS);
			}break;
			case EREASE_PENDING:{
				status=NORMAL;
				signal StreamStorage.ereaseDone(SUCCESS);
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
			storage_cookie_t startaddr_phy;
			startaddr=addr+((addr>>8)+1)*sizeof(current_addr);		//add the size of the metadata
			status=READ_PENDING_SEEK;
			readbuffer=buf;
			readlength=len;
			if(addr>=base_offset)//TODO: a current_addr számláló átfordulását valahogy kezelni
				startaddr_phy=base_offset+startaddr&~256;
			else
				startaddr_phy = current_addr&~256 + 256;
			 
			startaddr_phy+=(startaddr_phy>>8)*sizeof(current_addr);
			call LogRead.seek(startaddr_phy);
			return SUCCESS;
		}	
	}
	
	event void LogRead.readDone(void *buf, storage_len_t len, error_t error){
		storage_cookie_t startaddr_phy=call LogRead.currentOffset()-len+1;
		if(error!=SUCCESS){
			status=NORMAL;
			signal StreamStorage.readDone(readbuffer, readlength, error);
			return;
		}
		switch(status){
			case READ_PENDING_SEEK:{
				if(startaddr-buffer>=252){
					startaddr_phy+=256;//TODO itt lehet több lapot is ugrani, de amíg a current_addr átfordulás nincs kezelve nincs értelme foglalkzni vele
					call LogRead.seek(startaddr_phy);
				} else if(startaddr-buffer<0)//TODO szintén átfordulás
					signal StreamStorage.readDone(readbuffer, readlength, FAIL);
				else{
					startaddr_phy+=startaddr-buffer+sizeof(current_addr);
					if(startaddr_phy&256>(startaddr_phy+readlength)&256){
						readfirstlength=256-(startaddr_phy&256);
						status=READ_PENDING_DATA1;
						call LogRead.seek(startaddr_phy);
					} else {
						readfirstlength=0;
						status=READ_PENDING_DATA2;
						call LogRead.seek(startaddr_phy);
					}	
				}
										
			}break;
			case READ_PENDING_DATA1:{
				status=READ_PENDING_DATA2;
				call LogRead.seek(call LogRead.currentOffset()+sizeof(current_addr));
			}break;
			case READ_PENDING_DATA2:{
				status=NORMAL;
				signal StreamStorage.readDone(readbuffer, readlength, SUCCESS);
			}break;
			case INIT_START:{
				if(buffer!=0xffffffff){
					if(current_addr-call LogRead.currentOffset()+len<=256)
						current_addr=buffer-1+current_addr-call LogRead.currentOffset()+len;
					min_addr=buffer;
					base_offset=call LogRead.currentOffset()-len;
					status=INIT;
					call LogRead.seek(call LogRead.currentOffset()-len+256);				
				}
			}break;
			case INIT:{
				if(buffer!=0xffffffff){
					if(current_addr-call LogRead.currentOffset()+len<=256)
						current_addr=buffer-1+current_addr-call LogRead.currentOffset()+len;
					if((call LogRead.currentOffset()-len)<base_offset){
						status=NORMAL;
						signal SplitControl.startDone(SUCCESS);
					}else {
						if((buffer-min_addr)>256||(min_addr-buffer)>256){
							min_addr=buffer;
							base_offset=call LogRead.currentOffset()-len;
						}
						call LogRead.seek(call LogRead.currentOffset()-len+256);
					}
					
				}
			}break;
		}
	}

	event void LogRead.seekDone(error_t error){
		if(error!=SUCCESS){
			status=NORMAL;
			signal StreamStorage.readDone(readbuffer, readlength, error);
		} else {
			switch(status){
				case READ_PENDING_SEEK:{
					call LogRead.read(&buffer, sizeof(buffer));
				}break;
				case READ_PENDING_DATA2:{
					call LogRead.read(readbuffer+readfirstlength, readlength-readfirstlength);
				}break;
				case READ_PENDING_DATA1:{
					call LogRead.read(readbuffer, readfirstlength);
				}break;
				case INIT:{
					call LogRead.read(&buffer, sizeof(buffer));
				}break;
				case INIT_START:{
					call LogRead.read(&buffer, sizeof(buffer));
				}break;
			}				
		} 
	}

	command uint32_t StreamStorage.getMaxBlockId(){
		return current_addr;
	}

	command uint32_t StreamStorage.getMinBlockId(){
		return min_addr;
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