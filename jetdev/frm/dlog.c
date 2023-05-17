/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Xu.Y												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/26												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <pthread.h>
#include <dirent.h>
#include <stdarg.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "dlog_g.h"
#include "dlog.h"
#include "dlog_ringbuf_g.h"
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
DlogFileInf_t	s_dlog_FileInf;
DlogOutMode_t	s_dlog_OutMode;
DlogRingbuf_t	s_dlog_Ringbuf;
uchar			s_dlogIsLogWorking = 0u;
pthread_t		s_dlog_PthreadTid;
pthread_mutex_t	s_dlog_Lock;
pthread_cond_t	s_dlog_Ready;
uchar 			*s_ringbuf[DLOG_RINGBUF_SIZE];
schar			s_dlogBuf[DLOG_DATA_READ_SIZE + 1];
/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:	dlog模块初始化										*/
/*--------------------------------------------------------------------------*/
/*	param			:	filePath					文件路径				*/
/*					:	filePfxName					文件前缀名				*/
/*					:	fileMaxSize					文件大小				*/
/*					:	fileNum						文件个数				*/
/*					:	outMode						输出模式				*/
/*					:	fileCrtDelay				文件生成延时			*/
/*--------------------------------------------------------------------------*/
/*	return			:	DLOG_RESULT_OK				処理成功				*/
/*					:	DLOG_RESULT_NG				処理失敗				*/
/*					:	DLOG_RESULT_EPRM			引数異常				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	dlog模块初始化										*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/26 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
DlogResult_t DLOG_Init(const char *filePath, const char *filePfxName,
	ulong fileMaxSize, ulong fileNum, DlogOutMode_t outMode,
	ulong fileCrtDelay)
{
	DlogResult_t	result	= DLOG_RESULT_OK;
	sint			res		= 0;

	if ((NULL == filePath) || (NULL == filePfxName) || (!fileMaxSize) ||
		(!fileNum) || (DLOG_OUT_MODE_MAX < outMode))
	{
		result = DLOG_RESULT_EPRM;
	}
	else
	{
		memset(&s_dlog_FileInf, 0, sizeof(DlogFileInf_t));

		memcpy(s_dlog_FileInf.filePath, filePath, DLOG_FILE_PATH_SIZE);
		memcpy(s_dlog_FileInf.filePrefxName, filePfxName,
			DLOG_FILE_NAME_SIZE);

		snprintf((char *)s_dlog_FileInf.filePathPfxName,
			sizeof(s_dlog_FileInf.filePathPfxName),
			"%s/%s",
			filePath, filePfxName);

		s_dlog_FileInf.fileMaxSize = fileMaxSize;
		s_dlog_FileInf.fileMaxNum = fileNum;

		s_dlog_FileInf.fileCnt = dlog_GetFileNum(s_dlog_FileInf.filePath,
			s_dlog_FileInf.filePrefxName);

		s_dlog_FileInf.lastTimeStampSec = 0u;
		s_dlog_FileInf.timeIntervalSec = fileCrtDelay;

		s_dlog_OutMode = outMode;

		pthread_mutex_init(&s_dlog_Lock, NULL);
		pthread_cond_init(&s_dlog_Ready, NULL);
		DLOG_RingbufInit(&s_dlog_Ringbuf, DLOG_RINGBUF_ITEM_SIZE,
							s_ringbuf, DLOG_RINGBUF_SIZE);

		res = pthread_create(&s_dlog_PthreadTid, NULL,
								dlog_Pthread, &s_dlog_FileInf);
		if (res)
		{
			result = DLOG_RESULT_NG;
		}
		else
		{
			s_dlogIsLogWorking = 1u;
		}
	}

	return result;
}

/*==========================================================================*/
/*	Description		:	输出log												*/
/*--------------------------------------------------------------------------*/
/*	param			:	level						输出等级				*/
/*	param			:	fmt													*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	输出log												*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/27 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
void DLOG_Log(DlogLogLevel_t level, const char *fmt, ...)
{
	va_list args;
	slong len = 0, ret;
	schar logBuf[DLOG_LOG_BUF_MAX_SIZE];
	DlogTm_t time;

	/* 当前log级别大于等于默认log级别时，输出log */
	if ((s_dlogIsLogWorking) && (DLOG_LOG_LEVEL_DEFAULT <= level))
	{
		ret = dlog_GetTime(&time);
		if (DLOG_RESULT_OK == ret)
		{
#if 0
			len = snprintf((char *)logBuf, DLOG_LOG_BUF_MAX_SIZE,
							"[%04d-%02d-%02d %02d:%02d:%02d]",
							time.tmYear,
							time.tmMon,
							time.tmMday,
							time.tmHour,
							time.tmMin,
							time.tmSec);
#else
			len = snprintf((char *)logBuf, DLOG_LOG_BUF_MAX_SIZE,
							"[%02d:%02d:%02d]",
							time.tmHour,
							time.tmMin,
							time.tmSec);
#endif
		}
		else
		{
#if 0
			len = snprintf((char *)logBuf, DLOG_LOG_BUF_MAX_SIZE,
				"[00:00:00]");
#else
			len = snprintf((char *)logBuf, DLOG_LOG_BUF_MAX_SIZE,
				"[0000-00-00 00:00:00]");
#endif
		}

		va_start(args, fmt);
		len += vsnprintf((char *)(logBuf + len),
						sizeof(logBuf) - len, fmt, args);
		va_end(args);

		pthread_mutex_lock(&s_dlog_Lock);

		DLOG_RingbufIn(&s_dlog_Ringbuf, logBuf, len);

		pthread_cond_signal(&s_dlog_Ready);

		pthread_mutex_unlock(&s_dlog_Lock);
	}
	else
	{
		/* 処理無し */
	}
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:	获取路径下前缀名相同的文件个数						*/
/*--------------------------------------------------------------------------*/
/*	param			:	filePath					文件路径				*/
/*					:	filePfxName					文件前缀名				*/
/*--------------------------------------------------------------------------*/
/*	return			:	fileNum						文件个数				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	获取路径下前缀名相同的文件个数						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/27 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static slong dlog_GetFileNum(const uchar *filePath, const uchar *filePfxName)
{
	slong			fileNum = 0;
	DIR				*dir	= NULL;
	struct dirent	*ptr	= NULL;
	bool_t			loopFlg = TRUE;
	char			*tmpStr = NULL;

	if ((NULL == filePath) || (NULL == filePfxName))
	{
		/* DO NOTHING */
	}
	else
	{
		dir = opendir((const char *)filePath);

		while (loopFlg)
		{
			ptr = readdir(dir);
			if (NULL == ptr)
			{
				loopFlg = FALSE;
			}
			else if (DT_REG == ptr->d_type)
			{
				tmpStr = strstr(ptr->d_name, (const char *)filePfxName);
				if (tmpStr)
				{
					fileNum++;
				}
				else
				{
					/* DO NOTHING */
				}
			}
			else
			{
				/* DO NOTHING */
			}
		}

		closedir(dir);
	}

	return fileNum;
}

