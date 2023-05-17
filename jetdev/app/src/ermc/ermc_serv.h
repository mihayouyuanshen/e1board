/****************************************************************************/
/*	Description		:オプションリモコン通信機能 非公開ヘッダ				*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC keqi.liang											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/29												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ERMC													*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __ERMC_SERV_H__
#define __ERMC_SERV_H__

#include <typedef.h>

#include "ermc_cmn_g.h"

/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
/* リターンコード */
#define	ERMC_RET_INITIAL_VALUE					((sshort)-99)				/* 初期値 */
#define	ERMC_RET_OK								((sshort)0)					/* 正常終了 */
#define	ERMC_RET_NG								((sshort)-1)				/* 異常終了 */

/* OPリモコン通信関連 */
#define	ERMC_COMM_SERIAL_PORT					((schar*)"/dev/ttymxc2")	/* シリアルポート */
#define	ERMC_COMM_CYC_DECISION_TIME				((ushort)1250u)				/* 受信有無周期判定用の時間[ms] */
#define	ERMC_COMM_SEND_TIMEOUT					((ushort)250u)				/* 送信タイムアウト時間[ms] */
#define	ERMC_COMM_RECV_TIMEOUT					((ushort)250u)				/* 受信タイムアウト時間[ms] */
#define	ERMC_COMM_FAIL_ERROR_CODE				((schar*)"BC550")			/* OPリモコン通信不通エラーコード */
#define	ERMC_COMM_FAIL_ERROR_RATING				((schar)'8')				/* OPリモコン通信不通エラーレーティング */
#define	ERMC_COMM_FAIL_DECISION_TIME			((ulong)270000u)			/* OPリモコン通信不通判定時間[ms] */
#define	ERMC_COMM_FAIL_SET_FLAG_OFF				((ushort)0u)				/* OPリモコン通信不通エラー設定フラグ：OFF */
#define	ERMC_COMM_FAIL_SET_FLAG_ON				((ushort)1u)				/* OPリモコン通信不通エラー設定フラグ：ON */
#define	ERMC_COMM_TIMER_REG_FLAG_OFF			((ushort)0u)				/* OPリモコン通信/電文応答タイマ登録フラグ：OFF */
#define	ERMC_COMM_TIMER_REG_FLAG_ON				((ushort)1u)				/* OPリモコン通信/電文応答タイマ登録フラグ：ON */

/* 電文固定値関連 */
#define	ERMC_MSG_CODE_STX						((uchar)0x02u)				/* STX */
#define	ERMC_MSG_CODE_ETX						((uchar)0x03u)				/* ETX */
#define	ERMC_MSG_CODE_CR						((uchar)0x0Du)				/* CR */
#define	ERMC_MSG_CODE_HEADER_SEND				((schar*)"$R")				/* Header(送信) */
#define	ERMC_MSG_REQ_COMMAND_CMD0				((schar*)"G00")				/* 要求Command：CMD0 */
#define	ERMC_MSG_REQ_COMMAND_CMD24				((schar*)"G24")				/* 要求Command：CMD24 */
#define	ERMC_MSG_REQ_COMMAND_CMD41				((schar*)"S41")				/* 要求Command：CMD41 */
#define	ERMC_MSG_MODEL_GENERATION_E1			((schar*)"T ")				/* モデル世代名：E1 */
#define	ERMC_MSG_CURRENT_TIME_CH_1				((schar)'1')				/* 現在時刻のCH文字：'1' */

/* (CMD0)電文関連 */
#define	ERMC_CDM0_REQ_MSG_DATA_SIZE				((ushort)0u)				/* (CDM0)要求電文Data部サイズ */
#define	ERMC_CDM0_RES_MSG_DATA_SIZE				((ushort)2u)				/* (CDM0)応答電文Data部サイズ */

/* (CMD24)電文関連 */
#define	ERMC_CDM24_REQ_MSG_DATA_SIZE			((ushort)0u)				/* (CDM24)要求電文Data部サイズ */
#define	ERMC_CDM24_RES_MSG_DATA_SIZE			((ushort)38u)				/* (CDM24)応答電文Data部サイズ */

