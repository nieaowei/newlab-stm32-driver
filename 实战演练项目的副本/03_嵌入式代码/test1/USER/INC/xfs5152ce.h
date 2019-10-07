
/***************************************************************
  *	Name		:	XFS5152CE.H
  *	Data		:	2019.7.15
  *	Author	:	NIEAOWEI
  *	Note		:	语音播放和识别相关操作函数
****************************************************************/
#ifndef _XFS5152CE_H
#define _XFS5152CE_H
/*
	编码类型，配合函数使用的
*/
typedef enum {
	XFS_FORMAT_GB2312 = 0x00,
	XFS_FORMAT_GBK = 0x01,
	XFS_FORMAT_BIG5 = 0x02,
	XFS_FORMAT_UNICODE = 0x03,
}xfsEnodingFormatEnum;

typedef enum{
	XFS_RECO_UNKNOWN=0X00,
	XFS_RECO_SUCCESS=0X01,
	XFS_RECO_TIMEOUT=0X02,
	XFS_RECO_REJECT=0X03,
	XFS_RECO_ERROR=0X04,
}xfs_RecognizeState;

/*
	应答类型，配合函数使用
*/
typedef enum {
	XFS_ACK_INIT_SUCCESS = 0x4A,
	XFS_ACK_CMD_RIGHT = 0x41,
	XFS_ACK_CMD_ERROR = 0x45,
	XFS_ACK_BUSY = 0x4E,
	XFS_ACK_IDEL = 0x4F,
}xfsAckStatusEnum;

extern const char *cmd_word_list[];

int xfs_init(void);
int xfs_get_status(void);
int xfs_sleep(void);
int xfs_wake_up(void);
int xfs_voice_start(unsigned char *data,unsigned char format,int wait_finish);
int xfs_voice_stop(void);
int xfs_voice_pause(void);
int xfs_voice_restore(void);
int xfs_send_to_cache(unsigned char *data,unsigned char cache_num);
int xfs_play_from_cache(unsigned char num,unsigned char format);
int xfs_recognize(unsigned char *result,unsigned int waittime);

#endif

