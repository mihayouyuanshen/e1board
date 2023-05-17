#ifndef	__EENL_OTHER_H__
#define	__EENL_OTHER_H__

#include "frm_g.h"
//###################################　↓↓↓　ビルドするため、一時定義↓↓↓　将来は削除する予定　######################################################
//通信NG
#define	ENOT_ERRSTATE_PCS			0x00000040	//PCS通信不通(BF550)

/*****************************************************************************/
/*																			 */
/*				   メッセージ構造体											 */
/*																			 */
/*****************************************************************************/
//typedef struct _lfw_header_t {
//	ulong	mtype;				/* LFW_MAX_MTYPE 固定			*/
//	ulong	attribute;			/* LFW_MESSAGETYPE_E から選択	*/
//	ulong	functionId;			/* ファンクション番号			*/
//	ulong	returnMsgid;		/* 返信先メッセージID			*/
//	ulong	parameterLength;	/* パラメータ長					*/
//	slong	returnCode;			/* リターンコード				*/
//}LFW_HEADER_T;
///***********************************************************
//* @struct	イベント構造体(試運転状態通知イベント)
//* @breaf	イベント送信メッセージ
//************************************************************/
//typedef struct _egui_evt03_t {
//	LFW_HEADER_T	header;
//	uchar				evt_id;			/* イベントＩＤ（102：EGUI_EVTID_STATETESTRUN） */
//	ulong				stt_testrun;	/* 試運転の動作状態 */
//	ulong				start_flg;		/* 試運転開始区分 */
//}EGUI_EVT03_T;

//###################################　↑↑↑　ビルドするため、一時定義↑↑↑　将来は削除する予定　######################################################
//###################################　↓↓↓　ビルドするため、一時定義↓↓↓　将来は削除する予定　######################################################
// esch.h
#define	ESCH_OK						(0)							/* 正常終了 */
#define	ESCH_ERR_SYSCALL			(-1)						/* システムコールエラー */
#define	ESCH_DAILY_SCHED_DATA_LEN			(48)				/* 日間スケジュールデータ制御値の数 */
#define	ESCH_EVT_OUTPUT_CTRL_ONOFF			(0x00000004)		/* 出力制御対象／対象外変更通知 */
#define	ESCH_EVT_NEXT_ACCESS_DATE			(0x00000008)		/* 次回アクセス日時変更通知 */
#define	ESCH_REG_NOTICE_ENABLE				(0x01)				/* 通知必要 */




/***********************************************************
*
* 定数定義
*
************************************************************/

// 応答コード
#define	EPCS_OK								(0)				// 正常終了

// 運転モード												// MODE=SK,N1
#define	EPCS_OPEMODE_0						(0x00)			// N/A
#define	EPCS_OPEMODE_1						(0x01)			// 売電モード
#define	EPCS_OPEMODE_2						(0x02)			// グリーンモード
#define	EPCS_OPEMODE_3						(0x03)			// EVモード(運転モード保持で使用。PCSには送信しない)
#define	EPCS_OPEMODE_4						(0x04)			// 蓄電池充電
#define	EPCS_OPEMODE_5						(0x05)			// 蓄電池放電
#define	EPCS_OPEMODE_6						(0x06)			// 待機
#define	EPCS_OPEMODE_7						(0x07)			// メンテナン
#define	EPCS_OPEMODE_8						(0x08)			// N/A
#define	EPCS_OPEMODE_9						(0x09)			// 余剰充電・放電
#define	EPCS_OPEMODE_A						(0x0A)			// 余剰のみ充電
#define	EPCS_OPEMODE_B						(0x0B)			// 車両充電
#define	EPCS_OPEMODE_C						(0x0C)			// 車両放電
#define	EPCS_OPEMODE_D						(0x0D)			// N/A
#define	EPCS_OPEMODE_E						(0x0E)			// 車両拡張充電
#define	EPCS_OPEMODE_F						(0x0F)			// エレムーブ充電
#define	EPCS_OPEMODE_G						(0x10)			// エレムーブ放電
#define	EPCS_OPEMODE_H						(0x11)			// EVモード_余剰充電・放電
#define	EPCS_OPEMODE_I						(0x12)			// EVモード_余剰のみ充電
#define	EPCS_OPEMODE_J						(0x13)			// EVモード_エレムーブ充電
#define	EPCS_OPEMODE_K						(0x14)			// EVモード_車両充電
#define	EPCS_OPEMODE_L						(0x15)			// EVモード_待機

//
// 前回の自動運転モード
#define	EPCS_AUTO_OPE_UNDEF					(0xFF)			// 前回値なし

// 運転中状態
#define	EPCS_OPE_STOP						(0)				// 停止中
#define	EPCS_OPE_WAIT						(1)				// 待機中
#define	EPCS_OPE_DISCHARGE					(2)				// 放電中
#define	EPCS_OPE_CHARGE						(3)				// 充電中

// 停電状態
#define	EPCS_PWROUTAGE_NO					(0)				// 停電以外
#define	EPCS_PWROUTAGE_YES					(1)				// 停電

// PCSエラーコード
#define	EPCS_PCSERR_NONE					"BE000"			// PCSエラーなし


// 運転モード変更フラグ
#define	EPCS_SET_OPE_MODE						(0x00000001)	// 運転モード
#define	EPCS_SET_EMG_PER 						(0x00000004)	// 非常時安心設定
#define	EPCS_SET_TIME_ZONE						(0x00000008)	// 充放電時間帯設定
#define EPCS_SET_LIMIT_CTRL						(0x00000080)	// 充放電制限／制限解除フラグ



// V2H設定情報設定変更フラグ
#define	EPCS_SET_V2H_CHARGE_AMOUNT_SET_1	(0x00000001)	// 車載電池充電量設定値1
#define	EPCS_SET_V2H_POWER_SET_CHARGE		(0x00000004)	// 車載電池充電電力設定値
#define	EPCS_SET_V2H_POWER_SET_DISCHARGE	(0x00000008)	// 車載電池放電電力設定値
#define	EPCS_SET_V2H_DISCHARGE_SET_VALUE	(0x00000040)	// 車載電池放電量設定値

// V2H積算電力量リセット変更フラグ
#define	EPCS_SET_V2H_TTL_DISCHARGE_RESET	(0x0001)	// 車載電池充電量設定値1
#define	EPCS_SET_V2H_TTL_CHARGE_RESET		(0x0002)	// 車載電池充電量設定値2



// 太陽光発電情報設定変更フラグ
#define	EPCS_SET_PVINFO_FIT					(0x00000001)	// FIT契約タイプ

// 状態変化通知イベント種別
#define	EPCS_EVT_OPE_MODE					(0x00000001)	// 運転モード変更通知
#define	EPCS_EVT_EMG_MODE					(0x00000002)	// 非常時運転モード変更通知
#define	EPCS_EVT_EMG_PER 					(0x00000004)	// 非常時安心設定変更通知
#define	EPCS_EVT_TIME_ZONE					(0x00000010)	// 充放電時間帯変更通知
#define	EPCS_EVT_OPE_STTS					(0x00000020)	// 運転中状態変更通知
#define	EPCS_EVT_PWROUTAGE					(0x00000080)	// 停電状態変更通知
#define	EPCS_EVT_SOH						(0x00000400)	// SOH値変更通知
#define	EPCS_EVT_HEMS_STATE					(0x00040000)	// HEMS制御状態変化通知
#define	EPCS_EVT_PVGEN_NOTICE				(0x00080000)	// 発電状態通知情報変化通知
#define	EPCS_EVT_SUPPRESS_STATE				(0x00100000)	// 出力抑制状態(CMD21)変化通知
#define EPCS_EVT_RESET_PWR_SET				(0x00400000)	// 充放電電力設定値リセット通知
#define EPCS_EVT_AC_CHARGE_WAIT				(0x00800000)	// AC充電量設定値待機動作通知
#define EPCS_EVT_AC_DISCHARGE_WAIT			(0x01000000)	// AC放電量設定値待機動作通知

#define	EPCS_EVT_OTP						(0x00400000)	// 出力増減速度変更通知

//// 追加設定変更通知イベント種別
#define	EPCS_EVT_ADDM_PWR_FACT_CTRL			(0x00000010)	// 力率一定制御
#define	EPCS_EVT_ADDM_GEN_PWR_CAP			(0x00000020)	// 発電電力上限
#define	EPCS_EVT_ADDM_FIT_TYPE				(0x00000100)	// FIT契約タイプ変更通知
#define	EPCS_EVT_ADDM_SELF_CONSUMP			(0x00000200)	// 自家消費タイプ変更通知
#define	EPCS_EVT_ADDM_FACILITY_CAP			(0x00000400)	// 設備認定容量変更通知

// V2H状態変化通知イベント種別
#define	EPCS_EVT_V2H_CONNECTOR_LOCK_STATE 	(0x00000001)	// コネクタロック状態
#define	EPCS_EVT_V2H_AVAILABILITY_STATE		(0x00000004)	// 車両接続・充放電可否状態
//// V2H設定情報設定通知イベント種別
#define	EPCS_EVT_V2H_CHARGE_AMOUNT_SET_1	(0x00000001)	// 車載電池充電量設定値1
//#define	EPCS_EVT_V2H_CHARGE_AMOUNT_SET_2	(0x00000002)	// 車載電池充電量設定値2
#define	EPCS_EVT_V2H_POWER_SET_CHARGE		(0x00000004)	// 車載電池充電電力設定値
#define	EPCS_EVT_V2H_POWER_SET_DISCHARGE	(0x00000008)	// 車載電池放電電力設定値
//#define	EPCS_EVT_V2H_CURRENT_SET_CHARGE		(0x00000010)	// 車載電池充電電流設定値
//#define	EPCS_EVT_V2H_CURRENT_SET_DISCHARGE	(0x00000020)	// 車載電池放電電流設定値
#define	EPCS_EVT_V2H_RATED_VOLT				(0x00000040)	// 車両充放電器定格電圧
#define	EPCS_EVT_V2H_DISCHARGE_SET_VALUE	(0x00000200)	// 車載電池放電量設定値

