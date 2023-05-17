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
#ifndef __ELOG_FNC_G_H__
#define __ELOG_FNC_G_H__

#include "typedef.h"
#include "elog_g.h"

/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/

#define ELOG_DATESTR_LEN		((uchar)8u)				/* "YYYYMMDD" */
#define ELOG_FILENAME_MAX		((uchar)16u)			/* 文件名字最大长度 */
#define ELOG_FILEPATH_MAX		((uchar)64u)			/* 文件路径最大长度 */
#define ELOG_DATE_STR_MAX		((uchar)32u)			/* 日期最大长度 */
#define ELOG_LOG_DIR			"/mnt/log/remocon"		/* ログディレクトリ(NAND) */
#define ELOG_LOG_FNAME_HD		"voltalog"				/* ログファイル名先頭 */
#define ELOG_ACTION_DIR			"/mnt/log/action"		/* 动作log路径 */
#define ELOG_ACTIONZ_DIR		"/mnt/log/actiontgz"	/* 压缩动作log路径 */
#define ELOG_ACZ_TIMER			((ulong)(24*60*60*1000))/* 一天转换成毫秒 */
/* 圧縮SDPログディレクトリ */
#define ELOG_SDZ_DIR					"/mnt/log/sdptgz"
/* キーログディレクトリ */
#define ELOG_OPERATION_DIR				"/mnt/log/operation"
/* errcode保存路径 */
#define ELOG_ERR_EVENT_DIR				"/mnt/log/errcode"
/* 30min转换[ms] */
#define ELOG_POWER_OUT_TIMER 			((ulong)(30*60*1000))

/* 30min转换[ms] */
#define ELOG_OUTPUTCTRL_TIMER 			((ulong)(30*60*1000))
/* 5min 转换[ms] */
#define ELOG_CNT_LOCK_TIM 				((ulong)(5*60*1000))
/* 60min转换[ms] */
#define ELOG_POWER_TIMER 				((ulong)(60*60*1000))
/***** SDPログ*****/
/* SDPログディレクトリ */
#define ELOG_SDP_DIR					"/mnt/log/sdp"
/* 一天转换[ms] */
#define ELOG_SDP_TIMER					((ulong)(24*60*60*1000))
/* 10min转换[ms] */
#define ELOG_TMP_SDP_FILE_TIMER			((ulong)(10*60*60*1000))
/***** ダウンロードログ関連 *****/
/* ダウンロードログディレクトリ */
#define ELOG_NDL_DIR					"/mnt/log/download"
/* マウントポイント */
/* USBマウントポイント */
#define ELOG_FS_USB						"/media/usb_storage"
/* ログFSパス */
#define ELOG_FS_LOG						"/mnt/log"
/* 設定FSパス */
#define ELOG_FS_USR						"/mnt/user"
/* MD5文字列長 */
#define ELOG_RS_MD5_LEN			((uchar)32u)
/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
typedef enum{
	ELOG_TIMER_LOG,						/*Debug Log文件更新		*/
	ELOG_TIMER_ACTION_ACZ,				/*蓄电池动作压缩		*/
	ELOG_TIMER_SOURCE_OUT,				/* 电源AC输出			*/
	ELOG_TIMER_SOURCE_IN,				/* 电源AC输入			*/
	ELOG_TIMER_GENERATE_OUT,			/* 发电单机输出			*/
	ELOG_TIMER_LOG_OUTPUTCTRL,			/* 出力制御ログ			*/
	ELOG_TIMER_PV,						/*发电/买卖电年的文件	*/
	ELOG_TIMER_BT,						/*蓄电池充放电日的文件	*/
	ELOG_TIMER_V2H,						/*车辆充放电日的文件	*/
	ELOG_TIMER_V2H_CNT_LUCK,			/*车辆充放电连接锁		*/
	ELOG_TIMER_TMP_SDP_FILE,			/*sdp文件定时			*/
	ELOG_TIMER_SDP_SDZ,					/*sdp文件压缩			*/
	ELOG_TIMER_USBSAVE_TMRID,
	ELOG_TIMER_TYPE_NUM_MAX				/*文件类型种类			*/
}ElogTimerType_t;

