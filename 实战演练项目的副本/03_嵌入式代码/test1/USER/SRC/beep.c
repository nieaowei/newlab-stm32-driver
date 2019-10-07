/***************************************************************
  *	Name		:	BEEP.C
  *	Data		:	2019.7.8
  *	Author	:	NIEAOWEI
  *	Note		:	无源蜂鸣器驱动，利用延时函数对相关IO口产生一定频率
							的方波，达到蜂鸣器内振膜震动发声的效果。
****************************************************************/
#include "beep.h"
#include "delay.h"
//蜂鸣器相关IO口配置
#define BEEP_PORT 	GPIOA
#define BEEP_PIN 		GPIO_Pin_8
#define BEEP_CLK 		RCC_APB2Periph_GPIOA
//蜂鸣器频率配置
#define BEEP_INTERVAL 200 //IO toggle interval time.
#define BEEP_DRIVER 	2*BEEP_INTERVAL //IO toggle cycle.
/***************************************************************
  *	@brief	蜂鸣器延时引脚电平翻转函数
  *	@param	utime 翻转间隔时间 单位毫秒
  *	@retval	void
****************************************************************/
void BEEP_Pin_State_Toggler(uint32_t utime){
	delay_us(utime);
	GPIO_SetBits(BEEP_PORT,BEEP_PIN);
	delay_us(utime);
	GPIO_ResetBits(BEEP_PORT,BEEP_PIN);
}
/***************************************************************
  *	@brief	蜂鸣器相关IO口的初始化
  *	@param	void
  *	@retval	void
****************************************************************/
void BEEP_Init(void){
	GPIO_InitTypeDef beep;
	
	RCC_APB2PeriphClockCmd(BEEP_CLK,ENABLE);
	
	beep.GPIO_Mode=GPIO_Mode_Out_PP;
	beep.GPIO_Pin=BEEP_PIN;
	beep.GPIO_Speed=GPIO_Speed_2MHz;
	
	GPIO_Init(BEEP_PORT,&beep);
	
	GPIO_SetBits(BEEP_PORT,BEEP_PIN);
}
/***************************************************************
  *	@brief	蜂鸣器播放操作
  *	@param	mtime 蜂鸣器鸣叫时间 单位毫秒
  *	@retval	void
****************************************************************/
void BEEP_Play(uint32_t mtime){
	uint32_t i=0;
	while(i<(mtime*1000)){
		BEEP_Pin_State_Toggler(BEEP_INTERVAL);
		i+=BEEP_DRIVER;
	}
}

