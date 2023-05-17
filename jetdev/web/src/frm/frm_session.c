/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Zhang.WY											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/14												*/
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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#include "common_g.h"
#include "typedef.h"
#include "frm_session.h"
#include "session_g.h"
#include "util_logger_g.h"
#include "config.h"

#define SESSION_FILE_NAME_MAX_SIZE 64
#define SESSION_FOLDER_PATH_SIZE 16
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
sint FRM_CheckSession(const schar *sessionId)
{
	LOG_entry("FRM_CheckSession");

	schar tmp_filePath_cp[FILEPATH_MAX_LENGTH];
	memset(tmp_filePath_cp, '\0', FILEPATH_MAX_LENGTH);
	sint ret = RET_ERROR_CODE;
	if ((NULL == sessionId))
	{
		MlogErrorParameterNull();
	}
	else
	{
		/**
		 * open session file
		 */
		strcpy(tmp_filePath_cp, SESSION_FOLDER);
		strcat(tmp_filePath_cp, SESSION_FILE_PREFIX);
		strcat(tmp_filePath_cp, sessionId);
		LOG_debug("session file path is: %s", tmp_filePath_cp);
		if (0 == access(tmp_filePath_cp, 0))
		{
			ret = RET_SUCCESS_CODE;
		}
	}

	LOG_leave("FRM_CheckSession");
	return ret;
}

sint FRM_SetSession(schar *sessionId, const SessionUserInfo_t *sessionUserInfo)
{
	LOG_entry("FRM_SetSession");
	sint ret = RET_SUCCESS_CODE;
	if ((NULL == sessionUserInfo) || (NULL == sessionId))
	{
		MlogErrorParameterNull();
		ret = RET_ERROR_CODE;
	}
	else
	{
		frm_checkSessionTimeout();
		frm_generateSessionId(sessionId);
		frm_writeSessionFile(sessionId, sessionUserInfo);
	}

	LOG_leave("FRM_SetSession");

	return ret;
}

/*==========================================================================*/
/*	Description		: 获取Session											*/
/*--------------------------------------------------------------------------*/
/*	param			: schar* sessionId, SessionUserInfo_t* sessionUserInfo	*/
/*--------------------------------------------------------------------------*/
/*	return			: sint：返回操作结果									*/
/*--------------------------------------------------------------------------*/
/*	detail			:获取Session文件中的用户信息							  */
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC Zhang.WY : 新規作成 					*/
/*==========================================================================*/
sint FRM_GetSession(const schar *sessionId, SessionUserInfo_t *sessionUserInfo)
{
	memset(sessionUserInfo, '\0', 128);
	sint ret = RET_SUCCESS_CODE;
	LOG_entry("FRM_GetSession");
	if ((NULL == sessionId))
	{
		MlogErrorParameterNull();
		return RET_ERROR_CODE;
	}

	FILE *sessionFile;
	schar sessionFileName[SESSION_FILE_NAME_MAX_SIZE];
	schar str_now[11];

	time_t now;
	time(&now);
	sprintf(str_now, "%10ld", now);

	strcpy(sessionFileName, SESSION_FOLDER);
	/* if the folder don't exist, make it */
	if (0 != access(sessionFileName, 0))
	{
		ret = RET_ERROR_CODE;
	}
	else
	{
		strcat(sessionFileName, SESSION_FILE_PREFIX);
		strncat(sessionFileName, sessionId, SESSION_ID_LENGTH);
		sessionFile = fopen(sessionFileName, "r");
		char tmpLine[MAX_LINE_LENGTH];
		if (sessionFile != NULL)
		{
			uint char_num;
			/* 获取时间戳（跳过） */
			fgets(tmpLine, MAX_LINE_LENGTH, sessionFile);
			/* 获取操作者 */
			fgets(tmpLine, MAX_LINE_LENGTH, sessionFile);
			char_num = strlen(tmpLine);
			strncpy(sessionUserInfo->userId, tmpLine, char_num - 1);
			/* 写入phone type */
			fgets(tmpLine, MAX_LINE_LENGTH, sessionFile);
			char_num = strlen(tmpLine);
			strncpy(sessionUserInfo->phoneType, tmpLine, char_num - 1);
			/* 写入application version */
			fgets(tmpLine, MAX_LINE_LENGTH, sessionFile);
			char_num = strlen(tmpLine);
			strncpy(sessionUserInfo->appVersion, tmpLine, char_num - 1);

			/* 关闭文件 */
			fclose(sessionFile);
		}
		else
		{
			LOG_error("can't read session file");
		}

		LOG_leave("FRM_GetSession");
	}
}
/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
static void frm_generateSessionId(schar *tmp_sessionId_scp)
{
	LOG_entry("frm_generateSessionId");

	sint i = 0;
	time_t tmp_now_t;
	time(&tmp_now_t);
	srand(tmp_now_t);

	memset(tmp_sessionId_scp, '\0', SESSION_ID_LENGTH + 1);
	uint tmp_randFactor_ui = rand();
	srand(tmp_randFactor_ui);
	uint tmp_rand_ui = rand();
	for (i = 0; i < SESSION_ID_LENGTH; i++)
	{
		srand(tmp_rand_ui);
		tmp_rand_ui = rand();
		tmp_sessionId_scp[i] = tmp_rand_ui % 26 + 'A';
	}

	LOG_leave("frm_generateSessionId");
}

