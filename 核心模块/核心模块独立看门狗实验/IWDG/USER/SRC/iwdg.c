/***************************************************************
  *	Name		:	IWDG.C
  *	Data		:	2019.7.9
  *	Author	:	NIEAOWEI
  *	Note		:	独立看门狗的相关操作函数，独立看门狗的时钟是40khz
****************************************************************/
#include "iwdg.h"
#include "sys.h"
/***************************************************************
  *	@brief	看门狗负人初始化操作
  *	@param	void
  *	@retval	void
****************************************************************/
void IWDG_Init(void){
	//Enable 
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	IWDG_SetPrescaler(IWDG_Prescaler_16);//设置预分频
	IWDG_SetReload(0xfff);//设置重载值
	IWDG_ReloadCounter();//重载计数器
	
	IWDG_Enable();//启动看门狗
	
}
/***************************************************************
  *	@brief	喂狗操作
  *	@param	void
  *	@retval	void
****************************************************************/
void IWDG_Feed(void){
	IWDG_ReloadCounter();//
}

