/*
* Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author:Andras Biro
*/
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