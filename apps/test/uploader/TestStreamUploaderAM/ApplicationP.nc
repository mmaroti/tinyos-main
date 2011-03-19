module ApplicationP{
	uses{
		interface Boot;
		interface Leds;
		interface Timer<TMilli>;
		interface Read<uint16_t> as Light;
		interface Read<uint16_t> as Vref;
		interface Read<uint16_t> as Temp;
		interface LocalTime<TMilli>;
		interface StreamStorageWrite;
		interface SplitControl as RadioControl;
	}
}
implementation{
	
	nx_struct{
		nx_uint16_t light;
		nx_uint16_t temp;
		nx_uint16_t vref;
		nx_uint32_t timestamp;
	} data;

	event void Boot.booted(){
		call RadioControl.start();
	}
	
	event void RadioControl.startDone(error_t error){
		if(error!=SUCCESS)
			call RadioControl.start();
		else{
			call Timer.startOneShot(60000L);
		}
	}	

	event void Timer.fired(){
		call Vref.read();
	}
	
	event void Vref.readDone(error_t result, uint16_t val){
		if(result==SUCCESS)
			data.vref=val;
		else
			data.vref=0xffff;
		call Light.read();
	}

	event void Light.readDone(error_t result, uint16_t val){
		if(result==SUCCESS)
			data.light=val;
		else
			data.light=0xffff;
		call Temp.read();
	}

	event void Temp.readDone(error_t result, uint16_t val){
		if(result==SUCCESS)
			data.vref=val;
		else
			data.vref=0xffff;
		data.timestamp=call LocalTime.get();
		call StreamStorageWrite.append(&data, sizeof(data));
	}

	event void StreamStorageWrite.appendDone(void *buf, uint16_t len, error_t error){
		//nothing to do
	}
	
	event void StreamStorageWrite.appendDoneWithID(void *buf, uint16_t len, error_t error){}
	event void StreamStorageWrite.syncDone(error_t error){}
	event void RadioControl.stopDone(error_t error){}
}