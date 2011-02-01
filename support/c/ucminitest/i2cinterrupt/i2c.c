 /*! \file i2c.c \brief I2C interface using AVR Two-Wire Interface (TWI) hardware. */
 //*****************************************************************************
 //
 // File Name    : 'i2c.c'
 // Title        : I2C interface using AVR Two-Wire Interface (TWI) hardware
 // Author       : Pascal Stang - Copyright (C) 2002-2003
 // Created      : 2002.06.25
 // Revised      : 2003.03.02
 // Version      : 0.9
 // Target MCU   : Atmel AVR series
 // Editor Tabs  : 4
 //
 // Description : I2C (pronounced "eye-squared-see") is a two-wire bidirectional
 //      network designed for easy transfer of information between a wide variety
 //      of intelligent devices.  Many of the Atmel AVR series processors have
 //      hardware support for transmitting and receiving using an I2C-type bus.
 //      In addition to the AVRs, there are thousands of other parts made by
 //      manufacturers like Philips, Maxim, National, TI, etc that use I2C as
//      their primary means of communication and control.  Common device types
//      are A/D & D/A converters, temp sensors, intelligent battery monitors,
 //      MP3 decoder chips, EEPROM chips, multiplexing switches, etc.
 //
//      I2C uses only two wires (SDA and SCL) to communicate bidirectionally
 //      between devices.  I2C is a multidrop network, meaning that you can have
 //      several devices on a single bus.  Because I2C uses a 7-bit number to
 //      identify which device it wants to talk to, you cannot have more than
 //      127 devices on a single bus.
 //
 //      I2C ordinarily requires two 4.7K pull-up resistors to power (one each on
 //      SDA and SCL), but for small numbers of devices (maybe 1-4), it is enough
 //      to activate the internal pull-up resistors in the AVR processor.  To do
 //      this, set the port pins, which correspond to the I2C pins SDA/SCL, high.
 //      For example, on the mega163, sbi(PORTC, 0); sbi(PORTC, 1);.
 //
 //      For complete information about I2C, see the Philips Semiconductor
 //      website.  They created I2C and have the largest family of devices that
 //      work with I2C.
//
 // Note: Many manufacturers market I2C bus devices under a different or generic
 //      bus name like "Two-Wire Interface".  This is because Philips still holds
 //      "I2C" as a trademark.  For example, SMBus and SMBus devices are hardware
 //      compatible and closely related to I2C.  They can be directly connected
 //      to an I2C bus along with other I2C devices are are generally accessed in
 //      the same way as I2C devices.  SMBus is often found on modern motherboards
 //      for temp sensing and other low-level control tasks.
 //
 // This code is distributed under the GNU Public License
 //      which can be found at http://www.gnu.org/licenses/gpl.txt
 //
 //*****************************************************************************

#define F_CPU 16000000 

 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <util/twi.h>
 #include <util/delay.h>
 
 #include "i2c.h"
 
// #include "uart.h"       // include uart function library
//#include "rprintf.h"    // include printf function library
 
 
// Standard I2C bit rates are:
 // 100KHz for slow speed
 // 400KHz for high speed
 
 // I2C state and address variables
 static volatile eI2cStateType I2cState;
 static uint8_t I2cDeviceAddrRW;
 // send/transmit buffer (outgoing data)
 static uint8_t I2cSendData[I2C_SEND_DATA_BUFFER_SIZE];
 static uint8_t I2cSendDataIndex;
 static uint8_t I2cSendDataLength;
 // receive buffer (incoming data)
 static uint8_t I2cReceiveData[I2C_RECEIVE_DATA_BUFFER_SIZE];
 static uint8_t I2cReceiveDataIndex;
 static uint8_t I2cReceiveDataLength;
 
 uint16_t cycles;
int BRREG_VALUE;
 
 // function pointer to i2c receive routine
 //! I2cSlaveReceive is called when this processor
 // is addressed as a slave for writing
 static void (*i2cSlaveReceive)(uint8_t receiveDataLength, uint8_t* recieveData);
 //! I2cSlaveTransmit is called when this processor
 // is addressed as a slave for reading
 static uint8_t (*i2cSlaveTransmit)(uint8_t transmitDataLengthMax, uint8_t* transmitData);
 
 // functions

