#include "beep.h"
#include "delay.h"

#define BEEP_PORT 	GPIOA
#define BEEP_PIN 		GPIO_Pin_8
#define BEEP_CLK 		RCC_APB2Periph_GPIOA

#define BEEP_INTERVAL 200 //IO toggle interval time.
#define BEEP_DRIVER 	2*BEEP_INTERVAL //IO toggle cycle.

void BEEP_Pin_State_Toggler(uint32_t utime){
	delay_us(utime);
	GPIO_SetBits(BEEP_PORT,BEEP_PIN);
	delay_us(utime);
	GPIO_ResetBits(BEEP_PORT,BEEP_PIN);
}
void BEEP_Init(void){
	GPIO_InitTypeDef beep;
	
	RCC_APB2PeriphClockCmd(BEEP_CLK,ENABLE);
	
	beep.GPIO_Mode=GPIO_Mode_Out_PP;
	beep.GPIO_Pin=BEEP_PIN;
	beep.GPIO_Speed=GPIO_Speed_2MHz;
	
	GPIO_Init(BEEP_PORT,&beep);
	
	GPIO_SetBits(BEEP_PORT,BEEP_PIN);
}

void BEEP_Play(uint32_t mtime){
	uint32_t i=0;
	while(i<(mtime*1000)){
		BEEP_Pin_State_Toggler(BEEP_INTERVAL);
		i+=BEEP_DRIVER;
	}
}

