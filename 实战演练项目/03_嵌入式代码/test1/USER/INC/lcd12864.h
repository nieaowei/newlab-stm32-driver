/***************************************************************
  *	Name		:	LCD12864.H
  *	Data		:	2019.7.30
  *	Author	:	NIEAOWEI
  *	Note		:	该文件为lcd相关操作函数
****************************************************************/
#ifndef _LCD12864_H
#define _LCD12864_H

#include "stdint.h"

#define CSX_BUSY 		0X80
#define CSX_ON_OFF	0X20
#define CSX_RESET		0X10

void lcd_init (void);
void lcd_clr(void);
void lcd_clr_row(unsigned char row);
void lcd_writeOneChar(uint8_t x,uint8_t y, uint8_t c);
void lcd_writeLenthChar(uint8_t x,uint8_t y, char* str,uint8_t lenth);

void show_logo(void);
#endif

