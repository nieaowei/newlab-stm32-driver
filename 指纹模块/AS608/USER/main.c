
/***************************************************************
  *	Name		:	MAIN.C
  *	Data		:	2019.8.2
  *	Author	:	NIEAOWEI
  *	Note		:	该实验为AS608指纹识别芯片的驱动实验，使用串口5作为
							通信端口。
****************************************************************/
#include "usart1.h"
#include "uart5.h"
#include "delay.h"
#include <stdio.h>
#include "as608.h"

int main(){
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	Usart1_Init(115200);
	UART5_Init(57600);
	delay_init();
	delay_ms(100);
	printf("running.\r\n");
	AS608_Init();
	AS608_Demo();
	while(1){
		
	}
}


