/***************************************************************
  *	Name		:	UART5.H
  *	Data		:	2019.8.2
  *	Author	:	NIEAOWEI
  *	Note		:	串口5的相关操作函数
****************************************************************/
#ifndef _UART5_H
#define _UART5_H
#include "stdint.h"
#include "fifo.h"
extern FIFO_Type uart5_rx_fifo;//串口5的接收缓存数组
void UART5_Init(uint32_t baudrate);
uint8_t UART5_SendBytes(uint8_t *data,uint8_t lenth);

#endif


