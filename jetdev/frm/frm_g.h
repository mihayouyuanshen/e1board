/****************************************************************************/
/*	Description		:框架共通头文件											*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Chen.H												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/23												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:frm													*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __FRM_G_H__
#define __FRM_G_H__
#include <pthread.h>
#include "typedef.h"
#include "lfw_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define FRM_SUCCESS				((slong)0)
#define FRM_ERROR				((slong)-1)

/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef LfwHead_t FrmMsgHead_t;
typedef slong FrmPthread_t;
typedef LfwOpt_t FrmTimerOpt;
typedef sint ProcessMutex_t;

#define FRM_MSG_ASYNC	LFW_MSG_ASYNC
#define FRM_MSG_EVENT	LFW_MSG_EVENT
#define FRM_CALL_SYNC	LFW_CALL_SYNC
#define FRM_CALL_RET	LFW_CALL_RET
#define FRM_MSG_MAX		LFW_MSG_MAX

/* 模块描述结构体 */
typedef struct {
	ulong  modId;								/* 模块ID */
	ushort rcvQMaxSize;							/* 接收buffer最大size */
	void (*modInit)(void);						/* 模块初始化函数 */
	void (*modMsgProc)(FrmMsgHead_t const *msg);/* 模块消息处理函数 */
	void (*modFin)(void);						/* 模块终止处理函数 */
} ModDesc_t;

/* 设定Alarm时刻结构体 */
typedef struct {
	uchar sec;			/* 秒（0～60）※60は閏秒のため */
	uchar min;			/* 分（0～59）*/
	uchar hour;			/* 時（0～23）*/
	uchar day;			/* 日（1～31）*/
	uchar mon;			/* 月（1～12）*/
	ushort year;		/* 年（1900～）※西暦年 */
} FrmTime_t;

/* Timeout消息结构体 */
typedef struct {
	FrmMsgHead_t header;
	ushort timerId;
	FrmTimerOpt opt;
} FrmTimeout_t;
/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern slong FRM_MsgInit(ModDesc_t const *modDescTbl, ushort modDescTblSize);
extern slong FRM_MsgLoop(void);
extern slong FRM_MsgFin(void);

extern slong FRM_MsgModReboot(ulong modId);
extern ulong FRM_MsgSelfModId(void);

extern slong FRM_MsgSendAsync(ulong destModId, ushort msgSize, void *msg);
extern slong FRM_MsgSendEvent(ulong destModId, ushort msgSize, void *msg);

extern slong FRM_CallRet(ulong destModId, ushort msgSize, void *msg);
extern slong FRM_CallSync(ulong destModId,
						ushort msgSize,
						void *msg, void *retMsg);

extern slong FRM_PthreadCreate(FrmPthread_t *thread,
								const pthread_attr_t *attr,
								void *(*startRoutine)(void *),
								void *arg);
extern void FRM_PthreadCancel(FrmPthread_t thread);

extern slong FRM_TimerInit(void);
extern slong FRM_TimerSetOneShot(ushort timerId,
								ulong msec, const FrmTimerOpt *opt);
extern slong FRM_TimerCancelOneShot(ushort timerId);
extern slong FRM_TimerSetAlarm(ushort alarmId,
								const FrmTime_t *pTime,
								const FrmTimerOpt *opt);
extern slong FRM_TimerCancelAlarm(ushort alarmId);

extern ProcessMutex_t FRM_MutexCreate(slong key);
extern void FRM_MutexLock(ProcessMutex_t mutexId);
extern void FRM_MutexUnLock(ProcessMutex_t mutexId);
extern void FRM_MutexRemove(ProcessMutex_t mutexId);
#endif /*__FRM_G_H__*/