/* (CMD41)電文関連 */
#define	ERMC_CDM41_REQ_MSG_DATA_SIZE			((ushort)54u)				/* (CDM41)要求電文Data部サイズ */
#define	ERMC_CDM41_RES_MSG_DATA_SIZE			((ushort)357u)				/* (CDM41)応答電文Data部サイズ */
#define	ERMC_CDM41_DATA_GET_INDEX_0				((ushort)0u)				/* (CDM41)データ取得用インデックス：0 */
#define	ERMC_CDM41_DATA_GET_INDEX_2				((ushort)2u)				/* (CDM41)データ取得用インデックス：2 */
#define	ERMC_CDM41_DATA_GET_INDEX_4				((ushort)4u)				/* (CDM41)データ取得用インデックス：4 */
#define	ERMC_CDM41_DATA_GET_INDEX_6				((ushort)6u)				/* (CDM41)データ取得用インデックス：6 */
#define	ERMC_CDM41_DATA_GET_INDEX_8				((ushort)8u)				/* (CDM41)データ取得用インデックス：8 */
#define	ERMC_CDM41_DATA_GET_INDEX_10			((ushort)10u)				/* (CDM41)データ取得用インデックス：10 */
#define	ERMC_CDM41_DATA_GET_INDEX_12			((ushort)12u)				/* (CDM41)データ取得用インデックス：12 */
#define	ERMC_CDM41_DATA_GET_INDEX_13			((ushort)13u)				/* (CDM41)データ取得用インデックス：13 */
#define	ERMC_CDM41_DATA_GET_INDEX_18			((ushort)18u)				/* (CDM41)データ取得用インデックス：18 */
#define	ERMC_CDM41_DATA_GET_INDEX_22			((ushort)22u)				/* (CDM41)データ取得用インデックス：22 */
#define	ERMC_CDM41_DATA_GET_INDEX_26			((ushort)26u)				/* (CDM41)データ取得用インデックス：26 */
#define	ERMC_CDM41_DATA_GET_INDEX_30			((ushort)30u)				/* (CDM41)データ取得用インデックス：30 */
#define	ERMC_CDM41_DATA_GET_INDEX_38			((ushort)38u)				/* (CDM41)データ取得用インデックス：38 */
#define	ERMC_CDM41_DATA_GET_INDEX_46			((ushort)46u)				/* (CDM41)データ取得用インデックス：46 */
#define	ERMC_CDM41_DATA_GET_INDEX_50			((ushort)50u)				/* (CDM41)データ取得用インデックス：50 */

