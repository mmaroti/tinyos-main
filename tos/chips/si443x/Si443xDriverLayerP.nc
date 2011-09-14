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
        
        interface GpioInterrupt as IRQ;
            
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
    
    
    
    uint8_t sbuff[200];
    uint8_t* sbuffptr = NULL;

    uint8_t buff[200];
    uint8_t pktlen = 1;
    uint8_t* buffptr = NULL;

	void clearFifoPacket() {
	    uint8_t i ;
	    for (i = 0; i < 200; ++i) {
	        buff[i] = 0xff;
	        sbuff[i] = 0x00;
	    }
	    buffptr = NULL;
	    sbuffptr = sbuff;
	    pktlen = 1;
	    writeRegister(0x08,0x13);
	    writeRegister(0x08,0x10);
	    call BusyWait.wait(50);
	}

	inline void readFifo()
	{
		uint8_t i = 1;
	   	RADIO_ASSERT( call SpiResource.isOwner() );
	   	DIAGMSG_STR("readFifo","");

     	if( call DiagMsg.record() ) {
     	    call DiagMsg.str("buffer");
		    call DiagMsg.hex16((uint16_t)buff);
		    call DiagMsg.hex16((uint16_t)buffptr);
		    call DiagMsg.send();
		}
		atomic
		{
			call NSEL.clr();
			call FastSpiByte.splitWrite(SI443X_CMD_REGISTER_READ | 0x7F);
            call FastSpiByte.splitReadWrite(0);
            
            if ( buffptr == NULL ) {
                buffptr = buff;
                pktlen = call FastSpiByte.splitReadWrite(0);
                ++i;
            }          
            
			for (; i < 55 && buffptr < buff+pktlen-1; ++i) { // Fifo full thresh : 55
				*(buffptr++) = call FastSpiByte.splitReadWrite(0);
		    }
		    *(buffptr++) = call FastSpiByte.splitRead();
			call NSEL.set();
        }
       	if( call DiagMsg.record() ) {
	        call DiagMsg.hex16((uint16_t)buffptr);
	        call DiagMsg.send();
		}
	   	        
	}
	
	void printPacket() {
	    uint8_t i;
        // print the whole packet
    	if( call DiagMsg.record() ) {
            call DiagMsg.str("packet");
            call DiagMsg.hex8(pktlen);
            call DiagMsg.send();
    	}
        for ( i = 0; i < 13; ++i  ) {
			if( call DiagMsg.record() ) {
	            call DiagMsg.hex8s(buff+i*15,15);
	            call DiagMsg.send();
			}
		}
/*		if( call DiagMsg.record() ) {
		    call DiagMsg.hex8s(buff+i*15, pktlen%15);
		    call DiagMsg.send();
		}*/
	}

    uint8_t status;
    
