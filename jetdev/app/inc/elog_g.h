/****************************************************************************/
/*	Description		:ログ管理ミドル											*/
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
#ifndef __ELOG_G_H__
#define __ELOG_G_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "typedef.h"
#include "frm_g.h"
#include "dlog_g.h"
#include "esys_g.h"


/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/

#define ELOG_NDL_STRMAX					((ushort)128u)
#define ELOG_SERIES_LEN					((uchar)16u)
#define ELOG_SERIES_NINE_LEN			((uchar)9u)
#define ELOG_FILE_LINE_SIZE_MAX			((ushort)128u)
/*要素*/
#define ELOG_FNC_STR					((ushort)512u)
#define ELOG_PCS_SERIAL_NUM_LEN			((uchar)32u)
#define ELOG_OPT_MAX					((ushort)1024u)
#define ELOG_ERR_CODE					((uchar)6u)
#define ELOG_ERR_LEVEL					((uchar)2u)
#define	ELOG_ACTION_LOG_LEN				((ushort)3001u)
#define	ELOG_ACTION_LOG_COMP_LEN		((ushort)2048u)
#define	ELOG_SDP_LOG_COMP_LEN			((ushort)2048u)
#define	ELOG_GET_LOG_LIST				((uchar)9u)
#define	ELOG_DELETE_LOG_LIST			((uchar)9u)
#define ELOG_COMP_ACT_PATH				((ushort)256u)

/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
/***********************************************************
* @enum		ElogFunction_t
* @brief	ファンクションID定義
************************************************************/
typedef enum{
	ELOG_MSGID_START = 0,			/*创建线程				*/
	ELOG_LOG,
	ELOG_ASYNC_OPERATION,		/* キー操作通知				*/
	ELOG_ASYNC_ERR_EVENT_LOG,	/* PCS和通信板错误ログ保存	*/
	ELOG_ASYNC_SAVESDPLOG,		/* SDPログ保存				*/
	ELOG_SYNC_SAVEPCSLOG,		/* PCSログ保存（同期IF）	*/
	ELOG_ASYNC_ACTION_LOG,		/*蓄电池动作日志			*/
	ELOG_ASYNC_SOH_INFO_LOG,	/*soh推移度日志				*/
	ELOG_ASYNC_PWR_VOLT_HIST_LOG,/*电压上升抑制log			*/
	ELOG_ASYNC_HEMS_HISTORY_LOG,/*HEMS发电抑制值			*/
	ELOG_ASYNC_TOTAL_PWR,		/*电源累计					*/
	ELOG_ASYNC_PWR_DEAL_PWR,	/*发电/买卖电日志			*/
	ELOG_ASYNC_ACCUMULATOR_PWR,	/*蓄电池充放电				*/
	ELOG_ASYNC_VEHICLE_PWR,		/*车辆充放电				*/
	ELOG_ASYNC_PRESS_ACTION,	/* 动作日志圧縮				*/
	ELOG_ASYNC_DOWNLOAD_LOG,	/* 下载日志					*/
	ELOG_ASYNC_RUN_INFOR_CMD65_LOG,/* cmd65运行log			*/
	ELOG_ASYNC_RUN_INFOR_CMD75_LOG,/* cmd75运行log			*/
	ELOG_SYNC_ISTHEREUSB,		/* USB有無確認				*/
	ELOG_ASYNC_PRESSSDP,		/* SDPログ圧縮				*/
	ELOG_ASYNC_GETLOGLIST,		/* ログ一覧取得				*/
	ELOG_SYNC_GETLOGFREE,		/* ログ領域空き容量取得		*/
	ELOG_ASYNC_DELETELOG,		/* ログ削除要求				*/
	ELOG_ASYNC_DELETEFILE,		/* ファイル削除要求			*/
	ELOG_ASYNC_SETENABLEALL,		/* 全プロセスログOn/Off設定	*/
	ELOG_ASYNC_GETENABLEALL,		/* 全プロセスログOn/Off取得	*/
	ELOG_ASYNC_DOWNLOADLOG,		/* ダウンロードログ出力		*/
	ELOG_ASYNC_OUTPUTCTRL,		/* 出力制御ログ				*/
	ELOG_ASYNC_GETLOGDATA,		/* 							*/
	ELOG_ASYNC_CANCELTHR		/*线程取消					*/
}ElogFunction_t;

