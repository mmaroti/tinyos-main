#include "SerialAdder.h"
#include "Timer.h"

module SerialAdderC {
  uses
  {
    interface SplitControl as Control;
    interface Boot;
    interface Receive;
    interface AMSend;
    interface Packet;
  }
}
implementation {
  message_t packet;
  uint16_t sum;

  event void Boot.booted() {
    call Control.start();
  }
  
  event void Control.startDone(error_t err) {}
  event void Control.stopDone(error_t err) {}

  
  event void AMSend.sendDone(message_t* bufPtr, error_t error) {
    
  }

  event message_t* Receive.receive(message_t* bufPtr, void* payload, uint8_t len) {
    if (len == sizeof(test_serial_msg_t))
     {
      test_serial_msg_t* numb = (test_serial_msg_t*)payload;
       test_serial_msg_t* res = (test_serial_msg_t*)call Packet.getPayload(&packet, sizeof(test_serial_msg_t));
      sum = numb -> first + numb -> second;
     
      res-> result = sum;
      call AMSend.send(AM_BROADCAST_ADDR, &packet, sizeof(test_serial_msg_t));
      return bufPtr;
    }
  }
}
