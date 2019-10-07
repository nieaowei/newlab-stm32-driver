/***************************************************************
  *	Name		:	USART3.C
  *	Data		:	2019.7.15
  *	Author	:	NIEAOWEI
  *	Note		:	串口3的相关操作函数，及定义
****************************************************************/
#include "usart3.h"
#include "sys.h"

//串口3的引脚，时钟配置
#define USART3_RX_PORT	GPIOB
#define USART3_RX_PIN		GPIO_Pin_11
#define USART3_RX_CLK		RCC_APB2Periph_GPIOB

#define USART3_TX_PORT	GPIOB
#define USART3_TX_PIN		GPIO_Pin_10
#define USART3_TX_CLK		RCC_APB2Periph_GPIOB

#define USART3_CLK			RCC_APB1Periph_USART3

//接收缓存队列以及接收标志
FIFO_Type usart3_rx_fifo;
uint8_t 	usart3_receiving;
/***************************************************************
  *	@brief	串口三的相关io和外设的初始化
  *	@param	void
  *	@retval	void
****************************************************************/
void usart3_init(){
	GPIO_InitTypeDef usart3;
	
	USART_InitTypeDef usart3_;
	
	NVIC_InitTypeDef usart3_it;
	//时钟初始化
	RCC_APB1PeriphClockCmd(USART3_CLK,ENABLE);
	RCC_APB2PeriphClockCmd(USART3_RX_CLK|USART3_TX_CLK,ENABLE);
	//io口参数配置及初始化
	usart3.GPIO_Mode=GPIO_Mode_AF_PP;
	usart3.GPIO_Speed=GPIO_Speed_50MHz;
	usart3.GPIO_Pin=USART3_TX_PIN;
	GPIO_Init(USART3_TX_PORT,&usart3);
	
	usart3.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	usart3.GPIO_Pin=USART3_RX_PIN;
	GPIO_Init(USART3_RX_PORT,&usart3);
	//串口参数配置
	usart3_.USART_BaudRate=115200;
	usart3_.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	usart3_.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	usart3_.USART_Parity=USART_Parity_No;
	usart3_.USART_StopBits=USART_StopBits_1;
	usart3_.USART_WordLength=USART_WordLength_8b;
	
	//中断参数配置
	usart3_it.NVIC_IRQChannel=USART3_IRQn;
	usart3_it.NVIC_IRQChannelSubPriority=3;
	usart3_it.NVIC_IRQChannelPreemptionPriority=3;
	usart3_it.NVIC_IRQChannelCmd=ENABLE;
	
	NVIC_Init(&usart3_it);
	
	USART_Init(USART3,&usart3_);
	//启用接收中断
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	//使能串口3
	USART_Cmd(USART3,ENABLE);
	//初始化缓存队列
	FIFO_Init(&usart3_rx_fifo);
	
}
/***************************************************************
  *	@brief	串口3的中断函数，用于接收数据，存储到对应缓存队列
  *	@param	void
  *	@retval	void
****************************************************************/
void USART3_IRQHandler(void){
	static uint8_t current;
	if(USART_GetITStatus(USART3,USART_IT_RXNE)==SET){//如果是接收完成中断
		current=USART_ReceiveData(USART3);//读取接受到的数据存取current
		FIFO_In_Byte(&usart3_rx_fifo,current);//放到缓存队列里面
	}
}
/***************************************************************
	*	@brief	串口3发送lenth个字节的数据
	*	@param	*data 发送的数据指针
	* @param	lenth	发送的数据长度
  *	@retval	void
****************************************************************/
void usart3_send_str(uint8_t *data,uint16_t lenth){
	
	while(lenth--){//如果有数据，进入循环，没有则结束循环
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)!=SET){};//判断发送寄存器空，如果空，就结束循环，不空继续循环
			USART_SendData(USART3,*data);//发送一个字节的数据出去
			data++;
	}
}

