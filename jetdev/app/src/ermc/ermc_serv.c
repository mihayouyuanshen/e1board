/****************************************************************************/
/*	Description		:オプションリモコン通信機能 業務処理					*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC KQ.Liang											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/29												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ERMC													*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <poll.h>
#include <ctype.h>
#include <errno.h>
#include <semaphore.h>

#include "task_g.h"
#include "dlog_g.h"
#include "dgpio_g.h"
#include "esys_g.h"
#include "etim_g.h"

#include "ermc_cmn_g.h"
#include "ermc_serv_g.h"
#include "ermc_serv.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
ProcStatus_t			g_ermcProcStatus = 0u;								/* プロセス状態 */
ErmcRmcCommStatus_t		g_ermcRmcCommStatus = 0u;							/* OPリモコン通信状態 */

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
FrmPthread_t			s_ermcRecvMsgThread = 0;							/* 電文受信スレッド識別変数 */
ushort					s_ermcInvInitCommFinFlag = 0u;						/* INVマイコン初期化通信完了フラグ */
ushort					s_ermcRmcCommFailSetFlag = 0u;						/* OPリモコン通信不通エラー設定フラグ */
ushort					s_ermcMsgResTimerRegFlag = 0u;						/* 電文応答タイマ登録フラグ */
ushort					s_ermcRmcCommTimerRegFlag = 0u;						/* OPリモコン通信タイマ登録フラグ */
sint					s_ermcUartFd = -1;									/* UART3ドライバのファイルディスクリプタ */
sint					s_ermcGpioDesc = -1;								/* データイネーブル用GPIOディスクリプタ	*/
RecvMsgParseInfo_t		s_ermcRecvMsgParseInfo;								/* 受信電文解析情報 */
RecvMsgDataInfo_t		s_ermcRecvMsgDataInfo;								/* 受信電文データ情報 */
ResMsgDataInfo_t		s_ermcResMsgDataInfo;								/* 応答電文データ情報(応答電文作成用) */
uchar s_ermcRecvBuf[ERMC_MSG_FORMAT_SIZE + ERMC_MSG_DATA_BUF_SIZE];			/* 受信電文バッファ */
uchar s_ermcResBuf[ERMC_MSG_FORMAT_SIZE + ERMC_MSG_DATA_BUF_SIZE];			/* 応答電文バッファ */
ushort					s_ermcReadElapsedMs = 0u;							/* 電文読み込み経過時間[ms] */
sem_t s_ermcSem;

CmdTable_t s_ermcCmdTable[ERMC_MSG_CMD_CNT_MAX] =
{
		/* CMD0 */
	{	ERMC_CMD0_REQ_MSG_RECV_EVENT,			/* 受信イベント						：(CMD0)要求電文受信		 */
		ERMC_MSG_REQ_COMMAND_CMD0,				/* 要求電文Command部				："G00"						 */
		ERMC_CDM0_REQ_MSG_DATA_SIZE,			/* 要求電文Data部サイズ				：0							 */
		NULL,									/* 要求電文データ情報設定処理		：処理なし					 */
		ermc_GetCmd0ResDataInfo,				/* 応答電文データ情報取得処理		：ermc_GetCmd0ResDataInfo	 */
		NULL,									/* 応答電文データ情報初期アドレス	：NULL						 */
		ERMC_CDM0_RES_MSG_DATA_SIZE		},		/* 応答電文Data部サイズ				：2							 */

		/* CMD24 */
	{	ERMC_CMD24_REQ_MSG_RECV_EVENT,			/* 受信イベント						：(CMD24)要求電文受信		 */
		ERMC_MSG_REQ_COMMAND_CMD24,				/* 要求電文Command部				："G24"						 */
		ERMC_CDM24_REQ_MSG_DATA_SIZE,			/* 要求電文Data部サイズ				：0							 */
		NULL,									/* 要求電文データ情報設定処理		：処理なし					 */
		ermc_GetCmd24ResDataInfo,				/* 応答電文データ情報取得処理		：ermc_GetCmd24ResDataInfo	 */
		NULL,									/* 応答電文データ情報初期アドレス	：NULL						 */
		ERMC_CDM24_RES_MSG_DATA_SIZE	},		/* 応答電文Data部サイズ				：38						 */

		/* CMD41 */
	{	ERMC_CMD41_REQ_MSG_RECV_EVENT,			/* 受信イベント						：(CMD41)要求電文受信		 */
		ERMC_MSG_REQ_COMMAND_CMD41,				/* 要求電文Command部				："S41"						 */
		ERMC_CDM41_REQ_MSG_DATA_SIZE,			/* 要求電文Data部サイズ				：54						 */
		ermc_SetCmd41ReqDataInfo,				/* 要求電文データ情報設定処理		：ermc_SetCmd41ReqDataInfo	 */
		ermc_GetCmd41ResDataInfo,				/* 応答電文データ情報取得処理		：ermc_GetCmd41ResDataInfo	 */
		NULL,									/* 応答電文データ情報初期アドレス	：NULL						 */
		ERMC_CDM41_RES_MSG_DATA_SIZE	}		/* 応答電文Data部サイズ				：357						 */
};

const ActionTable_t s_ermcActionTable
						[ERMC_PROC_STTS_MAX][ERMC_PROC_ACTION_MAX] =
{
	/* ERMC_PROC_STTS_IDLE		：IDLE */
	{
		{	ERMC_PROC_STTS_IDLE,		NULL,			NULL				},	/* ERMC_PROC_ACTION_IDLE：IDLE */
		{	ERMC_PROC_STTS_RECEIVING,	NULL,			NULL				},	/* ERMC_PROC_ACTION_RECV：受信 */
		{	ERMC_PROC_STTS_IDLE,		NULL,			NULL				},	/* ERMC_PROC_ACTION_WAIT：待ち */
		{	ERMC_PROC_STTS_IDLE,		NULL,			NULL				}	/* ERMC_PROC_ACTION_SEND：送信 */
	},

	/* ERMC_PROC_STTS_RECEIVING	：電文受信処理中 */
	{
		{	ERMC_PROC_STTS_IDLE,		NULL,			NULL				},	/* ERMC_PROC_ACTION_IDLE：IDLE */
		{	ERMC_PROC_STTS_RECEIVING,	NULL,			NULL				},	/* ERMC_PROC_ACTION_RECV：受信 */
		{	ERMC_PROC_STTS_WAITING,		NULL,			NULL				},	/* ERMC_PROC_ACTION_WAIT：待ち */
		{	ERMC_PROC_STTS_SENDING,		ermc_SendMsg,	NULL				}	/* ERMC_PROC_ACTION_SEND：送信 */
	},

	/* ERMC_PROC_STTS_WAITING	：情報取得待ち中 */
	{
		{	ERMC_PROC_STTS_IDLE,		NULL,			ermc_Callsempost	},	/* ERMC_PROC_ACTION_IDLE：IDLE */
		{	ERMC_PROC_STTS_RECEIVING,	NULL,			ermc_Callsempost	},	/* ERMC_PROC_ACTION_RECV：受信 */
		{	ERMC_PROC_STTS_WAITING,		NULL,			NULL				},	/* ERMC_PROC_ACTION_WAIT：待ち */
		{	ERMC_PROC_STTS_SENDING,		ermc_SendMsg,	NULL				}	/* ERMC_PROC_ACTION_SEND：送信 */
	},

	/* ERMC_PROC_STTS_SENDING	：電文送信処理中 */
	{
		{	ERMC_PROC_STTS_IDLE,		NULL,			NULL				},	/* ERMC_PROC_ACTION_IDLE：IDLE */
		{	ERMC_PROC_STTS_RECEIVING,	NULL,			ermc_Callsempost	},	/* ERMC_PROC_ACTION_RECV：受信 */
		{	ERMC_PROC_STTS_SENDING,		NULL,			NULL				},	/* ERMC_PROC_ACTION_WAIT：待ち */
		{	ERMC_PROC_STTS_SENDING,		NULL,			NULL				}	/* ERMC_PROC_ACTION_SEND：送信 */
	}
};

const Cmd0ResMsgDataInfo_t s_ermcCmd0InitData =
{
	/* 初期値 */
	"T "					/* モデル世代名 */
};

const Cmd24ResMsgDataInfo_t s_ermcCmd24InitData =
{
	/* 初期値 */
	'6',					/* 運転モード */
	'0',					/* 非常時運転モード */
	'0',					/* 非常時安心設定(放電下限値設定) [未使用] */
	"2305",					/* 充電開始時刻 */
	"0659",					/* 充電停止時刻 */
	"1000",					/* 放電開始時刻 */
	"2259",					/* 放電停止時刻 */
	'0',					/* 運転／停止 */
	'0',					/* 充電上限値設定 [未使用] */
	"000",					/* 蓄電池放電下限値設定 */
	"100",					/* 蓄電池充電上限値設定 */
	'0',					/* 余剰充電制御 [未使用] */
	"1000",					/* 余剰充電制御開始時刻 [未使用] */
	"2259",					/* 余剰充電制御停止時刻 [未使用] */
	'0',					/* 車両放電時間帯制限基準 [未使用] */
	'0'						/* 試運転時停電出力 [未使用] */
};

