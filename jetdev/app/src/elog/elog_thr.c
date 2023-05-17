/******************************************************************************/
/*    Description    :ログ管理ミドル スレッド関連                               */
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
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "task_g.h"
#include "typedef.h"
#include "common_g.h"
#include "etim_g.h"
#include "eeep_g.h"
#include "elog_g.h"
#include "elog_utl_g.h"
#include "elog_fnc_g.h"
#include "einv_g.h"
#include "elog_thr_g.h"
#include "elog_thr.h"

/*----------------------------------------------------------------------------*/
/* 公開変数定義                                                                */
/*----------------------------------------------------------------------------*/
extern ElogFileSize_t s_elogFileSize[ELOG_FILE_SIZE_SIZE_MAX];
extern FrmPthread_t s_elogTid;
extern FrmPthread_t s_elogSdpTmpId;
extern FrmPthread_t s_elogLineSizeId;
extern pthread_mutex_t s_elogSysCmdMutx;				/* system()実行排他用 */
/*----------------------------------------------------------------------------*/
/* 非公開変数定義                                                              */
/*----------------------------------------------------------------------------*/
schar s_elogSdpFileName[ELOG_FILE_NAME_MAX] = {0};
/***********************************************************/
/***** USB保存スレッド *************************************/
/***********************************************************/
ElogThdata_t s_elogThrUSB;					/* 管理データ		*/
ElogUsbSave_t s_elogThrUSBData;				/* スレッドデータ	*/
bool_t s_elogAbortUsbFlag;					/* 中断フラグ		*/
pthread_mutex_t s_elogAbortUsbFlag_mutex;	/* 中断フラグ排他	*/

/* USB保存子プロセス */
pid_t s_elogUsbChild;						/* プロセスID		*/
pthread_mutex_t s_elogUsbChildMutex;		/* 排他				*/

/* USB保存進捗率 */
ulong s_elogUsbProgressTmr;					/* タイマーカウント */
uchar s_elogUsbProgress;					/* 進捗率			*/
pthread_mutex_t s_elogUsbProgressMutex;		/* 進捗率排他		*/


/***********************************************************/
/***** 圧縮処理スレッド ************************************/
/***********************************************************/
ElogThdata_t s_elogThrCompress;				/* 管理データ		*/
ElogTgzSave_t s_elogThrCompData;			/* スレッドデータ	*/


/***********************************************************/
/***** NAND容量回復スレッド ********************************/
/***********************************************************/


ElogThdata_t s_elogThrNandDel;
pthread_mutex_t s_elogThrNandDelMutex;			/* スレッド実行排他	*/
bool_t s_elogAbortNandDelFlag;					/* 中断フラグ		*/
pthread_mutex_t s_elogAbortNandDelFlagMutex;	/* 中断フラグ排他	*/

/* 削除対象検索日付上限フィルタ条件 */
static ElogType_t s_elogLogDateLimitType;				/* ファイルタイプ */
static schar s_elogLogDateLimit[ELOG_DATESTR_LEN + 1];	/* 上限日付 */


/***********************************************************/
/***** ログリスト取得スレッド ******************************/
/***********************************************************/
ElogThdata_t s_elogThrGetLogList;
pthread_mutex_t s_elogThrGetLogListMutex;			/* スレッド実行排他	*/
ElogFncGetLogList_t s_elogThrGetLogListData;		/* スレッドデータ	*/


/***********************************************************/
/***** ログ削除スレッド ************************************/
/***********************************************************/
ElogThdata_t s_elogThrDeleteLog;
pthread_mutex_t s_elogThrDeleteLogMutex;		/* スレッド実行排他	*/
ElogFncDeleteLog_t s_elogThrDeleteLogData;		/* スレッドデータ	*/


/***********************************************************/
/***** その他 **********************************************/
/***********************************************************/
pthread_mutex_t s_elogThrEXCLMutex;			/* USB vs Comp 同時実行排他 */

/* ログリスト・ログ削除日付フィルタ条件 */
static ElogType_t s_elogDateRangeType;					/* ログタイプ */
static schar s_elogDateRangeFrom[ELOG_DATESTR_LEN+1];	/* 開始日 */
static schar s_elogDateRangeTo[ELOG_DATESTR_LEN+1];		/* 終了日 */
static pthread_mutex_t s_elogDateRangeMutex;			/* 日付フィルタ排他 */
/*----------------------------------------------------------------------------*/
/* 公開関数定義                                                                */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開関数定義                                                              */
/*----------------------------------------------------------------------------*/


/*============================================================================*/
/*    Description    :スレッド関連初期化                                        */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/TRUE:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/07  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_ThrInit(void)
{
	/* USB保存・ログ圧縮スレッド同時実行排他 */
	CMN_MutexInit(&s_elogThrEXCLMutex);

	/* USB保存子プロセスID排他 */
	CMN_MutexInit(&s_elogUsbChildMutex);

	/* USB保存進捗通知排他 */
	CMN_MutexInit(&s_elogUsbProgressMutex);

	/* NAND容量回復スレッド実行排他 */
	CMN_MutexInit(&s_elogThrNandDelMutex);

	/* ログリスト取得スレッド実行排他 */
	CMN_MutexInit(&s_elogThrGetLogListMutex);

	/* ログ削除スレッド実行排他 */
	CMN_MutexInit(&s_elogThrDeleteLogMutex);

	/* ログ取得＆ログ削除日付フィルタ条件排他 */
	CMN_MutexInit(&s_elogDateRangeMutex);

	/* USB保存スレッド管理データ初期化 */
	memset(&s_elogThrUSB, 0x00, sizeof(s_elogThrUSB));
	s_elogThrUSB.state = ELOG_THSTATE_IDLE;
	s_elogThrUSB.start = elog_ThrUSBStart;
	s_elogThrUSB.data = &s_elogThrUSBData;
	CMN_MutexInit(&s_elogThrUSB.sMutex);
	CMN_MutexInit(&s_elogThrUSB.dMutex);

	/* ログ圧縮スレッド管理データ初期化 */
	memset(&s_elogThrCompress, 0x00, sizeof(s_elogThrCompress));
	s_elogThrCompress.state = ELOG_THSTATE_IDLE;
	s_elogThrCompress.start = elog_ThrCompressStart;
	s_elogThrCompress.data = &s_elogThrCompData;
	CMN_MutexInit(&s_elogThrCompress.sMutex);
	CMN_MutexInit(&s_elogThrCompress.dMutex);

	/* NAND容量回復スレッド管理データ初期化 */
	memset(&s_elogThrNandDel, 0x00, sizeof(s_elogThrNandDel));
	s_elogThrNandDel.state = ELOG_THSTATE_IDLE;
	s_elogThrNandDel.start = elog_ThrDeleteNandLog;
	s_elogThrNandDel.data = NULL;
	CMN_MutexInit(&s_elogThrNandDel.sMutex);

	/* ログリスト取得スレッド管理データ初期化 */
	memset(&s_elogThrGetLogList, 0x00, sizeof(s_elogThrGetLogList));
	s_elogThrGetLogList.state = ELOG_THSTATE_IDLE;
	s_elogThrGetLogList.start = elog_ThrGetLogList;
	s_elogThrGetLogList.data = &s_elogThrGetLogListData;
	CMN_MutexInit(&s_elogThrGetLogList.sMutex);
	CMN_MutexInit(&s_elogThrGetLogList.dMutex);

	/* ログ削除スレッド管理データ初期化 */
	memset(&s_elogThrDeleteLog, 0x00, sizeof(s_elogThrDeleteLog));
	s_elogThrDeleteLog.state = ELOG_THSTATE_IDLE;
	s_elogThrDeleteLog.start = elog_ThrDeleteLog;
	s_elogThrDeleteLog.data = &s_elogThrDeleteLogData;
	CMN_MutexInit(&s_elogThrDeleteLog.sMutex);
	CMN_MutexInit(&s_elogThrDeleteLog.dMutex);
}

