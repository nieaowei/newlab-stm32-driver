/***************************************************************
  *	Name		:	LCD.C
  *	Data		:	2019.7.31
  *	Author	:	NIEAOWEI
  *	Note		:
****************************************************************/
#include "lcd.h"
#include "sys.h"
#include "delay.h"
#include "usart1.h"
/*
	lcd相关结构定义
*/
typedef struct  
{										    
	u16 width;			//LCD 宽度
	u16 height;			//LCD 高度
	u16 id;				//LCD ID
	u8  dir;			//横屏还是竖屏控制：0，竖屏；1，横屏。	
	u16	wramcmd;		//开始写gram指令
	u16  setxcmd;		//设置x坐标指令
	u16  setycmd;		//设置y坐标指令 
}_lcd_dev;

//LCD地址结构体
typedef struct
{
	vu16 LCD_REG;
	vu16 LCD_RAM;
} LCD_TypeDef;

//IO口相关配置
/*
			FSMC总线口，这些口都设置为复用推挽输出
  PE7   ------> FSMC_D4
  PE8   ------> FSMC_D5
  PE9   ------> FSMC_D6
  PE10   ------> FSMC_D7
  PE11   ------> FSMC_D8
  PE12   ------> FSMC_D9
  PE13   ------> FSMC_D10
  PE14   ------> FSMC_D11
  PE15   ------> FSMC_D12
  PD8   ------> FSMC_D13
  PD9   ------> FSMC_D14
  PD10   ------> FSMC_D15
  PD11   ------> FSMC_A16
  PD14   ------> FSMC_D0
  PD15   ------> FSMC_D1
  PD0   ------> FSMC_D2
  PD1   ------> FSMC_D3
  PD4   ------> FSMC_NOE
  PD5   ------> FSMC_NWE
  PD7   ------> FSMC_NE1
*/
#define LCD_DB0_PIN			GPIO_Pin_14
#define LCD_DB0_PORT		GPIOD
#define LCD_DB0_INIT		LCD_DB0_PORT,LCD_DB0_PIN
#define LCD_DB1_PIN			GPIO_Pin_15
#define LCD_DB1_PORT		GPIOD
#define LCD_DB1_INIT		LCD_DB1_PORT,LCD_DB1_PIN
#define	LCD_DB2_PIN			GPIO_Pin_0
#define LCD_DB2_PORT		GPIOD
#define LCD_DB2_INIT		LCD_DB2_PORT,LCD_DB2_PIN
#define	LCD_DB3_PIN			GPIO_Pin_1
#define LCD_DB3_PORT		GPIOD
#define LCD_DB3_INIT		LCD_DB3_PORT,LCD_DB3_PIN
#define	LCD_DB4_PIN			GPIO_Pin_7
#define LCD_DB4_PORT		GPIOE
#define LCD_DB4_INIT		LCD_DB4_PORT,LCD_DB4_PIN
#define	LCD_DB5_PIN			GPIO_Pin_8
#define LCD_DB5_PORT		GPIOE
#define LCD_DB5_INIT		LCD_DB5_PORT,LCD_DB5_PIN
#define	LCD_DB6_PIN			GPIO_Pin_9
#define LCD_DB6_PORT		GPIOE
#define LCD_DB6_INIT		LCD_DB6_PORT,LCD_DB6_PIN
#define	LCD_DB7_PIN			GPIO_Pin_10
#define LCD_DB7_PORT		GPIOE
#define LCD_DB7_INIT		LCD_DB7_PORT,LCD_DB7_PIN
#define	LCD_DB8_PIN			GPIO_Pin_11
#define LCD_DB8_PORT		GPIOE
#define LCD_DB8_INIT		LCD_DB8_PORT,LCD_DB8_PIN
#define	LCD_DB9_PIN			GPIO_Pin_12
#define LCD_DB9_PORT		GPIOE
#define LCD_DB9_INIT		LCD_DB9_PORT,LCD_DB9_PIN
#define	LCD_DB10_PIN		GPIO_Pin_13
#define LCD_DB10_PORT		GPIOE
#define LCD_DB10_INIT		LCD_DB10_PORT,LCD_DB10_PIN
#define	LCD_DB11_PIN		GPIO_Pin_14
#define LCD_DB11_PORT		GPIOE
#define LCD_DB11_INIT		LCD_DB11_PORT,LCD_DB11_PIN
#define	LCD_DB12_PIN		GPIO_Pin_15
#define LCD_DB12_PORT		GPIOE
#define LCD_DB12_INIT		LCD_DB12_PORT,LCD_DB12_PIN
#define	LCD_DB13_PIN		GPIO_Pin_8
#define LCD_DB13_PORT		GPIOD
#define LCD_DB13_INIT		LCD_DB13_PORT,LCD_DB13_PIN
#define	LCD_DB14_PIN		GPIO_Pin_9
#define LCD_DB14_PORT		GPIOD
#define LCD_DB14_INIT		LCD_DB14_PORT,LCD_DB14_PIN
#define	LCD_DB15_PIN		GPIO_Pin_10
#define LCD_DB15_PORT		GPIOD
#define LCD_DB15_INIT		LCD_DB15_PORT,LCD_DB15_PIN
#define LCD_RS_PIN			GPIO_Pin_11							//rs 高电平数据  低电平指令
#define LCD_RS_PORT			GPIOD
#define LCD_RS_INIT			LCD_RS_PORT,LCD_RS_PIN
#define LCD_RD_PIN			GPIO_Pin_4							//rd 低电平有效
#define LCD_RD_PORT			GPIOD
#define LCD_RD_INIT			LCD_RD_PORT,LCD_RD_PIN
#define LCD_WR_PIN			GPIO_Pin_5							//wr 低电平有效
#define	LCD_WR_PORT			GPIOD
#define LCD_WR_INIT			LCD_WR_PORT,LCD_WR_PIN
#define LCD_CS_PIN			GPIO_Pin_7							//cs 低电平有效
#define LCD_CS_PORT			GPIOD
#define LCD_CS_INIT			LCD_CS_PORT,LCD_CS_PIN

