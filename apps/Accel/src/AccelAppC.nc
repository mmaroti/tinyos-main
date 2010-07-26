configuration AccelAppC{
	
}
implementation{
	components MainC, AccelP, LedsC, SimpleFileC;
	components ActiveMessageC;
	components new AMReceiverC(9) as AMRec; // FIXME Magic number
	AccelP.Boot -> MainC;
	AccelP.Leds -> LedsC;
	AccelP.AMControl -> ActiveMessageC;
	AccelP.Receive -> AMRec;
	AccelP.SF -> SimpleFileC;
	AccelP.SFCtrl -> SimpleFileC;
}