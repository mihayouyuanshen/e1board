/****************************************************************************/
/*	Description		:ECHONET Lite通信処理									*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC DX.zhao											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/11												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、ENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:ECHONET Lite通信で行う処理								*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "eenl_g.h"
#include "dlog_g.h"
#include "frm_g.h"
#include "task_g.h"
#include "eenl_cmn_g.h"
#include "eenl.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
const uchar g_eenlCodeNcc[3] = { 0x00u, 0x00u, 0x6Cu };/*メーカコード ニチコン 0x00006C */
uchar g_eenlId[17] = { 0xFEu, 0x00u, 0x00u, 0x00u, 0x40u, 0x20u, 0x20u, 0x20u, 
0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u };
uchar g_eenlBattery0x81 = EENL_LOCATION_INSIDE;		/* 蓄電池 設置場所(0x81) */
uchar g_eenlSolar0x81 = EENL_LOCATION_INSIDE;		/* 太陽光 設置場所(0x81) */
uchar g_eenlV2h0x81 = EENL_LOCATION_GARAGE;			/* 車両充放電器 設置場所(0x81) */
uchar g_eenlMpcs0x81 = EENL_LOCATION_INSIDE;		/* マルチ入力PCS 設置場所(0x81) */
ulong g_eenlBattery0xE7 = 0xFFFFFFFFu;				/* 蓄電池 充電量設定値(0xE7) */
ulong g_eenlBattery0xE8 = 0xFFFFFFFFu;				/* 蓄電池 放電量設定値(0xE8) */
ulong g_eenlBattery0xAA = 0xFFFFFFFFu;				/* 蓄電池 AC充電量設定値(0xAA) */
ulong g_eenlBattery0xAB = 0xFFFFFFFFu;				/* 蓄電池 AC放電量設定値(0xAB) */

uchar g_eenlPushedUp = EENL_PUSHEDUP_OFF;			/* 押し上げ 0x42:なし(固定値) */

uchar g_eenlMacAddr[6] = { 0u,0u,0u,0u,0u,0u };		/* MACアドレス */

uchar g_eenlIfMode = 0u;							/* IFタイプ */
uchar g_eenlCreate = EENL_CREATE_NG;				/* enl構築 0:未構築 1:構築済 */

EenlTestRunState_t g_eenlTestRunState = EENL_TESTRUN_UNDEF;		/* 試運転の起動有無 */

uchar g_eenlOpeCtrl = EENL_OPECTRL_OFF;				/* ENL充放電制御 (初期値:OFF) */


uchar g_eenlV2hConnectorLock = EPCS_V2H_CONNECTOR_UNLOCK;	/*  状態変化通知された時の「V2Hコネクタロック状態」を保持 */
uchar g_eenlOperation = EPCS_OPE_STOP;				/*  状態変化通知された時の「CMD21:動作中」を保持 */
uchar g_eenlPwrCut = EPCS_PWROUTAGE_NO;				/*  停電状態変更通知された時の「停電状態」を保持 */

uchar g_eenlKeepOpeMode = 0xFFu;					/*  状態変化通知された時の運転モード保存用。 */

slong g_eenlReboot = ESYS_START_FACT_NORMAL;		/*  プロセス再起動判定用(ESYS_START_FACT_RESET=プロセス再起動) */

EenlHemsHistory_t g_eenlHemsHistory;				/* HEMS発電抑制履歴 */

void* g_eenlDevNode = NULL;							/*  デバイスインスタンスアドレス(ノード) */
void* g_eenlDevBattery = NULL;						/*  デバイスインスタンスアドレス(蓄電池) */
void* g_eenlDevSolar = NULL;						/*  デバイスインスタンスアドレス(太陽光) */
void* g_eenlDevV2h = NULL;							/*  デバイスインスタンスアドレス(車両充放電器) */
void* g_eenlDevMpcs = NULL;							/*  デバイスインスタンスアドレス(マルチ入力PCS) */
void* g_eenlDevMeterE1 = NULL;						/*	デバイスインスタンスアドレス(分散型電源電力量メータ) E1*/
void* g_eenlDevMeterC2 = NULL;						/*	デバイスインスタンスアドレス(分散型電源電力量メータ) C2*/

void* g_eenlDevList[EENL_DEV_LIST_MAX] = { 0 };		/*  デバイスインスタンスアドレス */

uchar g_eenlEojSolar[3] = { 0x02u, 0x79u, 0x01u };	/*  太陽光のEOJ */
uchar g_eenlEojBt[3] = { 0x02u, 0x7Du, 0x01u };		/*  蓄電池のEOJ */
uchar* g_eenlEojList[2] = { g_eenlEojSolar, g_eenlEojBt };	/*  EOJリスト */

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
pthread_t s_eenlThTimer;							/* 定期的に実施する処理のスレッド */
slong s_eenlGetWlanStatus = EENL_OFF;				/* 無線LANの場合、電波強度を取得する結果 */
slong s_eenlEvtidFinishTestRun = EENL_OFF;			/* 試運転完了イベント */
ulong s_eenlConfirmRetryCnt = 0u;					/* 起動条件確認の回数 */
ulong s_eenlThreadCheckCnt = 0u;					/* 実行処理の回数 */
uchar s_eenlStatusType = EENL_STATUS_IDLE;			/* 状態管理 */
slong s_eenlEnetChk1 = 0;
slong s_eenlEnetChk2 = 0;

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*==========================================================================*/
/*	Description		:EENL初期化処理											*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
void EENL_Init(void)
{
	DEBUG_TRCIN();

	/* 状態管理(初期状態) */
	s_eenlStatusType = EENL_STATUS_IDLE;

	/* 無線LANの場合、電波強度を取得する結果 */
	s_eenlGetWlanStatus = EENL_OFF;
	/* 試運転完了イベント */
	s_eenlEvtidFinishTestRun = EENL_OFF;

	/* 起動条件確認のリトライ回数 */
	s_eenlConfirmRetryCnt = 0u;
	/* 実行処理の回数 */
	s_eenlThreadCheckCnt = 0u;

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:キー入力制御メッセージプロセス処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:FrmMsgHead_t *msg										*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
void EENL_MsgProc(FrmMsgHead_t const* msg)
{
	DEBUG_TRCIN();

	/* 状態管理 */
	switch (s_eenlStatusType) {
	case EENL_STATUS_IDLE:
		/* 起動条件の確認処理前の初期化処理 */
		eenl_ConfirmInit();
		break;
	case EENL_STATUS_CONFIRM_START_1:
		/* 起動条件の確認開始ステップ１ */
		eenl_ConfirmStart();
		break;
	case EENL_STATUS_CONFIRM_START_2:
		/* 起動条件の確認開始ステップ２(メッセージ受信) */
		eenl_ConfirmStartMsgProc(msg);
		break;
	case EENL_STATUS_EXECUTE_START_1:
		/* 実行処理開始ステップ１ */
		eenl_ExecuteInit();
		break;
	case EENL_STATUS_EXECUTE_START_2:
		/* 実行処理開始ステップ２(メッセージ受信) */
		eenl_ExecuteStartMsgProc(msg);
		break;
	default:
		break;
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:EENL終了化処理											*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
void EENL_Fin(void)
{
	DEBUG_TRCIN();

	s_eenlStatusType = EENL_STATUS_IDLE;

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:EENL開始処理											*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　fact 起動フラグ									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:成功)												*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_Start(slong fact)
{
	EenlConfirmStart_t msg;

	DEBUG_TRCIN();

	/* プロセス再起動判定フラグを設定 */
	g_eenlReboot = fact;
	if (ESYS_START_FACT_RESET == g_eenlReboot) {
		DLOG_Debug("*** exec from ewdt ***");
	} else {
		DLOG_Debug("*** exec from esys ***");
	}

	/* 起動条件の確認開始のメッセージ送信 */
	msg.Header.msgId = EENL_CONFIRM_START;
	FRM_MsgSendAsync(EENL_SPORT, sizeof(msg), &msg);

	DEBUG_TRCOUT();
	return 0;
}

/*==========================================================================*/
/*	Description		:EENL停止処理											*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:成功/-1:失敗)										*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_Stop(void)
{
	DEBUG_TRCIN();

	pthread_join(s_eenlThTimer, NULL);

	DLOG_Debug("===== enl dispose all devices =====");
	EENL_DisposeAllDevices();

	DLOG_Debug("===== enl terminate =====");
	EENL_Terminate();

	/* EINVへの停止完了通知 */
	ESYS_NotifyStopResult(EENL_SPORT, MOD_ACTION_RESULT_SUCCESS);
	DEBUG_TRCOUT();
	return 0;
}


/*==========================================================================*/
/*	Description		:設置場所取得の他プロセスに対するIF						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　location	設置場所格納アドレス					*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-2:システムコールエラー)					*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SyncGetLocation(EenlLocation_t* location)
{
	EenlGetLocation_t* sendMsg = NULL;		/* 設置場所取得  要求  構造体 */
	EenlGetLocationRet_t* retMsg = NULL;		/* 設置場所取得応答 */
	slong result = 0;		/* 戻り値初期化 */

	DEBUG_TRCIN();

	/* パラメータ格納メモリ確保 */
	sendMsg = CMN_MALLOC(MSG_MAX_SIZE_DEF);
	retMsg = CMN_MALLOC(MSG_MAX_SIZE_DEF);

	/* パラメータヘッダ設定 */
	sendMsg->header.msgId = EENL_GET_LOCATION;		/* メッセージID */

	/* 同期メッセージ送信 */
	result = FRM_CallSync(EENL_SPORT, sizeof(EenlGetLocation_t), sendMsg, &retMsg);
	if (result != 0) {
		/* システムコールエラー */
		result = -2;
	} else {
		result = retMsg->header.retCode;
		*location = retMsg->location;
	}

	/* パラメータ格納メモリ開放 */
	CMN_FREE(sendMsg);
	CMN_FREE(retMsg);

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:設置場所設定の他プロセスに対するIF						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　location	設置場所								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:引数異常/-2:システムコールエラー)		*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_AsyncSetLocation(const EenlLocation_t location)
{
	EenlSetLocation_t* sendMsg = NULL;		/* 設置場所設定  要求  構造体 */
	slong result = 0;						/* 戻り値初期化 */

	DEBUG_TRCIN();

	/* その他と車庫以外の場合 */
	if ((location != EENL_LOCATION_INSIDE)
		&& (location != EENL_LOCATION_OUTSIDE)) {
		result = -1;
	} else {
		/* パラメータ格納メモリ確保 */
		sendMsg = CMN_MALLOC(MSG_MAX_SIZE_DEF);

		/* パラメータヘッダ設定 */
		sendMsg->header.msgId = EENL_SET_LOCATION;	/* メッセージID */
		sendMsg->location = location;				/* 設置場所設定 */

		/* メッセージ送信 */
		result = FRM_MsgSendAsync(EENL_SPORT, 
			sizeof(EenlSetLocation_t), sendMsg);

		if (result != 0) {
			/* システムコールエラー */
			result = -2;
		} else {
			/* 処理無し */
		}

		/* パラメータ格納メモリ開放 */
		CMN_FREE(sendMsg);
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:ENLバージョン取得の他プロセスに対するIF				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　enl_version	ENLバージョン格納アドレス			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:引数異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SyncGetVersion(uchar* enl_version)
{

	DEBUG_TRCIN();

	if (enl_version == NULL) {
		/* 引数異常 */
		return EENL_ERR;
	} else {
		/* 内部で持っている固定値をコピーして返す */
		strncpy((char*)enl_version, EENL_VERSION_FIX_STR, strlen(EENL_VERSION_FIX_STR));
	}

	DEBUG_TRCOUT();

	return EENL_OK;
}

/*==========================================================================*/
/*	Description		:V2H設置場所取得の他プロセスに対するIF					*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　location	設置場所格納アドレス					*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-2:システムコールエラー)					*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/13 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SyncGetLocationV2H(EenlLocation_t* location)
{
	EenlGetLocation_t* sendMsg = NULL;		/* 設置場所取得  要求  構造体 */
	EenlGetLocationRet_t* retMsg = NULL;	/* 設置場所取得応答 */
	slong result = 0;						/* 戻り値初期化 */

	DEBUG_TRCIN();

	/* パラメータ格納メモリ確保 */
	sendMsg = CMN_MALLOC(MSG_MAX_SIZE_DEF);
	retMsg = CMN_MALLOC(MSG_MAX_SIZE_DEF);

	/* パラメータヘッダ設定 */
	sendMsg->header.msgId = EENL_GET_V2H_LOCATION;	/* メッセージID */

	/* 構造体はEenlGetLocation_tをそのまま使用 */
	/* 同期メッセージ送信 */
	result = FRM_CallSync(EENL_SPORT,
		sizeof(EenlGetLocation_t), sendMsg, &retMsg);
	if (result != 0) {
		/* システムコールエラー */
		result = -2;
	} else {
		result = retMsg->header.retCode;
		*location = retMsg->location;
	}

	/* パラメータ格納メモリ開放 */
	CMN_FREE(sendMsg);
	CMN_FREE(retMsg);

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:V2H設置場所設定の他プロセスに対するIF					*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　location	設置場所								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:引数異常/-2:システムコールエラー)		*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/13 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_AsyncSetLocationV2H(const EenlLocation_t location)
{
	EenlSetLocation_t* sendMsg = NULL;		/* 設置場所設定  要求  構造体 */
	slong result = 0;						/* 戻り値初期化 */

	DEBUG_TRCIN();

	/* 戻り値初期化 */
	if ((location != EENL_LOCATION_OTHERS)
		&& (location != EENL_LOCATION_GARAGE)) {
		result = -1;
	} else {
		/* パラメータ格納メモリ確保 */
		sendMsg = CMN_MALLOC(MSG_MAX_SIZE_DEF);

		/* パラメータヘッダ設定 */
		/* メッセージID */
		sendMsg->header.msgId = EENL_SET_V2H_LOCATION;
		sendMsg->location = location;

		/* 構造体はEenlSetLocation_tをそのまま使用 */
		/* メッセージ送信 */
		result = FRM_MsgSendAsync(EENL_SPORT,
			sizeof(EenlSetLocation_t), sendMsg);
		if (result != 0) {
			/* システムコールエラー */
			result = -2;
		} else {
			/* 処理無し */
		}

		/* パラメータ格納メモリ開放 */
		CMN_FREE(sendMsg);
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:HEMS発電抑制履歴取得の他プロセスに対するIF				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　hemshistory	HEMS発電抑制履歴格納アドレス		*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-2:システムコールエラー)					*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/13 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SyncGetHemsHistory(EenlHemsHistory_t* hemshistory)
{
	EenlGetHemsHistory_t* sendMsg = NULL;		/* HEMS発電抑制履歴取得  要求  構造体 */
	EenlGetHemsHistoryRet_t* retMsg = NULL;		/* HEMS発電抑制履歴取得応答 */
	slong result = 0;							/* 戻り値初期化 */

	DEBUG_TRCIN();

	/* パラメータ格納メモリ確保 */
	sendMsg = CMN_MALLOC(MSG_MAX_SIZE_DEF);
	retMsg = CMN_MALLOC(MSG_MAX_SIZE_DEF);

	/* パラメータヘッダ設定 */
	sendMsg->header.msgId = EENL_GET_HEMS_HISTORY;	/* メッセージID */

	 /* 構造体はEenlSetLocation_tをそのまま使用 */
	result = FRM_CallSync(EENL_SPORT,
		sizeof(EenlGetHemsHistory_t), sendMsg, &retMsg);
	if (result != 0) {
		/* システムコールエラー */
		result = -2;
	} else {
		result = retMsg->header.retCode;
		memcpy(hemshistory, &retMsg->hemshistory, sizeof(EenlHemsHistory_t));
	}

	/* パラメータ格納メモリ開放 */
	CMN_FREE(sendMsg);
	CMN_FREE(retMsg);

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:einv側の試運転状態設定イベント							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　state	試運転状態								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常/-2:システムコールエラー)			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/26 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_EventSetTestRunState(const EenlTestRunState_t state)
{
	EenlTestRun_t* sendMsg = NULL;		/* 試運転状態  応答  構造体 */
	slong result = 0;					/* 戻り値初期化 */

	DEBUG_TRCIN();

	/* 戻り値初期化 */
	if ((state != EENL_TESTRUN_UNDEF)
		&& (state != EENL_TESTRUN_STOP)
		&& (state != EENL_TESTRUN_START)) {
		result = -1;
	} else {
		/* パラメータ格納メモリ確保 */
		sendMsg = CMN_MALLOC(MSG_MAX_SIZE_DEF);

		/* メッセージID */
		sendMsg->Header.msgId = EENL_SET_TESTRUN_STATUS;
		/* 試運転状態 */
		sendMsg->state = state;

		/* 非同期イベント送信 */
		result = FRM_MsgSendEvent(EENL_SPORT, 
			sizeof(EenlTestRun_t), sendMsg);
		if (result != 0) {
			/* システムコールエラー */
			result = -2;
		} else {
			/* 処理無し */
		}

		/* パラメータ格納メモリ開放 */
		CMN_FREE(sendMsg);
	}

	DEBUG_TRCOUT();
	return result;
}

/*==========================================================================*/
/*	Description		:einv側の状態変化通知イベント							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　sendMsg	状態変化通知イベントメッセージ構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常/-2:システムコールエラー)			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/29 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_EventSttsChange(EenlEvtSttsChangeReq_t *sendMsg)
{
	slong result = 0;		/* 戻り値初期化 */

	DEBUG_TRCIN();

	/* 戻り値初期化 */
	if (NULL == sendMsg) {
		result = -1;
	} else {
		/* メッセージID */
		sendMsg->header.msgId = EENL_EVT_STTS_CHANGE_REQ;

		/* 非同期イベント送信 */
		result = FRM_MsgSendEvent(EENL_SPORT,
			sizeof(EenlEvtSttsChangeReq_t), sendMsg);
		if (result != 0) {
			/* システムコールエラー */
			result = -2;
		} else {
			/* 処理無し */
		}
	}

	DEBUG_TRCOUT();
	return result;
}

/*==========================================================================*/
/*	Description		: einv側の設定変更通知イベント							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　sendMsg	設定変更通知イベントメッセージ構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常/-2:システムコールエラー)			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/29 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_EventSpChange(EenlEvtSpChangeReq_t* sendMsg)
{
	slong result = 0;		/* 戻り値初期化 */

	DEBUG_TRCIN();

	/* 戻り値初期化 */
	if (NULL == sendMsg) {
		result = -1;
	} else {
		/* メッセージID */
		sendMsg->header.msgId = EENL_EVT_SP_CHANGE_REQ;

		/* 非同期イベント送信 */
		result = FRM_MsgSendEvent(EENL_SPORT,
			sizeof(EenlEvtSpChangeReq_t), sendMsg);
		if (result != 0) {
			/* システムコールエラー */
			result = -2;
		} else {
			/* 処理無し */
		}
	}

	DEBUG_TRCOUT();
	return result;
}

