/*****************************************************************************
*
* Atmel Corporation
*
* File              : main.c
* Compiler          : IAR C 3.10C Kickstart, AVR-GCC/avr-libc(>= 1.2.5)
* Revision          : $Revision: 1.8 $
* Date              : $Date: 2010/11/19 22:43:58 $
* Updated by        : $Author: szabomeister $
*
* Support mail      : avr@atmel.com
*
* Target platform   : All AVRs with bootloader support
*
* AppNote           : AVR109 - Self-programming
*
* Description   : This Program allows an AVR with bootloader capabilities to 
*                 Read/write its own Flash/EEprom. To enter Programming mode   
*                 an input pin is checked. If this pin is pulled low, programming mode  
*                 is entered. If not, normal execution is done from $0000 
*                 "reset" vector in Application area.
*
* Preparations  : Use the preprocessor.xls file for obtaining a customized
*                 defines.h file and linker-file code-segment definition for
*                 the device you are compiling for.
****************************************************************************/
#ifdef _ATMEGA1281
  #define F_CPU 8000000
#else
  #define F_CPU 16000000
#endif

#include "defines.h"
#include "serial.h"
#include "flash.h"
#include "util/delay.h"
#include <avr/wdt.h>


/* Uncomment the following to save code space */
//#define REMOVE_AVRPROG_SUPPORT
//#define REMOVE_FUSE_AND_LOCK_BIT_SUPPORT
//#define REMOVE_BLOCK_SUPPORT
#define REMOVE_EEPROM_BYTE_SUPPORT
#define REMOVE_FLASH_BYTE_SUPPORT

/*
 * GCC doesn't optimize long int arithmetics very clever.  As the
 * address only needs to be larger than 16 bits for the ATmega128 and
 * above (where flash consumptions isn't much of an issue as the
 * entire boot loader will still fit even into the smallest possible
 * boot loader section), save space by using a 16-bit variable for the
 * smaller devices.
 */
#ifdef LARGE_MEMORY
#  define ADDR_T unsigned long
#else  /* !LARGE_MEMORY */
#  define ADDR_T unsigned int
#endif /* LARGE_MEMORY */

#ifndef REMOVE_BLOCK_SUPPORT
unsigned char BlockLoad(unsigned int size, unsigned char mem, ADDR_T *address);
void BlockRead(unsigned int size, unsigned char mem, ADDR_T *address);
void status(int);

/* BLOCKSIZE should be chosen so that the following holds: BLOCKSIZE*n = PAGESIZE,  where n=1,2,3... */
#define BLOCKSIZE PAGESIZE

#endif /* REMOVE_BLOCK_SUPPORT */
#define TIMEOUT 30
int timeout=TIMEOUT;

void exitbl(void){
	void (*funcptr)( void ) = 0x0000; // Set up function pointer to RESET vector.
	int i;
	#ifdef _ATMEGA1281
	PORTA&=~((1<<0)|(1<<1)|(1<<2));
	#else
	PORTE&=~((1<<3)|(1<<5)|(1<<6)|(1<<7));
	#endif
	for(i=0;i<3;i++){
	  _delay_ms(100);
	  #ifdef _ATMEGA1281
	  PORTA|=(1<<0)|(1<<1)|(1<<2);
	  _delay_ms(100);
	  PORTA&=~((1<<0)|(1<<1)|(1<<2));
	  #else
	  PORTE|=(1<<3)|(1<<5)|(1<<6)|(1<<7);
	  _delay_ms(100);
	  PORTE&=~((1<<3)|(1<<5)|(1<<6)|(1<<7));
	  #endif
     }
     funcptr();
}