// 配列要素数定義
#define	EPCS_PCS_FW_VER_LEN					(31)			// PCS FWバージョンの最大文字列長
#define	EPCS_PCS_MODEL_NAME_LEN				(17)			// システム機種名の最大文字列長
#define	EPCS_PCS_SERIAL_NUM_LEN				(21)			// PCS製造番号の最大文字列長
#define	EPCS_PCS_ERR_LEN					(6)				// PCSエラーの最大文字列長
#define	EPCS_V2H_MODEL_NAME_LEN				(17)			// 車両充放電器機種名の最大文字列長(終端の’\0’を含む)
#define	EPCS_V2H_SERIAL_NUM_LEN				(17)			// 車両充放電器製造番号の最大文字列長(終端の’\0’を含む)
#define	EPCS_V2H_TYPE_LEN					(3)				// 車両充放電器タイプの最大文字列長(終端の’\0’を含む)
#define	EPCS_VEHICLE_ID_LEN					(25)			// 車両IDの最大文字列長(24文字 + 終端’\0)
#define EINV_METER_ENERGER_HISTORY_LEN		(194u)			// 積算電力量計測値履歴列長
#define EINV_METER_ENERGER_LEN				(11u)			// 定時積算電力量計測値列長

// 発電電力制限値設定/取得種別フラグ
#define	EPCS_RESTRICTED_TYPE_OUTPUT_CTRL	(0x00000003)	// 出力制御設定指定


// 発電状態通知情報（発電状態）
#define	EPCS_PVGEN_NOTICE_PVGEN_WAIT		(0x03)		// 待機中

// 発電状態通知情報（出力抑制状態）
#define	EPCS_PVGEN_NOTICE_SUPPRESS_NONE		(0x00)		// 抑制なし
#define	EPCS_PVGEN_NOTICE_SUPPRESS_VOLT		(0x01)		// 電圧上昇抑制中
#define	EPCS_PVGEN_NOTICE_SUPPRESS_OUTPUT	(0x02)		// 出力制御中

// システム構成ビット
#define	EPCS_SYS_CON_PV			(0x0001)	// 太陽光を含むシステム構成
#define	EPCS_SYS_CON_BT			(0x0002)	// 蓄電池を含むシステム構成
#define	EPCS_SYS_CON_V2H		(0x0004)	// V2Hを含むシステム構成
#define	EPCS_SYS_CON_METER_E1	(0x0005)	// METER(E1)を含むシステム構成
#define	EPCS_SYS_CON_METER_C2	(0x0006)	// METER(C2)を含むシステム構成



// 車両充放電器状態
#define	EPCS_V2H_STATE_WAIT				(0x00)	// 待機
#define	EPCS_V2H_STATE_PREPARING_START	(0x01)	// 充放電準備中
#define	EPCS_V2H_STATE_CHARGE			(0x02)	// 充電中
#define	EPCS_V2H_STATE_DISCHARGE		(0x03)	// 放電中
#define	EPCS_V2H_STATE_PREPARING_END	(0x04)	// 充放電終了準備中
#define	EPCS_V2H_STATE_ERR_STOP			(0x05)	// エラー停止

// コネクタロック状態
#define	EPCS_V2H_CONNECTOR_LOCK			(0x01)	// ロック
#define	EPCS_V2H_CONNECTOR_UNLOCK		(0x00)	// 解除

// 車載電池の積算量リセット設定
#define	EPCS_V2H_TTL_ENERGY_RESET_YES	(0x00)	// リセットする
#define	EPCS_V2H_TTL_ENERGY_RESET_NO	(0x01)	// リセットしない

/***********************************************************
* @def		出力制御対象／対象外
* @brief
***********************************************************/
#define	ESCH_OUTPUT_CTRL_ON					(0x01)				/* 出力制御対象 */

/*******************************************************************************
** 日付、時刻の文字列長
*******************************************************************************/
#define	CMN_YEAR_LEN			(5)		// 西暦年の文字列長(文字列終端あり)
#define	CMN_MONTH_LEN			(3)		// 月の文字列長(文字列終端あり)
#define	CMN_DAY_LEN				(3)		// 日の文字列長(文字列終端あり)
#define	CMN_HOUR_LEN			(3)		// 時の文字列長(文字列終端あり)
#define	CMN_MIN_LEN				(3)		// 分の文字列長(文字列終端あり)
#define	CMN_SEC_LEN				(3)		// 秒の文字列長(文字列終端あり)




//ensv_suchar.h
/**
 * @brief  設置場所の定義
 */
typedef enum {
	ENSV_LOCATION_INSIDE = (ulong)0,	/**< 設置場所：屋内(車庫)	*/
	ENSV_LOCATION_OUTSIDE,			/**< 設置場所：屋外			*/
	ENSV_LOCATION_MAX				/**< 終端要素				*/
} ENSV_LOCATION_E;



//epcs.h
/***********************************************************
* @enum		EPCS_EVT_ID_E
* @brief	イベント通知用ファンクションID
************************************************************/
typedef enum _epcs_evt_id_e {
	EPCS_EVT_STTS_CHANGE_REQ = 0,							// 状態変化通知イベント
	EPCS_EVT_SP_CHANGE_REQ,									// 設定変更通知イベント
	EPCS_EVT_V2H_STTS_CHANGE_REQ,							// V2H状態変化通知イベント
	EPCS_EVT_V2H_SETTING_CHANGE_REQ							// V2H設定変更通知イベント
}EPCS_EVT_ID_E;

/***********************************************************
* @enum		EPCS_LIMIT_E
* @brief	充放電電力量制御／制御解除定義
************************************************************/
typedef enum _epcs_limit_e {
	EPCS_LIMIT_CTRL_OFF = 0,								// 充放電電力量による制御を行う
	EPCS_LIMIT_CTRL_ON,										// 充放電電力量による制御を解除する
	EPCS_LIMIT_CTRL_UNDEF,									// 無効値（指定されても無視する）
}EPCS_LIMIT_E;
/***********************************************************
* @enum		EPCS_REMOTE_E
* @brief	リモート操作状態(運転モード設定)
************************************************************/
typedef enum _epcs_remote_e {
	EPCS_REMOTE_OFF = 0,									// リモート操作なし
	EPCS_REMOTE_ON,											// リモート操作あり
	EPCS_REMOTE_TEST_RUN,									// 試運転
}EPCS_REMOTE_E;


/***********************************************************
* @struct	EPCS_DATE_T
* @brief	日付情報構造体
************************************************************/
typedef struct _epcs_date_t {
	schar	year[CMN_YEAR_LEN];									// 年（“2000”～“2099”）
	schar	month[CMN_MONTH_LEN];								// 月（“01”～“12”）
	schar	day[CMN_DAY_LEN];									// 日（“01”～“31”）
}EPCS_DATE_T;


/***********************************************************
* @struct	EPCS_TIME_T
* @brief	時刻情報構造体
************************************************************/
typedef struct _epcs_time_t {
	schar	hour[CMN_HOUR_LEN];									// 時（“00”～“23”）
	schar	min[CMN_MIN_LEN];									// 分（“00”～“59”）
}EPCS_TIME_T;
/***********************************************************
* @struct	EPCS_CHARGE_TIME_ZONE_T
* @brief	充放電時間帯情報構造体
************************************************************/
typedef struct _epcs_charge_time_zone_t {
	EPCS_TIME_T		chargeStartTime;						// 充電開始時刻																						★	E1通信ボードのシステムの運転モードの自動運転（売電モード、グリーンモード）
															//																															 = 機器オブジェクト蓄電池クラス(0xF4)
	EPCS_TIME_T		chargeStopTime;							// 充電停止時刻
	EPCS_TIME_T		dischargeStartTime;						// 放電開始時刻
	EPCS_TIME_T		dischargeStopTime;						// 放電停止時刻
}EPCS_CHARGE_TIME_ZONE_T;


