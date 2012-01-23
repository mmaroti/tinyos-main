#include <Timer.h>
#include "GreenHouse.h"

module GreenHouseP{
	
	uses{
		
		interface Boot;
		interface Leds;
		interface StdControl as CtpControl;
		interface RootControl;
		interface SplitControl as RadioControl;
		interface SplitControl as SerialControl;
		
		interface Timer<TMilli>;
		interface Read<uint16_t>;
	
		interface CollectionPacket;
		interface CtpInfo;
		interface CtpCongestion;
		
		interface Send as CtpSend;
		interface Receive as CtpReceive;
		
		interface AMSend as SerialSend;
	
		interface DiagMsg;
	}
}
implementation{

	message_t packet;
	uint16_t seqno;
	uint16_t counter = 0;
	
	uint8_t measuring;	//from 0 to MEASURINGS
	
	bool sendBusy = FALSE;
	
	enum {
		SEND_INTERVAL = 8192
	};

	
	event void Boot.booted(){
		call SerialControl.start();
		call RadioControl.start();
		call CtpControl.start();

		if (TOS_NODE_ID /*% 500*/ == 0) {
			call RootControl.setRoot();
		}
		
		seqno = 0;
		
		call Timer.startPeriodic(1000);
	}
	
	event void RadioControl.startDone(error_t err) {
		
		if( err != SUCCESS )
			call Leds.led0On();
	}
	event void SerialControl.startDone(error_t err) {
	
		if( err != SUCCESS )
			call Leds.led0On();
	}
	
	event void RadioControl.stopDone(error_t err) {}
	event void SerialControl.stopDone(error_t err) {}
	
	event void Read.readDone(error_t result, uint16_t val ){
		GH_Msg* gh_msg = (GH_Msg*)call CtpSend.getPayload(&packet, sizeof(GH_Msg));
		
		if(result==SUCCESS){
			gh_msg->data[measuring] = -3960+(int32_t)val;
			measuring++;
		}
	} 
	
	void sendMessage() {
		uint16_t metric;
		am_addr_t parent;
		
		GH_Msg* gh_msg = (GH_Msg*)call CtpSend.getPayload(&packet, sizeof(GH_Msg));
		
		call CtpInfo.getParent(&parent);
		call CtpInfo.getEtx(&metric);

		gh_msg->source = TOS_NODE_ID;
		gh_msg->parent = parent;
		gh_msg->metric = metric;
		gh_msg->seqno = seqno;
		//gh_msg->hopcount = 0;
		
		if ( call CtpSend.send(&packet, sizeof(GH_Msg))!= SUCCESS) {
			call Leds.led0Toggle();
		}
		else {
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("CtpSend");
				call DiagMsg.uint16(TOS_NODE_ID);
				call DiagMsg.uint16(seqno);
				call DiagMsg.uint16(parent);
				call DiagMsg.uint16(metric);
				call DiagMsg.send();
			}
			sendBusy = TRUE;
			seqno++; 
		}
	}
	
	event void Timer.fired() {
		
		if( measuring < MEASURINGS )
		{
			call Read.read();
			//measuring++;	//Read.readDone() növeli
		
			call Leds.led2Toggle();
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("Timer");
				call DiagMsg.uint8(measuring);
				call DiagMsg.uint16(seqno);
				call DiagMsg.uint8(sendBusy);
				call DiagMsg.send();
			}
		}
		else if (!sendBusy)
		{
			sendMessage();
		}
	}
	
	
	event void CtpSend.sendDone(message_t* m, error_t err) {
		if (err != SUCCESS) {
			call Leds.led0On();
		}
		sendBusy = FALSE;
		measuring = 0;
		
	}
	
	event message_t* CtpReceive.receive(message_t* msg, void* payload, uint8_t len) {
		GH_Msg* gh_msg = (GH_Msg*)payload;
		
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("CtpReceive");
			call DiagMsg.uint16( gh_msg -> source);
			call DiagMsg.uint16( gh_msg -> seqno);
			call DiagMsg.uint16( gh_msg -> parent);
			call DiagMsg.uint16( gh_msg -> metric);
			call DiagMsg.uint16s( gh_msg -> data, 10 );
			call DiagMsg.send();
		}
		return msg;
	}
	
	event void SerialSend.sendDone(message_t *msg, error_t error) {	}
}