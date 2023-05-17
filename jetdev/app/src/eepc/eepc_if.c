/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Xu.Y												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/11												*/
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
#include "eepc_file.h"
#include "dlog_g.h"
#include <string.h>
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/

/* リトライ時刻情報テーブル	*/
/* リトライ開始時刻			*/
/*	時,		分,	秒,	予備	*/
EepcTimInf_t timInfTblStart[EEPC_RTRY_TIM_TBL_CNT] = {
	{21,	0,	0,	0},
	{21,	30,	0,	0},
	{22,	0,	0,	0},
	{22,	30,	0,	0},
	{23,	0,	0,	0},
	{23,	30,	0,	0},
	{0,		0,	0,	0},
	{0,		30,	0,	0},
	{1,		0,	0,	0},
	{1,		30,	0,	0},
};

/* リトライ終了時刻	*/
EepcTimInf_t timInfTblEnd[EEPC_RTRY_TIM_TBL_CNT] = {
	{21,	9,	59,	0},
	{21,	39,	59,	0},
	{22,	9,	59,	0},
	{22,	39,	59,	0},
	{23,	9,	59,	0},
	{23,	39,	59,	0},
	{0,		9,	59,	0},
	{0,		39,	59,	0},
	{1,		9,	59,	0},
	{1,		39,	59,	0},
};

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:	通信開始要求IF										*/
/*--------------------------------------------------------------------------*/
/*	param			:	ulong	reqID			リクエストID				*/
/*					:	uchar	seqID			シーケンスID				*/
/*					:	schar	*schDivStr		スケジュール区分			*/
/*					:	schar	*pwrStID		発電所ID					*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_RTN_CODE_OK		正常						*/
/*					:	EEPC_RTN_CODE_ESYS		システム異常				*/
/*					:	EEPC_RTN_CODE_EPRM		引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	通信開始要求IF										*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/12 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcRtnCode_t EEPC_IfAsyncReqComStart(const ulong reqId, const uchar seqId,
	const schar *schDivStr, const schar *pwrStId)
{
	EepcRtnCode_t	result	= EEPC_RTN_CODE_OK;	/* 処理結果 */
	ulong			idMax	= 0u;
	sint			schSize = EEPC_SCH_DIV_LEN - EEPC_STR_END_LEN;
	sint			pwrSize = EEPC_PWR_ST_ID - EEPC_STR_END_LEN;
	sint			schLen	= 0;
	sint			pwrLen	= 0;
	slong			tmpRtn	= 0;
	EepcComStart_t	prm;						/* プロセス間通信パラメータ	*/

	DEBUG_TRCIN();

	memset(&prm, 0, sizeof(EepcComStart_t));

#ifdef COM_DBG
	/* 通信開始を抑制する為、システム異常を返却させる	*/
	result = EEPC_RTN_CODE_ESYS;
#else
	/* パラメータチェック */
	idMax = LFW_SPORT_MIN + PROCESS_MAX - EEPC_PROCESS_LEN;
	if ((idMax < reqId) || (NULL == schDivStr) || (NULL == pwrStId))
	{
		result = EEPC_RTN_CODE_EPRM;
	}
	else
	{
		/* データチェック */
		schLen = (sint)strnlen(schDivStr, EEPC_SCH_DIV_LEN);
		pwrLen = (sint)strnlen(pwrStId, EEPC_PWR_ST_ID);
		if ((schLen != schSize) || (pwrLen != pwrSize))
		{
			result = EEPC_RTN_CODE_EPRM;
		}
		else
		{
			/* メッセージ生成 */
			/* メッセージ構造ヘッダ */
			prm.header.msgId = EEPC_FNC_ID_ASYNC_REQ_COM_START;

			prm.reqID					= reqId;				/* 返送ID			*/
			prm.seqID					= seqId;				/* シーケンスID		*/
			strncpy(prm.schDivStr, schDivStr, (size_t)schSize);	/* スケジュール区分	*/
			strncpy(prm.pwrStID, pwrStId, (size_t)pwrSize);		/* 発電所ID			*/

			tmpRtn = FRM_MsgSendAsync(EEPC_SPORT, sizeof(EepcComStart_t), (void *)&prm);
			if (tmpRtn)
			{
				result = EEPC_RTN_CODE_ESYS;
			}
			else
			{
				/* DO NOTHING */
			}
		}
	}