/***********************************************************
* @struct	EPCS_OPESTTS_INFO_T
* @brief	動作情報構造体
************************************************************/
typedef struct _epcs_opestts_info_t {
	slong	chargeDischargePwr;									// AC 充放電電力（インバータ部）[slong]										★	CMD81のPCS充放電電力(AC端計測値)				 = 機器オブジェクトマルチ入力PCSクラス(0xE7)
	ulong	remainingBtW;										// 電池残量[0.1kWh]															★	CMD81の電池残量									 = 機器オブジェクト蓄電池クラス(0xE2)
	ulong	soc;												// SOC値[0.1%]																★	CMD81のｒSOC									 = 機器オブジェクト蓄電池クラス(0xE4)
	ulong	soh;												// SOH値[0.1%]																★	CMD81のSOH										 = 機器オブジェクト蓄電池クラス(0xE5)
	uchar	operation;											// 蓄電池運転動作状態[EPCS011-001]											★	CMD81の蓄電池運転動作状態	 					 = 機器オブジェクトスーパークラス(0x80)
	uchar	pcsOperation;										// PCS運転状態																★	CMD81のPCS運転状態	 							 = 機器オブジェクトスーパークラス(0x80)
	uchar	v2hOperation;										// 車両充放電器運転動作状態													★	CMD81の車両充放電器運転動作状態	 				 = 機器オブジェクトスーパークラス(0x80)
	uchar	pwrCut;												// 停電状態																	★	CMD81の停電										 = 機器オブジェクト蓄電池クラス(0xDB) / 機器オブジェクトマルチ入力PCSクラス(0xD0) / 機器オブジェクト太陽光発電クラス(0xD0) / 機器オブジェクト電気自動車充放電器クラス(0xDB)
	slong	pvGenPwr;											// PV 発電電力(本製品以外のPV)[slong] 発電時：正の値、電力消費時：負の値	★	CMD81のPV発電電力(本製品以外のPV)				 = 全ての機器(0xF5)
	slong	revPwrFlow;											// 逆潮流電力[slong]														★	CMD81の逆潮流電力								 = 全ての機器(0xF5)
	schar	pcsErr[EPCS_PCS_ERR_LEN];							// PCSエラー																★	CMD81のPCSエラー								 = 機器オブジェクトスーパークラス(0x85 / 0x86)
	uchar	opeMode;											// 運転モード																★	E1通信ボードのシステムの運転モードに反映する	 = 機器オブジェクト蓄電池クラス(0xDA)(0xF0) / 機器オブジェクト電気自動車充放電器クラス(0xDA)
	ulong	ttlChargeEnergy;									// AC 積算充電電力量（インバータ部）[Wh]									★	通信ボードで算出								 = 機器オブジェクト蓄電池クラス(0xA8)
	ulong	ttlDischargeEnergy;									// AC 積算放電電力量（インバータ部）[Wh]									★	通信ボードで算出								 = 機器オブジェクト蓄電池クラス(0xA9)
	slong	chargeDischargePwr2;								// DC 充放電電力2（コンバータ部）[slong]									★	CMD81の蓄電池充放電電力(AC端計測値)				 = 機器オブジェクト蓄電池クラス(0xD3)
	ulong	ttlChargeEnergy2;									// DC 積算充電電力量2（コンバータ部）[Wh]									★	CMD81のDC積算蓄電池充電電力量2(コンバータ部)	 = 機器オブジェクト蓄電池クラス(0xD8)
	ulong	ttlDischargeEnergy2;								// DC 積算放電電力量2（コンバータ部）[Wh]									★	CMD81のDC積算蓄電池放電電力量2(コンバータ部)	 = 機器オブジェクト蓄電池クラス(0xD6)
	ulong	ttlPurchasePwr;										// 積算買電電力量[Wh]														★	CMD81の順潮流方向:積算買電電力量				 = 機器オブジェクトマルチ入力PCSクラス(0xF6)
	ulong	ttlSellingPwr;										// 積算売電電力量[Wh]														★	CMD81の逆潮流方向:積算売電電力量				 = 機器オブジェクトマルチ入力PCSクラス(0xF6)
	ulong	homeLoadPower;										// 家庭負荷電力[0.01kW]														★	CMD81の逆潮流電力、
																//																					   PV発電電力(本製品以外のPV) 、
																//																					   PCS充放電電力(AC端計測値) 、
																//																					   V2H瞬時充放電電力計測値(AC端計測値) 
																//																						   から家庭負荷電力を算出する			 = 全ての機器(0xF5)
	ulong	acChargeEffectiveCapacity;							// AC 充電実効容量 [Wh]														★	通信ボードで算出								 = 機器オブジェクト蓄電池クラス(0xA0)
	ulong	acDischargeEffectiveCapacity;						// AC 放電実効容量 [Wh]														★	通信ボードで算出								 = 機器オブジェクト蓄電池クラス(0xA1)
	ulong	acChargePossibleCapacity;							// AC 充電可能容量 [Wh]														★	通信ボードで算出								 = 機器オブジェクト蓄電池クラス(0xA2)
	ulong	acDishargePossibleCapacity;							// AC 放電可能容量 [Wh]														★	通信ボードで算出								 = 機器オブジェクト蓄電池クラス(0xA3)
	ulong	acChargePossibleAmount;								// AC 充電可能量 [Wh]														★	通信ボードで算出								 = 機器オブジェクト蓄電池クラス(0xA4)
	ulong	acDishargePossibleAmount;							// AC 放電可能量 [Wh]														★	通信ボードで算出								 = 機器オブジェクト蓄電池クラス(0xA5)
	uchar	v2hState;											// 車両充放電器状態															★	CMD81の車両充放電器運転動作状態					 = 機器オブジェクト電気自動車充放電器クラス(0xE1) 
	ulong	v2hDischargeableRemainingCapacity1;					// 車載電池の放電可能残容量1 [Wh]											★	CMD81の車載電池の放電可能残容量1				 = 機器オブジェクト電気自動車充放電器クラス(0xC2)
	uchar	v2hDischargeableRemainingCapacity3;					// 車載電池の放電可能残容量3 [％]											★	CMD81の車載電池の放電可能残容量３				 = 機器オブジェクト電気自動車充放電器クラス(0xC4)
	slong	v2hInstantPower;									// 車両充放電器瞬時充放電電力計測値 [slong]									★	CMD81のV2H瞬時充放電電力計測値(AC端計測値)		 = 機器オブジェクト電気自動車充放電器クラス(0xD3)
	ulong	v2hTtlDischargePower;								// 車載電池の積算放電電力量計測値 [0.001kWh]								★	CMD81の車載電池の積算放電電力量計測値
																//																								(AC端計測値)(トータル)			 = 機器オブジェクト電気自動車充放電器クラス(0xD6)
	ulong	v2hTtlChargePower;									// 車載電池の積算充電電力量計測値 [0.001kWh]								★	CMD81の車載電池の積算充電電力量計測値
																//																								(AC端計測値)(トータル)			 = 機器オブジェクト電気自動車充放電器クラス(0xD8)
	ulong	v2hRemainingCapacity1;								// 車載電池の電池残容量1 [Wh]												★	CMD81の車載電池の電池残容量1					 = 機器オブジェクト電気自動車充放電器クラス(0xE2)
	uchar	v2hRemainingCapacity3;								// 車載電池の電池残容量3 [％]												★	CMD81の車載電池の電池残容量３					 = 機器オブジェクト電気自動車充放電器クラス(0xE4)
	ulong	ttlChargeEnergyInv;									// AC積算充電電力量(インバータ部)[Wh]										★	CMD81の積算PCS充電電力量(AC端計測値)(系統連系時) = 機器オブジェクトマルチ入力PCSクラス(0xE0)
	ulong	ttlDischargeEnergyInv;								// AC積算放電電力量(インバータ部)[Wh]										★	CMD81の積算PCS放電電力量(AC端計測値)(系統連系時) = 機器オブジェクトマルチ入力PCSクラス(0xE3)
	ulong	v2hChargeableRemainCap;								// 車載電池の充電可能残容量値[Wh]											★	CMD81の車載電池の充電可能残容量値				 = 機器オブジェクト電気自動車充放電器クラス(0xCF)
	ulong	v2hChargeableCapValue;								// 車載電池の充電可能容量値[Wh]		0 ～ 999,999,999						★	CMD81の車載電池充電可能容量値					 = 機器オブジェクト電気自動車充放電器クラス(0xCE)
	ulong	v2hUsedCapacity1;									// 車載電池の使用容量値1[Wh]		0～999,999,999							★	CMD81の車載電池の使用容量値1					 = 機器オブジェクト電気自動車充放電器クラス(0xD0)
	ulong	v2hDischargeCapacity1;								// 車載電池の放電可能容量値1[Wh]	0～999,999,999							★	CMD81の車載電池の放電可能容量値1				 = 機器オブジェクト電気自動車充放電器クラス(0xC0)
	ulong	pcsTtlChargeEnergy;									// 積算PCS充電電力量(AC端計測値)(系統連系時)								★	CMD81の積算PCS充電電力量(AC端計測値)(系統連系時) = 機器オブジェクト分散型電源電力量メータクラス(0xE0)
	ulong	pcsTtlDischargeEnergy;								// 積算PCS放電電力量(AC端計測値)(系統連系時)								★	CMD81の積算PCS放電電力量(AC端計測値)(系統連系時) = 機器オブジェクト分散型電源電力量メータクラス(0xE2)
	ulong	v2hTtlChargeEnergy;									// 車載電池の積算充電電力量計測値(AC端計測値)(トータル)						★	CMD81の積算PCS充電電力量(AC端計測値)(系統連系時) = 機器オブジェクト分散型電源電力量メータクラス(0xE0)	
	ulong	v2hTtlDischargeEnergy;								// 車載電池の積算放電電力量計測値(AC端計測値)(トータル)						★	CMD81の積算PCS放電電力量(AC端計測値)(系統連系時) = 機器オブジェクト分散型電源電力量メータクラス(0xE2)
	uchar	pcsTtlChargeEnergyHistory[EINV_METER_ENERGER_HISTORY_LEN];
																// 積算電力量計測値履歴(交流入力)(E1)										★	INVマイコン側が実施する(QA:132)					 = 機器オブジェクト分散型電源電力量メータクラス(0xE1)
	uchar	v2hTtlChargeEnergyHistory[EINV_METER_ENERGER_HISTORY_LEN];
																// 積算電力量計測値履歴(交流入力)(C2)										★	INVマイコン側が実施する(QA:132)					 = 機器オブジェクト分散型電源電力量メータクラス(0xE1)
	uchar	pcsTtlTimeChargeEnergy[EINV_METER_ENERGER_LEN];		// 定時積算電力量計測値(交流入力)(E1)										★	INVマイコン側が実施する(QA:132)					 = 機器オブジェクト分散型電源電力量メータクラス(0xE6)
	uchar	v2hTtlTimeChargeEnergy[EINV_METER_ENERGER_LEN];		// 定時積算電力量計測値(交流入力)(C2)										★	INVマイコン側が実施する(QA:132)					 = 機器オブジェクト分散型電源電力量メータクラス(0xE6)

	uchar	pcsTtlDischargeEnergyHistory[EINV_METER_ENERGER_HISTORY_LEN];
																// 積算電力量計測値履歴(交流出力)(E1)										★	INVマイコン側が実施する(QA:132)					 = 機器オブジェクト分散型電源電力量メータクラス(0xE3)
	uchar	v2hTtlDischargeEnergyHistory[EINV_METER_ENERGER_HISTORY_LEN];
																// 積算電力量計測値履歴(交流出力)(C2)										★	INVマイコン側が実施する(QA:132)					 = 機器オブジェクト分散型電源電力量メータクラス(0xE3)
	uchar	pcsTtlTimeDischargeEnergy[EINV_METER_ENERGER_LEN];	// 定時積算電力量計測値(交流出力)(E1)										★	INVマイコン側が実施する(QA:132)					 = 機器オブジェクト分散型電源電力量メータクラス(0xE7)
	uchar	v2hTtlTimeDischargeEnergy[EINV_METER_ENERGER_LEN];	// 定時積算電力量計測値(交流出力)(C2)										★	INVマイコン側が実施する(QA:132)					 = 機器オブジェクト分散型電源電力量メータクラス(0xE7)

	//ulong	rsoc;												// rSOC値[0.1%]
	//ulong	remainingBtA;										// 電池残量[0.1Ah]
	//ulong	btVolt;												// 電池電圧[0.1V]
	//ulong	chargeDischargeCnt;									// 充放電回数[回]
	//ulong	pvSelfInputPwr;										// PV 自立入力電力[slong]
	//ulong	pvSelfInputVolt;									// PV 自立入力電圧[0.1V]
	//ulong	sysVolt;											// 系統電圧[0.1V]
	//schar	pcsErrRating;										// PCSエラーレーティング
	//schar	opeState[EPCS_OPE_STATE1_LEN];						// 運転状態
	//schar	opeState2[EPCS_OPE_STATE2_LEN];						// 運転状態2
	//ulong	opeTime;											// 通算稼働時間[時]
	//schar	sysErrState[EPCS_SYS_ERR_STATE_LEN];				// 系統異常状態
	//schar	equipFailState[EPCS_EQ_FAIL_STATE_LEN];				// 機器故障状態
	//schar	warningFlag[EPCS_WARN_FLAG_LEN];					// ワーニングフラグ
	//schar	errFlag1[EPCS_ERR_FLAG1_LEN];						// エラーフラグ1
	//schar	errFlag2[EPCS_ERR_FLAG2_LEN];						// エラーフラグ2
	//schar	errFlag3[EPCS_ERR_FLAG3_LEN];						// エラーフラグ3
	//ulong	pvGenPwr2;											// 太陽光発電電力（内蔵PV）[slong]
	//ulong	ttlPvGenPwr;										// 積算PV 発電電力量（本製品以外のPV）[Wh]
	//ulong	ttlPvGenPwr2;										// 積算太陽光発電電力量（内蔵PV）[Wh]
	//  M1M2追加
	//ulong	restrictedDischargePwr;								// 放電電力設定値[0.01kW]
	//uchar	suppressState;										// 出力抑制状態(CMD21)
	//uchar	v2hConnectorLockState;								// コネクタロック状態
	//ulong	v2hDischargeableRemainingCapacity2;					// 車載電池の放電可能残容量2 [0.1Ah]
	//uchar	v2hAvailabilityStatus;								// 車両接続・充放電可否状態
	//sshort	v2hInstantCurrent;									// 車両充放電器瞬時充放電電流計測値 [0.1Ah]
	//sshort	v2hInstantVolt;										// 車両充放電器瞬時充放電電圧計測値 [V]
//	uchar	v2hOpeMode;											// 車両充放電器運転モード
	//ulong	v2hRemainingCapacity2;								// 車載電池の電池残容量2 [0.1Ah]
//	ulong	v2hEstimateChargeEndMinute;							// 車両充電終了目安時間 [分]
	// 以下はCMD24前回値情報
//	uchar	opeModeCmd24;										// 運転モード(CMD24の前回値)
//	uchar	emergencyModeCmd24;									// 非常時運転モード(CMD24の前回値)
//	uchar	emergencyPercentCmd24;								// 非常時安心設定[%](CMD24の前回値)
//	uchar	startOrStopCmd24;									// 運転/停止(CMD24の前回値)
//	uchar	beforeHemsOpeMode;									// HEMS制御状態ONまたは待機時の自動運転モード
//	EPCS_HOMEINFO_REMOTE_E	isRemoteDispOn;					// ホーム画面リモート表示状態
//	EPCS_INV_OPE_STATE_E	inverterOpeState;				// インバータ動作状態
//	uchar						pwrCut_cmdGet;					// 停電状態(「電圧上昇抑制」でも丸め込みを行わない値)
	//ulong	restrictedChargePwr;								// 充電電力設定値[0.01kW]
	//ulong	restrictedPvGenPwr;									// 発電電力制限値[0.01kW]
	
}EPCS_OPESTTS_INFO_T;


