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
void Relay_ON(void);
void Relay_OFF(void);
void Relay_Toggle(void);

#endif


