/****************************************************************************/
/*	Description		:INV通信ミドル 基幹処理									*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Dai.P												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/23												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:INV通信												*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "typedef.h"
#include "task_g.h"
#include "frm_g.h"
#include "dlog_g.h"
#include "common_g.h"
#include "ewdt.h"
#include "esys_g.h"

#include "einv_g.h"
#include "einv_msg_g.h"
#include "einv_ram_g.h"
#include "einv_utl_g.h"
#include "einv_que_g.h"
#include "einv_com_g.h"
#include "einv_n_g.h"
#include "einv_n_func_g.h"
#include "einv_n_ram_g.h"
#include "einv_spec_g.h"
#include "einv_spec.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
FrmPthread_t g_einvComThread = 0UL;	/* 通信スレッド */


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
const ushort s_msgToTrig[EINV_TRIGGER_MAX] = {
	EINV_FID_INIT,
	EINV_FID_INIT_CMDNG,
	EINV_FID_INIT_CMD01_OK,
	EINV_FID_INIT_CMD60_OK,
	EINV_FID_INIT_CMD90_OK,
	EINV_FID_INIT_CMD65_OK,
	EINV_FID_INIT_CMD75_OK,
	EINV_FID_INIT_CMD6B_OK,
	EINV_FID_STOP,
	LFW_MSGID_TIMER_TIMEOUT,
	LFW_MSGID_ALARM_TIMEOUT,
	EINV_FID_REQ_OPEMODE,
	EINV_FID_TESTRUN_INDICATE,
	EINV_FID_TESTRUN_ABORT,
	EINV_FID_TESTRUN_CMD65_OK,
	EINV_FID_TESTRUN_CMD75_OK,
	EINV_FID_TESTRUN_TERM,
	EINV_FID_TESTRUN_CMD_NG
};

const FsmTrans_t s_einvFsmTransTbl[EINV_ST_MAX][EINV_TRIGGER_MAX] = {
	/* EINV_ST_START */
	{
		{	EINV_ST_START, einv_StartProc	},		/* EINV_FID_INIT */
		{	EINV_ST_START, einv_StartProc	},		/* EINV_FID_INIT_CMDNG */
		{	EINV_ST_START, einv_StartProc	},		/* EINV_FID_INIT_CMD01_OK */
		{	EINV_ST_START, einv_StartProc	},		/* EINV_FID_INIT_CMD60_OK */
		{	EINV_ST_START, einv_StartProc	},		/* EINV_FID_INIT_CMD90_OK */
		{	EINV_ST_START, einv_StartProc	},		/* EINV_FID_INIT_CMD65_OK */
		{	EINV_ST_START, einv_StartProc	},		/* EINV_FID_INIT_CMD75_OK */
		{	EINV_ST_NORMAL, einv_StartProc	},		/* EINV_FID_INIT_CMD6B_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_STOP */
		{	EINV_ST_START, einv_TimerProc	},		/* LFW_MSGID_TIMER_TIMEOUT */
		{	EINV_ST_KEEP, NULL	},		/* LFW_MSGID_ALARM_TIMEOUT */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_REQ_OPEMODE */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_INDICATE */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_ABORT */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_CMD65_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_CMD75_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_TERM */
		{	EINV_ST_KEEP, NULL	}		/* EINV_FID_TESTRUN_CMD_NG */
	},

	/* EINV_ST_NORMAL */
	{
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMDNG */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD01_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD60_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD90_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD65_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD75_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD6B_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_STOP */
		{	EINV_ST_NORMAL, einv_TimerProc	},		/* LFW_MSGID_TIMER_TIMEOUT */
		{	EINV_ST_NORMAL, einv_TimerProc	},		/* LFW_MSGID_ALARM_TIMEOUT */
		{	EINV_ST_NORMAL, einv_ProcInvCommV	},		/* EINV_FID_REQ_OPEMODE */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_INDICATE */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_ABORT */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_CMD65_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_CMD75_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_TERM */
		{	EINV_ST_KEEP, NULL	}		/* EINV_FID_TESTRUN_CMD_NG */
	},

	/* EINV_ST_TERMINAL */
	{
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMDNG */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD01_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD60_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD90_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD65_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD75_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD6B_OK */
		{	EINV_ST_TERMINAL, einv_StopProc	},		/* EINV_FID_STOP */
		{	EINV_ST_TERMINAL, einv_TimerProc	},		/* LFW_MSGID_TIMER_TIMEOUT */
		{	EINV_ST_TERMINAL, einv_TimerProc	},		/* LFW_MSGID_ALARM_TIMEOUT */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_REQ_OPEMODE */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_INDICATE */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_ABORT */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_CMD65_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_CMD75_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_TERM */
		{	EINV_ST_KEEP, NULL	}		/* EINV_FID_TESTRUN_CMD_NG */
	},

	/* EINV_ST_TEST_RUN */
	{
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMDNG */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD01_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD60_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD90_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD65_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD75_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD6B_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_STOP */
		{	EINV_ST_TEST_RUN, einv_TimerProc	},		/* LFW_MSGID_TIMER_TIMEOUT */
		{	EINV_ST_TEST_RUN, einv_TimerProc	},		/* LFW_MSGID_ALARM_TIMEOUT */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_REQ_OPEMODE */
		{	EINV_ST_TEST_RUN, einv_TestRunProc	},		/* EINV_FID_TESTRUN_INDICATE */
		{	EINV_ST_TEST_RUN, einv_TestRunProc	},		/* EINV_FID_TESTRUN_ABORT */
		{	EINV_ST_TEST_RUN, einv_TestRunProc	},		/* EINV_FID_TESTRUN_CMD65_OK */
		{	EINV_ST_TEST_RUN, einv_TestRunProc	},		/* EINV_FID_TESTRUN_CMD75_OK */
		{	EINV_ST_TEST_RUN, einv_TestRunProc	},		/* EINV_FID_TESTRUN_TERM */
		{	EINV_ST_TEST_RUN, einv_TestRunProc	}		/* EINV_FID_TESTRUN_CMD_NG */
	},

	/* EINV_ST_MAINTENANCE */
	{
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMDNG */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD01_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD60_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD90_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD65_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD75_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD6B_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_STOP */
		{	EINV_ST_MAINTENANCE, einv_TimerProc	},		/* LFW_MSGID_TIMER_TIMEOUT */
		{	EINV_ST_MAINTENANCE, einv_TimerProc	},		/* LFW_MSGID_ALARM_TIMEOUT */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_REQ_OPEMODE */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_INDICATE */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_ABORT */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_CMD65_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_CMD75_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_TERM */
		{	EINV_ST_KEEP, NULL	}		/* EINV_FID_TESTRUN_CMD_NG */
	},

	/* EINV_ST_FW_UPDATE */
	{
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMDNG */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD01_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD60_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD90_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD65_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD75_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_INIT_CMD6B_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_STOP */
		{	EINV_ST_FW_UPDATE, einv_TimerProc	},		/* LFW_MSGID_TIMER_TIMEOUT */
		{	EINV_ST_FW_UPDATE, einv_TimerProc	},		/* LFW_MSGID_ALARM_TIMEOUT */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_REQ_OPEMODE */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_INDICATE */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_ABORT */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_CMD65_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_CMD75_OK */
		{	EINV_ST_KEEP, NULL	},		/* EINV_FID_TESTRUN_TERM */
		{	EINV_ST_KEEP, NULL	}		/* EINV_FID_TESTRUN_CMD_NG */
	}
};

