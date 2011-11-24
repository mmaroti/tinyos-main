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
    interface ReadRef<calibration_t>;
    interface DiagMsg;
    interface AMSend as CalibSend;
    interface AMSend as MeasSend;
    interface Receive;
    interface Packet;
    interface Timer<TMilli>;
    interface Leds;
  }
}
implementation {  
  measurement_t *meas;
  message_t message, calibmessage;
  calibration_t *calib;
  bool starting=TRUE;

  event void Boot.booted() {
    calib = (calibration_t*)call Packet.getPayload(&calibmessage, sizeof(calibration_t));
    meas = (measurement_t*)call Packet.getPayload(&message, sizeof(measurement_t));
    call ReadRef.read(calib);
  }
  
  event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len){
    if(!starting)
      call CalibSend.send(AM_BROADCAST_ADDR, &calibmessage, sizeof(calibration_t));
    return msg;
  }
  
  event void ReadRef.readDone(error_t error, calibration_t *data){
    call CalibSend.send(AM_BROADCAST_ADDR, &calibmessage, sizeof(calibration_t));
  }
  
  event void CalibSend.sendDone(message_t* msg, error_t error){
    if(starting)
      call Timer.startPeriodic(512);
  }

  event void Timer.fired(){
    if(!starting)
      call MeasSend.send(AM_BROADCAST_ADDR, &message, sizeof(measurement_t));
    else
      starting=FALSE;
    
    call TempRead.read();
    call HumiRead.read();
    call LightRead.read();
    call PressRead.read();
    call Temp2Read.read();
    call VoltageRead.read();
  }
  
  event void TempRead.readDone(error_t error, uint16_t data){
    if(error==SUCCESS){
      meas->temp=data;
    } else
      call Leds.led3Toggle();
  }

  event void HumiRead.readDone(error_t error, uint16_t data) { 
    if(error==SUCCESS){
      meas->humi=data;
    } else
      call Leds.led3Toggle();
  }
  
  event void LightRead.readDone(error_t error, uint16_t data) { 
    if(error==SUCCESS){
      meas->light=data;
    } else
      call Leds.led3Toggle();
    
  }
  
  event void PressRead.readDone(error_t error, uint32_t data) { 
    if(error==SUCCESS){
      meas->press=data;
    } else
      call Leds.led3Toggle();
    
  }
  
  event void Temp2Read.readDone(error_t error, uint32_t data) { 
    if(error==SUCCESS){
      meas->temp2=data;
    } else
      call Leds.led3Toggle();
    
  }
  
  event void VoltageRead.readDone(error_t error, uint16_t data) { 
    if(error==SUCCESS){
      meas->voltage=data;
    } else
      call Leds.led3Toggle();
  }
  
  event void Temp3Read.readDone(error_t error, uint16_t data) { 
    if(error==SUCCESS){
      meas->temp3=data;
    } else
      call Leds.led3Toggle();
  }
  
  event void MeasSend.sendDone(message_t* msg, error_t error){}
    
}
