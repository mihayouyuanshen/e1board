/****************************************************************************/
/*	Description		:FW更新機能 各設定値チェック							*/
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
#include "eota_check.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:	10進チェック										*/
/*--------------------------------------------------------------------------*/
/*	param			:	チェック文字列										*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_CHECK_OK          チェック正常					*/
/*						EOTA_CHECK_NG          チェック異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	10進をチェックする									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_is_digit(const schar * pData)	/* [i] チェック文字列 */
{
	uchar	Ret	= EOTA_CHECK_OK;	/* 戻り値 */

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == pData)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_CHECK_NG;
	}
	else
	{
		/* 処理無し */
	}

	/* 圧縮ファイル名サイズチェック */
	if (EOTA_CHECK_OK == Ret)
	{
		size_t len = strlen(pData);

		for (size_t loop = 0; loop < len; loop++)
		{
			if ((EOTA_CHECK_OK == Ret) && (!isdigit(pData[loop])))
			{
				Ret = EOTA_CHECK_NG;
			}
			else
			{
				/* 処理無し */
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
/*	Description		:	圧縮ファイル名チェック								*/
/*--------------------------------------------------------------------------*/
/*	param			:	圧縮ファイル名										*/
/*						半角英数字											*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_CHECK_OK          チェック正常					*/
/*						EOTA_CHECK_NG          チェック異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	圧縮ファイル名をチェックする						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_CheckTarFile(const schar * TarName,	/* [i] 圧縮ファイル名  */
						ushort * 	  Number)	/* [o] 半角英数字      */
{
	/* 変数初期化 */
	uchar	Ret				= EOTA_CHECK_OK;	/* 戻り値				*/
	size_t 	TarNamelen		= 0u;				/* 圧縮ファイル名サイズ	*/
	schar *	StrStrRet		= NULL;				/* 文字列検索戻り値		*/
	uchar	IsDigitRet		= EOTA_CHECK_OK;	/* 数字チェック戻り値	*/
	sint	StrcmpRet		= 0;				/* 文字列比較戻り値		*/
	uchar	StrNumSize		= EOTA_PKG_NUM_SIZE + 1;
												/* 半角英数字サイズ		*/
	schar	StrNum[StrNumSize];					/* 半角英数字文字列		*/
	memset(StrNum,0,sizeof(StrNum));

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == TarName) || (NULL == Number))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_CHECK_NG;
	}
	else
	{
		*Number = 0u;
	}

	/* 圧縮ファイル名サイズチェック */
	if (EOTA_CHECK_OK == Ret)
	{
		TarNamelen = strlen(TarName);
		if(EOTA_PKG_TGZ_NAME_LEN != TarNamelen)
		{
			/* ファイル名サイズエラー */
			DLOG_Error("%s",EOTA_FILE_NAME_SIZE_ERR);
			Ret = EOTA_CHECK_NG;
		}
	}
	else
	{
		/* 処理無し */
	}

	/* 圧縮ファイル名チェック */
	if (EOTA_CHECK_OK == Ret)
	{
		StrStrRet = strstr(TarName, EOTA_PKG_TGZ_NAME);

		/* 文字列E1FW_PKG_vチェック */
		if ( (NULL != StrStrRet) && (TarName == StrStrRet))
		{
			/* 半角英数字チェック */
			StrStrRet += EOTA_E1FW_PKG_V_SIZE;
			/* 半角英数字コピー */
			strncpy(StrNum, StrStrRet, EOTA_PKG_NUM_SIZE);

			/* xxx半角英数字チェック */
			IsDigitRet = EOTA_is_digit(StrNum);
			if (EOTA_CHECK_NG == IsDigitRet)
			{
				Ret = EOTA_CHECK_NG;
				/* ファイル名半角英数字以外 */
				DLOG_Error("%s",EOTA_FILE_NAME_NUM_ERR);
			}
			else
			{
				/* 処理無し */
			}

			/* .tgz拡張子チェック */
			StrStrRet += EOTA_PKG_NUM_SIZE;
			StrcmpRet = strncmp(StrStrRet, EOTA_PKG_TGZ_EXT, EOTA_PKG_EXT_SIZE);
			if (EOTA_STRCMP_INDENTICAL != StrcmpRet)
			{
				Ret = EOTA_CHECK_NG;
				/* ファイル名拡張子エラー */
				DLOG_Error("%s",EOTA_FILE_NAME_EXT_ERR);
			}
			else
			{
				*Number = (ushort)atoi(StrNum);
			}
		}
		else
		{
			/* ファイル名エラー */
			DLOG_Error("%s",EOTA_FILE_NAME_ERR);
			Ret = EOTA_CHECK_NG;
		}
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return (uchar)(Ret);
}

