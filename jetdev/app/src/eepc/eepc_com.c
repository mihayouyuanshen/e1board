/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Xu.Y												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/07												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "dlog_g.h"
#include "eepc_file.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
static EepcFuncResult_t eepc_ComEvtComEnd(EepcEvt00_t *comResult, ulong reqId);
static EepcFuncResult_t eepc_ComEvtUpdtEndSrvMng(EepcUpdtSrv_t *updateSrvInf);
static EepcFuncResult_t eepc_ComFncClrThrInf(EepcThrType_t thrType);
static EepcFuncResult_t eepc_ComFncDivideEvtMsgEepc(FrmMsgHead_t const *msg);
static EepcFuncResult_t eepc_ComEvtResWdtg(EwdtEvtWdtReq_t *req);
static EepcFuncResult_t eepc_ComEvtResProcEnd(EwdtEvtTermReq_t *req);
static EepcFuncResult_t eepc_ComFncDivideEvtMsgEwdt(FrmMsgHead_t const *msg);
static EepcFuncResult_t eepc_ComFncRegEepcChgEvt(EepcRegChg_t *regChg);
static EepcFuncResult_t eepc_ComFncClrEepcChgEvt(EepcClrChg_t *clrChg);
static void *eepc_ComThrRcvSch(void *arg);
static void *eepc_ComThrRegId(void *arg);
static EepcFuncResult_t eepc_ComReqComStart(EepcComStart_t *comStart);
static void *eepc_ComThrUpdateSrvinf(void *arg);
static EepcFuncResult_t eepc_ComFncUpdateSrvinf(EepcUpdtSrv_t *updateSrvInf);
static EepcFuncResult_t eepc_ComFncPrcSrvCom(EepcSrvComInf_t *srvComInf, EepcEvt00_t *comResult);
static EepcFuncResult_t eepc_ComFncCancelSrvInf(EepcUpdtSrv_t *updateSrvInf);

