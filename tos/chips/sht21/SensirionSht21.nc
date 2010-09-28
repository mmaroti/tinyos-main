interface SensirionSht21 {
  command error_t reset();
  event void resetDone( error_t result );

  command error_t readUserReg();
  event void readUserRegDone( error_t result, uint8_t value );  

  command error_t writeUserReg( uint8_t value );
  event void writeUserRegDone( error_t result );

  command error_t measureTemperature();
  event void measureTemperatureDone( error_t result, uint16_t value );

  command error_t measureHumidity();
  event void measurehumidityDone( error_t result, uint16_t value );

  command error_t setHeater( bool isOn );
  event void setHeaterDone( error_t result );

  command error_t setResolution( uint8_t val );
  event void setResolutionDone( error_t result );
}
