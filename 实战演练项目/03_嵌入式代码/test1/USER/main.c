
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
#include "WiFiToCloud.h"
#include "lcd12864.h"
#include "rtc.h"
#include "digital_tube.h"
#include "relay.h"
#include "temp_light.h"
#include "key.h"

#define default_time 2019,1,14,17,42,55,1

void connect_cloud_process();
void lcd_display_process();
void digital_tube_process();
void rtc_process();
void door_process();

int main(){
	Byte temp[20];
	uint8_t key;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	Usart1_Init(115200);
	
	delay_init();
	connect_cloud_process();
	TEMP_LIGHT_Init();
	RTC_Init(default_time);
	HC595_GpioInit();
	lcd_init();
	
	
	lcd_display_process();
	digital_tube_process();
	

	
	door_process();
	
	//rtc_process();
	
	while(1){
		digital_tube_process();
		key = Key_Scan();
		if(key==KEY1_PRESSED){
			if(door_status==0){
				door_ON();
				
			}else{
				door_OFF();
			}
			ESP8266_SendSensor("door_status",door_status,"");
		}
		if(key==KEY2_PRESSED){
			if(latern_status==0){
				door_ON();
				latern_ON();
			}else{
				door_OFF();
				latern_OFF();
			}
		}
	}
}

void connect_cloud_process(){
	uint8_t ack;
	UART4_Init(115200);
	ack=ConnectToServer((char *)MY_DEVICE_ID, (char *)MY_SECRET_KEY);
	printf("wifi ack %d \r\n",ack);
}

void lcd_display_process(){
	
	//show_logo();
}

void digital_tube_process(){
	uint8_t date[8]={LED_2,LED_0,LED_1,LED_9|B_dot,LED_1,LED_0|B_dot,LED_0,calendar.sec%10};

	set_led_dispbuf(date,0,8);
	led_scan();
}

void rtc_process(){
	delay_ms(1000);
	RTC_Init(2029,1,14,17,42,55,1);
	printf("%d-%d-%d \r\n",calendar.w_year,calendar.w_month,calendar.w_date);
}

void door_process(){
	Relay_Init();
	door_ON();
	latern_OFF();
}


