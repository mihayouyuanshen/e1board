/****************************************************************************/
/*	Description		:INV・PV・BDC・J.B.転送 処理流れ						*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC.zhangs												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/20												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:FW更新機能の機能名は、EOTAとす							*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 DHC CORPORATION								*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "eota_setting_g.h"
#include "eota_log_g.h"
#include "eota_data_g.h"
#include "eota_serv_g.h"
#include "eota_file_g.h"
#include "eota_flow_g.h"

#include "eota_inv.h"

/*--------------------------------------------------------------------------*/
/* 公開変数定義																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* 非公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* 公開関数定義																*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:	一回応答タイムアウト 								*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	一回応答タイムアウトを実施する						*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
void EOTA_TimeOutRes()
{
	DEBUG_TRCIN();

	/* タイムを削除 */
	FRM_TimerCancelOneShot(EOTA_TIMEID_INV_RES);
	/* 一回応答タイム未実施設定 */
	EOTA_SetInvTimeRes(EOTA_TIME_NULL);

	/* 1s 以内に応答が無い場合は、前回送信値を再送すること */
	eota_SendInvBeforeMess();

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	通信タイムアウト 									*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	通信タイムアウトを実施する							*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
void EOTA_TimeOutInv()
{
	DEBUG_TRCIN();

	/* タイムを削除 */
	FRM_TimerCancelOneShot(EOTA_TIMEID_INV_TIME_OUT);
	/* 通信タイムアウト未実施設定 */
	EOTA_SetInvTimeOut(EOTA_TIME_NULL);

	/* INVマイ コ ン応答有無に関わらず、FW転送シーケンス終了して良い */
	/* TODO. */

	DEBUG_TRCOUT();
}


