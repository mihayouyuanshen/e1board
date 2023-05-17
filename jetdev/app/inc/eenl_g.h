/****************************************************************************/
/*	Description		:ENL通信ミドル 公開ヘッダ								*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC DX.zhao											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/24												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、ENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:ECHONET Lite通信で行う処理								*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/

#ifndef __EENL_G_H__		/* 二重インクルード防止 */
#define __EENL_G_H__

#include <stdio.h>
#include <typedef.h>
#include <lfw_g.h>
#include <frm_g.h>
#include <ewdt.h>
#include <einv_g.h>
#include <enet_g.h>
/* #include <esch_g.h> */
/* #include <ewtch_g.h> */
/* ###################################　↓↓↓　ビルドするため、一時定義↓↓↓　将来は削除する予定　###################################################### */
#include <eenl_other.h>
/* ###################################　↑↑↑　ビルドするため、一時定義↑↑↑　将来は削除する予定　###################################################### */


/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
/* 戻り値コード */
#define	EENL_RET_CODE_OK		((retcode_t)(0))								/* 正常終了 */
#define	EENL_RET_CODE_FAIL		((retcode_t)(-1))								/* 異常終了 */

#define	EENL_VPP_RAMFILE		1u												/* VPP対応のRAMファイル有効フラグ */
#define	EENL_INF_DIR			((schar*)("/mnt/user/eenl"))					/* infファイルの格納ディレクトリ */
#define	EENL_INF_FPATH3			((schar*)("/mnt/user/eenl/eenl3.inf"))			/* infファイルのフルパス3 */
#define	EENL_INF3_SIZE			((uchar)(50u))									/* infファイル3の初期ファイルサイズ */
#define	EENL_INF3_SIZE2			((uchar)(53u))									/* infファイル3の追加ファイルサイズ */
#define	EENL_TIMER				((uchar)(1u))									/* タイマー時間[sec] */
#define	EENL_THREAD_CHK			((uchar)(30u))									/* スレッド監視間隔 */

#define	EENL_TESTRUNSTATE_FILE	((schar*)("/mnt/ram/eenl_testrun_state.inf"))	/* 試運転状態情報 */
#define	EENL_OPECTRL_FILE		((schar*)("/mnt/user/eenl/eenl_opectrl.inf"))	/* ENL充放電制御 */

#define	EENL_NO_DATA			((uchar)(1u))									/* 正常終了(該当データなし) */
#define	EENL_OK					((uchar)(0u))									/* 正常終了 */
#define	EENL_ERR				((schar)(-1))									/* 異常終了(パラメータチェック含む) */
#define	EENL_ERR_SYSCALL		((schar)(-2))									/* 異常終了(システムコールエラー) */
#define	EENL_ERR_ENL_NOT_INIT	((schar)(-3))									/* 異常終了(ENLライブラリ未初期化) */
#define	EENL_ERR_FILE_OPEN		((schar)(-4))									/* 異常終了(ファイルオープンエラー) */

#define EENL_VERSION_FIX_STR	((schar*)("Release K"))							/* ENLバージョン文字列 */

#define EENL_FILE_MAX_LEN		 ((ulong)(100u))								/* ファイル名のパスを含む最大長 */
#define EENL_HEMS_HIST_FPATH	((schar*)("/mnt/user/eenl/hemshistory"))		/* HEMS発電抑制履歴ファイルのフルパス */

#define EENL_HEMS_HISTORY_MAX	((uchar)(100u))									/* HEMS発電抑制履歴データ格納最大数	*/

/* HEMS発電抑制情報 定数定義 */
#define EENL_HEMS_HIST_NOTSET	((uchar)(0x00u))								/* 未定状態（取得側では使用禁止） */
#define EENL_HEMS_HIST_VALID	((uchar)(0x01u))								/* HEMS発電抑制情報が有効 */
#define EENL_HEMS_HIST_INVALID	((uchar)(0xFFu))								/* HEMS発電抑制情報が無効 */

/* 共通定義 */
#define	EENL_NOT_CHANGE			((uchar)(0u))									/* 変更なし */
#define	EENL_CHANGE				((uchar)(1u))									/* 変更あり */
#define	EENL_NEW_DATA			((uchar)(2u))									/* 新しいデータ */
#define	EENL_ON					((uchar)(1u))									/* ON */
#define	EENL_OFF				((uchar)(0u))									/* OFF */