/***********************************************************
* @struct	EPCS_REG_INFO_T
* @brief	登録情報構造体
************************************************************/
typedef struct _epcs_reg_info_t {
	schar				pushedUpState;							// 押し上げあり／なし														★	-
	EPCS_DATE_T		installationDate;							// 設置日																	★	-
	uchar				unitAddr;								// ユニットアドレス															★	-
	schar				pcsFwVersion[EPCS_PCS_FW_VER_LEN];		// PCS FWバージョン															★	-
	ushort				btCapacity;								// 蓄電池容量情報[0.1kWh]													★	CMD60の蓄電池定格電力容量					= 機器オブジェクト蓄電池クラス(0xD0)
	ushort				sysConf;								// システム構成																★	-
	ushort				btRatedVolt;							// 蓄電池定格電圧[0.1V]														★	CMD60の蓄電池定格電圧						= 機器オブジェクト蓄電池クラス(0xD2)
	ulong				chargePowerValueMin;					// 蓄電池最小充電電力値[0.01kW]												★	※最小：0W固定値							= 機器オブジェクト蓄電池クラス(0xC8)
	ulong				chargePowerValueMax;					// 蓄電池最大充電電力値[0.01kW]												★	最大：CMD60の蓄電池最大充電電力値			= 機器オブジェクト蓄電池クラス(0xC8)
	ulong				dischargePowerValueMin;					// 蓄電池最小放電電力値[0.01kW]												★	※最小：0W固定値							= 機器オブジェクト蓄電池クラス(0xC9)
	ulong				dischargePowerValueMax;					// 蓄電池最大放電電力値[0.01kW]												★	最大：CMD60の蓄電池最大放電電力値			= 機器オブジェクト蓄電池クラス(0xC9)
	ulong				chargePowerValueMinCut;					// 蓄電池最小充電電力値_自立時[0.01kW]										★	※最小：0W									= 機器オブジェクト蓄電池クラス(0xDC)
	ulong				chargePowerValueMaxCut;					// 蓄電池最大充電電力値_自立時[0.01kW]										★	最大：CMD60の蓄電池最大充電電力値_自立時	= 機器オブジェクト蓄電池クラス(0xDC)
	ulong				dischargePowerValueMinCut;				// 蓄電池最小放電電力値_自立時[0.01kW]										★	※最小：0W									= 機器オブジェクト蓄電池クラス(0xDD)
	ulong				dischargePowerValueMaxCut;				// 蓄電池最大放電電力値_自立時[0.01kW]										★	最大：CMD60の蓄電池最大放電電力値_自立時	= 機器オブジェクト蓄電池クラス(0xDD)


//	schar				maker[EPCS_PCS_MAKER_LEN];				// メーカー仕向け
//	schar				jBoxSerialNumber[EPCS_JBOX_SERIAL_NUM_LEN];	// J-BOX 製造番号
//	ushort				btRatedCapacity;						// 蓄電池定格容量（Ah）[0.1Ah]
/* T3T1_026 T3T1_052　蓄電池・V2H運転モード情報仕様変更　*/
//	ushort				sohMain;								// SOH値Main側 [EPCS013-001]
//	ushort				sohSuchar;									// SOH値Suchar側 [EPCS013-001]
/**/
}EPCS_REG_INFO_T;

/***********************************************************
* @struct	EPCS_OPEMODE_GET_T
* @brief	運転モード取得用構造体
************************************************************/
typedef struct _epcs_opemode_get_t {
	uchar						opeMode;						// 運転モード																★	E1通信ボードのシステムの運転モードに反映する = 機器オブジェクト蓄電池クラス(0xDA)(0xF0) / 機器オブジェクト電気自動車充放電器クラス(0xDA)
	uchar						emergencyPercent;				// 非常時安心設定[%] <- 放電下限値設定として流用　[T3T1_052]				★	CMD65：蓄電池放電下限値設定					 = 機器オブジェクト蓄電池クラス(0xF2)
	uchar						beforeAutoOpeMode;				// 前回の自動運転モード														★	-


//	uchar						emergencyMode;					// 非常時運転モード
//	uchar						startOrStop;					// 運転/停止
//	uchar						acUpperLimit;					// 蓄電池充電上限値設定(蓄電池)[%] [T3T1_052] [T3T1_022]
/* T3T1_026 T3T1_052　蓄電池・V2H運転モード情報仕様変更　*/
//	uchar						pvSurplusChargeCtrl;			// 余剰充電制御 [T3T1_052]
//	EPCS_TIME_T				pvSurplusStartTime;				// 余剰充電制御開始時刻 [T3T1_052]
//	EPCS_TIME_T				pvSurplusEndTime;				// 余剰充電制御停止時刻 [T3T1_052]
//	uchar						v2hDischargeTimeZoneBasis;		// 車両放電時間帯制限基準 [T3T1_052][T3T1_053][T3T1_167][ EPCS011-001]
//	uchar						testrunOutAtPwrCut;				// 試運転時停電出力 [T3T1_052]
/**/
//	uchar						beforeHemsOpeMode;				// HEMS制御状態ONまたは待機時の自動運転モード
//	EPCS_HOMEINFO_REMOTE_E	isRemoteDispOn;					// ホーム画面リモート表示状態
/* T3T1_022 T3T1_026 非常時安心設定仕様変更 */
//	uchar						userUpperLimit;					// ユーザ設定の蓄電池充電上限値設定 [T3T1_022]
//	uchar						userLowerLimit;					// ユーザ設定の蓄電池放電下限値設定 [T3T1_022]
/**/
}EPCS_OPEMODE_GET_T;

