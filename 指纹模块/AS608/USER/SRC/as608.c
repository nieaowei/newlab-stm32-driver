/***************************************************************
  *	Name		:	AS608.C
  *	Data		:	2019.8.2
  *	Author	:	NIEAOWEI
  *	Note		:	指纹模块驱动的实现
****************************************************************/
#include "as608.h"
#include "sys.h"
#include "uart5.h"
#include "delay.h"

#define AS608ADDR	0xffffffff

//as608是16位芯片，记录指纹芯片的信息
struct asinfo{
	uint16_t sys_reg;//系统状态寄存器 
	uint16_t sensor_type;//传感器类型，偏移1字
	uint16_t store_size;//指纹库大小 偏移2字
	uint16_t safe;//安全等级 偏移3字
	uint32_t addr;//设备地址 偏移4字
	uint16_t data_size;//数据包大小 偏移6字
	uint16_t baudrate;//波特率 偏移7字
	uint8_t Store[300];//指纹库记录
	uint16_t ValidNum;//有效数量
}AS608_Info;

typedef enum{
	CMD_PACKAGE=0X01,
	DATA_PACKAGE=0X02,
	FINISH_PACKAGE=0X08,
	ACK_PACKAGE=0X07,
}PACKAGE_Type;
//记录指纹库中已有的位置
/*
		操作应答表
*/
typedef enum{
	AS608_ACK_OK,
	AS608_ACK_DATA_REC_ERROR,
	AS608_ACK_NO_FINGER,
	AS608_ACK_RECORD_FAIL,
	AS608_ACK_IMAGE_DRY,
	AS608_ACK_IAMGE_WET,
	AS608_ACK_IMAGE_CHAOS,
	AS608_ACK_IMAGE_FEATURE_LESS,
	AS608_ACK_MATCH_FAIL,
	AS608_ACK_SEARCH_FAIL,
	AS608_ACK_MEIGE_FAIL,
	AS608_ACK_NUM_OUVERFLOW,
	AS608_ACK_READ_TEMPLATE_FAIL,
	AS608_ACK_UPLOAD_FEATURE_FAIL,
	AS608_ACK_DATA_REC_BREAK,
	AS608_ACK_UPLOAD_IMAGE_FAIL,
	AS608_ACK_DELETE_TEMPLATE_FAIL,
	AS608_ACK_EMPTY_STORE_FAIL,
	AS608_ACK_LOWPORWER_FAIL,
	AS608_ACK_PASSWORD_ERROR,
	AS608_ACK_RESET_FAIL,
	AS608_ACK_BUFFER_NO_ORIGINAL_IAMGE,
	AS608_ACK_ONLINE_UPDATE_FAIL,
	AS608_ACK_FINGER_NO_CHANGE,
	AS608_ACK_READWRITE_FLASH_ERROR,
	AS608_ACK_FOLLOW_DATA,
	AS608_ACK_FOLLOW_CMD,
	AS608_ACK_WRITE_FLASH_PARITY_ERROR,
	AS608_ACK_WRITE_FLASH_ID_ERROR,
	AS608_ACK_WRITE_FLASH_LENTH_ERROR,
	AS608_ACK_WRITE_FLASH_CODELENTH_ERROR,
	AS608_ACK_WRITE_FLASH_ERROR,
	AS608_ACK_UNKNOWN_ERROR,
	AS608_ACK_NO_REGISTER,
	AS608_ACK_REGISTER_CONTENT_ERROR,
	AS608_ACK_TEXT_PAGE_ERROR,
	AS608_ACK_PORT_ERROR,
	AS608_ACK_ENROLL_ERROR,
	AS608_ACK_STORE_FULL,	
}AS608_ACK_State;

