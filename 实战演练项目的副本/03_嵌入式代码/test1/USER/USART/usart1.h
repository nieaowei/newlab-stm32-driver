/***************************************************************
  *	Name		:	USRT1.H
  *	Data		:	2019.7.15
  *	Author	:	NIEAOWEI
  *	Note		:	串口相关的操作函数
****************************************************************/
#ifndef _USART1_H
#define _USART1_H

#include "sys.h"
#include "fifo.h"


extern FIFO_Type usart1_rx_fifo;//The serial port's receiving buffer.
extern uint8_t usart1_Receiving;

void Usart1_Init(uint32_t baudrate);

#endif

