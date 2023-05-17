/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Xu.Y												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/27												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __DLOG_H__
#define __DLOG_H__

#include "dlog_g.h"
#include <stdio.h>
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define DLOG_FILE_PATH_SIZE			((uchar)255u)
#define DLOG_FILE_NAME_SIZE			((uchar)255u)
#define DLOG_FILE_PATH_NAME_SIZE	((ushort)512u)
#define DLOG_FILE_SUFFIX_NAME		".txt"
#define DLOG_FILE_CNT_FIRST			((slong)1)
#define DLOG_CTYPE_ERR				((slong)-1)
#define DLOG_FILE_WRITE_SIZE		((slong)1)
#define DLOG_DATA_READ_SIZE			((slong)1024)
#define DLOG_LOG_BUF_MAX_SIZE		((slong)256)
#define DLOG_RINGBUF_SIZE			((slong)1024 * 1024)
#define DLOG_RINGBUF_ITEM_SIZE		((slong)1)
/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef struct
{
	FILE	*filePtr;									/* log文件指针		*/
	uchar	filePath[DLOG_FILE_PATH_SIZE];				/* log文件路径		*/
	uchar	filePrefxName[DLOG_FILE_NAME_SIZE];			/* log文件前缀名	*/
	uchar	filePathPfxName[DLOG_FILE_PATH_NAME_SIZE];	/* 路径 + 前缀		*/
	ulong	fileMaxSize;								/* log文件size		*/
	ulong	fileMaxNum;									/* log文件个数		*/
	ulong	fileCnt;									/* log文件计数		*/
	ulong	fileWriteCnt;								/* 已写字节计数		*/
	time_t	lastTimeStampSec;							/* 上次创建log时间戳*/
	ulong	timeIntervalSec;							/* 新建log时间间隔	*/
} DlogFileInf_t;

/* 時刻設定・取得データ */
typedef struct {
	sint tmSec;					/* 秒（0～60）※60は閏秒のため */
	sint tmMin;					/* 分（0～59）*/
	sint tmHour;				/* 時（0～23）*/
	sint tmMday;				/* 日（1～31）*/
	sint tmMon;					/* 月（1～12）*/
	sint tmYear;				/* 年（1900～2037）※西暦年 */
} DlogTm_t;

static slong dlog_GetFileNum(const uchar *filePath, const uchar *filePfxName);
static DlogResult_t dlog_FileDelete(const uchar *fileName);
static DlogResult_t dlog_FileRename(const uchar *fileName,
	const uchar *fileRename);
static DlogResult_t dlog_FileChange(DlogFileInf_t *fileInf);
static DlogResult_t dlog_WriteFile(DlogFileInf_t *fileInf, uchar *data,
	ulong dataSize);
static void *dlog_Pthread(void *arg);
static DlogResult_t dlog_GetTime(DlogTm_t *pTime);

#endif /*__DLOG_H__*/
