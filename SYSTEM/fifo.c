#include "fifo.h"


void FIFO_Init(FIFO_Type *FIFO){
	FIFO->in=FIFO->out=0;
}

uint8_t FIFO_Is_Empty(FIFO_Type *FIFO){
	return FIFO->in==FIFO->out;
}

uint8_t FIFO_Is_Full(FIFO_Type *FIFO){
	return FIFO->is_full;
}

uint16_t FIFO_In_Byte(FIFO_Type *FIFO,Byte byte){
	if(FIFO_Is_Full(FIFO)){
		return FIFO_OP_FAIL;
	}
	FIFO->data[FIFO->in&FIFO_MASK]=byte;
	FIFO->in++;
	if((FIFO->in&FIFO_MASK)==(FIFO->out&FIFO_MASK)){
		FIFO->is_full=1;
	}
	return FIFO->in&FIFO_MASK;
}

uint16_t FIFO_Out_Byte(FIFO_Type *FIFO,Byte *byte){
	if(FIFO_Is_Empty(FIFO)){
		return FIFO_OP_FAIL;
	}
	if(FIFO_Is_Full(FIFO)){
		FIFO->is_full=0;
	}
	*byte=FIFO->data[FIFO->out&FIFO_MASK];
	FIFO->out++;
	return FIFO->out&FIFO_MASK;
}

uint16_t FIFO_Out_Bytes(FIFO_Type *FIFO,Byte *byte){
	uint16_t length=0;
	Byte current=0,former=0;
	while(1){
		former=current;
		if(FIFO_Out_Byte(FIFO,&current)==FIFO_OP_FAIL){
			return FIFO_OP_FAIL;
		}else{
			if(former==0x0d && current==0x0a){
				byte[length-1]='\0';
				return length;
			}
			byte[length++]=current;
		}
	}
}



