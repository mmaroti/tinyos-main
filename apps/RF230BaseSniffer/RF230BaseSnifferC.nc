configuration RF230BaseSnifferC {
}

implementation {

// just to avoid a timer compilation bug
  components new TimerMilliC();

  components MainC, LedsC;

  components SerialActiveMessageC as Serial;
  components RF230SnifferStackC   as Radio;

  components RF230BaseSnifferP;
  
  RF230BaseSnifferP.Boot -> MainC;
  RF230BaseSnifferP.Leds -> LedsC;

  RF230BaseSnifferP.SerialControl -> Serial;
  RF230BaseSnifferP.UartSend -> Serial;
  RF230BaseSnifferP.UartReceive -> Serial;
  RF230BaseSnifferP.UartPacket -> Serial;
  RF230BaseSnifferP.UartAMPacket -> Serial;

  RF230BaseSnifferP.RadioControl -> Radio;
  RF230BaseSnifferP.RadioPacket -> Radio;
  RF230BaseSnifferP.RadioReceive -> Radio;

}
