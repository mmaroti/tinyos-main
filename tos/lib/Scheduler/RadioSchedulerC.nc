configuration RadioSchedulerC{
	provides interface RadioScheduler[uint8_t radio_id];
	provides interface LocalTime<TMilli>;
}
implementation{
	components RadioSchedulerP, LocalTimeMilliC;
	components new TimerMilliC() as SignalTimer;
	RadioSchedulerP.SignalTimer->SignalTimer;
	
	RadioSchedulerP.LocalTime->LocalTimeMilliC;
	LocalTime=LocalTimeMilliC;
	RadioScheduler=RadioSchedulerP.RadioScheduler;
}