/*
 * twi.h
 *
 * Created: 7/03/2020 1:30:59 AM
 *  Author: eugen
 */ 


#ifndef TWI_H_
#define TWI_H_

#define TWI_FAST_MODE

#ifdef TWI_FAST_MODE               // TWI FAST mode timing limits. SCL = 100-400kHz
	#define T2_TWI    ((SYS_CLK *1300) /1000000) +1 // >1,3us
	#define T4_TWI    ((SYS_CLK * 600) /1000000) +1 // >0,6us
#else                              // TWI STANDARD mode timing limits. SCL <= 100kHz
	#define T2_TWI    ((SYS_CLK *4700) /1000000) +1 // >4,7us
	#define T4_TWI    ((SYS_CLK *4000) /1000000) +1 // >4,0us
#endif

#define DDR_USI             DDRA
#define PORT_USI            PORTA
#define PIN_USI             PINA
#define PORT_USI_SDA        PINA6
#define PORT_USI_SCL        PINA4
#define PIN_USI_SDA        PINA6
#define PIN_USI_SCL        PINA4



#define TWI_READ_BIT 0x01
#define TWI_NACK_BIT  0       // Bit position for (N)ACK bit.

#endif /* TWI_H_ */

