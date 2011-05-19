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

#include <SI443XDriverLayer.h>
#include <Tasklet.h>
#include <RadioAssert.h>
#include <TimeSyncMessageLayer.h>
#include <RadioConfig.h>

module SI443XDriverLayerP
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
    
//    interface GpioCapture as GPIO0;
//    interface GpioInterrupt as NIRQ;
            
    interface FastSpiByte;
    interface Resource as SpiResource;

    interface BusyWait<TMicro, uint16_t>;
    interface LocalTime<TRadio>;

    interface SI443XDriverConfig as Config;

    interface PacketFlag as TransmitPowerFlag;
    interface PacketFlag as RSSIFlag;
    interface PacketFlag as TimeSyncFlag;

    interface PacketTimeStamp<TRadio, uint32_t>;

    interface Tasklet;
    interface RadioAlarm;
    
#ifdef RADIO_DEBUG
    interface DiagMsg;
#endif
  }
}

implementation
{
  
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

    /*----------------- STATE -----------------*/

    enum
    {
        STATE_SHUTDOWN      = 0,
        
        STATE_STANDBY       = 1,
        STATE_SLEEP         = 2,
        STATE_SENSOR        = 3,
        STATE_READY         = 4,
        STATE_TUNING        = 5,
        // virtual state for state group separation        
        STATE_IDLE          = 6,
        
        STATE_TX            = 20,
        STATE_RX            = 30,
        
        STATE_TRANSITION    = 100,
    };
    
    tasklet_norace uint8_t state = STATE_SHUTDOWN;
    uint16_t    trans_delays_microsec[] = {16800,800,800,800,200,200};
    
    /**
     * WARNING: trans values are assembled carefully!
     * TRANS_? values must equal to the value of the target state (STATE_? values).
     *
     * This allows us to optimize code (clear out unwanted branches).
     *
     * Note: If two transitions have the same destination, then both will have the same numerical value.
     * This would not introcude bugs until the uniqueness of (state,trans) is guaranteed!
     */
    enum
    {
        TRANS_SHUTDOWN      = STATE_SHUTDOWN,       // transition to STATE_SHUTDOWN
        
        TRANS_STANDBY       = STATE_STANDBY,        // transition to STATE_STANDBY
        TRANS_SLEEP         = STATE_SLEEP,          // transition to STATE_SLEEP
        TRANS_SENSOR        = STATE_SENSOR,         // transition to STATE_SENSOR
        TRANS_READY         = STATE_READY,          // transition to STATE_READY
        TRANS_TUNING        = STATE_TUNING,         // transition to STATE_TUNING
        TRANS_TX            = STATE_TX,             // transition to STATE_TX
        TRANS_RX            = STATE_RX,             // transition to STATE_RX
    };
    
    enum { 
        TRANS_CCA           = 100,  // performing clear channel assesment
        TRANS_CHANNEL       = 101,  // changing the channel
        TRANS_SIGNAL_DONE   = 102,  // signal the end of the state transition
        TRANS_DOWNLOAD      = 103,  // download the received message

        TRANS_POWER_UP      = 254,  // accomplishing Power-On-Reset
        TRANS_NONE          = 255,  // the state machine has stopped
        
    };
    
    tasklet_norace uint8_t trans = TRANS_NONE; 

    enum
	{
		POR_TIME = (uint16_t)(17000 * RADIO_ALARM_MICROSEC),
	};


    norace bool radioIrq;

    tasklet_norace uint8_t txPower;
    tasklet_norace uint8_t channel;

    tasklet_norace message_t* rxMsg;
    message_t rxMsgBuffer;

    uint16_t capturedTime;	// the current time when the last interrupt has occured

    tasklet_norace uint8_t rssiClear;
    tasklet_norace uint8_t rssiBusy;

/*----------------- REGISTER -----------------*/

	inline void writeRegister(uint8_t reg, uint8_t value)
	{
		RADIO_ASSERT( call SpiResource.isOwner() );
		RADIO_ASSERT( reg == (reg & SI443X_CMD_REGISTER_MASK) );

		call NSEL.clr();
		call FastSpiByte.splitWrite(SI443X_CMD_REGISTER_WRITE | reg);
		call FastSpiByte.splitReadWrite(value);
		call FastSpiByte.splitRead();
		call NSEL.set();
	}
	
    inline uint8_t readRegister(uint8_t reg)
	{
		RADIO_ASSERT( call SpiResource.isOwner() );
		RADIO_ASSERT( reg == (reg & SI443X_CMD_REGISTER_MASK) );

		call NSEL.clr();
		call FastSpiByte.splitWrite(SI443X_CMD_REGISTER_READ | reg);
		call FastSpiByte.splitReadWrite(0);
		reg = call FastSpiByte.splitRead();
		call NSEL.set();
		return reg;
	}
	
	// See Bit Twiddling Hacks - Merge bits from two values according to a mask
	// mask : 1 where bits from 'next' should be selected; 0 where from 'old'.
	inline uint8_t maskedUpdate(uint8_t old, uint8_t next, uint8_t mask) {
	    return old ^ ((old ^ next) & mask);
	}

    // SI443X_TODO: burst register read/write implementation

/*----------------- INIT -----------------*/

	command error_t PlatformInit.init()
	{
#ifdef SI443X_SDN_PIN_CONNECTED
        call SDN.makeOutput();
        call SDN.set();
#endif
	    call NSEL.makeOutput();
		call NSEL.set();

		rxMsg = &rxMsgBuffer;

		// these are just good approximates
		rssiClear = 0;
		rssiBusy = 90;

		return SUCCESS;    
	}

	command error_t SoftwareInit.init()
	{
	    state = STATE_SHUTDOWN;
        trans = TRANS_POWER_UP;
    	return call SpiResource.request();
	}

/*----------------- ALARM -----------------*/

	tasklet_async event void RadioAlarm.fired()
	{
	    RADIO_ASSERT( state == STATE_TRANSITION && ( trans == TRANS_POWER_UP || trans == TRANS_TX || trans == TRANS_RX) );
	    
	    // Since TRANS_? and STATE_? values are assembled carefully, 
	    // the new state is always equivalent to the transition number.
	    state = ( trans == TRANS_POWER_UP ) ? STATE_READY   : trans;
	    
	    // after initial power-up go to standby.
        trans = ( trans == TRANS_POWER_UP ) ? TRANS_STANDBY : TRANS_NONE;
	    call Tasklet.schedule();
	}
    
/*----------------- SPI -----------------*/

	event void SpiResource.granted()
	{
		call NSEL.makeOutput();
		call NSEL.set();
    	call Tasklet.schedule();
	}

	bool isSpiAcquired()
	{
		if( call SpiResource.isOwner() )
			return TRUE;

		if( call SpiResource.immediateRequest() == SUCCESS )
		{
			call NSEL.makeOutput();
			call NSEL.set();
			return TRUE;
		}

		call SpiResource.request();
		return FALSE;
	}

/*----------------- TURN ON/OFF -----------------*/

    inline void enterReadyState() {
        // disable the oscillator buffer
        uint8_t oldvalue = readRegister(SI443X_XTAL_POR_RW);
        writeRegister(SI443X_XTAL_POR_RW, maskedUpdate(oldvalue, SI443X_XTALPOR_BUFFER_DISABLE, SI443X_XTALPOR_BUFFER_MASK));
        
        // enter ready state
	    writeRegister(SI443X_OPFCN_CTRL_1_RW, SI443X_OPFCN1_XTON );
	    state = STATE_READY;
	    
	    // check if state is in the IDLE group
		RADIO_ASSERT( 0 == (readRegister(SI443X_DEVICE_STATUS_R) & SI443X_DEVSTAT_CHIP_POWER_STATE_MASK) );
		// check the oscillator buffer - DISABLED is the right value
		RADIO_ASSERT( SI443X_XTALPOR_BUFFER_DISABLE == (readRegister(SI443X_XTAL_POR_RW) & SI443X_XTALPOR_BUFFER_MASK) );
    }
    
    inline void exitReadyState() {
        // re-enable the oscillator buffer
        uint8_t oldvalue = readRegister(SI443X_XTAL_POR_RW);
        writeRegister(SI443X_XTAL_POR_RW, maskedUpdate(oldvalue, SI443X_XTALPOR_BUFFER_ENABLE, SI443X_XTALPOR_BUFFER_MASK));
    }
    
    inline void enterStandbyState() {
        // enter standby state
		writeRegister(SI443X_OPFCN_CTRL_1_RW, SI443X_OPFCN1_STANDBY);
		state = STATE_STANDBY;
		
		// check if state is in the IDLE group
		RADIO_ASSERT( 0 == (readRegister(SI443X_DEVICE_STATUS_R) & SI443X_DEVSTAT_CHIP_POWER_STATE_MASK) );
    	// check the oscillator buffer - ENABLED is the right value
		RADIO_ASSERT( SI443X_XTALPOR_BUFFER_ENABLE == (readRegister(SI443X_XTAL_POR_RW) & SI443X_XTALPOR_BUFFER_MASK) );
    }
    
    inline void enterSleepState() {
        // enter sleep state
		writeRegister(SI443X_OPFCN_CTRL_1_RW, SI443X_OPFCN1_WAKEUP_TIMER);
        state = STATE_SLEEP;
        
        // check if state is in the IDLE group
		RADIO_ASSERT( 0 == (readRegister(SI443X_DEVICE_STATUS_R) & SI443X_DEVSTAT_CHIP_POWER_STATE_MASK) );
    	// check the oscillator buffer - ENABLED is the right value
		RADIO_ASSERT( SI443X_XTALPOR_BUFFER_ENABLE == (readRegister(SI443X_XTAL_POR_RW) & SI443X_XTALPOR_BUFFER_MASK) );
    }
    
    inline void enterPoweredUpState() {
        RADIO_ASSERT( state == STATE_SHUTDOWN );
    
        // enter powered up state
#ifdef SI443X_SDN_PIN_CONNECTED
        call SDN.makeOutput();
        call SDN.clr();
#endif
        call RadioAlarm.wait(POR_TIME);
        state = STATE_TRANSITION;
    }
    
    inline void enterShutdownState() {
        // enter shutdown state
#ifdef SI443X_SDN_PIN_CONNECTED
        call SDN.makeOutput();
        call SDN.set();
#endif
        state = STATE_SHUTDOWN;
    }
    
    inline bool enterRxState() {
        if ( call RadioAlarm.isFree() ) {
            enterReadyState();
            writeRegister(SI443X_OPFCN_CTRL_1_RW, SI443X_OPFCN1_RXON );
            call RadioAlarm.wait( trans_delays_microsec[state] );
            state = STATE_TRANSITION;
            return TRUE;
        } else
            return FALSE;
    }

    inline bool enterTxState() {
        if ( call RadioAlarm.isFree() ) {
            enterReadyState();
            writeRegister(SI443X_OPFCN_CTRL_1_RW, SI443X_OPFCN1_TXON );
            call RadioAlarm.wait( trans_delays_microsec[state] );
            state = STATE_TRANSITION;
            return TRUE;
        } else
            return FALSE;
    }

    inline void resetRadio() {
        // initiate a software reset
		// this will clear all registers to their default value.
		writeRegister(SI443X_OPFCN_CTRL_1_RW, SI443X_OPFCN1_SWRESET | SI443X_OPFCN1_XTON);
    }

	/*----------------- TASKLET -----------------*/

	task void releaseSpi()
	{
		call SpiResource.release();
	}

    tasklet_async event void Tasklet.run()
	{
	    RADIO_ASSERT ( trans != state );            // optimal scheduling ?
#ifndef SI443X_SDN_PIN_CONNECTED
        RADIO_ASSERT ( trans != TRANS_SHUTDOWN );   // if SDN is not connected, SHUTDOWN state is not accessible
#endif
//        if( radioIrq )
//			serviceRadio();
        
        if ( trans != TRANS_NONE && isSpiAcquired() ) {
        
            // only one state needs to be exited properly : STATE_READY (congrat. developers!)
	        if ( state == STATE_READY )
	            exitReadyState();
	            
	        switch (trans) {
                case TRANS_STANDBY:     enterStandbyState();    break;	                
   	            case TRANS_READY:       enterReadyState();     	break;
	            case TRANS_SLEEP:       enterSleepState();      break;
	            case TRANS_POWER_UP:    enterPoweredUpState();  break;
            
                //case TRANS_TUNE:
                case TRANS_TX:          enterTxState();         break;
                case TRANS_RX:          enterRxState();         break;
                default:
                    // not other transitions are supperted yet.
                    RADIO_ASSERT(FALSE);
                    break;
            }
        }
        
        if ( trans == TRANS_NONE && state == STATE_TX && ! radioIrq )
            signal RadioSend.ready();
            
        if ( trans == TRANS_NONE )
            post releaseSpi();
        
    }

	tasklet_async command error_t RadioState.turnOff()
	{
		if( trans != TRANS_NONE )
			return EBUSY;
			
#ifdef SI443X_SDN_PIN_CONNECTED			
		else if( state == STATE_SHUTDOWN )
			return EALREADY;

        trans = TRANS_SHUTDOWN;
#else
        else if( state == STATE_STANDBY )
		    return EALREADY;

  		trans = TRANS_STANDBY;
#endif
		call Tasklet.schedule();
		return SUCCESS;
	}
	
	tasklet_async command error_t RadioState.standby()
	{
		if( trans != TRANS_NONE || (state == STATE_SHUTDOWN && ! call RadioAlarm.isFree()) )
			return EBUSY;
		else if( state == STATE_READY )
			return EALREADY;

		trans = TRANS_READY;
		call Tasklet.schedule();
		return SUCCESS;
	}

	tasklet_async command error_t RadioState.turnOn()
	{
		if( trans != TRANS_NONE || (state == STATE_SLEEP && ! call RadioAlarm.isFree()) )
			return EBUSY;
		else if( state == STATE_RX )
			return EALREADY;

		trans = TRANS_RX;
		call Tasklet.schedule();
		return SUCCESS;
	}

	default tasklet_async event void RadioState.done() { }

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

/*	async event void GPIO0.captured(uint16_t time)
	{
		
	}
	*/
//	async event void NIRQ.fired() {}

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
