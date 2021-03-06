/**********************************
**头文件：
**文件名: 74hc595.c 
**创建人: 
**日期:   
**功能描述: 两串 74HC595 串行移位16位输出
**占用资源: 595SI, 595SCK,595RCK
*/
#ifndef __74HC595_H_
#define __74HC595_H_

#include "stdint.h"


/************************************************************
*函数：void HC595_GpioInit(void)
*功能：74HC595信号引脚对应的I/O口功能初始化
*输入：无
*输出：无
*特殊说明：无
************************************************************/
void HC595_GpioInit(void);

void hc595_out(uint8_t dh, uint8_t dl);

#endif
