

module AccelP {
   uses interface Boot;
   uses interface Leds;
   uses interface SimpleFile as SF;
   uses interface SplitControl as SFCtrl;
   uses interface SplitControl as RadioHandler;
}

implementation{
   
   event void Boot.booted() {
		// TODO
		error_t error;
		
		error = call SFCtrl.start();
		
		if (error)
			call Leds.led0On();

		error = call RadioHandler.start();

		if (error)
			call Leds.led0On();

   }

	event void SFCtrl.startDone(error_t error){

		if (error == SUCCESS)
			call Leds.led2Off();			
		else
			call Leds.led0On();
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

	event void RadioHandler.startDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void RadioHandler.stopDone(error_t error){
		// TODO Auto-generated method stub
	}
}