/*==========================================================================*/
/*	Description		: einv側のV2H状態変化イベント							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　sendMsg	V2H状態変化イベントメッセージ構造体		*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常/-2:システムコールエラー)			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/29 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_EventV2hSttsChange(EenlEvtV2hSttsChangeReq_t* sendMsg)
{
	slong result = 0;		/* 戻り値初期化 */

	DEBUG_TRCIN();

	/* 戻り値初期化 */
	if (NULL == sendMsg) {
		result = -1;
	} else {
		/* メッセージID */
		sendMsg->header.msgId = EENL_EVT_V2H_STTS_CHANGE_REQ;

		/* 非同期イベント送信 */
		result = FRM_MsgSendEvent(EENL_SPORT,
			sizeof(EenlEvtV2hSttsChangeReq_t), sendMsg);
		if (result != 0) {
			/* システムコールエラー */
			result = -2;
		} else {
			/* 処理無し */
		}
	}

	DEBUG_TRCOUT();
	return result;
}

/*==========================================================================*/
/*	Description		: einv側のV2H設定変更イベント							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　sendMsg	V2H設定変更イベントメッセージ構造体		*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常/-2:システムコールエラー)			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/29 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_EventV2hSettingChange(EenlEvtV2hSttingChangeReq_t* sendMsg)
{
	slong result = 0;		/* 戻り値初期化 */

	DEBUG_TRCIN();

	/* 戻り値初期化 */
	if (NULL == sendMsg) {
		result = -1;
	} else {
		/* メッセージID */
		sendMsg->header.msgId = EENL_EVT_V2H_SETTING_CHANGE_REQ;

		/* 非同期イベント送信 */
		result = FRM_MsgSendEvent(EENL_SPORT,
			sizeof(EenlEvtV2hSttingChangeReq_t), sendMsg);
		if (result != 0) {
			/* システムコールエラー */
			result = -2;
		} else {
			/* 処理無し */
		}
	}

	DEBUG_TRCOUT();
	return result;
}

