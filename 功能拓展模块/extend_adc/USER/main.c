
/***************************************************************
  *	Name		:	MAIN.C
  *	Data		:	2019.10.8
  *	Author	:	NIEAOWEI
  *	Note		:	拓展模块上的adc的使用。
****************************************************************/
#include "usart1.h"
#include "delay.h"
#include <stdio.h>
#include "extend_adc.h"

int main(){
	Byte temp[20];
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	Usart1_Init(115200);
	delay_init();
	printf("running...\r\n");
	PCF8591_Init(1);
	PCF8591_ReadChNd(&temp[0]);
	while(1){
		PCF8591_Init(1);
	PCF8591_ReadChNd(&temp[0]);
		delay_ms(1000);
	}
}


