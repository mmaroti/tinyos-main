interface RadioScheduler{
	command error_t RadioStart(uint32_t when);
	command error_t RadioStartCancel();
	event void RadioStartDone(error_t error);
	
	command error_t RadioStop(uint32_t when);
	command error_t RadioStopCancel();
	event void RadioStopDone(error_t error); 
}