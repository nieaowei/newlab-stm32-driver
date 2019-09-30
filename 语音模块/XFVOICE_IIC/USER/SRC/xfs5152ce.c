/***************************************************************
  *	Name		:	IIC.H
  *	Data		:	2019.7.30
  *	Author	:	NIEAOWEI
  *	Note		:	
							本程序采用IIC协议控制芯片，需要写好IIC驱动。
****************************************************************/
#include "xfs5152ce.h"
#include "iic.h"
#include "string.h"
#include "stdio.h"
#include "delay.h"
/*
	相关定义
*/
#define XFS5152CE_ADDR	0x40 //设备地址
#define XFS_TIMEOUT			1000	//全局超时时间ms
/*
	芯片指令
*/
typedef enum {
	XFS_CMD_STATUS = 0x21,//查询状态
	XFS_CMD_SLEEP = 0x88,//进入低功耗模式
	XFS_CMD_WAKE_UP = 0xFF,//从低功耗返回正常模式
	XFS_CMD_VOICE_START = 0x01,//开始语音合成
	XFS_CMD_VOICE_STOP = 0x02,//停止语音合成
	XFS_CMD_VOICE_PAUSE = 0x03,//暂停语音合成
	XFS_CMD_VOICE_RESTORE = 0x04,//恢复语音合成
	XFS_CMD_SEND_TO_CACHE = 0x31,//发送文本到缓存区
	XFS_CMD_PLAY_FROM_CACHE = 0x32,//从缓存区播放文本
	XFS_CMD_RECOGNIZE_START = 0x10,
	XFS_CMD_RECOGNIZE_STOP = 0x1F
}xfsCommandEnum;

/*
	语音识别映射表
*/
const char *cmd_word_list[] = {
	"我在吃饭",  //0
	"我在修车",  //1
	"我在加油",  //2
	"正在休息",//3
	"同意",//4
	"不同意",//5
	"我去",//6
	"现在几点",//7
	"今天几号",//8
	"读信息",//9
	"开始读",//10
	"这是哪儿",//11
	"打开广播",//12
	"关掉广播",//13
	"打开音乐",//14
	"关掉音乐",//15
	"再听一次",//16
	"再读一遍",//17
	"大声点",//18
	"小声点",//19
	"读短信",//20
	"读预警信息",//21
	"明天天气怎样",//22
	"紧急预警信息",//23
	"开始",//24
	"停止",//25
	"暂停",//26
	"继续读",//27
	"确定开始",//28
	"取消"//29
};


/***************************************************************
  *	@brief	初始化芯片所使用的协议的相关引脚
  *	@param	void
  *	@retval	void
****************************************************************/
void XFS_Init(void){
	IIC_Init();
	if(XFS_WaitResult(XFS_ACK_INIT_SUCCESS,XFS_TIMEOUT)==1){
		printf("语音模块复位电初始化");
	}else{
		printf("语音模块上电位初始化");
	}
}
/***************************************************************
  *	@brief	播放指定文本
  *	@param	*text 待播放的文本信息
	* @param	format 文本信息的编码格式
					@arg 	XFS_FORMAT_GB2312 = 0x00,
								XFS_FORMAT_GBK = 0x01,
								XFS_FORMAT_BIG5 = 0x02,
								XFS_FORMAT_UNICODE = 0x03,
  *	@retval	void
****************************************************************/
void XFS_Play(uint8_t* text,uint8_t format){
	uint8_t data[200];
	uint8_t lenth;
	lenth=strlen((char*)text);
	data[0]=0xfd;
	data[1]=0x00;
	data[2]=lenth+2;
	data[3]=0x01;
	data[4]=format;
	memcpy(&data[5],text,lenth);
	if(IIC_Write_Bytes(XFS5152CE_ADDR,data,lenth+5)!=0){
		return ;
	}else{
		if(XFS_WaitResult(XFS_ACK_IDEL,XFS_TIMEOUT)!=0){
			return;
		}
	}
}

