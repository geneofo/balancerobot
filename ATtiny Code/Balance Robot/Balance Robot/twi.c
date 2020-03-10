/*
 * twi.c
 *
 * Created: 7/03/2020 1:24:17 AM
 *  Author: eugen
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "twi.h"

void USI_TWI_Master_Initialise(void) {
	PORT_USI |= (1<<PORT_USI_SDA);  // Enable pullup on SDA, to set high as released state.
	PORT_USI |= (1<<PORT_USI_SCL);  // Enable pullup on SCL, to set high as released state.
	
	DDR_USI |= (1<<PORT_USI_SCL);   // Enable SCL as output.
	DDR_USI |= (1<<PORT_USI_SDA);   // Enable SDA as output.
	
	USIDR = 0xFF;						// Preload dataregister with "released level" data.
	USICR = (0<<USISIE)|(0<<USIOIE)|    // Disable Interrupts.
			(1<<USIWM1)|(0<<USIWM0)|    // Set USI in Two-wire mode.
			(1<<USICS1)|(0<<USICS0)|(1<<USICLK)| // Software stobe as counter clock source
			(0<<USITC);
	USISR = (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)| // Clear flags,
				(0x0<<USICNT0);                            // and reset counter.
	return;
}

unsigned char USI_TWI_Start_Transceiver_With_Data(unsigned char *msg, 
													unsigned char msgSize) {
	//Prepare register value to: Clear flags, and set USI to shift 8 bits 
	// i.e. count 16 clock edges.
	unsigned char tempUSISR_8bit = (1<<USISIF)|(1<<USIOIF)|
	(1<<USIPF)|(1<<USIDC)|(0x0<<USICNT0);      
	//Prepare register value to: Clear flags, and set USI to shift 1 bit 
	// i.e. count 2 clock edges.
	unsigned char tempUSISR_1bit = (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|      
										(0xE<<USICNT0);

	uint8_t addressMode = 1;
	uint8_t masterWriteDataMode = 0;

	if (!(*msg & (1<<TWI_READ_BIT))) { 
		//The LSB in the address byte determines if is a 
		// masterRead or masterWrite operation.
		masterWriteDataMode = 1;
	}

	/* Release SCL to ensure that (repeated) Start can be performed */
	PORT_USI |= (1<<PIN_USI_SCL);                     // Release SCL.
	while( !(PIN_USI & (1<<PIN_USI_SCL)) );          // Verify that SCL becomes high.
	#ifdef TWI_FAST_MODE
	_delay_us(T4_TWI/4);                         // Delay for T4TWI if TWI_FAST_MODE
	#else
	_delay_us(T2_TWI/4);                         // Delay for T2TWI if TWI_STANDARD_MODE
	#endif

	/* Generate Start Condition */
	PORT_USI &= ~(1<<PIN_USI_SDA);                    // Force SDA LOW.
	_delay_us(T4_TWI/4);
	PORT_USI &= ~(1<<PIN_USI_SCL);                    // Pull SCL LOW.
	PORT_USI |= (1<<PIN_USI_SDA);                     // Release SDA.

	/*Write address and Read/Write data */
	do {
		/* If masterWrite cycle (or inital address tranmission)*/
		if (addressMode || masterWriteDataMode) {
			/* Write a byte */
			PORT_USI &= ~(1<<PIN_USI_SCL);                // Pull SCL LOW.
			USIDR = *(msg++);                        // Setup data.
			USI_TWI_Master_Transfer(tempUSISR_8bit);    // Send 8 bits on bus.
			
			/* Clock and verify (N)ACK from slave */
			DDR_USI  &= ~(1<<PIN_USI_SDA);                // Enable SDA as input.
			if( USI_TWI_Master_Transfer(tempUSISR_1bit) & (1<<TWI_NACK_BIT)) {
				return 0;
			}
			addressMode = 0; // Only perform address transmission once.
		}
		/* Else masterRead cycle*/
		else {
			/* Read a data byte */
			DDR_USI   &= ~(1<<PIN_USI_SDA);               // Enable SDA as input.
			*(msg++) = USI_TWI_Master_Transfer(tempUSISR_8bit);

			/* Prepare to generate ACK (or NACK in case of End Of Transmission) */
			if( msgSize == 1) {    // If transmission of last byte was performed.
				USIDR = 0xFF;      // Load NACK to confirm End Of Transmission.
			} else {
				USIDR = 0x00;   // Load ACK. Set data register bit 7 (output for SDA) low.
			}
			USI_TWI_Master_Transfer(tempUSISR_1bit);   // Generate ACK/NACK.
		}
	} while(--msgSize) ;     // Until all data sent/received.
	
	USI_TWI_Master_Stop();    // Send a STOP condition on the TWI bus.

	/* Transmission successfully completed*/
}

/*---------------------------------------------------------------
 Core function for shifting data in and out from the USI.
 Data to be sent has to be placed into the USIDR prior to calling
 this function. Data read, will be return'ed from the function.
---------------------------------------------------------------*/
unsigned char USI_TWI_Master_Transfer(unsigned char temp) {
	USISR = temp;                                     // Set USISR according to temp.
													// Prepare clocking.
	temp  =  (0<<USISIE)|(0<<USIOIE)|                 // Interrupts disabled
			(1<<USIWM1)|(0<<USIWM0)|                 // Set USI in Two-wire mode.
			(1<<USICS1)|(0<<USICS0)|(1<<USICLK)|     // Software clock strobe as source.
			(1<<USITC);                              // Toggle Clock Port.
	do {
		_delay_us(T2_TWI/4);
		USICR = temp;                        // Generate positve SCL edge.
		while( !(PIN_USI & (1<<PIN_USI_SCL)) );// Wait for SCL to go high.
		_delay_us(T4_TWI/4);              
		USICR = temp;                          // Generate negative SCL edge.
	} while(!(USISR & (1<<USIOIF)));        // Check for transfer complete.
	_delay_us(T2_TWI/4);                
	temp  = USIDR;                           // Read out data.
	USIDR = 0xFF;                            // Release SDA.
	DDR_USI |= (1<<PIN_USI_SDA);             // Enable SDA as output.
	return temp;                             // Return the data from the USIDR
}

/*---------------------------------------------------------------
 Function for generating a TWI Stop Condition. Used to release 
 the TWI bus.
---------------------------------------------------------------*/
unsigned char USI_TWI_Master_Stop(void)
{
  PORT_USI &= ~(1<<PIN_USI_SDA);           // Pull SDA low.
  PORT_USI |= (1<<PIN_USI_SCL);            // Release SCL.
  while(!(PIN_USI & (1<<PIN_USI_SCL)));  // Wait for SCL to go high.
  _delay_us(T4_TWI/4);               
  PORT_USI |= (1<<PIN_USI_SDA);            // Release SDA.
  _delay_us(T2_TWI/4);                
  return 1;
}