const Cmd41ResMsgDataInfo_t s_ermcCmd41InitData =
{
	/* 初期値 */
	"0000",					/* AC充放電電力（インバータ部） */
	'0',					/* インバータ動作状態 */
	"00000000",				/* AC積算充電電力量（インバータ部） [未使用] */
	"00000000",				/* AC積算放電電力量（インバータ部） [未使用] */
	"0000",					/* rSOC */
	"0000",					/* 電池残量（Ah） [未使用] */
	"0000",					/* 電池残量（kWh） */
	"0000",					/* SOC */
	"0000",					/* 電池電圧 [未使用] */
	"0000",					/* 充放電サイクル数 [未使用] */
	"0000",					/* SOH */
	'0',					/* 蓄電池運転動作状態 */
	'0',					/* 停電 */
	"0000",					/* PV自立入力電力 */
	"0000",					/* PV自立入力電圧 [未使用] */
	"0000",					/* PV発電電力（本製品以外のPV） */
	"0000",					/* 逆潮流電力 */
	"0000",					/* 系統電圧 [未使用] */
	"BE000",				/* PCSエラー */
	'0',					/* PCSエラーレーティング */
	'6',					/* 運転モード */
	"00",					/* 運転状態 [未使用] */
	"00000000",				/* AC蓄電池積算充電電力量（インバータ部） [未使用] */
	"00000000",				/* AC蓄電池積算放電電力量（インバータ部） [未使用] */
	"000000",				/* 通算稼働時間 [未使用] */
	"0000",					/* DC蓄電池充放電電力２（コンバータ部） */
	"0000",					/* 太陽光発電電力（内蔵PV） */
	"00000000",				/* DC蓄電池積算充電電力量２（コンバータ部） [未使用] */
	"00000000",				/* DC蓄電池積算放電電力量２（コンバータ部） [未使用] */
	"00000000",				/* 積算太陽光発電電力量（内蔵PV） [未使用] */
	"00000000",				/* 積算PV発電電力量（本製品以外のPV） [未使用] */
	"00000000",				/* 積算買電電力量 [未使用] */
	"00000000",				/* 積算売電電力量 [未使用] */
	"0590",					/* 充電電力設定値（蓄電池）(kW) [未使用] */
	"9999",					/* 発電電力制限値(kW) [未使用] */
	"0590",					/* 放電電力設定値（蓄電池）(kW) [未使用] */
	"0000",					/* 家庭負荷電力 */
	"00000000",				/* AC充電実効容量（蓄電池） [未使用] */
	"00000000",				/* AC放電実効容量（蓄電池） [未使用] */
	"00000000",				/* AC充電可能容量（蓄電池） [未使用] */
	"00000000",				/* AC放電可能容量（蓄電池） [未使用] */
	"00000000",				/* AC充電可能量（蓄電池） [未使用] */
	"00000000",				/* AC放電可能量（蓄電池） [未使用] */
	'0',					/* 出力抑制状態 */
	'0',					/* コネクタロック状態 */
	'0',					/* 車両充放電器運転動作状態 [未使用] */
	"00000000",				/* 車載電池充電電力設定値 [未使用] */
	"00000000",				/* 車載電池放電電力設定値 [未使用] */
	"0000",					/* 車載電池充電電流設定値 [未使用] */
	"0000",					/* 車載電池放電電流設定値 [未使用] */
	"00000000",				/* 車載電池の放電可能残容量１ [未使用] */
	"0000",					/* 車載電池の放電可能残容量２ [未使用] */
	"00",					/* 車載電池の放電可能残容量３ [未使用] */
	"00000000",				/* 車載電池の充電可能残容量値 [未使用] */
	"FF",					/* 車両接続・充放電可否状態 [未使用] */
	"00000000",				/* 車両充放電器瞬時充放電電力計測値 */
	"0000",					/* 車両充放電器瞬時充放電電流計測値 [未使用] */
	"0000",					/* 車両充放電器瞬時充放電電圧計測値 [未使用] */
	"00000000",				/* 車載電池の積算放電電力量計測値 [未使用] */
	"00000000",				/* 車載電池の積算充電電力量計測値 [未使用] */
	'0',					/* 非常時運転モード */
	"00000000",				/* 車載電池の電池残容量1 */
	"0000",					/* 車載電池の電池残容量２ [未使用] */
	"00",					/* 車載電池の電池残容量３ */
	"0000",					/* 車両充電終了目安時間 [未使用] */
	'0',					/* 蓄電池満充電フラグ [未使用] */
	'0',					/* 蓄電池空放電フラグ [未使用] */
	'0',					/* 車載電池満充電フラグ [未使用] */
	'0',					/* 車載電池空放電フラグ [未使用] */
	'0',					/* 車両充放電器制御要求フラグ [未使用] */
	"230101000000",			/* 通信ボード現在時刻 */
	'0',					/* 通信ボード通信状態 */
	"000",					/* 通信ボード通信確認状態 */
	'0',					/* リモート運転状態 */
	'0',					/* 時刻同期状態 */
	"00",					/* 抑制率％ */
	"230615",				/* メンテナンスモード実施日 [未使用] */
	"--"					/* 表示用運転モード */
};

