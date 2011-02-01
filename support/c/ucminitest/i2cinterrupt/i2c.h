 #ifndef I2C_H
 #define I2C_H
 #define I2C_SEND_DATA_BUFFER_SIZE       0x20
 #define I2C_RECEIVE_DATA_BUFFER_SIZE    0x20
#define TRUE 1
#define FALSE 0
 
 #include <util/twi.h>
 //#include "global.h"
 
 // include project-specific configuration
 //#include "i2cconf.h"
 
 // defines and constants
 #define TWCR_CMD_MASK       0x0F
 #define TWSR_STATUS_MASK    0xF8
 
 // types
 typedef enum
 {
     I2C_IDLE = 0, I2C_BUSY = 1,
     I2C_MASTER_TX = 2, I2C_MASTER_RX = 3,
     I2C_SLAVE_TX = 4, I2C_SLAVE_RX = 5
 } eI2cStateType;
 
 // functions
 
 //! Initialize I2C (TWI) interface
 void i2cInit(void);
 
 //! Set the I2C transaction bitrate (in KHz)
 void i2cSetBitrate(uint16_t bitrateKHz);
 
 // I2C setup and configurations commands
 //! Set the local (AVR processor's) I2C device address
 void i2cSetLocalDeviceAddr(uint8_t deviceAddr, uint8_t genCallEn);
 
 //! Set the user function which handles receiving (incoming) data as a slave
 void i2cSetSlaveReceiveHandler(void (*i2cSlaveRx_func)(uint8_t receiveDataLength, uint8_t* recieveData));
 //! Set the user function which handles transmitting (outgoing) data as a slave
 void i2cSetSlaveTransmitHandler(uint8_t (*i2cSlaveTx_func)(uint8_t transmitDataLengthMax, uint8_t* transmitData));
 
 // Low-level I2C transaction commands 
 //! Send an I2C start condition in Master mode
 void i2cSendStart(void);
 //! Send an I2C stop condition in Master mode
 void i2cSendStop(void);
 //! Wait for current I2C operation to complete
 void i2cWaitForComplete(void);
 //! Send an (address|R/W) combination or a data byte over I2C
 void i2cSendByte(uint8_t data);
 //! Receive a data byte over I2C  
 // ackFlag = TRUE if recevied data should be ACK'ed
 // ackFlag = FALSE if recevied data should be NACK'ed
 void i2cReceiveByte(uint8_t ackFlag);
 //! Pick up the data that was received with i2cReceiveByte()
 uint8_t i2cGetReceivedByte(void);
 //! Get current I2c bus status from TWSR
 uint8_t i2cGetStatus(void);
 
 // high-level I2C transaction commands
 
 //! send I2C data to a device on the bus
 void i2cMasterSend(uint8_t deviceAddr, uint8_t length, uint8_t *data);
 //! receive I2C data from a device on the bus
 void i2cMasterReceive(uint8_t deviceAddr, uint8_t length, uint8_t* data);
 
 //! send I2C data to a device on the bus (non-interrupt based)
 void i2cMasterSendNI(uint8_t deviceAddr, uint8_t length, uint8_t* data);
 //! receive I2C data from a device on the bus (non-interrupt based)
 void i2cMasterReceiveNI(uint8_t deviceAddr, uint8_t length, uint8_t *data);
 
 #endif
