/***************************************************************
  *	Name		:	MAIN.C
  *	Data		:	2019.7.10
  *	Author	:	NIEAOWEI
  *	Note		:	该实验为IO输入实验，提供了延时消抖方案和状态机消抖
							方案。
							本实验效果为按下1键，蜂鸣器鸣叫，2键继电器开关翻转
****************************************************************/
#include "delay.h"
#include "key.h"
#include "beep.h"
#include "relay.h"


int main(){
	uint8_t key;
	delay_init();
	KEY_Init();
	Relay_Init();
	BEEP_Init();
	BEEP_Play(200);
	
	while(1){
		key=Key_Scan();
		if(key==KEY1_PRESSED){
			BEEP_Play(200);
		}
		if(key==KEY2_PRESSED){
			Relay_Toggle();
		}
	}
}