/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef enum {
	EENL_STATUS_IDLE = ((uchar)(0u)),							/* 初期状態 */
	EENL_STATUS_CONFIRM_START_1 = ((uchar)(1u)),				/* 起動条件の確認開始ステップ１ */
	EENL_STATUS_CONFIRM_START_2 = ((uchar)(2u)),				/* 起動条件の確認開始ステップ２(メッセージ受信) */
	EENL_STATUS_EXECUTE_START_1 = ((uchar)(3u)),				/* 実行処理開始ステップ１ */
	EENL_STATUS_EXECUTE_START_2 = ((uchar)(4u))					/* 実行処理開始ステップ２(メッセージ受信) */
} EenlStatusType_t;

/***********************************************************
* @enum		EenlMsgId_t
* @brief	メッセージID番号定義
************************************************************/
typedef enum {
	EENL_CONFIRM_START = ((uchar)1u),							/* 起動条件の確認開始 */
	EENL_EXECUTE_START = ((uchar)2u),							/* 実行処理開始 */
	EENL_GET_LOCATION = ((uchar)3u),							/* 設置場所取得 */
	EENL_SET_LOCATION = ((uchar)4u),							/* 設置場所設定 */
	EENL_GET_V2H_LOCATION = ((uchar)5u),						/* V2H設置場所取得 */
	EENL_SET_V2H_LOCATION = ((uchar)6u),						/* V2H設置場所設定 */
	EENL_GET_HEMS_HISTORY = ((uchar)7u),						/* HEMS発電抑制履歴取得 */
	EENL_SET_TESTRUN_STATUS = ((uchar)8u),						/* 試運転状態イベント（einv） */
	EENL_EVT_STTS_CHANGE_REQ = ((uchar)9u),						/* 状態変化通知イベント（einv） */
	EENL_EVT_SP_CHANGE_REQ = ((uchar)10u),						/* 設定変更通知イベント（einv） */
	EENL_EVT_V2H_STTS_CHANGE_REQ = ((uchar)11u),				/* V2H状態変化イベント（einv） */
	EENL_EVT_V2H_SETTING_CHANGE_REQ = ((uchar)12u),				/* V2H設定変更イベント（einv） */
	EENL_EVT_SETTING_CHANGE = ((uchar)13u)						/* 設定変更通知イベント（esch） */
} EenlMsgId_t;

/***********************************************************
* @enum		EenlType_t
* @brief	仕向け定義
************************************************************/
typedef enum {
	EENL_TYPE_NONE = ((uchar)0u),
	EENL_TYPE_KYC = ((uchar)1u),
	EENL_TYPE_NCC = ((uchar)2u),
	EENL_TYPE_SKK = ((uchar)4u)
} EenlType_t;


/***********************************************************
* @enum		EenlTestRunState_t
* @brief	試運転状態
************************************************************/
typedef enum {
	EENL_TESTRUN_UNDEF = ((uchar)0u),							/* 試運転未設定(待機) */
	EENL_TESTRUN_START = ((uchar)1u),							/* 試運転開始(試運転中) */
	EENL_TESTRUN_STOP = ((uchar)2u),							/* 試運転中止(途中終了) */
	EENL_TESTRUN_FINISH = ((uchar)3u),							/* 試運転終了(正常終了⇒後処理) */
	EENL_TESTRUN_MAX = ((uchar)4u)
} EenlTestRunState_t;

/***********************************************************
* @enum		EenlDevList_t
* @brief	デバイスリスト定義
*  memo		!!注意!!
*			ループカウントの開始/終了で使用しているため、
*			新規にデバイスを追加する場合は運用上問題ない位置に追加すること
************************************************************/
typedef enum _eenl_dev_list_e {
	EENL_DEV_LIST_NODE = ((uchar)0u),							/* ノード */
	EENL_DEV_LIST_MPCS = ((uchar)1u),							/* マルチ入力PCS */
	EENL_DEV_LIST_SO = ((uchar)2u),								/* 太陽光 */
	EENL_DEV_LIST_BT = ((uchar)3u),								/* 蓄電池 */
	EENL_DEV_LIST_V2H = ((uchar)4u),							/* V2H */
	EENL_DEV_LIST_METER_E1 = ((uchar)5u),						/* 分散型電源電力量メータ E1*/
	EENL_DEV_LIST_METER_C2 = ((uchar)6u),						/* 分散型電源電力量メータ C2*/
	EENL_DEV_LIST_MAX = ((uchar)7u)
} EenlDevList_t;


