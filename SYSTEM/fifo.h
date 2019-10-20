#ifndef _FIFO_H
#define _FIFO_H

#include "stdint.h"

#define FIFO_MAX			512
#define FIFO_MASK			FIFO_MAX-1
#define Byte					uint8_t
#define FIFO_OP_FAIL	FIFO_MAX

typedef struct info{
	
	uint8_t data[FIFO_MAX];
	uint16_t in;
	uint16_t out;
	uint8_t is_full;
	
}FIFO_Type;

void FIFO_Init(FIFO_Type *FIFO);

uint16_t FIFO_In_Byte(FIFO_Type *FIFO,Byte byte);

uint16_t FIFO_Out_Byte(FIFO_Type *FIFO,Byte *byte); 

uint16_t FIFO_Out_Bytes(FIFO_Type *FIFO,Byte *byte);//Read a set of data with the "newline" signs.

uint8_t FIFO_Is_Empty(FIFO_Type *FIFO);

#endif

