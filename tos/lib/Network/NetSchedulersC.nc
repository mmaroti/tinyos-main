#include "Network.h"
configuration NetSchedulersC{
	provides interface RadioScheduler as BeaconScheduler;
	provides interface RadioScheduler as DiscoveryScheduler;
	provides interface RadioScheduler as NeighborSchedulers[uint8_t user_id];
	provides interface LocalTime<TMilli>;
}
implementation{
	components RadioSchedulerC,SchDispatchP;
	SchDispatchP->RadioSchedulerC.RadioScheduler;
	NeighborSchedulers=SchDispatchP.NeighborSchedulers;
	BeaconScheduler=SchDispatchP.BeaconScheduler;
	DiscoveryScheduler=SchDispatchP.DiscoveryScheduler;
	LocalTime=RadioSchedulerC.LocalTime;
}