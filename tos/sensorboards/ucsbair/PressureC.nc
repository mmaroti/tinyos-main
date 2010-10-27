module PressureC {
  provides interface Read<uint16_t> as Temp;
  provides interface Read<uint16_t> as Press;
  uses interface Intersema;
}
implementation {
	
	enum{
		TP,
		PR,
	};	
	uint8_t	state=TP;

	command error_t Temp.read(){
		state=TP;		
		return call Intersema.read();	
	}

	command error_t Press.read(){
		state=PR;		
		return call Intersema.read();	
	}

	
	event void Intersema.readDone(error_t error, int16_t* mesResult){
		if (state==TP)
			signal Temp.readDone(error,mesResult[0]);
		else if (state==PR)
			signal Press.readDone(error,mesResult[1]);			

	}
	
	default event void Press.readDone(error_t error, uint16_t data){}
	default event void Temp.readDone(error_t error, uint16_t data){}

} 
