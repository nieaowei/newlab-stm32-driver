
/***************************************************************
  *	Name		:	FIFO.C
  *	Data		:	2019.7.15
  *	Author	:	NIEAOWEI
  *	Note		:	队列相关操作函数，主要用于串口接收
****************************************************************/
#include "fifo.h"

/***************************************************************
  *	@brief	初始化队列
  *	@param	*FIFO 需要初始化的队列指针或地址
  *	@retval	void
****************************************************************/
void FIFO_Init(FIFO_Type *FIFO){
	FIFO->in=FIFO->out=0;
	FIFO->is_full=0;
}
/***************************************************************
  *	@brief	判断队列是否为空
  *	@param	*FIFO 需要判断的队列指针或地址
  *	@retval	uint8_t
										0:非空
										非0：空
****************************************************************/
uint8_t FIFO_Is_Empty(FIFO_Type *FIFO){
	return FIFO->in==FIFO->out;
}
/***************************************************************
  *	@brief	返回队列的满标志，判断队列是否已满
  *	@param	*FIFO 
  *	@retval	uint8_t
										1:满
										0:未满
****************************************************************/
uint8_t FIFO_Is_Full(FIFO_Type *FIFO){
	return FIFO->is_full;
}
/***************************************************************
  *	@brief	入队一个数据或一个字节
  *	@param	*FIFO 需要操作的队列地址或指针
						byte	入队的的数据
  *	@retval	uint16_t
											FIFO_OP_FAIL:失败
											非FIFO_OP_FAIL:成功
****************************************************************/
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
/***************************************************************
  *	@brief	出队一个数据或一个字节
  *	@param	*FIFO 需要操作的队列地址或指针
						*byte	存储出队数据的地址或者指针
  *	@retval	uint16_t
											FIFO_OP_FAIL:失败
											非FIFO_OP_FAIL:成功											
****************************************************************/
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
/***************************************************************
  *	@brief	出队一组数据，以新行为数据结束标志
  *	@param	*byte 存储数据的数组地址
  *	@retval	uint16_t
											FIFO_OP_FAIL:失败
											非FIFO_OP_FAIL:成功
****************************************************************/
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



