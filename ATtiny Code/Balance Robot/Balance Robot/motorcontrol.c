/*
 * motorcontrol.c
 *
 * Created: 7/03/2020 3:33:17 PM
 *  Author: eugen
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "motorcontrol.h"

//static Motor motor = {0};
/**
 * Set up timer0 for using the PWM for motors
 */
void setup_motorcntl(void) {
	TCCR0A = (1 << COM0A1)|(1 << WGM00); //PWM Phase Correct
	TCCR0B = (1 << CS01); //Prescaler by 8 (4.9kHz period)
	TCNT0 = 0x00; //Reset timer
	OCR0A = 0x00; //Set PWM wave to 0
	OCR0B = 0x00;
}

void set_motor(uint8_t power, uint8_t direction) {
	if (direction == 0) {
		TCCR0A = (1 << COM0A1) | (1 << WGM00);
		OCR0A = power;
	} else {
		TCCR0A = (1 << COM0B1) | (1 << WGM00);
		OCR0B = power;
	}					
}