/***********************************************************
* @struct	EenlGetLocation_t
* @brief	設置場所取得  要求  構造体
************************************************************/
typedef struct {
	FrmMsgHead_t	header;										/* メッセージ構造体 */
} EenlGetLocation_t;

/***********************************************************
* @enum		EenlLocation_t
* @brief	設置場所定義
************************************************************/
typedef enum {
	EENL_LOCATION_INSIDE	= (uchar)(0x58u),					/* 室内 */
	EENL_LOCATION_OUTSIDE	= (uchar)(0x60u),					/* 室外 */
	EENL_LOCATION_GARAGE	= (uchar)(0x68u),					/* 車庫 */
	EENL_LOCATION_OTHERS	= (uchar)(0x78u),					/* その他 */
	EENL_LOCATION_NONE		= (uchar)(0x00u)					/* 無し */
} EenlLocation_t;

/***********************************************************
* @struct	EenlGetLocationRet_t
* @brief	設置場所取得  応答  構造体
************************************************************/
typedef struct {
	FrmMsgHead_t		header;									/* メッセージ構造体 */
	EenlLocation_t		location;								/* 設置場所 */
} EenlGetLocationRet_t;

/***********************************************************
* @struct	EenlSetLocation_t
* @brief	設置場所設定  要求  構造体
************************************************************/
typedef struct _eenl_set_location_t {
	FrmMsgHead_t		header;									/* メッセージ構造体 */
	EenlLocation_t		location;								/* 設置場所 */
} EenlSetLocation_t;

/***********************************************************
* @struct	EENL_DATE_T
* @brief	年月日情報構造体
************************************************************/
typedef struct _eenl_date_t {
	ushort	Year;												/*年 (値の範囲：2000～2037) */
	uchar	Month;												/*月 (値の範囲：1～12) */
	uchar	Day;												/*日 (値の範囲：1～31) */
}EENL_DATE_T;

/***********************************************************
* @struct	EENL_HEMS_HISTORY_SUPPRESS_T
* @brief	HEMS発電抑制履歴 状態変化
************************************************************/
typedef struct {
	uchar		valid;											/* 有効／無効	*/
	uchar		hours;											/* 開始／終了時	*/
	uchar		minutes;										/* 開始／終了分	*/
} EenlHemsHistSuppress_t;

/***********************************************************
* @struct	EenlHemsHistData_t
* @brief	HEMS発電抑制履歴データ
************************************************************/
typedef struct {
	EENL_DATE_T					date;							/* 発電制限値(0xE5,0xA0)が変化を開始した年月日 */
	EenlHemsHistSuppress_t		start;							/* HEMS発電抑制開始情報 */
	EenlHemsHistSuppress_t		end;							/* HEMS発電抑制終了情報 */
	uchar						value;							/* 発電制限値(%)の値(0～100) */
} EenlHemsHistData_t;

/***********************************************************
* @struct	EenlHemsHistory_t
* @brief	HEMS発電抑制履歴
************************************************************/
typedef struct {
	uchar					data_num;							/* HEMS発電抑制履歴データ要素数	*/
	uchar					reserved;							/* 予備 */
	EenlHemsHistData_t	data[EENL_HEMS_HISTORY_MAX];			/* HEMS発電抑制履歴データ */
} EenlHemsHistory_t;

/***********************************************************
* @struct	EenlGetHemsHistory_t
* @brief	HEMS発電抑制履歴取得  要求  構造体
************************************************************/
typedef struct {
	FrmMsgHead_t		header;									/* メッセージ構造体 */
} EenlGetHemsHistory_t;

/***********************************************************
* @struct	EenlGetHemsHistoryRet_t
* @brief	HEMS発電抑制履歴取得  応答  構造体
************************************************************/
typedef struct {
	FrmMsgHead_t		header;									/* メッセージ構造体 */
	EenlHemsHistory_t	hemshistory;							/* EMS発電抑制履歴 */
} EenlGetHemsHistoryRet_t;


/***********************************************************
* @struct	EenlTime_t
* @brief	時刻情報構造体
************************************************************/
typedef struct {
	schar	hour[CMN_HOUR_LEN];									/* 時（“00”～“23”） */
	schar	min[CMN_MIN_LEN];									/* 分（“00”～“59”） */
} EenlTime_t;

