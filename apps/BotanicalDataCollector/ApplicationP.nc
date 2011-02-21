module ApplicationP{
	uses{
		interface Boot;
		interface Leds;
		interface Timer<TMilli>;
		interface Read<uint16_t> as VLight;
		interface Read<uint16_t> as IRLight;
		interface Read<uint16_t> as Temp;
        interface Read<uint16_t> as Humidity;
		interface LocalTime<TMilli>;
		interface StreamStorageWrite;
		interface SplitControl as RadioControl;
		interface LowPowerListening as LPL;
		interface SystemLowPowerListening as SysLPL;		
	}
}
implementation{
	
	nx_struct{
		nx_uint16_t temp;
		nx_uint16_t humidity;
		nx_uint8_t vlight;
        nx_uint8_t irlight;
		nx_uint32_t timestamp;
	} data;

	event void Boot.booted(){
		call RadioControl.start();
	}
	
	event void RadioControl.startDone(error_t error){
		if(error!=SUCCESS)
			call RadioControl.start();
		else{
			call LPL.setLocalWakeupInterval(500);
			call SysLPL.setDefaultRemoteWakeupInterval(500);
			call SysLPL.setDelayAfterReceive(3000);			
			call Timer.startPeriodic(60000L);
		}
	}	

	event void Timer.fired(){
		call Humidity.read();
	}
	
	enum{
      SENSOR_HUMIDITY=0,
      SENSOR_TEMP=1,
      SENSOR_VLIGHT=2,
      SENSOR_IRLIGHT=3,
    };
	
	
	event void Humidity.readDone(error_t result, uint16_t val){
		if(result==SUCCESS)
			data.humidity=val;
		else
			data.humidity=0xffff;
		call Temp.read();
	}


	event void Temp.readDone(error_t result, uint16_t val){
		if(result==SUCCESS)
			data.temp=val;
		else
			data.temp=0xffff;
		call VLight.read();		
	}
	
	 event void VLight.readDone(error_t result, uint16_t val){
        if(result==SUCCESS)
            data.vlight=(uint8_t)val;
        else
            data.vlight=0;
        call IRLight.read();
    }
    
     event void IRLight.readDone(error_t result, uint16_t val){
        if(result==SUCCESS)
            data.irlight=(uint8_t)val;
        else
            data.irlight=0;
        call StreamStorageWrite.append(&data, sizeof(data));
    }

	event void StreamStorageWrite.appendDone(void *buf, uint16_t len, error_t error){
		//nothing to do
	}
	
	event void StreamStorageWrite.appendDoneWithID(void *buf, uint16_t len, error_t error){}
	event void StreamStorageWrite.syncDone(error_t error){}
	event void RadioControl.stopDone(error_t error){}
}