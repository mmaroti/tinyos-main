/*
* Copyright (c) 2007, Vanderbilt University
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
* Author: Miklos Maroti
* Author: Krisztian Veress
*/

#include <Si443xDriverLayer.h>
#include <Tasklet.h>
#include <RadioAssert.h>
#include <TimeSyncMessageLayer.h>
#include <RadioConfig.h>
   
module Si443xDriverLayerP
{
    provides
    {
        interface Init as PlatformInit @exactlyonce();
        interface Init as SoftwareInit @exactlyonce();

        interface RadioState;
        interface RadioSend;
        interface RadioReceive;
        interface RadioCCA;
        interface RadioPacket;

        interface PacketField<uint8_t> as PacketTransmitPower;
        interface PacketField<uint8_t> as PacketRSSI;
        interface PacketField<uint8_t> as PacketTimeSyncOffset;
        interface PacketField<uint8_t> as PacketLinkQuality;
    }

    uses
    {

        interface GeneralIO as SDN;
        interface GeneralIO as NSEL;
        
        interface GpioCapture as IRQ;
            
        interface FastSpiByte;
        interface Resource as SpiResource;

        interface BusyWait<TMicro, uint16_t>;
        interface LocalTime<TRadio>;

        interface Si443xDriverConfig as Config;

        interface PacketFlag as TransmitPowerFlag;
        interface PacketFlag as RSSIFlag;
        interface PacketFlag as TimeSyncFlag;

        interface PacketTimeStamp<TRadio, uint32_t>;

        interface Tasklet;
        interface RadioAlarm;
        
        interface Boot;
        interface Leds;
    
#ifdef RADIO_DEBUG
        interface DiagMsg;
#endif
    }
}

