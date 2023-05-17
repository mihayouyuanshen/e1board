/******************************************************************************/
/*    Description    :ログ管理ミドル マルチリンクDCシステム用処理                */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC Jiang.Z                                             */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/04/12                                              */
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#include "typedef.h"
#include "einv_g.h"
#include "etim_g.h"
#include "elog_g.h"
#include "elog_utl_g.h"
#include "elog_dcs_g.h"

/*----------------------------------------------------------------------------*/
/* 公開変数定義                                                                */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開変数定義                                                              */
/*----------------------------------------------------------------------------*/
ElogPcsLog_t s_elogDcsLog;			/* ログ取得応答データ */

/*----------------------------------------------------------------------------*/
/* 公開関数定義                                                                */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開関数定義                                                              */
/*----------------------------------------------------------------------------*/


/*============================================================================*/
/*    Description    :マルチリンクDCシステムPCSログ取得                          */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/12  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_DcsGetPcsLog(void)
{
#ifdef WORK_KYC	/* マルチリンクDCシステム仕様のみ有効 */

	slong ret;

	DEBUG_TRCIN();

	memset(&s_elogDcsLog, 0x0, sizeof(s_elogDcsLog));
	s_elogDcsLog.useful = FALSE;

	/* 日時文字列を取得 'YYYYMMDD-hhmmss' */
	ret = ELOG_GetDStr(s_elogDcsLog.date, sizeof(s_elogDcsLog.date), ELOG_DATESTR_DCS);
	if(ret != ETIM_OK)
	{
		DLOG_Error("ELOG_GetDStr() error. default string set.\n");
		strncpy(s_elogDcsLog.date, ELOG_DCS_DATENGSTR, sizeof(s_elogDcsLog.date));
	}
	else
	{
		/*Do nothing*/
	}

	/* PCSログ取得 */
	ret = epcs_sync_k_getPcsLog(&s_elogDcsLog.log);
	if(ret != EPCS_OK)
	{
		DLOG_Error("epcs_sync_k_getPcsLog() error.\n");
	}
	else
	{
		s_elogDcsLog.useful = TRUE;
	}
	DEBUG_TRCOUT();
	return ret;

#else
	DEBUG_TRCOUT();
	return ELOG_OK;
#endif
}