/***********************************************************
* @struct	EPCS_SPDATA_GET_T
* @brief	整定値・設定値取得用構造体
************************************************************/
typedef struct _epcs_spdata_get_t {
	ushort	pcsRatedPwr;										// 太陽光発電パワコン定格(PCS定格出力)[0.01kW]								★	CMD60のPCS定格出力(kW) をW換算する = 機器オブジェクト太陽光発電クラス(0xE8)


	//uchar	ovIdx;												// 系統過電圧レベルNo
	//uchar	ovtIdx;												// 系統過電圧時限No
	//uchar	uvIdx;												// 系統不足電圧レベルNo
	//uchar	uvtIdx;												// 系統不足電圧時限No
	//uchar	ofIdx;												// 周波数上昇レベルNo
	//uchar	oftIdx;												// 周波数上昇時限No
	//uchar	ufIdx;												// 周波数低下レベルNo
	//uchar	uftIdx;												// 周波数低下時限No
	//uchar	pasIdx;												// 位相急変検出レベルNo
	//uchar	hldIdx;												// 復帰時限No
	//uchar	aveIdx;												// 電圧上昇抑制（有効電力）No
	//uchar	otpIdx;												// 出力増減速度No
	//uchar	avr_mIdx;											// 電圧上昇抑制（無効電力）No
	//uchar	avtIdx;												// 電圧上昇抑制動作待機時限No
	//uchar	pwrFactCtrl;										// 力率一定制御[％]
	//uchar	campIdx;											// アンペアブレーカ値No
	//ushort	peakCtrlPwr;										// ピーク抑制電力[0.1kW]
	//uchar	pviIdx;												// PV自立運転充電引込電流No
	//uchar	tlogIdx;											// 運転ログ収集周期No
	//uchar	mtnIdx;												// メンテナンスモード設定No
	//ushort	pvPanelRatedPwr;									// 太陽光発電パネル定格(PVパネル定格)[W]※0xFFFF：未設定状態とする
	//ushort	genPwrCap;											// 発電電力上限[W]
	//uchar	pwrCutOtp;											// 停電時出力設定
	//uchar	pwrCutStart;										// 停電時出力開始
}EPCS_SPDATA_GET_T;

/***********************************************************
* @struct	EPCS_PV_CLASS_T
* @brief	太陽光発電クラス情報
************************************************************/
typedef struct _epcs_pv_class_t {

	uchar					otpCtrlSettingPer;					// 出力制御設定[%]															★	電力会社から取得した出力制御データの値、
																//																									またはHEMSで設定した値		= 機器オブジェクト太陽光発電クラス(0xA0)
	ushort					pvGenPwr2;							// 瞬時発電電力計測値[W]													★	CMD81の太陽光発電電力(AC端計測値)				= 機器オブジェクト太陽光発電クラス(0xE0)
	ulong					ttlPvGenPwr2;						// 積算発電電力量計測値[Wh]													★	CMD81の積算太陽光発電電力量(内蔵PV)(AC端計測値) = 機器オブジェクト太陽光発電クラス(0xE1)
	ulong					ttlSellingPwr;						// 積算売電電力量計測値[Wh]													★	CMD81の積算売電電力量							= 機器オブジェクト太陽光発電クラス(0xE3)
	uchar					pwrCut;								// 停電																		★	CMD81の停電										= 機器オブジェクト太陽光発電クラス(0xD0)
	uchar					otpSuppressState;					// 出力抑制状態																★	CMD81の出力抑制状態								= 機器オブジェクト太陽光発電クラス(0xD1)
//	uchar					opeState;							// 動作状態 EPCS_OPESTATE_ON：0x30	EPCS_OPESTATE_OFF：0x31
//	uchar					restrictedPvGenPwrPer;				// 発電電力制限設定1[％]
//	ushort					restrictedPvGenPwr;					// 発電電力制限設定2[W]
//	ushort					ratedPwrSys;						// 定格発電電力値(系統連係時) [W]
//	ushort					ratedPwrCut;						// 定格発電電力値(独立時) [W]
//	EPCS_PVGEN_NOTICE_T	pvGenNotice;						// 発電状態通知情報
}EPCS_PV_CLASS_T;

/***********************************************************
* @struct	EPCS_V2H_SETINFO_GET_T
* @brief	V2H設定情報取得構造体
************************************************************/
typedef struct _epcs_v2h_setinfo_get_t {

	schar	v2hModelName[EPCS_V2H_MODEL_NAME_LEN];		// 車両充放電器機種名																★	※機器オブジェクトスーパークラス(0x8C)
	schar	v2hSerialNumber[EPCS_V2H_SERIAL_NUM_LEN];	// 車両充放電器製造番号																★	※機器オブジェクトスーパークラス(0x83)
	schar	v2hType[EPCS_V2H_TYPE_LEN];					// 車両充放電器タイプ																★	CMD90の車両充放電器タイプ		= 機器オブジェクト電気自動車充放電器クラス(0xCC)
	ulong	v2hRatedCharge;								// 車両充放電器定格充電能力[W]		0～999,999,999									★	CMD90の車両充放電器定格充電能力 = 機器オブジェクト電気自動車充放電器クラス(0xC5)
	ulong	v2hRatedDischarge;							// 車両充放電器定格放電能力[W]		0～999,999,999									★	CMD90の車両充放電器定格放電能力 = 機器オブジェクト電気自動車充放電器クラス(0xC6)
	ulong	v2hDischargeSetValue;						// 車載電池放電量設定値[Wh]			0 ～ 999,999,999								★	通信ボードで算出				= 機器オブジェクト電気自動車充放電器クラス(0xEA)
	uchar	vehicleIdLen;								// 車両IDデータサイズ				0 ～ 24											★	CMD90の車両ＩＤ					= 機器オブジェクト電気自動車充放電器クラス(0xE6)
	schar	vehicleId[EPCS_VEHICLE_ID_LEN];					// 車両ID																		★	CMD90の車両ＩＤ					= 機器オブジェクト電気自動車充放電器クラス(0xE6)

//	schar	v2hFwVersion[EPCS_V2H_FW_VER_LEN];			// 車両充放電器FW バージョン
//	ulong	v2hDischargeCapacity1;						// 車載電池の放電可能容量値1[Wh]	0～999,999,999
//	ushort	v2hDischargeCapacity2;						// 車載電池の放電可能容量値2[0.1Ah]	0～32,766
//	ulong	v2hUsedCapacity1;							// 車載電池の使用容量値1[Wh]		0～999,999,999
//	ushort	v2hUsedCapacity2;							// 車載電池の使用容量値2[0.1Ah]		0～32,766
//	ushort	v2hRatedVoltSys;							// 車両充放電器定格電圧[V]			0～32,766
//	ushort	v2hRatedVoltCut;							// 車両充放電器定格電圧独立時[V]	0～32,766
//	ulong	v2hChargePwrMin;							// 車載電池最小充電電力値[W]		0～999,999,999
//	ulong	v2hChargePwrMax;							// 車載電池最大充電電力値[W]		0～999,999,999
//	ulong	v2hDishargePwrMin;							// 車載電池最小放電電力値[W]		0～999,999,999
//	ulong	v2hDishargePwrMax;							// 車載電池最大放電電力値[W]		0～999,999,999
//	ushort	v2hChargeCurrentMin;						// 車載電池最小充電電流値[0.1Ah]	0～32,766
//	ushort	v2hChargeCurrentMax;						// 車載電池最大充電電流値[0.1Ah]	0～32,766
//	ushort	v2hDishargeCurrentMin;						// 車載電池最小放電電流値[0.1Ah]	0～32,766
//	ushort	v2hDishargeCurrentMax;						// 車載電池最大放電電流値[0.1Ah]	0～32,766
//	ulong	v2hChargeAmountSet1;						// 車載電池充電量設定値1[Wh]		0～999,999,999
//	ushort	v2hChargeAmountSet2;						// 車載電池充電量設定値2[0.1Ah]		0～32,766
//	ulong	v2hPowerSetCharge;							// 車載電池充電電力設定値[W]		0～999,999,999
//	ulong	v2hPowerSetDiscCharge;						// 車載電池放電電力設定値[W]		0～999,999,999
//	ushort	v2hCurrentSetCharge;						// 車載電池充電電流設定値[0.1Ah]	0～32,766
//	ushort	v2hCurrentSetDischarge;						// 車載電池放電電流設定値[0.1Ah]	0～32,766
//	ulong	v2hChargeableCapValue;						// 車載電池の充電可能容量値[Wh]		0 ～ 999,999,999
//	schar	v2hChrDisTimeType;							// 充放電時間設定制限タイプ
}EPCS_V2H_SETINFO_GET_T;

/***********************************************************
* @struct	EPCS_SERIAL_NUM_T
* @brief	PCS機種名・製造番号情報構造体
************************************************************/
typedef struct _epcs_serial_num_t {
	schar	pcsModelName[EPCS_PCS_MODEL_NAME_LEN];				// PCS機種名																★	※機器オブジェクトスーパークラス(0x8C)
	schar	pcsSerialNumber[EPCS_PCS_SERIAL_NUM_LEN];			// PCS製造番号																★	※機器オブジェクトスーパークラス(0x83)
}EPCS_SERIAL_NUM_T;


