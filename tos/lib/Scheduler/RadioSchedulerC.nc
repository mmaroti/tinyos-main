configuration RadioSchedulerC{
	provides interface RadioScheduler[uint8_t radio_id];
	provides interface LocalTime<TMilli>;
}
implementation{
	components RadioSchedulerP, LocalTimeMilliC, new TimerMilliC() as Timer;
	 
	RadioSchedulerP.LocalTime->LocalTimeMilliC;
	RadioSchedulerP.Timer->Timer;
	LocalTime=LocalTimeMilliC;
	RadioScheduler=RadioSchedulerP.RadioScheduler;
}