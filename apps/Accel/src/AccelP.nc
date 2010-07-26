module AccelP {
   uses interface Boot;
   uses interface Leds;
   uses interface SplitControl as AMControl;
   uses interface Receive;
   uses interface SimpleFile as SF;
   uses interface SplitControl as SFCtrl;
}
implementation{
   
   event void Boot.booted() {
		// TODO
		call SFCtrl.start();

   }

	event void SFCtrl.startDone(error_t error){
		// TODO Auto-generated method stub
	}
	
	event void SF.formatDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void SF.appendDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void SF.seekDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void SF.readDone(error_t error, uint16_t length){
		// TODO Auto-generated method stub
	}

	event void SFCtrl.stopDone(error_t error){
		// TODO Auto-generated method stub
	}

	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len){
		// FIXME Auto-generated method stub
		return 0;
	}

	event void AMControl.stopDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void AMControl.startDone(error_t error){
		// TODO Auto-generated method stub
	}
}