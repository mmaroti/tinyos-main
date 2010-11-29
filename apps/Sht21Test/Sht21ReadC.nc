configuration Sht21ReadC { }
implementation {
	components Sht21ReadP, MainC, Sht21C, SerialActiveMessageC;
	components DiagMsgC;

	Sht21ReadP.Boot -> MainC;
	Sht21ReadP.Read -> Sht21C.Temperature;
	Sht21ReadP.Switch -> Sht21C.SplitControl;
	Sht21ReadP.SerialControl -> SerialActiveMessageC;
	
	Sht21ReadP.DiagMsg -> DiagMsgC;
}
