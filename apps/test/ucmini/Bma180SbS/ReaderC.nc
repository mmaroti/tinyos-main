#include "Bma180.h"
module ReaderC @safe()
{
  uses {
    interface Boot;
    interface Read<bma180_data_t>;
    //interface SplitControl as I2CControl;
    interface Leds;
    interface AMSend;
    interface SplitControl as AMControl;
    interface DiagMsg;
  }
}
implementation
{

 /* event void I2CControl.startDone(error_t error) {
    if(error!=SUCCESS)
      call I2CControl.start();
    else
      call AMControl.start();
  }*/

  event void AMControl.startDone(error_t error) {
    call Read.read();
  }

  event void Boot.booted() {
    //DDRF = _BV(PF2);
    //PORTF = _BV(PF2);
    //call I2CControl.start();
    call AMControl.start();
  }

  event void AMSend.sendDone(message_t* msg, error_t error) {}
  //event void I2CControl.stopDone(error_t error) {}
  event void AMControl.stopDone(error_t error) {}
  event void Read.readDone(error_t result, bma180_data_t data) {
    if(call DiagMsg.record()) {
      call DiagMsg.str("X: ");
      call DiagMsg.uint16(data.bma180_accel_x);
      call DiagMsg.str("Y: ");
      call DiagMsg.uint16(data.bma180_accel_y);
      call DiagMsg.str("Z: ");
      call DiagMsg.uint16(data.bma180_accel_z);
      call DiagMsg.str("Temp: ");
      call DiagMsg.uint8(data.bma180_temperature);
      call DiagMsg.send();
    }
  }
}
