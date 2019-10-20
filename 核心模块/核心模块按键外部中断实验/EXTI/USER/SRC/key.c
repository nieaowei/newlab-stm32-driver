#include "key.h"
#include "delay.h"

#define KEY1_PORT			GPIOC
#define KEY1_PIN			GPIO_Pin_13
#define KEY1_CLK		RCC_APB2Periph_GPIOC

#define KEY2_PORT			GPIOD
#define KEY2_PIN			GPIO_Pin_13
#define KEY2_CLK		RCC_APB2Periph_GPIOD

#define KEY_ALL_CLK	KEY1_CLK|KEY2_CLK

//#define KEY1					GPIO_ReadInputDataBit(KEY1_PORT,KEY1_PIN)
//#define KEY2					GPIO_ReadInputDataBit(KEY2_PORT,KEY2_PIN)

void KEY_Init(void){
	GPIO_InitTypeDef KEY;
	//Enable the relevant IO clock.
	RCC_APB2PeriphClockCmd(KEY_ALL_CLK,ENABLE);
	//Set the relevant IO port.
	KEY.GPIO_Mode=GPIO_Mode_IPU;
	
	KEY.GPIO_Pin=KEY1_PIN;
	GPIO_Init(KEY1_PORT,&KEY);
	
	KEY.GPIO_Pin=KEY2_PIN;
	GPIO_Init(KEY2_PORT,&KEY);
}

uint8_t KEY_Scan(uint8_t mode){
	static uint8_t key_pressed=1;
	if(mode==1){
		key_pressed=1;
	}
	if(key_pressed && (KEY1()==0 || KEY2()==0)){//If no button has been pressed.
		delay_ms(25);
		key_pressed=0;//Press the button
		if(KEY1()==0){
			return KEY1_PRESSED;
		}else if(KEY2()==0){
			return KEY2_PRESSED;
		}
	}else if(KEY1()==1 && KEY2()==1){//If there is a button pressed.
		key_pressed=1;//Release button.
	}
	return 0;
}

uint8_t KEY1(){
	return GPIO_ReadInputDataBit(KEY1_PORT,KEY1_PIN);
}

uint8_t KEY2(){
	return GPIO_ReadInputDataBit(KEY2_PORT,KEY2_PIN);
}