/****************************************************************************/
/*	Description		:LED制御プロセスメイン									*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Gu.CM												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/10												*/
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "task_g.h"
#include "common_g.h"

#include "dlog_g.h"
#include "eled_file_g.h"
#include "eled_serv_g.h"
#include "eled_g.h"
#include "lfw_g.h"
#include "task_g.h"
#include "eled_msg_g.h"
#include "esys_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:LED模块初始化											*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/15 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_Init(void)
{
	DEBUG_TRCIN();

	ELED_ServInit();

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:LED模块消息处理										*/
/*--------------------------------------------------------------------------*/
/*	param			:FrmMsgHead_t const *msg 消息							*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/15 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_MsgProc(FrmMsgHead_t const *msg)
{
	DEBUG_TRCIN();

	switch(msg->attr)
	{
	/* 非同期メッセージ */
	case FRM_MSG_ASYNC:
		switch(msg->msgId)
		{
		case ELED_MSGID_START:
			ELED_ProcStart(msg);
			ESYS_NotifyStartResult(ELED_SPORT, MOD_ACTION_RESULT_SUCCESS);
			break;
		case ELED_MSGID_STOP:
			break;
		case ELED_MSGID_MODE_CHG:
		{
			EledModeChg_t const *modeChgMsg = (EledModeChg_t const *)msg;
			ELED_SetLedMode(modeChgMsg->targetMode);
			break;
		}
		case ELED_MSGID_MODE_SET:
		{
			EledModeChg_t const *modeChgMsg = (EledModeChg_t const *)msg;
			DLOG_Debug("set mode=%d", modeChgMsg->targetMode);
			ELED_SetLedMode(modeChgMsg->targetMode);
			break;
		}
		case ELED_MSGID_MODE_CLR:
		{
			EledModeChg_t const *modeChgMsg = (EledModeChg_t const *)msg;
			DLOG_Debug("clr mode=%d", modeChgMsg->targetMode);
			ELED_LedBlinkStop(modeChgMsg->targetMode);
			break;
		}
		case ELED_MSGID_FIRST_SEQ:
			ELED_ProcDoFirstSeq();
			break;
		case ELED_MSGID_NEXT_SEQ:
			ELED_ProcDoNextSeq();
			break;
		case ELED_MSGID_FIRST_STEP:
			ELED_ProcDoFirstStep();
			break;
		case ELED_MSGID_NEXT_STEP:
			ELED_ProcDoNextStep();
			break;
		default:
			DLOG_Info("--unknown async msgId : %d--\n",msg->msgId);
			break;
		}
		break;
	/* イベントメッセージ */
	case FRM_MSG_EVENT:
		switch(msg->msgId)
		{
		case LFW_MSGID_TIMER_TIMEOUT:
		{
			FrmTimeout_t const *timeoutMsg = (FrmTimeout_t const *)msg;
			if (ELED_TIMEID_TRIGGER == timeoutMsg->timerId)
			{
				ELED_ProcDoNextStep();
			}
			break;
		}
		case LFW_MSGID_WDT_REQ:
			ELED_EwdtRes(msg);
			break;
		case LFW_MSGID_WDT_RES:
			ELED_EwdtTermReg(msg);
			break;
		default:
			break;
		}
	default:
		/* DLOG_Info("--unknown attr : %d--\n",msg->attr); */
		break;
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:LED模块资源释放										*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/15 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_Fin(void)
{
	/* TODO */
}

/*==========================================================================*/
/*	Description		:起動時処理												*/
/*--------------------------------------------------------------------------*/
/*	param			:wdtFlag WDTからの再起動フラグ(1の場合再起動)			*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/15 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
slong ELED_Start(slong fact)
{
	EledStart_t msg;

	DEBUG_TRCIN();

	ELED_OpenGpioStart();

	msg.header.msgId = ELED_MSGID_START;
	msg.fact = fact;
	FRM_MsgSendAsync(ELED_SPORT, sizeof(msg), &msg);

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:LED模块停止											*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:slong													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/15 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
slong ELED_Stop(void)
{
	DEBUG_TRCIN();

	ELED_LedOffStepProc();
	ELED_CloseGpioStop();

	ESYS_NotifyStopResult(ELED_SPORT, MOD_ACTION_RESULT_SUCCESS);

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:系统启动运行相关LED闪烁开始							*/
/*--------------------------------------------------------------------------*/
/*	param			:EledPwrLed_t pwrLed 闪烁模式							*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/15 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_PwrLedBlankStart(EledPwrLed_t pwrLed)
{
	EledModeChg_t msg;
	DEBUG_TRCIN();

	EledWorkMode_t tgtMode = ELED_WORKMODE_MAX;
	switch(pwrLed)
	{
	case ELED_PWRLED_ONING:
		tgtMode = ELED_WORKMODE_PWRON;
		break;
	case ELED_PWRLED_RUNNING:
		tgtMode = ELED_WORKMODE_PWRRUN;
		break;
	case ELED_PWRLED_OFFING:
		tgtMode = ELED_WORKMODE_PWROFF;
		break;
	case ELED_PWRLED_TESTRUN:
		tgtMode = ELED_WORKMODE_TESTRUN;
		break;
	case ELED_PWRLED_SHUTDOWN:
		tgtMode = ELED_WORKMODE_SHUTDOWN;
		break;
	case ELED_PWRLED_ERROR:
		tgtMode = ELED_WORKMODE_ERROR;
		break;
	default:
		break;
	}
	if (ELED_WORKMODE_MAX != tgtMode)
	{
		msg.header.msgId = ELED_MSGID_MODE_SET;
		msg.targetMode = tgtMode;
		FRM_MsgSendAsync(ELED_SPORT, sizeof(msg), &msg);
	}

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:FW更新相关LED闪烁开始									*/
/*--------------------------------------------------------------------------*/
/*	param			:EledFwLed_t fwLed 闪烁模式								*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_FwLedBlankStart(EledFwLed_t fwLed)
{
	EledModeChg_t msg;
	DEBUG_TRCIN();

	EledWorkMode_t tgtMode = ELED_WORKMODE_MAX;

	switch(fwLed)
	{
	case ELED_FWLED_DOWNLOAD:
		tgtMode = ELED_WORKMODE_FWDOWN;
		break;
	case ELED_FWLED_UPDATE:
		tgtMode = ELED_WORKMODE_FWUPDATA;
		break;
	case ELED_FWLED_FINISH:
		tgtMode = ELED_WORKMODE_FWFINISH;
		break;
	case ELED_FWLED_ERR_E1BOARD:
		tgtMode = ELED_WORKMODE_FWE1ERR;
		break;
	case ELED_FWLED_ERR_INV:
		tgtMode = ELED_WORKMODE_FWINVERR;
		break;
	case ELED_FWLED_ERR_PV:
		tgtMode = ELED_WORKMODE_FWPVERR;
		break;
	case ELED_FWLED_ERR_BDC:
		tgtMode = ELED_WORKMODE_FWBDCERR;
		break;
	case ELED_FWLED_ERR_JB:
		tgtMode = ELED_WORKMODE_FWJBERR;
		break;
	case ELED_FWLED_ERR_OTHER:
		tgtMode = ELED_WORKMODE_FWOTHERR;
		break;
	default:
		break;
	}

	if (ELED_WORKMODE_MAX != tgtMode)
	{
		msg.header.msgId = ELED_MSGID_MODE_SET;
		msg.targetMode = tgtMode;
		FRM_MsgSendAsync(ELED_SPORT, sizeof(msg), &msg);
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:系统启动运行相关LED闪烁停止							*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/15 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_PwrLedBlankStop(EledPwrLed_t pwrLed)
{
	EledModeChg_t msg;
	DEBUG_TRCIN();

	EledWorkMode_t tgtMode = ELED_WORKMODE_MAX;
	switch(pwrLed)
	{
	case ELED_PWRLED_ONING:
		tgtMode = ELED_WORKMODE_PWRON;
		break;
	case ELED_PWRLED_RUNNING:
		tgtMode = ELED_WORKMODE_PWRRUN;
		break;
	case ELED_PWRLED_OFFING:
		tgtMode = ELED_WORKMODE_PWROFF;
		break;
	case ELED_PWRLED_TESTRUN:
		tgtMode = ELED_WORKMODE_TESTRUN;
		break;
	case ELED_PWRLED_SHUTDOWN:
		tgtMode = ELED_WORKMODE_SHUTDOWN;
		break;
	case ELED_PWRLED_ERROR:
		tgtMode = ELED_WORKMODE_ERROR;
		break;
	default:
		break;
	}
	if (ELED_WORKMODE_MAX != tgtMode)
	{
		msg.header.msgId = ELED_MSGID_MODE_CLR;
		msg.targetMode = tgtMode;
		FRM_MsgSendAsync(ELED_SPORT, sizeof(msg), &msg);
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:FW更新相关LED闪烁停止									*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_FwLedBlankStop(EledFwLed_t fwLed)
{
	EledModeChg_t msg;
	EledWorkMode_t tgtMode = ELED_WORKMODE_MAX;
	DEBUG_TRCIN();

	switch(fwLed)
	{
	case ELED_FWLED_DOWNLOAD:
		tgtMode = ELED_WORKMODE_FWDOWN;
		break;
	case ELED_FWLED_UPDATE:
		tgtMode = ELED_WORKMODE_FWUPDATA;
		break;
	case ELED_FWLED_FINISH:
		tgtMode = ELED_WORKMODE_FWFINISH;
		break;
	case ELED_FWLED_ERR_E1BOARD:
		tgtMode = ELED_WORKMODE_FWE1ERR;
		break;
	case ELED_FWLED_ERR_INV:
		tgtMode = ELED_WORKMODE_FWINVERR;
		break;
	case ELED_FWLED_ERR_PV:
		tgtMode = ELED_WORKMODE_FWPVERR;
		break;
	case ELED_FWLED_ERR_BDC:
		tgtMode = ELED_WORKMODE_FWBDCERR;
		break;
	case ELED_FWLED_ERR_JB:
		tgtMode = ELED_WORKMODE_FWJBERR;
		break;
	case ELED_FWLED_ERR_OTHER:
		tgtMode = ELED_WORKMODE_FWOTHERR;
		break;
	default:
		break;
	}

	if (ELED_WORKMODE_MAX != tgtMode)
	{
		msg.header.msgId = ELED_MSGID_MODE_CLR;
		msg.targetMode = tgtMode;
		FRM_MsgSendAsync(ELED_SPORT, sizeof(msg), &msg);
	}

	DEBUG_TRCOUT();
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*End Of File*/
