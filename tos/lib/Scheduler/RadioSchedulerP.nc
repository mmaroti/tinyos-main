#define UQ_RADIO_SCH "RadioScheduler.RadioStart"
module RadioSchedulerP{
	provides interface RadioScheduler[uint8_t radio_id];
	uses {
		interface SplitControl;
		interface LocalTime<TMilli>;
		interface Timer<TMilli>;
	} 
}
implementation{
	enum {
    	USERNUM = uniqueCount(UQ_RADIO_SCH),
    	FLAG_BUSY = 1,//1, if there's event for the user
    	FLAG_TURNEDON = 2,//0, if the user doesn't need the device (between stopDone and startDone)
  	};
  	
  	typedef struct {
  		uint32_t eventTime;
  		uint8_t flags;
  	} radiouser;
  	
  	radiouser users[USERNUM];  	
  	
  	uint8_t nextEventUser;
  	bool radioon=FALSE;
  	bool nextEventIsReal;
  	 	
	inline void signalEvent(uint8_t user);
		  	 	
	inline void processTiming(){
		uint8_t i;
		uint32_t locTime=call LocalTime.get();
		nextEventUser=USERNUM;
		
		for(i=0;i<USERNUM;i++){
			if(users[i].flags&FLAG_BUSY>0){
				if(nextEventUser==USERNUM||(int32_t)(users[i].eventTime-locTime)<(int32_t)(users[nextEventUser].eventTime-locTime))
					nextEventUser=i;					
			}
		}
		if(nextEventUser==USERNUM)//there are no events
			return;
		nextEventIsReal=TRUE;
		if(radioon){
			if(users[nextEventUser].flags&FLAG_TURNEDON==0)//radio is on, and we want to turn it on
				nextEventIsReal=FALSE;
			else{//radio is on, we want to turn it off, but maybe other users still using it
				for(i=0;i<USERNUM;i++){
					if(users[i].flags&FLAG_BUSY==0&&users[i].flags&FLAG_TURNEDON!=0){ 
						nextEventIsReal=FALSE;
						break;
					} else if(users[i].flags&FLAG_BUSY!=0&&users[i].flags&FLAG_TURNEDON!=0){
						if((int32_t)(users[i].eventTime-locTime)>(int32_t)(users[nextEventUser].eventTime-locTime)){
							nextEventIsReal=FALSE;
							break;
						}
					}
				}
			}
			//otherwise, the event is real
			
			locTime=call LocalTime.get();
			if((int32_t)(users[nextEventUser].eventTime-locTime)<0){
				signalEvent(nextEventUser);
			} else {
				call Timer.startOneShotAt(users[nextEventUser].eventTime, 0);
			}
		}
	}  	
	
	event void Timer.fired(){
		signalEvent(nextEventUser);
	}
	
	inline void signalEvent(uint8_t user){
		
		if(nextEventUser==user&&nextEventIsReal){
			if(users[user].flags&FLAG_TURNEDON!=0)
				call SplitControl.stop();
			else
				call SplitControl.start();
		} else if(nextEventUser!=user) {//cancel
			if(users[user].flags&FLAG_TURNEDON!=0){
				users[user].flags&=~FLAG_BUSY;//!busy, turnedon
				signal RadioScheduler.RadioStopDone[user](ECANCEL);
			}else{
				users[user].flags&=~FLAG_BUSY;//!busy, !turnedon
				signal RadioScheduler.RadioStartDone[user](ECANCEL);
			}
			processTiming();
		} else { //just signaling success
			if(users[user].flags&FLAG_TURNEDON!=0){
				users[nextEventUser].flags&=~FLAG_TURNEDON;
				users[nextEventUser].flags&=~FLAG_BUSY;//!busy, !turnedon
				signal RadioScheduler.RadioStopDone[user](SUCCESS);
			}else{
				users[nextEventUser].flags|=FLAG_TURNEDON;
				users[nextEventUser].flags&=~FLAG_BUSY;//!busy, turnedon
				signal RadioScheduler.RadioStartDone[user](SUCCESS);
			}
			processTiming();
		}
	} 	
  	 	
	event void SplitControl.stopDone(error_t error){
		if(error!=SUCCESS)
			call SplitControl.stop();
		else {
			users[nextEventUser].flags&=~FLAG_TURNEDON;
			users[nextEventUser].flags&=~FLAG_BUSY;//!busy, !turnedon
			radioon=FALSE;
			signal RadioScheduler.RadioStopDone[nextEventUser](SUCCESS);
			processTiming();
		}
	}

	event void SplitControl.startDone(error_t error){
		if(error!=SUCCESS)
			call SplitControl.start();
		else {
			users[nextEventUser].flags|=FLAG_TURNEDON;
			users[nextEventUser].flags&=~FLAG_BUSY;//!busy, turnedon
			radioon=TRUE;
			signal RadioScheduler.RadioStartDone[nextEventUser](SUCCESS);
			processTiming();
		}
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
			
			signalEvent(radio_id);
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
			signalEvent(radio_id);
			return SUCCESS;
		} else {
			return EALREADY;	
		}
	}	

	
}