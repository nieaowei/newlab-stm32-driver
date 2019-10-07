
/***************************************************************
  *	Name		:	USART1.C
  *	Data		:	2019.7.10
  *	Author	:	NIEAOWEI
  *	Note		:	串口1的使用
****************************************************************/

#include "usart1.h"

//串口相关配置
#define	USART1_RX_PORT	GPIOA
#define USART1_TX_PORT	GPIOA

#define USART1_RX_PIN		GPIO_Pin_10
#define USART1_TX_PIN		GPIO_Pin_9

#define USART1_RX_CLK		RCC_APB2Periph_GPIOA
#define USART1_TX_CLK		RCC_APB2Periph_GPIOA

#define USART1_CLK			RCC_APB2Periph_USART1|USART1_RX_CLK|USART1_TX_CLK


FIFO_Type usart1_rx_fifo;//接收缓存队列
uint8_t usart1_Receiving=0;//以新行为标志，设置数据接收完的标志

#pragma import(__use_no_semihosting)                            
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//??_sys_exit()??????????    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//???fputc?? 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0){};//????,??????   
    USART1->DR = (u8) ch;      
	return ch;
}

/***************************************************************
  *	@brief	初始化串口1，
  *	@param	baudrate 波特率
  *	@retval	void
****************************************************************/
void Usart1_Init(uint32_t baudrate){
	GPIO_InitTypeDef usartIO;
	
	NVIC_InitTypeDef usartIRQ;
	
	USART_InitTypeDef	usart;
	//Enable the clock of the relevant IO port.
	RCC_APB2PeriphClockCmd(USART1_CLK,ENABLE);
	//Set the parameters of the transmition IO port.
	usartIO.GPIO_Mode=GPIO_Mode_AF_PP;
	usartIO.GPIO_Pin=USART1_TX_PIN;
	usartIO.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(USART1_TX_PORT,&usartIO);
	//Set the parameters of the receive IO port.
	usartIO.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	usartIO.GPIO_Pin=USART1_RX_PIN;
	
	GPIO_Init(USART1_RX_PORT,&usartIO);
	//Set the interrupt parameters of the serial port.
	usartIRQ.NVIC_IRQChannel=USART1_IRQn;
	usartIRQ.NVIC_IRQChannelPreemptionPriority=2;
	usartIRQ.NVIC_IRQChannelSubPriority=2;
	usartIRQ.NVIC_IRQChannelCmd=ENABLE;
	
	NVIC_Init(&usartIRQ);
	
	//Set the relvant parameters of the serial port.
	usart.USART_BaudRate=baudrate;
	usart.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	usart.USART_StopBits=USART_StopBits_1;
	usart.USART_WordLength=USART_WordLength_8b;
	usart.USART_Parity=USART_Parity_No;
	usart.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	
	USART_Init(USART1,&usart);
	//Enable the interrupt of receiving.
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	FIFO_Init(&usart1_rx_fifo);
	//Eanble the serial port.
	USART_Cmd(USART1,ENABLE);
	
}

/***************************************************************
  *	@brief	串口1的中断函数，主要用于接收数据
  *	@param	
  *	@retval	
****************************************************************/
void USART1_IRQHandler(void){
	static uint8_t former=0;
	static uint8_t current=0;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)){
		former=current;
		current=USART_ReceiveData(USART1);
		FIFO_In_Byte(&usart1_rx_fifo,current);
		if(former==0x0d && current==0x0a){
			usart1_Receiving=0;
			
		}else{
			usart1_Receiving=1;
		}
	}

}


