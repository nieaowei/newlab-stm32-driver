/***************************************************************
  *	Name		:	MAIN.C
  *	Data		:	2019.7.25
  *	Author	:	NIEAOWEI
  *	Note		:			本实验实现了T5557卡片的读写，已经lf125k模块的操
							作。切记不理解的情况下不可修改写的函数，否则可能会
							写坏卡片。
									本实验对时序要求非常高，几微秒都会影响读写操作，
							所以专门采用了定时器6来计时，请记住延时函数不能满
							足要求
****************************************************************/
#include "usart1.h"
#include "delay.h"
#include <stdio.h>
#include "lf125k.h"
#include "timer.h"
int main(){
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	Usart1_Init(115200);
	delay_init();
	TIM6_Init();
	LF125K_init();
	printf("running...\r\n");
	while(1){
		delay_ms(1500);
		LF125K_demo();
	}

}

