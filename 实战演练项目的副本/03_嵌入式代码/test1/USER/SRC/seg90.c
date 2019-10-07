/***************************************************************
  *	Name		:	MAIN.C
  *	Data		:	2019.7.15
  *	Author	:	NIEAOWEI
  *	Note		:	
****************************************************************/
#include "seg90.h"
#include "sys.h"

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
uint16_t CCR1_Val = TIM3_CCR_MIN;
uint16_t CCR2_Val = TIM3_CCR_MIN;
uint16_t CCR3_Val = TIM3_CCR_MIN;
uint16_t CCR4_Val = TIM3_CCR_MIN;
uint16_t PrescalerValue = 0;

/* Private function prototypes -----------------------------------------------*/
void TIM3_RCC_Configuration(void);
void TIM3_GPIO_Configuration(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  init program
  * @param  None
  * @retval None
  */
void TIM3_pwm_init(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     
       
  /* System Clocks Configuration */
  TIM3_RCC_Configuration();

  /* GPIO Configuration */
  TIM3_GPIO_Configuration();

  /* -----------------------------------------------------------------------
    TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles:
    The TIM3CLK frequency is set to SystemCoreClock (Hz), to get TIM3 counter
    clock at 3 MHz the Prescaler is computed as following:
     - Prescaler = (TIM3CLK / TIM3 counter clock) - 1
    SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
    and Connectivity line devices and to 24 MHz for Low-Density Value line and
    Medium-Density Value line devices

    The TIM3 is running at 50 Hz: TIM3 Frequency = TIM3 counter clock/(ARR + 1)
                                                  = 3 MHz / 60000 = 50 Hz
    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR)* 100 = 50%
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR)* 100 = 37.5%
    TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR)* 100 = 25%
    TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR)* 100 = 12.5%
  ----------------------------------------------------------------------- */
	
  /* Compute the prescaler value */
	
  PrescalerValue = (uint16_t) (SystemCoreClock / 3000000) - 1;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = TIM3_ARR;//设置自动重装在值
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;//设置预分频
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;//设置时钟分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//设置计数模式

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);//初始化时基

  /* PWM1 Mode configuration: Channel1 */
	
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//设置pwm1模式
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//设置输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//设置输出极性
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;//设置比较值
  
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);//初始化输出比较寄存器

  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);//启用预分频预装载模式

  /* PWM1 Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;

  TIM_OC2Init(TIM3, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;

  TIM_OC3Init(TIM3, &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;

  TIM_OC4Init(TIM3, &TIM_OCInitStructure);

  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TIM3, ENABLE);

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);

}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void TIM3_RCC_Configuration(void)
{
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
}

/**
  * @brief  Configure the TIM3 Ouput Channels.
  * @param  None
  * @retval None
  */
void TIM3_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOA Configuration:TIM3 Channel1, 2, 3 and 4 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);  //PA6(CH1)  PA7(CH2)

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; //PB0(CH3)  PB1(CH4)
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}


/**
  * @brief  set TIM3_CH1 PWM duty to control servo degree
  * @param  degree: 0 ~ 180
  
  注意: 用此函数控制舵机0时，角度只能在0度到130度，否则会烧毁舵机!!!!
  	舵机1角度可以在0度到180度。
	
	特别注意控制舵机的角度!!!!!!!!!!!!!
  */
void TIM3_CH1_set_servo_degree(unsigned char degree)
{
	uint16_t cmp_val;
	
	if(degree > 180)
		return ;

	cmp_val = (TIM3_CCR_MAX - TIM3_CCR_MIN)*degree/180 + TIM3_CCR_MIN;
	//printf("%d\r\n", cmp_val);
	TIM_SetCompare1(TIM3, cmp_val);
}

/**
  * @brief  set TIM3_CH2 PWM duty to control servo degree
  * @param  degree: 0 ~ 180
  
  注意: 用此函数控制舵机0时，角度只能在0度到130度，否则会烧毁舵机!!!!
  	舵机1角度可以在0度到180度

	特别注意控制舵机的角度!!!!!!!!!!!!!
  */
void TIM3_CH2_set_servo_degree(unsigned char degree)
{
	uint16_t cmp_val;
	
	if(degree > 180)
		return ;

	cmp_val = (TIM3_CCR_MAX - TIM3_CCR_MIN)*degree/180 + TIM3_CCR_MIN;
	//printf("%d\r\n", cmp_val);
	TIM_SetCompare2(TIM3, cmp_val);
}

/**
  * @brief  set TIM3_CH3 PWM duty to control servo degree
  * @param  degree: 0 ~ 180
  
  注意: 用此函数控制舵机0时，角度只能在0度到130度，否则会烧毁舵机!!!!
  	舵机1角度可以在0度到180度

	特别注意控制舵机的角度!!!!!!!!!!!!!
  */
void TIM3_CH3_set_servo_degree(unsigned char degree)
{
	uint16_t cmp_val;
	
	if(degree > 180)
		return ;

	cmp_val = (TIM3_CCR_MAX - TIM3_CCR_MIN)*degree/180 + TIM3_CCR_MIN;
	//printf("%d\r\n", cmp_val);
	TIM_SetCompare3(TIM3, cmp_val);
}

/**
  * @brief  set TIM3_CH4 PWM duty to control servo degree
  * @param  degree: 0 ~ 180
  
  注意: 用此函数控制舵机0时，角度只能在0度到130度，否则会烧毁舵机!!!!
  	舵机1角度可以在0度到180度

	特别注意控制舵机的角度!!!!!!!!!!!!!
  */
void TIM3_CH4_set_servo_degree(unsigned char degree)
{
	uint16_t cmp_val;
	
	if(degree > 180)
		return ;

	cmp_val = (TIM3_CCR_MAX - TIM3_CCR_MIN)*degree/180 + TIM3_CCR_MIN;
	//printf("%d\r\n", cmp_val);
	TIM_SetCompare4(TIM3, cmp_val);
}



#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  while (1)
  {}
}

#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
