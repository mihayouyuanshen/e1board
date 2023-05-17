/****************************************************************************/
/*	Description		:ログ管理ミドル Function0 (WDT応答) ヘッダ				*/
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
#ifndef __ELOG_FNC_H__
#define __ELOG_FNC_H__

#include "typedef.h"
#include "elog_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define ELOG_OP_MSG_SIZE		((ulong)1024u)		/* 手机app数据传输最大值 */
/*命令要素*/
#define	ELOG_CMDDATEYEAR_LEN	((uchar)3u)
#define	ELOG_CMDDATEMONTH_LEN	((uchar)3u)
#define	ELOG_CMDDATEDAY_LEN		((uchar)3u)
#define	ELOG_SYSMADENUM_LEN		((uchar)17u)
#define	ELOG_BTMADENUM_LEN		((uchar)17u)

/* FW更新ファイルディレクトリ 	*/
#define ELOG_FW_DIR				"/mnt/log/upgrade"
/* FW更新ファイルディレクトリ(/mnt/user配下) */
#define ELOG_USR_FW_DIR			"/mnt/user/upgrade"


#define ELOG_KB(n)				((n)*1024)				/* キロバイト */
#define ELOG_MB(n)				(ELOG_KB(n)*1024)		/* メガバイト */
#define ELOG_RAM_DIR			"/mnt/ram"				/* ログディレクトリ(RAM) */

#define ELOG_LOG_TIMER 			((ulong)(60*60*1000))	/* 1h转换[ms] */

/***** キー操作ログ関連 *****/
/* キーログファイル名先頭 */
#define ELOG_OPERATION_FNAME_HD	"operation"
/* キー操作ログファイルLINE数上限 */
#define	ELOG_OPERATION_FILES_LINE_LIM	((ushort)1000u)

/***** SOHログ関連 *****/
/* キーログディレクトリ */
#define ELOG_SOH_DIR			"/mnt/log/soh"
/* キーログファイル名先頭 */
#define ELOG_SOH_FNAME_HD		"soh_info_v2"
/* キー操作ログファイルLINE数上限 */
#define	ELOG_SOH_FILES_LINE_LIM	((uchar)100u)

/***** pcs and E1 errorログ *****/
/* お知らせログファイル名先頭 */
#define ELOG_SERVICE_FNAME_HD	"service"
/* サービスログファイル名先頭 */
#define ELOG_INFOR_FNAME_HD		"information"
#define ELOG_LINE_MAX					((uchar)64u)
#define ELOG_COMP_SIZE					((uchar)19u)
/* お知らせログファイルLINE数上限 */
#define ELOG_SERVICE_FILES_LINE_LIM		((ushort)1000u)
/* サービスログファイルLINE数上限 */
#define ELOG_INFOR_FILES_LIM			((uchar)100u)
/* error code 个数 */
#define ELOG_MAX_PCS_ERR_CODE_NUM		((uchar)4u)
/* E1 error code 个数 */
#define ELOG_MAX_E1_ERR_CODE_NUM		((uchar)29u)
/* 比对error code长度 */
#define ELOG_MAX_COMP_ERR_CODE_NUM		((uchar)5u)
/* 错误等级长度 */
#define ELOG__ERR_LEVEL_LEN				((uchar)2u)

/***** power rising edgeログ *****/
/* サービスログファイル名先頭 */
#define ELOG_POW_RIS_EDGE_FNAME_HD		"volt_hist"
/* キーログディレクトリ */
#define ELOG_POW_RIS_EDGE_DIR			"/mnt/log/power_risingedge"
/* サービスログファイルLINE数上限 */
#define	ELOG_POW_RIS_EDGE_FILES_LIM		((uchar)100u)
/***** HEMS Power edgeログ *****/
/* サービスログファイル名先頭 */
#define ELOG_HEMS_HISTORY_FNAME_HD		"hemshistory"
/* キーログディレクトリ */
#define ELOG_HEMSHISTORY_DIR			"/mnt/log/hems_power"
/* サービスログファイルLINE数上限 */
#define	ELOG_HEMS_HISTORYFILES_LIM		((uchar)100u)

