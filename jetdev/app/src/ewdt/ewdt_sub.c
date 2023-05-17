/****************************************************************************/
/*	Description		:WATCHDOG監視プロセス内部関数ソースコード					*/
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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/watchdog.h>
#include <pthread.h>
#include <signal.h>
#include <type.h>
#include <task_g.h>
#include <elog.h>
#include <common.h>
#include <etim.h>
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
/*	Description		:WATCHDOG監視プロセスサブレッド処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:void*	prm	スレッドパラメータ(未使用)					*/
/*--------------------------------------------------------------------------*/
/*	return			:void*	NULL											*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
void* ewdt_SubWdtTh(void* prm)
{
#ifdef EWDT_HW_WDT_ENABLE
	ulong keepAliveCycCnt = 0;
#endif
#ifdef EWDT_TERM_ENABLE
	ulong termChkCycCnt = 0;
#endif
#ifdef EWDT_PROC_WDT_ENABLE
	ulong procChkCycCnt = 0;
#endif
	ulong memChkCycCnt = 0;
	DEBUG_TRCIN();
	/* メモリクリアチェック用変数を初期化 */
	g_EWDT_MemChkFlg = TRUE;
	g_EWDT_MemChkTimErrCnt = 0;
	
	/* ループ */
	while(1) {
#ifdef EWDT_HW_WDT_ENABLE
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
			ioctl(g_EWDT_WdtFd, WDIOC_KEEPALIVE, NULL);
			keepAliveCycCnt = EWDT_KEEPALIVE_CYC_CNT;
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
			ewdt_SubChkMemClear();
			memChkCycCnt = EWDT_MEM_CHK_CYC_CNT;
		}
		else
		{
			/* 処理無し */
		}
#ifdef EWDT_TERM_ENABLE

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
			ewdt_SubChkTermTimeout();
			termChkCycCnt = EWDT_TERM_CHK_CYC_CNT;
		}
		else
		{
			/* 処理無し */
		}
#endif
		
#ifdef EWDT_PROC_WDT_ENABLE
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
			ewdt_SubChkProcAlive();
			procChkCycCnt = EWDT_PROC_CHK_CYC_CNT;
		}
		else
		{
			/* 処理無し */
		}
