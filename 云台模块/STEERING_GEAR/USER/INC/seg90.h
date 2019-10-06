/***************************************************************
  *	Name		:	MAIN.C
  *	Data		:	2019.7.15
  *	Author	:	NIEAOWEI
  *	Note		:	
****************************************************************/
#ifndef __SEG90_H
#define __SEG90_H

#include "stdint.h"
#define TIM3_SRC_CLK		3000000
#define TIM3_PWM_OUT_FREQ	50
#define	TIM3_ARR			(TIM3_SRC_CLK/TIM3_PWM_OUT_FREQ)

#define TIM3_CCR_MIN		1500
#define TIM3_CCR_MAX		7500

#define SERVO_DEGREE_MIN		0
#define SERVO_DEGREE_MAX		180


void TIM3_pwm_init(void);

/*
注意: 用此函数控制舵机0时，角度只能在0度到130度，否则会烧毁舵机!!!!
	  舵机1角度可以在0度到180度
*/
void TIM3_CH1_set_servo_degree(unsigned char degree);
void TIM3_CH2_set_servo_degree(unsigned char degree);
void TIM3_CH3_set_servo_degree(unsigned char degree);
void TIM3_CH4_set_servo_degree(unsigned char degree);


#endif

