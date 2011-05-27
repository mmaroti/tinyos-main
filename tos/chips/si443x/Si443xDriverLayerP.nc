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
#include <Si443xRadioConfig.h>

#define DIAGMSG_STR(p) if( call DiagMsg.record() ) { call DiagMsg.str(p); call DiagMsg.send(); }
#define DIAGMSG_UINT(p) if( call DiagMsg.record() ) { call DiagMsg.uint8(p); call DiagMsg.send();}

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
    
//    interface GpioCapture as GPIO0;
//    interface GpioInterrupt as NIRQ;
            
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
        // Low Idle states  000000xx
        STATE_STANDBY       = 1,    // 00000001
        STATE_SLEEP         = 2,    // 00000010
        STATE_SENSOR        = 3,    // 00000011
        STATE_LOW_IDLE_MASK = 0x3,  // 00000011
        
        // High Idle states 0000xx00
        STATE_READY         = 4,    // 00000100
        STATE_TUNE          = 8,    // 00001000
        STATE_HIGH_IDLE_MASK = 0xC, // 00001100
        
        STATE_IDLE_MASK     = 0xF,  // 00001111
        
        // Other static states  00xx0000
        STATE_TX            = 16,   // 00010000
        STATE_RX            = 32,   // 00100000
        STATE_SHUTDOWN      = 64,   // 01000000
        
        // Dynamic state - between 2 real SM states        
        STATE_TRANSITION    = 128,  // 10000000
    };
    
    enum
    {
        CMD_NONE        = 0,
        CMD_IGNORE      = 1,
        
        CMD_CCA         = 10, // performing clear channel assesment
        CMD_CHANNEL     = 11, // changing the channel
        CMD_DOWNLOAD    = 12, // download the received message
        CMD_POWERUP     = 13, // initiate a power-up sequence
        CMD_SWRESET     = 14, // initiate a software reset sequence
    };
    
    typedef struct {
        uint8_t state;
        uint8_t next;
        uint8_t cmd;
        bool emit;
    } stm_t;
    
    tasklet_norace stm_t smachine;

    enum
    {
        POWER_ON_RESET_TIME = 16383 * RADIO_ALARM_MICROSEC,
        LOW_IDLE_2_TXRX_TIME = 800 * RADIO_ALARM_MICROSEC,
        HIGH_IDLE_2_TXRX_TIME = 200 * RADIO_ALARM_MICROSEC,
        LOW_IDLE_2_HIGH_IDLE_TIME = 600 * RADIO_ALARM_MICROSEC,
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
#ifdef SI443X_SDN_PIN_CONNECTED
        smachine.state = STATE_SHUTDOWN;
#else   
        smachine.state = STATE_TRANSITION;
#endif
        smachine.next = STATE_READY;
        smachine.emit = FALSE;
        smachine.cmd = CMD_IGNORE;
        
        return call SpiResource.request();
    }

/*----------------- ALARM -----------------*/

    tasklet_async event void RadioAlarm.fired()
    {
        RADIO_ASSERT( smachine.state == STATE_TRANSITION );
        RADIO_ASSERT( smachine.next == STATE_READY || smachine.next == STATE_TX || smachine.next == STATE_RX );
                
        smachine.state = smachine.next;
        smachine.cmd = CMD_NONE;
        
        if (smachine.emit) {
            smachine.emit = FALSE;
            signal RadioState.done();
        }
        call Tasklet.schedule();
    }
    
