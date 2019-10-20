
/***************************************************************
  *	Name		:	MAIN.C
  *	Data		:	2019.7.15
  *	Author	:	NIEAOWEI
  *	Note		:	
****************************************************************/
#include "usart1.h"
#include "delay.h"
#include <stdio.h>
#include "seg90.h"

int main(){
	Byte temp[20];
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	Usart1_Init(115200);
	delay_init();
	TIM3_pwm_init();
	printf("running...\r\n");
	//TIM3_CH1_set_servo_degree(30);
	delay_ms(1000);
	TIM3_CH1_set_servo_degree(20);
	while(1){
		
		delay_ms(1000);
	}
}


