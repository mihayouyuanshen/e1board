/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Chen.H												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/20												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "esys_fsm_g.h"
#include "esys_fsm.h"
#include "esys_cmn_g.h"
#include "esys_func_g.h"
#include "dlog_g.h"
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/

static void esys_FsmStartProc(const FsmTrigerInfo_t *trigger);
static void esys_FsmRunningProc(const FsmTrigerInfo_t *trigger);
static void esys_FsmTimeoutRunningProc(const FsmTrigerInfo_t *trigger);
static void esys_FsmKeyPressProc(const FsmTrigerInfo_t *trigger);
static void esys_FsmPowerCutChgProc(const FsmTrigerInfo_t *trigger);
static void esys_FsmErrorHappenProc(const FsmTrigerInfo_t *trigger);
static void esys_FsmErrorCancelProc(const FsmTrigerInfo_t *trigger);
static void esys_FsmSysTermProc(const FsmTrigerInfo_t *trigger);
/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
FsmState_t s_esysCurState;
const FsmTrans_t s_esysFsmTransTbl[ESYS_ST_MAX][ESYS_TRIGGER_MAX] = {
	/* ESYS_ST_IDLE */
	{
		{ESYS_ST_STARTING, esys_FsmStartProc},	/* ESYS_TRIGGER_START */
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_START_FINISH */
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_START_TIMEOUT */
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_KEY_PRESS */
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_POWER_CUT_ST_CHANGE */
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_ERROR_HAPPEN */
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_ERROR_CANCEL */
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_SYSTEM_TERMINATE */
	},
	/* ESYS_ST_STARTING */
	{
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_START */
		{ESYS_ST_RUNNING, esys_FsmRunningProc},	/* ESYS_TRIGGER_START_FINISH */
		{ESYS_ST_RUNNING, esys_FsmTimeoutRunningProc},	/* ESYS_TRIGGER_START_TIMEOUT */
		{ESYS_ST_KEEP, esys_FsmKeyPressProc},	/* ESYS_TRIGGER_KEY_PRESS */
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_POWER_CUT_ST_CHANGE */
		{ESYS_ST_KEEP, esys_FsmErrorHappenProc},/* ESYS_TRIGGER_ERROR_HAPPEN */
		{ESYS_ST_KEEP, esys_FsmErrorCancelProc},/* ESYS_TRIGGER_ERROR_CANCEL */
		{ESYS_ST_STOPING, esys_FsmSysTermProc},	/* ESYS_TRIGGER_SYSTEM_TERMINATE */
	},
	/* ESYS_ST_RUNNING */
	{
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_START */
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_START_FINISH */
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_START_TIMEOUT */
		{ESYS_ST_KEEP, esys_FsmKeyPressProc},	/* ESYS_TRIGGER_KEY_PRESS */
		{ESYS_ST_KEEP, esys_FsmPowerCutChgProc},/* ESYS_TRIGGER_POWER_CUT_ST_CHANGE */
		{ESYS_ST_KEEP, esys_FsmErrorHappenProc},/* ESYS_TRIGGER_ERROR_HAPPEN */
		{ESYS_ST_KEEP, esys_FsmErrorCancelProc},/* ESYS_TRIGGER_ERROR_CANCEL */
		{ESYS_ST_STOPING, esys_FsmSysTermProc},	/* ESYS_TRIGGER_SYSTEM_TERMINATE */
	},
	/* ESYS_ST_STOPING */
	{
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_START */
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_START_FINISH */
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_START_TIMEOUT */
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_KEY_PRESS */
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_POWER_CUT_ST_CHANGE */
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_ERROR_HAPPEN */
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_ERROR_CANCEL */
		{ESYS_ST_KEEP, NULL},					/* ESYS_TRIGGER_SYSTEM_TERMINATE */
	},
	/* ESYS_ST_FW_UPDATE */
	{
		{ESYS_ST_KEEP, NULL},	/* ESYS_TRIGGER_START */
		{ESYS_ST_KEEP, NULL},	/* ESYS_TRIGGER_START_FINISH */
		{ESYS_ST_KEEP, NULL},	/* ESYS_TRIGGER_START_TIMEOUT */
		{ESYS_ST_KEEP, NULL},	/* ESYS_TRIGGER_KEY_PRESS */
		{ESYS_ST_KEEP, NULL},	/* ESYS_TRIGGER_POWER_CUT_ST_CHANGE */
		{ESYS_ST_KEEP, NULL},	/* ESYS_TRIGGER_ERROR_HAPPEN */
		{ESYS_ST_KEEP, NULL},	/* ESYS_TRIGGER_ERROR_CANCEL */
		{ESYS_ST_KEEP, NULL},	/* ESYS_TRIGGER_SYSTEM_TERMINATE */
	}
};

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:状态机初始化											*/
/*--------------------------------------------------------------------------*/
/*	param			:FsmState_t initState:初始状态							*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:初始化esys模块状态机									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
void esys_FsmInit(FsmState_t initState)
{
	DEBUG_TRCIN();

	s_esysCurState = initState;

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:状态设定												*/
/*--------------------------------------------------------------------------*/
/*	param			:FsmState_t state:状态									*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:设定当前状态											*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
void esys_FsmSetState(FsmState_t state)
{
	DEBUG_TRCIN();

	s_esysCurState = state;

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:状态机运行												*/
/*--------------------------------------------------------------------------*/
/*	param			:const FrmMsgHead_t *pMsg:消息							*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	-1:异常											*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
slong esys_fsmRun(const FrmMsgHead_t *pMsg)
{
	slong ret = ESYS_FSM_RET_ERR;
	FsmTrigerInfo_t triggerInfo;
	FsmState_t lastState;
	ushort trigger;

	DEBUG_TRCIN();
	trigger = esys_msg2Trig(pMsg);
	if (TRIGGER_INVALID != trigger)
	{
		triggerInfo.pMsg = pMsg;
		triggerInfo.trigger = trigger;
		lastState = s_esysCurState;
		if (ESYS_ST_KEEP != 
			s_esysFsmTransTbl[lastState][trigger].targetState)
		{
			s_esysCurState = 
				s_esysFsmTransTbl[lastState][trigger].targetState;
		}
		else
		{
			/* 処理無し */
		}

		if (NULL != s_esysFsmTransTbl[lastState][trigger].action)
		{
			s_esysFsmTransTbl[lastState][trigger].action(&triggerInfo);
		}
		else
		{
			/* 処理無し */
		}
		ret = ESYS_FSM_RET_OK;
	}

	DEBUG_TRCOUT();
	return ret;
}


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
ushort esys_msg2Trig(const FrmMsgHead_t *pMsg)
{
	ushort trigger = TRIGGER_INVALID;
	DEBUG_TRCIN();

	switch(pMsg->attr)
	{
		/* 非同期メッセージ */
		case LFW_MSG_ASYNC:
			switch(pMsg->msgId) {
				case ESYS_MSGID_START:
					trigger = ESYS_TRIGGER_START;
					break;
				case ESYS_MSGID_KEY1_PRESSED:
				case ESYS_MSGID_KEY1_LONG_PRESSED:
				case ESYS_MSGID_KEY2_PRESSED:
				case ESYS_MSGID_KEY2_LONG_PRESSED:
					trigger = ESYS_TRIGGER_KEY_PRESS;
					break;
				case ESYS_MSGID_POWER_CUT_ST_CHANGE:
					trigger = ESYS_TRIGGER_POWER_CUT_ST_CHANGE;
					break;
				case ESYS_MSGID_ERROR_HAPPEN:
					trigger = ESYS_TRIGGER_ERROR_HAPPEN;
					break;
				case ESYS_MSGID_ERROR_CANCEL:
					trigger = ESYS_TRIGGER_ERROR_CANCEL;
					break;
				default:
					DLOG_Error("Unknown AsyncMsg Rcv = %ld\n",
								pMsg->msgId);
					break;
			}
			break;
		/* イベントメッセージ */
		case LFW_MSG_EVENT:
			switch(pMsg->msgId) {
				/* WATCHDOG応答イベント */
				case WDT_EVT_WDT_REQ:
					esys_SendWdtRes();
					DLOG_Debug("Recv WDT Req!");
					break;
				/* 終了要求応答イベント */
				case WDT_EVT_TERM_REQ:
					trigger = ESYS_TRIGGER_SYSTEM_TERMINATE;
					break;
				default:
					DLOG_Error("Unknown AsyncEvt Rcv = %ld\n", pMsg->msgId);
					break;
			}
			break;
		default:
			DLOG_Error("Unknown Msg Rcv = %ld\n", pMsg->attr);
			break;
	}
	
	DEBUG_TRCOUT();
	return trigger;
}