/* 共通関連 */
#define	ERMC_MSG_CMD_CNT_MAX					((ushort)3u)				/* 電文最大コマンド数 */
#define	ERMC_MSG_BODY_SIZE_MIN					((ushort)7u)				/* 電文Body部最小サイズ */
#define	ERMC_MSG_CHECK_SUM_SIZE					((ushort)2u)				/* 電文Check Sumサイズ */
#define	ERMC_MSG_HEADER_SIZE					((ushort)2u)				/* 電文Header部サイズ */
#define	ERMC_MSG_ADDRESS_SIZE					((ushort)1u)				/* 電文Address部サイズ */
#define	ERMC_MSG_SEQUENCE_SIZE					((ushort)1u)				/* 電文Seq_NB部サイズ */
#define	ERMC_MSG_COMMAND_SIZE					((ushort)3u)				/* 電文Command部サイズ */
#define	ERMC_MSG_DATA_BUF_SIZE					((ushort)4600u)				/* 電文Data部バッファサイズ */
#define	ERMC_MSG_FORMAT_SIZE					((ushort)12u)				/* 電文フォーマット(Data部以外)サイズ */
#define	ERMC_CONVERT_BASE_DEC					((sshort)10u)				/* 変換基準(10進) */
#define	ERMC_CONVERT_BASE_HEX					((sshort)16u)				/* 変換基準(16進) */
#define	ERMC_SLEEP_TIME_1						((ushort)1u)				/* スリープ時間：1[ms] */
#define	ERMC_SEND_END_SLEEP_TIME				((ushort)10u)				/* 送信完了後スリープ時間：10[ms] */
#define	ERMC_BOARD_OPE_MODE_CNT_MAX				((ushort)8u)				/* 通信ボード内管理運転モード最大数 */
#define	ERMC_INV_INIT_COMM_FIN_FLAG_OFF			((ushort)0u)				/* INVマイコン初期化通信完了フラグ：OFF */
#define	ERMC_INV_INIT_COMM_FIN_FLAG_ON			((ushort)1u)				/* INVマイコン初期化通信完了フラグ：ON */
#define	ERMC_TIME_SYNC_FLAG_OFF					((ushort)0u)				/* 時刻同期要否フラグ：OFF */
#define	ERMC_TIME_SYNC_FLAG_ON					((ushort)1u)				/* 時刻同期要否フラグ：ON */
#define	ERMC_LOOP_STOP_FLAG_OFF					((ushort)0u)				/* ループ処理停止用フラグ：OFF */
#define	ERMC_LOOP_STOP_FLAG_ON					((ushort)1u)				/* ループ処理停止用フラグ：ON */
#define	ERMC_MSG_CMN_SIZE_1						((ushort)1u)				/* 共通文字列サイズ：1 */
#define	ERMC_MSG_CMN_SIZE_2						((ushort)2u)				/* 共通文字列サイズ：2 */
#define	ERMC_MSG_CMN_SIZE_3						((ushort)3u)				/* 共通文字列サイズ：3 */
#define	ERMC_MSG_CMN_SIZE_4						((ushort)4u)				/* 共通文字列サイズ：4 */
#define	ERMC_MSG_CMN_SIZE_5						((ushort)5u)				/* 共通文字列サイズ：5 */
#define	ERMC_MSG_CMN_SIZE_6						((ushort)6u)				/* 共通文字列サイズ：6 */
#define	ERMC_MSG_CMN_SIZE_8						((ushort)8u)				/* 共通文字列サイズ：8 */
#define	ERMC_MSG_CMN_SIZE_10					((ushort)10u)				/* 共通文字列サイズ：10 */
#define	ERMC_MSG_CMN_SIZE_12					((ushort)12u)				/* 共通文字列サイズ：12 */

#define	DIFF_TIME(eTime,sTime) (ushort)((eTime - sTime) * ((ushort)1000u))	/* 時間の差分をミリ秒で取得 */
#define UNUSED(a) (void)(a)

/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
/* ****************** 受信電文用の構造体定義 ******************** */
/* 時刻情報構造体 */
typedef struct {
	ushort			year;													/* 年(西暦の下2桁) */
	ushort			month;													/* 月 */
	ushort			day;													/* 日 */
	ushort			hour;													/* 時 */
	ushort			minute;													/* 分 */
	ushort			second;													/* 秒 */
} TimeInfo_t;

/* (CMD41)受信電文データ情報構造体 */
typedef struct {
	TimeInfo_t		currentTime;											/* 現在時刻 */
	ushort			timeSyncFlag;											/* 時刻同期要否フラグ */
	schar			rmcError[ERMC_MSG_CMN_SIZE_5];							/* RMCエラー */
	schar			rmcErrorRating;											/* RMCエラーレーティング */
	slong			restrictedChargePwr;									/* 充電電力設定値（蓄電池）[W] [未使用] */
	slong			restrictedPvGenPwr;										/* 発電電力制限値[W] [未使用] */
	slong			restrictedDischargePwr;									/* 放電電力設定値[W]（蓄電池） [未使用] */
	slong			v2hPowerSetCharge;										/* 車載電池充電電力設定値[W] [未使用] */
	slong			v2hPowerSetDiscCharge;									/* 車載電池放電電力設定値[W] [未使用] */
	ushort			v2hCurrentSetCharge;									/* 車載電池充電電流設定値[0.1A] [未使用] */
	ushort			v2hCurrentSetDischarge;									/* 車載電池放電電流設定値[0.1A] [未使用] */
} Cmd41RecvMsgDataInfo_t;

/* 受信電文データ情報構造体 */
typedef struct {
	Cmd41RecvMsgDataInfo_t		cmd41DataInfo;								/* (CMD41)受信電文データ情報 */
} RecvMsgDataInfo_t;