const BoardOpeModeConvTable_t s_ermcBoardOpeModeConv
								[ERMC_BOARD_OPE_MODE_CNT_MAX] =
{
	{'0', '1'},				/* 通信ボード内管理運転モード(0:売電モード),			運転モード(1:売電モード) */
	{'1', '2'},				/* 通信ボード内管理運転モード(1:グリーンモード),		運転モード(2:グリーンモード) */
	{'2', '4'},				/* 通信ボード内管理運転モード(2:蓄電池充電),			運転モード(4:蓄電池充電) */
	{'3', '5'},				/* 通信ボード内管理運転モード(3:蓄電池放電),			運転モード(5:蓄電池放電) */
	{'4', 'B'},				/* 通信ボード内管理運転モード(4:車両充電),				運転モード(B:車両充電) */
	{'5', 'C'},				/* 通信ボード内管理運転モード(5:車両放電),				運転モード(C:車両放電) */
	{'6', 'N'},				/* 通信ボード内管理運転モード(6:蓄電池・車両同時放電),	運転モード(N:蓄電池・車両同時放電) */
	{'7', '6'}				/* 通信ボード内管理運転モード(7:待機),					運転モード(6:待機) */
};

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:グローバル変数を初期化する								*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:本モジュールに使用するグローバル変数を初期化する		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/24 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
void ermc_InitGlobalVariable(void)
{
	DEBUG_TRCIN();

	/* グローバル変数初期化 */
	g_ermcProcStatus = ERMC_PROC_STTS_IDLE;									/* プロセス状態 */
	g_ermcRmcCommStatus = ERMC_RMC_COMM_STTS_DISCONNECT;					/* OPリモコン通信状態 */
	s_ermcInvInitCommFinFlag = ERMC_INV_INIT_COMM_FIN_FLAG_OFF;				/* INVマイコン初期化通信完了フラグ */
	s_ermcRmcCommFailSetFlag = ERMC_COMM_FAIL_SET_FLAG_OFF;					/* OPリモコン通信不通エラー設定フラグ */
	s_ermcMsgResTimerRegFlag = ERMC_COMM_TIMER_REG_FLAG_OFF;				/* 電文応答タイマ登録フラグ */
	s_ermcRmcCommTimerRegFlag = ERMC_COMM_TIMER_REG_FLAG_OFF;				/* OPリモコン通信タイマ登録フラグ */
	s_ermcUartFd = -1;														/* UART3ドライバのファイルディスクリプタ */
	s_ermcGpioDesc = -1;													/* データイネーブル用GPIOディスクリプタ	*/
	memset(s_ermcRecvBuf, 0u, sizeof(s_ermcRecvBuf));						/* 受信電文バッファ */
	memset(&s_ermcRecvMsgParseInfo, 0u, sizeof(s_ermcRecvMsgParseInfo));	/* 受信電文解析情報 */
	memset(&s_ermcRecvMsgDataInfo, 0u, sizeof(s_ermcRecvMsgDataInfo));		/* 受信電文データ情報 */
	memset(&s_ermcResMsgDataInfo, 0u, sizeof(s_ermcResMsgDataInfo));		/* 応答電文データ情報(応答電文作成用) */
	memset(s_ermcResBuf, 0u, sizeof(s_ermcResBuf));							/* 応答電文バッファ */

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:INVマイコン初期化通信完了通知受信処理を行う			*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:INVマイコン初期化通信完了フラグをONに設定する			*/
/*					 プロセス状態が「情報取得待ち中」の場合、				*/
/*					 応答データ情報設定、状態機実行処理を行う				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/29 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
void ermc_RecvInvInitCommFinNty(void)
{
	DEBUG_TRCIN();

	/* INVマイコン初期化通信完了フラグ設定：ON */
	s_ermcInvInitCommFinFlag = ERMC_INV_INIT_COMM_FIN_FLAG_ON;

	if (ERMC_PROC_STTS_WAITING == g_ermcProcStatus) {

		/* 応答電文データ情報(応答電文作成用)設定 */
		memcpy(&s_ermcResMsgDataInfo.cmd0DataInfo,
				&s_ermcCmd0InitData,
				sizeof(s_ermcResMsgDataInfo.cmd0DataInfo));

		/* 応答データ情報設定 */
		s_ermcCmdTable[s_ermcRecvMsgParseInfo.cmdTableIndex].resDataInfo =
			(uchar*)&s_ermcResMsgDataInfo.cmd0DataInfo;

		DLOG_Info("CMD0 response msg data info get finished.");

		/* 状態機実行 */
		ermc_RunStateMachine(ERMC_PROC_ACTION_SEND);
	}
	else {
		/* 処理なし */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:OPリモコン通信開始処理を行う							*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:UART、GPIOオープン処理を行い、							*/
/*					 成功した場合は状態機実行、電文受信スレッド生成処理、	*/
/*					 失敗した場合はUART、GPIOクローズ処理を行う				*/
/*					 モジュール起動結果をesysに通知する						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/24 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
void ermc_StartRmcComm(void)
{
	ModActionResult_t result = MOD_ACTION_RESULT_FAILED;	/* モジュール起動結果 */
	sshort fncRet1 = ERMC_RET_INITIAL_VALUE;				/* 呼び出し関数戻り値 */
	sshort fncRet2 = ERMC_RET_INITIAL_VALUE;				/* 呼び出し関数戻り値 */
	slong cmnRet = FRM_ERROR;								/* 共通関数戻り値 */

	DEBUG_TRCIN();

	DLOG_Info("oprmc communication start.");

	sem_init(&s_ermcSem, 0, 0u);

	/* 通信用UARTを開き、初期設定 */
	fncRet1 = ermc_OpenUart();

	/* 送受信切り替え用GPIOのファイルディスクリプタを開く */
	fncRet2 = ermc_OpenGpio();

	/* リターンコード判定 */
	if ((ERMC_RET_OK == fncRet1) && (ERMC_RET_OK == fncRet2)) {

		/* 状態機実行 */
		ermc_RunStateMachine(ERMC_PROC_ACTION_RECV);

		/* 電文受信スレッド生成 */
		cmnRet = FRM_PthreadCreate(
					&s_ermcRecvMsgThread,
					NULL,
					ermc_RecvMsgThread,
					NULL);

		if (FRM_SUCCESS != cmnRet) {
			/* スレッド生成失敗 */
			DLOG_Error("FRM_PthreadCreate() error.");
		}
		else {
			/* モジュール起動結果設定：成功 */
			result = MOD_ACTION_RESULT_SUCCESS;
		}
	}
	else {
		/* 通信用UARTを閉じる */
		ermc_CloseUart();

		/* 送受信切り替え用GPIOのファイルディスクリプタを閉じる */
		ermc_CloseGpio();
	}

	/* モジュール起動結果通知 */
	ESYS_NotifyStartResult(ERMC_SPORT, result);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:OPリモコン通信停止処理を行う							*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:UART、GPIOクローズ、状態機実行処理を行い、				*/
/*					 モジュール停止結果をesysに通知する						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/24 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
void ermc_StopRmcComm(void)
{
	DEBUG_TRCIN();

	/* 通信用UARTを閉じる */
	ermc_CloseUart();

	/* 送受信切り替え用GPIOのファイルディスクリプタを閉じる */
	ermc_CloseGpio();

	/* 状態機実行 */
	ermc_RunStateMachine(ERMC_PROC_ACTION_IDLE);

	sem_destroy(&s_ermcSem);

	DLOG_Info("oprmc communication stop.");

	/* モジュール停止結果通知 */
	ESYS_NotifyStopResult(ERMC_SPORT, MOD_ACTION_RESULT_SUCCESS);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:電文応答タイマタイムアウト処理を行う					*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:下記処理を行う											*/
/*					 状態機実行処理、										*/
/*					 OPリモコン通信に関する項目の設定処理					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
void ermc_RecvMsgResTimerTimeOut(void)
{
	DEBUG_TRCIN();

	/* OPリモコン通信に関する項目の設定処理 */
	ermc_SetRmcCommRelatedItems(ERMC_MSG_RES_TIMER_TO_EVENT);

	/* 状態機実行 */
	ermc_RunStateMachine(ERMC_PROC_ACTION_RECV);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:OPリモコン通信タイマタイムアウト処理を行う				*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:OPリモコン通信に関する項目の設定処理を行う				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
void ermc_RecvRmcCommTimerTimeOut(void)
{
	DEBUG_TRCIN();

	/* OPリモコン通信に関する項目の設定処理 */
	ermc_SetRmcCommRelatedItems(ERMC_RMC_COMM_TIMER_TO_EVENT);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:プロセス状態変更要求受信処理を行う						*/
/*--------------------------------------------------------------------------*/
/*	param			:msg プロセス状態変更要求メッセージ						*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:状態機実行処理を呼び出す								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/28 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
void ermc_RecvProcStatusChangeReq(ProcStatusChangeReq_t* msg)
{
	DEBUG_TRCIN();

	/* 状態機実行 */
	ermc_RunStateMachine(msg->nextAction);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:(CMD24)応答電文データ情報受信処理を行う				*/
/*--------------------------------------------------------------------------*/
/*	param			:msg (CMD24)情報取得完了通知メッセージ					*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:入力したメッセージで(CMD24)応答電文データ情報を設定する*/
/*					 状態機実行処理を呼び出す								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/31 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
void ermc_RecvCmd24ResMsgDataInfo(Cmd24InfoGetFinNty_t* msg)
{
	Cmd24ResMsgDataInfo_t dataInfo;		/* (CMD24)応答電文データ情報設定用 */

	DEBUG_TRCIN();

	/* (CMD24)応答電文データ情報設定 */
	/* 初期値設定 */
	memcpy(&dataInfo, &s_ermcCmd24InitData, sizeof(dataInfo));

	/* 運転／停止 */
	dataInfo.startOrStop = msg->dataInfo.startOrStop;

	/* 応答電文データ情報(応答電文作成用)設定 */
	memcpy(&s_ermcResMsgDataInfo.cmd24DataInfo,
			&dataInfo,
			sizeof(s_ermcResMsgDataInfo.cmd24DataInfo));

	/* 応答データ情報設定 */
	s_ermcCmdTable[s_ermcRecvMsgParseInfo.cmdTableIndex].resDataInfo =
		(uchar*)&s_ermcResMsgDataInfo.cmd24DataInfo;

	DLOG_Info("CMD24 response msg data info get finished.");

	/* 状態機実行 */
	ermc_RunStateMachine(ERMC_PROC_ACTION_SEND);

	DEBUG_TRCOUT();
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:UARTオープン処理を行う									*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:ERMC_RET_OK 正常終了									*/
/*					 ERMC_RET_NG 異常終了									*/
/*--------------------------------------------------------------------------*/
/*	detail			:通信用UARTを開いて初期設定処理を行う					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/24 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static sshort ermc_OpenUart(void)
{
	sshort retCode = ERMC_RET_INITIAL_VALUE;		/* リターンコード */
	sint fncRet1 = 0;								/* cfsetispeed関数戻り値 */
	sint fncRet2 = 0;								/* cfsetospeed関数戻り値 */
	sint fncRet3 = 0;								/* tcflush関数戻り値 */
	sint fncRet4 = 0;								/* tcsetattr関数戻り値 */
	struct termios newTio;							/* ポート設定用 */

	DEBUG_TRCIN();

	/***** 通信ポートオープン、初期化 *****/
	if (0 > s_ermcUartFd) {
		s_ermcUartFd = open(ERMC_COMM_SERIAL_PORT, (O_RDWR | O_SYNC), 0u);

		if (0 <= s_ermcUartFd) {
			DLOG_Info("serial port (%s) open success.",
							ERMC_COMM_SERIAL_PORT);

			/* 新しいポートの設定 */
			memset(&newTio, 0u, sizeof(newTio));					/* オールクリア */
			newTio.c_iflag = IGNPAR;								/* パリティ無視 */
			newTio.c_cflag = (B57600 | CS8 | CREAD | CLOCAL);		/* ボーレート,データ長8,受信有効,モデム制御線を無視 */

			/* ボーレートの設定 */
			fncRet1 = cfsetispeed(&newTio, B57600);					/* 入力ボーレートの設定 */
			fncRet2 = cfsetospeed(&newTio, B57600);					/* 出力ボーレートの設定 */
			fncRet3 = tcflush(s_ermcUartFd, TCIFLUSH);				/* 受信したが読んでいないデータをflush */
			fncRet4 = tcsetattr(s_ermcUartFd, TCSANOW, &newTio);	/* 設定反映 */

			/* リターンコード判定 */
			if ((0 == fncRet1) &&
				(0 == fncRet2) &&
				(0 == fncRet3) &&
				(0 == fncRet4)) {
				DLOG_Info("serial port (%s) set success.",
								ERMC_COMM_SERIAL_PORT);
				retCode = ERMC_RET_OK;
			}
			else {
				DLOG_Error("serial port (%s) set error.",
								ERMC_COMM_SERIAL_PORT);
				retCode = ERMC_RET_NG;
			}
		}
		else {
			DLOG_Error("serial port (%s) open error.",
							ERMC_COMM_SERIAL_PORT);
			retCode = ERMC_RET_NG;
		}
	}
	else {
		/* 既に開いている場合 */
		DLOG_Info("serial port (%s) already open.",
						ERMC_COMM_SERIAL_PORT);
		retCode = ERMC_RET_OK;
	}

	DEBUG_TRCOUT();

	return retCode;
}

/*==========================================================================*/
/*	Description		:GPIOオープン処理を行う									*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:ERMC_RET_OK 正常終了									*/
/*					 ERMC_RET_NG 異常終了									*/
/*--------------------------------------------------------------------------*/
/*	detail			:送受信切り替え用GPIOのファイルディスクリプタを開く		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/24 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static sshort ermc_OpenGpio(void)
{
	sshort retCode = ERMC_RET_INITIAL_VALUE;		/* リターンコード */

	DEBUG_TRCIN();

	if (0 > s_ermcGpioDesc) {
		/* DE出力処理(GPIO) */
		s_ermcGpioDesc = DGPIO_Open(DGPIO_PORT_RS485_DE);
		if (0 <= s_ermcGpioDesc) {
			DLOG_Info("gpiodesc open success.");
			retCode = ERMC_RET_OK;
		}
		else {
			DLOG_Error("gpiodesc open fail.");
			retCode = ERMC_RET_NG;
		}
	}
	else {
		/* 既に開いている場合 */
		DLOG_Info("gpiodesc already open.");
		retCode = ERMC_RET_OK;
	}

	DEBUG_TRCOUT();

	return retCode;
}

/*==========================================================================*/
/*	Description		:UARTクローズ処理を行う									*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:通信用UARTのファイルディスクリプタを閉じる				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/24 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void ermc_CloseUart(void)
{
	DEBUG_TRCIN();

	if (0 <= s_ermcUartFd) {
		/* 通信用UARTクローズ */
		close(s_ermcUartFd);
		s_ermcUartFd = -1;

		DLOG_Info("serial port (%s) close.", ERMC_COMM_SERIAL_PORT);
	}
	else {
		/* 処理なし */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:GPIOクローズ処理を行う									*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:送受信切り替え用GPIOのファイルディスクリプタを閉じる	*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/24 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void ermc_CloseGpio(void)
{
	DEBUG_TRCIN();

	if (0 <= s_ermcGpioDesc) {
		/* DE出力処理(GPIO) */
		DGPIO_Close(s_ermcGpioDesc, DGPIO_PORT_RS485_DE);
		s_ermcGpioDesc = -1;

		DLOG_Info("gpiodesc close.");
	}
	else {
		/* 処理なし */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:読み取り状態制御処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:GPIO制御でRS485通信を入力状態にする					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/24 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static sshort ermc_EnableRead(void)
{
	sshort retCode = ERMC_RET_INITIAL_VALUE;		/* リターンコード */
	sshort fncRet = ERMC_RET_INITIAL_VALUE;			/* 呼び出し関数戻り値 */

	DEBUG_TRCIN();

	if (0 > s_ermcGpioDesc) {
		/* GPIOオープン処理 */
		fncRet = ermc_OpenGpio();

		/* リターンコード判定 */
		if (ERMC_RET_OK == fncRet) {
			/* DEを入力状態にする(GPIO) */
			DGPIO_Write(s_ermcGpioDesc, DGPIO_VAL_LO);

			retCode = ERMC_RET_OK;
		}
		else {
			retCode = ERMC_RET_NG;
		}
	}
	else {
		/* DEを入力状態にする(GPIO) */
		DGPIO_Write(s_ermcGpioDesc, DGPIO_VAL_LO);

		retCode = ERMC_RET_OK;
	}

	DEBUG_TRCOUT();

	return retCode;
}

/*==========================================================================*/
/*	Description		:書き込み状態制御処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:GPIO制御でRS485通信を出力状態にする					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/24 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static sshort ermc_EnableWrite(void)
{
	sshort retCode = ERMC_RET_INITIAL_VALUE;		/* リターンコード */
	sshort fncRet = ERMC_RET_INITIAL_VALUE;			/* 呼び出し関数戻り値 */

	DEBUG_TRCIN();

	if (0 > s_ermcGpioDesc) {
		/* GPIOオープン処理 */
		fncRet = ermc_OpenGpio();

		/* リターンコード判定 */
		if (ERMC_RET_OK == fncRet) {
			/* DEを出力状態にする(GPIO) */
			DGPIO_Write(s_ermcGpioDesc, DGPIO_VAL_HI);

			retCode = ERMC_RET_OK;
		}
		else {
			retCode = ERMC_RET_NG;
		}
	}
	else {
		/* DEを出力状態にする(GPIO) */
		DGPIO_Write(s_ermcGpioDesc, DGPIO_VAL_HI);

		retCode = ERMC_RET_OK;
	}

	DEBUG_TRCOUT();

	return retCode;
}

/*==========================================================================*/
/*	Description		:現在の時間(秒)を取得する								*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:現在の時間(秒)											*/
/*--------------------------------------------------------------------------*/
/*	detail			:1970/01/01 00:00:00から現在までの時間(秒)を取得する	*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static double ermc_GetTimeSec(void)
{
	struct timeval tv;

	DEBUG_TRCIN();

	gettimeofday(&tv, NULL);

	DEBUG_TRCOUT();

	return ((double)tv.tv_sec + ((double)tv.tv_usec * 1e-6));
}

/*==========================================================================*/
/*	Description		:システム関数nanosleepを呼び出す						*/
/*--------------------------------------------------------------------------*/
/*	param			:sleepMs スリープ時間[ms]								*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:nanosleepを呼び出し、入力した時間でスリープする		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void ermc_Callnanosleep(ushort sleepMs)
{
	ushort stopFlag = ERMC_LOOP_STOP_FLAG_OFF;		/* ループ処理停止用フラグ */
	sint fncRet = 0;								/* nanosleep関数戻り値 */
	struct timespec	reqTime;						/* スリープ要求時間 */
	struct timespec	remTime;						/* スリープ残り時間 */
	char ebuf[256];

	DEBUG_TRCIN();

	/* ローカル変数初期化 */
	memset(&reqTime, 0u, sizeof(reqTime));			/* スリープ要求時間 */
	memset(&remTime, 0u, sizeof(remTime));			/* スリープ残り時間 */

	reqTime.tv_nsec = sleepMs * 1000u * 1000u;		/* [ms]→[ns] */

	while (ERMC_LOOP_STOP_FLAG_OFF == stopFlag) {
		fncRet = nanosleep(&reqTime, &remTime);
		if (0 == fncRet) {
			/* nanosleep()が正常終了 */
			stopFlag = ERMC_LOOP_STOP_FLAG_ON;
		}
		else {
			memset(ebuf, 0u, sizeof(ebuf));
			strerror_r(errno, ebuf, 256);
			if (errno == EINTR) {
				/* nanosleep()がシグナルで停止したので残り時間分再sleep */
				reqTime.tv_sec = remTime.tv_sec;
				reqTime.tv_nsec = remTime.tv_nsec;
			}
			else {
				/* nanosleep()がエラー */
				stopFlag = ERMC_LOOP_STOP_FLAG_ON;
			}
		}
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:システム関数sem_waitを呼び出す							*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:システム関数sem_waitを呼び出す							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/27 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void ermc_Callsemwait(void)
{
	DEBUG_TRCIN();

	sem_wait(&s_ermcSem);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:システム関数sem_postを呼び出す							*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:システム関数sem_postを呼び出す							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/27 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void ermc_Callsempost(void)
{
	DEBUG_TRCIN();

	sem_post(&s_ermcSem);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:電文応答タイマ登録処理を行う							*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:タイマ設定の共通関数を呼び出し、						*/
/*					 算出したタイムアウト時間で電文応答タイマ登録を行う		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void ermc_RegMsgResTimer(void)
{
	DEBUG_TRCIN();

	/* 電文応答タイマ登録フラグ判定 */
	if (ERMC_COMM_TIMER_REG_FLAG_ON == s_ermcMsgResTimerRegFlag) {

		/* 電文応答タイマ解除 */
		FRM_TimerCancelOneShot(ERMC_TIMER_ID_MSG_RES);

		DLOG_Info("msg response timer cancelled.");
	}
	else {
		/* 処理なし */
	}

	/* 電文応答タイマ登録 */
	FRM_TimerSetOneShot(
		ERMC_TIMER_ID_MSG_RES,
		(ulong)(ERMC_COMM_SEND_TIMEOUT - s_ermcReadElapsedMs),
		NULL);

	DLOG_Info("msg response timer registered.");

	/* 電文応答タイマ登録フラグ設定：ON */
	s_ermcMsgResTimerRegFlag = ERMC_COMM_TIMER_REG_FLAG_ON;

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:OPリモコン通信タイマ登録処理を行う						*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:タイマ設定の共通関数を呼び出し、						*/
/*					 OPリモコン通信不通判定時間で							*/
/*					 OPリモコン通信タイマ登録を行う							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void ermc_RegRmcCommTimer(void)
{
	DEBUG_TRCIN();

	/* OPリモコン通信タイマ登録フラグ判定 */
	if (ERMC_COMM_TIMER_REG_FLAG_ON == s_ermcRmcCommTimerRegFlag) {

		/* OPリモコン通信タイマ解除 */
		FRM_TimerCancelOneShot(ERMC_TIMER_ID_RMC_COMM);

		DLOG_Info("oprmc communication timer cancelled.");
	}
	else {
		/* 処理なし */
	}

	/* OPリモコン通信タイマ登録 */
	FRM_TimerSetOneShot(
		ERMC_TIMER_ID_RMC_COMM,
		ERMC_COMM_FAIL_DECISION_TIME,
		NULL);

	DLOG_Info("oprmc communication timer registered.");

	/* OPリモコン通信タイマ登録フラグ設定：ON */
	s_ermcRmcCommTimerRegFlag = ERMC_COMM_TIMER_REG_FLAG_ON;

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:文字列から数値に変換する処理(strtol)を行う				*/
/*--------------------------------------------------------------------------*/
/*	param			:str	変換文字列										*/
/*					 len	変換長											*/
/*					 base	変換ベース										*/
/*--------------------------------------------------------------------------*/
/*	return			:convVal 変換値											*/
/*--------------------------------------------------------------------------*/
/*	detail			:strtolを呼び出し、入力した文字列を入力した変換長と		*/
/*					 変換ベースで数値に変換する								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static slong ermc_ConvStrToIntStrtol(schar* str, ushort len, sshort base)
{
	slong convVal = 0;								/* 変換値(strtol関数戻り値) */
	schar tmpBuf[ERMC_MSG_CMN_SIZE_10 + 1];			/* 一時バッファ */

	DEBUG_TRCIN();

	/* ローカル変数初期化 */
	memset(tmpBuf, 0u, sizeof(tmpBuf));				/* 一時バッファ */

	/* 一時バッファ にコピー*/
	memcpy(tmpBuf, str, len);

	/* 数字に変換 */
	convVal = strtol(tmpBuf, NULL, base);

	DEBUG_TRCOUT();

	return convVal;
}

/*==========================================================================*/
/*	Description		:文字列から数値に変換する処理(strtoul)を行う			*/
/*--------------------------------------------------------------------------*/
/*	param			:str	変換文字列										*/
/*					 len	変換長											*/
/*					 base	変換ベース										*/
/*--------------------------------------------------------------------------*/
/*	return			:convVal 変換値											*/
/*--------------------------------------------------------------------------*/
/*	detail			:strtoulを呼び出し、入力した文字列を入力した変換長と	*/
/*					 変換ベースで数値に変換する								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static ulong ermc_ConvStrToIntStrtoul(schar* str, ushort len, sshort base)
{
	ulong convVal = 0u;								/* 変換値(strtol関数戻り値) */
	schar tmpBuf[ERMC_MSG_CMN_SIZE_10 + 1];			/* 一時バッファ */

	DEBUG_TRCIN();

	/* ローカル変数初期化 */
	memset(tmpBuf, 0u, sizeof(tmpBuf));				/* 一時バッファ */

	/* 一時バッファ にコピー*/
	memcpy(tmpBuf, str, len);

	/* 数字に変換 */
	convVal = strtoul(tmpBuf, NULL, base);

	DEBUG_TRCOUT();

	return convVal;
}

/*==========================================================================*/
/*	Description		:通信ボード内管理運転モード変換処理を行う				*/
/*--------------------------------------------------------------------------*/
/*	param			:boardOpeMode 通信ボード内管理運転モード				*/
/*--------------------------------------------------------------------------*/
/*	return			:opeMode 運転モード										*/
/*--------------------------------------------------------------------------*/
/*	detail			:通信ボード内管理運転モードから運転モードに変換する		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/07 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static uchar ermc_ConvBoardOpeMode(uchar boardOpeMode)
{
	ushort stopFlag = ERMC_LOOP_STOP_FLAG_OFF;		/* ループ処理停止用フラグ */
	ushort loopCnt = 0u;							/* ループカウンタ */
	uchar opeMode = 'M';							/* 運転モード(初期値:その他) */

	DEBUG_TRCIN();

	do {
		if (ERMC_BOARD_OPE_MODE_CNT_MAX > loopCnt) {
			if (s_ermcBoardOpeModeConv[loopCnt].boardOpeMode ==
					boardOpeMode) {

				/* 運転モード設定 */
				opeMode = s_ermcBoardOpeModeConv[loopCnt].opeMode;
				stopFlag = ERMC_LOOP_STOP_FLAG_ON;
			}
			else {
				loopCnt++;
			}
		}
		else {
			stopFlag = ERMC_LOOP_STOP_FLAG_ON;
		}
	} while (ERMC_LOOP_STOP_FLAG_OFF == stopFlag);

	DEBUG_TRCOUT();

	return opeMode;
}

/*==========================================================================*/
/*	Description		:OPリモコン通信に関する項目の設定処理を行う				*/
/*--------------------------------------------------------------------------*/
/*	param			:procEvent プロセスイベント								*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:入力したプロセスイベントによる下記処理を行う			*/
/*					 電文応答タイマ登録処理									*/
/*					 OPリモコン通信状態設定									*/
/*					 OPリモコン通信タイマ登録処理							*/
/*					 OPリモコン通信タイマ登録フラグ設定						*/
/*					 OPリモコン通信不通エラー通知/解除						*/
/*					 OPリモコン通信不通エラー設定フラグ設定					*/
/*					 電文応答タイマ登録処理									*/
/*					 電文応答タイマ登録フラグ設定							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/08 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void ermc_SetRmcCommRelatedItems(ProcEvent_t procEvent)
{
	DEBUG_TRCIN();

	switch (procEvent)
	{
	/* (CMD0)要求電文受信イベント */
	case ERMC_CMD0_REQ_MSG_RECV_EVENT:

		/* 処理なし */
		break;

	/* (CMD24)要求電文受信イベント */
	case ERMC_CMD24_REQ_MSG_RECV_EVENT:

		/* 電文応答タイマ登録処理 */
		ermc_RegMsgResTimer();
		break;

	/* (CMD41)要求電文受信イベント */
	case ERMC_CMD41_REQ_MSG_RECV_EVENT:

		/* 電文応答タイマ登録処理 */
		ermc_RegMsgResTimer();

		/* OPリモコン通信状態設定：通信中 */
		g_ermcRmcCommStatus = ERMC_RMC_COMM_STTS_CONNECT;

		/* OPリモコン通信タイマ登録処理 */
		ermc_RegRmcCommTimer();

		/* OPリモコン通信不通エラー設定フラグ判定 */
		if (ERMC_COMM_FAIL_SET_FLAG_ON == s_ermcRmcCommFailSetFlag) {

			/* OPリモコン通信不通エラー解除 */
			EMDTR_ClearBoardError(ERMC_COMM_FAIL_ERROR_CODE);

			/* OPリモコン通信不通エラー設定フラグ設定：OFF */
			s_ermcRmcCommFailSetFlag = ERMC_COMM_FAIL_SET_FLAG_OFF;
		}
		else {
			/* 処理なし */
		}
		break;

	/* 応答電文送信終了イベント */
	case ERMC_RES_MSG_SEND_FIN_EVENT:

		/* 電文応答タイマ登録フラグ判定 */
		if (ERMC_COMM_TIMER_REG_FLAG_ON == s_ermcMsgResTimerRegFlag) {

			/* 電文応答タイマ解除 */
			FRM_TimerCancelOneShot(ERMC_TIMER_ID_MSG_RES);

			/* 電文応答タイマ登録フラグ設定：OFF */
			s_ermcMsgResTimerRegFlag = ERMC_COMM_TIMER_REG_FLAG_OFF;

			DLOG_Info("msg response timer cancelled.");
		}
		else {
			/* 処理なし */
		}

		break;

	/* OPリモコン通信タイマタイムアウトイベント */
	case ERMC_RMC_COMM_TIMER_TO_EVENT:
		DLOG_Info("oprmc communication timeout");

		/* OPリモコン通信タイマ登録フラグ設定：OFF */
		s_ermcRmcCommTimerRegFlag = ERMC_COMM_TIMER_REG_FLAG_OFF;

		/* OPリモコン通信状態設定：通信断 */
		g_ermcRmcCommStatus = ERMC_RMC_COMM_STTS_DISCONNECT;

		/* OPリモコン通信不通エラー通知 */
		EMDTR_SetBoardError(
			ERMC_COMM_FAIL_ERROR_CODE,
			ERMC_COMM_FAIL_ERROR_RATING);

		/* OPリモコン通信不通エラー設定フラグ設定：ON */
		s_ermcRmcCommFailSetFlag = ERMC_COMM_FAIL_SET_FLAG_ON;
		break;

	/* 電文応答タイマタイムアウトイベント */
	case ERMC_MSG_RES_TIMER_TO_EVENT:
		DLOG_Info("msg response timeout.");

		/* 電文応答タイマ登録フラグ設定：OFF */
		s_ermcMsgResTimerRegFlag = ERMC_COMM_TIMER_REG_FLAG_OFF;
		break;

	default:
		DLOG_Error("unsupported process event:%d.", procEvent);
		break;
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:(CMD0)応答電文データ情報取得処理を行う					*/
/*--------------------------------------------------------------------------*/
/*	param			:nextAction 次のプロセスアクション						*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:INVマイコン初期化通信完了フラグがONの場合、			*/
/*					 固定値で(CMD0)応答電文データ情報を設定する				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/07 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void ermc_GetCmd0ResDataInfo(ProcAction_t* nextAction)
{
	DEBUG_TRCIN();

	/* INVマイコン初期化通信完了フラグ判定 */
	if (ERMC_INV_INIT_COMM_FIN_FLAG_ON == s_ermcInvInitCommFinFlag) {

		/* 応答電文データ情報(応答電文作成用)設定 */
		memcpy(&s_ermcResMsgDataInfo.cmd0DataInfo,
				&s_ermcCmd0InitData,
				sizeof(s_ermcResMsgDataInfo.cmd0DataInfo));

		/* 応答データ情報設定 */
		s_ermcCmdTable[s_ermcRecvMsgParseInfo.cmdTableIndex].resDataInfo =
			(uchar*)&s_ermcResMsgDataInfo.cmd0DataInfo;

		DLOG_Info("CMD0 response msg data info get finished.");

		/* 次のプロセスアクション設定：送信 */
		*nextAction = ERMC_PROC_ACTION_SEND;
	}
	else {
		/* 次のプロセスアクション設定：待ち */
		*nextAction = ERMC_PROC_ACTION_WAIT;
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:(CMD24)応答電文データ情報取得処理を行う				*/
/*--------------------------------------------------------------------------*/
/*	param			:nextAction 次のプロセスアクション						*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:公開関数「(CMD24)電文データ情報取得処理」を呼び出す	*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/07 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void ermc_GetCmd24ResDataInfo(ProcAction_t* nextAction)
{
	DEBUG_TRCIN();

	/* (CMD24)電文データ情報取得 */
	EMDTR_GetCmd24DataInfo();

	/* 次のプロセスアクション設定：待ち */
	*nextAction = ERMC_PROC_ACTION_WAIT;

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:(CMD41)受信電文データ情報設定処理を行う				*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:受信電文解析情報のData部により							*/
/*					 (CMD41)受信電文データ情報を設定する					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void ermc_SetCmd41ReqDataInfo(void)
{
	Cmd41RecvMsgDataInfo_t	dataInfo;				/* (CMD41)受信電文データ情報設定用 */
	slong tmpVal = 0;								/* 計算用一時変数 */

	DEBUG_TRCIN();

	/* ローカル変数初期化 */
	memset(&dataInfo, 0u, sizeof(dataInfo));		/* (CMD41)受信電文データ情報設定用 */

	/* (CMD41)受信電文データ情報設定 */
	/* 現在時刻 */
	/* 年 */
	dataInfo.currentTime.year =
		(ushort)ermc_ConvStrToIntStrtol(&s_ermcRecvMsgParseInfo.dataPart
											[ERMC_CDM41_DATA_GET_INDEX_0],
										ERMC_MSG_CMN_SIZE_2,
										ERMC_CONVERT_BASE_DEC);
	/* 月 */
	dataInfo.currentTime.month =
		(ushort)ermc_ConvStrToIntStrtol(&s_ermcRecvMsgParseInfo.dataPart
											[ERMC_CDM41_DATA_GET_INDEX_2],
										ERMC_MSG_CMN_SIZE_2,
										ERMC_CONVERT_BASE_DEC);
	/* 日 */
	dataInfo.currentTime.day =
		(ushort)ermc_ConvStrToIntStrtol(&s_ermcRecvMsgParseInfo.dataPart
											[ERMC_CDM41_DATA_GET_INDEX_4],
										ERMC_MSG_CMN_SIZE_2,
										ERMC_CONVERT_BASE_DEC);
	/* 時 */
	dataInfo.currentTime.hour =
		(ushort)ermc_ConvStrToIntStrtol(&s_ermcRecvMsgParseInfo.dataPart
											[ERMC_CDM41_DATA_GET_INDEX_6],
										ERMC_MSG_CMN_SIZE_2,
										ERMC_CONVERT_BASE_DEC);
	/* 分 */
	dataInfo.currentTime.minute =
		(ushort)ermc_ConvStrToIntStrtol(&s_ermcRecvMsgParseInfo.dataPart
											[ERMC_CDM41_DATA_GET_INDEX_8],
										ERMC_MSG_CMN_SIZE_2,
										ERMC_CONVERT_BASE_DEC);
	/* 秒 */
	dataInfo.currentTime.second =
		(ushort)ermc_ConvStrToIntStrtol(&s_ermcRecvMsgParseInfo.dataPart
											[ERMC_CDM41_DATA_GET_INDEX_10],
										ERMC_MSG_CMN_SIZE_2,
										ERMC_CONVERT_BASE_DEC);

	/* 時刻同期要否フラグ */
	if (ERMC_MSG_CURRENT_TIME_CH_1 ==
			s_ermcRecvMsgParseInfo.dataPart[ERMC_CDM41_DATA_GET_INDEX_12]) {

		/* 時刻同期要否フラグ設定：ON */
		dataInfo.timeSyncFlag = ERMC_TIME_SYNC_FLAG_ON;
	}
	else {
		/* 時刻同期要否フラグ設定：OFF */
		dataInfo.timeSyncFlag = ERMC_TIME_SYNC_FLAG_OFF;
	}

	/* RMCエラー */
	memcpy(dataInfo.rmcError,
			&s_ermcRecvMsgParseInfo.dataPart[ERMC_CDM41_DATA_GET_INDEX_13],
			sizeof(dataInfo.rmcError));

	/* 充電電力設定値（蓄電池）[W] [未使用] */
	tmpVal =
		ermc_ConvStrToIntStrtol(&s_ermcRecvMsgParseInfo.dataPart
									[ERMC_CDM41_DATA_GET_INDEX_18],
								ERMC_MSG_CMN_SIZE_4,
								ERMC_CONVERT_BASE_DEC);
	dataInfo.restrictedChargePwr = (slong)(tmpVal * 10);	/* [0.01KW]→[W] */

	/* 発電電力制限値[W] [未使用] */
	tmpVal =
		ermc_ConvStrToIntStrtol(&s_ermcRecvMsgParseInfo.dataPart
									[ERMC_CDM41_DATA_GET_INDEX_22],
								ERMC_MSG_CMN_SIZE_4,
								ERMC_CONVERT_BASE_DEC);
	dataInfo.restrictedPvGenPwr = (slong)(tmpVal * 10);		/* [0.01KW]→[W] */

	/* 放電電力設定値[W]（蓄電池） [未使用] */
	tmpVal =
		ermc_ConvStrToIntStrtol(&s_ermcRecvMsgParseInfo.dataPart
									[ERMC_CDM41_DATA_GET_INDEX_26],
								ERMC_MSG_CMN_SIZE_4,
								ERMC_CONVERT_BASE_DEC);
	dataInfo.restrictedDischargePwr = (slong)(tmpVal * 10);	/* [0.01KW]→[W] */

	/* 車載電池充電電力設定値[W] [未使用] */
	dataInfo.v2hPowerSetCharge =
		(slong)ermc_ConvStrToIntStrtoul(&s_ermcRecvMsgParseInfo.dataPart
											[ERMC_CDM41_DATA_GET_INDEX_30],
										ERMC_MSG_CMN_SIZE_8,
										ERMC_CONVERT_BASE_HEX);

	/* 車載電池放電電力設定値[W] [未使用] */
	dataInfo.v2hPowerSetDiscCharge =
		(slong)ermc_ConvStrToIntStrtoul(&s_ermcRecvMsgParseInfo.dataPart
											[ERMC_CDM41_DATA_GET_INDEX_38],
										ERMC_MSG_CMN_SIZE_8,
										ERMC_CONVERT_BASE_HEX);

	/* 車載電池充電電流設定値[0.1A] [未使用] */
	dataInfo.v2hCurrentSetCharge =
		(ushort)ermc_ConvStrToIntStrtoul(&s_ermcRecvMsgParseInfo.dataPart
											[ERMC_CDM41_DATA_GET_INDEX_46],
										ERMC_MSG_CMN_SIZE_4,
										ERMC_CONVERT_BASE_HEX);

	/* 車載電池放電電流設定値[0.1A] [未使用] */
	dataInfo.v2hCurrentSetDischarge =
		(ushort)ermc_ConvStrToIntStrtoul(&s_ermcRecvMsgParseInfo.dataPart
											[ERMC_CDM41_DATA_GET_INDEX_50],
										ERMC_MSG_CMN_SIZE_4,
										ERMC_CONVERT_BASE_HEX);

	/* (CMD41)受信電文データ情報設定 */
	memcpy(&s_ermcRecvMsgDataInfo.cmd41DataInfo,
			&dataInfo,
			sizeof(s_ermcRecvMsgDataInfo.cmd41DataInfo));

	DLOG_Info("CMD41 request msg data info set finished.");

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:(CMD41)応答電文データ情報取得処理を行う				*/
/*--------------------------------------------------------------------------*/
/*	param			:nextAction 次のプロセスアクション						*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:公開関数「時刻設定」を呼び出し、						*/
/*					 (CMD41)受信電文データ情報を他モジュールに通知する		*/
/*					 公開関数「(CMD41)電文データ情報取得処理」を呼び出し、	*/
/*					 取得した情報で(CMD41)応答電文データ情報を設定する		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/07 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void ermc_GetCmd41ResDataInfo(ProcAction_t* nextAction)
{
	EtimTm_t timeInfo;								/* 時刻情報 */
	EmdtrCmd41DataInfo_t data;						/* (CMD41)データ情報 */
	schar tmpBuf[ERMC_MSG_CMN_SIZE_10];				/* 一時バッファ */
	Cmd41ResMsgDataInfo_t dataInfo;					/* (CMD41)応答電文データ情報設定用 */
	ulong tmpVal = 0u;								/* 計算用一時変数 */

	DEBUG_TRCIN();

	/* ローカル変数初期化 */
	memset(&timeInfo, 0u, sizeof(timeInfo));		/* 時刻情報 */
	memset(&data, 0u, sizeof(data));				/* (CMD41)データ情報 */
	memset(tmpBuf, 0u, sizeof(tmpBuf));				/* 一時バッファ */

	/* 時刻同期要否フラグ判定 */
	if (ERMC_TIME_SYNC_FLAG_ON ==
			s_ermcRecvMsgDataInfo.cmd41DataInfo.timeSyncFlag) {
		/* 時刻同期が必要する場合 */
		/* 年(西暦の4桁) */
		timeInfo.tmYear	=
			s_ermcRecvMsgDataInfo.cmd41DataInfo.currentTime.year + 2000;
		/* 月 */
		timeInfo.tmMon	=
			s_ermcRecvMsgDataInfo.cmd41DataInfo.currentTime.month;
		/* 日 */
		timeInfo.tmMday	=
			s_ermcRecvMsgDataInfo.cmd41DataInfo.currentTime.day;
		/* 時 */
		timeInfo.tmHour	=
			s_ermcRecvMsgDataInfo.cmd41DataInfo.currentTime.hour;
		/* 分 */
		timeInfo.tmMin	=
			s_ermcRecvMsgDataInfo.cmd41DataInfo.currentTime.minute;
		/* 秒 */
		timeInfo.tmSec	=
			s_ermcRecvMsgDataInfo.cmd41DataInfo.currentTime.second;

		/* 時刻設定 */
		EMDTR_SetCurrentTime(&timeInfo);
	}
	else {
		/* 処理なし */
	}

	/* (CMD41)電文データ情報取得 */
	EMDTR_GetCmd41DataInfo(&data);

	/* (CMD41)応答電文データ情報設定 */
	/* 初期値設定 */
	memcpy(&dataInfo, &s_ermcCmd41InitData, sizeof(dataInfo));

	/* AC充放電電力（インバータ部） */
	snprintf(tmpBuf, sizeof(tmpBuf), "%04lX", 
				data.opesttsInfo.chargeDischargePwr);
	memcpy(dataInfo.chargeDischargePwr, tmpBuf, ERMC_MSG_CMN_SIZE_4);

	/* インバータ動作状態 */
	dataInfo.inverterOpeState = data.opesttsInfo.inverterOpeState;

	/* rSOC */
	snprintf(tmpBuf, sizeof(tmpBuf), "%04d", data.opesttsInfo.rsoc * 10);
	memcpy(dataInfo.rsoc, tmpBuf, ERMC_MSG_CMN_SIZE_4);

	/* SOC */
	snprintf(tmpBuf, sizeof(tmpBuf), "%04d", data.opesttsInfo.soc * 10);
	memcpy(dataInfo.soc, tmpBuf, ERMC_MSG_CMN_SIZE_4);

	/* 蓄電池運転動作状態 */
	dataInfo.operationState = data.opesttsInfo.operation;

	/* 停電 */
	dataInfo.pwrCut = data.opesttsInfo.pwrCut;

	/* PV発電電力（本製品以外のPV） */
	snprintf(tmpBuf, sizeof(tmpBuf), "%04X", (ushort)data.opesttsInfo.pvGenPwr);
	memcpy(dataInfo.pvGenPwr, tmpBuf, ERMC_MSG_CMN_SIZE_4);

	/* 逆潮流電力 */
	snprintf(tmpBuf, sizeof(tmpBuf), "%04X", (ushort)data.opesttsInfo.revPwrFlow);
	memcpy(dataInfo.revPwrFlow, tmpBuf, ERMC_MSG_CMN_SIZE_4);

	/* 運転モード */
	dataInfo.opeMode = ermc_ConvBoardOpeMode(data.opesttsInfo.opeMode);

	/* DC蓄電池充放電電力２（コンバータ部） */
	snprintf(tmpBuf, sizeof(tmpBuf), "%04lX", 
				data.opesttsInfo.chargeDischargePwr2);
	memcpy(dataInfo.chargeDischargePwr2, tmpBuf, ERMC_MSG_CMN_SIZE_4);

	/* 太陽光発電電力（内蔵PV） */
	snprintf(tmpBuf, sizeof(tmpBuf), "%04X", data.opesttsInfo.pvGenPwr2);
	memcpy(dataInfo.pvGenPwr2, tmpBuf, ERMC_MSG_CMN_SIZE_4);

	/* 家庭負荷電力 */
	tmpVal = (ulong)(data.opesttsInfo.homeLoadPower / 10);	/* [W]→[0.01KW] */
	snprintf(tmpBuf, sizeof(tmpBuf), "%04ld", tmpVal);
	memcpy(dataInfo.homeLoadPower, tmpBuf, ERMC_MSG_CMN_SIZE_4);

	/* 一時バッファクリア */
	memset(tmpBuf, 0u, sizeof(tmpBuf));							

	/* 通信ボード現在時刻 */
	/* 年(西暦の下2桁) */
	tmpVal = (ulong)(data.commBoardCurrentTime.tmYear % 100);
	snprintf(tmpBuf, sizeof(tmpBuf), "%02ld", tmpVal);
	memcpy(dataInfo.commBoardCurrentTime, tmpBuf, ERMC_MSG_CMN_SIZE_2);
	/* 月 */
	snprintf(tmpBuf, sizeof(tmpBuf), "%02d", data.commBoardCurrentTime.tmMon);
	memcpy(&dataInfo.commBoardCurrentTime[ERMC_MSG_CMN_SIZE_2],
			tmpBuf, ERMC_MSG_CMN_SIZE_2);
	/* 日 */
	snprintf(tmpBuf, sizeof(tmpBuf), "%02d", data.commBoardCurrentTime.tmMday);
	memcpy(&dataInfo.commBoardCurrentTime[ERMC_MSG_CMN_SIZE_4],
			tmpBuf, ERMC_MSG_CMN_SIZE_2);
	/* 時 */
	snprintf(tmpBuf, sizeof(tmpBuf), "%02d", data.commBoardCurrentTime.tmHour);
	memcpy(&dataInfo.commBoardCurrentTime[ERMC_MSG_CMN_SIZE_6],
			tmpBuf, ERMC_MSG_CMN_SIZE_2);
	/* 分 */
	snprintf(tmpBuf, sizeof(tmpBuf), "%02d", data.commBoardCurrentTime.tmMin);
	memcpy(&dataInfo.commBoardCurrentTime[ERMC_MSG_CMN_SIZE_8],
			tmpBuf, ERMC_MSG_CMN_SIZE_2);
	/* 秒 */
	snprintf(tmpBuf, sizeof(tmpBuf), "%02d", data.commBoardCurrentTime.tmSec);
	memcpy(&dataInfo.commBoardCurrentTime[ERMC_MSG_CMN_SIZE_10],
			tmpBuf, ERMC_MSG_CMN_SIZE_2);

	/* 応答電文データ情報(応答電文作成用)設定 */
	memcpy(&s_ermcResMsgDataInfo.cmd41DataInfo,
			&dataInfo, sizeof(s_ermcResMsgDataInfo.cmd41DataInfo));

	/* 応答データ情報設定 */
	s_ermcCmdTable[s_ermcRecvMsgParseInfo.cmdTableIndex].resDataInfo =
		(uchar*)&s_ermcResMsgDataInfo.cmd41DataInfo;

	DLOG_Info("CMD41 response msg data info get finished.");

	/* 次のプロセスアクション設定：送信 */
	*nextAction = ERMC_PROC_ACTION_SEND;

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:電文受信スレッド処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:下記の処理を行う										*/
/*					 電文受信、受信電文フォーマットチェック、				*/
/*					 電文解析、OPリモコン通信に関する項目の設定処理、		*/
/*					 要求データ情報設定、応答データ情報取得、				*/
/*					 プロセス状態変更要求送信、sem_wait()呼び出し			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/25 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void* ermc_RecvMsgThread(void* param)
{
	ProcAction_t nextAction = ERMC_PROC_ACTION_IDLE;	/* 次のプロセスアクション */
	sshort fncRet1 = ERMC_RET_INITIAL_VALUE;			/* 呼び出し関数戻り値 */
	sshort fncRet2 = ERMC_RET_INITIAL_VALUE;			/* 呼び出し関数戻り値 */
	sshort fncRet3 = ERMC_RET_INITIAL_VALUE;			/* 呼び出し関数戻り値 */
	ssize_t recvSize = 0;								/* 実際の受信電文サイズ */

	DEBUG_TRCIN();

	UNUSED(param);

	DLOG_Info("msg recive thread start.");

	/* 外部からの プロセス状態変更 まで繰り返し */
	do {
		/* グローバル変数初期化 */
		memset(s_ermcRecvBuf, 0u, sizeof(s_ermcRecvBuf));						/* 受信電文バッファ */
		memset(&s_ermcRecvMsgParseInfo, 0u, sizeof(s_ermcRecvMsgParseInfo));	/* 受信電文解析情報 */
		memset(&s_ermcRecvMsgDataInfo, 0u, sizeof(s_ermcRecvMsgDataInfo));		/* 受信電文データ情報 */
		memset(&s_ermcResMsgDataInfo, 0u, sizeof(s_ermcResMsgDataInfo));		/* 応答電文データ情報(応答電文作成用) */
		memset(s_ermcResBuf, 0u, sizeof(s_ermcResBuf));							/* 応答電文バッファ */
		s_ermcReadElapsedMs = 0u;												/* 電文読み込み経過時間[ms] */

		/* ローカル変数初期化 */
		recvSize = 0;				/* 実際の受信電文サイズ */

		/* 電文受信 */
		fncRet1 = ermc_RecvMsg(&recvSize);

		if ((ERMC_RET_OK == fncRet1) && (0 < recvSize)) {
			DLOG_Info("receive request msg success.");

			/* 受信電文フォーマットチェック */
			fncRet2 = ermc_CheckMsgFormat();
			if (ERMC_RET_OK == fncRet2) {
				DLOG_Info("recived msg format check success.");

				/* 受信電文解析 */
				fncRet3 = ermc_ParseMsg();
				if (ERMC_RET_OK == fncRet3) {
					DLOG_Info("recived msg parse success.");

					/* OPリモコン通信に関する項目の設定処理 */
					ermc_SetRmcCommRelatedItems(
						s_ermcCmdTable
							[s_ermcRecvMsgParseInfo.cmdTableIndex].recvEvent);

					if (NULL != s_ermcCmdTable
									[s_ermcRecvMsgParseInfo.cmdTableIndex].
										setReqDataInfo) {

						/* 要求データ情報設定 */
						s_ermcCmdTable[s_ermcRecvMsgParseInfo.cmdTableIndex].
							setReqDataInfo();
					}
					else {
						/* 処理なし */
					}

					if (ERMC_PROC_STTS_IDLE != g_ermcProcStatus) {

						/* 応答データ情報取得 */
						s_ermcCmdTable[s_ermcRecvMsgParseInfo.cmdTableIndex].
							getResDataInfo(&nextAction);

						/* プロセス状態変更要求送信 */
						ermc_SendProcStatusChangeReq(nextAction);

						/* sem_wait()を呼び出し、sem_post()が呼び出されるまで
							電文受信スレッドをスリープ */
						ermc_Callsemwait();
					}
					else {
						/* 処理なし */
					}
				}
				else {
					/* 処理なし */
				}
			}
			else {
				/* 処理なし */
			}
		}
		else {
			/* 処理なし */
		}
	} while (ERMC_PROC_STTS_RECEIVING == g_ermcProcStatus);

	DLOG_Info("msg recive thread stop.");

	DEBUG_TRCOUT();

	return NULL;
}

/*==========================================================================*/
/*	Description		:電文受信処理を行う										*/
/*--------------------------------------------------------------------------*/
/*	param			:recvSize	実際の受信電文サイズ						*/
/*--------------------------------------------------------------------------*/
/*	return			:ERMC_RET_OK 正常終了									*/
/*					 ERMC_RET_NG 異常終了									*/
/*--------------------------------------------------------------------------*/
/*	detail			:下記の処理を行う										*/
/*					 読み取り状態制御、受信確認、電文読み込み				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/26 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static sshort ermc_RecvMsg(ssize_t* recvSize)
{
	sshort retCode = ERMC_RET_INITIAL_VALUE;		/* リターンコード */
	sshort fncRet1 = ERMC_RET_INITIAL_VALUE;		/* 呼び出し関数戻り値 */
	sshort fncRet2 = ERMC_RET_INITIAL_VALUE;		/* 呼び出し関数戻り値 */
	struct pollfd fds[1u];							/* pollfd構造体設定用 */
	sint pollResult = 0;							/* poll関数戻り値 */

	DEBUG_TRCIN();

	/* 読み取り状態制御処理 */
	fncRet1 = ermc_EnableRead();
	if (ERMC_RET_OK == fncRet1) {

		/* pollfd構造体設定 */
		fds[0u].fd = s_ermcUartFd;
		fds[0u].events = POLLIN;	/* 読み出し可能データあり */
		fds[0u].revents = 0u;

		/* 受信確認 */
		pollResult = poll(fds, 1u, ERMC_COMM_CYC_DECISION_TIME);
		if (0 < pollResult) {

			/* 電文読み込み処理 */
			fncRet2 = ermc_ReadMsg(
						s_ermcRecvBuf,
						sizeof(s_ermcRecvBuf),
						recvSize);

			if (ERMC_RET_OK == fncRet2) {
				retCode = ERMC_RET_OK;
			}
			else {
				retCode = ERMC_RET_NG;
			}
		}
		else {
			retCode = ERMC_RET_NG;
		}
	}
	else {
		retCode = ERMC_RET_NG;
	}

	DEBUG_TRCOUT();

	return retCode;
}

/*==========================================================================*/
/*	Description		:電文読み込み処理を行う									*/
/*--------------------------------------------------------------------------*/
/*	param			:msgData	受信電文データ格納アドレス					*/
/*					 msgSize	受信電文データ格納最大サイズ				*/
/*					 totalSize	実際の読み込みサイズ						*/
/*--------------------------------------------------------------------------*/
/*	return			:ERMC_RET_OK 正常終了									*/
/*					 ERMC_RET_NG 異常終了									*/
/*--------------------------------------------------------------------------*/
/*	detail			:電文はエンドコード(CR)が読み込まれるまで繰り返す。		*/
/*					 250ms以内に受信が終わらなかった場合、異常終了を返す。	*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/26 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static sshort ermc_ReadMsg(uchar* msgData,
							size_t msgSize,
							ssize_t* totalSize)
{
	sshort retCode = ERMC_RET_INITIAL_VALUE;		/* リターンコード */
	ushort stopFlag = ERMC_LOOP_STOP_FLAG_OFF;		/* ループ処理停止用フラグ */
	ssize_t readSize = 0;							/* ループ毎の受信電文サイズ */
	struct pollfd fds[1u];							/* pollfd構造体設定用 */
	sint pollResult = 0;							/* poll関数戻り値 */
	char* ecPoint = NULL;							/* strrchr関数戻り値 */
	double startTime = 0;							/* 読み込み開始時間 */

	DEBUG_TRCIN();

	/* pollfd構造体設定 */
	fds[0u].fd = s_ermcUartFd;
	fds[0u].events = POLLIN;	/* 読み出し可能データあり */
	fds[0u].revents = 0u;

	/* 読み込み開始時間取得 */
	startTime = ermc_GetTimeSec();

	/* バッファ枯渇/電文終端検出/タイムアウトまで繰り返し */
	do {
		/* 受信確認 */
		pollResult = poll(fds, 1u, ERMC_COMM_RECV_TIMEOUT);
		if (0 < pollResult) {

			/* 1byteずつ情報読み込み */
			readSize = read(s_ermcUartFd, msgData, 1u);
			if (0 <= readSize) {

				/* 読み込んだ分だけ進む */
				*totalSize += readSize;
				msgData = msgData + readSize;
				msgSize -= (size_t)readSize;

				if (0u < msgSize) {
					/* エンドコードを検索 */
					ecPoint =
						strrchr((char*)(msgData - *totalSize),
							ERMC_MSG_CODE_CR);
					if (ecPoint != NULL) {
						/* エンドコードがあった場合、読み込み処理を終了 */
						*totalSize =
							ecPoint - (char*)(msgData - *totalSize) + 1;
						stopFlag = ERMC_LOOP_STOP_FLAG_ON;
						retCode = ERMC_RET_OK;
					}
					else {
						/* 何もせずに次のループ処理へ */
					}
				}
				else {
					/* バッファ枯渇 */
					DLOG_Error("receice msg size >= buf size");
					stopFlag = ERMC_LOOP_STOP_FLAG_ON;
					retCode = ERMC_RET_NG;
				}
			}
			else {
				DLOG_Error("read() error.");
				stopFlag = ERMC_LOOP_STOP_FLAG_ON;
				retCode = ERMC_RET_NG;
			}
		}
		else {
			stopFlag = ERMC_LOOP_STOP_FLAG_ON;
			retCode = ERMC_RET_NG;
		}

		/* 読み込み経過時間[ms]取得 */
		s_ermcReadElapsedMs = DIFF_TIME(ermc_GetTimeSec(), startTime);

		/* 読み込み経過時間[ms]判定 */
		if (ERMC_COMM_RECV_TIMEOUT < s_ermcReadElapsedMs) {
			DLOG_Error("receive msg timeout.");
			stopFlag = ERMC_LOOP_STOP_FLAG_ON;
			retCode = ERMC_RET_NG;
		}
		else {
			/* 処理なし */
		}
	} while (ERMC_LOOP_STOP_FLAG_OFF == stopFlag);

	DEBUG_TRCOUT();

	return retCode;
}

