/******************************************************************************/
/*    Description    :                                                        */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC Gu.CM                                              */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/04/03                                              */
/*----------------------------------------------------------------------------*/
/*    Module Name    :                                                        */
/*----------------------------------------------------------------------------*/
/*    Note           :                                                        */
/*----------------------------------------------------------------------------*/
/*    Copyright      :(c) 2020 NICHICON CORPORATION                           */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Include Header File                                                        */
/*----------------------------------------------------------------------------*/
#include <errno.h>
#include <string.h>

#include "etim_serv.h"
#include "etim_file.h"
#include "etim_utl.h"
#include "etim_msg.h"
#include "etim_ntp.h"
#include "etim_rtc.h"
#include "common_g.h"
#include "dlog_g.h"
#include "etim_msg.h"
#include "frm_g.h"
#include "task_g.h"
/*----------------------------------------------------------------------------*/
/* 公開変数定義                                                                */
/*----------------------------------------------------------------------------*/
pthread_mutex_t g_etimNtpTimeMutx;							/* NTP同期時刻排他制御用 */
pthread_mutex_t g_etimSysCmdMutx;							/* system()実行排他用 */
pthread_mutex_t g_etimNtpInfoMutex;							/* NTPサーバー情報排他制御用 */
pthread_mutex_t g_etimRtcMutx;								/* RTCアクセス排他用 */
ulong g_etimNtpRetry;										/* NTPリトライカウンタ */
bool_t g_etimRtcWriteEnable = TRUE;							/* RTC書込み許可フラグ */
struct tm g_etimNtpTime[ETIM_NTPTIME_SIZE];					/* NTP同期時刻管理 */
uchar g_etimNtpReset;										/* 時刻リセットフラグ  */
schar g_etimNtpHost[ETIM_NTPHOST_BUFSZ] = "localhost\0";	/* IPv4/IPv6アドレス又はホスト名 */
struct timespec g_etimLimitDate;
EtimNtpInfo_t g_etimNtpInfo = {0};							/* NTPサーバー情報 */
EtimNtpState_t s_etimSyncState;
/*----------------------------------------------------------------------------*/
/* 非公開変数定義                                                              */
/*----------------------------------------------------------------------------*/
#define ETIM_STATETBL_SIZE		((uchar)4u)
#define ETIM_STATETBL_SUB_SIZE	((uchar)4u)


/* pingスレッド実行状態 */
typedef enum {
	ETIM_PINGST_IDLE = 0,	/* 未実行 */
	ETIM_PINGST_START,		/* 開始 */
	ETIM_PINGST_RUN,		/* 実行中 */
	ETIM_PINGST_OK,			/* 実行完了(OK) */
	ETIM_PINGST_NG			/* 実行完了(NG) */
} EtimPinstE_t;

pthread_t s_etimPingWatchTh;			/* ping 監視スレッドID */
pthread_t s_etimpingThread;				/* ping スレッドID */
pthread_mutex_t s_etimPingStateMutex;
EtimPinstE_t s_etimPingState = ETIM_PINGST_IDLE;
pthread_mutex_t s_etimPingReturnIdMutex;
ulong s_etimPingReturnId = 0;
slong s_timerFlag = -1;

void etim_NtpSyncStart(void);
void etim_NtpSyncRetry(void);
void etim_NtpSyncSucc(void);
void etim_NtpSyncFail(void);

typedef struct {
	EtimNtpState_t ntpSyncTrigger;
	void (*action)();
}EtimNtpStateMana_t;


