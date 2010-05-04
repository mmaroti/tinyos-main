module TempEraseC {
	uses interface Boot;
	uses interface Leds;
    	uses interface LogWrite;
	
}
implementation {

	event void Boot.booted() {
		if (call LogWrite.erase() == SUCCESS) {
			call Leds.led0On();
      		}
	}

	event void LogWrite.eraseDone(error_t err) {
	if (err==SUCCESS) call Leds.led0Off();	
	}
	event void LogWrite.syncDone(error_t err) {}
	event void LogWrite.appendDone(void* buf, storage_len_t len, bool recordsLost, error_t err) {}


}
