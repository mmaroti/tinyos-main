interface RadioScheduler{
	command error_t RadioStart(uint32_t when);
	command error_t RadioStartCancel();
	event error_t RadioStartDone();
	
	command error_t RadioStop(uint32_t when);
	command error_t RadioStopCancel();
	event error_t RadioStopDone(); 
}