/*============================================================================*/
/*    Description    :マルチリンクDCシステムPCSログ・USB保存                     */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常                                      */
/*                   :                                                        */
/*                    ELOG_E_SYSCAL(-2)    システムコールエラー                */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/12  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_DcsSavePcsLog(void)
{
	slong ret = ELOG_OK;
#ifdef WORK_KYC	/* マルチリンクDCシステム仕様のみ有効 */

	slong ret = ELOG_OK;
	schar forMat[ELOG_DCS_PATHMAX+1];
	schar path[ELOG_DCS_PATHMAX+1];
	schar serno[ELOG_DCS_SERNO_LEN+1];
	struct stat sb;
	ulong i, j;
	bool_t flag = TRUE;
	FILE *fp;
	DEBUG_TRCIN();

	/* PCSからのログ取得が出来ていない場合は何もせず正常終了する */
	if(s_elogDcsLog.useful != TRUE)
	{
		DLOG_Error("pcslog data is not useful.\n");
		ret = ELOG_OK;
		flag = FALSE;
	}
	else
	{
		/*Do nothing*/
	}
	if(flag)
	{
		memset(forMat, 0x0, sizeof(forMat));
		memset(path, 0x0, sizeof(path));
		memset(serno, 0x0, sizeof(serno));
		memset(&sb, 0x0, sizeof(sb));

		/* 保存ディレクトリ(log)作成 */
		snprintf(path, sizeof(path)-1, "%s", ELOG_FS_USB);
		strncat(path, "/log", strlen("/log"));
		ret = mkdir(path, ELOG_ALLPERMS);
			if(ret != 0)
			{
		#ifdef ELOG_CROSSDBG
				int err = errno;
				if(err == EEXIST)
				{
		#else
				if(errno == EEXIST)
				{
		#endif
					/* 既存ディレクトリ有り : NOP */
				}
				else
				{
					/* ディレクトリ作成エラー */
					DLOG_Error("mkdir(%s) error.(%d)\n", path, errno);
					ret = ELOG_E_SYSCAL;
					flag = FALSE;
				}
			}
	}
	else
	{
		/*Do nothing*/
	}
	if(flag)
	{
		/* 保存ディレクトリ(log/pcs)作成 */
		strncat(path, "/pcs", strlen("/pcs"));
		ret = mkdir(path, ELOG_ALLPERMS);
		if(ret != 0)
		{
	#ifdef ELOG_CROSSDBG
			int err = errno;
			if(err == EEXIST)
			{
	#else
			if(errno == EEXIST)
			{
	#endif
				/* 既存ディレクトリ有り : NOP */
			}
			else
			{
				/* ディレクトリ作成エラー */
				DLOG_Error("mkdir(%s) error.(%d)\n", path, errno);
				ret = ELOG_E_SYSCAL;
				flag = FALSE;
			}
		}
	}
	else
	{
		/*Do nothing*/
	}

	if(flag)
	{
		memset(path, 0x0, sizeof(path));

		/* ログデータ中のシリアル番号を取り出し */
		for (i=ELOG_DCS_SERNO_OFS, j=0;
			i<ELOG_DCS_SERNO_OFS + ELOG_DCS_SERNO_LEN && i < EPCS_PCSLOG_K_LEN;
			i++)
			{
			/* パディング文字ならば終了 */
			if(ELOG_DCS_ISPADCHR(s_elogDcsLog.log.logBuf[i]))
			{
				i = EPCS_PCSLOG_K_LEN;
			}
			else
			{
				/* 英数字チェック */
				if(isalnum(s_elogDcsLog.log.logBuf[i]))
				{
					serno[j++] = s_elogDcsLog.log.logBuf[i];
				}
				else
				{
					/* 非英数字を'_'へ置換 */
					serno[j++] = '_';
				}
			}
		}
		/* シリアル番号取り出し結果がヌルの場合 */
		if(serno[0] == '\0')
		{
			DLOG_Error("serial number data is null 
						(or not all specific chars). default string set.\n");
			strncpy(serno, ELOG_DCS_SNONGSTR, sizeof(serno));
		}
		else
		{
			/*Do nothing*/
		}
		/* パス名のフォーマットを作成 */
		snprintf(forMat, sizeof(forMat)-1, "%s/%s/%s-LOG-%s-%%03d.dat",
				ELOG_FS_USB, ELOG_ERR_EVENT_DIR + 5, serno, s_elogDcsLog.date);

		/* ファイル重複検査 */
		for (i = 0; i <= ELOG_DCS_SUBNUM_MAX ; i++)
		{
			snprintf(path, sizeof(path)-1, forMat, i);
			ret = stat(path, &sb);
			if(ret != 0 && errno == ENOENT)
			{
				i == ELOG_DCS_SUBNUM_MAX;	/* 該当ファイルが無いのでこの名前で以降処理 */
			}
			else
			{
				/*Do nothing*/
			}
		}
		/* 全ての枝番号ファイルが存在->エラーとする */
		if(i > ELOG_DCS_SUBNUM_MAX)
		{
			DLOG_Error("save file sub numbers are all duplicated.\n");
			ret = ELOG_E_PROCS;
			flag = FALSE;
		}
		else
		{
			/*Do nothing*/
		}
	}
	else
	{
		/*Do nothing*/
	}

	if(flag)
	{
		/* 全ての枝番号ファイルが存在->エラーとする */
		if(i > ELOG_DCS_SUBNUM_MAX)
		{
			DLOG_Error("save file sub numbers are all duplicated.\n");
			ret = ELOG_E_PROCS;
		}
		else
		{
			/* ファイルオープン */
			fp = ELOG_FOPEN(path, "w");
			if(fp == NULL)
			{
				DLOG_Error("ELOG_FOPEN(%s) error.\n", path);
				ret = ELOG_E_SYSCAL;
			}
			else
			{
				/* PCSログをHEXダンプ(<sp>区切り) */
				for (i = 0; i < EPCS_PCSLOG_K_LEN; i++)
				{
					fprintf(fp, "%02X", s_elogDcsLog.log.logBuf[i]);
					if(i < EPCS_PCSLOG_K_LEN - 1)
					{
						fputc(ELOG_DCS_DIVCHR, fp);
					}
					else
					{
						/*Do nothing*/
					}
				}
			}
			/* ファイルクローズ */
			ELOG_FCLOSE(fp);
		}
	}
	else
	{
		/*Do nothing*/
	}

	DEBUG_TRCOUT();
	return ret;

#else
	DEBUG_TRCOUT();
	return ret;
#endif
}
/* End Of File */