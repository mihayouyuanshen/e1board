/****************************************************************************/
/*	Description		:FW更新機能 処理流れ									*/
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
#include "eota_log_g.h"
#include "eota_check_g.h"
#include "eota_file_g.h"
#include "eota_data_g.h"
#include "eota_serv_g.h"
#include "eota_inv_g.h"
#include "eota_setting_g.h"
#include "eota_serv.h"

#include "eota_flow.h"

/*--------------------------------------------------------------------------*/
/* 公開変数定義																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* 非公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* 公開関数定義																*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
static void * EOTA_CommThreadStartUpdate(void *arg);

/*==========================================================================*/
/*	Description		:	状態遷移											*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	状態遷移を実施する									*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
void EOTA_Flow(void)
{
	uchar UpdateState = EOTA_UPD_WAIT;		/* 更新状態初期化	*/

	DEBUG_TRCIN();

	UpdateState = EOTA_GetUpdateState();

	/* V:FW 更新状態 0:待機 */
	switch(UpdateState)
	{
	/* 待機状態 */
	case EOTA_UPD_WAIT:
		/* 待機 */
		eota_Wait();
		break;

	/* ダウンロード・解析 */
	case EOTA_UPD_DOWN:
		/* ダウンロード・解析  */
		eota_DownAnaly();
		break;

	/* FW 更新開始 */
	case EOTA_UPD_START:
		/* FW 更新開始  */
		eota_UpdateStart();
		break;

	/* FW 更新中 */
	case EOTA_UPD_ING:
		eota_Update();
		break;

	/* FW 更新終了 */
	case EOTA_UPD_END:
		/* FW 更新終了 */
		eota_UpdateEnd();
		break;

	default:
		break;
	}

	DEBUG_TRCOUT();
}

/*--------------------------------------------------------------------------*/
/* 非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:	待機												*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	待機処理を実施する									*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
static void eota_Wait(void)
{
	DEBUG_TRCIN();


	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	ダウンロード・解析									*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_OK	正常終了									*/
