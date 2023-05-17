/******************************************************************************/
/*    Description    :                                                        */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC Jiang.Z                                             */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/05/04                                              */
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
#include "elog_g.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "typedef.h"
#include "common_g.h"
#include "dlog_g.h"
#include "etest_shell_g.h"
/*----------------------------------------------------------------------------*/
/* 公開変数定義                                                                */
/*----------------------------------------------------------------------------*/
#define ELOG_ERR_DATA					((ulong)4096u)
#define ELOG_TEST_INFOR_FILES_LIM		((uchar)100u)
/*----------------------------------------------------------------------------*/
/* 非公開変数定義                                                              */
/*----------------------------------------------------------------------------*/

static void elogSetErrCodeTest(slong argc, schar *agrv[]);
static void elogDeleteDataTest(slong argc, schar *agrv[]);
static void elogTimestampTest(slong argc, schar *agrv[]);
static void elogSetSomeErrCodeTest(slong argc, schar *agrv[]);
static void elog_OutputDlogTest(slong argc, schar *argv[]);
static void elog_PrintDlogTest(slong argc, schar *argv[]);

ETEST_SubCmdItem_t g_ETest_ElogSubCmdTbl[] = {
	{"adderr", "simulate generate error, usage: adderr type errcode errrating", elogSetErrCodeTest},
	{"delete", "delete old data, usage: delete type line", elogDeleteDataTest},
	{"time", "compare time, usage: time YYYY/MM/DD/HH/MM/SS", elogTimestampTest},
	{"adderrs", "simulate generate some error, usage: adderr type number errcode errrating", elogSetSomeErrCodeTest},
	{"outputdlog", "outputdlog, usage: outputdlog", elog_OutputDlogTest},
	{"printlog", "print log, usage: printlog num", elog_PrintDlogTest},
};

ETEST_CmdItem_t g_ETest_ElogCmdItem = {
	"elog",
	"elog",
	g_ETest_ElogSubCmdTbl,
	ARRAY_SIZE(g_ETest_ElogSubCmdTbl)
};
/*----------------------------------------------------------------------------*/
/* 公開関数定義                                                                */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開関数定義                                                              */
/*----------------------------------------------------------------------------*/


/*============================================================================*/
/*    Description    :添加错误码                                               */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/05/09  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static void elogSetErrCodeTest(slong argc, schar *argv[])
{
	if (4 != argc)
	{
		printf("usage: adderr type errcode errrating\n");
		return;
	}

	slong type;

	type = atoi(argv[1]);
	if (type >= ELOG_DIFF_MAX)
	{
		printf("type should less than %d\n", ELOG_DIFF_MAX);
		return;
	}

	ELOG_ErrEventLog(type, argv[2], *argv[3]);
}
/*============================================================================*/
/*    Description    :添加多个错误码                                           */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/05/09  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static void elogSetSomeErrCodeTest(slong argc, schar *argv[])
{
	if (5 != argc)
	{
		printf("usage: adderr type number errcode errrating\n");
		return;
	}

	slong type;
	ulong num;
	schar errLevel;
	ulong i = 0;
	type = atoi(argv[1]);
	if (type >= ELOG_DIFF_MAX)
	{
		printf("type should less than %d\n", ELOG_DIFF_MAX);
		return;
	}
	num = atoi(argv[2]);
	while(i < num)
	{
		ELOG_ErrEventLog(type, argv[3], *argv[4]);
		i++;
	}
}
/*============================================================================*/
/*    Description    :删除指定行数                                             */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/05/09  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static void elogDeleteDataTest(slong argc, schar *argv[])
{
	if (3 != argc)
	{
		printf("usage: delete type line\n");
		return;
	}

	slong type;
	slong line;
	type = atoi(argv[1]);
	if (type > 0)
	{
		printf("type should less than 0\n");
		return;
	}
	line = atoi(argv[2]);
	if (line > ELOG_TEST_INFOR_FILES_LIM)
	{
		printf("line should less than %d\n", ELOG_TEST_INFOR_FILES_LIM);
		return;
	}

	ELOG_AsyncDeleteFile(type, line);
}
/*============================================================================*/
/*    Description    :根据时间查找文件内容                                      */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/05/09  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static void elogTimestampTest(slong argc, schar *argv[])
{
	if (2 != argc)
	{
		printf("usage: time YYYY/MM/DD/HH/MM/SS\n");
		return;
	}

	void *errData = NULL;
	uchar line = 0;
	struct tm* tmpTime = CMN_MALLOC(sizeof(struct tm));
	strptime(argv[1], "%Y/%m/%d/%H/%M/%S", tmpTime);
	tmpTime->tm_isdst = 0;
	time_t timestamp = mktime(tmpTime);
	errData = CMN_MALLOC(ELOG_ERR_DATA);
	memset(errData, 0x00, ELOG_ERR_DATA);
	elog_GetFileLogData(timestamp, errData, &line);
	printf("errData:\n%sline:%d\n", errData, line);
	fflush(stdout);
	CMN_FREE(errData);
	CMN_FREE(tmpTime);
}

