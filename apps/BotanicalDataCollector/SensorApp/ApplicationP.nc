#ifndef MEASURE_INTERVAL
  #define MEASURE_INTERVAL 60
#endif
#ifndef TIMESYNC_INTERVAL
  #define TIMESYNC_INTERVAL 600
#endif
#ifndef TIMESYNC_SAVEPOINT
  #define TIMESYNC_SAVEPOINT 3
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
    interface Read<uint16_t> as VRef;
    interface LocalTime<TMilli>;
    interface StreamStorageWrite as DataStorageWrite;
    interface StreamStorageWrite as TimeStorageWrite;
    interface SplitControl as RadioControl;
    interface LowPowerListening as LPL;
    interface SystemLowPowerListening as SysLPL;		
    interface Set<uint16_t> as SetInterval;
    interface Set<uint8_t> as SetSavePoints;
    interface Get<uint8_t> as GetLastRSSI;
    interface Get<uint8_t> as GetLastLQI;
    interface TimeSyncPoints;
    interface StdControl as TimeSync;
    interface Command;
    interface Set<uint16_t> as EepromWrite;
    interface Get<uint16_t> as EepromRead;
    interface Debug as StorDebug;
    interface Debug as DataDebug;
    interface Debug as TimeDebug;
    interface Debug as UplDebug;
  }
}
implementation{

  nx_struct data{//size=14
    nx_uint16_t temp;
    nx_uint16_t humidity;
    nx_uint16_t vref;
    nx_uint8_t vlight;
    nx_uint8_t irlight;
    nx_uint32_t timestamp;
    nx_uint16_t bootCount;
  } data;
  
  nx_struct time{//size=16
    nx_uint16_t nodeId;
    nx_uint32_t local;
    nx_uint16_t localBootCount;
    nx_uint32_t remote;
    nx_uint16_t remoteBootCount;
    nx_uint8_t rssi;
    nx_uint8_t lqi;
  } timeData;
  
  bool running=FALSE;

  event void Boot.booted(){
    call EepromWrite.set(call EepromRead.get()+1);
    call RadioControl.start();
  }

  event void RadioControl.startDone(error_t error){
    if(error!=SUCCESS)
      call RadioControl.start();
    else{
      call LPL.setLocalWakeupInterval(512);
      call SysLPL.setDefaultRemoteWakeupInterval(512);
      call SysLPL.setDelayAfterReceive(3072);			

      call SetInterval.set(TIMESYNC_INTERVAL);
      call SetSavePoints.set(TIMESYNC_SAVEPOINT);
      call TimeSync.start();

      call Timer.startPeriodicAt(((uint32_t)MEASURE_INTERVAL)<<9,(uint32_t)MEASURE_INTERVAL<<10);
      running=TRUE;
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
    call VRef.read();	
  }
  
  event void VRef.readDone(error_t result, uint16_t val){
    if(result==SUCCESS)
      data.vref=val;
    else
      data.vref=0xffff;
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
    data.bootCount=call EepromRead.get();
    call DataStorageWrite.append(&data, sizeof(data));
  }

  event void TimeSyncPoints.syncPoint(uint32_t local, am_addr_t nodeId, uint32_t remote, uint16_t remoteBootCount){

    timeData.nodeId=nodeId;
    timeData.local=local;
    timeData.localBootCount=call EepromRead.get();
    timeData.remote=remote;
    timeData.remoteBootCount=remoteBootCount;
    timeData.rssi=call GetLastRSSI.get();
    timeData.lqi=call GetLastLQI.get();
    call TimeStorageWrite.append(&timeData, sizeof(timeData));
  }
  
  event void Command.newCommand(uint32_t cmd){
    switch(cmd&0xff){
      case 0x11:{
        call DataStorageWrite.sync();
      }break;
      case 0x22:{
        if(running){
          call Timer.stop();
          call TimeSync.stop();
          running=FALSE;
          call Command.sendData(0x2201);
        } else {
          call TimeSync.start();
          call Timer.startPeriodicAt(((uint32_t)MEASURE_INTERVAL)<<9,(uint32_t)MEASURE_INTERVAL<<10);
          running=TRUE;
          call Command.sendData(0x2200);          
        }
      }break;
      case 0x33:{
        uint16_t interval=cmd>>16;
        call SetInterval.set(TIMESYNC_INTERVAL);
        call Command.sendData(((uint32_t)interval<<16)+0x3300);
      }break;
      case 0x44:{
        uint16_t interval=cmd>>16;
        call Timer.startPeriodic((uint32_t)interval<<10);
        call Command.sendData(((uint32_t)interval<<16)+0x4400);
      }break;
      case 0x55:{
        uint8_t value=cmd>>16;
        call SetSavePoints.set(value);
        call Command.sendData(((uint32_t)value<<16)+0x5500);
      }break;
      //debug
      case 0xF0:{
        call Command.sendData(((uint32_t)(call StorDebug.getStatus())<<16)+0xF000);
      }break;
      case 0xF1:{
        uint16_t owned=call UplDebug.isResourceOwned()?0xff00:0;
        owned+=call UplDebug.getStatus();
        call Command.sendData(((uint32_t)owned<<16)+0xF100);
      }break;
      case 0xF2:{
        uint8_t owned=call DataDebug.isResourceOwned()?0xff:0;
        call Command.sendData(((uint32_t)owned<<16)+0xF200);
      }break;
      case 0xF3:{
        uint8_t owned=call TimeDebug.isResourceOwned()?0xff:0;
        call Command.sendData(((uint32_t)owned<<16)+0xF200);
      }break;
      case 0xF4:{
        call StorDebug.resetStatus();
        call Command.sendData(0xF400);
      }break;
      case 0xF5:{
        call UplDebug.resetStatus();
        call Command.sendData(0xF500);
      }break;
      case 0xF6:{
        call UplDebug.releaseResource();
        call Command.sendData(0xF600);
      }break;
      case 0xF7:{
        call DataDebug.releaseResource();
        call Command.sendData(0xF700);
      }break;
      case 0xF8:{
        call DataDebug.releaseResource();
        call Command.sendData(0xF800);
      }break;
    }
  }
  
  event void DataStorageWrite.syncDone(error_t error){
    if(error==SUCCESS)
      call Command.sendData(0x1100);
    else
      call Command.sendData(0x1101);
  }


  event void TimeStorageWrite.appendDone(void *buf, uint16_t len, error_t error){}
  event void TimeStorageWrite.appendDoneWithID(void *buf, uint16_t len, error_t error){}
  event void TimeStorageWrite.syncDone(error_t error){}

  event void DataStorageWrite.appendDone(void *buf, uint16_t len, error_t error){}
  event void DataStorageWrite.appendDoneWithID(void *buf, uint16_t len, error_t error){}
  event void RadioControl.stopDone(error_t error){}
}