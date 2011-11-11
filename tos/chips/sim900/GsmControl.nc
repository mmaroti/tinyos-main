interface GsmControl
{
	command error_t connectToGSM();
	command error_t disconnectFromGSM();
	command error_t SendToGSM(char* msg);
	
	event void connectToGSMDone(error_t err);
	event void disconnectFromGSMDone(error_t err);
	event void SendToGSMDone(error_t err);
	
}
