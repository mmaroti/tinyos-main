#include "Scheduler.h"
//#include "printf.h"
module RadioSchedulerP{
	provides interface RadioScheduler[uint8_t radio_id];
	uses {
		interface SplitControl;
		interface LocalTime<TMilli>;
		interface Timer<TMilli> as SignalTimer;
		interface Leds;
	} 
}
implementation{
	enum {
    	USERNUM = uniqueCount(UQ_RADIO_SCH),
    	FLAG_BUSY = 1,//1, if there's event for the user
    	FLAG_TURNEDON = 2,//0, if the user doesn't need the device (between stopDone and startDone)
    	FLAG_CANCELLED=4,
    	RADIO_ON_DELAY=2,
  	};
  	
  	typedef struct {
  		uint32_t eventTime;
  		uint8_t flags;
  	} radiouser;
  	
  	radiouser users[USERNUM];  	
  	
  	uint8_t nextEventUser;
  	bool radioon=FALSE;
  	bool nextEventIsReal;
  	 	
	inline void signalEvent();
		  	 	
	task void processTiming(){
		uint8_t i;
		uint32_t locTime=call LocalTime.get();
		call SignalTimer.stop();
		nextEventUser=USERNUM;
		
		for(i=0;i<USERNUM;i++){
			if((users[i].flags&FLAG_BUSY)>0){
				if((users[i].flags&FLAG_CANCELLED)>0){
					users[i].flags&=~FLAG_BUSY;//!busy, turnedon
					users[i].flags&=~FLAG_CANCELLED;
					if((users[i].flags&FLAG_TURNEDON)!=0){
						signal RadioScheduler.RadioStopDone[i](ECANCEL);
					}else{
						signal RadioScheduler.RadioStartDone[i](ECANCEL);
					}
				} else if(nextEventUser==USERNUM||(int32_t)(users[i].eventTime-locTime)<(int32_t)(users[nextEventUser].eventTime-locTime)){
					nextEventUser=i;					
				}
			}
		}
		if(nextEventUser==USERNUM)//there are no events
			return;
		nextEventIsReal=TRUE;
		if(radioon){		
			if((users[nextEventUser].flags&FLAG_TURNEDON)==0)//radio is on, and we want to turn it on
				nextEventIsReal=FALSE;
			else{//radio is on, we want to turn it off, but maybe other users still using it
				for(i=0;i<USERNUM;i++){
					if(i!=nextEventUser&&(users[i].flags&FLAG_TURNEDON)!=0){ 
						nextEventIsReal=FALSE;
						break;
					} 
				}
			}
		}
//		nextEventIsReal?printf("SCH:Next event: from %d, at %ld (now:%ld)",nextEventUser,users[nextEventUser].eventTime,locTime):
//		printf("SCH:Next sim event: from %d, at %ld (now:%ld)",nextEventUser,users[nextEventUser].eventTime,locTime);
//			(users[nextEventUser].flags&FLAG_TURNEDON)!=0?printf(" OFF\n"):printf(" ON\n");
//		printfflush();
		//otherwise, the event is real
		//if we will turn on the radio, we substract  the switching delay from the waiting time
		if((int32_t)(users[nextEventUser].eventTime-(!radioon&&nextEventIsReal)?RADIO_ON_DELAY:0-locTime)<0){
			signalEvent();
		} else {
			call SignalTimer.startOneShotAt(locTime, users[nextEventUser].eventTime-(!radioon&&nextEventIsReal)?RADIO_ON_DELAY:0-locTime-locTime);
		}
	}  	
	
	event void SignalTimer.fired(){
		if((users[nextEventUser].flags&FLAG_CANCELLED)==0)
			signalEvent();
	}
	
	task void StopSplitControl(){
		if(call SplitControl.stop()!=SUCCESS)
			post StopSplitControl();
	}
	
	task void StartSplitControl(){
		if(call SplitControl.start()!=SUCCESS)
			post StartSplitControl();
	}
	
	inline void signalEvent(){
		if(nextEventIsReal){
			if((users[nextEventUser].flags&FLAG_TURNEDON)!=0){
				if(call SplitControl.stop()!=SUCCESS){
					post StopSplitControl();
				}
//				printf("OFF by %d at %ld\n",nextEventUser,call LocalTime.get());
//				printfflush();
			}else{
				if(call SplitControl.start()!=SUCCESS){
					post StartSplitControl();
				}
//				printf("ON by %d at %ld\n",nextEventUser, call LocalTime.get());
//				printfflush();
			}
		} else { //just signaling success
			if((users[nextEventUser].flags&FLAG_TURNEDON)!=0){
				users[nextEventUser].flags&=~FLAG_TURNEDON;
				users[nextEventUser].flags&=~FLAG_BUSY;//!busy, !turnedon
				signal RadioScheduler.RadioStopDone[nextEventUser](SUCCESS);
			}else{
				users[nextEventUser].flags|=FLAG_TURNEDON;
				users[nextEventUser].flags&=~FLAG_BUSY;//!busy, turnedon
				signal RadioScheduler.RadioStartDone[nextEventUser](SUCCESS);
			}
			post processTiming();
		}
	} 	
  	 	
	event void SplitControl.stopDone(error_t error){
		if(error!=SUCCESS)
			call SplitControl.stop();
		else {
			call Leds.led0Off();
			users[nextEventUser].flags&=~FLAG_TURNEDON;
			users[nextEventUser].flags&=~FLAG_BUSY;//!busy, !turnedon
			radioon=FALSE;
			signal RadioScheduler.RadioStopDone[nextEventUser](SUCCESS);
			post processTiming();
		}
	}

	event void SplitControl.startDone(error_t error){
		if(error!=SUCCESS)
			call SplitControl.start();
		else {
			call Leds.led0On();
			users[nextEventUser].flags|=FLAG_TURNEDON;
			users[nextEventUser].flags&=~FLAG_BUSY;//!busy, turnedon
			radioon=TRUE;
			signal RadioScheduler.RadioStartDone[nextEventUser](SUCCESS);
			post processTiming();
		}
	}
	
	
	
	command error_t RadioScheduler.RadioStart[uint8_t radio_id](uint32_t when){
		if((users[radio_id].flags&FLAG_BUSY)>0)
			return EBUSY;
		if((users[radio_id].flags&FLAG_TURNEDON)>0)
			return EOFF;
//		printf("SCH: start req: #%d@%ld\n",radio_id, when);
//		printfflush();
		users[radio_id].flags|=FLAG_BUSY;//busy, !turnedon
		users[radio_id].eventTime=when;
		post processTiming();
		return SUCCESS;
	}

	command error_t RadioScheduler.RadioStartCancel[uint8_t radio_id](){
		if((users[radio_id].flags&FLAG_BUSY)>0&&(users[radio_id].flags&FLAG_TURNEDON)==0&&(users[radio_id].flags&FLAG_CANCELLED)==0){
			users[radio_id].flags|=FLAG_CANCELLED;
			post processTiming();
			return SUCCESS;
		} else {
			return EALREADY;	
		}
	}
	
	command error_t RadioScheduler.RadioStop[uint8_t radio_id](uint32_t when){
//		printf("stop req from %d ",radio_id);
		if((users[radio_id].flags&FLAG_TURNEDON)==0){
//				printf("denied, off already\n");
//				printfflush();
			return EOFF;
		}
		if((users[radio_id].flags&FLAG_BUSY)!=0){
//				printf("denied, busy\n");
//				printfflush();
				return EBUSY;
				
		}
//		printf("accepted\n");
//		printf("SCH: stop req: #%d@%ld\n",radio_id, when);
//		printfflush();
		users[radio_id].flags|=FLAG_BUSY;//busy, turnedon
		users[radio_id].eventTime=when;
		post processTiming();
		return SUCCESS;
	}
	
	command error_t RadioScheduler.RadioStopCancel[uint8_t radio_id](){
		if((users[radio_id].flags&FLAG_BUSY)>0&&(users[radio_id].flags&FLAG_TURNEDON)>0&&(users[radio_id].flags&FLAG_CANCELLED)==0){
			users[radio_id].flags|=FLAG_CANCELLED;
			post processTiming();
			return SUCCESS;
		} else {
			return EALREADY;	
		}
	}	

	command bool RadioScheduler.IsStarted[uint8_t radio_id](){
		if((users[radio_id].flags&FLAG_TURNEDON)>0)
			return TRUE;
		else
			return FALSE;
	}
	
	default event void RadioScheduler.RadioStopDone[uint8_t user](error_t error) { }
	
	default event void RadioScheduler.RadioStartDone[uint8_t user](error_t error) { }
		
}