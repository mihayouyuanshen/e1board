/****************************************************************************/
/*	Description		:INV通信ミドル 基幹処理									*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Wang.M												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/23												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:INV通信												*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EINV_SPEC_H__
#define __EINV_SPEC_H__


/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define EINV_TRIGGER_INVALID ((ushort)0xFFFFU)
#define EINV_TRIGGER_MAX     ((ushort)18U)
#define EINV_TESTRUN_STATUS_LEN	 	 ((uchar)4u)	/*試運転状態数*/
#define EINV_TESTRUN_EVENT_LEN	 	 ((uchar)6u)	/*試運転イベント数*/
#define EINV_TESTRUN_FUNCID_OFFSET	 ((uchar)12u)	/*試運転FUNC ID OFFSET*/

/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef struct {
	ushort trigger;
	const FrmMsgHead_t* pMsg;
} FsmTrigerInfo_t;

typedef struct {
	FsmState_t targetState;
	void (*action)(const FsmTrigerInfo_t*);
} FsmTrans_t;

typedef struct {
	const FrmMsgHead_t* pMsg;
} SubFsmTrigerInfo_t;

typedef struct {
	slong (*action)(const SubFsmTrigerInfo_t*);
} SubFsmTrans_t;


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
static ushort einv_Msg2Trig(const FrmMsgHead_t *pMsg);
static void einv_StartProc(const FsmTrigerInfo_t *trigInfo);
static void einv_StopProc(const FsmTrigerInfo_t *trigInfo);
static void einv_TimerProc(const FsmTrigerInfo_t *trigInfo);
static slong einv_ProcNotInvCommV(const FsmTrigerInfo_t *trigInfo);
static slong einv_ProcInvCommV(const FsmTrigerInfo_t *trigInfo);

static void  einv_TestRunProc(const FsmTrigerInfo_t *trigInfo);
static slong einv_TestRunSubProcIdle(const SubFsmTrigerInfo_t *trigInfo);
static slong einv_TestRunSubProcDo(const SubFsmTrigerInfo_t *trigInfo);
static slong einv_TestRunSubProcFin(const SubFsmTrigerInfo_t *trigInfo);
static slong einv_TestRunSubProcAbort(const SubFsmTrigerInfo_t *trigInfo);

#endif /*__EINV_SPEC_H__*/
