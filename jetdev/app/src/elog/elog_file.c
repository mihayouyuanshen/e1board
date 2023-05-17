/******************************************************************************/
/*    Description    :ログ管理ミドル ファイル処理                               */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC Jiang.Z                                             */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/04/11                                              */
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
#include "common_g.h"
#include <task_g.h>
#include "typedef.h"
#include "elog_g.h"
#include "elog_val_g.h"
#include "elog_file_g.h"
#include "elog_utl_g.h"

/*----------------------------------------------------------------------------*/
/* 公開変数定義                                                                */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開変数定義                                                              */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 公開関数定義                                                                */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開関数定義                                                              */
/*----------------------------------------------------------------------------*/




/*============================================================================*/
/*    Description    :コンフィグファイルを読み込む                               */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    false:読み込み失敗/TRUE:読み込み成功                      */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
bool_t ELOG_FileReadConfig(void)
{
	FILE *fp;
	slong i;
	schar buf[ELOG_BUF_SIZE + 1];	/* elog.infの1行の最大文字数 */
	schar key[ELOG_BUF_SIZE + 1];
	bool_t flag = TRUE;
	bool_t ret = TRUE;
	schar levelSuffix[ELOG_BUF_SIZE + 1] = "_LOG_LEVEL=";
	schar enableSuffix[ELOG_BUF_SIZE + 1] = "_LOG_ENABLE=";
	schar *pval;
	ulong setLevelProcNum = 0;
	ulong setEnableProcNum = 0;
	DEBUG_TRCIN();

	fp = CMN_FOPEN(ELOG_CONF_FILENAME,"r");
	if(fp ==NULL)
	{
		DLOG_Error("CMN_FOPEN(%s) error.\n", ELOG_CONF_FILENAME);
		ret = FALSE;
	}
	else
	{
		while((flag) && (!feof(fp)))
		{
			/* ファイルの終わりまで */
			/* 1行づつ読み込み */
			if(fgets(buf,24,fp) == NULL)
			{
				flag = FALSE;
			}
			else
			{
				/* ログレベルの文字列検索 */
				for(i=0;i<PROCESS_MAX;i++)
				{
					/* ログレベルの先頭文字列作成 */
					snprintf(key, sizeof(key)-1, "%s%s",
					s_ElogTaskName[i].name, levelSuffix);

					/* ログレベルの先頭文字列検索 */
					pval = strstr(buf, key);
					if(pval != NULL)
					{
						/* 検索文字列の次のアドレスから値取得 */
						pval += strnlen(key, sizeof(key));
						/* テーブルに値セット */
						ELOG_ValSetLevel((ElogLv_t)atoi(pval),
										s_ElogTaskName[i].sport);
						setLevelProcNum++;
					}
					else
					{
						/*Do nothing*/
					}
				}

				/* ログ有効/無効の文字列検索 */
				for(i=0;i<PROCESS_MAX;i++)
				{
					/* ログ有効/無効の先頭文字列作成 */
					snprintf(key, sizeof(key)-1, "%s%s",
					s_ElogTaskName[i].name, enableSuffix);

					/* ログ有効/無効の先頭文字列検索 */
					pval = strstr(buf,key);
					if(pval != NULL)
					{
						/* 検索文字列の次のアドレスから値取得 */
						pval += strnlen(key, sizeof(key));
						/* テーブルに値セット */
						ELOG_ValSetEnable((bool_t)atoi(pval),
										s_ElogTaskName[i].sport);
						setEnableProcNum++;
					}
					else
					{
						/*Do nothing*/
					}
				}
			}
		}

		CMN_FCLOSE(fp);
		
		/* ファイルから読み出して設定したプロセス数が想定数と一致しない場合は上書き */
		if((ELOG_OUTPUT_LOG_PROC_NUM != setLevelProcNum) || 
			(ELOG_OUTPUT_LOG_PROC_NUM != setEnableProcNum))
		{
			ELOG_FileWriteConfig();
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :コンフィグファイルを書き込む                              */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    FALSE : 書き込み失敗 /TRUE  : 書き込み成功                */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
bool_t ELOG_FileWriteConfig(void)
{
	FILE *fp;
	slong i;
	slong ret = TRUE;
	schar levelSuffix[ELOG_BUF_SIZE + 1] = "_LOG_LEVEL=";
	schar enableSuffix[ELOG_BUF_SIZE + 1] = "_LOG_ENABLE=";

	DEBUG_TRCIN();
	fp = CMN_FOPEN(ELOG_CONF_FILENAME,"w");
	if(fp == NULL)
	{
		DLOG_Error("CMN_FOPEN(%s) error.\n", ELOG_CONF_FILENAME);
		ret = FALSE;
	}
	else
	{
		/* ログレベルの文字列作成 */
		for(i=0;i<PROCESS_MAX;i++)
		{
			if(s_ElogTaskName[i].sport != 0xFFFFFFFF)
			{
				/* ログレベルの先頭文字列作成 */
				fprintf(fp,"%s%s%d\n",
						s_ElogTaskName[i].name,
						levelSuffix,
						(int)ELOG_ValGetLevel(s_ElogTaskName[i].sport));
			}
		}

		/* ログ有効/無効の文字列作成 */
		for(i=0;i<PROCESS_MAX;i++)
		{
			/* ログレベルの先頭文字列作成 */
			if(s_ElogTaskName[i].sport != 0xFFFFFFFF)
			{
				fprintf(fp,"%s%s%d\n",
						s_ElogTaskName[i].name,
						enableSuffix,
						(int)ELOG_ValGetEnable(s_ElogTaskName[i].sport));
			}
		}
		CMN_FCLOSE(fp);
	}
	DEBUG_TRCOUT();
	return ret;
}
/* End Of File */