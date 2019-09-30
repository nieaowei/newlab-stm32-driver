/***************************************************************
  *	Name		:	MAIN.C
  *	Data		:	2019.7.9
  *	Author	:	NIEAOWEI
  *	Note		:	验证独立看门狗的使用
****************************************************************/
#include "beep.h"
#include "delay.h"
#include "iwdg.h"

int main(){
	delay_init();
	BEEP_Init();
	IWDG_Init();
	BEEP_Play(200);
	
	while(1){
		delay_ms(1000);
		IWDG_Feed();
		BEEP_Play(200);
	}
}
