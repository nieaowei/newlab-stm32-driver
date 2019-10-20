/***************************************************************
  *	Name		:	KEY.H
  *	Data		:	2019.7.10
  *	Author	:	NIEAOWEI
  *	Note		:	按键相关函数的实现以及配置
****************************************************************/
#include "key.h"
#include "delay.h"
/*
	按键相关IO配置
*/
#define KEY1_PORT			GPIOC
#define KEY1_PIN			GPIO_Pin_13
#define KEY1_CLK			RCC_APB2Periph_GPIOC

#define KEY2_PORT			GPIOD
#define KEY2_PIN			GPIO_Pin_13
#define KEY2_CLK			RCC_APB2Periph_GPIOD
/*
	按键所使用的所有时钟，方便初始化
*/
#define KEY_ALL_CLK	KEY1_CLK|KEY2_CLK

//#define KEY1					GPIO_ReadInputDataBit(KEY1_PORT,KEY1_PIN)
//#define KEY2					GPIO_ReadInputDataBit(KEY2_PORT,KEY2_PIN)
/***************************************************************
  *	@brief	按键相关IO口的初始化
  *	@param	void
  *	@retval	void
****************************************************************/
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
/***************************************************************
  *	@brief	按键扫描函数，延时消抖方案
  *	@param	mode 模式选择，1：支持连按 0：不支持连按
  *	@retval	uint8_t
							KEY1_PRESSED=1,
							KEY2_PRESSED=2,
							0:未扫描到按键按下
****************************************************************/
uint8_t KEY_Scan(uint8_t mode){
	static uint8_t key_pressed=1;
	uint8_t key1,key2;
	key1=KEY1();
	key2=KEY2();
	if(mode==1){
		key_pressed=1;
	}
	if(key_pressed && (key1==0 || key2==0)){//If no button has been pressed.
		delay_ms(25);
		key_pressed=0;//Press the button
		if(key1==0){
			return KEY1_PRESSED;
		}else if(key2==0){
			return KEY2_PRESSED;
		}
	}else if(key1==1 && key2==1){//If there is a button pressed.
		key_pressed=1;//Release button.
	}
	return 0;
}
/***************************************************************
  *	@brief	读取按键1
  *	@param	void
  *	@retval	uint8_t
										0：按下
										1：未按下
****************************************************************/
uint8_t KEY1(){
	return GPIO_ReadInputDataBit(KEY1_PORT,KEY1_PIN);
}
/***************************************************************
  *	@brief	读取按键2
  *	@param	void
  *	@retval	uint8_t
										0：按下
										1：未按下
****************************************************************/
uint8_t KEY2(){
	return GPIO_ReadInputDataBit(KEY2_PORT,KEY2_PIN);
}
/*
	以下为状态机消抖方案
*/
//状态定义
enum KEY_info{
	KEY_START,//起始
	KEY_PRESSED,//按下
	KEY_RELEASE,//释放
};
/***************************************************************
  *	@brief	按键扫描函数，状态机方案
  *	@param	void
  *	@retval	
****************************************************************/
uint8_t Key_Scan(void){
	static uint8_t key_state=KEY_START;
	static uint8_t last_key=0;
	uint8_t current;
	
	if(!KEY1()){
		current=KEY1_PRESSED;
	}else if(!KEY2()){
		current=KEY2_PRESSED;
	}else{
		current=0;
	}
	
	switch(key_state){
		case KEY_START:{
			if(current!=0){
				key_state=KEY_PRESSED;
				last_key=current;
			}
			break;
		}
		case KEY_PRESSED:{
				if(current==last_key){
					key_state=KEY_RELEASE;
					return current;
				}else{
					key_state=KEY_START;
				}
			break;
		}
		case KEY_RELEASE:{
			if(current==0){
				key_state=KEY_START;
			}
			break;
		}
	}
	return 0;
}

