/****************************************************************************/
/*	Description		:WATCHDOG監視プロセスソースコード						*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Chen.H												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/28												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:WDT													*/
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
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "dlog_g.h"
#include "task_g.h"
#include "typedef.h"
#include "wdt_g.h"
#include "wdt_cmn_g.h"
#include "wdt_val_g.h"
#include "wdt_fnc_g.h"
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
/*	param			:ulong sport	該当プロセスのメッセージポート			*/
/*--------------------------------------------------------------------------*/
/*	param			:uchar timeout	タイムアウト値							*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
void wdt_FncWdtRes(ulong sport, uchar timeout)
{
	DEBUG_TRCIN();
	DLOG_Info("wdtResRcv(sport:%lu)(timeout:%d)", sport, timeout);

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
		pthread_mutex_lock(&g_wdtMtx);
		
		/* 生存カウンタリセット */
		g_wdtProcWdtCnt[sport - LFW_SPORT_MIN] = timeout;
		g_wdtProcWdtResFlg[sport - LFW_SPORT_MIN] = WDT_FLG_ON;
		
		/* mutex解放 */
		pthread_mutex_unlock(&g_wdtMtx);
	}

	DEBUG_TRCOUT();
	return;
}

/*==========================================================================*/
/*	Description		:終了要求ファンクション処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:WdtTermType_t	type	終了種別						*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong	reqMsgid要求元プロセスのメッセージキューID（SPORT）*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
void wdt_FncTermReq(WdtTermType_t type, ulong reqMsgid)
{
	slong ret = 0;
	slong idx = 0;
	slong procIdx = 0;
	WdtEvtTermReq_t msg;
	DEBUG_TRCIN();
	DLOG_Debug("termReqRcv(type:%d)", type);
	
	/* パラメータチェック(第1引数) */
	if ((type != WDT_TERM_TYPE_REBOOT)
	 && (type != WDT_TERM_TYPE_SHUTDOWN))
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
	else if (g_wdtTermType == WDT_TERM_TYPE_SHUTDOWN)
	{
		DEBUG_TRCOUT();
		return;
	}
	/* 現在の終了タイプと同じ場合は要求受けない */
	else if (g_wdtTermType == type)
	{
		DEBUG_TRCOUT();
		return;
	}
	else
	{
		/* 処理無し */
	}

	/* mutex取得 */
	pthread_mutex_lock(&g_wdtMtx);
	
	/* 終了タイムアウト設定 */
	g_wdtTermTimeoutCnt = WDT_TERM_TIMEOUT;
	
	/* 全プロセス終了フラグクリア */
	g_wdtProcTermFinishFlg = WDT_FLG_OFF;
	
	/* 終了タイプ保持 */
	g_wdtTermType = type;
	
	/* 終了ログ出力 */
	DLOG_Info("%s",
			(g_wdtTermType == WDT_TERM_TYPE_REBOOT ? "Reboot" : "Shutdown"));
	
	/* 各プロセスに終了要求メッセージ送信 */
	memset(&msg, 0x00, sizeof(msg));
	msg.header.msgId = WDT_EVT_TERM_REQ;
	msg.type = g_wdtTermType;
	msg.sport = reqMsgid;
	
	idx = WDT_PROC_TBL_NUM - 1;
	for(idx = (WDT_PROC_TBL_NUM - 1); idx >= 0; idx--)
	{
		procIdx = g_wdtProcTbl[idx].sport - LFW_SPORT_MIN;
		if (procIdx >= 0)
		{
			/* 終了確認フラグ初期化 */
			g_wdtProcTermResFlg[procIdx] = WDT_FLG_OFF;
			/* 常駐プロセスタイムアウトカウンタリセット */
			g_wdtProcWdtCnt[procIdx] = WDT_PROC_TIMEOUT_TERM;
			DLOG_Info("WDT send term req to [%s]", g_wdtProcTbl[idx].name);
			/* メッセージ送信 */
			ret = LFW_SendEvent(g_wdtProcTbl[idx].sport,
								sizeof(msg), &msg);
		}
		else
		{
			/* 処理無し */
		}
	}
	
	/* mutex解放 */
	pthread_mutex_unlock(&g_wdtMtx);
	DEBUG_TRCOUT();
	return;
}

