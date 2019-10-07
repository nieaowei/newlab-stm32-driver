/***************************************************************
  *	Name		:	XFS5152CE.C
  *	Data		:	2019.7.15
  *	Author	:	NIEAOWEI
  *	Note		:	语音相关操作函数的实现
****************************************************************/
#include "xfs5152ce.h"
#include "usart3.h"
#include "string.h"
#include "delay.h"
#include "stdio.h"

//响应超时时间
#define XFS_RESPONSE_TIMEOUT 3000

static unsigned char firstflag=1;
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

//等待芯片回传响应
int wait_response(uint8_t result, uint16_t waittime){
	unsigned char temp;
	while(waittime--){
		delay_ms(1);
		if(!FIFO_Is_Empty(&usart3_rx_fifo)){
			if(FIFO_Out_Byte(&usart3_rx_fifo,&temp)!=FIFO_OP_FAIL){
				
			}
			if(result==temp){
				printf("result=%x\r\n",temp);
				return 1;//等待成功
			}
		}
		
	}
	printf("wait...\r\n");
	return -1;//超时
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
						1：成功
						-1：失败超时
****************************************************************/
static int xfs_send_cmd(uint8_t cmd)
{
	unsigned  char frame_info[8]; //定义的文本长度
	uint16_t frame_len = 0;
	uint8_t temp;
/*****************帧固定配置信息**************************************/           
	frame_info[frame_len++] = 0xFD ; 			//构造帧头FD
	frame_info[frame_len++] = 0x00 ; 			//构造数据区长度的高字节
	frame_info[frame_len++] = 0x01; 		//构造数据区长度的低字节
	frame_info[frame_len++] = cmd ; 			//构造命令字 

	if(firstflag==0){
		if(wait_response(XFS_ACK_CMD_RIGHT,XFS_RESPONSE_TIMEOUT)==1){
			if(wait_response(XFS_ACK_IDEL,XFS_RESPONSE_TIMEOUT)==1){
				usart3_send_str(frame_info, frame_len);
				firstflag=1;
			}
		}
	}else{
		usart3_send_str(frame_info, frame_len);
	}
	return wait_response(XFS_ACK_CMD_RIGHT, XFS_RESPONSE_TIMEOUT);
}
/***************************************************************
  *	@brief	初始化芯片
  *	@param	void
  *	@retval	int
							1：成功
****************************************************************/
int xfs_init(void){
	usart3_init();
	return 1;
}
/***************************************************************
  *	@brief	获取芯片当前状态
  *	@param	void
  *	@retval	int 
									XFS_ACK_INIT_SUCCESS = 0x4A,
									XFS_ACK_CMD_RIGHT = 0x41,
									XFS_ACK_CMD_ERROR = 0x45,
									XFS_ACK_BUSY = 0x4E,
									XFS_ACK_IDEL = 0x4F,
****************************************************************/
int xfs_get_status(void){
	unsigned char cmd[4];
	unsigned char temp=0;
	cmd[0]=0xFD;
	cmd[1]=0X00;
	cmd[2]=0X01;
	cmd[3]=0X21;
	usart3_send_str(cmd,4);
	if(FIFO_Out_Byte(&usart3_rx_fifo,&temp)!=FIFO_OP_FAIL)
	return (int)temp;
}
/***************************************************************
  *	@brief	使芯片进入睡眠状态
  *	@param	void
  *	@retval	int
							1：成功
							-1：超时失败
****************************************************************/
int xfs_sleep(void){
	return xfs_send_cmd(XFS_CMD_SLEEP);
}
/***************************************************************
  *	@brief	唤醒芯片
  *	@param	void
  *	@retval	int
							1：成功
							-1：超时失败
****************************************************************/
int xfs_wake_up(void){
	return xfs_send_cmd(XFS_CMD_WAKE_UP);
}
/***************************************************************
  *	@brief	停止当前合成
  *	@param	void
  *	@retval	int
							1：成功
							-1：超时失败
****************************************************************/
int xfs_voice_stop(void){
	return xfs_send_cmd(XFS_CMD_VOICE_STOP);
}
/***************************************************************
  *	@brief	暂停当前合成
  *	@param	void
  *	@retval	int
							1：成功
							-1：超时失败
****************************************************************/
int xfs_voice_pause(void){
	return xfs_send_cmd(XFS_CMD_VOICE_PAUSE);
}
/***************************************************************
  *	@brief	恢复已暂停的合成
  *	@param	void
  *	@retval	int
							1：成功
							-1：超时失败
****************************************************************/
int xfs_voice_restore(void){
	return xfs_send_cmd(XFS_CMD_VOICE_RESTORE);
}
/***************************************************************
  *	@brief	语音合成播放指定文本
  *	@param	*data 需要合成播放的文本
	* @param	format 编码格式
				@arg 	XFS_FORMAT_GB2312 = 0x00,
							XFS_FORMAT_GBK = 0x01,
							XFS_FORMAT_BIG5 = 0x02,
							XFS_FORMAT_UNICODE = 0x03,
	* @param	wait_finish 是否等待上次合成结束
  *	@retval	int
								0：成功
								1：失败
****************************************************************/
int xfs_voice_start(unsigned char *data,unsigned char format,int wait_finish){
	unsigned char cmd[100]="";
	unsigned int lenth=strlen((char *)data);

	cmd[0]=0xfd;
	cmd[1]=((lenth+2)>>8)&0xff;
	cmd[2]=(lenth+2)&0xff;
	cmd[3]=0x01;
	cmd[4]=format;
	memcpy(&cmd[5],data,lenth);
	/*usart3_send_str(cmd,lenth+5);
	if(wait_finish==1){
		if(wait_response(XFS_ACK_CMD_RIGHT,XFS_RESPONSE_TIMEOUT)==1){
			if(wait_response(XFS_ACK_IDEL,XFS_RESPONSE_TIMEOUT)==1){
				return 1;
			}
		}
	}*/
	//等待上次合成完毕
	if(wait_finish==1 && firstflag==0){
		if(wait_response(XFS_ACK_CMD_RIGHT,XFS_RESPONSE_TIMEOUT)==1){
			if(wait_response(XFS_ACK_IDEL,XFS_RESPONSE_TIMEOUT)==1){
				usart3_send_str(cmd,lenth+5);
			}else{
				return 1;
			}
		}else{
			return 1;
		}
	}else{
		usart3_send_str(cmd,lenth+5);
	}
	if(firstflag==1){
			//FIFO_Out_Byte(&usart3_rx_fifo,&data[99]);
			firstflag=0;
		}
	return 0;
}
/***************************************************************
  *	@brief	语音识别
  *	@param	*result 语音识别结果存储地址
	* @param 	waittime 等待时间
  *	@retval	int
								XFS_RECO_UNKNOWN=0X00,
								XFS_RECO_SUCCESS=0X01,
								XFS_RECO_TIMEOUT=0X02,
								XFS_RECO_REJECT=0X03,
								XFS_RECO_ERROR=0X04,
***************************************************************/
int xfs_recognize(unsigned char *result,unsigned int waittime){
	uint8_t temp[10];
	uint8_t lenth=1;
	int err = 1;
	printf("start..");
	if(xfs_send_cmd(XFS_CMD_RECOGNIZE_START)!=1){
		err=XFS_RECO_UNKNOWN;
	}
	
	while(waittime--){
		delay_ms(1);
		if(FIFO_Out_Byte(&usart3_rx_fifo,&temp[0])!=FIFO_OP_FAIL){
			if(temp[0]==0xfc){
				printf("%x\r\n",temp[0]);
				break;
			}
		}
	}
	
	while(lenth<10){
		if(FIFO_Out_Byte(&usart3_rx_fifo,&temp[lenth])!=FIFO_OP_FAIL)
		{
			printf("%x ",temp[lenth]);
			
		}
		lenth++;
	}
	
	if(waittime>0 && temp[0]==0xfc){
		
		switch(temp[3]){
			case XFS_RECO_TIMEOUT:
					err=XFS_RECO_TIMEOUT;
					break;
			case XFS_RECO_REJECT:
					err=XFS_RECO_REJECT;
					break;
			case XFS_RECO_ERROR:
					err=XFS_RECO_ERROR;
					break;
			case XFS_RECO_SUCCESS:
					err=XFS_RECO_SUCCESS;
					*result=temp[4];
					break;
		}
	}
	printf("stop..");
	if(xfs_send_cmd(XFS_CMD_RECOGNIZE_STOP)  != 1){
		err = XFS_RECO_UNKNOWN;
			printf("err:%d\r\n",err);
	}

	return err;
}

