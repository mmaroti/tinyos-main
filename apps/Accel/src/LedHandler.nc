
interface LedHandler{
	
	command void error();
	
	command void radioOn();
	
	command void radioOff();
	
	command void msgReceived();
	
	command void diskReady();

}