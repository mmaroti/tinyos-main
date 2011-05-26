configuration ReaderAppC { }
implementation
{
  components ReaderC, MainC, LedsC,
    new TimerMilliC(),
    TestSbSC as Sensor, 
    BmaStreamP,
    //new Taos2550C() as Sensor,
    //Ms5607C as Sensor,
    //new AMSenderC(10) as Send,
    //ActiveMessageC as AMC;
		new SerialAMSenderC(10) as Send,
    SerialActiveMessageC as AMC, DiagMsgC, PlatformC;

  ReaderC.Boot -> MainC;
  ReaderC.AMSend -> Send;
  ReaderC.AMControl -> AMC;
  ReaderC.Read -> Sensor;
  BmaStreamP.Read -> Sensor;
  BmaStreamP.DiagMsg -> DiagMsgC;
  BmaStreamP.Atm128Calibrate -> PlatformC;
  ReaderC.ReadStream -> BmaStreamP;
  //OscilloscopeC.Read -> Sensor.VisibleLight;
  ReaderC.BmaControl -> Sensor;
  ReaderC.Leds -> LedsC;
  ReaderC.DiagMsg -> DiagMsgC;

  //OscilloscopeC.I2CControl -> I2CBusC.BusControl;//Sensor.SplitControl; 
}