#endif
		
		/* スリープ */
		usleep(EWDT_CYC_TIME);
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
void ewdt_SubChkProcAlive(void)
{
	slong ret = 0;
	slong idx = 0;
	slong aliveIdx = 0;
	schar cmd[EWDT_CMD_BUF_LEN];
	EwdtEvtWdtReq_t msg;
	DEBUG_TRCIN();
	/* メッセージパラメータセット */
	memset(&msg, 0x00, sizeof(msg));
	msg.Header.MsgId = EWDT_EVT_WDT_REQ;
	
	/* mutex取得 */
	cmn_mutex_lock(&g_EWDT_Mtx);
	
	/* 終了処理中で終了タイムアウトの半分経過している場合は監視しない */
	if ((g_EWDT_TermType != EWDT_TERM_TYPE_NONE)
	 && (g_EWDT_TermTimeoutCnt < EWDT_PROC_TIMEOUT_TERM))
	{
		cmn_mutex_unlock(&g_EWDT_Mtx);
		DEBUG_TRCOUT();
		return;
	}
	else
	{
		/* プロセス生存確認 */
		for(idx = 0; idx < EWDT_PROC_TBL_NUM; idx++)
		{
			aliveIdx = g_EWDT_ProcTbl[idx].Sport - LFW_SPORT_MIN;
			if (aliveIdx >= 0)
			{
				/* 生存確認カウンタダウンカウント */
				if (g_EWDT_ProcWdtResFlg[aliveIdx] == EWDT_FLG_ON)
				{
					/* 応答受信済みなら次の要求送信フラグセット */
					g_EWDT_ProcWdtSndFlg[aliveIdx] = EWDT_FLG_ON;
				}
				else
				{
					/* 応答未受信ならダウンカウント */
					if (g_EWDT_ProcWdtCnt[aliveIdx] > 0)
					{
						g_EWDT_ProcWdtCnt[aliveIdx]--;
					}
					else
					{
						/* 処理無し */
					}
				}
				/* タイムアウトの場合はプロセス再起動 */
				if (g_EWDT_ProcWdtCnt[aliveIdx] == 0)
				{
					/* 該当プロセスをKILL */
					/* TODO */
					/* 起動 */
					/* TODO */
					/* 再起動ログ出力 */
					elog_log(ELOG_LV_ERROR, "<i>%s Reboot\n", g_EWDT_ProcTbl[idx].Name);
					/* 生存確認カウンタリセット */
					g_EWDT_ProcWdtCnt[aliveIdx] = EWDT_PROC_TIMEOUT_DEF;
					g_EWDT_ProcWdtSndFlg[aliveIdx] = EWDT_FLG_ON;
				}
				else
				{
					/* 処理無し */
				}
				/* 応答要求送信 */
				if (g_EWDT_ProcWdtSndFlg[aliveIdx] == EWDT_FLG_ON)
				{
					ret = FRM_MsgSendEvent(g_EWDT_ProcTbl[idx].Sport, 
											sizeof(msg), &msg);
					if (FRM_SUCCESS == ret)
					{
						/* 送信出来たら送信フラグクリア */
						g_EWDT_ProcWdtSndFlg[aliveIdx] = EWDT_FLG_OFF;
					}
					else
					{
						/* 処理無し */
					}
					/* 応答受信フラグクリア */
					g_EWDT_ProcWdtResFlg[aliveIdx] = EWDT_FLG_OFF;
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
	cmn_mutex_unlock(&g_EWDT_Mtx);
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
void ewdt_SubChkTermTimeout(void)
{
	slong ret = 0;
	slong sysTimeout = 1;
	DEBUG_TRCIN();
	/* mutex 取得 */
	cmn_mutex_lock(&g_EWDT_Mtx);
	
	/* 終了処理中なら処理 */
	if (g_EWDT_TermType != EWDT_TERM_TYPE_NONE)
	{
		/* 終了監視タイムアウトカウンタダウンカウント */
		if (g_EWDT_TermTimeoutCnt > 0)
		{
			g_EWDT_TermTimeoutCnt--;
		}
		else
		{
			/* 処理無し */
		}
		/* タイムアウト or 全プロセス終了ならリブート/電源OFF */
		if ((g_EWDT_TermTimeoutCnt == 0)
		 || (g_EWDT_ProcTermFinishFlg == EWDT_FLG_ON))
		{
		 	/* リブート/電源OFF前にファイルをSyncさせる */
		 	sync();
			if (g_EWDT_TermType == EWDT_TERM_TYPE_REBOOT)
			{
				elog_log(ELOG_LV_NORMAL, "/sbin/reboot\n");
				/* リブート */
				system("/sbin/reboot");
			}
			else
			{
				/* 電源OFF */
				elog_log(ELOG_LV_NORMAL, "/sbin/shutdown\n");
				system("/sbin/shutdown -h now");
			}
#ifdef EWDT_HW_WDT_ENABLE
			/* WATCHDOGタイマタイムアウトを1秒に設定(終了処理中に電源SW ONされた場合の対策) */
			sysTimeout = 1;
			ioctl(g_EWDT_WdtFd, WDIOC_SETTIMEOUT, &sysTimeout);
#endif
		}
		else
		{
			/* 処理無し */
		}
	}
	
	/* mutex解放 */
	cmn_mutex_unlock(&g_EWDT_Mtx);
	
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
void ewdt_SubChkMemClear(void)
{
	ETIM_TM_T tm_t;
	slong timRet = ETIM_OK;
	bool_t memClrExec = FALSE;	/* クリア未実行で初期化 */
	slong sysRet = 0;
	DEBUG_TRCIN();
	/* 現在時刻取得 */
	memset(&tm_t, 0x00, sizeof(tm_t));
	timRet = ETIM_GetTime(&tm_t);
	if (ETIM_OK == timRet)
	{
		if (EWDT_MEM_CLR_HOUR == tm_t.tm_hour)
		{
			/* クリア実行時刻帯の場合 */
			if (TRUE == g_EWDT_MemChkFlg){
				/* クリアチェック可状態の場合 */
				memClrExec = TRUE;	// クリア実行
			}
		}
		else
		{
			/* クリア時刻帯でない場合 */
			g_EWDT_MemChkFlg = TRUE;	// クリアチェック可(次のクリア実行時刻帯でチェック可にする)
		}
		g_EWDT_MemChkTimErrCnt = 0;	// 時刻取得失敗カウンタクリア
	}
	else
	{
		/* 現在時刻取得に失敗した場合 */
		g_EWDT_MemChkTimErrCnt++;	// 時刻取得失敗カウンタインクリメント
		if (EWDT_MEM_TIM_ERR_CNT <= g_EWDT_MemChkTimErrCnt)
		{
			/* 一定回数、時刻取得に失敗し続けた場合はキャッシュクリア実行する */
			memClrExec = TRUE;		// クリア実行
			g_EWDT_MemChkTimErrCnt = 0;	// 時刻取得連続失敗カウンタクリア
		}
		else
		{
			/* 処理無し */
		}
		g_EWDT_MemChkFlg = TRUE;	// クリアチェック可(次のクリア実行時刻帯でチェック可にする)
	}
	/* クリア実行判定ならメモリキャッシュクリア */
	if (TRUE == memClrExec)
	{
		sysRet = system("echo 3 > /proc/sys/vm/drop_caches");
		if (0 != sysRet)
		{
			/* クリア失敗の場合 */
			elog_log(ELOG_LV_ERROR, "drop caches Fail!!\n");	// ログ出力
		}
		else
		{
			/* クリア成功の場合 */
			g_EWDT_MemChkFlg = FALSE;	// クリアチェック不可にする
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
