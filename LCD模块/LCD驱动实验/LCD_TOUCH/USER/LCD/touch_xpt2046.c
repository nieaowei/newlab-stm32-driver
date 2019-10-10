/*-----------------------------------------------------------------------------
 * Name:    Touch_STMPE811.c
 * Purpose: Touchscreen STMPE811 interface
 * Rev.:    1.1
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2013 - 2014 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/
#include "sys.h"
#include "touch_xpt2046.h"
#include "Board_Touch.h"
#include "Board_GLCD.h"
#include "GLCD_Config.h"
#include "stm32f10x.h"
#include "delay.h"

#define	CHY 	0x90	//通道Y+的选择控制字	
#define	CHX 	0xd0 	//通道X+的选择控制字
/*-------------------引脚配置-------------------*/
#define	 SPI_CLK_PIN	  GPIO_Pin_1
#define  SPI_CLK_PORT	  GPIOE

#define	 SPI_MISO_PIN	  GPIO_Pin_3
#define	 SPI_MISO_PORT	GPIOE

#define	 SPI_MOSI_PIN	  GPIO_Pin_2
#define	 SPI_MOSI_PORT	GPIOE

#define  SPI_CS_PIN		  GPIO_Pin_0
#define  SPI_CS_PORT		GPIOE

#define  TP_INT_PIN	    GPIO_Pin_8
#define  TP_INT_PORT	  GPIOB
#define INT_IN_2046   GPIO_ReadInputDataBit(TP_INT_PORT,TP_INT_PIN)

// 触摸屏简称 TP
#define TP_DCLK(a)	\
						if (a)	\
						GPIO_SetBits(SPI_CLK_PORT,SPI_CLK_PIN);	\
						else		\
						GPIO_ResetBits(SPI_CLK_PORT,SPI_CLK_PIN)
#define TP_CS(a)	\
						if (a)	\
						GPIO_SetBits(SPI_CS_PORT,SPI_CS_PIN);	\
						else		\
						GPIO_ResetBits(SPI_CS_PORT,SPI_CS_PIN)
#define TP_DIN(a)	\
						if (a)	\
						GPIO_SetBits(SPI_MISO_PORT,SPI_MOSI_PIN);	\
						else		\
						GPIO_ResetBits(SPI_MISO_PORT,SPI_MOSI_PIN)

#define TP_DOUT		 GPIO_ReadInputDataBit(SPI_MISO_PORT,SPI_MISO_PIN)

static volatile unsigned char touch_flag;

int tp_get_flag(void);
void tp_set_flag(int status);

/*--------------------------------spi config---------------------------------*/

/**
  * @brief  
  * @param  None
  * @retval None
  */
/*
 * XPT 2046 IRQ 引脚配置
 * 当IRQ  换成其他的引脚时，需要修改下面的两个函数
 * 同时stm32f10x_it.c中的中断服务函数也应修改
 */
static void tp_int_nvic_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 

    /*使能EXTI9_5 中断 */		      
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn  ;    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static void tp_int_gpio_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline clock and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
												
	/* config the NVIC */
	tp_int_nvic_config();

	/* EXTI line gpio config */	
  GPIO_InitStructure.GPIO_Pin = TP_INT_PIN;       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // 上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(TP_INT_PORT, &GPIO_InitStructure);

	/* EXTI line mode config */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource8);  // PB8 
  EXTI_InitStructure.EXTI_Line = EXTI_Line8;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure); 
}