/***********************************************************
* @enum		ElogLv_t
* @brief	ログレベル定義
************************************************************/
typedef enum{
	ELOG_LV_FATAL = 0,			/* 致命的なレベル(続行不可)	*/
	ELOG_LV_ERROR,				/* エラー発生				*/
	ELOG_LV_NORMAL,				/* 通常処理					*/
	ELOG_LV_DEBUG,				/* デバッグ用途				*/
	ELOG_LV_VERBOSE				/* 冗長なログ				*/
}ElogLv_t;

#if 0
/***********************************************************
* @enum		ELOG_USBOP_E
* @brief	USBメモリ操作定義
************************************************************/
typedef enum{
	ELOG_USBOP_SAVELOG = 0,		/* ログの保存				*/
	ELOG_USBOP_LOADLOG,			/* ログの読み込み			*/
	ELOG_USBOP_SAVESETTING,		/* 設定値の保存				*/
	ELOG_UBSOP_LOADSETTING		/* 設定値の読み込み			*/
}ELOG_USBOP_E;
#endif

/***********************************************************
* @enum		ElogErr_t
* @brief	エラーログ定義
************************************************************/
typedef enum{
	ELOG_ERR_NORMAL = 0,		/* エラーなし				*/
	ELOG_ERR_READERROR,			/* 読み込みエラー			*/
	ELOG_ERR_WRITEERROR,		/* 書き込みエラー			*/
	ELOG_ERR_NOCAPCITY,			/* 容量不足エラー			*/
	ELOG_ERR_BUSY,				/* 動作中エラー				*/
	ELOG_ERR_NOENT,				/* 対象なしエラー			*/
	ELOG_ERR_PARAM,				/* パラメータエラー			*/
	ELOG_ERR_SYSCAL,			/* システムコールエラー		*/
	ELOG_ERR_OTHER				/* その他処理エラー			*/
}ElogErr_t;

/***********************************************************
* @enum		ElogEvent_t
* @brief	イベントID定義
************************************************************/
typedef enum{
	ELOG_EVT_USBPROGRESS= 1,	/* USB保存進捗通知			*/
	ELOG_EVT_COMPRESS,			/* 圧縮動作通知				*/
	ELOG_EVT_GETLOGLIST,		/* ログリスト取得動作通知	*/
	ELOG_EVT_DELETELOG			/* ログ削除動作通知			*/
}ElogEvent_t;

/***********************************************************
* @enum		ElogUsbst_t
* @brief	USB保存進捗状態
************************************************************/
typedef enum{
	ELOG_USBST_INPROGRESS = 0,	/* 保存中					*/
	ELOG_USBST_READERROR,		/* 読み込みエラー			*/
	ELOG_USBST_WRITEERROR,		/* 書き込みエラー			*/
	ELOG_USBST_ABORTED,			/* 中断						*/
	ELOG_USBST_COMPLETE			/* 保存完了					*/
}ElogUsbst_t;

/***********************************************************
* @enum		ElogUsbIs_t
* @brief	USB有無確認応答
************************************************************/
typedef enum{
	ELOG_USBIS_NOENT = 0,		/* USBなし					*/
	ELOG_USBIS_ISTHERE,			/* USB有り					*/
	ELOG_USBIS_ERROR			/* その他エラー				*/
}ElogUsbIs_t;

/***********************************************************
* @enum		ELOG_LOGTYPE_E
* @brief	ログ種別
************************************************************/
typedef enum {
	ELOG_TYPE_NON = 0,					/* 未定義			*/
	ELOG_TYPE_LOG,						/* プロセスログ		*/
	ELOG_TYPE_KEY,						/* キー通知ログ		*/
	ELOG_TYPE_SDP,						/* SDPログ			*/
	ELOG_TYPE_ACZ,						/* 压缩动作日志		*/
	ELOG_TYPE_SDZ,						/* 圧縮SDPログ		*/
	ELOG_TYPE_OPT_INFORM,				/* 运行信息			*/
	ELOG_TYPE_POWER_DEAL,				/* 发电/买卖电		*/
	ELOG_TYPE_SOH,						/* SOH				*/
	ELOG_TYPE_OTH						/* その他ログ		*/
} ElogType_t;