implementation
{

/* ----------------- DEBUGGER FUNCTIONS AND HELPERS  -----------------*/
#ifdef RADIO_DEBUG
    
#define DIAGMSG_STR(PSTR,STR) \
        atomic { if( call DiagMsg.record() ) { \
            call DiagMsg.str(PSTR);\
            call DiagMsg.str(STR); \
            call DiagMsg.send(); \
        }        }

#define DIAGMSG_REG_READ(REG,VALUE)       \
        atomic { if( call DiagMsg.record() ) { \
            call DiagMsg.str("R");\
            call DiagMsg.hex8(REG);\
            call DiagMsg.hex8(VALUE);\
            call DiagMsg.send(); \
        }}

#define DIAGMSG_REG_WRITE(REG,VALUE)       \
        atomic {if( call DiagMsg.record() ) { \
            call DiagMsg.str("W");\
            call DiagMsg.hex8(REG);\
            call DiagMsg.hex8(VALUE);\
            call DiagMsg.send(); \
        }}

#endif
/* ----------------- END DEBUGGER FUNCTIONS AND HELPERS  -----------------*/    
    
    
#define IS_TX ((TOS_NODE_ID) == 1)
    
    bool isSpiAcquired();
    void serviceRadio();

    norace bool radioIrq;

    uint8_t channel;
    tasklet_norace message_t* rxMsg;
    message_t rxMsgBuffer;
    

/*----------------- REGISTER -----------------*/

    inline void writeRegister(uint8_t reg, uint8_t value)
    {
        RADIO_ASSERT( call SpiResource.isOwner() );
        RADIO_ASSERT( reg == (reg & SI443X_CMD_REGISTER_MASK) );
    
        atomic {
            call NSEL.clr();
            call FastSpiByte.splitWrite(SI443X_CMD_REGISTER_WRITE | reg);
            call FastSpiByte.splitReadWrite(value);
            call FastSpiByte.splitRead();
            call NSEL.set();
        
            DIAGMSG_REG_WRITE(reg,value);
        }
    }

	inline void fillFifo()
	{
		uint8_t i;
		
		DIAGMSG_STR("fifo","write");
		RADIO_ASSERT( call SpiResource.isOwner() );

		atomic
		{
			call NSEL.clr();
			call FastSpiByte.splitWrite(SI443X_CMD_REGISTER_WRITE | 0x7F);

			for (i = 0; i < 8; ++i)
				call FastSpiByte.splitReadWrite(i);

			call FastSpiByte.splitRead();
			call NSEL.set();
		}
	}

	inline void readFifo()
	{
		uint8_t i, a;
		uint8_t buff[8];
		
		RADIO_ASSERT( call SpiResource.isOwner() );

		atomic
		{
			call NSEL.clr();
			call FastSpiByte.splitWrite(SI443X_CMD_REGISTER_READ | 0x7F);

			for (i = 0; i < 8; ++i)
			{
				a = call FastSpiByte.splitReadWrite(0);
				if( i > 0 )
					buff[i-1] = a;
			}

			buff[7] = call FastSpiByte.splitRead();
			call NSEL.set();

			if( call DiagMsg.record() ) {
		            call DiagMsg.str("fifo read");
		            call DiagMsg.hex8s(buff, 8);
		            call DiagMsg.send();
			}
		}
	}

    inline uint8_t readRegister(uint8_t reg)
    {
        uint8_t regist = reg;
    
        RADIO_ASSERT( call SpiResource.isOwner() );
        RADIO_ASSERT( reg == (reg & SI443X_CMD_REGISTER_MASK) );
        
        atomic {
            call NSEL.clr();
            call FastSpiByte.splitWrite(SI443X_CMD_REGISTER_READ | reg);
            call FastSpiByte.splitReadWrite(0);
            reg = call FastSpiByte.splitRead();
            call NSEL.set();

	    if( regist != 0x26 )
	            DIAGMSG_REG_READ(regist,reg);
        }
	
	return reg;
    }
    
    // See Bit Twiddling Hacks - Merge bits from two values according to a mask
    // mask : 1 where bits from 'next' should be selected; 0 where from 'old'.
    inline uint8_t masked(uint8_t old, uint8_t next, uint8_t mask) {
        return old ^ ((old ^ next) & mask);
    }


    uint8_t status;
    
/*----------------- INIT -----------------*/
       
	void si443x_reset()
	{
		DIAGMSG_STR("reset","");
		readRegister(0x03);
		readRegister(0x04);
		call IRQ.captureFallingEdge();	// previous interrupts mess up the PCINT handler
		writeRegister(0x07, 0x81);
		call BusyWait.wait(30000);
		readRegister(0x03);		// we will get interrupts here, just ignore them
		readRegister(0x04);
	}

	void si443x_standby()
	{
		DIAGMSG_STR("standby","");
		writeRegister(0x05,0x00);	// tricky reset of interrupts
		writeRegister(0x06,0x00);	// we might get interrupts here, just ignore them
		writeRegister(0x05,0xFF);
		writeRegister(0x06,0xFF);
		writeRegister(0x05,0x00);
		writeRegister(0x06,0x00);
		writeRegister(0x07,0x00);	// we instantly enter standby, NO interrupt will come
		readRegister(0x62);		// to verify status
	}

	void si443x_ready()
	{
		DIAGMSG_STR("ready","");
		writeRegister(0x05,0xFF);	// reenable mask after standby
		writeRegister(0x06,0xFF);
		writeRegister(0x07,0x01);	// we instantly enter ready, NO interrupt will come
		readRegister(0x62);		// to verify status
	}

	void si443x_status()			// can be called any time
	{
		DIAGMSG_STR("status","");
		readRegister(0x02);
		readRegister(0x03);
		readRegister(0x04);
		readRegister(0x05);
		readRegister(0x06);
		readRegister(0x07);
		readRegister(0x62);
		readRegister(0x71);
		readRegister(0x26);
	}

	void si443x_transmit()
	{
		DIAGMSG_STR("transmit","");
        
		fillFifo();
        
//		writeRegister(0x3E, 0x08);  // pkt length
		writeRegister(0x05, 0xFF);  // pkt sent interrupt + TX fifo almost empty
		writeRegister(0x06, 0xFF);
		readRegister(0x03);         // flush interrupts
		readRegister(0x04);

		writeRegister(0x07, 0x09);  // tx + ready
	}

	void si443x_receive()
	{
		DIAGMSG_STR("receive","");
		
		writeRegister(0x05, 0xFF); 
		writeRegister(0x06, 0x80);
		readRegister(0x03);
		readRegister(0x04);	
		
		writeRegister(0x07,0x05);
	}

	void si443x_setup()
	{
		DIAGMSG_STR("setup","");

		writeRegister(0x08, 0x10);	// multi receive
		writeRegister(0x30, 0xA9);	// packet handling, crc disable
		writeRegister(0x32, 0x00);	// no header bytes
		writeRegister(0x33, 0x08);	// fix length, no header, 1 sync
//		writeRegister(0x34, 0xFF);	// max preamble length
//		writeRegister(0x35, 0x1A);	// minimum preamble match
		writeRegister(0x34, 0x08);	// preamble length 4 bytes
		writeRegister(0x35, 0x2A);	// preamble match 2,5 bytes
//		writeRegister(0x36, 0x2D);	// default sync word
		writeRegister(0x3E, 0x08);	// length = 8

		writeRegister(0x1C, 0xAA);	// 400 khz IF bandwidth
		writeRegister(0x1D, 0x3C);	// disable AFC
		writeRegister(0x1E, 0x02);
		writeRegister(0x1F, 0x00);
		writeRegister(0x20, 0x77);
		writeRegister(0x21, 0x20);
		writeRegister(0x22, 0x57);
		writeRegister(0x23, 0x62);
		writeRegister(0x24, 0x10);
		writeRegister(0x25, 0x59);
		writeRegister(0x2A, 0xFF);
		writeRegister(0x2C, 0x28);
		writeRegister(0x2D, 0x9C);
		writeRegister(0x2E, 0x2A);
		writeRegister(0x58, 0x80);	// unknown register
		writeRegister(0x69, 0x60);	// AGC enabled
		writeRegister(0x6E, 0x41);	// 8 kbps datarate
		writeRegister(0x6F, 0x89);
		writeRegister(0x70, 0x2C);	// no manchester
		writeRegister(0x71, 0x21);	// FIFO mode, OOK
//		writeRegister(0x71, 0x22);	// FIFO mode, FSK
//		writeRegister(0x71, 0x23);	// FIFO mode, GFSK
//		writeRegister(0x72, 0x20);	// default 20 khz freq deviation
		writeRegister(0x72, 0xA0);	// 100 khz freq deviation

		writeRegister(0x6D, 0x1F);	// max power, LNA switch set
//		writeRegister(0x75, 0x33);	// 868 MHz
//		writeRegister(0x75, 0x13);	// 434 MHz
		writeRegister(0x75, 0x4B);	// 355 MHz, sideband
		writeRegister(0x76, 0x7D);
		writeRegister(0x77, 0x00);
	}

	uint8_t minRssi;
	uint8_t maxRssi;

	uint32_t avgRssi;
	uint16_t avgCount;

	task void measureRssi()
	{
		uint8_t a;

		a = readRegister(0x26);
		atomic
		{
			avgRssi += a;
			avgCount += 1;

			if( a < minRssi )
				minRssi = a;
			
			if( a > maxRssi )
				maxRssi = a;
		}

		post measureRssi();
	}

	void resetRssi()
	{
		atomic
		{
			avgRssi = 0;
			avgCount = 0;
			minRssi = 0xFF;
			maxRssi = 0x00;
		}
	}

	void initRadio()
	{
		si443x_reset();		// ignore interrupts during reset
		si443x_standby();
		si443x_setup();		// setup the comms
		status = 1;
//		si443x_status();	// you can put status calls anywhere
		si443x_ready();
		
		if ( ! IS_TX ) {
			resetRssi();
			post measureRssi();
	    		si443x_receive();
    		}
    	
	    	call RadioAlarm.wait(30000);
	}

//	uint8_t reg75 = 0;
//	uint8_t count = 0;

	tasklet_async event void RadioAlarm.fired()
	{   
		call Leds.led0Toggle();
/*
		if( ++count >= 19 )
		{
			call Leds.led1Toggle();

			count = 0;
			writeRegister(0x75, ++reg75);

			if( IS_TX )
			{
				si443x_standby();
				si443x_ready();
			}
			else
			{
				si443x_ready();
				si443x_receive();
			}

			return;
		}
*/		
		if( IS_TX )
		{
//			if( (count & 0x04) == 0 )
//				return;

			call Leds.led2On();
			si443x_transmit();
		}
		else
		{
			if( call DiagMsg.record() ) {
		            call DiagMsg.str("rssi");
//			    call DiagMsg.hex8(reg75);
		            call DiagMsg.uint8(minRssi);
		            call DiagMsg.uint8(maxRssi);
			    call DiagMsg.uint8(avgRssi / avgCount);
		            call DiagMsg.send();
			}
		}

		resetRssi();
	    	call RadioAlarm.wait(30000);
	}
    
	tasklet_async event void Tasklet.run()
	{
//		DIAGMSG_STR("tasklet","");
	}
    
	async event void IRQ.captured(uint16_t time)
	{
		uint8_t i1,i2;

//		DIAGMSG_STR("irq","");
		
		if ( status == 0 )
		    return;
		
		i1 = readRegister(0x03);
		i2 = readRegister(0x04);
		
//		readRegister(0x05);
//		readRegister(0x06);
//		readRegister(0x62);

		if ( (i1 & 0x80) != 0 )
			DIAGMSG_STR("Int","Fifo Error");

		if ( (i1 & 0x40) != 0 )
			DIAGMSG_STR("Int","TxFifo Full");

		if ( (i1 & 0x20) != 0 )
			DIAGMSG_STR("Int","TxFifo Empty");

		if ( (i1 & 0x10) != 0 )
			DIAGMSG_STR("Int","RxFifo Full");

		if ( (i1 & 0x04) != 0 )
		{
			call Leds.led2Off();
			DIAGMSG_STR("Int","Pkt Sent");
		}

		if ( (i1 & 0x02) != 0 )
		{
			DIAGMSG_STR("Int","Pkt Received");
			readFifo();
		}

		if ( (i1 & 0x01) != 0 )
			DIAGMSG_STR("Int","CRC Error");

		if ( (i2 & 0x80) != 0 )
		{
			DIAGMSG_STR("Int","Sync Detected");
			call Leds.led3Toggle();
		}

		if ( (i2 & 0x40) != 0 )
			DIAGMSG_STR("Int","Valid Preamble");

//		if ( (i2 & 0x02) != 0 )
//			DIAGMSG_STR("Int","Chip Ready");

		if ( (i2 & 0x01) != 0 )
			DIAGMSG_STR("Int","Power On Reset");
	}

    command error_t PlatformInit.init()
    {
        call NSEL.makeOutput();
        call NSEL.set();
        call IRQ.disable();

        return SUCCESS;
    }

    command error_t SoftwareInit.init()
    {
        rxMsg = &rxMsgBuffer;
        status = 0;
        return SUCCESS;                
    }
    
    event void Boot.booted() {
        DIAGMSG_STR("booted","!");
        call SpiResource.request();
    }

    event void SpiResource.granted()
    {
        if (status == 0 )
            initRadio();
    }

    bool isSpiAcquired()
    {
        if( call SpiResource.isOwner() || SUCCESS == call SpiResource.immediateRequest() ) {
            return TRUE;
        }
        else {
            call SpiResource.request();
            return FALSE;
        }
    }
    
    task void releaseSpi()
    {
        call SpiResource.release();
    }
    

/*----------------- TURN ON/OFF -----------------*/

 
    tasklet_async command error_t RadioState.turnOff()
    {
  /*      if( sm.cmd != CMD_NONE || sm.state & TRANS_FLAG )
            return EBUSY;            
        else if( sm.state == STATE_STANDBY )
            return EALREADY;
        
        sm.next = STATE_STANDBY;
        sm.cmd = CMD_EMIT;
    */    
        call Tasklet.schedule();
        return SUCCESS;
    }
    
    tasklet_async command error_t RadioState.standby()
    {
    
    /*    if( sm.cmd != CMD_NONE || sm.state & TRANS_FLAG || ! call RadioAlarm.isFree() )
            return EBUSY;
        else if( sm.state == STATE_READY )
            return EALREADY;
            
        sm.next = STATE_READY;
        sm.cmd = CMD_EMIT;
*/
        call Tasklet.schedule();
        return SUCCESS;
    }

    tasklet_async command error_t RadioState.turnOn()
    {
/*        if( sm.cmd != CMD_NONE || sm.state & TRANS_FLAG || ! call RadioAlarm.isFree() )
            return EBUSY;
        else if( sm.state == STATE_RX )
            return EALREADY;

        sm.next = STATE_RX;
        sm.cmd = CMD_EMIT;
  
*/
        call Tasklet.schedule();
        return SUCCESS;
    }

    //default tasklet_async event void RadioState.done() { }

    si443x_header_t* getHeader(message_t* msg)
    {
        return ((void*)msg) + call Config.headerLength(msg);
    }

    void* getPayload(message_t* msg)
    {
        return ((void*)msg) + call RadioPacket.headerLength(msg);
    }

    si443x_metadata_t* getMeta(message_t* msg)
    {
        return ((void*)msg) + sizeof(message_t) - call RadioPacket.metadataLength(msg);
    }

/*----------------- TRANSMIT -----------------*/

    tasklet_async command error_t RadioSend.send(message_t* msg)
    {
        return SUCCESS;
    }

    default tasklet_async event void RadioSend.sendDone(error_t error) { }
    default tasklet_async event void RadioSend.ready() { }

/*----------------- CCA -----------------*/

    tasklet_async command error_t RadioCCA.request()
    {
        return SUCCESS;
    }

    default tasklet_async event void RadioCCA.done(error_t error) { }

/*----------------- RECEIVE -----------------*/

/*----------------- IRQ -----------------*/


    default tasklet_async event bool RadioReceive.header(message_t* msg)
    {
        return TRUE;
    }

    default tasklet_async event message_t* RadioReceive.receive(message_t* msg)
    {
        return msg;
    }


/*----------------- CHANNEL -----------------*/

    tasklet_async command uint8_t RadioState.getChannel()
    {
        return channel;
    }

    tasklet_async command error_t RadioState.setChannel(uint8_t c)
    {
        return SUCCESS;
    }


/*----------------- RadioPacket -----------------*/
    
    async command uint8_t RadioPacket.headerLength(message_t* msg)
    {
        return call Config.headerLength(msg) + sizeof(si443x_header_t);
    }

    async command uint8_t RadioPacket.payloadLength(message_t* msg)
    {
        return getHeader(msg)->length - 2;
    }

    async command void RadioPacket.setPayloadLength(message_t* msg, uint8_t length)
    {
        RADIO_ASSERT( 1 <= length && length <= 125 );
        RADIO_ASSERT( call RadioPacket.headerLength(msg) + length + call RadioPacket.metadataLength(msg) <= sizeof(message_t) );

        // we add the length of the CRC, which is automatically generated
        getHeader(msg)->length = length + 2;
    }

    async command uint8_t RadioPacket.maxPayloadLength()
    {
        RADIO_ASSERT( call Config.maxPayloadLength() - sizeof(si443x_header_t) <= 125 );

        return call Config.maxPayloadLength() - sizeof(si443x_header_t);
    }

    async command uint8_t RadioPacket.metadataLength(message_t* msg)
    {
        return call Config.metadataLength(msg) + sizeof(si443x_metadata_t);
    }

    async command void RadioPacket.clear(message_t* msg)
    {
        // all flags are automatically cleared
    }

/*----------------- PacketTransmitPower -----------------*/

    async command bool PacketTransmitPower.isSet(message_t* msg)
    {
        return call TransmitPowerFlag.get(msg);
    }

    async command uint8_t PacketTransmitPower.get(message_t* msg)
    {
        return getMeta(msg)->power;
    }

    async command void PacketTransmitPower.clear(message_t* msg)
    {
        call TransmitPowerFlag.clear(msg);
    }

    async command void PacketTransmitPower.set(message_t* msg, uint8_t value)
    {
        call TransmitPowerFlag.set(msg);
        getMeta(msg)->power = value;
    }

/*----------------- PacketRSSI -----------------*/

    async command bool PacketRSSI.isSet(message_t* msg)
    {
        return call RSSIFlag.get(msg);
    }

    async command uint8_t PacketRSSI.get(message_t* msg)
    {
        return getMeta(msg)->rssi;
    }

    async command void PacketRSSI.clear(message_t* msg)
    {
        call RSSIFlag.clear(msg);
    }

    async command void PacketRSSI.set(message_t* msg, uint8_t value)
    {
        // just to be safe if the user fails to clear the packet
        call TransmitPowerFlag.clear(msg);

        call RSSIFlag.set(msg);
        getMeta(msg)->rssi = value;
    }

/*----------------- PacketTimeSyncOffset -----------------*/

    async command bool PacketTimeSyncOffset.isSet(message_t* msg)
    {
        return call TimeSyncFlag.get(msg);
    }

    async command uint8_t PacketTimeSyncOffset.get(message_t* msg)
    {
        return call RadioPacket.headerLength(msg) + call RadioPacket.payloadLength(msg) - sizeof(timesync_absolute_t);
    }

    async command void PacketTimeSyncOffset.clear(message_t* msg)
    {
        call TimeSyncFlag.clear(msg);
    }

    async command void PacketTimeSyncOffset.set(message_t* msg, uint8_t value)
    {
        // we do not store the value, the time sync field is always the last 4 bytes
        RADIO_ASSERT( call PacketTimeSyncOffset.get(msg) == value );

        call TimeSyncFlag.set(msg);
    }

/*----------------- PacketLinkQuality -----------------*/

    async command bool PacketLinkQuality.isSet(message_t* msg)
    {
        return TRUE;
    }

    async command uint8_t PacketLinkQuality.get(message_t* msg)
    {
        return getMeta(msg)->lqi;
    }

    async command void PacketLinkQuality.clear(message_t* msg)
    {
    }

    async command void PacketLinkQuality.set(message_t* msg, uint8_t value)
    {
        getMeta(msg)->lqi = value;
    }
}