//该IO口非FSMC总线口，设置为通用推挽输出
#define LCD_RST_PIN			GPIO_Pin_12							//rst 低电平复位
#define LCD_RST_PORT		GPIOD
#define LCD_RST_CLK			RCC_APB2Periph_GPIOD

//所有gpio的时钟
#define LCD_GPIO_CLK		RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|LCD_RST_CLK

/***************************************************************
  *	@brief	初始IO口至复用推挽输出模式，为FSMC总线
  *	@param	pin 引脚号
	* @param	port IO口编号
  *	@retval	void
****************************************************************/
void GPIO_InitTo_AFPP(GPIO_TypeDef* port,uint16_t pin){
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Mode=GPIO_Mode_AF_PP;
	gpio.GPIO_Speed=GPIO_Speed_50MHz;
	gpio.GPIO_Pin=pin;
	GPIO_Init(port,&gpio);
}


/***************************************************************
  *	@brief	FSMC总线的初始化，使用块1的norsram
  *	@param	void
  *	@retval	void
****************************************************************/
void LCD_FSMC_Init(void){
	FSMC_NORSRAMTimingInitTypeDef lcdt;
	FSMC_NORSRAMInitTypeDef lcdi;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);
	
	lcdi.FSMC_Bank=FSMC_Bank1_NORSRAM1;
	lcdi.FSMC_DataAddressMux=FSMC_DataAddressMux_Disable;//
	lcdi.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;//不使用同步模式
	lcdi.FSMC_MemoryType=FSMC_MemoryType_NOR;
	lcdi.FSMC_MemoryDataWidth=FSMC_MemoryDataWidth_16b;
	lcdi.FSMC_WriteOperation=FSMC_WriteOperation_Enable;
	lcdi.FSMC_ExtendedMode=FSMC_ExtendedMode_Disable;
	lcdi.FSMC_WriteTimingStruct=&lcdt;
	lcdi.FSMC_ReadWriteTimingStruct=&lcdt;
	
	lcdt.FSMC_AddressSetupTime=0;
	lcdt.FSMC_AccessMode=FSMC_AccessMode_B;
	lcdt.FSMC_AddressHoldTime=0;
	lcdt.FSMC_DataLatency=0;
	lcdt.FSMC_DataSetupTime=10;
	lcdt.FSMC_CLKDivision=0;
	lcdt.FSMC_BusTurnAroundDuration=0;
	
	FSMC_NORSRAMInit(&lcdi);
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1,ENABLE);
}

/***************************************************************
  *	@brief	相关IO口的初始化
  *	@param	void
  *	@retval	void
****************************************************************/
void LCD_GPIO_Init(void){
	GPIO_InitTypeDef rst;
	rst.GPIO_Mode=GPIO_Mode_Out_PP;
	rst.GPIO_Pin=LCD_RST_PIN;
	rst.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(LCD_RST_PORT,&rst);
	RCC_APB2PeriphClockCmd(LCD_GPIO_CLK,ENABLE);
	GPIO_InitTo_AFPP(LCD_DB0_INIT);
	GPIO_InitTo_AFPP(LCD_DB1_INIT);
	GPIO_InitTo_AFPP(LCD_DB2_INIT);
	GPIO_InitTo_AFPP(LCD_DB3_INIT);
	GPIO_InitTo_AFPP(LCD_DB4_INIT);
	GPIO_InitTo_AFPP(LCD_DB5_INIT);
	GPIO_InitTo_AFPP(LCD_DB6_INIT);
	GPIO_InitTo_AFPP(LCD_DB7_INIT);
	GPIO_InitTo_AFPP(LCD_DB8_INIT);
	GPIO_InitTo_AFPP(LCD_DB9_INIT);
	GPIO_InitTo_AFPP(LCD_DB10_INIT);
	GPIO_InitTo_AFPP(LCD_DB11_INIT);
	GPIO_InitTo_AFPP(LCD_DB12_INIT);
	GPIO_InitTo_AFPP(LCD_DB13_INIT);
	GPIO_InitTo_AFPP(LCD_DB14_INIT);
	GPIO_InitTo_AFPP(LCD_DB15_INIT);
	GPIO_InitTo_AFPP(LCD_RS_INIT);
	GPIO_InitTo_AFPP(LCD_RD_INIT);
	GPIO_InitTo_AFPP(LCD_WR_INIT);
	GPIO_InitTo_AFPP(LCD_WR_INIT);
	
}

/***************************************************************
  *	@brief	LCD的初始化
  *	@param	void
  *	@retval	void
****************************************************************/
void LCD_Init(void){
	LCD_FSMC_Init();
	LCD_GPIO_Init();
}
/***************************************************************
  *	@brief	重置LCD屏幕
  *	@param	void
  *	@retval	void
****************************************************************/
void LCD_Reset(void){
	GPIO_ResetBits(LCD_RST_PORT,LCD_RST_PIN);
	delay_ms(100);
	GPIO_SetBits(LCD_RST_PORT,LCD_RST_PIN);
}
/***************************************************************
  *	@brief	LCD测试函数
  *	@param	void
  *	@retval	void
****************************************************************/
void LCD_Demo(void){
	
}