/*==========================================================================*/
/*	Description		:電文フォーマットチェック処理を行う						*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:ERMC_RET_OK 正常終了									*/
/*					 ERMC_RET_NG 異常終了									*/
/*--------------------------------------------------------------------------*/
/*	detail			:<STX>～<ETX>～<CR>という電文構文をチェックする			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/31 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static sshort ermc_CheckMsgFormat(void)
{
	schar* stxPoint = NULL;		/* STXコードのポインタ */
	schar* etxPoint = NULL;		/* ETXコードのポインタ */
	schar* crPoint = NULL;		/* CRコードのポインタ */

	DEBUG_TRCIN();

	/* 特定コードのポインタを検索 */
	stxPoint = strchr((char*)s_ermcRecvBuf, ERMC_MSG_CODE_STX);
	etxPoint = strchr((char*)s_ermcRecvBuf, ERMC_MSG_CODE_ETX);
	crPoint = strchr((char*)s_ermcRecvBuf, ERMC_MSG_CODE_CR);

	/* 設定されていない特定コードがあった場合 */
	if ((NULL == stxPoint) || (NULL == etxPoint) || (NULL == crPoint)) {
		DLOG_Error("receive msg format error.");
		return ERMC_RET_NG;
	}
	else {
		/* 処理なし */
	}

	/* <STX>～<ETX>間のアドレスチェック */
	if (stxPoint >= etxPoint) {
		/* アドレス上、"<EXT>*****<STX>の並びの為、エラーとする */
		DLOG_Error("receive msg format error. STX > ETX.");
		return ERMC_RET_NG;
	}
	else {
		/* 処理なし */
	}

	/* <ETX>～<CR>間のアドレスチェック */
	if ((etxPoint + ERMC_MSG_CHECK_SUM_SIZE + 1) != crPoint) {
		/* アドレス上、"<CR>*****<ETX>の並びの為、エラーとする */
		DLOG_Error("receive msg format error. ETX > CR.");
		return ERMC_RET_NG;
	}
	else {
		/* 処理なし */
	}

	/* <STX>と<ETX>の間に、最低限必要なbyte数のチェック */
	if ((ulong)(etxPoint - stxPoint - 1) < ERMC_MSG_BODY_SIZE_MIN) {
		/* アドレス上、<STX><ETX>間に必要なサイズの文字列が無い為、エラーとする */
		DLOG_Error("receive msg format error. Body size 0.");
		return ERMC_RET_NG;
	}
	else {
		/* 処理なし */
	}

	DEBUG_TRCOUT();

	return ERMC_RET_OK;
}

