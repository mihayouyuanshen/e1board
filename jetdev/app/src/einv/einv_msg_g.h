/****************************************************************************/
/*	Description		:INV通信ミドル 送受信メッセージ							*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Dai.P												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/23												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:INV通信												*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EINV_MSG_G_H__
#define __EINV_MSG_G_H__


/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "typedef.h"
#include "frm_g.h"
#include "einv_g.h"
#include "ewdt.h"


/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define	EINV_CYCLE_TIME_MS					((ushort)1000)			/* 周期情報取得の周期時間[msec] */
#define	EINV_30ONESHOT_TIME_MS				((ushort)30*1000)		/* DELAY 30s後リセット */
#define	EINV_WDT_TIMEOUT					((uchar)10)				/* WDTプロセス無応答を検知するまでの秒数 */
#define	EINV_THREAD_LIFE_POINT_MAX			(EWDT_PROC_TIMEOUT_DEF)	/* INV通信スレッドのライフポイントの閾値 */


/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
/* INVコマンド送信要求構造体*/
typedef enum _einv_command_e{
	EINV_CMD01 = 0,
	EINV_CMD11,
	EINV_CMD18,
	EINV_CMD19,
	EINV_CMD60,
	EINV_CMD62,
	EINV_CMD63,
	EINV_CMD64,
	EINV_CMD65,
	EINV_CMD68,
	EINV_CMD69,
	EINV_CMD6A,
	EINV_CMD6B,
	EINV_CMD6D,
	EINV_CMD74,
	EINV_CMD75,
	EINV_CMD81,
	EINV_CMD90,
	EINV_CMD98,
	EINV_CMD_MAX
}EinvCommand_t;

/* INV通信確認要求構造体 */
typedef struct {
	FrmMsgHead_t	header;
}EinvMsgChkCommPar_t;
///* INV通信確認応答構造体 */
//typedef struct {
//	FrmMsgHead_t	header;
//}EINV_MSG_CHK_COMM_RES_T;

typedef struct {
	FrmMsgHead_t		header;
	EinvCommand_t		command;
	ulong				fact;
} EinvMsgInitreqPar_t;
//typedef struct {
//	FrmMsgHead_t		header;
//} EINV_MSG_INITREQ_RES_T;


typedef struct {
	FrmMsgHead_t		header;
} EinvMsgStopreqPar_t;

typedef struct {
	FrmMsgHead_t		header;
	EinvCommand_t		command;
	ulong				fact;
} EinvMsgTestRunreqPar_t;

/* プロセス間通信要求共用体 */
typedef union _einv_msg_par_u{
	FrmMsgHead_t							header;
	EinvMsgChkCommPar_t						chkComm;
	EinvMsgInitreqPar_t						initReq;
	EinvMsgTestRunreqPar_t					TestRunReq;
	EinvMsgStopreqPar_t						stopReq;
	/*  受信イベント */
	EwdtEvtWdtReq_t							wdtEvent;
	EwdtEvtTermReq_t						termEvent;
	FrmTimeout_t							timerEvent;
}EinvMsgPar_t;

