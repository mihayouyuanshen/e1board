/******************************************************************************/
/*    Description    :ログ管理ミドル Function0(WDT応答)処理                     */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC Jiang.Z                                             */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/03/23                                              */
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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <ctype.h>

#include "common_g.h"
#include <task_g.h>
#include "etim_g.h"

#include "elog_utl_g.h"
#include "elog_cnf_g.h"
#include "elog_val_g.h"
#include "elog_fnc_g.h"
#include "elog_fnc.h"
#include "elog_thr_g.h"
#include "elog_dcs_g.h"
#include "eeep_g.h"
#include "einv_g.h"
#include "frm_g.h"

/*----------------------------------------------------------------------------*/
/* 公開変数定義                                                                */
/*----------------------------------------------------------------------------*/
/* グローバル変数 */
extern pthread_mutex_t s_elogSysCmdMutx;			/* system()実行排他用 */
extern schar s_elogSdpFileName[ELOG_FILE_NAME_MAX];
FrmPthread_t s_elogTid;								/* スレッド識別変数 */
FrmPthread_t s_elogSdpTmpId;						/* スレッド識別変数 */
FrmPthread_t s_elogLineSizeId;						/* スレッド識別変数 */
schar s_beforErrCode[ELOG_ERR_CODE] = {"BE000"};
ElogPowerDiff_t s_elogPV;
ElogPowerDiff_t s_elogBt;
ElogPowerDiff_t s_elogV2h;
schar s_elogVoltHistBeTm;
schar s_elogHemsHistoryBeTm;
bool_t s_elogLogFlag = TRUE;
bool_t s_elogLogFileName[ELOG_FILEPATH_MAX] = {0};
/*----------------------------------------------------------------------------*/
/* 非公開変数定義                                                              */
/*----------------------------------------------------------------------------*/
static ElogInformSendMsg_t s_elogInforSendMsg;

schar * s_elogPcsErr[ELOG_MAX_PCS_ERR_CODE_NUM] = {"BE","BF","BC","BI"};
schar * s_elogPcsSecretErr[ELOG_MAX_PCS_ERR_CODE_NUM] = {"be","bf","bc","bi"};

ElogFileSize_t s_elogFileSize[ELOG_FILE_SIZE_SIZE_MAX] = {
	{ELOG_FILE_SIZE_SERVICE,	ELOG_SERVICE_FNAME_HD,
	ELOG_ERR_EVENT_DIR,	0,	ELOG_SERVICE_FILES_LINE_LIM,	ELOG_FILEPATH_MAX},
	{ELOG_FILE_SIZE_INFOR,		ELOG_INFOR_FNAME_HD,
	ELOG_ERR_EVENT_DIR,	0,	ELOG_INFOR_FILES_LIM,			ELOG_FILEPATH_MAX},
	{ELOG_FILE_SIZE_OPERATION,			ELOG_OPERATION_FNAME_HD,
	ELOG_OPERATION_DIR,	0,	ELOG_OPERATION_FILES_LINE_LIM,	ELOG_FILEPATH_MAX},
	{ELOG_FILE_SIZE_SOH,				ELOG_SOH_FNAME_HD,
	ELOG_SOH_DIR,		0,	ELOG_SOH_FILES_LINE_LIM,		ELOG_FILEPATH_MAX},
	{ELOG_FILE_SIZE_POWER_VOLT_HIST,	ELOG_POW_RIS_EDGE_FNAME_HD,
	ELOG_POW_RIS_EDGE_DIR,	0,	ELOG_POW_RIS_EDGE_FILES_LIM,ELOG_FILEPATH_MAX},
	{ELOG_FILE_SIZE_HEMS_HISTORY,	ELOG_HEMS_HISTORY_FNAME_HD,
	ELOG_HEMSHISTORY_DIR,	0, ELOG_HEMS_HISTORYFILES_LIM,	ELOG_FILEPATH_MAX},
};

ElogFileNum_t s_elogFileNum[ELOG_FILE_NUM_TYPE_NUM_MAX] = {
	{ELOG_FILE_NUM_OUTPUTCTRL,		ELOG_OUTPUTCTRL_FNAME_HD,
	ELOG_OUTPUTCTRL_DIR,		ELOG_OUTPUTCTRL_FILES_LIM,	ELOG_FILEPATH_MAX},
	{ELOG_FILE_NUM_POWER_OUT,	ELOG_POWER_SOURCE_OUT_FNAME_HD,
	ELOG_POWER_SOURCE_OUT_DIR,	ELOG_AC_FILES_LIM,			ELOG_FILEPATH_MAX},
	{ELOG_FILE_NUM_SOURCE_IN,		ELOG_POWER_SOURCE_IN_FNAME_HD,
	ELOG_POWER_SOURCE_IN_DIR,	ELOG_AC_FILES_LIM,			ELOG_FILEPATH_MAX},
	{ELOG_FILE_NUM_GENERATE_OUT,		ELOG_GENERATE_OUT_FNAME_HD,
	ELOG_GENERATE_OUT_DIR,		ELOG_AC_FILES_LIM,			ELOG_FILEPATH_MAX},
	{ELOG_FILE_NUM_POWER_DEAL_YEAR,		ELOG_POWER_DEAL_FNAME_HD,
	ELOG_POWER_DEAL_YEAR_DIR,	ELOG_YEAR_FILES_LIM,		ELOG_FILEPATH_MAX},
	{ELOG_FILE_NUM_POWER_DEAL_MONTH,	ELOG_POWER_DEAL_FNAME_HD,
	ELOG_POWER_DEAL_MONTH_DIR,	ELOG_MONTH_FILES_LIM,		ELOG_FILEPATH_MAX},
	{ELOG_FILE_NUM_POWER_DEAL_DAY,		ELOG_POWER_DEAL_FNAME_HD,
	ELOG_POWER_DEAL_DAY_DIR,	ELOG_DAY_FILES_LIM,			ELOG_FILEPATH_MAX},
	{ELOG_FILE_NUM_ACCUMULATOR_YEAR,	ELOG_ACCUMULATOR_FNAME_HD,
	ELOG_ACCUMULATOR_YEAR_DIR,	ELOG_YEAR_FILES_LIM,		ELOG_FILEPATH_MAX},
	{ELOG_FILE_NUM_ACCUMULATOR_MONTH,	ELOG_ACCUMULATOR_FNAME_HD,
	ELOG_ACCUMULATOR_MONTH_DIR,	ELOG_MONTH_FILES_LIM,		ELOG_FILEPATH_MAX},
	{ELOG_FILE_NUM_ACCUMULATOR_DAY,		ELOG_ACCUMULATOR_FNAME_HD,
	ELOG_ACCUMULATOR_DAY_DIR,	ELOG_DAY_FILES_LIM,			ELOG_FILEPATH_MAX},
	{ELOG_FILE_NUM_VEHICLE_YEAR,		ELOG_VEHICLE_FNAME_HD,
	ELOG_VEHICLE_YEAR_DIR,		ELOG_YEAR_FILES_LIM,		ELOG_FILEPATH_MAX},
	{ELOG_FILE_NUM_VEHICLE_MONTH,		ELOG_VEHICLE_FNAME_HD,
	ELOG_VEHICLE_MONTH_DIR,		ELOG_MONTH_FILES_LIM,		ELOG_FILEPATH_MAX},
	{ELOG_FILE_NUM_VEHICLE_DAY,			ELOG_VEHICLE_FNAME_HD,
	ELOG_VEHICLE_DAY_DIR,		ELOG_DAY_FILES_LIM,			ELOG_FILEPATH_MAX},
};

ElogE1ErrCode_t s_elogE1ErrCodeList[ELOG_MAX_E1_ERR_CODE_NUM] = {
	{"BC096",	"G",	TRUE,	TRUE}, {"bi663",	"C",	FALSE,	TRUE},
	{"bc656",	"C",	FALSE,	TRUE}, {"BC667",	"J",	FALSE,	FALSE},
	{"BC672",	"A",	TRUE,	TRUE}, {"bc673",	"C",	FALSE,	TRUE},
	{"bc674",	"C",	FALSE,	TRUE}, {"BF650",	"4",	TRUE,	TRUE},
	{"bf650",	"C",	FALSE,	TRUE}, {"BC550",	"8",	TRUE,	TRUE},
	{"BC662",	"M",	TRUE,	TRUE}, {"BC669",	"0",	TRUE,	TRUE},
	{"BC651",	"M",	TRUE,	TRUE}, {"BC668",	"0",	TRUE,	TRUE},
	{"BC665",	"Q",	TRUE,	TRUE}, {"BC666",	"0",	TRUE,	TRUE},
	{"BI670",	"L",	TRUE,	TRUE}, {"BF671",	"I",	TRUE,	TRUE},
	{"BC654",	"P",	TRUE,	TRUE}, {"BC664",	"3",	TRUE,	TRUE},
	{"bc654",	"C",	FALSE,	TRUE}, {"BC624",	"P",	TRUE,	TRUE},
	{"bc624",	"C",	FALSE,	TRUE}, {"BC625",	"P",	TRUE,	TRUE},
	{"bc625",	"C",	FALSE,	TRUE}, {"BC626",	"P",	TRUE,	TRUE},
	{"BC626",	"C",	FALSE,	TRUE}, {"BC627",	"P",	TRUE,	TRUE},
	{"bc627",	"C",	FALSE,	TRUE}
};
ElogDeleteFile_t s_elogDeleteFile[ELOG_DELETE_FILE_MAX] = {
	{ELOG_DELETE_FILE_INFORMATION,	ELOG_ERR_EVENT_DIR,	ELOG_INFOR_FNAME_HD}
};
/*----------------------------------------------------------------------------*/
/* 公開関数定義                                                                */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開関数定義                                                              */
/*----------------------------------------------------------------------------*/

