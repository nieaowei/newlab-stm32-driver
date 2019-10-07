/***************************************************************
  *	Name		:	LF125K.C
  *	Data		:	2019.7.25
  *	Author	:	NIEAOWEI
  *	Note		:	125k低频io口驱动读写，切记没有把握不要改动写的代码
							可能会写坏卡片。
****************************************************************/
#include <string.h>
#include "timer.h"
#include "sys.h"
#include "lf125k.h"
#include "stdio.h"
//卡片读写控制时序
#define LF_DBR_BASE 
#define LF_1us 72ul
#define LF_720us (720)
#define LF_650us (650)
#define LF_Star (5000)//5ms
#define LF_T557 (350)//350us
#define LF_380us (380)
#define LF_384us (384)//8*48
#define LF_350us  350
#define LF_250us (250)
#define LF_200us (200)
#define LF_128us (128)//8*18
#define LF_448us	448
#define LF_192us	192
#define LF_5ms (5000)//5ms
/*
			相关IO口配置
*/
#define LF125K_DATA_PORT 	GPIOE
#define LF125K_DATA_PIN 	GPIO_Pin_5
#define LF125K_DATA_CLK		RCC_APB2Periph_GPIOE
//读取数据
#define DATA()						GPIO_ReadInputDataBit(LF125K_DATA_PORT, LF125K_DATA_PIN)

#define LF125K_CLK_PORT 	GPIOE
#define LF125K_CLK_PIN 		GPIO_Pin_4
#define LF125K_CLK_CLK		RCC_APB2Periph_GPIOE
//设置时钟信号
#define LFCLK(n)						GPIO_WriteBit(LF125K_CLK_PORT,LF125K_CLK_PIN,(BitAction)n)
/***************************************************************
  *	@brief	初始化LF125要使用的相关IO口
  *	@param	void
  *	@retval	void
****************************************************************/
void LF125K_init(void){
	
	GPIO_InitTypeDef lf125k;
	
	RCC_APB2PeriphClockCmd(LF125K_CLK_CLK|LF125K_DATA_CLK,ENABLE);
	lf125k.GPIO_Mode=GPIO_Mode_Out_PP;
	lf125k.GPIO_Pin=LF125K_CLK_PIN;
	lf125k.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(LF125K_CLK_PORT,&lf125k);
	
	lf125k.GPIO_Mode=GPIO_Mode_IPU;
	lf125k.GPIO_Pin=LF125K_DATA_PIN;
	GPIO_Init(LF125K_DATA_PORT,&lf125k);
	LFCLK(1);
}
/***************************************************************
  *	@brief	数据接收函数，在每一次读指令后使用
  *	@param	*buf 存放读取数据的数组
  *	@retval	int8_t
									1：超时失败
									0：成功
****************************************************************/
uint8_t volue;//volue 为曼侧斯特码译码时的临时运算缓存
int8_t DATA_Processing(uint8_t *buf)
{
	uint32_t i,j,delay;
	long tus;
	for(i=0;i<50;i++){//find the first cync head
		volue=0;
		//while(DATA()==0);
		delay = 7200;
		while(DATA() == 0 && delay)//wait high level
			delay--;
		if (delay == 0) {
			return 1;//timeout
		}
		TIM_SetCounter(TIM6,0);
		while(DATA() == 1){
			if(TIM_GetCounter(TIM6) < 700*72){
				
			}else{
				i = 50;
				break;
			}
		}
   	}
	delay = 7200;
	while(DATA() == 1 && delay)//wait low level
		delay --;
	if (delay == 0) {
		return 1;
	}
	delay = 72000;
	while(DATA() == 0 && delay)//wait high level
		delay --;
	if (delay == 0) {
		return 1;
	}

	tus = LF_650us;
	TIM6_Delay_us(tus);
	tus = LF_350us;
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			if(DATA()==1)
			{
				delay = 7200;
				while(DATA()==1 && delay){
					delay --;
				}
				if(delay == 0)
					return 1;
				volue<<=1;
				volue|=0x01;
			}
			else
			{
				delay = 7200;
				while(DATA()==0 && delay){
					delay --;
				}
				if(delay == 0)
					return 1;
				volue<<=1;
				volue|=0x00;
			}
			TIM6_Delay_us(tus);
		}
		buf[i]=volue;
		volue=0x00;
	}	   
	return 0;
}