const SubFsmTrans_t s_einvSubFsmTransTbl[EINV_TESTRUN_EVENT_LEN][EINV_TESTRUN_STATUS_LEN] = {
	
	/*EINV_TESTRUN_IDLE*/			/*EINV_TESTRUN_DO*/ 	/*EINV_TESTRUN_FINISH*/		/*EINV_TESTRUN_ABORT*/
	{	einv_TestRunSubProcIdle,   	NULL, 					 NULL,						 NULL						},	  /* EINV_FID_TESTRUN_INDICATE */
	{    NULL,                      einv_TestRunSubProcDo,   NULL,                       NULL                       },    /* EINV_FID_TESTRUN_ABORT */
	{   einv_TestRunSubProcIdle,    NULL,   				 einv_TestRunSubProcFin,     einv_TestRunSubProcAbort  	},	  /* EINV_FID_TESTRUN_CMD65_OK */
	{   einv_TestRunSubProcIdle,    NULL,                    NULL,                       NULL                       },    /* EINV_FID_TESTRUN_CMD75_OK */
	{    NULL,                      einv_TestRunSubProcDo,   NULL,                       NULL                       },    /* EINV_FID_TESTRUN_TERM */
	{   einv_TestRunSubProcIdle,    einv_TestRunSubProcDo,   einv_TestRunSubProcFin,     einv_TestRunSubProcAbort   }     /* EINV_FID_TESTRUN_CMD_NG */
};

