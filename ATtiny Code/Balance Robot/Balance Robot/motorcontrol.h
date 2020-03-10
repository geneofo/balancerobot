/*
 * motorcontrol.h
 *
 * Created: 7/03/2020 9:34:00 PM
 *  Author: eugen
 */ 


#ifndef MOTORCONTROL_H_
#define MOTORCONTROL_H_

	struct Motor{
		uint8_t rotDirection;
	};

	
	
	/**
	 * Set up timer0 for using the PWM for motors
	 */
	void setup_motorcntl(void);

	void set_motor(uint8_t power, uint8_t direction);


#endif /* MOTORCONTROL_H_ */