/***********************************************************
* @struct	EenlChargeTimeZone_t
* @brief	充放電時間帯情報構造体
************************************************************/
typedef struct {
	EenlTime_t		chargeStartTime;							/* 充電開始時刻 */
	EenlTime_t		chargeStopTime;								/* 充電停止時刻 */
	EenlTime_t		dischargeStartTime;							/* 放電開始時刻 */
	EenlTime_t		dischargeStopTime;							/* 放電停止時刻 */
} EenlChargeTimeZone_t;

/***********************************************************
* @struct	EenlEvtSttsChangeReq_t
* @brief	状態変化通知イベントメッセージ構造体
************************************************************/
typedef struct {
	FrmMsgHead_t				header;							/* メッセージヘッダー */
	ulong						changedFlag;					/* 変更有無を示すフラグ */
	uchar						opeMode;						/* 変更後の運転モード */
	uchar						emergencyPercent;				/* 変更後の非常時安心設定 */
	uchar						timeZoneOpeMode;				/* 変更後の充放電時間帯の自動運転モード */
	EenlChargeTimeZone_t		chargeTimeZone;					/* 変更後の充放電時間帯 */
	uchar						operation;						/* 変更後の運転中状態 */
	uchar						pwrCut;							/* 変更後の停電状態 */
	ushort						soh;							/* 変更後のSOH値 */
	uchar						suppressState;					/* 変更後の出力抑制状態 */
} EenlEvtSttsChangeReq_t;

/***********************************************************
* @struct	EenlEvtSpChangeReq_t
* @brief	設定変更通知イベントメッセージ構造体
************************************************************/
typedef struct {
	FrmMsgHead_t		header;									/* メッセージヘッダー */
	ulong				changedFlag;							/* 変更有無を示すフラグ */
	ulong				addChangedFlag;							/* M1M2以降に追加された設定値の変更有無を示すフラグ */
	ushort				otp;									/* 変更後の出力増減速度[分] */
	uchar				pwrFactCtrl;							/* 変更後の力率一定制御 */
	ushort				genPwrCap;								/* 変更後の発電電力上限 */
	uchar				fitType;								/* FIT契約タイプ */
	uchar				selfConsumptionType;					/* 自家消費タイプ */
	ulong				facilityCertifiedCapacity;				/* 設備認定容量[W] */
} EenlEvtSpChangeReq_t;

/***********************************************************
* @struct	EenlEvtV2hSttsChangeReq_t
* @brief	V2H状態変化イベントメッセージ構造体
************************************************************/
typedef struct {
	FrmMsgHead_t		header;									/* メッセージヘッダー */
	ulong				changedFlag;							/* 変更有無を示すフラグ */
	uchar				v2hConnectorLockState;					/* 変更後のコネクタロック状態 */
	uchar				v2hAvailabilityStatus;					/* 変更後の車両接続・充放電可否状態 */
} EenlEvtV2hSttsChangeReq_t;

/***********************************************************
* @struct	EenlEvtV2hSttingChangeReq_t
* @brief	V2H設定変更イベントメッセージ構造体
************************************************************/
typedef struct {
	FrmMsgHead_t		header;									/* メッセージヘッダー */
	ulong				setEventType;							/* V2H設定情報設定変更フラグ */
	ulong				v2hChargeAmountSet1;					/* 車載電池充電量設定値1 */
	ushort				v2hChargeAmountSet2;					/* 車載電池充電量設定値2 */
	ulong				v2hPowerSetCharge;						/* 車載電池充電電力設定値 */
	ulong				v2hPowerSetDiscCharge;					/* 車載電池放電電力設定値 */
	ushort				v2hCurrentSetCharge;					/* 車載電池充電電流設定値 */
	ushort				v2hCurrentSetDischarge;					/* 車載電池放電電流設定値 */
	ushort				v2hReratedVolt;							/* 車両充放電器定格電圧 */
	uchar				chargeTimerSetting;						/* 充電タイマ設定 */
	ulong				v2hDischargeSetValue;					/* 車載電池放電量設定値[Wh]			0～999,999,999 */
	ulong				changedProcessMsgId;					/* 変更元プロセスのメッセージID(CPORT) */
} EenlEvtV2hSttingChangeReq_t;