/*==========================================================================*/
/*	Description		:電文解析処理を行う										*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:受信電文のBody部を解析してグローバル変数に保存する		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/03 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static sshort ermc_ParseMsg(void)
{
	sshort retCode = ERMC_RET_INITIAL_VALUE;		/* リターンコード */
	schar* tmpPoint = NULL;							/* 一時ポインタ */
	schar* etxPoint = NULL;							/* ETXコードのポインタ */
	ushort loopCnt = 0u;							/* ループカウンタ */
	ushort stopFlag = ERMC_LOOP_STOP_FLAG_OFF;		/* ループ処理停止用フラグ */

	DEBUG_TRCIN();

	/* Header部 */
	tmpPoint = strchr((char*)s_ermcRecvBuf, ERMC_MSG_CODE_STX) + 1;
	memcpy(s_ermcRecvMsgParseInfo.headerPart,
			tmpPoint,
			ERMC_MSG_HEADER_SIZE);

	/* Address部 */
	tmpPoint += ERMC_MSG_HEADER_SIZE;
	memcpy(s_ermcRecvMsgParseInfo.addressPart,
			tmpPoint,
			ERMC_MSG_ADDRESS_SIZE);

	/* Seq_NB部 */
	tmpPoint += ERMC_MSG_ADDRESS_SIZE;
	memcpy(s_ermcRecvMsgParseInfo.sequencePart,
			tmpPoint,
			ERMC_MSG_SEQUENCE_SIZE);

	/* Command部 */
	tmpPoint += ERMC_MSG_SEQUENCE_SIZE;
	memcpy(s_ermcRecvMsgParseInfo.commandPart,
			tmpPoint,
			ERMC_MSG_COMMAND_SIZE);

	/* ETXコードのポインタ取得 */
	etxPoint = strchr((char*)s_ermcRecvBuf, ERMC_MSG_CODE_ETX);

	tmpPoint += ERMC_MSG_COMMAND_SIZE;
	if (etxPoint == tmpPoint) {
		/* Data部なしの場合 */
		/* 処理なし */
	}
	else {
		/* Data部 */
		memcpy(s_ermcRecvMsgParseInfo.dataPart,
				tmpPoint,
				(size_t)(etxPoint - tmpPoint));

		/* Data部サイズ */
		s_ermcRecvMsgParseInfo.dataPartSize = (ushort)(etxPoint - tmpPoint);
	}

	/* コマンドテーブルインデックス */
	do {
		if (ERMC_MSG_CMD_CNT_MAX > loopCnt) {
			if (0 == strncmp(
						s_ermcCmdTable[loopCnt].reqCmdCode,
						s_ermcRecvMsgParseInfo.commandPart,
						ERMC_MSG_COMMAND_SIZE)) {

				/* 受信電文Data部サイズ判定 */
				if (s_ermcCmdTable[loopCnt].reqDataPartSize ==
						s_ermcRecvMsgParseInfo.dataPartSize) {

					DLOG_Info("recived msg command：%s.",
									s_ermcCmdTable[loopCnt].reqCmdCode);

					/* コマンドテーブルインデックス保存 */
					s_ermcRecvMsgParseInfo.cmdTableIndex = loopCnt;

					retCode = ERMC_RET_OK;
				}
				else {
					DLOG_Error("recived msg data part size error.(size=%d)",
									s_ermcRecvMsgParseInfo.dataPartSize);
					retCode = ERMC_RET_NG;
				}

				stopFlag = ERMC_LOOP_STOP_FLAG_ON;
			}
			else {
				/* インクリメント */
				loopCnt++;
			}
		}
		else {
			DLOG_Error("unsupported command：%s.",
							s_ermcRecvMsgParseInfo.commandPart);
			stopFlag = ERMC_LOOP_STOP_FLAG_ON;
			retCode = ERMC_RET_NG;
		}
	} while (ERMC_LOOP_STOP_FLAG_OFF == stopFlag);

	DEBUG_TRCOUT();

	return retCode;
}

