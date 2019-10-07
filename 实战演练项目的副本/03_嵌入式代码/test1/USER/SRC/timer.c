/***************************************************************
  *	Name		:	TIMER.C
  *	Data		:	2019.7.25
  *	Author	:	NIEAOWEI
  *	Note		:	定时器的函数实现
****************************************************************/
#include "timer.h"
#include "sys.h"

/***************************************************************
  *	@brief	定时器6的初始化，72为1微秒
  *	@param	void
  *	@retval	void
****************************************************************/
void TIM6_Init(void){
	TIM_TimeBaseInitTypeDef tim6;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	
	tim6.TIM_ClockDivision=TIM_CKD_DIV1;
	tim6.TIM_CounterMode=TIM_CounterMode_Up;
	tim6.TIM_Prescaler=1-1;
	tim6.TIM_Period=65535-1;
	TIM_TimeBaseInit(TIM6,&tim6);
	
	TIM_Cmd(TIM6,ENABLE);
	
}


/***************************************************************
  *	@brief	采用定时器6进行延时的函数
  *	@param	utime 延时的微秒数 范围在 0~910微秒之间，因为需要
									提供精确的延时
  *	@retval	void
****************************************************************/
void TIM6_Delay_us(int utime){
	
	TIM_SetCounter(TIM6,0);
	while(TIM_GetCounter(TIM6)<(utime*72));
	//printf(" time : %d \r\n",TIM_GetCounter(TIM6));
	
}