/***********************************************************
* @enum		ElogCompress_t
* @brief	ログ圧縮動作状態
************************************************************/
typedef enum{
	ELOG_COMPRESS_IDLE = 0,		/* 未実行					*/
	ELOG_COMPRESS_START,		/* 開始						*/
	ELOG_COMPRESS_END,			/* 終了						*/
	ELOG_COMPRESS_BUSY,			/* 処理不可					*/
	ELOG_COMPRESS_PRSERR,		/* 圧縮エラー				*/
	ELOG_COMPRESS_DELERR,		/* 削除エラー				*/
	ELOG_COMPRESS_OTHERR,		/* その他エラー				*/
	ELOG_COMPRESS_ENABLE		/* 圧縮要求許可				*/
}ElogCompress_t;

/***********************************************************
* @enum		ElogGetLogList_t
* @brief	ログリスト取得動作状態
************************************************************/
typedef enum{
	ELOG_GETLOGLIST_IDLE = 0,	/* 未実行					*/
	ELOG_GETLOGLIST_START,		/* 開始						*/
	ELOG_GETLOGLIST_END,		/* 終了						*/
	ELOG_GETLOGLIST_BUSY,		/* 処理不可					*/
	ELOG_GETLOGLIST_ERR,		/* 処理エラー				*/
	ELOG_GETLOGLIST_NOFILE		/* 対象ファイル無し			*/
}ElogGetLogList_t;
/***********************************************************
* @enum		ElogDeleteLog_t
* @brief	ログ削除動作状態
************************************************************/
typedef enum{
	ELOG_DELETELOG_IDLE = 0,	/* 未実行					*/
	ELOG_DELETELOG_START,		/* 開始						*/
	ELOG_DELETELOG_END,			/* 終了						*/
	ELOG_DELOG_BUSY,		/* 処理不可					*/
	ELOG_DELLOG_ERR,			/* 処理エラー				*/
	ELOG_DELETELOG_NOFILE		/* 対象ファイル無し			*/
}ElogDeleteLog_t;

typedef enum{
	ELOG_DIFF_PCS_ERR,
	ELOG_DIFF_E1_ERR,
	ELOG_DIFF_RMC,
	ELOG_DIFF_MAX
}ElogDiffErrType_t;



/*删除某路径下文件*/
typedef enum{
	ELOG_DELETE_FILE_INFORMATION = 0,	/*删除通知log		*/
	ELOG_DELETE_FILE_MAX
}ElogDeleteFileType_t;


typedef enum{
	ELOG_FILE_NUM_OUTPUTCTRL,
	ELOG_FILE_NUM_POWER_OUT,
	ELOG_FILE_NUM_SOURCE_IN,
	ELOG_FILE_NUM_GENERATE_OUT,
	ELOG_FILE_NUM_POWER_DEAL_YEAR,
	ELOG_FILE_NUM_POWER_DEAL_MONTH,
	ELOG_FILE_NUM_POWER_DEAL_DAY,
	ELOG_FILE_NUM_ACCUMULATOR_YEAR,
	ELOG_FILE_NUM_ACCUMULATOR_MONTH,
	ELOG_FILE_NUM_ACCUMULATOR_DAY,
	ELOG_FILE_NUM_VEHICLE_YEAR,
	ELOG_FILE_NUM_VEHICLE_MONTH,
	ELOG_FILE_NUM_VEHICLE_DAY,
	ELOG_FILE_NUM_UPDATE,
	ELOG_FILE_NUM_TYPE_NUM_MAX
}ElogFileNumType_t;


/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/

/***********************************************************
* @struct	ELOG_FNC01_T
* @brief	ログ出力 パラメータ
************************************************************/
typedef struct ElogFncLog{
	FrmMsgHead_t header;			/* メッセージヘッダ */
	ElogLv_t level;					/* ログレベル指定	*/
	uchar string[ELOG_FNC_STR];		/* ログの実体		*/
}ElogFncLog_t;
/***********************************************************
* @struct	ElogFncOnOff_t
* @brief	ログOn/Off設定・取得 パラメータ
************************************************************/
typedef struct ElogFncOnOff{
	FrmMsgHead_t header;			/* メッセージヘッダ	*/
	bool_t onOff;					/* ログのON/OFF		*/
	slong msgID;					/* 対象プロセスID	*/
}ElogFncOnOff_t;