uint8_t AS608_GetAck(uint8_t ack){
	switch(ack){
		case AS608_ACK_OK:return AS608_ACK_OK;
		case AS608_ACK_DATA_REC_ERROR:return AS608_ACK_DATA_REC_ERROR;
		case AS608_ACK_NO_FINGER:return AS608_ACK_NO_FINGER;
		case AS608_ACK_RECORD_FAIL:return AS608_ACK_RECORD_FAIL;
		case AS608_ACK_IMAGE_DRY:return AS608_ACK_IMAGE_DRY;
		case AS608_ACK_IAMGE_WET:return AS608_ACK_IAMGE_WET;
		case AS608_ACK_IMAGE_CHAOS:return AS608_ACK_IMAGE_CHAOS;
		case AS608_ACK_IMAGE_FEATURE_LESS:return AS608_ACK_IMAGE_FEATURE_LESS;
		case AS608_ACK_MATCH_FAIL:return AS608_ACK_MATCH_FAIL;
		case AS608_ACK_SEARCH_FAIL:return AS608_ACK_SEARCH_FAIL;
		case AS608_ACK_MEIGE_FAIL:return AS608_ACK_MEIGE_FAIL;
		case AS608_ACK_NUM_OUVERFLOW:return AS608_ACK_NUM_OUVERFLOW;
		case AS608_ACK_READ_TEMPLATE_FAIL:return AS608_ACK_READ_TEMPLATE_FAIL;
		case AS608_ACK_UPLOAD_FEATURE_FAIL:return AS608_ACK_UPLOAD_FEATURE_FAIL;
		case AS608_ACK_DATA_REC_BREAK:return AS608_ACK_DATA_REC_BREAK;
		case AS608_ACK_UPLOAD_IMAGE_FAIL:return AS608_ACK_UPLOAD_IMAGE_FAIL;
		case AS608_ACK_DELETE_TEMPLATE_FAIL:return AS608_ACK_DELETE_TEMPLATE_FAIL;
		case AS608_ACK_EMPTY_STORE_FAIL:return AS608_ACK_EMPTY_STORE_FAIL;
		case AS608_ACK_LOWPORWER_FAIL:return AS608_ACK_LOWPORWER_FAIL;
		case AS608_ACK_PASSWORD_ERROR:return AS608_ACK_PASSWORD_ERROR;
		case AS608_ACK_RESET_FAIL:return AS608_ACK_RESET_FAIL;
		case AS608_ACK_BUFFER_NO_ORIGINAL_IAMGE:return AS608_ACK_BUFFER_NO_ORIGINAL_IAMGE;
		case AS608_ACK_ONLINE_UPDATE_FAIL:return AS608_ACK_ONLINE_UPDATE_FAIL;
		case AS608_ACK_FINGER_NO_CHANGE:return AS608_ACK_FINGER_NO_CHANGE;
		case AS608_ACK_READWRITE_FLASH_ERROR:return AS608_ACK_READWRITE_FLASH_ERROR;
		case AS608_ACK_FOLLOW_DATA:return AS608_ACK_FOLLOW_DATA;
		case AS608_ACK_FOLLOW_CMD:return AS608_ACK_FOLLOW_CMD;
		case AS608_ACK_WRITE_FLASH_PARITY_ERROR:return AS608_ACK_WRITE_FLASH_PARITY_ERROR;
		case AS608_ACK_WRITE_FLASH_ID_ERROR:return AS608_ACK_WRITE_FLASH_ID_ERROR;
		case AS608_ACK_WRITE_FLASH_LENTH_ERROR:return AS608_ACK_WRITE_FLASH_LENTH_ERROR;
		case AS608_ACK_WRITE_FLASH_CODELENTH_ERROR:return AS608_ACK_WRITE_FLASH_CODELENTH_ERROR;
		case AS608_ACK_WRITE_FLASH_ERROR:return AS608_ACK_WRITE_FLASH_ERROR;
		case AS608_ACK_UNKNOWN_ERROR:return AS608_ACK_UNKNOWN_ERROR;
		case AS608_ACK_NO_REGISTER:return AS608_ACK_NO_REGISTER;
		case AS608_ACK_REGISTER_CONTENT_ERROR:return AS608_ACK_REGISTER_CONTENT_ERROR;
		case AS608_ACK_TEXT_PAGE_ERROR:return AS608_ACK_TEXT_PAGE_ERROR;
		case AS608_ACK_PORT_ERROR:return AS608_ACK_PORT_ERROR;
		case AS608_ACK_ENROLL_ERROR:return AS608_ACK_ENROLL_ERROR;
		case AS608_ACK_STORE_FULL:return AS608_ACK_STORE_FULL;	
	}
	return 0xff;
}