static void frm_writeSessionFile(const schar *sessionId, const SessionUserInfo_t *sessionUserInfo)
{
	LOG_entry("frm_writeSessionFile");
	if ((NULL == sessionId) || (NULL == sessionUserInfo->userId))
	{
		MlogErrorParameterNull();
	}
	else
	{
		FILE *tmp_sessionFile_p;
		schar tmp_sessionFileName_scp[SESSION_FILE_NAME_MAX_SIZE];
		schar str_now[11];

		time_t now;
		time(&now);
		sprintf(str_now, "%10ld", now);

		strcpy(tmp_sessionFileName_scp, SESSION_FOLDER);
		/* if the folder don't exist, make it */
		if (0 != access(tmp_sessionFileName_scp, 0))
		{
			mkdir(tmp_sessionFileName_scp, 0777);
		}

		LOG_debug("sessinId: %s", sessionId);
		strcat(tmp_sessionFileName_scp, SESSION_FILE_PREFIX);
		strncat(tmp_sessionFileName_scp, sessionId, SESSION_ID_LENGTH);
		LOG_debug("session filePath: %s", tmp_sessionFileName_scp);

		tmp_sessionFile_p = fopen(tmp_sessionFileName_scp, "w");
		chmod(tmp_sessionFileName_scp, 06777);

		if (tmp_sessionFile_p != NULL)
		{
			/* 写入时间戳 */
			fputs(str_now, tmp_sessionFile_p);
			fputs("\n", tmp_sessionFile_p);
			/* 写入操作者 */
			fputs(sessionUserInfo->userId, tmp_sessionFile_p);
			fputs("\n", tmp_sessionFile_p);
			/* 写入phone type */
			fputs(sessionUserInfo->phoneType, tmp_sessionFile_p);
			fputs("\n", tmp_sessionFile_p);
			/* 写入application version */
			fputs(sessionUserInfo->appVersion, tmp_sessionFile_p);
			fputs("\n", tmp_sessionFile_p);

			/* 关闭文件 */
			fclose(tmp_sessionFile_p);
		}
		else
		{
			LOG_error("can't creat session file");
		}
	}

	LOG_leave("frm_writeSessionFile");
}

static void frm_checkSessionTimeout(void)
{
	LOG_entry("frm_checkSessionTimeout");

	DIR *tmp_rootDir_p;
	sint i = 0;
	schar tmp_rootdirPath_p[SESSION_FOLDER_PATH_SIZE] = SESSION_FOLDER;

	if ((tmp_rootDir_p = opendir(tmp_rootdirPath_p)) != NULL)
	{
		frm_checkSessionFile(tmp_rootDir_p, tmp_rootdirPath_p);
	}
	else
	{
		LOG_error("Can't open %s", tmp_rootdirPath_p);
	}

	LOG_leave("frm_checkSessionTimeout");
}
/* check the sessin file, if timeout will delete it */
static void frm_checkSessionFile(DIR *tmp_rootDir_p, schar *tmp_rootdirPath_p)
{
	LOG_entry("frm_checkSessionFile");
	if ((NULL == tmp_rootDir_p) || (NULL == tmp_rootdirPath_p))
	{
		MlogErrorParameterNull();
	}
	else
	{
		struct dirent *tmp_file_p;
		schar *tmp_deleteFileList_cp[MAX_DELETE_FILE_COUNT];
		sint array_index = 0;

		while ((tmp_file_p = readdir(tmp_rootDir_p)) != NULL)
		{
			/* current path and pre-level path should be skip */
			if (strcmp(tmp_file_p->d_name, ".") == 0 || strcmp(tmp_file_p->d_name, "..") == 0)
			{
			}
			else
			{

				schar tmp_fileName_scp[MAX_FILENAME_LENGTH];
				strncpy(tmp_fileName_scp, tmp_rootdirPath_p, SESSION_FOLDER_PATH_SIZE);
				strcat(tmp_fileName_scp, tmp_file_p->d_name);
				LOG_debug("file path: %s", tmp_fileName_scp);

				schar tmp_lineBuff_cp[MAX_LINE_LENGTH]; /* buffer for line*/
				FILE *tmp_file_p;
				/* file open fail */
				if ((tmp_file_p = fopen(tmp_fileName_scp, "r")) == NULL)
				{
					LOG_error("fail to read");
				}
				/* file open success */
				else
				{
					/* get now timestampe */
					time_t tmp_now_t;
					time(&tmp_now_t);
					/* read the firs line, which is the timestamp of session created */
					fgets(tmp_lineBuff_cp, MAX_LINE_LENGTH, tmp_file_p);
					/* caculate the diff of time, if more than 20 min will delete it */
					long timestamp = atol(tmp_lineBuff_cp);
					long timespan = tmp_now_t - timestamp;
					if (timespan > 1200)
					{
						remove(tmp_fileName_scp);
					}
				}
			}
		}

		closedir(tmp_rootDir_p);
	}

	LOG_leave("frm_checkSessionFile");
}

/*End Of File*/