/***********************************************************
* @struct	ElogFncUsbSave_t
* @brief	USB保存要求 パラメータ
************************************************************/
typedef struct ElogFncUsbSave{
	FrmMsgHead_t header;						/* メッセージヘッダ	*/
	schar PcsSerial[ELOG_PCS_SERIAL_NUM_LEN];	/* PCS製造番号		*/
}ElogFncUsbSave_t;

/***********************************************************
* @struct	ElogFncAbort_t
* @brief	USB保存中断要求 パラメータ
************************************************************/
typedef struct ElogFncAbort{
	FrmMsgHead_t header;			/* メッセージヘッダ	*/
}ElogFncAbort_t;

/***********************************************************
* @struct	ElogFncOpt_t
* @brief	キー操作通知 パラメータ
************************************************************/
typedef struct ElogFncOpt{
	FrmMsgHead_t header;			/* メッセージヘッダ		*/
	slong ButtonID;					/* 操作されたボタンID	*/
	slong PictureID;				/* 操作された画面のID	*/
	schar ObjectType;				/* 操作对象				*/
	schar opt[ELOG_OPT_MAX];		/*操作内容				*/
}ElogFncOpt_t;


/***********************************************************
* @struct	ElogFncErrEvent_t
* @brief	pcs和通信板错误ログ保存 パラメータ
************************************************************/
typedef struct ElogFncErrEvent{
	FrmMsgHead_t header;					/* メッセージヘッダ	*/
	schar s_beforErrCode[ELOG_ERR_CODE];	/* 变化前错误码		*/
	schar currErrCode[ELOG_ERR_CODE];		/* 变化后错误码		*/
	schar errLevel;							/*error code等级	*/
	ElogDiffErrType_t type;					/*error 种类		*/
}ElogFncErrEvent_t;

/***********************************************************
* @struct	ElogFncAction_t
* @brief	蓄电池动作日志
************************************************************/
typedef struct ElogFncAction{
	FrmMsgHead_t header;					/* メッセージヘッダ	*/
	schar actionLog[ELOG_ACTION_LOG_LEN];
}ElogFncAction_t;

/***********************************************************
* @struct	ElogFncSdpComp_t
* @brief	蓄电池动作ログ圧縮要求 パラメータ
************************************************************/
typedef struct ElogFncActionComp{
	FrmMsgHead_t header;						/* メッセージヘッダ		*/
	schar string[ELOG_ACTION_LOG_COMP_LEN];		/* 圧縮対象ファイル検索キー	*/
	bool_t del;									/* 削除要否				*/
}ElogFncActionComp_t;

/***********************************************************
* @struct	ElogFncSOH_t
* @brief	SOH健康度推移日志
************************************************************/
typedef struct ElogFncSOH{
	FrmMsgHead_t header;					/* メッセージヘッダ	*/
	ushort percentage;					/*soh推移度			*/
}ElogFncSOH_t;

/***********************************************************
* @struct	ElogFncVoltHist_t
* @brief	
************************************************************/
typedef struct ElogFncVoltHist{
	FrmMsgHead_t header;					/* メッセージヘッダ	*/
	schar state;							/*制御状态			*/
}ElogFncVoltHist_t;

/***********************************************************
* @struct	ElogFncAction_t
* @brief	hems发电抑制值
************************************************************/
typedef struct ElogFncHemsHistory{
	FrmMsgHead_t header;					/* メッセージヘッダ	*/
	ushort percentage;					/*hems发电抑制值	*/
	schar state;						/*制御状态			*/
}ElogFncHemsHistory_t;

/***********************************************************
* @struct	ElogFncPowerTotal_t
* @brief	分布式电源累计
************************************************************/
typedef struct ElogFncPowerTotal{
	FrmMsgHead_t header;					/* メッセージヘッダ	*/
	ulong PowerTotal;						/* 总电量累计		*/
	ElogFileNumType_t type;					/*电源不同的timerID	*/
}ElogFncPowerTotal_t;

/***********************************************************
* @struct	ElogFncOutPutCtrl_t
* @brief	出力制御ログ
************************************************************/
typedef struct ElogFncOutPutCtrl{
	FrmMsgHead_t header;				/* メッセージヘッダ	*/
	sshort percentage;					/* 	*/
}ElogFncOutPutCtrl_t;

