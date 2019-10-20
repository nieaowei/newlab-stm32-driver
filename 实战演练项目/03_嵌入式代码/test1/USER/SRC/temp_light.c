/***************************************************************
  *	Name		:	TEMP_LIGHT.C
  *	Data		:	2019.10.5
  *	Author	:	NIEAOWEI
  *	Note		:	温度光照采集
****************************************************************/

#include "temp_light.h"

#include "sys.h"

#define TEMP_PORT	GPIOC
#define TEMP_PIN	GPIO_Pin_0
#define TEMP_CLK	RCC_APB2Periph_GPIOC

#define	LIGHT_PORT	GPIOC
#define	LIGHT_PIN		GPIO_Pin_1
#define LIGHT_CLK		RCC_APB2Periph_GPIOC

#define TEMP_LIGHT_ADC			ADC1
#define TEMP_LIGHT_ADC_CLK 	RCC_APB2Periph_ADC1

#define	LIGHT_CH	ADC_Channel_11
#define TEMP_CH		ADC_Channel_10

uint8_t temp_value;


/*
	ntc热敏电阻参考值
*/
#define AMPLIFY_VALUE ((uint16_t)(1000))
const uint32_t RES_TEMP_Table[] =//对应温度的电阻值，单位：Ω
{
	(uint32_t)(98.26*AMPLIFY_VALUE),//摄氏度：-20
	(uint32_t)(92.74*AMPLIFY_VALUE),//摄氏度：-19
	(uint32_t)(87.55*AMPLIFY_VALUE),//摄氏度：-18
	(uint32_t)(82.69*AMPLIFY_VALUE),//摄氏度：-17
	(uint32_t)(78.12*AMPLIFY_VALUE),//摄氏度：-16
	(uint32_t)(73.83*AMPLIFY_VALUE),//摄氏度：-15
	(uint32_t)(69.80*AMPLIFY_VALUE),//摄氏度：-14
	(uint32_t)(66.01*AMPLIFY_VALUE),//摄氏度：-13
	(uint32_t)(62.45*AMPLIFY_VALUE),//摄氏度：-12
	(uint32_t)(59.10*AMPLIFY_VALUE),//摄氏度：-11
	(uint32_t)(55.95*AMPLIFY_VALUE),//摄氏度：-10
	(uint32_t)(52.98*AMPLIFY_VALUE),//摄氏度：-9
	(uint32_t)(50.19*AMPLIFY_VALUE),//摄氏度：-8
	(uint32_t)(47.57*AMPLIFY_VALUE),//摄氏度：-7
	(uint32_t)(45.09*AMPLIFY_VALUE),//摄氏度：-6
	(uint32_t)(42.76*AMPLIFY_VALUE),//摄氏度：-5
	(uint32_t)(40.56*AMPLIFY_VALUE),//摄氏度：-4
	(uint32_t)(38.49*AMPLIFY_VALUE),//摄氏度：-3
	(uint32_t)(36.53*AMPLIFY_VALUE),//摄氏度：-2
	(uint32_t)(34.68*AMPLIFY_VALUE),//摄氏度：-1
	(uint32_t)(32.94*AMPLIFY_VALUE),//摄氏度：0
	(uint32_t)(31.30*AMPLIFY_VALUE),//摄氏度：1
	(uint32_t)(29.74*AMPLIFY_VALUE),//摄氏度：2
	(uint32_t)(28.28*AMPLIFY_VALUE),//摄氏度：3
	(uint32_t)(26.89*AMPLIFY_VALUE),//摄氏度：4
	(uint32_t)(25.58*AMPLIFY_VALUE),//摄氏度：5
	(uint32_t)(24.34*AMPLIFY_VALUE),//摄氏度：6
	(uint32_t)(23.17*AMPLIFY_VALUE),//摄氏度：7
	(uint32_t)(22.06*AMPLIFY_VALUE),//摄氏度：8
	(uint32_t)(21.01*AMPLIFY_VALUE),//摄氏度：9
	(uint32_t)(20.01*AMPLIFY_VALUE),//摄氏度：10
	(uint32_t)(19.07*AMPLIFY_VALUE),//摄氏度：11
	(uint32_t)(18.18*AMPLIFY_VALUE),//摄氏度：12
	(uint32_t)(17.33*AMPLIFY_VALUE),//摄氏度：13
	(uint32_t)(16.53*AMPLIFY_VALUE),//摄氏度：14
	(uint32_t)(15.77*AMPLIFY_VALUE),//摄氏度：15
	(uint32_t)(15.05*AMPLIFY_VALUE),//摄氏度：16
	(uint32_t)(14.37*AMPLIFY_VALUE),//摄氏度：17
	(uint32_t)(13.72*AMPLIFY_VALUE),//摄氏度：18
	(uint32_t)(13.10*AMPLIFY_VALUE),//摄氏度：19
	(uint32_t)(12.52*AMPLIFY_VALUE),//摄氏度：20
	(uint32_t)(11.96*AMPLIFY_VALUE),//摄氏度：21
	(uint32_t)(11.43*AMPLIFY_VALUE),//摄氏度：22
	(uint32_t)(10.93*AMPLIFY_VALUE),//摄氏度：23
	(uint32_t)(10.45*AMPLIFY_VALUE),//摄氏度：24
	(uint32_t)(10.00*AMPLIFY_VALUE),//摄氏度：25
	(uint32_t)(9.569*AMPLIFY_VALUE),//摄氏度：26
	(uint32_t)(9.158*AMPLIFY_VALUE),//摄氏度：27
	(uint32_t)(8.768*AMPLIFY_VALUE),//摄氏度：28
	(uint32_t)(8.396*AMPLIFY_VALUE),//摄氏度：29
	(uint32_t)(8.042*AMPLIFY_VALUE),//摄氏度：30
	(uint32_t)(7.705*AMPLIFY_VALUE),//摄氏度：31
	(uint32_t)(7.384*AMPLIFY_VALUE),//摄氏度：32
	(uint32_t)(7.078*AMPLIFY_VALUE),//摄氏度：33
	(uint32_t)(6.786*AMPLIFY_VALUE),//摄氏度：34
	(uint32_t)(6.508*AMPLIFY_VALUE),//摄氏度：35
	(uint32_t)(6.243*AMPLIFY_VALUE),//摄氏度：36
	(uint32_t)(5.990*AMPLIFY_VALUE),//摄氏度：37
	(uint32_t)(5.749*AMPLIFY_VALUE),//摄氏度：38
	(uint32_t)(5.519*AMPLIFY_VALUE),//摄氏度：39
	(uint32_t)(5.299*AMPLIFY_VALUE),//摄氏度：40
	(uint32_t)(5.089*AMPLIFY_VALUE),//摄氏度：41
	(uint32_t)(4.889*AMPLIFY_VALUE),//摄氏度：42
	(uint32_t)(4.697*AMPLIFY_VALUE),//摄氏度：43
	(uint32_t)(4.514*AMPLIFY_VALUE),//摄氏度：44
	(uint32_t)(4.339*AMPLIFY_VALUE),//摄氏度：45
	(uint32_t)(4.172*AMPLIFY_VALUE),//摄氏度：46
	(uint32_t)(4.012*AMPLIFY_VALUE),//摄氏度：47
	(uint32_t)(3.860*AMPLIFY_VALUE),//摄氏度：48
	(uint32_t)(3.713*AMPLIFY_VALUE),//摄氏度：49
	(uint32_t)(3.574*AMPLIFY_VALUE),//摄氏度：50
	(uint32_t)(3.440*AMPLIFY_VALUE),//摄氏度：51
	(uint32_t)(3.312*AMPLIFY_VALUE),//摄氏度：52
	(uint32_t)(3.189*AMPLIFY_VALUE),//摄氏度：53
	(uint32_t)(3.071*AMPLIFY_VALUE),//摄氏度：54
	(uint32_t)(2.959*AMPLIFY_VALUE),//摄氏度：55
	(uint32_t)(2.851*AMPLIFY_VALUE),//摄氏度：56
	(uint32_t)(2.748*AMPLIFY_VALUE),//摄氏度：57
	(uint32_t)(2.649*AMPLIFY_VALUE),//摄氏度：58
	(uint32_t)(2.554*AMPLIFY_VALUE),//摄氏度：59
	(uint32_t)(2.463*AMPLIFY_VALUE),//摄氏度：60
	(uint32_t)(2.375*AMPLIFY_VALUE),//摄氏度：61
	(uint32_t)(2.292*AMPLIFY_VALUE),//摄氏度：62
	(uint32_t)(2.211*AMPLIFY_VALUE),//摄氏度：63
	(uint32_t)(2.134*AMPLIFY_VALUE),//摄氏度：64
	(uint32_t)(2.060*AMPLIFY_VALUE),//摄氏度：65
	(uint32_t)(1.989*AMPLIFY_VALUE),//摄氏度：66
	(uint32_t)(1.921*AMPLIFY_VALUE),//摄氏度：67
	(uint32_t)(1.855*AMPLIFY_VALUE),//摄氏度：68
	(uint32_t)(1.792*AMPLIFY_VALUE),//摄氏度：69
	(uint32_t)(1.731*AMPLIFY_VALUE),//摄氏度：70
	(uint32_t)(1.673*AMPLIFY_VALUE),//摄氏度：71
	(uint32_t)(1.617*AMPLIFY_VALUE),//摄氏度：72
	(uint32_t)(1.563*AMPLIFY_VALUE),//摄氏度：73
	(uint32_t)(1.512*AMPLIFY_VALUE),//摄氏度：74
	(uint32_t)(1.462*AMPLIFY_VALUE),//摄氏度：75
	(uint32_t)(1.414*AMPLIFY_VALUE),//摄氏度：76
	(uint32_t)(1.368*AMPLIFY_VALUE),//摄氏度：77
	(uint32_t)(1.324*AMPLIFY_VALUE),//摄氏度：78
	(uint32_t)(1.281*AMPLIFY_VALUE),//摄氏度：79
	(uint32_t)(1.240*AMPLIFY_VALUE),//摄氏度：80
	(uint32_t)(1.201*AMPLIFY_VALUE),//摄氏度：81
	(uint32_t)(1.163*AMPLIFY_VALUE),//摄氏度：82
	(uint32_t)(1.126*AMPLIFY_VALUE),//摄氏度：83
	(uint32_t)(1.091*AMPLIFY_VALUE),//摄氏度：84
	(uint32_t)(1.056*AMPLIFY_VALUE),//摄氏度：85
	(uint32_t)(1.024*AMPLIFY_VALUE),//摄氏度：86
	(uint32_t)(0.992*AMPLIFY_VALUE),//摄氏度：87
	(uint32_t)(0.9615*AMPLIFY_VALUE),//摄氏度：88
	(uint32_t)(0.9321*AMPLIFY_VALUE),//摄氏度：89
	(uint32_t)(0.9038*AMPLIFY_VALUE),//摄氏度：90
	(uint32_t)(0.8764*AMPLIFY_VALUE),//摄氏度：91
	(uint32_t)(0.8500*AMPLIFY_VALUE),//摄氏度：92
	(uint32_t)(0.8245*AMPLIFY_VALUE),//摄氏度：93
	(uint32_t)(0.8000*AMPLIFY_VALUE),//摄氏度：94
	(uint32_t)(0.7762*AMPLIFY_VALUE),//摄氏度：95
	(uint32_t)(0.7533*AMPLIFY_VALUE),//摄氏度：96
	(uint32_t)(0.7312*AMPLIFY_VALUE),//摄氏度：97
	(uint32_t)(0.7099*AMPLIFY_VALUE),//摄氏度：98
	(uint32_t)(0.6892*AMPLIFY_VALUE),//摄氏度：99
	(uint32_t)(0.6693*AMPLIFY_VALUE),//摄氏度：100
	(uint32_t)(0.6500*AMPLIFY_VALUE),//摄氏度：101
	(uint32_t)(0.6314*AMPLIFY_VALUE),//摄氏度：102
	(uint32_t)(0.6135*AMPLIFY_VALUE),//摄氏度：103
	(uint32_t)(0.5961*AMPLIFY_VALUE),//摄氏度：104
	(uint32_t)(0.5793*AMPLIFY_VALUE),//摄氏度：105
	(uint32_t)(0.5630*AMPLIFY_VALUE),//摄氏度：106
	(uint32_t)(0.5473*AMPLIFY_VALUE),//摄氏度：107
	(uint32_t)(0.5321*AMPLIFY_VALUE),//摄氏度：108
	(uint32_t)(0.5174*AMPLIFY_VALUE),//摄氏度：109
	(uint32_t)(0.5031*AMPLIFY_VALUE),//摄氏度：110
	(uint32_t)(0.4894*AMPLIFY_VALUE),//摄氏度：111
	(uint32_t)(0.4760*AMPLIFY_VALUE),//摄氏度：112
	(uint32_t)(0.4631*AMPLIFY_VALUE),//摄氏度：113
	(uint32_t)(0.4506*AMPLIFY_VALUE),//摄氏度：114
	(uint32_t)(0.4386*AMPLIFY_VALUE),//摄氏度：115
	(uint32_t)(0.4268*AMPLIFY_VALUE),//摄氏度：116
	(uint32_t)(0.4155*AMPLIFY_VALUE),//摄氏度：117
	(uint32_t)(0.4045*AMPLIFY_VALUE),//摄氏度：118
	(uint32_t)(0.3939*AMPLIFY_VALUE),//摄氏度：119
	(uint32_t)(0.3835*AMPLIFY_VALUE)//摄氏度：120
};