/***************************************************************
	*	@brief	发送指令给语音芯片
  *	@param	cmd 命令字
					@arg		XFS_CMD_STATUS = 0x21,//查询状态
									XFS_CMD_SLEEP = 0x88,//进入低功耗模式
									XFS_CMD_WAKE_UP = 0xFF,//从低功耗返回正常模式
									XFS_CMD_VOICE_START = 0x01,//开始语音合成
									XFS_CMD_VOICE_STOP = 0x02,//停止语音合成
									XFS_CMD_VOICE_PAUSE = 0x03,//暂停语音合成
									XFS_CMD_VOICE_RESTORE = 0x04,//恢复语音合成
									XFS_CMD_SEND_TO_CACHE = 0x31,//发送文本到缓存区
									XFS_CMD_PLAY_FROM_CACHE = 0x32,//从缓存区播放文本
									XFS_CMD_RECOGNIZE_START = 0x10,
									XFS_CMD_RECOGNIZE_STOP = 0x1F
  *	@retval	int
						0：成功
						1：失败超时
****************************************************************/
uint8_t XFS_SendCmd(uint8_t cmd){
	uint8_t temp_cmd[4];
	temp_cmd[0]=0xfd;
	temp_cmd[1]=0x00;
	temp_cmd[2]=0x01;
	temp_cmd[3]=cmd;
	
	if(IIC_Write_Bytes(XFS5152CE_ADDR,temp_cmd,4)!=0){
		return 1;
	}
		printf("cmd scusee");
	if(XFS_WaitResult(XFS_ACK_CMD_RIGHT,XFS_TIMEOUT)!=0){
		return 1;
	}
	printf("cmd scusee");
	return 0;
	
}
/***************************************************************
  *	@brief	等待回传信息
  *	@param	result 需要等待的结果
	*	@param	waittime	超时时间
  *	@retval	uint8_t
										0：成功
										1：超时失败
****************************************************************/
uint8_t XFS_WaitResult(uint8_t result, uint16_t waittime){
	uint8_t temp;
	while(waittime--){
		delay_ms(1);
		if(IIC_ReadBytes_Ack(XFS5152CE_ADDR,&temp,1)!=0){
			continue;
		}
		if(temp==result){
			printf("%x ",temp);
			return 0;//成功
		}
	}
	return 1;//超时
}
/***************************************************************
  *	@brief	语音识别
  *	@param	*result 语音识别结果存储地址
	* @param 	waittime 等待时间
  *	@retval	uint8_t
								XFS_RECO_UNKNOWN=0X00,
								XFS_RECO_SUCCESS=0X01,
								XFS_RECO_TIMEOUT=0X02,
								XFS_RECO_REJECT=0X03,
								XFS_RECO_ERROR=0X04,
***************************************************************/
uint8_t XFS_Recogonize(uint8_t *result,uint32_t waittime ){
	uint8_t rec_data[10];
	if(XFS_SendCmd(XFS_CMD_RECOGNIZE_START)==1){
		return XFS_RECO_UNKNOWN;
	}
	while(waittime--){
		delay_ms(1);
		if(IIC_ReadBytes_Ack(XFS5152CE_ADDR,&rec_data[0],5)==0){
			if(rec_data[0]==0xfc){
				printf("%x ,,",rec_data[0]);
				break;
			}
		}
	}
	
	if(waittime>0 && rec_data[0]==0xfc){
		
		switch(rec_data[3]){
			case XFS_RECO_TIMEOUT:
					return XFS_RECO_TIMEOUT;
			case XFS_RECO_REJECT:
					return XFS_RECO_REJECT;
			case XFS_RECO_ERROR:
					return XFS_RECO_ERROR;
			case XFS_RECO_SUCCESS:
					*result=rec_data[4];
					break;
		}
	}
	
	if(XFS_SendCmd(XFS_CMD_RECOGNIZE_STOP)!=0){
		return XFS_RECO_UNKNOWN;
	}
	printf("%x,%x,%x,%x,%x",rec_data[0],rec_data[1],rec_data[2],rec_data[3],rec_data[4]);
	return XFS_RECO_SUCCESS;
}

