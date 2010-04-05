#include "Network.h"
module NetworkP{
	provides interface StdControl;
	provides interface TimeSyncPoints; 
	provides interface AMSend[am_id_t am_id];
	provides interface Receive[am_id_t am_id];
	provides interface Receive as Snoop[am_id_t am_id];
	uses {
		interface TimeSyncAMSend<TMilli, uint32_t>;
		interface Receive as TimeSyncReceive;
		interface TimeSyncPacket<TMilli, uint32_t>;
		interface AMPacket as TimeSyncAMPacket;
		interface Packet;
		interface AMPacket;
		interface LocalTime<TMilli>;
		interface Timer<TMilli>;
		interface RadioScheduler as DiscoveryScheduler;
		interface RadioScheduler as BeaconScheduler;
		interface RadioScheduler as NetworkScheduler[uint8_t user_id];
		interface Receive as SubReceive[am_id_t am_id];
		interface AMSend as SubSend[am_id_t am_id];
		interface Receive as SubSnoop[am_id_t am_id];
		interface Random;
		interface Init as RandomInit;
		interface PacketTimeStamp<TMilli, uint32_t> as PacketTimeStampMilli;
		interface Leds;
		interface PacketAcknowledgements;
	}
}
implementation{

	typedef nx_struct{
		nx_uint32_t localTime;
		nx_uint8_t flags;
	} beacon_t;
	
	typedef struct{
		uint16_t nodeid;
		uint32_t nextwake;
		uint32_t nextsleep;
		uint8_t lost;//lost=0: lost, lost=0xfe..x: didn't hear from the mote for 'lost' cycles, lost=0xff: present 
	} negihbor;
	
	uint32_t nextBeacon;
	bool init=TRUE;
	message_t beacon, *data;
	
	am_addr_t wakenBy=TOS_BCAST_ADDR;
	negihbor nt[MAX_NEIGHBOR];
	
	command error_t StdControl.stop(){
		return SUCCESS;
		// TODO Auto-generated method stub
	}

	command error_t StdControl.start(){
		call RandomInit.init();
		call BeaconScheduler.RadioStart(0);//send the first message ASAP
		call DiscoveryScheduler.RadioStart(0);
		return SUCCESS;
	}

	command uint8_t AMSend.maxPayloadLength[am_id_t am_id](){
		return call SubSend.maxPayloadLength[am_id]();
	}

	command void * AMSend.getPayload[am_id_t am_id](message_t *msg, uint8_t len){
		return call SubSend.getPayload[am_id](msg, len);
	}

	command error_t AMSend.send[am_id_t am_id](am_addr_t addr, message_t *msg, uint8_t len){
		call PacketAcknowledgements.requestAck(msg);
		if(call Timer.isRunning())
			return call SubSend.send[am_id](addr, msg, len);
		else{
			beacon_t* mess=call Packet.getPayload(&beacon, sizeof(beacon_t));
			mess->flags|=FLAG_DATA;
			data=msg;
			call Packet.setPayloadLength(data, len);
			call AMPacket.setDestination(msg, addr);
			call AMPacket.setType(msg, am_id);
			return SUCCESS;	
		}
	}

	command error_t AMSend.cancel[am_id_t am_id](message_t *msg){
		beacon_t* mess=call Packet.getPayload(&beacon, sizeof(beacon_t));
		if((mess->flags&FLAG_DATA)!=0){
			mess->flags&=~FLAG_DATA;
			return SUCCESS;
		} else {
			return call SubSend.cancel[am_id](msg);
		}
	}
	
	event void SubSend.sendDone[am_id_t am_id](message_t *msg, error_t error){
		if(error==SUCCESS&&call PacketAcknowledgements.wasAcked(msg))
			call Timer.startOneShot(call LocalTime.get()+BEACON_INTERVAL);
		signal AMSend.sendDone[am_id](msg, error);
	}

	event message_t * SubReceive.receive[am_id_t am_id](message_t *msg, void *payload, uint8_t len){
		if(call Timer.isRunning()&&call AMPacket.source(msg)==wakenBy)
			call NetworkScheduler.RadioStopCancel[am_id]();
		signal Receive.receive[am_id](msg, payload, len);
		return msg;
	}
	
	event message_t * SubSnoop.receive[am_id_t am_id](message_t *msg, void *payload, uint8_t len){
		if(call Timer.isRunning()&&call AMPacket.source(msg)==wakenBy)
			call NetworkScheduler.RadioStopCancel[am_id]();
		signal Snoop.receive[am_id](msg, payload, len);
		return msg;
	}

	event void DiscoveryScheduler.RadioStartDone(error_t error){
		if(error!=SUCCESS){
			call DiscoveryScheduler.RadioStart(0);
		} else {
			if(init){
				init=FALSE;
				call DiscoveryScheduler.RadioStop(call LocalTime.get()+INIT_DISCOVERY);
			} else {
				call DiscoveryScheduler.RadioStop(call LocalTime.get()+BEACON_INTERVAL);
			}
		}
	}

	event void DiscoveryScheduler.RadioStopDone(error_t error){
		if(error!=SUCCESS){
			call DiscoveryScheduler.RadioStop(0);
		} else {
			//TODO: randomize the waiting time
			call DiscoveryScheduler.RadioStart(call LocalTime.get()+INIT_DISCOVERY);
		}
	}

	event void BeaconScheduler.RadioStartDone(error_t error){
		beacon_t* mess=call Packet.getPayload(&beacon, sizeof(beacon_t));
		mess->localTime=call LocalTime.get();
		//call BeaconScheduler.RadioStop(mess->localTime+10);
		call TimeSyncAMSend.send(TOS_BCAST_ADDR, &beacon, sizeof(beacon_t), mess->localTime);
		if(nextBeacon==0)
			nextBeacon=mess->localTime+BEACON_INTERVAL;
		else
			nextBeacon+=BEACON_INTERVAL;
	}
	
	event void TimeSyncAMSend.sendDone(message_t *msg, error_t error){
		uint8_t i=0;
		uint32_t localtime=call LocalTime.get();
		beacon_t* mess=call Packet.getPayload(&beacon, sizeof(beacon_t));
		if((mess->flags&FLAG_DATA)==0)
			call BeaconScheduler.RadioStop(localtime);
		else {
			mess->flags&=~FLAG_DATA;
			call Timer.startOneShot(BEACON_INTERVAL);
			call SubSend.send[call AMPacket.type(data)](call AMPacket.destination(data), data, call Packet.payloadLength(data));
		}
		for(i=0;i<MAX_NEIGHBOR;i++){
			if(nt[i].lost>0){
				nt[i].lost--;
				if(nt[i].lost<LOST_LIMIT){
					nt[i].lost=0;
				}else{
					while(nt[i].nextwake<localtime){
						nt[i].nextwake+=BEACON_INTERVAL;
					}
					call NetworkScheduler.RadioStart[i](nt[i].nextwake);
				}
			}
		}
		
	}
	

	event void BeaconScheduler.RadioStopDone(error_t error){
		if(error!=SUCCESS){
			call BeaconScheduler.RadioStop(0);
		} else {
			call BeaconScheduler.RadioStart(nextBeacon);
		}	
	}
	
	event void NetworkScheduler.RadioStartDone[uint8_t user_id](error_t error){
		nt[user_id].nextsleep=call LocalTime.get()+WAKE_TIME;
		call NetworkScheduler.RadioStop[user_id](nt[user_id].nextsleep);//this should be enough to receive a message
	}

	event void NetworkScheduler.RadioStopDone[uint8_t user_id](error_t error){
		if(error==ECANCEL){
			nt[user_id].nextsleep=call LocalTime.get()+BEACON_INTERVAL;
			call NetworkScheduler.RadioStop[user_id](nt[user_id].nextsleep);
		}
	}

	event message_t * TimeSyncReceive.receive(message_t *msg, void *payload, uint8_t len){
		uint8_t i=0;
		beacon_t* mess=call Packet.getPayload(msg, sizeof(beacon_t));
		while(i<MAX_NEIGHBOR){
			if(nt[i].nodeid==call TimeSyncAMPacket.source(msg))
				break;
			i++;
		}
		if(i!=MAX_NEIGHBOR){//refresh the data of a neighbor
			if(call TimeSyncPacket.isValid(msg)){
				nt[i].nextwake=call TimeSyncPacket.eventTime(msg)+BEACON_INTERVAL;
			} else
				nt[i].nextwake+=BEACON_INTERVAL;
			call NetworkScheduler.RadioStopCancel[i]();
			wakenBy=call AMPacket.source(msg);
			call Leds.led2Toggle();
			nt[i].lost=0xff;
		} else if(call TimeSyncPacket.isValid(msg)){//if the timestamp is not valid, we can't insert the source as a new neighbor
			uint32_t farestwaketime=0;
			uint8_t farestwaketimeuser=MAX_NEIGHBOR;
			i=0;
			while(i<MAX_NEIGHBOR){
				if(nt[i].lost==0)
					break;
				if(nextBeacon-nt[i].nextwake>farestwaketime){
					farestwaketime=nextBeacon-nt[i].nextwake;
					farestwaketimeuser=i;
				}
				i++;
			}
			if(i!=MAX_NEIGHBOR){//insert a new neighbor into an empty place
				call Leds.led1Toggle();
				nt[i].lost=0xff;
				nt[i].nodeid=call TimeSyncAMPacket.source(msg);
				nt[i].nextwake=call TimeSyncPacket.eventTime(msg)+BEACON_INTERVAL;
			} else if(farestwaketimeuser!=MAX_NEIGHBOR||call TimeSyncAMPacket.source(msg)<=MAX_BS_NODEID){ //insert a new neighbor instead of a worse one, or insert a basestation instead of the worst one
				call Leds.led1Toggle();
				nt[i].lost=0xff;
				nt[i].nodeid=call TimeSyncAMPacket.source(msg);
				nt[i].nextwake=call TimeSyncPacket.eventTime(msg)+BEACON_INTERVAL;
			} 
		}
		if(call TimeSyncPacket.isValid(msg)&&call PacketTimeStampMilli.isValid(msg)){
			signal TimeSyncPoints.syncPoint(call PacketTimeStampMilli.timestamp(msg), call TimeSyncAMPacket.source(msg), call TimeSyncPacket.eventTime(msg)+mess->localTime);
		}
		return msg;
	}
	
	event void Timer.fired(){
		wakenBy=TOS_BCAST_ADDR;
	}
	
	default event message_t * Receive.receive[am_id_t am_id](message_t *msg, void *payload, uint8_t len){
		return msg;
	};
	
	default event message_t * Snoop.receive[am_id_t am_id](message_t *msg, void *payload, uint8_t len){
		return msg;
	};
	
	default event void AMSend.sendDone[am_id_t am_id](message_t *msg, uint8_t len){};
	
}