/*============================================================================*/
/*    Description    :                                                        */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/true:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/23  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void elog_SetLogFileName(void)
{
	DEBUG_TRCIN();
	s_elogLogFlag = TRUE;
	DEBUG_TRCOUT();
}
/*============================================================================*/
/*    Description    :检查文件size                                            */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFileSizeType_t type:检查的文件种别                   */
/*----------------------------------------------------------------------------*/
/*    param          :schar *filePath：最新文件                                */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                    ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_EXIST(1)    文件存在                                */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PROCS    処理エラー                               */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/03  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong CheckFileSizeAndCreateFile(ElogFileSizeType_t type, schar *filePath)
{
	slong ret = ELOG_OK;
	schar fName[ELOG_FILEPATH_MAX + 1];
	bool_t createFile = FALSE;

	DEBUG_TRCIN();
	/*通知log通知文件条数检验，删除写入时间最早的记录*/
	ret = elog_CheckCurrentAndDeleteOld(
			s_elogFileSize[type].path,
			s_elogFileSize[type].curlineNum,
			s_elogFileSize[type].fileLimNum,
			type, fName);
	if(ret == ELOG_E_NOENT)
	{
		/* ファイル無し */
		createFile = TRUE;
		s_elogFileSize[type].curlineNum = 0;
		ret = ELOG_EXIST;
	}
	else if(ret < 0)
	{
		DLOG_Error("elog_CheckCurrentAndDeleteOld() error.\n");
		ret = ELOG_E_PROCS;
	}
	else if(ret == 0)
	{
		/* 現ファイルパス */
		memset(filePath, 0x0, sizeof(filePath));
		snprintf(filePath, s_elogFileSize[type].pathSize, "%s/%s",
		s_elogFileSize[type].path, fName);
		ret = ELOG_EXIST;
	}
	else
	{
		/*Do nothing*/
	}
	/* 新しいファイルを作成する */
	if(createFile == TRUE)
	{
		/* フルパスファイル名作成 */
		memset(filePath, 0x0, sizeof(filePath));
		snprintf(filePath, s_elogFileSize[type].pathSize, "%s/%s",
		s_elogFileSize[type].path, s_elogFileSize[type].fileHead);
	}
	else
	{
		/*Do nothing*/
	}

	/* 正常終了 */
	return ret;
}
/*============================================================================*/
/*    Description    :检查文件行数并创建携带时间的新文件                         */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFileNumType_t type:文件类型                          */
/*----------------------------------------------------------------------------*/
/*                   :schar *filePath:文件保存路径                             */
/*----------------------------------------------------------------------------*/
/*                   :ElogDateStrType_t date:文件名携带时间的格式               */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/03  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong CheckFileNumAndCreateFile(ElogFileNumType_t type, schar *filePath,
								ElogDateStrType_t date)
{
	slong ret = ELOG_OK;
	schar newPath[ELOG_FILEPATH_MAX + 1];
	schar dateStr[ELOG_DATE_STR_MAX + 1];
	bool_t createFile = TRUE;
	DEBUG_TRCIN();

	/* NAND容量回復スレッド状態確認、中断 */
	ret = ELOG_AbortAndWaitNandDelete(FALSE);
	if(ret != ELOG_OK)
	{
		DLOG_Error("ELOG_AbortAndWaitNandDelete() error.\n");
		ret = ELOG_E_PROCS;
	}
	else
	{
		/* 現ログファイル確認＋最古ファイル削除 */
		ret = elog_CheckFileDateAndDeleteOld(
				s_elogFileNum[type].path, type,
				s_elogFileNum[type].fileLimNum);
		if(ret == ELOG_E_NOENT)
		{
			ret = ELOG_E_NOENT;
		}
		else if(ret != ELOG_OK)
		{
			DLOG_Error("elog_CheckFileDateAndDeleteOld() error.\n");
			ret = ELOG_E_PROCS;
			createFile = FALSE;
		}
		else
		{
			/*Do nothing*/
		}
		if(createFile)
		{
			/* ファイル名付与用日時文字列取得 */
			memset(dateStr, 0x0, sizeof(dateStr));
			ret = ELOG_GetDStr(dateStr, sizeof(dateStr), date, 0);
			if(ret != ELOG_OK)
			{
				DLOG_Error("ELOG_GetDStr() error.\n");
				ret = ELOG_E_PROCS;
			}
			else
			{
				/* フルパスファイル名作成 */
				memset(filePath, 0x0, sizeof(filePath));
				snprintf(filePath, s_elogFileNum[type].pathSize, "%s/%s%s",
				s_elogFileNum[type].path,
				s_elogFileNum[type].fileHead, dateStr);
			}
		}
		else
		{
			/*Do nothing*/
		}
	}
	DEBUG_TRCOUT();
	/* 正常終了 */
	return ret;
}
/*============================================================================*/
/*    Description    :ログレベル設定                                           */
/*----------------------------------------------------------------------------*/
/*    param          :ElogLv_t level:ログレベル設定                            */
/*----------------------------------------------------------------------------*/
/*                   :slong msgID:対象プロセスID                               */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_SyncSetLevel(ElogLv_t level, slong msgID)
{
	slong ret;
	DEBUG_TRCIN();

	DLOG_Error("level[%d] msgID[%d]\n",level,msgID);

	ret = ELOG_CnfSetLevel(level,msgID);

	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :ログOn/Off設定                                           */
/*----------------------------------------------------------------------------*/
/*    param          :bool_t onOff:ログのON/OFF                               */
/*----------------------------------------------------------------------------*/
/*                   :slong msgID:対象プロセスID                               */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_SyncSetEnable(bool_t onOff,slong msgID)
{
	slong ret;
	DEBUG_TRCIN();

	DLOG_Error("onOff[%d] msgID[%d]\n", onOff, msgID);
	ELOG_CnfSetEnable(onOff,msgID);
	DEBUG_TRCOUT();
}
/*============================================================================*/
/*    Description    :                                                        */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/true:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/28  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void elog_CreateThread(void)
{
	slong ret;
	DEBUG_TRCIN();
	ret = FRM_PthreadCreate(&s_elogTid, NULL, elog_SubThreadTimer, NULL);
	if(ret == 0)
	{
		DLOG_Info("pthread_create(elog_SubThreadTimer)\n");
	}
	else
	{
		/*Do nothing*/
	}
	ret = FRM_PthreadCreate(&s_elogSdpTmpId, NULL, elog_SubCreateSdp, NULL);
	if(ret == 0)
	{
		DLOG_Info("pthread_create(elog_SubCreateSdp)\n");
	}
	else
	{
		/*Do nothing*/
	}
	ret = FRM_PthreadCreate(&s_elogLineSizeId, NULL, elog_SubLineSize, NULL);
	if(ret == 0)
	{
		DLOG_Info("pthread_create(elog_SubCreateSdp)\n");
	}
	else
	{
		/*Do nothing*/
	}
	DEBUG_TRCOUT();
}
/*============================================================================*/
/*    Description    :定时器线程创建                                           */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/03  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void elog_Start(void)
{
	DEBUG_TRCIN();
	/* ログディレクトリ作成 */
	/* 変数初期化 */
	ELOG_ValInit();
	/* コンフィグファイル読み込み */
	ELOG_CnfInit();
	/* スレッド（圧縮、USB保存）初期化 */
	ELOG_ThrInit();
	elog_InitMkdir();
	/* 线程创建 */
	elog_CreateThread();
	DEBUG_TRCOUT();
}
/*============================================================================*/
/*    Description    :ログ出力 ファンクション1実行処理                           */
/*----------------------------------------------------------------------------*/
/*    param          :ELOG_FNC01_T param : Function1パラメータ                 */
/*----------------------------------------------------------------------------*/
/*    return         :ELOG_OK(0)    正常                                       */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/10  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncLogOutput(ElogFncLog_t *param)
{
	slong ret = ELOG_OK;
	size_t len;
	ElogLv_t level;
	bool_t enable;
	slong fileSize;
	schar filePath[ELOG_FILEPATH_MAX + 1];
	schar dateStr[ELOG_DATE_STR_MAX + 1];
	FILE *fp;
	bool_t createFile = FALSE;
	slong fileSile;
	DEBUG_TRCIN();

	/* OnOff状態取得 */
	enable = ELOG_CnfGetEnable(FRM_MsgSelfModId());
	if(enable)
	{
		/* ログレベル設定取得 */
		level = ELOG_CnfGetLevel(FRM_MsgSelfModId());
		if(param->level <= level)
		{
			DLOG_Error("level[%d] enable[%d] id[%ld]\n",
			level, enable, FRM_MsgSelfModId());
			/* 現ログファイル確認＋最古ファイル削除 */
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
	return ret;
}

/*============================================================================*/
/*    Description    :USB保存/読み込み要求                                     */
/*----------------------------------------------------------------------------*/
/*    param          :PcsSerial:版本号                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-2)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_NOSPC(-5)    容量不足エラー                       */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_BUSY(-6)    多重起動エラー                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncSaveUSB(schar* PcsSerial)
{
	slong ret = ELOG_OK;
	ElogMemoryInfo_t logInfo;
	ElogMemoryInfo_t userInfo;
	ElogMemoryInfo_t usbInfo;
	u64 needs;
	u64 space;
	u64 need_space;
	ulong fw_size;
	double expSize;
	ulong lsTime;
	ulong rsTime;
	ulong tTime;

	DEBUG_TRCIN();

	/* 多重起動防止 */
	ret = ELOG_CheckSubThreadRun(ELOG_THTYPE_USB);
	if(ret != ELOG_OK)
	{
		DLOG_Error("ELOG_CheckSubThreadRun() = %d.\n", ret);
		return ret;
	}
	else
	{
		/* ログパーティション使用サイズ取得 */
		ret = ELOG_GetPartitionInfoMB_All(ELOG_FS_LOG,
			&logInfo.avail, &logInfo.used, &logInfo.all);
		if(ret != ELOG_OK)
		{
			DLOG_Error("ELOG_GetPartitionInfoMB_All(%s) error\n", ELOG_FS_LOG);
			ret = ELOG_E_SYSCAL;
		}
		else
		{
			/* 設定パーティション使用サイズ取得 */
			ret = ELOG_GetPartitionInfoMB_All(ELOG_FS_USR,
				&userInfo.avail, &userInfo.used, &userInfo.all);
			if(ret != ELOG_OK)
			{
				DLOG_Error("ELOG_GetPartitionInfoMB_All(%s) error.\n",
				ELOG_FS_USR);
				ret = ELOG_E_SYSCAL;
			}
			else
			{
				needs = (u64)logInfo.used + (u64)userInfo.used;
				needs = needs + 2;		/* 最大切捨てサイズ＋1 (MB) */

				/* 必要空き容量算出
				(ログ/設定値ファイルサイズ + 読み込み専用ファイルサイズ(最大値)) */
				need_space = needs + ELOG_RS_FILESIZE;

				/* ログ/設定値ファイルのコピー時間を推量 */
				DLOG_Error("[log settting]\n");
				DLOG_Error("save size = %llu[MB]\n", needs);
				lsTime = (ulong)(ELOG_MB(needs) / ELOG_USBSAVE_RATE) + 1;
				DLOG_Error("save time = %ld (rate = %d)\n",
				lsTime, ELOG_USBSAVE_RATE);
			}
		}
	}
	if(ret == ELOG_OK)
	{
		/* 読み込み専用ファイルの対象のサイズを算出 */
		ret = elog_GetUpgradDirSize(&fw_size);
		if(ret != ELOG_OK)
		{
			DLOG_Error("ELOG_GetDirSize(%s) error.\n", ELOG_USR_FW_DIR);
			ret = ELOG_E_SYSCAL;
		}
		else
		{
			/* 読み込み専用ファイルのコピー時間を推量 */
			DLOG_Error("[read settting]\n");
			/* 予測サイズ算出 */
			expSize = (double)((userInfo.used - fw_size) * ELOG_RS_INC_COFE);
			DLOG_Error("save size = %llu[MB]\n", expSize);
			rsTime = (ulong)(ELOG_MB(expSize) / ELOG_RS_WRITERATE) + 1;
			DLOG_Error("save time=%ld(rate=%d)\n", rsTime, ELOG_RS_WRITERATE);

			/* 総コピー時間を推量(ログ/設定値ファイル時間 + 読み込み専用ファイル時間) */
			tTime = lsTime + rsTime;

			/* USBパーティション空きサイズ取得 */
			ret = ELOG_GetPartitionInfoMB_All(ELOG_FS_USB,
			&usbInfo.avail, &usbInfo.used, &usbInfo.all);
			if(ret != ELOG_OK)
			{
				DLOG_Error("(%s) error.\n", ELOG_FS_USB);
				DEBUG_TRCOUT();
				return ELOG_E_SYSCAL;
			}
			else
			{
				/* 充分な空き容量があるか */
				space = (u64)usbInfo.avail;
				if(need_space >= space)
				{
					DLOG_Error("not enough (USB=%llu vs LOG=%llu).\n",
					space, need_space);
					DEBUG_TRCOUT();
					return ELOG_E_NOSPC;
				}
				else
				{
					/* データを設定 */
					CMN_MutexLock(&(s_elogThrUSB.dMutex));

					((ElogUsbSave_t *)(s_elogThrUSB.data))->dirList  = NULL;
					((ElogUsbSave_t *)(s_elogThrUSB.data))->filelist = NULL;
					((ElogUsbSave_t *)(s_elogThrUSB.data))->tTime    = tTime;

					memcpy(&(((ElogUsbSave_t *)(s_elogThrUSB.data))->msgData),
							PcsSerial, sizeof(ELOG_PCS_SERIAL_NUM_LEN));

					CMN_MutexUnlock(&(s_elogThrUSB.dMutex));

					/* 処理中断フラグ初期化 */
					ELOG_SetUsbAbort(FALSE);

					/* コピースレッドを起動 */
					ret = ELOG_ThrStart(&s_elogThrUSB);
					if(ret != ELOG_OK)
					{
						DLOG_Error("ELOG_ThrStart() error.\n");
						DEBUG_TRCOUT();
						ret = ELOG_E_PROCS;
					}
					else
					{
						/*Do nothing*/
					}
				}
			}
		}
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :USB保存中断 ファンクション5実行処理                        */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFncAbort_t param : パラメータ                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常                                       */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_BUSY(-6)    多重起動                              */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncAbortUSB(void)
{
	slong ret = ELOG_OK;

	DEBUG_TRCIN();

	/* USB保存実行状態 */
	ret = ELOG_CheckSubThreadRun(ELOG_THTYPE_USB);
	if(ret == ELOG_E_PROCS)
	{
		DLOG_Error("ELOG_CheckSubThreadRun() error.\n");
		ret = ELOG_E_PROCS;
	}
	else if(ret == ELOG_E_BUSY)
	{
		/* 処理中断 */
		ELOG_SetUsbAbort(TRUE);
	}
	else
	{
		/*Do nothing*/
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :USB有無確認要求実行処理                                   */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常・USB有り                              */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-2)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PROCS(-3)    対象がディレクトリではない            */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_NOENT(-4)    USB無し                              */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncIsThereUSB(void)
{
	struct stat st;
	sint ret;
	schar tmpRet = ELOG_OK;
	DEBUG_TRCIN();

	ret = stat(ELOG_FS_USB, &st);
	if(ret != 0)
	{
		if(errno == ENOENT)
		{
			DLOG_Error("stat(%s) return ENOENT.\n", ELOG_FS_USB);
			tmpRet = ELOG_E_NOENT;
		}
		else
		{
			DLOG_Error("stat(%s) is failed.\n", ELOG_FS_USB);
			tmpRet = ELOG_E_SYSCAL;
		}
	}
	else
	{
		if(! S_ISDIR(st.st_mode))
		{
			DLOG_Error("%s is not dir.\n", ELOG_FS_USB);
			DEBUG_TRCIN();
			tmpRet = ELOG_E_PROCS;
		}
		else
		{
			/*Do nothing*/
		}
	}

	DEBUG_TRCOUT();
	return tmpRet;
}

/*============================================================================*/
/*    Description    :キーログ通知保存実行処理                                  */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFncOpt_t param : パラメータ                          */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                    ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/29  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncOperation(ElogFncOpt_t *param)
{
	FILE *fp;
	slong ret = ELOG_OK;
	schar fName[ELOG_FILENAME_MAX + 1];
	schar filePath[ELOG_FILEPATH_MAX + 1];
	schar dateStr[ELOG_DATE_STR_MAX + 1];

	DEBUG_TRCIN();

	ret = CheckFileSizeAndCreateFile(ELOG_FILE_SIZE_OPERATION, filePath);
	if(ret < 0)
	{
		DLOG_Error("CheckFileSizeAndCreateFile() error.\n");
		ret = ELOG_E_PROCS;
	}
	else
	{
		/* ファイル名付与用日時文字列取得 */
		memset(dateStr, 0x0, sizeof(dateStr));
		ret = ELOG_GetDStr(dateStr, sizeof(dateStr), ELOG_DATESTR_Y_S, 0);
		if(ret != ELOG_OK)
		{
			DLOG_Error("ELOG_GetDStr() error.\n");
			ret = ELOG_E_PROCS;
		}
		else
		{
			/* ファイルを追記モードでオープン */
			fp = ELOG_FOPEN(filePath, "a");
			if(fp == NULL)
			{
				DLOG_Error("ELOG_FOPEN(%s) error.(%d)\n", filePath, errno);
				DEBUG_TRCOUT();
				ret = ELOG_E_SYSCAL;
			}
			else
			{
				/* キー操作をファイルへ出力 */
				fprintf(fp, "%s %d %d %d %s\n", dateStr,
				param->ObjectType, param->PictureID,
				param->ButtonID, param->opt);
				s_elogFileSize[ELOG_FILE_SIZE_OPERATION].curlineNum++;
				ELOG_FCLOSE(fp);
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :筛选pcs 错误码类型                                       */
/*----------------------------------------------------------------------------*/
/*    param          :schar *errCode:pcs错误码                                */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_PCS_NO_ERRCODE    没有错误发生                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_PCS_SECRET_ERRCODE    隐秘错误发生                  */
/*----------------------------------------------------------------------------*/
/*                    ELOG_PCS_NORMAL_ERRCODE    正常错误发生                  */
/*----------------------------------------------------------------------------*/
/*                    ELOG_PCS_OTHER_ERRCODE    其他错误                       */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
ElogPcsErrCode_t elog_GetPcsErrCodeCategory(schar *errCode)
{
	uchar i = 0u;
	bool_t flag = TRUE;
	schar retType = ELOG_PCS_OTHER_ERRCODE;
	sint ret;

	DEBUG_TRCIN();
	ret = strncmp(errCode, "BE000", ELOG_MAX_COMP_ERR_CODE_NUM);
	if(ret == 0)
	{
		retType = ELOG_PCS_NO_ERRCODE;
	}
	else
	{
		while(i < ELOG_MAX_PCS_ERR_CODE_NUM)
		{
			ret = strncmp(errCode, s_elogPcsSecretErr[i], ELOG__ERR_LEVEL_LEN);
			if(ret == 0)
			{
				retType = ELOG_PCS_SECRET_ERRCODE;
				i = ELOG_MAX_PCS_ERR_CODE_NUM;
				flag = FALSE;
			}
			else
			{
				/*Do nothing*/
			}
			i++;
		}
		if(flag)
		{
			i = 0u;
			while(i < ELOG_MAX_PCS_ERR_CODE_NUM)
			{
				ret = strncmp(errCode, s_elogPcsErr[i], ELOG__ERR_LEVEL_LEN);
				if(ret == 0)
				{
					retType = ELOG_PCS_NORMAL_ERRCODE;
					i = ELOG_MAX_PCS_ERR_CODE_NUM;
				}
				i++;
			}
		}
	}

	DEBUG_TRCOUT();
	return retType;
}
/*============================================================================*/
/*    Description    :判断存入什么文件                                         */
/*----------------------------------------------------------------------------*/
/*    param          :schar *errCodeBe:上次发生的错误码                        */
/*----------------------------------------------------------------------------*/
/*                   :schar *errCodeCurr：这次发生的错误码                     */
/*----------------------------------------------------------------------------*/
/*                   :ElogDiffErrType_t type:错误码类型                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_SAVE_FILE_NOWRITE    不写入文件                     */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_SAVE_FILE_ONLY_SERVICE    只写入服务日志文件         */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_SAVE_FILE_DOUBLE    服务日志和通知日志都写入         */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/04  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
ElogSaveFile_t elog_JudgeSaveFile(schar *errCodeBe, schar *errCodeCurr,
								ElogDiffErrType_t type)
{
	ElogPcsErrCode_t retBe = ELOG_PCS_NO_ERRCODE;
	ElogPcsErrCode_t retCurr = ELOG_PCS_NO_ERRCODE;
	slong ret = ELOG_SAVE_FILE_NOWRITE;
	uchar i;
	DEBUG_TRCIN();

	if(type == ELOG_DIFF_PCS_ERR)
	{
		retBe = elog_GetPcsErrCodeCategory(errCodeBe);
		retCurr = elog_GetPcsErrCodeCategory(errCodeCurr);
		if(retBe == ELOG_PCS_NO_ERRCODE)
		{
			if(retCurr == ELOG_PCS_NORMAL_ERRCODE)
			{
				ret = ELOG_SAVE_FILE_DOUBLE;
			}
			else if(retCurr == ELOG_PCS_SECRET_ERRCODE)
			{
				ret = ELOG_SAVE_FILE_ONLY_SERVICE;
			}
			else
			{
				ret = ELOG_SAVE_FILE_NOWRITE;
			}
		}
		else if(retBe == ELOG_PCS_SECRET_ERRCODE)
		{
			if(retCurr == ELOG_PCS_NORMAL_ERRCODE)
			{
				ret = ELOG_SAVE_FILE_DOUBLE;
			}
			else if(retCurr == ELOG_PCS_SECRET_ERRCODE)
			{
				ret = ELOG_SAVE_FILE_ONLY_SERVICE;
			}
			else if(retCurr == ELOG_PCS_NO_ERRCODE)
			{
				ret = ELOG_SAVE_FILE_DOUBLE;
			}
			else
			{
				ret = ELOG_SAVE_FILE_NOWRITE;
			}
		}
		else if(retBe == ELOG_PCS_NORMAL_ERRCODE)
		{
			if((retCurr == ELOG_PCS_NORMAL_ERRCODE) ||
				(retCurr == ELOG_PCS_NO_ERRCODE))
			{
				ret = ELOG_SAVE_FILE_DOUBLE;
			}
			else if(retCurr == ELOG_PCS_SECRET_ERRCODE)
			{
				memset(errCodeCurr, 0, ELOG_ERR_CODE);
				strncpy(errCodeCurr, "BE000", ELOG_ERR_CODE - 1);
				ret = ELOG_SAVE_FILE_DOUBLE;
			}
			else
			{
				ret = ELOG_SAVE_FILE_NOWRITE;
			}
		}
		else
		{
			ret = ELOG_SAVE_FILE_NOWRITE;
		}
	}
	else if(type == ELOG_DIFF_E1_ERR)
	{
		for(i = 0u; i < ELOG_MAX_E1_ERR_CODE_NUM; i++)
		{
			ret = strncmp(s_elogE1ErrCodeList[i].E1ErrCode, errCodeCurr,
						ELOG_MAX_COMP_ERR_CODE_NUM);
			if(ret == 0)
			{
				if(s_elogE1ErrCodeList[i].E1Infor)
				{
					ret = ELOG_SAVE_FILE_DOUBLE;
					i = ELOG_MAX_E1_ERR_CODE_NUM;
				}
				else
				{
					if(s_elogE1ErrCodeList[i].E1Service)
					{
						ret = ELOG_SAVE_FILE_ONLY_SERVICE;
						i = ELOG_MAX_E1_ERR_CODE_NUM;
					}
					else
					{
						ret = ELOG_SAVE_FILE_NOWRITE;
						i = ELOG_MAX_E1_ERR_CODE_NUM;
					}
				}
			}
			else
			{
				ret = ELOG_SAVE_FILE_NOWRITE;
			}
		}
	}
	else
	{
		ret = ELOG_SAVE_FILE_ONLY_SERVICE;
	}
	DEBUG_TRCOUT();
	return ret;
}
/*============================================================================*/
/*    Description    :PCS動作ログ保存実行処理                                   */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFncErrEvent_t param : Function7パラメータ            */
/*----------------------------------------------------------------------------*/
/*                    bool_t sync : TRUE=SYNC or FALSE=ASYNC call             */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/20  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncSaveErrorEventLog(ElogFncErrEvent_t *param)
{
	slong ret = ELOG_OK;
	schar filePath[ELOG_FILEPATH_MAX + 1];
	schar dStr[ELOG_DATE_STR_MAX + 1];
	schar currErrCode[ELOG_ERR_CODE];
	FILE *fpServ;
	bool_t flag = TRUE;
	FILE *fpInfor;
	ElogSaveFile_t saveFile;
	DEBUG_TRCIN();
	memset(currErrCode, 0, ELOG_ERR_CODE);
	strncpy(currErrCode, param->currErrCode, ELOG_ERR_CODE - 1);

	saveFile = elog_JudgeSaveFile(s_beforErrCode, currErrCode, param->type);
	/* 写入服务log文件 */
	if(saveFile != ELOG_SAVE_FILE_NOWRITE)
	{
		/* 检查文件大小，并获取文件路径 */
		ret = CheckFileSizeAndCreateFile(ELOG_FILE_SIZE_SERVICE,
										filePath);
		if(ret < ELOG_OK)
		{
			DLOG_Error("CheckFileSizeAndCreateFile() error.\n");
			ret = ELOG_E_PROCS;
			flag = FALSE;
		}
		else
		{
			/* ファイル名付与用日時文字列取得 */
			memset(dStr, 0x0, sizeof(dStr));
			ret = ELOG_GetDStr(dStr, sizeof(dStr), ELOG_DATESTR_Y_S, 0);
			if(ret < ELOG_OK)
			{
				DLOG_Error("ELOG_GetDStr() error.\n");
				ret = ELOG_E_PROCS;
				flag = FALSE;
			}
			else
			{
				fpServ = ELOG_FOPEN(filePath, "a");
				if(fpServ == NULL)
				{
					DLOG_Error("ELOG_FOPEN(%s) error.(%d)\n", filePath, errno);
					ret = ELOG_E_SYSCAL;
					flag = FALSE;
				}
				else
				{
					if(param->type == ELOG_DIFF_RMC)
					{
						fprintf(fpServ, "%s %s\n", dStr, currErrCode);
						s_elogFileSize[ELOG_FILE_SIZE_SERVICE].curlineNum++;
					}
					else
					{
						fprintf(fpServ, "%s %s %c\n", dStr,
						param->currErrCode, param->errLevel);
						s_elogFileSize[ELOG_FILE_SIZE_SERVICE].curlineNum++;
					}
					ELOG_FCLOSE(fpServ);
				}
			}
		}
	}
	else
	{
		/*Do nothing*/
	}
	/* 写入通知log文件 */
	if((flag) && (saveFile == ELOG_SAVE_FILE_DOUBLE))
	{
		/* 检查文件大小，并获取文件路径 */
		ret = CheckFileSizeAndCreateFile(ELOG_FILE_SIZE_INFOR, filePath);
		if(ret < ELOG_OK)
		{
			DLOG_Error("CheckFileSizeAndCreateFile() error.\n");
			ret = ELOG_E_PROCS;
		}
		else
		{
			fpInfor = ELOG_FOPEN(filePath, "a");
			if(fpInfor == NULL)
			{
				DLOG_Error("ELOG_FOPEN(%s) error.(%d)\n", filePath, errno);
				ret = ELOG_E_SYSCAL;
			}
			else
			{
				if(s_elogInforSendMsg.elogInformNum < ELOG_INFOR_FILES_LIM)
				{
					s_elogInforSendMsg.elogInformNum++;
				}
				else
				{
					/*Do nothing*/
				}

				fprintf(fpInfor, "%s %s %c\n",
				dStr, currErrCode, param->errLevel);
				s_elogFileSize[ELOG_FILE_SIZE_INFOR].curlineNum++;
				ELOG_FCLOSE(fpInfor);
			}
		}
	}
	else
	{
		/*Do nothing*/
	}
	if(param->type == ELOG_DIFF_PCS_ERR)
	{
		/* 记录本次写入的pcs错误 */
		memset(s_beforErrCode, 0, ELOG_ERR_CODE);
		strncpy(s_beforErrCode, param->currErrCode, ELOG_ERR_CODE - 1);
	}
	else
	{
		/*Do nothing*/
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :动作履历log                                              */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-1)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/30  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncSaveActionLog(ElogFncAction_t *param)
{
	schar newPath[ELOG_FILEPATH_MAX + 1];
	schar dateStr[ELOG_DATE_STR_MAX + 1];
	slong ret = ELOG_OK;
	FILE *fp;
	EtimNtpState_t state = ETIM_STATE_NTP_SYNC_OK;
	ElogCmd60_t CMD60Action;
	uchar buf[ELOG_SERIES_NINE_LEN];
	DEBUG_TRCIN();

	/* NAND容量回復スレッド状態確認、中断 */
	ret = ELOG_AbortAndWaitNandDelete(FALSE);
	if(ret != ELOG_OK)
	{
		DLOG_Error("ELOG_AbortAndWaitNandDelete() error.\n");
		ret = ELOG_E_PROCS;
	}
	else
	{
		/*获取通信板FW版本号*/
		memset(buf, 0x0, sizeof(buf));
		EEEP_GetBoardSeries(buf);

		/*pcs系统编号从inv获取*/
		/*INV_getCMD60(&CMD60Action);*/

		memset(dateStr, 0x0, sizeof(dateStr));
		ret = ELOG_GetDStr(dateStr, sizeof(dateStr), ELOG_DATESTR_DCS, 0);
		if(ret != ELOG_OK)
		{
			DLOG_Error("ELOG_GetDStr() error.\n");
			ret = ELOG_E_PROCS;
		}
		else
		{
			memset(newPath, 0x0, sizeof(newPath));
			/*通信板时间未设定时,不同的命名规则*/
			ETIM_SyncGetSyncState(&state);

			if(state == ETIM_STATE_NTP_SYNC_OK)
			{
				snprintf(newPath, sizeof(newPath)-1, "%s/%s-%s-%s.txt",
				ELOG_ACTION_DIR, buf, CMD60Action.sysMadeNum, dateStr);
			}
			else
			{
				snprintf(newPath, sizeof(newPath)-1, "%s/TMP-%s-%s-%s.txt",
				ELOG_ACTION_DIR, buf, CMD60Action.sysMadeNum, dateStr);
			}

			fp = ELOG_FOPEN(newPath, "a");
			if(fp == NULL)
			{
				DLOG_Error("ELOG_FOPEN(%s) error.(%d)\n", newPath, errno);
				ret = ELOG_E_SYSCAL;
			}
			else
			{
				fprintf(fp, "%s\n", param->actionLog);		/*CMD62命令中的流*/
				ELOG_FCLOSE(fp);
				ret = ELOG_OK;
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}
/*============================================================================*/
/*    Description    :动作履历压缩                                             */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncCompressActionLog(ElogFncActionComp_t *param)
{
	slong ret = ELOG_OK;
	ElogTgzSave_t *pData;
	EtimNtpState_t state = ETIM_STATE_NTP_SYNC_OK;
	char dateStr[ELOG_DATE_STR_MAX + 1];
	uchar buf[ELOG_SERIES_LEN];
	DEBUG_TRCIN();

	/* サブスレッド実行状態確認 */
	ELOG_CheckSubThreadRun(ELOG_THTYPE_USB | ELOG_THTYPE_CPRS);

	memset(buf, 0x0, sizeof(buf));
	EEEP_GetBoardSeries(buf);

	memset(dateStr, 0x0, sizeof(dateStr));
	ret = ELOG_GetDStr(dateStr, sizeof(dateStr), ELOG_DATESTR_YMD , 0);
	if(ret != ELOG_OK)
	{
		DLOG_Error("ELOG_GetDStr() error.\n");
		ret = ELOG_E_PROCS;
	}
	else
	{
		/* 実行パラメータ設定 */
		CMN_MutexLock(&(s_elogThrCompress.dMutex));
		pData = (ElogTgzSave_t *)s_elogThrCompress.data;
		pData->type = ELOG_TYPE_ACZ;
		memcpy(&pData->header, &param->header, sizeof(pData->header));
		pData->del = param->del;
		CMN_MutexUnlock(&(s_elogThrCompress.dMutex));

		memset(pData->fileName, 0x0, sizeof(pData->fileName));
		ETIM_SyncGetSyncState(&state);
		if(state == ETIM_STATE_NTP_SYNC_OK)
		{
			/* フルパスファイル名作成 */
			snprintf(pData->fileName, sizeof(pData->fileName), "%s-%s.tgz",
			buf, dateStr);
		}
		else
		{
			snprintf(pData->fileName, sizeof(pData->fileName), "TMP-%s-%s.tgz",
			buf, dateStr);
		}


		/* 圧縮コマンド実行スレッドを起動 */
		ret = ELOG_ThrStart(&s_elogThrCompress);
		if(ret != ELOG_OK)
		{
			DLOG_Error("ELOG_ThrStart() error.\n");
			/* 処理不可イベント */
			ELOG_SendCompressEvent(param->header.destModId,
			ELOG_COMPRESS_OTHERR, ELOG_TYPE_ACZ, NULL, FALSE);
			ret = ELOG_E_PROCS;
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
/*    Description    :SOH 推移ログ                                             */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-1)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/30  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncSOHpercentage(ElogFncSOH_t *param)
{
	sint data;
	slong ret = ELOG_OK;
	FILE *fp;
	schar dateStr[ELOG_DATE_STR_MAX + 1];
	schar filePath[ELOG_FILEPATH_MAX + 1];

	DEBUG_TRCIN();

	ret = CheckFileSizeAndCreateFile(ELOG_FILE_SIZE_SOH, filePath);
	if(ret < 0)
	{
		DLOG_Error("CheckFileSizeAndCreateFile() error.\n");
		DEBUG_TRCOUT();
		ret = ELOG_E_PROCS;
	}
	else
	{
		fp = ELOG_FOPEN(filePath, "a");
		if(fp == NULL)
		{
			DLOG_Error("ELOG_FOPEN(%s) error.(%d)\n", filePath, errno);
			DEBUG_TRCOUT();
			ret = ELOG_E_SYSCAL;
		}
		else
		{
			/* ファイル名付与用日時文字列取得 */
			memset(dateStr, 0x0, sizeof(dateStr));
			ret = ELOG_GetDStr(dateStr, sizeof(dateStr), ELOG_DATESTR_YMD, 0);
			if(ret != ELOG_OK)
			{
				DLOG_Error("ELOG_GetDStr() error.\n");
				ELOG_FCLOSE(fp);
				ret = ELOG_E_PROCS;
			}
			else
			{
				fprintf(fp, "%s %d%%\n", dateStr, param->percentage);
				s_elogFileSize[ELOG_FILE_SIZE_SOH].curlineNum++;
			}
		}
		ELOG_FCLOSE(fp);
	}

	DEBUG_TRCOUT();
	return ret;
}
/*============================================================================*/
/*    Description    :电压上升抑制log                                          */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFncVoltHist_t *param:ElogFncVoltHistパラメータ       */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-1)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/30  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncPowerVoltHist(ElogFncVoltHist_t *param)
{
	sint data;
	slong ret = ELOG_OK;
	FILE *fp;
	schar filePath[ELOG_FILEPATH_MAX + 1];
	EtimTm_t etm;
	schar flag;
	EtimNtpState_t timStat = ETIM_STATE_NTP_SYNC_OK;
	DEBUG_TRCIN();

	ret = CheckFileSizeAndCreateFile(ELOG_FILE_SIZE_POWER_VOLT_HIST, filePath);
	if(ret < 0)
	{
		DLOG_Error("CheckFileSizeAndCreateFile() error.\n");
		DEBUG_TRCOUT();
		ret = ELOG_E_PROCS;
	}
	else
	{
		fp = ELOG_FOPEN(filePath, "a");
		if(fp == NULL)
		{
			DLOG_Error("ELOG_FOPEN(%s) error.(%d)\n", filePath, errno);
			ret = ELOG_E_SYSCAL;
		}
		else
		{
			ret = ETIM_GetTime(&etm);
			if(ret != ELOG_OK)
			{
				DLOG_Error("ETIM_GetTime() error.\n");
				ret = ELOG_E_SYSCAL;
			}
			else
			{
				ETIM_SyncGetSyncState(&timStat);

				if(timStat != ETIM_STATE_NTP_SYNC_OK)
				{
					DLOG_Error("ETIM_SyncGetSyncState() error.\n");
					fprintf(fp, "----/--/--- ");
					ret = ELOG_E_PROCS;
				}
				else
				{
					/*Do nothing*/
				}
			}
		}
	}
	if(ret == ELOG_OK)
	{
		/*EINV_SUPPRESS_STATE_VOLT_RISE = 1*/
		if(param->state == 1)
		{
			fprintf(fp, "%4d%02d%02d 发生%02d%02d",
					etm.tmYear, etm.tmMon, etm.tmMday,
					etm.tmHour, etm.tmMin);
			s_elogVoltHistBeTm = etm.tmMday;
			s_elogFileSize[ELOG_FILE_SIZE_POWER_VOLT_HIST].curlineNum++;
		}
		else 
		{
			if(s_elogVoltHistBeTm == etm.tmMday)
			{
				fprintf(fp, " 复归%02d%02d\n",etm.tmHour, etm.tmMin);
			}
			else
			{
				fprintf(fp, "\n%4d%02d%02d         复归%02d%02d\n",
				etm.tmYear, etm.tmMon, etm.tmMday, etm.tmHour, etm.tmMin);
				s_elogFileSize[ELOG_FILE_SIZE_POWER_VOLT_HIST].curlineNum++;
			}
		}
	}

	ELOG_FCLOSE(fp);
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :通过HEMS被进行发电抑制操作的log                           */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFncHemsHistory_t *param:ElogFncHemsHistoryパラメータ */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-1)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/30  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncHEMSPowerEdge(ElogFncHemsHistory_t *param)
{
	sint data;
	slong ret = ELOG_OK;
	FILE *fp;
	schar filePath[ELOG_FILEPATH_MAX + 1];
	EtimTm_t etm;
	EtimNtpState_t state = ETIM_STATE_NTP_SYNC_OK;
	DEBUG_TRCIN();

	ret = CheckFileSizeAndCreateFile(ELOG_FILE_SIZE_HEMS_HISTORY, filePath);
	if(ret < ELOG_OK)
	{
		DLOG_Error("CheckFileSizeAndCreateFile() error.\n");
		DEBUG_TRCOUT();
		ret = ELOG_E_PROCS;
	}
	else
	{
		ret = ETIM_GetTime(&etm);
		if(ret != ELOG_OK)
		{
			DLOG_Error("ETIM_GetTime() error.\n");
			ret = ELOG_E_SYSCAL;
		}
		else
		{
			fp = ELOG_FOPEN(filePath, "a");
			if(fp == NULL)
			{
				DLOG_Error("ELOG_FOPEN(%s) error.(%d)\n", filePath, errno);
				ret = ELOG_E_SYSCAL;
			}
			else
			{
				/*Do nothing*/
			}
		}
	}
	if(ret == ELOG_OK)
	{
		ETIM_SyncGetSyncState(&state);
		if(state != ETIM_STATE_NTP_SYNC_OK)
		{
			fprintf(fp, "----/--/--- %d%%\n", param->percentage);
		}
		else
		{
			/*从ECHONET Lite 获取制御状态*/
			if(param->state == 1) 
			{
				fprintf(fp, "%4d%02d%02d %02d%02d-",
						etm.tmYear, etm.tmMon, etm.tmMday,
						etm.tmHour, etm.tmMin);
				s_elogHemsHistoryBeTm = etm.tmMday;
				s_elogFileSize[ELOG_FILE_SIZE_HEMS_HISTORY].curlineNum++;
			}
			else
			{
				if(s_elogHemsHistoryBeTm == etm.tmMday)
				{
					fprintf(fp, "%02d%02d %d%%\n",
					etm.tmHour, etm.tmMin, param->percentage);
				}
				else
				{
					fprintf(fp, "\n%4d%02d%02d        -%02d%02d %d%%\n",
					etm.tmYear, etm.tmMon, etm.tmMday, etm.tmHour,
					etm.tmMin, param->percentage);
					s_elogFileSize[ELOG_FILE_SIZE_HEMS_HISTORY].curlineNum++;
				}
			}
			ret = ELOG_OK;
		}
	ELOG_FCLOSE(fp);
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :分布式电源                                               */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFncPowerTotal_t *param:ElogFncPowerTotalパラメータ   */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-1)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/30  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncPowerTotal(ElogFncPowerTotal_t *param)
{
	slong ret = ELOG_OK;
	schar newPath[ELOG_FILEPATH_MAX + 1];
	schar dateStr[ELOG_DATE_STR_MAX + 1];
	FILE *fp;

	DEBUG_TRCIN();
	ret = CheckFileNumAndCreateFile(param->type, newPath, ELOG_DATESTR_YMD);
	if(ret != ELOG_OK)
	{
		DLOG_Error("CheckFileNumAndCreateFile() error.\n");
		DEBUG_TRCOUT();
		ret = ELOG_E_PROCS;
	}
	else
	{
		/* ファイルを追記モードでオープン */
		fp = ELOG_FOPEN(newPath, "a");
		if(fp == NULL)
		{
			DLOG_Error("ELOG_FOPEN(%s) error.(%d)\n", newPath, errno);
			ret = ELOG_E_SYSCAL;
		}
		else
		{
			memset(dateStr, 0x0, sizeof(dateStr));
			ret = ELOG_GetDStr(dateStr, sizeof(dateStr), ELOG_DATESTR_HMS, 0);
			if(ret != ELOG_OK)
			{
				DLOG_Error("ELOG_GetDStr() error.\n");
				ret = ELOG_E_PROCS;
			}
			else
			{
				fprintf(fp, "%s[%08x]\n", dateStr, param->PowerTotal);
			}
		}
	}
	ELOG_FCLOSE(fp);
	DEBUG_TRCOUT();
	return ret;

}
/*============================================================================*/
/*    Description    :出力制御ログ                                             */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFncOutPutCtrl_t *param:ElogFncOutPutCtrl パラメータ  */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-1)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/20  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncOutputCtrl(ElogFncOutPutCtrl_t *param)
{
	slong ret = ELOG_OK;
	schar newPath[ELOG_FILEPATH_MAX + 1];
	schar dateStrAlarm[ELOG_DATE_STR_MAX*2 + 1];
	schar dateStrCurrent[ELOG_DATE_STR_MAX + 1];
	FILE *fp;
	ElogCmd81_t CMD81OutputCtrl;
	DEBUG_TRCIN();

	ret = CheckFileNumAndCreateFile(ELOG_FILE_NUM_OUTPUTCTRL, 
									newPath, ELOG_DATESTR_YMD);
	if(ret != ELOG_OK)
	{
		DLOG_Error("CheckFileNumAndCreateFile() error.\n");
		ret = ELOG_E_PROCS;
	}
	else
	{
		ret = ELOG_GetDStr(dateStrAlarm, sizeof(dateStrAlarm),
						ELOG_DATESTR_OUPUTCTRL_SECTION, ELOG_OUTPUTCTRL_TIMER);
		if(ret != ELOG_OK)
		{
			DLOG_Error("ELOG_GetDStr() error.\n");
			ret = ELOG_E_PROCS;
		}
		else
		{
			ret = ELOG_GetDStr(dateStrCurrent, sizeof(dateStrCurrent),
								ELOG_DATESTR_OUTPUTCTRL, 0);
			if(ret != ELOG_OK)
			{
				DLOG_Error("ELOG_GetDStr() error.\n");
				ret = ELOG_E_PROCS;
			}
			else
			{
				/* ファイルを追記モードでオープン */
				fp = ELOG_FOPEN(newPath, "a");
				if(fp == NULL)
				{
					DLOG_Error("ELOG_FOPEN(%s) error.(%d)\n", newPath, errno);
					ret = ELOG_E_SYSCAL;
				}
				else
				{
					/*EINV_getCMD81(&CMD81OutputCtrl);*/
					/*EINV_NO_SUPPRESS*/
					if(CMD81OutputCtrl.suppressState != 0)
					{
						fprintf(fp, "%s %d%%\n",
						dateStrAlarm, param->percentage);
					}
					else
					{
						fprintf(fp, "%s -%%\n", dateStrCurrent);
					}
				}
			}
		}
	}
	ELOG_FCLOSE(fp);
	DEBUG_TRCOUT();
	return ELOG_OK;
}
/*============================================================================*/
/*    Description    :发电/买卖电log                                           */
/*----------------------------------------------------------------------------*/
/*    param          :ElogPowerDeal_t *param:ElogPowerDeal パラメータ          */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-1)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/30  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncSavePowerDealLog(ElogPowerDeal_t *param)
{
	slong ret = ELOG_OK;
	schar filePathDay[ELOG_FILEPATH_MAX + 1];
	schar filePathMonth[ELOG_FILEPATH_MAX + 1];
	schar filePathYear[ELOG_FILEPATH_MAX + 1];
	schar fileCmd[ELOG_FILEPATH_MAX*2 + 1];
	EtimTm_t etm;
	FILE *fp;
	ElogCmd81_t CMD81Power;
	double pvGenPwr2;
	double pvGenPwr;
	double allPwrOut;
	double allPwrIn;

	DEBUG_TRCIN();

	/* NAND容量回復スレッド状態確認、中断 */
	ret = ELOG_AbortAndWaitNandDelete(FALSE);
	if(ret != ELOG_OK)
	{
		DLOG_Error("ELOG_AbortAndWaitNandDelete() error.\n");
		ret = ELOG_E_PROCS;
	}
	else
	{
		/* 現ログファイル確認＋最古ファイル削除 */
		ret = CheckFileNumAndCreateFile(ELOG_FILE_NUM_POWER_DEAL_DAY,
										filePathDay, ELOG_DATESTR_YMD);
		if(ret != ELOG_OK)
		{
			DLOG_Error("CheckFileNumAndCreateFile() error.\n");
			ret = ELOG_E_PROCS;
		}
		else
		{
			/* EINV_getCMD81(&CMD81Power); */

			pvGenPwr2 = CMD81Power.ttlPvGenPwr2 - s_elogPV.LastDayPvGenPwr2;
			pvGenPwr = CMD81Power.ttlPvGenPwr - s_elogPV.LastDayPvGenPwr;
			allPwrOut = CMD81Power.ttlSellingPwr - s_elogPV.LastDayallPwrOut;
			allPwrIn = CMD81Power.ttlPurchasePwr - s_elogPV.LastDayallPwrIn;

			s_elogPV.LastDayPvGenPwr2 = CMD81Power.ttlPvGenPwr2;
			s_elogPV.LastDayPvGenPwr = CMD81Power.ttlPvGenPwr;
			s_elogPV.LastDayallPwrOut = CMD81Power.ttlSellingPwr;
			s_elogPV.LastDayallPwrIn = CMD81Power.ttlPurchasePwr;

			ret = ETIM_GetTime(&etm);
			if(ret != ELOG_OK)
			{
				DLOG_Error("ETIM_GetTime() error.\n");
				DEBUG_TRCOUT();
				ret = ELOG_E_SYSCAL;
			}
			else
			{
				if((s_elogPV.tmMday == 0) ||
					(s_elogPV.tmMon == 0) ||
					(s_elogPV.tmYear == 0))
				{
					s_elogPV.tmMday = etm.tmMday;
					s_elogPV.tmMon = etm.tmMon;
					s_elogPV.tmYear = etm.tmYear;
				}
				else
				{
					/*Do nothing*/
				}
			}
		}
	}

	if(ret == ELOG_OK)
	{
		/* 日的文件作成 */
		memset(filePathDay, 0x0, sizeof(filePathDay));

		snprintf(filePathDay, sizeof(filePathDay)-1, "%s/%s%04d%02d%02d",
				ELOG_POWER_DEAL_DAY_DIR, ELOG_POWER_DEAL_FNAME_HD,
				s_elogPV.tmYear, s_elogPV.tmMon,
				s_elogPV.tmMday);

		fp = ELOG_FOPEN(filePathDay, "a");
		if(fp == NULL)
		{
			DLOG_Error("CMN_FOPEN(%s) error.(%d)\n", filePathDay, errno);
			DEBUG_TRCOUT();
			ret = ELOG_E_SYSCAL;
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
	if(ret == ELOG_OK)
	{
		fprintf(fp, "%02d:00 %.1f %.1f %.1f %.1f\n", etm.tmHour, pvGenPwr2,
				pvGenPwr, allPwrOut, allPwrIn);
		ELOG_FCLOSE(fp);
		/*日的累计*/
		if((etm.tmMday == s_elogPV.tmMday) || (etm.tmHour == 0))
		{
			s_elogPV.DaySumPvGenPwr2 += pvGenPwr2;
			s_elogPV.MonthSumPvGenPwr2 += s_elogPV.DaySumPvGenPwr2;
			s_elogPV.DaySumPvGenPwr += pvGenPwr;
			s_elogPV.MonthSumPvGenPwr += s_elogPV.DaySumPvGenPwr;
			s_elogPV.DaySumallPwrOut += allPwrOut;
			s_elogPV.MonthSumallPwrOut += s_elogPV.DaySumallPwrOut;
			s_elogPV.DaySumallPwrIn += allPwrIn;
			s_elogPV.MonthSumallPwrIn += s_elogPV.DaySumallPwrIn;

			/* 月的文件作成 */
			memset(filePathMonth, 0x0, sizeof(filePathMonth));
			snprintf(filePathMonth, sizeof(filePathMonth)-1, "%s/%s%04d%02d",
			ELOG_POWER_DEAL_MONTH_DIR, ELOG_POWER_DEAL_FNAME_HD,
			s_elogPV.tmYear, s_elogPV.tmMon);

			memset(fileCmd, 0x0, sizeof(fileCmd));
			snprintf(fileCmd, sizeof(fileCmd)-1, "sed -i '$d' %s",
					filePathMonth);
			CMN_MutexLock(&s_elogSysCmdMutx);
			ELOG_CALL_SYSTEM(fileCmd);
			CMN_MutexUnlock(&s_elogSysCmdMutx);

			fp = ELOG_FOPEN(filePathMonth, "a");
			if(fp == NULL)
			{
				DLOG_Error("CMN_FOPEN(%s) error.(%d)\n", filePathMonth, errno);
				DEBUG_TRCOUT();
				ret = ELOG_E_SYSCAL;
			}
			else
			{
				fprintf(fp, "%02d %.1f %.1f %.1f %.1f\n", 
				etm.tmMday, s_elogPV.DaySumPvGenPwr2,
				s_elogPV.DaySumPvGenPwr,
				s_elogPV.DaySumallPwrOut,
				s_elogPV.DaySumallPwrIn);
				ELOG_FCLOSE(fp);

				/* 年的文件作成 */
				memset(filePathYear, 0x0, sizeof(filePathYear));
				snprintf(filePathYear, sizeof(filePathYear)-1, "%s/%s%04d", 
				ELOG_POWER_DEAL_YEAR_DIR, ELOG_POWER_DEAL_FNAME_HD,
				s_elogPV.tmYear);

				memset(fileCmd, 0x0, sizeof(fileCmd));
				snprintf(fileCmd, sizeof(fileCmd)-1, "sed -i '$d' %s",
						filePathYear);
				CMN_MutexLock(&s_elogSysCmdMutx);
				ELOG_CALL_SYSTEM(fileCmd);
				CMN_MutexUnlock(&s_elogSysCmdMutx);
				fp = ELOG_FOPEN(filePathYear, "a");
				if(fp == NULL)
				{
					DLOG_Error("CMN_FOPEN(%s) error.(%d)\n",
							filePathYear, errno);
					DEBUG_TRCOUT();
					ret = ELOG_E_SYSCAL;
				}
				else
				{
					fprintf(fp, "%02d %.1f %.1f %.1f %.1f\n",
					etm.tmMon, s_elogPV.MonthSumPvGenPwr2,
					s_elogPV.MonthSumPvGenPwr,
					s_elogPV.MonthSumallPwrOut,
					s_elogPV.MonthSumallPwrIn);
					ELOG_FCLOSE(fp);
				}
			}
		}
		else
		{
			ret = CheckFileNumAndCreateFile(ELOG_FILE_NUM_POWER_DEAL_MONTH,
			filePathMonth, ELOG_DATESTR_YM);
			if(ret != ELOG_OK)
			{
				DLOG_Error("CheckFileNumAndCreateFile() error.\n");
				ret = ELOG_E_PROCS;
			}
			else
			{
				ret = CheckFileNumAndCreateFile(ELOG_FILE_NUM_POWER_DEAL_YEAR,
				filePathYear, ELOG_DATESTR_YMD);
				if(ret != ELOG_OK)
				{
					DLOG_Error("CheckFileNumAndCreateFile() error.\n");
					ret = ELOG_E_PROCS;
				}
				else
				{
					s_elogPV.DaySumPvGenPwr2 = pvGenPwr2;
					s_elogPV.MonthSumPvGenPwr2 += s_elogPV.DaySumPvGenPwr2;
					s_elogPV.DaySumPvGenPwr = pvGenPwr;
					s_elogPV.MonthSumPvGenPwr += s_elogPV.DaySumPvGenPwr;
					s_elogPV.DaySumallPwrOut = allPwrOut;
					s_elogPV.MonthSumallPwrOut += s_elogPV.DaySumallPwrOut;
					s_elogPV.DaySumallPwrIn = allPwrIn;
					s_elogPV.MonthSumallPwrIn += s_elogPV.DaySumallPwrIn;

					fp = ELOG_FOPEN(filePathMonth, "a");
					if(fp == NULL)
					{
						DLOG_Error("CMN_FOPEN(%s) error.(%d)\n",
								filePathMonth, errno);
						ret = ELOG_E_SYSCAL;
					}
					else
					{
						/*Do nothing*/
					}
				}
			}
			if(ret == ELOG_OK)
			{
				fprintf(fp, "%02d %.1f %.1f %.1f %.1f\n",
				etm.tmMday,s_elogPV.DaySumPvGenPwr2,
				s_elogPV.DaySumPvGenPwr,
				s_elogPV.DaySumallPwrOut,
				s_elogPV.DaySumallPwrIn);
				ELOG_FCLOSE(fp);

				if((etm.tmMon == s_elogPV.tmMon))
				{
					memset(fileCmd, 0x0, sizeof(fileCmd));
					snprintf(fileCmd, sizeof(fileCmd)-1, "sed -i '$d' %s",
							filePathYear);
					CMN_MutexLock(&s_elogSysCmdMutx);
					ret = ELOG_CALL_SYSTEM(fileCmd);
					CMN_MutexUnlock(&s_elogSysCmdMutx);

					fp = ELOG_FOPEN(filePathYear, "a");
					if(fp == NULL)
					{
						DLOG_Error("CMN_FOPEN(%s) error.(%d)\n",
								filePathYear, errno);
						ret = ELOG_E_SYSCAL;
					}
					else
					{
						fprintf(fp, "%02d %.1f %.1f %.1f %.1f\n",
						etm.tmMon, s_elogPV.MonthSumPvGenPwr2,
						s_elogPV.MonthSumPvGenPwr,
						s_elogPV.MonthSumallPwrOut,
						s_elogPV.MonthSumallPwrIn);
						ELOG_FCLOSE(fp);
					}
				}
				else
				{
					s_elogPV.MonthSumPvGenPwr2 = s_elogPV.DaySumPvGenPwr2;
					s_elogPV.MonthSumPvGenPwr = s_elogPV.DaySumPvGenPwr;
					s_elogPV.MonthSumallPwrOut = s_elogPV.DaySumallPwrOut;
					s_elogPV.MonthSumallPwrIn = s_elogPV.DaySumallPwrIn;

					fp = ELOG_FOPEN(filePathYear, "a");
					if(fp == NULL)
					{
						DLOG_Error("CMN_FOPEN(%s) error.(%d)\n",
								filePathYear, errno);
						ret = ELOG_E_SYSCAL;
					}
					else
					{
						fprintf(fp, "%02d %.1f %.1f %.1f %.1f\n",
						etm.tmMon, s_elogPV.MonthSumPvGenPwr2,
						s_elogPV.MonthSumPvGenPwr,
						s_elogPV.MonthSumallPwrOut,
						s_elogPV.MonthSumallPwrIn);
						ELOG_FCLOSE(fp);
					}
				}
			}
		}
	}
	else
	{
		/*Do nothing*/
	}

	s_elogPV.tmMday = etm.tmMday;
	s_elogPV.tmMon = etm.tmMon;
	s_elogPV.tmYear = etm.tmYear;

	return ret;
}

/*============================================================================*/
/*    Description    :蓄电池充放电                                             */
/*----------------------------------------------------------------------------*/
/*    param          :ElogAccumulator_t *param:ElogAccumulator パラメータ      */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-1)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/30  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncSaveAccumulatorLog(ElogAccumulator_t *param)
{
	slong ret = ELOG_OK;
	schar filePathDay[ELOG_FILEPATH_MAX + 1];
	schar filePathMonth[ELOG_FILEPATH_MAX + 1];
	schar filePathYear[ELOG_FILEPATH_MAX + 1];
	schar fileCmd[ELOG_FILEPATH_MAX*2 + 1];
	EtimTm_t etm;
	FILE *fp;
	ElogCmd81_t CMD81Power;
	double accPowerIn;
	double accPowerOut;
	double allPwrOut;
	double allPwrIn;
	double rsoc;
	DEBUG_TRCIN();

	/* NAND容量回復スレッド状態確認、中断 */
	ret = ELOG_AbortAndWaitNandDelete(FALSE);
	if(ret != ELOG_OK)
	{
		DLOG_Error("ELOG_AbortAndWaitNandDelete() error.\n");
		ret = ELOG_E_PROCS;
	}
	else
	{
		/* 現ログファイル確認＋最古ファイル削除 */
		ret = CheckFileNumAndCreateFile(ELOG_FILE_NUM_ACCUMULATOR_DAY,
										filePathDay, ELOG_DATESTR_YMD);
		if(ret != ELOG_OK)
		{
			DLOG_Error("CheckFileNumAndCreateFile() error.\n");
			ret = ELOG_E_PROCS;
		}
		else
		{
			/*Do nothing*/
		}
	}

	if(ret == ELOG_OK)
	{
		/*EINV_getCMD81(&CMD81Power);*/
		accPowerIn = CMD81Power.ttlChargeEnergyDC * 
						CMD81Power.pcsEfficiencyChar;
		accPowerOut = CMD81Power.ttlDischargeEnergyDC * 
						CMD81Power.pcsEfficiencyDischar;

		allPwrOut = accPowerIn - s_elogBt.LastDayallPwrOut;
		allPwrIn = accPowerOut - s_elogBt.LastDayallPwrIn;
		rsoc = CMD81Power.rsoc;

		s_elogBt.LastDayallPwrOut = accPowerOut;
		s_elogBt.LastDayallPwrIn = accPowerIn;

		ret = ETIM_GetTime(&etm);
		if(ret != ELOG_OK)
		{
			DLOG_Error("ETIM_GetTime() error.\n");
			ret = ELOG_E_SYSCAL;
		}
		else
		{
			if((s_elogBt.tmMday == 0) ||
				(s_elogBt.tmMon == 0) ||
				(s_elogBt.tmYear == 0))
			{
				s_elogBt.tmMday = etm.tmMday;
				s_elogBt.tmMon = etm.tmMon;
				s_elogBt.tmYear = etm.tmYear;
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

	if(ret == ELOG_OK)
	{
			/* 日的文件作成 */
		memset(filePathDay, 0x0, sizeof(filePathDay));

		snprintf(filePathDay, sizeof(filePathDay)-1, "%s/%s%04d%02d%02d",
		ELOG_ACCUMULATOR_DAY_DIR, ELOG_ACCUMULATOR_FNAME_HD,
		s_elogBt.tmYear, s_elogBt.tmMon,
		s_elogBt.tmMday);

		fp = ELOG_FOPEN(filePathDay, "a");
		if(fp == NULL)
		{
			DLOG_Error("CMN_FOPEN(%s) error.(%d)\n", filePathDay, errno);
			DEBUG_TRCOUT();
			ret = ELOG_E_SYSCAL;
		}
		else
		{
			fprintf(fp, "%02d:00 %.1f %.1f %.1f\n",
			etm.tmHour, allPwrOut, allPwrIn, rsoc);
			ELOG_FCLOSE(fp);
		}
	}
	else
	{
		/*Do nothing*/
	}

	if(ret == ELOG_OK)
	{
		/*日的累计*/
		if((etm.tmMday == s_elogBt.tmMday) || (etm.tmHour == 0))
		{
			s_elogBt.DaySumallPwrOut += allPwrOut;
			s_elogBt.MonthSumallPwrOut += s_elogBt.DaySumallPwrOut;
			s_elogBt.DaySumallPwrIn += allPwrIn;
			s_elogBt.MonthSumallPwrIn += s_elogBt.DaySumallPwrIn;

			/* 月的文件作成 */
			memset(filePathMonth, 0x0, sizeof(filePathMonth));
			snprintf(filePathMonth, sizeof(filePathMonth)-1, "%s/%s%04d%02d",
			ELOG_ACCUMULATOR_MONTH_DIR, ELOG_ACCUMULATOR_FNAME_HD,
			s_elogBt.tmYear, s_elogBt.tmMon);

			memset(fileCmd, 0x0, sizeof(fileCmd));
			snprintf(fileCmd, sizeof(fileCmd)-1, "sed -i '$d' %s",
					filePathMonth);
			CMN_MutexLock(&s_elogSysCmdMutx);
			ELOG_CALL_SYSTEM(fileCmd);
			CMN_MutexUnlock(&s_elogSysCmdMutx);

			fp = ELOG_FOPEN(filePathMonth, "a");
			if(fp == NULL)
			{
				DLOG_Error("CMN_FOPEN(%s) error.(%d)\n",
							filePathMonth, errno);
				DEBUG_TRCOUT();
				ret = ELOG_E_SYSCAL;
			}
			else
			{
				fprintf(fp, "%02d %.1f %.1f\n", etm.tmMday,
				s_elogBt.DaySumallPwrOut,
				s_elogBt.DaySumallPwrIn);
				ELOG_FCLOSE(fp);

				/* 年的文件作成 */
				memset(filePathYear, 0x0, sizeof(filePathYear));
				snprintf(filePathYear, sizeof(filePathYear)-1, "%s/%s%04d",
				ELOG_ACCUMULATOR_YEAR_DIR, ELOG_ACCUMULATOR_FNAME_HD,
				s_elogBt.tmYear);

				memset(fileCmd, 0x0, sizeof(fileCmd));
				snprintf(fileCmd, sizeof(fileCmd)-1, "sed -i '$d' %s",
						filePathYear);
				CMN_MutexLock(&s_elogSysCmdMutx);
				ELOG_CALL_SYSTEM(fileCmd);
				CMN_MutexUnlock(&s_elogSysCmdMutx);

				fp = ELOG_FOPEN(filePathYear, "a");
				if(fp == NULL)
				{
					DLOG_Error("(%s) error.(%d)\n", filePathYear, errno);
					ret = ELOG_E_SYSCAL;
				}
				else
				{
					fprintf(fp, "%02d %.1f %.1f\n",
					etm.tmMon,s_elogBt.MonthSumallPwrOut,
					s_elogBt.MonthSumallPwrIn);
					ELOG_FCLOSE(fp);
				}
			}
		}
		else
		{
			ret = CheckFileNumAndCreateFile(ELOG_FILE_NUM_ACCUMULATOR_MONTH,
											filePathMonth, ELOG_DATESTR_YM);
			if(ret != ELOG_OK)
			{
				DLOG_Error("CheckFileNumAndCreateFile() error.\n");
				ret = ELOG_E_PROCS;
			}
			else
			{
				ret = CheckFileNumAndCreateFile(ELOG_FILE_NUM_ACCUMULATOR_YEAR,
												filePathYear, ELOG_DATESTR_Y);
				if(ret != ELOG_OK)
				{
					DLOG_Error("CheckFileNumAndCreateFile() error.\n");
					ret = ELOG_E_PROCS;
				}
				else
				{
					s_elogBt.DaySumallPwrOut = allPwrOut;
					s_elogBt.MonthSumallPwrOut += s_elogBt.DaySumallPwrOut;
					s_elogBt.DaySumallPwrIn = allPwrIn;
					s_elogBt.MonthSumallPwrIn += s_elogBt.DaySumallPwrIn;

					fp = ELOG_FOPEN(filePathMonth, "a");
					if(fp == NULL)
					{
						DLOG_Error("CMN_FOPEN(%s) error.(%d)\n",
									filePathMonth, errno);
						DEBUG_TRCOUT();
						ret = ELOG_E_SYSCAL;
					}
					else
					{
						fprintf(fp, "%02d %.1f %.1f\n", etm.tmMday,
						s_elogBt.DaySumallPwrOut,
						s_elogBt.DaySumallPwrIn);
						ELOG_FCLOSE(fp);
					}
				}
			}
			if(ret == ELOG_OK)
			{
				if((etm.tmMon == s_elogBt.tmMon))
				{
					memset(fileCmd, 0x0, sizeof(fileCmd));
					snprintf(fileCmd, sizeof(fileCmd)-1, "sed -i '$d' %s",
					filePathMonth);
					CMN_MutexLock(&s_elogSysCmdMutx);
					ELOG_CALL_SYSTEM(fileCmd);
					CMN_MutexUnlock(&s_elogSysCmdMutx);

					fp = ELOG_FOPEN(filePathMonth, "a");
					if(fp == NULL)
					{
						DLOG_Error("CMN_FOPEN(%s) error.(%d)\n",
									filePathMonth, errno);
						ret = ELOG_E_SYSCAL;
					}
					else
					{
						fprintf(fp, "%02d %.1f %.1f\n", etm.tmMon,
						s_elogBt.MonthSumallPwrOut,
						s_elogBt.MonthSumallPwrIn);
						ELOG_FCLOSE(fp);
					}
				}
				else
				{
					s_elogBt.MonthSumallPwrOut = s_elogBt.DaySumallPwrOut;
					s_elogBt.MonthSumallPwrIn = s_elogBt.DaySumallPwrIn;

					fp = ELOG_FOPEN(filePathYear, "a");
					if(fp == NULL)
					{
						DLOG_Error("(%s) error.(%d)\n", filePathYear, errno);
						ret = ELOG_E_SYSCAL;
					}
					else
					{
						fprintf(fp, "%02d %.1f %.1f\n", etm.tmMon,
						s_elogBt.MonthSumallPwrOut,
						s_elogBt.MonthSumallPwrIn);
						ELOG_FCLOSE(fp);
					}
				}
			}
		}
	}

	s_elogBt.tmMday = etm.tmMday;
	s_elogBt.tmMon = etm.tmMon;
	s_elogBt.tmYear = etm.tmYear;

	return ret;
}
/*============================================================================*/
/*    Description    :车辆充放电连接锁记录                                      */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-1)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/07  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong elog_SaveVehicleConnectLock()
{
	slong ret = ELOG_OK;
	schar filePath[ELOG_FILEPATH_MAX + 1];
	EtimTm_t etm;
	FILE *fp;
	ElogCmd81_t CMD81Power;
	schar dateStr[ELOG_DATE_STR_MAX + 1];
	DEBUG_TRCIN();
	ETIM_GetTime(&etm);
	ret = ELOG_GetDStr(dateStr, sizeof(dateStr), ELOG_DATESTR_YMD, 0);
	if(ret != ELOG_OK)
	{
		DLOG_Error("ELOG_GetDStr() error.\n");
		ret = ELOG_E_PROCS;
	}
	else
	{
		memset(filePath, 0x0, sizeof(filePath));
		snprintf(filePath, sizeof(filePath)-1, "%s/%s%s",
		ELOG_VEHICLE_DAY_DIR, ELOG_VEHICLE_FNAME_HD, dateStr);

		fp = ELOG_FOPEN(filePath, "a");
		if(fp == NULL)
		{
			DLOG_Error("CMN_FOPEN(%s) error.(%d)\n", filePath, errno);
			ret = ELOG_E_SYSCAL;
		}
		else
		{
			/*EINV_getCMD81(&CMD81Power);*/
			if(etm.tmSec == 0)
			{
				fprintf(fp, "%d \n", CMD81Power.carConnectorLockState);
			}
			else
			{
				fprintf(fp, "%d ", CMD81Power.carConnectorLockState);
			}

		}
	}

	ELOG_FCLOSE(fp);
	DEBUG_TRCOUT();
	return ret;
}
/*============================================================================*/
/*    Description    :车辆充放电                                               */
/*----------------------------------------------------------------------------*/
/*    param          :ElogVehicle_t *param : ElogVehicle パラメータ            */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-1)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/30  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncSaveVehicleLog(ElogVehicle_t *param)
{
	slong ret = ELOG_OK;
	schar filePathDay[ELOG_FILEPATH_MAX + 1];
	schar filePathMonth[ELOG_FILEPATH_MAX + 1];
	schar filePathYear[ELOG_FILEPATH_MAX + 1];
	schar fileCmd[ELOG_FILEPATH_MAX*2 + 1];
	EtimTm_t etm;
	FILE *fp;
	ElogCmd81_t CMD81Power;

	double allPwrOut;
	double allPwrIn;
	double capacity3;

	DEBUG_TRCIN();

	/* NAND容量回復スレッド状態確認、中断 */
	ret = ELOG_AbortAndWaitNandDelete(FALSE);
	if(ret != ELOG_OK)
	{
		DLOG_Error("ELOG_AbortAndWaitNandDelete() error.\n");
		ret = ELOG_E_PROCS;
	}
	else
	{
		/* 現ログファイル確認＋最古ファイル削除 */
		ret = CheckFileNumAndCreateFile(ELOG_FILE_NUM_VEHICLE_DAY,
										filePathDay, ELOG_DATESTR_YMD);
		if(ret != ELOG_OK)
		{
			DLOG_Error("CheckFileNumAndCreateFile() error.\n");
			ret = ELOG_E_PROCS;
		}
		else
		{
			/*EINV_getCMD81(&CMD81Power);*/

			allPwrOut = CMD81Power.carTtlDischargePower - 
						s_elogV2h.LastDayallPwrOut;
			allPwrIn = CMD81Power.carTtlChargePower - 
						s_elogV2h.LastDayallPwrIn;
			capacity3 = CMD81Power.carRemainingCapacity3;

			s_elogV2h.LastDayallPwrOut = allPwrOut;
			s_elogV2h.LastDayallPwrIn = allPwrIn;

			ret = ETIM_GetTime(&etm);
			if(ret != ELOG_OK)
			{
				DLOG_Error("ETIM_GetTime() error.\n");
				ret = ELOG_E_SYSCAL;
			}
			else
			{
				if((s_elogV2h.tmMday == 0) ||
				(s_elogV2h.tmMon == 0) ||
				(s_elogV2h.tmYear == 0))
				{
					s_elogV2h.tmMday = etm.tmMday;
					s_elogV2h.tmMon = etm.tmMon;
					s_elogV2h.tmYear = etm.tmYear;
				}
				else
				{
					/* TODO */
				}
			}
		}
	}
	if(ret == ELOG_OK)
	{
		/* 日的文件作成 */
		memset(filePathDay, 0x0, sizeof(filePathDay));

		snprintf(filePathDay, sizeof(filePathDay)-1, "%s/%s%04d%02d%02d",
		ELOG_VEHICLE_DAY_DIR, ELOG_VEHICLE_FNAME_HD,
		s_elogV2h.tmYear, s_elogV2h.tmMon, s_elogV2h.tmMday);

		fp = ELOG_FOPEN(filePathDay, "a");
		if(fp == NULL)
		{
			DLOG_Error("CMN_FOPEN(%s) error.(%d)\n", filePathDay, errno);
			ret = ELOG_E_SYSCAL;
		}
		else
		{
			fprintf(fp, "%02d:00 %.1f %.1f %.1f ",
			etm.tmHour, allPwrOut, allPwrIn, capacity3);
			ELOG_FCLOSE(fp);
		}
	}
	else
	{
		/*Do nothing*/
	}

	if(ret == ELOG_OK)
	{
		/*日的累计*/
		if((etm.tmMday == s_elogV2h.tmMday) || (etm.tmHour == 0))
		{
			s_elogV2h.DaySumallPwrOut += allPwrOut;
			s_elogV2h.MonthSumallPwrOut += s_elogV2h.DaySumallPwrOut;
			s_elogV2h.DaySumallPwrIn += allPwrIn;
			s_elogV2h.MonthSumallPwrIn += s_elogV2h.DaySumallPwrIn;

			/* 月的文件作成 */
			memset(filePathMonth, 0x0, sizeof(filePathMonth));
			snprintf(filePathMonth, sizeof(filePathMonth)-1, "%s/%s%04d%02d",
			ELOG_VEHICLE_MONTH_DIR, ELOG_VEHICLE_FNAME_HD,
			s_elogV2h.tmYear, s_elogV2h.tmMon);

			memset(fileCmd, 0x0, sizeof(fileCmd));
			snprintf(fileCmd, sizeof(fileCmd)-1, "sed -i '$d' %s",
					filePathMonth);
			CMN_MutexLock(&s_elogSysCmdMutx);
			ELOG_CALL_SYSTEM(fileCmd);
			CMN_MutexUnlock(&s_elogSysCmdMutx);

			fp = ELOG_FOPEN(filePathMonth, "a");
			if(fp == NULL)
			{
				DLOG_Error("CMN_FOPEN(%s) error.(%d)\n", filePathMonth, errno);
				ret = ELOG_E_SYSCAL;
			}
			else
			{
				fprintf(fp, "%02d %.1f %.1f\n", etm.tmMday,
				s_elogV2h.DaySumallPwrOut, s_elogV2h.DaySumallPwrIn);
				ELOG_FCLOSE(fp);

				/* 年的文件作成 */
				memset(filePathYear, 0x0, sizeof(filePathYear));
				snprintf(filePathYear, sizeof(filePathYear)-1, "%s/%s%04d",
				ELOG_VEHICLE_YEAR_DIR, ELOG_VEHICLE_FNAME_HD,
				s_elogV2h.tmYear);

				memset(fileCmd, 0x0, sizeof(fileCmd));
				snprintf(fileCmd, sizeof(fileCmd)-1, "sed -i '$d' %s",
				filePathYear);
				CMN_MutexLock(&s_elogSysCmdMutx);
				ELOG_CALL_SYSTEM(fileCmd);
				CMN_MutexUnlock(&s_elogSysCmdMutx);

				fp = ELOG_FOPEN(filePathYear, "a");
				if(fp == NULL)
				{
					DLOG_Error("(%s) error.(%d)\n", filePathYear, errno);
					ret = ELOG_E_SYSCAL;
				}
				else
				{
					fprintf(fp, "%02d %.1f %.1f\n", etm.tmMon,
					s_elogV2h.MonthSumallPwrOut,s_elogV2h.MonthSumallPwrIn);
					ELOG_FCLOSE(fp);
				}
			}
		}
		else
		{
			ret = CheckFileNumAndCreateFile(ELOG_FILE_NUM_VEHICLE_MONTH,
			filePathMonth, ELOG_DATESTR_YM);
			if(ret != ELOG_OK)
			{
				DLOG_Error("CheckFileNumAndCreateFile() error.\n");
				ret = ELOG_E_PROCS;
			}
			else
			{
				ret = CheckFileNumAndCreateFile(ELOG_FILE_NUM_VEHICLE_YEAR,
				filePathYear, ELOG_DATESTR_Y);
				if(ret != ELOG_OK)
				{
					DLOG_Error("CheckFileNumAndCreateFile() error.\n");
					ret = ELOG_E_PROCS;
				}
				else
				{
					s_elogV2h.DaySumallPwrOut = allPwrOut;
					s_elogV2h.MonthSumallPwrOut += s_elogV2h.DaySumallPwrOut;
					s_elogV2h.DaySumallPwrIn = allPwrIn;
					s_elogV2h.MonthSumallPwrIn += s_elogV2h.DaySumallPwrIn;
				}
			}
			if(ret == ELOG_OK)
			{
				fp = ELOG_FOPEN(filePathMonth, "a");
				if(fp == NULL)
				{
					DLOG_Error("(%s) error.(%d)\n", filePathMonth, errno);
					ret = ELOG_E_SYSCAL;
				}
				else
				{
					fprintf(fp, "%02d %.1f %.1f\n", etm.tmMday,
					s_elogV2h.DaySumallPwrOut,
					s_elogV2h.DaySumallPwrIn);
					ELOG_FCLOSE(fp);
				}
			}
			else
			{
				/*Do nothing*/
			}
			if(ret == ELOG_OK)
			{
				if((etm.tmMon == s_elogV2h.tmMon))
				{
					memset(fileCmd, 0x0, sizeof(fileCmd));
					snprintf(fileCmd, sizeof(fileCmd)-1, "sed -i '$d' %s",
					filePathYear);
					CMN_MutexLock(&s_elogSysCmdMutx);
					ELOG_CALL_SYSTEM(fileCmd);
					CMN_MutexUnlock(&s_elogSysCmdMutx);

					fp = ELOG_FOPEN(filePathYear, "a");
					if(fp == NULL)
					{
						DLOG_Error("(%s) error.(%d)\n", filePathYear, errno);
						ret = ELOG_E_SYSCAL;
					}
					else
					{
						fprintf(fp, "%02d %.1f %.1f\n", etm.tmMon,
						s_elogV2h.MonthSumallPwrOut,
						s_elogV2h.MonthSumallPwrIn);
						ELOG_FCLOSE(fp);
					}
				}
				else
				{
					s_elogV2h.MonthSumallPwrOut = s_elogV2h.DaySumallPwrOut;
					s_elogV2h.MonthSumallPwrIn = s_elogV2h.DaySumallPwrIn;

					fp = ELOG_FOPEN(filePathYear, "a");
					if(fp == NULL)
					{
						DLOG_Error("(%s) error.(%d)\n", filePathYear, errno);
						DEBUG_TRCOUT();
						ret = ELOG_E_SYSCAL;
					}
					fprintf(fp, "%02d %.1f %.1f\n", etm.tmMon,
					s_elogV2h.MonthSumallPwrOut,
					s_elogV2h.MonthSumallPwrIn);
					ELOG_FCLOSE(fp);
				}
			}
			else
			{
				/*Do nothing*/
			}
		}
	}

	s_elogV2h.tmMday = etm.tmMday;
	s_elogV2h.tmMon = etm.tmMon;
	s_elogV2h.tmYear = etm.tmYear;

	return ret;
}
/*============================================================================*/
/*    Description    :运行信息                                                 */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFncRunInfor65_t *param : ElogFncRunInfor65 パラメータ*/
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-1)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/30  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_RunInformation65(ElogFncRunInfor65_t *param)
{
	slong ret = ELOG_OK;
	schar newPath[ELOG_FILEPATH_MAX + 1];
	schar filePath[ELOG_FILEPATH_MAX + 1];
	FILE *fp;
	DEBUG_TRCIN();


	/* フルパスファイル名作成 */
	memset(newPath, 0x0, sizeof(newPath));
	snprintf(newPath, sizeof(newPath)-1, "%s/%s",
	ELOG_OPT_INFORM_DIR, ELOG_OPT_INFORM_FNAME_HD);

	/* ファイルを追記モードでオープン */
	fp = ELOG_FOPEN(newPath, "w");
	if(fp == NULL)
	{
		DLOG_Error("ELOG_FOPEN(%s) error.(%d)\n", newPath, errno);
		ret = ELOG_E_SYSCAL;
	}
	else
	{
		fprintf(fp, "%d\n", param->btLowerLimit);/*CMD65的内容*/
	}
	ELOG_FCLOSE(fp);
	DEBUG_TRCOUT();
	return ret;
}


/*============================================================================*/
/*    Description    :运行信息                                                 */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFncRunInfor75_t *param : ElogFncRunInfor75 パラメータ*/
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-1)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/30  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_RunInformation75(ElogFncRunInfor75_t *param)
{
	slong ret = ELOG_OK;
	schar newPath[ELOG_FILEPATH_MAX + 1];
	schar filePath[ELOG_FILEPATH_MAX + 1];
	FILE *fp;
	DEBUG_TRCIN();


	/* フルパスファイル名作成 */
	memset(newPath, 0x0, sizeof(newPath));
	snprintf(newPath, sizeof(newPath)-1, "%s/%s",
	ELOG_OPT_INFORM_DIR, ELOG_OPT_INFORM_FNAME_HD);

	/* ファイルを追記モードでオープン */
	fp = ELOG_FOPEN(newPath, "a");
	if(fp == NULL)
	{
		DLOG_Error("ELOG_FOPEN(%s) error.(%d)\n", newPath, errno);
		ret = ELOG_E_SYSCAL;
	}
	else
	{
		fprintf(fp, "%d\n", param->carCharordiscBodyOprat);/*CMD75的内容*/
	}
	ELOG_FCLOSE(fp);
	DEBUG_TRCOUT();
	return ret;
}
/*============================================================================*/
/*    Description    :下载log                                                 */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFncDownload_t *param : ElogFncDownload パラメータ    */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/30  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_UpdateResult(ElogFncDownload_t *param)
{
	slong ret = ELOG_OK;
	schar newPath[ELOG_FILEPATH_MAX + 1];
	schar logfile[ELOG_FILEPATH_MAX + 1];
	schar fileCmd[ELOG_FILEPATH_MAX*2 + 10];
	schar dateStr[ELOG_DATE_STR_MAX + 1];
	FILE *fp;

	DEBUG_TRCIN();

	ret = CheckFileNumAndCreateFile(ELOG_FILE_NUM_UPDATE,
	newPath, ELOG_DATESTR_LOG);
	if(ret != ELOG_OK)
	{
		DLOG_Error("CheckFileNumAndCreateFile() error.\n");
		ret = ELOG_E_PROCS;
	}
	else
	{
		memset(logfile, 0x0, sizeof(logfile));
		memset(fileCmd, 0x0, sizeof(fileCmd));

		snprintf(logfile, sizeof(logfile)-1, "%s", newPath);
		snprintf(fileCmd, sizeof(fileCmd)-1, "cp -f %s %s",
		param->string, logfile);

		CMN_MutexLock(&s_elogSysCmdMutx);
		ELOG_CALL_SYSTEM(fileCmd);
		CMN_MutexUnlock(&s_elogSysCmdMutx);

		/* 一時ファイル削除 */
		remove(param->string);
		if(ret != 0)
		{
			/* ファイルコピー失敗 */
			DLOG_Error("ELOG_FILECOPY error.(%d)\n", ret);
			ret = ELOG_E_PROCS;
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
/*    Description    :设置各个功能定时器                                        */
/*----------------------------------------------------------------------------*/
/*    param          :FrmMsgHead_t const *msg:FrmMsgHead パラメータ            */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/29  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void elog_ProcTimeout(FrmMsgHead_t const *msg)
{
	ElogCmd81_t CMD81Power;
	DEBUG_TRCIN();

	FrmTimeout_t const *timeoutMsg = (FrmTimeout_t const *)msg;

	if((timeoutMsg->timerId == ELOG_TIMER_ACTION_ACZ))
	{
		/*EINV_getCMD81(&CMD81Power);*/
		ELOG_AsyncCompressActionLog(TRUE);
		FRM_TimerSetOneShot(ELOG_TIMER_ACTION_ACZ, ELOG_ACZ_TIMER, NULL);
	}
	else if((timeoutMsg->timerId == ELOG_TIMER_SOURCE_OUT))
	{
		/*EINV_getCMD81(&CMD81Power);*/
		ELOG_PowerTotalLog(CMD81Power.ttlChargeEnergySysAC, 
							ELOG_FILE_NUM_POWER_OUT);
		FRM_TimerSetOneShot(ELOG_TIMER_SOURCE_OUT, ELOG_POWER_OUT_TIMER, NULL);
	}
	else if(timeoutMsg->timerId == ELOG_TIMER_SOURCE_IN)
	{
		/*EINV_getCMD81(&CMD81Power);*/
		ELOG_PowerTotalLog(CMD81Power.ttlDischargeEnergySysAC,
							ELOG_FILE_NUM_SOURCE_IN);
		FRM_TimerSetOneShot(ELOG_TIMER_SOURCE_IN, ELOG_POWER_OUT_TIMER, NULL);
	}
	else if(timeoutMsg->timerId == ELOG_TIMER_GENERATE_OUT)
	{
		/*EINV_getCMD81(&CMD81Power);*/
		ELOG_PowerTotalLog(CMD81Power.ttlDischargeEnergyAloneAC,
							ELOG_FILE_NUM_GENERATE_OUT);
		FRM_TimerSetOneShot(ELOG_TIMER_GENERATE_OUT,
							ELOG_POWER_OUT_TIMER, NULL);
	}
	else if(timeoutMsg->timerId == ELOG_TIMER_LOG_OUTPUTCTRL)
	{
		/*percentage值从电力会社取值*/
		ELOG_OutPutCtrlLog(80);
		FRM_TimerSetOneShot(ELOG_TIMER_LOG_OUTPUTCTRL,
							ELOG_OUTPUTCTRL_TIMER, NULL);
	}
	else if(timeoutMsg->timerId == ELOG_TIMER_PV)
	{
		ELOG_PowerDealLog();
		FRM_TimerSetOneShot(ELOG_TIMER_PV, ELOG_POWER_TIMER, NULL);
	}
	else if(timeoutMsg->timerId == ELOG_TIMER_BT)
	{
		ELOG_AccumulatorLog();
		FRM_TimerSetOneShot(ELOG_TIMER_BT, ELOG_POWER_TIMER, NULL);
	}
	else if(timeoutMsg->timerId == ELOG_TIMER_V2H)
	{
		ELOG_VehicleLog();
		FRM_TimerSetOneShot(ELOG_TIMER_V2H, ELOG_POWER_TIMER, NULL);
	}
	else if(timeoutMsg->timerId == ELOG_TIMER_V2H_CNT_LUCK)
	{
		elog_SaveVehicleConnectLock();
		FRM_TimerSetOneShot(ELOG_TIMER_V2H_CNT_LUCK, ELOG_CNT_LOCK_TIM, NULL);
	}
	else if(timeoutMsg->timerId == ELOG_TIMER_SDP_SDZ)
	{
		ELOG_AsyncCompressSdpLog(TRUE);
		FRM_TimerSetOneShot(ELOG_TIMER_SDP_SDZ, ELOG_SDP_TIMER, NULL);
	}
	else if(timeoutMsg->timerId == ELOG_TIMER_LOG)
	{
		elog_SetLogFileName();
		FRM_TimerSetOneShot(ELOG_TIMER_LOG, ELOG_LOG_TIMER, NULL);
	}
	DEBUG_TRCOUT();
}
/*============================================================================*/
/*    Description    :                                                        */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/true:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/20  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void elog_ChangeTmpFileName(void)
{
	slong ret = ELOG_OK;
	char dateStr[ELOG_DATE_STR_MAX + 1];
	char tempPath[ELOG_FILEPATH_MAX + 1];
	schar fileCmd[ELOG_FILEPATH_MAX*2 + 10];
	struct stat statBuf;

	memset(dateStr, 0x0, sizeof(dateStr));
	ret = ELOG_GetDStr(dateStr, sizeof(dateStr), ELOG_DATESTR_DCS, 0);
	if(ret != ELOG_OK)
	{
		DLOG_Error("ELOG_GetDStr() error.\n");
		ret = ELOG_E_PROCS;
	}
	else
	{
		/*Do nothing*/
	}

	memset(fileCmd, 0x0, sizeof(fileCmd));
	snprintf(fileCmd, sizeof(fileCmd)-1, "mv %s/* %s",
	ELOG_TMP_SDP_DIR, ELOG_E_SDP_LOG_DIR);

	memset(tempPath, 0x0, sizeof(tempPath));
	snprintf(tempPath, sizeof(tempPath)-1, "%s/%s",
	ELOG_TMP_SDP_DIR, s_elogSdpFileName);

	ret = stat(tempPath, &statBuf);
	if(ret == 0)
	{
		CMN_MutexLock(&s_elogSysCmdMutx);
		ELOG_CALL_SYSTEM(fileCmd);
		CMN_MutexUnlock(&s_elogSysCmdMutx);
	}
	else
	{
		DLOG_Info("There are no files in the current path.\n");
	}
		/*改变中间文件名*/
	memset(s_elogSdpFileName, 0, ELOG_FILE_NAME_MAX);
	strncpy(s_elogSdpFileName, dateStr, sizeof(dateStr) - 1);
}
/*============================================================================*/
/*    Description    :SDPログ保存実行処理                                      */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFncSdp_t param : ElogFncSdp パラメータ               */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   ELOG_OK(0)    正常                                       */
/*----------------------------------------------------------------------------*/
/*                   ELOG_E_PARAM(-1)    パラメータエラー                      */
/*----------------------------------------------------------------------------*/
/*                   ELOG_E_PROCS(-3)    処理エラー                            */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/28  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncSaveSdpLog(ElogFncSdp_t *param)
{
	FILE *fp;
	FILE *fpTmpFile;
	slong ret = ELOG_OK;
	char newPath[ELOG_FILEPATH_MAX + 1];
	char dateStr[ELOG_DATE_STR_MAX + 1];
	char fName[ELOG_FILEPATH_MAX + 1];
	uchar series[ELOG_SERIES_LEN];
	bool_t createFile;
	DEBUG_TRCIN();

	/* NAND容量回復スレッド状態確認、中断 */
	ret = ELOG_AbortAndWaitNandDelete(FALSE);
	if(ret != ELOG_OK)
	{
		DLOG_Error("ELOG_AbortAndWaitNandDelete() error.\n");
		ret = ELOG_E_PROCS;
	}
	else
	{
		memset(series, 0x0, sizeof(series));
		EEEP_GetBoardSeries(series);

		memset(dateStr, 0x0, sizeof(dateStr));
		ret = ELOG_GetDStr(dateStr, sizeof(dateStr), ELOG_DATESTR_YMD , 0);
		if(ret != ELOG_OK)
		{
			DLOG_Error("ELOG_GetDStr() error.\n");
			ret = ELOG_E_PROCS;
		}
		else
		{
			/*Do nothing*/
		}
	}
	if(ret == ELOG_OK)
	{
		/* ファイルパス生成 */
		memset(newPath, 0x0, sizeof(newPath));
		snprintf(newPath, sizeof(newPath)-1, "%s/%s-%s-%s.txt",
		ELOG_SDP_DIR, ELOG_SDP_FNAME_HD, series, dateStr);


		fp = CMN_FOPEN(newPath, "a");
		if(fp == NULL)
		{
			DLOG_Error("CMN_FOPEN(%s) error.(%d)\n", newPath, errno);
			ret = ELOG_E_SYSCAL;
		}
		else
		{
			fprintf(fp, "%s\n", param->string);
			CMN_FCLOSE(fp);

			memset(newPath, 0x0, sizeof(newPath));
			snprintf(newPath, sizeof(newPath) - 1, "%s/%s",
			ELOG_TMP_SDP_DIR, s_elogSdpFileName);
			fpTmpFile = CMN_FOPEN(newPath, "a");
			if(fpTmpFile == NULL)
			{
				DLOG_Error("CMN_FOPEN(%s) error.(%d)\n", newPath, errno);
				ret = ELOG_E_SYSCAL;
			}
			else
			{
				fprintf(fpTmpFile, "%s\n", param->string);
				CMN_FCLOSE(fpTmpFile);
				/* NAND容量回復スレッド起動 */
				ret = elog_CheckAndDeleteNandLog();
				if(ret != ELOG_OK && ret != ELOG_E_BUSY)
				{
					DLOG_Error("elog_CheckAndDeleteNandLog() error.\n");
					ret = ELOG_E_PROCS;
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

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :sdp日志压缩                                              */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFncSdpComp_t param : ElogFncSdpComp パラメータ       */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_BUSY(-6)    実行中エラー                          */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncCompressSdpLog(ElogFncSdpComp_t *param)
{
	slong ret = ELOG_OK;
	bool_t flag = TRUE;
	ElogTgzSave_t *pData;
	uchar series[ELOG_SERIES_LEN];
	char dateStr[ELOG_DATE_STR_MAX + 1];

	DEBUG_TRCIN();

	/* サブスレッド実行状態確認 */
	ELOG_CheckSubThreadRun(ELOG_THTYPE_USB | ELOG_THTYPE_CPRS);
	if(ret != ELOG_OK)
	{
		/* 処理不可イベント */
		DLOG_Error("elog_checkSubThreadRun() = %d.\n", ret);
		if(ret == ELOG_E_BUSY)
		{
			ELOG_SendCompressEvent(param->header.destModId, 
			ELOG_COMPRESS_BUSY, ELOG_TYPE_SDZ, NULL, FALSE);
		}
		else
		{
			ELOG_SendCompressEvent(param->header.destModId,
			ELOG_COMPRESS_OTHERR, ELOG_TYPE_SDZ, NULL, FALSE);
		}
		flag = FALSE;
	}
	if(flag)
	{
		memset(series, 0x0, sizeof(series));
		EEEP_GetBoardSeries(series);

		memset(dateStr, 0x0, sizeof(dateStr));
		ret = ELOG_GetDStr(dateStr, sizeof(dateStr), ELOG_DATESTR_YMD, 0);
		if(ret != ELOG_OK)
		{
			DLOG_Error("ELOG_GetDStr() error.\n");
			ret = ELOG_E_PROCS;
		}
		else
		{
			/* 実行パラメータ設定 */
			CMN_MutexLock(&(s_elogThrCompress.dMutex));
			pData = (ElogTgzSave_t *)s_elogThrCompress.data;
			pData->type = ELOG_TYPE_SDZ;
			memcpy(&pData->header, &param->header, sizeof(pData->header));
			pData->del = param->del;
			CMN_MutexUnlock(&(s_elogThrCompress.dMutex));

			memset(pData->fileName, 0x0, sizeof(pData->fileName));
			/* フルパスファイル名作成 */
			snprintf(pData->fileName, sizeof(pData->fileName)-1,
					"%s/%s-%s-%s.tgz",
					ELOG_SDZ_DIR, ELOG_SDP_FNAME_HD, series, dateStr);

			/* 圧縮コマンド実行スレッドを起動 */
			ret = ELOG_ThrStart(&s_elogThrCompress);
			if(ret != ELOG_OK)
			{
				DLOG_Error("ELOG_ThrStart() error.\n");
				/* 処理不可イベント */
				ELOG_SendCompressEvent(param->header.destModId,
				ELOG_COMPRESS_OTHERR, ELOG_TYPE_SDZ, NULL, FALSE);
				ret = ELOG_E_PROCS;
			}
			else
			{
				/*Do nothing*/
			}
		}
	}


	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :ログ一覧取得処理                                         */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFncGetLogList_t param : ElogFncGetLogList パラメータ */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncGetLogList(ElogFncGetLogList_t *param)
{
	slong ret = ELOG_OK;
	DEBUG_TRCIN();

	if(param->type != ELOG_TYPE_OTH &&
		 param->type != ELOG_TYPE_ACZ &&
		 param->type != ELOG_TYPE_SDZ)
	{
		DLOG_Error("param->type is illegal(%d).\n", param->type);
		/* 処理エラーイベント */
		elog_sendGetLogListEvent(param->header.destModId, ELOG_GETLOGLIST_ERR);
		ret = ELOG_E_PARAM;
	}
	else
	{
		/* 重複実行確認 */
		ret = ELOG_CheckSubThreadRun(ELOG_THTYPE_LLST | ELOG_THTYPE_LDEL);
		if(ret != ELOG_OK)
		{
			DLOG_Error("ELOG_CheckSubThreadRun() = %d.\n", ret);
			/* 処理不可イベント */
			if(ret == ELOG_E_BUSY)
			{
				elog_sendGetLogListEvent(param->header.destModId,
				ELOG_GETLOGLIST_BUSY);
			}
			else
			{
				elog_sendGetLogListEvent(param->header.destModId,
				ELOG_GETLOGLIST_ERR);
			}
			DEBUG_TRCOUT();
		}
		else
		{
			/* 実行パラメータ設定 */
			CMN_MutexLock(&(s_elogThrGetLogList.dMutex));
			memcpy((ElogFncGetLogList_t *)s_elogThrGetLogList.data,
					param, sizeof(ElogFncGetLogList_t));
			CMN_MutexUnlock(&(s_elogThrGetLogList.dMutex));

			/* ログリスト取得スレッドを起動 */
			ret = ELOG_ThrStart(&s_elogThrGetLogList);
			if(ret != ELOG_OK)
			{
				DLOG_Error("ELOG_ThrStart() error.\n");
				/* 処理エラーイベント */
				elog_sendGetLogListEvent(param->header.destModId,
				ELOG_GETLOGLIST_ERR);
				ret = ELOG_E_PROCS;
			}
			else
			{
				/*Do nothing*/
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :ログ領域空き容量取得処理                                  */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFncFreeSpace_t param : ElogFncFreeSpace パラメータ   */
/*----------------------------------------------------------------------------*/
/*    param          :slong *freeMB : 空き容量(MB)                             */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/07  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncGetFreeLogSpace(ElogFncFreeSpace_t *param, ulong *freeMB)
{
	slong ret = ELOG_OK;
	ulong avail;
	ulong used;
	ulong all;

	DEBUG_TRCIN();
	if(freeMB == NULL)
	{
		DLOG_Error("freeMB is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		ret = ELOG_GetPartitionInfoMB(ELOG_FS_LOG, &avail, &used, &all);
		if(ret != ELOG_OK)
		{
			DLOG_Error("ELOG_GetPartitionInfoMB() error.\n");
			ret = ELOG_E_PROCS;
		}
		else
		{
			*freeMB = avail;
		}
	}

	DEBUG_TRCOUT();
	return ret;
}
/*============================================================================*/
/*    Description    :ログ削除要求処理                                         */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFncDeleteLog_t param : ElogFncDeleteLog パラメータ   */
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
/*    history        :2023/04/07  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncDeleteLog(ElogFncDeleteLog_t *param)
{
	sint ret = ELOG_OK;
	DEBUG_TRCIN();

	if(param->type != ELOG_TYPE_ACZ &&
		 param->type != ELOG_TYPE_SDZ)
	{
		DLOG_Error("param->type is illegal(%d).\n", param->type);
		/* 処理エラーイベント */
		elog_sendDelLogEvent(param->header.destModId, ELOG_DELLOG_ERR);
		ret = ELOG_E_PARAM;
	}
	else
	{
		/* 重複実行確認 */
		ret = ELOG_CheckSubThreadRun(ELOG_THTYPE_USB | 
									ELOG_THTYPE_LLST | ELOG_THTYPE_LDEL);
		if(ret != ELOG_OK)
		{
			DLOG_Error("ELOG_CheckSubThreadRun() = %d.\n", ret);
			/* 処理不可イベント */
			if(ret == ELOG_E_BUSY)
			{
				elog_sendDelLogEvent(param->header.destModId, ELOG_DELOG_BUSY);
			}
			else
			{
				elog_sendDelLogEvent(param->header.destModId, ELOG_DELLOG_ERR);
			}
		}
		else
		{
			/* 実行パラメータ設定 */
			CMN_MutexLock(&(s_elogThrDeleteLog.dMutex));
			memcpy((ElogFncDeleteLog_t *)s_elogThrDeleteLog.data,
					param, sizeof(ElogFncDeleteLog_t));
			CMN_MutexUnlock(&(s_elogThrDeleteLog.dMutex));

			/* ログリスト取得スレッドを起動 */
			ret = ELOG_ThrStart(&s_elogThrDeleteLog);
			if(ret != ELOG_OK)
			{
				DLOG_Error("ELOG_ThrStart() error.\n");
				/* 処理エラーイベント */
				elog_sendDelLogEvent(param->header.destModId, ELOG_DELLOG_ERR);
				ret = ELOG_E_PROCS;
			}
		}
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :指定ファイル削除要求処理                                  */
/*----------------------------------------------------------------------------*/
/*    param          :ElogFncDeleteFile_t param : ElogFncDeleteFile_t         */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-2)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_NOENT(-4)    指定ファイル無しエラー                */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/21  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_FncDeleteFile(ElogFncDeleteFile_t *param)
{
	slong tmpRet = ELOG_OK;
	schar FilePath[ELOG_FILEPATH_MAX + 1];
	schar fileCmd[ELOG_FILEPATH_MAX + 1];

	memset(FilePath, 0x0, sizeof(FilePath));
	snprintf(FilePath, sizeof(FilePath) - 1, "%s/%s",
			s_elogDeleteFile[param->type].path,
			s_elogDeleteFile[param->type].fileHead);

	memset(fileCmd, 0x0, sizeof(fileCmd));
	snprintf(fileCmd, sizeof(fileCmd) - 1, "sed -i '1,%dd' %s",
			param->line, FilePath);
	CMN_MutexLock(&s_elogSysCmdMutx);
	ELOG_CALL_SYSTEM(fileCmd);
	CMN_MutexUnlock(&s_elogSysCmdMutx);
}
/*============================================================================*/
/*    Description    :全プロセスログOnOff状態設定処理                           */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常                                      */
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FncSetEnableAll(bool_t onOff)
{
	slong ret;
	DEBUG_TRCIN();

	ret = ELOG_CnfSetEnableAll(onOff);

	DEBUG_TRCOUT();
	return(ret);
}
/*============================================================================*/
/*    Description    :ログOnOff状態取得処理                                    */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    TRUE : ON /FALSE : OFF                                  */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
bool_t ELOG_FncGetEnableAll(void)
{
	bool_t ret;
	DEBUG_TRCIN();
	ret = ELOG_ValGetEnableAll();
	DEBUG_TRCOUT();
	return(ret);
}

/*============================================================================*/
/*    Description    :ファイル名が最も新しいものを指定ディレクトリ内で検索する     */
/*----------------------------------------------------------------------------*/
/*    param          :schar *path : 検索ディレクトリパス名                      */
/*----------------------------------------------------------------------------*/
/*    param          :ElogType_t  type : ログファイル種別                      */
/*----------------------------------------------------------------------------*/
/*    param          :schar *resfile : 最新ファイル名                          */
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
/*    history        :2023/04/07  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong elog_SearchNewFile(schar *path, ElogFileSizeType_t type, schar *resfile)
{
	struct dirent **nameList = NULL;
	sint n = 0;
	sint r = 0;
	slong ret = ELOG_OK;
	bool_t flag = TRUE;
	DEBUG_TRCIN();
	if(path == NULL)
	{
		DLOG_Error("path is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	if(resfile == NULL)
	{
		DLOG_Error("resfile is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}

	/* 古いものから[0]-[n]の順に並ぶ */
	if(type == ELOG_FILE_SIZE_OPERATION)
	{
		n = scandir(path, &nameList, elog_DirentFilterOperatFiles, alphasort);
	}
	else if(type == ELOG_FILE_SIZE_INFOR)
	{
		n = scandir(path, &nameList, elog_DirentFilterPcsInfoFiles, alphasort);
	}
	else if(type == ELOG_FILE_SIZE_SERVICE)
	{
		n = scandir(path, &nameList, elog_DirentFilterPcsServFiles, alphasort);
	}
	else if(type == ELOG_FILE_SIZE_SOH)
	{
		n = scandir(path, &nameList, elog_DirentFilterSohFiles, alphasort);
	}
	else if(type == ELOG_FILE_SIZE_POWER_VOLT_HIST)
	{
		n = scandir(path, &nameList, elog_DirentFilterVolHistFiles, alphasort);
	}
	else if(type == ELOG_FILE_SIZE_HEMS_HISTORY)
	{
		n = scandir(path, &nameList, elog_DirentFilterHEMSFiles, alphasort);
	}
	else
	{
		DLOG_Error("type is illegal.(%d)\n", type);
		ret = ELOG_E_PARAM;
		flag = FALSE;
	}
	if(flag)
	{
		if(n < 0)
		{
			DLOG_Error("scandir() error.(%d,%d)\n", n, errno);
			ret = ELOG_E_SYSCAL;
		}
		else if(n == 0)
		{
			DLOG_Error("no file there.\n");
			ELOG_DirentFree(&nameList, n);
			ret = ELOG_E_NOENT;
		}
		else
		{
		r = strnlen(nameList[n-1]->d_name, sizeof(nameList[n-1]->d_name));
		strncpy(resfile, nameList[n-1]->d_name, r);
		resfile[r] = '\0';

		/* ソート結果を開放する */
		ELOG_DirentFree(&nameList, n);
		}

	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :NAND空き容量を確認、最古ファイル削除                       */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常終了                                   */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-2)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_CheckAndDeleteNandLog(void)
{
	slong ret = ELOG_OK;
	ulong avail;
	ulong used;
	ulong all;
	double fAvail;
	double fAll;
	ElogThstate_t state;

	DEBUG_TRCIN();
	/* USB保存・圧縮サブスレッドとの同時実行禁止 */
	ret = ELOG_CheckSubThreadRun(ELOG_THTYPE_USB | ELOG_THTYPE_CPRS);
	if(ret == ELOG_E_BUSY)
	{
		DLOG_Error("USB or Compress thread busy, then skip.\n");
		ret = ELOG_OK;
	}
	else if(ret == ELOG_E_PROCS)
	{
		DLOG_Error("ELOG_CheckSubThreadRun() error.\n");
		ret = ELOG_E_PROCS;
	}
	else
	{
		/* スレッド状態取得・多重起動防止 */
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
				DLOG_Error("NAND delete thread busy.\n");
				ret = ELOG_E_BUSY;
			}
			else
			{
				/*Do nothing*/
			}
		}
		if(ret == ELOG_OK)
		{
			/* NAND空き容量確認 */
			/* LOGパーティション空きサイズ取得 */
			ret = ELOG_GetPartitionInfoMB(ELOG_FS_LOG, &avail, &used, &all);
			if(ret != ELOG_OK)
			{
				DLOG_Error("elog_getPartitionAvailMB(%s) err.\n", ELOG_FS_LOG);
				ret = ELOG_E_SYSCAL;
			}
			else
			{
				fAvail = (double)avail;	/* 空き容量 */
				fAll   = (double)all;		/* 全容量 */

				/* 空き容量OKの場合起動しない */
				/* 但し、前回中断している場合は起動する */
				if(state != ELOG_THSTATE_ABORTED &&
					(fAvail >= fAll * ELOG_NAND_UNDR))
				{
					/* 起動不要 */
					ret = ELOG_OK;
				}
				else
				{

					/* 中断フラグ初期化 */
					ELOG_SetNandDeleteAbort(FALSE);

					/* NAND容量回復スレッドを起動 */
					ret = ELOG_ThrStart(&s_elogThrNandDel);
					if(ret != ELOG_OK)
					{
						DLOG_Error("ELOG_ThrStart() error.\n");
						ret = ELOG_E_PROCS;
					}
				}
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :dirent構造体リストの開放                                  */
/*----------------------------------------------------------------------------*/
/*    param          :dirent **list : 開放するdirent構造体配列                  */
/*----------------------------------------------------------------------------*/
/*    param          :ulong size : 配列の個数                                  */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/07  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_DirentFree(struct dirent ***list, ulong size)
{
	ulong n = size;

	DEBUG_TRCIN();
	if(list == NULL)
	{
		DLOG_Error("list is NULL.\n");
		DEBUG_TRCOUT();
		return;
	}
	if(*list == NULL)
	{
		DLOG_Error("*list is NULL.\n");
		DEBUG_TRCOUT();
		return;
	}

	while (n > 0)
	{
		n--;
		if((*list)[n] == NULL)
		{
			/*Do nothing*/
		}
		else
		{
			ELOG_FREE((*list)[n]);
		}
	}
	ELOG_FREE(*list);
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :dirent構造体リストへの追加開放                            */
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent ***list : 追加先dirent構造体配列へのポインタ */
/*----------------------------------------------------------------------------*/
/*                   :ulong    size : 追加先配列の個数                         */
/*----------------------------------------------------------------------------*/
/*                   :struct dirent **addList : 追加元dirent構造体配列         */
/*----------------------------------------------------------------------------*/
/*                   :ulong    addSize : 追加元配列の個数                      */
/*----------------------------------------------------------------------------*/
/*                   :schar    *path : 追加元の検索ディレクトリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/03  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirentAddCopy(
	struct dirent ***list, ulong size,
	struct dirent **addList, ulong addSize, schar *path)
{
	ulong i, j;
	slong ret = ELOG_OK;
	slong retSizeSum;
	struct dirent **tempList;
	DEBUG_TRCIN();

	if(list == NULL)
	{
		DLOG_Error("list is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		if(addList == NULL)
		{
			DLOG_Error("addList is NULL.\n");
			DEBUG_TRCOUT();
			return ELOG_E_PARAM;
		}
		else
		{
			if(path == NULL)
			{
				DLOG_Error("path is NULL.\n");
				DEBUG_TRCOUT();
				return ELOG_E_PARAM;
			}
			else
			{
				/*Do nothing*/
			}
		}
	}

	/* alloc new list */
	tempList = ELOG_MALLOC(sizeof(struct dirent *) * (size + addSize));
	if(tempList == NULL)
	{
		DLOG_Error("ELOG_MALLOC(newlist array) error.\n");
		ret = ELOG_E_SYSCAL;
	}
	else
	{
		for(i = 0; i < size; i++) {
			/* alloc new area */
			tempList[i] = ELOG_MALLOC(sizeof(struct dirent));
			if(tempList[i] == NULL)
			{
				DLOG_Error("ELOG_MALLOC(org copy area) error.\n");
				ELOG_DirentFree(&tempList, i);
				ret = ELOG_E_SYSCAL;
			}
			else
			{
				memcpy(tempList[i], (*list)[i], sizeof(struct dirent));

				for(j = 0; j < addSize; j++, i++)
				{
					/* alloc new area */
					tempList[i] = ELOG_MALLOC(sizeof(struct dirent));
					if(tempList[i] == NULL)
					{
						DLOG_Error("ELOG_MALLOC(new add area) error.\n");
						ELOG_DirentFree(&tempList, i);
						ret = ELOG_E_SYSCAL;
					}
					else
					{
						memcpy(tempList[i], addList[j], sizeof(struct dirent));
						snprintf(tempList[i]->d_name,
								sizeof(tempList[i]->d_name),
								"%s/%s", path, addList[j]->d_name);
						/* free old list & area */
						ELOG_DirentFree(list, size);
						/* assigne new list */
						*list = tempList;
						/* return total size */
						retSizeSum = size + addSize;
					}
				}
			}
		}
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :scandir()でディレクトリを対象としないためのフィルタ         */
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    0 : ディレクトリ /1 : ディレクトリ以外（ファイル）          */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirentFilterFiles(const struct dirent *ent)
{
	slong ret;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DEBUG_TRCOUT();
		return 0;
	}
	else
	{
		if(ent->d_type == DT_DIR)
		{
			ret = 0;
		}
		else
		{
			ret = 1;
		}
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :scandir()でディレクトリのみを対象とするためのフィルタ       */
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    0 : 対象外/ 1 : ディレクトリ（"." , ".."を除く）          */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirentFilterDirs(const struct dirent *ent)
{
	slong ret;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DEBUG_TRCOUT();
		return 0;
	}
	else
	{
		/* ディレクトリのみ対象 */
		if(ent->d_type != DT_DIR)
		{
			ret = 0;
		}
		else
		{
			/* "." と ".." は対象外 */
			if(strcmp(ent->d_name, ".") == 0 ||
				strcmp(ent->d_name, "..") == 0)
			{
				ret = 0;
			}
			else
			{
				ret = 1;
			}
		}
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :scandir()でプロセスログファイルのみを対象とするためのフィルタ*/
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    0: 対象外 / 1: プロセスログ（"volta" で始まる）            */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/17  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirFilterLog(const struct dirent *ent)
{
	sint r;
	slong ret = 1;
	if(ent == NULL)
	{
		return 0;
	}
	else
	{
		if(ent->d_type == DT_DIR)
		{
			return 0;
		}
		else
		{
			/*Do nothing*/
		}
	}

	r = strncmp(ent->d_name, ELOG_LOG_FNAME_HD, strlen(ELOG_LOG_FNAME_HD));
	if(r != 0)
	{
		ret = 0;
	}
	else
	{
		/*Do nothing*/
	}
	return ret;
}
/*============================================================================*/
/*    Description    :scandir()でキー通知ログファイルのみを対象とするためのフィルタ*/
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :0    対象外                                             */
/*----------------------------------------------------------------------------*/
/*                   :1    プロセスログ（"volta" で始まる）                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirentFilterOperatFiles(const struct dirent *ent)
{
	sint r;
	slong ret;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DEBUG_TRCOUT();
		return 0;
	}
	else
	{
		if(ent->d_type == DT_DIR)
		{
			ret = 0;
		}
		else
		{
			r = strncmp(ent->d_name, ELOG_OPERATION_FNAME_HD,
						strlen(ELOG_OPERATION_FNAME_HD));
			if(r != 0)
			{
				ret = 0;
			}
			else
			{
				ret = 1;
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}
/*============================================================================*/
/*    Description    :scandir()でキー通知ログファイルのみを対象とするためのフィルタ*/
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :0    対象外                                             */
/*----------------------------------------------------------------------------*/
/*                   :1    プロセスログ（"volta" で始まる）                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirentFilterPcsServFiles(const struct dirent *ent)
{
	sint r;
	slong ret;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DEBUG_TRCOUT();
		return 0;
	}
	else
	{
		if(ent->d_type == DT_DIR)
		{
			ret = 0;
		}
		else
		{
			r = strncmp(ent->d_name, ELOG_SERVICE_FNAME_HD,
						strlen(ELOG_SERVICE_FNAME_HD));
			if(r != 0)
			{
				ret = 0;
			}
			else
			{
				ret = 1;
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*============================================================================*/
/*    Description    :scandir()でキー通知ログファイルのみを対象とするためのフィルタ*/
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :0    対象外                                             */
/*----------------------------------------------------------------------------*/
/*                   :1    プロセスログ（"volta" で始まる）                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirentFilterSohFiles(const struct dirent *ent)
{
	sint r;
	slong ret;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DEBUG_TRCOUT();
		return 0;
	}
	else
	{
		if(ent->d_type == DT_DIR)
		{
			ret = 0;
		}
		else
		{
			r = strncmp(ent->d_name, ELOG_SOH_FNAME_HD,
						strlen(ELOG_SOH_FNAME_HD));
			if(r != 0)
			{
				ret = 0;
			}
			else
			{
				ret = 1;
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}
/*    Description    :scandir()でキー通知ログファイルのみを対象とするためのフィルタ*/
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :0    対象外                                             */
/*----------------------------------------------------------------------------*/
/*                   :1    プロセスログ（"volta" で始まる）                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirentFilterPcsInfoFiles(const struct dirent *ent)
{
	sint r;
	slong ret;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DEBUG_TRCOUT();
		return 0;
	}
	else
	{
		if(ent->d_type == DT_DIR)
		{
			ret = 0;
		}
		else
		{
			r = strncmp(ent->d_name, ELOG_INFOR_FNAME_HD,
						strlen(ELOG_INFOR_FNAME_HD));
			if(r != 0)
			{
				ret = 0;
			}
			else
			{
				ret = 1;
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}
/*============================================================================*/
/*    Description    :scandir()でキー通知ログファイルのみを対象とするためのフィルタ*/
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :0    対象外                                             */
/*----------------------------------------------------------------------------*/
/*                   :1    プロセスログ（"volta" で始まる）                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirentFilterVolHistFiles(const struct dirent *ent)
{
	sint r;
	slong ret;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DEBUG_TRCOUT();
		return 0;
	}
	else
	{
		if(ent->d_type == DT_DIR)
		{
			ret = 0;
		}
		else
		{
			r = strncmp(ent->d_name, ELOG_POW_RIS_EDGE_FNAME_HD,
						strlen(ELOG_POW_RIS_EDGE_FNAME_HD));
			if(r != 0)
			{
				ret = 0;
			}
			else
			{
				ret = 1;
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :scandir()でキー通知ログファイルのみを対象とするためのフィルタ*/
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :0    対象外                                             */
/*----------------------------------------------------------------------------*/
/*                   :1    プロセスログ（"volta" で始まる）                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirentFilterHEMSFiles(const struct dirent *ent)
{
	sint r;
	slong ret;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DEBUG_TRCOUT();
		return 0;
	}
	else
	{
		if(ent->d_type == DT_DIR)
		{
			ret = 0;
		}
		else
		{
			r = strncmp(ent->d_name, ELOG_HEMS_HISTORY_FNAME_HD,
						strlen(ELOG_HEMS_HISTORY_FNAME_HD));
			if(r != 0)
			{
				ret = 0;
			}
			else
			{
				ret = 1;
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :scandir()でキー通知ログファイルのみを対象とするためのフィルタ*/
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :0    対象外                                             */
/*----------------------------------------------------------------------------*/
/*                   :1    プロセスログ（"volta" で始まる）                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirFilterOutputCtl(const struct dirent *ent)
{
	sint r;
	slong ret;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DEBUG_TRCOUT();
		return 0;
	}
	else
	{
		if(ent->d_type == DT_DIR)
		{
			ret = 0;
		}
		else
		{
			r = strncmp(ent->d_name, ELOG_OUTPUTCTRL_FNAME_HD,
						strlen(ELOG_OUTPUTCTRL_FNAME_HD));
			if(r != 0)
			{
				ret = 0;
			}
			else
			{
				ret = 1;
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :scandir()でキー通知ログファイルのみを対象とするためのフィルタ*/
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :0    対象外                                             */
/*----------------------------------------------------------------------------*/
/*                   :1    プロセスログ（"volta" で始まる）                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirFilterSrcOut(const struct dirent *ent)
{
	sint r;
	slong ret;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DEBUG_TRCOUT();
		return 0;
	}
	else
	{
		if(ent->d_type == DT_DIR)
		{
			ret = 0;
		}
		else
		{
			r = strncmp(ent->d_name, ELOG_POWER_SOURCE_OUT_FNAME_HD,
						strlen(ELOG_POWER_SOURCE_OUT_FNAME_HD));
			if(r != 0)
			{
				ret = 0;
			}
			else
			{
				ret = 1;
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :scandir()でキー通知ログファイルのみを対象とするためのフィルタ*/
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :0    対象外                                             */
/*----------------------------------------------------------------------------*/
/*                   :1    プロセスログ（"volta" で始まる）                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirFilterSrcIn(const struct dirent *ent)
{
	sint r;
	slong ret;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DEBUG_TRCOUT();
		return 0;
	}
	else
	{
		if(ent->d_type == DT_DIR)
		{
			ret = 0;
		}
		else
		{
			r = strncmp(ent->d_name, ELOG_POWER_SOURCE_IN_FNAME_HD,
						strlen(ELOG_POWER_SOURCE_IN_FNAME_HD));
			if(r != 0)
			{
				ret = 0;
			}
			else
			{
				ret = 1;
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :scandir()でキー通知ログファイルのみを対象とするためのフィルタ*/
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :0    対象外                                             */
/*----------------------------------------------------------------------------*/
/*                   :1    プロセスログ（"volta" で始まる）                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirFilterGenerateOut(const struct dirent *ent)
{
	sint r;
	slong ret;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DEBUG_TRCOUT();
		return 0;
	}
	else
	{
		if(ent->d_type == DT_DIR)
		{
			ret = 0;
		}
		else
		{
			r = strncmp(ent->d_name, ELOG_GENERATE_OUT_FNAME_HD,
						strlen(ELOG_GENERATE_OUT_FNAME_HD));
			if(r != 0)
			{
				ret = 0;
			}
			else
			{
				ret = 1;
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :scandir()でキー通知ログファイルのみを対象とするためのフィルタ*/
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :0    対象外                                             */
/*----------------------------------------------------------------------------*/
/*                   :1    プロセスログ（"volta" で始まる）                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirFilterPowerDeal(const struct dirent *ent)
{
	sint r;
	slong ret;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DEBUG_TRCOUT();
		return 0;
	}
	else
	{
		if(ent->d_type == DT_DIR)
		{
			ret = 0;
		}
		else
		{
			r = strncmp(ent->d_name, ELOG_POWER_DEAL_FNAME_HD,
						strlen(ELOG_POWER_DEAL_FNAME_HD));
			if(r != 0)
			{
				ret = 0;
			}
			else
			{
				ret = 1;
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :scandir()でキー通知ログファイルのみを対象とするためのフィルタ*/
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :0    対象外                                             */
/*----------------------------------------------------------------------------*/
/*                   :1    プロセスログ（"volta" で始まる）                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirFilterAccumulator(const struct dirent *ent)
{
	sint r;
	slong ret;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DEBUG_TRCOUT();
		return 0;
	}
	else
	{
		if(ent->d_type == DT_DIR)
		{
			ret = 0;
		}
		else
		{
			r = strncmp(ent->d_name, ELOG_ACCUMULATOR_FNAME_HD,
						strlen(ELOG_ACCUMULATOR_FNAME_HD));
			if(r != 0)
			{
				ret = 0;
			}
			else
			{
				ret = 1;
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :scandir()でキー通知ログファイルのみを対象とするためのフィルタ*/
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :0    対象外                                             */
/*----------------------------------------------------------------------------*/
/*                   :1    プロセスログ（"volta" で始まる）                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirFilterVehicle(const struct dirent *ent)
{
	sint r;
	slong ret;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DEBUG_TRCOUT();
		return 0;
	}
	else
	{
		if(ent->d_type == DT_DIR)
		{
			ret = 0;
		}
		else
		{
			r = strncmp(ent->d_name, ELOG_VEHICLE_FNAME_HD,
						strlen(ELOG_VEHICLE_FNAME_HD));
			if(r != 0)
			{
				ret = 0;
			}
			else
			{
				ret = 1;
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :scandir()でキー通知ログファイルのみを対象とするためのフィルタ*/
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :0    対象外                                             */
/*----------------------------------------------------------------------------*/
/*                   :1    プロセスログ（"volta" で始まる）                     */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirFilterUpdateResult(const struct dirent *ent)
{
	sint r;
	slong ret;
	DEBUG_TRCIN();

	if(ent == NULL)
	{
		DEBUG_TRCOUT();
		return 0;
	}
	else
	{
		if(ent->d_type == DT_DIR)
		{
			ret = 0;
		}
		else
		{
			r = strncmp(ent->d_name, ELOG_UPDATE_RESULT_FNAME_HD,
						strlen(ELOG_UPDATE_RESULT_FNAME_HD));
			if(r != 0)
			{
				ret = 0;
			}
			else
			{
				ret = 1;
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :scandir()で特定ファイルを除外するためのフィルタ            */
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent *ent : ディレクトリエントリ                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    0 : 対象外 / 1 : 対象ファイル                             */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/17  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_DirentFilterExTemps(const struct dirent *ent)
{
	ulong len;
	slong tmpRet;
	schar *str;
	sint ret;
	bool_t flag = TRUE;
	/* リモコンシリアルNo長 + "HEMSOH-" + '\0'分 */
	schar hemsohHdr[ELOG_SERIES_LEN + 7 + 1]; 
	DEBUG_TRCIN();

	if(ent == NULL)
	{
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


	len = strnlen(ent->d_name, ELOG_FILENAME_MAX);

	/* HEMS制御動作履歴非圧縮ファイル除外判定 */
	memset(hemsohHdr, 0x00, sizeof(hemsohHdr));
	snprintf(hemsohHdr, sizeof(hemsohHdr), "%s%s",
			"HEMSOH-", s_elogRmcSerialNo);
	str = strstr(ent->d_name, hemsohHdr);
	if(str != NULL)
	{
		ret = strncmp(ent->d_name + len - 4, ".txt", 4);
		if(ret == 0)
		{
			tmpRet = 0;
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
		/* 1日分の出力制御用スケジュールファイル除外判定 */
		str = strstr(ent->d_name, "daily_sch_");
		if(str != NULL)
		{
			ret = strncmp(ent->d_name + len - 5, ".data", 5);
			if(ret == 0)
			{
				tmpRet = 0;
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
	else
	{
		/*Do nothing*/
	}

	if(flag)
	{
		/* CRC値保存ファイル除外判定 */
		ret = strncmp(ent->d_name + len - 4, ".crc", 4);
		if(ret == 0)
		{
			tmpRet = 0;
			flag = FALSE;
		}
		else
		{
			/* バックアップ用一時ファイル除外判定 */
			ret = strncmp(ent->d_name + len - 3, ".bk", 3);
			if(ret == 0)
			{
				tmpRet = 0;
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

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :PCSログファイルの日付部分を取得する                        */
/*----------------------------------------------------------------------------*/
/*    param          :schar *fileName : ログファイル名                         */
/*----------------------------------------------------------------------------*/
/*                   :schar **date : 日付部分の先頭ポインタ                     */
/*----------------------------------------------------------------------------*/
/*    return                                                                  */
/*                    ELOG_OK(0)    正常終了                                   */
/*                                                                            */
/*                    ELOG_E_PARAM(-1)    パラメータエラー                      */
/*                                                                            */
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_PcsLogGetFileDate(schar *fileName, schar **date)
{
	schar *pStr;
	slong ret = ELOG_OK;
	DEBUG_TRCIN();

	if(fileName == NULL)
	{
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		if(date == NULL)
		{
			DEBUG_TRCOUT();
			return ELOG_E_PARAM;
		}
		else
		{
			/*Do nothing*/
		}
	}

	pStr = strstr(fileName, "-LOG-");
	if(pStr == NULL)
	{
		ret = ELOG_E_PROCS;
	}
	else
	{
		pStr += strlen("-LOG-");
		*date = pStr;
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :SDPログファイルの日付部分を取得する                        */
/*----------------------------------------------------------------------------*/
/*    param          :schar *fileName : ログファイル名                         */
/*----------------------------------------------------------------------------*/
/*                   :schar **date : 日付部分の先頭ポインタ                     */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常終了                                   */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_SdpLogGetFileDate(schar *fileName, schar **date)
{
	schar *pStr;
	slong ret = ELOG_OK;
	DEBUG_TRCIN();

	if(fileName == NULL)
	{
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		if(date == NULL)
		{
			DEBUG_TRCOUT();
			return ELOG_E_PARAM;
		}
		else
		{
			/*Do nothing*/
		}
	}
	pStr = strstr(fileName, ".txt");
	if(pStr == NULL)
	{
		ret = ELOG_E_PROCS;
	}
	else
	{
		pStr -= ELOG_DATESTR_LEN;

		/* 日付文字列分移動後のポインタがファイル名先頭を越えてはいけない */
		if(pStr < fileName)
		{
			ret = ELOG_E_PROCS;
		}
		else
		{
			*date = pStr;
		}
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :圧縮動作ログファイルの日付部分を取得する                    */
/*----------------------------------------------------------------------------*/
/*    param          :schar *fileName : ログファイル名                          */
/*----------------------------------------------------------------------------*/
/*                   :schar **date : 日付部分の先頭ポインタ                     */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常終了                                   */
/*                                                                            */
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*                                                                            */
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_SvrLogGetFileDate(schar *fileName, schar **date)
{
	schar *pStr;
	slong ret = ELOG_OK;
	DEBUG_TRCIN();

	if(fileName == NULL)
	{
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		if(date == NULL)
		{
			DEBUG_TRCOUT();
			return ELOG_E_PARAM;
		}
		else
		{
			/*Do nothing*/
		}
	}

	pStr = strstr(fileName, ".tgz");
	if(pStr == NULL)
	{
		ret = ELOG_E_PROCS;
	}
	else
	{
		/* 少なくとも日付文字列分のファイル名が無くてはならない */
		if(pStr < (fileName + ELOG_DATESTR_LEN))
		{
			ret = ELOG_E_PROCS;
		}
		else
		{
			if(*(pStr - (ELOG_SUBNO_DIGITS+1)) == '-')
			{
				/* 枝番の'-'ありなら'-'の前を日付文字列とする */
				pStr -= (ELOG_DATESTR_LEN+ELOG_SUBNO_DIGITS+1);
			}
			else
			{
				/* 枝番の'-'なし */
				pStr -= ELOG_DATESTR_LEN;
			}
			/* 日付文字列分移動後のポインタがファイル名先頭を越えてはいけない */
			if(pStr < fileName)
			{
				ret = ELOG_E_PROCS;
			}
			else
			{
				*date = pStr;
			}
		}
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :圧縮SDPファイルの日付部分を取得する                        */
/*----------------------------------------------------------------------------*/
/*    param          :schar *fileName : ログファイル名                         */
/*----------------------------------------------------------------------------*/
/*                   :schar **date : 日付部分の先頭ポインタ                     */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常終了                                   */
/*                                                                            */
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*                                                                            */
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_SdzLogGetFileDate(schar *fileName, schar **date)
{
	schar *pStr;
	slong ret = ELOG_OK;
	DEBUG_TRCIN();

	if(fileName == NULL)
	{
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		if(date == NULL)
		{
			DEBUG_TRCOUT();
			return ELOG_E_PARAM;
		}
		else
		{
			/*Do nothing*/
		}
	}


	pStr = strstr(fileName, ".tgz");
	if(pStr == NULL)
	{
		ret = ELOG_E_PROCS;
	}
	else
	{
		pStr -= ELOG_DATESTR_LEN;

		/* 日付文字列分移動後のポインタがファイル名先頭を越えてはいけない */
		if(pStr < fileName)
		{
			ret = ELOG_E_PROCS;
		}
		else
		{
			*date = pStr;
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :フルパスディレクトリ名からログ種別を判定                    */
/*----------------------------------------------------------------------------*/
/*    param          :schar *fileName : ログファイル名（フルパス）              */
/*----------------------------------------------------------------------------*/
/*    return         :ElogType_t                                              */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static ElogType_t elog_whatTypeLog(schar *fileName)
{
	sint r;
	slong ret = ELOG_TYPE_NON;
	DEBUG_TRCIN();

	if(fileName == NULL)
	{
		DEBUG_TRCOUT();
		return ELOG_TYPE_NON;
	}
	else
	{
		r = strncmp(fileName, ELOG_ERR_EVENT_DIR, strlen(ELOG_ERR_EVENT_DIR));
		if(r == 0)
		{
			ret = ELOG_TYPE_OPT_INFORM;
		}
		else
		{
			r = strncmp(fileName, ELOG_SDZ_DIR, strlen(ELOG_SDZ_DIR));
			if(r == 0)
			{
				ret = ELOG_TYPE_SDZ;
			}
			else
			{
				r = strncmp(fileName, ELOG_SDP_DIR, strlen(ELOG_SDP_DIR));
				if(r == 0)
				{
					ret = ELOG_TYPE_SDP;
				}
				else
				{
					r = strncmp(fileName, ELOG_ACTION_DIR,
								strlen(ELOG_ACTION_DIR));
					if(r == 0)
					{
						ret = ELOG_ACTION_DIR;
					}
					else
					{
						ret = ELOG_TYPE_NON;
					}
				}
			}
		}
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :ログファイル名の日付部分を取得する                         */
/*----------------------------------------------------------------------------*/
/*    param          :schar *fileName : ログファイル名（フルパス）              */
/*----------------------------------------------------------------------------*/
/*                   :schar **date : 日付部分の先頭ポインタ                     */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                   ELOG_OK(0)    正常終了                                    */
/*                   ELOG_E_PARAM(-1)    パラメータエラー                      */
/*                   ELOG_E_PROCS(-3)    処理エラー                            */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_GetFileDate(schar *fileName, schar **date)
{
	ElogType_t type;
	slong ret;
	schar tmpRet;
	ulong i;
	DEBUG_TRCIN();
	if(fileName == NULL)
	{
		DLOG_Error("fileName is NULL\n");
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


	type = elog_whatTypeLog(fileName);
	switch(type)
	{
	case ELOG_TYPE_ACZ:
		ret = ELOG_PcsLogGetFileDate(fileName, date);
		break;
	case ELOG_TYPE_SDP:
		ret = ELOG_SdpLogGetFileDate(fileName, date);
		break;
	case ELOG_TYPE_OPT_INFORM:
		ret = ELOG_SvrLogGetFileDate(fileName, date);
		break;
	case ELOG_TYPE_SDZ:
		ret = ELOG_SdzLogGetFileDate(fileName, date);
		break;
	default:
		DLOG_Error("unknown log path.(%s)\n", fileName);
		tmpRet = ELOG_E_PROCS;
	}

	if(ret == ELOG_OK)
	{
		/* 日付文字列内の文字は数字でなければならない */
		for(i=0u; i < ELOG_DATESTR_LEN; i++)
		{
			if(! isdigit((sint)*((*date)+i)))
			{
				tmpRet = ELOG_E_PROCS;
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
	DEBUG_TRCOUT();
	return tmpRet;
}

/*============================================================================*/
/*    Description    :ログファイル名の日付部分を比較する                         */
/*----------------------------------------------------------------------------*/
/*    param          :struct dirent **entA : ディレクトリエントリ               */
/*----------------------------------------------------------------------------*/
/*                   :struct dirent **entB : ディレクトリエントリ               */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    -1 : entA < entB / 0 : entA = entB / 1 : entA > entB    */
/*----------------------------------------------------------------------------*/
/*    detail         :注意事項                                                 */
/*                    (1) この関数はファイル名がフルパスであることが前提のため、   */
/*                    scandir()のソート用には使用しないこと。                    */
/*                    (2) この関数の処理対象ファイルのディレクトリは              */
/*                        /mnt/log/pcs                                        */
/*                        /mnt/log/sdp                                        */
/*                        /mnt/log/server                                     */
/*                        /mnt/log/sdptgz                                     */
/*                    のみであり、上記以外のディレクトリファイルについては、       */
/*                    比較結果を常に後方とする。                                 */
/*                    (3) ログファイルの命名規則に従っていない場合には、正常に     */
/*                    比較できない。                                            */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_FileDateComp(const void *entA, const void *entB)
{
	schar *pA;
	schar *pB;
	sint ret;
	DEBUG_TRCIN();

	if(entA == NULL)
	{
		DEBUG_TRCOUT();
		return 1;	/* ファイル名規則に合わない場合はリストの最後尾にする */
	}
	else
	{
		if(entB == NULL)
		{
			DEBUG_TRCOUT();
			return -1;	/* ファイル名規則に合わない場合はリストの最後尾にする */
		}
		else
		{
			/*Do nothing*/
		}
	}

	ret = ELOG_GetFileDate((*(struct dirent **)(entA))->d_name, &pA);
	if(ret != ELOG_OK)
	{
		ret = 1;	/* ファイル名規則に合わない場合はリストの最後尾にする */
	}
	else
	{
		ret = ELOG_GetFileDate((*(struct dirent **)(entB))->d_name, &pB);
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
/*    Description    :ファイルサイズ取得                                        */
/*----------------------------------------------------------------------------*/
/*    param          :schar  *path : ファイルパス                              */
/*----------------------------------------------------------------------------*/
/*                   :ulong *size : サイズ返却用                               */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常終了                                   */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-2)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_NOENT(-4)    ファイル無し                         */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/10  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong elog_GetFileSize(schar *path, ulong *size)
{
	slong ret = ELOG_OK;
	struct stat st;
	DEBUG_TRCIN();

	if(path == NULL)
	{
		DLOG_Error("path is NULL.\n");
		DEBUG_TRCOUT();
		ret = ELOG_E_PARAM;
	}
	else
	{
		if(size == NULL)
		{
			DLOG_Error("size is NULL.\n");
			DEBUG_TRCOUT();
			ret = ELOG_E_PARAM;
		}
		else
		{
			/*Do thing*/
		}
	}

	ret = stat(path, &st);
	if(ret < 0)
	{
		DLOG_Error("stat(%s) is failed.(%d)\n", path, errno);
		if(errno == ENOENT)
		{
			ret = ELOG_E_NOENT;
		}
		else
		{
			ret = ELOG_E_SYSCAL;
		}
	}
	else
	{
		*size = st.st_size;
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :パーティションサイズパラメータ取得(MB単位)                 */
/*----------------------------------------------------------------------------*/
/*    param          :char  *path  : ファイルパス                              */
/*----------------------------------------------------------------------------*/
/*                   :ulong *avail : 残容量                                   */
/*----------------------------------------------------------------------------*/
/*                   :ulong *used  : 使用済み容量                              */
/*----------------------------------------------------------------------------*/
/*                   :ulong *all   : 総容量                                   */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK    正常終了                                      */
/*                                                                            */
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*                                                                            */
/*                    ELOG_E_SYSCAL(-2)    システムコールエラー                */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_GetPartitionInfoMB_All(schar *path, ulong *avail,
									ulong *used, ulong *all)
{
	slong ret = ELOG_OK;
	struct statvfs st;
	ulong blkPMB;

	DEBUG_TRCIN();
	if(path == NULL)
	{
		DLOG_Error("path is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		if(avail == NULL)
		{
			DLOG_Error("avail is NULL.\n");
			DEBUG_TRCOUT();
			return ELOG_E_PARAM;
		}
		else
		{
			if(used == NULL)
			{
				DLOG_Error("used is NULL.\n");
				DEBUG_TRCOUT();
				return ELOG_E_PARAM;
			}
			else
			{
				if(all == NULL)
				{
					DLOG_Error("avail is NULL.\n");
					DEBUG_TRCOUT();
					return ELOG_E_PARAM;
				}
				else
				{
					/*Do nothing*/
				}
			}
		}
	}

	ret = statvfs(path, &st);
	if(ret < 0)
	{
		DLOG_Error("statvfs(%s) is failed.(%d)\n", path, errno);
		DEBUG_TRCOUT();
		ret = ELOG_E_SYSCAL;
	}
	else
	{
		blkPMB = ELOG_MB(1) / st.f_frsize;
		*avail = st.f_bavail / blkPMB;
		*all   = st.f_blocks / blkPMB;
		*used  = *all - *avail;
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :ログパーティションサイズパラメータ取得(MB単位)              */
/*----------------------------------------------------------------------------*/
/*    param          :schar  *path  : ファイルパス                             */
/*----------------------------------------------------------------------------*/
/*                   :ulong *avail : 残容量                                   */
/*----------------------------------------------------------------------------*/
/*                   :ulong *used  : 使用済み容量                              */
/*----------------------------------------------------------------------------*/
/*                   :ulong *all   : 総容量                                   */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK    正常終了                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-2)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_GetPartitionInfoMB(schar *path, ulong *avail,
								ulong *used, ulong *all)
{
	slong ret = 0;
	struct statvfs st;
	ulong blkPMB = 0;
	ulong fwFileSum = 0;
	DIR* dirP = NULL;
	struct dirent entry;
	struct dirent *result = NULL;
	ulong cnt = 0;
	/* ファイル名最大長 + "/mnt/log/upgrade/" + '\0'分 */
	schar fPath[NAME_MAX + 17 + 1];
	struct stat stStat;

	DEBUG_TRCIN();
	memset(&st, 0x00, sizeof(st));
	memset(&entry, 0x00, sizeof(entry));
	memset(&fPath, 0x00, sizeof(fPath));
	memset(&stStat, 0x00, sizeof(stStat));

	/* パラメータチェック(pathは使用しない為、チェック対象外) */
	if(avail == NULL)
	{
		DLOG_Error("avail is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		if(used == NULL)
		{
			DLOG_Error("used is NULL.\n");
			DEBUG_TRCOUT();
			return ELOG_E_PARAM;
		}
		else
		{
			if(all == NULL)
			{
				DLOG_Error("avail is NULL.\n");
				DEBUG_TRCOUT();
				return ELOG_E_PARAM;
			}
			else
			{
				/*Do nothing*/
			}
		}
	}

	/* FW更新用ファイルの合計サイズ算出 */
	dirP = opendir(ELOG_FW_DIR);
	if(dirP != NULL)
	{
		while(cnt < ELOG_NAND_FW_NUM_MAX)
		{
			ret = readdir_r(dirP, &entry, &result);
			if(ret != 0)
			{
				cnt = ELOG_NAND_FW_NUM_MAX;
			}
			else if(result == NULL)
			{
				/* ディレクトリ走査完了 */
				cnt = ELOG_NAND_FW_NUM_MAX;
			}
			else
			{
				if(result->d_type == DT_REG)
				{
					/* 通常ファイルならサイズ取得 */
					memset(&fPath, 0x00, sizeof(fPath));
					snprintf(fPath, sizeof(fPath), "%s/%s",
						ELOG_FW_DIR, result->d_name);
					ret = stat(fPath, &stStat);
					if(ret == 0)
					{
						/* 合計値に加算 */
						/* 1blockは512byte(statの仕様)*/
						fwFileSum += (stStat.st_blocks * 512);	
						DLOG_Error("Add fwFileSum(%s)(%lu)\n",
									fPath, (stStat.st_blocks * 512));
					}
					else
					{
						/*Do nothing*/
					}
				}
			}
			cnt++;
		}
		closedir(dirP);
		dirP = NULL;
	}

	fwFileSum = fwFileSum / 1024 / 1024;	/* Byte ⇒ MByte換算 */
	DLOG_Error("fwFileSum(%lu)\n", fwFileSum);

	/* ログ用パーティションのサイズ取得 */
	ret = statvfs(ELOG_FS_LOG, &st);
	if(ret < 0)
	{
		DLOG_Error("statvfs(%s) is failed.(%d)\n", path, errno);
		ret = ELOG_E_SYSCAL;
	}
	else
	{
		blkPMB = ELOG_MB(1) / st.f_frsize;	/* 1MBあたりのblock数 */
		*avail = st.f_bavail / blkPMB;		/* 未使用容量(MB換算) */
		*all   = st.f_blocks / blkPMB;		/* 全容量(MB換算) */
		*used  = *all - *avail;				/* 使用量(MB換算) */
		DLOG_Error("statvfs result(all:%lu avail:%lu used:%lu)\n",
					 *all, *avail, *used);

		/* FW行進用ファイルマージンを減算した全容量を算出 */
		if(*all >= ELOG_NAND_FW_MARGINE)
		{
			*all -= ELOG_NAND_FW_MARGINE;
		}
		else
		{
			*all = 0;
		}
		/* FW更新用ファイルを減算した使用量を算出 */
		if(*used >= fwFileSum)
		{
			*used -= fwFileSum;
		}
		else
		{
			*used = 0;
		}
		/* FW行進用ファイルマージンを減算した空き容量を算出 */
		if(*all >= *used)
		{
			*avail = *all - *used;
		}
		else
		{
			*avail = 0;
		}
		DLOG_Error("calc result(all:%lu avail:%lu used:%lu)\n",
					*all, *avail, *used);
	}

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :FWアップグレードディレクトリサイズ取得(MB単位)              */
/*----------------------------------------------------------------------------*/
/*    param          :ulong *used  : 使用済み容量                              */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK    正常終了                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-2)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/31  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong elog_GetUpgradDirSize(ulong *used)
{
	slong ret = 0;
	ulong blkPMB = 0;
	ulong sum = 0;
	DIR* dirP = NULL;
	struct dirent entry;
	struct dirent *result = NULL;
	ulong cnt = 0;
	/* ファイル名最大長 + "/mnt/log/upgrade/" + '\0'分 */
	schar fPath[NAME_MAX + 17 + 1];
	struct stat stStat;

	DEBUG_TRCIN();
	memset(&entry, 0x00, sizeof(entry));
	memset(&fPath, 0x00, sizeof(fPath));
	memset(&stStat, 0x00, sizeof(stStat));

	if(used == NULL)
	{
		DLOG_Error("used is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		/*Do nothing*/
	}
	/* ディレクトリの合計サイズ算出 */
	dirP = opendir(ELOG_USR_FW_DIR);
	if(dirP != NULL)
	{
		while(cnt < ELOG_NAND_FW_NUM_MAX)
		{
			ret = readdir_r(dirP, &entry, &result);
			if(ret != 0)
			{
				cnt = ELOG_NAND_FW_NUM_MAX;
			}
			else if(result == NULL)
			{
				/* ディレクトリ走査完了 */
				cnt = ELOG_NAND_FW_NUM_MAX;
			}
			else
			{
				if(result->d_type == DT_REG)
				{
					/* 通常ファイルならサイズ取得 */
					memset(&fPath, 0x00, sizeof(fPath));
					snprintf(fPath, sizeof(fPath), "%s/%s",
						ELOG_USR_FW_DIR, result->d_name);
					ret = stat(fPath, &stStat);
					if(ret == 0)
					{
						/* 合計値に加算 */
						/* 1blockは512byte(statの仕様) */
						sum = sum + (stStat.st_blocks * 512);	
						DLOG_Error("Add sum(%s)(%lu)\n", fPath,
						(stStat.st_blocks * 512));
					}
				}
			}
			cnt++;
		}
		closedir(dirP);
		dirP = NULL;
	}
	*used = (ulong)(sum / 1024 / 1024);	/* Byte ⇒ MByte換算(端数切捨て) */
	DLOG_Error("calc result(used:%lu)\n", *used);

	DEBUG_TRCOUT();
	return ELOG_OK;
}

/*============================================================================*/
/*    Description    :ファイルに付与する現在日時文字列を取得                     */
/*----------------------------------------------------------------------------*/
/*    param          :schar *buf : 文字列格納ポインタ                           */
/*----------------------------------------------------------------------------*/
/*                    ulong size : 文字列格納バッファサイズ                     */
/*----------------------------------------------------------------------------*/
/*                    ElogDateStrType_t type : 文字列タイプ                    */
/*----------------------------------------------------------------------------*/
/*                    slong timer : 区间时间时，间隔时间                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常終了                                   */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-2)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/27  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_GetDStr(schar *buf, ulong size, 
					ElogDateStrType_t type , slong timToTim)
{
	slong ret;
	slong tmpRet = ELOG_OK;
	EtimTm_t etm;
	time_t tmSec;
	time_t changeTime = time(NULL);
	struct tm ltm;
	struct tm *rtm;
	DEBUG_TRCIN();

	if(buf == NULL)
	{
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		/*Do nothing*/
	}
	
	ret = ETIM_GetTime(&etm);
	if(ret != ELOG_OK)
	{
		DLOG_Error("ETIM_GetTime() error.\n");
		tmpRet = ELOG_E_SYSCAL;
	}
	else
	{
		if(timToTim < 0)
		{
			changeTime = tmSec + timToTim;
		}
		else
		{
			changeTime = tmSec + timToTim;
		}

		/* 年月日時分秒へ変換 */
		rtm = localtime_r(&changeTime, &ltm);
		if(NULL == rtm)
		{
			ret = ELOG_E_PROCS;
		}
		else
		{
			if(type == ELOG_DATESTR_DCS)
			{
				snprintf(buf, size, "%4d%02d%02d-%02d%02d%02d",
						etm.tmYear, etm.tmMon, etm.tmMday,
						etm.tmHour, etm.tmMin, etm.tmSec);
			}
			else if(type == ELOG_DATESTR_Y_S)
			{	
				snprintf(buf, size, "%4d/%02d/%02d %02d:%02d:%02d",
						etm.tmYear, etm.tmMon, etm.tmMday,
						etm.tmHour, etm.tmMin, etm.tmSec);
			}
			else if(type == ELOG_DATESTR_OUTPUTCTRL)
			{	/* ELOG_DATESTR_OUTPUTCTRL */
				snprintf(buf, size, "%04d/%02d/%02d %02d:%02d",
						etm.tmYear, etm.tmMon, etm.tmMday,
						etm.tmHour, etm.tmMin);
			}
			else if(type == ELOG_DATESTR_OUPUTCTRL_SECTION)
			{
				if(timToTim < 0)
				{
					snprintf(buf, size, "%4d/%02d/%02d %02d:%02d - %02d:%02d",
							rtm->tm_year, rtm->tm_mon, rtm->tm_mday,
							rtm->tm_hour, rtm->tm_min, etm.tmHour, etm.tmMin);
				}
				else
				{
					snprintf(buf, size, "%4d/%02d/%02d %02d:%02d - %02d:%02d",
							rtm->tm_year, rtm->tm_mon, rtm->tm_mday,
							etm.tmHour, etm.tmMin, rtm->tm_hour, rtm->tm_min);
				}
			}
			else if(type == ELOG_DATESTR_HMS)
			{
				snprintf(buf, size, "%02d%02d%02d",
				etm.tmHour, etm.tmMin, etm.tmSec);
			}
			else if(type == ELOG_DATESTR_UPDATE)
			{
				snprintf(buf, size, "%4d%02d%02d%02d%02d%02d",
						etm.tmYear, etm.tmMon, etm.tmMday,
						etm.tmHour, etm.tmMin, etm.tmSec);
			}
			else if(type == ELOG_DATESTR_YMD)
			{
				snprintf(buf, size, "%4d%02d%02d",
				etm.tmYear, etm.tmMon, etm.tmMday);
			}
			else if(type == ELOG_DATESTR_YM)
			{
				snprintf(buf, size, "%4d%02d", etm.tmYear, etm.tmMon);
			}
			else if(type == ELOG_DATESTR_Y)
			{
				snprintf(buf, size, "%4d", etm.tmYear);
			}
			else
			{
				/*Do nothing*/
			}
		}
	}

	DEBUG_TRCOUT();
	return tmpRet;
}

/*============================================================================*/
/*    Description    :获取文件大小                                             */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    返回文件大小                                             */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/23  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong elog_GetSize(const char *path)
{
	slong filesize = -1;
	struct stat statbuff;
	slong ret;
	DEBUG_TRCIN();
	if(stat(path, &statbuff) < 0)
	{
		ret = filesize;
	}
	else
	{
		ret = statbuff.st_size;
	}
	DEBUG_TRCOUT();
	return ret;
}
/*============================================================================*/
/*    Description    :oldPath から newPath へ移動して oldPath を消去            */
/*----------------------------------------------------------------------------*/
/*    param          :schar *oldPath : 移動元ファイルパス（フルパス）            */
/*----------------------------------------------------------------------------*/
/*                   :schar *newPath : 移動元ファイルパス（フルパス）            */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)     ファイルサイズ                            */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-2)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/07  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_MoveFile(schar *oldPath, schar *newPath)
{
	slong ret;

	DEBUG_TRCIN();
	if(oldPath == NULL)
	{
		DLOG_Error("oldPath is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		if(newPath == NULL)
		{
			DLOG_Error("newPath is NULL.\n");
			DEBUG_TRCOUT();
			return ELOG_E_PARAM;
		}
		else
		{
			/*Do nothing*/
		}
	}

	/* ファイルコピー＆消去 */
	ret = elog_CopyFile(oldPath, newPath, TRUE);

	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :現ファイルのサイズが超過していたら、最古ファイルを削除       */
/*----------------------------------------------------------------------------*/
/*    param          :schar *currDir       : 現ファイル検索ディレクトリ         */
/*----------------------------------------------------------------------------*/
/*                   :ulong lineNum        : 現ファイル line number            */
/*----------------------------------------------------------------------------*/
/*                   :ulong limNum         : 現ファイルline上限                */
/*----------------------------------------------------------------------------*/
/*                   :ElogType_t type     : ファイル種別                      */
/*----------------------------------------------------------------------------*/
/*                   :schar *fName         : 最新ファイル名                    */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    1  現ファイルサイズ超過  / 0    現ファイルサイズ範囲内       /
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/22  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_CheckCurrentAndDeleteOld(
		schar *currDir, ulong lineNum,
		ulong limNum, ElogType_t type,
		schar *fName
)
{
	slong ret = 0;
	schar currPath[ELOG_FILEPATH_MAX + 1];
	schar fileCmd[ELOG_FILENAME_MAX * 2 + 10];

	DEBUG_TRCIN();

	if(currDir == NULL)
	{
		DLOG_Error("currDir is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		if(fName == NULL)
		{
			DLOG_Error("fName is NULL.\n");
			DEBUG_TRCOUT();
			return ELOG_E_PARAM;
		}
		else
		{
			/*Do nothing*/
		}
	}

	/* 最新ファイル名取得 */
	ret = elog_SearchNewFile(currDir, type, fName);
	if(ret == ELOG_E_NOENT)
	{
		DLOG_Error("no file found.\n");
		ret = ELOG_E_NOENT;
	}
	else if(ret != ELOG_OK)
	{
		DLOG_Error("elog_SearchNewFile() error.\n");
		ret = ELOG_E_PROCS;
	}
	else
	{
		/* ファイルフルパス生成 */
		memset(currPath, 0x0, sizeof(currPath));
		snprintf(currPath, sizeof(currPath)-1, "%s/%s", currDir, fName);

		if(lineNum >= limNum)
		{
			memset(fileCmd, 0x0, sizeof(fileCmd));
			snprintf(fileCmd, sizeof(fileCmd)-1, "sed -i '1d' %s", currPath);
			CMN_MutexLock(&s_elogSysCmdMutx);
			ELOG_CALL_SYSTEM(fileCmd);
			CMN_MutexUnlock(&s_elogSysCmdMutx);
			s_elogFileSize[type].curlineNum--;
			ret = 0;
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
/*    Description    :检查文件日期，超过最大限制，删除最古老的文件                */
/*----------------------------------------------------------------------------*/
/*    param          :schar *currDir:现在文件的路径                            */
/*----------------------------------------------------------------------------*/
/*                   :ElogFileNumType_t type:检查的文件类型                    */
/*----------------------------------------------------------------------------*/
/*                   :slong limFileNum:文件的最大保存数                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_OK(0)    正常                                      */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_SYSCAL(-1)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*                   :ELOG_E_PROCS(-3)    処理エラー                           */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/27  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_CheckFileDateAndDeleteOld(
		schar *currDir,
		ElogFileNumType_t type,
		slong limFileNum
)
{
	struct dirent **nameList = NULL;
	sint n;
	slong ret = ELOG_OK;
	schar tmp[ELOG_FILEPATH_MAX];
	DEBUG_TRCIN();

	if(currDir == NULL)
	{
		DLOG_Error("currDir is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		/*Do nothing*/
	}

	if(type == ELOG_FILE_NUM_OUTPUTCTRL)
	{
		n = scandir(currDir, &nameList, elog_DirFilterOutputCtl, alphasort);
	}
	else if(type == ELOG_FILE_NUM_POWER_OUT)
	{
		n = scandir(currDir, &nameList, elog_DirFilterSrcOut, alphasort);
	}
	else if(type == ELOG_FILE_NUM_SOURCE_IN)
	{
		n = scandir(currDir, &nameList, elog_DirFilterSrcIn, alphasort);
	}
	else if(type == ELOG_FILE_NUM_GENERATE_OUT)
	{
		n = scandir(currDir, &nameList, elog_DirFilterGenerateOut, alphasort);
	}
	else if(type == ELOG_FILE_NUM_POWER_DEAL_YEAR)
	{
		n = scandir(currDir, &nameList, elog_DirFilterPowerDeal, alphasort);
	}
	else if(type == ELOG_FILE_NUM_POWER_DEAL_MONTH)
	{
		n = scandir(currDir, &nameList, elog_DirFilterPowerDeal, alphasort);
	}
	else if(type == ELOG_FILE_NUM_POWER_DEAL_DAY)
	{
		n = scandir(currDir, &nameList, elog_DirFilterPowerDeal, alphasort);
	}
	else if(type == ELOG_FILE_NUM_ACCUMULATOR_YEAR)
	{
		n = scandir(currDir, &nameList, elog_DirFilterAccumulator, alphasort);
	}
	else if(type == ELOG_FILE_NUM_ACCUMULATOR_MONTH)
	{
		n = scandir(currDir, &nameList, elog_DirFilterAccumulator, alphasort);
	}
	else if(type == ELOG_FILE_NUM_ACCUMULATOR_DAY)
	{
		n = scandir(currDir, &nameList, elog_DirFilterAccumulator, alphasort);
	}
	else if(type == ELOG_FILE_NUM_VEHICLE_YEAR)
	{
		n = scandir(currDir, &nameList, elog_DirFilterVehicle, alphasort);
	}
	else if(type == ELOG_FILE_NUM_VEHICLE_MONTH)
	{
		n = scandir(currDir, &nameList, elog_DirFilterVehicle, alphasort);
	}
	else if(type == ELOG_FILE_NUM_VEHICLE_DAY)
	{
		n = scandir(currDir, &nameList, elog_DirFilterVehicle, alphasort);
	}
	else if(type == ELOG_FILE_NUM_UPDATE)
	{
		n = scandir(currDir, &nameList, elog_DirFilterUpdateResult, alphasort);
	}
	else
	{
		/*Do nothing*/
	}
	if(n < 0)
	{
		DLOG_Error("scandir() error.(%d,%d)\n", n, errno);
		ret = ELOG_E_SYSCAL;
	}
	else if(n == 0)
	{
		DLOG_Error("no file there.\n");
		ELOG_DirentFree(&nameList, n);
		ret = ELOG_E_NOENT;
	}
	else
	{
		if(n == limFileNum)
		{
			memset(tmp, 0x0, sizeof(tmp));
			snprintf(tmp, sizeof(tmp) - 1, "%s/%s",
					currDir, nameList[0]->d_name);
			remove(tmp);
			if(ret == -1)
			{
				DLOG_Error("remove() is failed.(%d)\n", errno);
				ELOG_DirentFree(&nameList, n);
				ret = ELOG_E_SYSCAL;
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

	/* ソート結果を開放する */
	ELOG_DirentFree(&nameList, n);
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :指定ディレクトリpath内のディレクトリエントリを検索          */
/*                    検索対象がディレクトリの場合、下位のディレクトリを再帰検索する*/
/*----------------------------------------------------------------------------*/
/*    param          :schar *path : 検索ディレクトリ                           */
/*----------------------------------------------------------------------------*/
/*                   ELOG_FILELIST_T *nameList : ディレクトリ構造体（結果格納）  */
/*----------------------------------------------------------------------------*/
/*                   ELOG_SCANDIR_SELECT_E select : 検索対象                   */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                   ELOG_OK    正常終了                                       */
/*----------------------------------------------------------------------------*/
/*                   ELOG_E_PARAM(-1)    パラメータエラー                      */
/*----------------------------------------------------------------------------*/
/*                   ELOG_E_SYSCAL(-2)    システムコールエラー                  */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/17  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_ScanFiles(schar *path, ElogFileList_t *nameList,
					ElogScndirSelect_t select)
{
	sint ret;
	slong tmpRet = ELOG_OK;
	struct dirent **getList = NULL;
	slong n;
	slong i;
	slong idx;
	bool_t flag = TRUE;
	schar tmp[ELOG_FILENAME_MAX + 1];	/* direntのd_nameと同サイズ */

	if(path == NULL)
	{
		return ELOG_E_PARAM;
	}
	else
	{
		if(nameList == NULL)
		{
			return ELOG_E_PARAM;
		}
		else
		{
			/*Do nothing*/
		}
	}

	/* ディレクトリ内スキャン */
	if(select == ELOG_SCANDIR_DIRS)
	{
		n = scandir(path, &getList, elog_DirentFilterDirs, NULL);
	}
	else if(select == ELOG_SCANDIR_FILES)
	{
		n = scandir(path, &getList, elog_DirentFilterFiles, NULL);
	}
	else if(select == ELOG_SCANDIR_EXTMPS)
	{
		n = scandir(path, &getList, elog_DirentFilterExTemps, NULL);
	}
	else
	{
		DLOG_Error("select is unknown.(%d)\n", select);
		return ELOG_E_PARAM;
	}

	DLOG_Error("scandir() return size = %d.\n", n);

	if(n == 0)
	{
		/* 下位ディレクトリ無し */
		ELOG_DirentFree(&getList, n);
		tmpRet = ELOG_OK;
	}
	else if(n < 0)
	{
		if(errno == ENOENT)
		{
			/* エントリ無し */
			tmpRet = ELOG_OK;
		}
		else
		{
			DLOG_Error("scandir() error.(%d,%d)\n", n, errno);
			tmpRet = ELOG_E_SYSCAL;
		}
	}
	else
	{
		/* nameListへgetlistを追加コピー */
		ret = elog_DirentAddCopy(&(nameList->list), nameList->size,
								 getList, n, path);
		if(ret < 0)
		{
			DLOG_Error("elog_DirentAddCopy() error.\n");
			ELOG_DirentFree(&getList, n);
			tmpRet = ret;
		}
		else
		{
			DLOG_Error("direntAddCopy() return %ld.\n", nameList->size);
			nameList->size = ret;
			ELOG_DirentFree(&getList, n);
		}
	}

	if(flag)
	{
				/* ディレクトリ検索の場合 */
		if(select == ELOG_SCANDIR_DIRS)
		{
			/* getlistのエントリを再帰検索 */
			idx = nameList->size - n;
			for (i = 0; i < n; i++, idx++)
			{
				memset(tmp, 0x0, sizeof(tmp));
				strncpy(tmp, nameList->list[idx]->d_name, sizeof(tmp)-1);
				ret = ELOG_ScanFiles(tmp, nameList, select);
				if(ret != ELOG_OK)
				{
					DLOG_Error("elog_scanFiles(%s) error.\n", tmp);
					tmpRet = ELOG_E_PROCS;
					i = n;
				}
				else
				{
					/*Do nothing*/
				}
			}
		}
		DLOG_Error("fin(%ld).\n", nameList->size);
	}

	return tmpRet;
}
/*============================================================================*/
/*    Description    :プロセスログをRAMからNANDへコピーする                      */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/07  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_SaveLogRamToNand(void)
{
	slong ret;
	slong i;
	sint n;
	struct dirent **nameList = NULL;
	schar ram[ELOG_FILENAME_MAX + 1];	/* direntのd_nameと同サイズ */
	schar nand[ELOG_FILENAME_MAX + 1];	/* direntのd_nameと同サイズ */
	DEBUG_TRCIN();

	/* RAMディレクトリ内ログファイル(volta***.txt)スキャン */
	n = scandir(ELOG_RAM_DIR, &nameList, elog_DirFilterLog, NULL);
	if(n < 0)
	{
		DLOG_Error("scandir(%s) error.(%d,%d)\n", ELOG_RAM_DIR, n, errno);
	}
	else if(n == 0)
	{
		DLOG_Error("%s no file there.\n", ELOG_RAM_DIR);
		ELOG_DirentFree(&nameList, n);
	}
	else
	{
		if(nameList == NULL)
		{
			DLOG_Error("%s scan but list is NULL.\n", ELOG_RAM_DIR);
		}
		else
		{
			/* 対象ファイル１件づつNANDへコピー＆フラッシュ（通常は１件） */
			for(i = 0; i < n; i++) {

				/* RAMファイルpath */
				memset(ram, 0x0, sizeof(ram));
				snprintf(ram, sizeof(ram)-1, "%s/%s",
				ELOG_RAM_DIR, nameList[i]->d_name);

				/* NANDファイルpath */
				memset(nand, 0x0, sizeof(nand));
				snprintf(nand, sizeof(nand)-1, "%s/%s",
				ELOG_LOG_DIR, nameList[i]->d_name);

				/* ファイルコピー（消去なし） */
				ret = elog_CopyFile(ram, nand, FALSE);
				if(ret != ELOG_OK)
				{
					DLOG_Error("elog_CopyFile() error.\n");
				}
				else
				{
					/* リスト開放 */
					ELOG_DirentFree(&nameList, n);
				}
			}
		}
	}
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :ファイルをコピーする                                      */
/*----------------------------------------------------------------------------*/
/*    param          :schar *srcPath : コピー元ファイルパス（フルパス）          */
/*----------------------------------------------------------------------------*/
/*                   :schar *dstPath : コピー先ファイルパス（フルパス）          */
/*----------------------------------------------------------------------------*/
/*                   :bool_t ease;    コピー後にコピー元を消去（移動）           */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                   ELOG_OK(0)    ファイルサイズ                              */
/*                                                                            */
/*                   ELOG_E_PARAM(-1)    パラメータエラー                      */
/*                                                                            */
/*                   ELOG_E_SYSCAL(-2)    システムコールエラー                  */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_CopyFile(schar *srcPath, schar *dstPath, bool_t erase)
{
	sint ret;
	schar *pret;
	FILE *fpSrc;
	FILE *fpDst;
	sint fdDst;
	schar tmp[NAME_MAX + 1];	/* コピーバッファ */
	DEBUG_TRCIN();

	/* 引数チェック */
	if(srcPath == NULL)
	{
		DLOG_Error("srcPath is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		if(dstPath == NULL)
		{
			DLOG_Error("dstPath is NULL.\n");
			DEBUG_TRCOUT();
			return ELOG_E_PARAM;
		}
		else
		{
			/*Do nothing*/
		}
	}

	/* コピー元ファイルopen */
	fpSrc = ELOG_FOPEN(srcPath, "r");
	if(fpSrc == NULL)
	{
		DLOG_Error("ELOG_FOPEN(%s) error.(%d)\n", srcPath, errno);
		ret = ELOG_E_SYSCAL;
	}
	else
	{
		/* コピー先ファイルopen(上書き) */
		fpDst = ELOG_FOPEN(dstPath, "w");
		if(fpDst == NULL)
		{
			DLOG_Error("ELOG_FOPEN(%s) error.(%d)\n", dstPath, errno);
			ELOG_FCLOSE(fpSrc);
			ret = ELOG_E_SYSCAL;
		}
		else
		{
			/* 行コピー */
			ret = feof(fpSrc);
			while(!ret)
			{
				memset(tmp, 0x0, sizeof(tmp));
				pret = fgets(tmp, sizeof(tmp)-1, fpSrc);
				if(pret == NULL)
				{
					ret = TRUE;
				}
				else
				{
					ret = fputs(tmp, fpDst);
					if(ret == EOF)
					{
						ret = TRUE;
					}
					else
					{
						ret = feof(fpSrc);
					}
				}
			}
		}
		if(fpSrc != NULL)
		{
			/* コピー先ファイルフラッシュ */
			fflush(fpDst);

			/* コピー先ファイルSYNC */
			fdDst = fileno(fpDst);
			if(fdDst == -1)
			{
				DLOG_Error("fsync(%s) error.(%d)\n", dstPath, errno);
				ELOG_FCLOSE(fpSrc);
				ELOG_FCLOSE(fpDst);
				ret = ELOG_E_SYSCAL;
			}
			else
			{
				fsync(fdDst);

				/* ファイルクローズ */
				ELOG_FCLOSE(fpSrc);
				ELOG_FCLOSE(fpDst);

				/* コピー元ファイルをremove */
				if(erase == TRUE)
				{
					ret = remove(srcPath);
					if(ret == -1)
					{
						DLOG_Error("remove(%s)failed.(%d)\n", srcPath, errno);
						ret = ELOG_E_SYSCAL;
					}
					else
					{
						/*Do nothing*/
					}
				}
			}
		}
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :ファイル名重複確認（枝番重複含む)                          */
/*----------------------------------------------------------------------------*/
/*    param          :schar *filePath : ファイル名                             */
/*----------------------------------------------------------------------------*/
/*    param          :schar *braPath : 枝番付きファイル名返却バッファ            */
/*----------------------------------------------------------------------------*/
/*    param          :ulong size : 返却バッファサイズ                           */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    0    正常終了（枝番なし）                                 */
/*----------------------------------------------------------------------------*/
/*                    1以上    正常終了、枝番号                                 */
/*----------------------------------------------------------------------------*/
/*                    ELOG_E_PARAM(-1)    パラメータエラー                     */
/*----------------------------------------------------------------------------*/
/*                   ELOG_E_PROCS(-3)    処理エラー                            */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_CheckSameFileName(schar *filePath, schar* braPath, ulong size)
{
	struct stat sb;
	schar forMat[ELOG_FILENAME_MAX + 1];
	slong ret = ELOG_OK;
	slong serno = 1;
	bool_t flag = TRUE;
	ulong len;
	DEBUG_TRCIN();

	if(filePath == NULL)
	{
		DLOG_Error("filePath is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		if(braPath == NULL)
		{
			DLOG_Error("braPath is NULL.\n");
			DEBUG_TRCOUT();
			return ELOG_E_PARAM;
		}
		else
		{
			/*Do nothing*/
		}
	}

	/* 指定ファイルの有無チェック */
	memset(&sb, 0x0, sizeof(sb));
	ret = stat(filePath, &sb);
	if(ret != 0 && errno == ENOENT)
	{
		/* ファイルなし */
		ret = 0;
	}
	else
	{
		if(braPath == NULL)
		{
			DLOG_Error("braPath is NULL.\n");
			ret = ELOG_E_PARAM;
		}
		else
		{
			/* 枝番付きファイル名のsnprintfフォーマット取得 */
			memset(forMat, 0x0, sizeof(forMat));
			ret = elog_GetSnoFileFormat(filePath, forMat, sizeof(forMat));
			if(ret != ELOG_OK)
			{
				DLOG_Error("elog_GetSnoFileFormat() error.\n");
				ret = ELOG_E_PROCS;
			}
			else
			{
				/* バッファサイズ確認 */
				len = strnlen(forMat, sizeof(forMat));
				if(size < len)
				{
					DLOG_Error("size is not enogh.\n");
					ret = ELOG_E_PARAM;
				}
				else
				{
					/*Do nothing*/
				}
			}
		}
		if(ret == ELOG_OK)
		{
			/* 枝番をインクリメントしながら重複確認 */
			for(serno = 1; (serno < 1000) && (flag == TRUE); serno++)
			{
				memset(braPath, 0x0, size);
				snprintf(braPath, size-1, forMat, serno);
				ret = stat(braPath, &sb);
				if(ret != 0 && errno == ENOENT)
				{
					flag = FALSE;
					serno--;
				}
				else
				{
					/*Do nothing*/
				}
			}
			ret = serno;
			if(serno == 1000)
			{
				/* 枝番が上限に達したらエラーとする */
				ret = ELOG_E_PROCS;
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
/*    Description    :枝番号付きファイル名書式を取得                             */
/*----------------------------------------------------------------------------*/
/*    param          :schar *filePath : 元ファイル名                           */
/*----------------------------------------------------------------------------*/
/*                   :schar *forMat : 枝番号付きファイル名返却バッファ           */
/*----------------------------------------------------------------------------*/
/*                   :ulong size : 返却バッファサイズ                           */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    サブスレッド未実行                          */
/*                                                                            */
/*                    ELOG_E_PARAM(-1)    パラメータエラー                      */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static slong elog_GetSnoFileFormat(schar *filePath, schar *forMat, ulong size)
{
	schar path[ELOG_FILENAME_MAX + 1];
	schar *ext = NULL;
	ulong len;
	slong ret = ELOG_OK;
	DEBUG_TRCIN();

	if(filePath == NULL)
	{
		DLOG_Error("filePath is NULL.\n");
		DEBUG_TRCOUT();
		return ELOG_E_PARAM;
	}
	else
	{
		if(forMat == NULL)
		{
			DLOG_Error("forMat is NULL.\n");
			DEBUG_TRCOUT();
			return ELOG_E_PARAM;
		}
		else
		{
			/*Do nothing*/
		}
	}


	/* 指定ファイルパスをコピーして拡張子を取り除く */
	memset(path, 0x0, sizeof(path));
	strncpy(path, filePath, sizeof(path)-1);

	/* バッファサイズ確認 */
	len = strnlen(path, sizeof(path));
	if(size < len+6)
	{	/* filePath+"-%03d"+1 */
		DLOG_Error("size is not enogh.\n");
		ret = ELOG_E_PARAM;
	}
	else
	{
		/* コピーから拡張子を除去 */
		ext = rindex(path, '.');
		if(ext != NULL)
		{
			*ext = '\0';
		}
		else
		{
			/*Do nothing*/
		}
		/* 指定ファイルの拡張子位置を取得 */
		ext = rindex(filePath, '.');

		/* パス名のフォーマットを作成 */
		if(ext != NULL)
		{
			snprintf(forMat, size-1, "%s-%%03d%s", path, ext);
		}
		else
		{
			snprintf(forMat, size-1, "%s-%%03d", path);
		}
	}

	DEBUG_TRCOUT();	
	return ret;
}


/*============================================================================*/
/*    Description    :获取文件内容                                             */
/*----------------------------------------------------------------------------*/
/*    param          :time_t timep:时间戳                                      */
/*----------------------------------------------------------------------------*/
/*                   :schar *infoData:通知日志时间戳内的最新数据                */
/*----------------------------------------------------------------------------*/
/*                   :schar *number:获取到的数据条数                           */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    サブスレッド未実行                          */
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*                    ELOG_E_SYSCAL(-2)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/23  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong elog_GetFileLogData(time_t timep, schar *infoData, schar *number)
{
	FILE *fp;
	sint ret;
	slong tmpRet = ELOG_OK;
	bool_t flag = TRUE;
	schar filePath[ELOG_FILEPATH_MAX + 1];
	schar buf[ELOG_DATE_STR_MAX + 1];
	schar dateBuf[ELOG_DATE_STR_MAX + 1];
	struct tm ltm;
	struct tm *rtm;
	ulong len = 0u;
	DEBUG_TRCIN();

	rtm = localtime_r(&timep, &ltm);
	if(NULL == rtm)
	{
		tmpRet = ELOG_E_PROCS;
		flag = FALSE;
	}
	else
	{
		snprintf(dateBuf, sizeof(dateBuf), "%4d/%02d/%02d %02d:%02d:%02d",
							rtm->tm_year + 1900, rtm->tm_mon + 1, rtm->tm_mday,
							rtm->tm_hour, rtm->tm_min, rtm->tm_sec);

		memset(filePath, 0x0, sizeof(filePath));
		snprintf(filePath, sizeof(filePath)-1, "%s/%s",
				ELOG_ERR_EVENT_DIR, ELOG_INFOR_FNAME_HD);
	}
	if(flag)
	{
		fp = ELOG_FOPEN(filePath, "r");
		if(fp == NULL)
		{
			DLOG_Error("ELOG_FOPEN(%s) error.(%d)\n", filePath, errno);
			DEBUG_TRCOUT();
			tmpRet = ELOG_E_SYSCAL;
		}
		else
		{
			while(fgets(buf, sizeof(dateBuf), fp) != NULL)
			{
				if(0 <= strncmp(buf, dateBuf, sizeof(dateBuf)))
				{
					memcpy(infoData + len, buf, strlen(buf));
					len += strlen(buf);
					(*number)++;
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
	ELOG_FCLOSE(fp);
	DEBUG_TRCOUT();
	return tmpRet;
}


/*============================================================================*/
/*    Description    :information文件清除                                      */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    サブスレッド未実行                          */
/*                    ELOG_E_PROCS(-3)    処理エラー                           */
/*                    ELOG_E_SYSCAL(-2)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/27  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_InforFileClear(void)
{
	FILE *fp;
	char filePath[ELOG_FILEPATH_MAX + 1];
	char buf[ELOG_LINE_MAX] = {0};
	char timeBuf[ELOG_DATE_STR_MAX + 1];
	char fileCmd[ELOG_FILEPATH_MAX];
	slong line_num = 0;
	slong ret = 0;
	struct tm ltm;
	struct tm *rtm;
	DEBUG_TRCIN();

	rtm = localtime_r(&time, &ltm);
	if(NULL == rtm)
	{
		ret = ELOG_E_PROCS;
	}
	else
	{
		/*Do nothing*/
	}

	memset(timeBuf, 0x0, sizeof(timeBuf));
	snprintf(timeBuf, sizeof(timeBuf) - 1, "%4d/%02d/%02d %02d:%02d:%02d",
				rtm->tm_year, rtm->tm_mon, rtm->tm_mday,
				rtm->tm_hour, rtm->tm_min, rtm->tm_sec);

	memset(filePath, 0x0, sizeof(filePath));
	snprintf(filePath, sizeof(filePath)-1, "%s/%s.txt", 
	ELOG_ERR_EVENT_DIR, ELOG_INFOR_FNAME_HD);
	fp = ELOG_FOPEN(filePath, "r");
	if(fp == NULL)
	{
		DLOG_Error("ELOG_FOPEN(%s) error.(%d)\n", filePath, errno);
		ret = ELOG_E_SYSCAL;
	}
	else
	{
		while(fgets(buf, ELOG_LINE_MAX, fp))
		{
			ret = strncmp(timeBuf, buf, ELOG_COMP_SIZE);
			if(ret < 0)
			{
				break;
			}
			line_num++;
		}
		memset(fileCmd, 0x0, sizeof(fileCmd));
		snprintf(fileCmd, sizeof(fileCmd)-1, "sed -i '1d' %s", filePath);
		CMN_MutexLock(&s_elogSysCmdMutx);
		ELOG_CALL_SYSTEM(fileCmd);
		CMN_MutexUnlock(&s_elogSysCmdMutx);
	}


	DEBUG_TRCOUT();
	return ret;

}
/*============================================================================*/
/*    Description    :线程取消函数                                             */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/13  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_PthreadCancel(ElogThrCancel_t* param)
{
	FRM_PthreadCancel(param->id);
}
/*============================================================================*/
/*    Description    :获取新增通知数量                                          */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/05/09  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
ulong ELOG_GetInfoListNum(void)
{
	return s_elogInforSendMsg.elogInformNum;
}
/*============================================================================*/
/*    Description    :起動時ログディレクトリ作成関数                             */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/20  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
static void elog_InitMkdir(void)
{
	DEBUG_TRCIN();
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_LOG_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_ERR_EVENT_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_SDP_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_LOG_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_OPERATION_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_SDZ_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_NDL_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_FW_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_ACTION_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_ACTIONZ_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_POW_RIS_EDGE_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_SOH_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_HEMSHISTORY_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_POWER_SOURCE_OUT_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_POWER_SOURCE_IN_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_GENERATE_OUT_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_POWER_DEAL_DAY_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_UPDATE_RESULT_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_OUTPUTCTRL_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_POWER_DEAL_DAY_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_POWER_DEAL_MONTH_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_POWER_DEAL_YEAR_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_ACCUMULATOR_DAY_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_ACCUMULATOR_MONTH_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_ACCUMULATOR_YEAR_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_VEHICLE_DAY_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_VEHICLE_MONTH_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_VEHICLE_YEAR_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_OPT_INFORM_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_TMP_SDP_DIR);
	ELOG_CALL_SYSTEM("mkdir -p "ELOG_E_SDP_LOG_DIR);
	DEBUG_TRCOUT();
}
/* End Of File */