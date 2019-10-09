

#include "sys.h"
#include "timer7.h"
#include "uart4.h"
void TIM7_Init(){
	
	TIM_TimeBaseInitTypeDef tim7;
	NVIC_InitTypeDef irq;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
	
	tim7.TIM_ClockDivision=TIM_CKD_DIV1;
	tim7.TIM_CounterMode=TIM_CounterMode_Up;
	tim7.TIM_Prescaler=7200-1;
	tim7.TIM_Period=10-1;
	TIM_TimeBaseInit(TIM7,&tim7);
	
	irq.NVIC_IRQChannel=TIM7_IRQn;
	irq.NVIC_IRQChannelSubPriority=0;
	irq.NVIC_IRQChannelPreemptionPriority=2;
	irq.NVIC_IRQChannelCmd=ENABLE;
	
	NVIC_Init(&irq);
	
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);
	
	
	TIM_Cmd(TIM7,ENABLE);
	
}
extern void digital_tube_process();

extern uint16_t key_counter;
extern uint16_t cloud_counter;
extern uint16_t tube_counter;

void TIM7_IRQHandler(){
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)){
		TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
		key_counter++;
		cloud_counter++;
		tube_counter++;
		if(cloud_counter>65530){
			cloud_counter=0;
		}
		if(key_counter>65530){
			key_counter=0;
		}
		if(tube_counter>65530){
			tube_counter=0;
		}
		
		
		
//printf("timer 7 \r\n");
	}
}