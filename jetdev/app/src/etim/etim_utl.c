/******************************************************************************/
/*    Description    :時刻管理ミドル                                           */
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <ctype.h>

#include "lfw_g.h"
#include "task_g.h"
#include "etim_g.h"
#include "dlog_g.h"

#define ETIM_NODBG_PRINT	/* ログ出力抑止 */
#include "etim_utl.h"
#include "etim_msg.h"


/*----------------------------------------------------------------------------*/
/* 公開変数定義                                                                */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開変数定義                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* 公開関数定義                                                                */
/*----------------------------------------------------------------------------*/
/*============================================================================*/
/*    Description    :日時文字列を時刻データへ変換                               */
/*----------------------------------------------------------------------------*/
/*    param          :schar *dateStr    日時文字列                             */
/*                   :struct tm *rtm    日時データ                             */
/*----------------------------------------------------------------------------*/
/*    return         :sint	処理結果                                           */
/*                    (ETIM_OK(0):正常/                                       */
/*                     ETIM_E_PARAM(-2):引数異常（ポインタNULL）/               */
/*                     ETIM_E_OUTVAL(-3):設定値異常（範囲外))                   */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/03  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_StrpTime(const schar *dateStr, struct tm *rtm)
{
	schar str[ETIM_DATESTR_SIZE];
	schar *pstr;
	schar *savep;
	sint ret = ETIM_E_SYSCALL;
	size_t len;

	DEBUG_TRCIN();

	/* 引数確認 */
	if (NULL == dateStr)
	{
		DLOG_Error("dateStr is NULL.\n");
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	if (NULL == rtm)
	{
		DLOG_Error("rtm is NULL.\n");
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	/* 日時文字列長確認 */
	len = strnlen(dateStr, ETIM_DATESTR_SIZE);
	if (len == (ETIM_DATESTR_SIZE - 1))
	{
			/* 作業バッファへコピー */
		memset(str, 0x00, sizeof(str));
		strncpy(str, dateStr, ETIM_DATESTR_SIZE - 1);

		/* フォーマット確認 */
		pstr = str;
		if (0 == isdigit((sint)*pstr++)) /* 年 */
		{
			ret = ETIM_E_OUTVAL;
		}
		else if (0 == isdigit((sint)*pstr++)) /* 年 */
		{
			ret = ETIM_E_OUTVAL;
		}
		else if (0 == isdigit((sint)*pstr++)) /* 年 */
		{
			ret = ETIM_E_OUTVAL;
		}
		else if (0 == isdigit((sint)*pstr++)) /* 年 */
		{
			ret = ETIM_E_OUTVAL;
		}
		else if ('/' != *pstr++)
		{
			ret = ETIM_E_OUTVAL;
		}
		else if (0 == isdigit((sint)*pstr++))/* 月 */
		{
			ret = ETIM_E_OUTVAL; 
		}
		else if (0 == isdigit((sint)*pstr++))/* 月 */
		{
			ret = ETIM_E_OUTVAL; 
		}
		else if ('/' != *pstr++)
		{
			ret = ETIM_E_OUTVAL;
		}
		else if (0 == isdigit((sint)*pstr++))/* 日 */
		{
			ret = ETIM_E_OUTVAL; 
		}
		else if (0 == isdigit((sint)*pstr++))/* 日 */
		{
			ret = ETIM_E_OUTVAL; 
		}
		else if (' ' != *pstr++)
		{
			ret = ETIM_E_OUTVAL;
		}
		else if (0 == isdigit((sint)*pstr++)) /* 時 */
		{
			ret = ETIM_E_OUTVAL;
		}
		else if (0 == isdigit((sint)*pstr++)) /* 時 */
		{
			ret = ETIM_E_OUTVAL;
		}
		else if (':' != *pstr++)
		{
			ret = ETIM_E_OUTVAL;
		}
		else if (0 == isdigit((sint)*pstr++)) /* 分 */
		{
			ret = ETIM_E_OUTVAL;
		}
		else if (0 == isdigit((sint)*pstr++)) /* 分 */
		{
			ret = ETIM_E_OUTVAL;
		}
		else if (':' != *pstr++)
		{
			ret = ETIM_E_OUTVAL;
		}
		else if (isdigit(0 == (sint)*pstr++)) /* 秒 */
		{
			ret = ETIM_E_OUTVAL;
		}
		else if (0 == isdigit((sint)*pstr++)) /* 秒 */
		{
			ret = ETIM_E_OUTVAL;
		}

		if (ret == ETIM_E_OUTVAL)
		{
			DLOG_Error("parameter date string format error.\n");
			ret = ETIM_E_OUTVAL;
		}
		else
		{
			/* 値設定 */
			/* 年 */
			pstr = str;
			rtm->tm_year = (sint)strtol(pstr, &savep, 10);
			rtm->tm_year -= 1900;
			/* 月 */
			pstr = savep + 1;	/* '/' skip */
			rtm->tm_mon = (sint)strtol(pstr, &savep, 10);
			rtm->tm_mon -= 1;
			/* 日 */
			pstr = savep + 1;	/* '/' skip */
			rtm->tm_mday = (sint)strtol(pstr, &savep, 10);
			/* 時 */
			pstr = savep + 1;	/* ' ' skip */
			rtm->tm_hour = (sint)strtol(pstr, &savep, 10);
			/* 分 */
			pstr = savep + 1;	/* ':' skip */
			rtm->tm_min = (sint)strtol(pstr, &savep, 10);
			/* 秒 */
			pstr = savep + 1;	/* ':' skip */
			rtm->tm_sec = (sint)strtol(pstr, &savep, 10);

			/* 範囲チェック */
			ret = ETIM_CheckDateRange(rtm);
			if (ret == ETIM_OK)
			{
				/* do nothing */
			}
			else
			{
				DLOG_Error("ETIM_CheckDateRange() error.\n");
			}
		}
	}
	else
	{
		DLOG_Error("parameter length error.(%d)\n", len);
		ret = ETIM_E_OUTVAL;
	}

	/* リターンを返す */
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :システム時刻設定ラッパー                                  */
/*----------------------------------------------------------------------------*/
/*    param          :clockid_t clkId             クロック種別                 */
/*                   :const struct timespec *tp   設定時刻                     */
/*----------------------------------------------------------------------------*/
/*    return         :sint	処理結果                                           */
/*                    (ETIM_OK(0):正常/ETIM_E_SYSCALL(-1):システムコールエラー) */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/03  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_ClockSetTime(clockid_t clkId, const struct timespec *tp)
{
	sint ret = ETIM_E_SYSCALL;
	schar ebuf[ETIM_ERR_BUF_SIZE];

	DEBUG_TRCIN();

	/* 引数チェック */
	if (NULL == tp)
	{
		DLOG_Error("parameter is NULL.\n");
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	/* システム時刻設定 */
#ifndef DONTSETTIME
	ret = clock_settime(clkId, tp);
#else
	ret = 0;
#endif
	if (0 != ret)
	{
		memset(ebuf, 0x00, sizeof(ebuf));
		strerror_r(errno, ebuf, ETIM_ERR_BUF_SIZE);
		DLOG_Error("clock_settime() error(%s).\n", ebuf);
	}
	else
	{
		ret = ETIM_OK;
	}

	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :システム時刻取得ラッパー                                  */
/*----------------------------------------------------------------------------*/
/*    param          :clockid_t clkId            クロック種別                 */
/*                   :const struct timespec *tp   設定時刻                     */
/*----------------------------------------------------------------------------*/
/*    return         :sint	処理結果                                           */
/*                    (ETIM_OK(0):正常/ETIM_E_SYSCALL(-1):システムコールエラー) */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/03  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_ClockGetTime(clockid_t clkId, struct timespec *tp)
{
	sint ret = ETIM_E_SYSCALL;
	schar ebuf[ETIM_ERR_BUF_SIZE];

	DEBUG_TRCIN();

	/* 引数チェック */
	if (NULL == tp)
	{
		DLOG_Error("parameter is NULL.\n");
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	/* システム時刻取得 */
	ret = clock_gettime(clkId, tp);
	if (0 != ret)
	{
		memset(ebuf, 0x00, sizeof(ebuf));
		strerror_r(errno, ebuf, ETIM_ERR_BUF_SIZE);
		DLOG_Error("clock_gettime() error(%s).\n", ebuf);
	}
	else
	{
		ret = ETIM_OK;
	}

	DEBUG_TRCOUT();
	return ret;
}
/*============================================================================*/
/*    Description    :ローカル時刻変換ラッパー                                  */
/*----------------------------------------------------------------------------*/
/*    param          :const struct tm *timep      UTC時刻（秒）                */
/*                   :const time_t *result        変換時刻                     */
/*----------------------------------------------------------------------------*/
/*    return         :sint	処理結果                                           */
/*                    (ETIM_OK(0):正常/ETIM_E_SYSCALL(-1):システムコールエラー) */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/03  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_LocalTimeR(const time_t *timep, struct tm *result)
{
	struct tm *rtm;
	schar ebuf[ETIM_ERR_BUF_SIZE];
	sint ret = ETIM_E_SYSCALL;

	DEBUG_TRCIN();

	/* 引数チェック */
	if (NULL == timep)
	{
		DLOG_Error("timep is NULL.\n");
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}
	if (NULL == result)
	{
		DLOG_Error("result is NULL.\n");
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	rtm = localtime_r(timep, result);
	if (NULL == rtm)
	{
		memset(ebuf, 0x00, sizeof(ebuf));
		strerror_r( errno, ebuf, ETIM_ERR_BUF_SIZE);
		DLOG_Error("localtime_r() error(%s).\n", ebuf);
	}
	else
	{
		ret = ETIM_OK;
	}

	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :時刻->秒数変換ラッパー                                    */
/*----------------------------------------------------------------------------*/
/*    param          :const struct tm *ptm        時刻                        */
/*                   :const time_t *pt            秒                          */
/*----------------------------------------------------------------------------*/
/*    return         :sint	処理結果                                           */
/*                    (ETIM_OK(0):正常/ETIM_E_SYSCALL(-1):システムコールエラー) */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/03  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_MkTime( struct tm *ptm, time_t *pt )
{
	time_t t;
	schar ebuf[ETIM_ERR_BUF_SIZE];
	sint ret = ETIM_E_SYSCALL;

	DEBUG_TRCIN();

	/* 引数チェック */
	if (NULL == ptm)
	{
		DLOG_Error("ptm is NULL.\n");
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}
	if (NULL == pt)
	{
		DLOG_Error("pt is NULL.\n");
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	t = mktime(ptm);
	if (-1 == t)
	{
		memset(ebuf, 0x00, sizeof(ebuf));
		strerror_r(errno, ebuf, ETIM_ERR_BUF_SIZE);
		DLOG_Error("mktime() error(%s).\n", ebuf);
	}
	else
	{
		*pt = t;
		ret = ETIM_OK;
	}

	DEBUG_TRCOUT();

	return ret;
}

/*============================================================================*/
/*    Description    :日時文字列を時刻データへ変換                               */
/*----------------------------------------------------------------------------*/
/*    param          :struct tm *ptm    日時データ                             */
/*----------------------------------------------------------------------------*/
/*    return         :sint	処理結果                                           */
/*                    (ETIM_OK(0):正常/                                       */
/*                     ETIM_E_PARAM(-2):引数異常（ポインタNULL）/               */
/*                     ETIM_E_OUTVAL(-3):設定値異常（範囲外))                   */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/03  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_CheckDateRange(const struct tm *ptm)
{
	sint ret = ETIM_E_OUTVAL;

	DEBUG_TRCIN();

	/* 引数チェック */
	if (NULL == ptm)
	{
		DLOG_Error("parameter NULL.\n");
		DEBUG_TRCOUT();
		return ETIM_E_PARAM;
	}

	/* 年 */
	if ((ptm->tm_year < 70) || (138 < ptm->tm_year))
	{
		DLOG_Error("year out of range.(%d)\n", ptm->tm_year);
	}
	else if ((ptm->tm_mon < 0) || (11 < ptm->tm_mon))	/* 月 */
	{
		DLOG_Error("month out of range.(%d)\n", ptm->tm_mon);
	}
	else if ((ptm->tm_mday < 1) || (31 < ptm->tm_mday))	/* 日 */
	{
		DLOG_Error("day out of range.(%d)\n", ptm->tm_mday);
	}
	else if ((ptm->tm_hour < 0) || (23 < ptm->tm_hour))	/* 時 */
	{
		DLOG_Error("hour out of range.(%d)\n", ptm->tm_hour);
	}
	else if ((ptm->tm_min < 0) || (59 < ptm->tm_min))	/* 分 */
	{
		DLOG_Error("min out of range.(%d)\n", ptm->tm_min);
	}
	else if ((ptm->tm_sec < 0) || (60 < ptm->tm_sec))	/* 秒 */
	{
		DLOG_Error("sec out of range.(%d)\n", ptm->tm_sec);
	}
	else if (138 == ptm->tm_year)	/* 2038/1/1まで許容 */
	{
		if ((0 < ptm->tm_mon) || (1 < ptm->tm_mday))
		{
			DLOG_Error("over 2038/01/01 23:59:59. out of range.\n");
		}
	}
	else if ((70 == ptm->tm_year) && (0 == ptm->tm_mon) &&
		(1 == ptm->tm_mday) && (9 > ptm->tm_hour))	/* 日本時間で 1970/01/01 09:00:00 以前は不可 */
	{
		DLOG_Error("under 1970/01/01 09:00:00. out of range.\n");
	}
	else
	{
		ret = ETIM_OK;
	}

	DEBUG_TRCOUT();

	return ret;
}
/*----------------------------------------------------------------------------*/
/* 非公開関数定義                                                              */
/*----------------------------------------------------------------------------*/

/* End Of File */

