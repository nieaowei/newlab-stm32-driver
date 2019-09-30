/***************************************************************
  *	Name		:	MAIN.C
  *	Data		:	2019.7.8
  *	Author	:	NIEAOWEI
  *	Note		:	本实验是通过IO口控制继电器，继电器原理就是小电流
							控制磁吸开关的闭合。实质上就是简单的IO口输出实验。
****************************************************************/
#include "relay.h"
#include "delay.h"

int main(){

	delay_init();
	Relay_Init();
	
	while(1)
	{
		delay_ms(250);
		Relay_ON();
		delay_ms(250);
		Relay_OFF();
	}
}