/*==========================================================================*/
/*	Description		:電文送信処理を行う										*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:下記の処理を行う										*/
/*					 応答電文作成、電文書き込み、							*/
/*					 OPリモコン通信に関する項目の設定処理、					*/
/*					 プロセス状態変更要求送信								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/20 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void ermc_SendMsg(void)
{
	sshort fncRet = ERMC_RET_INITIAL_VALUE;		/* 呼び出し関数戻り値 */
	size_t resSize = 0u;						/* 作成した応答電文サイズ */
	ssize_t sendSize = 0;						/* 実際の送信電文サイズ */

	DEBUG_TRCIN();

	/* 応答電文作成 */
	ermc_CreateResMsg(
		s_ermcCmdTable[s_ermcRecvMsgParseInfo.cmdTableIndex].resDataInfo,
		s_ermcCmdTable[s_ermcRecvMsgParseInfo.cmdTableIndex].resDataPartSize,
		&resSize);

	DLOG_Info("response msg created.");

	/* 電文書き込み */
	fncRet = ermc_WriteMsg(s_ermcResBuf, resSize, &sendSize);
	if ((ERMC_RET_OK == fncRet) && (0 < sendSize)) {
		DLOG_Info("send response msg success.");
	}
	else {
		/* 処理なし */
	}

	/* OPリモコン通信に関する項目の設定処理 */
	ermc_SetRmcCommRelatedItems(ERMC_RES_MSG_SEND_FIN_EVENT);

	/* プロセス状態変更要求送信 */
	ermc_SendProcStatusChangeReq(ERMC_PROC_ACTION_RECV);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:応答電文作成処理を行う									*/
