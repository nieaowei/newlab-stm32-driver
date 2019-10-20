/***************************************************************
  *	Name		:	EXTEND_RTC.H
  *	Data		:	2019.10.06
  *	Author	:	NIEAOWEI
  *	Note		:	拓展模块rtc实时时钟的相关操作
****************************************************************/

#ifndef _EXTEND_RTC_H
#define _EXTEND_RTC_H

#include "stdint.h"

uint8_t PCF8563_Init(void);
void PCF8563_GetTime(uint8_t *ptr);
void PCF8563_WirteTime(uint8_t *ptr);
uint8_t Convert_BcdToHex(uint8_t *bcd, uint8_t *hex);
uint8_t Time_CheckValidity(uint8_t *rtc);

#endif

