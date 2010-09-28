#include "Timer.h"
#include "SensirionSht21.h"

generic module SensirionSht21LogicP() {
  provides interface SensirionSht21[ uint8_t client ];
  uses interface Timer<TMilli>;
  uses interface I2CPacket<TI2CBasicAddr>;
}
implementation {
  uint8_t currentClient;
  Sht21Command cmd;
  bool on = TRUE;
  uint8_t status=0;
  uint8_t newValue;

  command error_t SensirionSht21.reset[ uint8_t client ](){
    if ( !on ) { return EOFF; }
    currentClient = client;
    cmd = SOFT_RESET;
    return proceed();
  }

  command error_t SensirionSht21.readUserReg[ uint8_t client ](){
    if ( !on ) { return EOFF; }
    currentClient = client;
    cmd = READ_USER_REGISTER;
    return proceed();
  }

  command error_t SensirionSht21.writeUserReg[ uint8_t client ]( uint8_t value ){
    if ( !on ) { return EOFF; }
    currentClient = client;
    newValue = value;
    cmd = WRITE_USER_REGISTER;
    return proceed();
  }

  command error_t SensirionSht21.measureTemperature[ uint8_t client ](){
    if ( !on ) { return EOFF; }
    currentClient = client;
    cmd = TRIGGER_T_MEASUREMENT_HOLD_MASTER;
    return proceed();
  }

  command error_t SensirionSht21.measureHumidity[ uint8_t client ](){
    if ( !on ) { return EOFF; }
    currentClient = client;
    cmd = TRIGGER_RH_MEASUREMENT_HOLD_MASTER;
    return proceed();
  }

  error_t proceed() {
    switch(cmd) {
    case SOFT_RESET:
      call I2CPacket.write(I2C_START | I2C_STOP, WRITE_ADDRESS, 1, cmd);
      call Timer.startOneShot( TIMEOUT_RESET );
      break;

    case TRIGGER_T_MEASUREMENT_HOLD_MASTER:
      call I2CPacket.write(I2C_START, WRITE_ADDRESS, 1, cmd);
      call Timer.startOneShot( TIMEOUT_14BIT );
      break;

    case TRIGGER_RH_MEASUREMENT_HOLD_MASTER:
      call I2CPacket.write(I2C_START, WRITE_ADDRESS, 1, cmd);
      call Timer.startOneShot( TIMEOUT_12BIT );
      break;

    case WRITE_USER_REGISTER:
        I2CPacket.write(I2C_START, WRITE_ADDRESS, 1, cmd);
        I2CPacket.write(I2C_START | I2C_STOP, WRITE_ADDRESS, 1, newValue);
        post signalUserDone();
    //  break;

    case READ_USER_REGISTER:
        I2CPacket.write(I2C_START, WRITE_ADDRESS, 1, cmd);
        I2CPacket.read(I2C_START | I2C_STOP, READ_ADDRESS, 1, status);
        post signalUserDone();
    //  break;
    }//switch

    return SUCCESS;
  }//proceed

  event void Timer.fired() {
    switch(cmd) {
      case SOFT_RESET:
        signal SensirionSht21.resetDone[currentClient](SUCCESS);
        break;

      case TRIGGER_T_MEASUREMENT_HOLD_MASTER:
        signal SensirionSht21.measureTemperatureDone[currentClient](FAIL, 0);
        break;

      case TRIGGER_RH_MEASUREMENT_HOLD_MASTER:
        signal SensirionSht21.measureHumidityDone[currentClient](FAIL, 0);
        break;

      default:
        break;
    } //switch
  } //fired

  task signalUserDone() {
    bool _writeFail = writeFail;
    switch( cmd ) {
    case READ_USER_REGISTER:
      signal SensirionSht21.readUserRegDone[currentClient]( SUCCESS, status );
      break;
    case CMD_WRITE_STATUS:
      writeFail = FALSE;
      signal SensirionSht21.writeUserRegDone[currentClient]( (_writeFail ? FAIL : SUCCESS) );
      break;
    default:
      // shouldn't happen.
      break;
    }
  }

  default event void SensirionSht21.resetDone[ uint8_t client ]( error_t result );
  default event void SensirionSht21.readUserRegDone[ uint8_t client ]( error_t result, uint8_t value ); 
  default event void SensirionSht21.writeUserRegDone[ uint8_t client ]( error_t result ); 
  default event void SensirionSht21.measureTemperatureDone[ uint8_t client ]( error_t result, uint16_t value );
  default event void SensirionSht21.measurehumidityDone[ uint8_t client]( error_t result, uint16_t value );

}
