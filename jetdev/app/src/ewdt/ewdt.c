/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Chen.H											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/28												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:EWDT													*/
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
#include <type.h>
#include <task_g.h>
#include <lfw_g.h>
#include <elog.h>
#include "esys.h"
#include "ewdt.h"
#include "ewdt_local.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
static FrmPthread_t s_Tid;

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
void EWDT_Init(void)
{
	slong ret = 0;
	uchar idx = 0;
	slong sysTimeout = EWDT_SYS_TIMEOUT;
	DEBUG_TRCIN();
#ifdef EWDT_HW_WDT_ENABLE
	/* WATCHDOGタイマタイムアウト設定 */
	g_EWDT_WdtFd = (slong)(-1);
	g_EWDT_WdtFd = CMN_OPEN(EWDT_WDT_DEV_PATH, O_RDWR, 0);
	if(g_EWDT_WdtFd < 0)
	{
		elog_log(ELOG_LV_ERROR, "Open Wathdog Dev Fail\n");
		DEBUG_TRCOUT();
		return;
	}
	else
	{
		/* 再起動したプロセスにディスクリプタが引き継がれないようにclose-on-exec設定 */ 
		ret = fcntl(g_EWDT_WdtFd, F_SETFD, FD_CLOEXEC);
		/* タイムアウト時間設定 */
		ret = ioctl(g_EWDT_WdtFd, WDIOC_SETTIMEOUT, &sysTimeout);
		if(ret < 0){
			CMN_CLOSE(g_EWDT_WdtFd);
			elog_log(ELOG_LV_ERROR, "Set Wathdog Dev Timeout Fail\n");
			DEBUG_TRCOUT();
			return;
		}
		else
		{
			/* 処理無し */
		}
	}
#endif
	
	/* スレッド排他用ミューテックス初期化 */
	ret = cmn_mutex_init(&g_EWDT_Mtx);
	if(ret < 0){
		CMN_CLOSE(g_EWDT_WdtFd);
		elog_log(ELOG_LV_ERROR, "Init Mutex Fail\n");
		DEBUG_TRCOUT();
		return;
	}
	else
	{
		/* 処理無し */
	}
	
	/* 終了状態初期化 */
	g_EWDT_TermType = EWDT_TERM_TYPE_NONE;
	g_EWDT_ProcTermFinishFlg = EWDT_FLG_OFF;
	g_EWDT_PwSwOffDetectCnt = 0;
	
	/* プロセス監視情報初期化 */
	for(idx = 0; idx < PROCESS_MAX; idx++)
	{
		g_EWDT_ProcWdtCnt[idx] = EWDT_PROC_TIMEOUT_DEF;
		g_EWDT_ProcWdtResFlg[idx] = EWDT_FLG_ON;
		g_EWDT_ProcWdtSndFlg[idx] = EWDT_FLG_OFF;
		g_EWDT_ProcTermResFlg[idx] = EWDT_FLG_OFF;
	}
	DEBUG_TRCOUT();
}

void EWDT_Fin(void)
{
	DEBUG_TRCIN();
	DEBUG_TRCOUT();
}

void EWDT_MsgProc(FrmMsgHead_t const *msg)
{
	slong ret = 0;
	pthread_t tid;
	EwdtFncMsg_t *pmsg = (EwdtFncMsg_t *)msg;

	DEBUG_TRCIN();
	switch(pmsg->Header.Attr){
		/* 非同期メッセージ */
		case LFW_MSG_ASYNC:
			switch(pmsg->Header.MsgId) {
				/* WDT開始 */
				case EWDT_MSGID_WDT_START:
					/* WATCHDOG監視用サブスレッド生成 */
					ret = FRM_PthreadCreate(&s_Tid, NULL, ewdt_SubWdtTh, NULL);
					if (FRM_SUCCESS != ret)
					{
						CMN_CLOSE(g_EWDT_WdtFd);
						elog_log(ELOG_LV_ERROR, "Create Sub Thread Fail\n");
						ESYS_NotifyStartState(EWDT_SPORT, MOD_START_STATE_FAILED);
					}
					else
					{
						ESYS_NotifyStartState(EWDT_SPORT, MOD_START_STATE_SUCCESS);
					}
					break;
				/* 常駐プロセスWDT応答 */
				case EWDT_MSGID_WDT_RES:
					ewdt_FncWdtRes(pmsg->FncWdtRes.Header.SrcModId, 
									pmsg->FncWdtRes.Timeout);
					break;
				/* 終了要求 */
				case EWDT_MSGID_TERM_REQ:
					ewdt_FncTermReq(pmsg->FncTermReq.Type,
									pmsg->FncTermReq.Header.SrcModId);
					break;
				/* 常駐プロセス終了応答 */
				case EWDT_MSGID_TERM_RES:
					ewdt_FncTermRes(pmsg->FncTermRes.Header.SrcModId, 
										pmsg->FncTermRes.Type);
					break;
				default:
					elog_log(ELOG_LV_ERROR, "Unknown AsyncMsg Rcv = %ld\n", pmsg->Header.MsgId);
					break;
			}
			break;
		default:
			elog_log(ELOG_LV_ERROR, "Unknown Msg Rcv = %ld\n", pmsg->Header.Attr);
			break;
	}
	
	DEBUG_TRCOUT();
}

slong EWDT_Start(slong fact)
{
	EwdtFncMsg_t msg;
	DEBUG_TRCIN();

	msg.Header.MsgId = EWDT_MSGID_WDT_START;
	FRM_MsgSendAsync(EWDT_SPORT, sizeof(msg), &msg);
	DEBUG_TRCOUT();
}

slong EWDT_Stop(void)
{
	DEBUG_TRCIN();
	FRM_PthreadCancel(s_Tid);
	DEBUG_TRCOUT();
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*End Of File*/
