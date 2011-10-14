#include "Ms5607.h"
module UcminiSensorP {
	uses {
		interface Boot;
		interface Read<uint16_t> as TempRead;
    interface Read<uint16_t> as HumiRead;
    interface Read<uint16_t> as LightRead;
    interface Read<uint32_t> as PressRead;
    interface Read<uint32_t> as Temp2Read;
    interface ReadRef<calibration>;
		interface DiagMsg;
    interface Timer<TMilli>;
    interface Leds;
	}
}
implementation {
  typedef struct measurment_t{
    uint16_t temp;
    uint16_t humi;
    uint16_t light;
    uint32_t press;
    uint32_t temp2;
  } measurment_t;
  
  measurment_t meas;
  calibration calib;
  
  inline void readDone(){
    if(call DiagMsg.record()){
      call DiagMsg.uint16(meas.temp);
      call DiagMsg.uint16(meas.humi);
      call DiagMsg.uint16(meas.light);
      call DiagMsg.uint32(meas.press);
      call DiagMsg.uint32(meas.temp2);
      call DiagMsg.send();
    }
    //call TempRead.read();
  }

  event void Boot.booted() {
    //call Timer.startOneShot(1);
    //call Timer.startPeriodic(500);
    call ReadRef.read(&calib);
  }
  
  event void ReadRef.readDone(error_t error, calibration *data){
    if(call DiagMsg.record()){
      call DiagMsg.uint16s(calib.coefficient,6);
      call DiagMsg.send();
    }
    call Timer.startPeriodic(500);
  }

  event void Timer.fired(){
    call TempRead.read();
  }
  
  event void TempRead.readDone(error_t error, uint16_t data){
    if(error==SUCCESS){
      meas.temp=data;
      call HumiRead.read();
    } else
      call Leds.led3Toggle();
  }

	event void HumiRead.readDone(error_t error, uint16_t data) { 
    if(error==SUCCESS){
      meas.humi=data;
    } else
      call Leds.led3Toggle();
    call LightRead.read();
  }
  
  event void LightRead.readDone(error_t error, uint16_t data) { 
    if(error==SUCCESS){
      meas.light=data;
    } else
      call Leds.led3Toggle();
    call PressRead.read();
  }
  
  event void PressRead.readDone(error_t error, uint32_t data) { 
    if(error==SUCCESS){
      meas.press=data;
    } else
      call Leds.led3Toggle();
    call Temp2Read.read();
  }
  
  event void Temp2Read.readDone(error_t error, uint32_t data) { 
    if(error==SUCCESS){
      meas.temp2=data;
    } else
      call Leds.led3Toggle();
    readDone();
  }
  
  
  
}