/*----------------- INIT -----------------*/
       
	void si443x_reset()
	{
		DIAGMSG_STR("reset","");
		readRegister(0x03);
		readRegister(0x04);
		call IRQ.enableFallingEdge();	// previous interrupts mess up the PCINT handler
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

    #define _PKT_LEN_ 180
    
	void si443x_transmit()
	{
    	uint8_t lstatus;
    	DIAGMSG_STR("transmit","");
		atomic {
		    lstatus = status;
		    status = 2;
		}
		if ( lstatus == 2 ) {
		    DIAGMSG_STR("tx","ccur");
		    return;
		}
		writeRegister(0x3E, _PKT_LEN_ );  // pkt length
		writeRegister(0x05, 0xFF);  // pkt sent interrupt + TX fifo almost empty
		writeRegister(0x06, 0xFF);
		readRegister(0x03);         // flush interrupts
		readRegister(0x04);
		writeRegister(0x07, 0x09);  // tx + ready
	}

    

    uint8_t starter = 0;
    uint8_t fifodata = 0;    
    void makePacket() {
        uint8_t i;
                
        starter = (starter+1) % 16;
        for ( i = 0; i< 200; ++i)
            sbuff[i] = starter+i;

        sbuffptr = sbuff;
        fifodata = 0;
         
       	if( call DiagMsg.record() ) {
       	    call DiagMsg.str("makepacket");
	        call DiagMsg.hex8(starter);       	    
	        call DiagMsg.hex16((uint16_t)sbuff);       	    
	        call DiagMsg.hex16((uint16_t)sbuffptr);
	        call DiagMsg.send();
		}
    }

    
    void fillFifo() {

        uint8_t i;
        RADIO_ASSERT( call SpiResource.isOwner() );
        
        if ( sbuffptr == sbuff + _PKT_LEN_ ) {
            if( call DiagMsg.record() ) {
    	        call DiagMsg.str("fifoend");
    	        call DiagMsg.send();
    		}
            return;
        }
        
         if( call DiagMsg.record() ) {
    	        call DiagMsg.str("fillfifo");
    	        call DiagMsg.hex16((uint16_t)sbuff);
    	        call DiagMsg.hex16((uint16_t)sbuffptr);    	        
    	        call DiagMsg.hex8(_PKT_LEN_);
    	        call DiagMsg.hex8(fifodata);    	               
    	        call DiagMsg.send();
    		}
        
        atomic
		{
			call NSEL.clr();
			call FastSpiByte.splitWrite(SI443X_CMD_REGISTER_WRITE | 0x7F);
            
			for (i = 0; i < 60 && sbuffptr < sbuff+_PKT_LEN_; ++i) { // Fifo full thresh : 55, empty : 4
			    ++fifodata;
				call FastSpiByte.splitReadWrite(*(sbuffptr++));
		    }
		    call FastSpiByte.splitRead();
			call NSEL.set();
        }
        
       	if( call DiagMsg.record() ) {
	        call DiagMsg.hex16((uint16_t)sbuffptr);
	        call DiagMsg.hex8(fifodata);
	        call DiagMsg.send();
		}
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
		writeRegister(0x6D, 0x1F);	// max power, LNA switch set

		writeRegister( 0x1C, 0x9A );
		writeRegister( 0x1D, 0x3C );
		writeRegister( 0x1E, 0x02 );
		writeRegister( 0x1F, 0x00 );
		writeRegister( 0x20, 0x77 );
		writeRegister( 0x21, 0x20 );
		writeRegister( 0x22, 0x2B );
		writeRegister( 0x23, 0xB1 );
		writeRegister( 0x24, 0x10 );
		writeRegister( 0x25, 0x59 );
		writeRegister( 0x2A, 0xFF );
		writeRegister( 0x2C, 0x18 );
		writeRegister( 0x2D, 0x4E );
		writeRegister( 0x2E, 0x2A );
		writeRegister( 0x30, 0x8D );
		writeRegister( 0x32, 0x00 );
		writeRegister( 0x33, 0x00 );
		writeRegister( 0x34, 0x08 );
		writeRegister( 0x35, 0x2A );
//		writeRegister( 0x3E, 0xFF );
		writeRegister( 0x58, 0x80 );
		writeRegister( 0x69, 0x60 );
		writeRegister( 0x6E, 0x41 );
		writeRegister( 0x6F, 0x89 );
		writeRegister( 0x70, 0x2F );
		writeRegister( 0x71, 0x21 );
		writeRegister( 0x72, 0xA0 );
		writeRegister( 0x75, 0x4B );
		writeRegister( 0x76, 0x7D );
		writeRegister( 0x77, 0x00 );

	}

	void initRadio()
	{
		si443x_reset();		// ignore interrupts during reset
		si443x_standby();
		si443x_setup();		// setup the comms
		status = 1;
		si443x_ready();
		
		if ( ! IS_TX ) {
		    call Leds.led0On();
    		si443x_receive();

   		} else {
   		    call Leds.led0Off();    	
   		    call RadioAlarm.wait(30000);
   		}
	}


	tasklet_async event void RadioAlarm.fired()
	{   
   		call Leds.led1Toggle();
	    if( IS_TX && status != 2 )
		{
		    makePacket();
		    fillFifo();
		    si443x_transmit();
		}
       	call RadioAlarm.wait(30000);
	}
    


	async event void IRQ.fired()
	{
		uint8_t i1,i2;
		
		if ( status == 0 )
		    return;
		
		i1 = readRegister(0x03);
		i2 = readRegister(0x04);

		if ( (i1 & 0x80) != 0 )
			DIAGMSG_STR("Int","Fifo Error");

		if ( (i1 & 0x40) != 0 ) {
			DIAGMSG_STR("Int","TxFifo Full");
			si443x_transmit();
		}

		if ( (i1 & 0x20) != 0 ) {
			DIAGMSG_STR("Int","TxFifo Empty");
			fillFifo();
        }
        
		if ( (i1 & 0x10) != 0 ) {
			DIAGMSG_STR("Int","RxFifo Full");
			readFifo();
			printPacket();
        }
		if ( (i1 & 0x04) != 0 )
		{
			DIAGMSG_STR("Int","Pkt Sent");
			status = 1;
		}

		if ( (i1 & 0x02) != 0 )
		{
			DIAGMSG_STR("Int","Pkt Received");
			readFifo();
			printPacket();
			clearFifoPacket();
		}

		if ( (i1 & 0x01) != 0 ) {
			DIAGMSG_STR("Int","CRC Error");
			clearFifoPacket();
	    }

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


	tasklet_async event void Tasklet.run()
	{
//		DIAGMSG_STR("tasklet","");
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