/***** 分布式电源 *****/
/* 分布式电源AC累计输出Log保存路径 */
#define ELOG_POWER_SOURCE_OUT_DIR		"/mnt/log/power_src_out"
/* 分布式电源AC累计输出Log保存文件头*/
#define ELOG_POWER_SOURCE_OUT_FNAME_HD	"powerSrcOut"
/*分布式电源AC累计输入Log保存路径 */
#define ELOG_POWER_SOURCE_IN_DIR		"/mnt/log/power_src_in"
/* 分布式电源AC累计输入Log保存路径*/
#define ELOG_POWER_SOURCE_IN_FNAME_HD	"powerSrcIn"
/* 分布式发电单机AC累计输出Log保存路径*/
#define ELOG_GENERATE_OUT_DIR			"/mnt/log/generate_out"
/* 分布式发电单机AC累计输出Log保存路径*/
#define ELOG_GENERATE_OUT_FNAME_HD		"generateOut"
/* 分布式AClogファイルLINE数上限 */
#define	ELOG_AC_FILES_LIM				((uchar)99u)

/***** 出力制御操作ログ関連 *****/
/* 出力制御ログディレクトリ */
#define ELOG_OUTPUTCTRL_DIR				"/mnt/log/outputctrl"
/* 出力制御ログファイル名先頭 */
#define ELOG_OUTPUTCTRL_FNAME_HD		"daily_sch_"
/* 出力制御ログファイル数上限 */
#define	ELOG_OUTPUTCTRL_FILES_LIM		((ulong)3700u)

/***** 发电/买卖电ログ関連 *****/
/* 日間 ログディレクトリ */
#define ELOG_POWER_DEAL_DAY_DIR			"/mnt/user/egra/pv/day"
/* 月間 ログディレクトリ */
#define ELOG_POWER_DEAL_MONTH_DIR		"/mnt/user/egra/pv/month"
/* 年間 ログディレクトリ */
#define ELOG_POWER_DEAL_YEAR_DIR		"/mnt/user/egra/pv/year"
/* ログファイル名先頭 */
#define ELOG_POWER_DEAL_FNAME_HD		"pv_"

/***** 蓄电池充放电ログ関連 *****/
/* 日間 ログディレクトリ */
#define ELOG_ACCUMULATOR_DAY_DIR		"/mnt/user/egra/bt/day"
/* 月間 ログディレクトリ */
#define ELOG_ACCUMULATOR_MONTH_DIR		"/mnt/user/egra/bt/month"
/* 年間 ログディレクトリ */
#define ELOG_ACCUMULATOR_YEAR_DIR		"/mnt/user/egra/bt/year"
/* ログファイル名先頭 */
#define ELOG_ACCUMULATOR_FNAME_HD		"bt_"

/***** 车辆充放电ログ関連 *****/
/* 日間 ログディレクトリ */
#define ELOG_VEHICLE_DAY_DIR			"/mnt/user/egra/v2h/day"
/* 月間 ログディレクトリ */
#define ELOG_VEHICLE_MONTH_DIR			"/mnt/user/egra/v2h/month"
/* 年間 ログディレクトリ */
#define ELOG_VEHICLE_YEAR_DIR			"/mnt/user/egra/v2h/year"
/* ログファイル名先頭 */
#define ELOG_VEHICLE_FNAME_HD			"v2h_"
/* 日間 ログファイル数上限 */
#define	ELOG_DAY_FILES_LIM				((ushort)3700u)
/* 月間 ログファイル数上限 */
#define	ELOG_MONTH_FILES_LIM			((uchar)122u)
/* 年間 ログファイル数上限 */
#define	ELOG_YEAR_FILES_LIM				((uchar)11u)

/***** 下载ログ*****/
/* updateログディレクトリ */
#define ELOG_UPDATE_RESULT_DIR			"/mnt/log/update"
/* updateログファイル名先頭 */
#define ELOG_UPDATE_RESULT_FNAME_HD		"Update_Result_"
/* updateログディレクトリ */
#define ELOG_OPT_INFORM_DIR				"/mnt/log/opt_information"
/* updateログファイル名先頭 */
#define ELOG_OPT_INFORM_FNAME_HD		"e1operationstate"

