module AccelP {
   uses interface Boot;
   uses interface Leds;
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
}