/***************************************************************
  *	@brief	从接收缓存区获取一个响应包
  *	@param	*rec 用来存放数据的地址
  *	@retval	uint8_t
									接收到的包的类型
									CMD_PACKAGE=0X01,
									DATA_PACKAGE=0X02,
									FINISH_PACKAGE=0X08,
									ACK_PACKAGE=0X07,
									0：失败
****************************************************************/
uint8_t PS_GetAckData(uint8_t *rec,uint16_t mtime){
	uint16_t index=0,i;
	//等待包头
	do{
		if(FIFO_Out_Byte(&uart5_rx_fifo,&rec[index])!=FIFO_OP_FAIL){
			if(rec[index]==0xef){
				index++;
				break;
			}
		}
		delay_ms(1);
	}while(mtime--);
	//如果超时
	if(mtime==0){
		return 0;
	}
	delay_ms(100);//等待接收完毕
	//接收后续数据
	do{
		if(FIFO_Out_Byte(&uart5_rx_fifo,&rec[index])!=FIFO_OP_FAIL){
			index++;
		}
	}while(uart5_rx_fifo.data[uart5_rx_fifo.out+1]!=0xEF&&!FIFO_Is_Empty(&uart5_rx_fifo));
	//如果是数据包，继续接收后续数据
	if(rec[6]==DATA_PACKAGE){
		while(index<(12+AS608_Info.data_size)){
			if(FIFO_Out_Byte(&uart5_rx_fifo,&rec[index])!=FIFO_OP_FAIL){
				index++;
			}
		}
	}
	printf("index:%d\r\n",index);
	for(i=0;i<index;i++){
				printf("%X ",rec[i]);
			}
	printf("\r\n");
	return rec[6];
}
/***************************************************************
	*	@brief	发送命令包
  *	@param	*cmd 命令包数据地址
	*	@param  type 包的类型
	* @param	lenth 数据包的长度
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_SendCmd(uint8_t *cmd,PACKAGE_Type type,uint16_t lenth){
	uint8_t data[256],index=0,i;
	data[index++]=0XEF;
	data[index++]=0x01;
	data[index++]=AS608ADDR>>24;
	data[index++]=(uint8_t)(AS608ADDR>>16);
	data[index++]=(uint8_t)(AS608ADDR>>8);
	data[index++]=(uint8_t)(AS608ADDR);
	data[index++]=type;
	data[index++]=(lenth)>>8;
	data[index++]=(lenth);
	memcpy(&data[index],cmd,lenth);
	for(i=0;i<lenth+index;i++){
		printf("%X ",data[i]);
	}
	printf("\r\n");
	if(UART5_SendBytes(data,index+lenth)==0){
		return 1;
	}else{
		return 0;
	}
}

/***************************************************************
  *	@brief	探测手指，探测到后录入指纹图像存于 ImageBuffer
  *	@param	
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_GetImage(){
	uint8_t data[3],rec[12];
	data[0]=0x01;
	data[1]=(data[0]+3+CMD_PACKAGE)>>8;
	data[2]=(data[0]+3+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,3)){
		delay_ms(1000);
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	将 ImageBuffer 中的原始图像生成指纹特
  *	@param	BufferID 0x01,0x02
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_GenChar(uint8_t BufferID){
	uint8_t data[3+1],rec[12];
	data[0]=0x02;
	data[1]=BufferID;
	data[2]=(data[0]+data[1]+4+CMD_PACKAGE)>>8;
	data[3]=(data[0]+data[1]+4+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,4)){
		delay_ms(1000);
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	精确比对 CharBuffer1 与 CharBuffer2 中的
  *	@param	score 匹配得分存放地址
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_Match(uint16_t	*score){
	uint8_t data[3],rec[12+2];
	data[0]=0x03;
	data[1]=(data[0]+3+CMD_PACKAGE)>>8;
	data[2]=(data[0]+3+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,3)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				*score=((uint16_t)rec[10]<<8)|rec[11];
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	以 CharBuffer1 或 CharBuffer2 中的特征文件搜索整个
  *	@param	buffer 缓冲区0x01 0x02
	* @param	StartPage 起始页
	*	@param	PageNum	查找的页数
	*	@param	*PageID 找到的页码
	*	@param	*MatchScore 指纹匹配的得分数
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_Search(uint8_t buffer,uint16_t StartPage,uint16_t PageNum,uint16_t *PageID,uint16_t *MatchScore){
	uint8_t data[3+5],rec[12+4];
	data[0]=0x04;
	data[1]=buffer;
	data[2]=StartPage>>8;
	data[3]=StartPage;
	data[4]=PageNum>>8;
	data[5]=PageNum;
	data[6]=(data[0]+data[1]+data[2]+data[3]+data[4]+data[5]+8+CMD_PACKAGE)>>8;
	data[7]=data[0]+data[1]+data[2]+data[3]+data[4]+data[5]+8+CMD_PACKAGE;
	if(PS_SendCmd(data,CMD_PACKAGE,8)){
		if(PS_GetAckData(rec,1000)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				*MatchScore=((uint16_t)rec[12]<<8)|rec[13];
				*PageID=((uint16_t)rec[10]<<8)|rec[11];
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	将 CharBuffer1 与 CharBuffer2 中的特征文件合并生成 
						模板，结果存于 CharBuffer1 与 CharBuffer2。
  *	@param	
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_RegModel(){
	uint8_t data[3],rec[12];
	data[0]=0x05;
	data[1]=(data[0]+3+CMD_PACKAGE)>>8;
	data[2]=(data[0]+3+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,3)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	将 CharBuffer1 或 CharBuffer2 中的模板文件存到 PageID
						号 flash 数据库位置。
  *	@param	BufferID 0x01 0x02 缓冲区编号
	* @param 	PageID 位置页
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_StoreChar(uint8_t BufferID,uint16_t PageID){
	uint8_t data[3+3],rec[12];
	data[0]=0x06;
	data[1]=BufferID;
	data[2]=PageID>>8;
	data[3]=PageID;
	data[4]=(data[0]+data[1]+data[2]+data[3]+6+CMD_PACKAGE)>>8;
	data[5]=(data[0]+data[1]+data[2]+data[3]+6+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,6)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				AS608_Info.Store[PageID]=1;
				AS608_Info.ValidNum+=1;
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	将 flash 数据库中指定 ID 号的指纹模板读入到模板缓冲
						区 CharBuffer1 或 CharBuffer2
  *	@param	BufferID 缓存区号 0x01 0x02
	*	@param	PageID 指纹库模板号
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_LoadChar(uint8_t BufferID,uint16_t PageID){
	uint8_t data[3+3],rec[12];
	data[0]=0x07;
	data[1]=BufferID;
	data[2]=PageID>>8;
	data[3]=PageID;
	data[4]=(data[0]+data[1]+data[2]+data[3]+6+CMD_PACKAGE)>>8;
	data[5]=(data[0]+data[1]+data[2]+data[3]+6+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,6)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	将特征缓冲区中的特征文件上传给上位机
  *	@param	BufferID
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_UpChar(uint8_t BufferID,uint8_t *chardata){
	uint8_t data[4],rec[12];
	data[0]=0x08;
	data[1]=BufferID;
	data[2]=(data[0]+data[1]+4+CMD_PACKAGE)>>8;
	data[3]=(data[0]+data[1]+4+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,4)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				/*
				待写发送数据
				*/
				if(PS_GetAckData(chardata,300)==DATA_PACKAGE){
						
				}
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	上位机下载特征文件到模块的一个特征缓冲区
  *	@param	BufferID
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_DownChar(uint8_t BufferID,uint8_t *chardata){
	uint8_t data[4],rec[12];
	data[0]=0x09;
	data[1]=BufferID;
	data[2]=(data[0]+data[1]+4+CMD_PACKAGE)>>8;
	data[3]=(data[0]+data[1]+4+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,4)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(AS608_GetAck(rec[9])==AS608_ACK_OK){
				/*
				待写接收数据
				*/
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	将图像缓冲区中的数据上传给上位机
  *	@param	*iamge
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_UpImage(uint8_t *iamge){
	uint8_t data[3],rec[12];
	data[0]=0x0a;
	data[1]=(data[0]+3+CMD_PACKAGE)>>8;
	data[2]=(data[0]+3+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,3)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				//待写数据处理
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	上位机下载图像数据给模块
  *	@param	iamge
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_DownImage(uint8_t iamge){
	uint8_t data[3],rec[12];
	data[0]=0x0b;
	data[1]=(data[0]+3+CMD_PACKAGE)>>8;
	data[2]=(data[0]+3+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,3)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				//代谢数据处理
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	删除 flash 数据库中指定 ID 号开始的 N 个指纹模板
  *	@param	
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint16_t PS_DeletChar(uint16_t PageID,uint16_t N){
	uint8_t data[3+4],rec[12];
	data[0]=0x0c;
	data[1]=PageID>>8;
	data[2]=PageID;
	data[3]=N>>8;
	data[4]=N;
	data[5]=(data[0]+data[1]+data[2]+data[3]+data[4]+7+CMD_PACKAGE)>>8;
	data[6]=(data[0]+data[1]+data[2]+data[3]+data[4]+7+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,7
		)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				memset(&AS608_Info.Store[PageID],0,N);
				AS608_Info.ValidNum-=N;
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	删除 flash 数据库中所有指纹模板
  *	@param	
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_Empty(){
	
	uint8_t data[3],rec[12];
	data[0]=0x0d;
	data[1]=(data[0]+3+CMD_PACKAGE)>>8;
	data[2]=(data[0]+3+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,3)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				memset(AS608_Info.Store,0,300);
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	写模块寄存器
  *	@param	regId 寄存器序号 4~6
	*	@param	content 要修改的内容	
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_WriteReg(uint8_t regId,uint8_t content){
	uint8_t data[3+2],rec[12];
	data[0]=0x0e;
	data[1]=regId;
	data[2]=content;
	data[3]=(data[0]+data[1]+data[2]+5+CMD_PACKAGE)>>8;
	data[4]=(data[0]+data[1]+data[2]+5+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,5)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	读取模块的基本参数（波特率，包大小等）。
  *	@param	rece 16个字节的数据存储
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_ReadSysPara(){
	uint8_t data[3],rec[12+16];
	data[0]=0x0f;
	data[1]=(data[0]+3+CMD_PACKAGE)>>8;
	data[2]=(data[0]+3+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,3)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				AS608_Info.sys_reg=(rec[10]<<8)|rec[11];
				AS608_Info.sensor_type=(rec[12]<<8)|rec[13];
				AS608_Info.store_size=(rec[14]<<8)|rec[15];
				AS608_Info.safe=(rec[16]<<8)|rec[17];
				AS608_Info.addr=(rec[18]<<24)|(rec[19]<<16)|(rec[20]<<8)|rec[21];
				AS608_Info.data_size=(rec[22]<<8)|rec[23];
				switch(AS608_Info.data_size){
					case 0:AS608_Info.data_size=32;break;
					case 1:AS608_Info.data_size=64;break;
					case 2:AS608_Info.data_size=128;break;
					case 3:AS608_Info.data_size=256;break;
				}
				AS608_Info.baudrate=(rec[24]<<8)|rec[25];
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	采集一次指纹注册模板，在指纹库中搜索空位并存储，返回存储 ID
  *	@param	PageID 存储的id号
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_Enroll(uint16_t *PageID){
	uint8_t data[3],rec[12+2];
	data[0]=0x10;
	data[1]=(data[0]+3+CMD_PACKAGE)>>8;
	data[2]=(data[0]+3+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,3)){
		delay_ms(1000);
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				*PageID=((uint16_t)rec[10]<<8)|rec[11];
				AS608_Info.Store[*PageID]=1;
				AS608_Info.ValidNum+=1;
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	自动采集指纹，在指纹库中搜索目标模板并返回搜索结果。  
						如果目标模板同当前采集的指纹比对得分大于最高阀值，
						并且目标模板 为不完整特征则以采集的特征更新目标模
						板的空白区域。
  *	@param	PageID 匹配的id号
	*	@param	MathScore 匹配的分数
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_Identify(uint16_t *PageID,uint16_t *MathScore){
	uint8_t data[3],rec[12+4];
	data[0]=0x11;
	data[1]=(data[0]+3+CMD_PACKAGE)>>8;
	data[2]=(data[0]+3+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,3)){
		delay_ms(1000);
		if(PS_GetAckData(rec,1000)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				*PageID=((uint16_t)rec[10]<<8)|rec[11];
				*MathScore=((uint16_t)rec[12]<<8)|rec[13];
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	设置模块握手口令
  *	@param	PassWord 需要设置的口令
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_SetPwd(uint32_t PassWord){
	uint8_t data[3+4],rec[12+4];
	data[0]=0x12;
	data[1]=PassWord>>24;
	data[2]=PassWord>>16;
	data[3]=PassWord>>8;
	data[4]=PassWord;
	data[5]=(data[0]+data[1]+data[2]+data[3]+data[4]+7+CMD_PACKAGE)>>8;
	data[6]=(data[0]+data[1]+data[2]+data[3]+data[4]+7+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,7)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	验证模块握手口令
  *	@param	PassWord 验证的口令
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_VfyPwd(uint32_t PassWord){
	uint8_t data[3+4],rec[12+4];
	data[0]=0x13;
	data[1]=PassWord>>24;
	data[2]=PassWord>>16;
	data[3]=PassWord>>8;
	data[4]=PassWord;
	data[5]=(data[0]+data[1]+data[2]+data[3]+data[4]+7+CMD_PACKAGE)>>8;
	data[6]=(data[0]+data[1]+data[2]+data[3]+data[4]+7+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,7)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	令芯片生成一个随机数并返回给上位机
  *	@param	randomnum 产生的随机数
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_GetRandomCode(uint32_t *randomnum){
	uint8_t data[3],rec[12+4];
	data[0]=0x14;
	data[1]=(data[0]+3+CMD_PACKAGE)>>8;
	data[2]=(data[0]+3+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,3)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				*randomnum=((uint32_t)rec[10]<<24)|((uint32_t)rec[11]<<16)|\
				((uint32_t)rec[12]<<8)|((uint32_t)rec[12]);
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	设置芯片地址
  *	@param	addr 需要设置的地址
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_SetChipAddr(uint32_t addr){
	uint8_t data[3+4],rec[12+4];
	data[0]=0x15;
	data[1]=addr>>24;
	data[2]=addr>>16;
	data[3]=addr>>8;
	data[4]=addr;
	data[5]=(data[0]+data[1]+data[2]+data[3]+data[4]+7+CMD_PACKAGE)>>8;
	data[6]=(data[0]+data[1]+data[2]+data[3]+data[4]+7+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,3)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	读取 FLASH Information Page 所在的信息页(512bytes)
  *	@param	
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_ReadINFpage(uint8_t *rece){
	uint8_t data[3],rec[12];
	data[0]=0x16;
	data[1]=(data[0]+3+CMD_PACKAGE)>>8;
	data[2]=(data[0]+3+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,3)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				//数据处理
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	对于 UART 协议，该命令对 USB 通讯端口进行开关控制
  *	@param	
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_Port_Control(uint8_t control){
	uint8_t data[3+1],rec[12];
	data[0]=0x17;
	data[1]=control;
	data[2]=(data[0]+data[1]+4+CMD_PACKAGE)>>8;
	data[3]=(data[0]+data[1]+4+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,3)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	模块内部为用户开辟了 256bytes 的 FLASH 空间用于存放
						用户数 据，该存储空间称为用户记事本，该记事本逻辑上
						被分成 16 个页，写记事 本命令用于写入用户的 32bytes 
						数据到指定的记事本页。
  *	@param	
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_WriteNotepad(uint8_t userpageid,uint8_t *userdata){
	uint8_t data[3+33],rec[12];
	uint16_t temp=0,i;
	data[0]=0x18;
	data[1]=userpageid;
	memcpy(&data[2],userdata,32);
	for(i=0;i<34;i++){
		temp+=data[i];
	}
	data[34]=temp>>8;
	data[35]=temp;
	if(PS_SendCmd(data,CMD_PACKAGE,36)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){	
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	读取 FLASH 用户区的 128bytes 数据
  *	@param	userpageid 0~15
	*	@param	rece 存放读取的数据
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_ReadNotepad(uint8_t userpageid,uint8_t *rece){
	uint8_t data[3+1],rec[12];
	data[0]=0x19;
	data[1]=userpageid;
	data[2]=(data[0]+data[1]+4+CMD_PACKAGE)>>8;
	data[3]=(data[0]+data[1]+4+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,4)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				memcpy(rece,&rec[10],32);
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	以 CharBuffer1 或 CharBuffer2 中的特征文件高速搜索整
						个或部分指纹 库。若搜索到，则返回页码。
 					该指令对于的确存在于指纹库中，且登录时质量很好的指纹，
						会很快给 出搜索结果。
  *	@param	
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_HighSpeedSearch(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,uint16_t *PageID,uint16_t *MatchScore){
	uint8_t data[3+5],rec[12+4];
	data[0]=0x1b;
	data[1]=BufferID;
	data[2]=StartPage>>8;
	data[3]=StartPage;
	data[4]=PageNum>>8;
	data[5]=PageNum;
	data[6]=(data[0]+data[1]+data[2]+data[3]+data[4]+data[5]+8)>>8;
	data[7]=data[0]+data[1]+data[2]+data[3]+data[4]+data[5]+8;
	if(PS_SendCmd(data,CMD_PACKAGE,8)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				*MatchScore=((uint16_t)rec[12]<<8)|rec[13];
				*PageID=((uint16_t)rec[10]<<8)|rec[11];
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	对图像缓冲区中的指纹图像进行处理并生成细化指纹图像
  *	@param	BinImgTpye 0：二值化图像，1：不含特征点的图像，
												2：带有特征的的图像
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_GenBinImage(uint8_t BinImgTpye){
	uint8_t data[3+1],rec[12];
	data[0]=0x1c;
	data[1]=BinImgTpye;
	data[2]=(data[0]+data[1]+4+CMD_PACKAGE)>>8;
	data[3]=(data[0]+data[1]+4+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,4)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	读有效模板个数
  *	@param	ValidN 有效个数
  *	@retval	uint8_t
						0：失败
						1：成功
****************************************************************/
uint8_t PS_ValidTempleteNum(uint16_t *ValidN){
	uint8_t data[3],rec[12+2];
	data[0]=0x1d;
	data[1]=(data[0]+3+CMD_PACKAGE)>>8;
	data[2]=(data[0]+3+CMD_PACKAGE);
	if(PS_SendCmd(data,CMD_PACKAGE,3)){
		if(PS_GetAckData(rec,100)==ACK_PACKAGE){
			if(rec[9]==AS608_ACK_OK){
				*ValidN=((uint16_t)rec[10]<<8)|(rec[11]);
				AS608_Info.ValidNum=*ValidN;
				return 1;
			}
		}
	}
	return 0;
}
/***************************************************************
  *	@brief	设置数据包大小，需要重启生效
  *	@param	size 0：32字节 1：64字节  2：128字节 3：256字节
  *	@retval	uint8_t
										1：成功
										0：失败
****************************************************************/
uint8_t AS608_SetDataSize(uint8_t size){
	if(PS_WriteReg(6,size)){
		AS608_Info.data_size=size;
		return 1;
	}
	return 0;
}
/***************************************************************
  *	@brief	设置对比阈值，或者说是安全等级，需要重启生效
  *	@param	level	1~5
  *	@retval	uint8_t
										1：成功
										0：失败
****************************************************************/
uint8_t AS608_SetCompareValue(uint8_t leve1){
	if(PS_WriteReg(5,leve1)){
		AS608_Info.safe=leve1;
		return 1;
	}
	return 0;
}
/***************************************************************
  *	@brief	设置串口的波特率，必须为9600的整数倍，需要重启生效
  *	@param	baudrate 必须为9600的整数倍
  *	@retval	uint8_t
										1：成功
										0：失败
****************************************************************/
uint8_t AS608_SetBaudRate(uint32_t baudrate){
	if(PS_WriteReg(4,(uint8_t)(baudrate/9600))){
		AS608_Info.baudrate=baudrate;
		return 1;
	}
	return 0;
}
/***************************************************************
  *	@brief	测试demo
  *	@param	void
  *	@retval	void
****************************************************************/
void AS608_Demo(void){
	uint16_t temp,soc;
	uint8_t rec[512];
	if(PS_DeletChar(1,1)){
		printf("删除指纹成功\r\n");
	}
	if(PS_GetImage()){
		printf("录入指纹图像成功\r\n");
	}else{
		printf("录入指纹图像失败\r\n");
	}
	if(PS_GenChar(1)){
		printf("生成特征值成功\r\n");
		if(PS_UpChar(1,rec)){
			printf("特征值上传成功\r\n");
		}else{
			printf("特征值上传失败\r\n");
		}
	}else{
		printf("生成特征值失败\r\n");
	}
	
	if(PS_Search(1,0,8,&temp,&soc)){
		printf("%d %d 搜索成功\r\n",temp,soc);
	}else{
			if(PS_Search(1,0,8,&temp,&soc)){
				printf("%d %d 搜索成功\r\n",temp,soc);
				
			}else{
					printf("搜索失败\r\n");
					if(PS_Enroll(&temp)){
						printf("%d 自动录入指纹成功\r\n",temp);
					}
			}
	}
	
	/*
	if(PS_Identify(&temp,&soc)){//如果指纹存在
		printf("%d %d 自动搜索成功\r\n",temp,soc);
	}*/
	/*if(PS_DeletChar(1,1)){
		printf("删除指纹成功\r\n");
	}*/
}
/***************************************************************
  *	@brief	AS608相关io，外设，数据初始化
  *	@param	void
  *	@retval	void
****************************************************************/
void AS608_Init(void){
	uint16_t temp;
	if(PS_VfyPwd(0)){
		printf("握手成功\r\n");
	}
	/*
	if(AS608_SetDataSize(3)){
		printf("设置数据包大小成功\r\n");
	}*/
	/*if(PS_Empty()){
		printf("清空Flash成功\r\n");
	}*/
	if(PS_ValidTempleteNum(&temp)){
		printf("读取有效模板数成功\r\n");
	}
	if(PS_ReadSysPara()){
		printf("获取芯片信息成功\r\n");
		printf("系统状态寄存器：%x\r\n",AS608_Info.sys_reg);
		printf("传感器类型：%d\r\n",AS608_Info.sensor_type);
		printf("指纹库大小:%d\r\n",AS608_Info.store_size);
		printf("安全等级：%d\r\n",AS608_Info.safe);
		printf("设备地址：0x%X\r\n",AS608_Info.addr);
		printf("数据包大小：%d\r\n",AS608_Info.data_size);
		printf("波特率：%d\r\n",AS608_Info.baudrate*9600);
		printf("有效指纹数 ：%d\r\n",AS608_Info.ValidNum);
	}
}