/*==========================================================================*/
/*	Description		:启动处理												*/
/*--------------------------------------------------------------------------*/
/*	param			:const FsmTrigerInfo_t *trigger:事件信息				*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
void esys_FsmStartProc(const FsmTrigerInfo_t *trigger)
{
	DEBUG_TRCIN();
	DLOG_Info("System Starting");
	ESYS_Start(ESYS_GetStartFactor());
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:启动处理												*/
/*--------------------------------------------------------------------------*/
/*	param			:const FsmTrigerInfo_t *trigger:事件信息				*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
static void esys_FsmRunningProc(const FsmTrigerInfo_t *trigger)
{
	DEBUG_TRCIN();
	DLOG_Info("System Running");
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:启动处理												*/
/*--------------------------------------------------------------------------*/
/*	param			:const FsmTrigerInfo_t *trigger:事件信息				*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
static void esys_FsmTimeoutRunningProc(const FsmTrigerInfo_t *trigger)
{
	DEBUG_TRCIN();
	DLOG_Info("System Running");
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:按键处理												*/
/*--------------------------------------------------------------------------*/
/*	param			:const FsmTrigerInfo_t *trigger:事件信息				*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
void esys_FsmKeyPressProc(const FsmTrigerInfo_t *trigger)
{
	DEBUG_TRCIN();
	esys_KeyEventProc(trigger->pMsg->msgId);
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:停电状态变化处理										*/
/*--------------------------------------------------------------------------*/
/*	param			:const FsmTrigerInfo_t *trigger:事件信息				*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
void esys_FsmPowerCutChgProc(const FsmTrigerInfo_t *trigger)
{
	EsysPowerCutSt_t *priMsg = (EsysPowerCutSt_t *)trigger->pMsg;
	DEBUG_TRCIN();
	esys_PowerCutStChgProc(priMsg->status);
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:错误产生处理											*/
/*--------------------------------------------------------------------------*/
/*	param			:const FsmTrigerInfo_t *trigger:事件信息				*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
void esys_FsmErrorHappenProc(const FsmTrigerInfo_t *trigger)
{
	EsysMsgErrChg_t *priMsg = (EsysMsgErrChg_t *)trigger->pMsg;
	DEBUG_TRCIN();
	esys_ErrorHappenProc(&priMsg->errInfo);
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:错误消去处理											*/
/*--------------------------------------------------------------------------*/
/*	param			:const FsmTrigerInfo_t *trigger:事件信息				*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
void esys_FsmErrorCancelProc(const FsmTrigerInfo_t *trigger)
{
	EsysMsgErrChg_t *priMsg = (EsysMsgErrChg_t *)trigger->pMsg;
	DEBUG_TRCIN();
	esys_ErrorCancelProc(&priMsg->errInfo);
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:系统终止处理											*/
/*--------------------------------------------------------------------------*/
/*	param			:const FsmTrigerInfo_t *trigger:事件信息				*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
void esys_FsmSysTermProc(const FsmTrigerInfo_t *trigger)
{
	DEBUG_TRCIN();
	ESYS_Stop();
	DEBUG_TRCOUT();
}

/*End Of File*/
