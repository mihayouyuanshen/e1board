/****************************************************************************/
/*	Description		:機器オブジェクトスーパークラス 処理					*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jiayu.li											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/04												*/
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


/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*==========================================================================*/
/*	Description		:機器クラス 0x80(動作状態)のepcsデータの反映			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　opeStts	ope 動作情報構造体						*/
/*					:[in]　dev_type	デバイスタイプ							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_DevUpdate0x80(EPCS_OPESTTS_INFO_T* opeStts, void* dev_type)
{
	slong ret = 0;					/* 戻り値を初期化する*/
	uchar val = 0x00u;				/* 機器クラス動作状態を初期化する */
	slong i = 0;					/* ループのカウンタ */
	slong loop_cnt = 0;				/* ループのカウンタ */
	ENL_MESSAGE* msg = NULL;		/* メッセージ */
	val = 0x31u;					/* 機器クラス動作状態を「OFF」に設定 */
	bool_t list_flag = FALSE;		/* リストフラグ */
	bool_t set0x80_flag = FALSE;	/* 0x80設定フラグ */

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* V2Hは状態変化通知にて設定するため、コールされない */
	/* ノードからはコールされないため、ループのカウンタはEENL_DEV_LIST_MPCS(1)から開始する */
	for (i = EENL_DEV_LIST_MPCS; i < EENL_DEV_LIST_MAX; ++i)
	{
		if (g_eenlDevList[i] == dev_type)
		{
			loop_cnt = i;
			i = EENL_DEV_LIST_MAX;
			list_flag = TRUE;
		} else {
			/* 処理無し */
		}
	}

	if (list_flag == TRUE)
	{
		/* 蓄電池クラス */
		if (loop_cnt == EENL_DEV_LIST_BT)
		{
			/* 蓄電池運転動作状態チェック */
			switch (opeStts->operation)
			{
			case EPCS_OPE_STOP:					/* 停止中(0) */
				val = 0x31u;					/* 機器クラス動作状態を「OFF」に設定 */
				break;
			case EPCS_OPE_WAIT:					/* 待機中(1)   */
				val = 0x30u;					/* 機器クラス動作状態を「ON」に設定 */
				break;
			default:
				val = 0x31u;					/* 機器クラス動作状態を「OFF」に設定 */
				break;
			}
			/* メッセージ出力 */
			DLOG_Debug("operation : %d -> 0x%02x", opeStts->operation, val);
			set0x80_flag = TRUE;
		} else if ((loop_cnt == EENL_DEV_LIST_MPCS) || (loop_cnt == EENL_DEV_LIST_SO) || (loop_cnt == EENL_DEV_LIST_METER_E1))
		{
			/* 太陽光発電クラス、分散型電源電力量メータクラス（E1）、マルチ入力PCSクラス */

			/* PCS運転状態チェック */
			switch (opeStts->pcsOperation)
			{
			case EINV_PCS_OPESTATE_STOP:		/* 停止中(0) */
				val = 0x31u;					/* 機器クラス動作状態を「OFF」に設定 */
				break;
			case EINV_PCS_OPESTATE_RUN:			/* 運転中(1)   */
				val = 0x30u;					/* 機器クラス動作状態を「ON」に設定 */
				break;
			default:
				val = 0x31u;					/* 機器クラス動作状態を「OFF」に設定 */
				break;
			}
			/* メッセージ出力 */
			DLOG_Debug("operation : %d -> 0x%02x", opeStts->operation, val);
			set0x80_flag = TRUE;
		} else {
			set0x80_flag = FALSE;
		}
	} else {
		/* 処理無し */
	}

	if (set0x80_flag == TRUE)
	{
		/* プロパティ値設定 */
		ret = EENL_ExtEnlDevSetProperty(&msg, g_eenlDevList[loop_cnt], 0x80u, 1u, &val);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Debug("g_eenlDevList[%ld] 0x80 : 0x%02x", loop_cnt, val);
			if (msg != NULL)
			{
				/* 状変アナウンス通知 */
				EENL_SendMessage(NULL, msg);
				DLOG_Debug("#### send broadcast message (g_eenlDevList[%ld]:0x80) ####", loop_cnt);
			} else {
				/* 処理無し */
			}
		} else {
			DLOG_Error("g_eenlDevList[%ld] 0x80 pcs error!:%ld", loop_cnt, ret);
		}
	} else {
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:機器クラス 0x83(識別番号)のepcsデータの反映			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　seri_no	製造番号								*/
/*					:[in]　dev_type	デバイスタイプ							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_DevUpdate0x83(schar* seri_no, void* dev_type)
{
	slong ret = 0;					/* 戻り値を初期化「0」する*/
	uchar bs = 0u;					/* データサイズ */
	slong i = 0;					/* ループのカウンタ */
	bool_t loop_flag = FALSE;		/* ループのフラグ */
	uchar val_0x83[EENL_PDC_0x83 + 1];			/* '\0'含めるため+1 */
	memset(&val_0x83, 0x00u, sizeof(val_0x83));	/* 戻り値を初期化「0」する*/

	DEBUG_TRCIN();

	/* データサイズ設定 */
	bs = EENL_PDC_0x83;

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */
	/* ノードからはコールされないため、ループのカウンタはEENL_DEV_LIST_MPCS(1)から開始する */
	for (i = EENL_DEV_LIST_MPCS; i < EENL_DEV_LIST_MAX && loop_flag == FALSE; ++i)
	{
		if (g_eenlDevList[i] == dev_type)
		{
			ret = ENL_get_property(g_eenlDevList[i], 0x83u, &bs, val_0x83);
			DLOG_Debug("g_eenlDevList[%ld] 0x83 get  -> ret: %ld, %d", i, ret, bs);

			/* 押し上げ情報を設定（5バイト目）*/
			val_0x83[4] = g_eenlPushedUp;

			/* 製造番号を設定（6バイト目以降に12バイト分）*/
			memcpy(&val_0x83[5], seri_no, (EENL_PDC_0x83 - 5));

			ret = EENL_ExtEnlDevSetProperty(NULL, g_eenlDevList[i], 0x83u, EENL_PDC_0x83, val_0x83);
			if (ret == ENL_RESULT_SUCCESS)
			{
				DLOG_Debug("[epcs g_eenlDevList[%ld]] 0x83 : [%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X]", i,
					val_0x83[0], val_0x83[1], val_0x83[2], val_0x83[3], val_0x83[4], val_0x83[5], val_0x83[6], val_0x83[7], val_0x83[8],
					val_0x83[9], val_0x83[10], val_0x83[11], val_0x83[12], val_0x83[13], val_0x83[14], val_0x83[15], val_0x83[16]);
			} else {
				DLOG_Error("g_eenlDevList[%ld] 0x83 pcs error!:%ld", i, ret);
				/* 処理続行 */
			}
			loop_flag = TRUE;
		} else {
			/* 処理無し */
		}
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:機器クラス 0x8C(商品コード)のepcsデータの反映			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　mod_name	機種名									*/
/*					:[in]　dev_type	デバイスタイプ							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/06 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_DevUpdate0x8C(schar* mod_name, void* dev_type)
{
	slong ret = 0;						/* 戻り値を初期化「0」する */
	uchar val_0x8C[EENL_PDC_0x8C + 1];	/* '\0'含めるため+1 */
	slong i = 0; 						/* ループのカウンタ */
	bool_t loop_flag = FALSE;			/* ループのフラグ */

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	memset(&val_0x8C, 0x00u, sizeof(val_0x8C));	/* 初期化 */
	memset(&val_0x8C, 0x20u, EENL_PDC_0x8C);	/* 予めスペースで埋めておく */

	/* ノードからはコールされないため、ループのカウンタはEENL_DEV_LIST_MPCS(1)から開始する */
	for (i = EENL_DEV_LIST_MPCS; i < EENL_DEV_LIST_MAX && loop_flag == FALSE; ++i)
	{
		if (g_eenlDevList[i] == dev_type)
		{
			/* 不足分はスペース埋め */
			memcpy(&val_0x8C[0], mod_name, strnlen(mod_name, EENL_PDC_0x8C));
			ret = EENL_ExtEnlDevSetProperty(NULL, g_eenlDevList[i], 0x8Cu, EENL_PDC_0x8C, val_0x8C);
			if (ret == ENL_RESULT_SUCCESS)
			{
				DLOG_Debug("[epcs g_eenlDevList[%ld]] 0x8C : [%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X]", i,
					val_0x8C[0], val_0x8C[1], val_0x8C[2], val_0x8C[3], val_0x8C[4], val_0x8C[5], val_0x8C[6], val_0x8C[7], val_0x8C[8], val_0x8C[9], val_0x8C[10], val_0x8C[11]);
				DLOG_Debug("[epcs g_eenlDevList[%ld]] 0x8C : [%s]", i, val_0x8C);
			} else {
				DLOG_Error("v2h 0x8C pcs error!:%ld", ret);
				/* 処理続行 */
			}
			loop_flag = TRUE;
		} else {
			/* 処理無し */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:機器クラス 0x8D(製造番号)のepcsデータの反映			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　seri_no	製造番号								*/
/*					:[in]　dev_type	デバイスタイプ							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/06 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_DevUpdate0x8D(schar* seri_no, void* dev_type)
{
	slong ret = 0;						/* 戻り値を初期化「0」する */
	uchar val_0x8D[EENL_PDC_0x8D + 1];	/* '\0'含めるため+1 */
	slong i = 0;						/* ループカウンタ */
	bool_t loop_flag = FALSE;			/* ループのフラグ */

	DEBUG_TRCIN();
	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	memset(&val_0x8D, 0x00u, sizeof(val_0x8D));/* 初期化 */
	memcpy(&val_0x8D[0], seri_no, EENL_PDC_0x8D);	/* 0x8D(製造番号)設定 */

	/* ノードからはコールされないため、ループのカウンタはEENL_DEV_LIST_MPCS(1)から開始する */
	for (i = EENL_DEV_LIST_MPCS; i < EENL_DEV_LIST_MAX && loop_flag == FALSE; ++i) {
		if (g_eenlDevList[i] == dev_type)
		{
			ret = EENL_ExtEnlDevSetProperty(NULL, g_eenlDevList[i], 0x8Du, EENL_PDC_0x8D, val_0x8D);
			if (ret == ENL_RESULT_SUCCESS)
			{
				DLOG_Debug("[epcs g_eenlDevList[%ld]] 0x8D : [%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X]", i,
					val_0x8D[0], val_0x8D[1], val_0x8D[2], val_0x8D[3], val_0x8D[4], val_0x8D[5], val_0x8D[6], val_0x8D[7], val_0x8D[8], val_0x8D[9], val_0x8D[10], val_0x8D[11]);
				DLOG_Debug("[epcs g_eenlDevList[%ld]] 0x8D : [%s]", i, val_0x8D);
			} else {
				DLOG_Error("g_eenlDevList[%ld] 0x8D pcs error!:%ld", i, ret);
				/* 処理続行 */
			}
			loop_flag = TRUE;
		} else {
			/* 処理無し */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}
/*==========================================================================*/
/*	Description		:機器クラス 0x81(設置場所)のepcsデータの反映			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　instance		インスタンスハンドル				*/
/*					:[in]　pdc			設定値のデータサイズ				*/
/*					:[in]　edt			設定値								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/06 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_DevSetCheck0x81(void* instance, uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_ACCEPT;	/* 戻り値を初期化「0」する */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */
	uchar wk = 0u;					/* 変数初期化 */

	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoInstance(instance);

	if (*pdc != 1)
	{
		/* サイズが1以外の場合 */
		ret = ENL_RESULT_NOT_ACCEPT;
	} else {
		/* サイズが1の場合 */
		if (*edt >= 0x01 && *edt <= 0x07)
		{
			/* 位置情報定義(0x01)および予約領域(0x02-0x07)の場合、設定不可 */
			ret = ENL_RESULT_NOT_ACCEPT;
		} else {
			/* 0x00, 0x08-0xFFの場合、設定可能 */
			DLOG_Debug("edt(0x%02x) check ok", *edt);

			/* 設置場所を　屋内(0x58)、屋外(0x60)のどちらかのみに制限せずに保存 */
			if ((instance == info->devMpcs) || (instance == info->devSolar) || (instance == info->devBattery))
			{
				/* マルチ入力PCS、太陽光、蓄電池の設置場所は共通 */
				/* 設定するデバイスに依存せず全てのデバイスで比較する */
				if ((g_eenlMpcs0x81 != *edt) || (g_eenlSolar0x81 != *edt) || (g_eenlBattery0x81 != *edt))
				{
					/* 設置場所をファイルに保存 */
					g_eenlMpcs0x81 = *edt;
					g_eenlSolar0x81 = *edt;
					g_eenlBattery0x81 = *edt;

					EENL_FileWrite();
					DLOG_Debug("save ok");

					/* 見守りサーバへの通知 */
					/* 上位5ビットを取得 */
					wk = *edt & 0x00F8;
					if ((wk >= 0x08) && (wk <= 0x58))
					{
						/* 室内 */
						ensv_async_notify_LocationChanged(ENSV_LOCATION_INSIDE);
						DLOG_Debug("location change call (inside) : 0x%02x", *edt);
					} else {
						/* 室外 */
						ensv_async_notify_LocationChanged(ENSV_LOCATION_OUTSIDE);
						DLOG_Debug("location change call (outside) : 0x%02x", *edt);
					}
				} else {
					/* 処理無し */
				}
			} else if (instance == info->devV2h)
			{
				if (g_eenlV2h0x81 != *edt)
				{
					/* 設置場所をファイルに保存 */
					g_eenlV2h0x81 = *edt;
					EENL_FileWrite();
					DLOG_Debug("save ok");
					/* 見守りサーバへの通知 */
					/* 下位3ビットをマスクして見守りサーバへ通知 */
					if ((*edt & 0xF8) == EENL_LOCATION_GARAGE)
					{
						/* 車庫 */
						ensv_async_notify_V2hLocationChanged(ENSV_LOCATION_INSIDE); /* 車庫設置 */
						DLOG_Debug("v2h location change call (inside) : 0x%02x", *edt);
					} else {
						/* 室外 */
						ensv_async_notify_V2hLocationChanged(ENSV_LOCATION_OUTSIDE); /* 屋外設置 */
						DLOG_Debug("v2h location change call (outside) : 0x%02x", *edt);
					}
				} else {
					DLOG_Debug("NOT change v2h location");
				}
			} else {
				/* 処理無し */
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:機器クラス 0x97(現在時刻設定)の設定電文チェック		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pdc			設定値のデータサイズ				*/
/*					:[in]　edt			設定値								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/06 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_DevSetCheck0x97(uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_ACCEPT;	/* 戻り値を初期化「0」する */
	slong r = ETIM_OK;				/* 時刻取得戻り値 */
	EtimTm_t pTime;					/* 時刻設定取得情報 */

	DEBUG_TRCIN();

	if (*pdc != 2)
	{
		/* データが2バイト以外の場合 */
		ret = ENL_RESULT_NOT_ACCEPT;
	} else {
		/* etimへ情報を設定 */
		/* [etim]現在時刻 */
		memset(&pTime, 0x00, sizeof(pTime));

		/* etimから現在時刻を取得 */
		r = ETIM_GetTime(&pTime);
		if (r == ETIM_OK)
		{
			DLOG_Debug("pTime hhmm : %02d:%02d ->", pTime.tmHour, pTime.tmMin);

			/* 年月日はそのままにして、時分のみ書き換え */
			pTime.tmHour = *edt;
			pTime.tmMin = *(edt + 1);

			DLOG_Debug(" %02d:%02d", pTime.tmHour, pTime.tmMin);

			/* etimへの時刻設定 */
			r = ETIM_SyncSetTime(&pTime);
			if (r == ETIM_OK)
			{
				/* 正常に設定 */
				DLOG_Debug("setTime(hhmm) success");
			} else {
				/* 設定に失敗 */
				DLOG_Error("setTime(hhmm) fail");
				/* ENLへの設定をできないように応答 */
				ret = ENL_RESULT_NOT_ACCEPT;
			}
		} else {
			/* etimから情報を取得できなかった場合 */
			DLOG_Error("getTime(hhmm) fail");
			/* ENLへの設定をできないように応答 */
			ret = ENL_RESULT_NOT_ACCEPT;
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:機器クラス 0x98(現在時刻設定)の設定電文チェック		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pdc			設定値のデータサイズ				*/
/*					:[in]　edt			設定値								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/06 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_DevSetCheck0x98(uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_ACCEPT;		/* 戻り値を初期化「0」する */
	sint yyyy = 0;						/* 年 */
	sint mm = 0;						/* 月 */
	sint dd = 0;						/* 日 */
	slong r = ETIM_OK;					/* 時刻取得戻り値 */
	EtimTm_t pTime;						/* 時刻設定取得情報 */

	DEBUG_TRCIN();

	if (*pdc != 4)
	{
		/* データが4バイト以外の場合 */
		ret = ENL_RESULT_NOT_ACCEPT;
	} else {
		/* データが4バイトの場合 */
		DLOG_Debug("epc 0x98 : %02x %02x %02x %02x", *edt, *(edt + 1), *(edt + 2), *(edt + 3));

		/* 1,2バイト目の範囲チェック */
		/*  現在年月日はの有効範囲は機器プロパティ規約に記載の範囲ではなく、室内リモコン要求仕様書に記載の入力範囲とする */
		/*  0x0001 - 0x270F(9999) ⇒ 0x07D0(2000年) - 0x07F5(2037年)に修正 */
		yyyy = (*edt * 256) + *(edt + 1);
		mm = *(edt + 2);
		dd = *(edt + 3);
		DLOG_Debug("year: %04x(%04d)", yyyy, yyyy);
		DLOG_Debug("month: %02x(%02d)", mm, mm);
		DLOG_Debug("day: %02x(%02d)", dd, dd);

		if (yyyy >= EENL_YEAR_LIMIT_MIN && yyyy <= EENL_YEAR_LIMIT_MAX)
		{
			/* etimへ情報を設定 */
			/* [etim]現在時刻 */
			memset(&pTime, 0x00, sizeof(pTime));

			/* etimから現在時刻を取得 */
			r = ETIM_GetTime(&pTime);
			if (r == ETIM_OK)
			{
				DLOG_Debug("pTime yyyymmdd : %04d/%02d/%02d ->", pTime.tmYear, pTime.tmMon, pTime.tmMday);

				/* 時分はそのままにして、年月日のみ書き換え */
				pTime.tmYear = yyyy;
				pTime.tmMon = mm;
				pTime.tmMday = dd;

				DLOG_Debug("%04d/%02d/%02d", pTime.tmYear, pTime.tmMon, pTime.tmMday);

				/* etimへの時刻設定 */
				r = ETIM_SyncSetTime(&pTime);
				if (r == ETIM_OK)
				{
					/* 正常に設定 */
					DLOG_Debug("setTime(yyyymmdd) success");
				} else {
					/* 設定に失敗 */
					DLOG_Error("setTime(yyyymmdd) fail");
					/* ENLへの設定をできないように応答 */
					ret = ENL_RESULT_NOT_ACCEPT;
				}

			} else {
				/* etimから情報を取得できなかった場合 */
				DLOG_Error("getTime(yyyymmdd) fail");
				/* ENLへの設定をできないように応答 */
				ret = ENL_RESULT_NOT_ACCEPT;
			}
		} else {
			/* 範囲外 */
			ret = ENL_RESULT_NOT_ACCEPT;
		}
	}

	DEBUG_TRCOUT();
	return ret;
}


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
