/***************************************************************
  *	Name		:	IIC.H
  *	Data		:	2019.7.30
  *	Author	:	NIEAOWEI
  *	Note		:	iic通信相关操作函数
****************************************************************/
#ifndef _IIC_H
#define _IIC_H
#include <stdint.h>

void IIC_Init(void);
uint8_t IIC_Read_Byte(uint8_t ack,uint8_t *recv);
uint8_t IIC_Write_Byte(uint8_t byte);
uint8_t IIC_Write_Bytes(uint8_t addr,uint8_t* bytes,uint8_t lenth);
uint8_t IIC_Wait_Ack(void);
void IIC_Stop(void);
void IIC_Start(void);
void IIC_NAck(void);
void IIC_Ack(void);
uint8_t IIC_ReadBytes_Ack(uint8_t addr,uint8_t *rec,uint8_t lenth);

#endif