// EXTI Line --> PB8
void EXTI9_5_IRQHandler(void)
{ 
  if(EXTI_GetITStatus(EXTI_Line8) != RESET)
  {	
    EXTI_ClearITPendingBit(EXTI_Line8);
    tp_set_flag(1);
  }
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
/*------------------------------------------------------------------------------------------------------
 * 模拟SPI的GPIO配置 当SPI的4根信号线换成其他的IO时候，只需要修改该函数头文件的宏定义即可
 */
void tp_gpio_config(void)
{ 
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* 开启GPIO时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE);

  /* 模拟SPI GPIO初始化 */          
  GPIO_InitStructure.GPIO_Pin=SPI_CLK_PIN;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz ;	  
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_Init(SPI_CLK_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PIN;
  GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      
  GPIO_Init(SPI_MISO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = SPI_CS_PIN; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      
  GPIO_Init(SPI_CS_PORT, &GPIO_InitStructure); 
   
  /* XPT2046 中断IO配置 */
  tp_int_gpio_config();
	
  /* 拉低片选，选择XPT2046 */
//  GPIO_ResetBits(SPI_CS_PORT,SPI_CS_PIN);
  //GPIO_SetBits(SPI_CS_PORT,SPI_CS_PIN);
}

/*--------------------------------spi config end---------------------------------*/

/**
  * @brief  
  * @param  None
  * @retval None
  */
int tp_get_flag(void)
{
	return (int)touch_flag;
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void tp_set_flag(int status)
{
	touch_flag = status;
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
/*
*********************************************************************************************************
*	函 数 名: tp_write_cmd
*	功能说明: 写命令
*	形    参：CHX 	0x90 	//通道Y+的选择控制字 CHY 	0xd0	//通道X+的选择控制字
*	返 回 值: 无
*********************************************************************************************************
*/
static void tp_write_cmd(unsigned char cmd) 
{
    unsigned char buf;
    unsigned char i;
    TP_CS(0);
    TP_DIN(0);
    TP_DCLK(0);
    for(i=0;i<8;i++) 
    {
        buf=(cmd>>(7-i))&0x1;
        TP_DIN(buf);
				delay_us(5);
        TP_DCLK(1);
				delay_us(5);
        TP_DCLK(0);
    }
    //TP_CS(1);
}


/**
  * @brief  
  * @param  None
  * @retval None
  */
/*
*********************************************************************************************************
*	函 数 名: tp_read_cmd
*	功能说明: 选择一个模拟通道，启动ADC，并返回ADC采样结果
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/

static unsigned short tp_read_cmd(void) 
{
    unsigned short buf=0,temp;
    unsigned char i;
	TP_CS(0);
    TP_DIN(0);
    TP_DCLK(1);
    for(i=0;i<12;i++) 
    {
        TP_DCLK(0);          
        temp= (TP_DOUT) ? 1:0;
        buf|=(temp<<(11-i));
        TP_DCLK(1);
    }
    buf&=0x0fff;
		TP_CS(1);
    return(buf);
}


/**
  * @brief  
  * @param  None
  * @retval None
  */
/*
*********************************************************************************************************
*	函 数 名: tp_read_adc
*	功能说明: 选择一个模拟通道，启动ADC，并返回ADC采样结果
*	形    参：_ucCh = 0x90 表示Y通道； 0xd0 表示X通道
*	返 回 值: 12位ADC值
*********************************************************************************************************
*/
uint16_t tp_read_adc(uint8_t _ucCh)
{
    tp_write_cmd(_ucCh);

	return 	tp_read_cmd();
}


/**
  * @brief  
  * @param  None
  * @retval None
  */
/*
 * 读取TP x y 的AD值(12bit，最大是4096)
 */
void tp_get_ad_xy(int *x,int *y)  
{ 
    int adx,ady; 
    adx = tp_read_adc(CHX);
  
    delay_us(1); 
    ady = tp_read_adc(CHY);  
    *x=adx; 
    *y=ady; 
}

/*---------------------------------emWin touch 接口---------------------------------------*/
/**
  \fn          int32_t Touch_Initialize (void)
  \brief       Initialize touchscreen
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Touch_Initialize (void) {
  
	tp_gpio_config();
  return 0;
}

/**
  \fn          int32_t Touch_Uninitialize (void)
  \brief       De-initialize touchscreen
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Touch_Uninitialize (void) {
  return 0;
}

/**
  \fn          int32_t Touch_GetState (TOUCH_STATE *pState)
  \brief       Get touchscreen state
  \param[out]  pState  pointer to TOUCH_STATE structure
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Touch_GetState (TOUCH_STATE *pState) {
  int adx,ady; 
	
  /* Read touch status */
  pState->pressed = !GPIO_ReadInputDataBit(TP_INT_PORT,TP_INT_PIN);
  
  if (pState->pressed) {
    adx = tp_read_adc(CHX);
    delay_us(1); 
    ady = tp_read_adc(CHY); 
		pState->x = adx;
		pState->y = ady;
  }

  return 0;
}

/*----------------------------------不使用GUI时调用的接口------------------------------------*/
/* 触摸屏校正系数 */
#if 1
long double aa1=0,bb1=0,cc1=0,aa2=0,bb2=0,cc2=0;
#elif 0
long double aa1=0.088370,\
            bb1=-0.000468,\
            cc1=-24.042172,\
            aa2=0.0001891,\
            bb2=0.062395,\
            cc2=-10.223455;
#endif

/* 差值门限 */
#define THRESHOLD 2 

/*--------------------------------------------------------------------*/

/* 触摸采样AD值保存结构体 */
Coordinate ScreenSample[4];

/* LCD逻辑坐标，用来校正时画十字用 */
Coordinate DisplaySample[4] =   
{
    { 35,   45},
    { 200,  20},
    { 200, 290},
    { 35, 200}
};
/* 用于保存校正系数 */
Parameter   touch_para ;

/* 液晶计算坐标，用于真正画点的时候用 */
Coordinate  display ;


/**
  * @brief  
  * @param  None
  * @retval None
  */
/******************************************************
* 函数名：Read_2046
* 描述  ：得到简单滤波之后的X Y
* 输入  : 无
* 输出  ：Coordinate结构体地址
* 举例  ：无
* 注意  ：”画板应用实例"专用,不是很精准，但是速度比较快
*********************************************************/    
Coordinate *tp_read_ad_xy_filter(void)
{
    static Coordinate  screen2;
    int TP_X[1],TP_Y[1];
    uint8_t count=0;
    int buffer[2][10]={{0},{0}};  /*坐标X和Y进行多次采样*/
    int min_x,max_x;
    int min_y,max_y;
    int	i=0;
    
    do					       				
    {		/* 循环采样10次 */   
        tp_get_ad_xy(TP_X,TP_Y);  
        buffer[0][count]=TP_X[0];  
        buffer[1][count]=TP_Y[0];
        count++;  
    }	/*用户点击触摸屏时即TP_INT_IN信号为低 并且 count<10*/
    while(!INT_IN_2046&& count<10);
    
		/*如果触笔弹起*/
    if(INT_IN_2046)						
    {
				/*中断标志复位*/
        tp_set_flag(0);					 
    }
		
		/*如果成功采样10个样本*/
    if(count ==10)		 					
    {
        max_x=min_x=buffer[0][0];
        max_y=min_y=buffer[1][0];       
        for(i=1; i<10; i++)
        {
            if(buffer[0][i]<min_x)
            {
                min_x=buffer[0][i];
            }
            else
            if(buffer[0][i]>max_x)
            {
                max_x = buffer[0][i];
            }
        }
        
        for(i=1; i<10; i++)
        {
            if(buffer[1][i]<min_y)
            {
                min_y=buffer[1][i];
            }
            else
            if(buffer[1][i]>max_y)
            {
                max_y = buffer[1][i];
            }
        }
				/*去除最小值和最大值之后求平均值*/
        screen2.x=(buffer[0][0]+buffer[0][1]+buffer[0][2]+buffer[0][3]+buffer[0][4]+buffer[0][5]+buffer[0][6]+buffer[0][7]+buffer[0][8]+buffer[0][9]-min_x-max_x)>>3;
        screen2.y=(buffer[1][0]+buffer[1][1]+buffer[1][2]+buffer[1][3]+buffer[1][4]+buffer[1][5]+buffer[1][6]+buffer[1][7]+buffer[1][8]+buffer[1][9]-min_y-max_y)>>3; 
        
        return &screen2;
    }    
    return 0;    
}


/**
  * @brief  
  * @param  None
  * @retval None
  */
/******************************************************
* 函数名：cal_touch_para
* 描述  ：计算出触摸屏到液晶屏坐标变换的转换函数的 K A B C D E F系数
* 输入  : 无
* 输出  ：返回1表示成功 0失败
* 举例  ：无
* 注意  ：只有在LCD和触摸屏间的误差角度非常小时,才能运用下面公式
*********************************************************/    
static FunctionalState cal_touch_para( Coordinate * displayPtr,
                                Coordinate * screenPtr,
                                Parameter * para)
{
    
    FunctionalState retTHRESHOLD = ENABLE ;

    /* K＝(X0－X2) (Y1－Y2)－(X1－X2) (Y0－Y2) */
    para->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
    
		if( para->Divider == 0 )
    {
        retTHRESHOLD = DISABLE;
    }
    else
    {
        /* A＝((XD0－XD2) (Y1－Y2)－(XD1－XD2) (Y0－Y2))／K	*/
        para->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                   ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y));
        
        /* B＝((X0－X2) (XD1－XD2)－(XD0－XD2) (X1－X2))／K	*/
        para->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) - 
                   ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x));
        
        /* C＝(Y0(X2XD1－X1XD2)+Y1(X0XD2－X2XD0)+Y2(X1XD0－X0XD1))／K */
        para->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                   (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                   (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;
        
        /* D＝((YD0－YD2) (Y1－Y2)－(YD1－YD2) (Y0－Y2))／K	*/
        para->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) - 
                   ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;
        
        /* E＝((X0－X2) (YD1－YD2)－(YD0－YD2) (X1－X2))／K	*/
        para->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) - 
                   ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;
        
        
        /* F＝(Y0(X2YD1－X1YD2)+Y1(X0YD2－X2YD0)+Y2(X1YD0－X0YD1))／K */
        para->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                   (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                   (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y;
        
    }
    return( retTHRESHOLD ) ;
}


