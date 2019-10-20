
/***************************************************************
  *	Name		:	IIC.C
  *	Data		:	2019.7.30
  *	Author	:	NIEAOWEI
  *	Note		:	
							1.编写IIC协议时切记不能使用串口调试，
							串口发送占用时间会干扰IIC时序，影响
							时钟频率。
							
****************************************************************/
#include "iic.h"
#include "sys.h"
#include "delay.h"

//IIC相关引脚配置
#define IIC_SCL_PORT	GPIOB
#define IIC_SCL_PIN		GPIO_Pin_6
#define IIC_SCL_CLK		RCC_APB2Periph_GPIOB
#define SCL(n)				GPIO_WriteBit(IIC_SCL_PORT,IIC_SCL_PIN,(BitAction)n)

#define IIC_SDA_PORT	GPIOB
#define IIC_SDA_PIN		GPIO_Pin_7
#define IIC_SDA_CLK		RCC_APB2Periph_GPIOB
#define SDAW(n)				GPIO_WriteBit(IIC_SDA_PORT,IIC_SDA_PIN,(BitAction)n)
#define SDAR()				GPIO_ReadInputDataBit(IIC_SDA_PORT,IIC_SDA_PIN)

#define IIC_RATE			2//us

/***************************************************************
  *	@brief	IIC引脚初始化
  *	@param	void
  *	@retval	void
****************************************************************/
void IIC_Init(void){
	
	GPIO_InitTypeDef iic;
	RCC_APB2PeriphClockCmd(IIC_SCL_CLK|IIC_SDA_CLK,ENABLE);
	
	iic.GPIO_Mode=GPIO_Mode_Out_PP;
	iic.GPIO_Speed=GPIO_Speed_50MHz;
	iic.GPIO_Pin=IIC_SDA_PIN;
	GPIO_Init(IIC_SDA_PORT,&iic);
	iic.GPIO_Pin=IIC_SCL_PIN;
	GPIO_Init(IIC_SCL_PORT,&iic);
	SCL(1);
	SDAW(1);
}
/***************************************************************
  *	@brief	设置SDA口为输出模式
  *	@param	void.
  *	@retval	void.
****************************************************************/
void enableWrite(void){
	GPIO_InitTypeDef iic;
	iic.GPIO_Mode=GPIO_Mode_Out_PP;
	iic.GPIO_Speed=GPIO_Speed_50MHz;
	iic.GPIO_Pin=IIC_SDA_PIN;
	GPIO_Init(IIC_SDA_PORT,&iic);
}
/***************************************************************
  *	@brief	设置SDA口为上拉输入，根据iic协议特性，只有0的输入
  *	@param	void.
  *	@retval	void.
****************************************************************/
void enableRead(void){
	GPIO_InitTypeDef iic;
	iic.GPIO_Mode=GPIO_Mode_IPU;
	iic.GPIO_Speed=GPIO_Speed_50MHz;
	iic.GPIO_Pin=IIC_SDA_PIN;
	GPIO_Init(IIC_SDA_PORT,&iic);
}
/***************************************************************
  *	@brief	发出IIC起始信号。
														起始时序
																		SDA _
																				 \__
																		SCL ___
																					 \_
  *	@param	void
  *	@retval	void
****************************************************************/
void IIC_Start(void){
	enableWrite();
	SDAW(1);
	SCL(1);
	delay_us(IIC_RATE);
	SDAW(0);
	delay_us(IIC_RATE);
	SCL(0);
}
/***************************************************************
  *	@brief	发出IIC停止信号
														停止时序
																		sda 	 _
																				 _/
																		scl 
																				 ___
  *	@param	
  *	@retval	
****************************************************************/
void IIC_Stop(void){
	enableWrite();
	SCL(0);
	SDAW(0);
	delay_us(IIC_RATE);
	SCL(1);
	SDAW(1);
	delay_us(IIC_RATE);

}
/***************************************************************
  *	@brief	产生应答信号
												 应答时序
																sda 
																		x____
																scl		 _
																		__/	\_
  *	@param	
  *	@retval	
****************************************************************/
void IIC_Ack(void){
	SCL(0);
	enableWrite();
	SDAW(0);
	delay_us(IIC_RATE);
	SCL(1);
	delay_us(IIC_RATE);
	SCL(0);
}
/***************************************************************
  *	@brief	产生不应答信号
													不应答时序
																	sda  _____
																			x
																	scl		 _
																			__/	\_
																		
  *	@param	
  *	@retval	
****************************************************************/
void IIC_NAck(void){
	SCL(0);
	enableWrite();
	SDAW(1);
	delay_us(IIC_RATE);
	SCL(1);
	delay_us(IIC_RATE);
	SCL(0);
}
/***************************************************************
  *	@brief	等待从机的低电平应答信号
  *	@param	void
  *	@retval	uint8_t.
						1:等待应答超时
						0:等待应答成功
****************************************************************/
uint8_t IIC_Wait_Ack(void){
	u8 ucErrTime=0;
	SDAW(1);
	enableRead();      //SDA设置为输入
	delay_us(IIC_RATE);	   
	SCL(1);
	delay_us(IIC_RATE);	 
	while(SDAR())
	{
		
		ucErrTime++;
		if(ucErrTime>IIC_RATE)
		{
			IIC_Stop();
			return 1;
		}
	}
	SCL(0);//   
	return 0;  
} 
/***************************************************************
  *	@brief	发送一个字节的数据并等待应答
  *	@param	byte：一个字节的数据
  *	@retval	uint8_t.
										1:发送失败
										0:发送成功
****************************************************************/
uint8_t IIC_Write_Byte(uint8_t byte){
	uint8_t count=8;
	enableWrite();
	while(count--){
		if((byte&0x80)!=0){
			SDAW(1);
		}else{
			SDAW(0);
		}
		byte<<=1;
		delay_us(IIC_RATE);
		SCL(1);
		delay_us(IIC_RATE);
		SCL(0);
	}	
	return IIC_Wait_Ack();
}
/***************************************************************
	*	@brief	通过IIC发送多个字节的数据
  *	@param	bytes 数据字节数组
	* @param	lenth	数据字节数组的长度
	*	@param	addr	从机设备地址
  *	@retval	uint8_t
										0:发送成功
										1:发送失败
****************************************************************/
uint8_t IIC_Write_Bytes(uint8_t addr,uint8_t* bytes,uint8_t lenth){
	uint8_t index=0;
	IIC_Start();
	if(IIC_Write_Byte(addr<<1)!=0){
		return 1;
	}
	while(lenth--){
		if(IIC_Write_Byte(bytes[index++])==0){
			continue;
		}else{
			return 1;
		}
	}
	IIC_Stop();
	return 0;
}