FsmState_t s_einvCurState;
/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description	:INV通信スレッド初期処理									*/
/*--------------------------------------------------------------------------*/
/*	param		:fact		:起動要因										*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	処理結果											*/
/*				:		EINV_OK				正常終了						*/
/*				:		EINV_ERR_SYSCALL	システムコールエラー			*/
/*				:		EINV_ERR_FAIL		処理失敗エラー					*/
/*--------------------------------------------------------------------------*/
/*	detail		:INV通信スレッド初期処理を行う。							*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/04 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_InitCommThread(void)
{
	slong ret = EINV_OK;

	DEBUG_TRCIN();

	/* INV通信スレッド初期処理 */
	ret = EINV_n_InitCommThread();

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:Init state machine										*/
/*--------------------------------------------------------------------------*/
/*	param			:FsmState_t initState: initial state					*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC huan.chen : 新規作成					*/
/*==========================================================================*/
void EINV_FsmInit(FsmState_t initState)
{
	DEBUG_TRCIN();

	s_einvCurState = initState;

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:Set current state										*/
/*--------------------------------------------------------------------------*/
/*	param			:FsmState_t state: current state						*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC huan.chen : 新規作成					*/
/*==========================================================================*/
void EINV_FsmSetState(FsmState_t state)
{
	DEBUG_TRCIN();

	s_einvCurState = state;

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:Run state machine										*/
/*--------------------------------------------------------------------------*/
/*	param			:const FrmMsgHead_t *pMsg:Current message				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong:0-OK -1:ERR										*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC huan.chen : 新規作成					*/
/*==========================================================================*/
slong EINV_FsmRun(const FrmMsgHead_t *pMsg)
{
	slong ret = EINV_ERR_PARAM;
	FsmTrigerInfo_t triggerInfo;
	FsmState_t lastState;

	DEBUG_TRCIN();

	triggerInfo.trigger = einv_Msg2Trig(pMsg);
	if (EINV_TRIGGER_INVALID != triggerInfo.trigger)
	{
		triggerInfo.pMsg = pMsg;
		lastState = s_einvCurState;
		if (EINV_ST_KEEP != s_einvFsmTransTbl[lastState][triggerInfo.trigger].targetState)
		{
			s_einvCurState = s_einvFsmTransTbl[lastState][triggerInfo.trigger].targetState;
		}
		else
		{
			/* 処理無し */
		}

		if (NULL != s_einvFsmTransTbl[lastState][triggerInfo.trigger].action)
		{
			s_einvFsmTransTbl[lastState][triggerInfo.trigger].action(&triggerInfo);
		}
		else
		{
			/* 処理無し */
		}
		ret = EINV_OK;
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:INV通信IF処理											*/
/*--------------------------------------------------------------------------*/
/*	param			:reqMsg		要求メッセージ								*/
/*--------------------------------------------------------------------------*/
/*	return			:ret	EINV_OK		正常終了							*/
/*							EINV_ERR_PARAM									*/
/*							EPCS_ERR_SYSCALL		システムコールエラー	*/
/*							EPCS_ERR_FAIL			処理失敗エラー			*/
/*							EINV_ERR_NOT_SUPPORT	 非サポートエラー		*/
/*--------------------------------------------------------------------------*/
/*	detail			:通信IF処理作成											*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC Feng.XZ : 新規作成					*/
/*==========================================================================*/
slong EINV_ProcInvCommunication(EinvMsgPar_t *reqMsg)
{
	slong ret = EINV_OK;
	FrmTimeout_t	*etimReqMsg;
	bool_t			shutdown = FALSE;
	SubFsmTrigerInfo_t info;

	DEBUG_TRCIN();

	/* 引数チェック */
	if(reqMsg == NULL)
	{
		DLOG_Error("EINV_procPcsCommunication(),EINV_ERR_PARAM,reqMsg == NULL.\n");
		DEBUG_TRCOUT();
		return EINV_ERR_PARAM;
	}
	else
	{
		/* 処理無し */
	}

	if (reqMsg->header.attr == FRM_MSG_EVENT)
	{
		switch(reqMsg->header.msgId){			/* IDで処理を判別 */
		case LFW_MSGID_TIMER_TIMEOUT:			/* ワンショットタイマイベント通知 */
			switch(reqMsg->timerEvent.timerId){
			case EINV_TIM_ID_CYCLE:				/* 周期情報取得処理 */
				ret = EINV_n_GetCycleOpeStateInfo();
				break;

			case EINV_TIM_ID_FWUPDATE:			/* INVFW更新処理 */
				//ret = epcs_n_fwUpdate();
				break;
			default:
				ret = EINV_ERR_NOT_SUPPORT;
				break;
			}
			break;

		case LFW_MSGID_ALARM_TIMEOUT:						/* アラームイベント通知 */
			/* ログ収集イベント */
			//if(etimReqMsg->id == (EINV_TIM_ID_LOG + g_pcsLogSeq))
			//{
			//	ret = epcs_n_getCycleLog();
			//}
			//// 余剰充電制御アラームイベント
			//else if(etimReqMsg->id == (EPCS_TIM_ID_SURCHARGE + g_pcsSurChgSeq))
			//{
			//	ret = epcs_n_setSurchargeParam();
			//}
			//else
			{
				ret = EINV_ERR_NOT_SUPPORT;
			}
			break;

		default:
			ret = EINV_ERR_NOT_SUPPORT;
			break;
		}
		//	break;
		//case EWDT_SPORT:							/* ウォッチドック管理ミドルからのイベント */
		//	switch(reqMsg->header.srcModId)			/* IDで処理を判別 */
		//	{
		//	case LFW_MSGID_WDT_REQ:					/* 終了要求応答 */
		//		//ret = epcs_n_ewdtTermReq((EWDT_EVT_TERM_REQ_T*)reqMsg);
		//		break;
		//	default:
		//		ret = EINV_ERR_NOT_SUPPORT;
		//		break;
		//	}
		//	break;
		//case EGUI_CPORT:							/* ホーム画面情報取得ミドルからのイベント */
		//	switch(reqMsg->header.msgId)
		//	{
		//		//case EGUI_EVTID_FINISHTESTRUN:		// 試運転完了通知イベント
		//		//	ret = epcs_n_notifyTestRunFinish((EGUI_EVT01_T*)reqMsg);
		//		//	break;
		//	default:
		//		ret = EINV_ERR_NOT_SUPPORT;
		//		break;
		//	}
		//	break;
		//default:
		//	ret = EINV_ERR_NOT_SUPPORT;
		//	break;
		//}
	}
	else if(reqMsg->header.attr == LFW_MSG_ASYNC)
	{
		switch(reqMsg->header.msgId){
		case EINV_FID_INIT:							/* CMD01送信 */
			ret = EINV_n_InitSendCMD01((EinvMsgInitreqPar_t*)reqMsg);
			break;
		case EINV_FID_INIT_CMDNG:					/* タイムアウト後も初期化通信は継続して行う */
			ret = EINV_n_InitSendCMD01((EinvMsgInitreqPar_t*)reqMsg);
			break;
		case EINV_FID_INIT_CMD01_OK:				/* CMD60送信 */
			ret = EINV_n_InitSendCMD60((EinvMsgInitreqPar_t*)reqMsg);
			break;
		case EINV_FID_INIT_CMD60_OK:				/* CMD90送信 */
			ret = EINV_n_InitSendCMD90((EinvMsgInitreqPar_t*)reqMsg);
			break;
		case EINV_FID_INIT_CMD90_OK:				/* CMD65送信 */
			ret = EINV_n_InitSendCMD65((EinvMsgInitreqPar_t*)reqMsg);
			break;
		case EINV_FID_INIT_CMD65_OK:				/* CMD75送信 */
			ret = EINV_n_InitSendCMD75((EinvMsgInitreqPar_t*)reqMsg);
			break;
		case EINV_FID_INIT_CMD75_OK:				/* CMD75送信 */
			ret = EINV_n_InitSendCMD6B((EinvMsgInitreqPar_t*)reqMsg);
			break;
		case EINV_FID_REQ_OPEMODE:					/* 運転モード蓄電池電力取得 */
			ret = EINV_n_SendCMD64();
			break;
		case EINV_FID_TESTRUN_CMD65:
		case EINV_FID_TESTRUN_CMD75:
			/*CMD65 コマンド発行指示*/
			/*CMD75 コマンド発行指示*/
			ret = EINV_n_TestRunSendCMD((EinvMsgTestRunreqPar_t*)reqMsg);	/*CMD65/75送信*/
			break;

		default:
			ret = EINV_ERR_NOT_SUPPORT;
			break;
		}
	}
	else
	{
		ret = EINV_ERR_NOT_SUPPORT;
	}

	DEBUG_TRCOUT();

	return ret;
}


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description		:Trans message to trigger								*/
/*--------------------------------------------------------------------------*/
/*	param			:const FrmMsgHead_t *pMsg:Current message				*/
/*--------------------------------------------------------------------------*/
/*	return			:FsmTrigger_t:trigger									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC huan.chen : 新規作成					*/
/*==========================================================================*/
ushort einv_Msg2Trig(const FrmMsgHead_t *pMsg)
{
	ushort trigger = EINV_TRIGGER_INVALID;
	ushort i = 0U;
	bool_t breakFlag = FALSE;

	DEBUG_TRCIN();

	while (!breakFlag)
	{
		if (i >= EINV_TRIGGER_MAX)
		{
			breakFlag = TRUE;
		}
		else if (pMsg->msgId == s_msgToTrig[i])
		{
			trigger = i;
			breakFlag = TRUE;
		}
		else
		{
			/* 処理無し */
		}
		i++;
	}

	DEBUG_TRCOUT();

	return trigger;
}

/*==========================================================================*/
/*	Description	:INVマイコン通信 初期化処理									*/
/*--------------------------------------------------------------------------*/
/*	param		:trigInfo	要求メッセージ									*/
/*--------------------------------------------------------------------------*/
/*	return		:なし														*/
/*--------------------------------------------------------------------------*/
/*	detail		:INVマイコン通信 初期化処理を行う。							*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/14 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
static void einv_StartProc(const FsmTrigerInfo_t *trigInfo)
{
	slong threadRet = FRM_SUCCESS;
	slong ret = EINV_OK;
	EinvMsgPar_t *msg = (EinvMsgPar_t*)trigInfo->pMsg;

	DEBUG_TRCIN();

	/* メッセージ処理 */
	switch (trigInfo->pMsg->msgId)
	{
	case EINV_FID_INIT:
		/* INV共通レイヤーのグローバル変数の初期化 */
		EINV_InitRam();

		/* グローバル変数の初期化 */
		EINV_n_InitNEinvGrobal();

		/* ファイル読み込み */
		EINV_n_InitMainThread();

		/* 通信要求キューの初期化 */
		EINV_InitCommReqQue();

		/* INV通信スレッド生成 */
		threadRet = FRM_PthreadCreate(&g_einvComThread, NULL, EINV_CommThreadStartRoutine, NULL);
		if (threadRet != FRM_SUCCESS)
		{
			DLOG_Error("einv_StartProc FRM_PthreadCreate fail\n");
		} 
		else
		{
			/* 処理無し */
		}

		FRM_TimerSetOneShot(EINV_TIM_ID_INIT_ERR, EINV_INIT_ERR_TIME, NULL);

		ret = einv_ProcInvCommV(trigInfo);
		if (ret != EINV_OK)
		{
			DLOG_Error("einv_StartProc EINV_enqCommReqQue fail\n\n");
		} 
		else
		{
			/* 処理無し */
		}
		break;
	case EINV_FID_INIT_CMDNG:
	case EINV_FID_INIT_CMD01_OK:
	case EINV_FID_INIT_CMD60_OK:
	case EINV_FID_INIT_CMD90_OK:
	case EINV_FID_INIT_CMD65_OK:
	case EINV_FID_INIT_CMD75_OK:
		ret = einv_ProcInvCommV(trigInfo);
		if (ret != EINV_OK)
		{
			DLOG_Error("einv_StartProc EINV_enqCommReqQue fail\n\n");
		} 
		else
		{
			/* 処理無し */
		}
		break;
	case EINV_FID_INIT_CMD6B_OK:
		ret = EINV_n_InitFin();
		break;
	default:
		DLOG_Error("unexpected msgid(%u) occur\n", trigInfo->pMsg->msgId);
		break;
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description	:INVマイコン通信終了処理									*/
/*--------------------------------------------------------------------------*/
/*	param		:trigInfo	要求メッセージ									*/
/*--------------------------------------------------------------------------*/
/*	return		:なし														*/
/*--------------------------------------------------------------------------*/
/*	detail		:INVマイコン通信終了処理を行う。							*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/18 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
static void einv_StopProc(const FsmTrigerInfo_t *trigInfo)
{
	DEBUG_TRCIN();

	/* メッセージ処理 */
	switch (trigInfo->pMsg->msgId)
	{
	case EINV_FID_STOP:
		FRM_PthreadCancel(g_einvComThread);
		ESYS_NotifyStopResult(EINV_SPORT, MOD_ACTION_RESULT_SUCCESS);
		break;
	default:
		DLOG_Error("unexpected msgid(%u) occur\n", trigInfo->pMsg->msgId);
		break;
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description	:INVマイコン通信終了処理									*/
/*--------------------------------------------------------------------------*/
/*	param		:trigInfo	要求メッセージ									*/
/*--------------------------------------------------------------------------*/
/*	return		:なし														*/
/*--------------------------------------------------------------------------*/
/*	detail		:INVマイコン通信終了処理を行う。							*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/18 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
static void einv_TimerProc(const FsmTrigerInfo_t *trigInfo)
{
	slong ret = EINV_OK;
	EinvMsgPar_t *msg = (EinvMsgPar_t*)trigInfo->pMsg;
	EinvMsgPar_t par;
	TestRunSubFsmState_t testRunSubFsmState;

	DEBUG_TRCIN();
	
	testRunSubFsmState = EINV_n_GetTestRunSubStatusValue();

	/* メッセージ処理 */
	switch (trigInfo->pMsg->msgId)
	{
	case LFW_MSGID_TIMER_TIMEOUT:
		if (msg->timerEvent.timerId == EINV_TIM_ID_CYCLE)
		{
			if (s_einvCurState != EINV_ST_FW_UPDATE)
			{
				/* ワンショットタイマ起動 */
				FRM_TimerSetOneShot(EINV_TIM_ID_CYCLE, EINV_CYCLE_TIME_MS, NULL);
				einv_ProcInvCommV(trigInfo);
			}
			else
			{
				/* 処理無し */
			}
		}
		else if(msg->timerEvent.timerId == (EINV_TIM_ID_COMM_ERR + g_einvCommErrSeq))
		{
			/* 通信不通エラー判定を実行 */
			einv_ProcNotInvCommV(trigInfo);
		}
		else if (msg->timerEvent.timerId == EINV_TIM_ID_INIT_ERR)
		{
			if (s_einvCurState == EINV_ST_START)
			{
				/* INVマイコン初期化通信失敗判定を実行 */
				einv_ProcNotInvCommV(trigInfo);
			} 
			else
			{
				/* 処理無し */
			}
		}
		else if(msg->timerEvent.timerId == EINV_TIM_ID_TESTRUNCYCLE)
		{
			if(testRunSubFsmState == EINV_TESTRUN_IDLE)
			{
				/*試運転IDLE状態*/
				/*TESTRUN CMD65コマンドデータ作成＆CMD65コマンド発行*/
				/*試運転開始指示*/
				/*CMD65発行指示retry開始*/
				par.TestRunReq.header.msgId = EINV_FID_TESTRUN_CMD65;
				par.TestRunReq.header.attr == LFW_MSG_ASYNC;
				par.TestRunReq.command = EINV_SENDCMD65;
				par.TestRunReq.fact = EINV_TESTRUNSTART;
				einv_ProcInvCommV(&par);
				/*CMD65周期1sタイマ起動*/
				FRM_TimerSetOneShot(EINV_TIM_ID_TESTRUNCYCLE, EINV_CYCLE_TIME_MS, NULL);
			}
			else if(testRunSubFsmState == EINV_TESTRUN_ABORT)
			{	/*試運転中止処理状態*/
				/*CMD65発行指示retry停止*/
				par.TestRunReq.header.msgId = EINV_FID_TESTRUN_CMD65;
				par.TestRunReq.header.attr == LFW_MSG_ASYNC;
				par.TestRunReq.command = EINV_SENDCMD65;
				par.TestRunReq.fact = EINV_TESTRUNABORT;
				einv_ProcInvCommV(&par);
				/*CMD65周期1sタイマ起動*/
				FRM_TimerSetOneShot(EINV_TIM_ID_TESTRUNCYCLE, EINV_CYCLE_TIME_MS, NULL);
			}
			else if(testRunSubFsmState == EINV_TESTRUN_FINISH)
			{
				/*試運転後処理状態*/
				/*CMD65発行指示retry停止*/
				par.TestRunReq.header.msgId = EINV_FID_TESTRUN_CMD65;
				par.TestRunReq.header.attr == LFW_MSG_ASYNC;
				par.TestRunReq.command = EINV_SENDCMD65;
				par.TestRunReq.fact = EINV_TESTRUNSTOP;
				einv_ProcInvCommV(&par);
				/*CMD65周期1sタイマ起動*/
				FRM_TimerSetOneShot(EINV_TIM_ID_TESTRUNCYCLE, EINV_CYCLE_TIME_MS, NULL);
			}
			else
			{
				
			}
		}
		else if(msg->timerEvent.timerId == EINV_TIM_ID_TESTRUNRESET)
		{
			/*SOFT RESET通知*/
			/*SOFT RESETモジュールへメッセージ発行し、ソフトリセット実施すること*/
		}
		else
		{
			DLOG_Error("unexpected msgid(%u) occur\n", trigInfo->pMsg->msgId);
		}
		break;
	case LFW_MSGID_ALARM_TIMEOUT:
		break;
	default:
		DLOG_Error("unexpected msgid(%u) occur\n", trigInfo->pMsg->msgId);
		break;
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description	:	INVマイコン通信 試運転処理								*/
/*--------------------------------------------------------------------------*/
/*	param		:trigInfo	要求メッセージ									*/
/*--------------------------------------------------------------------------*/
/*	return		:なし														*/
/*--------------------------------------------------------------------------*/
/*	detail		:	INVマイコン通信 試運転処理								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/26 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
static void einv_TestRunProc(const FsmTrigerInfo_t *trigInfo)
{
	uchar event = trigInfo->pMsg->msgId - EINV_TESTRUN_FUNCID_OFFSET;
	SubFsmTrigerInfo_t *msg = (SubFsmTrigerInfo_t*)trigInfo->pMsg;
	slong ret = EINV_OK;
	TestRunSubFsmState_t testRunSubFsmState;

	DEBUG_TRCIN();
	
	testRunSubFsmState = EINV_n_GetTestRunSubStatusValue();
	
	if (NULL != s_einvSubFsmTransTbl[event][testRunSubFsmState].action)
	{
		ret = s_einvSubFsmTransTbl[event][testRunSubFsmState].action(msg);
	}
	else
	{
		/* 処理無し */
	}
	
	/*エラー後処理*/
	
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description	:	INVマイコン通信 試運転前処理							*/
/*--------------------------------------------------------------------------*/
/*	param		:	trigInfo	要求メッセージ								*/
/*--------------------------------------------------------------------------*/
/*	return		:	なし													*/
/*--------------------------------------------------------------------------*/
/*	detail		:	INVマイコン通信 試運転前処理							*/
/*				:	ログスイッチ											*/
/*				:	試運転済みファイルを削除								*/
/*				:	CMD65/75発行											*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/05/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
static slong einv_TestRunSubProcIdle(const SubFsmTrigerInfo_t *trigInfo)
{
	ushort 	msgId;
	slong ret = EINV_OK;
	EinvMsgPar_t msg;
	uchar 	testRunStatusFlag;
	uchar   testRunSendCmd75FinishFlag;
	uchar   testRunSendCmd65FinishFlag;
	msgId = trigInfo->pMsg->msgId;
	EinvSysStruct_t sysStruct;
	
	DEBUG_TRCIN();
	
	switch (msgId)
	{
	
	case EINV_FID_TESTRUN_INDICATE:
		/*システム構成設置*/
		/*チャックシステム構成*/
		ret = EINV_n_CheckSysStructure(g_einvCmd60_r.sysStruct);
		if(ret == EINV_VALID){
			/*システム構成設置＆ASCIICODE->DATA変換*/
			sysStruct.sysStructAscii = g_einvCmd60_r.sysStruct;
			sysStruct.sysStructNum = EINV_n_ConvertSysStructureFromAsciiToNum(g_einvCmd60_r.sysStruct);
			EINV_n_SetSysStructValue(&sysStruct);
		}
		else
		{
			ret = EINV_ERR_SYS_CONF;
		}
		
		/*運転情報ログを更新する*/
	
		/*試運転済みファイルを削除する*/
		
		ret = EINV_n_CalcNeedSendCmd(EINV_SENDCMD65);
		if(ret == EINV_NEED){
			/*TESTRUN CMD65コマンドデータ作成＆CMD65コマンド発行*/
			/*試運転開始指示*/
			/*CMD65発行指示*/
			EINV_n_SetTestRunCmd65SendFinishValue(EINV_CLR);
			msg.TestRunReq.header.msgId = EINV_FID_TESTRUN_CMD65;
			msg.TestRunReq.header.attr = LFW_MSG_ASYNC;
			msg.TestRunReq.command = EINV_SENDCMD65;
			msg.TestRunReq.fact = EINV_TESTRUNSTART;
			einv_ProcInvCommV(&msg);
			/*CMD65周期1sタイマ起動*/
			ret = FRM_TimerSetOneShot(EINV_TIM_ID_TESTRUNCYCLE, EINV_CYCLE_TIME_MS, NULL);
		}
		else if(ret == EINV_NONEED)
		{
			EINV_n_SetTestRunCmd65SendFinishValue(EINV_SET);
		}
		else
		{
			
		}
		ret = EINV_n_CalcNeedSendCmd(EINV_SENDCMD75);
		if(ret == EINV_NEED){
			/*TESTRUN CMD75コマンドデータ作成＆CMD75コマンド発行*/
			/*CMD75発行指示*/
			EINV_n_SetTestRunCmd75SendFinishValue(EINV_CLR);
			msg.TestRunReq.header.msgId = EINV_FID_TESTRUN_CMD75;
			msg.TestRunReq.header.attr = LFW_MSG_ASYNC;
			msg.TestRunReq.command = EINV_SENDCMD75;
			einv_ProcInvCommV(&msg);
		}
		else if(ret == EINV_NONEED)
		{
			/*CMD75発行不必要なら*/
			EINV_n_SetTestRunCmd75SendFinishValue(EINV_SET);
		}
		else
		{
			
		}
		break;
	case EINV_FID_TESTRUN_CMD65_OK:
		EINV_n_SetTestRunCmd65SendFinishValue(EINV_SET);
		testRunStatusFlag = EINV_n_GetCmd81TestRunOutInfromValue();
		testRunSendCmd75FinishFlag = EINV_n_GetTestRunCmd75SendFinishValue();
		if(testRunStatusFlag == EINV_TESTRUN_ON)
		{
			/*CMD65周期1sタイマ停止*/
			FRM_TimerCancelOneShot(EINV_TIM_ID_TESTRUNCYCLE);
			if(testRunSendCmd75FinishFlag == EINV_SET)
			{
				/*試運転サブ状態更新試運転中*/
				EINV_n_SetTestRunSubStatusValue(EINV_TESTRUN_DO);
				/*[LAN:試運転開始可]返信*/
				/*6時間タイマ処理*/
				ret = EINV_n_SixHourTimerCtl();
			}
			else
			{
				
			}
		}
		else
		{
			/*TMOUT処理*/
			/*TMOUT時[LAN:試運転開始不可]返信*/
			/*関連状態値と変数値復帰*/
		}
		break;
	case EINV_FID_TESTRUN_CMD75_OK:
		EINV_n_SetTestRunCmd75SendFinishValue(EINV_SET);
		testRunSendCmd65FinishFlag = EINV_n_GetTestRunCmd65SendFinishValue();
		testRunStatusFlag = EINV_n_GetCmd81TestRunOutInfromValue();
		if(testRunStatusFlag == EINV_TESTRUN_ON)
		{
			/*CMD65周期1sタイマ停止*/
			FRM_TimerCancelOneShot(EINV_TIM_ID_TESTRUNCYCLE);
			if(testRunSendCmd65FinishFlag == EINV_SET)
			{
				/*試運転サブ状態更新試運転中*/
				EINV_n_SetTestRunSubStatusValue(EINV_TESTRUN_DO);
				/*[LAN:試運転開始可]返信*/
				/*6時間タイマ処理*/
				ret = EINV_n_SixHourTimerCtl();
			}
			else
			{
				
			}
		}
		else
		{
			/*TMOUT処理*/
			/*TMOUT時[LAN:試運転開始不可]返信*/
			/*関連状態値と変数値復帰*/
		}
		break;
	case EINV_FID_TESTRUN_CMD_NG:
		/*通信異常処理コマンド発行異常*/
		/*異常処理*/
		/*関連状態値と変数値復帰*/
		/*[LAN:試運転開始不可]返信*/
		break;
	default:
		break;
	}
	
	DEBUG_TRCOUT();
	
	return ret;
}

/*==========================================================================*/
/*	Description	:	INVマイコン通信 試運転中処理							*/
/*--------------------------------------------------------------------------*/
/*	param		:	trigInfo	要求メッセージ								*/
/*--------------------------------------------------------------------------*/
/*	return		:	EINV_OK													*/
/*				:	FRM_TIMER_E_	参照									*/
/*--------------------------------------------------------------------------*/
/*	detail		:	INVマイコン通信 試運転中処理							*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/05/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
static slong einv_TestRunSubProcDo(const SubFsmTrigerInfo_t *trigInfo)
{
	ushort 	msgId;
	EinvMsgPar_t msg;
	slong 	ret = EINV_OK;
	msgId = trigInfo->pMsg->msgId;
	
	DEBUG_TRCIN();
	
	switch (msgId)
	{
		case EINV_FID_TESTRUN_CMD_NG:
			/*通信異常処理コマンド発行異常*/
			/*異常処理*/
			/*関連状態値と変数値復帰*/
			/*試運転中失敗*/
			break;
		case EINV_FID_TESTRUN_TERM:
			/*TESTRUN CMD65コマンドデータ作成＆CMD65コマンド発行*/
			/*試運転停止指示*/
			/*CMD65発行指示*/
			msg.TestRunReq.header.msgId = EINV_FID_TESTRUN_CMD65;
			msg.TestRunReq.header.attr == LFW_MSG_ASYNC;
			msg.TestRunReq.command = EINV_SENDCMD65;
			msg.TestRunReq.fact = EINV_TESTRUNSTOP;
			einv_ProcInvCommV(&msg);
			/*CMD65周期1sタイマ起動*/
			ret = FRM_TimerSetOneShot(EINV_TIM_ID_TESTRUNCYCLE, EINV_CYCLE_TIME_MS, NULL);
			/*試運転サブ状態更新*/
			/*試運転サブ状態:後処理*/
			EINV_n_SetTestRunSubStatusValue(EINV_TESTRUN_FINISH);
			break;
		case EINV_FID_TESTRUN_ABORT:
			/*TESTRUN CMD65コマンドデータ作成＆CMD65コマンド発行*/
			/*試運転停止指示*/
			/*CMD65発行指示*/
			msg.TestRunReq.header.msgId = EINV_FID_TESTRUN_CMD65;
			msg.TestRunReq.header.attr == LFW_MSG_ASYNC;
			msg.TestRunReq.command = EINV_SENDCMD65;
			msg.TestRunReq.fact = EINV_TESTRUNABORT;
			einv_ProcInvCommV(&msg);
			/*CMD65周期1sタイマ起動*/
			ret = FRM_TimerSetOneShot(EINV_TIM_ID_TESTRUNCYCLE, EINV_CYCLE_TIME_MS, NULL);
			/*試運転サブ状態更新*/
			/*試運転サブ状態:中止処理*/
			EINV_n_SetTestRunSubStatusValue(EINV_TESTRUN_ABORT);
			break;
		default:
			break;
	}
	DEBUG_TRCOUT();
	
	return ret;
}

/*==========================================================================*/
/*	Description	:	INVマイコン通信 試運転後処理							*/
/*--------------------------------------------------------------------------*/
/*	param		:	trigInfo	要求メッセージ								*/
/*--------------------------------------------------------------------------*/
/*	return		:	FRM_TIMER_E_	参照									*/
/*				:	EINV_OK													*/
/*--------------------------------------------------------------------------*/
/*	detail		:	INVマイコン通信 試運転後処理							*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/05/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
static slong einv_TestRunSubProcFin(const SubFsmTrigerInfo_t *trigInfo)
{
	ushort 	msgId;
	slong 	ret = EINV_OK;
	msgId = trigInfo->pMsg->msgId;
	uchar 	testRunStatusFlag;
	
	DEBUG_TRCIN();
	
	switch (msgId)
	{
		case EINV_FID_TESTRUN_CMD65_OK:
			testRunStatusFlag = EINV_n_GetCmd81TestRunOutInfromValue();
			if(testRunStatusFlag == EINV_TESTRUN_OFF)
			{	
				/*CMD65周期1sタイマ停止*/
				ret = FRM_TimerCancelOneShot(EINV_TIM_ID_TESTRUNCYCLE);
				/*スマホアプリから受信する試運転結果から試運転済みファイルを生成する。 試運転済みファイルの生成は表 3-1 を参照*/
				
				/*運転情報ログを CMD65,75 で送信する。*//*運転情報ログを更新する*/
				
				/*関連状態値と変数値復帰*/
				
				
				/*30s ONESHOTタイマ起動*/
				/*30s後 SOFT RESET*/
				ret = FRM_TimerSetOneShot(EINV_TIM_ID_TESTRUNRESET, EINV_30ONESHOT_TIME_MS, NULL);
			}
			else
			{
				/*TMOUT処理*/
				/*試運転中失敗*/
				/*関連状態値と変数値復帰*/
			}
			break;
		case EINV_FID_TESTRUN_CMD_NG:
			/*通信異常処理コマンド発行異常*/
			/*異常処理*/
			/*関連状態値と変数値復帰*/
			/*試運転中失敗*/
			break;
		default:
			break;
	}
	
	DEBUG_TRCOUT();
	
	return ret;
}

