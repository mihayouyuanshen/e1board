/******************************************************************************/
/*    Description    :時刻管理ミドル（外部API）                                 */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC Gu.CM                                               */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/03/31                                              */
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
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "lfw_g.h"
#include "task_g.h"
#include "etim_g.h"
#include "etim_msg.h"
#include "etim_utl.h"
#include "etim_ntp.h"
#include "dlog_g.h"

/*----------------------------------------------------------------------------*/
/* 公開変数定義                                                                */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開変数定義                                                              */
/*----------------------------------------------------------------------------*/
#ifndef CROSSDEBUG	/* ログ出力抑止 */
static sint s_etimRegfd;
static sint s_etimRegopen = 0;	/* 0:not open, 1:opend, 2:error */
#endif
/*----------------------------------------------------------------------------*/
/* 公開関数定義                                                                */
/*----------------------------------------------------------------------------*/
/*============================================================================*/
/*    Description    :現在時刻取得IF                                           */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :sint   処理結果                                           */
/*                    (ETIM_OK(0):正常/                                       */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー/                 */
/*                     ETIM_E_PARAM(-2):引数異常（ポインタNULL）/               */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_GetTime(EtimTm_t *pTime)
{
	struct timespec tspec;
	struct tm ltm;
	sint ret;

	DEBUG_TRCIN();

	/* 引数確認 */
	if (NULL == pTime)
	{
		DLOG_Error("parameter NULL.\n");
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	/* システム時刻取得 */
	ret = ETIM_ClockGetTime(CLOCK_REALTIME, &tspec);
	if (ETIM_OK == ret)
	{
		/* 年月日時分秒へ変換 */
		ret = ETIM_LocalTimeR(&tspec.tv_sec, &ltm);
		if (ETIM_OK == ret)
		{
			/* 返却データへ設定 */
			pTime->tmYear = ltm.tm_year + 1900;
			pTime->tmMon = ltm.tm_mon + 1;
			pTime->tmMday = ltm.tm_mday;
			pTime->tmMin = ltm.tm_min;
			pTime->tmHour = ltm.tm_hour;
			pTime->tmSec = ltm.tm_sec;
			DLOG_Info("ETIM_GetTime : %d/%d/%d %d:%d:%d\n", pTime->tmYear, pTime->tmMon, pTime->tmMday,
						pTime->tmHour, pTime->tmMin, pTime->tmSec);
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

	/* リターンを返す */
	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :現在時刻（文字列）取得IF                                  */
/*----------------------------------------------------------------------------*/
/*    param          :schar *pTimeStr     取得時刻文字列                       */
/*----------------------------------------------------------------------------*/
/*    return         :sint   処理結果                                          */
/*                    (ETIM_OK(0):正常/                                        */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー/                 */
/*                     ETIM_E_PARAM(-2):引数異常（ポインタNULL）/               */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_GetTimeStr(schar *pTimeStr)
{
	struct timespec tspec;
	struct tm ltm;
	sint ret;

	DEBUG_TRCIN();

	/* 引数確認 */
	if (NULL == pTimeStr)
	{
		DLOG_Error("parameter NULL.\n");
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	/* システム時刻取得 */
	ret = ETIM_ClockGetTime(CLOCK_REALTIME, &tspec);
	if (ETIM_OK == ret)
	{
		/* 年月日時分秒へ変換 */
		ret = ETIM_LocalTimeR(&tspec.tv_sec, &ltm);
		if (ETIM_OK == ret)
		{
			/* 文字列返却データへ設定 */
			ret = (sint)strftime(pTimeStr, ETIM_DATESTR_SIZE, ETIM_TIMEFMT, &ltm);
			if (ret == (ETIM_DATESTR_SIZE - 1))
			{
				ret = ETIM_OK;
			}
			else
			{
				DLOG_Error("strftime() length error(%d).\n", ret);
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

	/* リターンを返す */
	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :現在時刻(1970年起源UTC経過秒)取得IF                       */
/*----------------------------------------------------------------------------*/
/*    param          :NT *pSec     取得時刻（秒）                              */
/*----------------------------------------------------------------------------*/
/*    return         :sint   処理結果                                           */
/*                    (ETIM_OK(0):正常/                                       */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー/                 */
/*                     ETIM_E_PARAM(-2):引数異常（ポインタNULL）/               */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_GetTimeEpoch(sint *pSec)
{
	struct timespec tspec;
	sint ret;

	DEBUG_TRCIN();

	/* 引数確認 */
	if (NULL == pSec)
	{
		DLOG_Error("parameter NULL.\n");
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	/* システム時刻取得 */
	ret = ETIM_ClockGetTime(CLOCK_REALTIME, &tspec);
	if (ETIM_OK == ret)
	{
		/* 返却データへ設定 */
		*pSec = (sint)tspec.tv_sec;
	}
	else
	{
		DLOG_Error("ETIM_ClockGetTime() error.\n");
	}

	/* リターンを返す */
	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :現在時刻(1970年起源UTC経過纳秒)取得IF                     */
/*----------------------------------------------------------------------------*/
/*    param          :NT *pSec     取得時刻（纳秒）                             */
/*----------------------------------------------------------------------------*/
/*    return         :sint   処理結果                                          */
/*                    (ETIM_OK(0):正常/                                       */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー/                 */
/*                     ETIM_E_PARAM(-2):引数異常（ポインタNULL）/               */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Gu.CM : 新規作成                       */
/*============================================================================*/
sint ETIM_GetTimeEpochNsec(slong *pSec)
{
	struct timespec tspec;
	sint ret;

	DEBUG_TRCIN();

	/* 引数確認 */
	if (NULL == pSec)
	{
		DLOG_Error("parameter NULL.\n");
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	/* システム時刻取得 */
	ret = ETIM_ClockGetTime(CLOCK_REALTIME, &tspec);
	if (ETIM_OK == ret)
	{
		/* 返却データへ設定 */
		*pSec = (slong)tspec.tv_nsec;
	}
	else
	{
		DLOG_Error("ETIM_ClockGetTime() error.\n");
	}

	/* リターンを返す */
	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :時刻設定IF                                               */
/*----------------------------------------------------------------------------*/
/*    param          :EtimTm_t *pTime   設定時刻                              */
/*----------------------------------------------------------------------------*/
/*    return         :sint   処理結果                                           */
/*                    (ETIM_OK(0):正常/                                       */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー/                 */
/*                     ETIM_E_PARAM(-2):引数異常（ポインタNULL）/               */
/*                     ETIM_E_OUTVAL(-3):設定値異常（範囲外）/）/               */
/*                     ETIM_E_OTHER(-4):その他エラー)                          */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_SyncSetTime(const EtimTm_t *pTime)
{
	EtimSetTime_t req;
	EtimMsgRspData_t res;
	sint ret;

	DEBUG_TRCIN();

	/* 引数確認 */
	if (NULL == pTime)
	{
		DLOG_Error("parameter NULL.\n");
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	/* 送受信データ初期化 */
	memset(&req, 0x00, sizeof(req));
	memset(&res, 0x00, sizeof(res));

	/* 時刻データ設定 */
	req.time.tm_year = pTime->tmYear - 1900;
	req.time.tm_mon = pTime->tmMon - 1;
	req.time.tm_mday = pTime->tmMday;
	req.time.tm_hour = pTime->tmHour;
	req.time.tm_min = pTime->tmMin;
	req.time.tm_sec = pTime->tmSec;

	/* 範囲確認 */
	ret = ETIM_CheckDateRange((const struct tm *)&req.time);
	if (ETIM_OK == ret)
	{
		req.header.msgId = ETIM_MSGID_SETTIME_REQ;
		/* 同期メッセージ送信 */
		ret = (sint)FRM_CallSync(ETIM_SPORT, sizeof(EtimSetTime_t), (void *)&req, (void *)&res);
	}
	else
	{
		DLOG_Error("ETIM_CheckDateRange() error.\n");
	}

	/* リターンを返す */
	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :時刻設定IF                                               */
/*----------------------------------------------------------------------------*/
/*    param          :schar *pTimeStr       設定時刻文字列                     */
/*----------------------------------------------------------------------------*/
/*    return         :sint   処理結果                                          */
/*                    (ETIM_OK(0):正常/                                       */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー/                 */
/*                     ETIM_E_PARAM(-2):引数異常（ポインタNULL）/               */
/*                     ETIM_E_OUTVAL(-3):設定値異常（範囲外）/）/               */
/*                     ETIM_E_OTHER(-4):その他エラー)                          */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_SyncSetTimeStr(const schar *pTimeStr)
{
	EtimSetTime_t	req;
	EtimMsgRspData_t res;
	sint ret;
	struct tm ltm;

	DEBUG_TRCIN();

	/* 引数確認 */
	if (NULL == pTimeStr)
	{
		DLOG_Error("parameter NULL.\n");
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	/* 文字列を時刻データへ変換 */
	memset(&ltm, 0x00, sizeof(ltm));
	ret = ETIM_StrpTime(pTimeStr, &ltm);
	if (ETIM_OK == ret)
	{
		/* 送受信データ初期化 */
		memset(&req, 0x00, sizeof(req));
		memset(&res, 0x00, sizeof(res));

		/* 時刻データ設定 */
		req.time.tm_year = ltm.tm_year;
		req.time.tm_mon = ltm.tm_mon;
		req.time.tm_mday = ltm.tm_mday;
		req.time.tm_hour = ltm.tm_hour;
		req.time.tm_min = ltm.tm_min;
		req.time.tm_sec = ltm.tm_sec;

		/* 範囲確認 */
		ret = ETIM_CheckDateRange(&req.time);
		if (ETIM_OK == ret)
		{

			req.header.msgId = ETIM_MSGID_SETTIME_REQ;
			/* 同期メッセージ送信 */
			ret = (sint)FRM_CallSync(ETIM_SPORT, sizeof(EtimSetTime_t), (void *)&req, (void *)&res);
		}
		else
		{
			DLOG_Error("ETIM_CheckDateRange() error.\n");
		}
	}
	else
	{
		DLOG_Error("ETIM_StrpTime() error.\n");
	}

	/* リターンを返す */
	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :NTPサーバー疎通確認IF                                     */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :sint   処理結果                                           */
/*                    (ETIM_OK(0):正常/                                       */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー/                 */
/*                     ETIM_E_OTHER(-4):その他エラー)                          */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_SyncPingNtpsvr(void)
{
	EtimMsgRspData_t req;
	EtimMsgRspData_t res;
	sint ret;

	DEBUG_TRCIN();

	/* 送受信データ初期化 */
	memset(&req, 0x00, sizeof(req));
	memset(&res, 0x00, sizeof(res));

	req.header.msgId = ETIM_MSGID_PING_NTP;

	/* 同期メッセージ送信 */
	ret = (sint)FRM_CallSync(ETIM_SPORT, sizeof(EtimMsgRspData_t), &req, &res);

	/* リターンを返す */
	DEBUG_TRCOUT();

	return ret;
}
/*============================================================================*/
/*    Description    :マイクロ秒のハードウェアカウンタ読み出し                    */
/*----------------------------------------------------------------------------*/
/*    param          :ulong *res     読み出し値                                   */
/*----------------------------------------------------------------------------*/
/*    return         :sint   処理結果                                           */
/*                    (ETIM_OK(0):正常/                                       */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー)                 */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_ReadHwCount(ulong *res)
{
	schar readHwFlag = ETIM_FLAG_ERR;
	sint ret = ETIM_OK;

	DEBUG_TRCIN();

#ifndef CROSSDEBUG
	ulong buf[ETIM_HW_BUF_SIZE];

	if (0 == s_etimRegopen)
	{
		s_etimRegfd = open(ETIM_BWREG_DEV, O_RDONLY | O_SYNC);
		if (-1 != s_etimRegfd)
		{
			s_etimRegopen = 1;
			readHwFlag = ETIM_FLAG_OK;
			DLOG_Info("bwreg open device.\n");
		}
		else
		{
			s_etimRegopen = 2;
			DLOG_Error("bwreg open error.\n");
		}
	}
	else if (1 == s_etimRegopen)
	{
		readHwFlag = ETIM_FLAG_OK;
	}

	if(ETIM_FLAG_OK == readHwFlag)
	{
		buf[0] = ETIM_BWREG_HW_DIGCTL_MICROSECONDS;
		buf[1] = (uint)tolower('w');
		if (0 != ioctl(s_etimRegfd, ETIM_BWREG_IOCREAD, buf))
		{
			*res = (ulong)buf[2];
		}
		else
		{
			DLOG_Error("bwreg ioctl read error.\n");
			ret = ETIM_E_SYSCALL;
		}
	}

#endif

	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :NTPサーバーとの時刻同期を開始する                          */
/*----------------------------------------------------------------------------*/
/*    param          :EtimNtpInfo_t *ntpInfo  NTPサーバー情報                */
/*----------------------------------------------------------------------------*/
/*    return         :sint   処理結果                                           */
/*                    (ETIM_OK(0):正常/                                       */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー/                 */
/*                     ETIM_E_PARAM(-2)	引数異常)                              */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_AsyncStartSyncNtpsvr(EtimNtpInfo_t* ntpInfo)
{
	EtimStartNtpSync_t req;
	size_t len;
	sint ret = ETIM_E_PARAM;
	schar syncNtpFlag = ETIM_FLAG_ERR;

	DEBUG_TRCIN();

	/* 引数確認 */
	if (NULL == ntpInfo)
	{
		DLOG_Error( "parameter NULL.\n" );
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	/* 引数妥当値確認(接続先NTPサーバー) */
	switch(ntpInfo->ntpserver)
	{
	case ETIM_NTP_DEFAULT:
		/* 他のメンバはdon't careのため、何もしない */
		break;
	case ETIM_NTP_SPECIFY:
		/* 引数妥当値確認(ポート番号) */
		if (0 != ntpInfo->portNum)
		{
			/* 引数妥当値確認(定時同期開始時刻) */
			if ((ETIM_TIME_LIMIT_HOUR > ntpInfo->starSynctime.hour) ||
				(ETIM_TIME_LIMIT_MIN > ntpInfo->starSynctime.min) ||
				(ETIM_TIME_LIMIT_SEC > ntpInfo->starSynctime.sec))
			{
				/* 引数妥当値確認(定時同期終了時刻) */
				if ((ntpInfo->endSynctime.hour < ETIM_TIME_LIMIT_HOUR) ||
					(ntpInfo->endSynctime.min < ETIM_TIME_LIMIT_MIN) ||
					(ntpInfo->endSynctime.sec < ETIM_TIME_LIMIT_SEC))
				{
					/* 引数妥当値確認(接続先サーバーアドレス) */
					len = strnlen(ntpInfo->addr, sizeof(ntpInfo->addr));
					if (len < sizeof(ntpInfo->addr))
					{
						syncNtpFlag = ETIM_FLAG_OK;
					}
					else
					{
						DLOG_Error( "ntp server address error(len = %ld).\n", len );
					}
				}
				else
				{
					DLOG_Error("endSynctime error(%02d:%02d:%02d).\n", ntpInfo->endSynctime.hour,
								ntpInfo->endSynctime.min, ntpInfo->endSynctime.sec);
				}
			}
			else
			{
				DLOG_Error("starSynctime error(%02d:%02d:%02d).\n", ntpInfo->starSynctime.hour,
							ntpInfo->starSynctime.min, ntpInfo->starSynctime.sec);
			}
		}
		else
		{
			DLOG_Error( "portNum error.\n" );
		}
		break;
	default:
		DLOG_Error("ntpserver error(%ld).\n", ntpInfo->ntpserver);
		break;
	}

	if(ETIM_FLAG_OK == syncNtpFlag)
	{
		/* メッセージヘッダ設定 */
		req.header.msgId = ETIM_MSGID_STARTSYNC_REQ;

		/* メッセージデータ設定 */
		req.ntpInfo = *ntpInfo;

		/* 非同期メッセージ送信 */
		ret = FRM_MsgSendAsync(ETIM_SPORT, sizeof(EtimStartNtpSync_t), (void*)&req);

		/* 処理結果確認 */
		if (0 == ret)
		{
			ret = ETIM_OK;
		}
		else
		{
			DLOG_Error( "lfw_ASyncCallWithoutWait() error.\n" );
			ret = ETIM_E_SYSCALL;
		}
	}

	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :NTP時刻同期状態取得                                      */
/*----------------------------------------------------------------------------*/
/*    param          :EtimNtpState_t* state   NTP時刻同期状態                 */
/*----------------------------------------------------------------------------*/
/*    return         :sint   処理結果                                          */
/*                    (ETIM_OK(0):正常/                                       */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー/                 */
/*                     ETIM_E_PARAM(-2)	引数異常)                              */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_SyncGetSyncState(EtimNtpState_t *state)
{
	sint ret = ETIM_E_SYSCALL;				/* 戻り値 */

	DEBUG_TRCIN();

	/* 引数確認 */
	if (NULL == state)
	{
		DLOG_Error( "parameter NULL.\n" );
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	*state = ETIM_GetSyncState();

	ret = ETIM_OK;

	/* リターンを返す */
	DEBUG_TRCOUT();

	return ret;
}
/*----------------------------------------------------------------------------*/
/* 非公開関数定義                                                              */
/*----------------------------------------------------------------------------*/


/* End Of File */



