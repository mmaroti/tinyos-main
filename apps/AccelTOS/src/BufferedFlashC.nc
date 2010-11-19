configuration BufferedFlashC {
	
	provides interface BufferedFlash;
	
}
// FIXME This entire component should go into SimpleFileP
// FIXME Code duplication! BufferedFlashP already inconsistent with BufferedSendP
implementation{
	
	components BufferedFlashP, SimpleFileC, LedHandlerC, DiagMsgC;
	
	BufferedFlash = BufferedFlashP;
	BufferedFlashP.LedHandler -> LedHandlerC;
	BufferedFlashP.SimpleFile -> SimpleFileC;
	BufferedFlashP.DiagMsg -> DiagMsgC;
}