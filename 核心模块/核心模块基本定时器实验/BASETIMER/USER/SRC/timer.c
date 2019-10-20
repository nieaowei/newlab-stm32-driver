#include "timer.h"
#include "sys.h"
#include "beep.h"

void TIM6_Init(uint16_t ar,uint16_t pre){
	
	TIM_TimeBaseInitTypeDef TIM6b;
	
	NVIC_InitTypeDef TIM6_it;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	
	TIM6b.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM6b.TIM_CounterMode=TIM_CounterMode_Up;
	TIM6b.TIM_Prescaler=pre;
	TIM6b.TIM_Period=ar;
	//TIM6b.TIM_RepetitionCounter=1;
	TIM_TimeBaseInit(TIM6,&TIM6b);
		
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	
	TIM6_it.NVIC_IRQChannel=TIM6_IRQn;
	TIM6_it.NVIC_IRQChannelPreemptionPriority=2;
	TIM6_it.NVIC_IRQChannelSubPriority=3;
	TIM6_it.NVIC_IRQChannelCmd=ENABLE;

	NVIC_Init(&TIM6_it);
	
	TIM_Cmd(TIM6,ENABLE);
}


void TIM6_IRQHandler(void){
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)!=RESET){
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
		BEEP_Play(200);
	}
}