/* 時刻設定・取得データ */
typedef struct {
	sint tmSec;					/* 秒（0～60）※60は閏秒のため */
	sint tmMin;					/* 分（0～59）*/
	sint tmHour;				/* 時（0～23）*/
	sint tmMday;				/* 日（1～31）*/
	sint tmMon;					/* 月（1～12）*/
	sint tmYear;				/* 年（1900～2037）※西暦年 */
} DlogTm_t;

slong dlog_GetTime(DlogTm_t *pTime)
{
	struct timespec tspec;
	struct tm ltm;
	slong ret;
	struct tm *ptm;

	/* 引数確認 */
	if (NULL == pTime)
	{
		ret = 2;
	}
	else
	{
		/* システム時刻取得 */
		ret = clock_gettime(CLOCK_REALTIME, &tspec);
		if (0 == ret)
		{
			/* 年月日時分秒へ変換 */
			ptm = localtime_r(&tspec.tv_sec, &ltm);
			if (NULL != ptm)
			{
				/* 返却データへ設定 */
				pTime->tmYear = ltm.tm_year + 1900;
				pTime->tmMon = ltm.tm_mon + 1;
				pTime->tmMday = ltm.tm_mday;
				pTime->tmMin = ltm.tm_min;
				pTime->tmHour = ltm.tm_hour;
				pTime->tmSec = ltm.tm_sec;

				ret = 0;
			}
			else
			{
				ret = 1;
			}
		}
		else
		{
			ret = 1;
		}
	}

	return ret;
}

/*==========================================================================*/
/*	Description		:输出log												*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/12 : DHC Xu.Y : 新規作成						*/
/*==========================================================================*/
static void elog_OutputDlogTest(slong argc, schar *argv[])
{
	slong loopCnt = 0;
	DlogTm_t curTime;
	uchar tmpName[512];
	FILE *fp;

	for (loopCnt = 0; loopCnt < 49; loopCnt++)
	{
		dlog_GetTime(&curTime);

		memset(tmpName, 0, 512);
		snprintf((char *)tmpName,
			sizeof(tmpName),
			"%s%04d%02d%02d%02d%02d%02d%s",
			"/mnt/log/debug_log/voltalog",
			curTime.tmYear,
			curTime.tmMon,
			curTime.tmMday,
			curTime.tmHour,
			curTime.tmMin,
			curTime.tmSec,
			".txt");

		fp = fopen(tmpName, "w");
		fclose(fp);
		fflush(fp);
		fsync(fileno(fp));
		sleep(1);
	}
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/13 : DHC Jiang.Z : 新規作成					*/
/*==========================================================================*/
static void elog_PrintDlogTest(slong argc, schar *argv[])
{
	slong loopCnt = 0;
	slong num;
	num = atoi(argv[1]);
	for(; loopCnt < num; loopCnt++)
	{
		DLOG_Info("Test Code!\n");
	}
}
/* End Of File */