/***********************************************************
* @struct	EenlDateTime_t
* @brief	日付型情報
************************************************************/
typedef struct {
	ushort	year;												/* 年(1970～2037) */
	uchar	month;												/* 月（1～12）*/
	uchar	day;												/* 日（1～31）*/
	uchar	hour;												/* 時（0～23）*/
	uchar	minute;												/* 分（0～59）*/
	uchar	second;												/* 秒（0～59）*/
} EenlDateTime_t;

/***********************************************************
* @struct	EenlEvtSttingChangeReq_t
* @brief	設定変更通知イベント構造体
************************************************************/
typedef struct {
	FrmMsgHead_t		header;									/* メッセージ構造体 */
	ulong				eventType;								/* 設定変更通知イベント種別 */
	uchar				outputCtrlOnOff;						/* 出力制御 対象／対象外 */
	EenlDateTime_t		nextAccessDate;							/* 次回アクセス日時 */
} EenlEvtSttingChangeReq_t;

/***********************************************************
* @struct	EenlConfirmStart_t
* @brief	起動条件の確認  要求  構造体
************************************************************/
typedef struct {
	FrmMsgHead_t Header;										/* メッセージ構造体 */
} EenlConfirmStart_t;

/***********************************************************
* @struct	EenlExecuteStart_t
* @brief	実行処理開始  要求  構造体
************************************************************/
typedef struct {
	FrmMsgHead_t Header;										/* メッセージ構造体 */
} EenlExecuteStart_t;

/***********************************************************
* @struct	EenlExecuteStart_t
* @brief	試運転状態  応答  構造体
************************************************************/
typedef struct {
	FrmMsgHead_t Header;										/* メッセージ構造体 */
	EenlTestRunState_t state;									/* 試運転状態 */
} EenlTestRun_t;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* EENL初期化処理 */
extern void EENL_Init(void);

/* メッセージ処理 */
extern void EENL_MsgProc(FrmMsgHead_t const* msg);

/* EENL終了化処理 */
extern void EENL_Fin(void);

/* EENL開始処理 */
extern slong EENL_Start(slong fact);

/* EENL停止処理 */
extern slong EENL_Stop(void);



/* 設置場所取得の他プロセスに対するIF */
extern slong EENL_SyncGetLocation(EenlLocation_t *location);

/* 設置場所設定の他プロセスに対するIF */
extern slong EENL_AsyncSetLocation(const EenlLocation_t location);

/* ENLバージョン取得の他プロセスに対するIF */
extern slong EENL_SyncGetVersion(uchar *enl_version);

/* V2H設置場所取得の他プロセスに対するIF */
extern slong EENL_SyncGetLocationV2H(EenlLocation_t *location);

/* V2H設置場所設定の他プロセスに対するIF */
extern slong EENL_AsyncSetLocationV2H(const EenlLocation_t location);

/* HEMS発電抑制履歴取得の他プロセスに対するIF */
extern slong EENL_SyncGetHemsHistory(EenlHemsHistory_t *hemshistory);

/* einv側の試運転状態設定イベント */
extern slong EENL_EventSetTestRunState(const EenlTestRunState_t state);

/* einv側の状態変化通知イベント */
extern slong EENL_EventSttsChange(EenlEvtSttsChangeReq_t *sendMsg);

/* einv側の設定変更通知イベント */
extern slong EENL_EventSpChange(EenlEvtSpChangeReq_t *sendMsg);

/* einv側のV2H状態変化イベント */
extern slong EENL_EventV2hSttsChange(EenlEvtV2hSttsChangeReq_t *sendMsg);

/* einv側のV2H設定変更イベント */
extern slong EENL_EventV2hSettingChange(EenlEvtV2hSttingChangeReq_t *sendMsg);

/* esch側の設定変更通知イベント */
extern slong EENL_EventSettingChange(EenlEvtSttingChangeReq_t *sendMsg);

/* 書き込み要求受信（コールバック関数） */
extern slong ENL_DEV_callback_request_write(void* instance,
	uchar epc, uchar* pdc, uchar* edt);

/* 読み出し要求/通知要求受信（コールバック関数） */
extern slong ENL_DEV_callback_request_read(void* instance,
	uchar epc, uchar* pdc, uchar* edt);


#ifdef __cplusplus
}
#endif
#endif	/* __EENL_G_H__ */