/**
  * @brief  
  * @param  None
  * @retval None
  */
/*
 * 校正触摸时画十字专用 
 * x:0~300
 * y:0~230
 */
void DrawCross(uint16_t x,uint16_t y)
{
	GLCD_SetForegroundColor(GLCD_COLOR_RED);
	GLCD_DrawHLine(x-10, y, 20);
	GLCD_DrawVLine(x, y-10, 20);
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
/******************************************************
* 函数名：Touchl_Calibrate
* 描述  ：触摸屏校正函数
* 输入  : 无
* 输出  ：0	---	校正成功
					1	---	校正失败
* 举例  ：无
* 注意  ：无
*********************************************************/ 
int tp_calibrate(void)
{
    #if 1
    uint8_t i;
    u16 test_x=0, test_y=0;
    u16 gap_x=0, gap_y=0;
    Coordinate * Ptr;   
		GLCD_SetForegroundColor  (GLCD_COLOR_BLACK);
		GLCD_SetBackgroundColor  (GLCD_COLOR_WHITE);  
	
    for(i=0; i<4; i++)
    {        
        GLCD_ClearScreen();
      
        /* 适当的延时很有必要 */        
        delay_ms(500);     
        DrawCross(DisplaySample[i].x,DisplaySample[i].y);  //显示校正用的“十”字
        do
        {
            Ptr=tp_read_ad_xy_filter();        //读取XPT2046数据到变量ptr            
        }
        while( Ptr == (void*)0 );     //当ptr为空时表示没有触点被按下
        ScreenSample[i].x= Ptr->x; 	  //把读取的原始数据存放到全局变量ScreenSample结构体
        ScreenSample[i].y= Ptr->y;

    }
		/* 用原始参数计算出 原始参数与坐标的转换系数。 */
    cal_touch_para( &DisplaySample[0],&ScreenSample[0],&touch_para ) ;  	   
    
		/*取一个点计算X值*/
    test_x = ( (touch_para.An * ScreenSample[3].x) + 
               (touch_para.Bn * ScreenSample[3].y) + 
                touch_para.Cn 
             ) / touch_para.Divider ;			 
    
		/*取一个点计算Y值*/
    test_y = ( (touch_para.Dn * ScreenSample[3].x) + 
               (touch_para.En * ScreenSample[3].y) + 
               touch_para.Fn 
             ) / touch_para.Divider ;
    
    /* 实际坐标与计算坐标的差 */
    gap_x = (test_x > DisplaySample[3].x)?(test_x - DisplaySample[3].x):(DisplaySample[3].x - test_x);
    gap_y = (test_y > DisplaySample[3].y)?(test_y - DisplaySample[3].y):(DisplaySample[3].y - test_y);
    

    //LCD_Rectangle(0,0,320,240,CAL_BACKGROUND_COLOR);
    GLCD_ClearScreen();
    
    /* 可以通过修改这两个值的大小来调整精度 */
    if((gap_x>100)||(gap_y>100))
    {
      delay_ms(2000);
      return 1;
    }    
    
    /* 校正系数为全局变量 */
    aa1 = (touch_para.An*1.0)/touch_para.Divider;
    bb1 = (touch_para.Bn*1.0)/touch_para.Divider;
    cc1 = (touch_para.Cn*1.0)/touch_para.Divider;
    
    aa2 = (touch_para.Dn*1.0)/touch_para.Divider;
    bb2 = (touch_para.En*1.0)/touch_para.Divider;
    cc2 = (touch_para.Fn*1.0)/touch_para.Divider;
    
    #elif 0
    aa1=0.088370;
    bb1=-0.000468;
    cc1=-24.042172;
    aa2=0.0001891;
    bb2=0.062395;
    cc2=-10.223455;
    
    #endif     
    return 0;    
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
/******************************************************
* 函数名：get_touch_point
* 描述  ：通过 K A B C D E F 把通道X Y的值转换为液晶屏坐标
* 输入  : 无
* 输出  ：返回1表示成功 0失败
* 举例  ：无
* 注意  ：如果获取的触点信息有误，将返回DISABLE
*********************************************************/    
FunctionalState get_touch_point(Coordinate * displayPtr,
                                Coordinate * screenPtr,
                                Parameter * para )
{
  FunctionalState retTHRESHOLD =ENABLE ;

  if(screenPtr==0) {
    /*如果获取的触点信息有误，则返回DISABLE*/
    retTHRESHOLD = DISABLE;			
  }
  else
  {    
    if( para->Divider != 0 )
    {        
      displayPtr->x = ( (aa1 * screenPtr->x) + (bb1 * screenPtr->y) + cc1);        
      displayPtr->y = ((aa2 * screenPtr->x) + (bb2 * screenPtr->y) + cc2 );
    }
    else
    {
      retTHRESHOLD = DISABLE;
    }
  }
  return(retTHRESHOLD);
} 

