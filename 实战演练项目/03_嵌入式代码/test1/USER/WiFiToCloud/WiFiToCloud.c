/******************************************************************************
*
*
*
*
*
*
*
*
*
*
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "delay.h"
#include "lcd12864.h"
#include "rtc.h"
#include "relay.h"
#include "WiFiToCloud.h"


uint8_t wait_result(const char *result,uint16_t timeout){
	uint8_t temp[128];
	while(timeout--){
		FIFO_Out_Bytes(&uart4_rx_fifo,temp);
		if(strstr((const char*)temp,result)!=NULL){
			printf("res2: %s \r\n",temp);
			return 0;
		}
		ESP8266_DataAnalysisProcess(temp);
		memset(temp,0,128);
		delay_ms(100);
		//delay_ms(1);
	}
	return 1;
}

/*******************************************************************
*函数：int8_t ESP8266_SetStation(void)
*功能：ESP8266设置为station模式
*输入：无
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_SetStation(void)
{
	//ClrAtRxBuf();//清空缓存
	uint8_t temp[50];
	SendAtCmd((uint8_t *)AT_CWMODE,strlen(AT_CWMODE));
	delay_ms(200);
	if(wait_result("OK",1000)){
		return -1;
	}
	return 0;
}

/*******************************************************************
*函数：int8_t ESP8266_SetAP(void)
*功能：设置ESP8266要连接的热点名称和密码
*输入：char *wifi-热点名称 char *pwd-热点密码
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_SetAP(char *wifi, char *pwd)
{
	uint8_t temp[50];
	uint8_t AtCwjap[MAX_AT_TX_LEN];
	memset(AtCwjap, 0x00, MAX_AT_TX_LEN);//清空缓存
	//ClrAtRxBuf();//清空缓存
	sprintf((char *)AtCwjap,"AT+CWJAP_CUR=\"%s\",\"%s\"",wifi, pwd);
	SendAtCmd((uint8_t *)AtCwjap,strlen((const char *)AtCwjap));
	if(wait_result((const char*)"OK",5500)){
		return -1;
	}
	return 0;
}

/*******************************************************************
*函数：int8_t ESP8266_SetStation(void)
*功能：ESP8266建立TCP连接
*输入：
	char *IpAddr-IP地址，例如：120.77.58.34
	uint16_t port-端口号，取值0~65535
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_IpStart(char *IpAddr, uint16_t port)
{
	uint8_t IpStart[MAX_AT_TX_LEN];
	memset(IpStart, 0x00, MAX_AT_TX_LEN);//清空缓存
	//ClrAtRxBuf();//清空缓存
	sprintf((char *)IpStart,"AT+CIPSTART=\"TCP\",\"%s\",%d",IpAddr, port);
	SendAtCmd((uint8_t *)IpStart,strlen((const char *)IpStart));
	if(wait_result("OK",1500))
	{
		return -1;
	}
	return 0;
}

/*******************************************************************
*函数：int8_t ESP8266_IpSend(char *IpBuf, uint8_t len)
*功能：ESP8266发送数据
*输入：
		char *IpBuf-IP数据
		uint8_t len-数据长度
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_IpSend(char *IpBuf, uint8_t len)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t IpSend[MAX_AT_TX_LEN];
	memset(IpSend, 0x00, MAX_AT_TX_LEN);//清空缓存
	//ClrAtRxBuf();//清空缓存
	sprintf((char *)IpSend,"AT+CIPSEND=%d",len);
	//printf("%s\r\n",IpSend);////////////////////////////////////////////////////////////
	SendAtCmd((uint8_t *)IpSend,strlen((const char *)IpSend));
	
	if(wait_result("OK",500))
	{
		return -1;
	}
	//ClrAtRxBuf();//清空缓存
	SendStrLen((uint8_t *)IpBuf, len);
	//printf("%s\r\n",IpBuf);////////////////////////////////////////////////////////////
	for(TryGo = 0; TryGo<60; TryGo++)//最多等待时间100*60=6000ms
	{
		
		if(wait_result("SEND OK",200))
		{
			error = -2;
		}
		else
		{
			error = 0;
			break;
		}
	}
	return error;
}

/*******************************************************************
*函数：int8_t ConnectToServer(void)
*功能：连接到服务器
*输入：无
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ConnectToServer(char *DeviceID, char *SecretKey)
{
	uint16_t TryGo = 0;
	int8_t error = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//清空缓存
	printf("设置WiFi通讯模块工作模式\r\n");
//	LCD_WriteEnglishString(4,0,(unsigned char *)"Set WiFi Station");//                
	for(TryGo = 0; TryGo<3; TryGo++)
	{
		if(ESP8266_SetStation() == 0)//设置WiFi通讯模块工作模式
		{
			error = 0;
			break;
		}
		else
		{
			error = -1;
		}
	}
	if(error < 0)
	{
		return error;
	}
	printf("设置热点名称和密码\r\n");
//	LCD_WriteEnglishString(4,0,(unsigned char *)"Connect To WiFi ");//                
	for(TryGo = 0; TryGo<3; TryGo++)
	{
		if(ESP8266_SetAP((char *)WIFI_AP, (char *)WIFI_PWD) == 0)//设置热点名称和密码
		{
			error = 0;
			break;
		}
		else
		{
			error = -2;
		}
	}
	if(error < 0)
	{
		return error;
	}
	printf("连接服务器\r\n");
//	LCD_WriteEnglishString(4,0,(unsigned char *)"ConnectToServer ");//                
	for(TryGo = 0; TryGo<3; TryGo++)
	{
		if(ESP8266_IpStart((char *)SERVER_IP,SERVER_PORT) == 0)//连接服务器IP地址，端口：120.77.58.34,8600
		{
			error = 0;
			break;
		}
		else
		{
			error = -3;
		}
	}
	if(error < 0)
	{
		return error;
	}
	
	sprintf((char *)TxetBuf,"{\"t\":1,\"device\":\"%s\",\"key\":\"%s\",\"ver\":\"v0.0.0.0\"}",DeviceID,SecretKey);
	printf("握手认证：%s\r\n",TxetBuf);  
	if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
	{//发送失败
		error=-4;
	}else
	{//发送成功
		for(TryGo = 0; TryGo<2; TryGo++)//最多等待时间500*10=5000ms
		{
			if(wait_result((const char *)"\"status\":0",500))//检查响应状态是否为握手成功
			{
				printf("error\r\n");
				error = -5;
			}
			else
			{
				error = 0;
				break;
			}
		}
	}
	
	return error;
}

/*******************************************************************
*函数：int8_t ESP8266_SendSensor(char *ApiTag, 
									uint32_t sensor, char *TimeStr)
*功能：发送传感数据到服务器
*输入：
	char *ApiTag 实体对象标签名称
	uint32_t sensor, 传感数值
	char *TimeStr，采集数据时的时间，须是yyyy-mm-dd hh:mm:ss格式
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_SendSensor(char *ApiTag, uint32_t sensor, char *TimeStr)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//清空缓存
	sprintf((char *)TxetBuf,"{\"t\":3,\"datatype\":2,\"datas\":{\"%s\":{\"%s\":%d}},\"msgid\":001}",ApiTag,TimeStr,sensor);
	//printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
	if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
	{//发送失败
		error=-1;
	}
	else
	{//发送成功
		for(TryGo = 0; TryGo<10; TryGo++)//最多等待时间50*10=500ms
		{
			if(wait_result((const char *)"\"status\":0",10))//检查响应状态是否为"上报成功"
			{
				error = -1;
			}
			else
			{
				printf("send ok\r\n");
				error = 0;
				break;
			}
			//delay_ms(10);
		}
	}
	return error;
}

/*******************************************************************
*函数：int8_t ESP8266_SendSensorTry3rd(char *SensorTag, char *TimeStr, uint8_t sensor)
*功能：发送传感数据到服务器
*输入：
	char *SensorTag, 对象标签名称
	uint8_t sensor, 传感数值
	char *TimeStr,采集数据时的时间，须是yyyy-mm-dd hh:mm:ss格式
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：发送失败会重发，重发最多三次
*******************************************************************/
int8_t ESP8266_SendSensorTry3rd(char *SensorTag, uint8_t sensor, char *TimeStr)
{
	uint8_t TryGo = 0;
	uint8_t TrySend = 0;
	int8_t error = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//清空缓存
	sprintf((char *)TxetBuf,"{\"t\":3,\"datatype\":2,\"datas\":{\"%s\":{\"%s\":%d}},\"msgid\":001}",SensorTag, TimeStr, sensor);
	//printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
SendAgain:
	if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
	{//发送失败
		error=-1;
	}
	else
	{//发送成功
		for(TryGo = 0; TryGo<50; TryGo++)//最多等待时间50*10=500ms
		{
			if(wait_result((const char *)"\"status\":0",10))//检查响应状态是否为"上报成功"
			{
				error = -1;
			}
			else
			{
				error = 0;
				break;
			}
			delay_ms(10);
		}
	}
	if((error != 0)&&(TrySend<2))
	{
		printf("重发传感数据%d次\r\n",TrySend+1);
		TrySend++;
		goto SendAgain;
	}
	return error;
}

