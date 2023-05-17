/****************************************************************************/
/*	Description		:WATCHDOG監視プロセス									*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Chen.H											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/28												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:WDT													*/
/*--------------------------------------------------------------------------*/
/*	Note			:WATCHDOG監視プロセスソースコード						*/
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
#include <errno.h>
#include <signal.h>
#include "typedef.h"
#include "task_g.h"
#include "lfw_g.h"
#include "dlog_g.h"
#include "wdt_g.h"
#include "wdt_cmn_g.h"
#include "wdt_sub_g.h"
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
/*	Description		:WATCHDOG監視プロセスメインスレッド						*/
/*--------------------------------------------------------------------------*/
/*	param			:sint	argc	コマンドライン引数						*/
/*--------------------------------------------------------------------------*/
/*	param			:schar*	argv[]	コマンドライン引数						*/
/*--------------------------------------------------------------------------*/
/*	return			:0	正常終了	-1	異常終了							*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
sint main(sint argc, schar* argv[])
{
	sint ret = 0;
	sint sysTimout = WDT_SYS_TIMEOUT;
	pthread_t tid;
	uchar idx = 0;
	WdtFncMsg_t msg;

	wdt_DebugLogProc();
	DEBUG_TRCIN();
	/* メッセージPORTセット */
	LFW_SetReceiver(WDT_SPORT);

#ifdef WDT_HW_WDT_ENABLE
	/* WATCHDOGタイマタイムアウト設定 */
	g_wdtFd = (sint)(-1);
	g_wdtFd = open(WDT_WDT_DEV_PATH, O_RDWR, 0);
	if(g_wdtFd < 0)
	{
		DLOG_Error("Open Wathdog Dev Fail");
		DEBUG_TRCIN();
		return (sint)(-1);
	}
	else
	{
		/* 再起動したプロセスにディスクリプタが引き
		   継がれないようにclose-on-exec設定 */
		ret = fcntl(g_wdtFd, F_SETFD, FD_CLOEXEC);
		/* タイムアウト時間設定 */
		ret = ioctl(g_wdtFd, WDIOC_SETTIMEOUT, &sysTimout);
		if(ret < 0){
			close(g_wdtFd);
			DLOG_Error("Set Wathdog Dev Timeout Fail");
			DEBUG_TRCIN();
			return (sint)(-1);
		}
		else
		{
			DLOG_Info("Set Wathdog Dev Success");
		}
	}
#endif
	
	/* スレッド排他用ミューテックス初期化 */
	ret = pthread_mutex_init(&g_wdtMtx, NULL);
	if(ret < 0){
		close(g_wdtFd);
		DLOG_Error("Init Mutex Fail");
		DEBUG_TRCIN();
		return (sint)(-1);
	}
	else
	{
		/* 処理無し */
	}

	/* 終了状態初期化 */
	g_wdtTermType = WDT_TERM_TYPE_NONE;
	g_wdtProcTermFinishFlg = WDT_FLG_OFF;
	g_wdtPwSwOffDetectCnt = 0;
	
	/* プロセス監視情報初期化 */
	for (idx=0; idx<PROCESS_MAX; idx++)
	{
		g_wdtProcWdtCnt[idx] = WDT_PROC_TIMEOUT_DEF;
		g_wdtProcWdtResFlg[idx] = WDT_FLG_ON;
		g_wdtProcWdtSndFlg[idx] = WDT_FLG_OFF;
		g_wdtProcTermResFlg[idx] = WDT_FLG_OFF;
	}
	
	/* WATCHDOG監視用サブスレッド生成 */
	ret = pthread_create(&tid, NULL, wdt_SubWdtTh, NULL);
	if (ret < 0){
		close(g_wdtFd);
		DLOG_Error("Create Sub Thread Fail");
		DEBUG_TRCIN();
		return (sint)(-1);
	}
	else
	{
		/* 処理無し */
	}

	memset(&msg, 0x00, sizeof(msg));
	/* メインループ */
	while(1)
	{
		/* メッセージ待ち */
		if(LFW_WaitMessage(WDT_SPORT, 0, (void*)&msg) != (-1))
		{
			switch(msg.header.attr)
			{
				/* 非同期メッセージ */
				case LFW_MSG_ASYNC:
					switch(msg.header.msgId){
						/* 常駐プロセスWDT応答 */
						case WDT_MSGID_WDT_RES:
							wdt_FncWdtRes(msg.fncWdtRes.header.srcModId, 
											msg.fncWdtRes.timeout);
							break;
						/* 終了要求 */
						case WDT_MSGID_TERM_REQ:
							wdt_FncTermReq(msg.fncTermReq.type,
											 msg.fncTermReq.header.srcModId);
							break;
						/* 常駐プロセス終了応答 */
						case WDT_MSGID_TERM_RES:
							wdt_FncTermRes(msg.fncTermRes.header.srcModId,
											msg.fncTermRes.type);
							break;
						default:
							DLOG_Error("Unknown AsyncMsg Rcv = %ld", 
										msg.header.msgId);
							break;
					}
					break;
				default:
					DLOG_Error("Unknown Msg Rcv = %ld", msg.header.attr);
					break;
			}
		}
		else
		{
			DLOG_Error("Wait Msg Fail");
		}
	}
	DEBUG_TRCIN();
	return (sint)(0);
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*End Of File*/