/* Measure clock cycles per Jiffy (1/32768s) */
void Serial_init() {
	uint8_t wraps;
	uint16_t now;

	/* Setup timer2 to at 32768 Hz, and timer1 cpu cycles */
	TCCR1B = 1 << CS10;
	ASSR = 1 << AS2;
	TCCR2B = 1 << CS20;

	// one wrap is 256/32768 = 1/128 sec
	for(wraps = 255; wraps != 0; --wraps)
	{
		while( TCNT2 != 0 )
			;

		now = TCNT1;

		while( TCNT2 != 32 )	// wait 32/32768 = 1/1024 sec
			;

		cycles = TCNT1;
	}

	cycles -= now;

	/* Reset to boot state */
	TCCR1B = 0;
	ASSR = 0;
	TCCR2B = 0;
}

int baudrateRegister(uint32_t baudrate) {
	Serial_init();

	// we use the fast setting: (cycles * 1024) / (8 * baudrate) - 1
	return (((uint32_t)cycles) << 7) / ((uint32_t)baudrate) - 1;
}

 //---defines
void initbootuart(void)
{
  BRREG_VALUE = baudrateRegister(57600);

  UBRR1L = BRREG_VALUE;
  UBRR1H= (BRREG_VALUE << 8);
  UCSR1A = (1<<U2X1);
  UCSR1B = (1 << RXEN1) |
                     (1 << TXEN1); // enable receive and transmit
  
  UCSR1C = (0 << USBS1) | (3 << UCSZ10); // setting uart frame format 8 data bits; 1 stop bit
  
}


