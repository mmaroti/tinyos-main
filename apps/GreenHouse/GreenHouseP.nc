#include <Timer.h>
#include "GreenHouse.h"
/**
komment teszt
*/
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
		//Szenzorérték periodikus leolvasását végzõ interfészek
		interface Timer<TMilli>;
		interface Read<uint16_t>;
		//CTP protokollal való kapcsolatot megvalósító interfészek
		interface CollectionPacket;
		interface CtpInfo;
		interface CtpCongestion;
		interface Send as CtpSend;
		interface Receive as CtpReceive;
		//Soros kapcsolatot megvalósító interfész
		interface AMSend as SerialSend;
		//A csomagforgalom bufferelését megvalósító interfészek
		interface Queue<message_t*> as QueueCtp;
		interface Queue<message_t*> as QueueSerial;
		interface Queue<message_t*> as QueueTemp;
		//Rendszer diagnosztizálás
		interface DiagMsg;
	}
}

implementation
{
	uint16_t seqno;				//A szenzorlapka által küldött csomagok száma
	uint8_t readings = 0;		//0 - NREADINGS között változhat
	
	//message_t a_queueCtp[QUEUE_SIZE];
	//message_t a_queueSerial[QUEUE_SIZE];
	message_t a_queueTemp[QUEUE_SIZE];
	message_t*	m_temp;
	
	//Kommunikációs csatornák foglaltáság jelzõ "flag"-ek
	bool CTPsendBusy = FALSE;
	bool UARTsendBusy = FALSE;
	//Kommunikációs csatornákon való küldést végzõ taszkok
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

		if (TOS_NODE_ID % 200 == 0) {
			call RootControl.setRoot();
		}
		
		seqno = 0;
		//Küldési sorok feltöltése mutatókkal
		for(i = 0; i < QUEUE_SIZE; i++)
		{
			/*
			call QueueCtp.enqueue( &a_queueCtp[i] );
			call QueueSerial.enqueue( &a_queueSerial[i] );
			*/
			call QueueTemp.enqueue( &a_queueTemp[i] );
		}
		m_temp = NULL;
		//Idõzítõ elindítása
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
	//Esemény: Idõzítõ lejárt
	event void Timer.fired()
	{
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("Timer fired");
			call DiagMsg.send();
		}
		
		if( readings < NREADINGS )
		{
			call Read.read();
			//readings++;	//Read.readDone() növeli
		
			radioreadBlink();
		}
	}
	//Esemény: szenzor leolvasása kész
	event void Read.readDone(error_t result, uint16_t val )
	{
		GHMsg*	gh_temp;
		//Ha nincs aktív buffer
		if(	m_temp == NULL )
		{
			//és a szabad bufferek listája nem üres (azaz van szabad buffer)
			if( !call QueueTemp.empty() )
				//lefoglalunk egy buffert.
				m_temp = call QueueTemp.dequeue();
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
		
		gh_temp = (GHMsg*)call CtpSend.getPayload(m_temp, sizeof(GHMsg));
		//Ha sikerült leolvasni a szenzorértéket
		if(result == SUCCESS)
		{
			gh_temp->data[readings++] = val;
			
			//Ha összegyûlt egy csomagnyi mért adat
			if(readings == NREADINGS)
			{
				//Az üzenet bekerül a CTP küldési sorba
				if( call QueueCtp.enqueue( m_temp ) == SUCCESS )
				{
					m_temp = NULL;
					readings = 0;
					post ctpSendTask();		//Küldés CTP-nek...
					
					if( call DiagMsg.record() )
					{
						call DiagMsg.str("Read.readDone.");
						call DiagMsg.str("QueueCtp size:");
						call DiagMsg.uint8(call QueueCtp.size());
						call DiagMsg.send();
					}
				}
				else
				{
					if( call DiagMsg.record() )
					{
						call DiagMsg.str("CTP packet dropped...");
						call DiagMsg.send();
					}
				}
				
			}
		}
	} 
	//Taszk: csomag küldése a CTP motornak
	task void ctpSendTask()
	{
		GHMsg*	gh_temp;
		am_addr_t parent;
		uint16_t metric;
		
		//Ha van elküldendõ üzenet
		if( !call QueueCtp.empty() )
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("CTP send task.");
				call DiagMsg.str("QueueCtp size:");
				call DiagMsg.uint8(call QueueCtp.size());
				call DiagMsg.send();
			}
			//Beállítjuk a többi mezõ értékét
			gh_temp = (GHMsg*)call CtpSend.getPayload( call QueueCtp.head(), sizeof(GHMsg) );
			
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
		else
			return;
			
		//Ha szabad a rádió
		if(!CTPsendBusy)
		{
			//és a hálózati réteg elfogadja a csomagot
			if ( call CtpSend.send( call QueueCtp.head(), sizeof(GHMsg) ) == SUCCESS )
			{
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
		//post ctpSendTask();
	}
	//Esemény: küldés befejezve
	event void CtpSend.sendDone(message_t* m, error_t err)
	{
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("CtpSend.sendDone");
			call DiagMsg.send();
		}
		//Sikeres küldés esetén a mutatót kikerül a CTP sorból,
		//és átkerül az szabad mutatók listájába.
		if (err == SUCCESS)
		{
			call QueueCtp.dequeue();
			call QueueTemp.enqueue(m);
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
		
		//post ctpSendTask();
		
		CTPsendBusy = FALSE;
		//readings = 0;
		
	}
	//Esemény: csomag érkezett
	//Csak a CTP gyökere kapja meg.
	event message_t* CtpReceive.receive(message_t* msg, void* payload, uint8_t len)
	{
		message_t* p_ret;
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("CtpReceive.receive");
			call DiagMsg.send();
		}
		//Üzenet "másolása"
		if( call QueueSerial.enqueue( msg ) == SUCCESS )
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("CtpReceive.receive");
				call DiagMsg.str("QueueSerial size:");
				call DiagMsg.uint8(call QueueSerial.size());
				call DiagMsg.send();
			}
			p_ret = msg;
		}
		//Ha van üres buffer, azzal térünk vissza,
		//hogy a fogadott csomagot legyen idõ feldolgozni.
		else if( !call QueueTemp.empty() )
			p_ret = call QueueTemp.dequeue();
		else
			p_ret = msg;
			
		post uartSendTask();	//Küldés PC-re...
		/*
		if (UARTsendBusy)
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("Serial busy...");
				call DiagMsg.send();
			}
		}
		*/
		return p_ret;
	}
	//Taszk: csomag küldése a soros portra
	task void uartSendTask()
	{
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("UART sender task.");
			call DiagMsg.send();
		}
		//Ha van elküldendõ csomag, és szabad a csatorna.
		if( !call QueueSerial.empty() && !UARTsendBusy )
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("UART sender task.");
				call DiagMsg.str("QueueSerial size:");
				call DiagMsg.uint8(call QueueSerial.size());
				call DiagMsg.send();
			}
			if( call SerialSend.send(AM_BROADCAST_ADDR, call QueueSerial.head(), sizeof(GHMsg)) == SUCCESS )
			{
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
	//Esemény: soros porton való küldés befejezve
	event void SerialSend.sendDone(message_t *msg, error_t error)
	{
		//GHMsg* gh_temp;
		
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("SerialSend.sendDone");
			call DiagMsg.send();
		}
		//Sikeres küldés esetén a csomag mutató visszakerül a szabad bufferek listájába
		if (error == SUCCESS)
		{
			call QueueSerial.dequeue();
			call QueueTemp.enqueue( msg );
			serialErrorOff();
			/*
			gh_temp = (GHMsg*)call SerialSend.getPayload(msg, sizeof(GHMsg));
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
			*/
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