int main(void)
{
    ADDR_T address;
    
    unsigned int temp_int=0;
    unsigned char val;
    
    /* Initialization */   
    MCUSR = 0;
    wdt_disable();
    #ifdef _ATMEGA1281
    DDRA |= _BV(2);
    DDRA |= _BV(1);
    DDRA |= _BV(0);
    PORTA = 7;
    #else
    DDRE |= _BV(3);
    DDRE |= _BV(5);
    DDRE |= _BV(6);
    DDRE |= _BV(7);
    DDRE &= ~(_BV(PE4));    //make this pin input
    DDRB &= ~(_BV(PB7));    //make this pin input too
    #endif

    initbootuart(); // Initialize UART.
    blinker = 0;

//isWriting=1;
    
    /* Branch to bootloader or application code? */
    //if( !(PROGPIN & (1<<PROG_NO)) ) // If PROGPIN is pulled low, enter programmingmode.
//	for baudrate test
// 	for(;;)
// 	{
// 	  val=recchar();
// 	  if(val!=255)
// 	    sendchar(val);
// 	}
	for(;;)
        {
            val=recchar(); // Wait for command character.
			status(timeout);
            // Check autoincrement status.
            if(val=='a')
            {
                sendchar('Y'); // Yes, we do autoincrement.
                //_WAIT_FOR_SPM();        
                //boot_lock_bits_set(0xff); // Read and set lock bits.

                ////sendchar('\r'); // Send OK back.
 
            }


            // Set address.
            else if(val=='A') // Set address...
            { // NOTE: Flash addresses are given in words, not bytes.                                            
                address=(recchar()<<8) | recchar(); // Read address high and low byte.
                sendchar('\r'); // Send OK back.
            }

            
            // Chip erase.
            else if(val=='e')
            {  
		#ifdef _ATMEGA1281
		PORTA = 7;
		PORTA &= ~(_BV(PA2));
		#else
		PORTE =0;
		PORTE = _BV(PE7);
		#endif

                for(address = 0; address < APP_END;address += PAGESIZE)
                { // NOTE: Here we use address as a byte-address, not word-address, for convenience.
                    _WAIT_FOR_SPM();        
                    _PAGE_ERASE( address );
                }
                _delay_ms(5);//if write the program just after we erased the flash, sometimes the first few byte is wrong
                sendchar('\r'); // Send OK back.
		#ifdef _ATMEGA1281
                PORTA = 7;
		#else
		PORTE = 0;
		#endif
		blinker = 1;
            }
            
#ifndef REMOVE_BLOCK_SUPPORT
            // Check block load support.
            else if(val=='b')
		    {
    			sendchar('Y'); // Report block load supported.
    			sendchar((BLOCKSIZE>>8) & 0xFF); // MSB first.
    			sendchar(BLOCKSIZE&0xFF); // Report BLOCKSIZE (bytes).
    		}


            // Start block load.
    		else if(val=='B')
    		{isWriting=1;
	    	    temp_int = (recchar()<<8) | recchar(); // Get block size.
		    	val = recchar(); // Get memtype.
			    sendchar( BlockLoad(temp_int,val,&address) ); // Block load.
		    }
		
		    
		    // Start block read.
    		else if(val=='g')
    		{isWriting=0;
	    	    temp_int = (recchar()<<8) | recchar(); // Get block size.
    			val = recchar(); // Get memtype
	    		BlockRead(temp_int,val,&address); // Block read
    		}		
#endif /* REMOVE_BLOCK_SUPPORT */

#ifndef REMOVE_FLASH_BYTE_SUPPORT            
            // Read program memory.
            else if(val=='R')
            {        
                // Send high byte, then low byte of flash word.
                _WAIT_FOR_SPM();        
                _ENABLE_RWW_SECTION();
                sendchar( _LOAD_PROGRAM_MEMORY( (address << 1)+1 ) );
                sendchar( _LOAD_PROGRAM_MEMORY( (address << 1)+0 ) );

                address++; // Auto-advance to next Flash word.
            }
        

            // Write program memory, low byte.        
            else if(val=='c')
            { // NOTE: Always use this command before sending high byte.
                temp_int=recchar(); // Get low byte for later _FILL_TEMP_WORD.
                sendchar('\r'); // Send OK back.
            }
            
            
            // Write program memory, high byte.
            else if(val=='C')
            {
                temp_int |= (recchar()<<8); // Get and insert high byte.
                _WAIT_FOR_SPM();
                _FILL_TEMP_WORD( (address << 1), temp_int ); // Convert word-address to byte-address and fill.
                address++; // Auto-advance to next Flash word.
                sendchar('\r'); // Send OK back.
            }
        
        
            // Write page.       
            else if(val== 'm')
            {
                if( address >= (APP_END>>1) ) // Protect bootloader area.
                {
                    sendchar('?');
                } else
                {
                    _WAIT_FOR_SPM();      
                    _PAGE_WRITE( address << 1 ); // Convert word-address to byte-address and write.
                }
                sendchar('\r'); // Send OK back.
            }
#endif /* REMOVE_FLASH_BYTE_SUPPORT */

#ifndef REMOVE_EEPROM_BYTE_SUPPORT
            // Write EEPROM memory.
            else if (val == 'D')
            {
                _WAIT_FOR_SPM();        
                EEARL = address; // Setup EEPROM address.
                EEARH = (address >> 8);
                EEDR = recchar(); // Get byte.
                EECR |= (1<<EEMPE); // Write byte.
                EECR |= (1<<EEPE);
                while (EECR & (1<<EEPE)) // Wait for write operation to finish.
                    ;
                    
                address++; // Auto-advance to next EEPROM byte.
                sendchar('\r');// Send OK back.
            }

            
            // Read EEPROM memory.
            else if (val == 'd')
            {
                EEARL = address; // Setup EEPROM address.
                EEARH = (address >> 8);
                EECR |= (1<<EERE); // Read byte...
                sendchar(EEDR); // ...and send it back.
                address++; // Auto-advance to next EEPROM byte.
            }
#endif /* REMOVE_EEPROM_BYTE_SUPPORT */

#ifndef REMOVE_FUSE_AND_LOCK_BIT_SUPPORT
            // Write lockbits.
            else if(val=='l')
            {
                _WAIT_FOR_SPM();        
                _SET_LOCK_BITS( recchar() ); // Read and set lock bits.
                sendchar('\r'); // Send OK back.
            }
               

#if defined(_GET_LOCK_BITS)
            // Read lock bits.
            else if(val=='r')
            {
                _WAIT_FOR_SPM();        
                sendchar( _GET_LOCK_BITS() );
            }


            // Read fuse bits.
            else if(val=='F')
            {
                _WAIT_FOR_SPM();        
                sendchar( _GET_LOW_FUSES() );
            }


            // Read high fuse bits.
            else if(val=='N')
            {
                _WAIT_FOR_SPM();        
                sendchar( _GET_HIGH_FUSES() );
            }


            // Read extended fuse bits.
            else if(val=='Q')
            {
                _WAIT_FOR_SPM();        
                sendchar( _GET_EXTENDED_FUSES() );
            }
#endif /* defined(_GET_LOCK_BITS) */
#endif /* REMOVE_FUSE_AND_LOCK_BIT_SUPPORT */

#ifndef REMOVE_AVRPROG_SUPPORT        
            // Enter and leave programming mode.
            else if((val=='P')||(val=='L'))
            {  
                sendchar('\r'); // Nothing special to do, just answer OK.
            }
            
            
            // Exit bootloader.
            else if(val=='E')
            {
                _WAIT_FOR_SPM();        
                _ENABLE_RWW_SECTION();
                sendchar('\r');
                exitbl(); // Jump to Reset vector 0x0000 in Application Section.
            }

    
            // Get programmer type.        
            else if (val=='p')
            {
                sendchar('S'); // Answer 'SERIAL'.
            }
            
            
            // Return supported device codes.
            else if(val=='t')
            {
#if PARTCODE+0 > 0
                sendchar( PARTCODE ); // Supports only this device, of course.
#endif /* PARTCODE */
                sendchar( 0 ); // Send list terminator.
            }
            
            
            // Set LED, clear LED and set device type.
            else if((val=='x')||(val=='y')||(val=='T'))
            {
                recchar(); // Ignore the command and it's parameter.
                sendchar('\r'); // Send OK back.
            }
#endif /* REMOVE_AVRPROG_SUPPORT */
       
            // Return programmer identifier.
            else if(val=='S')
            { 
                sendchar('A'); // Return 'AVRBOOT'.
                sendchar('V'); // Software identifier (aka programmer signature) is always 7 characters.
                sendchar('R');
                sendchar('B');
                sendchar('O');
                sendchar('O');
                sendchar('T');
            }
        
            
            // Return software version.
            else if(val=='V')
            {
                sendchar('1');
                sendchar('5');
            }        


            // Return signature bytes.
            else if(val=='s')
            {							
                sendchar( SIGNATURE_BYTE_3 );
                sendchar( SIGNATURE_BYTE_2 );
                sendchar( SIGNATURE_BYTE_1 );
            }       


            else if (val==255)
            {
              if (timeout==0)
              {
                _WAIT_FOR_SPM();        
                _ENABLE_RWW_SECTION();
                exitbl(); // Jump to Reset vector 0x0000 in Application Section.
              }
              else
              {
                timeout--;
		if((timeout&0x03)==0)
		  sendchar('?');
              }
            }

            // The last command to accept is ESC (synchronization).
            else if(val!=0x1b)                  // If not ESC, then it is unrecognized...
            {
                sendchar('?');
            }
        } // end: for(;;)
} // end: main


