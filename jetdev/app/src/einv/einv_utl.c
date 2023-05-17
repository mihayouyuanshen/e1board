/****************************************************************************/
/*	Description		:INV通信ミドル ユーティリティ関数						*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Dai.P												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/23												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:INV通信												*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

#include "typedef.h"
#include "task_g.h"
#include "common_g.h"
#include "einv_g.h"
#include "frm_g.h"
#include "dlog_g.h"

#include "einv_msg_g.h"
#include "einv_ram_g.h"
#include "einv_utl_g.h"
#include "einv_utl.h"

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
/*	Description	:経過時間計算												*/
/*--------------------------------------------------------------------------*/
/*	param		:base	:基準時刻ポインタ									*/
/*				:now	:現在時刻格納ポインタ								*/
/*--------------------------------------------------------------------------*/
/*	return		:result	処理結果											*/
/*				:		0以上	正常、経過時間（0～2147483647 [10msec単位]）*/
/*				:		EINV_ERR_SYSCALL(-1)	システムコールエラー		*/
/*				:		EINV_ERR_PARAM(-4)		パラメータエラー			*/
/*--------------------------------------------------------------------------*/
/*	detail		:経過時間計算処理を行う。									*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/30 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_PassTime( struct timespec *base, struct timespec *now)
{
	slong ret = EINV_OK;
	slong baseR = 0L;
	slong nowR = 0L;
	slong result = 0L;

	DEBUG_TRCIN();

	if ( now == NULL )
	{
		DLOG_Error("now parameter is NULL\n");
		DEBUG_TRCOUT();
		return EINV_ERR_PARAM;
	}

	ret = einvClockGettime(CLOCK_MONOTONIC_RAW, now);
	if ( ret != EINV_OK ) 
	{
		DLOG_Error("now einv_clock_gettime() error.\n");
		result = EINV_ERR_SYSCALL;
	}
	else
	{
		if ( base == NULL )
		{
			/* 処理無し */
		} 
		else 
		{
			baseR = base->tv_sec * 100;					/* sec -> 10msec */
			baseR = baseR + base->tv_nsec / 10000000;	/* nsec -> 10msec */
			nowR  = now->tv_sec * 100;					/* sec -> 10msec */
			nowR  = nowR  + now->tv_nsec / 10000000;	/* nsec -> 10msec */

			/* signed の値どうしを比較する事で、オーバーフローさせて必ず正となる。*/
			result = nowR - baseR;
			if ( result < 0 )
			{
				DLOG_Error("base > now. illegal arguments.\n");
				result = EINV_ERR_PARAM;
			}
			else
			{
				/* 処理無し */
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description	:usleep関数ラッパー											*/
/*--------------------------------------------------------------------------*/
/*	param		:usec	:スリープ時間[usec](1～999999)						*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	処理結果											*/
/*				:		EINV_OK(0)				正常						*/
/*				:		EINV_ERR_SYSCALL(-1)	システムコールエラー		*/
/*				:		EINV_ERR_PARAM(-4)		パラメータエラー			*/
/*--------------------------------------------------------------------------*/
/*	detail		:usleep関数ラッパー処理を行う。								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/30 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_Usleep( slong usec )
{
	slong ret = EINV_OK;
	struct timespec	req;

	DEBUG_TRCIN();

	/* 引数チェック */
	if ( usec <= 0 || usec > 999999 )
	{
		DLOG_Error("parameter out of range.\n");
		ret = EINV_ERR_PARAM;
	} else {
		memset(&req, 0x0, sizeof(req));
		req.tv_nsec = usec * 1000;

		ret = einv_Nanosleep( &req );
		if ( ret != EINV_OK )
		{
			ret = EINV_ERR_SYSCALL;
		}
		else
		{
			/* 処理無し */
		}
	}

	DEBUG_TRCOUT();

	return ret;
}


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description	:システム時刻取得ラッパー									*/
/*--------------------------------------------------------------------------*/
/*	param		:clk_id	:クロック種別										*/
/*				:tp		:設定時刻											*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	処理結果											*/
/*				:		EINV_OK(0)				正常						*/
/*				:		EINV_ERR_SYSCALL(-1)	システムコールエラー		*/
/*				:		EINV_ERR_PARAM(-4)		パラメータエラー			*/
/*--------------------------------------------------------------------------*/
/*	detail		:PUART制御 初期化処理を行う。								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/30 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
static slong einvClockGettime( clockid_t clk_id, struct timespec *tp )
{
	slong	ret = EINV_OK;
	sint	apiRet = 0;
	schar	ebuf[256] = { 0 };

	DEBUG_TRCIN();

	/* 引数チェック */
	if ( tp == NULL ) 
	{
		DLOG_Error("parameter is NULL.\n");
		DEBUG_TRCOUT();
		return EINV_ERR_PARAM;
	}

	/* システム時刻取得 */
	apiRet = clock_gettime( clk_id, tp );
	if ( apiRet != 0 ) {
		memset( ebuf, 0x00, sizeof(ebuf) );
		strerror_r( errno, ebuf, 256 );
		DLOG_Error("clock_gettime() error(%d,%s).\n", errno, ebuf);
		ret = EINV_ERR_SYSCALL;
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:nanosleep関数ラッパー										*/
/*--------------------------------------------------------------------------*/
/*	param		:arg	:設定時刻											*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	処理結果											*/
/*				:		EINV_OK(0)				正常						*/
/*				:		EINV_ERR_PARAM(-4)		パラメータエラー			*/
/*--------------------------------------------------------------------------*/
/*	detail		:nanosleep関数ラッパー処理を行う。							*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/30 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
static slong einv_Nanosleep( struct timespec *arg )
{
	sint	ret = 0;
	schar	ebuf[256] = { 0 };
	struct timespec	req;
	struct timespec	rem;
	bool_t breakFlag = FALSE;

	DEBUG_TRCIN();

	/* 引数チェック */
	if ( arg == NULL )
	{
		DLOG_Error("parameter is NULL.\n");
		DEBUG_TRCOUT();
		return EINV_ERR_PARAM;
	}

	memcpy(&req, arg, sizeof(req));
	memset(&rem, 0x0, sizeof(rem));

	while(!breakFlag)
	{
		ret = nanosleep( &req, &rem );
		if ( ret == 0 )
		{
			/* nanosleep()が正常終了 */
			breakFlag = TRUE;
		}
		else
		{
			memset( ebuf, 0x00, sizeof(ebuf) );
			strerror_r( errno, ebuf, 256 );
			if ( errno == EINTR )
			{
				/* nanosleep()がシグナルで停止したので残り時間分再sleep */
				req.tv_sec	= rem.tv_sec;
				req.tv_nsec = rem.tv_nsec;
				DLOG_Error("nanosleep() stop by sig.(%d,%s) -> continue.\n", errno, ebuf);
			} 
			else
			{
				/* nanosleep()がエラー */
				DLOG_Error("nanosleep() error.(%d,%s)\n", errno, ebuf);
				breakFlag = TRUE;
			}
		}
	}

	DEBUG_TRCOUT();

	return EINV_OK;
}

/*==========================================================================*/
/*	Description	:メモリダンプ												*/
/*--------------------------------------------------------------------------*/
/*	param		:buf	:ダンプポインタ										*/
/*				:size	:ダンプサイズ										*/
/*--------------------------------------------------------------------------*/
/*	return		:なし														*/
/*--------------------------------------------------------------------------*/
/*	detail		:メモリダンプ処理を行う。									*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/30 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
#ifdef DEBUG_DUMP
static schar epcs_toascii(sint x)
{
	schar ret = '.';

	DEBUG_TRCIN();

	if ( x >= 0x00 && x <= 0x09 )
	{
		ret = ( 0x30 + (x) );
	} 
	else if ( x >= 0x0a && x <= 0x0f )
	{
		ret = ( 0x61 + (x) - 0x0a );
	}
	else
	{
		ret = '.';
	}

	DEBUG_TRCOUT();

	return ret;
}
#endif
void EINV_Dump( void *buf, ulong size )
{
#ifdef DEBUG_DUMP
	ulong	i = 0L;
	ulong	j = 0L;
	schar	tmp[37];
	schar	str[17];
	schar	*p;

	DEBUG_TRCIN();

	memset(tmp, 0x20, sizeof(tmp));
	memset(str, 0x20, sizeof(str));
	tmp[34] = ':';
	tmp[36] = '\0';
	str[16] = '\0';
	p = buf;

	DLOG_Debug("[Dump:%p, %ld]\n", buf, size);
	DLOG_Debug("-<HEX>---------- ---------------- - -<ASCII>--------\n");
	for (i=0, j=0; i<size; i++)
	{
		tmp[j++] = epcs_toascii((sint)((*p & 0xf0) >> 4));
		tmp[j++] = epcs_toascii((sint)(*p & 0x0f));
		if ( ((i+1) % 8) == 0 )
		{
			tmp[j++] = ' ';
		}
		else
		{
			/* 処理無し */
		}
		if ( isalpha((sint)(*p)) || isdigit((sint)(*p)) )
		{
			str[i%16] = *p;
		}
		else
		{
			str[i%16] = '.';
		}
		if ( ((i+1) % 16) == 0 )
		{
			DLOG_Debug("%s%s\n", tmp, str);
			memset(tmp, 0x20, sizeof(tmp));
			memset(str, 0x20, sizeof(str));
			tmp[34] = ':';
			tmp[36] = '\0';
			str[16] = '\0';
			j = 0;
		}
		else
		{
			/* 処理無し */
		}
		p++;
	}
	if ( j != 0 )
	{
		DLOG_Debug("%s%s\n", tmp, str);
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();
#endif
}


/*End Of File*/