/***** SDPログ*****/
/* 存放十分钟的临时sdp log路径 */
#define ELOG_TMP_SDP_DIR				"/mnt/log/tmpsdp"
/* 对sdp的十分钟log路径 */
#define ELOG_E_SDP_LOG_DIR				"/mnt/sdplog"
/* SDPログファイル名先頭 */
#define ELOG_SDP_FNAME_HD				"SDP"

/* NANDログ削除判定(5%) */
#define	ELOG_NAND_UNDR			((double)(0.05))
/* FW更新ファイル保持用のマージン(MB) */
#define	ELOG_NAND_FW_MARGINE	((uchar)128u)
/* FW更新ファイル数上限(ディレクトリ走査時のループ上限) */
#define	ELOG_NAND_FW_NUM_MAX	((uchar)64u)

/* USB保存関連 */
/* USB保存の1秒あたりの進捗バイト数 */
#define ELOG_USBSAVE_RATE		ELOG_KB((uchar)(63u))

/* 読み込み専用ファイル最大サイズ(90MB) */
#define ELOG_RS_FILESIZE		((uchar)90u)
/* 解凍時の増加係数 */
#define ELOG_RS_INC_COFE		((double)(1.3))
/* 読み込み専用ファイル書き込みレート */
#define ELOG_RS_WRITERATE		ELOG_KB((uchar)(28u))

/* 運転ログ・圧縮動作ログ枝番 */
#define	ELOG_SUBNO_DIGITS		((uchar)3u)	/* 桁数 */
/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
/*pcs错误码类别*/
typedef enum{
	ELOG_PCS_NO_ERRCODE,						/*没有错误发生*/
	ELOG_PCS_SECRET_ERRCODE,					/*隐秘错误发生*/
	ELOG_PCS_NORMAL_ERRCODE,					/*正常错误发生*/
	ELOG_PCS_OTHER_ERRCODE						/*其他错误码*/
}ElogPcsErrCode_t;


typedef struct ElogMemoryInfo{
		ulong avail;
		ulong used;
		ulong all;
}ElogMemoryInfo_t;

typedef enum{
	ELOG_SAVE_FILE_ONLY_SERVICE,	/*仅保存在服务log中		*/
	ELOG_SAVE_FILE_DOUBLE,			/*服务log和通知log都保存*/
	ELOG_SAVE_FILE_NOWRITE			/*不保存在文件中		*/
}ElogSaveFile_t;
/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/
/*手机端获取数据结构体*/
typedef struct ElogInformSendMsg{
	schar msg[ELOG_OP_MSG_SIZE];
	uchar elogInformNum;
}ElogInformSendMsg_t;

typedef struct ElogCmdDate{
	schar year[ELOG_CMDDATEYEAR_LEN];				/* 年（“00”～“99”）*/
	schar month[ELOG_CMDDATEMONTH_LEN];				/* 月（“01”～“12”）*/
	schar day[ELOG_CMDDATEDAY_LEN];					/* 日（“01”～“31”）*/
}ElogCmdDate_t;

typedef struct ElogCmd60{
	schar	sysMadeNum[ELOG_SYSMADENUM_LEN];	/* システム製造番号 */
	ushort	btRateEltc;							/* 蓄電池定格電力量 */
	ushort	pcsRateOut;							/* PCS定格出力 */
	ushort	pcsRateVol;							/* PCS定格電圧 */
	ushort	pcsRateVolAlone;					/* PCS定格電圧_自立時 */
	ElogCmdDate_t	setDate;					/* 設置日 */
	schar	invMcuFw;							/* INVマイコン_FW */
	schar	pvMcuFw;							/* PVマイコン_FW */
	schar	bdcMcuFw;							/* BDCマイコン_FW */
	schar	jbFw;								/* J.B._FW */
	schar	invMcuBootLoader;					/* INVマイコン_ブートローダー */
	schar	pvMcuBootLoader;					/* PVマイコン_ブートローダー */
	schar	bdcMcuBootLoader;					/* BDCマイコン_ブートローダー */
	uchar	sysStruct;							/* システム構成 */
	schar	btMadeNum[ELOG_BTMADENUM_LEN];		/* 蓄電池ユニット製造番号 */
	ushort	btRatedVol;							/* 蓄電池定格電圧 */
	ulong	btMaxChargePwr;						/* 蓄電池最大充電電力値 */
	ulong	btMaxDischarPwr;					/* 蓄電池最大放電電力値 */
	ulong	btMaxChargePwrIndep;				/* 蓄電池最大充電電力値_自立時 */
	ulong	btMaxDischarPwrIndep;				/* 蓄電池最大放電電力値_自立時 */
	uchar	sysFrequency;						/* 系統周波数(50Hz or 60Hz) */
}ElogCmd60_t;