/*==========================================================================*/
/*	Description		: esch側の設定変更通知イベント							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　sendMsg	設定変更通知イベント構造体				*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常/-2:システムコールエラー)			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/29 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_EventSettingChange(EenlEvtSttingChangeReq_t* sendMsg)
{
	slong result = 0;		/* 戻り値初期化 */

	DEBUG_TRCIN();

	/* 戻り値初期化 */
	if (NULL == sendMsg) {
		result = -1;
	} else {
		/* メッセージID */
		sendMsg->header.msgId = EENL_EVT_SETTING_CHANGE;

		/* 非同期イベント送信 */
		result = FRM_MsgSendEvent(EENL_SPORT,
			sizeof(EenlEvtSttingChangeReq_t), sendMsg);
		if (result != 0) {
			/* システムコールエラー */
			result = -2;
		} else {
			/* 処理無し */
		}
	}

	DEBUG_TRCOUT();
	return result;
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*==========================================================================*/
/*	Description		:device main(thread function)							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info	デバイス情報								*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:invoke this thread for each nodes as main process		*/
/*					:other threads wake this thread with event code			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_ThreadDevice(EenlDeviceInfo_t* info)
{
	ENL_MESSAGE* msg = NULL;
	uchar epc = 0u;
	slong event = EENL_EVENT_NONE;
	bool_t isLoop = TRUE;

	DEBUG_TRCIN();

	while (isLoop) {
		EENL_EventWait(info);
		epc = info->epc;
		event = info->event;
		EENL_EvenClear(info);
		if (event == EENL_EVENT_DISPOSE) {
			DLOG_Debug("[0x%08lx] device disposesd", (ulong)info);
			isLoop = FALSE;

		} else if (event == EENL_EVENT_READ) {
			/* ノード　Get */
			DLOG_Debug("[node]tepc: (0x%02hhX) read", epc);


		} else if (event == EENL_EVENT_WRITE) {
			/* ノード　Set */
			DLOG_Debug("[node]epc: (0x%02hhX) write", epc);


		} else if (event == EENL_EVENT_BATTERY_READ) {
			/* 蓄電池　Get */
			DLOG_Debug("[battery] epc: (0x%02hhX) read", epc);


		} else if (event == EENL_EVENT_SOLAR_READ) {
			/* 太陽光　Get */
			DLOG_Debug("[solar]epc: (0x%02hhX) read", epc);


		} else if (event == EENL_EVENT_V2H_READ) {
			/* 車両充放電器　Get */
			DLOG_Debug("[v2h]epc: (0x%02hhX) read", epc);


		} else if (event == EENL_EVENT_BATTERY_WRITE) {
			/* 蓄電池　Set */
			DLOG_Debug("[battery] epc: (0x%02hhX) write", epc);


		} else if (event == EENL_EVENT_SOLAR_WRITE) {
			/* 太陽光　Set */
			DLOG_Debug("[solar]epc: (0x%02hhX) write", epc);


		} else if (event == EENL_EVENT_V2H_WRITE) {
			/* 車両充放電器　Set */
			DLOG_Debug("[v2h] epc: (0x%02hhX) write", epc);


		} else if (event == EENL_EVENT_ACCEPTED) {
			msg = EENL_GetLatestRequest();
			DLOG_Debug("[0x%08lx] latest request (0x%08lx) was accepted",
				(ulong)info, (ulong)msg);

		} else if (event == EENL_EVENT_REJECTED) {
			msg = EENL_GetLatestRequest();
			DLOG_Debug("[0x%08lx] latest request (0x%08lx) was rejected", 
				(ulong)info, (ulong)msg);

		} else {
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:create device(device management)						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　regInfo	登録情報構造体							*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_CreateDevice(EPCS_REG_INFO_T regInfo)
{
	slong ret = 0;
	EenlDeviceInfo_t* info = NULL;
	ENL_MESSAGE* msg = NULL;
	ushort meterDevCnt = 0u;
	
	DEBUG_TRCIN();

	EENL_InitEpcValidFlg();	/*  EPC設定済フラグを初期化 */

	/*  PGreliefでチェックに引っかかりますが、enl_dispose_all_devices()関数で開放しているため修正なし。 */
	info = CMN_MALLOC(sizeof(EenlDeviceInfo_t));

	if (info != NULL) {

		info->addr.ipType = EENL_IP_TYPE_V4;
		info->addr.v4addr.sin_family = AF_INET;			/* IPv4 */
		info->addr.v4addr.sin_addr.s_addr = INADDR_ANY;
		info->addr.v4addr.sin_port = htons(EENL_UDP_PORT);

		info->addr.v6addr.sin6_family = AF_INET6;		/* IPv6 */
		info->addr.v6addr.sin6_addr = in6addr_any;
		info->addr.v6addr.sin6_port = htons(EENL_UDP_PORT);

		/* ノードプロファイルの作成 */
		ret = EENL_CreateNode(info);
		if (ret < 0) {
			DLOG_Error("node profile create error!");
		} else {
			/* DO NOTHING */
		}

		/* create device object instance */
		if ((regInfo.sysConf & EPCS_SYS_CON_BT) == EPCS_SYS_CON_BT) {
			/* 蓄電池デバイス */
			ret = EENL_CreateDeviceBattery(info);
			if (ret == 0) {
				/* デバイスを登録 */
				ENL_DEV_register_instance(&msg, info->node, info->devBattery);
			} else {
				DLOG_Error("battery device create error!");
			}
		} else {
			info->devBattery = NULL;
		}

		if ((regInfo.sysConf & EPCS_SYS_CON_PV) == EPCS_SYS_CON_PV) {
			/* 太陽光発電デバイス */
			ret = EENL_CreateDeviceSolarNcc(info);
			if (ret == 0) {
				/* デバイスを登録 */
				ENL_DEV_register_instance(&msg, info->node, info->devSolar);
			} else {
				DLOG_Error("solar device create error!(ncc)");
			}
		} else {
			info->devSolar = NULL;
		}

		if ((regInfo.sysConf & EPCS_SYS_CON_V2H) == EPCS_SYS_CON_V2H) {
			/* 車両充放電器デバイス */
			ret = EENL_CreateDeviceV2h(info);
			if (ret == 0) {
				/* デバイスを登録 */
				ENL_DEV_register_instance(&msg, info->node, info->devV2h);
			} else {
				DLOG_Error("v2h device create error!");
			}
		} else {
			info->devV2h = NULL;
		}

		if ((regInfo.sysConf & EPCS_SYS_CON_METER_E1) == EPCS_SYS_CON_METER_E1) {
			/* 分散型電源電力量メータデバイス */
			ret = EENL_CreateDeviceMeter(info, EPCS_SYS_CON_METER_E1, meterDevCnt);
			if (ret == 0) {
				/* デバイスを登録 */
				ENL_DEV_register_instance(&msg, info->node, info->devMeterE1);
				meterDevCnt++;
			} else {
				DLOG_Error("meter E1 device create error!");
			}
		} else {
			info->devMeterE1 = NULL;
		}

		if ((regInfo.sysConf & EPCS_SYS_CON_METER_C2) == EPCS_SYS_CON_METER_C2) {
			/* 分散型電源電力量メータデバイス */
			ret = EENL_CreateDeviceMeter(info, EPCS_SYS_CON_METER_C2, meterDevCnt);
			if (ret == 0) {
				/* デバイスを登録 */
				ENL_DEV_register_instance(&msg, info->node, info->devMeterC2);
			} else {
				DLOG_Error("meter C2 device create error!");
			}
		} else {
			info->devMeterC2 = NULL;
		}

		/*  Release K start */
		/* デバイスリストを初期化 */
		eenl_InitDevList();  /* EENL_CreateDeviceMpcs()でデバイスリストが必要になるため移動 */

		/* マルチ入力PCSデバイス */
		/* 機器構成は限定しない */
		/* 接続機器リスト(0xE8)を作成するために各デバイスのインスタンスアドレスが必要になるため、 */
		/* マルチ入力PCSデバイスの作成は各デバイス作成後に行う */
		ret = EENL_CreateDeviceMpcs(info);
		if (ret == 0) {
			/* デバイスを登録 */
			ENL_DEV_register_instance(&msg, info->node, info->devMpcs);
		} else {
			DLOG_Error("mpcs device create error!");
		}
		/*  Release K end */

		info->event = EENL_EVENT_NONE;

		pthread_mutex_init(&info->mutex, NULL);
		pthread_cond_init(&info->cond, NULL);

		pthread_create(&info->th, NULL,
			(void* (*)(void*))eenl_ThreadDevice, (void*)info);

		info->next = NULL;

		EENL_RegisterDeviceInfo(info);

		if (msg != NULL) {
			/* 状変アナウンス通知 */

			sleep(2);

			EENL_SendMessage(NULL, msg);
			DLOG_Debug("#### send broadcast message (node:0xD5) ####");
		} else {
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:グローバル情報のデバイスリストの初期化					*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:EENL_CreateNode()を呼び出してから実行すること			*/
/*					:EENL_CreateDeviceBattery()を呼び出してから実行すること	*/
/*					:EENL_CreateDeviceSolarNcc()を呼び出してから実行すること*/
/*					:EENL_CreateDeviceV2h()を呼び出してから実行すること		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_InitDevList(void)
{
	slong i = 0;

	DEBUG_TRCIN();

	/* 初期化 */
	for (i = EENL_DEV_LIST_NODE; i < EENL_DEV_LIST_MAX; i++) {
		g_eenlDevList[i] = NULL;
	}

	/* デバイスリストにインスタンスのアドレスを格納 */
	/* マルチ入力PCSはEENL_CreateDeviceMpcs()で格納する */
	g_eenlDevList[EENL_DEV_LIST_NODE] = g_eenlDevNode;
	g_eenlDevList[EENL_DEV_LIST_SO] = g_eenlDevSolar;
	g_eenlDevList[EENL_DEV_LIST_BT] = g_eenlDevBattery;
	g_eenlDevList[EENL_DEV_LIST_V2H] = g_eenlDevV2h;

	DEBUG_TRCOUT();
}

#if 0
/*==========================================================================*/
/*	Description		:グローバル情報のデバイスリストの初期化					*/
/*--------------------------------------------------------------------------*/
/*	param			:info	デバイス情報									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (EENL_OK:正常終了)										*/
/*--------------------------------------------------------------------------*/
/*	detail			:EENL_UpdateEpcsBattery()を呼び出してから実行すること	*/
/*					:update_epcs_solar_ncc()を呼び出してから実行すること	*/
/*					:update_epcs_v2h()を呼び出してから実行すること			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_SetEpcsEvent(EenlDeviceInfo_t* info)
{
	ulong  eventType = 0u;
	ulong  eventTypeSp = 0u;
	ulong  eventTypeAddSp = 0u;
	ulong receiver = EENL_SPORT;
	slong ret = 0;

	DEBUG_TRCIN();

	/*  EPCS_EVT_OPE_STTSとEPCS_EVT_PWROUTAGEは必須 */
	eventType = EPCS_EVT_OPE_STTS | EPCS_EVT_PWROUTAGE;

	if (info->devBattery != NULL) {
		/*  epcsへの状態変化通知イベント登録(仕向け共通のフラグを初期値に設定しておく) */
		eventType = eventType | EPCS_EVT_OPE_MODE
			| EPCS_EVT_EMG_MODE | EPCS_EVT_EMG_PER
			| EPCS_EVT_TIME_ZONE | EPCS_EVT_SOH
			| EPCS_EVT_RESET_PWR_SET | EPCS_EVT_AC_CHARGE_WAIT 
			| EPCS_EVT_AC_DISCHARGE_WAIT;

	} else {
		/* DO NOTHING */
	}

	if (info->devSolar != NULL) {
		/*  epcsへの状態変化通知イベント登録 */
		eventType = eventType | EPCS_EVT_OPE_MODE
			| EPCS_EVT_PVGEN_NOTICE | EPCS_EVT_SUPPRESS_STATE;

		/*  epcsへの設定変更通知イベント登録 */
		eventTypeSp = eventTypeSp | EPCS_EVT_OTP;

		/*  epcsへの追加設定変更通知イベント登録 */
		eventTypeAddSp = eventTypeAddSp | EPCS_EVT_ADDM_PWR_FACT_CTRL
			| EPCS_EVT_ADDM_GEN_PWR_CAP | EPCS_EVT_ADDM_FIT_TYPE
			| EPCS_EVT_ADDM_SELF_CONSUMP | EPCS_EVT_ADDM_FACILITY_CAP;
	} else {
		/* DO NOTHING */
	}

	DLOG_Debug("eventType = %ld(0x%08lx). eventTypeSp = %ld(0x%08lx). eventTypeAddSp = %ld(0x%08lx)",
		eventType, eventType, eventTypeSp, eventTypeSp, eventTypeAddSp, eventTypeAddSp);

	/*  epcsの状態変化通知を登録 */
	if (eventType != 0u) {
		ret = epcs_sync_regSttsChangeEvent(receiver, eventType);
		DLOG_Debug("regSttsChangeEvent : 0x%08lx", eventType);
		if (ret != EPCS_OK) {
			DLOG_Error("regSttsChangeEvent error! : %ld", ret);
			/* 処理続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	/*  epcsの設定変更通知を登録 */
	if ((eventTypeSp != 0u) || (eventTypeAddSp != 0u)) {
		ret = epcs_sync_regAddMSpChangeEvent(receiver, eventTypeSp, eventTypeAddSp);
		DLOG_Debug("regAddMSpChangeEvent eventTypeSp    : 0x%08lx", eventTypeSp);
		DLOG_Debug("regAddMSpChangeEvent eventTypeAddSp : 0x%08lx", eventTypeAddSp);
		if (ret != EPCS_OK) {
			DLOG_Error("regAddMSpChangeEvent error! : %ld", ret);
			/* 処理続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	if (info->devV2h != NULL) {
		/*  epcsへのV2H状態変化通知イベント登録 */
		/*  T3でEPCS_EVT_V2H_OPEMODE_STATEの登録を削除 */
		ulong eventTypeV2HStts = EPCS_EVT_V2H_CONNECTOR_LOCK_STATE 
			| EPCS_EVT_V2H_AVAILABILITY_STATE; 

		/*  epcsへのV2H設定変更通知イベント登録 */
		ulong eventTypeV2HSttingSet = EPCS_EVT_V2H_CHARGE_AMOUNT_SET_1
			| EPCS_EVT_V2H_RATED_VOLT
			| EPCS_EVT_V2H_DISCHARGE_SET_VALUE;
		/*  EPCS_EVT_V2H_OPEMODE;  T3でEPCS_EVT_V2H_OPEMODEの登録を削除 */
		ulong eventTypeV2HSttingMode = 0; 

		/*  epcsのV2H状態変化通知を登録 */
		ret = epcs_sync_regV2hSttsChangeEvent(receiver, eventTypeV2HStts);
		if (ret != EPCS_OK) {
			DLOG_Error("regV2hSttsChangeEvent error! : %ld", ret);
			/* 処理続行 */
		} else {
			/* DO NOTHING */
		}
		DLOG_Debug("regV2hSttsChangeEvent : 0x%08lx", eventTypeV2HStts);

		/*  epcsへのV2H設定変更通知を登録 */
		ret = epcs_sync_regV2hSttingChangeEvent(receiver,
			eventTypeV2HSttingSet, eventTypeV2HSttingMode);
		if (ret != EPCS_OK) {
			DLOG_Error("regV2hSttingChangeEvent error! : %ld", ret);
			/* 処理続行 */
		} else {
			/* DO NOTHING */
		}
		DLOG_Debug("regV2hSttingChangeEvent : 0x%08lx 0x%08lx", 
			eventTypeV2HSttingSet, eventTypeV2HSttingMode);
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
	return EENL_OK;
}

/*==========================================================================*/
/*	Description		:eschへのイベント登録									*/
/*--------------------------------------------------------------------------*/
/*	param			:info	デバイス情報									*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:update_esch_solar()を呼び出してから実行すること		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_SetEschEvent(EenlDeviceInfo_t* info)
{
	slong ret = 0;

	DEBUG_TRCIN();

	if (info->devSolar != NULL) {
		/*  eschへのイベント登録 */
		ret = esch_async_regSettingChangeEvent(EENL_SPORT,
			(ESCH_EVT_OUTPUT_CTRL_ONOFF | ESCH_EVT_NEXT_ACCESS_DATE),
			ESCH_REG_NOTICE_ENABLE);
		if (ret < 0) {
			DLOG_Error("eenl_SetEschEvent error!");
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
}
#endif

/*==========================================================================*/
/*	Description		:充放電関連EPCの設定値をepcsに反映						*/
/*--------------------------------------------------------------------------*/
/*	param			:info	デバイス情報									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常									*/
/*--------------------------------------------------------------------------*/
/*	detail			:EENL_UpdateEpcsBattery()を呼び出してから実行すること	*/
/*					:eenl_et_epcs_event()を呼び出してから実行すること		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_SetEpcsBattery(EenlDeviceInfo_t* info)
{
	slong ret = 0;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (info == NULL) {
		DLOG_Error("-----eenl_SetEpcsBattery NULL-----");
		DEBUG_TRCOUT();
		return -1;	/* error */
	} else {
		/* DO NOTHING */
	}

	/* パラメータチェック */
	if (info->devBattery == NULL) { /*  batteryが無いなら */
		DLOG_Debug("-----eenl_SetEpcsBattery pass-----");
		DEBUG_TRCOUT();
		return 0;	/* errorにしない */
	} else {
		/* DO NOTHING */
	}

	/* 前回のAC充電量設定値をepcsに反映 */
	if (g_eenlBattery0xAA == 0xFFFFFFFF) {
		/* 初期値はEPCSには反映しない */

		DLOG_Debug("0xAA einv NOT set. (%ld, 0x%08lx)",
			g_eenlBattery0xAA, g_eenlBattery0xAA);
		/* セットせず、処理は続行 */
	} else {
		/*  単位は[Wh]で変換不要 */
		ret = epcs_sync_setAcChargePwr(g_eenlBattery0xAA);
		if (ret == EPCS_OK) {
			/* 設定OK */
			DLOG_Debug("0xAA einv load set ok! (%ld, %08lx)", 
				g_eenlBattery0xAA, g_eenlBattery0xAA);
		} else {
			/* 設定失敗 */
			DLOG_Error("0xAA einv load set NG! :%ld (%ld, %08lx)", 
				ret, g_eenlBattery0xAA, g_eenlBattery0xAA);
		}
	}

	/* 前回のAC放電量設定値をeinvに反映 */
	/*   前回のAC放電量設定値情報は、EENL_FileRead()であらかじめグローバル変数に格納済み */
	if (g_eenlBattery0xAB == 0xFFFFFFFF) {
		/* 初期値はeinvには反映しない */

		DLOG_Debug("0xAB einv NOT set. (%ld, 0x%08lx)", 
			g_eenlBattery0xAA, g_eenlBattery0xAA);
	} else {
		/*  単位は[Wh]で変換不要 */
		ret = epcs_sync_setAcDischargePwr(g_eenlBattery0xAB);
		if (ret == EPCS_OK) {
			/* 設定OK */
			DLOG_Debug("0xAB einv load set ok! (%ld, %08lx)", 
				g_eenlBattery0xAB, g_eenlBattery0xAB);
		} else {
			/* 設定失敗 */
			DLOG_Error("0xAB einv load set NG! :%ld (%ld, %08lx)",
				ret, g_eenlBattery0xAB, g_eenlBattery0xAB);
		}
	}

	/* 前回の充電量設定値をeinvに反映 */
	if (g_eenlBattery0xE7 == 0xFFFFFFFF) {
		/* 初期状態の場合は、処理しない */

		DLOG_Debug("0xE7 einv NOT set. (%ld, 0x%08lx)",
			g_eenlBattery0xE7, g_eenlBattery0xE7);
	} else {
		/*  単位は[Wh]で変換不要 */
		ret = epcs_sync_setChargePwr(g_eenlBattery0xE7);
		if (ret == EPCS_OK) {
			/* 設定OK */
			DLOG_Debug("0xE7 einv load set ok! (%ld, %08lx)", 
				g_eenlBattery0xE7, g_eenlBattery0xE7);
		} else {
			/* 設定失敗 */
			DLOG_Error("0xE7 einv load set NG! :%ld (%ld, %08lx)", 
				ret, g_eenlBattery0xE7, g_eenlBattery0xE7);
		}
	}

	/* 前回の放電量設定値をeinvに反映 */
	if (g_eenlBattery0xE8 == 0xFFFFFFFF) {
		/* 初期状態の場合は、何もしない */

		DLOG_Debug("0xE8 einv NOT set. (%ld, 0x%08lx)", 
			g_eenlBattery0xE8, g_eenlBattery0xE8);
	} else {
		/*  単位は[Wh]で変換不要 */
		ret = epcs_sync_setDischargePwr(g_eenlBattery0xE8);
		if (ret == EPCS_OK) {
			/* 設定OK */
			DLOG_Debug("0xE8 einv load set ok! (%ld, %08lx)",
				g_eenlBattery0xE8, g_eenlBattery0xE8);
		} else {
			/* 設定失敗 */
			DLOG_Error("0xE8 einv load set NG! :%ld (%ld, %08lx)",
				ret, g_eenlBattery0xE8, g_eenlBattery0xE8);
		}
	}

	DEBUG_TRCOUT();
	return 0;
}

/*==========================================================================*/
/*	Description		:状態変化通知イベント受信処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]fmsg	メッセージ構造体							*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_EvtSttsChangeReq(EenlEvtSttsChangeReq_t *fmsg)
{
	EenlEvtSttsChangeReq_t *cmsg	= NULL;
	slong ret						= 0;

	DEBUG_TRCIN();

	cmsg = fmsg;

	DLOG_Debug("changedFlag : %08lx", cmsg->changedFlag);
	DLOG_Debug("changedProcessMsgId : %ld", cmsg->header.srcModId);

	/* 運転モード変更通知 */
	if ((cmsg->changedFlag & EPCS_EVT_OPE_MODE) == EPCS_EVT_OPE_MODE) {
		DLOG_Debug("<opeMode>");

		/* 運転モード変更通知処理 */
		ret = EENL_EvtEpcsOpeMode(cmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態を更新できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}


	/* 非常時安心設定変更通知 */
	if ((cmsg->changedFlag & EPCS_EVT_EMG_PER) == EPCS_EVT_EMG_PER) {
		DLOG_Debug("<emergencyPercent>");

		/* 非常時安心設定変更通知処理 */
		ret = EENL_EvtEpcsEmgPer(cmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態を更新できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}

	} else {
		/* DO NOTHING */
	}

	/* 充放電時間帯変更通知 */
	if ((cmsg->changedFlag & EPCS_EVT_TIME_ZONE) == EPCS_EVT_TIME_ZONE) {
		DLOG_Debug("<chargeTimeZone>");

		/* 充放電時間帯変更通知処理 */
		ret = EENL_EvtEpcsChargeTimeZone(cmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態を更新できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
			} else {
		/* DO NOTHING */
	}

	/* 運転中状態変更通知 */
	if ((cmsg->changedFlag & EPCS_EVT_OPE_STTS) == EPCS_EVT_OPE_STTS) {
		DLOG_Debug("<operation>");

		/* 運転中状態変更通知処理 */
		ret = EENL_EvtEpcsOpeStts(cmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態を更新できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	/* 停電状態変更通知 */
	if ((cmsg->changedFlag & EPCS_EVT_PWROUTAGE) == EPCS_EVT_PWROUTAGE) {
		DLOG_Debug("<pwrCut>");

		/* 停電状態変更通知処理 */
		ret = EENL_EvtEpcsPwroutage(cmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態を更新できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	/* SOH値変更通知 */
	if ((cmsg->changedFlag & EPCS_EVT_SOH) == EPCS_EVT_SOH) {
		DLOG_Debug("<soh>");

		ret = EENL_EvtEpcsSoh(cmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態を更新できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	/* 出力抑制状態(CMD81)変化通知 */
	if ((cmsg->changedFlag & EPCS_EVT_SUPPRESS_STATE) == EPCS_EVT_SUPPRESS_STATE) {
		DLOG_Debug("<pvgen suppressState>");
		ret = EENL_EvtEpcsSuppressState(cmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態を更新できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	/* 充電電力設定値リセット通知 */
	if ((cmsg->changedFlag 
		& EPCS_EVT_RESET_PWR_SET) == EPCS_EVT_RESET_PWR_SET) {
		DLOG_Debug("<reset power set>");

		ret = EENL_EvtEpcsResetPwrset(cmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態を更新できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
			}
	} else {
		/* DO NOTHING */
	}

	/* AC充電量設定値待機動作通知 */
	if ((cmsg->changedFlag 
		& EPCS_EVT_AC_CHARGE_WAIT) == EPCS_EVT_AC_CHARGE_WAIT) {
		DLOG_Debug("<ac charge wait>");

		ret = EENL_EvtEpcsAcChargeWait();
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態を更新できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	/* AC放電量設定値待機動作通知 */
	if ((cmsg->changedFlag 
		& EPCS_EVT_AC_DISCHARGE_WAIT) == EPCS_EVT_AC_DISCHARGE_WAIT) {
		DLOG_Debug("<ac discharge wait>");

		ret = EENL_EvtEpcsAcDischargeWait();
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態を更新できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:einvからの設定変更通知イベント受信処理					*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]fmsg	メッセージ構造体							*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_EvtSpChangeReq(EenlEvtSpChangeReq_t* fmsg)
{
	slong ret = 0;

	DEBUG_TRCIN();

	/*  設定変更通知イベント */
	/*  実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/*  出力増減速度変更通知 */
	if ((fmsg->changedFlag & EPCS_EVT_OTP) == EPCS_EVT_OTP) {
		DLOG_Debug(" <EVT_SP otp>");
		ret = EENL_EvtEpcsOutputFluctuateSpdChg0xB3(fmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態は反映できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	/*  力率一定制御変更通知 */
	if ((fmsg->addChangedFlag 
		& EPCS_EVT_ADDM_PWR_FACT_CTRL) == EPCS_EVT_ADDM_PWR_FACT_CTRL) {
		DLOG_Debug("<EVT_SP pwrFactCtrl>");
		ret = EENL_EvtEpcsPwrFactCtrlSpdChg(fmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態は反映できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	/*  発電電力上限変更通知 */
	if ((fmsg->addChangedFlag 
		& EPCS_EVT_ADDM_GEN_PWR_CAP) == EPCS_EVT_ADDM_GEN_PWR_CAP) {
		DLOG_Debug("<EVT_SP genPwrCap>");
		ret = EENL_EvtEpcsGenPwrCapSpdChg(fmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態は反映できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	/*  FIT契約タイプ変更通知 */
	if ((fmsg->addChangedFlag 
		& EPCS_EVT_ADDM_FIT_TYPE) == EPCS_EVT_ADDM_FIT_TYPE) {
		DLOG_Debug("<EVT_SP fitType>");
		ret = EENL_EvtEpcsFitTypeSpdChg(fmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態は反映できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	/*  自家消費タイプ変更通知 */
	if ((fmsg->addChangedFlag 
		& EPCS_EVT_ADDM_SELF_CONSUMP) == EPCS_EVT_ADDM_SELF_CONSUMP) {
		DLOG_Debug("<EVT_SP selfConsumptionType>");
		ret = EENL_EvtEpcsSelfConsumptionTypeSpdChg(fmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態は反映できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	/*  設備認定容量変更通知 */
	if ((fmsg->addChangedFlag 
		& EPCS_EVT_ADDM_FACILITY_CAP) == EPCS_EVT_ADDM_FACILITY_CAP) {
		DLOG_Debug("<EVT_SP facilityCertifiedCapacity>");
		ret = EENL_EvtEpcsFacilityCertifiedCapacitySpdChg(fmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態は反映できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:V2H状態変化イベント受信処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]fmsg	メッセージ構造体							*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_EvtV2hSttsChangeReq(EenlEvtV2hSttsChangeReq_t *fmsg)
{
	slong ret = 0;

	DEBUG_TRCIN();

	/*  bit 0：コネクタロック状態 */
	if ((fmsg->changedFlag
		& EPCS_EVT_V2H_CONNECTOR_LOCK_STATE) == EPCS_EVT_V2H_CONNECTOR_LOCK_STATE) {
		/* V2H コネクタロック状態変化通知処理 */
		DLOG_Debug("<v2h connector lock>");
		ret = EENL_EvtEpcsV2hConnectorLock(fmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態は反映できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	/*  bit 2：車両接続・充放電可否状態 */
	if ((fmsg->changedFlag
		& EPCS_EVT_V2H_AVAILABILITY_STATE) == EPCS_EVT_V2H_AVAILABILITY_STATE) {
		DLOG_Debug("<v2h availability charge>");

		/* V2H 車両接続・充放電可否状態変化通知処理 */
		ret = EENL_EvtEpcsV2hAvailabilityCharge(fmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態は反映できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:V2H設定変更イベント受信処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]fmsg	メッセージ構造体							*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_EvtV2hSettingChangeReq(EenlEvtV2hSttingChangeReq_t *fmsg)
{
	slong ret = 0;

	DEBUG_TRCIN();

	/* 車載電池充電量設定値1 */
	if ((fmsg->setEventType 
		& EPCS_EVT_V2H_CHARGE_AMOUNT_SET_1) == EPCS_EVT_V2H_CHARGE_AMOUNT_SET_1) {
		DLOG_Debug("<v2h charge amount1>");
		/* 車載電池充電量設定値1変更通知処理 */
		ret = EENL_EvtEpcsV2hChargeAmount1(fmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態は反映できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	/* 車載電池充電量設定値2 */
	//if ((fmsg->setEventType 
	//	& EPCS_EVT_V2H_CHARGE_AMOUNT_SET_2) == EPCS_EVT_V2H_CHARGE_AMOUNT_SET_2) {
	//	DLOG_Debug("<v2h charge amount2>");
	//	/* 車載電池充電量設定値2変更通知処理 */
	//	ret = EENL_EvtEpcsV2hChargeAmount2(fmsg);
	//	if (ret != 0) {
	//		/* 異常の場合 */
	//		/* 状態は反映できていないが、処理は続行 */
	//	} else {
	//		/* DO NOTHING */
	//	}
	//} else {
	//	/* DO NOTHING */
	//}

	/* 車載電池充電電力設定値(通知未使用) */
	if ((fmsg->setEventType 
		& EPCS_EVT_V2H_POWER_SET_CHARGE) == EPCS_EVT_V2H_POWER_SET_CHARGE) {
		DLOG_Debug("<v2h power set charge>");
		/* 車載電池充電電力設定値変更通知処理 */
		ret = EENL_EvtEpcsV2hPowerSetCharge(fmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態は反映できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	/* 車載電池放電電力設定値(通知未使用) */
	if ((fmsg->setEventType 
		& EPCS_EVT_V2H_POWER_SET_DISCHARGE) == EPCS_EVT_V2H_POWER_SET_DISCHARGE) {
		DLOG_Debug("<v2h power set discharge>");
		/* 車載電池放電電力設定値変更通知処理 */
		ret = EENL_EvtEpcsV2hPowerSetDischarge(fmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態は反映できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	/* 車載電池充電電流設定値 */
	//if ((fmsg->setEventType 
	//	& EPCS_EVT_V2H_CURRENT_SET_CHARGE) == EPCS_EVT_V2H_CURRENT_SET_CHARGE) {
	//	DLOG_Debug("<v2h current set charge>");
	//	/* 車載電池充電電流設定値変更通知処理 */
	//	ret = EENL_EvtEpcsV2hCurrentSetCharge(fmsg);
	//	if (ret != 0) {
	//		/* 異常の場合 */
	//		/* 状態は反映できていないが、処理は続行 */
	//	} else {
	//		/* DO NOTHING */
	//	}
	//} else {
	//	/* DO NOTHING */
	//}

	/* 車載電池放電電流設定値 */
	//if ((fmsg->setEventType 
	//	& EPCS_EVT_V2H_CURRENT_SET_DISCHARGE) == EPCS_EVT_V2H_CURRENT_SET_DISCHARGE) {
	//	DLOG_Debug("<v2h current set discharge>");
	//	/* 車載電池放電電流設定値変更通知処理 */
	//	ret = EENL_EvtEpcsV2hCurrentSetDischarge(fmsg);
	//	if (ret != 0) {
	//		/* 異常の場合 */
	//		/* 状態は反映できていないが、処理は続行 */
	//	} else {
	//		/* DO NOTHING */
	//	}
	//} else {
	//	/* DO NOTHING */
	//}

	/* 車両充放電器定格電圧 */
	if ((fmsg->setEventType 
		& EPCS_EVT_V2H_RATED_VOLT) == EPCS_EVT_V2H_RATED_VOLT) {
		DLOG_Debug("<v2h rated volt>");
		/* 車両充放電器定格電圧変更通知処理 */
		ret = EENL_EvtEpcsV2hRatedVolt(fmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態は反映できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:epcsからの設定変更通知イベント受信処理					*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]fmsg	メッセージ構造体							*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_EschReq(EenlEvtSttingChangeReq_t* fmsg)
{
	slong ret = 0;
	slong r_getNextDate = ESCH_OK;
	EenlEvtSttingChangeReq_t* schmsg = NULL;

	DEBUG_TRCIN();
	DLOG_Debug("===== ESCH_EVT_SETTING_CHANGE is called =====");

	/*  実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	schmsg = fmsg;
	if ((schmsg->eventType 
		& ESCH_EVT_OUTPUT_CTRL_ONOFF) == ESCH_EVT_OUTPUT_CTRL_ONOFF) {
		/*  出力制御対象／対象外変更通知 */
		/*  次回アクセス日時と両方変更されていた場合、出力制御対象／対象外変更通知を優先する */
		DLOG_Debug("<EVT_SCH esch_outputCtrlOnOff>");
		if (schmsg->outputCtrlOnOff == ESCH_OUTPUT_CTRL_ON) {
			/* 出力制御対象               */
			/* 次回アクセス日時を取得する */
			r_getNextDate = esch_sync_getNextAccessDate(
				(ESCH_DATE_TIME_T*)(&schmsg->nextAccessDate));
		} else {
			/* 出力制御対象外 or 出力制御未設定              */
			/* 次回アクセス日時を0xFFにするため、0クリアする */
			memset(&schmsg->nextAccessDate,
				0, sizeof(schmsg->nextAccessDate));
		}

		if (r_getNextDate == ESCH_OK) {
			ret = EENL_EvtEschNextAccessDate(schmsg);
			if (ret != 0) {
				/* 異常の場合 */
				/* 状態は反映できていないが、処理は続行 */
			} else {
				/* DO NOTHING */
			}
		} else {
			/* DO NOTHING */
		}
	} else if ((schmsg->eventType 
		& ESCH_EVT_NEXT_ACCESS_DATE) == ESCH_EVT_NEXT_ACCESS_DATE) {
		/*  次回アクセス日時変更通知 */
		DLOG_Debug("<EVT_SCH nextAccessDate>");
		ret = EENL_EvtEschNextAccessDate(schmsg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態は反映できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:同期メッセージ処理										*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　msg	メッセージ									*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/24 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_MsgProcSync(FrmMsgHead_t const* msg)
{
	slong ret = 0;

	DEBUG_TRCIN();

	/* header.functionIDで各処理に分岐 */
	switch (msg->msgId) {
	case EENL_GET_LOCATION:
		DLOG_Debug("===== EENL_GET_LOCATION is called =====");
		/* 設置場所取得 */
		ret = eenl_GetLocation((EenlGetLocation_t*)msg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態を取得できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
		break;

	case EENL_GET_V2H_LOCATION:
		DLOG_Debug("===== EENL_GET_V2H_LOCATION is called =====");
		/* V2H設置場所取得 */
		ret = EENL_GetV2hLocation((EenlGetLocation_t*)msg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態を取得できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
		break;

	case EENL_GET_HEMS_HISTORY:
		DLOG_Debug("===== EENL_GET_HEMS_HISTORY is called =====");
		/* HEMS発電抑制履歴取得 */
		ret = EENL_GetHemsHistory((EenlGetHemsHistory_t*)msg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態を取得できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
		break;
	default:
		/* DO NOTHING */
		break;
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:非同期メッセージ処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　msg	メッセージ									*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/24 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_MsgProcAsync(FrmMsgHead_t const* msg)
{
	slong ret = 0;

	DEBUG_TRCIN();

	/* header.functionIDで各処理に分岐 */
	switch (msg->msgId) {
	case EENL_SET_LOCATION:
		DLOG_Debug("===== EENL_SET_LOCATION is called =====");
		/*  設置場所設定 */
		ret = eenl_SetLocation((EenlSetLocation_t*)msg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態を取得できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
		break;

	case EENL_SET_V2H_LOCATION:
		DLOG_Debug("===== EENL_SET_V2H_LOCATION is called =====");
		/* V2H設置場所設定 */
		ret = EENL_SetV2hLocation((EenlSetLocation_t*)msg);
		if (ret != 0) {
			/* 異常の場合 */
			/* 状態を取得できていないが、処理は続行 */
		} else {
			/* DO NOTHING */
		}
		break;

	default:
		/* DO NOTHING */
		break;
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:イベント通知処理										*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　msg	メッセージ									*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/24 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_MsgProcEvent(FrmMsgHead_t const* msg)
{
	EenlEvtSttsChangeReq_t *evtSttsChang = NULL;		/* 状態変化通知イベントメッセージ構造体 */
	EenlEvtSttingChangeReq_t *evtSttingChange = NULL;	/* 設定変更通知イベント構造体 */
	slong ret = 0;

	DEBUG_TRCIN();

	/* 起動条件の確認開始ステップ２(メッセージ受信) */
	if (EENL_STATUS_CONFIRM_START_2 == s_eenlStatusType) {
		if ((ENET_CPORT == msg->srcModId)
			|| (ENET_SPORT == msg->srcModId)) {
			/* 送信元判定(enet) */
			if (ENET_SYNC_SET_CHGEVNTLISTENER == msg->msgId) {
				eenl_ConfirmEnetEvent(msg);
			} else {
				/* 何もしない */
				DLOG_Debug("????? unknown enet msgId = %d ?????",
					msg->msgId);
			}
		} else if ((EINV_CPORT == msg->srcModId) 
			|| (EINV_SPORT == msg->srcModId)) {
			if (EENL_SET_TESTRUN_STATUS == msg->msgId) {
				/* 試運転状態イベント */
				eenl_ConfirmTestRunEvent(msg);
			} else if (EENL_EVT_STTS_CHANGE_REQ == msg->msgId) {
				/* 状態変化通知イベント */
				DLOG_Debug("===== EENL_EVT_STTS_CHANGE_REQ is called =====");

				evtSttsChang = (EenlEvtSttsChangeReq_t*)msg;

				DLOG_Debug("changedFlag : %08lx", 
					evtSttsChang->changedFlag);
				DLOG_Debug("changedProcessMsgId : %ld",
					evtSttsChang->header.srcModId);

				/* 運転モード変更通知 */
				if ((evtSttsChang->changedFlag 
					& EPCS_EVT_OPE_MODE) == EPCS_EVT_OPE_MODE) {
					DLOG_Debug("<opeMode>");

					/* 運転モード変更通知処理 */
					ret = EENL_EvtEpcsOpeMode(evtSttsChang);
					if (ret != 0) {
						/* 異常の場合 */
						/* 状態を更新できていないが、処理は続行 */
					} else {
						/* DO NOTHING */
					}
				} else {
					/* DO NOTHING */
				}
			} else {
				/* DO NOTHING */
			}
		} else {
			/* 何もしない */
			DLOG_Debug("????? unknown srcModId = %ld ?????", msg->srcModId);
		}

	} else if (EENL_STATUS_EXECUTE_START_2 == s_eenlStatusType) {
		/* 実行処理開始ステップ２(メッセージ受信) */
		if ((ENET_CPORT == msg->srcModId) || (ENET_SPORT == msg->srcModId)) {
			/* 送信元判定(enet) */
			if (ENET_SYNC_SET_CHGEVNTLISTENER == msg->msgId) {
				eenl_ExecuteEnetEvent(msg);
			} else {
				/* 何もしない */
				DLOG_Debug("????? unknown enet msgId = %d ?????", msg->msgId);
			}
		} else if ((EINV_CPORT == msg->srcModId)
			|| (EINV_SPORT == msg->srcModId)) {
			if (EENL_SET_TESTRUN_STATUS == msg->msgId) {
				/* 試運転状態イベント */
				eenl_ConfirmTestRunEvent(msg);
			} else if (EENL_EVT_STTS_CHANGE_REQ == msg->msgId) {
				/* 状態変化通知イベント */
				eenl_EvtSttsChangeReq((EenlEvtSttsChangeReq_t*)msg);
			} else if (EENL_EVT_SP_CHANGE_REQ == msg->msgId) {
				/* 設定変更通知イベント */
				eenl_EvtSpChangeReq((EenlEvtSpChangeReq_t*)msg);
			} else if (EENL_EVT_V2H_STTS_CHANGE_REQ == msg->msgId) {
				/* V2H状態変化イベント */
				eenl_EvtV2hSttsChangeReq((EenlEvtV2hSttsChangeReq_t*)msg);
			} else if (EENL_EVT_V2H_SETTING_CHANGE_REQ == msg->msgId) {
				/* V2H設定変更イベント */
				eenl_EvtV2hSettingChangeReq((EenlEvtV2hSttingChangeReq_t*)msg);
			} else {
				/* その他 */
				DLOG_Debug("????? unknown einv msgId = %d ?????", msg->msgId);
				/* 何もしない */
			}
		} else if ((ESCH_CPORT == msg->srcModId)
			|| (ESCH_SPORT == msg->srcModId)) {
			if (EENL_EVT_SETTING_CHANGE == msg->msgId) {
				/* 設定変更通知イベント（esch） */
				evtSttingChange = (EenlEvtSttingChangeReq_t*)msg;
				eenl_EschReq(evtSttingChange);
			} else {
				/* その他 */
				DLOG_Debug("????? unknown einv msgId = %d ?????", msg->msgId);
				/* 何もしない */
			}
		} else {
			/* 何もしない */
			DLOG_Debug("????? unknown srcModId = %ld ?????", msg->srcModId);
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:起動条件の確認時にEnetからイベント通知処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　msg	メッセージ									*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/26 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_ConfirmEnetEvent(FrmMsgHead_t const* msg)
{
	EnetFncListenerPort_t* nmsg = NULL;

	DEBUG_TRCIN();

	nmsg = (EnetFncListenerPort_t*)msg;
	DLOG_Debug("nmsg->flag : %ld", nmsg->flag);

	if (nmsg->flag == ENET_CHGEVNT_IP_ADDRESS) {
		/* IPアドレス変化時 */
		DLOG_Debug("<<<<< IP Address changed >>>>>");

		if (0 == s_eenlEnetChk1) {
			/* enetへのイベント登録直後に通知があるので、この場合は処理しない */
			s_eenlEnetChk1 = 1;
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	if (nmsg->flag == ENET_CHGEVNT_NETWORK_TYPE) {
		/* ネットワーク種別変化時 */

		DLOG_Debug("<<<<< Network Interface changed >>>>>");

		if (0 == s_eenlEnetChk2) {
			/* enetへのイベント登録直後に通知があるので、この場合は処理しない */
			s_eenlEnetChk2 = 1;
		} else {
			/* 2回目以降は、ネットワーク種別が変わっているため、現在のネットワークを取得 */
			g_eenlIfMode = ENET_SyncGetNetworkType();
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:起動条件の確認時に試運転イベント通知処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　msg	メッセージ									*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/26 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_ConfirmTestRunEvent(FrmMsgHead_t const* msg)
{
	EenlTestRun_t* nmsg = NULL;
	slong ret = 0;

	DEBUG_TRCIN();

	nmsg = (EenlTestRun_t*)msg;

	/* 試運転完了 */
	if (EENL_TESTRUN_FINISH == nmsg->state) {
		if (EENL_STATUS_CONFIRM_START_2 == s_eenlStatusType) {
			DLOG_Debug("<<<<< TestRun end >>>>>");
			s_eenlEvtidFinishTestRun = EENL_ON;
		} else if (EENL_STATUS_EXECUTE_START_2 == s_eenlStatusType) {
			DLOG_Debug("<<<<< TestRun end >>>>>");
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	ret = EENL_EvtEguiTestrunStatus(nmsg);
	if (ret != 0) {
		/* 異常の場合 */
		/* 状態を更新できていないが、処理は続行 */
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:実行処理開始の時にEnetからイベント通知処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　msg	メッセージ									*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/26 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_ExecuteEnetEvent(FrmMsgHead_t const* msg)
{
	EnetFncListenerPort_t *nmsg		= NULL;
	EenlDeviceInfo_t *info			= NULL;
	slong ret						= 0;
	sint ret2						= 0;
	ENL_MESSAGE *elnMsg				= NULL;

	DEBUG_TRCIN();

	/* IP変化通知イベント */
	DLOG_Debug("===================================================");
	DLOG_Debug("===== ENET_SYNC_SET_CHGEVNTLISTENER is called =====");
	DLOG_Debug("===================================================");

	nmsg = (EnetFncListenerPort_t*)msg;
	DLOG_Debug("nmsg->flag : %ld", nmsg->flag);

	if (nmsg->flag == ENET_CHGEVNT_IP_ADDRESS) {
		/* IPアドレス変化時 */
		DLOG_Debug("<<<<< IP Address changed >>>>>");

		if (0 == s_eenlEnetChk1) {
			/* enetへのイベント登録直後に通知があるので、この場合は処理しない */
			s_eenlEnetChk1 = 1;
		} else {
			/* マルチキャストに登録 */
			EENL_AddMulticast();
			sleep(4);

			/* 新しいIPアドレスになったのでノードプロファイル 0xD5の状変アナウンス通知を発行 */
			info = EENL_GetDeviceInfoIndex(0);
			if (info != NULL) {
				/* メッセージの生成 */
				ret = ENL_DEV_create_notification_single(&elnMsg,
					info->node, NULL, 0xD5, ret2);
				if (ret == ENL_RESULT_SUCCESS) {
					if (elnMsg != NULL) {
						/* メッセージの送信 */
						EENL_SendMessage(NULL, elnMsg);
						DLOG_Debug("#### send broadcast message (node:0xD5) ####");
					} else {
						DLOG_Error("notification message error");
					}
				} else {
					DLOG_Error("notification error : %ld", ret);
				}
			} else {
				DLOG_Error("notification error : null");
			}
		}
	} else {
		/* DO NOTHING */
	}

	if (nmsg->flag == ENET_CHGEVNT_NETWORK_TYPE) {
		/* ネットワーク種別変化時 */

		DLOG_Debug("<<<<< Network Interface changed >>>>>");

		if (0 == s_eenlEnetChk2) {
			/* enetへのイベント登録直後に通知があるので、この場合は処理しない */
			s_eenlEnetChk2 = 1;
		} else {
			/* ネットワーク種別が変化した際の処理をここに記述 */
			/* マルチキャストの登録 */
			/* IPを手動設定でネットワークが変わった場合を想定 */
			EENL_AddMulticast();
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:設置場所情報の取得処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　fmsg	メッセージ構造体							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(0:正常終了/-1:異常/-2:引数不正)						*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_GetLocation(EenlGetLocation_t* fmsg)
{
	slong ret = 0;
	EenlDeviceInfo_t* info = NULL;
	ulong loc = EENL_LOCATION_INSIDE;
	uchar wk = 0x00u;
	EenlGetLocationRet_t LocationRet;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (fmsg == NULL) {
		return -2;	/* 引数不正 */
	} else {
		/* DO NOTHING */
	}

	if (g_eenlCreate == EENL_CREATE_OK) {
		/* enl構築済の場合は、enlから情報を取得 */
		info = EENL_GetDeviceInfoIndex(0);
		if (info != NULL) {
			if (info->devBattery != NULL) {
				/* ENLライブラリから現在の情報を取得 */
				ret = ENL_get_property_value(info->devBattery, 0x81, &loc, 0);
				if (ret != ENL_RESULT_SUCCESS) {
					/* 失敗 */
					loc = EENL_LOCATION_INSIDE;
					DLOG_Debug("get 0x81 value : %ld, 0x%02lx", ret, loc);
				} else {
					/* 成功 */
					/* 取得した値をそのまま利用するので、何もしない */
				}
			} else {
				/* 蓄電池が未生成の場合、初期状態(室内)を返す */
				loc = EENL_LOCATION_INSIDE;
				DLOG_Debug("battery non. 0x81 value : 0x%02lx", loc);
			}
		} else {
			/* 蓄電池が見つからない場合、初期状態(室内)を返す */
			loc = EENL_LOCATION_INSIDE;
			DLOG_Debug("get 0x81 null : 0x%02lx", loc);
		}
	} else {
		/* enl未構築の場合は、ファイルから直接取得 */
		EENL_FileInit();
		EENL_FileRead();

		/* グローバル変数に保存された設置場所情報を利用 */
		loc = g_eenlBattery0x81;
		DLOG_Debug("get 0x81 file : 0x%02lx", loc);

	}

	/* 取得値の判定 */
	if (loc != EENL_LOCATION_OUTSIDE && loc != EENL_LOCATION_INSIDE) {
		/* 室内、室外以外の場合 */
		wk = loc & 0x00F8;	/* 上位5ビットを取得 */
		if (wk >= 0x08 && wk <= 0x58) {
			DLOG_Debug("change data : 0x%02lx -> 0x%02x", 
				loc, EENL_LOCATION_INSIDE);
			loc = EENL_LOCATION_INSIDE;
		} else {
			DLOG_Debug("change data : 0x%02lx -> 0x%02x", 
				loc, EENL_LOCATION_OUTSIDE);
			loc = EENL_LOCATION_OUTSIDE;
		}
	} else {
		/* 室内、室外の場合 */
		/* そのまま利用するので、何もしない */
	}

	/*  同期通信の応答を生成 */
	LocationRet.header.retCode = 0;
	LocationRet.location = loc;
	DLOG_Debug("LocationRet=0x%02x", LocationRet.location);

	/* 同期通信用リターンを返す */
	FRM_CallRet(fmsg->header.srcModId, sizeof(EenlGetLocationRet_t),
		(void*)&LocationRet);

	DEBUG_TRCOUT();
	return 0;
}

/*==========================================================================*/
/*	Description		:設置場所情報の設定処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　fmsg	メッセージ構造体							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(0:正常終了/-1:異常/-2:引数不正)						*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_SetLocation(EenlSetLocation_t* fmsg)
{
	slong ret = 0;
	uchar unitAddr = 0u;
	void* dev_list[EENL_DEV_LIST_MAX] = { 0 };
	EPCS_REG_INFO_T regInfo;

	DEBUG_TRCIN();

	/*  実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	memset(&regInfo, 0x00, sizeof(regInfo));

	/*  登録情報取得 */
	ret = epcs_sync_getRegInfo(unitAddr, &regInfo);
	if (ret != EPCS_OK) {
		DLOG_Error("getRegInfo error! : %ld", ret);
	} else {
		/* デバイスリストの作成(enl構築済みの場合に使用する) */
		EENL_SetBroadcastDevList(dev_list);

		/* ブロードキャストで設定しないデバイスにNULLを設定する */
		dev_list[EENL_DEV_LIST_NODE] = NULL;
		dev_list[EENL_DEV_LIST_V2H] = NULL;
	}

	DLOG_Debug(" location=0x%02x", fmsg->location);

	/* 設定値の判定 */
	switch (fmsg->location) {
	case EENL_LOCATION_INSIDE:		/* 室内 */
		/* breakなし */
	case EENL_LOCATION_OUTSIDE:		/* 室外 */
		/* 設定可能 */

		if (g_eenlCreate == EENL_CREATE_OK) {
			/* enl構築済みの場合 */
			DLOG_Debug("enl_create : ok");

			/*  蓄電池がある場合に設定する */
			/*  蓄電池がなくてもaguiからコールされるため、構成上蓄電池が存在するか確認する */
			if ((regInfo.sysConf & EPCS_SYS_CON_BT) == EPCS_SYS_CON_BT) {
				if (g_eenlBattery0x81 != fmsg->location) {
					EENL_BroadcastSetProperty(
						dev_list, EENL_DEV_LIST_MAX, 0x81u, 1u,
						(uchar*)(&fmsg->location), EENL_ANNOUNCE_ON);
					/* 失敗してもグローバル値を更新しておけば周期処理にて更新されるため、戻り値は参照しない */

					/* グローバル変数に変更情報を格納 */
					g_eenlBattery0x81 = fmsg->location;
					g_eenlSolar0x81 = fmsg->location;
					g_eenlMpcs0x81 = fmsg->location;
					/* 情報を保存 */
					EENL_FileWrite();

					DLOG_Debug("location change(N) call : 0x%02x", fmsg->location);
					if (fmsg->location == EENL_LOCATION_INSIDE) {
						/* 室内 ENSV_LOCATION_INSIDE */
						ensv_async_notify_LocationChanged(ENSV_LOCATION_INSIDE);
					} else {
						/* 室外 ENSV_LOCATION_OUTSIDE */
						ensv_async_notify_LocationChanged(ENSV_LOCATION_OUTSIDE);
					}
				} else {
					/* DO NOTHING */
				}
			} /*  batteryがあるなら */
			else {
				DLOG_Debug("battery not found");
				/* 処理は続行 */
			}
		} else {
			/* enl未構築の場合、ファイルに直接保存 */
			/* 蓄電池がなくてもaguiからコールされるが、設定される値はファイルから読み込んだ値のため変更されず問題なし */
			if (g_eenlBattery0x81 != fmsg->location) {
				DLOG_Debug("enl_create : file");
				EENL_FileInit();
				/* グローバル変数に設置場所を保存 */
				g_eenlBattery0x81 = fmsg->location;
				g_eenlSolar0x81 = fmsg->location;
				g_eenlMpcs0x81 = fmsg->location;
				EENL_FileWrite();

				/* 見守りサーバへの通知を行う */
				DLOG_Debug("location change(N) call : 0x%02x", fmsg->location);
				if (fmsg->location == EENL_LOCATION_INSIDE) {
					/* 室内 ENSV_LOCATION_INSIDE */
					ensv_async_notify_LocationChanged(ENSV_LOCATION_INSIDE);
				} else {
					/* 室外 ENSV_LOCATION_OUTSIDE */
					ensv_async_notify_LocationChanged(ENSV_LOCATION_OUTSIDE);
				}
			} else {
				/* DO NOTHING */
			}
		}

		break;
	default:						/* その他 */
		/* 設定不可 */
		DLOG_Error("location parameter error %02x", fmsg->location);
		/* 処理は続行 */
		break;
	}	/*  fmsg->location */

	/* 非同期で呼ばれるので、応答は返さない */

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:起動条件の確認処理前にの初期化処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_ConfirmInit(void)
{
	slong ret = 0;
	EenlConfirmStart_t msg;

	DEBUG_TRCIN();

	DLOG_Debug("===== eenl boot check =====");

	/* enl未構築 */
	g_eenlCreate = EENL_CREATE_NG;

	/* enetにIP変更/IF種別変更通知イベントの登録 */
	ret = ENET_SyncSetChgEvntListener(EENL_SPORT,
		ENET_CHGEVNT_IP_ADDRESS | ENET_CHGEVNT_NETWORK_TYPE);
	if (0 == ret) {
		/* 成功 */
		DLOG_Debug("*** ip change event set ok ***");

	} else {
		/* 失敗 */
		DLOG_Debug("*** ip change event set fail ***");
		/* 処理続行 */
	}

	/* 試運転終了 */
	g_eenlTestRunState = EENL_TESTRUN_STOP;

	/* 現在の使用ネットワークを取得 */
	g_eenlIfMode = ENET_SyncGetNetworkType();

	/* HEMS発電抑制履歴ファイル読み出し */
	memset(&g_eenlHemsHistory, 0x00, sizeof(g_eenlHemsHistory));
	ret = EENL_ReadHemsHistory(&g_eenlHemsHistory);
	DLOG_Debug("EENL_ReadHemsHistory called. ret=%ld", ret);

	/* ENL充放電制御ファイル読み出し */
	ret = EENL_CmnFileRead(EENL_OPECTRL_FILE,
		&g_eenlOpeCtrl, sizeof(g_eenlOpeCtrl));
	if (ret == 0) {
		DLOG_Debug("read enl operation control file = 0x%02x.",
			g_eenlOpeCtrl);
	} else {
		/* ENL充放電制御ファイルが読み出せない場合(通常起動時)はOFFとする */
		g_eenlOpeCtrl = EENL_OPECTRL_OFF;
		DLOG_Debug("can NOT read enl operation control file = 0x%02x.",
			g_eenlOpeCtrl);
	}

	/* 起動条件の確認開始 */
	s_eenlStatusType = EENL_STATUS_CONFIRM_START_1;

	/* 起動条件の確認開始のメッセージ送信 */
	msg.Header.msgId = EENL_CONFIRM_START;
	FRM_MsgSendAsync(EENL_SPORT, sizeof(msg), &msg);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:起動条件の確認処理の開始								*/
/*--------------------------------------------------------------------------*/
/*	param			:arg スレッド関数のパラメータ							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(NULL:スレッド終了)										*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/23 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_ConfirmStart(void)
{
	slong ret = 0;
	EenlExecuteStart_t ExecuteStartmsg;

	DEBUG_TRCIN();

	/* 起動条件の確認 */
	if ((s_eenlGetWlanStatus == EENL_OFF)
		|| (s_eenlEvtidFinishTestRun == EENL_OFF)) {

		if (g_eenlIfMode == ENET_NETWORK_WIRELESS) {
			/* 無線LANの場合、電波強度を取得 */
			ret = ENET_SyncGetWlanStatus();
			DLOG_Debug("##### enet check : %ld (%ld) #####", 
				ret, s_eenlGetWlanStatus);
			if (ret > EENL_LIMIT_0) {
				/* 電波強度が取得できている場合 */
				s_eenlGetWlanStatus = EENL_ON;
			} else {
				/* 何もしない */
			}

		} else if (g_eenlIfMode == ENET_NETWORK_NONE) {
			/* 使用しない */
			/* ネットワークが利用できない場合は、起動しない */
			DLOG_Debug("##### enet check : none #####");
			s_eenlGetWlanStatus = EENL_OFF;
		} else {
			/* 無線LAN以外は、電波強度の確認をスキップ */
			s_eenlGetWlanStatus = EENL_ON;
		}
		DLOG_Debug("##### enet check : %ld (%ld) #####",
			s_eenlConfirmRetryCnt, s_eenlGetWlanStatus);

		/* 起動条件の確認開始ステップ２ */
		s_eenlStatusType = EENL_STATUS_CONFIRM_START_2;
	} else {
		/* 実行処理開始ステップ１ */
		s_eenlStatusType = EENL_STATUS_EXECUTE_START_1;

		/* 実行処理開始のメッセージ送信 */
		ExecuteStartmsg.Header.msgId = EENL_EXECUTE_START;
		FRM_MsgSendAsync(EENL_SPORT,
			sizeof(ExecuteStartmsg), &ExecuteStartmsg);
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:起動条件の確認処理のメッセージ受信						*/
/*--------------------------------------------------------------------------*/
/*	param			:msg メッセージ											*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_ConfirmStartMsgProc(FrmMsgHead_t const* msg)
{
	EenlConfirmStart_t ConfirmStartmsg;

	/* 起動条件の確認 */
	if ((s_eenlGetWlanStatus == EENL_OFF)
		|| (s_eenlEvtidFinishTestRun == EENL_OFF)) {

		switch (msg->attr) {
		case FRM_CALL_SYNC:
			/* 同期メッセージ処理 */
			eenl_MsgProcSync(msg);
			break;
		case FRM_MSG_ASYNC:
			/* 非同期メッセージ処理 */
			eenl_MsgProcAsync(msg);
			break;
		case FRM_MSG_EVENT:
			/* イベント通知処理 */
			eenl_MsgProcEvent(msg);
			break;
		default:
			/* DO NOTHING */
			break;
		}

		s_eenlConfirmRetryCnt++;
		if (s_eenlConfirmRetryCnt > EENL_RETRY_COUNT) {
			DLOG_Debug("retry");
			s_eenlConfirmRetryCnt = 0u;
		} else {
			/* DO NOTHING */
		}

		/* 起動条件の確認開始 */
		s_eenlStatusType = EENL_STATUS_CONFIRM_START_1;

		/* 起動条件の確認開始のメッセージ送信 */
		ConfirmStartmsg.Header.msgId = EENL_CONFIRM_START;
		FRM_MsgSendAsync(EENL_SPORT,
			sizeof(ConfirmStartmsg), &ConfirmStartmsg);

	} else {
		/* 実行処理開始ステップ１ */
		s_eenlStatusType = EENL_STATUS_EXECUTE_START_1;

		/* 実行処理開始のメッセージ送信 */
		ConfirmStartmsg.Header.msgId = EENL_EXECUTE_START;
		FRM_MsgSendAsync(EENL_SPORT, 
			sizeof(ConfirmStartmsg), &ConfirmStartmsg);
	}
}

/*==========================================================================*/
/*	Description		:実行処理開始の初期化									*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_ExecuteInit(void)
{
	sint fd = -1;
	slong i = 0;
	slong ret = 0;
	uchar unitAddr = 1u;
	EenlDeviceInfo_t* info = NULL;
	struct ifreq ifr;
	EnetIpv4Settings_t v4adr;
	EPCS_REG_INFO_T regInfo;

	DEBUG_TRCIN();

	DLOG_Debug("===== eenl boot check2 =====");

	/* ----- IPv4 address ----- */
	memset(&v4adr, 0x00, sizeof(v4adr));

	DLOG_Debug("enet getIPAddr4");
	ret = ENET_SyncGetIpAddr4(&v4adr);
	if (0 == ret) {
		/* 正常 */
		DLOG_Debug("enet IPv4 addr : %d.%d.%d.%d", v4adr.ip.octet[0],
			v4adr.ip.octet[1], v4adr.ip.octet[2], v4adr.ip.octet[3]);
	} else {
		/* 失敗 */
		DLOG_Error("enet getIPv4 error : %ld", ret);
	}

	/* ----- IPv4 address ----- */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	/* IPv4 */
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);
	/* get IP address */
	ioctl(fd, SIOCGIFADDR, &ifr);
	DLOG_Debug("eth0 IPv4 addr : %s",
		inet_ntoa(((struct sockaddr_in*) & ifr.ifr_addr)->sin_addr));
	close(fd);

	/* ----- MACアドレス ----- */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	/* IPv4 */
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);
	/* get MAC address */
	ioctl(fd, SIOCGIFHWADDR, &ifr);

	/* MACアドレスをグローバル変数に格納 */
	for (i = 0;i < 6;i++) {
		g_eenlMacAddr[i] = (uchar)(ifr.ifr_hwaddr.sa_data[i]);
	}
	DLOG_Debug("mac addr : %02x-%02x-%02x-%02x-%02x-%02x", g_eenlMacAddr[0],
		g_eenlMacAddr[1], g_eenlMacAddr[2], g_eenlMacAddr[3], g_eenlMacAddr[4],
		g_eenlMacAddr[5]);

	close(fd);

	DLOG_Debug("===== eenl initialize =====");
	/* 設定ファイルの初期処理 */
	EENL_FileInit();
	/* 設定情報の読み込み */
	EENL_FileRead();

	DLOG_Debug("===== enl initialize =====");
	EENL_Initialize();

	/* (※einvの登録情報取得をupdate_epcs_battery()からここに移動) */
	memset(&regInfo, 0x00, sizeof(regInfo));
	/* PCSとの通信確認は不要(※ペアリング判定機能により必要な情報は取れているため) */

	/* 登録情報取得 */
	ret = epcs_sync_getRegInfo(unitAddr, &regInfo);
	if (ret != EPCS_OK) {
		DLOG_Error("getRegInfo error! : %ld", ret);
		/* ※ペアリング判定機能により、ここでエラーが発生することはあり得ない */
	} else {
		DLOG_Debug("  sysConf=0x%04x", regInfo.sysConf);
		DLOG_Debug("  pushedUpState='%c' 0x%02x",
			regInfo.pushedUpState, regInfo.pushedUpState);
		DLOG_Debug("  installationDate");
		DLOG_Debug("    year=\"%s\" strlen=%ld", 
			regInfo.installationDate.year,
			strlen(regInfo.installationDate.year));
		DLOG_Debug("    month=\"%s\" strlen=%ld", 
			regInfo.installationDate.month, 
			strlen(regInfo.installationDate.month));
		DLOG_Debug("    day=\"%s\" strlen=%ld", 
			regInfo.installationDate.day,
			strlen(regInfo.installationDate.day));
		DLOG_Debug("  unitAddr=%d", regInfo.unitAddr);
		DLOG_Debug("  pcsFwVersion=\"%s\" strlen=%ld", 
			regInfo.pcsFwVersion, strlen(regInfo.pcsFwVersion));
		DLOG_Debug("  btCapacity=%d", regInfo.btCapacity);
	}

	DLOG_Debug("===== enl create device =====");
	eenl_CreateDevice(regInfo);
	/* enl構築済 */
	g_eenlCreate = EENL_CREATE_OK;

	/* ENLデバイス情報を取得 */
	info = EENL_GetDeviceInfoIndex(0);

	/* einvデータの反映（蓄電池） */
	ret = EENL_UpdateEpcsBattery(info, regInfo);
	if (ret < 0) {
		DLOG_Error("einv battery update error!");
	} else {
		/* DO NOTHING */
	}

	/* einvデータの反映（太陽光） */
	ret = EENL_UpdateEpcsSolarNcc(info);
	if (ret < 0) {
		DLOG_Error("einv solar update error!");
		/* 反映に失敗しても処理続行 */
	} else {
		/* DO NOTHING */
	}

	/* eschデータの反映(太陽光) */
	ret = EENL_UpdateEschSolar(info);
	if (ret < 0) {
		DLOG_Error("esch solar update error!");
		/* 反映に失敗しても処理続行 */
	} else {
		/* DO NOTHING */
	}

	/* einvデータの反映（車両充放電器） */
	ret = EENL_UpdateEpcsV2h(info);
	if (ret < 0) {
		DLOG_Error("einv v2h update error!");
	} else {
		/* DO NOTHING */
	}

	/* einvデータの反映（マルチ入力PCS） */
	ret = EENL_UpdateEpcsMpcs(info);
	if (ret < 0) {
		DLOG_Error("einv mpcs update error!");
	} else {
		/* DO NOTHING */
	}

#if 0
	/* einvへのイベント登録 */
	ret = eenl_SetEpcsEvent(info);
	if (ret < 0) {
		DLOG_Error("set_epcs_event error!");
		/* 処理続行 */
	} else {
		/* DO NOTHING */
	}

	/* eschへのイベント登録 */
	eenl_SetEschEvent(info);
#endif
	/* 充放電関連EPCの設定値をeinvに反映 */
	ret = eenl_SetEpcsBattery(info);
	if (ret < 0) {
		DLOG_Error("set_epcs_battery error!");
		/* 処理続行 */
	} else {
		/* DO NOTHING */
	}

	/* 定期的に実施する処理のスレッドを開始 */
	ret = pthread_create(&s_eenlThTimer, 
		NULL, (void* (*)(void*))EENL_Timer, NULL);
	if (ret != 0) {
		DLOG_Error("timer thread create error!");
	} else {
		/* DO NOTHING */
	}

	/* プロセス再起動でなければ、HEMS発電抑制履歴ファイルの起動時書き込みを行う */
	if (g_eenlReboot != ESYS_START_FACT_RESET) {
		if (g_eenlHemsHistory.data_num == 0) {
			/* HEMS発電抑制履歴が0件＝発電制御値100％を新しい行に書き込み */
			ret = EENL_UpdateHemsHistory(100, EENL_NEW_DATA);

			DLOG_Debug("eenl_updateHemsHistory called. value=100, ret=%ld", 
				ret);
		} else {
			/* HEMS発電抑制履歴がある＝最後の発電制御値を新しい行に書き込み */
			ret = EENL_UpdateHemsHistory(g_eenlHemsHistory.data[0].value,
				EENL_NEW_DATA);

			DLOG_Debug("eenl_updateHemsHistory called. value=%d, ret=%ld",
				g_eenlHemsHistory.data[0].value, ret);
		}
	} else {
		/* DO NOTHING */
	}

	/* 実行処理開始ステップ２(メッセージ受信) */
	s_eenlStatusType = EENL_STATUS_EXECUTE_START_2;

	/* EINVへの起動完了通知 */
	ESYS_NotifyStartResult(EENL_SPORT, MOD_ACTION_RESULT_SUCCESS);
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:実行処理開始のメッセージ受信							*/
/*--------------------------------------------------------------------------*/
/*	param			:msg メッセージ											*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_ExecuteStartMsgProc(FrmMsgHead_t const* msg)
{
	slong threadRet = 0;

	DEBUG_TRCIN();

	switch (msg->attr) {
	case FRM_CALL_SYNC:
		/* 同期メッセージ処理 */
		eenl_MsgProcSync(msg);
		break;
	case FRM_MSG_ASYNC:
		/* 非同期メッセージ処理 */
		eenl_MsgProcAsync(msg);
		break;
	case FRM_MSG_EVENT:
		/* イベント通知処理 */
		eenl_MsgProcEvent(msg);
		break;
	default:
		/* DO NOTHING */
		break;
	}

	s_eenlThreadCheckCnt++;
	if (s_eenlThreadCheckCnt > EENL_THREAD_CHK) {
		threadRet = EENL_ThreadCheck();
		if (threadRet != 0) {
			DLOG_Debug("thread_check error : %ld", threadRet);
		} else {
			/* DO NOTHING */
		}
		s_eenlThreadCheckCnt = 0u;
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:書き込み要求受信（コールバック関数）					*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　instance	インスタンスハンドル					*/
/*					:[in]　epc プロパティコード								*/
/*					:[in]　pdc 要求値データ長								*/
/*					:[in]　edt 要求値データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*				(ENL_RESULT_ACCEPT:要求を受理する/							*/
/*				ENL_RESULT_COLLECT_AND_ACCEPT:値を補正した上で要求受理する/	*/
/*				ENL_RESULT_NOT_ACCEPT:要求を受理しない)						*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong ENL_DEV_callback_request_write(void* instance,
	uchar epc, uchar* pdc, uchar* edt)
{
	EenlDeviceInfo_t* info = NULL;
	slong ret = ENL_RESULT_NOT_ACCEPT;
	bool_t chkSetEnable = FALSE;

	DEBUG_TRCIN();

	chkSetEnable = EENL_ChkSetEnable(instance, epc);
	if (FALSE == chkSetEnable) {
		DLOG_Error("callback_request_write:NotReady!!(epc:%02X)",
			(uchar)epc);
		/*  初期値設定処理を通過していない場合は不可応答 */
	} else {
		info = EENL_GetDeviceInfoInstance(instance);

		if (info != NULL) {
			if (instance == info->devSolar) {
				/* 太陽光デバイスの場合 */

				/* 設定情報チェック（太陽光） */
				ret = EENL_SolarSetCheck(instance, epc, pdc, edt);
				if (ret == ENL_RESULT_ACCEPT) {
					EENL_EventSet(info, EENL_EVENT_SOLAR_WRITE, epc);
				} else {
					/* DO NOTHING */
				}

			} else if (instance == info->devBattery) {
				/* 蓄電池デバイスの場合 */

				/* 設定情報チェック（蓄電池） */
				ret = EENL_BatterySetCheck(instance, epc, pdc, edt);
				if ((ret == ENL_RESULT_ACCEPT) 
					|| (ret == EENL_RESULT_COLLECT_AND_ACCEPT_BAT0xF4)) {
					EENL_EventSet(info, EENL_EVENT_BATTERY_WRITE, epc);
				} else {
					/* DO NOTHING */
				}

				/*  EENL_RESULT_COLLECT_AND_ACCEPT_BAT0xF4なら戻り値をENL_RESULT_COLLECT_AND_ACCEPTにする。 */
				if (ret == EENL_RESULT_COLLECT_AND_ACCEPT_BAT0xF4) {
					ret = ENL_RESULT_COLLECT_AND_ACCEPT;
				} else {
					/* DO NOTHING */
				}

			} else if (instance == info->devV2h) {
				/* 車両充放電器デバイスの場合 */
				/* 設定情報チェック（車両充放電器） */
				ret = EENL_V2hSetCheck(instance, epc, pdc, edt);
				if (ret == ENL_RESULT_ACCEPT) {
					EENL_EventSet(info, EENL_EVENT_V2H_WRITE, epc);
				} else {
					/* DO NOTHING */
				}

			} else if (instance == info->devMpcs) {
				/* マルチ入力PCSデバイスの場合 */
				/* 設定情報チェック（マルチ入力PCS） */
				ret = EENL_MpcsSetCheck(instance, epc, pdc, edt);
				if (ret == ENL_RESULT_ACCEPT) {
					EENL_EventSet(info, EENL_EVENT_MPCS_WRITE, epc);
				} else {
					/* DO NOTHING */
				}

			} else {
				/* ノードの場合 */
				/* 設定情報チェック（ノード） */
				/* 無し */

			}
		} else {
			/* デバイスが未登録の場合 */
			ret = ENL_RESULT_NOT_ACCEPT;
		}

		/* 受け付けた場合はEPC設定済フラグをON */
		if ((ret == ENL_RESULT_ACCEPT) 
			|| (ret == ENL_RESULT_COLLECT_AND_ACCEPT)) {
			EENL_SetEpcValidFlg(instance, epc);
		} else {
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:読み出し要求/通知要求受信（コールバック関数）			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　instance	インスタンスハンドル					*/
/*					:[in]　epc プロパティコード								*/
/*					:[in]　pdc 候補値データ長								*/
/*					:[in]　edt 候補値データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*				(ENL_RESULT_ACCEPT:要求を受理する/							*/
/*				ENL_RESULT_COLLECT_AND_ACCEPT:値を補正した上で要求受理する/	*/
/*				ENL_RESULT_NOT_ACCEPT:要求を受理しない)						*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong ENL_DEV_callback_request_read(void* instance, 
	uchar epc, uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_ACCEPT;
	slong epcsRet = EPCS_OK;
	EenlDeviceInfo_t* info = NULL;
	ulong work = 0u;
	bool_t chkSetEnable = FALSE;
	EPCS_RESTRICTED_VALUE_T restrictedValue;


	DEBUG_TRCIN();
	EENL_UNUSED_PARA(pdc);	/* 未使用引数用 */

	chkSetEnable = EENL_ChkGetEnable(instance, epc);
	if (FALSE == chkSetEnable) {
		DLOG_Error("callback_request_read:NotReady!!(epc:%02X)", (uchar)epc);
		ret = ENL_RESULT_NOT_ACCEPT;	/*  値が設定されていない場合は不可応答 */
	} else {
		/* DO NOTHING */
	}

	info = EENL_GetDeviceInfoInstance(instance);

	if (info != NULL && !chkSetEnable) {
		if (instance == info->devSolar) {
			/* 太陽光デバイスの場合 */
			EENL_EventSet(info, EENL_EVENT_SOLAR_READ, epc);

			/* コールバック処理 */
			ret = EENL_SolarCallbackRequestRead(epc, edt);

		} else if (instance == info->devBattery) {
			/* 蓄電池デバイスの場合 */
			EENL_EventSet(info, EENL_EVENT_BATTERY_READ, epc);

			if ((epc == 0xEBu) || (epc == 0xECu)) {              /*  充電電力設定値(0xEB)または放電電力設定値(0xEC)のGet要求なら */
				/*  充放電電力設定値取得 */
				memset(&restrictedValue, 0, sizeof(EPCS_RESTRICTED_VALUE_T));
				epcsRet = epcs_sync_getRestrictedPwrValue(&restrictedValue);
				if (epcsRet == EPCS_OK) {
					/*  充電電力設定値(0xEB)なら */
					if (epc == 0xEBu) {
						 /*  蓄電池の場合は単位変換が必要。0.01KW -> 1W */
						work = (ulong)((ulong)restrictedValue.bt_chargeSetValue * (ulong)10u); 
						DLOG_Debug("ENL_DEV_callback_request_read.restrictedValue.bt_chargeSetValue=%d(0x%08X)/work=%ld(0x%08lX)",
							restrictedValue.bt_chargeSetValue,
							restrictedValue.bt_chargeSetValue, work, work);
					} else {
						/*  放電電力設定値(0xEC)なら */
						/*  蓄電池の場合は単位変換が必要。0.01KW -> 1W */
						work = (ulong)((ulong)restrictedValue.bt_dischargeSetValue * (ulong)10u); 
						DLOG_Debug("ENL_DEV_callback_request_read.restrictedValue.bt_dischargeSetValue=%d(0x%08X)/work=%ld(0x%08lX)",
							restrictedValue.bt_dischargeSetValue,
							restrictedValue.bt_dischargeSetValue, work, work);
					}

					work = (ulong)EENL_BYTE_SWAP_32(work);             /*  BYTE SWAP */
					/*  edtの値をeinvから取得した値に書き換えて、ENL_RESULT_COLLECT_AND_ACCEPTで返す */
					memcpy(edt, &work, sizeof(work));
					ret = ENL_RESULT_COLLECT_AND_ACCEPT;
					DLOG_Debug("ENL_DEV_callback_request_read battery 0x%02x(0x%08lX->0x%02X%02X%02X%02X) => RESULT COLLECT AND ACCEPT.",
						epc, work, *edt, *(edt + 1), *(edt + 2), *(edt + 3));
				} else {
					ret = ENL_RESULT_NOT_ACCEPT;
					DLOG_Debug("ENL_DEV_callback_request_read battery 0x%02x(0x%02X%02X%02X%02X) => einv ERROR! RESULT NOT ACCEPT.",
						epc, *edt, *(edt + 1), *(edt + 2), *(edt + 3));
				}
			} else {
				/* DO NOTHING */
			}
		} else if (instance == info->devV2h) {
			/* 車両充放電器デバイスの場合 */
			EENL_EventSet(info, EENL_EVENT_V2H_READ, epc);

			/*  コネクタロック解除中に0xC0,0xC2,0xC4,0xCE,0xCF,0xD0,0xE2,0xE4,0xE6のGet要求が来たら、不可応答を返す */
			if (g_eenlV2hConnectorLock == EPCS_V2H_CONNECTOR_UNLOCK) { 
				if ((epc == 0xC0u) || (epc == 0xC2u) || (epc == 0xC4u)
					|| (epc == 0xCEu) || (epc == 0xCFu) || (epc == 0xD0u)
					|| (epc == 0xE2u) || (epc == 0xE4u) || (epc == 0xE6u)) {
					DLOG_Debug("ENL_DEV_callback_request_read V2H clock=%d, epc=0x%02x => RESULT NOT ACCEPT.",
						g_eenlV2hConnectorLock, epc);
					ret = ENL_RESULT_NOT_ACCEPT;
				} else {
					/* DO NOTHING */
				}
			} else {
				/* DO NOTHING */
			}

			/*  放電電力設定値(0xEC)または放電電流設定値(0xEE)のGet要求なら */
			if ((epc == 0xECu) || (epc == 0xEEu)) {
				if (g_eenlPwrCut == EPCS_PWROUTAGE_YES) {
					/* 停電中は不可応答 */
					DLOG_Debug("ENL_DEV_callback_request_read v2h epc=0x%02X detect power outage! flag=%d",
						epc, g_eenlPwrCut);
					ret = ENL_RESULT_NOT_ACCEPT;
				} else {
					/* DO NOTHING */
				}
			} else {
				/* DO NOTHING */
			}

			if (ret == ENL_RESULT_ACCEPT) {
				/*  充電電力設定値(0xEB)または放電電力設定値(0xEC)のGet要求なら */
				if ((epc == 0xEBu) || (epc == 0xECu)) {
					/*  充放電電力設定値取得 */
					memset(&restrictedValue, 0, sizeof(EPCS_RESTRICTED_VALUE_T));
					epcsRet = epcs_sync_getRestrictedPwrValue(&restrictedValue);
					if (epcsRet == EPCS_OK) {
						if (epc == 0xEBu) {
							/*  充電電力設定値(0xEB)なら */
							/*  電気自動車充放電器の場合は単位変換不要。1W -> 1W */
							work = restrictedValue.v2h_chargeSetValue;
							DLOG_Debug("ENL_DEV_callback_request_read.restrictedValue.v2h_chargeSetValue=%ld(0x%08lX)/work=%ld(0x%08lX)",
								restrictedValue.v2h_chargeSetValue, 
								restrictedValue.v2h_chargeSetValue, work, work);
						} else {
							/*  放電電力設定値(0xEC)なら */
							work = restrictedValue.v2h_dischargeSetValue;
							/*  電気自動車充放電器の場合は単位変換不要。1W -> 1W */
							DLOG_Debug("ENL_DEV_callback_request_read.restrictedValue.v2h_dischargeSetValue=%ld(0x%08lX)/work=%ld(0x%08lX)", 
								restrictedValue.v2h_dischargeSetValue,
								restrictedValue.v2h_dischargeSetValue, work, work);
						}

						work = (ulong)EENL_BYTE_SWAP_32(work);         /*  BYTE SWAP */
						/*  edtの値をeinvから取得した値に書き換えて、ENL_RESULT_COLLECT_AND_ACCEPTで返す */
						memcpy(edt, &work, sizeof(work));
						ret = ENL_RESULT_COLLECT_AND_ACCEPT;
						DLOG_Debug("ENL_DEV_callback_request_read V2H 0x%02x(0x%08lX->0x%02X%02X%02X%02X) => RESULT COLLECT AND ACCEPT.",
							epc, work, *edt, *(edt + 1), *(edt + 2), *(edt + 3));
					} else {
						ret = ENL_RESULT_NOT_ACCEPT;
						DLOG_Debug("ENL_DEV_callback_request_read V2H 0x%02x(0x%02X%02X%02X%02X) => einv ERROR! RESULT NOT ACCEPT.",
							epc, *edt, *(edt + 1), *(edt + 2), *(edt + 3));
					}
				} else {
					/* DO NOTHING */
				}
			} else {
				/* DO NOTHING */
			}
		} else if (instance == info->devMpcs) {
			/* マルチ入力PCSデバイスの場合 */
			EENL_EventSet(info, EENL_EVENT_MPCS_READ, epc);

		} else if (instance == info->devMeterE1) {
		/* 分散型電源電力量メータクラス(E1)デバイスの場合 */
		EENL_EventSet(info, EENL_EVENT_E1METER_READ, epc);

		} else if (instance == info->devMeterC2) {
		/* 分散型電源電力量メータクラス(C2)デバイスの場合 */
		EENL_EventSet(info, EENL_EVENT_C2METER_READ, epc);

		} else {
			/* ノード */
			EENL_EventSet(info, EENL_EVENT_READ, epc);
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
	return ret;
}
