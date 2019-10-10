#include "exti.h"
#include "beep.h"
#include "relay.h"
#include "key.h"
#include "sys.h"
#include "delay.h"

void EXTIx_Init(void){
	EXTI_InitTypeDef exti;
	
	NVIC_InitTypeDef exti_it;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	//GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource13);
	
	exti.EXTI_Line=EXTI_Line13;
	exti.EXTI_Mode=EXTI_Mode_Interrupt;
	exti.EXTI_Trigger=EXTI_Trigger_Falling;
	exti.EXTI_LineCmd=ENABLE;
	EXTI_Init(&exti);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource13);
	
	exti_it.NVIC_IRQChannel=EXTI15_10_IRQn;
	exti_it.NVIC_IRQChannelPreemptionPriority=2;
	exti_it.NVIC_IRQChannelSubPriority=2;
	exti_it.NVIC_IRQChannelCmd=ENABLE;
	
	NVIC_Init(&exti_it);
	
	
	delay_init();
	BEEP_Init();
	Relay_Init();
	KEY_Init();
	
	BEEP_Play(200);
}


void EXTI15_10_IRQHandler(void){
	
	delay_ms(10);
	//if(KEY2()==0){
	//	BEEP_Play(200);
	//}
	if(KEY1()==0){
		Relay_Toggle();
	}
	EXTI_ClearITPendingBit(EXTI_Line13);
}