///* プロセス間通信応答共用体 */
//typedef union _einv_msg_res_u{
//	FrmMsgHead_t								header;
//	EINV_MSG_CHK_COMM_RES_T						chkComm;
//	EINV_MSG_INITREQ_RES_T						initReq;
//}EINV_MSG_RES_U;


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
/* ファンクションID */
typedef enum _einv_fid_e{
	/* Function ID */
	EINV_FID_NONE = 0,										/* 未定義（使用しないこと） */
	EINV_FID_INIT,											/* INVマイコン通信初期化 */
	EINV_FID_INIT_CMDNG,
	EINV_FID_INIT_CMD01_OK,
	EINV_FID_INIT_CMD60_OK,
	EINV_FID_INIT_CMD90_OK,
	EINV_FID_INIT_CMD65_OK,
	EINV_FID_INIT_CMD75_OK,
	EINV_FID_INIT_CMD6B_OK,
	EINV_FID_STOP,
	EINV_FID_REQ_OPEMODE,									/* 運転モード蓄電池電力取得 */
	EINV_FID_TESTRUN_INDICATE,								/*12*/
	EINV_FID_TESTRUN_ABORT,									/*13*/
	EINV_FID_TESTRUN_CMD65_OK,								/*14*/
	EINV_FID_TESTRUN_CMD75_OK,								/*15*/
	EINV_FID_TESTRUN_TERM,									/*16*/
	EINV_FID_TESTRUN_CMD_NG,								/*17*/
	EINV_FID_TESTRUN_CMD65,									/*18*/
	EINV_FID_TESTRUN_CMD75,									/*19*/
	/**/
	EINV_FID_MAX_PLUSONE,									/* ファンクションIDの定義数+1 */

	EINV_FID_CHK_COMM,										/* INV通信確認 */
	EINV_FID_GET_OPEMODE,									/* 運転モード取得 */
	EINV_FID_SET_OPEMODE,									/* 運転モード設定 */
	EINV_FID_GET_TIMEZONE,									/* 充放電時間帯取得 */
	EINV_FID_GET_SPTABLE,									/* 整定値・設定値テーブル取得 */
	EINV_FID_GET_SPDATA,									/* 整定値・設定値取得 */
	EINV_FID_SET_SPDATA,									/* 整定値・設定値設定 */
	EINV_FID_GET_REG,										/* 登録情報取得 */
	EINV_FID_GET_SERINUM,									/* 機種名・製造番号取得 */
	EINV_FID_GET_SERIINP,									/* INV製造番号入力情報取得 */
	EINV_FID_SET_UNIQ,										/* 特殊設定値設定 */
	EINV_FID_GET_HOME,										/* ホーム情報取得 */
	EINV_FID_GET_OPEINFO,									/* 運転情報取得 */
	EINV_FID_GET_SYSMAP,									/* システムマップ情報取得 */
	EINV_FID_GET_OPESTTS,									/* 動作情報取得 */
	EINV_FID_GET_CT,										/* CT電流方向取得 */
	EINV_FID_REQ_ERRRST,									/* INVエラー消去要求 */
	EINV_FID_IS_OPECHNG,									/* ユーザ権限時運転モード変更可否取得 */
	EINV_FID_GET_BT,										/* 蓄電池情報取得 */
	EINV_FID_SET_CHARGE,									/* 充電電力量設定 */
	EINV_FID_SET_DISCHARGE,									/* 放電電力量設定 */
	EINV_FID_NTFY_TIMESYNC,									/* 時刻同期通知 */
	EINV_FID_REG_STTS_EVT,									/* 状態変化通知イベント登録 */
	EINV_FID_CLR_STTS_EVT,									/* 状態変化通知イベント登録解除 */
	EINV_FID_REG_SP_EVT,									/* 設定変化通知イベント登録 */
	EINV_FID_CLR_SP_EVT,									/* 設定変化通知イベント登録解除 */
	EINV_FID_GET_BTDIAGNO,									/* 蓄電値診断データ取得 */
	EINV_FID_SET_OPEMDASYNC,								/* 運転モード設定(非同期型) */
	EINV_FID_SET_TIMEZONE,									/* 充放電時間帯設定 */
	EINV_FID_SET_OPEMDRSV,									/* 自動運転モード予約設定 */
	EINV_FID_SAVE_SERINUM,									/* INV製造番号保存 */
	EINV_FID_GET_PVCLASS,									/* 太陽光発電クラス情報取得 */
	EINV_FID_SET_PVGENPWR,									/* 発電電力制限値設定 */
	EINV_FID_SET_TESTRUN_STATUS,							/* 試運転常状態設定 */
	EINV_FID_FWUPDATE_START,								/* INVFW更新開始要求 */
	EINV_FID_FWUPDATE_STOP,									/* INVFW更新中止要求 */
	EINV_FID_SAVE_OPERATION_FW,								/* FW更新前運転情報保存 */
	EINV_FID_CHK_CMD0_COMM,									/* INVCMD0通信確認 */
	EINV_FID_SAVE_OPERATION,								/* 運転情報ファイル更新設定 */
	EINV_FID_GET_SYSTEMINFO,								/* システム情報取得 */
	EINV_FID_SET_CHARGINGPWR,								/* 充電電力設定値設定 */
	EINV_FID_GET_V2H_LOG,									/* V2Hログ情報取得要求 */
	EINV_FID_CHK_V2H_CONNCONF,								/* V2H車両接続確認 */
	EINV_FID_GET_V2H_OPEMODE,								/* V2H運転モード取得 */
	EINV_FID_SET_V2H_OPEMODE,								/* V2H運転モード設定 */
	EINV_FID_GET_V2H_SETINFO,								/* V2H設定情報取得 */
	EINV_FID_SET_V2H_SETINFO,								/* V2H設定情報設定 */
	EINV_FID_SET_V2H_TTLENERGYRESET,						/* V2H積算情報リセット */
	EINV_FID_REG_V2H_STTS_EVT,								/* V2H状態変化通知イベント登録 */
	EINV_FID_CLR_V2H_STTS_EVT,								/* V2H状態変化通知イベント登録解除 */
	EINV_FID_REG_V2H_SP_EVT,								/* V2H設定変更通知イベント登録 */
	EINV_FID_CLR_V2H_SP_EVT,								/* V2H設定変更通知イベント登録解除 */
	EINV_FID_SET_DISCHARGEPWR,								/* 放電電力設定値設定 */
	EINV_FID_SET_MAX_CHARGINGPWR,							/* 最大充電電力設定値設定 */
	EINV_FID_GET_MAX_CHARGINGPWR,							/* 最大充電電力設定値取得 */
	EINV_FID_SET_AC_CHARGE,									/* AC充電量設定値設定 */
	EINV_FID_SET_AC_DISCHARGE,								/* AC放電量設定値設定 */
	EINV_FID_GET_RESTRICTED_SET_VALUE,						/* 充放電電力設定値取得 */
	EINV_FID_GET_V2H_FWVER,									/* V2HFWバージョン情報取得 */
	EINV_FID_VALID_V2H_TIMEZONE_CHG,						/* 車両充放電時間帯変更設定 */
	EINV_FID_GET_V2H_SETINFO_NOTCOMM,						/* V2H設定情報取得(非通信) */
	EINV_FID_SET_PVINFO_NOTCOMM,							/* 太陽光発電情報(リモコン設定値)設定 */
	EINV_FID_GET_PVINFO_NOTCOMM,							/* 太陽光発電情報(リモコン設定値)取得 */
	EINV_FID_GET_GENPWR,									/* 発電抑制値取得 */
	EINV_FID_GET_V2H_SETINFO_CURRENT_NOTCOMM,				/* V2H設定情報取得(非通信) */
	EINV_FID_LOAD_RMC_DATA,									/* リモコン設定値取得 */
	EINV_FID_SAVE_RMC_DATA,									/* リモコン設定値保存 */
	EINV_FID_SAVE_OPEMODE,									/* 運転モード情報保存 */
	EINV_FID_SAVE_V2H_OPEMODE,								/* V2H運転情報保存 */
	EINV_FID_GET_DATA2_CMD16,								/* 設定値２情報取得 */
	EINV_FID_SET_DATA2_CMD17,								/* 設定値２情報設定 */
	EINV_FID_IS_FFG_VALID,									/* 周波数フィードバックゲインサポート判定 */
	EINV_FID_GET_EMGMODE,									/* 非常時運転モード取得*/
	EINV_FID_SET_EMGMODE,									/* 非常時運転モード設定*/
	EINV_FID_CHANGE_MAINTENANCE_MODE,						/* メンテナンスモード切替要求 */
	EINV_FID_SKIP_MAINTENANCE_MODE,							/* メンテナンスモードスキップ要求 */
	EINV_FID_GET_MAINTENANCE_DATE,							/* メンテナンスモード予定日情報取得 */
	EINV_FID_GET_MAINTENANCE_TESTNO,						/* メンテナンスモードテストNo取得 */
	EINV_FID_SET_MAINTENANCE_INFO,							/* メンテナンスモード情報設定 */
	EINV_FID_BLOCK_OPEMODE,									/* 運転モードブロック設定（車両充放電器製造番号書換開始通知) */
	EINV_FID_GET_INVLOG,									/* INVログ取得 */
	EINV_FID_MAX_PLUSONE_TMP								/* ファンクションIDの定義数+1 */
}EinvFid_t;

