configuration NetTestC{
}
implementation{
	components NetworkC,MainC,NetTestP,LedsC;
	NetTestP->MainC.Boot;
	NetTestP->NetworkC.TimeSyncPoints;
	NetTestP->NetworkC.StdControl;
	NetTestP->NetworkC.AMSend;
	NetTestP.Receive->NetworkC.Receive;
	NetTestP.Snoop->NetworkC.Snoop;
	NetTestP->LedsC.Leds;
}