/*==========================================================================*/
/*	Description		:	删除文件											*/
/*--------------------------------------------------------------------------*/
/*	param			:	fileName					文件名					*/
/*--------------------------------------------------------------------------*/
/*	return			:	DLOG_RESULT_OK				処理成功				*/
/*					:	DLOG_RESULT_NG				処理失敗				*/
/*					:	DLOG_RESULT_EPRM			引数異常				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	删除文件											*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/27 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static DlogResult_t dlog_FileDelete(const uchar *fileName)
{
	DlogResult_t	result = DLOG_RESULT_OK;
	slong			accRes = 0;
	slong			remRes = 0;

	if (NULL == fileName)
	{
		result = DLOG_RESULT_EPRM;
	}
	else
	{
		accRes = access(fileName, 0);
		if (DLOG_CTYPE_ERR != accRes)
		{
			remRes = remove((const char *)fileName);
			if (DLOG_CTYPE_ERR != remRes)
			{
				/* DO NOTHING */
			}
			else
			{
				result = DLOG_RESULT_NG;
			}
		}
		else
		{
			result = DLOG_RESULT_NG;
		}
	}

	return result;
}

/*==========================================================================*/
/*	Description		:	文件重命名											*/
/*--------------------------------------------------------------------------*/
/*	param			:	fileName					原文件名				*/
/*					:	fileRename					新文件名				*/
/*--------------------------------------------------------------------------*/
/*	return			:	DLOG_RESULT_OK				処理成功				*/
/*					:	DLOG_RESULT_NG				処理失敗				*/
/*					:	DLOG_RESULT_EPRM			引数異常				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	文件重命名											*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/27 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static DlogResult_t dlog_FileRename(const uchar *fileName,
	const uchar *fileRename)
{
	DlogResult_t	result = DLOG_RESULT_OK;
	slong			accRes = 0;
	slong			rnmRes = 0;

	if ((NULL == fileName) || (NULL == fileRename))
	{
		result = DLOG_RESULT_EPRM;
	}
	else
	{
		accRes = access(fileName, 0);
		if (DLOG_CTYPE_ERR != accRes)
		{
			rnmRes = rename((const char *)fileName, (const char *)fileRename);
			if (DLOG_CTYPE_ERR != rnmRes)
			{
				printf("dlog: rename file ok\n");
			}
			else
			{
				result = DLOG_RESULT_NG;
			}
		}
		else
		{
			result = DLOG_RESULT_NG;
		}
	}

	return result;
}

/*==========================================================================*/
/*	Description		:	获取文件名列表										*/
/*--------------------------------------------------------------------------*/
/*	param			:	ent													*/
/*--------------------------------------------------------------------------*/
/*	return			:	ret						文件个数					*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/27 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static slong dlog_FileFilter(const struct dirent *ent)
{
	sint res;
	slong result;

	if (NULL == ent)
	{
		result = 0;
	}
	else
	{
		if (DT_DIR == ent->d_type)
		{
			result = 0;
		}
		else
		{
			res = strncmp(ent->d_name,
						(const char *)s_dlog_FileInf.filePrefxName,
						strlen((const char *)s_dlog_FileInf.filePrefxName));
			if (res)
			{
				result = 0;
			}
			else
			{
				result = 1;
			}
		}
	}

	return result;
}

/*==========================================================================*/
/*	Description		:	删除最早的文件										*/
/*--------------------------------------------------------------------------*/
/*	param			:	void												*/
/*--------------------------------------------------------------------------*/
/*	return			:	DLOG_RESULT_OK				処理成功				*/
/*					:	DLOG_RESULT_NG				処理失敗				*/
/*					:	DLOG_RESULT_EPRM			引数異常				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	删除最早的文件										*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/27 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static DlogResult_t dlog_DelOldLog(void)
{
	DlogResult_t result = DLOG_RESULT_OK;
	struct dirent **fileList = NULL;
	sint fileCnt;
	uchar filePathName[DLOG_FILE_NAME_SIZE];

	fileCnt = scandir((const char *)s_dlog_FileInf.filePath,
						&fileList, dlog_FileFilter, alphasort);

	if (0 < fileCnt)
	{
		snprintf((char *)filePathName, sizeof(filePathName), "%s/%s", 
					s_dlog_FileInf.filePath, fileList[0]->d_name);
		dlog_FileDelete(filePathName);
		/* ソート結果を開放する */
		while (0 < fileCnt)
		{
			fileCnt--;
			if (fileList[fileCnt] == NULL)
			{
				/*Do nothing*/
			}
			else
			{
				free(fileList[fileCnt]);
			}
		}
		free(fileList);
	}
	else
	{
		/* DO NOTHING */
	}

	return result;
}