/***********************************************************
* @struct	ElogPowerDeal_t
* @brief	发电/买卖电log
************************************************************/
typedef struct ElogPowerDeal{
	FrmMsgHead_t header;			/* メッセージヘッダ	*/
}ElogPowerDeal_t;

/***********************************************************
* @struct	ElogPowerDeal_t
* @brief	蓄电池充放电log
************************************************************/
typedef struct ElogAccumulator{
	FrmMsgHead_t header;			/* メッセージヘッダ	*/
}ElogAccumulator_t;

/***********************************************************
* @struct	ElogPowerDeal_t
* @brief	车辆充放电log
************************************************************/
typedef struct ElogVehicle{
	FrmMsgHead_t header;			/* メッセージヘッダ	*/
}ElogVehicle_t;

/***********************************************************
* @struct	ElogFncRunInfor
* @brief	运行log
************************************************************/
typedef struct ElogFncRunInfor65{
	uchar opeMode;						/* 運転モード */
	uchar emergencyMode;				/* 非常時運転モード */
	uchar startOrStop;					/* 運転／停止 */
	ushort btLowerLimit;				/* 蓄電池放電下限値設定 */
	ushort btUpperLimit;				/* 蓄電池充電上限値設定 */
	uchar testrunOutAtPwrCut;			/* 試運転時停電出力 */
	uchar testrunFlag;					/* 試運転指示フラグ */
	ushort chargePwrSet;				/* 充電電力設定値(蓄電池) */
	ushort dischargePwrSet;				/* 放電電力設定値(蓄電池) */
}ElogFncRunInfor65_t;

typedef struct ElogFncRunInfor75{
	uchar	carOpertPriority;			/* 車両操作優先 */
	uchar	carCharordiscBodyOprat;		/* 車両充放電器本体操作 */
	uchar	pwrcutDischar;				/* 停電時放電動作 */
	uchar	pwrcutDiscPrioritySet;		/* 停電時放電優先設定 */
	uchar	pwrcutCharPrioritySet;		/* 停電時充電優先設定 */
	uchar	connectLockOrUnlock	;		/* コネクタロック／解除 */
	ushort	carDiscLowerLimitSet;		/* 車両放電下限値設定 */
	ushort	carCharUpperLimitSet;		/* 車両充電上限値設定 */
	uchar	carDiscOrnot;				/* 車両放電有無 */
	uchar	discharPrioritySet;			/* 放電優先設定 */
	ushort	pwrcutDiscLowerLimitSet;	/* 停電時車両放電下限値設定 */
	uchar	chargePrioritySet;			/* 充電優先設定 */
	ushort	carCharPwrValueSet;			/* 車載電池充電電力設定値 */
	ushort	carDischarPwrValueSet;		/* 車載電池放電電力設定値 */
}ElogFncRunInfor75_t;

typedef struct ElogFncRunInfor{
	FrmMsgHead_t header;			/* メッセージヘッダ	*/
	ElogFncRunInfor65_t runCmd65;
	ElogFncRunInfor75_t runCmd75;
}ElogFncRunInfor;
/***********************************************************
* @struct	ElogFncSdp_t
* @brief	SDPログ保存 パラメータ
************************************************************/
typedef struct ElogFncSdp{
	FrmMsgHead_t header;				/* メッセージヘッダ	*/
	schar string[ELOG_SDP_LOG_COMP_LEN];/* SDPログ			*/
}ElogFncSdp_t;

/***********************************************************
* @struct	ElogFncIsThere_t
* @brief	USB有無確認要求 パラメータ
************************************************************/
typedef struct ElogFncIsThere{
	FrmMsgHead_t header;			/* メッセージヘッダ	*/
}ElogFncIsThere_t;

/***********************************************************
* @struct	ElogFncSdpComp_t
* @brief	SDPログ圧縮要求 パラメータ
************************************************************/
typedef struct ElogFncSdpComp{
	FrmMsgHead_t header;			/* メッセージヘッダ		*/
	bool_t del;						/* 削除要否				*/
}ElogFncSdpComp_t;


/***********************************************************
* @struct	ElogFncGetLogList_t
* @brief	ログ一覧取得 パラメータ
************************************************************/
typedef struct ElogFncGetLogList{
	FrmMsgHead_t header;			/* メッセージヘッダ	*/
	ElogType_t type;				/* 種別指定			*/
	schar begin[ELOG_GET_LOG_LIST];	/* 期間指定開始日(YYYYMMDD)	*/
	schar end[ELOG_GET_LOG_LIST];	/* 期間指定終了日(YYYYMMDD)	*/
}ElogFncGetLogList_t;