/***************************************************************
  *	@brief	读取一个字节的数据，产生应答或不应答
	*	@param	ack 是否产生应答，1产生应答，0不产生应答
	*	@param	*recv 用来接收存储这个一个字节的数据
  *	@retval	uint8_t 
										0:接收完成
****************************************************************/
uint8_t IIC_Read_Byte(uint8_t ack,uint8_t *recv){
	uint8_t i=8;
	SDAW(1);
	enableRead();
	while(i--){
		SCL(0);
		delay_us(IIC_RATE);
		SCL(1);
		*recv<<=1;
		if(SDAR()){
			(*recv)++;
		}
		delay_us(IIC_RATE);
	}
	if(ack){
		IIC_Ack();
	}else{
		IIC_NAck();
	}
	return 0;
}
/***************************************************************
  *	@brief	读取lenth长度字节的数据存放在rec数组
  *	@param	addr 从机设备地址
	* @param	*rec 存放数组的数组地址
	*	@param	lenth	需要读取的数据长度
  *	@retval	uint8_t
										1:失败
										0:成功
****************************************************************/
uint8_t IIC_ReadBytes_Ack(uint8_t addr,uint8_t *rec,uint8_t lenth)
{
	uint8_t index = 0;
	IIC_Start();
	
	if(IIC_Write_Byte((addr<<1)|0x01)==1){
		return 1;
	}
	while(lenth--){
		IIC_Read_Byte(1,&rec[index++]);
	}
	IIC_Stop();
	return 0;
}