/*--------------------------------------------------------------------------*/
/*	param			:msgData		応答電文Data部							*/
/*					 msgDataSize	応答電文Data部サイズ					*/
/*					 resSize		作成した応答電文のサイズ				*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:受信電文解析情報と入力した応答電文Data部と				*/
/*					 応答電文Data部サイズにより、応答電文を作成する			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/20 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void ermc_CreateResMsg(	uchar* msgData,
								ulong msgDataSize,
								size_t* resSize)
{
	schar resCmd[ERMC_MSG_COMMAND_SIZE];			/* 応答電文のCommand */
	ulong offSet = 0u;								/* ポインタオフセット */
	ulong loopCnt = 0u;								/* ループカウンタ */
	slong checkSum = 0;								/* 電文のチェックサム算出用 */
	schar resChkSum[ERMC_MSG_CHECK_SUM_SIZE + 1];	/* 応答電文のチェックサム(終端'\0'含む) */

	DEBUG_TRCIN();

	/* ローカル変数初期化 */
	memset(resCmd, 0u, sizeof(resCmd));				/* 応答電文のCommand初期化 */
	memset(resChkSum, 0u, sizeof(resChkSum));		/* 応答電文のチェックサム初期化 */

	/* STX */
	s_ermcResBuf[offSet++] = ERMC_MSG_CODE_STX;

	/* Header部 */
	memcpy(&s_ermcResBuf[offSet],
			ERMC_MSG_CODE_HEADER_SEND,
			ERMC_MSG_HEADER_SIZE);
	offSet += ERMC_MSG_HEADER_SIZE;

	/* Address部 */
	memcpy(&s_ermcResBuf[offSet],
			s_ermcRecvMsgParseInfo.addressPart,
			ERMC_MSG_ADDRESS_SIZE);
	offSet += ERMC_MSG_ADDRESS_SIZE;

	/* Seq_NB部 */
	memcpy(&s_ermcResBuf[offSet],
			s_ermcRecvMsgParseInfo.sequencePart,
			ERMC_MSG_SEQUENCE_SIZE);
	offSet += ERMC_MSG_SEQUENCE_SIZE;

	/* Command部 */
	memcpy(resCmd,
			s_ermcRecvMsgParseInfo.commandPart,
			ERMC_MSG_COMMAND_SIZE);
	resCmd[0u] =
		(schar)tolower((sint)(s_ermcRecvMsgParseInfo.commandPart[0u]));
	memcpy(&s_ermcResBuf[offSet], resCmd, ERMC_MSG_COMMAND_SIZE);
	offSet += ERMC_MSG_COMMAND_SIZE;

	/* Data部 */
	for (loopCnt = 0u; msgDataSize > loopCnt; loopCnt++) {
		s_ermcResBuf[offSet++] = *(msgData + loopCnt);
	}

	/* ETX */
	s_ermcResBuf[offSet++] = ERMC_MSG_CODE_ETX;

	/* チェックサム */
	for (loopCnt = 0u; offSet > loopCnt; loopCnt++) {
		checkSum = (slong)(checkSum + s_ermcResBuf[loopCnt]);
	}
	snprintf(resChkSum, sizeof(resChkSum), "%02lX", checkSum & 0xFF);
	memcpy(&s_ermcResBuf[offSet], resChkSum, ERMC_MSG_CHECK_SUM_SIZE);
	offSet += ERMC_MSG_CHECK_SUM_SIZE;

	/* CR */
	s_ermcResBuf[offSet++] = ERMC_MSG_CODE_CR;

	/* 応答電文サイズ設定 */
	*resSize = (size_t)offSet;

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:電文書き込み処理を行う									*/
/*--------------------------------------------------------------------------*/
/*	param			:msgData	受信電文データ格納アドレス					*/
/*					 msgSize	受信電文データ格納最大サイズ				*/
/*					 totalSize	実際の読み込みサイズ						*/
/*--------------------------------------------------------------------------*/
/*	return			:ERMC_RET_OK 正常終了									*/
/*					 ERMC_RET_NG 異常終了									*/
/*--------------------------------------------------------------------------*/
/*	detail			:下記の処理を行う										*/
/*					 書き込み状態制御、すべて書き込むまで繰り返し、			*/
/*					 電文送信完了待ち、送信していないデータをflush、		*/
/*					 読み取り状態制御										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static sshort ermc_WriteMsg(const void* msgData,
							size_t msgSize,
							ssize_t* totalSize)
{
	sshort retCode = ERMC_RET_OK;					/* リターンコード */
	sshort fncRet1 = ERMC_RET_INITIAL_VALUE;		/* 呼び出し関数戻り値 */
	sshort fncRet2 = ERMC_RET_INITIAL_VALUE;		/* 呼び出し関数戻り値 */
	ushort stopFlag = ERMC_LOOP_STOP_FLAG_OFF;		/* ループ処理停止用フラグ */
	ssize_t writeSize = 0;							/* ループ毎の受信電文サイズ */

	DEBUG_TRCIN();

	/* 書き込み状態制御処理 */
	fncRet1 = ermc_EnableWrite();
	if (ERMC_RET_OK == fncRet1) {
		

		/* すべて書き込むまで繰り返し */
		while ((0 < msgSize) && (ERMC_LOOP_STOP_FLAG_OFF == stopFlag)) {
			/* 情報書き込む */
			writeSize = write(s_ermcUartFd, msgData, msgSize);
			if (0 < writeSize) {
				/* 書き込んだ分だけ進む */
				*totalSize += writeSize;
				msgData = ((const char*)msgData) + writeSize;
				msgSize -= (size_t)writeSize;
			}
			else {
				DLOG_Error("write() error.");
				stopFlag = ERMC_LOOP_STOP_FLAG_ON;
				retCode = ERMC_RET_NG;
			}
		} /* while end */

		if (ERMC_RET_OK == retCode) {

			/* 送信完了待ちを行う */
			ermc_WaitSendEnd();

			/* 出力したが送信していないデータをflush */
			tcflush(s_ermcUartFd, TCOFLUSH);
		}
		else {
			/* 処理なし */
		}

		/* 読み取り状態制御処理 */
		fncRet2 = ermc_EnableRead();
		if (ERMC_RET_OK == fncRet2) {
			/* 処理なし */
		}
		else {
			retCode = ERMC_RET_NG;
		}
	}
	else {
		retCode = ERMC_RET_NG;
	}

	DEBUG_TRCOUT();

	return retCode;
}

