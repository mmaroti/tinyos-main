configuration BufferedFlashC {
	
	provides interface BufferedFlash;
	
}

implementation{
	
	components BufferedFlashP, SimpleFileC;
	
	BufferedFlash = BufferedFlashP;
	BufferedFlashP.SimpleFile -> SimpleFileC;
}