#define UQ_RADIO_SCH "RadioScheduler.RadioStart"
module RadioSchedulerP{
	provides interface RadioScheduler[uint8_t radio_id];
	uses {
		interface SplitControl;
		interface LocalTime<TMilli>;
	} 
}
implementation{
	enum {
    	USERNUM = uniqueCount(UQ_RADIO_SCH),
    	FLAG_BUSY = 1,
    	FLAG_TURNEDON = 2,
  	};
  	
  	typedef struct {
  		uint32_t eventTime;
  		uint8_t flags;
  	} radiouser;
  	
  	radiouser users[USERNUM];  	
  	
  	uint8_t nextEventUser;
  	bool radioon=FALSE;
  	bool nextEventIsReal;
  	 	
	inline void processTiming(){
		uint8_t i;
		nextEventUser=USERNUM+1;
		for(i=0;i<USERNUM;i++){
			if(users[i].flags&FLAG_BUSY>0){
				if(nextEventUser==USERNUM+1)
					nextEventUser=i;
				else if(users[i].eventTime<users[nextEventUser].eventTime)
					nextEventUser=i;					
			}
		}
		if(radioon){
			if(users[nextEventUser].flags&FLAG_TURNEDON==0)//radio is on, and we want to turn it on
				nextEventIsReal=FALSE;
			else{
			//radio is on, we want to turn it off, but maybe other users still using it
			}
			//radio is on, we want to turn it off, but maybe other users still using it
			//otherwise, the event is real
		}
	}  	 	
  	 	
	event void SplitControl.stopDone(error_t error){
		users[nextEventUser].flags&=~FLAG_TURNEDON;
		users[nextEventUser].flags&=~FLAG_BUSY;//!busy, !turnedon
		radioon=FALSE;
		signal RadioScheduler.RadioStopDone[nextEventUser]();
		processTiming();
	}

	event void SplitControl.startDone(error_t error){
		users[nextEventUser].flags|=FLAG_TURNEDON;
		users[nextEventUser].flags&=~FLAG_BUSY;//!busy, turnedon
		radioon=TRUE;
		signal RadioScheduler.RadioStartDone[nextEventUser]();
		processTiming();
	}
	
	
	
	command error_t RadioScheduler.RadioStart[uint8_t radio_id](uint32_t when){
		if(users[radio_id].flags&FLAG_BUSY>0)
			return EBUSY;
		if(users[radio_id].flags&FLAG_TURNEDON>0)
			return EOFF;
			
		users[radio_id].flags|=FLAG_BUSY;//busy, !turnedon
		users[radio_id].eventTime=when;
		
		processTiming();		
		return SUCCESS;
	}

	command error_t RadioScheduler.RadioStartCancel[uint8_t radio_id](){
		if(users[radio_id].flags&FLAG_BUSY>0&&users[radio_id].flags&FLAG_TURNEDON==0){
			users[radio_id].flags&=~FLAG_BUSY;//!busy, !turnedon
			processTiming();
			return SUCCESS;
		} else {
			return EALREADY;	
		}
	}
	
	command error_t RadioScheduler.RadioStop[uint8_t radio_id](uint32_t when){
		if(users[radio_id].flags&FLAG_TURNEDON==0)
				return EOFF;
		if(users[radio_id].flags&FLAG_BUSY==0)
				return EBUSY;
		
		users[radio_id].flags|=FLAG_BUSY;//busy, turnedon
		users[radio_id].eventTime=when;
		
		processTiming();
		return SUCCESS;
	}
	
	command error_t RadioScheduler.RadioStopCancel[uint8_t radio_id](){
		if(users[radio_id].flags&FLAG_BUSY>0&&users[radio_id].flags&FLAG_TURNEDON>0){
			users[radio_id].flags&=~FLAG_BUSY;//!busy, !turnedon
			processTiming();
			return SUCCESS;
		} else {
			return EALREADY;	
		}
	}	
}