/*==========================================================================*/
/*	Description		:	切换当前使用文件									*/
/*--------------------------------------------------------------------------*/
/*	param			:	fileInf						文件内容结构体			*/
/*--------------------------------------------------------------------------*/
/*	return			:	DLOG_RESULT_OK				処理成功				*/
/*					:	DLOG_RESULT_NG				処理失敗				*/
/*					:	DLOG_RESULT_EPRM			引数異常				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	切换当前使用文件									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/27 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static DlogResult_t dlog_FileChange(DlogFileInf_t *fileInf)
{
	DlogResult_t	result		= DLOG_RESULT_OK;
	uchar tmpName[DLOG_FILE_PATH_NAME_SIZE];
	DlogTm_t curTime;

	if (NULL == fileInf)
	{
		result = DLOG_RESULT_EPRM;
	}
	else
	{
		if (fileInf->filePtr)
		{
			fclose(fileInf->filePtr);
		}
		else
		{
			/* DO NOTHING */
		}

		if (fileInf->fileCnt < fileInf->fileMaxNum)
		{
			fileInf->fileCnt++;
		}
		else
		{
			/* 删除时间最早文件 */
			dlog_DelOldLog();
		}

		dlog_GetTime(&curTime);
		fileInf->lastTimeStampSec = time(NULL);
		memset(tmpName, 0, DLOG_FILE_PATH_NAME_SIZE);

		snprintf((char *)tmpName,
			sizeof(tmpName),
			"%s%04d%02d%02d%02d%02d%02d%s",
			fileInf->filePathPfxName,
			curTime.tmYear,
			curTime.tmMon,
			curTime.tmMday,
			curTime.tmHour,
			curTime.tmMin,
			curTime.tmSec,
			DLOG_FILE_SUFFIX_NAME);

		fileInf->filePtr = fopen((const char *)tmpName, "w");
		if (NULL == fileInf->filePtr)
		{
			result = DLOG_RESULT_NG;
		}
		else
		{
			fileInf->fileWriteCnt = 0;
		}
	}

	return result;
}

