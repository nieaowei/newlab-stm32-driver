
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
#include "uart4.h"
#include "WiFiToCloud.h"
int main(){
	Byte temp[128];
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	delay_init();
	Usart1_Init(115200);
	
	uart4_init();
	//delay_ms(1000);
	//ESP8266_SetStation();
	//ESP8266_SetStation();
	ConnectToServer(MY_DEVICE_ID,MY_SECRET_KEY);
	//ESP8266_SetAP((char *)WIFI_AP, (char *)WIFI_PWD);
	while(1){
		FIFO_Out_Bytes(&uart4_rx_fifo,temp);
		printf("main:%s\r\n",temp);
		ESP8266_DataAnalysisProcess(temp);
		memset(temp,0,128);
		delay_ms(1000);
	}
}