/* ****************** 送信電文用の構造体定義 ******************** */
/* (CMD0)応答電文データ情報構造体 */
typedef struct {
	uchar			modelGeneration[ERMC_MSG_CMN_SIZE_2];					/* モデル世代名 */
} Cmd0ResMsgDataInfo_t;

/* (CMD24)応答電文データ情報構造体 */
typedef struct {
	uchar			opeMode;												/* 運転モード */
	uchar			emergencyMode;											/* 非常時運転モード */
	uchar			emergencyPercent;										/* 非常時安心設定(放電下限値設定) [未使用] */
	uchar			chargeStartTime[ERMC_MSG_CMN_SIZE_4];					/* 充電開始時刻 */
	uchar			chargeStopTime[ERMC_MSG_CMN_SIZE_4];					/* 充電停止時刻 */
	uchar			dischargeStartTime[ERMC_MSG_CMN_SIZE_4];				/* 放電開始時刻 */
	uchar			dischargeStopTime[ERMC_MSG_CMN_SIZE_4];					/* 放電停止時刻 */
	uchar			startOrStop;											/* 運転／停止 */
	uchar			acUpperLimit;											/* 充電上限値設定 [未使用] */
	uchar			acLowerLimitBt[ERMC_MSG_CMN_SIZE_3];					/* 蓄電池放電下限値設定 */
	uchar			acUpperLimitBt[ERMC_MSG_CMN_SIZE_3];					/* 蓄電池充電上限値設定 */
	uchar			pvSurplusChargeCtrl;									/* 余剰充電制御 [未使用] */
	uchar			pvSurplusStartTime[ERMC_MSG_CMN_SIZE_4];				/* 余剰充電制御開始時刻 [未使用] */
	uchar			pvSurplusEndTime[ERMC_MSG_CMN_SIZE_4];					/* 余剰充電制御停止時刻 [未使用] */
	uchar			v2hDischargeTimeZoneBasis;								/* 車両放電時間帯制限基準 [未使用] */
	uchar			testrunOutAtPwrCut;										/* 試運転時停電出力 [未使用] */
} Cmd24ResMsgDataInfo_t;

