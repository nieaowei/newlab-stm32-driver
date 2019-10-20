
/***************************************************************
  *	Name		:	MAIN.C
  *	Data		:	2019.7.15
  *	Author	:	NIEAOWEI
  *	Note		:	串口1的使用，采用队列缓存接收数据，保证所有数据
							都能被处理到位。
****************************************************************/
#include "usart1.h"
#include "delay.h"
#include <stdio.h>
#include "extend_rtc.h"


int main(){
	Byte temp[7];
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	Usart1_Init(115200);
	delay_init();
	PCF8563_Init();
	printf("running...\r\n");
	while(1){
		PCF8563_GetTime(temp);
		printf("%x %x %x %x %x %x %x\r\n",temp[0],temp[1],temp[2],temp[3],temp[4],temp[5],temp[6]);
		delay_ms(1000);
	}
}


