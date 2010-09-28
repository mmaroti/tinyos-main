#include "SensirionSht21.h"

generic module SensirionSht21ReaderP() {
  provides interface Read<uint16_t> as Temperature;
  provides interface Read<uint16_t> as Humidity;

  uses {
    interface Resource as TempResource;
    interface Resource as HumResource;
    interface SensirionSht21 as Sht21Temp;
    interface SensirionSht21 as Sht21Hum;
  }
}
implementation {

  command error_t Temperature.read() {
    call TempResource.request();
    return SUCCESS;
  }
  
  event void TempResource.granted() {
    error_t result;
    if((result = call Sht21Temp.measureTemperature()) != SUCCESS) {
      call TempResource.release();
      signal Temperature.readDone(result, 0);
    }
  }

  event void Sht21Temp.measureTemperatureDone(error_t result, uint16_t val) {
    call TempResource.release();
    signal Temperature.readDone(result, val);
  }

  command error_t Humidity.read() {
    call HumResource.request();
    return SUCCESS;
  }

  event void HumResource.granted() {
    error_t result;
    if((result = call Sht21Hum.measureHumidity()) != SUCCESS) {
      call HumResource.release();
      signal Humidity.readDone(result, 0);
    }
  }
  
  event void Sht21hum.measureHumidityDone(error_t result, uint16_t val) {
    call HumResource.release();
    signal Humidity.readDone(result, val);
  }

  default event void Temperature.readDone(error_t result, uint16_t val) { }
  default event void Humidity.readDone(error_t result, uint16_t val) { }
}