/***************************************************************
  *	@brief	发送命令给卡片
  *	@param	*data	命令存储数组
	*	@param	lenth	数组长度
  *	@retval	void
****************************************************************/
void LF125K_SendCmd(uint8_t *data,int lenth){
	uint8_t i,j,count=0,time;
	//发送数据
	//预处理
	time=6;
	while(time--){
		TIM6_Delay_us(900);
	}
	LFCLK(0);
	TIM6_Delay_us(LF_384us);
	//开始发送
	for(i=0;i<10;i++){
		for(j=0;j<8;j++){
			if((data[i]&0x80)==0){
				LFCLK(1);
				TIM6_Delay_us(LF_192us);
			}else{
				LFCLK(1);
				TIM6_Delay_us(LF_448us);
			}
			LFCLK(0);
			TIM6_Delay_us(LF_128us);
			data[i]<<=1;
			count++;
			if(count==lenth){
				i=10;
				break;
			}
		}
	}
	//发送结束，善后处理
	LFCLK(1);
	time=22;
	while(time--){
		TIM6_Delay_us(900);
	}
}
/***************************************************************
  *	@brief	读取卡片0页的数据
  *	@param	adr 地址
					@arg 	LF125_PAGE0_BANK0=0x00,
								LF125_PAGE0_BANK1=0x01,
								LF125_PAGE0_BANK2=0x02,
								LF125_PAGE0_BANK3=0x03,
								LF125_PAGE0_BANK4=0x04,
								LF125_PAGE0_BANK5=0x05,
								LF125_PAGE0_BANK6=0x06,
								LF125_PAGE0_BANK7=0x07,
		@param	*buf 数据存放的数组地址
  *	@retval	int8_t
									0：成功
									1：失败
****************************************************************/
int8_t LF125K_read_0_page(uint8_t adr,uint8_t* buf)
{	
	uint8_t cmd=0,buf2[8];
	uint8_t i=10;
	cmd|=0x80;
	cmd|=(adr<<2);
	LF125K_SendCmd(&cmd,6);
	while(i--){
		DATA_Processing(buf2);
		DATA_Processing(buf);
		if(memcmp(buf2,buf,4)!=0){
			if(i==0){
				return 1;
			}
			TIM6_Delay_us(500);
			continue;
		}else{
			break;
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	读取页1的数据
  *	@param	*buf 数据的存放地址
  *	@retval	int8_t
									0：成功
									1：失败
****************************************************************/
int8_t LF125K_read_1_page(uint8_t* buf)
{
	
	uint8_t cmd=0xc0,buf2[8];
	uint8_t i=10;
	LF125K_SendCmd(&cmd,2);
	while(i--){
		DATA_Processing(buf2);
		DATA_Processing(buf);
		if(memcmp(buf2,buf,8)!=0){
			if(i==0){
				return 1;
			}
			TIM6_Delay_us(500);
			continue;
		}else{
			break;
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	lf125k低频卡读取测试
  *	@param	void
  *	@retval	void
****************************************************************/
void LF125K_demo(void)
{
	int ret;
   uint8_t buf[8]={0xc1,0x22,0xF4,0x04,0xff,0,0,0},buf1[8];
	
	ret=LF125K_read_1_page(buf1);
	if(ret == 0)
	{
	 printf("卡号获取成功！\r\n");
	 printf("id card:%X %X %X %X %X %X %X %X\r\n", buf1[0], buf1[1], buf1[2], buf1[3], buf1[4], buf1[5], buf1[6], buf1[7]);
						
	}
	else
	{
	 printf("卡号获取失败！\r\n");
		printf("id card:%X %X %X %X %X %X %X %X\r\n", buf1[0], buf1[1], buf1[2], buf1[3], buf1[4], buf1[5], buf1[6], buf1[7]);
	}
	
	TIM6_Delay_us(900);
	ret=LF125K_write_0_page(LF125_PAGE0_BANK4,buf);
	if(ret==0){
		printf("数据写入成功!\r\n");
		printf("%X %X %X %X %X\r\n",buf[0],buf[1],buf[2],buf[3],buf[4]);
	}else{
		printf("数据写入失败!\r\n");
		printf("%X %X %X %X %X\r\n",buf[0],buf[1],buf[2],buf[3],buf[4]);
	}
	TIM6_Delay_us(900);
	ret=LF125K_read_0_page(LF125_PAGE0_BANK4,buf1);
	if(ret==0){
		printf("读取写入数据成功!\r\n");
		printf("%X %X %X %X\r\n",buf1[0],buf1[1],buf1[2],buf1[3]);
	}else{
		printf("读取写入数据失败!\r\n");
		printf("%X %X %X %X\r\n",buf1[0],buf1[1],buf1[2],buf1[3]);
	}
}
/***************************************************************
  *	@brief	对0页相关块进行写数据操作
  *	@param	adr 地址
					@arg 	LF125_PAGE0_BANK0=0x00,
								LF125_PAGE0_BANK1=0x01,
								LF125_PAGE0_BANK2=0x02,
								LF125_PAGE0_BANK3=0x03,
								LF125_PAGE0_BANK4=0x04,
								LF125_PAGE0_BANK5=0x05,
								LF125_PAGE0_BANK6=0x06,
								LF125_PAGE0_BANK7=0x07,
		@param	*buf 要写入的数据
  *	@retval	int8_t
									0：成功
									1：失败
****************************************************************/
int8_t LF125K_write_0_page(unsigned char adr,unsigned char *buf) {
	uint8_t cmd[5]={0};
	uint8_t count=0,counti=4,i,j;
	uint8_t buf1[8];
	for(i=0;i<4;i++){
		buf1[i]=buf[i];
	}
	cmd[0]=0x04;
	cmd[0]<<=1;
	for(i=0,j=0;count<32;){
		cmd[i]|=((buf[j]&0x80)>>7);
		cmd[i]<<=1;
		buf[j]<<=1;
		count++;
		counti++;
		if(count%8==0){
			j++;
		}
		if(counti%8==0){
			i++;
		}
	}
	cmd[4]=(cmd[4]<<4)|(adr<<2);
	LF125K_SendCmd(cmd,38);
	
	LF125K_read_0_page(adr,buf);
	if(memcmp(buf1,buf,4)!=0){
		return 1;
	}
	return 0;
}