#define Vref 3.3     //参考电压，单位：V
#define Res  10000 //参考电阻，单位：Ω

/***************************************************************
  *	@brief	引脚初始化
  *	@param	
  *	@retval	void
****************************************************************/
void GPIO_Configuration(){
	GPIO_InitTypeDef temp;
	RCC_APB2PeriphClockCmd(TEMP_CLK|LIGHT_CLK,ENABLE);
	temp.GPIO_Mode=GPIO_Mode_AIN;
	temp.GPIO_Pin=TEMP_PIN;
	GPIO_Init(TEMP_PORT,&temp);
	temp.GPIO_Pin=LIGHT_PIN;
	GPIO_Init(LIGHT_PORT,&temp);
}
/***************************************************************
  *	@brief	adc初始化
  *	@param	
  *	@retval	void
****************************************************************/
void TEMP_LIGHT_Init(){
	ADC_InitTypeDef temp;
	
	GPIO_Configuration();
	RCC_APB2PeriphClockCmd(TEMP_LIGHT_ADC_CLK,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	temp.ADC_ContinuousConvMode=DISABLE;
	temp.ADC_DataAlign=ADC_DataAlign_Right;
	temp.ADC_Mode=ADC_Mode_Independent;
	temp.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	temp.ADC_NbrOfChannel=2;
	temp.ADC_ScanConvMode=DISABLE;
	ADC_Init(ADC1,&temp);
	ADC_Cmd(ADC1,ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1)){};
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1)){};
		
}
/***************************************************************
  *	@brief	软件转换获取一次采集值
  *	@param	uint8 ch 通道号 ADC_Channel_x x：0~15
  *	@retval	uint16 返回adc采集到的数据值
****************************************************************/
uint16_t GetADC(uint8_t ch){
	ADC_RegularChannelConfig(TEMP_LIGHT_ADC,ch,1,ADC_SampleTime_239Cycles5);
	ADC_SoftwareStartConvCmd(TEMP_LIGHT_ADC,ENABLE);
	while(!ADC_GetFlagStatus(TEMP_LIGHT_ADC,ADC_FLAG_EOC)){};
	return ADC_GetConversionValue(TEMP_LIGHT_ADC);
}
/***************************************************************
  *	@brief	获取time次的平均值
  *	@param	uint8 ch 通道号 ADC_Channel_x x：0~15
	*	@param	uint8 time 次数
  *	@retval	uint16 返回adc采集到的数据平均值
****************************************************************/
uint16_t GetADC_Time(uint8_t ch,uint8_t time){
	uint32_t sum=0;
	uint8_t count=time;
	while(count>0){
		sum+=GetADC(ch);
		count--;
	}
	return sum/time;
}