const EtimNtpStateMana_t s_ntpSyncStateTbl[ETIM_STATETBL_SIZE][ETIM_STATETBL_SUB_SIZE] = {
	{
		{ETIM_STATE_NTP_SYNC_NG, etim_NtpSyncRetry},
		{ETIM_STATE_NTP_SYNC_OK, NULL},
		{ETIM_STATE_NTP_SYNC_RETRY, NULL},
		{ETIM_STATE_NTP_SYNC_INIT, etim_NtpSyncStart}
	},
	{
		{ETIM_STATE_NTP_SYNC_NG, NULL},
		{ETIM_STATE_NTP_SYNC_OK, etim_NtpSyncSucc},
		{ETIM_STATE_NTP_SYNC_RETRY, NULL},
		{ETIM_STATE_NTP_SYNC_INIT, etim_NtpSyncStart}
	},
	{
		{ETIM_STATE_NTP_SYNC_NG, etim_NtpSyncFail},
		{ETIM_STATE_NTP_SYNC_OK, NULL},
		{ETIM_STATE_NTP_SYNC_RETRY, etim_NtpSyncRetry},
		{ETIM_STATE_NTP_SYNC_INIT, etim_NtpSyncStart}
	},
	{
		{ETIM_STATE_NTP_SYNC_NG, NULL},
		{ETIM_STATE_NTP_SYNC_OK, NULL},
		{ETIM_STATE_NTP_SYNC_RETRY, NULL},
		{ETIM_STATE_NTP_SYNC_INIT, etim_NtpSyncStart}
	}
};

/*----------------------------------------------------------------------------*/
/* 公開関数定義                                                                */
/*----------------------------------------------------------------------------*/
void *etim_PingWatch(void *arg);
sint etim_MutexTrylock(pthread_mutex_t *pmutex);
void *etim_PingStart(void *arg);
ulong etim_GetPingReturnId(void);
void etim_SetPingReturnId(ulong returnMsgId);
EtimPinstE_t etim_GetPingState(void);
void etim_SetPingState(EtimPinstE_t set);

/*============================================================================*/
/*    Description    :NTP时间同步状态处理                                       */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/13  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void ETIM_NtpSyncStateProc(EtimNtpState_t trigger)
{
	if(NULL != s_ntpSyncStateTbl[s_etimSyncState][trigger].action)
	{
		s_ntpSyncStateTbl[s_etimSyncState][trigger].action();
	}
	else
	{
		/* do nothing */
	}
}

