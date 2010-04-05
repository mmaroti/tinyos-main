#include "Network.h"
#include "Scheduler.h"
module SchDispatchP{
	uses interface RadioScheduler[uint8_t user_id];
	provides interface RadioScheduler as BeaconScheduler;
	provides interface RadioScheduler as DiscoveryScheduler;
	provides interface RadioScheduler as NeighborSchedulers[uint8_t user_id];
}
implementation{
	enum {
		USERS=uniqueN(UQ_RADIO_SCH,2+MAX_NEIGHBOR),
	};
	
	event void RadioScheduler.RadioStopDone[uint8_t user_id](error_t error){
		if(user_id<MAX_NEIGHBOR)
			signal NeighborSchedulers.RadioStopDone[user_id](error);
		else if(user_id==MAX_NEIGHBOR+1)
			signal DiscoveryScheduler.RadioStopDone(error);
		else if(user_id==MAX_NEIGHBOR)
			signal BeaconScheduler.RadioStopDone(error);
	}

	event void RadioScheduler.RadioStartDone[uint8_t user_id](error_t error){
		if(user_id<MAX_NEIGHBOR)
			signal NeighborSchedulers.RadioStartDone[user_id](error);
		else if(user_id==MAX_NEIGHBOR+1)
			signal DiscoveryScheduler.RadioStartDone(error);
		else if(user_id==MAX_NEIGHBOR)
			signal BeaconScheduler.RadioStartDone(error);
	}

	command error_t BeaconScheduler.RadioStopCancel(){
		return call RadioScheduler.RadioStopCancel[MAX_NEIGHBOR]();
	}

	command error_t BeaconScheduler.RadioStop(uint32_t when){
		return call RadioScheduler.RadioStop[MAX_NEIGHBOR](when);
	}

	command error_t BeaconScheduler.RadioStartCancel(){
		return call RadioScheduler.RadioStartCancel[MAX_NEIGHBOR]();
	}

	command error_t BeaconScheduler.RadioStart(uint32_t when){
		return call RadioScheduler.RadioStart[MAX_NEIGHBOR](when);
	}
	
	command error_t BeaconScheduler.IsStarted(){
		return call RadioScheduler.IsStarted[MAX_NEIGHBOR]();
	}

	command error_t DiscoveryScheduler.RadioStopCancel(){
		return call RadioScheduler.RadioStopCancel[MAX_NEIGHBOR+1]();
	}

	command error_t DiscoveryScheduler.RadioStop(uint32_t when){
		return call RadioScheduler.RadioStop[MAX_NEIGHBOR+1](when);
	}

	command error_t DiscoveryScheduler.RadioStartCancel(){
		return call RadioScheduler.RadioStartCancel[MAX_NEIGHBOR+1]();
	}

	command error_t DiscoveryScheduler.RadioStart(uint32_t when){
		return call RadioScheduler.RadioStart[MAX_NEIGHBOR+1](when);
	}
	
	command error_t DiscoveryScheduler.IsStarted(){
		return call RadioScheduler.IsStarted[MAX_NEIGHBOR+1]();
	}

	command error_t NeighborSchedulers.RadioStart[uint8_t user_id](uint32_t when){
		return call RadioScheduler.RadioStart[user_id](when);
	}

	command error_t NeighborSchedulers.RadioStartCancel[uint8_t user_id](){
		return call RadioScheduler.RadioStartCancel[user_id]();
	}

	command error_t NeighborSchedulers.RadioStop[uint8_t user_id](uint32_t when){
		return call RadioScheduler.RadioStop[user_id](when);
	}

	command error_t NeighborSchedulers.RadioStopCancel[uint8_t user_id](){
		return call RadioScheduler.RadioStopCancel[user_id]();
	}
	
	command error_t NeighborSchedulers.IsStarted[uint8_t user_id](){
		return call RadioScheduler.IsStarted[user_id]();
	}
	
	default event void NeighborSchedulers.RadioStartDone[uint8_t user_id](error_t err){};
	default event void NeighborSchedulers.RadioStopDone[uint8_t user_id](error_t err){};
	default event void DiscoveryScheduler.RadioStartDone(error_t err){};
	default event void DiscoveryScheduler.RadioStopDone(error_t err){};
	default event void BeaconScheduler.RadioStartDone(error_t err){};
	default event void BeaconScheduler.RadioStopDone(error_t err){};
}