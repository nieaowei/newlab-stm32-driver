

#include "extend_adc.h"
#include "iic.h"
#include "stdio.h"

/*******************************************************************
*函数名：uint8_t PCF8591_Write(uint8_t DAC_Date)
*功能：PCF8591按DAC_Date的大小进行数模转换输出，输出对应大小电压在
*				DAC引脚上;
*输入：uint8_t DAC_Date：数字量
*输出：返回 0成功 或者 1失败
*特殊说明：无
*******************************************************************/
uint8_t PCF8591_Write(uint8_t DAC_Date)
{
	uint8_t ack;
	uint8_t data[2]={Model_PCF8591,DAC_Date};
	
	return IIC_Write_Bytes(Address_PCF8591,data,2);
	
}

/**********************************************************
*函数名：uint8_t PCF8591_Read(uint8_t *a, uint8_t ch, uint8_t n)
*功能：从PCF8591中读出自ch通道开始的n个ADC通道的数据
*输入： uint8_t *Data, ADC数据缓存区，
*								Data[0]存放通道ch的ADC数据,
*								Data[1]存放通道ch+1的ADC数据,
*								......,
*								Data[n-1]存放通道ch+n-1的ADC数据；
*				uint8_t ch,开始通道编号；
*				uint8_t n,通道数量；
*输出：返回 HAL_I2C_OK 或return HAL_I2C_FAIL;
*特殊说明：无
**********************************************************/
uint8_t PCF8591_Read(uint8_t *a, uint8_t ch, uint8_t n)
{    
	uint8_t ack,i;
	uint8_t ad[4],cmd[1]={Model_PCF8591auto};
	ch &= 3;
	if(n > 4)
	{
		n = 4;
	}
	ack=IIC_Write_Bytes(Address_PCF8591,cmd,1);
	
	if(ack == 1)                   //写入失败 从机设备没有相应
	{
		return (1);
	}
	
	ack = IIC_ReadBytes_Ack(Address_PCF8591,ad,4);
	
	
	if(ack == 1)                   
	{
		return (1);
	}
	return 0;
}


/**************************************************************
*函数名：uint8_t PCF8591_ReadChannel(uint8_t *a, uint8_t ch)
*功能：从PCF8591中读出ch通道的ADC数据
*输入： uint8_t *Data, ch通道的ADC数据缓冲区
*				uint8_t ch，通道编号
*输出：返回 HAL_I2C_OK 或return HAL_I2C_FAIL;
*特殊说明：无
***************************************************************/
uint8_t PCF8591_ReadChannel(uint8_t *a, uint8_t ch)
{    
	uint8_t ack,cmd[1]={Model_PCF8591S | ch},data[4];
	ch &= 3;
	ack = IIC_Write_Bytes(Address_PCF8591,cmd,1);
	if (ack==1){
		return 1;
	}
	ack = IIC_ReadBytes_Ack(Address_PCF8591,data,4);
	if (ack==1){
		return 1;
	}
	
	*a=data[3];

	return (0);	
}

/************************************************************
*函数名：uint8_t  PCF8591_Init(uint8_t ch)
*功能：初始化PCF8591和IIC总线，并指定PCF8591的通道和工作模式
*输入： uint8_t *Data, ch通道的ADC数据缓冲区
*				uint8_t ch，通道编号
*输出：返回 HAL_I2C_OK 或return HAL_I2C_FAIL;
*特殊说明：无
************************************************************/
uint8_t PCF8591_Init(uint8_t ch)
{
	uint8_t ack,cmd[1]={Model_PCF8591S | ch};
	ch &= 3;
	IIC_Init();
	ack=IIC_Write_Bytes(Address_PCF8591,cmd,1);
	
	if(ack==1){
		return 0;
	}
	
	ack=IIC_ReadBytes_Ack(Address_PCF8591,cmd,1);
	
	
	return ack;
}

/************************************************************
*函数名：uint8_t  PCF8591_ReadChNd(uint8_t *a)
*功能：从指定通道读出ADC数据，与PCF8591_Init配合使用
*输入： uint8_t *Data ，ADC数据缓冲区
*输出：返回 HAL_I2C_OK 或return HAL_I2C_FAIL;
*特殊说明：无
************************************************************/
uint8_t PCF8591_ReadChNd(uint8_t *a)
{
	uint8_t ack;
	ack =IIC_ReadBytes_Ack(Address_PCF8591,a,1);
	printf("adc的值：%d\r\n",*a);
	return ack;	
}
