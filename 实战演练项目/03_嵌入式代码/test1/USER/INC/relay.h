/***************************************************************
  *	Name		:	RELAY.H
  *	Data		:	2019.7.8
  *	Author	:	NIEAOWEI
  *	Note		:	继电器控制的相关操作
****************************************************************/
#ifndef _RELAY_H
#define _RELAY_H

#include "stdint.h"

void Relay_Init(void);
void door_ON(void);
void door_OFF(void);
void door_Toggle(void);
void latern_ON();
void latern_OFF();

extern  uint8_t door_status;
extern  uint8_t latern_status;
#endif


