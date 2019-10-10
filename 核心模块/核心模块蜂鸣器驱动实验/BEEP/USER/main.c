/***************************************************************
  *	Name		:	MAIN.C
  *	Data		:	2019.7.8
  *	Author	:	NIEAOWEI
  *	Note		:	无源蜂鸣器驱动，利用延时函数对相关IO口产生一定频率
							的方波，达到蜂鸣器内振膜震动发声的效果。
****************************************************************/
#include "beep.h"
#include "delay.h"

int main(){
	delay_init();
	BEEP_Init();
	
	while(1){
		BEEP_Play(200);
		delay_ms(1000);
	}
}
