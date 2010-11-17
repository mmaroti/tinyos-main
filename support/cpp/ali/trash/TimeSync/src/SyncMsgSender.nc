
interface SyncMsgSender {
	
	command void setFirstBlock(uint32_t position);
	
	command error_t send();
	
	event void sendDone(error_t error);
}
