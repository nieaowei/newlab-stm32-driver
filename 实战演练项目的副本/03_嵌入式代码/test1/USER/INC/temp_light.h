/***************************************************************
  *	Name		:	TEMP_LIGHT.H
  *	Data		:	2019.10.5
  *	Author	:	NIEAOWEI
  *	Note		:	温度光照采集
****************************************************************/

#ifndef _TEMP_LIGHT_H
#define _TEMP_LIGHT_H

#include <stdint.h>

uint8_t GetTemp(void);
uint8_t GetLight();
void TEMP_LIGHT_Init(void);

#endif