/* (CMD41)応答電文データ情報構造体 */
typedef struct {
	uchar			chargeDischargePwr[ERMC_MSG_CMN_SIZE_4];				/* AC充放電電力（インバータ部） */
	uchar			inverterOpeState;										/* インバータ動作状態 */
	uchar			ttlChargeEnergyInv[ERMC_MSG_CMN_SIZE_8];				/* AC積算充電電力量（インバータ部） [未使用] */
	uchar			ttlDischargeEnergyInv[ERMC_MSG_CMN_SIZE_8];				/* AC積算放電電力量（インバータ部） [未使用] */
	uchar			rsoc[ERMC_MSG_CMN_SIZE_4];								/* rSOC */
	uchar			remainingBtA[ERMC_MSG_CMN_SIZE_4];						/* 電池残量（Ah） [未使用] */
	uchar			remainingBtW[ERMC_MSG_CMN_SIZE_4];						/* 電池残量（kWh） */
	uchar			soc[ERMC_MSG_CMN_SIZE_4];								/* SOC */
	uchar			btVolt[ERMC_MSG_CMN_SIZE_4];							/* 電池電圧 [未使用] */
	uchar			chargeDischargeCnt[ERMC_MSG_CMN_SIZE_4];				/* 充放電サイクル数 [未使用] */
	uchar			soh[ERMC_MSG_CMN_SIZE_4];								/* SOH */
	uchar			operationState;											/* 蓄電池運転動作状態 */
	uchar			pwrCut;													/* 停電 */
	uchar			pvSelfInputPwr[ERMC_MSG_CMN_SIZE_4];					/* PV自立入力電力 */
	uchar			pvSelfInputVolt[ERMC_MSG_CMN_SIZE_4];					/* PV自立入力電圧 [未使用] */
	uchar			pvGenPwr[ERMC_MSG_CMN_SIZE_4];							/* PV発電電力（本製品以外のPV） */
	uchar			revPwrFlow[ERMC_MSG_CMN_SIZE_4];						/* 逆潮流電力 */
	uchar			sysVolt[ERMC_MSG_CMN_SIZE_4];							/* 系統電圧 [未使用] */
	uchar			pcsError[ERMC_MSG_CMN_SIZE_5];							/* PCSエラー */
	uchar			pcsErrorRating;											/* PCSエラーレーティング */
	uchar			opeMode;												/* 運転モード */
	uchar			opeState[ERMC_MSG_CMN_SIZE_2];							/* 運転状態 [未使用] */
	uchar			ttlChargeEnergyBtInv[ERMC_MSG_CMN_SIZE_8];				/* AC蓄電池積算充電電力量（インバータ部） [未使用] */
	uchar			ttlDischargeEnergyBtInv[ERMC_MSG_CMN_SIZE_8];			/* AC蓄電池積算放電電力量（インバータ部） [未使用] */
	uchar			opeTime[ERMC_MSG_CMN_SIZE_6];							/* 通算稼働時間 [未使用] */
	uchar			chargeDischargePwr2[ERMC_MSG_CMN_SIZE_4];				/* DC蓄電池充放電電力２（コンバータ部） */
	uchar			pvGenPwr2[ERMC_MSG_CMN_SIZE_4];							/* 太陽光発電電力（内蔵PV） */
	uchar			ttlChargeEnergy2[ERMC_MSG_CMN_SIZE_8];					/* DC蓄電池積算充電電力量２（コンバータ部） [未使用] */
	uchar			ttlDischargeEnergy2[ERMC_MSG_CMN_SIZE_8];				/* DC蓄電池積算放電電力量２（コンバータ部） [未使用] */
	uchar			ttlPvGenPwr2[ERMC_MSG_CMN_SIZE_8];						/* 積算太陽光発電電力量（内蔵PV） [未使用] */
	uchar			ttlPvGenPwr[ERMC_MSG_CMN_SIZE_8];						/* 積算PV発電電力量（本製品以外のPV） [未使用] */
	uchar			ttlPurchasePwr[ERMC_MSG_CMN_SIZE_8];					/* 積算買電電力量 [未使用] */
	uchar			ttlSellingPwr[ERMC_MSG_CMN_SIZE_8];						/* 積算売電電力量 [未使用] */
	uchar			restrictedChargePwr[ERMC_MSG_CMN_SIZE_4];				/* 充電電力設定値（蓄電池）(kW) [未使用] */
	uchar			restrictedPvGenPwr[ERMC_MSG_CMN_SIZE_4];				/* 発電電力制限値(kW) [未使用] */
	uchar			restrictedDischargePwr[ERMC_MSG_CMN_SIZE_4];			/* 放電電力設定値（蓄電池）(kW) [未使用] */
	uchar			homeLoadPower[ERMC_MSG_CMN_SIZE_4];						/* 家庭負荷電力 */
	uchar			acChargeEffectiveCapacity[ERMC_MSG_CMN_SIZE_8];			/* AC充電実効容量（蓄電池） [未使用] */
	uchar			acDischargeEffectiveCapacity[ERMC_MSG_CMN_SIZE_8];		/* AC放電実効容量（蓄電池） [未使用] */
	uchar			acChargePossibleCapacity[ERMC_MSG_CMN_SIZE_8];			/* AC充電可能容量（蓄電池） [未使用] */
	uchar			acDishargePossibleCapacity[ERMC_MSG_CMN_SIZE_8];		/* AC放電可能容量（蓄電池） [未使用] */
	uchar			acChargePossibleAmount[ERMC_MSG_CMN_SIZE_8];			/* AC充電可能量（蓄電池） [未使用] */
	uchar			acDischargePossibleAmount[ERMC_MSG_CMN_SIZE_8];			/* AC放電可能量（蓄電池） [未使用] */
	uchar			suppressState;											/* 出力抑制状態 */
	uchar			v2hConnectorLockState;									/* コネクタロック状態 */
	uchar			v2hOpeState;											/* 車両充放電器運転動作状態 [未使用] */
	uchar			v2hPowerSetCharge[ERMC_MSG_CMN_SIZE_8];					/* 車載電池充電電力設定値 [未使用] */
	uchar			v2hPowerSetDiscCharge[ERMC_MSG_CMN_SIZE_8];				/* 車載電池放電電力設定値 [未使用] */
	uchar			v2hCurrentSetCharge[ERMC_MSG_CMN_SIZE_4];				/* 車載電池充電電流設定値 [未使用] */
	uchar			v2hCurrentSetDischarge[ERMC_MSG_CMN_SIZE_4];			/* 車載電池放電電流設定値 [未使用] */
	uchar			v2hDischargeableRemCapacity1[ERMC_MSG_CMN_SIZE_8];		/* 車載電池の放電可能残容量１ [未使用] */
	uchar			v2hDischargeableRemCapacity2[ERMC_MSG_CMN_SIZE_4];		/* 車載電池の放電可能残容量２ [未使用] */
	uchar			v2hDischargeableRemCapacity3[ERMC_MSG_CMN_SIZE_2];		/* 車載電池の放電可能残容量３ [未使用] */
	uchar			v2hChargeableRemainCap[ERMC_MSG_CMN_SIZE_8];			/* 車載電池の充電可能残容量値 [未使用] */
	uchar			v2hAvailabilityStatus[ERMC_MSG_CMN_SIZE_2];				/* 車両接続・充放電可否状態 [未使用] */
	uchar			v2hInstantPower[ERMC_MSG_CMN_SIZE_8];					/* 車両充放電器瞬時充放電電力計測値 */
	uchar			v2hInstantCurrent[ERMC_MSG_CMN_SIZE_4];					/* 車両充放電器瞬時充放電電流計測値 [未使用] */
	uchar			v2hInstantVolt[ERMC_MSG_CMN_SIZE_4];					/* 車両充放電器瞬時充放電電圧計測値 [未使用] */
	uchar			v2hTtlDischargePower[ERMC_MSG_CMN_SIZE_8];				/* 車載電池の積算放電電力量計測値 [未使用] */
	uchar			v2hTtlChargePower[ERMC_MSG_CMN_SIZE_8];					/* 車載電池の積算充電電力量計測値 [未使用] */
	uchar			emergencyMode;											/* 非常時運転モード */
	uchar			v2hRemainingCapacity1[ERMC_MSG_CMN_SIZE_8];				/* 車載電池の電池残容量1 */
	uchar			v2hRemainingCapacity2[ERMC_MSG_CMN_SIZE_4];				/* 車載電池の電池残容量２ [未使用] */
	uchar			v2hRemainingCapacity3[ERMC_MSG_CMN_SIZE_2];				/* 車載電池の電池残容量３ */
	uchar			v2hEstimateChargeEndMinute[ERMC_MSG_CMN_SIZE_4];		/* 車両充電終了目安時間 [未使用] */
	uchar			chargeFullFlag;											/* 蓄電池満充電フラグ [未使用] */
	uchar			dischargeEmptyFlag;										/* 蓄電池空放電フラグ [未使用] */
	uchar			v2hChargeFullFlag;										/* 車載電池満充電フラグ [未使用] */
	uchar			v2hDischargeEmptyFlag;									/* 車載電池空放電フラグ [未使用] */
	uchar			v2hChargeCtrlFlag;										/* 車両充放電器制御要求フラグ [未使用] */
	uchar			commBoardCurrentTime[ERMC_MSG_CMN_SIZE_12];				/* 通信ボード現在時刻 */
	uchar			commBoardCommState;										/* 通信ボード通信状態 */
	uchar			commBoardCommConfirmState[ERMC_MSG_CMN_SIZE_3];			/* 通信ボード通信確認状態 */
	uchar			remoteOpeState;											/* リモート運転状態 */
	uchar			timeSyncState;											/* 時刻同期状態 */
	uchar			suppressPer[ERMC_MSG_CMN_SIZE_2];						/* 抑制率％ */
	uchar			maintenanceModeDate[ERMC_MSG_CMN_SIZE_6];				/* メンテナンスモード実施日 [未使用] */
	uchar			representOpeMode[ERMC_MSG_CMN_SIZE_2];					/* 表示用運転モード */
} Cmd41ResMsgDataInfo_t;

