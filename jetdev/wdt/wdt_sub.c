/****************************************************************************/
/*	Description		:WATCHDOG監視プロセス内部関数ソースコード					*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Chen.H											*/
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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/watchdog.h>
#include <pthread.h>
#include <signal.h>
#include "typedef.h"
#include "task_g.h"
#include "wdt_g.h"
#include "lfw_g.h"
#include "wdt_cmn_g.h"
#include "wdt_fnc_g.h"
#include "wdt_val_g.h"
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
/*	Description		:WATCHDOG監視プロセスサブレッド処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:void*	prm	スレッドパラメータ(未使用)					*/
/*--------------------------------------------------------------------------*/
/*	return			:void*	NULL											*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
void* wdt_SubWdtTh(void* prm)
{
#ifdef WDT_HW_WDT_ENABLE
	ulong keepAliveCycCnt = 0;
#endif
#ifdef WDT_TERM_ENABLE
	ulong termChkCycCnt = 0;
#endif
#ifdef WDT_PROC_WDT_ENABLE
	ulong procChkCycCnt = 0;
#endif
	ulong memChkCycCnt = 0;
	DEBUG_TRCIN();
	
	/* メモリクリアチェック用変数を初期化 */
	g_wdtMemChkFlg = TRUE;
	g_wdtMemChkTimErrCnt = 0;
	
	/* ループ */
	while(1) {
#ifdef WDT_HW_WDT_ENABLE
		/* WATCHDOGタイマクリア */
		if (keepAliveCycCnt > 0)
		{
			keepAliveCycCnt--;
		}
		else
		{
			/* 処理無し */
		}
		if (keepAliveCycCnt == 0)
		{
			ioctl(g_wdtFd, WDIOC_KEEPALIVE, NULL);
			keepAliveCycCnt = WDT_KEEPALIVE_CYC_CNT;
		}
		else
		{
			/* 処理無し */
		}
#endif
		/* メモリキャッシュクリア監視 */
		if (memChkCycCnt > 0)
		{
			memChkCycCnt--;
		}
		else
		{
			/* 処理無し */
		}
		if (memChkCycCnt == 0)
		{
			wdt_SubChkMemClear();
			memChkCycCnt = WDT_MEM_CHK_CYC_CNT;
		}
		else
		{
			/* 処理無し */
		}
#ifdef WDT_TERM_ENABLE

		/* POWER SW監視はekeyプロセスに移行 */
		/* 終了監視処理 */
		if (termChkCycCnt > 0)
		{
			termChkCycCnt--;
		}
		else
		{
			/* 処理無し */
		}
		if (termChkCycCnt == 0)
		{
			wdt_SubChkTermTimeout();
			termChkCycCnt = WDT_TERM_CHK_CYC_CNT;
		}
		else
		{
			/* 処理無し */
		}
#endif
		
#ifdef WDT_PROC_WDT_ENABLE
		/* 常駐プロセス生存確認処理 */
		if (procChkCycCnt > 0)
		{
			procChkCycCnt--;
		}
		else
		{
			/* 処理無し */
		}
		if (procChkCycCnt == 0)
		{
			wdt_SubChkProcAlive();
			procChkCycCnt = WDT_PROC_CHK_CYC_CNT;
		}
		else
		{
			/* 処理無し */
		}
#endif
		
		/* スリープ */
		usleep(WDT_CYC_TIME);
	}
	DEBUG_TRCOUT();
	return NULL;
}