/***********************************************************
* @struct	ElogFncFreeSpace_t
* @brief	ログ領域空き容量取得 パラメータ
************************************************************/
typedef struct ElogFncFreeSpace{
	FrmMsgHead_t header;			/* メッセージヘッダ	*/
}ElogFncFreeSpace_t;

/***********************************************************
* @struct	ElogFncDeleteLog_t
* @brief	ログ削除要求 パラメータ
************************************************************/
typedef struct ElogFncDeleteLog{
	FrmMsgHead_t header;				/* メッセージヘッダ	*/
	ElogType_t type;					/* 種別指定			*/
	schar begin[ELOG_DELETE_LOG_LIST];	/* 期間指定開始日(YYYYMMDD)	*/
	schar end[ELOG_DELETE_LOG_LIST];	/* 期間指定終了日(YYYYMMDD)	*/
}ElogFncDeleteLog_t;

/***********************************************************
* @struct	ElogFncDeleteFile_t
* @brief	指定ファイル削除要求 パラメータ
************************************************************/
typedef struct ElogFncDeleteFile{
	FrmMsgHead_t header;			/* メッセージヘッダ		*/
	ElogDeleteFileType_t type;
	uchar line;
}ElogFncDeleteFile_t;

/***********************************************************
* @struct	ElogFncDownload_t
* @brief	ダウンロードログ出力要求 パラメータ
************************************************************/
typedef struct ElogFncDownload{
	FrmMsgHead_t header;				/* メッセージヘッダ	*/
	schar string[ELOG_NDL_STRMAX];		/* 坚守服务器下载数据*/
}ElogFncDownload_t;

/***********************************************************
* @struct	ElogFncE1OptState_t
* @brief	运行信息log
************************************************************/
typedef struct ElogFncE1OptState{
	FrmMsgHead_t header; 					/* メッセージヘッダ	*/
}ElogFncE1OptState_t;

/***********************************************************
* @struct	ElogRet_t
* @brief	リターン構造体
************************************************************/
typedef struct ElogRet{
	FrmMsgHead_t header;			/* メッセージヘッダ		*/
}ElogRet_t;

/***********************************************************
* @struct	ElogRet13_t
* @brief	ログ領域空き容量取得 応答パラメータ
************************************************************/
typedef struct ElogRet13{
	FrmMsgHead_t header;			/* メッセージヘッダ	*/
	ulong freeSpace;				/* 空き容量(MB)		*/
}ElogRet13_t;

/***********************************************************
* @struct	ElogProgress_t
* @brief	USBログ保存進捗通知パラメータ
************************************************************/
typedef struct ElogProgress{
	FrmMsgHead_t header;			/* メッセージヘッダ		*/
	ElogUsbst_t stat;				/* 進捗ステータス		*/
	uchar progress;					/* 進捗率[%]			*/
	ulong time;						/* 開始時見込み時間[秒] */
}ElogProgress_t;

/***********************************************************
* @struct	ElogCompressAction_t
* @brief	ログ圧縮動作状態通知パラメータ
************************************************************/
typedef struct ElogCompressAction{
	FrmMsgHead_t header;				/* メッセージヘッダ		*/
	ElogCompress_t stat;				/* 圧縮動作状態			*/
	ElogType_t type;					/* ログ種別				*/
	schar savepath[ELOG_COMP_ACT_PATH];	/* 圧縮ファイル名（フルパス）*/
	bool_t branched;					/* 枝番情報(TRUE=枝番有り)	*/
}ElogCompressAction_t;

/***********************************************************
* @struct	ElogGetLogList_t
* @brief	ログ圧縮動作状態通知パラメータ
************************************************************/
typedef struct ElogGetLogListInfor{
	FrmMsgHead_t header;			/* メッセージヘッダ		*/
	ElogGetLogList_t stat;			/* 圧縮動作状態			*/
}ElogGetLogListInfor_t;

/***********************************************************
* @struct	ElogDeleteLog_t
* @brief	ログ圧縮動作状態通知パラメータ
************************************************************/
typedef struct ElogDeleteLogInfor{
	FrmMsgHead_t header;			/* メッセージヘッダ		*/
	ElogDeleteLog_t stat;			/* 圧縮動作状態			*/
}ElogDeleteLogInfor_t;

