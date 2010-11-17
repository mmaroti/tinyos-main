
interface SyncMsgSender {
	
	command error_t send();
	
	event void sendDone(error_t error);		

}
