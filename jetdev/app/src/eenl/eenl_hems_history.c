/****************************************************************************/
/*	Description		:ECHONET Lite application for hems history funciton		*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jiayu.li											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/17												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、EENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:無し													*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/
#include "eenl_cmn_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
extern EenlHemsHistory_t		g_eenlHemsHistory;		/* HEMS発電抑制履歴 */

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*==========================================================================*/
/*	Description		:HEMS発電抑制履歴取得									*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　fmsg	HEMS発電抑制履歴取得  要求  構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了)											*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/17 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_GetHemsHistory(EenlGetHemsHistory_t* fmsg)
{
	slong returnCode = EENL_OK;		/* 戻り値 */
	EenlGetHemsHistoryRet_t resMsg;	/* HEMS発電抑制履歴取得  応答  構造体 */

	DEBUG_TRCIN();

	/* 引数チェックは省略(NULLになりえない) */

	/* 構造体の初期化 */
	memset(&resMsg, 0, sizeof(resMsg));

	/* HEMS発電抑制履歴ファイルは読み出し済判定不要(起動時にしか読み出さないため) */

	if (g_eenlHemsHistory.data_num == 0) {
		returnCode = EENL_NO_DATA;
		DLOG_Debug("eenl_get_hems_history no data. ret=%ld", returnCode);
	} else {
		memcpy(
			&resMsg.hemshistory,
			&g_eenlHemsHistory,
			sizeof(EenlHemsHistory_t));
	}

	/* 同期通信の応答を生成 */
	resMsg.header.retCode = returnCode;

	/* 同期通信用リターンを返す */
	FRM_CallRet(fmsg->header.srcModId, sizeof(EenlGetHemsHistoryRet_t), (void*)&resMsg);

	DEBUG_TRCOUT();
	return EENL_OK;
}