/*============================================================================*/
/*    Description    :スレッド起動                                             */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/TRUE:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/07  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_ThrStart(ElogThdata_t *thData)
{
	slong ret = ELOG_OK;

	DEBUG_TRCIN();

	if(thData == NULL)
	{
		DLOG_Error("thData is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		/*Do nothing*/
	}

	ELOG_ThrState(thData, ELOG_THSTATE_START);

	/* スレッド起動 */
	ret = FRM_PthreadCreate(&s_elogTid, NULL, thData->start, NULL);
	if(ret != 0)
	{
		ELOG_ThrState(thData, ELOG_THSTATE_IDLE);
		DEBUG_TRCOUT();
		DLOG_Error("pthread_create() error.(%d)\n", errno);
		ret = ELOG_E_SYSCAL;
	}
	else
	{
		ret = ELOG_OK;
	}

	DEBUG_TRCOUT();
	return ret;
}
/*============================================================================*/
/*    Description    :USB保存スレッド                                          */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/07  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void *elog_ThrUSBStart(void *arg)
{
	schar cmdLine[ELOG_USBSAVE_CMD_LINE + 1];
	slong ret;
	ulong rId;
	ulong i;
	schar buf[ELOG_USBSAVE_CMD_LINE + 1];
	uchar progress = 0;
	ulong tTime = 0;
	ElogUsbst_t usbSt;
	bool_t flag = TRUE;
	uchar data[ELOG_SERIES_LEN];

	pid_t child;
	sint status;
	schar serial[ELOG_SERIES_LEN + 1];
	schar dateStr[ELOG_USBSAVE_DATELEN + 1];
	DEBUG_TRCIN();

	/* ロック */
	CMN_MutexLock(&s_elogThrEXCLMutex);

	/* 初期化 */
	memset(cmdLine, 0x0, sizeof(cmdLine));
	memset(buf, 0x0, sizeof(buf));
	memset(dateStr, 0x0, sizeof(dateStr));

	ELOG_ThrState(&s_elogThrUSB, ELOG_THSTATE_RUN);

	/* スレッドデータ */
	CMN_MutexLock(&s_elogThrUSB.dMutex);

	tTime = ((ElogUsbSave_t *)(s_elogThrUSB.data))->tTime;
	rId = ((ElogUsbSave_t *)(s_elogThrUSB.data))->msgData.header.destModId;

	ELOG_thrSendUsbInProgress(TRUE);

	/* NAND容量回復スレッド状態確認、中断 */
	ret = ELOG_AbortAndWaitNandDelete(TRUE);
	if(ret != ELOG_OK)
	{
		DLOG_Error("ELOG_AbortAndWaitNandDelete() error.\n");
		elog_ThrUSBStartExit(rId, &s_elogThrUSB,
				ELOG_USBST_READERROR, 0, 0, ELOG_THSTATE_INCOMPLETE);
	}
	else
	{
		/* 中断確認 */
		if(ELOG_GetUsbAbort() == TRUE)
		{
			progress = elog_GetUsbProgress();
			elog_ThrUSBStartExit(rId, &s_elogThrUSB,
					ELOG_USBST_ABORTED, progress, tTime, ELOG_THSTATE_ABORTED);
		}
		else
		{
			/* ログ削除スレッド待ち */
			ret = ELOG_CheckSubThreadRun(ELOG_THTYPE_LDEL);
			if(ret == ELOG_E_BUSY)
			{
				CMN_MutexLock(&s_elogThrDeleteLogMutex);
				CMN_MutexUnlock(&s_elogThrDeleteLogMutex);
			}
			else if(ret == ELOG_E_PROCS)
			{
				DLOG_Error("ELOG_CheckSubThreadRun() error.\n");
				progress = elog_GetUsbProgress();
				elog_ThrUSBStartExit(rId, &s_elogThrUSB,
						ELOG_USBST_READERROR, progress, tTime,
						ELOG_THSTATE_INCOMPLETE);
				flag = FALSE;
			}
			if(flag)
			{
				/* 中断確認 */
				if(ELOG_GetUsbAbort() == TRUE)
				{
					progress = elog_GetUsbProgress();
					elog_ThrUSBStartExit(rId, &s_elogThrUSB,
							ELOG_USBST_ABORTED, progress, tTime,
							ELOG_THSTATE_ABORTED);
					flag = FALSE;
				}
				else
				{
					/*Do nothing*/
				}
			}
		}
	}
	if(flag)
	{
		/* シリアル番号を取得 */
		memset(serial, 0x0, sizeof(serial));
		#ifdef ELOG_CROSSDBG
			ret = EEEP_R_OK;
			strncpy(serial, "REMOCON000000000" sizeof(serial) - 1);
		#else
			EEEP_GetBoardSeries(data);
		#endif

		/* 保存ディレクトリ作成 */
		snprintf(buf, sizeof(buf)-1, "%s/%s", ELOG_FS_USB, serial);
		ret = mkdir(buf, ELOG_ALLPERMS);
		if(ret != 0)
		{
			if(errno == EEXIST)
			{
				/* 既存ディレクトリ有り : NOP */
			}
			else
			{
				/* ディレクトリ作成エラー */
				DLOG_Error("mkdir(%s) error.(%d)\n", buf, errno);
				/* 異常終了イベント通知 */
				progress = elog_GetUsbProgress();
				elog_ThrUSBStartExit(rId, &s_elogThrUSB,
						ELOG_USBST_WRITEERROR, progress, tTime,
						ELOG_THSTATE_INCOMPLETE);
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
		/* 保存日時取得 */
		ret = ELOG_GetDStr(dateStr, sizeof(dateStr), ELOG_DATESTR_LOG, 0);
		if(ret != ELOG_OK)
		{
			DLOG_Error("ELOG_GetDStr() error.\n");
			/* 異常終了イベント通知 */
			progress = elog_GetUsbProgress();
			elog_ThrUSBStartExit(rId, &s_elogThrUSB,
					ELOG_USBST_WRITEERROR, progress, tTime,
					ELOG_THSTATE_INCOMPLETE);
			flag = FALSE;
		}
	}
	else
	{
		/*Do nothing*/
	}

	if(flag)
	{
		snprintf(buf, sizeof(buf)-1, ELOG_USBSAVE_NAME, ELOG_FS_USB,
				serial, dateStr);

		/* RAM→NANDコピー実行 */
		ELOG_SaveLogRamToNand();

		/* 子プロセスID保存排他ロック */
		CMN_MutexLock(&s_elogUsbChildMutex);

		/* 子プロセスIDクリア */
		elog_SetUsbChild(0);

		/* 中断確認 */
		if(ELOG_GetUsbAbort() == TRUE)
		{
			/* 子プロセスID保存排他アンロック */
			CMN_MutexUnlock(&s_elogUsbChildMutex);
			progress = elog_GetUsbProgress();
			elog_ThrUSBStartExit(rId, &s_elogThrUSB,
					ELOG_USBST_ABORTED, progress, tTime, ELOG_THSTATE_ABORTED);
			flag = FALSE;
		}
	}
	else
	{
		/*Do nothing*/
	}

	if(flag)
	{
		/* 子プロセスfork() */
		child = fork();
		if(child == -1)
		{
			/* 子プロセスID保存排他アンロック */
			CMN_MutexUnlock(&s_elogUsbChildMutex);
			/* 異常終了イベント通知 */
			progress = elog_GetUsbProgress();
			elog_ThrUSBStartExit(rId, &s_elogThrUSB,
					ELOG_USBST_WRITEERROR, progress, tTime,
					ELOG_THSTATE_INCOMPLETE);
			flag = FALSE;
		}
	}
	else
	{
		/*Do nothing*/
	}

	if(flag)
	{
		if(child == 0)
		{
			/* 子プロセス処理 */
			DLOG_Error("child:exec tar+gz\n");

			/* デバッグ用
			sleep(30);
			*/

			/* 圧縮コマンド実行 */
			/*(tar czf /media/usb_storage/remocondata.tgz /mnt/log /mnt/user)*/
			execl("/bin/tar", "tar", "czf", buf, ELOG_FS_LOG, ELOG_FS_USR,
					"--exclude", ELOG_EXCLUDES_DIR_ENET, (schar*)NULL);
			DLOG_Error("child:execl() failed.\n");
			exit(-1);
		}
		else
		{
			/* 元プロセス処理 */

			/* 子プロセスID設定 */
			elog_SetUsbChild(child);

			/* 子プロセスID保存排他アンロック */
			CMN_MutexUnlock(&s_elogUsbChildMutex);

			/* 子プロセス終了待ち */
			DLOG_Error("waiting child(PID=%d)...\n", child);
			waitpid(child, &status, 0);
			DLOG_Error("child(PID=%d) finished!\n", child);

			/* 子プロセスID保存排他ロック */
			CMN_MutexLock(&s_elogUsbChildMutex);

			if(WIFEXITED(status))
			{
				/* 子プロセスはexit()で終了 */
				DLOG_Error("exit, status=%d\n", WEXITSTATUS(status));
				if(WEXITSTATUS(status) == 0)
				{
					status = 0;
				}
				else
				{
					/* tarコマンドでエラー発生時、WEXITSTATUS(status)は1となる */
					status = -1;
				}
			}
			else if
			(WIFSIGNALED(status))
			{
				/* 子プロセスはシグナルで終了 */
				DLOG_Error("signal, sig=%d\n", WTERMSIG(status));
				if(WTERMSIG(status) == SIGKILL)
				{
					status = -2;
				}
				else
				{
					status = -1;
				}
			}
			else
			{
				/* それ以外 */
				DLOG_Error("abnormal exit\n");
				status = -1;
			}

			/* 子プロセスIDクリア */
			elog_SetUsbChild(0);

			/* 子プロセスID保存排他アンロック */
			CMN_MutexUnlock(&s_elogUsbChildMutex);

			if(status == -1)
			{
				/* 異常終了イベント通知 */
				progress = elog_GetUsbProgress();
				elog_ThrUSBStartExit(rId, &s_elogThrUSB,
						ELOG_USBST_WRITEERROR, progress, tTime,
						ELOG_THSTATE_INCOMPLETE);
				flag = FALSE;
			}
			else if(status == -2)
			{
				/* 異常終了イベント通知 */
				progress = elog_GetUsbProgress();
				elog_ThrUSBStartExit(rId, &s_elogThrUSB,
						ELOG_USBST_ABORTED, progress, tTime,
						ELOG_THSTATE_ABORTED);
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
		/* 読み込み専用ファイル出力 */
		usbSt = elog_SaveReadSetting(serial);
		if(usbSt != ELOG_USBST_COMPLETE)
		{
			/* 異常終了イベント通知 */
			progress = elog_GetUsbProgress();
			if(usbSt != ELOG_USBST_ABORTED)
			{
				elog_ThrUSBStartExit(rId, &s_elogThrUSB, usbSt, 
									progress, tTime, ELOG_THSTATE_INCOMPLETE);
			}
			else
			{
				elog_ThrUSBStartExit(rId, &s_elogThrUSB, usbSt,
									progress, tTime, ELOG_THSTATE_ABORTED);
			}
			flag = FALSE;
		}
	}
	else
	{
		/*Do nothing*/
	}

	if(flag)
	{
		/* マルチリンクDCシステムログ保存 */
		ret = ELOG_DcsSavePcsLog();
		if(ret != ELOG_OK)
		{
			progress = elog_GetUsbProgress();
			elog_ThrUSBStartExit(rId, &s_elogThrUSB,
					ELOG_USBST_WRITEERROR, progress, tTime,
					ELOG_THSTATE_INCOMPLETE);
			flag = FALSE;
		}
		/* 正常終了イベント通知 */
		elog_SetUsbProgress(100);

		elog_SendStateEvent(rId, ELOG_USBST_INPROGRESS,
							elog_GetUsbProgress(), tTime);
		sleep(2);

		elog_ThrUSBStartExit(rId, &s_elogThrUSB,
				ELOG_USBST_COMPLETE, 100, tTime, ELOG_THSTATE_COMPLETE);
	}
	else
	{
		/*Do nothing*/
	}

	return NULL;
}

/* USB保存スレッドエラー処理　注意：USB保存スレッド以外からの呼び出し禁止 */
static void elog_ThrUSBStartExit(
		ulong rId,
		ElogThdata_t *thData,
		ElogUsbst_t	usbSt,
		uchar progress,
		ulong tTime,
		ElogThstate_t thrSt
)
{
	DEBUG_TRCIN();
	/* 進捗通知タイマーキャンセル */


	/* USB進捗イベント通知 */
	elog_SendStateEvent(rId, usbSt, progress, tTime);

	/* USBスレッドステータス更新 */
	if(thData != NULL)
	{
		ELOG_ThrState(thData, thrSt);
	}
	else
	{
		DLOG_Error("thData is NULL.\n");
	}

	/* ENSVへ圧縮要求許可イベントを通知 */
	ELOG_SendCompressEvent(ENSV_SUB_CPORT, ELOG_COMPRESS_ENABLE,
							ELOG_TYPE_NON, NULL, FALSE);

	CMN_MutexUnlock(&s_elogThrEXCLMutex);
	CMN_MutexUnlock(&s_elogThrUSB.dMutex);

	/* スレッド取り消し */
	pthread_detach(pthread_self());
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :圧縮処理スレッド                                         */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void *elog_ThrCompressStart(void *arg)
{
	ElogTgzSave_t *pData;
	schar tarCmd[ELOG_USBSAVE_CMD_LINE + 1];
	schar rmCmd[ELOG_USBSAVE_CMD_LINE + 1];
	slong ret;
	ulong rId;
	schar currentDir[ELOG_USBSAVE_CMD_LINE + 1];

	/* スレッド起動 */
	DEBUG_TRCIN();

	/* ロック */
	CMN_MutexLock(&s_elogThrEXCLMutex);

	ELOG_ThrState(&s_elogThrCompress, ELOG_THSTATE_RUN);

	pData = (ElogTgzSave_t *)s_elogThrCompress.data;

	memset(tarCmd, 0x0, sizeof(tarCmd));
	memset(rmCmd, 0x0, sizeof(tarCmd));

	rId = pData->header.destModId;

	if(pData->type == ELOG_TYPE_ACZ)
	{
		/* 圧縮コマンドライン */

		getcwd(currentDir, sizeof(currentDir));
		chdir(ELOG_ACTIONZ_DIR);

		snprintf(tarCmd, sizeof(tarCmd)-1, ELOG_CMD_TAR "%s %s",
				pData->fileName,
				ELOG_ACTION_DIR);

		/* 削除コマンドライン */
		snprintf(rmCmd, sizeof(rmCmd)-1, ELOG_CMD_RM "%s/*", ELOG_ACTION_DIR);
	}
	else
	{
		chdir(ELOG_SDZ_DIR);
		/* 圧縮コマンドライン */
		snprintf(tarCmd, sizeof(tarCmd)-1, ELOG_CMD_TAR "%s %s",
				pData->fileName, ELOG_SDP_DIR);
		/* 削除コマンドライン */
		snprintf(rmCmd, sizeof(rmCmd)-1, ELOG_CMD_RM "%s/*", ELOG_SDP_DIR);
	}

	/* 処理開始イベント */
	ELOG_SendCompressEvent(rId, ELOG_COMPRESS_START, pData->type, NULL, FALSE);


	/* NAND容量回復スレッド状態確認、中断 */
	ret = ELOG_AbortAndWaitNandDelete(TRUE);
	if(ret != ELOG_OK)
	{
		DLOG_Error("ELOG_AbortAndWaitNandDelete() error.\n");
		/* エラーイベント */
		ELOG_ThrState(&s_elogThrCompress, ELOG_THSTATE_INCOMPLETE);
		ELOG_SendCompressEvent(rId, ELOG_COMPRESS_OTHERR,
								pData->type, NULL, FALSE);
	}
	else
	{
		ret = ELOG_CALL_SYSTEM(tarCmd);
		if(ret != 0)
		{
			DLOG_Error("system(%s) failed.\n", tarCmd);
			/* 圧縮ファイル削除 */
			remove(pData->fileName);
			/* 圧縮エラーイベント */
			ELOG_ThrState(&s_elogThrCompress, ELOG_THSTATE_INCOMPLETE);
			ELOG_SendCompressEvent(rId, ELOG_COMPRESS_PRSERR,
									pData->type, NULL, FALSE);
		}
		else
		{
			if(pData->del == TRUE)
			{
				DLOG_Error("run [%s]\n", rmCmd);
				ret = ELOG_CALL_SYSTEM(rmCmd);
				if(ret != 0)
				{
					DLOG_Error("system(%s) failed.\n", rmCmd);
					/* 削除エラーイベント */
					ELOG_ThrState(&s_elogThrCompress, ELOG_THSTATE_INCOMPLETE);
					ELOG_SendCompressEvent(rId, ELOG_COMPRESS_DELERR, 
											pData->type, pData->fileName,
											pData->branched);
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
		}

		if(ret == 0)
		{
			/* 正常終了イベント */
			ELOG_ThrState(&s_elogThrCompress, ELOG_THSTATE_COMPLETE);
			ELOG_SendCompressEvent(rId, ELOG_COMPRESS_END, pData->type,
									pData->fileName, pData->branched);
		}
	}

	chdir(currentDir);

	/* アンロック */
	CMN_MutexUnlock(&s_elogThrEXCLMutex);

	/* スレッド取り消し */
	pthread_detach(pthread_self());

	/* スレッド終了 */
	DEBUG_TRCOUT();

	return NULL;
}

/*============================================================================*/
/*    Description    :スレッドステータス取得                                    */
/*----------------------------------------------------------------------------*/
/*    param          :ElogThdata_t *thData;	スレッドデータ                     */
/*----------------------------------------------------------------------------*/
/*    param          :ElogThstate_t *state;    ステータス返却用               */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常 (未ロック)                            */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_ThrGetState(ElogThdata_t *thData, ElogThstate_t *state)
{
	DEBUG_TRCIN();
	if(thData == NULL)
	{
		DLOG_Error("thData is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	if(state == NULL)
	{
		DLOG_Error("state is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	CMN_MutexLock(&(thData->sMutex));
	*state = thData->state;
	CMN_MutexUnlock(&(thData->sMutex));
	DEBUG_TRCOUT();
	return ELOG_OK;
}

/*============================================================================*/
/*    Description    :スレッドステータス変更                                    */
/*----------------------------------------------------------------------------*/
/*    param          :ElogThdata_t *thData;    スレッドデータ                  */
/*----------------------------------------------------------------------------*/
/*    param          :ElogThstate_t state;    変更ステータス                  */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常 (未ロック)                            */
/*                   :                                                        */
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/07  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_ThrState(ElogThdata_t *thData, ElogThstate_t state)
{
	DEBUG_TRCIN();
	if(thData == NULL)
	{
		DLOG_Error("thData is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	CMN_MutexLock(&(thData->sMutex));
	DLOG_Error("change state %d to %d\n", thData->state, state);
	thData->state = state;
	CMN_MutexUnlock(&(thData->sMutex));

	DEBUG_TRCOUT();
	return ELOG_OK;
}

/*============================================================================*/
/*    Description    :USB進捗ステータス通知                                     */
/*----------------------------------------------------------------------------*/
/*    param          :bool_t first; 初回実行フラグ                             */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/TRUE:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_thrSendUsbInProgress(bool_t first)
{
	slong ret;
	ulong rId;
	ulong tTime;
	uchar progress;
	DEBUG_TRCIN();
	/* スレッド実行中以外は何もしない */
	ret = ELOG_CheckSubThreadRun(ELOG_THTYPE_USB);
	if(ret != ELOG_E_BUSY)
	{
		/*Do nothing*/
	}
	else
	{
		CMN_MutexLock(&s_elogUsbProgressMutex);

		rId = ((ElogUsbSave_t *)(s_elogThrUSB.data))->msgData.header.destModId;
		tTime = ((ElogUsbSave_t *)(s_elogThrUSB.data))->tTime;

		if(first == TRUE)
		{
			s_elogUsbProgressTmr = 0;
		}
		else
		{
			if(s_elogUsbProgressTmr < tTime)
			{
				s_elogUsbProgressTmr++;
			}
		}

		progress = (uchar)(((double)s_elogUsbProgressTmr /
					(double)tTime) * 100.0);

		if(first == TRUE || (progress > s_elogUsbProgress && progress < 100))
		{
			elog_SendStateEvent(rId, ELOG_USBST_INPROGRESS, progress, tTime);
			s_elogUsbProgress = progress;
		}
		else
		{
			/*Do nothing*/
		}
		if(progress < 100)
		{
			/* タイマー予約(1sec) */
			FRM_TimerSetOneShot(ELOG_TIMER_USBSAVE_TMRID, 1000, NULL);
		}
		else
		{
			/*Do nothing*/
		}
		CMN_MutexUnlock(&s_elogUsbProgressMutex);
	}
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :USB保存子プロセス中断                                     */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_SetUsbAbort(bool_t abort)
{
	pid_t id;
	DEBUG_TRCIN();

	DLOG_Error("aborting usb save.\n");

	CMN_MutexLock(&s_elogAbortUsbFlag_mutex);
	s_elogAbortUsbFlag = abort;
	CMN_MutexUnlock(&s_elogAbortUsbFlag_mutex);

	if(abort == TRUE)
	{
		/* 子プロセスをkillする */
		id = elog_GetUsbChild();
		if(id != 0)
		{
			kill(id, SIGKILL);
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
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :USB中断フラグ取得                                        */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
bool_t ELOG_GetUsbAbort(void)
{
	bool_t ret;
	DEBUG_TRCOUT();

	CMN_MutexLock(&s_elogAbortUsbFlag_mutex);
	ret = s_elogAbortUsbFlag;
	CMN_MutexUnlock(&s_elogAbortUsbFlag_mutex);
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :ステータス通知イベント送信                                */
/*----------------------------------------------------------------------------*/
/*    param          :ulong port; 送信先プロセス(CPORT)                        */
/*----------------------------------------------------------------------------*/
/*                   :ElogThstate_t state; ステータス                          */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void elog_SendStateEvent(ulong port, ElogUsbst_t state,
							uchar progress, ulong tTime)
{
	DEBUG_TRCOUT();
	ElogProgress_t result;
	result.header.msgId = ELOG_EVT_USBPROGRESS;
	result.stat = state;
	result.progress = progress;
	result.time = tTime;

	DLOG_Error("send status %d, time %ld, progress %d\n",
				state, tTime, progress);
	FRM_MsgSendAsync(FRM_MsgSelfModId(), sizeof(result), &result);
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :圧縮通知イベント送信                                      */
/*----------------------------------------------------------------------------*/
/*    param          :ulong port; 送信先プロセス(CPORT)                         */
/*----------------------------------------------------------------------------*/
/*    param          :ElogCompress_t state; ステータス                        */
/*----------------------------------------------------------------------------*/
/*    param          :ElogType_t type; ログタイプ                             */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/TRUE:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/12  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_SendCompressEvent(ulong port, ElogCompress_t state, ElogType_t type,
							schar* savepath, bool_t branched)
{
	ElogCompressAction_t result;
	DEBUG_TRCOUT();
	result.header.msgId = ELOG_EVT_COMPRESS;
	result.stat = state;
	result.type = type;
	result.branched = branched;

	memset(result.savepath, 0x0, sizeof(result.savepath));
	if(savepath != NULL)
	{
		strncpy(result.savepath, savepath, sizeof(result.savepath) - 1);
	}

	FRM_MsgSendAsync(FRM_MsgSelfModId(), sizeof(result), &result);
	DEBUG_TRCOUT();
}
/*============================================================================*/
/*    Description    :NAND空き容量回復スレッド                                  */
/*----------------------------------------------------------------------------*/
/*    param          :void *arg;    レッド管理データ                           */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void *elog_ThrDeleteNandLog(void *arg)
{
	slong ret;
	ulong avail;
	ulong used;
	ulong all;
	double fAvail;
	double fAll;
	ulong i;
	bool_t flag = TRUE;
	bool_t tmpFlag = TRUE;
	ElogFileList_t nameList;
	schar *dptr;
	schar *cptr;
	schar limitDate[ELOG_DATESTR_LEN + 1];
	bool_t limitFlag = FALSE;

	ElogType_t typeTab[ELOG_TYPE_NUM] = {
		ELOG_TYPE_ACZ, ELOG_TYPE_SDZ, 0, ELOG_TYPE_SDP
		};

	CMN_MutexLock(&s_elogThrNandDelMutex);

	nameList.list = NULL;
	nameList.size = 0;
	DEBUG_TRCIN();

	ELOG_ThrState(&s_elogThrNandDel, ELOG_THSTATE_RUN);

	/* LOGパーティション空きサイズ取得 */
	ret = ELOG_GetPartitionInfoMB(ELOG_FS_LOG, &avail, &used, &all);
	if(ret != ELOG_OK)
	{
		DLOG_Error("elog_getPartitionAvailMB(%s) error.\n", ELOG_FS_LOG);
		ELOG_ThrState(&s_elogThrNandDel, ELOG_THSTATE_INCOMPLETE);
		flag = FALSE;
	}
	else
	{
		fAvail = (double)avail;	/* 空き容量 */
		fAll = (double)all;		/* 全容量 */

		/* 回復容量まで回復していれば終了 */
		if(fAvail > fAll * ELOG_NAND_RCOV)
		{
			ELOG_ThrState(&s_elogThrNandDel, ELOG_THSTATE_COMPLETE);
			flag = FALSE;
		}
		else
		{
			/*Do nothing*/
		}
	}

	if(flag)
	{
		/* 日付リミット条件を初期化 */
		memset(limitDate, 0x0, sizeof(limitDate));
		limitDate[0] = '*';

		/* 各ログのディレクトリ内の削除候補ファイルリスト取得・結合 */
		for(i = 0; i < 4; i++)
		{
			ret = elog_ScanLogWithLimit(typeTab[i], limitDate, &nameList, &limitFlag);
			if(ret != ELOG_OK)
			{
				DLOG_Error("ELOG_ScanFiles() error.\n");
				ELOG_ThrState(&s_elogThrNandDel, ELOG_THSTATE_INCOMPLETE);
				flag = FALSE;
			}
			else
			{
				DLOG_Error("elog_ScanLogWithLimit() : lD[%s], lF[%d]\n",
							limitDate, limitFlag);
			}
		}
	}
	else
	{
		/*Do nothing*/
	}

	if(flag)
	{
		if(nameList.list != NULL && nameList.size > 0)
		{
			/* 古い順に並べ替え */
			qsort(nameList.list, nameList.size, sizeof(struct dirent *),
					ELOG_FileDateComp);

			DLOG_Error("nameList size = %ld\n", nameList.size);
			/* 古いファイルから削除 */
			cptr = NULL;
			dptr = NULL;
			for(i = 0; i < nameList.size; i++)
			{
				/* 削除ファイル名称内の日付取得 */
				ret = ELOG_GetFileDate(nameList.list[i]->d_name, &cptr);
				if(ret != ELOG_OK)
				{
					DLOG_Error("ELOG_GetFileDate() error.\n");
				}
				else
				{
					/*Do nothing*/
				}
				/* 同日のファイルは続けて削除 */
				if((ret == ELOG_OK) && (dptr != NULL))
				{
					ret = strncmp(dptr, cptr, ELOG_DATESTR_LEN);
					if(ret != 0)
					{
						/* LOGパーティション空きサイズ取得 */
						ret = ELOG_GetPartitionInfoMB(ELOG_FS_LOG,
														&avail, &used, &all);
						if(ret != ELOG_OK)
						{
							DLOG_Error("get(%s) error.\n", ELOG_FS_LOG);
							ELOG_ThrState(&s_elogThrNandDel,
												ELOG_THSTATE_INCOMPLETE);
							i = nameList.size;
							tmpFlag = FALSE;
						}
						else
						{
							fAvail = (double)avail;	/* 空き容量 */
							fAll = (double)all;		/* 全容量 */

							/* 回復容量まで回復していれば終了 */
							if(fAvail > fAll * ELOG_NAND_RCOV)
							{
								ELOG_ThrState(&s_elogThrNandDel,
													ELOG_THSTATE_COMPLETE);
								i = nameList.size;
								tmpFlag = FALSE;
							}
						}
					}
					else
					{
						/* 日付が同じ間は継続する : NOP */
					}
				}
				else
				{
					/*Do nothing*/
				}
				if(tmpFlag)
				{
					dptr = cptr;
				}
				else
				{
					/*Do nothing*/
				}
				/* ファイル削除実行 */
				ret = remove(nameList.list[i]->d_name);
				if(ret == -1)
				{
					if(errno != ENOENT)
					{
						DLOG_Error("remove() is failed.(%d)\n", errno);
						ELOG_ThrState(&s_elogThrNandDel,
											ELOG_THSTATE_INCOMPLETE);
						i = nameList.size;
						tmpFlag = FALSE;
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
				if(tmpFlag)
				{
					/* 中断要求確認 */
					if(ELOG_GetNandDeleteAbort() == TRUE)
					{
						ELOG_ThrState(&s_elogThrNandDel,
											ELOG_THSTATE_ABORTED);
						i = nameList.size;
						tmpFlag = FALSE;
					}
					else
					{
						/*Do nothing*/
					}
				}
			}
			if(i >= nameList.size)
			{
				DLOG_Error("all list deleted.\n");
				if(limitFlag == TRUE)
				{
					/* 件数上限により制限された場合は、*/
					/*中断と同じ扱いとする（次回継続）*/
					ELOG_ThrState(&s_elogThrNandDel, ELOG_THSTATE_ABORTED);
				}
				else
				{
					ELOG_ThrState(&s_elogThrNandDel, ELOG_THSTATE_COMPLETE);
				}
			}
		}
		else
		{
			ELOG_ThrState(&s_elogThrNandDel, ELOG_THSTATE_INCOMPLETE);
		}
	}
	else
	{
		/*Do nothing*/
	}
	/* ソート結果を開放する */
	ELOG_DirentFree(&nameList.list, nameList.size);

	CMN_MutexUnlock(&s_elogThrNandDelMutex);

	/* スレッド終了 */
	pthread_detach(pthread_self());

	DEBUG_TRCOUT();
	return NULL;
}

/*============================================================================*/
/*    Description    :NAND容量回復スレッド中断フラグ設定                         */
/*----------------------------------------------------------------------------*/
/*    param          :bool_t    f;    設定値                                  */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_SetNandDeleteAbort(bool_t f)
{
	DEBUG_TRCIN();
	DLOG_Error("nand delete abort flag %d to %d\n", s_elogAbortNandDelFlag, f);
	CMN_MutexLock(&s_elogAbortNandDelFlagMutex);
	s_elogAbortNandDelFlag = f;
	CMN_MutexUnlock(&s_elogAbortNandDelFlagMutex);
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :NAND容量回復スレッド中断フラグ取得                         */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    bool_t    フラグ値                                       */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
bool_t ELOG_GetNandDeleteAbort(void)
{
	bool_t f;
	DEBUG_TRCIN();

	CMN_MutexLock(&s_elogAbortNandDelFlagMutex);
	f = s_elogAbortNandDelFlag;
	CMN_MutexUnlock(&s_elogAbortNandDelFlagMutex);
	DEBUG_TRCOUT();
	return f;
}

/*============================================================================*/
/*    Description    :NAND容量回復スレッド中断＆停止待ち                         */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                    ELOG_OK : 正常終了 / slong    処理結果                   */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_AbortAndWaitNandDelete(bool_t wait)
{
	slong ret = ELOG_OK;
	DEBUG_TRCIN();

	/* スレッド実行中以外は何もしない */
	ret = ELOG_CheckSubThreadRun(ELOG_THTYPE_NAND);
	if(ret == ELOG_E_BUSY)
	{
		/* NAND容量回復スレッド停止要求 */
		ELOG_SetNandDeleteAbort(TRUE);

		/* NAND容量回復スレッド停止待ち */
		if(wait == TRUE)
		{
			CMN_MutexLock(&s_elogThrNandDelMutex);
			CMN_MutexUnlock(&s_elogThrNandDelMutex);
		}
		else
		{
			/*Do nothing*/
		}
	}
	else if(ret != ELOG_OK)
	{
		DLOG_Error("ELOG_CheckSubThreadRun() error.\n");
		ret = ELOG_E_PROCS;
	}
	else
	{
		/*Do nothing*/
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :子プロセスID設定                                         */
/*----------------------------------------------------------------------------*/
/*    param          :pid_t id; 子プロセスID                                   */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/TRUE:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static void elog_SetUsbChild(pid_t id)
{
	DEBUG_TRCIN();
	/* 排他ロック制御は呼び出し側で行う */
	s_elogUsbChild = id;
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :子プロセスID取得                                         */
/*----------------------------------------------------------------------------*/
/*    param          :pid_t; 子プロセスID                                      */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/TRUE:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static pid_t elog_GetUsbChild(void)
{
	pid_t id;
	DEBUG_TRCIN();
	CMN_MutexLock(&s_elogUsbChildMutex);
	id = s_elogUsbChild;
	CMN_MutexUnlock(&s_elogUsbChildMutex);
	DEBUG_TRCOUT();
	return id;
}

/*============================================================================*/
/*    Description    :USB保存進捗率排他設定                                    */
/*----------------------------------------------------------------------------*/
/*    param          :uchar progress; 進捗率                                  */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/TRUE:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static void elog_SetUsbProgress(uchar progress)
{
	DEBUG_TRCIN();
	CMN_MutexLock(&s_elogUsbProgressMutex);
	s_elogUsbProgress = progress;
	CMN_MutexUnlock(&s_elogUsbProgressMutex);
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :子プロセスID排他アクセス用関数                            */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    uchar; 進捗率                                            */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static uchar elog_GetUsbProgress(void)
{
	DEBUG_TRCIN();
	uchar progress;
	CMN_MutexLock(&s_elogUsbProgressMutex);
	progress = s_elogUsbProgress;
	CMN_MutexUnlock(&s_elogUsbProgressMutex);
	DEBUG_TRCOUT();
	return progress;
}

/*============================================================================*/
/*    Description    :PCSログファイル検索                                       */
/*----------------------------------------------------------------------------*/
/*    param          :ElogType_t type : ログ種別                               */
/*----------------------------------------------------------------------------*/
/*                   :schar *dateLimit	: 検索ファイル日付上限                  */
/*----------------------------------------------------------------------------*/
/*                   :ElogFileList_t *nameList : ディレクトリ構造体（結果格納）  */
/*----------------------------------------------------------------------------*/
/*                   :schar *date : 上限ファイル日付（返却値）                  */
/*----------------------------------------------------------------------------*/
/*                   :bool_t *flag : 日付上限判定                              */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/TRUE:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_ScanLogWithLimit(ElogType_t type, schar *date,
									ElogFileList_t *nameList, bool_t *flag)
{
	slong ret = ELOG_OK;
	struct dirent **getList = NULL;
	struct dirent **tempList = NULL;
	int n;
	ulong i;
	ulong j;
	bool_t tmpFlag = TRUE;
	schar *pStr;
	ulong addSize;
	ulong sizeSum;
	schar *path = NULL;
	DEBUG_TRCIN();

	if(nameList == NULL)
	{
		DLOG_Error("list is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		if(date == NULL)
		{
			DLOG_Error("list is NULL.\n");
			DEBUG_TRCOUT();
			return ELOG_E_PARAM;
		}
		else
		{
			if(flag == NULL)
			{
				DLOG_Error("flag is NULL.\n");
				DEBUG_TRCOUT();
				return ELOG_E_PARAM;
			}
			else
			{
				/*Do nothing*/
			}
		}
	}

	if(type == ELOG_TYPE_ACZ)
	{
		path = ELOG_ACTION_DIR;
	}
	else if(type == ELOG_TYPE_SDZ)
	{
		path = ELOG_SDZ_DIR;
	}
	else if(type == ELOG_TYPE_OPT_INFORM)
	{
		path = ELOG_ERR_EVENT_DIR;
	}
	else if(type == ELOG_TYPE_SDP)
	{
		path = ELOG_SDP_DIR;
	}
	else
	{
		DLOG_Error("unknown type (%d).\n", type);
		ret = ELOG_E_PARAM;
		tmpFlag = FALSE;
	}
	if(tmpFlag)
	{
		ret = elog_SetLogDateLimit(type, date);
		if(ret != ELOG_OK)
		{
			DLOG_Error("elog_SetLogDateLimit() error.\n");
			ret = ELOG_E_PROCS;
			tmpFlag = FALSE;
		}
		else
		{
			n = scandir(path, &getList, elog_DirentFilterDateLimit,
						elog_LogDateSort);
			if(n == 0)
			{
				/* 下位ディレクトリ無し */
				ELOG_DirentFree(&getList, n);
				ret = ELOG_OK;
				tmpFlag = FALSE;
			}
			else if(n < 0)
			{
				if(errno == ENOENT)
				{
					/* エントリ無し */
					ret = ELOG_OK;
					tmpFlag = FALSE;
				}
				else
				{
					DLOG_Error("scandir() error.(%d,%d)\n", n, errno);
					ret = ELOG_E_SYSCAL;
				}
			}
			if(n >= ELOG_LOGSCAN_LIMIT)
			{
				/* 上限を超えた場合、上限要素のファイル日付を取得 */
				ret = elog_GetLogFileDate(s_elogLogDateLimitType,
										getList[ELOG_LOGSCAN_LIMIT-1]->d_name,
										&pStr);
				if(ret != ELOG_OK || pStr == NULL)
				{
					DLOG_Error("elog_GetLogFileDate() error.");
					ELOG_DirentFree(&getList, n);
					ret = ELOG_E_PROCS;
					tmpFlag = FALSE;
				}
				else
				{
					/* 上限ファイル日付上限を更新 */
					strncpy(date, pStr, ELOG_DATESTR_LEN);
					addSize = ELOG_LOGSCAN_LIMIT;
					*flag = TRUE;
				}
			}
			else
			{
				addSize = n;
			}
		}
	}
	else
	{
		/*Do nothing*/
	}

	if(tmpFlag)
	{
		/* 結果をフルパス形式にして結合する */

		/* 現在のリストサイズ＋取得したリストサイズ分の一時リストを作成 */
		sizeSum = nameList->size + addSize;
		tempList = ELOG_MALLOC(sizeof(struct dirent *) * (sizeSum));
		if(tempList == NULL)
		{
			DLOG_Error("ELOG_MALLOC(newlist array) error.\n");
			ELOG_DirentFree(&getList, n);
			ret = ELOG_E_SYSCAL;
			tmpFlag = FALSE;
		}
		else
		{
			/* 現在のリストの内容を一時リストへコピー */
			for(i = 0; i < nameList->size; i++)
			{
				tempList[i] = ELOG_MALLOC(sizeof(struct dirent));
				if(tempList[i] == NULL)
				{
					DLOG_Error("ELOG_MALLOC(org copy area) error.\n");
					ELOG_DirentFree(&getList, n);
					ELOG_DirentFree(&tempList, i);
					ret = ELOG_E_SYSCAL;
					tmpFlag = FALSE;
				}
				else
				{
					memcpy(tempList[i], nameList->list[i],
							sizeof(struct dirent));
				}
			}
		}
	}
	else
	{
		/*Do nothing*/
	}

	if(tmpFlag)
	{
		/* 取得したリストの内容をフルパス形式にして一時リストへコピー */
		for(j = 0; j < addSize; j++, i++)
		{
			tempList[i] = ELOG_MALLOC(sizeof(struct dirent));
			if(tempList[i] == NULL)
			{
				DLOG_Error("ELOG_MALLOC(new add area) error.\n");
				ELOG_DirentFree(&getList, n);
				ELOG_DirentFree(&tempList, i);
				ret = ELOG_E_SYSCAL;
				tmpFlag = FALSE;
			}
			else
			{
				memcpy(tempList[i], getList[j], sizeof(struct dirent));
				snprintf(tempList[i]->d_name, sizeof(tempList[i]->d_name),
						"%s/%s", path, getList[j]->d_name);
				/* free addlist area by caller. */
			}
		}
		/* 取得したリストを開放 */
		ELOG_DirentFree(&getList, n);

		/* 現在のリストを開放 */
		ELOG_DirentFree(&(nameList->list), nameList->size);
		/* 一時リストを現在のリストにする */
		nameList->list = tempList;
		nameList->size = i;
	}
	else
	{
		/*Do nothing*/
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :ログファイル日付上限フィルタ範囲設定                       */
/*----------------------------------------------------------------------------*/
/*    param          :ElogType_t type;                                        */
/*----------------------------------------------------------------------------*/
/*                   :schar *date;                                            */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    サブスレッド未実行                          */
/*----------------------------------------------------------------------------*/
/*                    E_PARAM(-1)    パラメータエラー                          */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_SetLogDateLimit(ElogType_t type, schar *date)
{
	ulong i;
	slong ret = ELOG_OK;
	DEBUG_TRCIN();

	if(date == NULL)
	{
		DLOG_Error("end is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		memset(s_elogLogDateLimit, 0x0, sizeof(s_elogLogDateLimit));
		if(*date == '*')
		{
			/* 上限指定なしとする */
			s_elogLogDateLimit[0] = '*';
		}
		else
		{
			/* 数字以外が有る場合はエラーとする */
			for(i=0; i < ELOG_DATESTR_LEN; i++)
			{
				if(! isdigit((int)*(date+i)))
				{
					DLOG_Error("date has not digit.\n");
					ret = ELOG_E_PARAM;
					i = ELOG_DATESTR_LEN;
				}
				else
				{
					/*Do nothing*/
				}
			}
			strncpy(s_elogLogDateLimit, date, sizeof(s_elogLogDateLimit) - 1);
		}

		s_elogLogDateLimitType = type;
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :ファイル日付上限フィルタ処理                              */
/*----------------------------------------------------------------------------*/
/*    param          :const struct dirent *ent                                */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    0 : 範囲外/1 : 範囲内                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static ulong elog_DirentFilterDateLimit(const struct dirent *ent)
{
	schar *pStr = NULL;
	sint ret = 1;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DLOG_Error("ent is NULL.\n");
		DEBUG_TRCOUT();
		return 0;
	}

	if(ent->d_type == DT_DIR)
	{
		DEBUG_TRCOUT();
		return 0;
	}

	ret = elog_GetLogFileDate(s_elogLogDateLimitType,
							(schar *)(ent->d_name), &pStr);
	if(ret != ELOG_OK || pStr == NULL)
	{
		DLOG_Error("file date get(%s) error.\n", ent->d_name);
		ret = 0;
	}
	else
	{
		if(s_elogLogDateLimit[0] != '*')
		{
			/* 期日終了指定有り */
			ret = strncmp(pStr, s_elogLogDateLimit, ELOG_DATESTR_LEN);
			if(ret > 0)
			{
				ret = 0;
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
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :ログファイル名日付ソート                                  */
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent **entA : ディレクトリエントリ               */
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent **entB : ディレクトリエントリ               */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    -1 : entA < entB/ 0 : entA = entB/1 : entA > entB       */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static ulong elog_LogDateSort(const struct dirent **entA,
							const struct dirent **entB)
{
	schar *pA;
	schar *pB;
	sint ret;
	DEBUG_TRCIN();

	if(entA == NULL)
	{
		DEBUG_TRCOUT();
		/* ファイル名規則に合わない場合はリストの最後尾にする */
		return 1;
	}
	else
	{
		if(entB == NULL)
		{
			DEBUG_TRCOUT();
			/* ファイル名規則に合わない場合はリストの最後尾にする */
			return -1;
		}
		else
		{
			if(*entA == NULL)
			{
				DEBUG_TRCOUT();
				/* ファイル名規則に合わない場合はリストの最後尾にする */
				return 1;
			}
			else
			{
				if(*entB == NULL)
				{
					DEBUG_TRCOUT();
					/* ファイル名規則に合わない場合はリストの最後尾にする */
					return -1;
				}
				else
				{
					/*Do nothing*/
				}
			}
		}
	}

	ret = elog_GetLogFileDate(s_elogLogDateLimitType,
							(*(struct dirent **)(entA))->d_name, &pA);
	if(ret != ELOG_OK)
	{
		ret = 1;	/* ファイル名規則に合わない場合はリストの最後尾にする */
	}
	else
	{
		ret = elog_GetLogFileDate(s_elogLogDateLimitType,
								(*(struct dirent **)(entB))->d_name, &pB);
		if(ret != ELOG_OK)
		{
			ret = -1;	/* ファイル名規則に合わない場合はリストの最後尾にする */
		}
		else
		{
			ret = strncmp(pA, pB, ELOG_DATESTR_LEN);
		}
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :ログファイル名の日付部分を取得する                         */
/*----------------------------------------------------------------------------*/
/*    param          :schar *FileName : ログファイル名（フルパス）              */
/*----------------------------------------------------------------------------*/
/*    param          :schar **date : 日付部分の先頭ポインタ                     */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常終了                                  */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_GetLogFileDate(ElogType_t type, schar *FileName, schar **date)
{
	slong ret;
	ulong i;

	if(FileName == NULL)
	{
		DLOG_Error("FileName is NULL\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		if(date == NULL)
		{
			DLOG_Error("date is NULL\n");
			DEBUG_TRCOUT();
			return ELOG_E_PARAM;
		}
		else
		{
			/*Do nothing*/
		}
	}

	switch(type)
	{
	case ELOG_TYPE_OPT_INFORM:
		ret = ELOG_PcsLogGetFileDate(FileName, date);
		break;
	case ELOG_TYPE_SDP:
		ret = ELOG_SdpLogGetFileDate(FileName, date);
		break;
	case ELOG_TYPE_ACZ:
		ret = ELOG_SvrLogGetFileDate(FileName, date);
		break;
	case ELOG_TYPE_SDZ:
		ret = ELOG_SdzLogGetFileDate(FileName, date);
		break;
	default:
		DLOG_Error("unknown log type.(%d)\n", type);
		ret = ELOG_E_PARAM;
		break;
	}

	if(ret == ELOG_OK)
	{
		/* 日付文字列内の文字は数字でなければならない */
		for(i=0; i<ELOG_DATESTR_LEN; i++)
		{
			if(!isdigit((int)*((*date)+i)))
			{
				ret = ELOG_E_PROCS;
			}
			else
			{
				/*Do nothing*/
			}
		}
	}
	else
	{
		/*Do nothing*/
	}

	return ret;
}

/*============================================================================*/
/*    Description    :USB保存、ログ圧縮サブスレッド状態確認                      */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    サブスレッド未実行                          */
/*                                                                            */
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*                                                                            */
/*                    ELOG_E_BUSY(-6)    サブスレッド実行中                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_CheckSubThreadRun(ulong mask)
{
	slong ret = ELOG_OK;
	bool_t flag = TRUE;
	ElogThstate_t state;
	DEBUG_TRCIN();

	/* USB保存スレッド状態確認 */
	if(mask & ELOG_THTYPE_USB)
	{
		ret = ELOG_ThrGetState(&s_elogThrUSB, &state);
		if(ret != ELOG_OK)
		{
			DLOG_Error("ELOG_ThrGetState() error.\n");
			ret = ELOG_E_PROCS;
			flag = FALSE;
		}
		else
		{
			if(state == ELOG_THSTATE_START || state == ELOG_THSTATE_RUN)
			{
				DLOG_Error("USB copy thread busy.\n");
				ret = ELOG_E_BUSY;
				flag = FALSE;
			}
			else
			{
				/*Do nothing*/
			}
		}
	}
	else
	{
		/*Do nothing*/
	}
	if(flag)
	{
		/* ログ圧縮スレッド状態確認 */
		if(mask & ELOG_THTYPE_CPRS)
		{
			ret = ELOG_ThrGetState(&s_elogThrCompress, &state);
			if(ret != ELOG_OK)
			{
				DLOG_Error("ELOG_ThrGetState() error.\n");
				ret = ELOG_E_PROCS;
				flag = FALSE;
			}
			else
			{
				if(state == ELOG_THSTATE_START || state == ELOG_THSTATE_RUN)
				{
					DLOG_Error("compress thread busy.\n");
					ret = ELOG_E_BUSY;
					flag = FALSE;
				}
				else
				{
					/*Do nothing*/
				}
			}
		}
		else
		{
			/*Do nothing*/
		}
	}

	if(flag)
	{
		/* NAND自動回復スレッド状態確認 */
		if(mask & ELOG_THTYPE_NAND)
		{
			ret = ELOG_ThrGetState(&s_elogThrNandDel, &state);
			if(ret != ELOG_OK)
			{
				DLOG_Error("ELOG_ThrGetState() error.\n");
				ret = ELOG_E_PROCS;
				flag = FALSE;
			}
			else
			{
				if(state == ELOG_THSTATE_START || state == ELOG_THSTATE_RUN)
				{
					DLOG_Error("NandDel thread busy.\n");
					ret = ELOG_E_BUSY;
					flag = FALSE;
				}
				else
				{
					/*Do nothing*/
				}
			}
		}
		else
		{
			/*Do nothing*/
		}
	}

	if(flag)
	{
		/* ログリスト取得スレッド状態確認 */
		if(mask & ELOG_THTYPE_LLST)
		{
			ret = ELOG_ThrGetState(&s_elogThrGetLogList, &state);
			if(ret != ELOG_OK)
			{
				DLOG_Error("ELOG_ThrGetState() error.\n");
				ret = ELOG_E_PROCS;
				flag = FALSE;
			}
			if(state == ELOG_THSTATE_START || state == ELOG_THSTATE_RUN)
			{
				DLOG_Error("GetLogList thread busy.\n");
				ret = ELOG_E_BUSY;
				flag = FALSE;
			}
		}
		else
		{
			/*Do nothing*/
		}
	}

	if(flag)
	{
		/* ログ削除スレッド状態確認 */
		if(mask & ELOG_THTYPE_LDEL)
		{
			ret = ELOG_ThrGetState(&s_elogThrNandDel, &state);
			if(ret != ELOG_OK)
			{
				DLOG_Error("ELOG_ThrGetState() error.\n");
				ret = ELOG_E_PROCS;
			}
			else
			{
				if(state == ELOG_THSTATE_START || state == ELOG_THSTATE_RUN)
				{
					DLOG_Error("NandDel thread busy.\n");
					ret = ELOG_E_BUSY;
				}
				else
				{
					/*Do nothing*/
				}
			}
		}
		else
		{
			/*Do nothing*/
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :ログ一覧取得スレッド                                      */
/*----------------------------------------------------------------------------*/
/*    param          :void *arg : スレッドデータ（未使用）                      */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/12  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void *elog_ThrGetLogList(void *arg)
{
	struct dirent **nameList = NULL;
	sint n = 0;
	bool_t flag = TRUE;
	schar *path;
	ulong i;
	slong ret;
	bool_t rep = TRUE;
	bool_t fnd = TRUE;

	ElogFileList_t othList;
	ElogFileList_t dirList;

	ElogFncGetLogList_t *param;
	
	bool_t exclOth = FALSE;

	DEBUG_TRCIN();
	/* スレッドロック */
	CMN_MutexLock(&s_elogThrGetLogListMutex);

	param = (ElogFncGetLogList_t *)(s_elogThrGetLogList.data);
	if(param == NULL)
	{
		DLOG_Error("param is NULL.\n");
		/* paramがNULLの場合通知を含め動作できない */
		ELOG_ThrState(&s_elogThrGetLogList, ELOG_THSTATE_INCOMPLETE);
		CMN_MutexUnlock(&s_elogThrGetLogListMutex);
		pthread_detach(pthread_self());
		flag = FALSE;
	}
	else
	{
		/*Do nothing*/
	}

	if(flag)
	{
		/* スレッドステータス⇒RUN */
		ELOG_ThrState(&s_elogThrGetLogList, ELOG_THSTATE_RUN);
		/* 処理開始イベント */
		elog_sendGetLogListEvent(param->header.destModId,
								ELOG_GETLOGLIST_START);

		memset(&othList, 0x0, sizeof(othList));
		memset(&dirList, 0x0, sizeof(dirList));
		/* その他ログ */
		if(param->type == ELOG_TYPE_OTH)
		{
			/* デバッグログ */
			ret = ELOG_ScanFiles(ELOG_LOG_DIR, &othList, ELOG_SCANDIR_FILES);
			if(ret != ELOG_OK)
			{
				DLOG_Error("ELOG_ScanFiles(%s, ELOG_SCANDIR_FILES) error.\n",
							ELOG_LOG_DIR);
				rep = FALSE;
				flag = FALSE;
			}
			else
			{
				/*Do nothing*/
			}
			if(flag)
			{
				/* キーログ */
				ret = ELOG_ScanFiles(ELOG_OPERATION_DIR, &othList,
									ELOG_SCANDIR_FILES);
				if(ret != ELOG_OK)
				{
					DLOG_Error("%s, ELOG_SCANDIR_FILES error.\n",
								ELOG_OPERATION_DIR);
					rep = FALSE;
					flag = FALSE;
				}
			}
			else
			{
				/*Do nothing*/
			}
			if(flag)
			{
				/* ダウンロードログ */
				ret = ELOG_ScanFiles(ELOG_NDL_DIR, &othList,
									ELOG_SCANDIR_FILES);
				if(ret != ELOG_OK)
				{
					DLOG_Error("%s, ELOG_SCANDIR_FILES error.\n",
								ELOG_NDL_DIR);
					rep = FALSE;
					flag = FALSE;
				}
			}
			else
			{
				/*Do nothing*/
			}

			if(flag)
			{
				/* 設定パーティション以下ディレクトリ */
				ret = ELOG_ScanFiles(ELOG_FS_USR, &dirList, ELOG_SCANDIR_DIRS);
				if(ret != ELOG_OK)
				{
					DLOG_Error("%s, ELOG_SCANDIR_DIRS error.\n", ELOG_FS_USR);
					rep = FALSE;
					flag = FALSE;
				}
			}
			else
			{
				/*Do nothing*/
			}

			if(flag)
			{
				/* 設定パーティション直下ファイル */
				ret = ELOG_ScanFiles(ELOG_FS_USR, &othList,
									ELOG_SCANDIR_EXTMPS);
				if(ret != ELOG_OK)
				{
					DLOG_Error("%s, ELOG_SCANDIR_FILES error.\n", ELOG_FS_USR);
					rep = FALSE;
					flag = FALSE;
				}
			}
			else
			{
				/*Do nothing*/
			}

			if(flag)
			{
				/* 設定パーティションサブディレクトリ配下 */
				for(i = 0; i < dirList.size; i++)
				{
					/* 除外ディレクトリ判定 */
					exclOth = elog_ChkExclDirOThList(dirList.list[i]->d_name);
					if(TRUE == exclOth)
					{
						/*Do nothing*/
					}
					else
					{
						/* 除外対象でなければ該当ディレクトリ内のファイルをリストに追加 */
						ret = ELOG_ScanFiles(dirList.list[i]->d_name, &othList,
											ELOG_SCANDIR_EXTMPS);
						if(ret != ELOG_OK)
						{
							DLOG_Error("%s, ELOG_SCANDIR_FILES error.\n",
										dirList.list[i]->d_name);
							rep = FALSE;
							flag = FALSE;
							i = dirList.size;
						}
						else
						{
							/*Do nothing*/
						}
					}
				}
			}

			if(flag)
			{
				/* 不要なのでディレクトリリストを開放 */
				ELOG_DirentFree(&(dirList.list), dirList.size);
				if(othList.size == 0)
				{
					DLOG_Error("no file there.\n");
					rep = FALSE;
					fnd = FALSE;
					flag = FALSE;
				}
				else
				{
					/* 昇順ソート */
					qsort(othList.list, othList.size,
						sizeof(struct dirent *), elog_AlphaSort);
					/* リストをファイルへ上書き保存 */
					ret = elog_WriteListToFile(ELOG_LOGLISTPATH,
												othList.list, othList.size);
					if(ret != ELOG_OK)
					{
						rep = FALSE;
						flag = FALSE;
					}
				}
			}
		}
		else
		{
			if(param->type == ELOG_TYPE_ACZ)
			{
				path = ELOG_ACTION_DIR;
			}
			else if(param->type == ELOG_TYPE_SDZ)
			{
				path = ELOG_SDZ_DIR;
			}
			else
			{
				rep = FALSE;
				flag = FALSE;
			}
			if(flag)
			{
				/* 日付フィルタ条件排他ロック */
				CMN_MutexLock(&s_elogDateRangeMutex);

				/* 開始～終了日設定 */
				ret = elog_SetFilterDateRange(param->type,
											param->begin, param->end);
				if(ret != ELOG_OK)
				{
					DLOG_Error("elog_SetFilterDateRange() error.\n");
					CMN_MutexUnlock(&s_elogDateRangeMutex);
					rep = FALSE;
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
				/* ファイル検索 */
				n = scandir(path, &nameList, elog_DirentFilterDateRange,
							elog_SortSvrLogList);

				/* 日付フィルタ条件排他アンロック */
				CMN_MutexUnlock(&s_elogDateRangeMutex);

				if(n < 0)
				{
					DLOG_Error("scandir() error.(%d,%d)\n", n, errno);
					rep = FALSE;
					flag = FALSE;
				}
				else if(n == 0)
				{
					DLOG_Error("no file there.\n");
					rep = FALSE;
					fnd = FALSE;
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
				/* リストをファイルへ上書き保存 */
				ret = elog_WriteListToFile(ELOG_LOGLISTPATH, nameList, n);
				if(ret != ELOG_OK)
				{
					DLOG_Error("elog_WriteListToFile() error.\n");
					rep = FALSE;
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
		}
	}
	else
	{
		/*Do nothing*/
	}

	/* リスト開放 */
	if(dirList.list != NULL)
	{
		ELOG_DirentFree(&(dirList.list), dirList.size);
	}
	if(othList.list != NULL)
	{
		ELOG_DirentFree(&(othList.list), othList.size);
	}
	if(nameList != NULL)
	{
		ELOG_DirentFree(&nameList, n);
	}

	if(rep == TRUE)
	{
		/* スレッドステータスCOMPLETE */
		ELOG_ThrState(&s_elogThrGetLogList, ELOG_THSTATE_COMPLETE);
		/* 終了イベント */
		elog_sendGetLogListEvent(param->header.destModId, ELOG_GETLOGLIST_END);
	}
	else
	{
		/* スレッドステータスINCOMPLETE */
		ELOG_ThrState(&s_elogThrGetLogList, ELOG_THSTATE_INCOMPLETE);
		if(fnd == TRUE)
		{
			/* 処理エラーイベント */
			elog_sendGetLogListEvent(param->header.destModId,
									ELOG_GETLOGLIST_ERR);
		}
		else
		{
			/* ファイル無しイベント */
			elog_sendGetLogListEvent(param->header.destModId,
									ELOG_GETLOGLIST_NOFILE);
		}
	}

	/* スレッドアンロック */
	CMN_MutexUnlock(&s_elogThrGetLogListMutex);

	pthread_detach(pthread_self());
	DEBUG_TRCOUT();
	return NULL;
}

/*============================================================================*/
/*    Description    :ログ削除要求処理                                         */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFncDeleteLog_t param : Function14パラメータ          */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-2)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_NOENT(-4)    対象ファイルなし                     */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_BUSY(-6)    USB保存/圧縮処理中                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/12  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void *elog_ThrDeleteLog(void *arg)
{
	struct dirent **nameList = NULL;
	sint n = 0;
	schar *path;
	schar tmp[ELOG_FILENAME_MAX + 1];
	ulong i;
	bool_t flag;
	sint ret;
	FILE *fp = NULL;
	sint fd = 0;
	ElogFncDeleteLog_t *param;
	bool_t rep = TRUE;
	bool_t fnd = TRUE;
	DEBUG_TRCIN();

	/* スレッドロック */
	CMN_MutexLock(&s_elogThrDeleteLogMutex);

	param = (ElogFncDeleteLog_t *)(s_elogThrDeleteLog.data);
	if(param == NULL)
	{
		DLOG_Error("param is NULL.\n");
		/* paramがNULLの場合通知を含め動作できない */
		ELOG_ThrState(&s_elogThrDeleteLog, ELOG_THSTATE_INCOMPLETE);
		CMN_MutexUnlock(&s_elogThrDeleteLogMutex);
		pthread_detach(pthread_self());
		flag = FALSE;
	}
	else
	{
		/*Do nothing*/
	}

	if(flag)
	{
		/* スレッドステータス⇒RUN */
		ELOG_ThrState(&s_elogThrDeleteLog, ELOG_THSTATE_RUN);
		/* 処理開始イベント */
		elog_sendDelLogEvent(param->header.destModId, ELOG_DELETELOG_START);

		if(param->type == ELOG_TYPE_ACZ)
		{
			path = ELOG_ACTION_DIR;
		}
		else if(param->type == ELOG_TYPE_SDZ)
		{
			path = ELOG_SDZ_DIR;
		}
		else
		{
			rep = FALSE;
			flag = FALSE;
		}
	}
	else
	{
		/*Do nothing*/
	}

	if(flag)
	{
		/* 日付フィルタ条件排他ロック */
		CMN_MutexLock(&s_elogDateRangeMutex);

		/* 開始～終了日設定 */
		ret = elog_SetFilterDateRange(param->type, param->begin, param->end);
		if(ret != ELOG_OK)
		{
			DLOG_Error("elog_SetFilterDateRange() error.\n");
			CMN_MutexUnlock(&s_elogDateRangeMutex);
			rep = FALSE;
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
		/* ファイル検索 */
		n = scandir(path, &nameList, elog_DirentFilterDateRange, NULL);

		/* 日付フィルタ条件排他アンロック */
		CMN_MutexUnlock(&s_elogDateRangeMutex);

		if(n < 0)
		{
			DLOG_Error("scandir() error.(%d,%d)\n", n, errno);
			rep = FALSE;
			flag = FALSE;
		}
		else if(n == 0)
		{
			DLOG_Error("no file there.\n");
			rep = FALSE;
			fnd = FALSE;
			flag = FALSE;
		}
		else
		{
			/* 削除リストをファイルへ上書き保存 */
			fp = ELOG_FOPEN(ELOG_DELLISTPATH, "w");
			if(fp == NULL)
			{
				DLOG_Error("(%s) error.(%d)\n", ELOG_DELLISTPATH, errno);
				rep = FALSE;
				flag = FALSE;
			}
			else
			{
				/*Do nothing*/
			}
		}
	}
	else
	{
		/*Do nothing*/
	}

	if(flag)
	{
		for(i=0; i<n; i++)
		{
			memset(tmp, 0x0, sizeof(tmp));
			snprintf(tmp, sizeof(tmp)-1, "%s/%s", path, nameList[i]->d_name);

			ret = remove(tmp);
			if(ret == -1)
			{
				if(errno != ENOENT)
				{
					DLOG_Error("remove(%s) is failed.(%d)\n", tmp, errno);
					rep = FALSE;
				}
			}
			else
			{
				/* 削除リストへ追加 */
				ret = fprintf(fp, "%s\n", tmp);
				if(ret < 0)
				{
					DLOG_Error("delete file(%s) list out failed.(%d)\n",
								tmp, errno);
					rep = FALSE;
				}
			}
		}
		/* 応答先の別プロセスで削除リストファイルを参照するためsyncする */
		fd = fileno(fp);
		if(fd == -1)
		{
			DLOG_Error("fileno(%s) error.(%d)\n", ELOG_DELLISTPATH, errno);
			rep = FALSE;
			flag = FALSE;
		}
		else
		{
		fsync(fd);
		}
	}
	else
	{
		/*Do nothing*/
	}

	/* リスト開放 */
	if(nameList != NULL)
	{
		ELOG_DirentFree(&nameList, n);
	}
	/* ファイルCLOSE */
	if(fp != NULL)
	{
		ELOG_FCLOSE(fp);
	}

	if(rep == TRUE)
	{
		/* スレッドステータスCOMPLETE */
		ELOG_ThrState(&s_elogThrDeleteLog, ELOG_THSTATE_COMPLETE);
		/* 正常終了イベント */
		elog_sendDelLogEvent(param->header.destModId, ELOG_DELETELOG_END);
	}
	else
	{
		/* スレッドステータスINCOMPLETE */
		ELOG_ThrState(&s_elogThrDeleteLog, ELOG_THSTATE_INCOMPLETE);
		if(fnd == TRUE)
		{
			/* 処理エラーイベント */
			elog_sendDelLogEvent(param->header.destModId, ELOG_DELLOG_ERR);
		}
		else
		{
			/* ファイル無しイベント */
			elog_sendDelLogEvent(param->header.destModId,
								ELOG_DELETELOG_NOFILE);
		}
	}

	/* スレッドアンロック */
	CMN_MutexUnlock(&s_elogThrDeleteLogMutex);

	pthread_detach(pthread_self());
	DEBUG_TRCOUT();
	return NULL;
}
/*============================================================================*/
/*    Description    :ログリスト取得通知イベント送信                             */
/*----------------------------------------------------------------------------*/
/*    param          :ulong port; 送信先プロセス(CPORT)                        */
/*----------------------------------------------------------------------------*/
/*                   :ElogCompressAction_t state; ステータス                         */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/TRUE:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void elog_sendGetLogListEvent(ulong port, ElogGetLogList_t state)
{
	DEBUG_TRCIN();
	ElogGetLogListInfor_t result;
	result.header.msgId = ELOG_EVT_GETLOGLIST;
	result.stat = state;

	DLOG_Error("send GetLogList status %d\n", state);
	FRM_MsgSendAsync(CONVERT_CPORT2SPORT(port),
			sizeof(ElogGetLogList_t), (void *)&result);
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :ログ削除通知イベント送信                                  */
/*----------------------------------------------------------------------------*/
/*    param          :ulong port; 送信先プロセス(CPORT)                        */
/*----------------------------------------------------------------------------*/
/*                   :ElogCompressAction_t state; ステータス                   */
/*----------------------------------------------------------------------------*/
/*                   :ElogType_t type; ログタイプ                              */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/TRUE:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void elog_sendDelLogEvent(ulong port, ElogDeleteLog_t state)
{
	DEBUG_TRCIN();
	ElogDeleteLogInfor_t result;
	result.header.msgId = ELOG_EVT_DELETELOG;
	result.stat = state;

	DLOG_Error("send DeleteLog status %d\n", state);
	FRM_MsgSendAsync(CONVERT_CPORT2SPORT(port),
			sizeof(ElogDeleteLog_t), (void *)&result);
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :ファイル日時フィルタ範囲設定                              */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    サブスレッド未実行                          */
/*                   :                                                        */
/*                    ELOG_E_PARAM(-1)    パラメータエラー                      */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_SetFilterDateRange(ElogType_t type, schar *begin, schar *end)
{
	ulong i;
	ulong ret = ELOG_OK;
	DEBUG_TRCIN();

	if(begin == NULL)
	{
		DLOG_Error("begin is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		if(end == NULL)
		{
			DLOG_Error("end is NULL.\n");
			DEBUG_TRCOUT();
			return ELOG_E_PARAM;
		}
		else
		{
			/*Do nothing*/
		}
	}

	memset(s_elogDateRangeFrom, 0x0, sizeof(s_elogDateRangeFrom));
	memset(s_elogDateRangeTo, 0x0, sizeof(s_elogDateRangeTo));
	if(*begin == '*')
	{
		/* 下限指定なしとする */
		s_elogDateRangeFrom[0] = '*';
	}
	else
	{
		/* 数字以外が有る場合はエラーとする */
		for(i=0; i<ELOG_DATESTR_LEN; i++)
		{
			if(! isdigit((int)*(begin+i)))
			{
				DLOG_Error("begin has not digit.\n");
				ret = ELOG_E_PARAM;
			}
			else
			{
				/*Do nothing*/
			}
		}
		strncpy(s_elogDateRangeFrom, begin, sizeof(s_elogDateRangeFrom) - 1);
	}
	if(*end == '*')
	{
		/* 上限指定なしとする */
		s_elogDateRangeTo[0] = '*';
	}
	else
	{
		/* 数字以外が有る場合はエラーとする */
		for(i=0; i<ELOG_DATESTR_LEN; i++)
		{
			if(! isdigit((int)*(end+i)))
			{
				DLOG_Error("end has not digit.\n");
				ret = ELOG_E_PARAM;
			}
			else
			{
				/*Do nothing*/
			}
		}
		strncpy(s_elogDateRangeTo, end, sizeof(s_elogDateRangeTo) - 1);
	}

	s_elogDateRangeType = type;
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :ファイル日時範囲フィルタ処理                               */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    サブスレッド未実行                          */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_BUSY(-6)    サブスレッド実行中                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/12  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static ulong elog_DirentFilterDateRange(const struct dirent *ent)
{
	schar *pStr = NULL;
	ulong i;
	sint ret = 1;
	bool_t flag = TRUE;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DLOG_Error("ent is NULL.\n");
		DEBUG_TRCOUT();
		return 0;
	}
	else
	{
		if(ent->d_type == DT_DIR)
		{
			DEBUG_TRCOUT();
			return 0;
		}
		else
		{
			/*Do nothing*/
		}
	}


	if(s_elogDateRangeType == ELOG_TYPE_ACZ)
	{
		ret = ELOG_SvrLogGetFileDate((schar *)(ent->d_name), &pStr);
	}
	else if(s_elogDateRangeType == ELOG_TYPE_SDZ)
	{
		ret = ELOG_SdzLogGetFileDate((schar *)(ent->d_name), &pStr);
	}
	else
	{
		ret = 0;
		flag = FALSE;
	}

	if(flag)
	{
		if(ret != ELOG_OK || pStr == NULL)
		{
			DLOG_Error("file date get(%s) error.\n", ent->d_name);
			ret = 0;
			flag = FALSE;
		}
		else
		{
			for(i=0; i < ELOG_DATESTR_LEN; i++)
			{
				if(! isdigit((int)*(pStr+i)))
				{
					DLOG_Error("file date string has not digit.\n");
					ret = 0;
					flag = FALSE;
				}
				else
				{
					/*Do nothing*/
				}
			}
		}
	}
	else
	{
		/*Do nothing*/
	}
	if(flag)
	{
		if(s_elogDateRangeFrom[0] != '*')
		{
			/* 期日開始指定有り */
			ret = strncmp(pStr, s_elogDateRangeFrom, ELOG_DATESTR_LEN);
			if(ret < 0)
			{
				ret = 0;
				flag = FALSE;
			}
			else
			{
				if(s_elogDateRangeTo[0] != '*')
				{
					/* 期日終了指定有り */
					ret = strncmp(pStr, s_elogDateRangeTo, ELOG_DATESTR_LEN);
					if(ret > 0)
					{
						ret = 0;
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
			}
		}
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :動作圧縮ログリスト昇順ソート関数                           */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    負 : entA < entB/ 0 : entA = entB/正 : entA > entB      */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/12  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static ulong elog_SortSvrLogList(const struct dirent **entA,
								const struct dirent **entB)
{
	sint retA;
	slong ret;
	sint retB;
	schar nameA[ELOG_FILENAME_MAX + 1];
	schar nameB[ELOG_FILENAME_MAX + 1];
	schar *pstrA;
	schar *pstrB;
	DEBUG_TRCIN();

	if(entA == NULL)
	{
		DLOG_Error("entA is NULL.\n");
		DEBUG_TRCOUT();
		return 1;
	}
	else
	{
		if(entB == NULL)
		{
			DLOG_Error("entB is NULL.\n");
			DEBUG_TRCOUT();
			return -1;
		}
		else
		{
			if(*entA == NULL)
			{
				DLOG_Error("*entA is NULL.\n");
				DEBUG_TRCOUT();
				return 1;
			}
			else
			{
				if(*entB == NULL)
				{
					DLOG_Error("*entB is NULL.\n");
					DEBUG_TRCOUT();
					return -1;
				}
				else
				{
					/*Do nothing*/
				}
			}
		}
	}

	retA = strncmp((*entA)->d_name, "TMP-", 4);
	retB = strncmp((*entB)->d_name, "TMP-", 4);
	if(retA == 0 && retB != 0)
	{
		/* entAが"TMP-"で始まり、entBがそうでない場合はentAを後ろにする */
		ret = 1;
	}
	else if(retA != 0 && retB == 0)
	{
		/* entBが"TMP-"で始まり、entAがそうでない場合はentBを後ろにする */
		ret = -1;
	}
	else
	{
		memset(nameA, 0x0, sizeof(nameA));
		memset(nameB, 0x0, sizeof(nameA));
		strncpy(nameA, (*entA)->d_name, sizeof(nameA) - 1);
		strncpy(nameB, (*entB)->d_name, sizeof(nameB) - 1);

		/* '.'の前までを比較 */
		pstrA = index(nameA, '.');
		if(pstrA != NULL)
		{
			*pstrA = '\0';
		}
		pstrB = index(nameB, '.');
		if(pstrB != NULL)
		{
			*pstrB = '\0';
		}
		retA = strncmp(nameA, nameB, ELOG_FILENAME_MAX);
		ret = retA;
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :qsort用alphasortラッパー関数                             */
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent **entA : ディレクトリエントリ               */
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent **entB : ディレクトリエントリ               */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    -1 : entA < entB/ 0 : entA = entB/ 1 : entA > entB      */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/12  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_AlphaSort(const void *entA, const void *entB)
{
	slong re;
	DEBUG_TRCIN();
	re = alphasort((const struct dirent **)entA, (const struct dirent **)entB);
	DEBUG_TRCOUT();
	return re;
}

/*============================================================================*/
/*    Description    :ログリストファイル出力関数                                */
/*----------------------------------------------------------------------------*/
/*    param          :schar    *fPath : 出力ファイルパス                       */
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent **list  : 出力リスト                       */
/*----------------------------------------------------------------------------*/
/*    param          :ulong size  : リストサイズ                               */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常終了                                   */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-2)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/12  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_WriteListToFile(schar *fPath, 
								struct dirent **list, ulong size)
{
	FILE *fp;
	sint fd;
	ulong i;
	slong ret = ELOG_OK;
	DEBUG_TRCIN();

	if(fPath == NULL)
	{
		DLOG_Error("fPath is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		if(list == NULL)
		{
			DLOG_Error("list is NULL.\n");
			DEBUG_TRCOUT();
			return ELOG_E_PARAM;
		}
		else
		{
			/*Do nothing*/
		}
	}

	/* リストをファイルへ上書き出力 */
	fp = ELOG_FOPEN(fPath, "w");
	if(fp == NULL)
	{
		DLOG_Error("ELOG_FOPEN(%s) error.(%d)\n", fPath, errno);
		ret = ELOG_E_SYSCAL;
	}
	else
	{
		for(i=0; i<size; i++)
		{
			if(list[i] != NULL)
			{
				/* ログリストのリスト通し番号の記録をやめる */
				fprintf(fp, "%s\n", list[i]->d_name);
			}
		}

		/* 応答先の別プロセスでファイルを参照するためsyncする */
		fd = fileno(fp);
		if(fd == -1)
		{
			DLOG_Error("fileno(%s) error.(%d)\n", fPath, errno);
			ELOG_FCLOSE(fp);
			ret = ELOG_E_SYSCAL;
		}
		else
		{
			fsync(fd);
			ELOG_FCLOSE(fp);
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :読み込み専用ファイル出力関数                              */
/*----------------------------------------------------------------------------*/
/*    param          :schar* rmc_serial ：リモコンシリアルNo.                  */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_USBST_COMPLETE(4)    保存完了                       */
/*----------------------------------------------------------------------------*/
/*                    ELOG_USBST_WRITEERROR(2)    書き込みエラー               */
/*----------------------------------------------------------------------------*/
/*                    ELOG_USBST_ABORTED(3)    中断                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/12  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static ElogUsbst_t elog_SaveReadSetting(schar* rmc_serial)
{
	ElogUsbst_t stat = ELOG_USBST_COMPLETE;
	pid_t child;
	slong status;
	slong ret;
	bool_t flag = TRUE;
	schar cmd[ELOG_CMD_BUFSIZE];
	ElogUsbSave_t * pData = NULL;
	DEBUG_TRCIN();

	/* 引数確認 */
	if(rmc_serial == NULL)
	{
		DEBUG_TRCOUT();
		return ELOG_USBST_WRITEERROR;
	}
	else
	{
		/* 中断確認 */
		if(ELOG_GetUsbAbort() == TRUE)
		{
			ret = ELOG_USBST_ABORTED;
			flag = FALSE;
		}
		else
		{
			/*Do nothing*/
		}
	}

	if(flag)
	{
		/* ファイル削除 */
		elog_RsDeleteFile();

		/* PCS製造番号取得 */
		if(s_elogThrUSB.data == NULL)
		{
			ret = ELOG_USBST_WRITEERROR;
			flag = FALSE;
		}
		else
		{
			pData = (ElogUsbSave_t *)s_elogThrUSB.data;

			CMN_MutexLock(&s_elogUsbChildMutex);

			/* 子プロセスIDクリア */
			elog_SetUsbChild(0);

			/* 子プロセス生成 */
			child = fork();
			if(child == -1)
			{
				ret = ELOG_USBST_WRITEERROR;
				flag = FALSE;
			}
			else
			{
				/*Do nothing*/
			}
		}
	}
	else
	{
		/*Do nothing*/
	}

	if(flag)
	{
		/* 子プロセスID判定 */
		if(child == 0)
		{
			schar hemsoh_tgz[ELOG_CMD_BUFSIZE];

			/* 子プロセス処理 */
			DLOG_Error("child:exec tar\n");
			snprintf(cmd, sizeof(cmd), ELOG_RS_NAME, ELOG_FS_USB,
					pData->msgData.PcsSerial);
			snprintf(hemsoh_tgz, sizeof(hemsoh_tgz), "%s%s.tgz",
					ELOG_RS_EXCLUDE_ESCH_HEMSOH_TGZ, rmc_serial);

			/* tarコマンド実行                                                */
			/* (tar cf /media/usb_storage/readsetting_PCS製造番号            */
			/* /mnt/user/agui/batterystat                                   */
			/* /mnt/user/ebkl    /mnt/user/ebuz /mnt/user/eepc              */
			/* /mnt/user/egra    /mnt/user/ensv /mnt/user/ehem              */
			/* /mnt/user/epcs/rs /mnt/user/eenl /mnt/user/esch              */
			/* --exclude mnt/user/eepc/eepc_ComHist.csv                     */
			/* --exclude mnt/user/esch/esch_hemsOH                          */
			/* --exclude mnt/user/esch/esch_HEMSOH_List.txt                 */
			/* --exclude mnt/user/esch/HEMSONOFFhistory.txt                 */
			/* --exclude mnt/user/esch/esch_HS_NonVolatile.inf              */
			/* --exclude mnt/user/esch/HEMSOH-リモコンシリアル番号.tgz)       */
			execl("/bin/tar", "tar", "cf", cmd,
						ELOG_RS_INCLUDE_AGUI,
						ELOG_RS_INCLUDE_EBKL,
						ELOG_RS_INCLUDE_EBUZ,
						ELOG_RS_INCLUDE_EEPC,
						ELOG_RS_INCLUDE_EGRA,
						ELOG_RS_INCLUDE_EHEM,
						ELOG_RS_INCLUDE_ENSV,
						ELOG_RS_INCLUDE_EPCS,
						ELOG_RS_INCLUDE_EENL,
						ELOG_RS_INCLUDE_ESCH,
						ELOG_RS_INCLUDE_EPCS_PV,
						ELOG_RS_INCLUDE_EPCS_MAINT,
						"--exclude", ELOG_RS_EXCLUDE_EEPC_COMHIST,
						"--exclude", ELOG_RS_EXCLUDE_ESCH_HEMSOH,
						"--exclude", ELOG_RS_EXCLUDE_ESCH_HEMSOH_LST,
						"--exclude", ELOG_RS_EXCLUDE_ESCH_HEMSHIST,
						"--exclude", ELOG_RS_EXCLUDE_ESCH_HS_NONVOL,
						"--exclude", hemsoh_tgz, (schar*)NULL);
			DLOG_Error("child:execl() failed.\n");
			flag = FALSE;
		}
		else
		{
			/* 子プロセス終了待ち */
			elog_SetUsbChild(child);
			CMN_MutexUnlock(&s_elogUsbChildMutex);

			DLOG_Error("waiting child(PID=%d)...\n", child);
			waitpid(child, &status, 0);
			DLOG_Error("child(PID=%d) finished!\n", child);

			CMN_MutexLock(&s_elogUsbChildMutex);

			/* 終了状態判定 */
			if(WIFEXITED(status))
			{
				/* 子プロセスはexit()で終了 */
				DLOG_Error("exit, status=%d\n", WEXITSTATUS(status));
				if(WEXITSTATUS(status) != 0)
				{
					/* tarコマンドでエラー発生時、WEXITSTATUS(status)は1となる */
					stat = ELOG_USBST_WRITEERROR;	/* 書き込みエラー */
				}
			}
			else if(WIFSIGNALED(status))
			{
				/* 子プロセスはシグナルで終了 */
				DLOG_Error("signal, sig=%d\n", WTERMSIG(status));
				if(WTERMSIG(status) == SIGKILL)
				{
					stat = ELOG_USBST_ABORTED;		/* 中断 */
				}
				else
				{
					stat = ELOG_USBST_WRITEERROR;	/* 書き込みエラー */
				}
			}
			else
			{
				/* それ以外 */
				DLOG_Error("abnormal exit\n");
				stat = ELOG_USBST_WRITEERROR;
			}

			/* 子プロセスIDクリア */
			elog_SetUsbChild(0);

			/* 子プロセスID保存排他アンロック */
			CMN_MutexUnlock(&s_elogUsbChildMutex);

			/* MD5作成＆リモコンシリアルNoファイル作成 */
			if(stat == ELOG_USBST_COMPLETE)
			{
				/* MD5作成 */
				ret = elog_RsCreatMD5(pData->msgData.PcsSerial);
				if(ret != ELOG_OK)
				{
					ret = ELOG_USBST_WRITEERROR;
					flag = FALSE;
				}
				else
				{
					/*Do nothing*/
				}
				/* リモコンシリアルNoファイル作成 */
				ret = elog_RsCreatRmcSerial(rmc_serial);
				if(ret != ELOG_OK)
				{
					ret = ELOG_USBST_WRITEERROR;
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
		}
		
	}
	else
	{
		/*Do nothing*/
	}

	if(flag)
	{
		ret = stat;
	}
	else
	{
		/*Do nothing*/
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :読み込み専用関連ファイル削除関数                           */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/12  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static void	elog_RsDeleteFile(void)
{
	schar cmd[ELOG_CMD_BUFSIZE];
	slong ret;
	DEBUG_TRCIN();

	/* "readsetting_*"を削除 */
	snprintf(cmd, sizeof(cmd)-1, ELOG_CMD_RM "%s/%s*",
			ELOG_FS_USB, ELOG_RS_NAME_TOP);
	ret = ELOG_CALL_SYSTEM(cmd);
	if(ret != 0)
	{
		/* 通常はファイルは存在しないせず、エラーでも問題ないため何もしない */
	}
	else
	{
		/* "md5_readsetting_*"を削除 */
		snprintf(cmd, sizeof(cmd)-1, ELOG_CMD_RM "%s/%s*",
				ELOG_FS_USB, ELOG_RS_MD5_NAME_TOP);
		ret = ELOG_CALL_SYSTEM(cmd);
		if(ret != 0)
		{
			/* 通常はファイルは存在しないせず、エラーでも問題ないため何もしない */
		}
		else
		{
			/* "rmc_serial_no"を削除 */
			snprintf(cmd, sizeof(cmd)-1, ELOG_CMD_RM "%s/%s",
					ELOG_FS_USB, ELOG_RS_RMC_SERIAL);
			ret = ELOG_CALL_SYSTEM(cmd);
			if(ret != 0)
			{
				/* 通常はファイルは存在しないせず、エラーでも問題ないため何もしない */
			}
			else
			{
				/*Do nothing*/
			}
		}
	}

	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :読み込み専用関連ファイル削除関数                           */
/*----------------------------------------------------------------------------*/
/*    param          :schar* PcsSerial ： PCS製造番号                          */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                   ELOG_OK(0)    正常終了                                    */
/*----------------------------------------------------------------------------*/
/*                   ELOG_E_PARAM(-1)    パラメータエラー                      */
/*----------------------------------------------------------------------------*/
/*                   ELOG_E_SYSCAL(-2)    システムコールエラー                  */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/12  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_RsCreatMD5(schar* PcsSerial)
{
	schar fPath[ELOG_MAX_FPATH] = {0};
	schar cmd[ELOG_CMD_BUFSIZE] = {0};
	schar strMd5[ELOG_RS_MD5_LEN + 2] = {0};
	schar strDmy[ELOG_MAX_FPATH] = {0};
	schar forMat[ELOG_FORMAT_BUFSIZE] = {0};
	ulong length;
	ulong writeNum;
	bool_t flag;
	FILE* fpMd5 = NULL;
	FILE* fpRs = NULL;
	slong ret = ELOG_OK;
	DEBUG_TRCIN();

	/* 引数確認 */
	if(PcsSerial == NULL)
	{
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		/*Do nothing*/
	}

	/* MD5算出 */
	snprintf(fPath, sizeof(fPath), ELOG_RS_NAME, ELOG_FS_USB, PcsSerial);
	snprintf(cmd, sizeof(cmd), "%s%s", ELOG_CMD_MD5SUM, fPath);
	fpRs = popen(cmd, "r");
	if(fpRs == NULL)
	{
		ret = ELOG_E_SYSCAL;	/* MD5算出失敗 */
		flag = FALSE;
	}
	else
	{
		/* MD5＆ファイルパス取得 */
		snprintf(forMat, sizeof(forMat), "%%%lus %%%lus",
				sizeof(strMd5)-1, sizeof(strDmy)-1);
		ret = fscanf(fpRs, forMat, strMd5, strDmy);
		pclose(fpRs);
		if(ret != 2)
		{
			ret = ELOG_E_SYSCAL;	/* MD5算出失敗 */
			flag = FALSE;
		}
		else
		{
			/*Do nothing*/
		}
	}

	if(flag)
	{
		/* MD5桁数チェック */
		length = strnlen(strMd5, sizeof(strMd5));
		if(length  != ELOG_RS_MD5_LEN)
		{
			ret = ELOG_E_SYSCAL;	/* MD5算出失敗 */
		}
		else
		{
			/* 読み込み専用ファイルのMD5ファイル作成 */
			snprintf(fPath, sizeof(fPath),
					ELOG_RS_MD5_NAME, ELOG_FS_USB, PcsSerial);
			fpMd5 = CMN_FOPEN(fPath, "wb");
			if(fpMd5 == NULL)
			{
				ret = ELOG_E_SYSCAL;	/* ファイル作成失敗 */
			}
			else
			{
				/* MD5チェックサム書き込み */
				writeNum = fwrite(strMd5, ELOG_RS_MD5_LEN, 1, fpMd5);
				CMN_FCLOSE(fpMd5);

				/* 書き込み結果チェック */
				if(writeNum != 1)
				{
					ret = ELOG_E_SYSCAL;	/* 書き込み失敗 */
				}
				else
				{
					/*Do nothing*/
				}
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :読み込み専用関連ファイル削除関数                           */
/*----------------------------------------------------------------------------*/
/*    param          :schar* rmc_serial ： PCS製造番号                         */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常終了                                   */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-2)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/12  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_RsCreatRmcSerial(schar* rmc_serial)
{
	schar fPath[ELOG_MAX_FPATH];
	ulong writeNum;
	FILE* fp = NULL;
	slong ret = ELOG_OK;
	DEBUG_TRCIN();

	/* 引数確認 */
	if(rmc_serial == NULL)
	{
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		/* "rmc_serial_no"を作成 */
		snprintf(fPath, sizeof(fPath)-1, "%s/%s",
				ELOG_FS_USB, ELOG_RS_RMC_SERIAL);
		fp = CMN_FOPEN(fPath, "wb");
		if(fp == NULL)
		{
			ret = ELOG_E_SYSCAL;	/* ファイル作成失敗 */
		}
		else
		{
			/* リモコン製造番号書き込み */
			writeNum = fwrite(rmc_serial, ELOG_SERIES_LEN, 1, fp);
			CMN_FCLOSE(fp);

			/* 書き込み結果チェック */
			if(writeNum != 1)
			{
				ret = ELOG_E_SYSCAL;	/* 書き込み失敗 */
			}
			else
			{
				/*Do nothing*/
			}
		}
	}

	return ret;
}

/*============================================================================*/
/*    Description    :ログ一覧(OTH)の除外ディレクトリ判定                        */
/*----------------------------------------------------------------------------*/
/*    param          :schar* d_name ： ディレクトリ名                          */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    TRUE    除外対象    /    FALSE    除外非対称              */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/12  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static bool_t elog_ChkExclDirOThList(schar* d_name)
{
	bool_t ret = FALSE;
	
	if(NULL == d_name)
	{
		/* 判定不可は除外対象扱いとする */
		ret = TRUE;
	}
	else
	{
		/* 以下で除外対象とするものかチェック */
		
		/* ディレクトリ除外判定(enet) */
		if(NULL != strstr(d_name, ELOG_EXCLUDES_DIR_ENET))
		{
			ret = TRUE;
		}
		/* ディレクトリ除外判定(蓄電池充放電グラフ) */
		else if(NULL != strstr(d_name, ELOG_EXCLUDES_DIR_EGRA_BT))
		{
			ret = TRUE;
		}
		/* ディレクトリ除外判定(発電/売買電グラフ) */
		else if(NULL != strstr(d_name, ELOG_EXCLUDES_DIR_EGRA_PV))
		{
			ret = TRUE;
		}
		/* ディレクトリ除外判定(車両充放電グラフ) */
		else if(NULL != strstr(d_name, ELOG_EXCLUDES_DIR_EGRA_V2H))
		{
			ret = TRUE;
		}
		/* ディレクトリ除外判定(ルート証明書ファイル_0) */
		else if(NULL != strstr(d_name, ELOG_EXCLUDES_DIR_EEPC_CER_0))
		{
			ret = TRUE;
		}
		/* ディレクトリ除外判定(ルート証明書ファイル_1) */
		else if(NULL != strstr(d_name, ELOG_EXCLUDES_DIR_EEPC_CER_1))
		{
			ret = TRUE;
		}
		/* 上記以外は除外非対称	*/
		else
		{
			ret = FALSE;
		}
	}
	
	return ret;
}
/*============================================================================*/
/*    Description    :系统启动时，定时器设置                                    */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void *elog_SubThreadTimer(void *arg)
{
	EtimTm_t etm;
	ulong tmpMsec;
	ulong secSum;
	ulong daySecSum;
	ulong ret = ELOG_OK;
	ElogThrCancel_t par;
	DEBUG_TRCIN();

	ret = ETIM_GetTime(&etm);
	if(ret != ELOG_OK)
	{
		DLOG_Error("ETIM_GetTime() error.\n");
	}
	else
	{
		/*现在时间 分钟和秒数的总秒数*/
		secSum = etm.tmMin*60 + etm.tmSec;

		/*如果分钟时间正好是0或者30，直接给定时30分钟的发消息*/
		if((etm.tmMin == 0) || (etm.tmMin == 30))
		{
			ElogCmd81_t CMD81Power;
			/*EINV_getCMD81(&CMD81Power);*/
			/*percentage值从电力会社取值*/
			ELOG_OutPutCtrlLog(10);
			FRM_TimerSetOneShot(ELOG_TIMER_LOG_OUTPUTCTRL,
								ELOG_OUTPUTCTRL_TIMER, NULL);
			ELOG_PowerTotalLog(CMD81Power.ttlChargeEnergySysAC,
								ELOG_FILE_NUM_POWER_OUT);
			FRM_TimerSetOneShot(ELOG_TIMER_SOURCE_OUT,
								ELOG_POWER_OUT_TIMER, NULL);

			ELOG_PowerTotalLog(CMD81Power.ttlDischargeEnergySysAC,
								ELOG_FILE_NUM_SOURCE_IN);
			FRM_TimerSetOneShot(ELOG_TIMER_SOURCE_IN,
								ELOG_POWER_OUT_TIMER, NULL);

			ELOG_PowerTotalLog(CMD81Power.ttlDischargeEnergyAloneAC,
								ELOG_FILE_NUM_GENERATE_OUT);
			FRM_TimerSetOneShot(ELOG_TIMER_GENERATE_OUT,
								ELOG_POWER_OUT_TIMER, NULL);

			ELOG_PowerDealLog();
			FRM_TimerSetOneShot(ELOG_TIMER_PV, ELOG_POWER_TIMER, NULL);

			ELOG_AccumulatorLog();
			FRM_TimerSetOneShot(ELOG_TIMER_BT, ELOG_POWER_TIMER, NULL);

			ELOG_VehicleLog();
			FRM_TimerSetOneShot(ELOG_TIMER_V2H, ELOG_POWER_TIMER, NULL);
		}
		else
		{
			/*系统启动到半点的定时*/
			if(secSum <= ELOG_HALF_HOUR_TO_SEC)
			{
				tmpMsec = (ELOG_HALF_HOUR_TO_SEC - secSum)*1000;
			}
			else
			{
				tmpMsec = (ELOG_HOUR_TO_SEC - secSum)*1000;
			}
				FRM_TimerSetOneShot(ELOG_TIMER_LOG_OUTPUTCTRL, tmpMsec, NULL);
				FRM_TimerSetOneShot(ELOG_TIMER_SOURCE_OUT, tmpMsec, NULL);
				FRM_TimerSetOneShot(ELOG_TIMER_SOURCE_IN, tmpMsec, NULL);
				FRM_TimerSetOneShot(ELOG_TIMER_GENERATE_OUT, tmpMsec, NULL);
		}
		/*系统启动到整点的定时*/
		tmpMsec = (ELOG_FIVE_MIN_TO_SEC - secSum)*1000;
		FRM_TimerSetOneShot(ELOG_TIMER_PV, tmpMsec, NULL);
		FRM_TimerSetOneShot(ELOG_TIMER_BT, tmpMsec, NULL);
		FRM_TimerSetOneShot(ELOG_TIMER_V2H, tmpMsec, NULL);
		FRM_TimerSetOneShot(ELOG_TIMER_LOG, tmpMsec, NULL);


		/*如果分钟时间正好是5的倍数，直接给定时5分钟的发消息*/
		if((etm.tmMin%5) == 0)
		{
			elog_SaveVehicleConnectLock();
			FRM_TimerSetOneShot(ELOG_TIMER_V2H_CNT_LUCK,
								ELOG_CNT_LOCK_TIM, NULL);
		}
		else
		{
			/*系统启动到五分钟的定时*/
			tmpMsec = ((etm.tmMin)%5)*60 + etm.tmSec;
			tmpMsec = (ELOG_FIVE_MIN_TO_SEC - tmpMsec)*1000;
			FRM_TimerSetOneShot(ELOG_TIMER_V2H_CNT_LUCK, tmpMsec, NULL);
		}

		/*如果分钟时间正好是00:00:00，直接给定时一天的发消息*/
		if((etm.tmHour == 0) && (etm.tmMin == 0) && (etm.tmSec == 0))
		{
			ELOG_AsyncCompressActionLog(TRUE);
			FRM_TimerSetOneShot(ELOG_TIMER_ACTION_ACZ, ELOG_ACZ_TIMER, NULL);

			ELOG_AsyncCompressSdpLog(TRUE);
			FRM_TimerSetOneShot(ELOG_TIMER_SDP_SDZ, ELOG_SDP_TIMER, NULL);
		}
		else
		{
			daySecSum = etm.tmHour*60*60 + secSum;
			tmpMsec = (ELOG_DAY_TO_SEC - daySecSum)*1000;

			FRM_TimerSetOneShot(ELOG_TIMER_ACTION_ACZ, ELOG_ACZ_TIMER, NULL);
			FRM_TimerSetOneShot(ELOG_TIMER_SDP_SDZ, ELOG_SDP_TIMER, NULL);
		}

		/*如果分钟时间正好是10的倍数，直接给定时10分钟的发消息*/
		if((etm.tmMin%10) == 0)
		{
			elog_ChangeTmpFileName();
			FRM_TimerSetOneShot(ELOG_TIMER_TMP_SDP_FILE,
								ELOG_TMP_SDP_FILE_TIMER, NULL);
		}
		else
		{
			/*系统启动到十分钟的定时*/
			tmpMsec = ((etm.tmMin)%10)*60 + etm.tmSec;
			tmpMsec = (ELOG_TEN_MIN_TO_SEC - tmpMsec)*1000;
			FRM_TimerSetOneShot(ELOG_TIMER_TMP_SDP_FILE, 10000, NULL);
		}
	}
	par.header.msgId = ELOG_ASYNC_CANCELTHR;
	par.id = s_elogTid;
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :系统启动时，中间文件名名始化                              */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/20  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void *elog_SubCreateSdp(void *arg)
{
	char dateStr[ELOG_DATE_STR_MAX + 1];
	schar fileCmd[ELOG_FILEPATH_MAX*2 + 10];
	slong ret;
	ElogThrCancel_t par;
	DEBUG_TRCIN();

	memset(dateStr, 0x0, sizeof(dateStr));
	ret = ELOG_GetDStr(dateStr, sizeof(dateStr), ELOG_DATESTR_HMS, 0);
	if(ret != ELOG_OK)
	{
		DLOG_Error("ELOG_GetDStr() error.\n");
	}
	else
	{
		/*Do nothing*/
	}
	/* 记录中间文件创建时对应名字的时间 */
	strncpy(s_elogSdpFileName, dateStr, sizeof(dateStr) - 1);

	par.header.msgId = ELOG_ASYNC_CANCELTHR;
	par.id = s_elogSdpTmpId;
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}
/*============================================================================*/
/*    Description    :系统启动时，文件行数获取                                  */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/27  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void *elog_SubLineSize(void *arg)
{
	slong lineNum[ELOG_FILE_LINE_SIZE_MAX] = {0};
	schar filePath[ELOG_FILEPATH_MAX] = {0};
	slong ulNum = 0;
	uchar type;
	ElogThrCancel_t par;
	FILE *fp;

	for(type = ELOG_FILE_SIZE_SERVICE; type < ELOG_FILE_SIZE_SIZE_MAX; type++)
	{
		memset(filePath, 0x0, sizeof(filePath));
		snprintf(filePath, s_elogFileSize[type].pathSize, "%s/%s",
		s_elogFileSize[type].path, s_elogFileSize[type].fileHead);

		fp = ELOG_FOPEN(filePath, "r");
		if(NULL != fp)
		{
			while(fgets(lineNum, sizeof(lineNum) - 1, fp))
			{
				ulNum++;
			}
			s_elogFileSize[type].curlineNum = ulNum;
			ELOG_FCLOSE(fp);
		}
		else
		{
			/*Do nothing*/
		}
		ulNum = 0;
	}

	par.header.msgId = ELOG_ASYNC_CANCELTHR;
	par.id = s_elogLineSizeId;
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}
/* End Of File */