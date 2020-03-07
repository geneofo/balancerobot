/*
 * twi.c
 *
 * Created: 7/03/2020 1:24:17 AM
 *  Author: eugen
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "twi.h"

uint8_t twi_setup(void) {
	PORT_USI |= (1<<PORT_USI_SDA);           // Enable pullup on SDA, to set high as released state.
	PORT_USI |= (1<<PORT_USI_SCL);           // Enable pullup on SCL, to set high as released state.
	
	DDR_USI |= (1<<PORT_USI_SCL);           // Enable SCL as output.
	DDR_USI |= (1<<PORT_USI_SDA);           // Enable SDA as output.
	
	USIDR = 0xFF;									// Preload dataregister with "released level" data.
	USICR = (0<<USISIE)|(0<<USIOIE)|                // Disable Interrupts.
			(1<<USIWM1)|(0<<USIWM0)|                // Set USI in Two-wire mode.
			(1<<USICS1)|(0<<USICS0)|(1<<USICLK)|    // Software stobe as counter clock source
			(0<<USITC);
	USISR = (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|      // Clear flags,
	(0x0<<USICNT0);                                     // and reset counter.
}