/*==========================================================================*/
/*	Description		:	メッセージ受信処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:	受信メッセージ										*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_INV_OK:処理正常								*/
/*						EOTA_INV_NG:処理異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	INVからメッセージ受信処理を実施する					*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_SendInvMessage()
{
	uchar	Ret				= EOTA_INV_OK;		/* 戻り値					*/
	uchar	UpdProject		= 0;				/* 更新部品状態				*/
	uchar	UpdIngConFlg	= EOTA_UPD_ING_CONFLG_OFF;
												/* 更新引き続きフラグ		*/

	DEBUG_TRCIN();

	/* 更新部品状態取得 */
	UpdProject = EOTA_GetUpdProjectState();

	/* INV転送 */
	if (	(EOTA_TGT_FLG_ON == EOTA_GetTgtInv())
		&&	(0 == (UpdProject & EOTA_INV_SEND_END)))
	{
		/* FW 転送問い合わせ */
		Ret = eota_SendInvMessInq(EOTA_UPD_TGT_INV);
		if (EOTA_INV_OK == Ret)
		{
			/* 転送中Tgt設定 */
			EOTA_SetUpdTgt(EOTA_UPD_TGT_INV);

			/* 更新部品状態設定 */
			EOTA_SetUpdProjectState(UpdProject | EOTA_INV_SEND_END);

			UpdIngConFlg = EOTA_UPD_ING_CONFLG_ON;
		}
		else
		{
			/* LED設定 */
			EOTA_Set_Led(EOTA_INV_UPD_FAIL);
		}
	}
	else
	{
		/* 処理無し */
	}

	/* PV転送 */
	if (	(EOTA_INV_OK == Ret)
		&&	(EOTA_TGT_FLG_ON == EOTA_GetTgtPV())
		&&	(0 == (UpdProject & EOTA_PV_SEND_END)))
	{
		/* FW 転送問い合わせ */
		Ret = eota_SendInvMessInq(EOTA_UPD_TGT_PV);
		if (EOTA_INV_OK == Ret)
		{
			/* 転送中Tgt設定 */
			EOTA_SetUpdTgt(EOTA_UPD_TGT_PV);

			/* 更新部品状態設定 */
			EOTA_SetUpdProjectState(UpdProject | EOTA_PV_SEND_END);

			UpdIngConFlg = EOTA_UPD_ING_CONFLG_ON;
		}
		else
		{
			/* LED設定 */
			EOTA_Set_Led(EOTA_PV_UPD_FAIL);
		}
	}
	else
	{
		/* 処理無し */
	}

	/* BDC転送 */
	if (	(EOTA_INV_OK == Ret)
		&&	(EOTA_TGT_FLG_ON == EOTA_GetTgtBDC())
		&&	(0 == (UpdProject & EOTA_BDC_SEND_END)))
	{
		/* FW 転送問い合わせ */
		Ret = eota_SendInvMessInq(EOTA_UPD_TGT_BDC);
		if (EOTA_INV_OK == Ret)
		{
			/* 転送中Tgt設定 */
			EOTA_SetUpdTgt(EOTA_UPD_TGT_BDC);

			/* 更新部品状態設定 */
			EOTA_SetUpdProjectState(UpdProject | EOTA_BDC_SEND_END);

			UpdIngConFlg = EOTA_UPD_ING_CONFLG_ON;
		}
		else
		{
			/* LED設定 */
			EOTA_Set_Led(EOTA_BDC_UPD_FAIL);
		}
	}
	else
	{
		/* 処理無し */
	}

	/* J.B転送 */
	if (	(EOTA_INV_OK == Ret)
		&&	(EOTA_TGT_FLG_ON == EOTA_GetTgtJB())
		&&	(0 == (UpdProject & EOTA_JB_SEND_END)))
	{
		/* FW 転送問い合わせ */
		Ret = eota_SendInvMessInq(EOTA_UPD_TGT_J_B);
		if (EOTA_INV_OK == Ret)
		{
			/* 転送中Tgt設定 */
			EOTA_SetUpdTgt(EOTA_UPD_TGT_BDC);

			/* 更新部品状態設定 */
			EOTA_SetUpdProjectState(UpdProject | EOTA_JB_SEND_END);

			UpdIngConFlg = EOTA_UPD_ING_CONFLG_ON;
		}
		else
		{
			/* LED設定 */
			EOTA_Set_Led(EOTA_JB_UPD_FAIL);
		}
	}
	else
	{
		/* 処理無し */
	}

	if ((EOTA_INV_OK == Ret) && (EOTA_UPD_ING_CONFLG_OFF == UpdIngConFlg))
	{
		/* 更新中状態設定 */
		EOTA_SetUpdIngState(EOTA_UPD_ING_INV_END);
		/* 状態に遷移 */
		EOTA_Flow();
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	メッセージ受信処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:	受信メッセージ										*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_INV_OK:処理正常								*/
/*						EOTA_INV_NG:処理異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	INVからメッセージ受信処理を実施する					*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_RevInvMessage(
					EOTA_CMD11_RES_INFO_T * Message) 	/* [i]受信メッセージ */
{
	uchar	Ret			= EOTA_INV_OK;		/* 戻り値		*/
	uchar	FwTranState = 0u;				/* FW 転送状態	*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == Message)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_INV_NG;
		return Ret;
	}

	/* タイム終了 */
	eota_TimeEnd();

	/* FW 転送状態取得 */
	FwTranState = Message->FwTranState;

	/* 0:FW 転送継続 */
	if (EOTA_FW_TRAN_STATE_C == FwTranState)
	{
		Ret = eota_FWTranStateCon(Message);
	}
	/* 1:FW 転送終了 */
	else if (EOTA_FW_TRAN_STATE_E == FwTranState)
	{
		Ret = eota_FWTranStateEnd(Message);
	}
	else
	{
		/* 処理無し */
	}
	DEBUG_TRCOUT();

	return Ret;
}

/*--------------------------------------------------------------------------*/
/* 非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:	タイマー起動										*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	タイマーを起動する									*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
void eota_TimeStart()
{
	DEBUG_TRCIN();

	/* 一回応答タイム未実施の場合 */
	if (EOTA_TIME_NULL == EOTA_GetInvTimeRes())
	{
		/* 一回応答タイム起動 */
		FRM_TimerSetOneShot(EOTA_TIMEID_INV_RES, EOTA_TIME_INV_RES, NULL);
		/* 一回応答タイム実施中設定 */
		EOTA_SetInvTimeRes(EOTA_TIME_ING);
	}
	else
	{
		/* 処理無し */
	}

	/* 通信タイムアウト未実施の場合 */
	if  (EOTA_TIME_NULL == EOTA_GetInvTimeOut())
	{
		/* 通信タイムアウト起動 */
		FRM_TimerSetOneShot(EOTA_TIMEID_INV_TIME_OUT,
							EOTA_TIME_INV_TIME_OUT,
							NULL);
		/* 一回応答タイム実施中設定 */
		EOTA_SetInvTimeOut(EOTA_TIME_ING);
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	タイマー終了										*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	タイマーを終了する									*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
void eota_TimeEnd()
{
	DEBUG_TRCIN();

	/* 一回応答タイム実施の場合 */
	if (EOTA_TIME_ING == EOTA_GetInvTimeRes())
	{
		/* タイムを削除 */
		FRM_TimerCancelOneShot(EOTA_TIMEID_INV_RES);
		/* 一回応答タイム未実施設定 */
		EOTA_SetInvTimeRes(EOTA_TIME_NULL);
	}
	else
	{
		/* 処理無し */
	}

	/* 通信タイムアウト実施の場合 */
	if  (EOTA_TIME_ING == EOTA_GetInvTimeOut())
	{
		/* タイムを削除 */
		FRM_TimerCancelOneShot(EOTA_TIMEID_INV_TIME_OUT);
		/* 一回応答タイム未実施設定 */
		EOTA_SetInvTimeOut(EOTA_TIME_NULL);
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	前回のメッセージ送信								*/
/*--------------------------------------------------------------------------*/
/*	param			:	前回のメッセージ									*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	INVへメッセージを送信する							*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
void eota_SendInvBeforeMess()
{
	EOTA_CMD11_REQ_INFO_T message;

	DEBUG_TRCIN();

	/* タイマー起動 */
	eota_TimeStart();

	/* 前回のメッセージ取得 */
	EOTA_GetBefMessage(&message);

	/* INV送信共通関数 */
	EOTA_SendInvCMD11(&message);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	FW 転送終了処理										*/
/*--------------------------------------------------------------------------*/
/*	param			:	受信メッセージ										*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_INV_OK	処理正常								*/
/*				 		EOTA_INV_NG	処理異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW 転送終了受信処理を実施する						*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar eota_FWTranStateEnd(
					EOTA_CMD11_RES_INFO_T * Message) /* [i]受信メッセージ	*/
{
	uchar	Ret			= EOTA_INV_OK;		/* 戻り値		*/
	uchar	AckNak		= 0u;				/* ACK/NAK		*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == Message)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_INV_NG;
		return Ret;
	}

	/* Ack/Nak取得 */
	AckNak = Message->AckNak;

	/* パラメータチェック */
	if (NULL == Message)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_INV_NG;
		return Ret;
	}

	switch (AckNak)
	{
	/* 0:NAK */
	case EOTA_NAK:
		/* FW 転送終了 */
		Ret = eota_SendInvMessStop();
		break;
	/* 1:ACK */
	case EOTA_ACK:
		/* FW 転送シーケンスを終了 TODO. */


		break;
	/* 2:Busy */
	case EOTA_BUSY:
		/* Busy送信 */
		Ret = eota_SendInvMessBusy();
		break;
	/* 3:再送要求 */
	case EOTA_RE_SEND:
		/* 存在しない */
		break;
	/* 4:F:リトライ要求 */
	case EOTA_F_RE_SEND:
		/* 存在しない */
		break;
	default:
		break;
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	メッセージ送信処理(FW 転送問い合わせ)				*/
/*--------------------------------------------------------------------------*/
/*	param			:	送信対象											*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_INV_OK	処理正常								*/
/*				 		EOTA_INV_NG	処理異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	INVへメッセージを送信する							*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar eota_SendInvMessInq(uchar Tgt) 	/* [i]送信対象			*/
{
	uchar	Ret			= EOTA_INV_OK;	/* 戻り値					*/
	uchar	MessRet		= EOTA_DATA_NG;	/* メッセージ作成の戻り値	*/
	EOTA_CMD11_REQ_INFO_T Mssage;

	DEBUG_TRCIN();

	/* メッセージ初期化 */
	memset(&Mssage, 0, sizeof(EOTA_CMD11_REQ_INFO_T));

	/* メッセージ作成 */
 	MessRet = EOTA_SetCMD11Inquiry(Tgt, &Mssage);

	/* メッセージ作成OKの場合 */
	if (EOTA_DATA_OK == MessRet)
	{
		/* タイマー起動 */
		eota_TimeStart();
		/* INV送信共通関数 */
		EOTA_SendInvCMD11(&Mssage);
	}
	/* メッセージ作成NGの場合 */
	else
	{
		Ret = EOTA_INV_NG;
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	メッセージ送信処理(FW 転送開始)						*/
/*--------------------------------------------------------------------------*/
/*	param			:	送信対象											*/
/*						BANK No												*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_INV_OK:処理正常								*/
/*						EOTA_INV_NG:処理異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	INVへメッセージを送信する							*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar eota_SendInvMessStart(uchar  Tgt, 		/* [i]送信対象		*/
						    uchar  BankNo)  	/* [i]BANK No		*/
{
	uchar	Ret			= EOTA_INV_OK;	/* 戻り値					*/
	uchar	MessRet		= EOTA_DATA_NG;	/* メッセージ作成の戻り値	*/
	EOTA_CMD11_REQ_INFO_T Mssage;

	DEBUG_TRCIN();

	/* メッセージ初期化 */
	memset(&Mssage, 0, sizeof(EOTA_CMD11_REQ_INFO_T));

	/* メッセージ作成 */
 	MessRet = EOTA_SetCMD11Start(Tgt, BankNo, &Mssage);

	/* メッセージ作成OKの場合 */
	if (EOTA_DATA_OK == MessRet)
	{
		/* タイマー起動 */
		eota_TimeStart();
		/* INV送信共通関数 */
		EOTA_SendInvCMD11(&Mssage);
	}
	/* メッセージ作成NGの場合 */
	else
	{
		Ret = EOTA_INV_NG;
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	メッセージ送信処理(FW 転送)							*/
/*--------------------------------------------------------------------------*/
/*	param			:	DataNo												*/
/* 						送信対象											*/
/*						BANK No												*/
/* 						ファイル操作開始位置								*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_INV_OK:処理正常								*/
/*						EOTA_INV_NG:処理異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	INVへメッセージを送信する							*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar eota_SendInvMessTran(ulong  DataNo, 		/* [i]DataNo		 		*/
						   uchar  Tgt, 			/* [i]送信対象				*/
						   uchar  BankNo, 		/* [i]BANK No			 	*/
						   slong  Offset) 		/* [i]ファイル操作開始位置	*/
{
	uchar	Ret					= EOTA_INV_OK;	/* 戻り値					*/
	uchar	MessRet				= EOTA_DATA_NG;
											  	/* メッセージ作成の戻り値	*/
	uchar	FileRet			= EOTA_FILE_OK;
											 	/* ファイル操作の戻り値		*/
	schar	TarPath[EOTA_TAR_FILE_DIR_LEN + 1]
								= {0};		 	/* 更新ファイルフォルダ		*/
	schar*	p_tarpath 			= TarPath;	 	/* ポイント					*/
	schar	File[EOTA_SETUP_FILE_SIZE + 1]
								= {0};		 	/* ファイル名				*/
	schar*	FilePonit 			= File; 	 	/* ファイルのポイント		*/
	uchar   Data[EOTA_FW_DATA_SIZE]
								= {0};			/* 送信データ				*/
	size_t	Len					= 0;		 	/* 送信データサイズ			*/
	EOTA_CMD11_REQ_INFO_T Mssage;

	DEBUG_TRCIN();

	/* メッセージ初期化 */
	memset(&Mssage, 0, sizeof(EOTA_CMD11_REQ_INFO_T));

	/* 更新ファイルフォルダパス取得 */
	EOTA_GetUpdFileDri(&p_tarpath);

	/* ファイル名取得 */
	switch (Tgt)
	{
	/* 1:INV */
	case EOTA_UPD_TGT_INV:
		/* BAK1の場合 */
		if (EOTA_BANK_NO_1 == BankNo)
		{
			EOTA_GetInvBank1File(&FilePonit);
		}
		/* BAK2の場合 */
		else if (EOTA_BANK_NO_2 == BankNo)
		{
			EOTA_GetInvBank2File(&FilePonit);
		}
		else
		{
			/* 処理無し */
		}

		break;
	/* 2:PV */
	case EOTA_UPD_TGT_PV:
		/* BAK1の場合 */
		if (EOTA_BANK_NO_1 == BankNo)
		{
			EOTA_GetPvBank1File(&FilePonit);
		}
		/* BAK2の場合 */
		else if (EOTA_BANK_NO_2 == BankNo)
		{
			EOTA_GetPvBank2File(&FilePonit);
		}
		else
		{
			/* 処理無し */
		}

		break;

	/* 3:BDC */
	case EOTA_UPD_TGT_BDC:
		/* BAK1の場合 */
		if (EOTA_BANK_NO_1 == BankNo)
		{
			EOTA_GetBdcBank1File(&FilePonit);
		}
		/* BAK2の場合 */
		else if (EOTA_BANK_NO_2 == BankNo)
		{
			EOTA_GetBdcBank2File(&FilePonit);
		}
		else
		{
			/* 処理無し */
		}
		break;
	/* 4:J.B. */
	case EOTA_UPD_TGT_J_B:
		EOTA_GetBdcBank2File(&FilePonit);
		break;
	default:
		break;
	}

	/* ファイルパス作成 */
	strncat(p_tarpath, FilePonit, strlen(FilePonit));
	/* ファイルデータ取得 */
	FileRet = EOTA_GetFileData(p_tarpath, Offset, &Len, Data);

	if (EOTA_FILE_OK == FileRet)
	{
		/* メッセージ作成 */
 		MessRet = EOTA_SetCMD11Send(DataNo, Data, Len, &Mssage);
	}
	else
	{
		Ret = EOTA_INV_NG;
	}

	/* メッセージ作成OKの場合 */
	if ((EOTA_INV_OK == Ret) && (EOTA_DATA_OK == MessRet))
	{
		/* タイマー起動 */
		eota_TimeStart();
		/* INV送信共通関数 */
		EOTA_SendInvCMD11(&Mssage);
	}
	/* メッセージ作成NGの場合 */
	else
	{
		Ret = EOTA_INV_NG;
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	メッセージ送信処理(FW 転送完了)						*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_INV_OK	処理正常								*/
/*						EOTA_INV_NG	処理異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	INVへメッセージを送信する							*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar eota_SendInvMessEnd()
{
	uchar	Ret			= EOTA_INV_OK;			/* 戻り値					*/
	uchar	MessRet		= EOTA_DATA_NG;			/* メッセージ作成の戻り値	*/
	EOTA_CMD11_REQ_INFO_T Mssage;

	DEBUG_TRCIN();

	/* メッセージ初期化 */
	memset(&Mssage, 0, sizeof(EOTA_CMD11_REQ_INFO_T));

	/* メッセージ作成 */
	MessRet = EOTA_SetCMD11End(&Mssage);

	/* メッセージ作成OKの場合 */
	if (EOTA_DATA_OK == MessRet)
	{
		/* タイマー起動 */
		eota_TimeStart();
		/* INV送信共通関数 */
		EOTA_SendInvCMD11(&Mssage);
	}
	/* メッセージ作成NGの場合 */
	else
	{
		Ret = EOTA_INV_NG;
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	メッセージ送信処理(FW 転送終了)						*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_INV_OK	処理正常								*/
/*				 		EOTA_INV_NG	処理異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	INVへメッセージを送信する							*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar eota_SendInvMessStop()
{
	uchar	Ret			= EOTA_INV_OK;			/* 戻り値					*/
	uchar	MessRet		= EOTA_DATA_NG;			/* メッセージ作成の戻り値	*/
	EOTA_CMD11_REQ_INFO_T Mssage;

	DEBUG_TRCIN();

	/* メッセージ初期化 */
	memset(&Mssage, 0, sizeof(EOTA_CMD11_REQ_INFO_T));

	/* メッセージ作成 */
	MessRet = EOTA_SetCMD11Stop(&Mssage);

	/* メッセージ作成OKの場合 */
	if (EOTA_DATA_OK == MessRet)
	{
		/* タイマー起動 */
		eota_TimeStart();
		/* INV送信共通関数 */
		EOTA_SendInvCMD11(&Mssage);
	}
	/* メッセージ作成NGの場合 */
	else
	{
		Ret = EOTA_INV_NG;
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	メッセージ送信処理(Busy送信)						*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_INV_OK:処理正常								*/
/*						EOTA_INV_NG:処理異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	INVへメッセージを送信する							*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar eota_SendInvMessBusy()
{
	uchar	Ret			= EOTA_INV_OK;			/* 戻り値					*/
	uchar	MessRet		= EOTA_DATA_NG;			/* メッセージ作成の戻り値	*/
	EOTA_CMD11_REQ_INFO_T Mssage;

	DEBUG_TRCIN();

	/* メッセージ初期化 */
	memset(&Mssage, 0, sizeof(EOTA_CMD11_REQ_INFO_T));

	/* メッセージ作成 */
	MessRet = EOTA_SetCMD11Busy(&Mssage);

	/* メッセージ作成OKの場合 */
	if (EOTA_DATA_OK == MessRet)
	{
		/* タイマー起動 */
		eota_TimeStart();
		/* INV送信共通関数 */
		EOTA_SendInvCMD11(&Mssage);
	}
	/* メッセージ作成NGの場合 */
	else
	{
		Ret = EOTA_INV_NG;
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	FW 転送継続処理										*/
/*--------------------------------------------------------------------------*/
/*	param			:	受信メッセージ										*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_INV_OK:処理正常								*/
/*						EOTA_INV_NG:処理異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW 転送継続受信処理を実施する						*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar eota_FWTranStateCon(
					EOTA_CMD11_RES_INFO_T * Message) 	/* [i]受信メッセージ */
{
	uchar	Ret			= EOTA_INV_OK;				/* 戻り値				*/
	ulong	DataNo		= 0u;						/* DataNo				*/
	slong	NextDataSize= 0u;						/* 次のデータサイズ		*/
	uchar	AckNak		= 0u;						/* ACK/NAK				*/
	uchar	BankNo		= 0u;						/* BankNo				*/
	slong 	FileSize	= 0;						/* ファイルサイズ		*/

	DEBUG_TRCIN();

	/* DataNo取得 */
	memcpy(&DataNo, Message->DataNo, sizeof(ushort));
	/* BankNo取得 */
	BankNo = Message->BankNo;
	/* Ack/Nak取得 */
	AckNak = Message->AckNak;

	/* パラメータチェック */
	if (NULL == Message)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_INV_NG;
		return Ret;
	}

	switch (AckNak)
	{
	/* 0:NAK */
	case EOTA_NAK:
		/* FW 転送終了 */
		Ret = eota_SendInvMessStop();
		break;
	/* 1:ACK */
	case EOTA_ACK:
		/* FW 転送シーケンス継続 */
		switch (DataNo)
		{
		/* INVマイコン転送問い合わせ */
		case EOTA_DATANO_INV_INQ:
			/* FW 転送開始送信 */
			Ret = eota_SendInvMessStart(EOTA_UPD_TGT_INV, BankNo);
			break;
		/* PVマイコン転送問い合わせ */
		case EOTA_DATANO_PV_INQ:
			/* FW 転送開始送信 */
			Ret = eota_SendInvMessStart(EOTA_UPD_TGT_PV, BankNo);
			break;
		/* BDCマイコン転送問い合わせ */
		case EOTA_DATANO_BDC_INQ:
			/* FW 転送開始送信 */
			Ret = eota_SendInvMessStart(EOTA_UPD_TGT_BDC, BankNo);
			break;
		/* J.B.マイコン転送問い合わせ */
		case EOTA_DATANO_JB_INQ:
			/* FW 転送開始送信 */
			Ret = eota_SendInvMessStart(EOTA_UPD_TGT_J_B, BankNo);
			break;
		/* FW 転送開始 */
		case EOTA_DATANO_FW_S:
			/* DataNo */
			DataNo = 0x0001;
			/* 転送中ファイルoffsetを0に設定 */
			EOTA_SetUpdFileOffset(0);
			/* FW 転送 */
			Ret = eota_SendInvMessTran(DataNo, EOTA_GetUpdTgt(),
										BankNo, EOTA_GetUpdFileOffset());
			break;
		/* FW 転送終了 */
		case EOTA_DATANO_FW_E:
			/* CMD60送信 TODO. */
			EOTA_SendInvCMD60( );

			break;
		/* FW 転送完了 */
		case EOTA_DATANO_FW_F:
			/* FW 転送終了 */
			Ret = eota_SendInvMessStop();
			break;
		default:
			/* FW 転送 */
			if ((EOTA_DATANO_FW_NUM_S <= DataNo)
				&& (EOTA_DATANO_FW_NUM_E >= DataNo))
			{
				NextDataSize = EOTA_GetUpdFileOffset() + EOTA_FW_DATA_SIZE;

				/* 転送対象 */
				switch (EOTA_GetUpdTgt())
				{
				/* 1:INV */
				case EOTA_UPD_TGT_INV:
					/* BAK1の場合 */
					if (EOTA_BANK_NO_1 == BankNo)
					{
						FileSize = EOTA_GetInvBank1FileSize();
					}
					/* BAK2の場合 */
					else if (EOTA_BANK_NO_2 == BankNo)
					{
						FileSize = EOTA_GetInvBank1FileSize();
					}
					else
					{
						/* 処理無し */
					}

					break;
				/* 2:PV */
				case EOTA_UPD_TGT_PV:
					/* BAK1の場合 */
					if (EOTA_BANK_NO_1 == BankNo)
					{
						FileSize = EOTA_GetPVBank1FileSize();
					}
					/* BAK2の場合 */
					else if (EOTA_BANK_NO_2 == BankNo)
					{
						FileSize = EOTA_GetPVBank2FileSize();
					}
					else
					{
						/* 処理無し */
					}

					break;

				/* 3:BDC */
				case EOTA_UPD_TGT_BDC:
					/* BAK1の場合 */
					if (EOTA_BANK_NO_1 == BankNo)
					{
						FileSize = EOTA_GetBdcBank1FileSize();
					}
					/* BAK2の場合 */
					else if (EOTA_BANK_NO_2 == BankNo)
					{
						FileSize = EOTA_GetBdcBank2FileSize();
					}
					else
					{
						/* 処理無し */
					}
					break;

				/* 4:J.B. */
				case EOTA_UPD_TGT_J_B:
					FileSize = EOTA_GetJbFileSize();
					break;
				default:
					break;
				}

				/* 次のデータ有無確認 */
				if (NextDataSize < FileSize)
				{
					/* DataNo */
					DataNo += 1;
					/* 転送中ファイルoffset設定 */
					EOTA_SetUpdFileOffset(NextDataSize);
					/* FW 転送 */
					Ret = eota_SendInvMessTran(DataNo,
												EOTA_GetUpdTgt(),
												BankNo,
												EOTA_GetUpdFileOffset());
				}
				else
				{
					/* FW 転送完了 */
					Ret = eota_SendInvMessEnd();
				}
			}
			break;
		}
		break;

	/* 2:Busy */
	case EOTA_BUSY:
		/* Busy送信 */
		Ret = eota_SendInvMessBusy();
		break;
	/* 3:再送要求 */
	case EOTA_RE_SEND:
		/* 直前に送信した DataNo を送信 */
		eota_SendInvBeforeMess();
		break;
	/* 4:F:リトライ要求 */
	case EOTA_F_RE_SEND:
		/* 直前に送信した DataNo  */
		DataNo = 0x0001;
		/* 転送中ファイルoffsetを0に設定 */
		EOTA_SetUpdFileOffset(0);
		/* FW 転送 */
		Ret = eota_SendInvMessTran(DataNo,
									EOTA_GetUpdTgt(),
									BankNo,
									EOTA_GetUpdFileOffset());

		break;
	default:
		break;
	}

	DEBUG_TRCOUT();

	return Ret;
}