/*==========================================================================*/
/*	Description	:	INVマイコン通信 試運転中止処理							*/
/*--------------------------------------------------------------------------*/
/*	param		:	trigInfo	要求メッセージ								*/
/*--------------------------------------------------------------------------*/
/*	return		:	FRM_TIMER_E_	参照									*/
/*				:	EINV_OK													*/
/*--------------------------------------------------------------------------*/
/*	detail		:	INVマイコン通信 試運転中止処理							*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/05/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
static slong einv_TestRunSubProcAbort(const SubFsmTrigerInfo_t *trigInfo)
{
	ushort 	msgId;
	slong 	ret = EINV_OK;
	msgId = trigInfo->pMsg->msgId;
	uchar 	testRunStatusFlag;
	
	DEBUG_TRCIN();
	
	switch (msgId)
	{
		case EINV_FID_TESTRUN_CMD65_OK:
			testRunStatusFlag = EINV_n_GetCmd81TestRunOutInfromValue();
			if(testRunStatusFlag == EINV_TESTRUN_OFF)
			{	
				/*CMD65周期1sタイマ停止*/
				ret = FRM_TimerCancelOneShot(EINV_TIM_ID_TESTRUNCYCLE);
				/*試運転サブ状態復帰*/
				EINV_n_SetTestRunSubStatusValue(EINV_INVALIDVALUE);
				/*MAIN状態復帰*/
				EINV_n_SetSysStatusValue(EINV_SYS);
				/*試運転CMD75コマンド発行フラッグ*/
				EINV_n_SetTestRunCmd75SendFinishValue(EINV_CLR);
				/*試運転CMD65コマンド発行フラッグ*/
				EINV_n_SetTestRunCmd65SendFinishValue(EINV_CLR);
				/*状態遷移マトリクス状態復帰*/
				EINV_FsmSetState(EINV_ST_NORMAL);
				/*管理運転モードを待機モードへ切替*/
				EINV_n_SetCtlRunModeValue(EINV_COMMU_BOARD_MANAGE_MODE_7);
				/*6時間タイマ処理*/
				ret = EINV_n_SixHourTimerCtl();
				/*試運転指示設定変更フラッグクリア*/
				EINV_n_ClrUpDateFlag();
			}
			else
			{
				/*TMOUT処理*/
				/*試運転中失敗*/
				/*関連状態値と変数値復帰*/
			}
			break;
		case EINV_FID_TESTRUN_CMD_NG:
			/*通信異常処理コマンド発行異常*/
			/*異常処理*/
			/*関連状態値と変数値復帰*/
			/*試運転中失敗*/
			break;
		default:
			break;
	}
	
	DEBUG_TRCOUT();
	
	return ret;
}