/***********************************************************
* @struct	EPCS_SPDATA_SET_T
* @brief	整定値・設定値設定用構造体
************************************************************/
typedef struct _epcs_spdata_set_t {								//																			★	※処理なし
	ulong					changedFlag;						// 変更フラグ
	EPCS_SPDATA_GET_T	spdata;								// 整定値情報
}EPCS_SPDATA_SET_T;

/***********************************************************
* @struct	EPCS_PVINFO_SET_T
* @brief	太陽光発電情報設定構造体
************************************************************/
typedef struct _epcs_pvinfo_set_t {

	ulong	changeFlg;					/* 変更フラグ					*/
	uchar	fitType;					/* FIT契約タイプ				*/
//	uchar	selfConsumptionType;		/* 自家消費タイプ				*/
//	ulong	facilityCertifiedCapacity;	/* 設備認定容量[W]			*/

} EPCS_PVINFO_SET_T;



/***********************************************************
* @struct	EPCS_V2H_SETINFO_SET_T
* @brief	V2H設定情報設定用構造体
************************************************************/
typedef struct _epcs_v2h_setinfo_set_t {
	ulong	setChangeFlag;								// 変更有無を示すフラグ																★	-
	ulong	v2hChargeAmountSet1;						// 車載電池 充電量設定値1[Wh]		0～999,999,999									★	通信ボードで算出	= 機器オブジェクト電気自動車充放電器クラス(0xE7)
	ulong	v2hPowerSetCharge;							// 車載電池 充電電力設定値[W]		0～999,999,999									★	通信ボードで算出	= 機器オブジェクト電気自動車充放電器クラス(0xEB)
	ulong	v2hPowerSetDiscCharge;						// 車載電池 放電電力設定値[W]		0～999,999,999									★	通信ボードで算出	= 機器オブジェクト電気自動車充放電器クラス(0xEC)
	ulong	v2hDischargeSetValue;						// 車載電池 放電量設定値[Wh]			0～999,999,999								★	通信ボードで算出	= 機器オブジェクト電気自動車充放電器クラス(0xEA)

//	ushort	v2hChargeAmountSet2;						// 車載電池 充電量設定値2[0.1Ah]		0～32,766
//	ushort	v2hCurrentSetCharge;						// 車載電池 充電電流設定値[0.1Ah]	0～32,766
//	ushort	v2hCurrentSetDischarge;						// 車載電池 放電電流設定値[0.1Ah]	0～32,766
	// ReleaseK対応
	
}EPCS_V2H_SETINFO_SET_T;

/***********************************************************
* @struct	EPCS_V2H_TTL_ENERGY_RESET_T
* @brief	V2H積算情報リセット通知構造体
************************************************************/
typedef struct _epcs_v2h_ttl_energy_reset_t {
	ushort	setChangeFlag;								// 変更有無を示すフラグ																★	-
	uchar	v2hTtlDishargeEnergyReset;					// 車載電池の積算放電電力量リセット設定												★	E1通信ボードでリセット後の積算値算出 = 機器オブジェクト電気自動車充放電器クラス(0xD7)
	uchar	v2hTtlChargeEnergyReset;					// 車載電池の積算充電電力量リセット設定												★	E1通信ボードでリセット後の積算値算出 = 機器オブジェクト電気自動車充放電器クラス(0xD9)
}EPCS_V2H_TTL_ENERGY_RESET_T;

// enot.h
/***********************************************************
* @struct	ファンクション５構造体(リモコン内部エラー状態取得)
* @breaf	ファンクション５用送受信メッセージ
************************************************************/
typedef struct enot_err_st {
	ulong				errState;		//リモコン内部エラー状態																			★	-
	ulong				errStateNow;	//現時点のリモコン内部エラー状態(BC551･BC562の24時間ﾀｲﾏｰ制御無)										★	-

	//	ulong				newest_err;		//最新リモコン内部エラー
}ENOT_ERRST_T;



/***********************************************************
* @struct	ESCH_DATE_TIME_T
* @brief	esch日付型情報
************************************************************/
typedef struct _esch_date_time_t_ {
	ushort	year;					/* 年(1970～2037) */
	uchar	month;					/* 月（1～12）*/
	uchar	day;					/* 日（1～31）*/
	uchar	hour;					/* 時（0～23）*/
	uchar	minute;					/* 分（0～59）*/
	uchar	second;					/* 秒（0～59）*/
}ESCH_DATE_TIME_T;


/***********************************************************
* @struct	EPCS_OPEMODE_SET_T
* @brief	運転モード設定用構造体
************************************************************/
typedef struct _epcs_opemode_set_t {
	ulong						setChangeFlag;					// 変更フラグ																★	-								
	uchar						opeMode;						// 運転モード																★	E1通信ボードのシステムの運転モードに反映する = 機器オブジェクト蓄電池クラス(0xDA)(0xF0) / 機器オブジェクト電気自動車充放電器クラス(0xDA)
	uchar						emergencyPercent;				// 非常時安心設定 <- 放電下限値設定として流用　[T3T1_052]					★	CMD65：蓄電池放電下限値設定					 = 機器オブジェクト蓄電池クラス(0xF2)	
	EPCS_CHARGE_TIME_ZONE_T	chargeTimeZone;						// 充放電時間帯																★	E1通信ボードのシステムの運転モードの自動運転（売電モード、グリーンモード）
																//																															 = 機器オブジェクト蓄電池クラス(0xF4)
	EPCS_REMOTE_E			remoteState;						// リモート操作状態															★	E1通信ボードのシステムの運転モード			 = 機器オブジェクト蓄電池クラス(0xDA) / 機器オブジェクト電気自動車充放電器クラス(0xDA)
	EPCS_LIMIT_E			limitCtrl;							// 充放電制御／制御解除														★	E1通信ボードのシステムの運転モード			 = 機器オブジェクト蓄電池クラス(0xDA) / 機器オブジェクト電気自動車充放電器クラス(0xDA)

//	uchar						emergencyMode;					// 非常時運転モード
//	uchar						startOrStop;					// 運転/停止
//	uchar						acUpperLimit;					// 蓄電池充電上限値設定(蓄電池)[%] [T3T1_052]
/* T3T1_026 T3T1_052　蓄電池・V2H運転モード情報仕様変更　*/
//	uchar						pvSurplusChargeCtrl;			// 余剰充電制御 [T3T1_052]
//	EPCS_TIME_T				pvSurplusStartTime;				// 余剰充電制御開始時刻 [T3T1_052]
//	EPCS_TIME_T				pvSurplusEndTime;				// 余剰充電制御停止時刻 [T3T1_052]
//	uchar						v2hDischargeTimeZoneBasis;		// 車両放電時間帯制限基準　[T3T1_052][T3T1_053]
//	uchar						testrunOutAtPwrCut;				// 試運転時停電出力 [T3T1_052]
/**/
//	EPCS_HEMS_STATE_E		hemsState;						// HEMS制御状態
	// 強制設定フラグは、変更フラグ無しにIF呼び元で設定可能
	// eschからHEMS制御状態を強制的に設定する為に使用する
	// TRUEを設定し、呼び出す場合、epcsではV2H動作状態を問わずに
	// HEMS制御状態を変更する
//	bool_t					forceSetFlg;					// 強制設定フラグ
}EPCS_OPEMODE_SET_T;

/***********************************************************
* @struct	ESCH_DAILY_SCHED_DATA_T
* @brief	出力制御日間スケジュール情報
************************************************************/
typedef struct _esch_daily_sched_data_t_ {
	ESCH_DATE_TIME_T	tartgetDate;							/* 出力制御対象日付 */
	uchar					schedData[ESCH_DAILY_SCHED_DATA_LEN];	/* 1日分の出力制御値 */
}ESCH_DAILY_SCHED_DATA_T;


/***********************************************************
* @struct	EPCS_RESTRICTED_VALUE_T
* @brief	充放電電力設定値構造体
************************************************************/
typedef struct _epcs_restricted_value_t {

	ushort	bt_chargeSetValue;		/* 蓄電池充電電力設定値[0.01kW]		*///																★	※								
	ushort	bt_dischargeSetValue;	/* 蓄電池放電電力設定値[0.01kW]		*///																★	※								
	ulong	v2h_chargeSetValue;		/* 車載電池充電電力設定値[0.01kW]	*///																★	※								
	ulong	v2h_dischargeSetValue;	/* 車載電池放電電力設定値[0.01kW]	*///																★	※								

} EPCS_RESTRICTED_VALUE_T;

#if 0

/***********************************************************
* @struct	EenlEvtSttsChangeReq_t
* @brief	状態変化通知イベントメッセージ構造体
************************************************************/
typedef struct {
	FrmMsgHead_t				header;																				// ★ 設定不要
	ulong						changedFlag;					/*  変更有無を示すフラグ */							// ★ -
	uchar						opeMode;						/*  変更後の運転モード */							// ★	E1通信ボードのシステムの運転モードに反映する = 機器オブジェクト蓄電池クラス(0xDA)(0xF0) / 機器オブジェクト電気自動車充放電器クラス(0xDA)
	uchar						emergencyPercent;				/*  変更後の非常時安心設定 */						// ★	CMD65：蓄電池放電下限値設定					 = 機器オブジェクト蓄電池クラス(0xF2)	
	uchar						timeZoneOpeMode;				/*  変更後の充放電時間帯の自動運転モード */			// ★	※
	EenlChargeTimeZone_t		chargeTimeZone;					/*  変更後の充放電時間帯 */							// ★	E1通信ボードのシステムの運転モードの自動運転（売電モード、グリーンモード） = 機器オブジェクト蓄電池クラス(0xF4)
	uchar						operation;						/*  変更後の運転中状態 */							// ★	※notify
	uchar						pwrCut;							/*  変更後の停電状態 */								// ★	※notify
	ushort						soh;							/*  変更後のSOH値 */								// ★	※notify
	uchar						suppressState;					/*  変更後の出力抑制状態 */							// ★	※notify
} EenlEvtSttsChangeReq_t;