void status(int time_out)

{
	#ifdef _ATMEGA1281
	if(time_out>2*TIMEOUT/3){
	  PORTA|=(1<<0)|(1<<1)|(1<<2);
	}else if(time_out>TIMEOUT/3){
	  PORTA|=(1<<1)|(1<<2);
	  PORTA&=~(1<<0);
	}else{
	  PORTA|=(1<<2);
	  PORTA&=~((1<<0)|(1<<1));
	}
	#else
	if(time_out>3*TIMEOUT/4){
	  PORTE|=(1<<3)|(1<<5)|(1<<6)|(1<<7);
	}else if(time_out>2*TIMEOUT/4){
	  PORTE|=(1<<5)|(1<<6)|(1<<7);
	  PORTE&=~(1<<3);
	}else if(time_out>TIMEOUT/4){
	  PORTE|=(1<<5)|(1<<6);
	  PORTE&=~((1<<3)|(1<<7));
	}else{
	  PORTE|=(1<<5);
	  PORTE&=~((1<<3)|(1<<6)|(1<<7));
	}
	#endif
}

#ifndef REMOVE_BLOCK_SUPPORT
unsigned char BlockLoad(unsigned int size, unsigned char mem, ADDR_T *address)
{
    unsigned char buffer[BLOCKSIZE];
    unsigned int data;
    ADDR_T tempaddress;
	
    // EEPROM memory type.
    if(mem=='E')
    {
        /* Fill buffer first, as EEPROM is too slow to copy with UART speed */
        for(tempaddress=0;tempaddress<size;tempaddress++)
            buffer[tempaddress] = recchar();
        
        /* Then program the EEPROM */
        _WAIT_FOR_SPM();
    	for( tempaddress=0; tempaddress < size; tempaddress++)
    	{
	        EEARL = *address; // Setup EEPROM address
            EEARH = ((*address) >> 8);
            EEDR = buffer[tempaddress]; // Get byte.
            EECR |= (1<<EEMPE); // Write byte.
            EECR |= (1<<EEPE);
            while (EECR & (1<<EEPE)) // Wait for write operation to finish.
                ;

  			(*address)++; // Select next EEPROM byte
        }

        return '\r'; // Report programming OK
    } 
    
    // Flash memory type.
	else if(mem=='F')
    { // NOTE: For flash programming, 'address' is given in words.

        (*address) <<= 1; // Convert address to bytes temporarily.
        tempaddress = (*address);  // Store address in page.
	
        do
		{
            data = recchar();
            data |= (recchar() << 8);
            _FILL_TEMP_WORD(*address,data);
            (*address)+=2; // Select next word in memory.
            size -= 2; // Reduce number of bytes to write by two.
        } while(size); // Loop until all bytes written.
		//bootloader protection.
		if(((*address)>>1)>APP_END)
			return '?';

		_PAGE_WRITE(tempaddress);
		_WAIT_FOR_SPM();
		_ENABLE_RWW_SECTION();

        (*address) >>= 1; // Convert address back to Flash words again.
        return '\r'; // Report programming OK
    }
    
    // Invalid memory type?
    else
    {
        return '?';
    }
}


void BlockRead(unsigned int size, unsigned char mem, ADDR_T *address)
{
    // EEPROM memory type.
    if (mem=='E') // Read EEPROM
    {
        do
        {
            EEARL = *address; // Setup EEPROM address
            EEARH = ((*address) >> 8);
            (*address)++; // Select next EEPROM byte
            EECR |= (1<<EERE); // Read EEPROM
            sendchar(EEDR); // Transmit EEPROM dat ato PC

            size--; // Decrease number of bytes to read
        } while (size); // Repeat until all block has been read
    }
    
    // Flash memory type.
	else if(mem=='F')
	{
        (*address) <<= 1; // Convert address to bytes temporarily.
	
        do
        {
            sendchar( _LOAD_PROGRAM_MEMORY(*address) );
            sendchar( _LOAD_PROGRAM_MEMORY((*address)+1) );
            (*address) += 2; // Select next word in memory.
            size -= 2; // Subtract two bytes from number of bytes to read
        } while (size); // Repeat until all block has been read

        (*address) >>= 1; // Convert address back to Flash words again.
    }
}
#endif /* REMOVE_BLOCK_SUPPORT */


/* end of file */
