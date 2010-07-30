configuration BufferedFlashC {
	
	provides interface BufferedFlash;
	
}
// FIXME This entire component should go into SimpleFileP
implementation{
	
	components BufferedFlashP, SimpleFileC;
	
	BufferedFlash = BufferedFlashP;
	BufferedFlashP.SimpleFile -> SimpleFileC;
}