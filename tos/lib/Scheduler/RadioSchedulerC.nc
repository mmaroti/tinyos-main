generic configuration RadioSchedulerC(uint8_t radio_id){
	provides interface RadioScheduler;
	provides interface LocalTime<TMilli>;
}
implementation{
	components RadioSchedulerP, ActiveMessageC, LocalTimeMilliC;
	 
	RadioSchedulerP.SplitControl -> ActiveMessageC;
	RadioSchedulerP.LocalTime->LocalTimeMilliC;
	
	LocalTime=LocalTimeMilliC;
	RadioScheduler=RadioSchedulerP.RadioScheduler[radio_id];
}