/* プロセス状態 */
typedef enum _einv_stts_e{
	EINV_STTS_COMM_UNCONF = 1,								/* INV通信未確認 */
	EINV_STTS_COMM_INIT_CONF,								/* INV起動時通信確立 */
	EINV_STTS_COMM_INIT_ERR,								/* INV起動時通信エラー */
	EINV_STTS_COMM_CONF,									/* INV通信確立 */
	EINV_STTS_COMM_ERR										/* INV通信エラー */
}EinvStts_t;

/* ワンショットタイマID */
typedef enum _einv_tim_id_e{
	EINV_TIM_ID_CYCLE		= 10,							/* 周期情報取得処理 */
	EINV_TIM_ID_LOG			= 20,							/* 動作ログ取得処理 */
	EINV_TIM_ID_COMM_ERR	= 30,							/* INV通信エラー処理 */
	EINV_TIM_ID_FWUPDATE	= 40,							/* INVFW更新処理 */
	EPCS_TIM_ID_SURCHARGE	= 50,							/* 余剰充電制御情報設定処理 */
	EPCS_TIM_ID_MAINTENANCE = 60,							/* メンテナンスモード開始処理 */

	/* ここから、1を増加 */
	EINV_TIM_ID_SELLCHARGSTART		= 100,					/* 売電モード充電開始時刻 */
	EINV_TIM_ID_SELLCHARGSTOP		= 101,					/* 売電モード充電停止時刻 */
	EINV_TIM_ID_SELLDISCHARGSTART	= 102,					/* 売電モード放電開始時刻 */
	EINV_TIM_ID_SELLDISCHARGSTOP	= 103,					/* 売電モード放電停止時刻 */
	EINV_TIM_ID_GREENCHARGSTART		= 104,					/* グリーンモード充電開始時刻 */
	EINV_TIM_ID_GREENCHARGSTOP		= 105,					/* グリーンモード充電停止時刻 */
	EINV_TIM_ID_GREENDISCHARGSTART	= 106,					/* グリーンモード放電開始時刻 */
	EINV_TIM_ID_GREENDISCHARGSTOP	= 107,					/* グリーンモード放電停止時刻 */
	EINV_TIM_ID_SIXHOUR				= 108,					/* 6時間継続 */
	EINV_TIM_ID_TESTRUNCYCLE 		= 109,					/* 試運転周期情報取得処理 1s*/
	EINV_TIM_ID_TESTRUNRESET 		= 110,					/* 試運転ソフトリセット処理 30s*/
	EINV_TIM_ID_TESTRUNLOSTLAN 		= 111,					/* 試運転スマートフォン通信途絶処理 1分*/
	EINV_TIM_ID_TESTRUNTMOUT 		= 112,					/* 試運転TMOUT*/
	EINV_TIM_ID_INIT_ERR			= 113					/* INVマイコン初期化通信失敗 */
}EinvTimid_t;


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/


#endif /*__EINV_MSG_G_H__*/
