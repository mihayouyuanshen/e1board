/**************************************************************************
* @file		ewdt_local.h
* @brief	WATCHDOG監視プロセス内部ヘッダ
* @date		2017/07/10
* @author	(HiICS)Y.Hoshino
* Copyright (C) 2014. Nichicon Corporation. All Rights Reserved.
***************************************************************************/
#ifndef	__EWDT_LOCAL_H__
#define	__EWDT_LOCAL_H__

#include <pthread.h>
#include "ewdt.h"

/***********************************************************
* 構造体定義
************************************************************/
/***********************************************************
* @struct	EwdtProcTbl_t
* @brief	プロセス監視用テーブル構造
************************************************************/
typedef struct {
	schar Name[8];		/* プロセス名 */
	ulong Sport;		/* メッセージキューID */
} EwdtProcTbl_t;

/***********************************************************
* 定数定義
************************************************************/
#define	EWDT_WDT_DEV_PATH			"/dev/watchdog"	/* WATCHDOGタイマデバイスPATH */
#define	EWDT_SYS_TIMEOUT			(120)		/* WATCHDOGタイマタイムアウト時間(秒) */

#define	EWDT_CYC_TIME				(50*1000)	/* サブスレッド実行周期(us) */
#define	EWDT_KEEPALIVE_CYC_CNT		(ulong)((ulong)(20*1000*1000)/(ulong)EWDT_CYC_TIME)
											/* WATCHDOGタイマクリア周期(カウント値) */
#define	EWDT_PROC_CHK_CYC_CNT		(ulong)((ulong)(1*1000*1000)/(ulong)EWDT_CYC_TIME)
											/* 常駐プロセス生存確認周期(カウント値) */
#define	EWDT_TERM_CHK_CYC_CNT		(ulong)((ulong)(1*1000*1000)/(ulong)EWDT_CYC_TIME)
											/* 終了監視周期(カウント値) */
#define	EWDT_MEM_CHK_CYC_CNT		(ulong)((ulong)(60*1000*1000)/(ulong)EWDT_CYC_TIME)
											/* メモリキャッシュクリア監視周期(カウント値) */
#define	EWDT_MEM_CLR_HOUR			(3)			/* メモリキャッシュクリア実行時刻(AM3時) */
#define	EWDT_MEM_TIM_ERR_CNT		(60*24)		/* メモリキャッシュクリア時の時刻取得失敗カウント(24時間分) */

//#define	EWDT_PROC_TIMEOUT_DEF	(60)	/* 常駐プロセスタイムアウト時間初期値(カウント値) */
#define	EWDT_PROC_TIMEOUT_MIN		(2)		/* 常駐プロセスタイムアウト時間(最小値) */
#define	EWDT_PROC_TIMEOUT_MAX		(10)	/* 常駐プロセスタイムアウト時間(最大値) */

#define	EWDT_TERM_TIMEOUT			(EWDT_PROC_TIMEOUT_DEF*2)	/* 終了監視タイムアウト時間(カウント値) */
#define	EWDT_PROC_TIMEOUT_TERM		(EWDT_TERM_TIMEOUT/2)
											/* 終了処理中の常駐プロセスタイムアウト時間 */
#define	EWDT_PWSW_OFF_DETECT_CNT	(3)		/* 電源SW OFF判定一致回数 */

#define	EWDT_PROC_KILL_CMD			"/usr/bin/killall -KILL %s"	/* プロセス削除コマンド書式 */
#define	EWDT_PROC_EXE_CMD			"/usr/bin/%s 1 &"		/* プロセス起動コマンド書式(GUI以外) */
#define	EWDT_PROC_GUI_EXE_CMD		"export TSLIB_TSDEVICE=/dev/input/ts0;export QWS_MOUSE_PROTO=tslib:/dev/input/ts0;export TSLIB_CALIBFILE=/mnt/user/pointercal;/usr/bin/%s 1 -qws &"
														/* プロセス起動コマンド書式(GUI) */
#define	EWDT_CMD_BUF_LEN			(256)					/* プロセス削除/起動コマンド文字列バッファ長 */

#define	EWDT_PROC_TBL_NUM			(12)			/* プロセス監視テーブル要素数 */

#define	EWDT_FLG_OFF				(0x00)	/* 内部フラグ設定値(フラグOFF) */
#define	EWDT_FLG_ON					(0x01)	/* 内部フラグ設定値(フラグON) */

/***********************************************************
* グローバル変数プロトタイプ
************************************************************/
extern sint g_EWDT_WdtFd;
extern pthread_mutex_t g_EWDT_Mtx;
extern const EwdtProcTbl_t g_EWDT_ProcTbl[EWDT_PROC_TBL_NUM];
extern uchar g_EWDT_ProcWdtCnt[PROCESS_MAX];
extern uchar g_EWDT_ProcWdtResFlg[PROCESS_MAX];
extern uchar g_EWDT_ProcWdtSndFlg[PROCESS_MAX];
extern EwdtTermType_t g_EWDT_TermType;
extern uchar g_EWDT_TermTimeoutCnt;
extern uchar g_EWDT_ProcTermResFlg[PROCESS_MAX];
extern uchar g_EWDT_ProcTermFinishFlg;
extern uchar g_EWDT_PwSwOffDetectCnt;
extern bool_t g_EWDT_MemChkFlg;
extern ulong g_EWDT_MemChkTimErrCnt;

/***********************************************************
* 内部関数プロトタイプ
************************************************************/
extern void ewdt_FncWdtRes(ulong sport, uchar timeout);
extern void ewdt_FncTermReq(EwdtTermType_t type, ulong reqMsgid);
extern void ewdt_FncTermRes(ulong sport, EwdtTermType_t type);
extern void* ewdt_SubWdtTh(void* prm);
extern void ewdt_SubChkProcAlive(void);
extern void ewdt_SubChkTermTimeout(void);
extern void ewdt_SubChkMemClear(void);

#endif	/* __EWDT_H__ */