void sendchar(unsigned char c)
{
  UDR1 = c;                                   // prepare transmission
  while (!(UCSR1A & (1 << TXC1)));// wait until byte sendt
  UCSR1A |= (1 << TXC1);          // delete TXCflag
}

 
 void i2cInit(void)
 {
     // set pull-up resistors on I2C bus pins
   
     PORTD = _BV(PD0) | _BV(PD1);
		 
		 DDRF |= _BV(PF2);
		 PORTF |= _BV(PF2);
 
     // clear SlaveReceive and SlaveTransmit handler to null
     i2cSlaveReceive = 0;
    i2cSlaveTransmit = 0;
     // set i2c bit rate to 100KHz
     //i2cSetBitrate(100);
		 TWBR = 255;
		 TWSR |= (1<<TWPS0) | (1<<TWPS1);
     // enable TWI (two-wire interface)
    TWCR |= (1 << TWEN);
     // set state
     I2cState = I2C_IDLE;
     // enable TWI interrupt and slave address ACK
     TWCR |= ((1 << TWIE) | (1 << TWEA));
   //outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWEA));
     // enable interrupts
     sei();
 }
 
 void i2cSetBitrate(uint16_t bitrateKHz)
{
     uint8_t bitrate_div;

     // calculate bitrate division   
     bitrate_div = ((F_CPU/1000l)/bitrateKHz);
     if(bitrate_div >= 16)
         bitrate_div = (bitrate_div-16)/2;
     TWBR = bitrate_div;
 }
 
 
 void i2cSetSlaveReceiveHandler(void (*i2cSlaveRx_func)(uint8_t receiveDataLength, uint8_t* recieveData))
 {
     i2cSlaveReceive = i2cSlaveRx_func;
 }
 
 void i2cSetSlaveTransmitHandler(uint8_t (*i2cSlaveTx_func)(uint8_t transmitDataLengthMax, uint8_t* transmitData))
 {
     i2cSlaveTransmit = i2cSlaveTx_func;
 }
 
 inline void i2cSendStart(void)
 {
    // send start condition
     TWCR = ((TWCR&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWSTA));
 }
 
 inline void i2cSendStop(void)
 {
     // transmit stop condition
     // leave with TWEA on for slave receiving
		// sendchar('S');
     TWCR = ((TWCR&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWEA)|_BV(TWSTO));
 }
 
 inline void i2cWaitForComplete(void)
 {
     // wait for i2c interface to complete operation
     while( !(TWCR & _BV(TWINT)) );
 }
 
 inline void i2cSendByte(uint8_t data)
 {
     // save data to the TWDR
    TWDR = data;
     // begin send
    TWCR = ((TWCR&TWCR_CMD_MASK)|_BV(TWINT));
		//_delay_ms(3000);
 }
 
 inline void i2cReceiveByte(uint8_t ackFlag)
 {
     // begin receive over i2c
     if( ackFlag )
     {
         // ackFlag = TRUE: ACK the recevied data
         TWCR = ((TWCR&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWEA));
     }
     else
    {
         // ackFlag = FALSE: NACK the recevied data
         TWCR = ((TWCR&TWCR_CMD_MASK)|_BV(TWINT));
     }
 }
 
 inline uint8_t i2cGetReceivedByte(void)
 {
     // retieve received data byte from i2c TWDR
    return TWDR;
 }
 
 inline uint8_t i2cGetStatus(void)
 {
     // retieve current i2c status from i2c TWSR
     return TWSR;
}
 
 void i2cMasterSend(uint8_t deviceAddr, uint8_t length, uint8_t* data)
 {
    uint8_t i;
     // wait for interface to be ready
     while(I2cState);
     // set state
     I2cState = I2C_MASTER_TX;
     // save data
     I2cDeviceAddrRW = (deviceAddr & 0xFE);  // RW cleared: write operation
     for(i=0; i<length; i++)
         I2cSendData[i] = *data++;
    I2cSendDataIndex = 0;
    I2cSendDataLength = length;
     // send start condition
    i2cSendStart();
}
 
 void i2cMasterReceive(uint8_t deviceAddr, uint8_t length, uint8_t* data)
 {
     uint8_t i;
     // wait for interface to be ready
     while(I2cState);
     // set state
     I2cState = I2C_MASTER_RX;
     // save data
     I2cDeviceAddrRW = (deviceAddr|0x01);    // RW set: read operation
     I2cReceiveDataIndex = 0;
     I2cReceiveDataLength = length;
     // send start condition
     i2cSendStart();
     // wait for data
     while(I2cState);
     // return data
     for(i=0; i<length; i++)
         *data++ = I2cReceiveData[i];
 }
 
 //! I2C (TWI) interrupt service routine
 ISR(TWI_vect)
 {
     // read status bits
     uint8_t status = (TWSR & TWSR_STATUS_MASK);
 //sendchar(status);
     switch(status)
     {
     // Master General
     case TW_START:                      // 0x08: Sent start condition
     case TW_REP_START:                  // 0x10: Sent repeated start condition
         // send device address
				 //_delay_ms(30);
         i2cSendByte(I2cDeviceAddrRW);
				 ////_delay_ms(3000);
         //outb(TWDR, I2cDeviceAddrRW);
         //outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|_BV(TWINT));
         break;
     
     // Master Transmitter & Receiver status codes
     case TW_MT_SLA_ACK:                 // 0x18: Slave address acknowledged
     case TW_MT_DATA_ACK:                // 0x28: Data acknowledged
         if(I2cSendDataIndex < I2cSendDataLength)
         {
             // send data
						 //_delay_ms(30);
             i2cSendByte( I2cSendData[I2cSendDataIndex++] );
             //outb(TWDR, I2cSendData[I2cSendDataIndex++]);
             //outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|_BV(TWINT));
         }
        else
         {
             // transmit stop condition, enable SLA ACK
             i2cSendStop();
						 //_delay_ms(30);
             //outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWSTO)|_BV(TWEA));
             // set state
             I2cState = I2C_IDLE;
         }
        break;
     case TW_MR_DATA_NACK:               // 0x58: Data received, NACK reply issued
         // store final received data byte
         I2cReceiveData[I2cReceiveDataIndex++] = TWDR;
        // continue to transmit STOP condition
    case TW_MR_SLA_NACK:                // 0x48: Slave address not acknowledged
     case TW_MT_SLA_NACK:                // 0x20: Slave address not acknowledged
    case TW_MT_DATA_NACK:               // 0x30: Data not acknowledged
        // transmit stop condition, enable SLA ACK
         i2cSendStop();
        //outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWSTO)|_BV(TWEA));
         // set state
         I2cState = I2C_IDLE;
        break;
     case TW_MT_ARB_LOST:                // 0x38: Bus arbitration lost
     //case TW_MR_ARB_LOST:              // 0x38: Bus arbitration lost
         // release bus
        TWCR = ((TWCR&TWCR_CMD_MASK)|_BV(TWINT));
         // set state
         I2cState = I2C_IDLE;
         // release bus and transmit start when bus is free
         //outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWSTA));
         break;
     case TW_MR_DATA_ACK:                // 0x50: Data acknowledged
         // store received data byte
         I2cReceiveData[I2cReceiveDataIndex++] = TWDR;
         // fall-through to see if more bytes will be received
				 //break;
     case TW_MR_SLA_ACK:                 // 0x40: Slave address acknowledged
         if(I2cReceiveDataIndex < (I2cReceiveDataLength-1))
             // data byte will be received, reply with ACK (more bytes in transfer)
						 {////_delay_ms(30);
             i2cReceiveByte(TRUE);}
             //outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWEA));
        else
             // data byte will be received, reply with NACK (final byte in transfer)
						 {////_delay_ms(30);
             i2cReceiveByte(FALSE);}
             //outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|_BV(TWINT));
         break;
 
    // Slave Receiver status codes
     case TW_SR_SLA_ACK:                 // 0x60: own SLA+W has been received, ACK has been returned
     case TW_SR_ARB_LOST_SLA_ACK:        // 0x68: own SLA+W has been received, ACK has been returned
    case TW_SR_GCALL_ACK:               // 0x70:     GCA+W has been received, ACK has been returned
    case TW_SR_ARB_LOST_GCALL_ACK:      // 0x78:     GCA+W has been received, ACK has been returned
         // we are being addressed as slave for writing (data will be received from master)
        // set state
         I2cState = I2C_SLAVE_RX;
         // prepare buffer
         I2cReceiveDataIndex = 0;
         // receive data byte and return ACK
        TWCR = ((TWCR&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWEA));
        break;
     case TW_SR_DATA_ACK:                // 0x80: data byte has been received, ACK has been returned
     case TW_SR_GCALL_DATA_ACK:          // 0x90: data byte has been received, ACK has been returned
         // get previously received data byte
         I2cReceiveData[I2cReceiveDataIndex++] = TWDR;
         // check receive buffer status
        if(I2cReceiveDataIndex < I2C_RECEIVE_DATA_BUFFER_SIZE)
         {
            // receive data byte and return ACK
            TWCR = ((TWCR&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWEA));
         }
        else
         {
             // receive data byte and return NACK
            TWCR = ((TWCR&TWCR_CMD_MASK)|_BV(TWINT));
         }
         break;
     case TW_SR_DATA_NACK:               // 0x88: data byte has been received, NACK has been returned
     case TW_SR_GCALL_DATA_NACK:         // 0x98: data byte has been received, NACK has been returned
         // receive data byte and return NACK
        TWCR = ((TWCR&TWCR_CMD_MASK)|_BV(TWINT));
         break;
     case TW_SR_STOP:                    // 0xA0: STOP or REPEATED START has been received while addressed as slave
         // switch to SR mode with SLA ACK
         TWCR = ((TWCR&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWEA));
         // i2c receive is complete, call i2cSlaveReceive
         if(i2cSlaveReceive) i2cSlaveReceive(I2cReceiveDataIndex, I2cReceiveData);
         // set state
        I2cState = I2C_IDLE;
         break;
 
     // Slave Transmitter
     case TW_ST_SLA_ACK:                 // 0xA8: own SLA+R has been received, ACK has been returned
     case TW_ST_ARB_LOST_SLA_ACK:        // 0xB0:     GCA+R has been received, ACK has been returned
         // we are being addressed as slave for reading (data must be transmitted back to master)
         // set state
         I2cState = I2C_SLAVE_TX;
         // request data from application
         if(i2cSlaveTransmit) I2cSendDataLength = i2cSlaveTransmit(I2C_SEND_DATA_BUFFER_SIZE, I2cSendData);
         // reset data index
         I2cSendDataIndex = 0;
         // fall-through to transmit first data byte
     case TW_ST_DATA_ACK:                // 0xB8: data byte has been transmitted, ACK has been received
         // transmit data byte
         TWDR = I2cSendData[I2cSendDataIndex++];
        if(I2cSendDataIndex < I2cSendDataLength)
             // expect ACK to data byte
             TWCR = ((TWCR&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWEA));
         else
            // expect NACK to data byte
             TWCR = ((TWCR&TWCR_CMD_MASK)|_BV(TWINT));
         break;
    case TW_ST_DATA_NACK:               // 0xC0: data byte has been transmitted, NACK has been received
     case TW_ST_LAST_DATA:               // 0xC8:
         // all done
         // switch to open slave
         TWCR = ((TWCR&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWEA));
         // set state
         I2cState = I2C_IDLE;
         break;
 
    // Misc
     case TW_NO_INFO:                    // 0xF8: No relevant state information
         // do nothing
         break;
     case TW_BUS_ERROR:                  // 0x00: Bus error due to illegal start or stop condition
         // reset internal hardware and release bus
         TWCR = ((TWCR&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWSTO)|_BV(TWEA));
         // set state
         I2cState = I2C_IDLE;
         break;
     }
     //outb(PORTB, ~I2cState);
 }

int main(void) {
	uint8_t cmd = 0x50;
	
	uint8_t data[3];
	initbootuart();
  i2cInit();
	
  _delay_ms(1000);

	for(;;){
	
	i2cMasterSend(238,1, &cmd);
	_delay_ms(100);
	cmd= 0;
	i2cMasterSend(238,1, &cmd);
	cmd= 0x50;
	i2cMasterReceive(238,3, data);
	 
	sendchar(data[0]);
	sendchar(data[1]);
	sendchar(data[2]);
	}
	 return 0;
}
