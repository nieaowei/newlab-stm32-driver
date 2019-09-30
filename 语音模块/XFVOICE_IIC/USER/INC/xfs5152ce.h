/***************************************************************
  *	Name		:	XFS5152CE.C
  *	Data		:	2019.7.15
  *	Author	:	NIEAOWEI
  *	Note		:	语音相关功能函数
****************************************************************/
#ifndef _XFS5152CE_H
#define _XFS5152CE_H

#include <stdint.h>

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

void XFS_Init(void);
void XFS_Play(uint8_t* text,uint8_t format);
uint8_t XFS_WaitResult(uint8_t result, uint16_t waittime);
uint8_t XFS_Recogonize(uint8_t *result,uint32_t waittime );

#endif

