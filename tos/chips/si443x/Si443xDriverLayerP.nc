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
        
        interface Leds;
        interface Boot;
        
#ifdef RADIO_DEBUG
        interface DiagMsg;
#endif
    }
}

implementation
{

/* ----------------- DEBUGGER FUNCTIONS AND HELPERS  -----------------*/
#ifdef RADIO_DEBUG

uint8_t DM_ENABLE = FALSE;
    
#define DIAGMSG_STR(PSTR,STR) \
        atomic { if( DM_ENABLE && call DiagMsg.record() ) { \
            call DiagMsg.str(PSTR);\
            call DiagMsg.str(STR); \
            call DiagMsg.send(); \
        }        }

#define DIAGMSG_REG_READ(REG,VALUE)       \
        atomic { if( DM_ENABLE && call DiagMsg.record() ) { \
            call DiagMsg.str("R");\
            call DiagMsg.hex8(REG);\
            call DiagMsg.hex8(VALUE);\
            call DiagMsg.send(); \
        }}

#define DIAGMSG_REG_WRITE(REG,VALUE)       \
        atomic {if( DM_ENABLE && call DiagMsg.record() ) { \
            call DiagMsg.str("W");\
            call DiagMsg.hex8(REG);\
            call DiagMsg.hex8(VALUE);\
            call DiagMsg.send(); \
        }}

#define DIAGMSG_CHIP()       \
        atomic { if( DM_ENABLE && call DiagMsg.record() ) { \
            call DiagMsg.str("C");\
            call DiagMsg.hex8(chip.state);\
            call DiagMsg.hex8(chip.cmd);\
            call DiagMsg.send(); \
        }}        

#define DIAGMSG_VAR(PSTR,VAR) \
        atomic { if( DM_ENABLE && call DiagMsg.record() ) { \
            call DiagMsg.str(PSTR);\
            call DiagMsg.hex8(VAR); \
            call DiagMsg.send(); \
        }        }



#endif

/*----------------- STATE -----------------*/

	enum
	{
		STATE_POR = 0,
		STATE_SLEEP = 1,
		STATE_READY = 2,
		STATE_RX = 3,
		STATE_TX = 4,

		STATE_IGNORE_INT = 10,    
    
		CMD_NONE = 0,			// the state machine has stopped
		CMD_TURNOFF = 1,		// goto SLEEP state
		CMD_STANDBY = 2,		// goto READY state
		CMD_TURNON = 3,			// goto RX state
        // CMD_TURNOFF, CMD_STANDBY, CMD_TURNON values MUST match STATE_SLEEP, STATE_READY, STATE_RX values in that order!
        
		CMD_CHANNEL = 5,        // change channel
		
		CMD_START_DOWNLOAD = 9,
		CMD_FINISH_RX = 10,	    // finish receiving
		CMD_FINISH_TX = 11,		// finish transmitting
		CMD_FINISH_CCA = 12,	// finish clear chanel assesment
	};

    tasklet_norace struct {
        uint8_t state;
        uint8_t cmd;
    } chip;

	norace bool radioIrq;

	tasklet_norace uint8_t txPower;
	tasklet_norace uint8_t channel;

	tasklet_norace message_t* rxMsg;
	message_t rxMsgBuffer;

	tasklet_norace uint8_t rssiClear;
	tasklet_norace uint8_t rssiBusy;


/*----------------- MESSAGE HANDLING -----------------*/    
    

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


	void printPacket() {
	    uint8_t i,l;
    	l = getHeader(rxMsg)->length;
    		    
        // print the whole packet
    	if( call DiagMsg.record() ) {
            call DiagMsg.str("packet");
            call DiagMsg.hex8(l);
            call DiagMsg.send();
    	}

        for ( i = 0; i < l/15; ++i  ) {
			if( call DiagMsg.record() ) {
	            call DiagMsg.hex8s(getPayload(rxMsg)+i*15,15);
	            call DiagMsg.send();
			}
		}
		if( l%15 != 0 && call DiagMsg.record() ) {
            call DiagMsg.hex8s(getPayload(rxMsg)+i*15,l%15);
            call DiagMsg.send();
		}
	}

/*----------------- REGISTER -----------------*/

    inline void writeRegister(uint8_t reg, uint8_t value)
    {
        RADIO_ASSERT( call SpiResource.isOwner() );
        RADIO_ASSERT( reg == (reg & SI443X_SPI_REGMASK) );
    
        call NSEL.clr();
        call FastSpiByte.splitWrite(SI443X_SPI_WRITE | reg);
        call FastSpiByte.splitReadWrite(value);
        call FastSpiByte.splitRead();
        call NSEL.set();
    }
    
   
    inline uint8_t readRegister(uint8_t reg)
    {
        RADIO_ASSERT( call SpiResource.isOwner() );
        RADIO_ASSERT( reg == (reg & SI443X_SPI_REGMASK) );
        
        call NSEL.clr();
        call FastSpiByte.splitWrite(SI443X_SPI_READ | reg);
        call FastSpiByte.splitReadWrite(0);
        reg = call FastSpiByte.splitRead();
        call NSEL.set();
	    return reg;
    }
    
    enum {
        POR_TIME = (uint16_t)30000,
        CCA_REQUEST_TIME = (uint16_t)(140 * RADIO_ALARM_MICROSEC),
        TX_SFD_DELAY = (uint16_t)(176 * RADIO_ALARM_MICROSEC),
    };
    
/*----------------- LOW LEVEL FUNCTIONS -----------------*/
  	
	uint8_t* msgdata;
	uint8_t queued;
    uint8_t hdrlen;
	
    inline void _clearFifo() {
        uint8_t old = readRegister(SI443X_CTRL_2);
        writeRegister(SI443X_CTRL_2, old |  SI443X_CLEAR_RX_FIFO | SI443X_CLEAR_TX_FIFO );
        writeRegister(SI443X_CTRL_2, old & (~(SI443X_CLEAR_RX_FIFO | SI443X_CLEAR_TX_FIFO)) );
        queued = 0;
        hdrlen = -1;
        msgdata = NULL;
    }
        
    inline void _setPower(uint8_t power) {
        writeRegister(SI443X_TXPOWER, SI443X_LNA | (power & SI443X_RFPOWER_MASK));
    }
    
    inline void _setPktLength(uint8_t length) {
        writeRegister(SI443X_PKTLEN, length);
    }
    
    inline void _changeChannel() {
	    writeRegister(SI443X_CHANNEL_SELECT,channel);
	}
    
    inline void _reset() {
        DIAGMSG_STR("reset","");
		readRegister(SI443X_INT_1);
		readRegister(SI443X_INT_2);

    	// previous interrupts mess up the PCINT handler
		call IRQ.enableFallingEdge();
		writeRegister(SI443X_CTRL_1, SI443X_CTRL1_SWRESET | SI443X_CTRL1_READY );
		call BusyWait.wait(POR_TIME);

		// we might get interrupts here, MUST ignore them
		readRegister(SI443X_INT_1);
		readRegister(SI443X_INT_2);
    }
    
    inline void _standby() {
        DIAGMSG_STR("standby","");
        
       	// tricky reset of interrupts
       	// we might get interrupts here, MUST ignore them
		writeRegister(SI443X_IEN_1,SI443X_I_NONE);
		writeRegister(SI443X_IEN_2,SI443X_I_NONE);
		writeRegister(SI443X_IEN_1,SI443X_I_ALL);
		writeRegister(SI443X_IEN_2,SI443X_I_ALL);
		writeRegister(SI443X_IEN_1,SI443X_I_NONE);
		writeRegister(SI443X_IEN_2,SI443X_I_NONE);
		
    	// we instantly enter standby, NO interrupt will come
		writeRegister(SI443X_CTRL_1,SI443X_CTRL1_STANDBY);
    }
    
    inline void _ready() {
        DIAGMSG_STR("ready","");
		writeRegister(SI443X_IEN_1, SI443X_I_ALL);
		writeRegister(SI443X_IEN_2, SI443X_I_ALL);
        // we instantly enter ready, NO interrupt will come
		writeRegister(SI443X_CTRL_1, SI443X_CTRL1_READY);
    }
    
    inline void _transmit()
	{
    	DIAGMSG_STR("transmit","");
    		
	    RADIO_ASSERT( chip.state != STATE_TX );
	    RADIO_ASSERT( chip.cmd == CMD_FINISH_TX );

        writeRegister(SI443X_IEN_1, SI443X_I_ALL);
        writeRegister(SI443X_IEN_2, SI443X_I_ALL);
		//writeRegister(SI443X_IEN_1, SI443X_I1_FIFOERROR | SI443X_I1_TXFIFOFULL | SI443X_I1_TXFIFOEMPTY | SI443X_I1_PKTSENT);
		//writeRegister(SI443X_IEN_2, SI443X_I_NONE);
		readRegister(SI443X_INT_1);
		readRegister(SI443X_INT_2);
		writeRegister(SI443X_CTRL_1, SI443X_CTRL1_TRANSMIT | SI443X_CTRL1_READY);
	}
	
	inline void _receive()
	{
		DIAGMSG_STR("receive","");
        RADIO_ASSERT( chip.state != STATE_RX );
        
        writeRegister(SI443X_IEN_1, SI443X_I_ALL);
		//writeRegister(SI443X_IEN_1, SI443X_I1_FIFOERROR | SI443X_I1_RXFIFOFULL | SI443X_I1_PKTRECEIVED | SI443X_I1_CRCERROR); 
		writeRegister(SI443X_IEN_2, SI443X_I2_SYNCDETECT );
		readRegister(SI443X_INT_1);
		readRegister(SI443X_INT_2);	
    	writeRegister(SI443X_CTRL_1, SI443X_CTRL1_RECEIVE | SI443X_CTRL1_READY );
	}
    
    void _fillTxFifo() {
        uint8_t space;
        DIAGMSG_STR("fill","txfifo");
        RADIO_ASSERT( call SpiResource.isOwner() );
        
        space = SI443X_FIFO_SIZE - readRegister(SI443X_TXFIFO_EMPTY);
		call NSEL.clr();
		call FastSpiByte.splitWrite(SI443X_SPI_WRITE | SI443X_FIFO);
            
        while ( space && queued ) {
            call FastSpiByte.splitReadWrite(*(msgdata++));
            --space;
            --queued;
            // WARNING: cannot decrement these values in the condition!
            // Doing that would cause inappropriate values in between _fillTxFifo() calls!
        }
        call FastSpiByte.splitRead();
		call NSEL.set();
    }
    
    inline void _readRxFifo()
	{
	    uint8_t fifoload;
	   	RADIO_ASSERT( call SpiResource.isOwner() );
	   	RADIO_ASSERT( chip.cmd == CMD_START_DOWNLOAD || chip.cmd == CMD_FINISH_RX );

        fifoload = readRegister(SI443X_RXFIFO_FULL);
        if ( chip.cmd == CMD_START_DOWNLOAD ) { // if first call
            // read packet length
            queued = readRegister(SI443X_FIFO);
            --fifoload;
            
            call RadioPacket.setPayloadLength(rxMsg, queued);
            msgdata = getPayload(rxMsg);
                        
            hdrlen = call Config.headerPreloadLength()-1;
			if( queued < hdrlen )
				hdrlen = queued;
                
            --hdrlen; // 1B already read (pktlen)
        } else {
          hdrlen = -1; // just to be cautious
        }
        
        // CONDITION:
        // if chip.cmd == CMD_START_DOWNLOAD: fifoload: RXFIFO Full Thresh - 1, queued: pktlength
        // if chip.cmd == CMD_FINISH_RX     : fifoload: RXFIFO Full Thresh, queued: remaining
        
        if ( queued < fifoload )
            fifoload = queued;
        queued -= fifoload;
        
        call NSEL.clr();
		call FastSpiByte.splitWrite(SI443X_SPI_READ | SI443X_FIFO);
        call FastSpiByte.splitReadWrite(0);
        
        while( --fifoload ) {
            *(msgdata++) = call FastSpiByte.splitReadWrite(0);
            if ( --hdrlen == 0 )
                signal RadioReceive.header(rxMsg);
        }
        *(msgdata++) = call FastSpiByte.splitRead();
   		call NSEL.set();
   		
        if ( --hdrlen == 0 )
            signal RadioReceive.header(rxMsg);
	}

    void _setupModem()
	{
		DIAGMSG_STR("setup","");

		writeRegister(0x08, 0x10);	    // multi receive
		writeRegister(0x6D, 0x1F);	    // max power, LNA switch set

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
		
		writeRegister( 0x58, 0x80 );
		writeRegister( 0x69, 0x60 );
		writeRegister( 0x6E, 0x41 );
		writeRegister( 0x6F, 0x89 );
		writeRegister( 0x70, 0x2F );
		writeRegister( 0x71, 0x21 );
		writeRegister( 0x72, 0xA0 );
		
		writeRegister( 0x75, 0x4B );    // carrier freq
		writeRegister( 0x76, 0x7D );
		writeRegister( 0x77, 0x00 );
	}
	
	


/*----------------- SPI -----------------*/
    
    event void SpiResource.granted()
    {
       	if( chip.state == STATE_POR )
		{
			chip.state = STATE_IGNORE_INT; 
			_reset();
			_standby();
			_setupModem();
			chip.state = STATE_SLEEP;
			call SpiResource.release();
		}
		else {
			call Tasklet.schedule();
		}
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
      
/*----------------- TASKLET HANDLER -----------------*/

	async event void IRQ.fired()
	{
    	RADIO_ASSERT( ! radioIrq );
		radioIrq = TRUE;
		call Tasklet.schedule();
	}
	
	void serviceRadio()
	{
		if( isSpiAcquired() )
		{
			uint8_t irq1, irq2;
			
			radioIrq = FALSE;
		
		    irq1 = readRegister(SI443X_INT_1) & readRegister(SI443X_IEN_1);
		    irq2 = readRegister(SI443X_INT_2) & readRegister(SI443X_IEN_2);

		    if ( irq1 & SI443X_I1_FIFOERROR ) {
			    DIAGMSG_STR("Int","Fifo Error");		        
		        RADIO_ASSERT(
		            (chip.state == STATE_RX && chip.cmd == CMD_FINISH_RX) ||
		            (chip.state == STATE_TX && chip.cmd == CMD_FINISH_TX)
		        );
            }
		    if ( irq1 & SI443X_I1_TXFIFOFULL ) {
		        RADIO_ASSERT( chip.cmd == CMD_FINISH_TX );
			    DIAGMSG_STR("Int","TxFifo Full");
			    chip.cmd = CMD_FINISH_TX;
		    }

		    if ( irq1 & SI443X_I1_TXFIFOEMPTY ) {
		        RADIO_ASSERT( chip.state == STATE_TX && chip.cmd == CMD_FINISH_TX );
			    DIAGMSG_STR("Int","TxFifo Empty");
			    if ( queued > 0 )
			        _fillTxFifo();
            }
        
		    if ( irq1 & SI443X_I1_RXFIFOFULL ) {
		        RADIO_ASSERT( chip.state == STATE_RX );
		        RADIO_ASSERT( chip.cmd == CMD_START_DOWNLOAD || chip.cmd == CMD_FINISH_RX );
			    DIAGMSG_STR("Int","RxFifo Full");
			    _readRxFifo();
			    chip.cmd = CMD_FINISH_RX;
            }
		    if ( irq1 & SI443X_I1_PKTSENT )
		    {
		        RADIO_ASSERT( chip.state == STATE_TX && chip.cmd == CMD_FINISH_TX );
			    DIAGMSG_STR("Int","Pkt Sent");
                chip.cmd = CMD_NONE;
                chip.state = STATE_READY;
                signal RadioSend.sendDone(SUCCESS);
		    }

		    if ( irq1 & SI443X_I1_PKTRECEIVED )
		    {
		        RADIO_ASSERT( chip.state == STATE_RX );
  		        RADIO_ASSERT( chip.cmd == CMD_START_DOWNLOAD || chip.cmd == CMD_FINISH_RX );
			    DIAGMSG_STR("Int","Pkt Received");
			    _readRxFifo();
			    chip.cmd = CMD_NONE;
			    rxMsg = signal RadioReceive.receive(rxMsg);
			    printPacket();
		    }

		    if ( irq1 & SI443X_I1_CRCERROR ) {
   		        RADIO_ASSERT( chip.cmd == CMD_FINISH_RX || chip.cmd == CMD_CHANNEL );
		        RADIO_ASSERT( chip.state == STATE_RX );
			    DIAGMSG_STR("Int","CRC Error");
			    _clearFifo();			    
	        }

		    if ( irq2 & SI443X_I2_SYNCDETECT ) {
		        RADIO_ASSERT( chip.state == STATE_RX );
			    DIAGMSG_STR("Int","Sync Detected");
			    chip.cmd = CMD_START_DOWNLOAD;
		    }

		    if ( irq2 & SI443X_I2_PREAVALID ) {
   		        RADIO_ASSERT( chip.state == STATE_RX && chip.cmd == CMD_FINISH_RX );
			    DIAGMSG_STR("Int","Valid Preamble");
            }
/*    		if ( irq2 & SI443X_I2_CHIPREADY ) {
    			DIAGMSG_STR("Int","Chip Ready");
    			DIAGMSG_CHIP();
    			DIAGMSG_VAR("0x62",readRegister(0x62));
    		}
*/
		    if ( irq2 & SI443X_I2_POR )
			    DIAGMSG_STR("Int","Power On Reset");
	    }
	}
	
	tasklet_async event void Tasklet.run()
	{
		if( radioIrq && chip.state != STATE_IGNORE_INT )
			serviceRadio();

		if( chip.cmd != CMD_NONE )
		{
		    if ( chip.cmd <= CMD_CHANNEL && isSpiAcquired() ) {
    		    switch ( chip.cmd ) {
        		    case CMD_CHANNEL:
        		        // TODO: experiment if we REALLY MUST enter TX or RX for channel change.
	    	            if ( chip.state != STATE_RX && chip.state != STATE_TX ) {
	    	                _receive();
	    	                _changeChannel();
	    	                chip.cmd = chip.state;  // all state changing commands equals to the destination state!
	    	                // do not break here, fallthrought to the state changing label
                        } else {
                            _changeChannel();
                            break;
                        }
	    	        case CMD_TURNOFF:    
	    	            chip.state = STATE_IGNORE_INT; 
	    	            _standby(); chip.state = STATE_SLEEP; break;
			        case CMD_STANDBY:
			            _ready();   chip.state = STATE_READY; break;
	    	        case CMD_TURNON:     
	    	            _receive(); chip.state = STATE_RX; break;
	    	        
	    	    }
				signal RadioState.done();	    	    
	    	    chip.cmd = CMD_NONE;
		    }
		    // TODO: Question: This one needs an atomic guard or not?
  		    if ( chip.cmd == CMD_FINISH_TX && chip.state != STATE_TX ) {
                _transmit();
                chip.state = STATE_TX;
            }
		}        

		if( chip.cmd == CMD_NONE && chip.state == STATE_TX && ! radioIrq )
			signal RadioSend.ready();

		if( chip.cmd == CMD_NONE && chip.state != STATE_POR )
			post releaseSpi();
	}


/*----------------- TRANSMIT - RECEIVE -----------------*/

    tasklet_async command error_t RadioSend.send(message_t* msg)
    {
        uint16_t time;
		uint8_t power;
		uint32_t time32;
		void* timesync;

		if( chip.cmd != CMD_NONE || (chip.state != STATE_READY && chip.state != STATE_TX ) || ! isSpiAcquired() || radioIrq )
			return EBUSY;

        // get the required RF power setting
		power = (call PacketTransmitPower.isSet(msg) ? call PacketTransmitPower.get(msg) : SI443X_DEF_RFPOWER) & SI443X_RFPOWER_MASK;
		if( power != txPower )
		{
			txPower = power;
			_setPower(txPower);
		}
		
        // RSSI Clear Channel Assessment
		if( call Config.requiresRssiCca(msg) && ( readRegister(SI443X_RSSI) > ((rssiClear + rssiBusy) >> 3) ) )
			return EBUSY;


        // TODO: Fix Time Synchronization measurements
   		time32 = call LocalTime.get();
		timesync = call PacketTimeSyncOffset.isSet(msg) ? ((void*)msg) + call PacketTimeSyncOffset.get(msg) : 0;
        time = call RadioAlarm.getNow();
        time32 += (int16_t)(time + TX_SFD_DELAY) - (int16_t)(time32);

        chip.cmd = CMD_FINISH_TX;
        
        // Fill the TX FIFO
        msgdata = getPayload(msg);
		queued = getHeader(msg)->length;
		_setPktLength(queued); // MUST call before the first _fillTxFifo() call!
        _fillTxFifo();

        /* Enter TX state
         * If the packet length > TX Fifo Almost Full threshold, 
         * an interrupt have already been generated, and TX state entered.
         */
		if( timesync != 0 )
			*(timesync_relative_t*)timesync = (*(timesync_absolute_t*)timesync) - time32;

		call PacketTimeStamp.set(msg, time32);
		
		call Tasklet.schedule();
        return SUCCESS;
    }

//    default tasklet_async event void RadioSend.ready() { } 

    default tasklet_async event bool RadioReceive.header(message_t* msg)
    {
        //DIAGMSG_STR("RReceive","Hdr");
        return TRUE;
    }

    default tasklet_async event message_t* RadioReceive.receive(message_t* msg)
    {
        //DIAGMSG_STR("RReceive","receive");
        return msg;
    }

/*----------------- DRIVER CONTROL -----------------*/

    command error_t PlatformInit.init()
    {
        call NSEL.makeOutput();
        call NSEL.set();
        call IRQ.disable();
        
        return SUCCESS;
    }
    
    event void Boot.booted() {
        DM_ENABLE = TRUE;
    }

    command error_t SoftwareInit.init()
    {
   		// these are just good approximates
		rssiClear = 0;
		rssiBusy = 90;
        rxMsg = &rxMsgBuffer;

        chip.state = STATE_POR;
        chip.cmd = CMD_NONE;
        
        txPower = SI443X_DEF_RFPOWER & SI443X_RFPOWER_MASK;
		channel = SI443X_DEF_CHANNEL;
        
        return call SpiResource.request();
    }
    
    tasklet_async command error_t RadioState.turnOff()
    {
        if( chip.cmd != CMD_NONE )
            return EBUSY;            
        else if( chip.state == STATE_SLEEP )
            return EALREADY;
        
        chip.cmd = CMD_TURNOFF;
        call Tasklet.schedule();
        return SUCCESS;
    }
    
    tasklet_async command error_t RadioState.standby()
    {
        if( chip.cmd != CMD_NONE || (chip.state == STATE_SLEEP && ! call RadioAlarm.isFree()) )
			return EBUSY;
		else if( chip.state == STATE_READY )
			return EALREADY;

		chip.cmd = CMD_STANDBY;
		call Tasklet.schedule();
		return SUCCESS;
    }

    tasklet_async command error_t RadioState.turnOn()
    {
        if( chip.cmd != CMD_NONE || (chip.state == STATE_SLEEP && ! call RadioAlarm.isFree()) )
			return EBUSY;
		else if( chip.state == STATE_RX )
			return EALREADY;

		chip.cmd = CMD_TURNON;
		call Tasklet.schedule();
		return SUCCESS;
    }

    tasklet_async command uint8_t RadioState.getChannel() { return channel; }

    tasklet_async command error_t RadioState.setChannel(uint8_t c)
    {
    	if( chip.cmd != CMD_NONE )
			return EBUSY;
		else if( channel == c )
			return EALREADY;

		channel = c;
		chip.cmd = CMD_CHANNEL;
		call Tasklet.schedule();
		return SUCCESS;
    }    
    
    tasklet_async command error_t RadioCCA.request()
    {
        if( chip.cmd != CMD_NONE || chip.state != STATE_RX || ! isSpiAcquired() || ! call RadioAlarm.isFree() )
			return EBUSY;

		call RadioAlarm.wait(CCA_REQUEST_TIME);
		chip.cmd = CMD_FINISH_CCA;
        return SUCCESS;
    }

    default tasklet_async event void RadioCCA.done(error_t error) { }
    
    tasklet_async event void RadioAlarm.fired()
	{
	    RADIO_ASSERT( chip.cmd == CMD_FINISH_CCA );
	    RADIO_ASSERT( chip.state == STATE_RX );

	    chip.cmd = CMD_NONE;
		signal RadioCCA.done( SUCCESS );
		call Tasklet.schedule();
	}


/*----------------- RadioPacket -----------------*/
    
    async command uint8_t RadioPacket.headerLength(message_t* msg)
    {
        return call Config.headerLength(msg) + sizeof(si443x_header_t);
    }

    async command uint8_t RadioPacket.payloadLength(message_t* msg)
    {
        return getHeader(msg)->length;
    }

    async command void RadioPacket.setPayloadLength(message_t* msg, uint8_t length)
    {
        RADIO_ASSERT( 1 <= length && length <= 125 );
        RADIO_ASSERT( call RadioPacket.headerLength(msg) + length + call RadioPacket.metadataLength(msg) <= sizeof(message_t) );

        getHeader(msg)->length = length;
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