/*******************************************************************
*函数：uint8_t ESP8266_GetIpData(uint8_t *AtRxBuf, char *GetIpData)
*功能：解析服务器数据
*输入：
		uint8_t *AtRxBuf ，原始AT串口缓存
		char *GetIpData ，截取出来的Itcp/ip数据
*输出：返回收到的IP数据长度
*特殊说明：
	AT+CIPSEND=76
	{"t":3,"datatype":2,"datas":{"alarm":{"2018-06-19 18:15:02":0}},"msgid":001}
	+IPD,29:{"msgid":1,"status":0,"t":4}
*******************************************************************/
uint8_t ESP8266_GetIpData(uint8_t *AtRxBuf, char *GetIpData)
{
	char *Point = NULL;
	uint8_t len = 0;
	
	Point = strstr((const char *)AtRxBuf, (const char *)"+IPD,");
	if(Point != NULL)//检查模块收到TCP/IP数据包？
	{
		len=(Point[5]-'0')*10+Point[6]-'0';//这里IP数据长度不会超过100，故可以按“个位”和“十位”计算收到的IP数据长度
		Point = strstr((const char *)AtRxBuf, (const char *)":")+1;
		memcpy(GetIpData, Point, len);
		GetIpData[len]='\0';
		printf("收到IP数据：%s\r\n",GetIpData);///////////////////////////////////////////////////////
		printf("收到IP数据长度=%d\r\n",len);///////////////////////////////////////////////////
	}
	
	return (len);
}