/*==========================================================================*/
/*	Description		:常駐プロセス生存確認									*/
/*--------------------------------------------------------------------------*/
/*	param			:void													*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
void wdt_SubChkProcAlive(void)
{
	slong ret = 0;
	slong idx = 0;
	slong aliveIdx = 0;
	schar cmd[WDT_CMD_BUF_LEN];
	WdtEvtWdtReq_t msg;
	DEBUG_TRCIN();
	/* メッセージパラメータセット */
	memset(&msg, 0x00, sizeof(msg));
	msg.header.msgId = WDT_EVT_WDT_REQ;
	
	/* mutex取得 */
	pthread_mutex_lock(&g_wdtMtx);
	
	/* 終了処理中で終了タイムアウトの半分経過している場合は監視しない */
	if ((g_wdtTermType != WDT_TERM_TYPE_NONE)
	 && (g_wdtTermTimeoutCnt < WDT_PROC_TIMEOUT_TERM))
	{
		pthread_mutex_unlock(&g_wdtMtx);
		DEBUG_TRCOUT();
		return;
	}
	else
	{
		/* プロセス生存確認 */
		for(idx = 0; idx < WDT_PROC_TBL_NUM; idx++)
		{
			aliveIdx = g_wdtProcTbl[idx].sport - LFW_SPORT_MIN;
			if (aliveIdx >= 0)
			{
				/* 生存確認カウンタダウンカウント */
				if (g_wdtProcWdtResFlg[aliveIdx] == WDT_FLG_ON)
				{
					/* 応答受信済みなら次の要求送信フラグセット */
					g_wdtProcWdtSndFlg[aliveIdx] = WDT_FLG_ON;
				}
				else
				{
					/* 応答未受信ならダウンカウント */
					if (g_wdtProcWdtCnt[aliveIdx] > 0)
					{
						g_wdtProcWdtCnt[aliveIdx]--;
					}
					else
					{
						/* 処理無し */
					}
				}
				/* タイムアウトの場合はプロセス再起動 */
				if (g_wdtProcWdtCnt[aliveIdx] == 0)
				{
					/* 該当プロセスをKILL */
					memset(cmd, 0x00, WDT_CMD_BUF_LEN);
					snprintf(cmd, sizeof(cmd), WDT_PROC_KILL_CMD,
						g_wdtProcTbl[idx].name);
					system(cmd);
					/* 起動 */
					memset(cmd, 0x00, WDT_CMD_BUF_LEN);
					if(g_wdtProcTbl[idx].sport == AGUI_SPORT)
					{
						snprintf(cmd, sizeof(cmd), WDT_PROC_EXE_CMD,
							g_wdtProcTbl[idx].name);
					}
					else
					{
						snprintf(cmd, sizeof(cmd), WDT_PROC_EXE_CMD,
							g_wdtProcTbl[idx].name);
					}
					system(cmd);
					/* 再起動ログ出力 */
					DLOG_Info("Restart %s", g_wdtProcTbl[idx].name);
					/* 生存確認カウンタリセット */
					g_wdtProcWdtCnt[aliveIdx] = WDT_PROC_TIMEOUT_DEF;
					g_wdtProcWdtSndFlg[aliveIdx] = WDT_FLG_ON;
				}
				else
				{
					/* 処理無し */
				}
				/* 応答要求送信 */
				if (g_wdtProcWdtSndFlg[aliveIdx] == WDT_FLG_ON)
				{
					DLOG_Debug("send wdt req to [%s]", g_wdtProcTbl[idx].name);
					ret = LFW_SendEvent(g_wdtProcTbl[idx].sport, 
											sizeof(msg), &msg);
					if (LFW_RET_OK == ret)
					{
						/* 送信出来たら送信フラグクリア */
						g_wdtProcWdtSndFlg[aliveIdx] = WDT_FLG_OFF;
					}
					else
					{
						/* 処理無し */
					}
					/* 応答受信フラグクリア */
					g_wdtProcWdtResFlg[aliveIdx] = WDT_FLG_OFF;
				}
				else
				{
					/* 処理無し */
				}
			}
			else
			{
				/* 処理無し */
			}
		}
	}
	
	/* mutex解放 */
	pthread_mutex_unlock(&g_wdtMtx);
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:電源SW監視処理											*/
/*--------------------------------------------------------------------------*/
/*	param			:void													*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
void wdt_SubChkTermTimeout(void)
{
	slong ret = 0;
	slong sysTimeout = 1;
	DEBUG_TRCIN();
	/* mutex 取得 */
	pthread_mutex_lock(&g_wdtMtx);
	
	/* 終了処理中なら処理 */
	if (g_wdtTermType != WDT_TERM_TYPE_NONE)
	{
		/* 終了監視タイムアウトカウンタダウンカウント */
		if (g_wdtTermTimeoutCnt > 0)
		{
			g_wdtTermTimeoutCnt--;
		}
		else
		{
			/* 処理無し */
		}
		/* タイムアウト or 全プロセス終了ならリブート/電源OFF */
		if ((g_wdtTermTimeoutCnt == 0)
		 || (g_wdtProcTermFinishFlg == WDT_FLG_ON))
		{
		 	/* リブート/電源OFF前にファイルをSyncさせる */
		 	sync();
			if (g_wdtTermType == WDT_TERM_TYPE_REBOOT)
			{
				DLOG_Info("System Reboot Now!");
				/* リブート */
				system("/sbin/reboot");
			}
			else
			{
				/* 電源OFF */
				DLOG_Info("System Shutdown Now!");
				system("/sbin/shutdown -h now");
			}
#ifdef WDT_HW_WDT_ENABLE
			/* WATCHDOGタイマタイムアウトを1秒に設定(終了処理中に電源SW ONされた場合の対策) */
			sysTimeout = 1;
			ioctl(g_wdtFd, WDIOC_SETTIMEOUT, &sysTimeout);
#endif
		}
		else
		{
			/* 処理無し */
		}
	}
	
	/* mutex解放 */
	pthread_mutex_unlock(&g_wdtMtx);
	
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:メモリクリア監視処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:void													*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
void wdt_SubChkMemClear(void)
{
	WdtTm_t tm;
	slong timRet = WDT_OK;
	bool_t memClrExec = FALSE;	/* クリア未実行で初期化 */
	slong sysRet = 0;
	DEBUG_TRCIN();
	/* 現在時刻取得 */
	memset(&tm, 0x00, sizeof(tm));
	timRet = wdt_GetTime(&tm);
	if (WDT_OK == timRet)
	{
		if (WDT_MEM_CLR_HOUR == tm.tmHour)
		{
			/* クリア実行時刻帯の場合 */
			if (TRUE == g_wdtMemChkFlg){
				/* クリアチェック可状態の場合 */
				memClrExec = TRUE;	// クリア実行
			}
		}
		else
		{
			/* クリア時刻帯でない場合 */
			g_wdtMemChkFlg = TRUE;	// クリアチェック可(次のクリア実行時刻帯でチェック可にする)
		}
		g_wdtMemChkTimErrCnt = 0;	// 時刻取得失敗カウンタクリア
	}
	else
	{
		/* 現在時刻取得に失敗した場合 */
		g_wdtMemChkTimErrCnt++;	// 時刻取得失敗カウンタインクリメント
		if (WDT_MEM_TIM_ERR_CNT <= g_wdtMemChkTimErrCnt)
		{
			/* 一定回数、時刻取得に失敗し続けた場合はキャッシュクリア実行する */
			memClrExec = TRUE;		// クリア実行
			g_wdtMemChkTimErrCnt = 0;	// 時刻取得連続失敗カウンタクリア
		}
		else
		{
			/* 処理無し */
		}
		g_wdtMemChkFlg = TRUE;	// クリアチェック可(次のクリア実行時刻帯でチェック可にする)
	}
	/* クリア実行判定ならメモリキャッシュクリア */
	if (TRUE == memClrExec)
	{
		sysRet = system("echo 3 > /proc/sys/vm/drop_caches");
		if (0 != sysRet)
		{
			/* クリア失敗の場合 */
			DLOG_Error("drop caches Fail!!");	// ログ出力
		}
		else
		{
			/* クリア成功の場合 */
			g_wdtMemChkFlg = FALSE;	// クリアチェック不可にする
										// (非クリア実行時間帯になる or 時刻取得不可で再度可にする)
		}
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