/*----------------- SPI -----------------*/

    event void SpiResource.granted()
    {
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
        
        RADIO_ASSERT( smachine.state != STATE_READY );
        RADIO_ASSERT( smachine.state != STATE_SHUTDOWN );
        
        writeRegister(SI443X_XTAL_POR_RW, maskedUpdate(oldvalue, SI443X_XTALPOR_BUFFER_DISABLE, SI443X_XTALPOR_BUFFER_MASK));
        
        // enter ready state
        writeRegister(SI443X_OPFCN_CTRL_1_RW, SI443X_OPFCN1_XTON );
        
        if ( smachine.state & STATE_LOW_IDLE_MASK ) {
            smachine.state = STATE_TRANSITION;
            smachine.cmd = CMD_IGNORE;
            call RadioAlarm.wait(LOW_IDLE_2_HIGH_IDLE_TIME); // OR wait for IRQ?
        }
        else if ( smachine.state & STATE_HIGH_IDLE_MASK ) {
            smachine.state = STATE_READY;
            smachine.cmd = CMD_NONE;
        } 
        else {
            RADIO_ASSERT(FALSE);
        }
    }
    
    inline void exitReadyState() {
        // re-enable the oscillator buffer
        uint8_t oldvalue = readRegister(SI443X_XTAL_POR_RW);
        
        RADIO_ASSERT( smachine.state == STATE_READY );
                
        writeRegister(SI443X_XTAL_POR_RW, maskedUpdate(oldvalue, SI443X_XTALPOR_BUFFER_ENABLE, SI443X_XTALPOR_BUFFER_MASK));
    }
    
    inline void enterStandbyState() {
        RADIO_ASSERT( smachine.state != STATE_STANDBY );
        
        // enter standby state
        writeRegister(SI443X_OPFCN_CTRL_1_RW, SI443X_OPFCN1_STANDBY);
        
        smachine.state = STATE_STANDBY;
        smachine.cmd = CMD_NONE;
        
        // check if state is in the IDLE group
        RADIO_ASSERT( 0 == (readRegister(SI443X_DEVICE_STATUS_R) & SI443X_DEVSTAT_CHIP_POWER_STATE_MASK) );
        // check if stat is low-power
        RADIO_ASSERT( 0x00 == (readRegister(SI443X_XTAL_POR_RW) & SI443X_XTALPOR_PWRST_MASK) );
        // check the oscillator buffer - ENABLED is the right value
        RADIO_ASSERT( SI443X_XTALPOR_BUFFER_ENABLE == (readRegister(SI443X_XTAL_POR_RW) & SI443X_XTALPOR_BUFFER_MASK) );
    }

    inline void enterRxState() {
        RADIO_ASSERT( smachine.state & STATE_IDLE_MASK );
        RADIO_ASSERT( smachine.state != STATE_RX );
        RADIO_ASSERT( smachine.next == STATE_RX );
        
        writeRegister(SI443X_OPFCN_CTRL_1_RW, SI443X_OPFCN1_RXON );
        
        smachine.cmd = CMD_IGNORE;
        
        if ( smachine.state & STATE_LOW_IDLE_MASK ) {
            smachine.state = STATE_TRANSITION;
            call RadioAlarm.wait(LOW_IDLE_2_TXRX_TIME); // OR wait for IRQ?
        }
        else if ( smachine.state & STATE_HIGH_IDLE_MASK ) {
            smachine.state = STATE_TRANSITION;
            call RadioAlarm.wait(HIGH_IDLE_2_TXRX_TIME); // OR wait for IRQ?
        } 
        else {
            RADIO_ASSERT(FALSE);
        }
        
    }

    inline void enterTxState() {
        RADIO_ASSERT( smachine.state & STATE_IDLE_MASK );
        RADIO_ASSERT( smachine.state != STATE_TX );
        RADIO_ASSERT( smachine.next == STATE_TX );
        
        writeRegister(SI443X_OPFCN_CTRL_1_RW, SI443X_OPFCN1_TXON );
        
        smachine.cmd = CMD_IGNORE;
        
        if ( smachine.state & STATE_LOW_IDLE_MASK ) {
            smachine.state = STATE_TRANSITION;
            call RadioAlarm.wait(LOW_IDLE_2_TXRX_TIME); // OR wait for IRQ?
        }
        else if (smachine.state & STATE_HIGH_IDLE_MASK ) {
            smachine.state = STATE_TRANSITION;
            call RadioAlarm.wait(HIGH_IDLE_2_TXRX_TIME); // OR wait for IRQ?
        }
        else {
            RADIO_ASSERT(FALSE);
        }
    }

    inline void enterPoweredUpState() {
        RADIO_ASSERT( smachine.state == STATE_SHUTDOWN );
#ifdef SI443X_SDN_PIN_CONNECTED
        call SDN.makeOutput();
        call SDN.clr();
#endif
        smachine.state = STATE_TRANSITION;
        smachine.cmd = CMD_IGNORE;
        call RadioAlarm.wait(POWER_ON_RESET_TIME); // OR wait for IRQ ?
    }
    
    inline void resetRadio() {
        writeRegister(SI443X_OPFCN_CTRL_1_RW, SI443X_OPFCN1_SWRESET | SI443X_OPFCN1_XTON);
    }

    /*----------------- TASKLET -----------------*/

    task void releaseSpi()
    {
        call SpiResource.release();
    }

    tasklet_async event void Tasklet.run()
    {
        stm_t localsm;
        atomic {
            localsm = smachine;
            smachine.cmd = CMD_IGNORE;
        }
        
        if ( localsm.cmd != CMD_IGNORE && localsm.state != STATE_TRANSITION && isSpiAcquired() ) {
            
            // if state transition is needed
            if ( localsm.state != localsm.next ) {

                // only one state needs to be exited properly : STATE_READY                
                if ( localsm.state == STATE_READY ) {
                    exitReadyState();
                }
                switch (localsm.next) {
                    case STATE_STANDBY:       enterStandbyState(); break;
      	            case STATE_READY:         enterReadyState();   break;
                    case STATE_RX:            enterRxState();      break;
                    case STATE_TX:            enterTxState();      break;
                    default:
                        RADIO_ASSERT(FALSE);
                }
            }
            
            if (smachine.emit && smachine.cmd != CMD_IGNORE) {
                signal RadioState.done();
                smachine.emit = FALSE;
            }
        }
        
        if ( smachine.cmd == CMD_NONE && smachine.state == STATE_TX && ! radioIrq )
            signal RadioSend.ready();
            
        if ( smachine.cmd == CMD_NONE )
            post releaseSpi();
        
    }

    tasklet_async command error_t RadioState.turnOff()
    {
        if( smachine.cmd != CMD_NONE || smachine.state == STATE_TRANSITION )
            return EBUSY;            
        else if( smachine.state == STATE_STANDBY )
            return EALREADY;
        
        smachine.next = STATE_STANDBY;
        smachine.emit = TRUE;
        smachine.cmd = CMD_NONE;
        
        call Tasklet.schedule();
        return SUCCESS;
    }
    
    tasklet_async command error_t RadioState.standby()
    {
        if( smachine.cmd != CMD_NONE || smachine.state == STATE_TRANSITION )
            return EBUSY;
        else if( smachine.state == STATE_READY )
            return EALREADY;
            
        smachine.next = STATE_READY;
        smachine.emit = TRUE;
        smachine.cmd = CMD_NONE;
        
        call Tasklet.schedule();
        return SUCCESS;
    }

    tasklet_async command error_t RadioState.turnOn()
    {
        if( smachine.cmd != CMD_NONE || smachine.state == STATE_TRANSITION || ! call RadioAlarm.isFree() )
            return EBUSY;
        else if( smachine.state == STATE_RX )
            return EALREADY;

        smachine.next = STATE_RX;
        smachine.emit = TRUE;
        smachine.cmd = CMD_NONE;
        
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