/********************************************************
*函数名称：uint32_t calculateResValue(uint16_t ADC_Value)
*功能：输入热敏电阻两端ADC值，输出热敏电阻值，单位：Ω；
*输入：uint16_t ADC_Value，热敏电阻两端ADC值，
*输出：热敏电阻值，单位：Ω；
*特殊说明：ADC分辨率：12位分辨率，2^8=256
**********************************************************/
uint32_t calculateResValue(uint16_t ADC_Value)
{
	float Vrt=0.0;
	uint32_t Rrt=0;
	
	Vrt=(Vref/(4096))*ADC_Value;//热敏电阻两端电压
	Rrt=Vrt/((Vref-Vrt)/Res);//热敏电阻，阻值
	return (uint32_t)Rrt;
}

/********************************************************
*函数名称：int8_t calculateTemperature(uint32_t Res)
*功能：输入热敏电阻，单位：Ω，输出温度值，单位：℃
*输入：uint32_t Res，热敏电阻值，单位：Ω
*输出：输出温度值，单位：℃
*特殊说明：电阻值范围98.26KΩ~0.3835KΩ，温度范围-20℃~120℃
**********************************************************/
int8_t calculateTemperature(uint32_t Rth)
{
	uint8_t i=0;
	int8_t temp=0;
	uint32_t mide=0;
	
	for(i=0;i<140;i++)
	{
		if(i == 139)
		{
			return 120;//127;//超出温度最大测量范围
		}
		else if(i == 0)
		{
			if(Rth > RES_TEMP_Table[0])
			{
				return -20;//-128;//超出温度最小测量范围
			}
		}
		
		if(Rth<=RES_TEMP_Table[i] && Rth>=RES_TEMP_Table[i+1])
		{
			mide = (RES_TEMP_Table[i]+RES_TEMP_Table[i+1])/2;
			if(Rth>=mide)
			{
				temp=i;
			}
			else
			{
				temp=i+1;
			}
			temp=i;
			break;
		}
	}
	return (temp-20);
}

/***************************************************************
  *	@brief	获取一次温度值，查表
  *	@param	
  *	@retval	uint8 温度值 
****************************************************************/
uint8_t GetTemp(){
	uint16_t temp;
	temp=GetADC_Time(TEMP_CH,10);
	printf("temp adc value:%d\r\n",temp);
	return calculateTemperature(calculateResValue(temp));
}

/***************************************************************
  *	@brief	获取一次光照值，以百分比的形式计算
  *	@param	
  *	@retval	uint8 光照值
****************************************************************/
uint8_t GetLight(){
	uint16_t	light;
	light=GetADC_Time(LIGHT_CH,10);
	printf("light adc value:%d\r\n",light);
	return (uint8_t)(100-(light*1000/4096)*100/1000);
}



