#include "Scheduler.h"
generic configuration RadioSchedulerClientC(){
	provides interface RadioScheduler;
	provides interface LocalTime<TMilli>;
}
implementation{
	components RadioSchedulerC;
	LocalTime=RadioSchedulerC;
	RadioScheduler=RadioSchedulerC.RadioScheduler[unique(UQ_RADIO_SCH)];
}