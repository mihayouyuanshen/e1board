/****************************************************************************/
/*	Description		:系统管理模块											*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Chen.H											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/27												*/
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
#include "typedef.h"
#include "frm_g.h"
#include "dlog_g.h"
#include "wdt_g.h"
#include "esys_mod.h"
#include "esys_cmn_g.h"
#include "esys_func_g.h"
#include "esys_fsm_g.h"
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
slong s_esysStartFactor = ESYS_START_FACT_NORMAL;
/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:系统管理功能初始化										*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:系统管理功能初始化										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
void ESYS_Init(void)
{
	EsysMsgUnion_t msg;
	DEBUG_TRCIN();

	esys_SystemWakeup();
#ifdef USE_FSM
	esys_FsmInit(ESYS_ST_IDLE);
#endif
	/* メッセージPORTセット */
	LFW_SetReceiver(ESYS_SPORT);
	msg.header.msgId = ESYS_MSGID_START;
	FRM_MsgSendAsync(ESYS_SPORT, sizeof(msg), &msg);
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:系统管理功能消息处理									*/
/*--------------------------------------------------------------------------*/
/*	param			:FrmMsgHead_t const *msg：消息							*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:系统管理功能消息处理									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
void ESYS_MsgProc(FrmMsgHead_t const *msg)
{
	EsysMsgUnion_t *pmsg =(EsysMsgUnion_t *)msg;
	DEBUG_TRCIN();

	if (NULL == msg)
	{
		DLOG_Error("NULL pointer");
		DEBUG_TRCOUT();
		return;
	}
	else
	{
#ifdef USE_FSM
		esys_fsmRun(msg);
#else
		switch(pmsg->header.attr)
		{
			/* 非同期メッセージ */
			case LFW_MSG_ASYNC:
				switch(pmsg->header.msgId) {
					case ESYS_MSGID_START:
						ESYS_Start(s_esysStartFactor);
						break;
					case ESYS_MSGID_KEY1_PRESSED:
					case ESYS_MSGID_KEY1_LONG_PRESSED:
					case ESYS_MSGID_KEY2_PRESSED:
					case ESYS_MSGID_KEY2_LONG_PRESSED:
						esys_KeyEventProc(pmsg->header.msgId);
						break;
					case ESYS_MSGID_POWER_CUT_ST_CHANGE:
						{
							EsysPowerCutSt_t *priMsg = 
												(EsysPowerCutSt_t *)pmsg;
							esys_PowerCutStChgProc(priMsg->status);
						}
						break;
					case ESYS_MSGID_ERROR_HAPPEN:
						{
							EsysMsgErrChg_t *priMsg = (EsysMsgErrChg_t *)pmsg;
							esys_ErrorHappenProc(&priMsg->errInfo);
						}
						break;
					case ESYS_MSGID_ERROR_CANCEL:
						{
							EsysMsgErrChg_t *priMsg = (EsysMsgErrChg_t *)pmsg;
							esys_ErrorCancelProc(&priMsg->errInfo);
						}
						break;
					case ESYS_MSGID_START_RESULT_NOTIFY:
						{
							EsysActRsltNotify_t *priMsg = 
												(EsysActRsltNotify_t *)pmsg;
							esys_ModStartRsltProc(priMsg->modId,
													priMsg->result);
						}
						break;
					case ESYS_MSGID_STOP_RESULT_NOTIFY:
						{
							EsysActRsltNotify_t *priMsg = 
												(EsysActRsltNotify_t *)pmsg;
							esys_ModStopRsltProc(priMsg->modId,
													priMsg->result);
						}
						break;
					case ESYS_MSGID_READ_EEP_ERR_HAPPEN:
						{
							esys_SetReadEEPromErr();
						}
						break;
					case ESYS_MSGID_READ_EEP_ERR_CLEAR:
						{
							esys_ClearReadEEPromErr();
						}
						break;
					default:
						DLOG_Error("Unknown AsyncMsg Rcv = %ld\n",
									pmsg->header.msgId);
						break;
				}
				break;
			/* イベントメッセージ */
			case LFW_MSG_EVENT:
				switch(pmsg->header.msgId) {
					/* WATCHDOG応答イベント */
					case WDT_EVT_WDT_REQ:
						esys_SendWdtRes();
						DLOG_Debug("Recv WDT Req!");
						break;
					/* 終了要求応答イベント */
					case WDT_EVT_TERM_REQ:
						{
							DLOG_Debug("Recv Term Req!");
							WdtEvtTermReq_t *priMsg = (WdtEvtTermReq_t *)pmsg;
							ESYS_Stop();
							esys_SendTermRes(priMsg->type);
						}
						break;
					default:
						DLOG_Error("Unknown AsyncMsg Rcv = %ld\n", pmsg->header.msgId);
						break;
				}
				break;
			default:
				DLOG_Error("Unknown Msg Rcv = %ld\n", pmsg->header.attr);
				break;
		}
#endif
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:系统管理功能结束处理									*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:系统管理功能结束处理									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
void ESYS_Fin(void)
{
	/* TODO */
}

/*==========================================================================*/
/*	Description		:系统管理功能开始										*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:系统管理功能开始处理									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
slong ESYS_Start(slong fact)
{
	ulong i;
	DEBUG_TRCIN();

	DLOG_Info("<<<<<<System Starting>>>>>>");
	printf("\n<<<<<<System Starting>>>>>>\n");
	for (i = MOD_ESYS + 1; i < MOD_NUM; i++)
	{
		g_esysModSchTbl[i].modStart(fact);
	}
	ELED_PwrLedBlankStart(ELED_PWRLED_RUNNING);
	DEBUG_TRCOUT();
	return ESYS_OK;
}

/*==========================================================================*/
/*	Description		:系统管理功能停止										*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:系统管理功能停止处理									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
slong ESYS_Stop(void)
{
	ulong i;
	DEBUG_TRCIN();
	DLOG_Info("<<<<<<System Stopping>>>>>>");
	printf("\n<<<<<<System Stopping>>>>>>\n");

	for (i = MOD_ESYS + 1; i < MOD_NUM; i++)
	{
		g_esysModSchTbl[i].modStop();
	}
	DEBUG_TRCOUT();
	return ESYS_OK;
}

/*==========================================================================*/
/*	Description		:检查启动因素											*/
/*--------------------------------------------------------------------------*/
/*	param			:slong fact:启动因素									*/
/*--------------------------------------------------------------------------*/
/*	return			:0-有效 -1-无效											*/
/*--------------------------------------------------------------------------*/
/*	detail			:检查启动因素有效性										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
slong ESYS_CheckStartFactor(slong fact)
{
	slong ret = ESYS_ERROR;
	DEBUG_TRCIN();
	if ((ESYS_START_FACT_MIN <= fact) && (fact <= ESYS_START_FACT_MAX))
	{
		ret = ESYS_OK;
	}
	else
	{
		/* 処理無し */
	}
	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:设定启动因素											*/
/*--------------------------------------------------------------------------*/
/*	param			:slong fact:启动因素									*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:设定启动因素，包含正常启动、重启、固件更新重启等			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
void ESYS_SetStartFactor(slong fact)
{
	DEBUG_TRCIN();
	s_esysStartFactor = fact;
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:获取启动因素											*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:0-正常启动 1-重启 2-固件更新重启						*/
/*--------------------------------------------------------------------------*/
/*	detail			:获取启动因素											*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
extern slong ESYS_GetStartFactor(void)
{
	DEBUG_TRCIN();
	DEBUG_TRCOUT();
	return s_esysStartFactor;
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*End Of File*/