/*==========================================================================*/
/*	Description		:	将数据写入文件										*/
/*--------------------------------------------------------------------------*/
/*	param			:	fileInf						文件内容结构体			*/
/*					:	data						数据					*/
/*					:	dataSize					数据长度				*/
/*--------------------------------------------------------------------------*/
/*	return			:	DLOG_RESULT_OK				処理成功				*/
/*					:	DLOG_RESULT_NG				処理失敗				*/
/*					:	DLOG_RESULT_EPRM			引数異常				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将数据写入文件										*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/26 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static DlogResult_t dlog_WriteFile(DlogFileInf_t *fileInf, uchar *data,
	ulong dataSize)
{
	DlogResult_t	result		= DLOG_RESULT_OK;
	ulong			fwriteRes	= 0;
	ulong			residueSize	= fileInf->fileMaxSize - fileInf->fileWriteCnt;
	time_t			curTime;

	if ((NULL == fileInf) || (NULL == data) || (!data))
	{
		/* 参数错误 */
		result =  DLOG_RESULT_EPRM;
	}
	else
	{
		/* 当前未打开文件，或文件剩余Size不足,或时间超过新建log间隔 */
		curTime = time(NULL);
		if ((NULL == fileInf->filePtr) || 
			(0 == residueSize) ||
			(residueSize < dataSize) ||
			(fileInf->timeIntervalSec <= 
			(curTime - fileInf->lastTimeStampSec)))
		{
			result = dlog_FileChange(fileInf);
		}
		else
		{
			/* DO NOTHING */
		}

		if (NULL != fileInf->filePtr)
		{
			/* 将数据写入文件 */
			fwriteRes = fwrite(data, DLOG_FILE_WRITE_SIZE,
				(size_t)dataSize, fileInf->filePtr);
			if (fwriteRes == dataSize)
			{
				/* 记录已写文件Size */
				fileInf->fileWriteCnt += fwriteRes;
			}
			else
			{
				printf("dlog: fwrite err\n");
			}
			fflush(fileInf->filePtr);
			fsync(fileno(fileInf->filePtr));
		}
		else
		{
			printf("dlog: fopen file failed\n");
		}
	}

	return result;
}

/*==========================================================================*/
/*	Description		:	处理log线程											*/
/*--------------------------------------------------------------------------*/
/*	param			:	arg													*/
/*--------------------------------------------------------------------------*/
/*	return			:	NULL												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	处理log线程											*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/26 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void *dlog_Pthread(void *arg)
{
	DlogFileInf_t	*fileInf	= NULL;
	slong			readSize	= 0;
	slong			remainSize	= 0;
	bool_t			isEmpty		= FALSE;

	if (NULL == arg)
	{
		DEBUG_TRCOUT();
		return NULL;
	}
	else
	{
		/* DO NOTHING */
	}

	fileInf = (DlogFileInf_t *)arg;

	while (s_dlogIsLogWorking)
	{
		pthread_mutex_lock(&s_dlog_Lock);

		isEmpty = DLOG_RingbufIsEmpty(&s_dlog_Ringbuf);
		if (isEmpty)
		{
			pthread_cond_wait(&s_dlog_Ready, &s_dlog_Lock);
		}
		else
		{
			/* DO NOTHING */
		}

		remainSize = s_dlog_FileInf.fileMaxSize - s_dlog_FileInf.fileWriteCnt;
		if (DLOG_DATA_READ_SIZE <= remainSize)
		{
			readSize = DLOG_DATA_READ_SIZE;
		}
		else
		{
			readSize = remainSize;
		}
		readSize = DLOG_RingbufOut(&s_dlog_Ringbuf, s_dlogBuf, readSize);

		s_dlogBuf[readSize] = '\0';

		pthread_mutex_unlock(&s_dlog_Lock);

		switch (s_dlog_OutMode)
		{
		case DLOG_OUT_MODE_FILE:
			dlog_WriteFile(fileInf, s_dlogBuf, readSize);
			break;
		case DLOG_OUT_MODE_CONSOLE:
			printf("%s", s_dlogBuf);
			break;
		default:
			printf("dlog: dlog out mode err %d\n", s_dlog_OutMode);
			break;
		}
	}

	return NULL;
}

/*==========================================================================*/
/*	Description		:	現在時刻取得										*/
/*--------------------------------------------------------------------------*/
/*	param			:	pTime						現在時刻				*/
/*--------------------------------------------------------------------------*/
/*	return			:	DLOG_RESULT_OK				処理成功				*/
/*					:	DLOG_RESULT_NG				処理失敗				*/
/*					:	DLOG_RESULT_EPRM			引数異常				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	現在時刻取得										*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/25 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
static DlogResult_t dlog_GetTime(DlogTm_t *pTime)
{
	struct timespec tspec;
	struct tm ltm;
	DlogResult_t ret;
	struct tm *ptm;

	/* 引数確認 */
	if (NULL == pTime)
	{
		ret = DLOG_RESULT_EPRM;
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

				ret = DLOG_RESULT_OK;
			}
			else
			{
				ret = DLOG_RESULT_NG;
			}
		}
		else
		{
			ret = DLOG_RESULT_NG;
		}
	}

	return ret;
}

/*End Of File*/