/*==========================================================================*/
/*	Description		:	EEPC起動時処理										*/
/*--------------------------------------------------------------------------*/
/*	param			:	なし												*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	EEPC起動時処理										*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/10 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcFuncResult_t EEPC_ComInitProc(void)
{
	EepcFuncResult_t	result	= EEPC_FUNC_RESULT_OK;	/* 処理結果					*/
	sint				dirRet	= 0;					/* ディレクトリチェック結果	*/
	ulong				loopCnt	= 0u;					/* ループカウンタ			*/
	struct stat			statInfo;

	DEBUG_TRCIN();

	memset(&statInfo, 0, sizeof(struct stat));

	/* グローバル変数を初期化	*/
	/* 終了要求種別	*/
	g_eepcReqProcEndType = EWDT_TERM_TYPE_NONE;

	/* 使用中電力会社のIndex値	*/
	g_eepcUseSrvIndex = 0u;

	/* 設定変更通知イベントの登録数	*/
	g_eepcChgEvtRegCnt = 0u;

	/* 電力会社情報更新用構造体	*/
	memset(&g_eepcUpdtSrv, 0, sizeof(EepcUpdtSrv_t));

	/* 通信先情報用構造体	*/
	memset(&g_eepcComInf, 0, sizeof(EepcComInf_t));

	/* 電力会社サーバ管理ファイル用構造体	*/
	memset(&g_eepcFileSrvMng, 0, sizeof(EepcFileSrvMng_t));

	/* リトライ情報の初期値を設定する	*/
	/* 固定スケジュール受信のリトライ回数	*/
	g_eepcFileSrvMng.srvRtryCntFixSch = EEPC_RTRY_CNT_FIX_SCH;

	/* 固定スケジュール受信のリトライ間隔	*/
	g_eepcFileSrvMng.srvRtryIntvlFixSch = EEPC_RTRY_INTVL_FIX_SCH;

	/* 更新スケジュール受信のリトライ回数	*/
	g_eepcFileSrvMng.srvRtryCntUpdtSch = EEPC_RTRY_CNT_UPDT_SCH;

	/* 更新スケジュール受信のリトライ間隔	*/
	g_eepcFileSrvMng.srvRtryIntvlUpdtSch = EEPC_RTRY_INTVL_UPDT_SCH;

	/* 時刻同期のリトライ回数	*/
	g_eepcFileSrvMng.ntpRtryCntTimSync = EEPC_RTRY_CNT_TIM_SYNC;

	/* 時刻同期のリトライ間隔	*/
	g_eepcFileSrvMng.ntpRtryIntvlTimSync = EEPC_RTRY_INTVL_TIM_SYNC;

	/* 時刻同期の再リトライ間隔	*/
	g_eepcFileSrvMng.ntpRtryOvrIntvlTimSync = EEPC_RTRY_OVR_INTVL_TIM_SYNC;

	/* 電力会社サーバ選択ファイル用構造体	*/
	memset(&g_eepcFileUseSrv, 0, sizeof(EepcFileUseSrv_t));

	/* 電力会社サーバ通信履歴ファイル用構造体	*/
	memset(&g_eepcFileComHist, 0, sizeof(EepcFileComHist_t));

	/* 電力会社サーバ設定済みファイル用構造体	*/
	memset(&g_eepcFileSrvFixed, 0, sizeof(EepcFileSrvFixed_t));

	/* サーバ通信情報用構造体(スケジュール受信)	*/
	memset(&g_eepcSrvComInfSch, 0, sizeof(EepcSrvComInf_t));

	/* サーバ通信情報用構造体(スケジュール受信)	*/
	memset(&g_eepcSrvComInfRegId, 0, sizeof(EepcSrvComInf_t));

	/* 設定変更通知イベント情報ファイル用の配列	*/
	for (loopCnt = 0u; loopCnt < PROCESS_MAX; loopCnt++)
	{
		memset(&g_eepcFileEvt01[loopCnt], 0, sizeof(EepcFileEvt01_t));
	}

	/* サブスレッドの動作中フラグの配列	*/
	for(loopCnt = 0u; loopCnt < EEPC_THR_TYPE_MAX; loopCnt++)
	{
		g_eepcThrExecFlg[loopCnt] = 0;
	}

	/* サブスレッドID保持用の配列	*/
	for(loopCnt = 0u; loopCnt < EEPC_THR_TYPE_MAX; loopCnt++)
	{
		memset(&g_eepcThrId[loopCnt], 0, sizeof(FrmPthread_t));
	}
	
	/* サブスレッド毎の死活監視カウンタ */
	for(loopCnt = 0u; loopCnt < EEPC_THR_TYPE_MAX; loopCnt++)
	{
		g_eepcThrWdtCnt[loopCnt] = 0;
	}

	/* mutex変数の初期化 */
	/* 電力会社サーバ管理ファイル用構造体用排他mutex */
	g_eepcMutexFileSrvMng = FRM_MutexCreate(EEPC_SPORT);

	/* 電力会社サーバ通信履歴ファイル用構造体用排他mutex */
	g_eepcMutexFileComHist = FRM_MutexCreate(EEPC_SPORT);

	/* サーバ通信用排他mutex */
	g_eepcMutexComSrv = FRM_MutexCreate(EEPC_SPORT);

	/* リモコン内のディレクトリチェック */
	/* 不揮発ディレクトリが存在しない場合 */
	dirRet = stat(EEPC_USR_NAND_DIR_PATH, &statInfo);
	if (dirRet)
	{
		/* 不揮発ディレクトリ作成 */
		dirRet = mkdir(EEPC_USR_NAND_DIR_PATH, 0755);

		if (!dirRet)
		{
			DLOG_Info( "mkdir success. path=%s", EEPC_USR_NAND_DIR_PATH);
		}
		else
		{
			DLOG_Error( "mkdir error. path=%s", EEPC_USR_NAND_DIR_PATH);
		}
	}
	else
	{
		/* DO NOTHING */
	}

	/* ルート証明書ファイル格納先ディレクトリ(Bank0)生成 */
	dirRet = mkdir(EEPC_RTCER_BNK0_NAND_DIR_PATH, 0755);
	if ((!dirRet) || (EEXIST == errno))
	{
		DLOG_Info("mkdir success. path=%s", EEPC_RTCER_BNK0_NAND_DIR_PATH);
	}
	else
	{
		DLOG_Error("mkdir error. path=%s", EEPC_RTCER_BNK0_NAND_DIR_PATH);
	}

	/* ルート証明書ファイル格納先ディレクトリ(Bank1)生成 */
	dirRet = mkdir(EEPC_RTCER_BNK1_NAND_DIR_PATH, 0755);
	if ((!dirRet) || (EEXIST == errno))
	{
		DLOG_Info("mkdir success. path=%s", EEPC_RTCER_BNK1_NAND_DIR_PATH);
	}
	else
	{
		DLOG_Error("mkdir error. path=%s", EEPC_RTCER_BNK1_NAND_DIR_PATH);
	}

	/* 揮発ディレクトリが存在しない場合	*/
	dirRet = stat(EEPC_RAM_DIR_PATH, &statInfo);
	if (dirRet)
	{
		/* 揮発ディレクトリ作成	*/
		dirRet = mkdir(EEPC_RAM_DIR_PATH, 0755);
		if (!dirRet)
		{
			DLOG_Info( "mkdir success. path=%s", EEPC_RAM_DIR_PATH);
		}
		else
		{
			DLOG_Error( "mkdir error. path=%s", EEPC_RAM_DIR_PATH);
		}
	}
	else
	{
		/* DO NOTHING */
	}

	/* テストサーバ接続設定読み込みとユーザー領域パス設定 */
	result = EEPC_FileReadTestSrvConnect();
	if (EEPC_FUNC_RESULT_OK == result)
	{
		/* ファイルチェック処理	*/
		result = EEPC_FileFncChkAllFiles();
	}
	else
	{
		DLOG_Error("EEPC_FileReadTestSrvConnect() error.n");
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	非同期メッセージ振分処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:	FrmMsgHead_t const *msg		受信パラメータ			*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK			処理成功				*/
/*					:	EEPC_FUNC_RESULT_NG			処理失敗				*/
/*					:	EEPC_FUNC_RESULT_EPRM		引数異常				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	非同期メッセージ振分処理							*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/10 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcFuncResult_t EEPC_ComDivideAsyncMsg(FrmMsgHead_t const *msg)
{
	EepcFuncResult_t result = EEPC_FUNC_RESULT_OK;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == msg)
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;		/* 引数異常 */
	}
	else
	{
		/* ファンクションIDチェック */
		switch (msg->msgId)
		{
		/* 通信開始要求	*/
		case EEPC_FNC_ID_ASYNC_REQ_COM_START:
			DLOG_Debug("recv req com start");

			/* 通信開始要求処理	*/
			result = eepc_ComReqComStart((EepcComStart_t *)msg);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Error("eepc_fnc_reqComStart err %d", result);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
			break;
		/* 電力会社情報更新	*/
		case EEPC_FNC_ID_ASYNC_UPDATE_SRV_INF:
			DLOG_Debug("recv updt srv inf");

			/* 電力会社情報更新処理	*/
			result = eepc_ComFncUpdateSrvinf((EepcUpdtSrv_t *)msg);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Error("eepc_fnc_reqComStart err %d", result);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
			break;
		/* 電力会社情報更新中断	*/
		case EEPC_FNC_ID_ASYNC_CANCEL_SRV_INF:
			DLOG_Debug("recv cancel srv inf");

			/* 電力会社情報更新中断処理 */
			result = eepc_ComFncCancelSrvInf((EepcUpdtSrv_t *)msg);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Error("eepc_ComFncCancelSrvInf err %d", result);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
			break;
		default:
			DLOG_Error("msgId err %d", msg->msgId);
			result = EEPC_FUNC_RESULT_NG;
			break;
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	イベントメッセージ振分処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:	FrmMsgHead_t const *msg		受信パラメータ			*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK			処理成功				*/
/*					:	EEPC_FUNC_RESULT_NG			処理失敗				*/
/*					:	EEPC_FUNC_RESULT_EPRM		引数異常				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	イベントメッセージ振分処理							*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/10 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcFuncResult_t EEPC_ComDivideEventMsg(FrmMsgHead_t const *msg)
{
	EepcFuncResult_t result = EEPC_FUNC_RESULT_OK;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == msg)
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;		/* 引数異常 */
	}
	else
	{
		/* ポート番号チェック */
		switch (msg->destModId)
		{
		case EWDT_SPORT:
			result = eepc_ComFncDivideEvtMsgEwdt(msg);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Error("eepc_ComFncDivideEvtMsgEwdt err %d", result);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
			break;
		/* eepc内部のイベント通知 */
		case EEPC_SPORT:
			result = eepc_ComFncDivideEvtMsgEepc(msg);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Error("eepc_ComFncDivideEvtMsgEepc err %d", result);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
			break;
		/* 無効なポート番号 */
		default:
			DLOG_Error("DestModId err %ld", msg->destModId);
			result = EEPC_FUNC_RESULT_NG;
			break;
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	同期メッセージ振分処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:	FrmMsgHead_t const *msg		受信パラメータ			*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK			処理成功				*/
/*					:	EEPC_FUNC_RESULT_NG			処理失敗				*/
/*					:	EEPC_FUNC_RESULT_EPRM		引数異常				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	同期メッセージ振分処理								*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/10 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcFuncResult_t EEPC_ComDivideSyncMsg(FrmMsgHead_t const *msg)
{
	EepcFuncResult_t result = EEPC_FUNC_RESULT_OK;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == msg)
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;	/* 引数異常 */
	}
	else
	{
		/* ファンクションIDチェック */
		switch (msg->msgId)
		{
		/* 電力会社情報保存 */
		case EEPC_FNC_ID_SYNC_SET_SRV_INF:
			DLOG_Debug("recv set usr srv inf");

			/* 電力会社情報保存処理 */
			result = EEPC_FileSetUsrSrvInf((EepcSetSrv_t *)msg);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Error("EEPC_FileSetUsrSrvInf err %d", result);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
			break;
		/* 設定変更通知イベント登録 */
		case EEPC_FNC_ID_SYNC_REG_EEPC_CHG_EVT:
			DLOG_Debug("recv reg chg evt");

			/* 設定変更通知イベント登録処理 */
			result = eepc_ComFncRegEepcChgEvt((EepcRegChg_t *)msg);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Error("EEPC_FileSetUsrSrvInf err %d", result);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
			
			break;
		/* 設定変更通知イベント登録解除	*/
		case EEPC_FNC_ID_SYNC_CLR_EEPC_CHG_EVT:
		/* 設定変更通知イベント登録解除には非対応の為、実装を無効化	*/
#if 0
			DLOG_Debug("recv clr chg evt");
			/* 設定変更通知イベント登録解除処理	*/
			result = eepc_ComFncClrEepcChgEvt((EepcClrChg_t *)msg);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
#endif
			break;
		/* 無効なファンクションID */
		default:
			DLOG_Error("msgId err %d", msg->msgId);
			result = EEPC_FUNC_RESULT_NG;	/* 処理失敗 */
			break;
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:	通信終了応答処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcEvt00_t *comResult		通信結果情報の格納先	*/
/*					:	ulong reqId					返送先ID				*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	通信終了応答処理									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/10 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_ComEvtComEnd(EepcEvt00_t *comResult, ulong reqId)
{
	EepcFuncResult_t	result	= EEPC_FUNC_RESULT_OK;	/* 処理結果	*/
	ulong 				idMax	= 0u;

	DEBUG_TRCIN();

	/* パラメータチェック	*/
	idMax = LFW_SPORT_MIN + PROCESS_MAX - EEPC_PROCESS_LEN;
	if ((NULL == comResult) || (!((LFW_SPORT_MIN <= reqId) && (reqId <= idMax))))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* メッセージ生成	*/
		comResult->header.msgId		= EEPC_EVT_ID_RES_SRV_COM;

		DLOG_Debug("send com end. comResult:%d, seqID:%d, resSts:%ld, fileNameStr:%s",
			comResult->comResult,
			comResult->seqID,
			comResult->resSts,
			&(comResult->fileNameStr[0]));

		/* イベントメッセージ送信	*/
		slong tmpEvtRet = FRM_MsgSendEvent(reqId, sizeof(EepcEvt00_t), comResult);
		if (tmpEvtRet)
		{
			DLOG_Error("FRM_MsgSendEvent() err:%ld", tmpEvtRet);
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社情報更新終了応答処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcUpdtSrv_t *updateSrvInf		更新用の格納先		*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社情報更新終了応答処理						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/10 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_ComEvtUpdtEndSrvMng(EepcUpdtSrv_t *updateSrvInf)
{
	EepcFuncResult_t	result	= EEPC_FUNC_RESULT_OK;
	slong				callEvt	= 0;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == updateSrvInf)
	{
		DLOG_Debug("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* メッセージ生成 */
		updateSrvInf->header.msgId = EEPC_EVT_ID_SRV_INF_UPDATE;
		DLOG_Debug("send updt end");

		/* イベントメッセージ送信 */
		callEvt = FRM_MsgSendEvent(updateSrvInf->reqID,
			sizeof(EepcUpdtSrv_t), (void *)updateSrvInf);
		if (callEvt)
		{
			/* プロセス間通信失敗 */
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* DO NOTHING */
		}
		
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	サブスレッド情報クリア処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcThrType_t thrType	スレッド種別				*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	サブスレッド情報クリア処理							*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/10 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_ComFncClrThrInf(EepcThrType_t thrType)
{
	EepcFuncResult_t result = EEPC_FUNC_RESULT_OK;	/* 処理結果 */

	DEBUG_TRCIN();

	/* パラメータチェック	*/
	if (EEPC_THR_TYPE_MAX <= thrType)
	{
		DLOG_Debug("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* 対応するサブスレッドのIDのクリア	*/
		memset(&g_eepcThrId[thrType], 0, sizeof(FrmPthread_t));
		
		/* 対応するサブスレッド動作フラグのクリア	*/
		g_eepcThrExecFlg[thrType] = FALSE;
		
		/* 対応するサブスレッド死活監視カウンタのクリア	*/
		g_eepcThrWdtCnt[thrType] = 0u;
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	イベントメッセージ振分処理(eepcプロセス)			*/
/*--------------------------------------------------------------------------*/
/*	param			:	FrmMsgHead_t *msg		受信パラメータ				*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	イベントメッセージ振分処理(eepcプロセス)			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/10 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_ComFncDivideEvtMsgEepc(FrmMsgHead_t const *msg)
{
	EepcFuncResult_t	result		= EEPC_FUNC_RESULT_OK;		/* 処理結果					*/
	EepcFuncResult_t	resResult	= EEPC_FUNC_RESULT_OK;		/* 応答処理結果				*/
	slong				wdtResult	= 0;						/* EWDT応答処理結果			*/
	EepcSetSrv_t		setSrv;									/* 電力会社情報保存用構造体	*/
	bool_t				chgFlg		= FALSE;

	DEBUG_TRCIN();

	memset(&setSrv, 0, sizeof(EepcSetSrv_t));
	/* パラメータチェック */
	if (NULL == msg)
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;	/* 引数異常 */
	}
	else
	{
		switch (msg->msgId)
		{
		/* 電力会社情報更新スレッド終了 */
		case EEPC_LCL_EVT_ID_UPDATE_SRV_INF:
			/* 更新スレッドの結果を確認 */
			if (EEPC_FUNC_RESULT_NG == msg->retCode)
			{
				/* 電力会社サーバ選択ファイル詳細チェック処理 */
				resResult = EEPC_FileDetailChkUseSrvFile(&g_eepcFileUseSrv, &chgFlg);
				/* 電力会社情報更新の結果、選択中の電力会社が使用不可になる場合 */
				if (EEPC_FUNC_RESULT_OK != resResult)
				{
					/* 電力会社情報保存処理内でメッセージ送信を抑止する為、	*/
					/* 返信先メッセージIDにはEEPC_CPORTを設定				*/
					setSrv.header.srcModId = EEPC_CPORT;

					/* 更新後の電力会社情報のリスト先頭で、				*/
					/* 選択中の電力会社の情報を上書きする為、0を設定	*/
					setSrv.useSrvIndex = 0u;
					resResult = EEPC_FileSetUsrSrvInf(&setSrv);
					if (EEPC_FUNC_RESULT_OK != resResult)
					{
						result = EEPC_FUNC_RESULT_NG;
					}
					else
					{
						/* DO NOTHING */
					}
				}
				/* 選択中の電力会社が存在するが、情報に変化があった場合は更新 */
				else if (TRUE == chgFlg)
				{
					setSrv.header.srcModId = EEPC_CPORT;
					/* 見つかったものを指定 */
					setSrv.useSrvIndex = g_eepcUseSrvIndex;
					resResult = EEPC_FileSetUsrSrvInf(&setSrv);
					if (EEPC_FUNC_RESULT_OK != resResult)
					{
						result = EEPC_FUNC_RESULT_NG;
					}
					else
					{
						/* DO NOTHING */
					}
				}
				else
				{
					/* DO NOTHING */
				}
			}
			else
			{
				/* DO NOTHING */
			}

			DLOG_Debug("Update_corpDiv %d", g_eepcFileUseSrv.srvInf.corpDiv);
			DLOG_Debug("Update_srvPort %d", g_eepcFileUseSrv.srvInf.srvPort);
			DLOG_Debug("Update_ntpSrvPort %d", g_eepcFileUseSrv.srvInf.ntpSrvPort);
			DLOG_Debug("Update_srvNameStr %s", g_eepcFileUseSrv.srvInf.srvNameStr);
			DLOG_Debug("Update_srvUrlStr %s", g_eepcFileUseSrv.srvInf.srvUrlStr);
			DLOG_Debug("Update_ntpFqdnStr %s", g_eepcFileUseSrv.srvInf.ntpFqdnStr);
			DLOG_Debug("Update_rtCerFileStr %s", g_eepcFileUseSrv.srvInf.rtCerFileStr);
			
			/* 電力会社情報更新終了応答処理	*/
			resResult = eepc_ComEvtUpdtEndSrvMng((EepcUpdtSrv_t *)msg);
			if (EEPC_FUNC_RESULT_OK != resResult)
			{
				DLOG_Debug("eepc_ComEvtUpdtEndSrvMng err %d",resResult);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}

			resResult = eepc_ComFncClrThrInf(EEPC_THR_TYPE_ID_REG);
			if (EEPC_FUNC_RESULT_OK != resResult)
			{
				DLOG_Error("eepc_ComFncClrThrInf err %d", resResult);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
			break;
		/* スケジュール受信スレッド終了 */
		case EEPC_LCL_EVT_ID_RCV_SCH:
		/* ID登録確認スレッド終了 */
		case EEPC_LCL_EVT_ID_ID_REG:
			/* 通信終了応答処理	*/
			resResult = eepc_ComEvtComEnd((EepcEvt00_t *)msg,
				g_eepcSrvComInfRegId.comStart.reqID);
			if (EEPC_FUNC_RESULT_OK != resResult)
			{
				DLOG_Error("eepc_ComEvtComEnd err%d", resResult);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}

			resResult = eepc_ComFncClrThrInf(msg->msgId);
			if (EEPC_FUNC_RESULT_OK != resResult)
			{
				DLOG_Error("eepc_ComFncClrThrInf err %d", resResult);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
			break;
		default:
			DLOG_Error("msgId err:%d", msg->msgId);
			result = EEPC_FUNC_RESULT_NG;
			break;
		}

		if (EWDT_TERM_TYPE_NONE != g_eepcReqProcEndType)
		{
			/* 全てのスレッドが終了した場合	*/
			if ((TRUE != g_eepcThrExecFlg[EEPC_THR_TYPE_UPDATE_SRV_INF]) &&
				(TRUE != g_eepcThrExecFlg[EEPC_THR_TYPE_RCV_SCH]) &&
				(TRUE != g_eepcThrExecFlg[EEPC_THR_TYPE_ID_REG]))
			{
				DLOG_Debug("call ewdt_async_sendTermRes() sub thread end");
				/* 終了応答通知処理を実行			*/
				wdtResult = EWDT_AsyncSendTermRes(g_eepcReqProcEndType);
				/* 終了応答通知処理結果チェック	*/
				if (wdtResult)
				{
					result = EEPC_FUNC_RESULT_NG;
				}
				else
				{
					/* DO NOTHING */
				}
			}
			else
			{
				/* DO NOTHING */
			}
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	WATCHDOG応答処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:	EwdtEvtWdtReq_t *req	リクエストメッセージ		*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	WATCHDOG応答処理									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/11 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_ComEvtResWdtg(EwdtEvtWdtReq_t *req)
{
	EepcFuncResult_t	result		= EEPC_FUNC_RESULT_OK;	/* 処理結果					*/
	EepcFuncResult_t	eepcRet		= EEPC_FUNC_RESULT_OK;	/* eepc処理結果				*/
	slong				ewdtRet		= 0;					/* ewdt処理結果				*/
	ulong				loopCnt		= 0u;					/* ループカウンタ			*/
	EepcUpdtSrv_t		updateSrvInf;						/* 電力会社情報更新用構造体	*/
	EepcEvt00_t			comEnd;								/* 通信終了応答用構造体		*/

	DEBUG_TRCIN();
	
	memset(&updateSrvInf, 0, sizeof(EepcUpdtSrv_t));
	memset(&comEnd, 0, sizeof(EepcEvt00_t));

	/* パラメータチェック */
	if (NULL == req)
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* スレッド死活監視ループ */
		for (loopCnt = 0u; loopCnt < EEPC_THR_TYPE_MAX; loopCnt++)
		{
			/* スレッド死活監視	*/
			if (TRUE == g_eepcThrExecFlg[loopCnt])
			{
				FRM_PthreadCancel(g_eepcThrId[loopCnt]);

				/* スレッド死活監視カウンタチェック */
				if (EWDT_PROC_TIMEOUT_DEF <= g_eepcThrWdtCnt[loopCnt])
				{
					/* スレッド動作中フラグをOFF */
					g_eepcThrExecFlg[loopCnt] = FALSE;
					DLOG_Debug("g_eepcThrWdtCnt[%ld] = %d", loopCnt,
								g_eepcThrWdtCnt[loopCnt]);

					switch (loopCnt)
					{
					/* 電力会社情報更新スレッドが動作中の場合 */
					case EEPC_THR_TYPE_UPDATE_SRV_INF:
						/* 電力会社情報更新終了応答処理を設定する	*/
						updateSrvInf.reqID = g_eepcUpdtSrv.reqID;
						updateSrvInf.seqID = g_eepcUpdtSrv.seqID;

						/* 電力会社情報更新終了応答処理を代理で実行	*/
						eepcRet = eepc_ComEvtUpdtEndSrvMng(&updateSrvInf);
						break;
					/* スケジュール受信スレッドが動作中の場合 */
					case EEPC_THR_TYPE_RCV_SCH:
						/* 通信終了応答処理用のメッセージを生成する	*/
						comEnd.comResult = (schar)EEPC_RTN_CODE_ESYS;

						/* 通信終了応答処理を代理で実行	*/
						eepcRet = eepc_ComEvtComEnd(&comEnd,
							g_eepcSrvComInfSch.comStart.reqID);
						break;
					/* ID登録確認スレッドが動作中の場合 */
					case EEPC_THR_TYPE_ID_REG:
						/* 通信終了応答処理用のメッセージを生成する	*/
						comEnd.comResult = (schar)EEPC_RTN_CODE_ESYS;

						/* 通信終了応答処理を代理で実行	*/
						eepcRet = eepc_ComEvtComEnd(&comEnd,
							g_eepcSrvComInfRegId.comStart.reqID);
						break;
					/* 境界値超過	*/
					default:
						eepcRet = EEPC_FUNC_RESULT_NG;
						break;
					}
				}
				else
				{
					/* DO NOTHING */
				}
			}
			else
			{
				/* DO NOTHING */
			}
		}

		DLOG_Debug("call EWDT_AsyncSendWdtRes()");

		/* WDT要求イベント応答通知 */
		ewdtRet = EWDT_AsyncSendWdtRes(EWDT_PROC_TIMEOUT_DEF);
		/* ewdtとeepcの処理結果チェック	*/
		if ((ewdtRet) || (EEPC_FUNC_RESULT_OK != eepcRet))
		{
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	プロセス終了応答処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:	EwdtEvtTermReq_t *req	リクエストメッセージ		*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	プロセス終了応答処理								*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/11 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_ComEvtResProcEnd(EwdtEvtTermReq_t *req)
{
	EepcFuncResult_t	result 		= EEPC_FUNC_RESULT_OK;	/* 処理結果				*/
	slong				ewdtRet		= 0;					/* ewdt処理結果			*/
	ulong				loopCnt		= 0u;					/* ループカウンタ		*/
	bool_t				threacFlg	= FALSE;				/* スレッド動作中フラグ	*/
	sint				ret			= 0;					/* systemコマンド戻り値 */
	schar				cmd[EEPC_TMP_BUF_SIZE];				/* systemコマンド文字列バッファ */

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == req)
	{
		DLOG_Debug("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* 終了要求種別を保持 */
		g_eepcReqProcEndType = req->Type;

		/* スレッド死活監視ループ */
		for (loopCnt = 0u; loopCnt < EEPC_THR_TYPE_MAX; loopCnt++ )
		{
			/* 死活監視結果チェック */
			if(g_eepcThrExecFlg[loopCnt] == TRUE)
			{
				/* スレッド動作中フラグ：ON */
				threacFlg = TRUE;
			}
			else
			{
				/* DO NOTHING */
			}
		}

		/* スレッド動作中フラグチェック */
		if (FALSE == threacFlg)
		{
			DLOG_Debug("call EWDT_AsyncSendTermRes()");

			/* テストサーバ接続設定削除 */
			sprintf(cmd, "umount -l %s", EEPC_TEST_PATH_DEVSDA);
			ret = system(cmd);
			if (ret)
			{
				DLOG_Debug("umount command(%s) return(%d)\n", cmd, ret);
			}
			else
			{
				/* DO NOTHING */
			}

			sprintf(cmd, "unlink %s", EEPC_LINK_PATH);
			ret = system(cmd);
			if (ret)
			{
				DLOG_Debug("unlink command(%s) return(%d)\n", cmd, ret);
			}
			else
			{
				/* DO NOTHING */
			}

			/* スレッド動作中でない場合のみ、	*/
			/* 終了応答通知処理を実行			*/
			ewdtRet = EWDT_AsyncSendTermRes(g_eepcReqProcEndType);
			/* 終了応答通知処理結果チェック */
			if (ewdtRet)
			{
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	イベントメッセージ振分処理(ewdtプロセス)			*/
/*--------------------------------------------------------------------------*/
/*	param			:	FrmMsgHead_t *msg		受信パラメータ				*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	イベントメッセージ振分処理(ewdtプロセス)			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/10 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_ComFncDivideEvtMsgEwdt(FrmMsgHead_t const *msg)
{
	EepcFuncResult_t result	= EEPC_FUNC_RESULT_OK;	/* 処理結果	*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == msg)
	{
		DLOG_Debug("prm err");
		result = EEPC_FUNC_RESULT_EPRM;	/* 引数異常 */
	}
	else
	{
		/* イベントIDチェック */
		switch (msg->msgId)
		{
		/* WATCHIDOG応答 */
		case EWDT_EVT_WDT_REQ:
			DLOG_Debug("recv evt res wdtg");
			result = eepc_ComEvtResWdtg((EwdtEvtWdtReq_t *)msg);
			break;
		/* プロセス終了応答 */
		case EWDT_EVT_TERM_REQ:
			DLOG_Debug("recv res proc end");
			result = eepc_ComEvtResProcEnd((EwdtEvtTermReq_t *)msg);
			break;
		default:
			DLOG_Error("evtID err %d", msg->msgId);
			result = EEPC_FUNC_RESULT_NG;	/* 処理失敗 */
			break;
		}

		if (EEPC_FUNC_RESULT_OK != result)
		{
			result = EEPC_FUNC_RESULT_NG;	/* 処理失敗 */
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	設定変更通知イベント登録処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcRegChg_t *regChg	設定変更通知登録情報		*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	設定変更通知イベント登録処理						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/11 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_ComFncRegEepcChgEvt(EepcRegChg_t *regChg)
{
	EepcFuncResult_t	result									= EEPC_FUNC_RESULT_OK;	/* 処理結果					*/
	EepcRtnCode_t		ifResult								= EEPC_RTN_CODE_OK;		/* プロセス間IF向け処理結果	*/
	schar				writeFilePath[EEPC_TMP_FILE_PATH_SIZE]	= {'\0'};				/* 書き込みファイルパス		*/
	EepcPrcRes_t		ret;															/* EEPCプロセス応答用構造体	*/

	DEBUG_TRCIN();

	memset(&ret, 0, sizeof(EepcPrcRes_t));
	/* パラメータチェック */
	if (NULL == regChg)
	{
		DLOG_Debug("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* パラメータチェック */
		if (EEPC_EVT01_CHG_ALL < regChg->eventType)
		{
			DLOG_Debug("prm err");
			result		= EEPC_FUNC_RESULT_EPRM;
			ifResult	= EEPC_RTN_CODE_EPRM;
		}
		else
		{
			/* 設定変更通知イベント情報ファイルのファイルパスを生成	*/
			sprintf(writeFilePath, "%s%s",
				EEPC_RAM_DIR_PATH, EEPC_NTC_SET_FILE_NAME);

			/* 設定変更通知イベント登録データ作成処理	*/
			result = EEPC_FileCrtRegChgEvtData(regChg, writeFilePath);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				result		= EEPC_FUNC_RESULT_NG;
				ifResult	= EEPC_RTN_CODE_ESYS;
			}
			else
			{
				/* DO NOTHING */
			}
		}

		/* メッセージ生成	*/
		ret.header.retCode = (slong)ifResult;

		slong tmpCallRtn = FRM_CallRet(regChg->header.srcModId,
			sizeof(EepcPrcRes_t), (void *)&ret);
		if (tmpCallRtn)
		{
			DLOG_Error("FRM_CallRet error");
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	設定変更通知イベント登録解除処理					*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcClrChg_t *clrChg	設定変更通知登録解除情報	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	設定変更通知イベント登録解除処理					*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/12 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_ComFncClrEepcChgEvt(EepcClrChg_t *clrChg)
{
	EepcFuncResult_t	result									= EEPC_FUNC_RESULT_OK;	/* 処理結果					*/
	EepcRtnCode_t		ifResult								= EEPC_RTN_CODE_OK;		/* プロセス間IF向け処理結果	*/
	schar				writeFilePath[EEPC_TMP_FILE_PATH_SIZE]	= {'\0'};				/* 書き込みファイルパス		*/
	EepcPrcRes_t		ret;															/* EEPCプロセス応答用構造体	*/

	DEBUG_TRCIN();

	memset(&ret, 0, sizeof(EepcPrcRes_t));

	/* パラメータチェック */
	if (NULL == clrChg)
	{
		DLOG_Debug("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* 設定変更通知イベント情報ファイルのファイルパスを生成 */
		sprintf(writeFilePath, "%s%s",
			EEPC_RAM_DIR_PATH, EEPC_NTC_SET_FILE_NAME);

		/* 設定変更通知イベント登録解除データ作成処理 */
		result = EEPC_FileCrtClrChgEvtData(clrChg, writeFilePath);
		if (EEPC_FUNC_RESULT_OK != result)
		{
			result		= EEPC_FUNC_RESULT_NG;
			ifResult	= EEPC_RTN_CODE_ESYS;
		}
		else
		{
			/* DO NOTHING */
		}

		/* メッセージ生成	*/
		ret.header.retCode		= ifResult;

		slong tmpCallRtn = FRM_CallRet(clrChg->header.srcModId,
			sizeof(EepcPrcRes_t), (void *)&ret);
		if (tmpCallRtn)
		{
			DLOG_Error("FRM_CallRet error");
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	スケジュール受信スレッド処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:	void *arg	起動パラメータ							*/
/*--------------------------------------------------------------------------*/
/*	return			:	なし												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	スケジュール受信スレッド処理						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/12 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void *eepc_ComThrRcvSch(void *arg)
{
	EepcFuncResult_t	result		= EEPC_FUNC_RESULT_OK;	/* 処理結果				*/
	EepcSrvComInf_t		*srvComInf	= NULL;					/* サーバ通信用構造体	*/
	EepcEvt00_t			comResult;							/* 通信終了応答用構造体	*/
	slong				callEvtRtn	= 0;

	DEBUG_TRCIN();
	
	memset(&comResult, 0, sizeof(EepcEvt00_t));
	#if 0
	/* 自スレッドをDetach */
	pthread_detach( pthread_self() );
	#endif

	/* パラメータチェック */
	if (NULL == arg)
	{
		DLOG_Debug("prm err");
	}
	else
	{
		/* パラメータよりサーバ通信用構造体として取得 */
		srvComInf = (EepcSrvComInf_t *)arg;

		/* 終了要求種別チェック */
		if (EWDT_TERM_TYPE_NONE == g_eepcReqProcEndType)
		{
			/* 電力会社サーバ通信処理 */
			result = eepc_ComFncPrcSrvCom(srvComInf, &comResult);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Debug("eepc_ComFncPrcSrvCom() err %d", result);
			}
			else
			{
				/* DO NOTHING */
			}
		}
		else
		{
			DLOG_Error("rcv proc end");
			/* 通信終了応答処理のパラメータにプロセス終了要求を受信済みを設定 */
			comResult.comResult = EEPC_EVT_CODE_PROC_END;
		}

		/* メッセージ生成 */
		comResult.header.msgId = EEPC_LCL_EVT_ID_RCV_SCH;

		/* スケジュール区分の設定 */
		memcpy(comResult.reqSchDiv,
			srvComInf->comStart.schDivStr,
			EEPC_SCH_DIV_LEN);

		/* シーケンスIDの設定 */
		comResult.seqID = srvComInf->comStart.seqID;

		/* EEPCのサブスレッド処理終了の為、送信先はEEPCプロセスを指定 */
		callEvtRtn = FRM_MsgSendEvent(EEPC_SPORT, sizeof(EepcEvt00_t), (void *)&comResult);
		if (callEvtRtn)
		{
			/* プロセス間通信失敗	*/
			DLOG_Error("FRM_MsgSendEvent() err");
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return NULL;
}

/*==========================================================================*/
/*	Description		:	ID登録確認スレッド処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:	void *arg				起動パラメータ				*/
/*--------------------------------------------------------------------------*/
/*	return			:	なし												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ID登録確認スレッド処理								*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/12 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void *eepc_ComThrRegId(void *arg)
{
	EepcFuncResult_t	result		= EEPC_FUNC_RESULT_OK;	/* 処理結果				*/
	ulong				loopCnt		= 0u;					/* ループカウンタ		*/
	EepcSrvComInf_t		*srvComInf	= NULL;					/* サーバ通信用構造体	*/
	EepcEvt00_t			comResult;							/* 通信終了応答用構造体	*/
	ulong				idRegMax	= EEPC_RTRY_ID_REG_MAX - 1u;
	slong				callEvtRtn	= 0u;

	DEBUG_TRCIN();

	memset(&comResult, 0, sizeof(EepcEvt00_t));
	#if 0
	/* 自スレッドをDetach */
	pthread_detach( pthread_self() );
	#endif

	/* パラメータチェック */
	if (NULL == arg)
	{
		DLOG_Debug("prm err");
	}
	else
	{
		/* パラメータよりサーバ通信用構造体として取得 */
		srvComInf = (EepcSrvComInf_t *)arg;

		/* ID登録確認ループ */
		for (loopCnt = 0u; loopCnt < EEPC_RTRY_ID_REG_MAX; loopCnt++)
		{
			DLOG_Debug("eepc_thr_regId() rtry: %ld", loopCnt);

			/* 終了要求種別チェック */
			if (EWDT_TERM_TYPE_NONE != g_eepcReqProcEndType)
			{
				/* 終了要求種別が終了処理未実行以外の場合はループを抜ける */
				DLOG_Error("rcv proc end.");

				/* 通信終了応答処理のパラメータにプロセス終了要求を受信済みを設定 */
				comResult.comResult = EEPC_EVT_CODE_PROC_END;
				loopCnt = EEPC_RTRY_ID_REG_MAX;
			}
			else
			{
				/* 電力会社サーバ通信処理 */
				result = eepc_ComFncPrcSrvCom(srvComInf, &comResult);
				/* 通信確認結果チェック	*/
				if (EEPC_FUNC_RESULT_OK != result)
				{
					DLOG_Debug("eepc_ComFncPrcSrvCom() err %d", result);

					if (loopCnt < idRegMax)
					{
						/* リトライの為、待機	*/
						sleep(EEPC_RTRY_ID_REG_INTVL);
					}
					else
					{
						/* DO NOTHING */
					}
				}
				else
				{
					/* 通信成功の場合はループを抜ける	*/
					loopCnt = EEPC_RTRY_ID_REG_MAX;
				}
			}
		}

		/* メッセージ生成 */
		comResult.header.msgId = EEPC_LCL_EVT_ID_ID_REG;

		/* スケジュール区分の設定 */
		memcpy(comResult.reqSchDiv, srvComInf->comStart.schDivStr, EEPC_SCH_DIV_LEN);

		/* シーケンスIDの設定 */
		comResult.seqID = srvComInf->comStart.seqID;

		/* EEPCのサブスレッド処理終了の為、送信先はEEPCプロセスを指定 */
		callEvtRtn = FRM_MsgSendEvent(EEPC_SPORT, sizeof(EepcEvt00_t), (void *)&comResult);
		if (callEvtRtn)
		{
			/* プロセス間通信失敗	*/
			DLOG_Error("FRM_MsgSendEvent() err");
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return NULL;
}

/*==========================================================================*/
/*	Description		:	通信開始要求処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcComStart_t *comStart	通信用の格納先			*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK			処理成功				*/
/*					:	EEPC_FUNC_RESULT_NG			処理失敗				*/
/*					:	EEPC_FUNC_RESULT_EPRM		引数異常				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	通信開始要求処理									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/12 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_ComReqComStart(EepcComStart_t *comStart)
{
	EepcFuncResult_t	result		= EEPC_FUNC_RESULT_OK;	/* 処理結果				*/
	slong				crtThrRet	= 0;					/* スレッド生成結果		*/
	ulong				dataLen		= 0u;					/* データの文字列長		*/
	ulong				loopCnt		= 0u;					/* ループカウンタ		*/
	EepcEvt00_t			comInf;								/* 通信終了応答用構造体	*/
	ulong				dataSize	= EEPC_SCH_DIV_LEN - EEPC_STR_END_LEN;
	sint				tmpIsdRtn	= 0;
	schar				*fixYear	= NULL;
	schar				*updt		= NULL;
	schar				*loginId	= NULL;

	DEBUG_TRCIN();

	memset(&comInf, 0, sizeof(EepcEvt00_t));

	/* パラメータチェック */
	if (NULL == comStart)
	{
		DLOG_Debug("prm err");
		DEBUG_TRCOUT();

		return  EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* DO NOTHING */
	}

	/* 応答用にシーケンスIDを取得	*/
	comInf.seqID = comStart->seqID;

	/* データの文字列長を取得	*/
	dataLen = strnlen(comStart->schDivStr, sizeof(comStart->schDivStr));
	/* データの文字列長をチェック	*/
	if(dataLen != dataSize)
	{
		/* 通信終了応答処理のパラメータにシステム異常を設定	*/
		comInf.comResult = EEPC_EVT_CODE_SYSERR;
		result = EEPC_FUNC_RESULT_NG;
	}
	else
	{
		/* スケジュール区分の不正チェック	*/
		for (loopCnt = 0u; loopCnt < dataLen; loopCnt++)
		{
			/* 現在参照する文字が値であることをチェックする	*/
			tmpIsdRtn = isdigit((sint)comStart->schDivStr[loopCnt]);
			if (!tmpIsdRtn)
			{
				DLOG_Debug("sch div err");
				/* 通信終了応答処理のパラメータにシステム異常を設定	*/
				comInf.comResult = EEPC_EVT_CODE_SYSERR;
				/* 一つでも間違いであれば、処理異常としてループを抜ける	*/
				result = EEPC_FUNC_RESULT_NG;
				loopCnt = dataLen;
			}
			else
			{
				/* DO NOTHING */
			}
		}

		if (EEPC_FUNC_RESULT_OK == result)
		{
			strncpy(comInf.reqSchDiv, comStart->schDivStr, EEPC_SCH_DIV_LEN);
			DLOG_Debug("reqSchDiv[%s]", comInf.reqSchDiv);

			/* スケジュール区分チェック	*/
			fixYear	= strstr(&(comStart->schDivStr[0]), EEPC_SCH_DIV_FIX);
			updt	= strstr(&(comStart->schDivStr[0]), EEPC_SCH_DIV_UPDT);
			loginId	= strstr(&(comStart->schDivStr[0]), EEPC_SCH_DIV_ID);
			/* 固定/更新スケジュール受信 */
			if ((NULL != fixYear) || (NULL != updt))
			{
				/* スケジュール受信中フラグチェック */
				if (FALSE == g_eepcThrExecFlg[EEPC_THR_TYPE_RCV_SCH])
				{
					/* スケジュール受信スレッドの死活監視カウンタをリセット */
					g_eepcThrWdtCnt[EEPC_THR_TYPE_RCV_SCH] = 0u;

					/* スケジュール受信スレッドに渡すパラメータを生成 */
					memcpy(&(g_eepcSrvComInfSch.comStart),
						comStart,
						sizeof(EepcComStart_t));
					memcpy(&(g_eepcSrvComInfSch.usrSrv),
						&g_eepcFileUseSrv,
						sizeof(EepcFileUseSrv_t));

					/* 電力会社サーバ管理ファイル用mutex取得 */
					FRM_MutexLock(g_eepcMutexFileSrvMng);

					/* ルート証明書ファイル格納ディレクトリBankNoを設定 */
					g_eepcSrvComInfSch.rtCerBnkNo = g_eepcFileSrvMng.rtCerBnkNo;

					/* 電力会社サーバ管理ファイル用mutex解放 */
					FRM_MutexUnLock(g_eepcMutexFileSrvMng);

					/* スケジュール受信スレッド生成 */
					crtThrRet = FRM_PthreadCreate(
						&g_eepcThrId[EEPC_THR_TYPE_RCV_SCH],
						NULL,
						eepc_ComThrRcvSch,
						(void *)&g_eepcSrvComInfSch);

					/* スレッド生成結果チェック */
					if (crtThrRet)
					{
						/* 通信終了応答処理のパラメータにシステム異常を設定 */
						comInf.comResult = EEPC_EVT_CODE_SYSERR;
						result = EEPC_FUNC_RESULT_NG;
						DLOG_Error("recv sch thread create err");
					}
					else
					{
						/* スケジュール受信中フラグをON */
						g_eepcThrExecFlg[EEPC_THR_TYPE_RCV_SCH] = TRUE;
					}
				}
				else
				{
					DLOG_Error("recv sch thread exec");
					/* スケジュール受信中エラー */
					/* 通信終了応答のパラメータを二重要求受信に変更 */
					comInf.comResult = EEPC_EVT_CODE_CONFLICT_SCH;
					result = EEPC_FUNC_RESULT_NG;
				}
			}
			/* ID登録確認 */
			if (NULL != loginId)
			{
				/* ID登録確認中フラグチェック */
				if (FALSE == g_eepcThrExecFlg[EEPC_THR_TYPE_ID_REG])
				{
					/* ID登録確認スレッドの死活監視カウンタをリセット */
					g_eepcThrWdtCnt[EEPC_THR_TYPE_ID_REG] = 0u;
					/* ID登録確認スレッドに渡すパラメータを生成 */
					memcpy(&(g_eepcSrvComInfRegId.comStart),
						comStart,
						sizeof(EepcComStart_t));
					memcpy(&(g_eepcSrvComInfRegId.usrSrv),
						&g_eepcFileUseSrv,
						sizeof(EepcFileUseSrv_t));

					/* 電力会社サーバ管理ファイル用mutex取得 */
					FRM_MutexLock(g_eepcMutexFileSrvMng);

					/* ルート証明書ファイル格納ディレクトリBankNoを設定 */
					g_eepcSrvComInfRegId.rtCerBnkNo = g_eepcFileSrvMng.rtCerBnkNo;

					/* 電力会社サーバ管理ファイル用mutex解放 */
					FRM_MutexUnLock(g_eepcMutexFileSrvMng);

					/* ID登録確認スレッド生成	*/
					crtThrRet = FRM_PthreadCreate(
						&g_eepcThrId[EEPC_THR_TYPE_ID_REG],
						NULL,
						eepc_ComThrRegId,
						(void *)&g_eepcSrvComInfRegId);

					/* スレッド生成結果チェック	*/
					if (crtThrRet)
					{
						/* 通信終了応答処理のパラメータにシステム異常を設定	*/
						comInf.comResult = EEPC_EVT_CODE_SYSERR;
						/* 通信終了応答処理	*/
						result = EEPC_FUNC_RESULT_NG;
						DLOG_Error("reg id thread create err");
					}
					else
					{
						/* ID登録確認中フラグをON	*/
						g_eepcThrExecFlg[EEPC_THR_TYPE_ID_REG] = TRUE;
					}
				}
				else
				{
					DLOG_Debug("reg id thread exec");
					/* ID登録確認中エラー */
					/* 通信終了応答のパラメータを二重要求受信に変更	*/
					comInf.comResult = EEPC_EVT_CODE_CONFLICT_ID;
					result = EEPC_FUNC_RESULT_NG;
				}
			}
			else
			{
				DLOG_Debug("reg id thread exec");
				/* 通信終了応答処理のパラメータにシステム異常を設定	*/
				comInf.comResult = EEPC_EVT_CODE_SYSERR;
				result = EEPC_FUNC_RESULT_NG;
			}
		}
		else
		{
			/* DO NOTHING */
		}
	}

	if (EEPC_FUNC_RESULT_OK != result)
	{
		/* 通信終了応答処理	*/
		result = eepc_ComEvtComEnd(&comInf, comStart->reqID);
	}
	else
	{
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社情報管理更新スレッド処理					*/
/*--------------------------------------------------------------------------*/
/*	param			:	void *arg	起動パラメータ							*/
/*--------------------------------------------------------------------------*/
/*	return			:	なし												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社情報管理更新スレッド処理					*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/12 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void *eepc_ComThrUpdateSrvinf(void *arg)
{
	EepcFuncResult_t	result		= EEPC_FUNC_RESULT_OK;		/* 処理結果					*/
	EepcUpdtSrv_t		updateSrvInf;							/* 電力会社情報更新用構造体	*/
	slong				callEvt		= 0;

	DEBUG_TRCIN();

	memset(&updateSrvInf, 0, sizeof(EepcUpdtSrv_t));
#if 0
	/* 自スレッドをDetach */
	pthread_detach( pthread_self() );	
#endif
	/* パラメータチェック */
	if (NULL == arg)
	{
		DLOG_Debug("prm err");
	}
	else
	{
		memcpy(&updateSrvInf, arg, sizeof(EepcUpdtSrv_t));

		/* ファイル更新中断フラグチェック */
		if(FALSE != g_eepcFileUpdateCanselFlg)
		{
			/* ファイル更新中断を検出した場合 */
			DLOG_Error("rcv update cancel.");
		}
		else
		{
			/* 電力会社サーバ管理情報更新処理 */
			result = EEPC_FileUpdateSrvMngFile();
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Debug("EEPC_FileUpdateSrvMngFile() err %d", result);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
		}

		/* メッセージ生成 */
		updateSrvInf.header.msgId	= EEPC_LCL_EVT_ID_UPDATE_SRV_INF;
		updateSrvInf.header.retCode	= result;

		/* EEPCのサブスレッド処理終了の為、送信先はEEPCプロセスを指定 */
		callEvt = FRM_MsgSendEvent(EEPC_SPORT, sizeof(EepcUpdtSrv_t), (void *)&updateSrvInf);
		if (callEvt)
		{
			/* プロセス間通信失敗 */
			DLOG_Error("FRM_MsgSendEvent() err");
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return NULL;
}

/*==========================================================================*/
/*	Description		:	電力会社情報更新処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcUpdtSrv_t *updateSrvInf		更新用の格納先		*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社情報更新処理								*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/12 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_ComFncUpdateSrvinf(EepcUpdtSrv_t *updateSrvInf)
{
	EepcFuncResult_t	result		= EEPC_FUNC_RESULT_OK;	/* 処理結果					*/
	slong				crtThrRet	= 0;					/* スレッド生成結果			*/

	DEBUG_TRCIN();

	/* パラメータチェック	*/
	if (NULL == updateSrvInf)
	{
		DLOG_Debug("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		memcpy(&g_eepcUpdtSrv,
			updateSrvInf,
			sizeof(EepcUpdtSrv_t));
		
		/* 電力会社情報管理更新スレッド動作中フラグチェック	*/
		if (FALSE == g_eepcThrExecFlg[EEPC_THR_TYPE_UPDATE_SRV_INF])
		{
			/* 電力会社情報管理更新スレッドの死活監視カウンタをリセット	*/
			g_eepcThrWdtCnt[EEPC_THR_TYPE_UPDATE_SRV_INF] = 0u;

			/* 電力会社情報管理更新スレッド生成	*/
			crtThrRet = FRM_PthreadCreate(
				&g_eepcThrId[EEPC_THR_TYPE_UPDATE_SRV_INF],
				NULL,
				eepc_ComThrUpdateSrvinf,
				(void *)&g_eepcUpdtSrv);

			/* スレッド生成結果チェック	*/
			if (crtThrRet)
			{
				DLOG_Error("update thread create err");
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* 電力会社情報管理更新スレッド動作中フラグをON	*/
				g_eepcThrExecFlg[EEPC_THR_TYPE_UPDATE_SRV_INF] = TRUE;
			}
		}
		else
		{
			DLOG_Error("update thread exec");
			result = EEPC_FUNC_RESULT_NG;
		}
		
		if (EEPC_FUNC_RESULT_OK != result)
		{
			/* 電力会社情報更新終了処理	*/
			result = eepc_ComEvtUpdtEndSrvMng(updateSrvInf);
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社サーバ通信処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcSrvComInf_t *srvComInf	サーバ通信情報用構造体	*/
/*					:	EepcEvt00_t *comResult		通信終了応答用構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社サーバ通信処理								*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/13 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_ComFncPrcSrvCom(EepcSrvComInf_t *srvComInf, EepcEvt00_t *comResult)
{
	EepcFuncResult_t	result		= EEPC_FUNC_RESULT_OK;	/* 処理結果					*/
	EepcFuncResult_t	socResult	= EEPC_FUNC_RESULT_OK;	/* ソケット処理結果			*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == srvComInf) || (NULL == comResult))
	{
		DLOG_Debug("prm err");
		result = EEPC_FUNC_RESULT_EPRM;

		if (comResult != NULL)
		{
			comResult->comResult = EEPC_EVT_CODE_SYSERR;
		}
		else
		{
			/* DO NOTHING */
		}
	}
	else
	{
		/* TODO: */
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社情報更新中断処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcUpdtSrv_t *updateSrvInf		更新用の格納先		*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社情報更新中断処理							*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/12 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_ComFncCancelSrvInf(EepcUpdtSrv_t *updateSrvInf)
{
	EepcFuncResult_t result = EEPC_FUNC_RESULT_OK;	/* 処理結果	*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == updateSrvInf)
	{
		DLOG_Debug("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* ファイル更新中チェック */
		if (TRUE == g_eepcThrExecFlg[EEPC_THR_TYPE_UPDATE_SRV_INF])
		{
			/* ファイル更新中断フラグON */
			g_eepcFileUpdateCanselFlg = TRUE;
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*End Of File*/