/***********************************************************
* @struct	EenlEvtSpChangeReq_t
* @brief	設定変更通知イベントメッセージ構造体
************************************************************/
typedef struct {
	FrmMsgHead_t		header;																							// ★ 設定不要
	ulong				changedFlag;							/*  変更有無を示すフラグ */								// ★ -
	ulong				addChangedFlag;							/*  M1M2以降に追加された設定値の変更有無を示すフラグ */	// ★ -
	ushort				otp;									/*  変更後の出力増減速度[分] */							// ★	CMD68、CMD69の出力増減変化時間 = 機器オブジェクト太陽光発電クラス(0xB3)
	uchar				pwrFactCtrl;							/*  変更後の力率一定制御 */								// ★	CMD68、CMD69の力率一定制御			= 機器オブジェクト太陽光発電クラス(0xC0)
	ushort				genPwrCap;								/*  変更後の発電電力上限 */								// ★	※発電電力上限(上限クリップ設定値)	= 機器オブジェクト太陽光発電クラス(0xB4)
	uchar				fitType;								/* FIT契約タイプ				*/						// ★	※試運転時設定のFIT契約タイプ		= 機器オブジェクト太陽光発電クラス(0xC1)
	uchar				selfConsumptionType;					/* 自家消費タイプ				*/						// ★	※試運転時設定の自家消費タイプ　	= 機器オブジェクト太陽光発電クラス(0xC2)
	ulong				facilityCertifiedCapacity;				/* 設備認定容量[W]				*/						// ★	※設備認定容量						= 機器オブジェクト太陽光発電クラス(0xC3)
} EenlEvtSpChangeReq_t;

/***********************************************************
* @struct	EenlEvtV2hSttsChangeReq_t
* @brief	V2H状態変化イベントメッセージ構造体
************************************************************/
typedef struct {
	FrmMsgHead_t		header;																							// ★ 設定不要
	ulong				changedFlag;					/* 	変更有無を示すフラグ */										// ★ -
	uchar				v2hConnectorLockState;			/* 	変更後のコネクタロック状態 */								// ★	コネクタロック状態				= V2Hのコネクタロック状態
	uchar				v2hAvailabilityStatus;			/* 	変更後の車両接続・充放電可否状態 */							// ★	CMD81の車両接続・充放電可否状態	= 機器オブジェクト電気自動車充放電器クラス(0xC7) 
} EenlEvtV2hSttsChangeReq_t;

/***********************************************************
* @struct	EenlEvtV2hSttingChangeReq_t
* @brief	V2H設定変更イベントメッセージ構造体
************************************************************/
typedef struct {
	FrmMsgHead_t		header;																							// ★ 設定不要
	ulong				setEventType;						/* V2H設定情報設定変更フラグ */								// ★ -
	ulong				v2hChargeAmountSet1;				/* 車載電池充電量設定値1 */									// ★	通信ボードで算出	= 機器オブジェクト電気自動車充放電器クラス(0xE7)
	ushort				v2hChargeAmountSet2;				/* 車載電池充電量設定値2 */									// ★	※機器オブジェクト電気自動車充放電器クラス(0xE9) → 削除
	ulong				v2hPowerSetCharge;					/* 車載電池充電電力設定値 */								// ★	通信ボードで算出	= 機器オブジェクト電気自動車充放電器クラス(0xEB)
	ulong				v2hPowerSetDiscCharge;				/* 車載電池放電電力設定値 */								// ★	通信ボードで算出	= 機器オブジェクト電気自動車充放電器クラス(0xEC)
	ushort				v2hCurrentSetCharge;				/* 車載電池充電電流設定値 */								// ★	※機器オブジェクト電気自動車充放電器クラス(0xED) → 削除
	ushort				v2hCurrentSetDischarge;				/* 車載電池放電電流設定値 */								// ★	※機器オブジェクト電気自動車充放電器クラス(0xEE) → 削除 
	ushort				v2hReratedVolt;						/* 車両充放電器定格電圧 */									// ★	※２００V固定値、削除予定、処理変更要
	uchar				chargeTimerSetting;					/* 充電タイマ設定 */										// ★	※処理なし、削除予定
	ulong				v2hDischargeSetValue;				/* 車載電池放電量設定値[Wh]			0～999,999,999 */		// ★	通信ボードで算出	= 機器オブジェクト電気自動車充放電器クラス(0xEA)
	ulong				changedProcessMsgId;				/* 変更元プロセスのメッセージID(CPORT) */					// ★	-
} EenlEvtV2hSttingChangeReq_t;
#endif


//函数说明：運転モード取得【从INV取得】
//参数：[out]運転モード取得用構造体
slong epcs_sync_getOpeMode(
	EPCS_OPEMODE_GET_T* opeModeGet
);
//中间模块函数名：extern slong EMDTR_SyncGetOpeMode(EmdtrOpeModeGet_t* opeModeGet);

//函数说明：運転モード設定【向INV设定】
//参数：[in]運転モード設定用構造体
slong epcs_sync_setOpeMode(
	const EPCS_OPEMODE_SET_T* opeModeSet
);
//中间模块函数名：extern slong EMDTR_SyncSetOpeMode(const EmdtrOpeModeSet_t* opeModeSet);

//函数说明：充放電時間帯取得【从INV取得】
//参数：[in]自動運転モード
//参数：[out]充放電時間帯情報構造体
slong epcs_sync_getChargeTimeZone(
	uchar autoOpeMode,
	EPCS_CHARGE_TIME_ZONE_T* chargeTomeZone
);
//中间模块函数名：extern slong EMDTR_SyncGetChargeTimeZone(uchar autoOpeMode, EmdtrChargeTimeZone_t* chargeTomeZone);


//函数说明：整定値・設定値取得【从INV取得】
//参数：[out]整定値・設定値取得用構造体
slong epcs_sync_getSpData(
	EPCS_SPDATA_GET_T* spDataGet
);
//中间模块函数名：extern slong EMDTR_SyncGetSpData(EmdtrSpDataGet_t* spDataGet);

//函数说明：整定値・設定値設定【向INV设定】
//参数：[in]整定値・設定値設定用構造体
slong epcs_sync_setSpData(
	const EPCS_SPDATA_SET_T* spDataSet
);
//中间模块函数名：extern slong EMDTR_SyncSetSpData(const EmdtrSpDataSet_t* spDataSet);

//函数说明：登録情報取得【从INV取得】
//参数：[in]共通地址
//参数：[out]登録情報構造体
slong epcs_sync_getRegInfo(
	uchar unitAddr,
	EPCS_REG_INFO_T* regInfo
);
//中间模块函数名：extern slong EMDTR_SyncGetRegInfo(uchar unitAddr, EmdtrRegInfo_t* regInfo);

//函数说明：機種名・製造番号取得【从INV取得】
//参数：[in]共通地址
//参数：[out]PCS機種名・製造番号情報構造体
slong epcs_sync_getPcsModelSerialNumber(
	uchar unitAddr,
	EPCS_SERIAL_NUM_T* pcsSerial
);
//中间模块函数名：extern slong EMDTR_SyncGetPcsModelSerialNumber(uchar unitAddr, EmdtrSerialNum_t* pcsSerial);

//函数说明：動作情報取得【从INV取得】
//参数：[out]動作情報構造体
slong epcs_sync_getOpeSttsInfo(
	EPCS_OPESTTS_INFO_T* opeSttsInfo
);
//中间模块函数名：extern slong EMDTR_SyncGetOpeSttsInfo(EmdtrOpeSttsInfo_t* opeSttsInfo);

//函数说明：充電電力量設定【向INV设定】
//参数：[in]充電電力量
slong epcs_sync_setChargePwr(
	ulong charge
);
//中间模块函数名：extern slong EMDTR_SyncSetChargePwr(ulong charge);

//函数说明：放電電力量設定【向INV设定】
//参数：[in]放電電力量
slong epcs_sync_setDischargePwr(
	ulong discharge
);
//中间模块函数名：extern slong EMDTR_SyncSetDischargePwr(ulong discharge);

//函数说明：状態変化通知イベント登録【向INV设定】
//参数：Port番号
//参数：状態変化通知イベント種別
//★如果不发送事件消息，中间模块侧可以不追加此函数
slong epcs_sync_regSttsChangeEvent(
	ulong receiver,
	ulong eventType
);

//函数说明：充放電時間帯設定【向INV设定】
//参数：[in]充放電模式
//参数：[in]充放電時間帯情報構造体
slong epcs_sync_setChargeTimeZone(
	uchar autoOpeMode,
	const EPCS_CHARGE_TIME_ZONE_T* chargeTimeZone
);
//中间模块函数名：extern slong EMDTR_SyncSetChargeTimeZone(uchar autoOpeMode, const EmdtrChargeTimeZone_t* chargeTimeZone);

//函数说明：自動運転モード予約設定【向INV设定】
//参数：[in]充放電模式
slong epcs_sync_setAutoOpeModeReserve(
	uchar autoOpeMode
);
//中间模块函数名：extern slong EMDTR_SyncSetAutoOpeModeReserve(uchar autoOpeMode);

//函数说明：太陽光発電クラス情報取得【从INV取得】
//参数：[out]太陽光発電クラス情報
slong epcs_sync_getPvClassInfo(
	EPCS_PV_CLASS_T* pvClassInfo
);
//中间模块函数名：extern slong EMDTR_SyncGetPvClassInfo(EmdtrPvClass_t* pvClassInfo);

