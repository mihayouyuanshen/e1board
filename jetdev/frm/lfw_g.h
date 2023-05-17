/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Chen.H												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/23												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __LFW_G_H__
#define __LFW_G_H__

#include "typedef.h"

/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define MSG_TYPE_ASYNC		((slong)1)
#define MSG_TYPE_SSYNC		((slong)2)

#define MSG_MAX_SIZE_DEF	((ulong)4u * 1024u)		/* 最大メッセージ長 */

#define LFW_RET_OK			((slong)0)
#define LFW_RET_ERR			((slong)-1)
/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef enum {
	LFW_MSG_ASYNC = 0,
	LFW_MSG_EVENT,
	LFW_CALL_SYNC,
	LFW_CALL_RET,
	LFW_MSG_MAX
} LfwMsgType_t;

typedef union {
	void *p;
	ulong dw;
	ushort w[2];
	uchar b[4];
} LfwOpt_t;

typedef struct {
	slong mType;
	ulong srcModId;		/* 返信先メッセージキューID */
	ulong destModId;
	ushort attr;		/* FrmMsgType_e */
	ushort msgSize;
	ushort msgId;
	slong retCode;
	LfwOpt_t opt;
} LfwHead_t;

enum {
	LFW_MSGID_TIMER_TIMEOUT = 0xFFE0,
	LFW_MSGID_ALARM_TIMEOUT,
	LFW_MSGID_WDT_REQ,			/* WDT応答要求 */
	LFW_MSGID_WDT_RES,			/* WDT応答通知 */
	LFW_MSGID_TERM_REQ,			/* 終了要求 */
	LFW_MSGID_TERM_RES			/* 終了応答通知 */
};

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern slong LFW_WaitMessage(slong receiver, ushort msgType, void *retMsg);
extern slong LFW_SendAsync(ulong destModId, ushort msgSize, void *msg);
extern slong LFW_SendEvent(ulong destModId, ushort msgSize, void *msg);

extern slong LFW_CallRet(ulong destModId, ushort msgSize, void *msg);
extern slong LFW_CallSync(ulong destModId,
							ushort msgSize, void *msg, void *retMsg);

extern void LFW_SetReceiver(slong receiver);
extern slong LFW_GetReceiver(void);

#endif /*__LFW_G_H__*/
