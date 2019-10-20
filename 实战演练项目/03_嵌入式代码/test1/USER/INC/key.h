/***************************************************************
  *	Name		:	KEY.H
  *	Data		:	2019.7.10
  *	Author	:	NIEAOWEI
  *	Note		:	按键相关操作
****************************************************************/
#ifndef _KEY_H
#define _KEY_H

#include "stdint.h"

/*
		按键相关定义，供外部使用
*/
enum	KEY_State{
	KEY1_PRESSED=1,
	KEY2_PRESSED,
};

void KEY_Init(void);

uint8_t KEY_Scan(uint8_t mode);

uint8_t KEY1(void);

uint8_t KEY2(void);

unsigned char read_key(void);

uint8_t Key_Scan(void);

#endif

