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
	uint8_t readings = 0;	//from 0 to NREADINGS
	
	//GH_Msg* ghmsg;
	
	bool CTPsendBusy = FALSE;
	bool serialsendBusy = FALSE;
	
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
		
		call Timer.startPeriodic(DEFAULT_INTERVAL);
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
	
	event void Timer.fired() {
		
		if( readings < NREADINGS )	//Átrakható lenne a Read.readDone()-ba.
									//Szintén ott else ágon bufferelés.
		{
			/*readings ellenõrzése!!!!!!!!*/
			
			
			call Read.read();
			//readings++;	//Read.readDone() növeli		?!
		
			call Leds.led2Toggle();
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("Timer");
				call DiagMsg.uint8(readings);
				call DiagMsg.uint16(seqno);
				call DiagMsg.uint8(CTPsendBusy);
				call DiagMsg.send();
			}
		}
	}
	
	void sendMessage(void);
	
	event void Read.readDone(error_t result, uint16_t val ){
		GH_Msg* ghmsg = (GH_Msg*)call CtpSend.getPayload(&packet, sizeof(GH_Msg));
		
		if(result==SUCCESS){
			ghmsg->data[readings++] = /*-3960+*/(int32_t)val;
		}
		
		if(readings == NREADINGS)
		{
			sendMessage();
		}
	} 
	
	void sendMessage() {
		uint16_t metric;
		am_addr_t parent;
		
		GH_Msg* ghmsg = (GH_Msg*)call CtpSend.getPayload(&packet, sizeof(GH_Msg));
		
		call CtpInfo.getParent(&parent);
		call CtpInfo.getEtx(&metric);

		ghmsg->source = TOS_NODE_ID;
		ghmsg->parent = parent;
		ghmsg->metric = metric;
		ghmsg->seqno = seqno;
		//ghmsg->hopcount = 0;
		
		if(!CTPsendBusy)
		{
			if ( call CtpSend.send(&packet, sizeof(GH_Msg))!= SUCCESS)
			{
				call Leds.led0Toggle();
			}
			else 
			{
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("CtpSend");
					call DiagMsg.uint16(TOS_NODE_ID);
					call DiagMsg.uint16(seqno);
					call DiagMsg.uint16(parent);
					call DiagMsg.uint16(metric);
					call DiagMsg.send();
				}
				CTPsendBusy = TRUE;
				seqno++; 
			}
		}
	}
	
	
	event void CtpSend.sendDone(message_t* m, error_t err) {
		if (err != SUCCESS) {
			call Leds.led0On();
		}
		CTPsendBusy = FALSE;
		
		readings = 0;
		
	}
		
	event message_t* CtpReceive.receive(message_t* msg, void* payload, uint8_t len) {
		GH_Msg* ghmsg = (GH_Msg*)payload;
		
		if( call RootControl.isRoot() )
		{
			//Send to serial port
			memcpy(	call SerialSend.getPayload(&packet, sizeof(GH_Msg)), &ghmsg, sizeof(GH_Msg)	);
			if (call SerialSend.send(AM_BROADCAST_ADDR, &packet, sizeof(GH_Msg) ) == SUCCESS)
				serialsendBusy = TRUE;
		}
		
		
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("CtpReceive");
			call DiagMsg.uint16( ghmsg -> source);
			call DiagMsg.uint16( ghmsg -> seqno);
			call DiagMsg.uint16( ghmsg -> parent);
			call DiagMsg.uint16( ghmsg -> metric);
			call DiagMsg.uint16s( ghmsg -> data, 2 );
			call DiagMsg.send();
		}
		return msg;
	}
	
	event void SerialSend.sendDone(message_t *msg, error_t error) {
	
	    serialsendBusy = FALSE;
	}
	
}