
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
#include "digital_tube.h"

int main(){
	Byte temp[20]={LED_1|B_dot,2,3};
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	Usart1_Init(115200);
	delay_init();
	HC595_GpioInit();
	set_led_dispbuf(temp,1,3);
	while(1){
		led_scan();
		delay_ms(1);
	}
}