/* 応答電文データ情報構造体 */
typedef struct {
	Cmd0ResMsgDataInfo_t		cmd0DataInfo;								/* (CMD0)応答電文データ情報 */
	Cmd24ResMsgDataInfo_t		cmd24DataInfo;								/* (CMD24)応答電文データ情報 */
	Cmd41ResMsgDataInfo_t		cmd41DataInfo;								/* (CMD41)応答電文データ情報 */
} ResMsgDataInfo_t;

/* ******************** そのたの構造体定義 ********************** */
/* 受信電文解析情報構造体 */
typedef struct {
	/*  */
	schar			headerPart[ERMC_MSG_HEADER_SIZE];						/* Header部 */
	schar			addressPart[ERMC_MSG_ADDRESS_SIZE];						/* Address部 */
	schar			sequencePart[ERMC_MSG_SEQUENCE_SIZE];					/* Seq_NB部 */
	schar			commandPart[ERMC_MSG_COMMAND_SIZE + 1];					/* Command部 */
	schar			dataPart[ERMC_MSG_DATA_BUF_SIZE];						/* Data部 */
	ushort			dataPartSize;											/* Data部サイズ */
	ushort			cmdTableIndex;											/* コマンドテーブルインデックス */
} RecvMsgParseInfo_t;

/* コマンドテーブル構造体 */
typedef struct {
	ProcEvent_t		recvEvent;												/* 受信イベント */
	schar*			reqCmdCode;												/* 要求コマンドコード */
	ushort			reqDataPartSize;										/* 要求Data部サイズ */
	void			(*setReqDataInfo)(void);								/* 要求データ情報設定 */
	void			(*getResDataInfo)(ProcAction_t* procAction);			/* 応答データ情報取得 */
	uchar*			resDataInfo;											/* 応答データ情報 */
	ushort			resDataPartSize;										/* 応答Data部サイズ */
} CmdTable_t;