//函数说明：発電電力制限値設定【向INV设定】
//参数：[in]発電電力制限値設定/取得種別
//参数：[in]設定値
slong epcs_sync_setRestrictedPvGenPwr(
	ulong type,
	ulong power
);
//中间模块函数名：extern slong EMDTR_SyncSetRestrictedPvGenPwr(ulong type, ulong power);

//函数说明：充電電力設定値設定【向INV设定】
//参数：[in]設定値
slong epcs_sync_setRestrictedChargePwr(
	ulong	power
);
//中间模块函数名：extern slong EMDTR_SyncSetRestrictedChargePwr(ulong power);

//函数说明：V2H車両接続確認【从INV取得】
//参数：[in]接続確認
//参数：[out]接続確認結果
slong epcs_sync_chkV2hConnConf(
	uchar connConf,
	schar* pcsRes
);
//中间模块函数名：extern slong EMDTR_SyncChkV2hConnConf(uchar connConf, schar* pcsRes);

//函数说明：V2H設定情報取得【从INV取得】
//参数：[out]V2H設定情報取得構造体
slong epcs_sync_getV2hSetInfo(
	EPCS_V2H_SETINFO_GET_T* v2hSetInfoGet
);
//中间模块函数名：extern slong EMDTR_SyncGetV2hSetInfo(EmdtrV2hSetInfoGet_t* v2hSetInfoGet);

//函数说明：V2H設定情報設定【向INV设定】
//参数：[in]V2H設定情報設定用構造体
slong epcs_sync_setV2hSetInfo(
	EPCS_V2H_SETINFO_SET_T* v2hSetInfoSet
);
//中间模块函数名：extern slong EMDTR_SyncSetV2hSetInfo(EmdtrV2hSetInfoSet_t* v2hSetInfoSet);

//函数说明：V2H積算情報リセット【向INV设定】
//参数：[in]V2H積算情報リセット通知構造体
slong epcs_sync_setV2hTtlEnergyReset(
	EPCS_V2H_TTL_ENERGY_RESET_T* ttlEnergyReset
);
//中间模块函数名：extern slong EMDTR_SyncSetV2hTtlEnergyReset(EmdtrV2hTtlEnergyReset_t* ttlEnergyReset);

//函数说明：V2H状態変化通知イベント登録【向INV设定】
//参数：Port番号
//参数：状態変化通知イベント種別
//★如果不发送事件消息，中间模块侧可以不追加此函数
slong epcs_sync_regV2hSttsChangeEvent(
	ulong receiver,
	ulong eventType
);

//函数说明：V2H設定変更通知イベント登録【向INV设定】
//参数：Port番号
//参数：状態変化通知イベント種別
//参数：V2H状態変化通知イベント種別
//★如果不发送事件消息，中间模块侧可以不追加此函数
slong epcs_sync_regV2hSttingChangeEvent(									// V2H設定変更通知イベント登録
	ulong receiver,
	ulong setEventType,
	ulong modeEventType
);


//函数说明：放電電力設定値設定【向INV设定】
//参数：[in]放電電力設定値（0 ～ 99990[W]）
slong epcs_sync_setRestrictedDischargePwr(
	ulong power
);
//中间模块函数名：extern slong EMDTR_SyncSetRestrictedDischargePwr(ulong power);

//函数说明：追加設定変更通知イベント登録【向INV设定】
//参数：Port番号
//参数：設定変更通知イベント種別
//参数：追加設定変更通知イベント種別
//★如果不发送事件消息，中间模块侧可以不追加此函数
slong epcs_sync_regAddMSpChangeEvent(
	ulong receiver,
	ulong eventType,
	ulong addEventType
);
//中间模块函数名：extern slong EMDTR_SyncRegAddMSpChangeEvent(ulong receiver,ulong eventType,ulong addEventType);

//函数说明：AC充電量設定値設定【向INV设定】
//参数：[in]設定値
slong epcs_sync_setAcChargePwr(
	ulong charge
);
//中间模块函数名：extern slong EMDTR_SyncSetAcChargePwr(ulong charge);

//函数说明：AC放電量設定値設定【向INV设定】
//参数：[in]設定値
slong epcs_sync_setAcDischargePwr(
	ulong discharge
);
//中间模块函数名：extern slong EMDTR_SyncSetAcDischargePwr(ulong discharge);

//函数说明：充放電電力設定値取得【从INV取得】
//参数：[out]充放電電力設定値構造体
slong epcs_sync_getRestrictedPwrValue(EPCS_RESTRICTED_VALUE_T* restrictedValue);
//中间模块函数名：extern slong EMDTR_SyncGetRestrictedPwrValue(EmdtrRestictedValue_t* restrictedValue);

//函数说明：太陽光発電情報(リモコン設定値)設定【向INV设定】
//参数：[in]太陽光発電情報設定構造体
slong epcs_sync_setPvInfoNotComm(EPCS_PVINFO_SET_T* pvInfoSet);
//中间模块函数名：extern slong EMDTR_SyncSetPvInfoNotComm(EmdtrPvInfoSet_t* pvInfoSet);


//函数说明：V2H設定情報取得(非通信)【从INV取得】
//参数：[out]V2H設定情報取得構造体
slong epcs_sync_getV2hSetInfoCurrent_NotComm(EPCS_V2H_SETINFO_GET_T* v2hSetInfoGet);
//中间模块函数名：extern slong EMDTR_SyncGetV2hSetInfoCurrentNotComm(EmdtrV2hSetInfoGet_t* v2hSetInfoGet);

//函数说明：蓄電池クラス 0xC1(充電方式)の設定処理【向INV设定】
//参数：充電方式
slong epcs_sync_setChargeMode(uchar chargeMode);
//中间模块函数名：extern slong EMDTR_SyncSetChargeMode(uchar chargeMode);

//函数说明：蓄電池クラス 0xC2(放電方式)の設定処理【向INV设定】
//参数：放電方式
slong epcs_sync_setDischargeMode(uchar dischargeMode);
//中间模块函数名：extern slong EMDTR_SyncSetDischargeMode(uchar dischargeMode);


//函数说明：蓄電池設定場所変更通知
//参数：設置場所の定義
//見守りサーバ（ewtch）模块侧的函数接口
extern	void	ensv_async_notify_LocationChanged(ENSV_LOCATION_E location);		/*蓄電池設定場所変更通知	*/
//中间模块函数名：extern void EMDTR_AsyncNotifyLocationChanged(EmdtrLocation_t location);

//函数说明：V2H設置場所変更通知【向EENL设定】
//参数：設置場所の定義
//見守りサーバ（ewtch）模块侧的函数接口
extern	void	ensv_async_notify_V2hLocationChanged(ENSV_LOCATION_E location);	/*V2H設置場所変更通知		*/
//中间模块函数名：extern void EMDTR_AsyncNotifyV2hLocationChanged(EmdtrLocation_t location);

//函数说明：設定変更通知イベント登録IF 【向esch设定】
//参数：	ポート番号
//参数：	イベント種別
//参数：	通知要否フラグ
//★如果不发送事件消息，中间模块侧可以不追加此函数
extern slong esch_async_regSettingChangeEvent(slong receiver, ulong eventType, uchar noticerflg);

//函数说明：次回アクセス日時取得要求IF【从ESCH取得】
//参数：[out]esch日付型情報
extern slong esch_sync_getNextAccessDate(ESCH_DATE_TIME_T* nextDate);
//中间模块函数名：extern slong EMDTR_SyncGetNextAccessDate(EmdtrDateTime_t* nextDate);

//函数说明：出力制御 対象／対象外取得IF【从ESCH取得】
//参数：[out]出力制御 対象/対象外格納先アドレス
slong esch_sync_getOutputCtrlOnOff(uchar* outputCtrlOnOff);
//中间模块函数名：extern slong EMDTR_SyncGetOutputCtrlOnOff(uchar* outputCtrlOnOff);

//函数说明：出力制御履歴取得IF【从ESCH取得】
//参数：[in]esch日付型情報
//参数：[out]出力制御日間スケジュール情報
slong esch_sync_getOutputCtrlHistory(ESCH_DATE_TIME_T* targetDate, ESCH_DAILY_SCHED_DATA_T* dailySchData);
//中间模块函数名：extern slong EMDTR_SyncGetOutputCtrlHistory(EmdtrDateTime_t* targetDate, EmdtrDailySchedData_t* dailySchData);

//函数说明：操作音鳴動
extern slong ebuz_async_soundTouchBuz(void);
//中间模块函数名：extern slong EMDTR_AsyncSoundTouchBuz(void);


// ★★★★★★★★★★★★如果不发送事件消息，EENL侧需要追加以下接口，在各种状態发生変化时，需要中间模块（emdtr）侧调用以下函数向EENL通知各种状態変化 ★★★★★★★★★★★★
#if 0
//#include "eenl_g.h" ★
/* einv側の試運転状態設定イベント */
extern slong EENL_EventSetTestRunState(const EenlTestRunState_t state);

/* einv側の状態変化通知イベント */
extern slong EENL_EventSttsChange(EenlEvtSttsChangeReq_t* sendMsg);

/* einv側の設定変更通知イベント */
extern slong EENL_EventSpChange(EenlEvtSpChangeReq_t* sendMsg);

/* einv側のV2H状態変化イベント */
extern slong EENL_EventV2hSttsChange(EenlEvtV2hSttsChangeReq_t* sendMsg);

/* einv側のV2H設定変更イベント */
extern slong EENL_EventV2hSettingChange(EenlEvtV2hSttingChangeReq_t* sendMsg);

/* esch側の設定変更通知イベント */
extern slong EENL_EventSettingChange(EenlEvtSttingChangeReq_t* sendMsg);
#endif

//###################################　↑↑↑　ビルドするため、一時定義↑↑↑　将来は削除する予定　######################################################


#endif	//__EENL_OTHER_H__
