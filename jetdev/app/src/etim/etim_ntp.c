/******************************************************************************/
/*    Description    :時刻管理ミドル                                           */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC Gu.CM                                               */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/04/04                                              */
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>

#include "task_g.h"
#include "common_g.h"
#include "lfw_g.h"
#include "etim_serv.h"
#include "etim_ntp.h"
#include "etim_rtc.h"
#include "etim_utl.h"
#include "dlog_g.h"
#include "etim_msg.h"
//#include <esch.h>

/*----------------------------------------------------------------------------*/
/* 公開変数定義                                                                */
/*----------------------------------------------------------------------------*/
extern schar g_etimNtpHost[ETIM_NTPHOST_BUFSZ];			/* IPv4/IPv6アドレス又はホスト名 */
extern struct tm g_etimNtpTime[ETIM_NTPTIME_SIZE];		/* NTP同期時刻管理 */
extern pthread_mutex_t g_etimNtpTimeMutx;				/* NTP同期時刻排他制御用 */
extern ulong g_etimNtpRetry;							/* NTPリトライカウンタ */
extern uchar g_etimNtpReset;										/* 時刻リセットフラグ  */
extern pthread_mutex_t g_etimSysCmdMutx;				/* system()実行排他用 */
extern pthread_mutex_t g_etimNtpInfoMutex;				/* NTPサーバー情報排他制御用 */
extern EtimNtpState_t s_etimSyncState;					/* 時刻同期状態 */
extern struct timespec g_etimLimitDate;					/* リトライ限界時刻 */
extern EtimNtpInfo_t g_etimNtpInfo;				/* NTPサーバー情報 */

/*----------------------------------------------------------------------------*/
/* 非公開変数定義                                                              */
/*----------------------------------------------------------------------------*/
typedef struct {
	EtimNtpInfo_t ntpInfo;							/* NTPサーバー接続先情報 */
	ulong reryNum;									/* リトライ回数 */
	uchar reserved[ETIM_RESERVED_BUF_SIZE];			/* 予備 */
	EtimNtpState_t state;							/* 時刻同期状態 */
} EtimNtpSyncState_t;

schar s_etimNtpSyncCmd[ETIM_CMDLINE_BUFSIZE] = {0};		/* NTPサーバー同期コマンド */

