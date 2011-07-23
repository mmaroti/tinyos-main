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
        
        interface Timer<TMilli> as MilliTimer;
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
    
#define DIAGMSG_STM(PSTR)         \
        if( call DiagMsg.record() ) { \
            call DiagMsg.str(PSTR);\
            call DiagMsg.str("sm");\
            call DiagMsg.hex8(sm.state); \
            call DiagMsg.hex8(sm.next); \
            call DiagMsg.uint8(sm.cmd);\
            call DiagMsg.hex8(sm.debug);\
            call DiagMsg.send(); \
        }

#define DIAGMSG_VAR(PSTR,VAR) \
        if( call DiagMsg.record() ) { \
            call DiagMsg.str(PSTR);\
            call DiagMsg.uint8(VAR); \
            call DiagMsg.hex8(VAR); \
            call DiagMsg.send(); \
        }
        
#define DIAGMSG_STR(PSTR,STR) \
        atomic { if( call DiagMsg.record() ) { \
            call DiagMsg.str(PSTR);\
            call DiagMsg.str(STR); \
            call DiagMsg.send(); \
        }        }





       
    
 /*    uint8_t deviceState() {
        uint8_t r1 = 0,r2 = 0,r3 = 0;
        if ( !isSpiAcquired() )
            return 0;
        
        r1 = readRegister(SI443X_OPFCN_CTRL_1_RW);
        r2 = readRegister(SI443X_DEVICE_STATUS_R) & 0x03;
        r3 = readRegister(SI443X_XTAL_POR_RW) & 0xE0;
        
        // Ready
        if ( (r1 & 0x01) == 1 && r2 == 0 && r3 == 0x20 )
            return STATE_READY;
        else if ( r1 == 0 && r2 == 0 && r3 == 0)
            return STATE_STANDBY;
        else if ( (r1 & 0x08) == 0x08 && r2 == 2 && r3 == 0x40)
            return STATE_TX;
        else if ( (r1 & 0x04) == 0x04 && r2 == 1 && r3 == 0xE0)
            return STATE_RX;        
        else
            return 255;
    }
    #define SMACHINE_CHECK() { uint8_t ds = deviceState(); if( ds == 0 || sm.state == ds ) RADIO_ASSERT(0); } 
   */ 
    

#endif
/* ----------------- END DEBUGGER FUNCTIONS AND HELPERS  -----------------*/    
    
    
    
    
    bool isSpiAcquired();
    void serviceRadio();
  
    /*----------------- STATE -----------------*/

    enum
    {
        // Low Idle states  000000xx
        STATE_STANDBY       = 0x01,   // 00000001
        STATE_SLEEP         = 0x02,   // 00000010
        STATE_SENSOR        = 0x03,   // 00000011
        STATE_LOW_IDLE_MASK = 0x03,   // 00000011
        
        // High Idle states 0000xx00
        STATE_READY         = 0x04,   // 00000100
        STATE_TUNE          = 0x08,   // 00001000
        STATE_HIGH_IDLE_MASK = 0xC,   // 00001100
        
        STATE_IDLE_MASK     = 0xF,    // 00001111
        
        // Other static states  00xx0000
        STATE_TX            = 0x10,   // 00010000
        STATE_RX            = 0x20,   // 00100000
        STATE_TXRX_MASK     = 0x30,   // 00110000
        
        STATE_SHUTDOWN      = 0x40,   // 01000000
        
        // Transition Flag to signal that the current state has not yet been reached.
        TRANS_FLAG          = 0x80,   // 10000000
    } state_enum;
    
    enum
    {
        CMD_IGNORE_FLAG = 1<<7,
    
        CMD_NONE        = 0,
        CMD_INIT_RADIO  = 1,
        
        CMD_EMIT        = 2,

        CMD_CHANNEL     = 11, // changing the channel
        CMD_DOWNLOAD    = 12, // download the received message
        CMD_POWERUP     = 13, // initiate a power-up sequence
        CMD_SWRESET     = 14, // initiate a software reset sequence
        CMD_CCA         = 15, // performing clear channel assesment
                
    } cmd_enum;
    
    typedef struct {
        uint8_t state;
        uint8_t next;
        uint8_t cmd;
        uint8_t debug;
    } stm_t;
    
    tasklet_norace stm_t sm;

 
    norace bool radioIrq;
    uint16_t capturedTime;	// the current time when the last interrupt has occured
        
    tasklet_norace uint8_t txPower;
    tasklet_norace uint8_t channel;

    tasklet_norace message_t* rxMsg;
    message_t rxMsgBuffer;

    tasklet_norace uint8_t rssiClear;
    tasklet_norace uint8_t rssiBusy;