/*==========================================================================*/
/*	Description		:HEMS発電抑制履歴読み出し								*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　hemsHist	HEMS発電抑制履歴						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/17 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_ReadHemsHistory(EenlHemsHistory_t* hemsHist)
{
	slong ret = EENL_OK;					/* 戻り値 */
	schar fileName[EENL_FILE_MAX_LEN] = {0};/* ファイルリスト */

	DEBUG_TRCIN();

	/* 引数チェックは省略(NULLになりえない) */
	memset(&fileName[0], 0, sizeof(fileName));

	snprintf(
		fileName,
		EENL_FILE_MAX_LEN,
		"%s",
		EENL_HEMS_HIST_FPATH);

	ret = EENL_CmnFileRead(
		fileName,
		hemsHist,
		sizeof(EenlHemsHistory_t));

	if (EENL_OK != ret)
	{
		if (EENL_ERR_FILE_OPEN == ret)
		{
			DLOG_Debug("EENL_ReadHemsHistory file open error. ret=%ld", ret);
			/* 履歴ファイル無しは異常としない */
			ret = EENL_OK;
		} else {
			DLOG_Error("EENL_ReadHemsHistory error. ret=%ld", ret);
			ret = EENL_ERR;
		}
	} else {
		DLOG_Debug("EENL_ReadHemsHistory success. ret=%ld", ret);
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:電圧上昇抑制履歴書き出し								*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　hemsHist	HEMS発電抑制履歴						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/17 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_WriteHemsHistory(EenlHemsHistory_t* hemsHist)
{
	slong ret = EENL_OK;				/* 戻り値 */
	schar fileName[EENL_FILE_MAX_LEN] = {0};/* ファイルリスト */

	DEBUG_TRCIN();

	/* 引数チェックは省略(NULLになりえない) */
	memset(&fileName[0], 0, sizeof(fileName));

	snprintf(
		fileName,
		EENL_FILE_MAX_LEN,
		"%s",
		EENL_HEMS_HIST_FPATH);

	ret = EENL_CmnFileWrite(
		fileName,
		hemsHist,
		sizeof(EenlHemsHistory_t));

	if (EENL_OK != ret)
	{
		DLOG_Error("eenl_writeHemsHistory error. ret=%ld", ret);
		ret = EENL_ERR;
	} else {
		DLOG_Debug("eenl_writeHemsHistory success. ret=%ld", ret);
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:HEMS発電抑制履歴の発電制御値変化チェック				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　value	発電制御値(%)(0～100)					*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:変更なし/1:変更あり/2:新しいデータ)					*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/17 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_CheckHemsHistValue(uchar value)
{
	slong ret = EENL_NOT_CHANGE;		/* 戻り値 */

	DEBUG_TRCIN();

	if ((EENL_HEMS_HIST_VALID == g_eenlHemsHistory.data[0].start.valid) &&
		(value != g_eenlHemsHistory.data[0].value))
	{
		DLOG_Debug("value change. value=%d, data[0].value=%d", value, g_eenlHemsHistory.data[0].value);
		ret = EENL_CHANGE;
	} else {
		/* 処理無し */
	}

	/* 0件でデータがない(＝ファイルが消失) */
	if ((g_eenlHemsHistory.data_num == 0) &&
		(EENL_HEMS_HIST_VALID != g_eenlHemsHistory.data[0].start.valid)) {
		/* 新しいデータとして記録する */
		ret = EENL_NEW_DATA;
	} else {
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:HEMS発電抑制履歴ファイル更新のラッパー関数				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　value	発電制御値(%)(0～100)					*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:変更なし/1:変更あり/2:新しいデータ)					*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/17 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
void EENL_ExtUpdateHemsHistory(uchar value)
{
	slong ret = 0;				/* 戻り値 */
	uchar check = EENL_CHANGE;	/* チェック結果 */

	DEBUG_TRCIN();

	/* 0件でデータがない(＝ファイルが消失) */
	if ((g_eenlHemsHistory.data_num == 0) &&
		(EENL_HEMS_HIST_VALID != g_eenlHemsHistory.data[0].start.valid)) {
		/* 新しいデータとして記録する */
		check = EENL_NEW_DATA;
	} else {
		/* 処理無し */
	}

	/* HEMS発電抑制履歴に記録 */
	ret = EENL_UpdateHemsHistory(value, check);
	if (ret == EENL_OK)
	{
		DLOG_Debug("solar update hems history success : %d", value);
	} else {
		DLOG_Debug("solar update hems history error (ret = %ld): %d", ret, value);
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:HEMS発電抑制履歴ファイル更新							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　value	発電制御値(%)(0～100)					*/
/*					:[in]　check	発電制御値変化チェック結果				*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/17 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_UpdateHemsHistory(uchar value, uchar check)
{
	slong ret = EENL_OK;		/* 戻り値 */
	slong etim_ret = ETIM_OK;	/* etim関数戻り値 */
	slong idx = 0;				/* データ要素数 */
	EtimTm_t etim_tm;			/* 時刻設定・取得データ */
	bool_t rtn_flg = FALSE;		/* 戻り値フラグ */

	DEBUG_TRCIN();

	DLOG_Debug("eenl_updateHemsHistory IN (value:%d, check:%d)", value, check);

	if (value > 100)
	{/* unsigned型のため、0未満はチェック不要 */
		DLOG_Error("parameter error. value=%d", value);
		ret = EENL_ERR;
		rtn_flg = TRUE;
	} else {
		/* 処理無し */
	}

	if ((EENL_NOT_CHANGE == check) && (rtn_flg == FALSE))
	{
		DLOG_Debug("value not change. do nothing. value=%d", value);
		ret = EENL_OK;
		rtn_flg = TRUE;
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		memset(&etim_tm, 0, sizeof(etim_tm));
		etim_ret = ETIM_GetTime(&etim_tm);
	
		if (ETIM_OK != etim_ret)
		{
			DLOG_Debug("etim_getTime error. ret=%ld", etim_ret);
			ret = EENL_ERR;
		} else {
			/* HEMS発電抑制履歴ファイルの読み直しはしない */
			/* (※読み出しエラー時しかあり得ないが、再度読み出しても状況は変わらないため) */

			if (EENL_CHANGE == check)
			{
				if (EENL_HEMS_HIST_VALID == g_eenlHemsHistory.data[0].start.valid)
				{
					/* 開始時刻を記録済の場合は、終了時刻を記録 */
					g_eenlHemsHistory.data[0].end.valid = EENL_HEMS_HIST_VALID;
					g_eenlHemsHistory.data[0].end.hours = (uchar)etim_tm.tmHour;
					g_eenlHemsHistory.data[0].end.minutes = (uchar)etim_tm.tmMin;

					DLOG_Debug("set end time. (%d,%02d:%02d)",
						g_eenlHemsHistory.data[0].end.valid,
						g_eenlHemsHistory.data[0].end.hours,
						g_eenlHemsHistory.data[0].end.minutes);
				} else {
					if (g_eenlHemsHistory.data_num > 0)
					{
						/* データ異常検知：処理は続行 */
						DLOG_Debug("data error. start time invalid.");
					} else {
						/* 処理無し */
					}
				}
			} else {
				/* 起動後の初回記録時は、常に新しいデータとして記録する */
				/* 処理無し */
			}

			/* HEMS発電抑制履歴の情報を１つずつ後ろにずらす(最新は常に[0]) */
			idx = g_eenlHemsHistory.data_num;
			if (idx >= EENL_HEMS_HISTORY_MAX)
			{
				idx = EENL_HEMS_HISTORY_MAX - 1;
			} else {
				/* 処理無し */
			}

			for (; idx > 0; idx--)
			{
				memcpy(&g_eenlHemsHistory.data[idx],
					&g_eenlHemsHistory.data[idx - 1],
					sizeof(EenlHemsHistData_t));
			}

			g_eenlHemsHistory.data[0].date.Year = (ushort)etim_tm.tmYear;
			g_eenlHemsHistory.data[0].date.Month = (uchar)etim_tm.tmMon;
			g_eenlHemsHistory.data[0].date.Day = (uchar)etim_tm.tmMday;

			g_eenlHemsHistory.data[0].start.valid = EENL_HEMS_HIST_VALID;
			g_eenlHemsHistory.data[0].start.hours = (uchar)etim_tm.tmHour;
			g_eenlHemsHistory.data[0].start.minutes = (uchar)etim_tm.tmMin;
			g_eenlHemsHistory.data[0].value = value;
			g_eenlHemsHistory.data[0].end.valid = EENL_HEMS_HIST_NOTSET;

			/* EENL_HEMS_HISTORY_MAXを超えるまでカウントし続ける */
			if (EENL_HEMS_HISTORY_MAX > g_eenlHemsHistory.data_num)
			{
				++g_eenlHemsHistory.data_num;
			} else {
				/* 処理無し */
			}

			DLOG_Debug("set start date and time. [%d](%04d/%02d/%02d,%d,%02d:%02d,%d)",
				g_eenlHemsHistory.data_num,
				g_eenlHemsHistory.data[0].date.Year,
				g_eenlHemsHistory.data[0].date.Month,
				g_eenlHemsHistory.data[0].date.Day,
				g_eenlHemsHistory.data[0].start.valid,
				g_eenlHemsHistory.data[0].start.hours,
				g_eenlHemsHistory.data[0].start.minutes,
				g_eenlHemsHistory.data[0].value);

			/* HEMS発電抑制履歴をファイルに書き込み */
			ret = EENL_WriteHemsHistory(&g_eenlHemsHistory);
			if (EENL_OK != ret)
			{
				DLOG_Error("eenl_writeHemsHistory error. ret=%ld", ret);
			} else {
				/* 処理無し */
			}
		}
	}
	else {
		/* 処理無し */
	}

	DLOG_Debug("eenl_updateHemsHistory OUT");

	DEBUG_TRCOUT();

	return ret;
}
