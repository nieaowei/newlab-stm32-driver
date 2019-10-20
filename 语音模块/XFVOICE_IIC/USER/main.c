
/***************************************************************
  *	Name		:	MAIN.C
  *	Data		:	2019.7.31
  *	Author	:	nieaowei
  *	Note		:	该实验为IIC通信控制语音模块
****************************************************************/
#include "usart1.h"
#include "delay.h"
#include <stdio.h>
#include "xfs5152ce.h"

int main(){
	Byte data[10];
	uint8_t a;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	Usart1_Init(115200);
	delay_init();
	XFS_Init();
	delay_us(100);
	XFS_Play((unsigned char *)"请说出启动指令",XFS_FORMAT_GBK);
	XFS_Play((unsigned char *)"请说出启动指令",XFS_FORMAT_GBK);
	XFS_Play((unsigned char *)"请说出启动指令",XFS_FORMAT_GBK);
	
	//*delay_ms(1000);
	if(XFS_Recogonize(&a, 4000) == 1)
	{
		XFS_Play((unsigned char *)"语音识别成功", XFS_FORMAT_GBK);
		sprintf((char *)data,"%d",a);
		XFS_Play((unsigned char *)data,XFS_FORMAT_GBK);
		XFS_Play((unsigned char *)cmd_word_list[a],XFS_FORMAT_GBK);
	}else{
		XFS_Play((unsigned char *)"语音识别失败",XFS_FORMAT_GBK);
	}
	while(1){

	}
	
}