/*----------------- REGISTER -----------------*/

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
    void debug() {
              
        DIAGMSG_STR("dbg","beg")
        readRegister(0x62);
        DIAGMSG_STR("dbg","end")
        
    }    
    void debugi() {
        uint8_t i;
        
        for (i = 0; i< 10; ++i) {
            readRegister(0x04);
            readRegister(0x62);
            call BusyWait.wait(100);            
        }
        
        call BusyWait.wait(30000);
        readRegister(0x04);
        readRegister(0x62);        
        DIAGMSG_STR("---","-")
    }    
    
    
    
    /** DO NOT DELETE - VERIFIED **/
    void _si443x_ready() {
    
     /*  writeRegister(0x05,0xff);
        writeRegister(0x06,0xff);
        readRegister(0x03);
        readRegister(0x04);
    */
        // wait for CHIPRDY interrupt
        writeRegister(0x06,0x02);
     
        // READY!
        writeRegister(0x07,0x01);

       // buf disable
        //writeRegister(0x62, masked(readRegister(0x62),0x02,0x02) );
        //debugi();
    }

    /** DO NOT DELETE - VERIFIED **/
    void _si443x_sby() {
    
      //  readRegister(0x62);
     //   readRegister(0x04);
       
        // disable POR an CHPRDY interrupts -> THIS IS THE ONLY ESSENTIAL CONDITION
        // TODO: Burst write
          writeRegister(0x05,0x00);
        writeRegister(0x06,0x00);
        
        // READY
        writeRegister(0x07,0x00);
        
        //debugi();
     

    }

    /** DO NOT DELETE - VERIFIED **/
    void _si443x_initialize() {
        call BusyWait.wait(30000);

        // SWRESET
        writeRegister(SI443X_OPFCN_CTRL_1_RW, SI443X_OPFCN1_SWRESET | SI443X_OPFCN1_XTON);
        // wait for XTAL settling time
        call BusyWait.wait(30000);
        
        DIAGMSG_STR("test","!");
        
        _si443x_sby();    
       
        // sm.state = STATE_STANDBY
        status = 1;
    }
    
    void initRadio() {
        status = 0;
        _si443x_initialize();
        debug();
        call Tasklet.schedule();
    }
    
     
    tasklet_async event void Tasklet.run()
    {
        if ( radioIrq ) {
            radioIrq = FALSE;
            DIAGMSG_STR("readI","!");
            readRegister(0x04);
            
        }
    
        switch ( status ) {
            case 1:
                _si443x_ready();
                status = 2;
                break;
            case 2:
                _si443x_sby();
//                debugi();
                
                status = 3;
                call Tasklet.schedule();
                break;
            case 3:
                debugi(); break;
                
        }
    }
    
	async event void IRQ.captured(uint16_t time)
    {
        /**
         * Findings : 
         * 
         *  1, When Ready->Standby transition is needed, NO REGISTER, NOTHING can be read before the STANDBY sequence
         *     Otherwise, it does not go to low power mode.
         */ 
        
        DIAGMSG_STR("IRQ","!");
         
        radioIrq = TRUE;
        call Tasklet.schedule();
        
    }
    
    event void MilliTimer.fired() {
    }


    command error_t PlatformInit.init()
    {
        call NSEL.makeOutput();
        call NSEL.set();
        call IRQ.captureFallingEdge();
            
        return SUCCESS;
    }

    command error_t SoftwareInit.init()
    {
        rxMsg = &rxMsgBuffer;
        rssiClear = 0;
        rssiBusy = 90;
        
        // Waiting for POR to finish, then go to low-power state
        sm.state = STATE_READY;
        sm.next  = STATE_STANDBY;
        sm.cmd   = CMD_INIT_RADIO;
    
        return SUCCESS;                
    }
    
    event void Boot.booted() {
        DIAGMSG_STR("booted","!");
        call SpiResource.request();
    }

    event void SpiResource.granted()
    {
        if (sm.cmd == CMD_INIT_RADIO )
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

 /*   void enterState(uint8_t newstate) {
        
        RADIO_ASSERT( sm.state != newstate );
        
        DIAGMSG_STM("+E");        

        readRegister(0x07);
        readRegister(0x02);
        readRegister(0x62);

        switch ( newstate ) {
            case STATE_STANDBY:
                DIAGMSG_STR("  E","SBY");
                writeRegister(SI443X_OPFCN_CTRL_1_RW, SI443X_OPFCN1_STANDBY);
                break;

            case STATE_READY:
                DIAGMSG_STR("  E","RDY");
                // disable oscillator buffer
         //       oldregvalue = readRegister(SI443X_XTAL_POR_RW);
         //       writeRegister(SI443X_XTAL_POR_RW, maskedUpdate(oldregvalue, SI443X_XTALPOR_BUFFER_DISABLE, SI443X_XTALPOR_BUFFER_MASK));
                
                // enter       
                writeRegister(SI443X_OPFCN_CTRL_1_RW, SI443X_OPFCN1_XTON );
                
                break;
                
            case STATE_TX:
                DIAGMSG_STR("  E","TX");
                writeRegister(SI443X_OPFCN_CTRL_1_RW, SI443X_OPFCN1_TXON );
                break;
                
            case STATE_RX:
                DIAGMSG_STR("  E","RX");
                writeRegister(SI443X_OPFCN_CTRL_1_RW, SI443X_OPFCN1_RXON );
                break;
                
            case STATE_TUNE:
                DIAGMSG_STR("  E","TUN");
                writeRegister(SI443X_OPFCN_CTRL_1_RW, SI443X_OPFCN1_PLLON | SI443X_OPFCN1_XTON );
                break;
                
            default:
                RADIO_ASSERT(0);
        }
        
        sm.state = newstate;
        if ( newstate & STATE_TXRX_MASK || newstate == STATE_READY )
            sm.state |= TRANS_FLAG;
        
        readRegister(0x07);
        readRegister(0x02);
        readRegister(0x62);
        
        DIAGMSG_STM("-E");
    }


    inline void exitReadyState() {
        // re-enable the oscillator buffer
      //  uint8_t oldvalue = readRegister(SI443X_XTAL_POR_RW);
      //  writeRegister(SI443X_XTAL_POR_RW, maskedUpdate(oldvalue, SI443X_XTALPOR_BUFFER_ENABLE, SI443X_XTALPOR_BUFFER_MASK));
    }*/
    
    
    
        /*

        lstate = sm.state & ~TRANS_FLAG;
              
        // If not in transition AND current state differs from next state
        // -> Initiate a state change
        if ( !(sm.state & TRANS_FLAG) && lstate != sm.next && isSpiAcquired() ) {
            
            // READY state must be exited properly
            if ( lstate == STATE_READY )
                exitReadyState();
            
            // switch between RX and TX needs to go back to TUNE|READY mode
            if ( lstate & STATE_TXRX_MASK && sm.next & STATE_TXRX_MASK ) {
                DIAGMSG_STR(" r","RTX")
                
                enterState(STATE_TUNE);
                lstate = STATE_TUNE;
            }
            
            // enter the new state
            enterState(sm.next);
        }
                
        if ( sm.cmd == CMD_EMIT && sm.state == sm.next ) {
            DIAGMSG_STR(" r","emit")
            signal RadioState.done();
            sm.cmd = CMD_NONE;
        }
                        
        if ( sm.cmd == CMD_NONE ) {
        
            if ( sm.state == STATE_TX && ! radioIrq ) {
                DIAGMSG_STR(" r","RSrdy")
                signal RadioSend.ready();
            }
            
            if ( sm.state == sm.next) {
                DIAGMSG_STR(" r","rlsSpi")
                post releaseSpi();
            }            
        } 
               
        DIAGMSG_STM("-r");*/
    


























    tasklet_async command error_t RadioState.turnOff()
    {
        DIAGMSG_STM("+tOff")
    
        if( sm.cmd != CMD_NONE || sm.state & TRANS_FLAG )
            return EBUSY;            
        else if( sm.state == STATE_STANDBY )
            return EALREADY;
        
        sm.next = STATE_STANDBY;
        sm.cmd = CMD_EMIT;
        
        DIAGMSG_STM("!tOff")
        
        call Tasklet.schedule();
        return SUCCESS;
    }
    
    tasklet_async command error_t RadioState.standby()
    {
        DIAGMSG_STM("+sby")
        
        if( sm.cmd != CMD_NONE || sm.state & TRANS_FLAG || ! call RadioAlarm.isFree() )
            return EBUSY;
        else if( sm.state == STATE_READY )
            return EALREADY;
            
        sm.next = STATE_READY;
        sm.cmd = CMD_EMIT;
        
        DIAGMSG_STM("!sby")
      
        call Tasklet.schedule();
        return SUCCESS;
    }

    tasklet_async command error_t RadioState.turnOn()
    {
    
        DIAGMSG_STM("+tOn")
        
        if( sm.cmd != CMD_NONE || sm.state & TRANS_FLAG || ! call RadioAlarm.isFree() )
            return EBUSY;
        else if( sm.state == STATE_RX )
            return EALREADY;

        sm.next = STATE_RX;
        sm.cmd = CMD_EMIT;
        
        DIAGMSG_STM("!tOn")
        
        call Tasklet.schedule();
        return SUCCESS;
    }



















    //default tasklet_async event void RadioState.done() { }

 /*----------------- ALARM -----------------*/

    tasklet_async event void RadioAlarm.fired()
    {

    }

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
