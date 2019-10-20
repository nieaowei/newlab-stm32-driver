
/***************************************************************
  *	Name		:	MAIN.C
  *	Data		:	2019.7.31
  *	Author	:	NIEAOWEI
  *	Note		:	
****************************************************************/
#include "usart1.h"
#include "delay.h"
#include <stdio.h>
#include "Board_GLCDx.h"
#include "GLCD_Config.h"
extern GLCD_FONT GLCD_Font_16x24;
int main(){
	Byte temp[20];
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	Usart1_Init(115200);
	delay_init();
	GLCDx_Initialize();
	GLCDx_ClearScreen();
	GLCDx_SetForegroundColor  (GLCD_COLOR_WHITE);
	GLCDx_SetBackgroundColor  (GLCD_COLOR_BLUE);
	GLCDx_SetFont             (&GLCD_Font_16x24);
	GLCDx_DrawString(0,0,"nieaowei");
	GLCDx_SetBackgroundColor(GLCD_COLOR_BLACK);
	while(1){
		if(!FIFO_Is_Empty(&usart1_rx_fifo) && usart1_Receiving==0 ){
			if(FIFO_Out_Bytes(&usart1_rx_fifo,temp)!=FIFO_OP_FAIL){
				printf("%s\r\n",temp);
			}else{
				printf("running...\r\n");
				delay_ms(500);
			}
		}
		delay_ms(1000);
	}
}


