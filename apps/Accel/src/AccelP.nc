

module AccelP {

   uses interface Boot;
   uses interface LedHandler;
   uses interface SimpleFile as SF;
   uses interface SplitControl as SFCtrl;
   uses interface StdControl as RadioHandler;
}

implementation{
   
   event void Boot.booted() {
		// TODO
		error_t error;
		
		error = call SFCtrl.start();
		
		if (error)
			call LedHandler.error();

		error = call RadioHandler.start();

		if (error)
			call LedHandler.error();

   }

	event void SFCtrl.startDone(error_t error){

		if (error == SUCCESS)
			call LedHandler.diskReady();			
		else
			call LedHandler.error();
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
