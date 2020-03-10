/*
 * kalman_filter.h
 *
 * Created: 10/03/2020 9:52:16 PM
 *  Author: eugen
 */ 


#ifndef KALMAN_FILTER_H_
#define KALMAN_FILTER_H_

struct KalmanFilt {
	signed accum K;
	signed accum sampT;
	signed accum x_k;
	signed accum v_k;
};

void kal_next(KalmanFilt filt, signed accum Y_pos, signed accum Y_vel);

#endif /* KALMAN_FILTER_H_ */