typedef union ElogRetMsg{
	FrmMsgHead_t header;
	ElogRet_t ret;
	ElogRet13_t ret13;			/* ログ領域空き容量取得		*/
}ElogRetMsg_t;

/***********************************************************
* @struct	ElogThrCancel_t
* @brief	线程取消
************************************************************/
typedef struct ElogThrCancel{
	FrmMsgHead_t header;			/* メッセージヘッダ	*/
	FrmPthread_t id;				/* 线程id			*/
}ElogThrCancel_t;

typedef struct {
	FrmMsgHead_t header;
	slong Fact;
}ELOG_Start_t;

typedef struct {
	FrmMsgHead_t header;
} ELOG_Stop_t;

/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
extern void elog_ProcTimeout(FrmMsgHead_t const *msg);
extern void ELOG_Log(ElogLv_t level,schar *fmt, ...);
extern void ELOG_SyncSetLevel(ElogLv_t level,slong msgID);
extern void ELOG_SyncSetEnable(bool_t onOff,slong msgID);
extern void ELOG_SyncSaveUSB(schar* PcsSerial);
extern slong ELOG_FncAbortUSB(void);
extern void ELOG_AsyncOperationLog(slong PictureID,
			slong ButtonID, schar ObjectType, schar *opt);
extern void ELOG_ErrEventLog(ElogDiffErrType_t type,
			schar *currErrCode, schar errLevel);
/* 获取通知消息数 */
extern ulong ELOG_GetInfoListNum(void);
/* 动作log存储 */
extern void ELOG_ActionLog(schar *actionLog);
/* 动作log压缩 */
extern void ELOG_AsyncCompressActionLog(bool_t del);
/* SOH log存储 */
extern void ELOG_SOHInfoLog(ushort percentage);
/* 电压上升 log存储 */
extern void ELOG_PowerVoltHistLog(schar state);
/* SDP log存储 */
extern void ELOG_AsyncSaveSdpLog(schar *string);
/* HEMS log存储 */
extern void ELOG_HemsHistoryLog(ushort percentage, schar state);
/* 分布式电源累计 log存储 */
extern void ELOG_PowerTotalLog(ulong PowerTotal, ElogFileNumType_t type);
/* 出力制御 log存储 */
extern void ELOG_OutPutCtrlLog(sshort percentage);
/* 发电/买卖电 log存储 */
extern void ELOG_PowerDealLog(void);
/* 蓄电池充放电 log存储 */
extern void ELOG_AccumulatorLog(void);
/* 车辆充放电 log存储 */
extern void ELOG_VehicleLog(void);
/* 运行信息 log存储 */
extern void ELOG_RunInforCMD65Log(ElogFncRunInfor65_t *runInfor);
extern void ELOG_RunInforCMD75Log(ElogFncRunInfor75_t *runInfor);
/* SDP log压缩 */
extern void ELOG_AsyncCompressSdpLog(bool_t del);
extern void ELOG_AsyncGetLogList(ElogType_t type, uchar *begin, uchar *end);
extern void ELOG_SyncGetFreeLogSpace(ulong *pFreeSpace);
extern void ELOG_AsyncDeleteLog(ElogType_t type, uchar *begin, uchar *end);
/* 指定行数删除 */
extern void ELOG_AsyncDeleteFile(ElogDeleteFileType_t type, uchar line);
extern void ELOG_AsyncSetEnableAll(bool_t onOff);
extern void ELOG_AsyncGetEnableAll(bool_t *onOff);
extern slong elog_GetFileSize(schar *path, ulong *size);
extern void ELOG_AsyncDownloadLog(schar*string);
/* 出力制御 log */
extern void ELOG_OutPutCtrlLog(sshort value);
/* 通知log文件内容获取 */
extern slong elog_GetFileLogData(time_t timep, schar *infoData, schar *number);
extern void ELOG_Init(void);
extern void ELOG_MsgProc(FrmMsgHead_t const *msg);
extern void ELOG_Fin(void);
extern slong ELOG_Stop(void);
extern slong ELOG_Start(slong fact);

#ifdef __cplusplus
}
#endif

#endif /* __ELOG_G_H__ */
