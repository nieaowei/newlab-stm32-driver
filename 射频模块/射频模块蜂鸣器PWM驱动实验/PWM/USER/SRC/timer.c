#include "timer.h"
#include "sys.h"
#include "beep.h"

void TIM3_Init(uint16_t ar,uint16_t pre){
	
	TIM_TimeBaseInitTypeDef TIM3b;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM3b.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM3b.TIM_CounterMode=TIM_CounterMode_Up;
	TIM3b.TIM_Prescaler=pre;
	TIM3b.TIM_Period=ar;
	//TIM3b.TIM_RepetitionCounter=1;
	TIM_TimeBaseInit(TIM3,&TIM3b);

}

void TIM3_PWM_Init(uint16_t ar,uint16_t pre){
	
	TIM_OCInitTypeDef tim3;
	
	TIM3_Init(ar,pre);
	
	tim3.TIM_OCMode=TIM_OCMode_PWM2;
	tim3.TIM_OutputState=TIM_OutputState_Enable;
	tim3.TIM_OCPolarity=TIM_OCPolarity_High;
	
	TIM_OC2Init(TIM3,&tim3);
	
	TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM3,ENABLE);
	
	TIM_CCxCmd(TIM3,TIM_Channel_2,TIM_CCx_Disable);
	
}