/*============================================================================*/
/*    Description    :起動処理                                                 */
/*----------------------------------------------------------------------------*/
/*    param          :rebootPrm   WDTからの再起動の場合1、通常起動の場合0       */
/*----------------------------------------------------------------------------*/
/*    return         :sint	処理結果                                           */
/*                    (ETIM_OK(0):正常/ETIM_E_SYSCALL(-1):システムコールエラー) */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/03  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_ProcStart(sint rebootPrm)
{
	struct tm tm;
	sint ret;
	EtimNtpInfo_t ntpInfo;

	DEBUG_TRCIN();

	/* グローバル変数初期化 */
	memset(g_etimNtpTime, 0x00, sizeof(g_etimNtpTime) );
	g_etimNtpTime[ETIM_NTPTIME_BASE].tm_year = 70;
	g_etimNtpTime[ETIM_NTPTIME_BASE].tm_mon  = 0;
	g_etimNtpTime[ETIM_NTPTIME_BASE].tm_mday = 2;

	/* 次回予定時刻の初期値には、2038/1/1 00:00:00を設定 */
	/* 時刻同期開始、及びリトライする場合は再設定が必要  */
	g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_year = 138;
	g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_mon  = 0;
	g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_mday = 1;

	/* mutex初期化 */
	CMN_MutexInit(&g_etimNtpTimeMutx);
	CMN_MutexInit(&g_etimRtcMutx);
	CMN_MutexInit(&g_etimSysCmdMutx);
	CMN_MutexInit(&g_etimNtpInfoMutex);

	/* ローカル変数初期化 */
	memset(&tm, 0x00, sizeof(tm));

	/* 起動要因チェック */
	if (0 != rebootPrm)
	{
		/* NTPサーバー情報読み込み */
		ret = ETIM_ReadNtpServerInfo(&ntpInfo);
		if (ret == ETIM_OK)
		{
			ETIM_SetNtpServerInfo(&ntpInfo);
		}
	}
	else
	{
		/* NTPサーバー情報を読み込んでいない場合、NTPサーバー情報初期化する */
		/* RAMから読み込んだ情報をサブスレッドの先頭で上書きする不具合対策  */
		ETIM_InitNtpServerInfo();
	}

	/* RTC時刻取得 */
	ret = ETIM_GetRtc(&tm);
	if (ETIM_OK != ret)
	{
		DLOG_Error("ETIM_GetRtc() error.\n");
	}
	else
	{
		/* システム時刻設定 */
		ret = ETIM_SetTime(&tm);
		if (ETIM_OK != ret)
		{
			DLOG_Error("ETIM_SetTime() error.\n");
		}
		else
		{
			/* do nothing */
		}
	}

	g_etimNtpReset = 0;

	/* リターンを返す */
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :看门狗请求                                               */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/true:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/03  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void ETIM_WdtReq(void)
{
	DEBUG_TRCIN();

	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :                                                        */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/true:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/03  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void ETIM_TemrReq(void)
{
	DEBUG_TRCIN();

	CMN_MutexLock(&g_etimRtcMutx);
	g_etimRtcWriteEnable = FALSE;
	CMN_MutexUnlock(&g_etimRtcMutx);

	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :NTPサーバー疎通確認起動処理                               */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :sint	処理結果                                           */
/*                    (ETIM_OK(0):正常/                                        */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー/                 */
/*                     ETIM_E_OTHER(-4):その他エラー(不通含む))                 */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_PingNtpsvr(sint returnMsgId)
{
	EtimPinstE_t state;
	sint ret = ETIM_E_SYSCALL;

	DEBUG_TRCIN();

	state = etim_GetPingState();
	if (ETIM_PINGST_IDLE == state)
	{
		etim_SetPingState(ETIM_PINGST_START);
		etim_SetPingReturnId((ulong)returnMsgId);

		ret = (sint)FRM_PthreadCreate(&s_etimPingWatchTh, NULL, etim_PingWatch, NULL);
		if (0 != ret)
		{
			DLOG_Error("FRM_PthreadCreate() error.(%d)\n", errno);
		}
		else
		{
			ret = ETIM_OK;
		}
	}
	else
	{
		DLOG_Error("ping already exec.\n");
		ret = ETIM_E_OTHER;
	}

	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :時刻設定処理関数                                         */
/*----------------------------------------------------------------------------*/
/*    param          :const struct tm *ptm 設定時刻                            */
/*----------------------------------------------------------------------------*/
/*    return         :sint 処理結果                                            */
/*                    (ETIM_OK(0):正常/                                       */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー)/                */
/*                     ETIM_E_PARAM(-2):引数異常(ポインタNULL)/                */
/*                     ETIM_E_OUTVAL(-3):設定値異常(範囲外)/                   */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_SetTime(struct tm *ptm)
{
	struct timespec tspec;
	sint ret;
	sint result = ETIM_E_SYSCALL;

	DEBUG_TRCIN();

	/* 引数確認 */
	if (NULL == ptm)
	{
		DLOG_Error("prameter NULL.\n");
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	/* 時刻範囲確認 */
	ret = ETIM_CheckDateRange(ptm);
	if (ETIM_OK == ret)
	{
		/* NTP同期時刻排他制御ロック */
		CMN_MutexLock(&g_etimNtpTimeMutx);

		if(ETIM_STATE_NTP_SYNC_NG == s_etimSyncState)
		{
			/* システム時刻へ変換 */
			memset(&tspec, 0x00, sizeof(tspec));
			ret = ETIM_MkTime(ptm, &tspec.tv_sec);
			if(ETIM_OK == ret)
			{
					/* システム時刻設定 */
				ret = ETIM_ClockSetTime(CLOCK_REALTIME, &tspec);
				if(ETIM_OK == ret)
				{
					s_etimSyncState = ETIM_STATE_NTP_SYNC_OK;
					ETIM_NtpSyncStateProc(ETIM_STATE_NTP_SYNC_OK);
				}
				else
				{
					result = ret;
					DLOG_Error("ETIM_ClockSetTime() error.\n");
				}
			}
			else
			{
				DLOG_Error("ETIM_MkTime() error.\n");
				result = ret;
			}
		}
		/* NTP同期時刻排他制御アンロック */
		CMN_MutexUnlock(&g_etimNtpTimeMutx);
	}
	else
	{
		DLOG_Error("ETIM_CheckDateRange() error.\n");
		result = ret;
	}

	/* リターンを返す */
	DEBUG_TRCOUT();
	return result;
}

/*============================================================================*/
/*    Description    :设置NTP时间同步定时                                      */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/13  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void Etim_NtpSyncTimer(void)
{
	struct tm ltm;
	FrmTime_t ntpSyncTimer;
	struct tm *ret;

	ETIM_SetSyncTime(&g_etimNtpInfo);

	ret = localtime_r(&g_etimLimitDate.tv_sec, &ltm);
	if(NULL != ret)
	{
		ntpSyncTimer.year = (ushort)(ltm.tm_year + 1900);
		ntpSyncTimer.mon = (uchar)(ltm.tm_mon + 1);
		ntpSyncTimer.day = (uchar)ltm.tm_mday;
		ntpSyncTimer.hour = (uchar)ltm.tm_hour;
		ntpSyncTimer.min = (uchar)ltm.tm_min;
		ntpSyncTimer.sec = (uchar)ltm.tm_sec;

		FRM_TimerSetAlarm(ETIM_NTP_SYNC_TIMER_ID, &ntpSyncTimer, NULL);
	}
	else
	{
		/* do nothing */
	}
}

/*----------------------------------------------------------------------------*/
/* 非公開関数定義                                                              */
/*----------------------------------------------------------------------------*/

/*============================================================================*/
/*    Description    :疎通確認IF返信先プロセスID取得                            */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :ulong   返回ID                                             */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
ulong etim_GetPingReturnId(void)
{
	ulong ret;

	DEBUG_TRCIN();

	CMN_MutexLock(&s_etimPingReturnIdMutex);
	ret = s_etimPingReturnId;
	CMN_MutexUnlock(&s_etimPingReturnIdMutex);

	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :疎通確認IF返信先プロセスID設定                            */
/*----------------------------------------------------------------------------*/
/*    param          :ulong returnMsgId   消息ID                              */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void etim_SetPingReturnId(ulong returnMsgId)
{
	DEBUG_TRCIN();

	CMN_MutexLock(&s_etimPingReturnIdMutex);
	s_etimPingReturnId = returnMsgId;
	CMN_MutexUnlock( &s_etimPingReturnIdMutex );

	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :疎通確認実行状態取得                                      */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :EtimPinstE_t                                            */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
EtimPinstE_t	etim_GetPingState(void)
{
	EtimPinstE_t ret;

	DEBUG_TRCIN();

	CMN_MutexLock(&s_etimPingStateMutex);
	ret = s_etimPingState;
	CMN_MutexUnlock(&s_etimPingStateMutex);

	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :疎通確認実行状態設定                                      */
/*----------------------------------------------------------------------------*/
/*    param          :EtimPinstE_t                                            */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void etim_SetPingState(EtimPinstE_t set)
{
	DEBUG_TRCIN();

	CMN_MutexLock(&s_etimPingStateMutex);
	s_etimPingState = set;
	CMN_MutexUnlock(&s_etimPingStateMutex);

	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :NTPサーバー疎通ping発行スレッド                           */
/*----------------------------------------------------------------------------*/
/*    param          :void  *arg                                              */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void *etim_PingStart(void *arg)
{
	schar cmd[ETIM_CMDLINE_BUFSIZE];
	sint ret;

	DEBUG_TRCIN();

	etim_SetPingState(ETIM_PINGST_RUN);

	memset(cmd, 0x00, sizeof(cmd));

	snprintf(cmd, sizeof(cmd) - 1, ETIM_PINGCMD"%s"ETIM_CMDRED, g_etimNtpHost);

	ret = ETIM_CALL_SYSTEM(cmd);
	DLOG_Info("system(%s) return %d\n", cmd, ret);

	if (0 != ret)
	{
		etim_SetPingState(ETIM_PINGST_NG);
	}
	else
	{
		etim_SetPingState(ETIM_PINGST_OK);
	}

	pthread_detach(pthread_self());

	DEBUG_TRCOUT();

	return NULL;
}

/*============================================================================*/
/*    Description    :NTPサーバー疎通ping監視スレッド                           */
/*----------------------------------------------------------------------------*/
/*    param          :void  *arg                                              */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void *etim_PingWatch(void *arg)
{
	sint ret;
	struct timespec req;	/* スリープ要求時間 */
	struct timespec rem;	/* スリープ残り時間 */
	ulong cnt = 0;
	schar cmd[ETIM_CMDLINE_BUFSIZE];
	EtimPinstE_t state;
	EtimMsgRspData_t fRsp;			/* 返信パラメータ */
	slong returnCode;
	ulong returnMsgId;
	sint pingStateFlag = 1;

	DEBUG_TRCIN();

	ret = etim_MutexTrylock(&g_etimSysCmdMutx);

	if (ETIM_OK == ret)
	{
		etim_SetPingState(ETIM_PINGST_START);

		ret = (sint)FRM_PthreadCreate(&s_etimpingThread, NULL, etim_PingStart, NULL);
		if (0 == ret)
		{
			cnt = 0;
			while(pingStateFlag)
			{
				state = etim_GetPingState();

				if ((ETIM_PINGST_START != state) && (ETIM_PINGST_RUN != state))
				{
					pingStateFlag = 0;
				}
				else if (ETIM_PINGTOUT <= cnt)
				{
					snprintf(cmd, sizeof(cmd) - 1, ETIM_PINGKILL ETIM_CMDRED);
					ret = ETIM_CALL_SYSTEM(cmd);
					DLOG_Info("system(%s) return %d\n", cmd, ret);

					DLOG_Error("ping timeout. pthread_cancel().\n");
					pthread_cancel(s_etimpingThread);

					etim_SetPingState(ETIM_PINGST_NG);
					pingStateFlag = 0;
				}
				else
				{
					req.tv_sec  = 0;
					req.tv_nsec = ETIM_M_MSEC(ETIM_PINGWAIT_DIV);
					nanosleep(&req, &rem);
					cnt += ETIM_PINGWAIT_DIV;
				}
			}

			pthread_join(s_etimpingThread, NULL);
		}
		else
		{
			DLOG_Error("FRM_PthreadCreate() error.(%d)\n", errno );
			etim_SetPingState(ETIM_PINGST_NG);
		}
	}
	else
	{
		DLOG_Error("other system() was executed.\n");
		etim_SetPingState( ETIM_PINGST_NG );
	}

	CMN_MutexUnlock(&g_etimSysCmdMutx);

	memset(&fRsp, 0x00, sizeof(fRsp));

	returnCode = (ETIM_PINGST_OK == etim_GetPingState()) ? ETIM_OK : ETIM_E_OTHER;

	fRsp.header.mType			= MSG_TYPE_ASYNC;
	fRsp.header.attr			= FRM_MSG_ASYNC;
	fRsp.header.msgId	 		= 0;
	fRsp.header.destModId	 	= 0;
	fRsp.header.msgSize			= sizeof(slong);
	fRsp.header.retCode			= (slong)returnCode;

	returnMsgId = etim_GetPingReturnId();
	ret = (sint)FRM_MsgSendAsync(returnMsgId, sizeof(EtimMsgRspData_t), (void *)&fRsp);
	if (-1 == ret)
	{
		DLOG_Error("response lfw_ASyncCall() error.\n");
	}

	etim_SetPingState(ETIM_PINGST_IDLE);
	pthread_detach(pthread_self());

	DEBUG_TRCOUT();

	return NULL;
}

/*============================================================================*/
/*    Description    :排他ロック処理（トライ）                                  */
/*----------------------------------------------------------------------------*/
/*    param          :pthread_mutex_t *pmutex    mutexポイン                   */
/*----------------------------------------------------------------------------*/
/*    return         :sint 処理結果                                            */
/*                    (ETIM_OK(0):正常/                                       */
/*                     ETIM_E_PARAM(-2):引数異常(ポインタNULL)/                */
/*                     ETIM_E_OUTVAL(-3):設定値異常(範囲外))                   */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint etim_MutexTrylock(pthread_mutex_t *pmutex)
{
	sint ret = ETIM_E_SYSCALL;

	DEBUG_TRCIN();

	/* 引数確認 */
	if (NULL == pmutex)
	{
		DLOG_Error("parameter NULL.\n");
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	/* ロック */
	ret = pthread_mutex_trylock(pmutex);
	if (EINVAL == ret)
	{
		/* mutexが適切に初期化されていない */
		CMN_MutexInit(pmutex);
		/* 再ロック */
		pthread_mutex_lock(pmutex);
	}
	else if (EBUSY == ret)
	{
		ret = ETIM_E_OTHER;
	}
	else
	{
		ret = ETIM_OK;
	}

	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :进行NTP同步                                              */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/12  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void etim_NtpSyncStart(void)
{
	sint ret;
	EtimNtpInfo_t ntpInfo;
	EtimNtpInfo_t afterNtpInfo;
	schar cmd[ETIM_CMDLINE_BUFSIZE];
	bool_t ntpChange;
	NtpStateMsg_t ntpSyncMsg;

	DEBUG_TRCIN();

	/* 同期状態更新 */
	ETIM_UpdateSyncState();

	/* NTPサーバー情報取得 */
	ret = ETIM_GetNtpServerInfo(&ntpInfo, cmd);

	/* NTP実行予定時刻排他アンロック */
	CMN_MutexLock(&g_etimNtpTimeMutx);

	/* NTP同期コマンド実行 */
	ret = ETIM_NtpSync(cmd);

	/* NTP実行予定時刻排他ロック */
	CMN_MutexUnlock(&g_etimNtpTimeMutx);

	if(ETIM_OK == ret)
	{
		/* NTPサーバー情報再取得 */
		ret = ETIM_GetNtpServerInfo(&afterNtpInfo, NULL);
		/* 接続先変更チェック */
		ntpChange = ETIM_IsChangeNtpServerInfo(&ntpInfo, &afterNtpInfo);
		if(!ntpChange)
		{
			s_etimSyncState = ETIM_STATE_NTP_SYNC_OK;
		}
		else
		{
			s_etimSyncState = ETIM_STATE_NTP_SYNC_RETRY;
		}

		ntpSyncMsg.header.msgId = ETIM_MSGID_NTP_SYNC_TREG;
		ntpSyncMsg.trigger = s_etimSyncState;
		FRM_MsgSendAsync(ETIM_SPORT, sizeof(NtpStateMsg_t), &ntpSyncMsg);
	}
	else
	{
		s_etimSyncState = ETIM_STATE_NTP_SYNC_INIT;
	}

	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :                                                        */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/true:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/13  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void etim_NtpSyncSucc(void)
{
	DEBUG_TRCIN();

	g_etimNtpRetry = 0;

	if(s_timerFlag)
	{
		FRM_TimerCancelOneShot(ETIM_NTP_SYNC_JUDGE_TIMER_ID);
		s_timerFlag = FALSE;
	}

	Etim_NtpSyncTimer();

	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :                                                        */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/true:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/17  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void etim_NtpSyncFail(void)
{
	NtpStateMsg_t ntpSyncMsg;
	s_etimSyncState = ETIM_STATE_NTP_SYNC_NG;

	ntpSyncMsg.header.msgId = ETIM_MSGID_NTP_SYNC_TREG;
	ntpSyncMsg.trigger = s_etimSyncState;
	FRM_MsgSendAsync(ETIM_SPORT, sizeof(NtpStateMsg_t), &ntpSyncMsg);
}

/*============================================================================*/
/*    Description    :NTP同步重试                                              */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/12  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void etim_NtpSyncRetry(void)
{
	ulong interval;
	bool_t retryFlg;
	sint ret;

	DEBUG_TRCIN();

	if(!s_timerFlag)
	{
		ret = FRM_TimerSetOneShot(ETIM_NTP_SYNC_JUDGE_TIMER_ID, ETIM_DAYSEC_SZ, NULL);
		if(ETIM_OK == ret)
		{
			s_timerFlag = TRUE;
		}
	}

	ret = ETIM_CheckRetry(&g_etimNtpInfo, g_etimNtpRetry, &interval, &retryFlg);
	if(retryFlg)
	{
		FRM_TimerSetOneShot(ETIM_RETRY_TIMER_ID, interval, NULL);
		g_etimNtpRetry++;
		ETIM_UpdateSyncState();
	}
	else
	{
		/* do nothing */
	}

	DEBUG_TRCIN();
}

/* End Of File */