/*==========================================================================*/
/*	Description		:非INV通信IF処理										*/
/*--------------------------------------------------------------------------*/
/*	param			:reqMsg		要求メッセージ								*/
/*--------------------------------------------------------------------------*/
/*	return			:ret	EINV_OK					正常終了				*/
/*							EPCS_ERR_SYSCALL		システムコールエラー	*/
/*							EPCS_ERR_FAIL			処理失敗エラー			*/
/*							EINV_ERR_NOT_SUPPORT	非サポートエラー		*/
/*--------------------------------------------------------------------------*/
/*	detail			:通信IF処理作成											*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC Feng.XZ : 新規作成					*/
/*==========================================================================*/
static slong einv_ProcNotInvCommV(const FsmTrigerInfo_t *trigInfo)
{
	slong			ret = EINV_OK;
	EinvMsgPar_t	*reqMsg = NULL;

	DEBUG_TRCIN();

	// 引数チェック
	if(trigInfo == NULL)
	{
		DLOG_Error("trigInfo is NULL.\n");
		DEBUG_TRCOUT();
		return EINV_ERR_PARAM;
	}
	else
	{
		/* 処理無し */
	}

	reqMsg = (EinvMsgPar_t*)trigInfo->pMsg;

	/* イベント通知の場合 */
	if (reqMsg->header.attr == FRM_MSG_EVENT){
		switch(reqMsg->header.msgId)
		{
		case LFW_MSGID_TIMER_TIMEOUT:		/* ワンショットタイマイベント通知 */
			/* INV通信不通エラー判定ワンショットタイマの場合 */
			if(reqMsg->timerEvent.timerId == (EINV_TIM_ID_COMM_ERR + g_einvCommErrSeq)){
				/* 通信不通エラー判定を実行 */
				ret = EINV_n_SetInvCommErr();
			}
			else if (reqMsg->timerEvent.timerId == EINV_TIM_ID_INIT_ERR)
			{
				/* INVマイコン初期化通信失敗判定を実行 */
				ret = EINV_n_SetInvInitErr();
			}
			else
			{
				ret = EINV_ERR_NOT_SUPPORT;
			}
			break;
			/* メンテナンスモード開始処理用のコードを記述 */
		case LFW_MSGID_ALARM_TIMEOUT:
			//if(etimReqMsg->id == (EPCS_TIM_ID_MAINTENANCE + g_maintenance_seq)){
			//	epcs_n_startMaintenanceMode();
			//}else{
			//	ret = EPCS_ERR_NOT_SUPPORT;
			//}
			//break;
			/**/
		default:
			ret = EINV_ERR_NOT_SUPPORT;
			break;
		}
		//case ESCH_SPORT:		/* スケジュール管理ミドルからのイベント */
		//	switch(reqMsg->header.msgId){
		//		//case ESCH_EVT_SETTING_CHANGE:	// スケジュール管理ミドル状態変化通知
		//		//	ret = epcs_n_notifyEschSttsChange((ESCH_EVT_SETTING_CHANGE_T*)reqMsg);
		//		//	break;
		//	default:	/* 想定外イベント受信 */
		//		ret = EINV_ERR_NOT_SUPPORT;
		//		break;
		//	}
		//	break;
		//default:
		//	ret = EINV_ERR_NOT_SUPPORT;
		//	break;
		//}
	}else if(reqMsg->header.attr == FRM_MSG_ASYNC){
		switch(reqMsg->header.msgId)
		{

		default:
			ret = EINV_ERR_NOT_SUPPORT;
			break;
		}
	}
	else
	{
		ret = EINV_ERR_NOT_SUPPORT;
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:INV通信ありIF処理										*/
/*--------------------------------------------------------------------------*/
/*	param			:reqMsg		要求メッセージ								*/
/*--------------------------------------------------------------------------*/
/*	return			:ret	EINV_OK		正常終了							*/
/*							EINV_ERR_PARAM									*/
/*--------------------------------------------------------------------------*/
/*	detail			:通信ありIF処理作成										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC Feng.XZ : 新規作成					*/
/*==========================================================================*/
static slong einv_ProcInvCommV(const FsmTrigerInfo_t *trigInfo)
{
	slong ret = EINV_OK;
	EinvMsgPar_t* reqMsg = NULL;
	DEBUG_TRCIN();

	/* 引数チェック */
	if(trigInfo == NULL)
	{
		DLOG_Error("EINV_procPcsCommV(),EINV_ERR_PARAM,trigInfo == NULL.\n");
		DEBUG_TRCOUT();
		return EINV_ERR_PARAM;
	}
	else
	{
		/* 処理無し */
	}

	reqMsg = (EinvMsgPar_t*)trigInfo->pMsg;

	ret = EINV_EnqCommReqQue(reqMsg);

	DEBUG_TRCOUT();
	return(ret);
}

/*==========================================================================*/
/*	Description		:	取得運転モード設定依頼元,指定電力運転フラグ			*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Chongyang.Chen : 新規作成				*/
/*==========================================================================*/
slong EINV_SpecGetOpeMode(EinvOpemodeGet_t *opeModeGet)
{
	slong ret = EINV_OK;

	DEBUG_TRCIN();
	
	ret = EINV_n_GetOpeMode(opeModeGet);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:	動作情報取得										*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Chongyang.Chen : 新規作成				*/
/*==========================================================================*/
slong EINV_SpecGetOpeSttsInfo(EinvOpesttsInfo_t *opeSttsInfo)
{
	slong ret = EINV_OK;

	DEBUG_TRCIN();
	
	ret = EINV_n_GetOpeSttsInfo(opeSttsInfo);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:	CMD41表示用シグナル処理値取得						*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/05 : DHC Chongyang.Chen : 新規作成				*/
/*==========================================================================*/
slong EINV_SpecGetCalcAvgValue(EinvCalcAvg_t *calcAvgValueGet)
{
	slong ret = EINV_OK;

	DEBUG_TRCIN();

	ret = EINV_n_GetCalcAvgValue(calcAvgValueGet);

	DEBUG_TRCOUT();

	return ret;
}
/*End Of File*/