/***********************************************************
* @enum		ElogDateStrType_t
* @brief	ディレクトリ検索種別
************************************************************/
typedef enum{
	ELOG_DATESTR_LOG = 1,			/* "YYYYMMDDhhmmss"  形式	*/
	ELOG_DATESTR_DCS,				/* "YYYYMMDD-hhmmss" 形式	*/
	ELOG_DATESTR_Y_S,			/*YYYY/MM/DD HH:MM:SS		*/
	ELOG_DATESTR_OUTPUTCTRL,		/*YYYY/MM/DD HH:MM			*/
	ELOG_DATESTR_YMD,				/*YYYYMMDD					*/
	ELOG_DATESTR_OUPUTCTRL_SECTION,	/*YYYY/MM/DD HH:MM - HH:MM	*/
	ELOG_DATESTR_YM,				/*YYYYMM					*/
	ELOG_DATESTR_Y,					/*YYYY						*/
	ELOG_DATESTR_HMS,				/*HHMMSS					*/
	ELOG_DATESTR_UPDATE,			/*"YYYYMMDDhhmmss"形式		*/
} ElogDateStrType_t;

/***********************************************************
* @enum		ElogScndirSelect_t
* @brief	ディレクトリ検索種別
************************************************************/
typedef enum{
	ELOG_SCANDIR_DIRS = 1,			/* ディレクトリ検索 */
	ELOG_SCANDIR_FILES,				/* ファイル検索 */
	ELOG_SCANDIR_EXTMPS				/* ファイル検索（一時ファイル除外）*/
} ElogScndirSelect_t;

/*检查文件内条数的类型*/
typedef enum{
	ELOG_FILE_SIZE_SERVICE = 0,
	ELOG_FILE_SIZE_INFOR,
	ELOG_FILE_SIZE_OPERATION,
	ELOG_FILE_SIZE_SOH,
	ELOG_FILE_SIZE_POWER_VOLT_HIST,
	ELOG_FILE_SIZE_HEMS_HISTORY,
	ELOG_FILE_SIZE_SIZE_MAX
}ElogFileSizeType_t;

/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/
/***********************************************************
* @struct	ElogFileList_t
* @brief	ディレクトリエントリ配列
************************************************************/
typedef struct ElogFileList{
	struct dirent **list;	/* 配列のポインタ */
	ulong size;				/* 配列のサイズ   */
}ElogFileList_t;

typedef struct{
	ElogFileSizeType_t type;
	schar *fileHead;
	schar *path;
	ulong curlineNum;
	ulong fileLimNum;
	ulong pathSize;
}ElogFileSize_t;

