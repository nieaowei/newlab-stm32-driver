/***************************************************************
  *	Name		:	RELAY.C
  *	Data		:	2019.7.8
  *	Author	:	NIEAOWEI
  *	Note		:	继电器相关函数的实现，简单的说就是IO口高低电平输出。
							继电器原理就是小电流控制磁吸开关的闭合。实质上就是
							简单的IO口输出实验。
****************************************************************/
#include "relay.h"
#include "sys.h"

//Define the parameters of the relay IO port.
#define RELAY_CLK 			RCC_APB2Periph_GPIOB
#define RELAY_PORT 			GPIOB
#define RELAY_PIN 			GPIO_Pin_5
#define RELAY_ON				GPIO_SetBits(RELAY_PORT,RELAY_PIN)
#define RELAY_OFF				GPIO_ResetBits(RELAY_PORT,RELAY_PIN)
/***************************************************************
  *	@brief	继电器相关IO的初始化
  *	@param	void
  *	@retval	void
****************************************************************/
void Relay_Init(){
	GPIO_InitTypeDef relay;//Declare the IO initialization structure
	
	RCC_APB2PeriphClockCmd(RELAY_CLK,ENABLE);//Enable the relevant clock.
	
	//Set the parameters of the IO port
	relay.GPIO_Pin=RELAY_PIN;
	relay.GPIO_Mode=GPIO_Mode_Out_PP;//Ste push-pull-output mode.
	relay.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(RELAY_PORT,&relay);
	
	GPIO_ResetBits(RELAY_PORT,RELAY_PIN);//Initialization IO status.
	
}
/***************************************************************
  *	@brief	打开继电器开关
  *	@param	void
  *	@retval	void
****************************************************************/
void Relay_ON(void){
	RELAY_ON;
}
/***************************************************************
  *	@brief	关闭继电器开关
  *	@param	void
  *	@retval	void
****************************************************************/
void Relay_OFF(void){
	RELAY_OFF;
}

