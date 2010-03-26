module StorageFrameP{
	provides interface StreamStorage as Framed;
	uses interface StreamStorage;
}
implementation{
	enum {
		FRAMEBYTE=0x5e,
		ESCAPEBYTE=0x5d,
		XORESCAPEBYTE=0x20,
	};

	void* writebuf;
	uint16_t writelen;
	int16_t current;
	uint8_t writeid;
	nx_uint16_t buffer;

	command error_t Framed.appendWithID(nx_uint8_t id, void *buf, uint16_t len){
		writelen=len;
		current=-2;
		writeid=id;
		writebuf=buf;
		buffer=FRAMEBYTE;
		call StreamStorage.append(&buffer, 1);
		return SUCCESS;
	}

	command error_t Framed.append(void *buf, uint16_t len){
		writelen=len;
		current=-1;
		buffer=FRAMEBYTE<<8;
		writeid=0;
		writebuf=buf;
		call StreamStorage.append(&buffer, 1);
		return SUCCESS;
	}

	event void StreamStorage.appendDone(void *buf, uint16_t len, error_t error){
		current++;
		if(current==-1){//ID
			if(writeid==ESCAPEBYTE||writeid==FRAMEBYTE){
				buffer=ESCAPEBYTE<<8;
				buffer+=(writeid^XORESCAPEBYTE);
				call StreamStorage.append(&buffer, 2);
			} else{
				call StreamStorage.append(&writeid, 1);
			}
		} else if(current<writelen){
			if(*((uint8_t* )(writebuf+current))==FRAMEBYTE||*((uint8_t* )(writebuf+current))==ESCAPEBYTE){
				buffer=ESCAPEBYTE<<8;
				buffer+=*((uint8_t* )(writebuf+current))^XORESCAPEBYTE;
				call StreamStorage.append(&buffer, 2);	
			} else {
				uint16_t i=0;
				while(i<250&&i+current<writelen&&*((uint8_t* )(writebuf+current+i))!=FRAMEBYTE&&*((uint8_t* )(writebuf+current+i))!=ESCAPEBYTE){
					i++;
				}
				call StreamStorage.append(writebuf+current, i);
				current+=i-1;
			}
		} else if(current==writelen){//closing frame
			buffer=FRAMEBYTE<<8;
			call StreamStorage.append(&buffer, 1);	
		} else {
			if(writeid==0)
				signal Framed.appendDone(writebuf, writelen, SUCCESS);
			else
				signal Framed.appendDoneWithID(writebuf, writelen, SUCCESS);
		}
	}

//from here, we're just forwarding the calls and signals
	event void StreamStorage.appendDoneWithID(void *buf, uint16_t len, error_t error){
	}
	
	command error_t Framed.erase(){
		return call StreamStorage.erase();
	}

	command error_t Framed.sync(){
		return call StreamStorage.sync(); 
	}

	command uint32_t Framed.getMaxAddress(){
		return call StreamStorage.getMaxAddress();
	}

	command error_t Framed.read(uint32_t addr, void *buf, uint8_t len){
		return call StreamStorage.read(addr, buf, len);
	}


	command error_t Framed.getMinAddress(){
		return call StreamStorage.getMinAddress();
	}

	event void StreamStorage.eraseDone(error_t error){
		signal Framed.eraseDone(error);
	}

	event void StreamStorage.syncDone(error_t error){
		signal Framed.syncDone(error);
	}

	event void StreamStorage.readDone(void *buf, uint8_t len, error_t error){
		signal Framed.readDone(buf, len, error);
	}

	event void StreamStorage.getMinAddressDone(uint32_t addr){
		signal Framed.getMinAddressDone(addr);
	}
}