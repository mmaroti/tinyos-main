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

		interface Queue<message_t*> as QueueCtp;
		interface Queue<message_t*> as QueueSerial;
		interface Queue<message_t*> as QueueTemp;
	
		interface DiagMsg;
	}
}
implementation{
	/*
	enum {
		SEND_INTERVAL = 8192
	};
	*/

	uint16_t seqno;
	uint8_t readings = 0;	//from 0 to NREADINGS
	//uint8_t serialmsgs = 0;	//from 0 to NSERIALMSGS
	
	message_t*	m_temp;
	GH_Msg*		gh_temp;
	
	bool CTPsendBusy = FALSE;
	bool UARTsendBusy = FALSE;

	task void ctpSendTask();
	task void uartSendTask();

	//Piros LED = HIBA
	void errorBlink()	{ call Leds.led0Toggle(); }
	//Zöld LED = MÉRÉS
	void radioreadBlink()	{ call Leds.led1Toggle(); }
	//Kék LED = SOROS PORTRA KÜLDÉS HIBA
	void serialErrorOn(){ call Leds.led2On(); }
	void serialErrorOff(){ call Leds.led2Off(); }
	
	event void Boot.booted(){
		call SerialControl.start();
		call RadioControl.start();
		call CtpControl.start();

		if (TOS_NODE_ID /*% 500*/ == 0) {
			call RootControl.setRoot();
		}
		
		seqno = 0;
		m_temp = NULL;
		
		call Timer.startPeriodic(DEFAULT_INTERVAL);
	}
	
	event void RadioControl.startDone(error_t err) {
	
		if( err != SUCCESS );
			//errorBlink();
	}
	event void SerialControl.startDone(error_t err) {
	
		if( err != SUCCESS );
			//errorBlink();
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
		}
	}
	
	//void ctpSendTask(void);
	
	event void Read.readDone(error_t result, uint16_t val ){
	
		if(	m_temp == NULL )		//Ha nincs aktív buffer
		{
			if( !call QueueTemp.empty() )			//és a szabad bufferek listája nem üres (azaz van szabad buffer)
				m_temp = call QueueTemp.dequeue();	//lefoglaljuk a buffert.
			else
				return;								//Egyébként eldobjuk az adatot.
		}
		
		gh_temp = (GH_Msg*)call CtpSend.getPayload(m_temp, sizeof(GH_Msg));
		//Ha sikerült leolvasni a szenzorértéket
		if(result == SUCCESS)
		{
			gh_temp->data[readings++] = /*-39.6 + */0.04 * val;		//0.04 = 1/256 = 1/(2^8) = 1 bájttal jobbra shiftelés
		}
		//Ha megtelt
		if(readings == NREADINGS)
		{
			call QueueCtp.enqueue( m_temp );
			post ctpSendTask();
			m_temp = NULL;
		}
	} 
	
	task void ctpSendTask() {
		am_addr_t parent;
		uint16_t metric;
		//Ha van elküldendõ üzenet
		if( !QueueCtp.empty() )
		{
			m_temp = call QueueCtp.head();
			gh_temp = (GH_Msg*)call CtpSend.getPayload(m_temp, sizeof(GH_Msg));
			
			if( call CtpInfo.getParent(&parent) == FAIL )
			{
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("getParent failure...");
					call DiagMsg.send();
				}
			}
			if( call CtpInfo.getEtx(&metric) == FAIL )
			{
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("getEtx failure...");
					call DiagMsg.send();
				}
			}
			gh_temp->source = TOS_NODE_ID;
			gh_temp->parent = parent;
			gh_temp->metric = metric;
			gh_temp->seqno = seqno;
		}
		//Ha szabad a rádió
		if(!CTPsendBusy)
		{
			if ( call CtpSend.send( m_temp, sizeof(GH_Msg))== SUCCESS )
			{
				//call QueueCtp.dequeue();
				CTPsendBusy = TRUE;
				seqno++;
			}
			else
			{
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("CTP packet was not accepted to send...");
					call DiagMsg.send();
				}
				//errorBlink();
			}
		}
		post ctpSendTask();
	}
	
	event void CtpSend.sendDone(message_t* m, error_t err) {
		if (err != SUCCESS)
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("Failure sending packet...");
				call DiagMsg.send();
			}
			//errorBlink();
		}
		else
		{
			call QueueCtp.dequeue();
		}
		
		post ctpSendTask();
		
		CTPsendBusy = FALSE;
		readings = 0;
		
	}
	
	event message_t* CtpReceive.receive(message_t* msg, void* payload, uint8_t len) {

		GH_Msg* ghmsg;
		
		//atomic /*NEM KELL, mert taskok szinkron futnak*/	//ha nincs async kulcsszó ÉS task kontextusban dolgozunk => nem kell atomic
		//{
			/*
			UART_Msg* uartmsg = (UART_Msg*)call SerialSend.getPayload(&UARTpacket, sizeof(UART_Msg));
			uartmsg -> messages[serialmsgs++] = 
			*/
			//Üzenet "másolása"
			if( !call QueueSerial.empty() )
				call QueueSerial.enqueue( msg );
				
			if (!UARTsendBusy)
			{
				post uartSendTask();
			}
			else
			{
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("Serial busy...");
					call DiagMsg.send();
				}
			}
		//}
		if( call !QueueTemp.empty() )
			return call QueueTemp.dequeue();
		else
			return msg;
	}
  
	task void uartSendTask() {
		if( !call QueueSerial.empty() && !uartSendBusy )
		{
			if( call SerialSend.send(AM_BROADCAST_ADDR, call QueueSerial.head(), sizeof(GH_Msg)) == SUCCESS )
			{
				//call QueueSerial.dequeue()
				UARTsendBusy = TRUE;
				serialErrorOff();
			}
			else
			{
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("UART packet was not accepted to send...");
					call DiagMsg.send();
				}
				serialErrorOn();
			}		
		}
		post uartSendTask();
	}
	
	event void SerialSend.sendDone(message_t *msg, error_t error) {
		
		GH_Msg* ghmsg = (GH_Msg*)call SerialSend.getPayload(msg, sizeof(GH_Msg));
		
		if (error != SUCCESS)
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("SerialSend.sendDone error.");
				call DiagMsg.send();
			}
			serialErrorOn();
		}
		else
		{
			call QueueSerial.dequeue();
			call QueueTemp.enqueue( msg );
			serialErrorOff();
			
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("SerialSend.sendDone");
				call DiagMsg.uint16( ghmsg -> source);
				call DiagMsg.uint16( ghmsg -> seqno);
				call DiagMsg.uint16( ghmsg -> parent);
				call DiagMsg.uint16( ghmsg -> metric);
				call DiagMsg.uint16s( (uint16_t*)(ghmsg -> data), NREADINGS );
				call DiagMsg.send();
			}
		}
		
		UARTsendBusy = FALSE;
	}
}