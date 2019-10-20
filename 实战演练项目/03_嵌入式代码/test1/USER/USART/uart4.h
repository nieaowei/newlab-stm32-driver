/***************************************************************
  *	Name		:	USART3.H
  *	Data		:	2019.7.15
  *	Author	:	NIEAOWEI
  *	Note		:	串口3的相关操作函数，及定义
****************************************************************/
#ifndef _USART3_H
#define _USART3_H
#include "stdint.h"
#include "fifo.h"

void uart4_init(void);
void UART4_SendStrlen(uint8_t *data,uint16_t lenth);
//串口3的缓存队列
extern FIFO_Type uart4_rx_fifo;
extern uint8_t 	uart4_receiving;

#endif