/*==========================================================================*/
/*	Description		:終了応答ファンクション処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong	sport	該当プロセスのメッセージポート			*/
/*--------------------------------------------------------------------------*/
/*	param			:WdtTermType_t	type	終了種別						*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
void wdt_FncTermRes(ulong sport, WdtTermType_t type)
{
	slong idx = 0;
	bool_t isAllProcRes = 1;
	DEBUG_TRCIN();
	DLOG_Debug("termResRcv(sport:%lu)(type:%d)", sport, type);
	
	/* パラメータチェック */
	if ((sport < LFW_SPORT_MIN)
	 || (sport >= (LFW_SPORT_MIN + PROCESS_MAX)))
	{
		DEBUG_TRCOUT();
		return;
	}
	else if ((type != WDT_TERM_TYPE_REBOOT)
	 && (type != WDT_TERM_TYPE_SHUTDOWN))
	{
		DEBUG_TRCOUT();
		return;
	}
	/* 現在の終了状態と違う場合は何もしない */
	else if (g_wdtTermType != type)
	{
		DEBUG_TRCOUT();
		return;
	}
	else
	{
		/* 処理無し */
	}

	/* 該当プロセスの終了フラグセット */
	g_wdtProcTermResFlg[sport - LFW_SPORT_MIN] = WDT_FLG_ON;
	
	/* 全プロセス終了チェック */
	idx = 0;
	for(idx = 0; (isAllProcRes) && (idx < WDT_PROC_TBL_NUM); idx++)
	{
		if (g_wdtProcTermResFlg[g_wdtProcTbl[idx].sport - LFW_SPORT_MIN]
			== WDT_FLG_OFF)
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
		DLOG_Debug("All Proc Term OK");
		/* mutex取得 */
		pthread_mutex_lock(&g_wdtMtx);
		/* 全プロセス終了フラグセット */
		g_wdtProcTermFinishFlg = WDT_FLG_ON;
		/* mutex解放 */
		pthread_mutex_unlock(&g_wdtMtx);
	}
	else
	{
		/* 処理無し */
	}
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:現在時刻取得											*/
/*--------------------------------------------------------------------------*/
/*	param			:WdtTm_t *pTime:現在時刻								*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
slong wdt_GetTime(WdtTm_t *pTime)
{
	struct timespec tspec;
	struct tm ltm;
	slong ret;
	struct tm *ptm;

	DEBUG_TRCIN();

	/* 引数確認 */
	if (NULL == pTime)
	{
		DLOG_Error("parameter NULL.");
		DEBUG_TRCOUT();
		return WDT_ERROR;
	}

	/* システム時刻取得 */
	ret = clock_gettime(CLOCK_REALTIME, &tspec);
	if (0 == ret)
	{
		/* 年月日時分秒へ変換 */
		ptm = localtime_r(&tspec.tv_sec, &ltm);
		if (NULL != ptm)
		{
			/* 返却データへ設定 */
			pTime->tmYear = ltm.tm_year + 1900;
			pTime->tmMon = ltm.tm_mon + 1;
			pTime->tmMday = ltm.tm_mday;
			pTime->tmMin = ltm.tm_min;
			pTime->tmHour = ltm.tm_hour;
			pTime->tmSec = ltm.tm_sec;

			ret = WDT_OK;
		}
		else
		{
			DLOG_Error("localtime_r() error.");
			ret = WDT_ERROR;
		}
	}
	else
	{
		DLOG_Error("clock_gettime() error.");
		ret = WDT_ERROR;
	}

	/* リターンを返す */
	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:デバッグログ出力管理									*/
/*--------------------------------------------------------------------------*/
/*	param			:void													*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:デバッグログ出力管理									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
void wdt_DebugLogProc(void)
{
	sint ret, fd, rdSz;
	struct stat swFileSt;
	schar swVal;
	uchar logOnFlag = 0;

	/* ファイル存在チェック */
	ret = stat(WDT_DEBUG_LOG_SWITCH_FILE, &swFileSt);
	if ((0 == ret) && ((slong)1 == swFileSt.st_size))
	{
		/* ファイル読み出し */
		fd = open(WDT_DEBUG_LOG_SWITCH_FILE, O_RDONLY);
		if (0 <= fd)
		{
			rdSz = read(fd, (uchar*)&swVal, sizeof(swVal));
			if ((rdSz == sizeof(swVal)) && (WDT_DEBUG_LOG_ON == swVal))
			{
				logOnFlag = 1;
			}
			else
			{
				/* 処理無し */
			}
			close(fd);
		}
	}
	else
	{
		/* 処理無し */
	}

	if (logOnFlag)
	{
		DLOG_Init(WDT_DEBUG_LOG_DIR, 
				WDT_DEBUG_LOG_PREFIX,
				WDT_DEBUG_LOG_MAX_SZ, 
				WDT_DEBUG_LOG_MAX_NUM, 
				DLOG_OUT_MODE_FILE,
				WDT_DEBUG_LOG_CRT_INTERVAL_S);
	}
	else
	{
		/* 処理無し */
	}
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*End Of File*/
