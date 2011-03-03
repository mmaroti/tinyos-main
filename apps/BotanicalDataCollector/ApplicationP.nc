#ifndef MEASURE_INTERVAL
  #define MEASURE_INTERVAL 60
#endif
#ifndef TIMESYNC_INTERVAL
  #define TIMESYNC_INTERVAL 600
#endif
module ApplicationP{
  uses{
    interface Boot;
    interface Leds;
    interface Timer<TMilli>;
    interface Read<uint8_t> as VLight;
    interface Read<uint8_t> as IRLight;
    interface Read<uint16_t> as Temp;
    interface Read<uint16_t> as Humidity;
    interface LocalTime<TMilli>;
    interface StreamStorageWrite as DataStorageWrite;
    interface StreamStorageWrite as TimeStorageWrite;
    interface SplitControl as RadioControl;
    interface LowPowerListening as LPL;
    interface SystemLowPowerListening as SysLPL;		
    interface Set<uint16_t> as SetInterval;
    interface TimeSyncPoints;
    interface StdControl as TimeSync;
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

      call SetInterval.set(TIMESYNC_INTERVAL);
      call TimeSync.start();

      call Timer.startPeriodicAt(((uint32_t)60)<<10,(uint32_t)MEASURE_INTERVAL<<10);
    }
  }	

  event void Timer.fired(){
    call Humidity.read();
  }


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

  event void VLight.readDone(error_t result, uint8_t val){
    if(result==SUCCESS){
      data.vlight=val;
    }else{
      data.vlight=0;
    }
    call IRLight.read();
  }

  event void IRLight.readDone(error_t result, uint8_t val){
    if(result==SUCCESS){
	data.irlight=val;
    }else{
	data.irlight=0;
    }
    data.timestamp=call LocalTime.get();
    call DataStorageWrite.appendWithID(0xaa,&data, sizeof(data));
  }

  event void TimeSyncPoints.syncPoint(uint32_t local, am_addr_t nodeId, uint32_t remote){
    nx_struct{
      nx_uint16_t nodeId;
      nx_uint32_t local;
      nx_uint32_t remote;
    } timeData;
    call TimeStorageWrite.appendWithID(0x3d,&timeData, sizeof(timeData));
  }


  event void TimeStorageWrite.appendDone(void *buf, uint16_t len, error_t error){}
  event void TimeStorageWrite.appendDoneWithID(void *buf, uint16_t len, error_t error){}
  event void TimeStorageWrite.syncDone(error_t error){}

  event void DataStorageWrite.appendDone(void *buf, uint16_t len, error_t error){}
  event void DataStorageWrite.appendDoneWithID(void *buf, uint16_t len, error_t error){}
  event void DataStorageWrite.syncDone(error_t error){}
  event void RadioControl.stopDone(error_t error){}
}