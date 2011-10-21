#include "Ms5607.h"
module UcminiSensorP {
  uses {
    interface Boot;
    interface Read<uint16_t> as TempRead;
    interface Read<uint16_t> as HumiRead;
    interface Read<uint16_t> as LightRead;
    interface Read<uint32_t> as PressRead;
    interface Read<uint32_t> as Temp2Read;
    interface Read<uint16_t> as Temp3Read;
    interface Read<uint16_t> as VoltageRead;
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
    uint16_t temp3;
    uint16_t voltage;
  } measurment_t;
  
  measurment_t meas;
  calibration calib;
  bool print=FALSE;

  event void Boot.booted() {
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
    if(print && call DiagMsg.record()){
      call DiagMsg.uint16(meas.temp);
      call DiagMsg.uint16(meas.humi);
      call DiagMsg.uint16(meas.light);
      call DiagMsg.uint32(meas.press);
      call DiagMsg.uint32(meas.temp2);
      call DiagMsg.uint16(meas.temp3);
      call DiagMsg.uint16(meas.voltage);
      call DiagMsg.send();
    } else
      print=TRUE;
    call TempRead.read();
    call HumiRead.read();
    call LightRead.read();
    call PressRead.read();
    call Temp2Read.read();
    call VoltageRead.read();
  }
  
  event void TempRead.readDone(error_t error, uint16_t data){
    if(error==SUCCESS){
      meas.temp=data;
    } else
      call Leds.led3Toggle();
  }

  event void HumiRead.readDone(error_t error, uint16_t data) { 
    if(error==SUCCESS){
      meas.humi=data;
    } else
      call Leds.led3Toggle();
  }
  
  event void LightRead.readDone(error_t error, uint16_t data) { 
    if(error==SUCCESS){
      meas.light=data;
    } else
      call Leds.led3Toggle();
    
  }
  
  event void PressRead.readDone(error_t error, uint32_t data) { 
    if(error==SUCCESS){
      meas.press=data;
    } else
      call Leds.led3Toggle();
    
  }
  
  event void Temp2Read.readDone(error_t error, uint32_t data) { 
    if(error==SUCCESS){
      meas.temp2=data;
    } else
      call Leds.led3Toggle();
    
  }
  
  event void VoltageRead.readDone(error_t error, uint16_t data) { 
    if(error==SUCCESS){
      meas.voltage=data;
    } else
      call Leds.led3Toggle();
  }
  
  event void Temp3Read.readDone(error_t error, uint16_t data) { 
    if(error==SUCCESS){
      meas.temp3=data;
    } else
      call Leds.led3Toggle();
  }  
  
}
