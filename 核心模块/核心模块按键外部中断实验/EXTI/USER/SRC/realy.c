#include "relay.h"
#include "sys.h"

#define RELAY_CLK			RCC_APB2Periph_GPIOB
#define RELAY_PORT 		GPIOB
#define RELAY_PIN			GPIO_Pin_5

#ifdef _sys_h_

#define RELAY					PBout(5)
#define RELAY_ON			RELAY=1
#define RELAY_OFF			RELAY=0
#define RELAY_TOGGLE	RELAY=!RELAY

#else



#define RELAY_ON			GPIO_SetBits(RELAY_PORT,RELAY_PIN)
#define RELAY_OFF			GPIO_ResetBits(RELAY_PORT,RELAY_PIN)

#define RELAY_TOGGLE	GPIO_WriteBit(RELAY_PORT,RELAY_PIN,\
											GPIO_ReadOutputDataBit(RELAY_PORT,RELAY_PIN))

#endif

void Relay_Init(void){
	
	GPIO_InitTypeDef relay;
	
	RCC_APB2PeriphClockCmd(RELAY_CLK,ENABLE);
	
	relay.GPIO_Mode=GPIO_Mode_Out_PP;
	relay.GPIO_Pin=RELAY_PIN;
	relay.GPIO_Speed=GPIO_Speed_2MHz;
	
	GPIO_Init(RELAY_PORT,&relay);
	
	GPIO_ResetBits(RELAY_PORT,RELAY_PIN);
	
}

void Relay_Toggle(void){
	RELAY_TOGGLE;
}

void Relay_ON(void){
	RELAY_ON;
}

void Relay_OFF(void){
	RELAY_OFF;
}