/* アクションテーブル構造体 */
typedef struct {
	ProcStatus_t	procState;												/* プロセス状態 */
	void			(*sendAction)(void);									/* 送信処理 */
	void			(*semAction)(void);										/* sem処理 */
} ActionTable_t;

/* 通信ボード内管理運転モード変換テーブル構造体 */
typedef struct {
	uchar			boardOpeMode;											/* 通信ボード内管理運転モード */
	uchar			opeMode;												/* 運転モード */
} BoardOpeModeConvTable_t;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
static sshort ermc_OpenUart(void);
static sshort ermc_OpenGpio(void);
static void ermc_CloseUart(void);
static void ermc_CloseGpio(void);
static sshort ermc_EnableRead(void);
static sshort ermc_EnableWrite(void);
static double ermc_GetTimeSec(void);
static void ermc_Callnanosleep(ushort sleepMs);
static void ermc_Callsemwait(void);
static void ermc_Callsempost(void);
static void ermc_RegMsgResTimer(void);
static void ermc_RegRmcCommTimer(void);
static slong ermc_ConvStrToIntStrtol(schar* str, ushort len, sshort base);
static ulong ermc_ConvStrToIntStrtoul(schar* str, ushort len, sshort base);
static uchar ermc_ConvBoardOpeMode(uchar boardOpeMode);
static void ermc_SetRmcCommRelatedItems(ProcEvent_t procEvent);
static void ermc_GetCmd0ResDataInfo(ProcAction_t* nextAction);
static void ermc_GetCmd24ResDataInfo(ProcAction_t* nextAction);
static void ermc_SetCmd41ReqDataInfo(void);
static void ermc_GetCmd41ResDataInfo(ProcAction_t* nextAction);
static void* ermc_RecvMsgThread(void* param);
static sshort ermc_RecvMsg(ssize_t* recvSize);
static sshort ermc_ReadMsg(uchar* msgData, size_t msgSize,
							ssize_t* totalSize);
static sshort ermc_CheckMsgFormat(void);
static sshort ermc_ParseMsg(void);
static void ermc_SendMsg(void);
static void ermc_CreateResMsg(uchar* msgData, ulong msgDataSize,
								size_t* resSize);
static sshort ermc_WriteMsg(const void* msgData, size_t msgSize,
								ssize_t* totalSize);
static void ermc_WaitSendEnd(void);
static void ermc_SendProcStatusChangeReq(ProcAction_t nextAction);
static void ermc_RunStateMachine(ProcAction_t nextAction);

#endif /*__ERMC_SERV_H__*/