/* 						EOTA_NG	予期せぬエラー								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ダウンロード・解析を実施する						*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
static schar eota_DownAnaly(void)
{
	schar Ret  		= EOTA_OK;							/* 戻り値			*/
	uchar FileRet	= EOTA_FILE_OK;						/* ファイルの戻り値 */
	schar UpdTime[EOTA_SETUP_TIME_SIZE + 1] = {0};	  	/* 更新時間			*/
	schar * UpdTime_p 						= UpdTime;	/* 更新時間ポイント	*/
	uchar CheckRet	= EOTA_CHECK_OK;					/* チェックの戻り値	*/

	DEBUG_TRCIN();
	/* LED:V:FW 更新状態=1:ダウンロード・解析 */
	EOTA_Set_Led(EOTA_DL_ANA_SW);

	/* 初期値設定 */
	/* V:BOARD_tgt初期値設定(0:対象外) */
	EOTA_SetTgtBoard(EOTA_TGT_FLG_OFF);
	/* V:INV_tgt初期値設定(0:対象外) */
	EOTA_SetTgtInv(EOTA_TGT_FLG_OFF);
	/* V:PV_tgt初期値設定(0:対象外) */
	EOTA_SetTgtPV(EOTA_TGT_FLG_OFF);
	/* V:BDC_tgt初期値設定(0:対象外) */
	EOTA_SetTgtBDC(EOTA_TGT_FLG_OFF);
	/* V:J.B._tgt初期値設定(0:対象外) */
	EOTA_SetTgtJB(EOTA_TGT_FLG_OFF);
	/* V:解析結果初期値設定(0:未実施) */
	EOTA_SetAnalyRes(EOTA_UPD_RES_NULL);
	/* V:削除フラグ初期値設定(0:なし) */
	EOTA_SetDownDelFlg(EOTA_DOWN_DEL_FLG_OFF);

	/* 既存の FW 更新ファイルを削除 */
	FileRet = EOTA_DelFWFile(EOTA_FW_VER_NUP_FILE_PATH);

	if (EOTA_FILE_OK != FileRet)
	{
		Ret = EOTA_NG;
	}
	else
	{
		/* LAN:FW ダウンロード処理状態で FW 更新全体の進捗を応答 */
		/* 進捗率(ダウンロード中) */
		EOTA_SetProgressRate(EOTA_PRO_RATE_DOWN_ING);

		/* ダウンロード経由 */
		switch(EOTA_GetDownSrc())
		{
		/* USB経由 */
		case EOTA_START_MODE_USB:
			Ret = eota_DownloadUSB();
			break;

		/* 見守り経由 */
		case EOTA_START_MODE_SUB:
			eota_DownloadSUB();
			break;

		/* スマホアプリ経由 */
		case EOTA_START_MODE_PHONE:
			Ret = eota_DownloadAPP();
			break;

		/* USB 経由 かつ タクト SW */
		case EOTA_START_MODE_SW:
			Ret = eota_DownloadSW();
			break;

		default:
			/* 処理無し */
			break;
		}
	}

	/* 異常の場合、FW 更新処理は終了 */
	if (EOTA_OK != Ret)
	{
		/* FW 更新実施状態:End */
		EOTA_SetRunState(EOTA_RUN_STATE_E);

		/* LED:その他の異常 */
		EOTA_Set_Led(EOTA_OTHER_FAIL);

		/* V:FW 更新状態 0:待機 */
		EOTA_SetUpdateState(EOTA_UPD_WAIT);
	}
	else
	{
		/* V:解析結果=1:OK */
		/* FW 更新実施時刻取得 */
		EOTA_GetUpdTime(&UpdTime_p);
		/* 時刻チェック */
		CheckRet = EOTA_UpdateCheckTime(UpdTime_p);

		/* V:更新 No≠0 AND (V:更新 Mode=1:U OR V:更新 Mode=2:F)
			AND V:FW 更新実施時刻≧現在時刻 */
		if(    (0 != EOTA_GetUpdNO())
			&& ((EOTA_UPD_MODE_U == EOTA_GetUpdMode()) 
				|| (EOTA_UPD_MODE_F == EOTA_GetUpdMode()))
			&& ( EOTA_CHECK_OK == CheckRet))
		{
			EOTA_SetUpdateState(EOTA_UPD_START);

			/* 更新開始に遷移 */
			EOTA_Flow();
		}
		else
		{
			/* V:FW 更新状態 0:待機 */
			EOTA_SetUpdateState(EOTA_UPD_WAIT);
		}
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	ダウンロード(スマホからUSB 経由) 					*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/* 						EOTA_OK				正常終了						*/
/* 						EOTA_NG				予期せぬエラー					*/
/*						EOTA_DL_FW_FILE_NO	FW 対象なし						*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ダウンロードを実施する								*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
static schar eota_DownloadUSB(void)
{
	schar Ret  							 		= EOTA_OK;
												/* 戻り値					*/
	uchar FileRet						 		= EOTA_FILE_OK;
												/* ファイルの戻り値 		*/
	schar  TarFile[EOTA_PKG_TGZ_NAME_LEN] 		= {0};
												/* 圧縮ファイル名			*/
	schar  FilePathUsb[EOTA_TAR_FILE_DIR_LEN]	= {0};
												/* 圧縮ファイルパス(USB)	*/
	schar  FilePathNand[EOTA_TAR_FILE_DIR_LEN]	= {0};
												/* 圧縮ファイルパス(NSND)	*/

	DEBUG_TRCIN();

	/* ダウンロード開始(スマホへ) */
	EOTA_SetDownLoadState(EOTA_DL_STATE_START);

	/* USB のルートディレクトリ内に圧縮ファイルが複数ある場合は
		xxx が最も大きいものをダウンロード */
	FileRet = EOTA_GetTarFile(EOTA_FW_USB_FILE_PATH, TarFile);
	/* 402:FW 対象なし */
	if (EOTA_FILE_TAR_NOT_ERR == FileRet)
	{
		Ret = EOTA_DL_FW_FILE_NO;
		/* 更新対象なし(スマホへ) */
		EOTA_SetDownLoadState(EOTA_DL_STATE_UPD_NO);
	}
	/* 301:その他失敗 */
	else if (EOTA_FILE_OK != FileRet)
	{
		Ret = EOTA_NG;

		/* エラー(スマホへ) */
		EOTA_SetDownLoadState(EOTA_DL_STATE_ERR);
	}
	/* 303:中止 */
	else if (EOTA_UPDATE_STOP_ON == EOTA_GetUpdStop())
	{
		Ret = EOTA_UPD_STOP;

		/* TODO. */
		// EOTA_SetDownLoadState(EOTA_DL_STATE_ERR);
	}
	else
	{
		Ret = EOTA_OK;
	}

	/* FW 更新の圧縮ファイルをダウンロード */
	if (EOTA_OK == Ret)
	{
		/* ダウンロード busy 状態(スマホへ) */
		EOTA_SetDownLoadState(EOTA_DL_STATE_BUSY);

		sprintf(FilePathNand, "%s/%s", EOTA_FW_VER_NUP_FILE_PATH, TarFile);
		sprintf(FilePathUsb, "%s/%s", EOTA_FW_USB_FILE_PATH, TarFile);

		/* USB→NAND */
		FileRet = EOTA_TarCopy(FilePathUsb, FilePathNand);
		/* 解凍失敗の場合、エラーを戻る */
		if (EOTA_FILE_OK != FileRet)
		{
			Ret = EOTA_NG;

			/* エラー送信(スマホへ) */
			EOTA_SetDownLoadState(EOTA_DL_STATE_ERR);
		}
		else
		{
			/* 処理無し */
		}
	}
	else
	{
		/* 処理無し */
	}

	/* ファイルを解凍 */
	if (EOTA_OK == Ret)
	{
		/* 解凍 */
		FileRet = EOTA_TarunZip(FilePathNand);
		/* 解凍失敗の場合、エラーを戻る */
		if (EOTA_FILE_OK != FileRet)
		{
			Ret = EOTA_NG;

			/* エラー送信(スマホへ) */
			EOTA_SetDownLoadState(EOTA_DL_STATE_ERR);
		}
		else
		{
			/* 処理無し */
		}
	}
	else
	{
		/* 処理無し */
	}

	/* 圧縮ファイルの解凍と圧縮ファイルの解析 */
	if (EOTA_OK == Ret)
	{
		/* ダウンロード完了(スマホへ) */
		EOTA_SetDownLoadState(EOTA_DL_STATE_START);

		/* 解析 */
		eota_Analyze(FilePathNand);
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	ダウンロード(見守り経由) 							*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ダウンロードを実施する								*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
static void eota_DownloadSUB(void)
{
	schar Ret  									= EOTA_OK;
												/* 戻り値					*/
	uchar FileRet	 							= EOTA_FILE_OK;
												/* ファイルの戻り値 		*/
	schar FilePathNand[EOTA_TAR_FILE_DIR_LEN]	= {0};
												/* 圧縮ファイルパス(NSND)	*/
	schar TarFile[EOTA_PKG_TGZ_NAME_LEN] 		= {0};
												/* 圧縮ファイル名			*/
	EOTA_SUB_UPD_INFO_T  UpdInfo;				/* 見守り送信情報			*/
	memset(&UpdInfo, 0, sizeof(EOTA_SUB_UPD_INFO_T));

	DEBUG_TRCIN();

	/* NAND のルートディレクトリ内に圧縮ファイルが複数ある場合は
		xxx が最も大きいものをダウンロード */
	FileRet = EOTA_GetTarFile(EOTA_FW_VER_NUP_FILE_PATH, TarFile);

	/* 402:FW 対象なし */
	if (EOTA_FILE_TAR_NOT_ERR == FileRet)
	{
		Ret = EOTA_DL_FW_FILE_NO;

		/* 更新対象なし、見守りへ送信 */
		memset(&UpdInfo, 0, sizeof(EOTA_SUB_UPD_INFO_T));
		UpdInfo.UpdState = EOTA_CHECK_UPDATE_SETUP;
		UpdInfo.Err = EOTA_DL_FW_FILE_NO_SUB;

		EOTA_SetUpdInfoSub(&UpdInfo);
	}
	/* 301:その他失敗 */
	else if (EOTA_FILE_OK != FileRet)
	{
		Ret = EOTA_NG;

		/* エラーの場合、見守りへ送信 */
		memset(&UpdInfo, 0, sizeof(EOTA_DL_STATE_ERR));
		UpdInfo.UpdState = EOTA_CHECK_UPDATE_SETUP;
		UpdInfo.Err = EOTA_DL_ERR_SUB;

		EOTA_SetUpdInfoSub(&UpdInfo);
	}
	else
	{
		Ret = EOTA_OK;
	}

	if ( EOTA_OK == Ret)
	{
		/* 解凍 */
		FileRet = EOTA_TarunZip(FilePathNand);
		/* 解凍失敗の場合、エラーを戻る */
		if (EOTA_FILE_OK != FileRet)
		{
			Ret = EOTA_NG;

			/* エラーの場合、見守りへ送信 */
			memset(&UpdInfo, 0, sizeof(EOTA_DL_STATE_ERR));
			UpdInfo.UpdState = EOTA_CHECK_UPDATE_SETUP;
			UpdInfo.Err = EOTA_DL_ERR_SUB;

			EOTA_SetUpdInfoSub(&UpdInfo);
		}
		else
		{
			/* 処理無し */
		}
	}

	/* 圧縮ファイルの解凍と圧縮ファイルの解析 */
	if (EOTA_OK == Ret)
	{
		/* ダウンロード完了 */
		memset(&UpdInfo, 0, sizeof(EOTA_DL_STATE_ERR));
		UpdInfo.UpdState = EOTA_DL_STATE_START;

		EOTA_SetUpdInfoSub(&UpdInfo);

		/* 解析 */
		eota_Analyze(FilePathNand);
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

}

/*==========================================================================*/
/*	Description		:	ダウンロード(スマホアプリ経由) 						*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ダウンロードを実施する								*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
static schar eota_DownloadAPP(void)
{
	schar Ret  									= EOTA_OK;
												/* 戻り値					*/
	uchar FileRet	 							= EOTA_FILE_OK;
												/* ファイルの戻り値 		*/
	schar FilePathNand[EOTA_TAR_FILE_DIR_LEN]	= {0};
												/* 圧縮ファイルパス(NSND)	*/
	schar TarFile[EOTA_PKG_TGZ_NAME_LEN] 		= {0};
												/* 圧縮ファイル名			*/

	DEBUG_TRCIN();

	/* ダウンロード開始(スマホへ) */
	EOTA_SetDownLoadState(EOTA_DL_STATE_START);

	/* NAND のルートディレクトリ内に圧縮ファイルが複数ある場合は
		xxx が最も大きいものをダウンロード */
	FileRet = EOTA_GetTarFile(EOTA_FW_VER_NUP_FILE_PATH, TarFile);
	/* 402:FW 対象なし */
	if (EOTA_FILE_TAR_NOT_ERR == FileRet)
	{
		Ret = EOTA_DL_FW_FILE_NO;

		/* 更新対象なし(スマホへ) */
		EOTA_SetDownLoadState(EOTA_DL_STATE_UPD_NO);
	}
	/* 301:その他失敗 */
	else if (EOTA_FILE_OK != FileRet)
	{
		Ret = EOTA_NG;

		/* エラー(スマホへ) */
		EOTA_SetDownLoadState(EOTA_DL_STATE_ERR);
	}
	/* 303:中止 */
	else if (EOTA_UPDATE_STOP_ON == EOTA_GetUpdStop())
	{
		Ret = EOTA_UPD_STOP;

		/* TODO. */
		// EOTA_SetDownLoadState(EOTA_DL_STATE_ERR);
	}
	else
	{
		Ret = EOTA_OK;
	}

	if ( EOTA_OK == Ret)
	{
		/* ダウンロード busy 状態(スマホへ) */
		EOTA_SetDownLoadState(EOTA_DL_STATE_BUSY);

		/* 解凍 */
		FileRet = EOTA_TarunZip(FilePathNand);
		/* 解凍失敗の場合、エラーを戻る */
		if (EOTA_FILE_OK != FileRet)
		{
			Ret = EOTA_NG;

			/* エラー送信(スマホへ) */
			EOTA_SetDownLoadState(EOTA_DL_STATE_ERR);
		}
		else
		{
			/* 処理無し */
		}
	}

	/* 圧縮ファイルの解凍と圧縮ファイルの解析 */
	if (EOTA_OK == Ret)
	{
		/* 解析 */
		eota_Analyze(FilePathNand);
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	ダウンロード(USB 経由 かつ タクト SW経由) 			*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ダウンロードを実施する								*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
static schar eota_DownloadSW(void)
{
	schar Ret  							 		= EOTA_OK;
												/* 戻り値					*/
	uchar FileRet						 		= EOTA_FILE_OK;
												/* ファイルの戻り値 		*/
	schar  TarFile[EOTA_PKG_TGZ_NAME_LEN] 		= {0};
												/* 圧縮ファイル名			*/
	schar  FilePathUsb[EOTA_TAR_FILE_DIR_LEN]	= {0};
												/* 圧縮ファイルパス(USB)	*/
	schar  FilePathNand[EOTA_TAR_FILE_DIR_LEN]	= {0};
												/* 圧縮ファイルパス(NSND)	*/

	DEBUG_TRCIN();

	/* USB のルートディレクトリ内に圧縮ファイルが複数ある場合は
			xxx が最も大きいものをダウンロード */
	FileRet = EOTA_GetTarFile(EOTA_FW_USB_FILE_PATH, TarFile);
	/* 402:FW 対象なし */
	if (EOTA_FILE_TAR_NOT_ERR == FileRet)
	{
		Ret = EOTA_DL_FW_FILE_NO;
	}
	/* 301:その他失敗 */
	else if (EOTA_FILE_OK != FileRet)
	{
		Ret = EOTA_NG;
	}
	/* 303:中止 */
	else if (EOTA_UPDATE_STOP_ON == EOTA_GetUpdStop())
	{
		Ret = EOTA_UPD_STOP;
	}
	else
	{
		Ret = EOTA_OK;
	}

	/* FW 更新の圧縮ファイルをダウンロード */
	if (EOTA_OK == Ret)
	{
		sprintf(FilePathNand, "%s/%s", EOTA_FW_VER_NUP_FILE_PATH, TarFile);
		sprintf(FilePathUsb, "%s/%s", EOTA_FW_USB_FILE_PATH, TarFile);

		/* USB→NAND */
		FileRet = EOTA_TarCopy(FilePathUsb, FilePathNand);
		/* 解凍失敗の場合、エラーを戻る */
		if (EOTA_FILE_OK != FileRet)
		{
			Ret = EOTA_NG;
		}
		else
		{
			/* 処理無し */
		}
	}
	else
	{
		/* 処理無し */
	}

	/* ファイルを解凍 */
	if (EOTA_OK == Ret)
	{
		/* 解凍 */
		FileRet = EOTA_TarunZip(FilePathNand);
		/* 解凍失敗の場合、エラーを戻る */
		if (EOTA_FILE_OK != FileRet)
		{
			Ret = EOTA_NG;
		}
		else
		{
			/* 処理無し */
		}
	}
	else
	{
		/* 処理無し */
	}

	/* 圧縮ファイルの解凍と圧縮ファイルの解析 */
	if (EOTA_OK == Ret)
	{
		/* 解析 */
		eota_Analyze(FilePathNand);
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	MD5ファイル確認										*/
/*--------------------------------------------------------------------------*/
/*	param			:	フォルダ											*/
/* 				 		MD5ファイル名										*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_FLOW_OK	処理正常							*/
/*				 		EOTA_FLOW_NG	処理異常							*/
/*--------------------------------------------------------------------------*/
/*	detail			:	MD5でファイルを確認									*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
static uchar eota_Md5FileCheck(const schar * Folder,	/* [i] フォルダ				*/
						const schar * FileName 	/* [i] MD5ファイル名		*/
					)
{
	uchar		Ret			= EOTA_FLOW_OK;		  /* 戻り値 			*/
	uchar		CheckRet	= EOTA_CHECK_OK;	  /* チェックの戻り値	*/
	schar		FilePath[EOTA_TAR_FILE_DIR_LEN]
							= {0};			 	  /* 更新ファイルパス 	*/
	size_t		StrSize		= 0;				  /* strlenサイズ		*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == Folder) || (NULL == FileName))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FLOW_NG;

		/* 関数を戻る */
		return Ret;
	}
	else
	{
		/* 処理無し */
	}

	/* MD5ファイルパス作成 */
	StrSize = strlen(FileName);
	strncpy(FilePath, Folder, strlen(Folder));
	strncat(FilePath, FileName, StrSize);

	/* MD5チェック */
	CheckRet = EOTA_CheckMD5(FilePath);
	if(EOTA_CHECK_OK == CheckRet)
	{
		Ret = EOTA_FLOW_OK;
	}
	else
	{
		Ret = EOTA_FLOW_NG;
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	更新ファイルサイズ取得								*/
/*--------------------------------------------------------------------------*/
/*	param			:	フォルダ											*/
/* 				 		更新ファイル名										*/
/* 				 		ファイルサイズ										*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_FLOW_OK	処理正常							*/
/*				 		EOTA_FLOW_NG	処理異常							*/
/*--------------------------------------------------------------------------*/
/*	detail			:	更新ファイルサイズを取得する						*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
static uchar eota_GetUpdFileSize(const schar * Folder,		/* [i] フォルダ			*/
						  const schar * FileName, 	/* [i] 更新ファイル名	*/
						  slong * FileSize			/* [o] ファイルサイズ	*/
					)
{
	uchar		Ret				= EOTA_FLOW_OK;		/* 戻り値 			*/
	schar		FilePath[EOTA_TAR_FILE_DIR_LEN]
								= {0};			  	/* 更新ファイルパス */
	size_t		StrSize			= 0;			  	/* strlenサイズ		*/

	DEBUG_TRCIN();

	/* 更新ファイルパス作成 */
	strncpy(FilePath, Folder, strlen(Folder));
	StrSize = strlen(FileName);
	strncat(FilePath, FileName, StrSize);
	*FileSize = EOTA_GetFileSize(FilePath);

	/* ファイルサイズがある場合 */
	if (EOTA_FIlE_READ_ERR < *FileSize)
	{
		Ret = EOTA_FLOW_OK;
	}
	else
	{
		Ret = EOTA_FLOW_NG;
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	TGTのBOARD解析										*/
/*--------------------------------------------------------------------------*/
/*	param			:	FW 更新指示ファイル情報								*/
/* 				 		更新ファイルフォルダ								*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_FLOW_OK	処理正常							*/
/*						EOTA_FLOW_NG	処理異常							*/
/*--------------------------------------------------------------------------*/
/*	detail			:	TGTのBOARDを解析する								*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
static uchar eota_AnalyzeBoard(
		EOTA_UPDATE_SETUP_T * UpdateSetup, 	/* [i] FW 更新指示ファイル情報	*/
		const schar * TarPath) 				/* [i] 更新ファイルフォルダ		*/
{
	uchar		Ret				= EOTA_FLOW_OK;		  /* 戻り値 			*/
	sint		atoi_ret		= 0;				  /* atoi戻り値			*/
	size_t		StrSize			= 0;				  /* strlenサイズ		*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == UpdateSetup) || (NULL == TarPath))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FLOW_NG;

		/* 関数を戻る */
		return Ret;
	}
	else
	{
		/* 処理無し */
	}

	atoi_ret = atoi(UpdateSetup->TgtBoard);

	/* TGTのBOARD=0？ */
	if (EOTA_TGT_FLG_OFF != atoi_ret)
	{
		/* FW指示ファイルの通信ボードFWバージョンが現在の通信ボードのバージョンと一致しない) */
		/* TODO. EEPROMから通信ボードのバージョンを取得 */
		#if 0
		CheckRet = EOTA_CheckUpdateVersion(UpdateSetup->BoardFw.Version, "V0001");

		if(EOTA_CHECK_OK == CheckRet)
		{
			Ret = EOTA_FLOW_OK;
		}
		else
		{
			Ret = EOTA_FLOW_NG;
		}
		#endif

		/* 通信ボードF WファイルがMD5ハッシュ値と一致する */
		if (EOTA_FLOW_OK == Ret)
		{
			StrSize = strlen(UpdateSetup->BoardFw.Md5File);
			if (0 < StrSize)
			{
				/* MD5チェック */
				Ret = eota_Md5FileCheck(TarPath, UpdateSetup->BoardFw.Md5File);
				if(EOTA_CHECK_OK == Ret)
				{
					/* V:BOARD_tgt=1:対象 */
					EOTA_SetTgtBoard(EOTA_TGT_FLG_ON);
				}
				else
				{
					Ret = EOTA_FLOW_NG;
				}
			}
			else
			{
				/* ファイルがない */
				Ret = EOTA_FLOW_NG;
			}
		}
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	TGTのINV解析										*/
/*--------------------------------------------------------------------------*/
/*	param			:	FW 更新指示ファイル情報								*/
/* 				 		更新ファイルフォルダ								*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_FLOW_OK	処理正常							*/
/*				 		EOTA_FLOW_NG	処理異常							*/
/*--------------------------------------------------------------------------*/
/*	detail			:	INVのBOARDを解析する								*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
static uchar eota_AnalyzeInv(
		EOTA_UPDATE_SETUP_T * UpdateSetup, 	/* [i] FW 更新指示ファイル情報	*/
		const schar * TarPath )				/* [i] 更新ファイルフォルダ		*/
{
	uchar		Ret				= EOTA_FLOW_OK;		  /* 戻り値 			*/
	sint		atoi_ret		= 0;				  /* atoi戻り値			*/
	ulong		md5_num			= 0u;				  /* MD5ファイル数		*/
	size_t		StrSize			= 0;				  /* strlenサイズ		*/
	slong		FileSize		= EOTA_FIlE_READ_ERR; /* ファイルサイズ		*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == UpdateSetup) || (NULL == TarPath))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FLOW_NG;
		return Ret;
	}
	else
	{
		/* 処理無し */
	}

	/* TGTのINV取得 */
	atoi_ret = atoi(UpdateSetup->TgtInv);
	/* TGTのBOARD=0？ */
	if (EOTA_TGT_FLG_OFF != atoi_ret)
	{
		/* F W指示ファイルのWCMD60:INVマイコン_F Wと一致しない) */
		/* TODO. WCMD60:INVマイコンからバージョンを取得 */
		#if 0
		CheckRet = EOTA_CheckUpdateVersion(UpdateSetup->InvFw.Version, "V0001");

		if(EOTA_CHECK_OK == CheckRet)
		{
			Ret = EOTA_FLOW_OK;
		}
		else
		{
			Ret = EOTA_FLOW_NG;
		}
		#endif

		/* BANK1,BANK2のINV_F Wファイルが各MD5ハッシュ値と一致す る */
		/* BANK1 */
		if (EOTA_FLOW_OK == Ret)
		{
			StrSize = strlen(UpdateSetup->InvFw.Bank1Md5File);
			if (0 < StrSize)
			{
				/* MD5チェック */
				Ret = eota_Md5FileCheck(TarPath,
										UpdateSetup->InvFw.Bank1Md5File);
				if (EOTA_CHECK_OK == Ret)
				{
					md5_num += 1u;

					/* 更新ファイルサイズ取得 */
					Ret = eota_GetUpdFileSize(TarPath,
									UpdateSetup->InvFw.Bank1File, &FileSize);
					if (EOTA_CHECK_OK == Ret)
					{
						/* ファイルサイズ設定 */
						EOTA_SetInvBank1FileSize(FileSize);
					}
					else
					{
						Ret = EOTA_FLOW_NG;
					}
				}
				else
				{
					Ret = EOTA_FLOW_NG;
				}
			}
			else
			{
				/* 処理無し */
			}
		}
		else
		{
			/* 処理無し */
		}

		/* BANK2 */
		if (EOTA_FLOW_OK == Ret)
		{
			StrSize = strlen(UpdateSetup->InvFw.Bank2Md5File);
			if (0 < StrSize)
			{
				/* MD5チェック */
				Ret = eota_Md5FileCheck(TarPath,
										UpdateSetup->InvFw.Bank2Md5File);
				if (EOTA_CHECK_OK == Ret)
				{
					md5_num += 1u;

					/* 更新ファイルサイズ取得 */
					Ret = eota_GetUpdFileSize(TarPath,
												UpdateSetup->InvFw.Bank2File,
												&FileSize);
					if (EOTA_CHECK_OK == Ret)
					{
						/* ファイルサイズ設定 */
						EOTA_SetInvBank2FileSize(FileSize);
					}
					else
					{
						Ret = EOTA_FLOW_NG;
					}
				}
				else
				{
					Ret = EOTA_FLOW_NG;
				}
			}
			else
			{
				/* 処理無し */
			}
		}
		else
		{
			/* 処理無し */
		}

		if ((0u == md5_num) && (EOTA_FLOW_OK == Ret))
		{
			Ret = EOTA_FLOW_NG;
		}
		else if (EOTA_FLOW_OK == Ret)
		{
			/* V:BOARD_tgt=1:対象 */
			EOTA_SetTgtInv(EOTA_TGT_FLG_ON);
		}
		else
		{
			/* 処理無し */
		}
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	TGTのPV解析											*/
/*--------------------------------------------------------------------------*/
/*	param			:	FW 更新指示ファイル情報								*/
/* 						更新ファイルフォルダ								*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_FLOW_OK:処理正常								*/
/*						EOTA_FLOW_NG:処理異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	PVのBOARDを解析する									*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
static uchar eota_AnalyzePV(
		EOTA_UPDATE_SETUP_T * UpdateSetup, 	/* [i] FW 更新指示ファイル情報	*/
		const schar * TarPath ) 			/* [i] 更新ファイルフォルダ		*/
{
	uchar		Ret				= EOTA_FLOW_OK;		  /* 戻り値 			*/
	sint		atoi_ret		= 0;				  /* atoi戻り値			*/
	ulong		md5_num			= 0u;				  /* MD5ファイル数		*/
	size_t		StrSize			= 0;				  /* strlenサイズ		*/
	slong		FileSize		= EOTA_FIlE_READ_ERR; /* ファイルサイズ		*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == UpdateSetup) || (NULL == TarPath))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FLOW_NG;
		return Ret;
	}
	else
	{
		/* 処理無し */
	}

	/* TGTのPV取得 */
	atoi_ret = atoi(UpdateSetup->TgtPv);
	/* TGTのBOARD=0？ */
	if (EOTA_TGT_FLG_OFF != atoi_ret)
	{
		/* F W指示ファイルのWCMD60:PVマイコン_F Wと一致しない) */
		/* TODO. WCMD60:PVマイコンからバージョンを取得 */
		#if 0
		CheckRet = EOTA_CheckUpdateVersion(UpdateSetup->PvFw.Version, "V0001");

		if(EOTA_CHECK_OK == CheckRet)
		{
			Ret = EOTA_FLOW_OK;
		}
		else
		{
			Ret = EOTA_FLOW_NG;
		}
		#endif

		/* BANK1,BANK2のPV_F Wファイルが各MD5ハッシュ値と一致す る */
		/* BANK1 */
		if (EOTA_FLOW_OK == Ret)
		{
			StrSize = strlen(UpdateSetup->PvFw.Bank1Md5File);
			if (0 < StrSize)
			{
				/* MD5チェック */
				Ret = eota_Md5FileCheck(TarPath,
										UpdateSetup->PvFw.Bank1Md5File);
				if (EOTA_CHECK_OK == Ret)
				{
					md5_num += 1u;

					/* 更新ファイルサイズ取得 */
					Ret = eota_GetUpdFileSize(TarPath,
											UpdateSetup->PvFw.Bank1File,
											&FileSize);
					if (EOTA_CHECK_OK == Ret)
					{
						/* ファイルサイズ設定 */
						EOTA_SetPVBank1FileSize(FileSize);
					}
					else
					{
						Ret = EOTA_FLOW_NG;
					}
				}
				else
				{
					Ret = EOTA_FLOW_NG;
				}
			}
			else
			{
				/* 処理無し */
			}
		}
		else
		{
			/* 処理無し */
		}

		/* BANK2 */
		if (EOTA_FLOW_OK == Ret)
		{
			StrSize = strlen(UpdateSetup->PvFw.Bank2Md5File);
			if (0 < StrSize)
			{
				/* MD5チェック */
				Ret = eota_Md5FileCheck(TarPath,
										UpdateSetup->PvFw.Bank2Md5File);
				if (EOTA_CHECK_OK == Ret)
				{
					md5_num += 1u;

					/* 更新ファイルサイズ取得 */
					Ret = eota_GetUpdFileSize(TarPath,
												UpdateSetup->PvFw.Bank2File,
												&FileSize);
					if (EOTA_CHECK_OK == Ret)
					{
						/* ファイルサイズ設定 */
						EOTA_SetPVBank2FileSize(FileSize);
					}
					else
					{
						Ret = EOTA_FLOW_NG;
					}
				}
				else
				{
					Ret = EOTA_FLOW_NG;
				}
			}
			else
			{
				/* 処理無し */
			}
		}
		else
		{
			/* 処理無し */
		}

		if ((0u == md5_num) && (EOTA_FLOW_OK == Ret))
		{
			Ret = EOTA_FLOW_NG;
		}
		else if (EOTA_FLOW_OK == Ret)
		{
			/* V:BOARD_tgt=1:対象 */
			EOTA_SetTgtPV(EOTA_TGT_FLG_ON);
		}
		else
		{
			/* 処理無し */
		}
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	TGTのBDC解析										*/
/*--------------------------------------------------------------------------*/
/*	param			:	FW 更新指示ファイル情報								*/
/* 				 		更新ファイルフォルダ								*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_FLOW_OK	処理正常							*/
/*				 		EOTA_FLOW_NG	処理異常							*/
/*--------------------------------------------------------------------------*/
/*	detail			:	BDCのBOARDを解析する								*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
static uchar eota_AnalyzeBDC(
		EOTA_UPDATE_SETUP_T * UpdateSetup, 	/* [i] FW 更新指示ファイル情報	*/
		const schar * TarPath) 				/* [i] 更新ファイルフォルダ		*/
{
	uchar		Ret				= EOTA_FLOW_OK;		  /* 戻り値 			*/
	sint		atoi_ret		= 0;				  /* atoi戻り値			*/
	ulong		md5_num			= 0u;				  /* MD5ファイル数		*/
	size_t		StrSize			= 0u;				  /* strlenサイズ		*/
	slong		FileSize		= EOTA_FIlE_READ_ERR; /* ファイルサイズ		*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == UpdateSetup) || (NULL == TarPath))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FLOW_NG;
		return Ret;
	}
	else
	{
		/* 処理無し */
	}

	/* TGTのBDC取得 */
	atoi_ret = atoi(UpdateSetup->TgtBdc);
	/* TGTのBOARD=0？ */
	if (EOTA_TGT_FLG_OFF != atoi_ret)
	{
		/* (システム構成=(0,1,3,4))  */

		/* F W指示ファイルのWCMD60:BDCマイコン_F Wと一致しない) */
		/* TODO. CMD60:BDCマイコンからバージョンを取得 */
		#if 0
		CheckRet = EOTA_CheckUpdateVersion(UpdateSetup->BdcFw.Version, "V0001");

		if(EOTA_CHECK_OK == CheckRet)
		{
			Ret = EOTA_FLOW_OK;
		}
		else
		{
			Ret = EOTA_FLOW_NG;
		}
		#endif

		/* BANK1,BANK2のBDC_F Wファイルが各MD5ハッシュ値と一致す る */
		/* BANK1 */
		if (EOTA_FLOW_OK == Ret)
		{
			StrSize = strlen(UpdateSetup->BdcFw.Bank1Md5File);
			if (0 < StrSize)
			{
				/* MD5チェック */
				Ret = eota_Md5FileCheck(TarPath,
										UpdateSetup->BdcFw.Bank1Md5File);
				if (EOTA_CHECK_OK == Ret)
				{
					md5_num += 1u;

					/* 更新ファイルサイズ取得 */
					Ret = eota_GetUpdFileSize(TarPath,
												UpdateSetup->BdcFw.Bank1File,
												&FileSize);
					if (EOTA_CHECK_OK == Ret)
					{
						/* ファイルサイズ設定 */
						EOTA_SetBdcBank1FileSize(FileSize);
					}
					else
					{
						Ret = EOTA_FLOW_NG;
					}
				}
				else
				{
					Ret = EOTA_FLOW_NG;
				}
			}
			else
			{
				/* 処理無し */
			}
		}
		else
		{
			/* 処理無し */
		}

		/* BANK2 */
		if (EOTA_FLOW_OK == Ret)
		{
			StrSize = strlen(UpdateSetup->BdcFw.Bank2Md5File);
			if (0 < StrSize)
			{
				/* MD5チェック */
				Ret = eota_Md5FileCheck(TarPath,
										UpdateSetup->BdcFw.Bank2Md5File);
				if (EOTA_CHECK_OK == Ret)
				{
					md5_num += 1u;

					/* 更新ファイルサイズ取得 */
					Ret = eota_GetUpdFileSize(TarPath,
												UpdateSetup->BdcFw.Bank2File,
												&FileSize);
					if (EOTA_CHECK_OK == Ret)
					{
						/* ファイルサイズ設定 */
						EOTA_SetBdcBank2FileSize(FileSize);
					}
					else
					{
						Ret = EOTA_FLOW_NG;
					}
				}
				else
				{
					Ret = EOTA_FLOW_NG;
				}
			}
			else
			{
				/* 処理無し */
			}
		}
		else
		{
			/* 処理無し */
		}

		if ((0u == md5_num) && (EOTA_FLOW_OK == Ret))
		{
			Ret = EOTA_FLOW_NG;
		}
		else if (EOTA_FLOW_OK == Ret)
		{
			/* V:BOARD_tgt=1:対象 */
			EOTA_SetTgtBDC(EOTA_TGT_FLG_ON);
		}
		else
		{
			/* 処理無し */
		}
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	TGTのJ.B.解析										*/
/*--------------------------------------------------------------------------*/
/*	param			:	FW 更新指示ファイル情報								*/
/* 						更新ファイルフォルダ								*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_FLOW_OK:処理正常								*/
/*						EOTA_FLOW_NG:処理異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	TGTのJ.B.を解析する									*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
static uchar eota_AnalyzeJB(
		EOTA_UPDATE_SETUP_T * UpdateSetup, 	/* [i] FW 更新指示ファイル情報	*/
		const schar * TarPath)				/* [i] 更新ファイルフォルダ		*/
{
	uchar		Ret				= EOTA_FLOW_OK;		  /* 戻り値 			*/
	sint		atoi_ret		= 0;				  /* atoi戻り値			*/
	size_t		StrSize			= 0u;				  /* strlenサイズ		*/
	slong		FileSize		= EOTA_FIlE_READ_ERR; /* ファイルサイズ		*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == UpdateSetup) || (NULL == TarPath))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FLOW_NG;

		/* 関数を戻る */
		return Ret;
	}
	else
	{
		/* 処理無し */
	}

	atoi_ret = atoi(UpdateSetup->TgtJb);
	/* TGTのJ.B.=0？ */
	if (EOTA_TGT_FLG_OFF != atoi_ret)
	{
		/* (システム構成=(0,1,3,4)) TODO.  */


		/* F W指示ファイルのJ.B_F WバージョンがCMD60:J.B._F Wと一致しない */
		/* TODO. CMD60:J.B.マイコンからバージョンを取得 */
		#if 0
		CheckRet = EOTA_CheckUpdateVersion(UpdateSetup->JbFw.Version, "V0001");

		if(EOTA_CHECK_OK == CheckRet)
		{
			Ret = EOTA_FLOW_OK;
		}
		else
		{
			Ret = EOTA_FLOW_NG;
		}
		#endif

		/* 通信ボードFWファイルがMD5ハッシュ値と一致する */
		if (EOTA_FLOW_OK == Ret)
		{
			/* MD5ハッシュ値と一致する */
			StrSize = strlen(UpdateSetup->JbFw.Md5File);
			if (0 < StrSize)
			{
				/* MD5チェック */
				Ret = eota_Md5FileCheck(TarPath, UpdateSetup->JbFw.Md5File);
				if (EOTA_CHECK_OK == Ret)
				{
					/* 更新ファイルサイズ取得 */
					Ret = eota_GetUpdFileSize(TarPath,
												UpdateSetup->JbFw.File,
												&FileSize);
					if (EOTA_CHECK_OK == Ret)
					{
						/* ファイルサイズ設定 */
						EOTA_SetJbFileSize(FileSize);
					}
					else
					{
						Ret = EOTA_FLOW_NG;
					}
				}
				else
				{
					Ret = EOTA_FLOW_NG;
				}
			}
			else
			{
				/* 処理無し */
			}

			if(EOTA_FLOW_OK == Ret)
			{
				/* V:BOARD_tgt=1:対象 */
				EOTA_SetTgtJB(EOTA_TGT_FLG_ON);
			}
			else
			{
				Ret = EOTA_FLOW_NG;
			}
		}
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	ファイル解析						 				*/
/*--------------------------------------------------------------------------*/
/*	param			:	圧縮ファイル名										*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ファイル解析を実施する								*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
static void eota_Analyze(const schar * FileName) 	  /* [i] 圧縮ファイル名  */
{
	uchar		Ret				= EOTA_FLOW_OK;		/* 戻り値 				*/
	uchar		CheckRet		= EOTA_CHECK_OK;	/* チェックの戻り値		*/
	uchar		FileRet			= EOTA_FILE_OK;		/* ファイルの戻り値		*/
	ushort		Number  		= 0u;				/* 半角英数字 			*/
	schar		Path[EOTA_TAR_FILE_DIR_LEN]
								= {0};				/* 更新指示ファイルパス	*/
	schar *		PathPoint 		= Path;				/* パスポイント			*/
	schar		TarPath[EOTA_TAR_FILE_DIR_LEN + 1]
								= {0};				/* 更新ファイルフォルダ	*/
	size_t		CpySize			= 0u;				/* コピーサイズ			*/
	uchar		UpdateMode		= EOTA_UPD_MODE_U;	/* 更新モード			*/
	schar 		TgtList[EOTA_UPD_TGT_LEN]
								= {0};				/* TGTリスト			*/

	EOTA_UPDATE_SETUP_T update_setup;
	memset(&update_setup, 0, sizeof(EOTA_UPDATE_SETUP_T));

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == FileName)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FLOW_NG;
	}
	else
	{
		/* FLASHの中に、for FW versionupのパスを設定 */
		CpySize = strlen(EOTA_FW_VER_NUP_FILE_PATH);
		strncpy(PathPoint, EOTA_FW_VER_NUP_FILE_PATH, CpySize);
		PathPoint += CpySize;
	}

	/* LAN:FW ダウンロード処理状態で FW 更新全体の進捗を応答 */
	/* 進捗率(進捗率(解析中)) */
	EOTA_SetProgressRate(EOTA_PRO_RATE_ANA_ING);

	if(EOTA_FLOW_OK == Ret)
	{
		/* 圧縮ファイル名=E1F W_PKG_vxxx(xxx半角英数字3文字)? */
		CheckRet = EOTA_CheckTarFile(FileName, &Number);

		/* 圧縮ファイル名不正の場合、エラーを戻る */
		if (EOTA_CHECK_OK != CheckRet)
		{
			Ret = EOTA_FLOW_NG;
		}
		else
		{
			Ret = EOTA_FLOW_OK;
		}
	}
	else
	{
		/* 処理無し */
	}

	/* 更新指示ファイルを読み込む */
	if(EOTA_FLOW_OK == Ret)
	{
		/* パスにて解凍のパスを設定 */
		CpySize = strlen(FileName) - EOTA_PKG_EXT_SIZE;
		strncpy(PathPoint, FileName, CpySize);
		PathPoint += CpySize;

		/* 更新ファイルフォルダ設定 */
		strncpy(TarPath, Path, strlen(Path));
		EOTA_SetUpdFileDri(TarPath);

		/* パスにてFW 更新指示ファイルを設定 */
		CpySize = strlen(EOTA_UPD_CMD_IND_NAME);
		strncpy(PathPoint, EOTA_UPD_CMD_IND_NAME, CpySize);

		/* 読み込む */
		FileRet = EOTA_GetUpdateSetupFile(Path, &update_setup);
		if (EOTA_FILE_OK == FileRet)
		{
			Ret = EOTA_FLOW_OK;
		}
		else
		{
			Ret = EOTA_FLOW_NG;
		}
	}
	else
	{
		/* 処理無し */
	}

	/* NOチェック */
	if(EOTA_FLOW_OK == Ret)
	{
		/* NOチェック */
		CheckRet = EOTA_CheckSetupNO(update_setup.No);
		if(EOTA_CHECK_OK == CheckRet)
		{
			Ret = EOTA_FLOW_OK;
		}
		else
		{
			Ret = EOTA_FLOW_NG;
		}
	}
	else
	{
		/* 処理無し */
	}

	/* TYPEチェック */
	if(EOTA_FLOW_OK == Ret)
	{
		/* NOチェック */
		CheckRet = EOTA_CheckSetupType(update_setup.Type);
		if(EOTA_CHECK_OK == CheckRet)
		{
			Ret = EOTA_FLOW_OK;
		}
		else
		{
			Ret = EOTA_FLOW_NG;
		}
	}
	else
	{
		/* 処理無し */
	}

	/* MODEチェック */
	if(EOTA_FLOW_OK == Ret)
	{
		/* MODEチェック */
		CheckRet = EOTA_CheckSetupMode(update_setup.Mode, &UpdateMode);

		if(EOTA_CHECK_OK == CheckRet)
		{
			if (EOTA_UPD_MODE_F == UpdateMode)
			{
				/* TIMEチェック */
				CheckRet = EOTA_CheckSetupTime(update_setup.Time);

				if(EOTA_CHECK_OK == CheckRet)
				{
					Ret = EOTA_FLOW_OK;
				}
				else
				{
					Ret = EOTA_FLOW_NG;
				}
			}
			else if (EOTA_UPD_MODE_C == UpdateMode)
			{
				/* V:PKG削除フラグ=1 */
				EOTA_SetDownDelFlg(EOTA_DOWN_DEL_FLG_ON);
			}
			else
			{
				Ret = EOTA_FLOW_OK;
			}
		}
		else
		{
			Ret = EOTA_FLOW_NG;
		}
	}
	else
	{
		/* 処理無し */
	}

	/* TGTチェック */
	if ((EOTA_FLOW_OK == Ret)
		&& (EOTA_DOWN_DEL_FLG_OFF == EOTA_GetDownDelFlg()))
	{
		/* BOARDチェック */
		/* TGTの値に重複が無い	 */
		CheckRet = EOTA_CheckSetupTGT(update_setup.TgtBoard,
										EOTA_UPD_TGT_BOARD, TgtList);
		if(EOTA_CHECK_OK == CheckRet)
		{
			Ret = EOTA_FLOW_OK;
		}
		else
		{
			Ret = EOTA_FLOW_NG;
		}

		/* INVチェック */
		if (EOTA_FLOW_OK == Ret)
		{
			/* TGTの値に重複が無い	 */
			CheckRet = EOTA_CheckSetupTGT(update_setup.TgtInv,
											EOTA_UPD_TGT_INV, TgtList);
			if(EOTA_CHECK_OK == CheckRet)
			{
				Ret = EOTA_FLOW_OK;
			}
			else
			{
				Ret = EOTA_FLOW_NG;
			}
		}

		/* PVチェック */
		if (EOTA_FLOW_OK == Ret)
		{
			/* TGTの値に重複が無い	 */
			CheckRet = EOTA_CheckSetupTGT(update_setup.TgtPv,
											EOTA_UPD_TGT_PV, TgtList);
			if(EOTA_CHECK_OK == CheckRet)
			{
				Ret = EOTA_FLOW_OK;
			}
			else
			{
				Ret = EOTA_FLOW_NG;
			}
		}

		/* BDCチェック */
		if (EOTA_FLOW_OK == Ret)
		{
			/* TGTの値に重複が無い	 */
			CheckRet = EOTA_CheckSetupTGT(update_setup.TgtBdc,
											EOTA_UPD_TGT_BDC, TgtList);
			if(EOTA_CHECK_OK == CheckRet)
			{
				Ret = EOTA_FLOW_OK;
			}
			else
			{
				Ret = EOTA_FLOW_NG;
			}
		}

		/* J.B.チェック */
		if (EOTA_FLOW_OK == Ret)
		{
			/* TGTの値に重複が無い	 */
			CheckRet = EOTA_CheckSetupTGT(update_setup.TgtJb,
										EOTA_UPD_TGT_J_B, TgtList);
			if(EOTA_CHECK_OK == CheckRet)
			{
				Ret = EOTA_FLOW_OK;
			}
			else
			{
				Ret = EOTA_FLOW_NG;
			}
		}

		/* TGTの値が全て0でない	 */
		if (EOTA_FLOW_OK == Ret)
		{
			CheckRet = EOTA_CheckSetupTGTAllZero(TgtList);
			if(EOTA_CHECK_OK == CheckRet)
			{
				Ret = EOTA_FLOW_OK;
			}
			else
			{
				Ret = EOTA_FLOW_NG;
			}
		}

		/* TGTの値が連続である	 */
		if (EOTA_FLOW_OK == Ret)
		{
			CheckRet = EOTA_CheckSetupTGTOrder(TgtList);
			if(EOTA_CHECK_OK == CheckRet)
			{
				Ret = EOTA_FLOW_OK;
			}
			else
			{
				Ret = EOTA_FLOW_NG;
			}
		}
	}
	else
	{
		/* 処理無し */
	}

	/* TGTのBOARDチェック */
	if ((EOTA_FLOW_OK == Ret)
		&& (EOTA_DOWN_DEL_FLG_OFF == EOTA_GetDownDelFlg()))
	{
		Ret = eota_AnalyzeBoard(&update_setup, TarPath);
	}
	else
	{
		/* 処理無し */
	}

	/* TGTのINVチェック */
	if ((EOTA_FLOW_OK == Ret)
		&& (EOTA_DOWN_DEL_FLG_OFF == EOTA_GetDownDelFlg()))
	{
		Ret = eota_AnalyzeInv(&update_setup, TarPath);
	}
	else
	{
		/* 処理無し */
	}

	/* TGTのPVチェック */
	if ((EOTA_FLOW_OK == Ret)
		&& (EOTA_DOWN_DEL_FLG_OFF == EOTA_GetDownDelFlg()))
	{
		Ret = eota_AnalyzePV(&update_setup, TarPath);
	}
	else
	{
		/* 処理無し */
	}

	/* TGTのBDCチェック */
	if ((EOTA_FLOW_OK == Ret)
		&& (EOTA_DOWN_DEL_FLG_OFF == EOTA_GetDownDelFlg()))
	{
		Ret = eota_AnalyzeBDC(&update_setup, TarPath);
	}
	else
	{
		/* 処理無し */
	}

	/* TGTのJ.B.チェック */
	if ((EOTA_FLOW_OK == Ret)
		&& (EOTA_DOWN_DEL_FLG_OFF == EOTA_GetDownDelFlg()))
	{
		Ret = eota_AnalyzeJB(&update_setup, TarPath);
	}
	else
	{
		/* 処理無し */
	}

	/* PKG削除フラグの場合 */
	if ((EOTA_FLOW_OK == Ret)
		&& (EOTA_DOWN_DEL_FLG_OFF == EOTA_GetDownDelFlg()))
	{
		/* V:解析結果=1:OK */
		EOTA_SetAnalyRes(EOTA_UPD_RES_OK);
	}
	/* 解析結果OK */
	else if(EOTA_FLOW_OK == Ret)
	{
		/* V:解析結果=1:OK */
		EOTA_SetAnalyRes(EOTA_UPD_RES_OK);

		/* V:更新 No=FW 更新指示ファイルの No */
		EOTA_SetUpdNO((ushort)atoi(update_setup.No));

		/* 送信情報保存 */
		EOTA_SetSendData(&update_setup);

		/* ・MODE=U の場合 */
		if ((EOTA_UPD_MODE_U == UpdateMode) && (EOTA_FLOW_OK == Ret))
		{
			/* メモ:実施時刻はスマホアプリから通知される */
			/* TODO. */
			/* 外部通信機器から FW 更新情報を要求された際は、表 2-2 の更新情報ファイル(*_info.txt)のう */
			/* 			ち*_tgt=1:対象のものを送信すること。 */


		}
		else if ((EOTA_UPD_MODE_F == UpdateMode) && (EOTA_FLOW_OK == Ret))
		{
			/*・V:FW 更新実施時刻=FW 更新指示ファイルの TIME */
			EOTA_SetUpdTime(update_setup.Time);
		}
		else
		{
			/* 処理無し */
		}
	}
	/* 解析結果NG */
	else
	{
		/* V:解析結果=1:NG */
		EOTA_SetAnalyRes(EOTA_UPD_RES_NG);
	}

	/* 待機に遷移 */
	EOTA_SetUpdateState(EOTA_UPD_WAIT);
	EOTA_Flow();

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	FW 更新開始											*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW 更新開始処理を実施する							*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
static void eota_UpdateStart(void)
{
	DEBUG_TRCIN();

	/* CMD65:運転/停止=停止を送信 */
	EOTA_SendCMD65Stop();

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	FW 更新中											*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW 更新中処理を実施する								*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
static uchar eota_Update(void)
{
	uchar	Ret			= EOTA_FLOW_OK;				/* 戻り値 				*/
	uchar	InvRet 		= EOTA_INV_OK;				/* INV送信・受信戻り値	*/
	uchar	UpdIngState = EOTA_UPD_ING_START;		/* 更新中状態			*/

	DEBUG_TRCIN();

	/* 更新中状態取得 */
	UpdIngState = EOTA_GetUpdIngState();

	/* FW 更新中状態:開始の場合 */
	if( EOTA_UPD_ING_START == UpdIngState)
	{
		/* CMD81 の定期送信を停止 */
		EOTA_SendCMD81Stop();

		/* ENL には不可応答を返す TODO. */


		/* LAN:FW 更新状況で FW 更新全体の進捗を応答 */
		/* 進捗率(進捗率(解析完了)) */
		EOTA_SetProgressRate(EOTA_PRO_RATE_ANA_END);

		/* 更新部品初期化(00000000) */
		EOTA_SetUpdProjectState(0);

		/* INV、PV、BDC、J.B.更新  */
		InvRet = EOTA_SendInvMessage();

		if (EOTA_INV_OK == InvRet)
		{
			/* 更新中状態設定 */
			EOTA_SetUpdIngState(EOTA_UPD_ING_CON);
		}
		else
		{
			Ret = EOTA_FLOW_NG;
		}
	}
	else if (EOTA_UPD_ING_INV_END == UpdIngState)
	{
		/* 通信ボード更新 */
		if (EOTA_TGT_FLG_ON == EOTA_GetTgtBoard())
		{
			/* 通信ボード更新 TODO. */
			

			/* FW 更新指示ファイルで指定された更新が終了 */
			/* LED設定 */
			EOTA_Set_Led(EOTA_FW_UPD_STATE);

			/* V:FW 更新状態=4:FW 更新終了 */
			EOTA_SetUpdateState(EOTA_UPD_END);
			/* 状態に遷移 */
			EOTA_Flow();
		}
	}
	else if (EOTA_UPD_ING_CON == UpdIngState)
	{
		/* INV、PV、BDC、J.B.更新  */
		InvRet = EOTA_SendInvMessage();
	}
	else
	{
		/* 処理無し */
	}

	/* TODO. 以下の機能を実装しない */
	/* OR 通信ボード更新失敗時 */
	/* OR ((FW 転送シーケンスで応答値 Ack/Nak=0:Nak */
	/* 			OR FW 転送シーケンスで応答値 FW 転送状態=1:終了 */
	/* 			OR LAN:FW 更新中止指令を受信 */
	/* 			OR  CMD11 送信に応答がない) */
	/* 		AND FW 転送シーケンスが終了)) */
	/* OR CMD60 応答が更新した FW バージョンと一致しない */

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	FW 更新終了											*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW 更新終了処理を実施する							*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
static void eota_UpdateEnd(void)
{
	DEBUG_TRCIN();

	/* V:更新 No=0 */
	EOTA_SetUpdNO(0);

	/* V:FW 更新 LED フラグ 0:なし */
	EOTA_SetUpdateLed(EOTA_UPD_LED_ON);

	/* 全ての FW 更新成功 */
	EOTA_Set_Led(EOTA_ALL_FW_SUCESS);

	/* FW 更新実施状態:End */
	EOTA_SetRunState(EOTA_RUN_STATE_E);

	/* 通信ボード再起動 TODO. */
	

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	CMD64 送信											*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	CMD64を送信する										*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/05/05 : DHC.lib : 新規作成						*/
/*==========================================================================*/
void eota_SendCmd64(void)
{
	uchar	updateState = 0;
	DEBUG_TRCIN();

	updateState = EOTA_GetUpdateState();
	if(EOTA_UPD_START == updateState)
	{
		/*	CMD64を送信する	TODO.	*/
		
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	停止コマンド送信から60sまでシステムが停止しません	*/
/*--------------------------------------------------------------------------*/
/*	param			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	停止コマンド送信から60sまでシステムが停止しません	*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/05/05 : DHC.lib : 新規作成						*/
/*==========================================================================*/
void eota_Cmd64TimeOut(void)
{
	uchar	updateState = 0;
	DEBUG_TRCIN();

	/* 取消Timer */
	FRM_TimerCancelOneShot(EOTA_MSGID_SEND_CMD64);

	/* CMD64:運転/停止=1:運転 TODO. */
	updateState = EOTA_GetUpdateState();
	if(EOTA_UPD_START == updateState)
	{
		/* ??? TODO  Error info save / error led / */

		/* LED設定 */
		EOTA_Set_Led(EOTA_FW_UPD_STATE);

		/* V:FW 更新状態=0:待機 */
		EOTA_SetUpdateState(EOTA_UPD_WAIT);

		/* 状態に遷移 */
		EOTA_Flow();
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	CMD64結果処理										*/
/*--------------------------------------------------------------------------*/
/*	param			:	Msg	メッセージ										*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	CMD64結果処理										*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/05/06 : DHC.lib : 新規作成						*/
/*==========================================================================*/
void eota_GetCmd64Result(FrmMsgHead_t const * Msg)
{
	uchar	updateState = 0;
	EotaMsgPar_t* p_msg = (EotaMsgPar_t*)Msg;
	FrmPthread_t Thread = 0UL;	/* スレッド */
	slong threadRet = 0L;
	DEBUG_TRCIN();

	updateState = EOTA_GetUpdateState();
	if(EOTA_UPD_START == updateState)
	{
		/* CMD64 REQ:運転/停止=0:停止 TODO. */
		if (0 == p_msg->cmd64_data.startOrStop)
		{
			/* 取消Timer */
			FRM_TimerCancelOneShot(EOTA_MSGID_CMD64_TIMEOUT);

			/* LED設定 */
			EOTA_Set_Led(EOTA_FW_UPD_STATE);

			/* V:FW 更新状態=3:FW 更新中 */
			EOTA_SetUpdateState(EOTA_UPD_ING);

			/* FW 更新中状態:開始 */
			EOTA_SetUpdIngState(EOTA_UPD_ING_START);

			threadRet = FRM_PthreadCreate(&Thread, NULL, EOTA_CommThreadStartUpdate, NULL);
			if (threadRet != FRM_SUCCESS)
			{
				DLOG_Error("eota_GetCmd64Result FRM_PthreadCreate fail\n");
			} 
			else
			{
				/* 処理無し */
			}
		}
		else
		{
			/* CMD64 を 1s 毎に送信  */
			FRM_TimerSetOneShot(EOTA_TIM_ID_CMD64, EOTA_MSGID_SEND_CMD64, NULL);
		}
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	CMD65結果処理										*/
/*--------------------------------------------------------------------------*/
/*	param			:	Msg	メッセージ										*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	CMD65結果処理										*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/05/06 : DHC.lib : 新規作成						*/
/*==========================================================================*/
void eota_GetCmd65Result(FrmMsgHead_t const * Msg)
{
	EotaMsgPar_t* p_msg = (EotaMsgPar_t*)Msg;
	DEBUG_TRCIN();

	if (0 == p_msg->cmd65_data.ret)
	{
		/* CMD65送信成功 */
		/* CMD64 を 1s 毎に送信  */
		FRM_TimerSetOneShot(EOTA_TIM_ID_CMD64, EOTA_MSGID_SEND_CMD64, NULL);

		/* CMD64 を 60s timeout 送信  */
		FRM_TimerSetOneShot(EOTA_TIM_ID_CMD64_TIMEOUT, EOTA_MSGID_CMD64_TIMEOUT, NULL);
	}
	else
	{
		/* CMD65送信失敗 */
		/* ??? TODO  Error info save / error led / */

		/* LED設定 */
		EOTA_Set_Led(EOTA_FW_UPD_STATE);

		/* V:FW 更新状態=0:待機 */
		EOTA_SetUpdateState(EOTA_UPD_WAIT);

		/* 状態に遷移 */
		EOTA_Flow();
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description	:FW更新スレッド												*/
/*--------------------------------------------------------------------------*/
/*	param		:arg		:NULL指定										*/
/*--------------------------------------------------------------------------*/
/*	return		:NULL														*/
/*--------------------------------------------------------------------------*/
/*	detail		:FW更新開始処理を行う。										*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/05/05 : DHC lib : 新規作成							*/
/*==========================================================================*/
void * EOTA_CommThreadStartUpdate(void *arg)
{
	DEBUG_TRCIN();

	/* 状態遷移 */
	EOTA_Flow();

	DEBUG_TRCOUT();

    pthread_exit(NULL);
}