/*******************************************************************
*函数：void ESP8266_DataAnalysisProcess(char *RxBuf)
*功能：解析服务器数据
*输入：char *RxBuf 服务器下发数据
*输出：
*特殊说明：用户可以在此基础上改造和扩展该函数，这里只是个简单的DEMO
*******************************************************************/
void ESP8266_DataAnalysisProcess(char *RxBuf)
{
	if(strstr((const char *)RxBuf, (const char *)"\"t\":5") != NULL)//命令请求？
	{
		if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"latern_cmd\"") != NULL)//布防/撤防请求
		{
			if((strstr((const char *)RxBuf, (const char *)"\"data\":1") != NULL))//布防
			{
				printf("布防！\r\n");
				latern_ON();
				;//...
				;//...
				;//...
			}
			else if((strstr((const char *)RxBuf, (const char *)"\"data\":0") != NULL))//撤防
			{
				latern_OFF();
				printf("撤防！\r\n");
				;//...
				;//...
				;//...
			}
		}
		if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"door_cmd\"") != NULL)//布防/撤防请求
		{
			if((strstr((const char *)RxBuf, (const char *)"\"data\":1") != NULL))//布防
			{
				printf("布防！\r\n");
				latern_ON();
				;//...
				;//...
				;//...
			}
			else if((strstr((const char *)RxBuf, (const char *)"\"data\":0") != NULL))//撤防
			{
				latern_OFF();
				printf("撤防！\r\n");
				;//...
				;//...
				;//...
			}
		}
		if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"lcd_text_cmd\"") != NULL)//布防/撤防请求
		{
			if((strstr((const char *)RxBuf, (const char *)"\"data\":1") != NULL))//布防
			{
				
			}
			else if((strstr((const char *)RxBuf, (const char *)"\"data\":") != NULL))//撤防
			{
				char *temp;
				temp=strstr((const char *)RxBuf, (const char *)"\"data\":");
				lcd_writeLenthChar(3,0,temp+8,strlen(temp+8));
			}
		}
		if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"time_cmd\"") != NULL)//布防/撤防请求
		{
		 if((strstr((const char *)RxBuf, (const char *)"\"data\":") != NULL))//撤防
			{
				char *temp;
				uint8_t date[8],i;
				temp=strstr((const char *)RxBuf, (const char *)"\"data\":");
				lcd_writeLenthChar(3,0,temp+8,10);
				date[0]=temp[0+8]-'0';
				date[1]=temp[1+8]-'0';
				date[2]=temp[2+8]-'0';
				date[3]=temp[3+8]-'0';
				date[4]=temp[5+8]-'0';
				date[5]=temp[6+8]-'0';
				date[6]=temp[8+8]-'0';
				date[7]=temp[9+8]-'0';
				for(i=0;i<8;i++){
					printf("%d,",date[i]);
				}
				RTC_Init(date[0]*1000+date[1]*100+date[2]*10+date[3],date[4]*10+date[5],date[6]*10+date[7],0,0,0,1);
			}
		}
	}
}
