/****************************************************************************/
/*	Description		:WATCHDOG監視プロセスソースコード							*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Chen.H											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/28												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:EWDT													*/
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
#include <pthread.h>
#include <task_g.h>
#include <elog.h>
#include <common.h>
#include "ewdt.h"
#include "ewdt_local.h"

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
/*	Description		:WDT応答ファンクション処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong sport	該当プロセスのメッセージポート				*/
/*--------------------------------------------------------------------------*/
/*	param			:uchar timeout	タイムアウト値							*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
void ewdt_FncWdtRes(ulong sport, uchar timeout)
{
	DEBUG_TRCIN();
	elog_log(ELOG_LV_VERBOSE, "wdtResRcv(sport:%lu)(timeout:%d)\n", sport, timeout);

	/* パラメータチェック */
	if ((sport < LFW_SPORT_MIN)
	 	|| (sport >= (LFW_SPORT_MIN + PROCESS_MAX)))
	{
		DEBUG_TRCOUT();
		return;
	}
	else
	{
		/* mutex取得 */
		cmn_mutex_lock(&g_EWDT_Mtx);
		
		/* 生存カウンタリセット */
		g_EWDT_ProcWdtCnt[sport - LFW_SPORT_MIN] = timeout;
		g_EWDT_ProcWdtResFlg[sport - LFW_SPORT_MIN] = EWDT_FLG_ON;
		
		/* mutex解放 */
		cmn_mutex_unlock(&g_EWDT_Mtx);
	}

	DEBUG_TRCOUT();
	return;
}

/*==========================================================================*/
/*	Description		:終了要求ファンクション処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:EwdtTermType_t	type	終了種別						*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong	reqMsgid	要求元プロセスのメッセージキューID（SPORT）*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
void ewdt_FncTermReq(EwdtTermType_t type, ulong reqMsgid)
{
	slong ret = 0;
	slong idx = 0;
	slong procIdx = 0;
	EwdtEvtTermReq_t msg;
	DEBUG_TRCIN();
	elog_log(ELOG_LV_DEBUG, "termReqRcv(type:%d)\n", type);
	
	/* パラメータチェック(第1引数) */
	if ((type != EWDT_TERM_TYPE_REBOOT)
	 && (type != EWDT_TERM_TYPE_SHUTDOWN))
	{
		DEBUG_TRCOUT();
		return;
	}
	/* パラメータチェック(第2引数) */
	else if ((reqMsgid < LFW_SPORT_MIN)
	 || (reqMsgid >= (LFW_SPORT_MIN + PROCESS_MAX)))
	{
		DEBUG_TRCOUT();
		return;
	}
	/* 現在電源OFF処理中の場合は要求受けない */
	else if (g_EWDT_TermType == EWDT_TERM_TYPE_SHUTDOWN)
	{
		DEBUG_TRCOUT();
		return;
	}
	/* 現在の終了タイプと同じ場合は要求受けない */
	else if (g_EWDT_TermType == type)
	{
		DEBUG_TRCOUT();
		return;
	}
	else
	{
		/* 処理無し */
	}

	/* mutex取得 */
	cmn_mutex_lock(&g_EWDT_Mtx);
	
	/* 終了タイムアウト設定 */
	g_EWDT_TermTimeoutCnt = EWDT_TERM_TIMEOUT;
	
	/* 全プロセス終了フラグクリア */
	g_EWDT_ProcTermFinishFlg = EWDT_FLG_OFF;
	
	/* 終了タイプ保持 */
	g_EWDT_TermType = type;
	
	/* 終了ログ出力 */
	elog_log(ELOG_LV_ERROR, "<i>%s\n", (g_EWDT_TermType == EWDT_TERM_TYPE_REBOOT ? "Reboot" : "Shutdown"));
	
	/* 各プロセスに終了要求メッセージ送信 */
	memset(&msg, 0x00, sizeof(msg));
	msg.Header.MsgId = EWDT_EVT_TERM_REQ;
	msg.Type = g_EWDT_TermType;
	msg.Sport = reqMsgid;
	
	idx = EWDT_PROC_TBL_NUM - 1;
	for(idx = (EWDT_PROC_TBL_NUM - 1); idx >= 0; idx--)
	{
		procIdx = g_EWDT_ProcTbl[idx].Sport - LFW_SPORT_MIN;
		if (procIdx >= 0)
		{
			/* 終了確認フラグ初期化 */
			g_EWDT_ProcTermResFlg[procIdx] = EWDT_FLG_OFF;
			/* 常駐プロセスタイムアウトカウンタリセット */
			g_EWDT_ProcWdtCnt[procIdx] = EWDT_PROC_TIMEOUT_TERM;
			/* メッセージ送信 */
			ret = FRM_MsgSendEvent(g_EWDT_ProcTbl[idx].Sport,
									sizeof(msg), &msg);
		}
		else
		{
			/* 処理無し */
		}
	}
	
	/* mutex解放 */
	cmn_mutex_unlock(&g_EWDT_Mtx);
	DEBUG_TRCOUT();
	return;
}

/*==========================================================================*/
/*	Description		:終了応答ファンクション処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong	sport	該当プロセスのメッセージポート				*/
/*--------------------------------------------------------------------------*/
/*	param			:EwdtTermType_t	type	終了種別						*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
void ewdt_FncTermRes(ulong sport, EwdtTermType_t type)
{
	slong idx = 0;
	bool_t isAllProcRes = 1;
	DEBUG_TRCIN();
	elog_log(ELOG_LV_DEBUG, "termResRcv(sport:%lu)(type:%d)\n", sport, type);
	
	/* パラメータチェック */
	if ((sport < LFW_SPORT_MIN)
	 || (sport >= (LFW_SPORT_MIN + PROCESS_MAX)))
	{
		DEBUG_TRCOUT();
		return;
	}
	else if ((type != EWDT_TERM_TYPE_REBOOT)
	 && (type != EWDT_TERM_TYPE_SHUTDOWN))
	{
		DEBUG_TRCOUT();
		return;
	}
	/* 現在の終了状態と違う場合は何もしない */
	else if (g_EWDT_TermType != type)
	{
		DEBUG_TRCOUT();
		return;
	}
	else
	{
		/* 処理無し */
	}

	/* 該当プロセスの終了フラグセット */
	g_EWDT_ProcTermResFlg[sport - LFW_SPORT_MIN] = EWDT_FLG_ON;
	
	/* 全プロセス終了チェック */
	idx = 0;
	for(idx = 0; (isAllProcRes) && (idx < EWDT_PROC_TBL_NUM); idx++)
	{
		if (g_EWDT_ProcTermResFlg[g_EWDT_ProcTbl[idx].Sport - LFW_SPORT_MIN] == EWDT_FLG_OFF)
		{
			isAllProcRes = 0;
		}
		else
		{
			/* 処理無し */
		}
	}
	
	/* 全プロセス終了なら終了フラグON */
	if (isAllProcRes)
	{
		elog_log(ELOG_LV_DEBUG, "All Proc Term OK\n");
		/* mutex取得 */
		cmn_mutex_lock(&g_EWDT_Mtx);
		/* 全プロセス終了フラグセット */
		g_EWDT_ProcTermFinishFlg = EWDT_FLG_ON;
		/* mutex解放 */
		cmn_mutex_unlock(&g_EWDT_Mtx);
	}
	else
	{
		/* 処理無し */
	}
	DEBUG_TRCOUT();
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*End Of File*/
