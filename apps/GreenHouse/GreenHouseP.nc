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
	/*
	enum {
		SEND_INTERVAL = 8192
	};
	*/
	message_t packet;
	uint16_t seqno;
	uint8_t readings = 0;	//from 0 to NREADINGS
	
	//GH_Msg* ghmsg;
	
	bool CTPsendBusy = FALSE;
	bool serialsendBusy = FALSE;
	
	message_t buffer;

	task void uartSendTask();

	//Piros LED = HIBA
	void errorBlink()	{ call Leds.led0Toggle(); }
	//Zöld LED = MÉRÉS
	void radioreadBlink()	{ call Leds.led1Toggle(); }
	//Kék LED = KÜLDÉS SOROS PORTRA
	void serialsendBlink(){ call Leds.led2Toggle(); }
	
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
			errorBlink();
	}
	event void SerialControl.startDone(error_t err) {
	
		if( err != SUCCESS )
			errorBlink();
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
		
			radioreadBlink();
			/*
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("Timer");
				call DiagMsg.uint8(readings);
				call DiagMsg.uint16(seqno);
				call DiagMsg.uint8(CTPsendBusy);
				call DiagMsg.send();
			}
			*/
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
		am_addr_t parent;
		uint16_t metric;
		
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
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("CTPsendBusy");
					call DiagMsg.send();
				}
				errorBlink();
			}
			else
			{
				/*
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("CtpSend");
					call DiagMsg.uint16(ghmsg -> source);
					call DiagMsg.uint16(ghmsg -> seqno);
					call DiagMsg.uint16(ghmsg -> parent);
					call DiagMsg.uint16(ghmsg -> metric);
					call DiagMsg.send();
				}
				*/
				CTPsendBusy = TRUE;
				seqno++; 
			}
		}
	}
	
	event void CtpSend.sendDone(message_t* m, error_t err) {
		if (err != SUCCESS)
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("CtpSend.sendDone error");
				call DiagMsg.send();
			}
			errorBlink();
		}
		CTPsendBusy = FALSE;
		
		readings = 0;
		
	}
	
	message_t* /*ONE */receive(message_t* /*ONE */msg, void* payload, uint8_t len);
		
	event message_t* CtpReceive.receive(message_t* msg, void* payload, uint8_t len) {
		
		if( call RootControl.isRoot() )
		{
			//Send to serial port
			/*
			 * SerialSend -> AMSenderC konfiguráció esetén
			 *********************************			
			memcpy(	call SerialSend.getPayload(&packet, sizeof(GH_Msg)), &ghmsg, sizeof(GH_Msg)	);
			if (call SerialSend.send(AM_BROADCAST_ADDR, &packet, sizeof(GH_Msg) ) == SUCCESS)
				serialsendBusy = TRUE;
			 */
			receive(msg, payload, len);
		}
		/*
		GH_Msg* ghmsg = (GH_Msg*)payload;
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
		*/
		return msg;
	}
	
	message_t* receive(message_t *msg, void *payload, uint8_t len) {

		/*
		GH_Msg* ghmsg = (GH_Msg*)payload;	//SZÜKSÉGES???
		
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("CtpReceive");
			call DiagMsg.uint16( ghmsg -> source);
			call DiagMsg.uint16( ghmsg -> seqno);
			call DiagMsg.uint16( ghmsg -> parent);
			call DiagMsg.uint16( ghmsg -> metric);
			call DiagMsg.uint16s( (uint16_t*)(ghmsg -> data), 2 );
			call DiagMsg.send();
		}
		*/
		
		//atomic /*NEM KELL, mert taskok szinkron futnak*/
		//ha nincs async kulcsszó ÉS task kontextusban dolgozunk => nem kell atomic
		//{

			if (!serialsendBusy)
			{
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("Not busy.");
					call DiagMsg.send();
				}
				//üzenet MÁSOLÁSA
				memcpy(&buffer, msg, sizeof (message_t));
				post uartSendTask();
				serialsendBusy = TRUE;
			}
			else
			{
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("Busy.");
					call DiagMsg.send();
				}
				errorBlink();
			}
		//}
		return msg;
	}
  
	//uint8_t tmpLen;
  
	task void uartSendTask() {
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("Task");
			call DiagMsg.send();
		}
		/*
		uint8_t len;
		am_id_t id;
		am_addr_t addr, src;
		message_t* msg;
		am_group_t grp;
		*/
		/*
		atomic
		  if (uartIn == uartOut && !uartFull)
		{
		  serialsendBusy = FALSE;
		  return;
		}
		*/
		/*
		 *BaseStationP.nc
		 *******************************************************************
		tmpLen = len = call RadioPacket.payloadLength(msg);
		id = call RadioAMPacket.type(msg);
		addr = call RadioAMPacket.destination(msg);
		src = call RadioAMPacket.source(msg);
		grp = call RadioAMPacket.group(msg);
		call UartPacket.clear(msg);
		call UartAMPacket.setSource(msg, src);
		call UartAMPacket.setGroup(msg, grp);
		 *******************************************************************
		 */
		if (call SerialSend.send(AM_BROADCAST_ADDR, &buffer, sizeof(GH_Msg)) == SUCCESS)
			;//serialsendBlink();
		else
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("Task error.");
				call DiagMsg.send();
			}
			
			errorBlink();
			//if( call DiagMsg.record() )
			
			post uartSendTask();
		}
  }

	event void SerialSend.sendDone(message_t *msg, error_t error) {
		if (error != SUCCESS)
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("SerialSend.sendDone error");
				call DiagMsg.send();
			}
			errorBlink();
		}
		serialsendBusy = FALSE;
	}
	
}