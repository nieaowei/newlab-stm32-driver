
/***************************************************************
  *	Name		:	MAIN.C
  *	Data		:	2019.7.15
  *	Author	:	NIEAOWEI
  *	Note		:	温度光照采集实验，使用adc1；
****************************************************************/
#include "usart1.h"
#include "delay.h"
#include <stdio.h>
#include "temp_light.h"

int main(){
	Byte temp[20];
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	Usart1_Init(115200);
	delay_init();
	TEMP_LIGHT_Init();
	printf("runnning...\r\n");
	while(1){
		printf("temp:%d\r\n",GetTemp());
		printf("light:%d\r\n",GetLight());
		delay_ms(1000);
	}
}


