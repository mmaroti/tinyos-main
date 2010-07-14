#include "Serial.h"
#include "PacketSniffer.h"

configuration SerialPacketSnifferC {
  provides {
    interface SplitControl;
    interface Send;
    interface Receive;
  }
  uses interface Leds;
}
implementation { 
  components MainC, SerialPacketInfoPacketSnifferP as Info, SerialDispatcherC;

  MainC.SoftwareInit -> SerialDispatcherC;
  SplitControl = SerialDispatcherC;
  Leds = SerialDispatcherC;
  
  Send = SerialDispatcherC.Send[TOS_SERIAL_PACKET_SNIFFER_ID];
  Receive = SerialDispatcherC.Receive[TOS_SERIAL_PACKET_SNIFFER_ID];
  SerialDispatcherC.SerialPacketInfo[TOS_SERIAL_PACKET_SNIFFER_ID] -> Info;
}