/*----------------------------------------------------------------------------*/
/* 公開関数定義                                                                */
/*----------------------------------------------------------------------------*/
/*============================================================================*/
/*    Description    :NTP時刻同期処理                                          */
/*----------------------------------------------------------------------------*/
/*    param          :const schar* pCmd    同期コマンド                        */
/*----------------------------------------------------------------------------*/
/*    return         :sint	処理結果                                           */
/*                    (ETIM_OK(0):正常/                                        */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー:異常発生有))      */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_NtpSync(const schar *pCmd)
{
	schar ebuf[ETIM_ERR_BUF_SIZE];
	sint ret = ETIM_E_SYSCALL;
	size_t len;

	DEBUG_TRCIN();

	/* 引数確認 */
	if (NULL == pCmd)
	{
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	len = strnlen(pCmd, ETIM_CMDLINE_BUFSIZE);

	CMN_MutexLock(&g_etimSysCmdMutx);
	DLOG_Info("command = \"%s\"(%d)\n", pCmd, len);

#ifndef DONTNTPD
	ret = ETIM_CALL_SYSTEM(pCmd);
#else
	ret = 0;
#endif

	DLOG_Info("system() return %d\n", ret);
	CMN_MutexUnlock(&g_etimSysCmdMutx);

	if (0 != ret)
	{
		memset(ebuf, 0x00, sizeof(ebuf));
		strerror_r(errno, ebuf, ETIM_ERR_BUF_SIZE);
		DLOG_Error("system() error(%d,%s).\n", errno, ebuf);
	}
	else
	{
		/* RTCへ反映 */
		ret = ETIM_SetRtc();
		if (ret != ETIM_OK)
		{
			DLOG_Error("ETIM_SetRtc() error.\n");
		}

		/* eschへ時刻同期イベントを送信 */
		//esch_async_notifyTimeSync();
		ret = ETIM_OK;
	}

	DEBUG_TRCOUT();

	return ret;
}
/*============================================================================*/
/*    Description    :NTP同期起動時刻決定処理                                   */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :sint	処理結果                                           */
/*                    (ETIM_OK(0):正常/                                        */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー:異常発生有))      */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_MakeNtpTime(void)
{
	sint fd;				/* socketディスクリプタ */
	struct ifreq ifr;		/* インターフェース要求構造体 */
	struct timespec t0;		/* 基準時刻（秒） */
	sint ret = ETIM_E_SYSCALL;
	uint seed = 0;
	sint rand = 0;
    schar ebuf[ETIM_ERR_BUF_SIZE];

	DEBUG_TRCIN();

	/* 初期化 */
	memset(&ifr, 0x00, sizeof(ifr));
	memset(&t0, 0x00, sizeof(t0));

	/* ソケットをopenする */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 != fd)
	{
		/* アドレスファミリとIF名を設定 */
		ifr.ifr_addr.sa_family = AF_INET;
		strncpy(ifr.ifr_name, ETIM_IFNAME, IFNAMSIZ - 1);

		/* 情報を取得する */
		ret = ioctl(fd, SIOCGIFHWADDR, &ifr);

		/* ソケットをクローズする */
		ETIM_CLOSE(fd);

		if (-1 != ret)
		{

#ifdef DEBUG
			DLOG_Info("MAC addr = %02x:%02x:%02x:%02x:%02x:%02x\n",
				ifr.ifr_hwaddr.sa_data[0],
				ifr.ifr_hwaddr.sa_data[1],
				ifr.ifr_hwaddr.sa_data[2],
				ifr.ifr_hwaddr.sa_data[3],
				ifr.ifr_hwaddr.sa_data[4],
				ifr.ifr_hwaddr.sa_data[5]);
#endif

			/* 乱数の種を設定 */
			seed = ((uint)ifr.ifr_hwaddr.sa_data[3]) & 0x000000FF;
			seed = (seed << 8) | (((unsigned int)ifr.ifr_hwaddr.sa_data[4]) & 0x000000FF);
			seed = (seed << 8) | (((unsigned int)ifr.ifr_hwaddr.sa_data[5]) & 0x000000FF);
			seed++;	/* 0回避 */

#ifdef DEBUG
			DLOG_Info("seed = %08x\n", seed);
#endif
			srandom(seed);

			/* 乱数を取得 */
			rand = (sint)random();
			/* １日分の秒数に変換 */
			rand = ETIM_DAYSEC(rand);

#ifdef DEBUG
			DLOG_Info("rand = %d\n", rand);
#endif

			/* 基準時刻（時分秒）を設定 */
			g_etimNtpTime[ETIM_NTPTIME_BASE].tm_hour = rand / 3600;
			g_etimNtpTime[ETIM_NTPTIME_BASE].tm_min  = (rand % 3600) / 60;
			g_etimNtpTime[ETIM_NTPTIME_BASE].tm_sec  = (rand % 3600) % 60;

			DLOG_Info("t0 = %02d:%02d:%02d\n",
			g_etimNtpTime[ETIM_NTPTIME_BASE].tm_hour,
			g_etimNtpTime[ETIM_NTPTIME_BASE].tm_min,
			g_etimNtpTime[ETIM_NTPTIME_BASE].tm_sec);
		}
		else
		{
			/* エラー */
			memset(ebuf, 0x00, sizeof(ebuf));
			strerror_r(errno, ebuf, ETIM_ERR_BUF_SIZE);
			DLOG_Error("ioctl() error(%s).\n", ebuf);
		}
	}
	else
	{
		memset(ebuf, 0x00, sizeof(ebuf));
		strerror_r(errno, ebuf, ETIM_ERR_BUF_SIZE);
		DLOG_Error("socket() error(%s).\n", ebuf);
	}

	DEBUG_TRCOUT();

	return ret;
}
/*============================================================================*/
/*    Description    :NTP同期予定時間設定（基準時刻）                            */
/*----------------------------------------------------------------------------*/
/*    param          :sint	offset          現在時刻オフセット                  */
/*----------------------------------------------------------------------------*/
/*    return         :sint	処理結果                                           */
/*                    (ETIM_OK(0):正常/                                        */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー))      */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_NtpSetNextTime(sint offset)
{
	struct timespec t0;		/* 基準時刻 */
	struct timespec t1;		/* 現在時刻 */
	struct timespec tt;		/* 比較計算用 */
	struct tm ttm;			/* 比較計算用 */
	struct tm t1m;			/* 現在時刻用 */
	ulong diff;				/* 差分秒数 */
	sint ret = ETIM_E_SYSCALL;

	DEBUG_TRCIN();

	/* 初期化 */
	memset(&t0, 0x00, sizeof(t0));
	memset(&t1, 0x00, sizeof(t1));
	memset(&tt, 0x00, sizeof(tt));
	memset(&ttm, 0x00, sizeof(ttm));
	memset(&t1m, 0x00, sizeof(t1m));

	/* システム時刻取得 */
	ret = ETIM_ClockGetTime(CLOCK_REALTIME, &t1);
	if (ETIM_OK == ret)
	{
		t1.tv_sec += offset;
		/* 現在時刻をローカル時刻へ変換して予定時刻へ設定 */
		ret = ETIM_LocalTimeR(&t1.tv_sec, &t1m);
		if (ETIM_OK == ret)
		{
				/* 基準時刻を秒数に変換 */
#ifdef DEBUG
			DLOG_Info("NtpTime[0] is %04d/%02d/%02d %02d:%02d:%02d\n",
				g_etimNtpTime[ETIM_NTPTIME_BASE].tm_year + 1900,
				g_etimNtpTime[ETIM_NTPTIME_BASE].tm_mon  + 1,
				g_etimNtpTime[ETIM_NTPTIME_BASE].tm_mday,
				g_etimNtpTime[ETIM_NTPTIME_BASE].tm_hour,
				g_etimNtpTime[ETIM_NTPTIME_BASE].tm_min,
				g_etimNtpTime[ETIM_NTPTIME_BASE].tm_sec);
#endif
			ret = ETIM_MkTime(&g_etimNtpTime[ETIM_NTPTIME_BASE], &t0.tv_sec);
			if (ETIM_OK != ret)
			{
				/* 計算用コピー */
				memcpy(&ttm, &t1m, sizeof(ttm));
				/* 時分秒の差分のみ残すため基準日の年月日を設定 */
				ttm.tm_year = g_etimNtpTime[ETIM_NTPTIME_BASE].tm_year;
				ttm.tm_mon  = g_etimNtpTime[ETIM_NTPTIME_BASE].tm_mon;
				ttm.tm_mday = g_etimNtpTime[ETIM_NTPTIME_BASE].tm_mday;
				/* 秒へ変換 */
				tt.tv_nsec  = 0;
				ret = ETIM_MkTime(&ttm, &tt.tv_sec);
				if (ETIM_OK == ret)
				{
					/* 予定時刻へコピー */
					memcpy(&g_etimNtpTime[ETIM_NTPTIME_SCHE], &t1m, sizeof(g_etimNtpTime[ETIM_NTPTIME_SCHE]));

					/* 比較 */
					diff = (ulong)difftime(t0.tv_sec, tt.tv_sec);

#ifdef DEBUG
					DLOG_Info("difftime(t0.tv_sec(%ld), tt.tv_sec(%ld) = %ld\n",
						t0.tv_sec, tt.tv_sec, diff);
#endif

					if (0 >= diff)
					{
					/* 基準時刻(t0)が現在時刻(tt)より過去 */
					/* 翌日を指定(mktimeで正規化されるので単純加算) */
					g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_mday++;
					}

					/* 基準時刻を設定 */
					g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_hour = g_etimNtpTime[ETIM_NTPTIME_BASE].tm_hour;
					g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_min = g_etimNtpTime[ETIM_NTPTIME_BASE].tm_min;
					g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_sec = g_etimNtpTime[ETIM_NTPTIME_BASE].tm_sec;

#ifdef DEBUG
					DLOG_Info("next NTP schedule is %04d/%02d/%02d %02d:%02d:%02d\n",
							g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_year + 1900,
							g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_mon  + 1,
							g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_mday,
							g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_hour,
							g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_min,
							g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_sec);
#endif
				}
				else
				{
					DLOG_Error("ETIM_MkTime(ETIM_NTPTIME_SCHE) error.\n");
				}
			}
			else
			{
				DLOG_Error("ETIM_MkTime(ETIM_NTPTIME_BASE) error.\n");
			}
		}
		else
		{
			DLOG_Error("ETIM_LocalTimeR() error.\n");
		}
	}
	else
	{
		DLOG_Error("ETIM_ClockGetTime() error.\n");
	}

	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :NTP時刻同期開始                                          */
/*----------------------------------------------------------------------------*/
/*    param          :const EtimNtpInfo_t*	pInfo   NTPサーバー情報            */
/*----------------------------------------------------------------------------*/
/*    return         :sint	処理結果                                           */
/*                    (ETIM_OK(0):正常/                                        */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー)/                */
/*                     ETIM_E_PARAM(-2):引数異常)                              */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_StartNtpSync(const EtimNtpInfo_t* pInfo)
{
	EtimNtpInfo_t ntpInfo;
	bool_t ntpChange = FALSE;
	sint ret = ETIM_E_PARAM;

	DEBUG_TRCIN();

	/* 引数確認 */
	if (NULL == pInfo)
	{
		return ETIM_E_PARAM;
	}

	/* NTPサーバー情報取得 */
	ret = ETIM_GetNtpServerInfo(&ntpInfo, NULL);

	/* NTPサーバー情報比較 */
	ntpChange = ETIM_IsChangeNtpServerInfo(&ntpInfo, pInfo);
	if (FALSE == ntpChange)
	{
		 /* NTPサーバー情報が同じ場合は、現在の動作を継続するため何もしない */
		 ret = ETIM_OK;
	}
	else
	{
			/* NTP同期時刻排他ロック */
		CMN_MutexLock(&g_etimNtpTimeMutx);

		/* NTPサーバー情報設定 */
		ret = ETIM_SetNtpServerInfo(pInfo);

		/* NTPサーバー情報取得 */
		ret = ETIM_GetNtpServerInfo(&ntpInfo, NULL);

		/* NTPサーバー情報書き込み */
		ret = ETIM_WriteNtpServerInfo(&ntpInfo);

		/* 次回予定時刻設定 */
		/* 接続先変更の場合、すぐに時刻同期をする */

		/* リトライカウンタ、リセットフラグ、同期状態を初期化 */
		g_etimNtpRetry = 0;
		s_etimSyncState = ETIM_STATE_NTP_SYNC_INIT;

		FRM_TimerCancelAlarm(ETIM_NTP_SYNC_TIMER_ID);
		FRM_TimerCancelAlarm(ETIM_RETRY_TIMER_ID);

		ETIM_NtpSyncStateProc(ETIM_STATE_NTP_SYNC_INIT);

#ifdef DEBUG
	DLOG_Error("next time init[19%02d/%02d/%02d %02d:%02d:%02d].\n"
						, g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_year
						, g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_mon + 1
						, g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_mday
						, g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_hour
						, g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_min
						, g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_sec );
#endif

		/* NTP同期時刻排他アンロック */
		CMN_MutexUnlock(&g_etimNtpTimeMutx);
	}

	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :NTPサーバー情報初期化                                     */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void ETIM_InitNtpServerInfo(void)
{
	DEBUG_TRCIN();

	/* NTPサーバー情報排他ロック */
	CMN_MutexLock(&g_etimNtpInfoMutex);

	/* NTPサーバー情報初期化 */
	memset(&g_etimNtpInfo, 0x00, sizeof(g_etimNtpInfo));
	g_etimNtpInfo.ntpserver = ETIM_NTP_INVALID;

	/* NTPサーバー情報排他アンロック */
	CMN_MutexUnlock(&g_etimNtpInfoMutex);

	DEBUG_TRCOUT();
	return;
}

/*============================================================================*/
/*    Description    :NTPサーバー情報設定                                      */
/*----------------------------------------------------------------------------*/
/*    param          :const EtimNtpInfo_t* pInfo   NTPサーバー情報             */
/*----------------------------------------------------------------------------*/
/*    return         :sint	処理結果                                           */
/*                    (ETIM_OK(0):正常/                                        */
/*                     ETIM_E_PARAM(-2):引数異常)                              */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_SetNtpServerInfo(const EtimNtpInfo_t* pInfo)
{
	sint ret;

	DEBUG_TRCIN();

	/* 引数確認 */
	if (NULL == pInfo)
	{
		return ETIM_E_PARAM;
	}

	/* NTPサーバー情報排他ロック */
	CMN_MutexLock(&g_etimNtpInfoMutex);

	/* NTPサーバー同期コマンド設定 */
	if (ETIM_NTP_DEFAULT == pInfo->ntpserver)
	{
		/* デフォルトNTPサーバー */
		g_etimNtpInfo.ntpserver = ETIM_NTP_DEFAULT;
		g_etimNtpInfo.portNum = ETIM_NTP_DEFAULT_PORT;
		g_etimNtpInfo.retryThreshold1 = ETIM_NTP_DEFAULT_THRESHOLD_1;
		g_etimNtpInfo.retryInterval1 = ETIM_NTP_DEFAULT_INTERVAL_1;
		g_etimNtpInfo.retrThreshold2 = ETIM_NTP_DEFAULT_THRESHOLD_2;
		g_etimNtpInfo.retryInterval2  = ETIM_NTP_DEFAULT_INTERVAL_2;
		memset(&g_etimNtpInfo.starSynctime, 0, sizeof(g_etimNtpInfo.starSynctime));
		memset(&g_etimNtpInfo.endSynctime, 0, sizeof(g_etimNtpInfo.endSynctime));
		memset(&g_etimNtpInfo.addr, 0, sizeof(g_etimNtpInfo.addr));
		snprintf(s_etimNtpSyncCmd, sizeof(s_etimNtpSyncCmd) - 1, ETIM_NTPCMD"%s"ETIM_CMDRED, g_etimNtpHost);
	}
	else
	{
		/* デフォルトNTPサーバー以外 */
		/* NTPサーバー情報設定 */
		g_etimNtpInfo = *pInfo;
		snprintf(s_etimNtpSyncCmd, sizeof(s_etimNtpSyncCmd) - 1, ETIM_NTPCMD"%s:%d"ETIM_CMDRED,
				pInfo->addr, pInfo->portNum);
	}

	/* NTPサーバー情報排他アンロック */
	CMN_MutexUnlock(&g_etimNtpInfoMutex);

	/* NTP同期実行時刻設定 */
	ret = ETIM_SetSyncTime(pInfo);

	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :NTPサーバー情報取得                                      */
/*----------------------------------------------------------------------------*/
/*    param          :EtimNtpInfo_t *pInfo   NTPサーバー情報                   */
/*                   :schar *Cmd             NTPサーバー同期コマンド            */
/*----------------------------------------------------------------------------*/
/*    return         :sint	処理結果                                           */
/*                    (ETIM_OK(0):正常/                                        */
/*                     ETIM_E_PARAM(-2):引数異常)                              */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_GetNtpServerInfo(EtimNtpInfo_t* pInfo, schar* pCmd)
{
	DEBUG_TRCIN();
	/* 引数確認 */
	if (NULL == pInfo)
	{
		return ETIM_E_PARAM;
	}

	/* NTPサーバー情報排他ロック */
	CMN_MutexLock(&g_etimNtpInfoMutex);

	/* NTPサーバー情報取得 */
	*pInfo = g_etimNtpInfo;

	if (NULL != pCmd)
	{
		/* NTPサーバー同期コマンド取得 */
		strncpy(pCmd, s_etimNtpSyncCmd, sizeof(s_etimNtpSyncCmd));
	}

	/* NTPサーバー情報排他アンロック */
	CMN_MutexUnlock(&g_etimNtpInfoMutex);

	DEBUG_TRCOUT();

	return ETIM_OK;
}

/*============================================================================*/
/*    Description    :NTPサーバー情報書き込み                                   */
/*----------------------------------------------------------------------------*/
/*    param          :const EtimNtpInfo_t*  pInfo   NTPサーバー情報            */
/*----------------------------------------------------------------------------*/
/*    return         : sint	処理結果                                           */
/*                    (ETIM_OK(0):正常/                                        */
/*                    ETIM_E_SYSCALL(-1):システムコールエラー/                  */
/*                     ETIM_E_PARAM(-2):引数異常)                              */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_WriteNtpServerInfo(const EtimNtpInfo_t* pInfo)
{
	FILE *fp;
	size_t size;
	sint ret = ETIM_E_SYSCALL;

	DEBUG_TRCIN();

	/* 引数確認 */
	if (NULL == pInfo)
	{
		return ETIM_E_PARAM;
	}

	/* ファイルオープン */
	fp = CMN_FOPEN(ETIM_NTPINFO_FILEPATH, "w");
	if (NULL != fp)
	{
		/* NTPサーバー情報書き込み */
		size = fwrite(pInfo, sizeof(EtimNtpInfo_t), 1, fp);
		if (1 < size)
		{
			ret = ETIM_OK;
		}
		else
		{
			DLOG_Error("%s write error(%d).\n", ETIM_NTPINFO_FILEPATH, errno);
		}

		/* ファイルクローズ */
		CMN_FCLOSE(fp);
	}
	else
	{
		DLOG_Error("%s open(w) error(%d).\n", ETIM_NTPINFO_FILEPATH, errno);
	}

	DEBUG_TRCOUT();

	return ret;
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
sint ETIM_ReadNtpServerInfo(EtimNtpInfo_t* pInfo)
{
	FILE *fp;
	size_t size;
	sint ret = ETIM_E_SYSCALL;

	/* 引数確認 */
	if (pInfo == NULL)
	{
		return ETIM_E_PARAM;
	}

	/* ファイルオープン */
	fp = CMN_FOPEN(ETIM_NTPINFO_FILEPATH, "r");
	if (fp != NULL)
	{
		/* NTPサーバー情報書き込み */
		size = fread(pInfo, sizeof(EtimNtpInfo_t), 1, fp);
		if (size >= 1)
		{
			ret = ETIM_OK;
		}
		else
		{
			DLOG_Error("%s read error(%d).\n", ETIM_NTPINFO_FILEPATH, errno);
		}
	}
	else
	{
		DLOG_Error("%s open(r) error(%d).\n", ETIM_NTPINFO_FILEPATH, errno);
	}

	/* ファイルクローズ */
	CMN_FCLOSE(fp);

	return ret;
}

/*============================================================================*/
/*    Description    :同期実行時刻設定                                          */
/*----------------------------------------------------------------------------*/
/*    param          :const EtimNtpInfo_t* pInfo    NTPサーバー情報            */
/*----------------------------------------------------------------------------*/
/*    return         : sint	処理結果                                           */
/*                    (ETIM_OK(0):正常/                                        */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー/                 */
/*                     ETIM_E_PARAM(-2):引数異常)                              */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_SetSyncTime(const EtimNtpInfo_t* pInfo)
{
	struct timespec ts;
	double diff;
	sint ret = ETIM_OK;

	DEBUG_TRCIN();

	/* 引数確認 */
	if (NULL == pInfo)
	{
		return ETIM_E_PARAM;
	}

	/* リトライ限界時間を設定するため、現在時刻取得する。時分秒は定時同期の値に更新する */
	ret = ETIM_ClockGetTime(CLOCK_REALTIME, &ts);
	if (ETIM_OK == ret)
	{
		ret = ETIM_LocalTimeR(&ts.tv_sec, &g_etimNtpTime[ETIM_NTPTIME_BASE]);
	}

	if (ETIM_OK != ret)
	{
		ts.tv_sec = 0;
		/* 最小値(1970/01/01 00:00:00)にする */
		memset(&g_etimNtpTime[ETIM_NTPTIME_BASE], 0, sizeof( struct tm ));
		g_etimNtpTime[ETIM_NTPTIME_BASE].tm_year = ETIM_NTP_MIN_SYNC_TIME_YEAR;
		g_etimNtpTime[ETIM_NTPTIME_BASE].tm_mon  = ETIM_NTP_MIN_SYNC_TIME_MON;
		g_etimNtpTime[ETIM_NTPTIME_BASE].tm_mday = ETIM_NTP_MIN_SYNC_TIME_DAY;
#ifdef DEBUG
		DLOG_Error("base time init[19%02d/%02d/%02d %02d:%02d:%02d].\n"
							, g_etimNtpTime[ETIM_NTPTIME_BASE].tm_year
							, g_etimNtpTime[ETIM_NTPTIME_BASE].tm_mon + 1
							, g_etimNtpTime[ETIM_NTPTIME_BASE].tm_mday
							, g_etimNtpTime[ETIM_NTPTIME_BASE].tm_hour
							, g_etimNtpTime[ETIM_NTPTIME_BASE].tm_min
							, g_etimNtpTime[ETIM_NTPTIME_BASE].tm_sec);
#endif
	}

	/* 接続先NTPサーバー確認 */
	if (ETIM_NTP_DEFAULT == pInfo->ntpserver)
	{
		/* デフォルトNTPサーバー */
		ret = ETIM_MakeNtpTime();
	}
	else
	{
		/* デフォルトNTPサーバー以外 */
		/* 時分秒に定時同期開始時刻を設定 */
		g_etimNtpTime[ETIM_NTPTIME_BASE].tm_hour = (sint)pInfo->starSynctime.hour;
		g_etimNtpTime[ETIM_NTPTIME_BASE].tm_min  = (sint)pInfo->starSynctime.min;
		g_etimNtpTime[ETIM_NTPTIME_BASE].tm_sec  = (sint)pInfo->starSynctime.sec;
	}

	/* リトライ限界時刻設定 */
	ETIM_MkTime(&g_etimNtpTime[ETIM_NTPTIME_BASE], &g_etimLimitDate.tv_sec);
	diff = difftime(g_etimLimitDate.tv_sec, ts.tv_sec);
	if (0 >= diff)
	{
		g_etimLimitDate.tv_sec += ETIM_DAYSEC_SZ;
	}
#ifdef DEBUG
		struct tm tm_wk;
		ETIM_LocalTimeR(&g_etimLimitDate.tv_sec, &tm_wk);
		DLOG_Info("retry limit date[%04d/%02d/%02d %02d:%02d:%02d]\n",
					tm_wk.tm_year + 1900,
					tm_wk.tm_mon  + 1,
					tm_wk.tm_mday,
					tm_wk.tm_hour,
					tm_wk.tm_min,
					tm_wk.tm_sec);
#endif

	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :時刻同期リトライ確認                                      */
/*----------------------------------------------------------------------------*/
/*    param          :const EtimNtpInfo_t* pInfo NTPサーバー情報               */
/*                   :const uchar retry          リトライ回数                  */
/*                   :ulong *pInt                NTPサーバー情報               */
/*                   :bool_t *pRet;	             リトライ結果                  */
/*----------------------------------------------------------------------------*/
/*    return         :ssint	処理結果                                           */
/*                    (ETIM_OK(0):正常/                                        */
/*                     ETIM_E_PARAM(-2):引数異常)                              */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_CheckRetry(const EtimNtpInfo_t* pInfo, const ulong retry, ulong* pInt, bool_t* pRet)
{
	sint ret = ETIM_E_PARAM;

	DEBUG_TRCIN();

	/* 引数確認 */
	if ((NULL == pInfo) || (NULL == pInt) || (NULL == pRet)) 
	{
		return ret;
	}

	/* リトライ確認結果TRUE設定 */
	*pRet = TRUE;

	/* リトライ回数閾値1チェック */
	if (ETIM_NTPRTY_INFINITE == pInfo->retryThreshold1)
	{
		*pInt = pInfo->retryInterval1;
		ret = ETIM_OK;
	}
	else if (0 == pInfo->retryThreshold1)	/* リトライ回数チェック1 */
	{
		/* リトライ回数1閾値が0の場合、リトライ無し */
		*pRet = FALSE;
		ret = ETIM_OK;
	}
	else if ((0 != (retry % pInfo->retryThreshold1)) || (0 == retry))
	{
		*pInt = pInfo->retryInterval1;
		ret = ETIM_OK;
	}
	else if (ETIM_NTPRTY_INFINITE == pInfo->retrThreshold2)	/* リトライ回数閾値2チェック */
	{
		*pInt = pInfo->retryInterval2;
		ret = ETIM_OK;
	}
	else if ((retry / pInfo->retryThreshold1) <= (pInfo->retrThreshold2))
	{
		*pInt = pInfo->retryInterval2;
		ret = ETIM_OK;
	}
	else
	{
		/* リトライ確認結果FALSE設定 */
		*pRet = FALSE;
	}

	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :同期状態更新                                             */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void ETIM_UpdateSyncState( void )
{
	FILE *fp;
	EtimNtpSyncState_t syncState;
	size_t size;

	DEBUG_TRCIN();

	/* ファイルオープン */
	fp = CMN_FOPEN(ETIM_SYNCSTATE_FILEPATH, "w");
	if (NULL == fp)
	{
		DLOG_Error("%s open(w) error(%d).\n", ETIM_SYNCSTATE_FILEPATH, errno);
		return;
	}

	/* NTPサーバー情報取得＆書き込み情報作成 */
	ETIM_GetNtpServerInfo(&syncState.ntpInfo, NULL);
	syncState.reryNum = g_etimNtpRetry;
	syncState.state = s_etimSyncState;

	/* NTPサーバー情報書き込み */
	size = fwrite(&syncState, sizeof(syncState), 1, fp);
	if (1 > size)
	{
		DLOG_Error("%s write error(%d).\n", ETIM_SYNCSTATE_FILEPATH, errno);
	}

	DEBUG_TRCIN();

	/* ファイルクローズ */
	CMN_FCLOSE(fp);
}

/*============================================================================*/
/*    Description    :同期状態回復                                             */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void ETIM_RecoverSyncState(void)
{
	FILE *fp;
	EtimNtpSyncState_t syncState;
	EtimNtpInfo_t ntpInfo;
	schar cmd[ETIM_CMDLINE_BUFSIZE];
	size_t len;
	struct timespec ts;
	size_t size;
	bool_t ntpChange = FALSE;
	sint ret;

	/* ファイルオープン */
	fp = CMN_FOPEN(ETIM_SYNCSTATE_FILEPATH, "r");
	if (NULL == fp)
	{
		DLOG_Error("%s open(r) error(%d).\n", ETIM_SYNCSTATE_FILEPATH, errno);
		return;
	}

	/* NTPサーバー情報取得 */
	ret = ETIM_GetNtpServerInfo(&ntpInfo, &cmd[0]);
	len = strnlen(&cmd[0], sizeof(cmd));

	/* 同期状態ファイル読み込み */
	size = fread(&syncState, sizeof(syncState), 1, fp);
	if (1 > size)
	{
		/* 読み込み失敗 */
		DLOG_Error("%s read error(%d).\n", ETIM_SYNCSTATE_FILEPATH, errno);
		g_etimNtpRetry  = 0;
		s_etimSyncState = ETIM_STATE_NTP_SYNC_INIT;
	}
	else
	{
		/* NTPサーバー情報比較 */
		/* 接続するために情報をチェックする */
		ntpChange = ETIM_IsChangeNtpServerInfo(&ntpInfo, &syncState.ntpInfo);
		if (FALSE == ntpChange)
		{
			/* NTPサーバー情報一致(接続先変更無し) */
			g_etimNtpRetry = syncState.reryNum;
			s_etimSyncState = syncState.state;
		}
		else
		{
			/* NTPサーバー情報不一致(接続先変更有り) */
			g_etimNtpRetry  = 0;
			s_etimSyncState = ETIM_STATE_NTP_SYNC_INIT;
		}
	}

	/* リセットフラグチェック */
	/* 瞬低(電源ON動作でramにファイルが残っているケース)を考慮して同期コマンドが作成済みか確認する */
	if ((0 == g_etimNtpReset) && (0 < len))
	{
		ret = ETIM_ClockGetTime(CLOCK_REALTIME, &ts);
		if (ETIM_OK == ret)
		{
			ret = ETIM_LocalTimeR( &ts.tv_sec, &g_etimNtpTime[ETIM_NTPTIME_SCHE] );
		}

		if (ETIM_OK != ret)
		{
			/* 同期予定時刻を最小値(1970/01/01 00:00:00) */
			memset( &g_etimNtpTime[ETIM_NTPTIME_SCHE], 0, sizeof(struct tm) );
			g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_year = ETIM_NTP_MIN_SYNC_TIME_YEAR;
			g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_mon = ETIM_NTP_MIN_SYNC_TIME_MON;
			g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_mday = ETIM_NTP_MIN_SYNC_TIME_DAY;
#ifdef DEBUG
			DLOG_Error("next time init[19%02d/%02d/%02d %02d:%02d:%02d].\n"
								, g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_year
								, g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_mon + 1
								, g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_mday
								, g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_hour
								, g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_min
								, g_etimNtpTime[ETIM_NTPTIME_SCHE].tm_sec );
#endif
		}
	}

	/* ファイルクローズ */
	CMN_FCLOSE(fp);

	return;
}

/*============================================================================*/
/*    Description    :時刻同期状態取得                                         */
/*----------------------------------------------------------------------------*/
/*    param          :EtimNtpState_t    時刻同期状態                           */
/*----------------------------------------------------------------------------*/
/*    return         :EtimNtpState_t 時刻同期状態                              */
/*                    (ETIM_STATE_NTP_SYNC_INIT(0):NTP時刻同期未実施/                */
/*                     ETIM_STATE_NTP_SYNC_OK(1):NTP時刻同期OK/                      */
/*                     ETIM_STATE_NTP_SYNC_NG(2):NTP時刻同期NG)                      */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
EtimNtpState_t ETIM_GetSyncState(void)
{
	EtimNtpState_t state;

	/* NTP時刻同期排他ロック */
	CMN_MutexLock(&g_etimNtpTimeMutx);

	/* NTP時刻同期状態取得 */
	state = s_etimSyncState;

	/* NTP時刻同期排他アンロック */
	CMN_MutexUnlock(&g_etimNtpTimeMutx);

	return state;
}

/*============================================================================*/
/*    Description    :设置NTP同步状态                                          */
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
void ETIM_SetSyncState(EtimNtpState_t syncState)
{
	/* NTP時刻同期排他ロック */
	CMN_MutexLock(&g_etimNtpTimeMutx);

	/* NTP時刻同期状態取得 */
	s_etimSyncState = syncState;

	/* NTP時刻同期排他アンロック */
	CMN_MutexUnlock(&g_etimNtpTimeMutx);
}

/*============================================================================*/
/*    Description    :定期更新時刻取得処理                                      */
/*----------------------------------------------------------------------------*/
/*    param          :time_t *nowTime   リモコンの現在時刻(秒)                  */
/*                   :time_t *nextTime  次回定期更新時刻(秒)                    */
/*----------------------------------------------------------------------------*/
/*    return         :sint	処理結果                                           */
/*                    (ETIM_OK(0):正常/                                        */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー/                 */
/*                     ETIM_E_PARAM(-2):引数異常)                              */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_GetNextTime(const time_t *nowTime, time_t *nextTime)
{
	sint ret = ETIM_E_SYSCALL;
	struct tm time;

	/* 変数初期化 */
	memset(&time, 0x00, sizeof(time));

	/* 引数チェック */
	if ((NULL == nowTime) || (NULL == nextTime))
	{
		DLOG_Error("parameter NULL.\n");
		return ETIM_E_PARAM;
	}

	/* 現在日時をローカルに設定 */
	ret = ETIM_LocalTimeR(nowTime, &time);
	if (ETIM_OK != ret)
	{
		/* 次回定期更新日時に現在年月日+基準時刻を設定 */
		time.tm_hour = g_etimNtpTime[ETIM_NTPTIME_BASE].tm_hour;
		time.tm_min = g_etimNtpTime[ETIM_NTPTIME_BASE].tm_min;
		time.tm_sec = g_etimNtpTime[ETIM_NTPTIME_BASE].tm_sec;

		/* 次回定期更新時刻を秒数に変換 */
		ret = ETIM_MkTime(&time, nextTime);
		if (ETIM_OK == ret)
		{
			/* 時刻判定 */
			if (*nextTime <= *nowTime)
			{
				/* 現在時刻が次回定期更新時刻より未来の場合は翌日に設定(24時間分加算) */
				*nextTime += (time_t)ETIM_DAYSEC_SZ;
				ret = ETIM_OK;
			}
			else
			{
				/* 現在時刻が次回定期更新時刻より過去の場合はそのまま */
				/* Do Nothing. */
			}
		}
		else
		{
			DLOG_Error("ETIM_MkTime() error.\n");
		}
	}
	else
	{
		DLOG_Error("ETIM_LocalTimeR() error.\n");
	}

	return ret;
}

/*============================================================================*/
/*    Description    :NTPサーバー情報変更チェック                               */
/*----------------------------------------------------------------------------*/
/*    param          :time_t *nowTime   リモコンの現在時刻(秒)                  */
/*                   :time_t *nextTime  次回定期更新時刻(秒)                    */
/*----------------------------------------------------------------------------*/
/*    return         :bool_t   変更有無                                        */
/*                   (TRUE:変更有り/FALSE:変更無し)                             */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
bool_t ETIM_IsChangeNtpServerInfo(const EtimNtpInfo_t* before, const EtimNtpInfo_t* after)
{
	sint cmp;
	bool_t ret = FALSE;

	/* 引数がNULLでないことは呼び出し元で保証しているため、NULLチェックはしない */
	/* 接続先サーバーアドレスの比較結果取得 */
	cmp = strncmp(before->addr, after->addr, sizeof(after->addr));

	/* 接続先NTPサーバー */
	if (before->ntpserver != after->ntpserver)
	{
		ret = TRUE;
	}
	else if (ETIM_NTP_DEFAULT == before->ntpserver)	/* デフォルトNTPサーバー確認 */
	{
		ret = FALSE;
	}
	else if (before->portNum != after->portNum)	/* ポート番号 */
	{
		ret = TRUE;
	}
	else if (before->retryThreshold1 != after->retryThreshold1)	/* リトライ回数閾値1 */
	{
		ret = TRUE;
	}
	else if (before->retryInterval1 != after->retryInterval1)	/* リトライ間隔(秒)  */
	{
		ret = TRUE;
	}
	else if (before->retrThreshold2 != after->retrThreshold2)	/* リトライ回数閾値1超過のリトライを繰り返す回数 */
	{
		ret = TRUE;
	}
	else if (before->retryInterval2 != after->retryInterval2)	/* リトライ回数閾値1超過後のリトライ間隔(秒) */
	{
		ret = TRUE;
	}
	else if (before->starSynctime.hour != after->starSynctime.hour)	/* 定時同期開始時刻(時) */
	{
		ret = TRUE;
	}
	else if (before->starSynctime.min != after->starSynctime.min)	/* 定時同期開始時刻(分) */
	{
		ret = TRUE;
	}
	else if (before->starSynctime.sec != after->starSynctime.sec)	/* 定時同期開始時刻(秒) */
	{
		ret = TRUE;
	}
	else if (before->endSynctime.hour != after->endSynctime.hour)	/* 定時同期終了時刻(時) */
	{
		ret = TRUE;
	}
	else if (before->endSynctime.min != after->endSynctime.min)	/* 定時同期終了時刻(分) */
	{
		ret = TRUE;
	}
	else if (before->endSynctime.sec != after->endSynctime.sec)	/* 定時同期終了時刻(秒) */
	{
		ret = TRUE;
	}
	else if (0 != cmp)	/* 接続先サーバーアドレス */
	{
		ret = TRUE;
	}
	else
	{
		/* 全部一致した場合は何もしない */
	}

	return ret;
}

/*----------------------------------------------------------------------------*/
/* 非公開関数定義                                                              */
/*----------------------------------------------------------------------------*/


/* End Of File */