/*==========================================================================*/
/*	Description		:	FW 更新指示ファイルのNO項目チェック					*/
/*--------------------------------------------------------------------------*/
/*	param			:	No項目												*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_CHECK_OK          チェック正常					*/
/*						EOTA_CHECK_NG          チェック異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW 更新指示ファイルのNO項目をチェックする			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_CheckSetupNO(const schar * No)	/* [i] No項目		   */
{
	/* 変数初期化 */
	uchar	Ret			= EOTA_CHECK_OK;	/* 戻り値				*/
	sint	NoValue		= 0;				/* NO項目値				*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == No)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_CHECK_NG;
	}
	else
	{
		/* 処理無し */
	}

	/* NO数字チェック */
	if (EOTA_CHECK_OK == Ret)
	{
		/* チェック処理実施 */
		Ret = EOTA_is_digit(No);
		if (EOTA_CHECK_OK != Ret)
		{
			DLOG_Error("%s",EOTA_SETUP_NO_NOT_NUM_ERR);
			Ret = EOTA_CHECK_NG;
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

	/* Rangeチェック */
	if (EOTA_CHECK_OK == Ret)
	{
		/* チェック処理実施 */
		NoValue = atoi(No);
		if ((EOTA_SETUP_DOWN_NO_MIN > NoValue) || (EOTA_SETUP_DOWN_NO_MAX < NoValue))
		{
			DLOG_Error("%s",EOTA_SETUP_NO_RANGE_ERR);
			Ret = EOTA_CHECK_NG;
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

	return (uchar)(Ret);
}

/*==========================================================================*/
/*	Description		:	FW 更新指示ファイルのTYPE項目チェック				*/
/*--------------------------------------------------------------------------*/
/*	param			:	TYPE項目											*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_CHECK_OK          チェック正常					*/
/*						EOTA_CHECK_NG          チェック異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW 更新指示ファイルのTYPE項目をチェックする			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_CheckSetupType(const schar * Type)/* [i] TYPE項目		   */
{
	/* 変数初期化 */
	uchar	Ret				= EOTA_CHECK_OK;	/* 戻り値				*/
	sint	StrcmpRet		= 0;				/* 文字列比較戻り値		*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == Type))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_CHECK_NG;
	}
	else
	{
		/* 処理無し */
	}

	/* Type設定値チェック */
	if (EOTA_CHECK_OK == Ret)
	{
		/* fw_model_e1？ */
		StrcmpRet = strncmp(Type, EOTA_SETUP_FW_MODEL_E1, strlen(Type));
		if (EOTA_STRCMP_INDENTICAL != StrcmpRet)
		{
			Ret = EOTA_CHECK_NG;

			/* TYPE項目値不正 */
			DLOG_Error("%s",EOTA_SETUP_TYPE_ERR);
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

	return (uchar)(Ret);
}

/*==========================================================================*/
/*	Description		:	FW 更新指示ファイルのMODE項目チェック				*/
/*--------------------------------------------------------------------------*/
/*	param			:	MODE項目											*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_CHECK_OK          チェック正常					*/
/*						EOTA_CHECK_NG          チェック異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW 更新指示ファイルのMODE項目をチェックする			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_CheckSetupMode(const schar * Mode,	/* [i] MODE 項目	   */
						  uchar * UpdateMode)	/* [o] 更新モード	   */
{
	/* 変数初期化 */
	uchar	Ret				= EOTA_CHECK_OK;	/* 戻り値				*/
	sint	StrcmpRet		= 0;				/* 文字列比較戻り値		*/

	DEBUG_TRCIN();

	/* 更新モード:U */
	*UpdateMode = EOTA_UPD_MODE_U;

	/* パラメータチェック */
	if ((NULL == Mode))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_CHECK_NG;
	}
	else
	{
		/* 処理無し */
	}

	/* 内容チェック */
	if (EOTA_CHECK_OK == Ret)
	{
		/* 強制的に FW 更新？ */
		StrcmpRet = strncmp(Mode, EOTA_SETUP_MODE_F, strlen(Mode));
		if (EOTA_STRCMP_INDENTICAL != StrcmpRet)
		{
			/* スマホアプリ UI から操作して FW更新？ */
			StrcmpRet = strncmp(Mode, EOTA_SETUP_MODE_U, strlen(Mode));
			if (EOTA_STRCMP_INDENTICAL != StrcmpRet)
			{
				/* 更新待ち FW を削除？ */
				StrcmpRet = strncmp(Mode, EOTA_SETUP_MODE_C, strlen(Mode));
				if (EOTA_STRCMP_INDENTICAL != StrcmpRet)
				{
					Ret = EOTA_CHECK_NG;

					/* MODE項目値不正 */
					DLOG_Error("%s",EOTA_SETUP_MODE_ERR);
				}
				else
				{
					/* 更新モード:C */
					*UpdateMode = EOTA_UPD_MODE_C;
				}
			}
			else
			{
				/* 更新モード:F */
				*UpdateMode = EOTA_UPD_MODE_F;
			}
		}
		else
		{
			/* 更新モード:U */
			*UpdateMode = EOTA_UPD_MODE_U;
		}
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return (uchar)(Ret);
}

/*==========================================================================*/
/*	Description		:	FW 更新指示ファイルのTGT設定値チェック				*/
/*--------------------------------------------------------------------------*/
/*	param			:	TGT設定値											*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_CHECK_OK          チェック正常					*/
/*						EOTA_CHECK_NG          チェック異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW 更新指示ファイルのTGT設定値をチェックする		*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_CheckSetupTGTValue(const schar * Tgt)	/* [o] TGT項目値   */
{
	/* 変数初期化 */
	uchar	Ret				= EOTA_CHECK_OK;	/* 戻り値				*/
	sint	TgtValue		= 0;				/* TGT項目値			*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == Tgt)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_CHECK_NG;
	}
	else
	{
		/* 処理無し */
	}

	/* TGT数字チェック */
	if (EOTA_CHECK_OK == Ret)
	{
		/* チェック処理実施 */
		Ret = EOTA_is_digit(Tgt);
		if (EOTA_CHECK_OK != Ret)
		{
			DLOG_Error("%s",EOTA_SETUP_TGT_NOT_NUM_ERR);
			Ret = EOTA_CHECK_NG;
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

	/* Rangeチェック */
	if (EOTA_CHECK_OK == Ret)
	{
		/* チェック処理実施 */
		TgtValue = atoi(Tgt);
		if ((EOTA_SETUP_TGT_MIN > TgtValue) || (EOTA_SETUP_TGT_MAX < TgtValue))
		{
			DLOG_Error("%s",EOTA_SETUP_TGT_RANGE_ERR);
			Ret = EOTA_CHECK_NG;
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

	return (uchar)(Ret);
}

/*==========================================================================*/
/*	Description		:	FW 更新指示ファイルのTGT項目チェック				*/
/*--------------------------------------------------------------------------*/
/*	param			:	TGT項目値											*/
/*						TGT項目種類											*/
/*						TGT項目リスト										*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_CHECK_OK          チェック正常					*/
/*						EOTA_CHECK_NG          チェック異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW 更新指示ファイルのTGT項目をチェックする			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_CheckSetupTGT(const schar * Tgt,		/* [i] TGT項目値    */
						 uchar 		   Type,	/* [i] TGT項目種類  */
						 schar *	   List		/* [o] TGT項目リスト*/
						)
{
	/* 変数初期化 */
	uchar	Ret				= EOTA_CHECK_OK;	/* 戻り値				*/
	schar *	StrStrRet		= NULL;				/* 文字列検索戻り値		*/
	sint	TgtValue		= 0;				/* TGT項目値			*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == Tgt) || (NULL == List) || (EOTA_UPD_TGT_J_B < Type))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_CHECK_NG;
	}
	else
	{
		/* 処理無し */
	}

	/* TGT項目設定値チェック */
	Ret = EOTA_CheckSetupTGTValue(Tgt);

	if (EOTA_CHECK_OK == Ret)
	{ 
		TgtValue = atoi(Tgt);
		if(0 != TgtValue)
		{
			/* 重複チェック */
			StrStrRet = strstr(List, Tgt);
			if (NULL != StrStrRet)
			{
				/* EOTA_SETUP_TGT_REPEAT_ERR */
				Ret = EOTA_CHECK_NG;
				DLOG_Error("%s",EOTA_SETUP_TGT_REPEAT_ERR);
			}
			else
			{
				List[Type] = *Tgt;
			}
		}
		else
		{
			List[Type] = *Tgt;
		}
	}
	else
	{
		Ret = EOTA_CHECK_NG;
	}

	DEBUG_TRCOUT();

	return (uchar)(Ret);
}

/*==========================================================================*/
/*	Description		:	FW 更新指示ファイルのTGT連続チェック				*/
/*--------------------------------------------------------------------------*/
/*	param			:	TGT項目リスト										*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_CHECK_OK          チェック正常					*/
/*						EOTA_CHECK_NG          チェック異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW 更新指示ファイルのTGT連続をチェックする			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_CheckSetupTGTOrder(const schar * List)/* [i] TGT項目リスト*/
{
	/* 変数初期化 */
	uchar	Ret						= EOTA_CHECK_OK;	/* 戻り値			*/
	schar *	str_chr_ret				= NULL;				/* 文字列検索戻り値	*/
	schar   Number[EOTA_UPD_TGT_LEN]= {EOTA_SETUP_TGT_NUM_1,
										EOTA_SETUP_TGT_NUM_2,
										EOTA_SETUP_TGT_NUM_3,
										EOTA_SETUP_TGT_NUM_4,
										EOTA_SETUP_TGT_NUM_5};
	uchar	ExitFlg				= EOTA_DATA_EXIT;	    /* データ存在フラグ	*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == List)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_CHECK_NG;
	}
	else
	{
		/* 処理無し */
	}

	/* 連続チェック */
	if (EOTA_CHECK_OK == Ret)
	{
		for (uchar loop = 0; loop < EOTA_UPD_TGT_LEN; loop++)
		{
			str_chr_ret = strchr(List, Number[loop]);
			if (NULL != str_chr_ret)
			{
				if (EOTA_DATA_NOT_EXIT == ExitFlg)
				{
					/* 連続でないの場合、エラーログを出力する */
					DLOG_Error("%s",EOTA_SETUP_TGT_ORDER_ERR);
					Ret = EOTA_CHECK_NG;
				}
				else
				{
					/* 処理無し */
				}
			}
			else
			{
				ExitFlg = EOTA_DATA_NOT_EXIT;
			}
		}
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return (uchar)(Ret);
}

/*==========================================================================*/
/*	Description		:	FW 更新指示ファイルのTGT全て0チェック				*/
/*--------------------------------------------------------------------------*/
/*	param			:	TGT項目リスト										*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_CHECK_OK          チェック正常					*/
/*						EOTA_CHECK_NG          チェック異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW 更新指示ファイルのTGTTGT全て0をチェックする		*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_CheckSetupTGTAllZero(const schar * List)/* [i] TGT項目リスト*/
{
	/* 変数初期化 */
	uchar	Ret				= EOTA_CHECK_OK;	/* 戻り値				*/
	sint	StrcmpRet		= 0;			 	/* 文字列比較戻り値		*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == List)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_CHECK_NG;
	}
	else
	{
		/* 処理無し */
	}

	/* TGT全て0チェック */
	if (EOTA_CHECK_OK == Ret)
	{
		StrcmpRet = strncmp(List, EOTA_SETUP_TGT_ALL_0, strlen(List));

		if (EOTA_STRCMP_INDENTICAL == StrcmpRet)
		{
			Ret = EOTA_CHECK_NG;

			/* TGT項目番号全て0エラー */
			DLOG_Error("%s",EOTA_SETUP_TGT_ALL_Z_ERR);
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

	return (uchar)(Ret);
}

/*==========================================================================*/
/*	Description		:	FW 更新指示ファイルのTIME項目チェック				*/
/*--------------------------------------------------------------------------*/
/*	param			:	TIME項目											*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_CHECK_OK          チェック正常					*/
/*						EOTA_CHECK_NG          チェック異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW 更新指示ファイルのTIME項目をチェックする			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_CheckSetupTime(const schar * Time)	/* [i] TIME項目		   */
{
	/* 変数初期化 */
	uchar	Ret				= EOTA_CHECK_OK;	/* 戻り値				*/
	size_t 	TimeLen			= 0u;				/* TIMEサイズ			*/
	schar *	StrStrRet		= NULL;				/* 文字列検索戻り値		*/
	schar *	TimePoint		= (schar *)Time;	/* 入力文字列ポイント	*/
	schar	TmpTime[EOTA_SETUP_TIME_LEN];
	memset(TmpTime, 0, sizeof(TmpTime));

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == Time))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_CHECK_NG;
	}
	else
	{
		/* 処理無し */
	}

	/* サイズチェック */
	if (EOTA_CHECK_OK == Ret)
	{
		TimeLen = strlen(Time);
		if (EOTA_SETUP_TIME_LEN != TimeLen)
		{
			/* サイズは不正の場合、エラーログを出力する */
			DLOG_Error("%s",EOTA_SETUP_TIME_LEN_ERR);
			Ret = EOTA_CHECK_NG;
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

	/* フォーマットチェック */
	if (EOTA_CHECK_OK == Ret)
	{
		StrStrRet = strstr(Time, EOTA_SETUP_TIME_UNDER);
		if (NULL != StrStrRet)
		{
			/* 前部分チェック */
			strncpy(TmpTime, TimePoint, EOTA_SETUP_TIME_F_LEN);

			/* チェック処理実施 */
			Ret = EOTA_is_digit(TmpTime);
			if (EOTA_CHECK_OK != Ret)
			{
				DLOG_Error("%s",EOTA_SETUP_TIME_FORM_ERR);
				Ret = EOTA_CHECK_NG;
			}
			else
			{
				/* 処理無し */
			}
			if (EOTA_CHECK_OK == Ret)
			{
				memset(TmpTime, 0, sizeof(TmpTime));
				TimePoint += (EOTA_SETUP_TIME_F_LEN + 1);

				/* 後部分チェック */
				strncpy(TmpTime, TimePoint, EOTA_SETUP_TIME_B_LEN);

				/* チェック処理実施 */
				Ret = EOTA_is_digit(TmpTime);
				if (EOTA_CHECK_OK != Ret)
				{
					DLOG_Error("%s",EOTA_SETUP_TIME_FORM_ERR);
					Ret = EOTA_CHECK_NG;
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
		}
		else
		{
			Ret = EOTA_CHECK_NG;

			/* フォーマットエラー */
			DLOG_Error("%s",EOTA_SETUP_TIME_FORM_ERR);
		}
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return (uchar)(Ret);
}

/*==========================================================================*/
/*	Description		:	FW 更新指示ファイルのバージョンチェック				*/
/*--------------------------------------------------------------------------*/
/*	param			:	FW 更新指示ファイルのFWバージョン					*/
/*						該当バージョン										*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_CHECK_OK          チェック正常					*/
/*						EOTA_CHECK_NG          チェック異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FWバージョンをチェックする							*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_CheckUpdateVersion(const schar * UpdVer,/* [i] FW 更新指示ファイルのFWバージョン */
							  const schar * CurVer)/* [i] 該当FWバージョン					*/
{
	/* 変数初期化 */
	uchar	Ret				= EOTA_CHECK_OK;	/* 戻り値				*/
	sint	StrcmpRet		= 0;				/* 文字列比較戻り値		*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == UpdVer) || (NULL == CurVer))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_CHECK_NG;
	}
	else
	{
		/* 処理無し */
	}

	/* FWバージョンチェック */
	if (EOTA_CHECK_OK == Ret)
	{
		StrcmpRet = strncmp(UpdVer, CurVer, strlen(UpdVer));

		if (EOTA_STRCMP_INDENTICAL == StrcmpRet)
		{
			Ret = EOTA_CHECK_NG;

			/* 更新バージョンは該当バージョンと同じ */
			DLOG_Info("%s", EOTA_UPD_VER_SAME);
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

	return (uchar)(Ret);
}

/*==========================================================================*/
/*	Description		:	FW 更新指示ファイルのMD5ハッシュ値チェック			*/
/*--------------------------------------------------------------------------*/
/*	param			:	MD5ファイル											*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_CHECK_OK          チェック正常					*/
/*						EOTA_CHECK_NG          チェック異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	MD5ハッシュ値をチェックする							*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_CheckMD5(const schar * Md5File)		/* [i] MD5ファイル */
{
	/* 変数初期化 */
	uchar	Ret						 = EOTA_CHECK_OK;	/* 戻り値					*/
	sint 	system_ret  		 	 = -1;				/* systemコマンドの戻り値 	*/
	schar	Cmd[EOTA_CMD_BUFSIZE]	 = {0};				/* コマンド				 	*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == Md5File)
	{
		/* パラメータはNULLの場合、エラーログを出力 */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_CHECK_NG;
	}
	else
	{
		/* 処理無し */
	}

	/* MD5ハッシュ値チェック */
	if (EOTA_CHECK_OK == Ret)
	{
		// コマンド
		snprintf(Cmd, sizeof(Cmd), "%s -c %s", EOTA_FILE_CMD_MD5SUM, Md5File);

		/* MD5チェック */
		system_ret = system(Cmd);
		if (EOTA_SYSTEM_RET == system_ret)
		{
			if ( EOTA_SYSTEM_RET != WEXITSTATUS(system_ret) )
			{
				/* MD5チェック不正の場合、エラーログを出力  */
				DLOG_Error("%s",EOTA_FILE_MD5_ERR);
				Ret = EOTA_CHECK_NG;
			}
			else
			{
				/* 処理無し */
			}
		}
		else
		{
			/* MD5チェック不正の場合、エラーログを出力  */
			DLOG_Error("%s",EOTA_FILE_MD5_ERR);
			Ret = EOTA_CHECK_NG;
		}
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return (uchar)(Ret);
}

/*==========================================================================*/
/*	Description		:	更新時間チェック									*/
/*--------------------------------------------------------------------------*/
/*	param			:	更新実施時刻										*/
/*--------------------------------------------------------------------------*/
/*	return			:	EOTA_CHECK_OK          チェック正常					*/
/*						EOTA_CHECK_NG          チェック異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	更新時間をチェックする								*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_UpdateCheckTime(const schar * UpdateTime)		/* [i] 更新実施時刻 */
{
	/* 変数初期化 */
	uchar		Ret		 		= EOTA_CHECK_OK;	/* 戻り値				*/
	schar		CurTime[EOTA_SETUP_TIME_SIZE + 1]
								= {0};				/* 現在時刻文字列		*/
	schar		Year[EOTA_TIME_YEAR_SIZE + 1]
								= {0};				/* 文字列(年)			*/
	sint		StrcmpRet		= 0;				/* 文字列比較戻り値		*/
	schar *		YearPoint		= NULL;				/* Yearポイント			*/
	time_t  	Now				= 0;				/* 現在時刻				*/
	struct tm * TmpPoint		= NULL;				/* tmポイント			*/

	DEBUG_TRCIN();

	/* 現在時刻取得 */
	time(&Now);
	TmpPoint = localtime(&Now);

	sprintf((schar*)Year, "%04d", (EOTA_TIME_YEAR_START + TmpPoint->tm_year));
	YearPoint = Year;
	YearPoint += 2;

	/* YYMMDD_hhmm */
	sprintf((schar*)CurTime,
			"%02d%02d%02d_%02d%02d",
			atoi(YearPoint),
			(TmpPoint->tm_mon + 1),
			TmpPoint->tm_mday,
			TmpPoint->tm_hour,
			TmpPoint->tm_min);

	/* 更新時間チェック */
	StrcmpRet = strncmp(CurTime, UpdateTime, EOTA_SETUP_TIME_SIZE);
	/* V:FW 更新実施時刻≧現在時刻の場合、EOTA_CHECK_OKを戻る */
	if (EOTA_STRCMP_INDENTICAL >= StrcmpRet)
	{
		Ret = EOTA_CHECK_OK;
	}
	/* FW 更新実施時刻＞現在時刻の場合、EOTA_CHECK_NGを戻る */
	else
	{
		Ret = EOTA_CHECK_NG;
	}

	DEBUG_TRCOUT();

	return (uchar)(Ret);
}


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