typedef struct{
	double pvGenPwr2;
	double pvGenPwr;
	double allPwrOut;
	double allPwrIn;
	double carRemainingCapacity3;
	double LastDayPvGenPwr2;
	double LastDayPvGenPwr;
	double LastDayallPwrOut;
	double LastDayallPwrIn;
	double DaySumPvGenPwr2;
	double DaySumPvGenPwr;
	double DaySumallPwrOut;
	double DaySumallPwrIn;
	double MonthSumPvGenPwr;
	double MonthSumPvGenPwr2;
	double MonthSumallPwrOut;
	double MonthSumallPwrIn;
	slong tmYear;
	slong tmMon;
	slong tmMday;
}ElogPowerDiff_t;

typedef struct{
	ElogTimerType_t type;
	schar *fileHead;
	schar *path;
	ulong fileLimNum;
	ulong pathSize;
}ElogFileNum_t;

typedef struct{
	schar E1ErrCode[ELOG_MAX_COMP_ERR_CODE_NUM];
	schar E1ErrLevel[ELOG__ERR_LEVEL_LEN];
	bool_t E1Infor;
	bool_t E1Service;
}ElogE1ErrCode_t;

typedef struct{
	ElogDeleteFileType_t type;
	schar *path;
	schar *fileHead;
}ElogDeleteFile_t;


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
static slong elog_CheckAndDeleteNandLog(void);
static slong elog_DirentAddCopy(
				struct dirent ***list, ulong size,
				struct dirent **addList, ulong addSize, schar *path);
static slong elog_DirentFilterFiles(const struct dirent *ent);
static slong elog_DirentFilterDirs(const struct dirent *ent);
static slong elog_DirFilterLog(const struct dirent *ent);
static slong elog_DirentFilterPcsInfoFiles(const struct dirent *ent);
static slong elog_DirentFilterOperatFiles(const struct dirent *ent);
static slong elog_DirentFilterPcsServFiles(const struct dirent *ent);
static slong elog_DirentFilterSohFiles(const struct dirent *ent);
static slong elog_DirentFilterVolHistFiles(const struct dirent *ent);
static slong elog_DirentFilterHEMSFiles(const struct dirent *ent);
static slong elog_DirFilterOutputCtl(const struct dirent *ent);
static slong elog_DirentFilterNdlFiles(const struct dirent *ent);
static slong elog_DirentFilterExTemps(const struct dirent *ent);
static slong elog_GetUpgradDirSize(ulong *used);
static ElogType_t elog_whatTypeLog(schar *fileName);
static void *elog_SubThreadTimer(void *arg);
static void *elog_SubCreateSdp(void *arg);
static void *elog_SubLineSize(void *arg);
static slong elog_CheckFileDateAndDeleteOld(schar *currDir,
				ElogFileNumType_t type, slong limFileNum);
static slong elog_MoveFile(schar *oldPath, schar *newPath);
static slong elog_CheckCurrentAndDeleteOld(
				schar *currDir, ulong lineNum, ulong limNum,
				ElogType_t type, schar *fName);
static slong elog_CopyFile(schar *srcPath, schar *dstPath, bool_t erase);
static slong elog_CheckSameFileName(schar *filePath,
				schar* braPath, ulong size);
static slong elog_GetSnoFileFormat(schar *filePath, schar *forMat, ulong size);
static void elog_CreateThread(void);
static void elog_InitMkdir(void);

#endif /*__ELOG_FNC_H__*/