/*==========================================================================*/
/*	Description		:電文送信完了待ち処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	param			:なし													*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:送信バッファにあるバイト数が0になるまで				*/
/*					 1msごとにioctl関数を呼び出す(最大250回)				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void ermc_WaitSendEnd(void)
{
	sint fncRet = 0;								/* ioctl関数戻り値 */
	slong sendBufBytes = 0;							/* 送信バッファバイト数 */
	slong sendBufBytesBef = 0;						/* 送信バッファバイト数(前回値保存用) */
	ulong retryCnt = 0u;							/* リトライ数 */
	ushort stopFlag = ERMC_LOOP_STOP_FLAG_OFF;		/* ループ処理停止用フラグ */

	DEBUG_TRCIN();

	do {
		/* 送信バッファにあるバイト数確認 */
		sendBufBytes = 0;
		fncRet = ioctl(s_ermcUartFd, TIOCOUTQ, &sendBufBytes);
		
		if (0 == fncRet) {
			if (0 != sendBufBytes) {
				if (sendBufBytes == sendBufBytesBef) {
					/* リトライ数インクリメント */
					retryCnt++;

					if (ERMC_COMM_SEND_TIMEOUT <= retryCnt) {
						/* 250msec間送信バッファから送信出来ていない */
						DLOG_Error("send msg timeout.");
						stopFlag = ERMC_LOOP_STOP_FLAG_ON;
					}
					else {
						/* 処理なし */
					}
				}
				else {
					/* リトライ数初期化 */
					retryCnt = 0u;
				}

				if (ERMC_LOOP_STOP_FLAG_OFF == stopFlag) {
					/* 前回値保存 */
					sendBufBytesBef = sendBufBytes;

					/* 1msecスリープ */
					ermc_Callnanosleep(ERMC_SLEEP_TIME_1);
				}
				else {
					/* 処理なし */
				}
			}
			else {
				/* 送信完了の場合 */
				stopFlag = ERMC_LOOP_STOP_FLAG_ON;
			}
		}
		else {
			DLOG_Error("ioctl() error.");
			stopFlag = ERMC_LOOP_STOP_FLAG_ON;
		}
	} while (ERMC_LOOP_STOP_FLAG_OFF == stopFlag);

	/* ioctl()でermc_EnableRead()によりread状態にするとOPリモコン側が受信出来ないようなので、
		tcdrain()に合わせ10msecスリープさせる */
	ermc_Callnanosleep(ERMC_SEND_END_SLEEP_TIME);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:プロセス状態変更要求送信処理を行う						*/
/*--------------------------------------------------------------------------*/
/*	param			:nextAction 次のプロセスアクション						*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:プロセス状態変更要求メッセージを送信する				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/28 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void ermc_SendProcStatusChangeReq(ProcAction_t nextAction)
{
	ProcStatusChangeReq_t sendMsg;

	DEBUG_TRCIN();

	/* 初期化 */
	memset(&sendMsg, 0u, sizeof(sendMsg));

	/* プロセス状態変更要求メッセージ送信 */
	sendMsg.header.msgId = ERMC_PROC_STATUS_CHANGE_REQ;
	sendMsg.nextAction = nextAction;
	FRM_MsgSendAsync(ERMC_SPORT, sizeof(sendMsg), &sendMsg);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:状態機処理を実行する									*/
/*--------------------------------------------------------------------------*/
/*	param			:nextAction 次のプロセスアクション						*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:現在のプロセス状態と入力した次のプロセスアクション		*/
/*					 により、プロセス状態変更、電文送信、					*/
/*					 sem_post()呼び出しを行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/06 : DHC KQ.Liang : 新規作成					*/
/*==========================================================================*/
static void ermc_RunStateMachine(ProcAction_t nextAction)
{
	ProcStatus_t oldState = ERMC_PROC_STTS_IDLE;	/* プロセス状態一時保存用 */

	DEBUG_TRCIN();

	/* 現在のプロセス状態を一時保存 */
	oldState = g_ermcProcStatus;

	/* プロセス状態設定 */
	g_ermcProcStatus = s_ermcActionTable[oldState][nextAction].procState;

	if (NULL != s_ermcActionTable[oldState][nextAction].sendAction) {

		/* 電文送信処理呼び出し */
		s_ermcActionTable[oldState][nextAction].sendAction();
	}
	else {
		/* 処理なし */
	}

	if (NULL != s_ermcActionTable[oldState][nextAction].semAction) {

		/* sem_post()を呼び出し、電文受信スレッドを起床させる */
		s_ermcActionTable[oldState][nextAction].semAction();
	}
	else {
		/* 処理なし */
	}
	
	DEBUG_TRCOUT();
}

/*End Of File*/
