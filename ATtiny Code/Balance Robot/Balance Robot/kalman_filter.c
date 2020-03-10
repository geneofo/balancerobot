/*
 * kalman_filter.c
 *
 * Created: 7/03/2020 10:21:50 PM
 *  Author: eugen
 */ 

#include <avr/io.h>
#include <stdio.h>

#include <stdfix.h>
#include "kalman_filter.h"

struct KalmanFilt {
	signed accum K;
	signed accum sampT;
	signed accum x_k;
	signed accum v_k;
};

void kal_next(KalmanFilt filt, signed accum Y_pos, signed accum Y_vel) {
	filt.x_k = (1 - filt.K) * filt.x_k + filt.sampT * filt.v_k + filt.K * Y_pos;
	//Note can replace x_k because it doesn't change x_k
	filt.v_k = (1 - filt.K) * filt.v_k + filt.K * Y_vel;
	return
}


