/****************************************************************************/
/*	Description		:ログ管理ミドル スレッド関連ヘッダ						*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Jiang.Z											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/05/09												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __ELOG_THR_H__
#define __ELOG_THR_H__
#include "elog_g.h"
#include <pthread.h>
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define ELOG_DAY_TO_SEC					((ulong)(24*60*60))
#define ELOG_HALF_HOUR_TO_SEC			((ushort)(30*60))
#define ELOG_HOUR_TO_SEC				((ushort)(60*60))
#define ELOG_FIVE_MIN_TO_SEC			((ushort)(5*60))
#define ELOG_TEN_MIN_TO_SEC				((ushort)(10*60))

/* NAND削除		: 00000100 */
#define	ELOG_THTYPE_NAND				(0x00000004)
/* PCS検索件数上限 */
#define ELOG_LOGSCAN_LIMIT				(ushort)(5000u)
#define ELOG_TYPE_NUM					(uchar)(4u)
/* ファイルパス最大長(終端'\0'含む) */
#define ELOG_MAX_FPATH			((ushort)256u)
/* format指定子用バッファサイズ */
#define ELOG_FORMAT_BUFSIZE		((uchar)16u)
/* ログ一覧ファイル名 */
#define ELOG_LOGLISTPATH		"/mnt/ram/loglist.txt"
/* 削除ファイル一覧ファイル名 */
#define ELOG_DELLISTPATH		"/mnt/ram/dellist.txt"
/* ダウンロードログ最大文字列長 */
/***********************************************************
* USB保存・ログ一覧除外ディレクトリ
************************************************************/
#define	ELOG_EXCLUDES_DIR_ENET			"mnt/user/enet"
/***********************************************************
* ログ一覧除外ディレクトリ
************************************************************/
#define	ELOG_EXCLUDES_DIR_EGRA			"mnt/user/egra"
#define	ELOG_EXCLUDES_DIR_EGRA_BT		ELOG_EXCLUDES_DIR_EGRA "/bt"
#define	ELOG_EXCLUDES_DIR_EGRA_PV		ELOG_EXCLUDES_DIR_EGRA "/pv"
#define	ELOG_EXCLUDES_DIR_EGRA_V2H		ELOG_EXCLUDES_DIR_EGRA "/v2h"
#define	ELOG_EXCLUDES_DIR_EEPC			"mnt/user/eepc"
#define	ELOG_EXCLUDES_DIR_EEPC_CER_0	ELOG_EXCLUDES_DIR_EEPC "/rtcer_0"
#define	ELOG_EXCLUDES_DIR_EEPC_CER_1	ELOG_EXCLUDES_DIR_EEPC "/rtcer_1"
/***********************************************************
* 読み込み専用ファイル対象ディレクトリ/ファイル
************************************************************/
#define	ELOG_RS_INCLUDE_AGUI			"/mnt/user/agui/batterystat"
#define	ELOG_RS_INCLUDE_EBKL			"/mnt/user/ebkl"
#define	ELOG_RS_INCLUDE_EBUZ			"/mnt/user/ebuz"
#define	ELOG_RS_INCLUDE_EEPC			"/mnt/user/eepc"
#define	ELOG_RS_INCLUDE_EGRA			"/mnt/user/egra"
#define	ELOG_RS_INCLUDE_EHEM			"/mnt/user/ehem"
#define	ELOG_RS_INCLUDE_ENSV			"/mnt/user/ensv"
#define	ELOG_RS_INCLUDE_EPCS			"/mnt/user/epcs/rs"
#define	ELOG_RS_INCLUDE_EENL			"/mnt/user/eenl"
#define	ELOG_RS_INCLUDE_ESCH			"/mnt/user/esch"
#define	ELOG_RS_INCLUDE_EPCS_PV			"/mnt/user/epcs/pvRmcSet.inf"
#define	ELOG_RS_INCLUDE_EPCS_MAINT		"/mnt/user/epcs/mnt.inf"
/***********************************************************
* 読み込み専用ファイル除外ディレクトリ/ファイル
************************************************************/
#define	ELOG_RS_EXCLUDE_EEPC_COMHIST	"mnt/user/eepc/eepc_ComHist.csv"
#define	ELOG_RS_EXCLUDE_ESCH_HEMSOH		"mnt/user/esch/esch_hemsOH"
#define	ELOG_RS_EXCLUDE_ESCH_HEMSOH_TGZ	"mnt/user/esch/HEMSOH-"
#define	ELOG_RS_EXCLUDE_ESCH_HEMSOH_LST	"mnt/user/esch/esch_HEMSOH_List.txt"
#define	ELOG_RS_EXCLUDE_ESCH_HEMSHIST	"mnt/user/esch/HEMSONOFFhistory.txt"
#define	ELOG_RS_EXCLUDE_ESCH_HS_NONVOL	"mnt/user/esch/esch_HS_NonVolatile.inf"

