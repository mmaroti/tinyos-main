module Sht21ReadP {
	uses {
		interface Boot;
		interface Read<uint16_t>;
		interface SplitControl as Switch;
		interface SplitControl as SerialControl;
		interface DiagMsg;
	}
}
implementation {
	event void Boot.booted() {
		call Switch.start();
    if (call SerialControl.start() != SUCCESS)
      //report_problem();
			;
	}

	event void Read.readDone(error_t error, uint16_t data) { }

	event void Switch.startDone(error_t error) { call Read.read(); }
  event void Switch.stopDone(error_t error) { }

	event void SerialControl.startDone(error_t error) { }
	event void SerialControl.stopDone(error_t error) { }
}