typedef struct ElogCmd81{
	sshort	chargeDischargePwrAC;					/* PCS充放電電力（AC端計測値） */
	uchar	opeState;								/* PCS運転状態 */
	ulong	ttlChargeEnergySysAC;					/* 積算PCS充電電力量(AC端計測値)(系統連系時) */
	ulong	ttlDischargeEnergySysAC;				/* 積算PCS放電電力量(AC端計測値)(系統連系時) */
	ushort	rsoc;									/* rSOC */
	ushort	remainingBt;							/* 電池残量 */
	ushort	btVolt;									/* 電池電圧*/
	ushort	soh;									/* SOH */
	uchar	btOperation;							/* 蓄電池運転動作状態 */
	uchar	pwrCut;									/* 停電 */
	sshort	pvGenPwr;								/* PV発電電力(本製品以外のPV) */
	sshort	revPwrFlow;								/* 逆潮流電力 */
	ushort	sysVolt;								/* 系統電圧 */
	schar	pcsErr;									/* PCSエラー */
	uchar	pcsErrRating;							/* PCSエラーレーティング */
	uchar	opeMode;								/* 運転モード */
	sshort	chargeDischargePwrDC;					/* DC蓄電池充放電電力2(コンバータ部) */
	ushort	ttlChargeEnergyDC;						/* DC積算蓄電池充電電力量2(コンバータ部) */
	ushort	ttlDischargeEnergyDC;					/* DC積算蓄電池放電電力量2(コンバータ部) */
	ulong	ttlPvGenPwr;							/* 積算PV発電電力量(本製品以外のPV) */
	ulong	ttlPurchasePwr;							/* 積算買電電力量 */
	ulong	ttlSellingPwr;							/* 積算売電電力量 */
	ushort	GenPwrLimit;							/* 発電電力制限値(kW) */
	uchar	suppressState;							/* 出力抑制状態 */
	uchar	carConnectorLockState;					/* コネクタロック状態 */
	uchar	carCharDiscOpratState;					/* 車両充放電器運転動作状態 */
	ulong	carDischargeableRemainingCapacity1;		/* 車載電池の放電可能残容量1 */
	uchar	carDischargeableRemainingCapacity3;		/* 車載電池の放電可能残容量3 */
	ulong	carChargeableRemainCap;					/* 車載電池の充電可能残容量値 */
	uchar	carAvailabilityStatus;					/* 車両接続充放電可否状態 */
	slong	v2hInstantPower;						/* V2H瞬時充放電電力計測値(AC端計測値) */
	ulong	carTtlDischargePower;					/* 車載電池の積算放電電力量計測値(AC端計測値)(トータル) */
	ulong	carTtlChargePower;						/* 車載電池の積算充電電力量計測値(AC端計測値)(トータル) */
	uchar	emergencyMode;							/* 非常時運転モード */
	ulong	carRemainingCapacity1;					/* 車載電池の電池残容量1 */
	uchar	carRemainingCapacity3;					/* 車載電池の電池残容量3 */
	ulong	ttlChargeEnergyAloneAC;					/* 積算PCS充電電力量(AC端計測値)(自立時) */
	ulong	ttlDischargeEnergyAloneAC;				/* 積算PCS放電電力量(AC端計測値)(自立時) */
	ushort	pcsEfficiencyChar;						/* PCS効率(充電方向) */
	ushort	pcsEfficiencyDischar;					/* PCS効率(放電方向) */
	slong	btCharDischarPwr;						/* 蓄電池充放電電力(AC端計測値) */
	ushort	pvPwr2;									/* 太陽光発電電力(AC端計測値) */
	ulong	ttlPvGenPwr2;							/* 積算太陽光発電電力量(AC端計測値) */
	ushort	cellVoltMIN;							/* 最低セル電圧 */
	ushort	cellAverageVolt;						/* 平均セル電圧 */
	ushort	cellVoltMAX;							/* 最高セル電圧 */
	sshort	cellTempeMIN;							/* 最低セル温度 */
	sshort	cellTempeMAX;							/* 最高セル温度 */
	ushort	pvStringVolt1;							/* 太陽光発電ストリング電圧(1) */
	ushort	pvStringVolt2;							/* 太陽光発電ストリング電圧(2) */
	ushort	pvStringVolt3;							/* 太陽光発電ストリング電圧(3) */
	ushort	pvStringVolt4;							/* 太陽光発電ストリング電圧(4) */
	ushort	pvStringVolt5;							/* 太陽光発電ストリング電圧(5) */
	uchar	testRunoutinfrom;						/* 試運転状態通知 */
	uchar	v2hOperationButton1;					/* V2H本体操作ボタン1 */
	uchar	v2hOperationButton2;					/* V2H本体操作ボタン2 */
	uchar	v2hOperationButton3;					/* V2H本体操作ボタン3 */
	uchar	connectCarTypeOrManufacturer;			/* 接続中の車種またはメーカー(特定車両判定) */
	uchar	notChargeFlag;							/* 充電不可フラグ */
	uchar	notDisChargeFlag;						/* 放電不可フラグ */
	ushort	revCtCurrentU;							/* 逆潮CTの電流値(U相) */
	ushort	revCtCurrentW;							/* 逆潮CTの電流値(W相) */
	ulong	carbtDiscCapacityValue1;				/* 車載電池の放電可能容量値1 */
	ulong	carbtUseCapacityValue1;					/* 車載電池の使用容量値1 */
	ulong	carbtCharCapacityValue;					/* 車載電池の充電可能容量値 */
	uchar	btCharCopltNotice;						/* 蓄電池充電完了通知 */
	uchar	btDischarCopltNotice;					/* 蓄電池放電完了通知 */
	uchar	maintenanceBtState;						/* メンテ電池状態 */
}ElogCmd81_t;


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern void ELOG_PthreadCancel(ElogThrCancel_t* param);
extern slong ELOG_FncLogOutput(ElogFncLog_t *param);
extern void ELOG_SyncSetLevel(ElogLv_t level,slong msgID);
extern slong ELOG_FncSetEnable(ElogFncOnOff_t *param);
extern slong ELOG_FncSaveUSB(schar* PcsSerial);
extern slong ELOG_FncAbortUSB(void);
extern slong ELOG_FncOperation(ElogFncOpt_t *param);
extern slong ELOG_FncSaveErrorEventLog(ElogFncErrEvent_t *param);
extern slong ELOG_FncSavePowerDealLog(ElogPowerDeal_t *param);
extern slong ELOG_FncCompressActionLog(ElogFncActionComp_t*msg);
extern slong ELOG_FncSaveSdpLog(ElogFncSdp_t *param);
extern slong ELOG_FncCompressSdpLog(ElogFncSdpComp_t *param);
extern slong ELOG_FncGetLogList(ElogFncGetLogList_t *param);
extern slong ELOG_FncGetFreeLogSpace(ElogFncFreeSpace_t *param, ulong *freeMB);
extern slong ELOG_FncDeleteLog(ElogFncDeleteLog_t *param);
extern void ELOG_FncDeleteFile(ElogFncDeleteFile_t *param);
extern slong ELOG_FncOutputCtrl(ElogFncOutPutCtrl_t *param);
extern slong ELOG_FncSetEnableAll(bool_t onOff);
extern bool_t ELOG_FncGetEnableAll(void);
extern slong ELOG_RunInformation65(ElogFncRunInfor65_t *param);
extern slong ELOG_RunInformation75(ElogFncRunInfor75_t *param);
extern slong ELOG_FncDownLoadLogOutput(ElogFncDownload_t* param);
extern void ELOG_DirentFree(struct dirent ***list, ulong size);
extern slong ELOG_DeleteOldFiles(schar *path, ulong limit,
								ulong delnum, ElogType_t type);