/* コマンド */
#define	ELOG_CMD_TAR			"tar czPf "			/* zip圧縮 */
#define	ELOG_CMD_RM				"rm -f "			/* 強制消去 */
#define ELOG_CMD_MD5SUM			"md5sum "			/* MD5チェックサム */
#define ELOG_CMD_BUFSIZE		((uchar)512u)		/* コマンドバッファサイズ */
#define	ELOG_NAND_RCOV			((double)(0.12))	/* NANDログ回復判定(12%) */
/* USB保存関連 */
/* 保存ファイル名 "YYYYMMDDhhmmss_LOG_SETTING.tgz" */
#define	ELOG_USBSAVE_NAME		"%s/%s/%s_LOG_SETTING.tgz"
#define ELOG_USBSAVE_DATELEN	((uchar)15u)		/* "YYYYMMDDhhmmss\0" */
#define ELOG_USBSAVE_CMD_LINE	((ushort)512u)
/*******************************************************************************
** ログ/設定値読み込み専用ファイル関連
*******************************************************************************/
/* ログ/設定値読み込み専用ファイル名の接頭辞 */
#define	CMN_LOG_SETTING_FILE_NAME_PREFIX		"t1t2setting_"
/* 読み込み専用ファイル先頭名称 */
#define ELOG_RS_NAME_TOP		CMN_LOG_SETTING_FILE_NAME_PREFIX
/* 読み込み専用MD5ファイル先頭名称 */
#define ELOG_RS_MD5_NAME_TOP	"md5_"ELOG_RS_NAME_TOP
/* 読み込み専用ファイル書式 */
#define ELOG_RS_NAME			"%s/"ELOG_RS_NAME_TOP"%s"
/* 読み込み専用MD5ファイル書式 */
#define ELOG_RS_MD5_NAME		"%s/"ELOG_RS_MD5_NAME_TOP"%s"
/* リモコンシリアルNo.ファイル名称 */
#define ELOG_RS_RMC_SERIAL		"rmc_serial_no"
/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
/* ローカル関数プロトタイプ */
void *elog_ThrUSBStart(void *arg);
void *elog_ThrCompressStart(void *arg);
void *elog_ThrDeleteNandLog(void *arg);
void *elog_ThrGetLogList(void *arg);
void *elog_ThrDeleteLog(void *arg);
void elog_SendStateEvent(ulong port, ElogUsbst_t state,
						uchar progress, ulong tTime);
static void elog_ThrUSBStartExit(
		ulong rId, ElogThdata_t *thData, ElogUsbst_t usbSt,
		uchar progress, ulong tTime, ElogThstate_t thrSt);
static void elog_SetUsbChild(pid_t id);
static pid_t elog_GetUsbChild(void);
static void elog_SetUsbProgress(uchar progress);
static uchar elog_GetUsbProgress(void);
static slong elog_ScanLogWithLimit(ElogType_t type, schar *date,
									ElogFileList_t *nameList, bool_t *flag);
static slong elog_GetLogFileDate(ElogType_t type,
								schar *FileName, schar **date);
static ulong elog_DirentFilterDateLimit(const struct dirent *ent);
static ulong elog_LogDateSort(const struct dirent **entA, 
							const struct dirent **entB);
static slong elog_SetLogDateLimit(ElogType_t type, schar *date);
/* ログリスト・ログ削除 */
static slong elog_SetFilterDateRange(ElogType_t type, 
									schar *begin, schar *end);
static ulong elog_DirentFilterDateRange(const struct dirent *ent);
static ulong elog_SortSvrLogList(const struct dirent **entA,
								const struct dirent **entB);
static slong elog_AlphaSort(const void *entA, const void *entB);
static slong elog_WriteListToFile(schar *fPath,
								struct dirent **list, ulong size);
static bool_t elog_ChkExclDirOThList(schar* d_name);
/* 読み込み専用ファイル作成 */
static ElogUsbst_t elog_SaveReadSetting(schar* rmc_serial);
static void elog_RsDeleteFile(void);
static slong elog_RsCreatMD5(schar* PcsSerial);
static slong elog_RsCreatRmcSerial(schar* rmc_serial);

#endif /*__ELOG_THR_H__*/