#endif

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社情報更新IF									*/
/*--------------------------------------------------------------------------*/
/*	param			:	ulong reqId				リクエストID				*/
/*					:	uchar seqId				シーケンスID				*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_RTN_CODE_OK		正常						*/
/*					:	EEPC_RTN_CODE_ESYS		システム異常				*/
/*					:	EEPC_RTN_CODE_EPRM		引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社情報更新IF									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/13 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcRtnCode_t EEPC_IfAsyncUpdateSrvInf(const ulong reqId, const uchar seqId)
{
	EepcRtnCode_t	result	= EEPC_RTN_CODE_OK;	/* 処理結果					*/
	EepcUpdtSrv_t	prm;						/* プロセス間通信パラメータ	*/
	ulong			idMax	= 0u;
	slong			tmpRtn	= 0;

	DEBUG_TRCIN();

	memset(&prm, 0, sizeof(EepcUpdtSrv_t));

	/* パラメータチェック	*/
	idMax = LFW_SPORT_MIN + PROCESS_MAX - EEPC_PROCESS_LEN;
	if (idMax < reqId)
	{
		result = EEPC_RTN_CODE_EPRM;
	}
	else
	{
		/* メッセージ生成 */
		/* メッセージ構造ヘッダ */
		prm.header.msgId	= EEPC_FNC_ID_ASYNC_UPDATE_SRV_INF;

		prm.reqID			= reqId;	/* 返送ID		*/
		prm.seqID			= seqId;	/* シーケンスID	*/

		tmpRtn = FRM_MsgSendAsync(EEPC_SPORT, sizeof(EepcComStart_t), (void *)&prm);
		if (tmpRtn)
		{
			result = EEPC_RTN_CODE_ESYS;
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
/*	Description		:	電力会社情報更新中断IF								*/
/*--------------------------------------------------------------------------*/
/*	param			:	ulong reqId				リクエストID				*/
/*					:	uchar seqId				シーケンスID				*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_RTN_CODE_OK		正常						*/
/*					:	EEPC_RTN_CODE_ESYS		システム異常				*/
/*					:	EEPC_RTN_CODE_EPRM		引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社情報更新中断IF								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/13 : DHC Xu.Y : 新規作成						*/
/*==========================================================================*/
EepcRtnCode_t EEPC_IfAsyncCancelSrvInf(const ulong reqId, const uchar seqId)
{
	EepcRtnCode_t	result	= EEPC_RTN_CODE_OK;	/* 処理結果					*/
	EepcUpdtSrv_t	prm;						/* プロセス間通信パラメータ	*/
	ulong			idMax	= 0u;
	slong			tmpRtn	= 0;

	DEBUG_TRCIN();

	memset(&prm, 0, sizeof(EepcUpdtSrv_t));

	/* パラメータチェック	*/
	idMax = LFW_SPORT_MIN + PROCESS_MAX - EEPC_PROCESS_LEN;
	if (idMax < reqId)
	{
		result = EEPC_RTN_CODE_EPRM;
	}
	else
	{
		/* メッセージ生成 */
		/* メッセージ構造ヘッダ */
		prm.header.msgId	= EEPC_FNC_ID_ASYNC_CANCEL_SRV_INF;

		prm.reqID			= reqId;	/* 返送ID		*/
		prm.seqID			= seqId;	/* シーケンスID	*/

		tmpRtn = FRM_MsgSendAsync(EEPC_SPORT, sizeof(EepcComStart_t), (void *)&prm);
		if (tmpRtn)
		{
			result = EEPC_RTN_CODE_ESYS;
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
/*	Description		:	電力会社情報取得IF									*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcSrvMng_t *srvMng	情報格納先のポインタ		*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_RTN_CODE_OK		正常						*/
/*					:	EEPC_RTN_CODE_ESYS		システム異常				*/
/*					:	EEPC_RTN_CODE_EPRM		引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社情報取得IF									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/11 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcRtnCode_t EEPC_IfSyncGetSrvInf(EepcSrvMng_t *srvMng)
{
	EepcRtnCode_t	result		= EEPC_RTN_CODE_OK;		/* 処理結果			*/
	sint			loopCnt		= 0;					/* ループカウンタ	*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == srvMng)
	{
		DLOG_Error("prm err");
		result = EEPC_RTN_CODE_EPRM;	/* 引数異常 */
	}
	else
	{
		/* 電力会社サーバ管理ファイル用mutex取得 */
		FRM_MutexLock(g_eepcMutexFileSrvMng);

		/* 使用可能な電力会社の総数	*/
		srvMng->totalSrv = g_eepcFileSrvMng.srvCnt;

		/* 使用中の電力会社のIndex値 */
		srvMng->useSrvIndex = g_eepcUseSrvIndex;

		/* 電力会社名のリスト & 電力会社区分のリスト */
		for(loopCnt = 0; loopCnt < EEPC_SRV_LIST_MAX; loopCnt++){
			memcpy(srvMng->srvNameStrList[loopCnt],
				g_eepcFileSrvMng.srvInfList[loopCnt].srvNameStr,
				EEPC_SRV_NAME_LEN);
			srvMng->corpDiv[loopCnt] = g_eepcFileSrvMng.srvInfList[loopCnt].corpDiv;
		}

		/* 電力会社サーバ管理ファイル用mutex解放 */
		FRM_MutexUnLock(g_eepcMutexFileSrvMng);
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社情報保存IF									*/
/*--------------------------------------------------------------------------*/
/*	param			:	uchar usrSrvIndex		保存対象のIndex値			*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_RTN_CODE_OK		正常						*/
/*					:	EEPC_RTN_CODE_ESYS		システム異常				*/
/*					:	EEPC_RTN_CODE_EPRM		引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社情報保存IF									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/11 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcRtnCode_t EEPC_IfSyncSetSrvInf(const uchar useSrvIndex)
{
	EepcRtnCode_t	result	= EEPC_RTN_CODE_OK;	/* 処理結果					*/
	EepcSetSrv_t	prm;						/* 電力会社情報保存用構造体	*/
	EepcPrcRes_t	ret;						/* EEPCプロセス応答用構造体	*/

	DEBUG_TRCIN();

	memset(&prm, 0, sizeof(EepcSetSrv_t));
	memset(&ret, 0, sizeof(EepcPrcRes_t));
	/* パラメータチェック */
	if(EEPC_SRV_LIST_MAX <= useSrvIndex)
	{
		result = EEPC_RTN_CODE_EPRM;
	}
	else
	{
		/* メッセージ生成 */
		/* メッセージ構造ヘッダ */
		prm.header.msgId		= EEPC_FNC_ID_SYNC_SET_SRV_INF;
		prm.header.retCode		= 0;
		prm.useSrvIndex			= useSrvIndex;	/* 使用中の電力会社のIndex値 */

		/* 同期メッセージ送信 */
		slong tmpCallRtn = FRM_CallSync(EEPC_SPORT, sizeof(EepcSetSrv_t), &prm, &ret);
		if (tmpCallRtn)
		{
			/* プロセス間通信失敗 */
			result = EEPC_RTN_CODE_ESYS;
		}
		else
		{
			/* プロセス間通信結果を取得 */
			result = (sint)ret.header.retCode;
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	設定変更通知イベント登録IF							*/
/*--------------------------------------------------------------------------*/
/*	param			:	sint receiver			ポート番号					*/
/*					:	ulong eventType			イベント種別				*/
/*					:	uchar noticerflg		通知要否					*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_RTN_CODE_OK		正常						*/
/*					:	EEPC_RTN_CODE_ESYS		システム異常				*/
/*					:	EEPC_RTN_CODE_EPRM		引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	設定変更通知イベント登録IF							*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/11 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcRtnCode_t EEPC_IFSyncRegEepcChgEvt(const sint receiver, const ulong eventType, const uchar noticerflg)
{
	EepcRtnCode_t	result	= EEPC_RTN_CODE_OK;	/* 処理結果							*/
	sint 			recvMax	= 0;
	EepcRegChg_t	prm;						/* 設定変更通知イベント登録用構造体	*/
	EepcPrcRes_t	ret;						/* EEPCプロセス応答用構造体			*/

	DEBUG_TRCIN();

	memset(&prm, 0, sizeof(EepcRegChg_t));
	memset(&ret, 0, sizeof(EepcPrcRes_t));

	/* パラメータチェック	*/
	recvMax = LFW_SPORT_MIN + PROCESS_MAX - EEPC_PROCESS_LEN;
	if ((recvMax < receiver) || (receiver < LFW_SPORT_MIN) ||
		(EEPC_EVT01_CHG_ALL < eventType) ||
		((EEPC_REG_NOTICE_DISABLE != noticerflg) &&
		(EEPC_REG_NOTICE_ENABLE != noticerflg)))
	{
		result = EEPC_RTN_CODE_EPRM;
	}
	else
	{
		/* メッセージ生成	*/
		/* メッセージ構造ヘッダ	*/
		prm.header.msgId	= EEPC_FNC_ID_SYNC_REG_EEPC_CHG_EVT;
		prm.receiver		= receiver;		/* イベント通知先のポート番号		*/
		prm.eventType		= eventType;	/* 設定変更通知イベント種別			*/
		prm.noticerflg		= noticerflg;	/* イベント登録後の通知要否フラグ	*/

		/* 同期メッセージ送信 */
		slong tmpCallRtn = FRM_CallSync(EEPC_SPORT, sizeof(EepcRegChg_t), &prm, &ret);
		if (tmpCallRtn)
		{
			/* プロセス間通信失敗 */
			result = EEPC_RTN_CODE_ESYS;
		}
		else
		{
			/* プロセス間通信結果を取得 */
			result = (EepcRtnCode_t)ret.header.retCode;
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	設定変更通知イベント登録解除IF						*/
/*--------------------------------------------------------------------------*/
/*	param			:	sint receiver			ポート番号					*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_RTN_CODE_OK		正常						*/
/*					:	EEPC_RTN_CODE_ESYS		システム異常				*/
/*					:	EEPC_RTN_CODE_EPRM		引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	設定変更通知イベント登録解除IF						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/11 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcRtnCode_t EEPC_IfSyncClrEepcChgEvt(const sint receiver)
{
	EepcRtnCode_t	result	= EEPC_RTN_CODE_OK;	/* 処理結果							*/
	sint 			recvMax	= 0;
	EepcClrChg_t	prm;						/* 設定変更通知イベント登録用構造体	*/
	EepcPrcRes_t	ret;						/* EEPCプロセス応答用構造体			*/

	DEBUG_TRCIN();

	memset(&prm, 0, sizeof(EepcClrChg_t));
	memset(&ret, 0, sizeof(EepcPrcRes_t));

	recvMax = LFW_SPORT_MIN + PROCESS_MAX - EEPC_PROCESS_LEN;
	if ((recvMax < receiver) || (receiver < LFW_SPORT_MIN))
	{
		result = EEPC_RTN_CODE_EPRM;
	}
	else
	{
		prm.header.msgId	= EEPC_FNC_ID_SYNC_CLR_EEPC_CHG_EVT;
		prm.receiver		= receiver;		/* イベント通知先のポート番号	*/

		/* 同期メッセージ送信 */
		slong tmpCallRtn = FRM_CallSync(EEPC_SPORT, sizeof(EepcClrChg_t), &prm, &ret);
		if (tmpCallRtn)
		{
			/* プロセス間通信失敗 */
			result = EEPC_RTN_CODE_ESYS;
		}
		else
		{
			/* プロセス間通信結果を取得 */
			result = (EepcRtnCode_t)ret.header.retCode;
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	通信先情報取得IF									*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcComInf_t *comInf	情報格納先のポインタ		*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_RTN_CODE_OK		正常						*/
/*					:	EEPC_RTN_CODE_ESYS		システム異常				*/
/*					:	EEPC_RTN_CODE_EPRM		引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	通信先情報取得IF									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/11 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcRtnCode_t EEPC_IfSyncGetComInf(EepcComInf_t *comInf)
{
	EepcRtnCode_t result = EEPC_RTN_CODE_OK;

	DEBUG_TRCIN();

	if (NULL == comInf)
	{
		DLOG_Error("prm err");
		result = EEPC_RTN_CODE_EPRM;	/* 引数異常	*/
	}
	else
	{
		/* 通信先情報用構造体更新処理	*/
		result = EEPC_FileUpdateComInf(comInf);

		if (EEPC_RTN_CODE_OK != result)
		{
			DLOG_Error("EEPC_FileUpdateComInf() err %d", result);
			result = EEPC_RTN_CODE_ESYS;
		}
		else
		{
			DLOG_Debug("EEPC_FileUpdateComInf() success");
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	同期時刻情報取得IF									*/
/*--------------------------------------------------------------------------*/
/*	param			:	uchar chkSum			チェックサム(発電所IDの末尾)*/
/*					:	EepcSyncTimInf_t *timInf	情報格納先のポインタ	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_RTN_CODE_OK		正常						*/
/*					:	EEPC_RTN_CODE_EPRM		引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	同期時刻情報取得IF									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/11 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcRtnCode_t EEPC_IfLibGetSyncTimInf(uchar chkSum, EepcSyncTimInf_t *timInf)
{
	EepcRtnCode_t result = EEPC_RTN_CODE_OK;	/* 処理結果 */

	DEBUG_TRCIN();

	/* パラメータチェック */
	if((EEPC_RTRY_TIM_TBL_CNT <= chkSum) || (NULL == timInf))
	{
		result = EEPC_RTN_CODE_EPRM;
	}
	else
	{
		/* リトライ開始時間設定	*/
		memcpy(&(timInf->ntpRtryStrtTimSync),
			&timInfTblStart[chkSum],
			sizeof(EepcSyncTimInf_t));
		
		/* リトライ終了時間設定	*/
		memcpy(&(timInf->ntpRtryEndTimSync),
			&timInfTblEnd[chkSum],
			sizeof(EepcSyncTimInf_t));
	}

	DEBUG_TRCOUT();

	return result;
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*End Of File*/