extern slong ELOG_GetDStr(schar *buf, ulong size,
						ElogDateStrType_t type , slong timer);
extern void	ELOG_SaveLogRamToNand(void);
extern slong ELOG_FncSaveActionLog(ElogFncAction_t *param);
extern slong ELOG_FncSOHpercentage(ElogFncSOH_t *param);
extern slong ELOG_FncPowerVoltHist(ElogFncVoltHist_t *param);
extern slong ELOG_FncHEMSPowerEdge(ElogFncHemsHistory_t *param);
extern slong ELOG_FileDateComp(const void *entA, const void *entB );
extern slong ELOG_ScanFiles(
				schar *path,
				ElogFileList_t *nameList,
				ElogScndirSelect_t select);
extern slong ELOG_GetPartitionInfoMB_All(schar *path, ulong *avail,
										ulong *used, ulong *all);
extern slong ELOG_GetPartitionInfoMB(schar *path, ulong *avail,
									ulong *used, ulong *all);
extern slong ELOG_GetDirSize(ulong *used);
extern slong ELOG_GetFileDate(schar *FileName, schar **date);

extern slong ELOG_PcsLogGetFileDate(schar *FileName, schar **date);
extern slong ELOG_SdpLogGetFileDate(schar *FileName, schar **date);
extern slong ELOG_SvrLogGetFileDate(schar *FileName, schar **date);
extern slong ELOG_SdzLogGetFileDate(schar *FileName, schar **date);

#endif /*__ELOG_FNC_G_H__*/