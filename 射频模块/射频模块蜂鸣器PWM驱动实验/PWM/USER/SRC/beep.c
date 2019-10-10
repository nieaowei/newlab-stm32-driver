#include "beep.h"
#include "delay.h"

#define BEEP_PORT 	GPIOB
#define BEEP_PIN 		GPIO_Pin_5
#define BEEP_CLK 		RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO

void BEEP_SetRate(uint32_t utime){
	TIM_SetCompare2(TIM3,utime);
}


void BEEP_Init(void){
	GPIO_InitTypeDef beep;
	
	RCC_APB2PeriphClockCmd(BEEP_CLK,ENABLE);
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);
	
	beep.GPIO_Mode=GPIO_Mode_AF_PP;
	beep.GPIO_Pin=BEEP_PIN;
	beep.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(BEEP_PORT,&beep);
	
	GPIO_SetBits(BEEP_PORT,BEEP_PIN);
	
}

void BEEP_Play(uint32_t mtime,uint32_t rate_utime){
	BEEP_SetRate(rate_utime);
	TIM_CCxCmd(TIM3,TIM_Channel_2,TIM_CCx_Enable);
	delay_ms(mtime);
	TIM_CCxCmd(TIM3,TIM_Channel_2,TIM_CCx_Disable);
}

