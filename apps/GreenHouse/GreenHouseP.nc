#include <Timer.h>
#include "GreenHouse.h"

module GreenHouseP
{
	uses
	{
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

implementation
{
	/*
	enum {
		SEND_INTERVAL = 8192
	};
	*/

	uint16_t seqno;
	uint8_t readings = 0;	//from 0 to NREADINGS
	//uint8_t serialmsgs = 0;	//from 0 to NSERIALMSGS
	
	message_t a_queueCtp[QUEUE_SIZE];
	message_t a_queueSerial[QUEUE_SIZE];
	message_t a_queueTemp[QUEUE_SIZE];
	message_t*	m_temp;
	
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
	
	event void Boot.booted()
	{
		int i;
		call SerialControl.start();
		call RadioControl.start();
		call CtpControl.start();

		if (TOS_NODE_ID /*% 500*/ == 0) {
			call RootControl.setRoot();
		}
		
		seqno = 0;
		//Küldési sorok feltöltése mutatókkal
		for(i = 0; i < QUEUE_SIZE; i++)
		{
			call QueueCtp.enqueue( &a_queueCtp[i] );
			call QueueCtp.enqueue( &a_queueSerial[i] );
			call QueueCtp.enqueue( &a_queueTemp[i] );
		}
		m_temp = NULL;
		
		call Timer.startPeriodic(DEFAULT_INTERVAL);
	}
	
	event void RadioControl.startDone(error_t err)
	{
		if( err != SUCCESS )
			errorBlink();
	}
	event void SerialControl.startDone(error_t err)
	{
		if( err != SUCCESS )
			errorBlink();
	}
	
	event void RadioControl.stopDone(error_t err) {}
	event void SerialControl.stopDone(error_t err) {}
	
	event void Timer.fired()
	{
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("Timer fired");
			call DiagMsg.send();
		}
		
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
	
	event void Read.readDone(error_t result, uint16_t val )
	{
		GH_Msg*	gh_temp;
		
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("Read.readDone");
			call DiagMsg.send();
		}
		if(	m_temp == NULL )		//Ha nincs aktív buffer
		{
			if( !call QueueTemp.empty() )			//és a szabad bufferek listája nem üres (azaz van szabad buffer)
				m_temp = call QueueTemp.dequeue();	//lefoglalunk egy buffert.
			else
			{
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("QueueTemp is empty...");
					call DiagMsg.send();
				}
				return;				//Egyébként eldobjuk az adatot.
			}
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
			post ctpSendTask();
			if( call QueueCtp.enqueue( m_temp ) == FAIL )
			{
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("CTP packet dropped...");
					call DiagMsg.send();
				}
			}
			else
			{
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("Read.readDone.");
					call DiagMsg.str("QueueCtp size:");
					call DiagMsg.uint8(call QueueCtp.size());
					call DiagMsg.send();
				}
			}
			call QueueTemp.enqueue( m_temp );
			m_temp = NULL;
		}
	} 
	
	task void ctpSendTask()
	{
		GH_Msg*	gh_temp;
		am_addr_t parent;
		uint16_t metric;
		
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("CTP send task.");
			call DiagMsg.send();
		}
		//Ha van elküldendõ üzenet
		if( !call QueueCtp.empty() )
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("CTP sending task.");
				call DiagMsg.str("QueueCtp size:");
				call DiagMsg.uint8(call QueueCtp.size());
				call DiagMsg.send();
			}
			gh_temp = (GH_Msg*)call CtpSend.getPayload( call QueueCtp.head(), sizeof(GH_Msg) );
			
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
			if ( call CtpSend.send( call QueueCtp.head(), sizeof(GH_Msg) ) == SUCCESS )
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
				errorBlink();
			}
		}
		post ctpSendTask();
	}
	
	event void CtpSend.sendDone(message_t* m, error_t err)
	{
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("CtpSend.sendDone");
			call DiagMsg.send();
		}
		if (err == SUCCESS)
		{
			call QueueCtp.dequeue();
		}
		else
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("Failure sending packet...");
				call DiagMsg.send();
			}
			errorBlink();
		}
		
		post ctpSendTask();
		
		CTPsendBusy = FALSE;
		readings = 0;
		
	}
	
	event message_t* CtpReceive.receive(message_t* msg, void* payload, uint8_t len)
	{
		message_t* p_ret;
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("CtpReceive.receive");
			call DiagMsg.send();
		}
		//atomic /*NEM KELL, mert taskok szinkron futnak*/	//ha nincs async kulcsszó ÉS task kontextusban dolgozunk => nem kell atomic
		//{
			/*
			UART_Msg* uartmsg = (UART_Msg*)call SerialSend.getPayload(&UARTpacket, sizeof(UART_Msg));
			uartmsg -> messages[serialmsgs++] = 
			*/
			//Üzenet "másolása"
			if( call QueueSerial.enqueue( msg ) == SUCCESS )
			{
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("UART sending task.");
					call DiagMsg.str("QueueSerial size:");
					call DiagMsg.uint8(call QueueSerial.size());
					call DiagMsg.send();
				}
				p_ret = msg;
			}
			else if( !call QueueTemp.empty() )
				p_ret = call QueueTemp.dequeue();
			else
				p_ret = msg;
				
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
		return p_ret;
	}
  
	task void uartSendTask()
	{
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("UART send task.");
			call DiagMsg.send();
		}
		if( !call QueueSerial.empty() && !UARTsendBusy )
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("UART sending task.");
				call DiagMsg.str("QueueSerial size:");
				call DiagMsg.uint8(call QueueSerial.size());
				call DiagMsg.send();
			}
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
	
	event void SerialSend.sendDone(message_t *msg, error_t error)
	{
		GH_Msg* gh_temp;
		
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("SerialSend.sendDone");
			call DiagMsg.send();
		}
		if (error == SUCCESS)
		{
			call QueueSerial.dequeue();
			call QueueTemp.enqueue( msg );
			serialErrorOff();
			
			gh_temp = (GH_Msg*)call SerialSend.getPayload(msg, sizeof(GH_Msg));
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("SerialSend.sendDone");
				call DiagMsg.uint16( gh_temp -> source);
				call DiagMsg.uint16( gh_temp -> seqno);
				call DiagMsg.uint16( gh_temp -> parent);
				call DiagMsg.uint16( gh_temp -> metric);
				call DiagMsg.uint16s( (uint16_t*)(gh_temp -> data), NREADINGS );
				call DiagMsg.send();
			}
		}
		else
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("SerialSend.sendDone error.");
				call DiagMsg.send();
			}
			serialErrorOn();
		}
		
		UARTsendBusy = FALSE;
	}
}