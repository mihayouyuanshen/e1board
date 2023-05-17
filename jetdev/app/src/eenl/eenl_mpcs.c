/****************************************************************************/
/*	Description		:機器オブジェクトマルチ入力PCSクラス 処理				*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jiayu.li											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/12												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、EENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:無し													*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/
#include "eenl_cmn_g.h"
#include "eenl_mpcs.h"

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
/*	Description		:マルチ入力PCSデバイスの作成							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info	デバイス情報								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_CreateDeviceMpcs(EenlDeviceInfo_t* info)
{
	slong ret = 0;
	uchar data = 0x00u;
	uchar seri[12] = { 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u };
	uchar prod[12] = { 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u };
	uchar work1[4] = { 0x07u, 0xDEu, 0x01u, 0x01u };
	uchar work_E8[31] = { 0x00u };
	uchar work_E8_pdc = 0u;
	uchar merr[4] = { 0x00u, 0x00u, 0x00u, 0x00u };
	slong work_F5[4] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	ulong work_F6[8] = { 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u };
	bool_t rtn_flg = FALSE;	/* 戻り値フラグ */

	DEBUG_TRCIN();

	DLOG_Debug("-----[create mpcs] start-----");
	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* マルチ入力PCSデバイスの作成 */
	ret = ENL_create_device_multi_input_pcs(&info->devMpcs, g_devicePropMpcs, g_devicePropMpcsExt, 0x01, ENL_INSTANCE_MODE_DEVICE);
	if (ret != ENL_RESULT_SUCCESS)
	{
		DLOG_Error("mpcs device create error!:%ld", ret);
		/* error */
		rtn_flg = TRUE;
		ret = -1;
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* インスタンスのアドレスを保持 */
		g_eenlDevMpcs = info->devMpcs;
		/* デバイスリストにインスタンスのアドレスを格納 */
		g_eenlDevList[EENL_DEV_LIST_MPCS] = g_eenlDevMpcs;
		DLOG_Debug("mpcs device created!!(instance:0x%08lX)", (ulong)g_eenlDevMpcs);

		/* 0x80: 動作状態 off */
		/* ライブラリの初期値とする */
		/* 状変アナウンスは無し */

		/* 0x81: 設置場所 */
		/* 屋内 */
		data = EENL_LOCATION_INSIDE;
		ret = ENL_DEV_set_property(NULL, info->devMpcs, 0x81u, 1u, &data);
		if (ret != ENL_RESULT_SUCCESS)
		{
			DLOG_Error("mpcs 0x81 init error!:%ld", ret);
			/* error */
			rtn_flg = TRUE;
			ret = -1;
		} else {
			/* 状変アナウンスは無し */
			/* 前回の設置場所情報を取得して反映 */
			/* 前回の設置場所情報は、eenl_fileRead()であらかじめグローバル変数に格納済み */
			DLOG_Debug("0x81 load value : 0x%02x", g_eenlMpcs0x81);
			ret = EENL_ExtEnlDevSetProperty(NULL, info->devMpcs, 0x81u, 1u, &g_eenlMpcs0x81);
			if (ret != ENL_RESULT_SUCCESS)
			{
				DLOG_Error("mpcs 0x81 load init fail(%02x) : %ld", g_eenlMpcs0x81, ret);
				/* 前回の情報が反映できなくても初期値で動作可能なので処理続行 */
			} else {
				/* 処理無し */
			}
		}
	} else {
		/* 処理無し */
	}

	/* 0x82: 規格バージョン AppendixK  （ライブラリ自動生成） */

	if (rtn_flg == FALSE)
	{
		/* 0x83: 識別番号 */
		memcpy(&g_eenlId[1], &g_eenlCodeNcc[0], 3);
		/* 蓄電池の識別番号5バイト目は押し上げ状態 */
		g_eenlId[4] = g_eenlPushedUp;
		memcpy(&g_eenlId[5], &prod[0], 12);
		ret = ENL_DEV_set_property(NULL, info->devMpcs, 0x83u, 17u, (uchar*)g_eenlId);
		if (ret != ENL_RESULT_SUCCESS)
		{
			DLOG_Error("mpcs 0x83 init error!:%ld", ret);
			/* error */
			rtn_flg = TRUE;
			ret = -1;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0x86: メーカ異常コード */
		memcpy(&merr[1], &g_eenlCodeNcc[0], 3);
		ret = ENL_DEV_set_property(NULL, info->devMpcs, 0x86u, 4u, (uchar*)merr);
		if (ret != ENL_RESULT_SUCCESS)
		{
			DLOG_Error("mpcs 0x86 init error!:%ld", ret);
			/* error */
			rtn_flg = TRUE;
			ret = -1;
		} else {
			/* 処理無し */
		}
		/* 状変アナウンスは無し */
	} else {
		/* 処理無し */
	}

	/* 0x88: 異常発生状態 */
	/* ライブラリの初期値とする */
	/* 状変アナウンスは無し */

	if (rtn_flg == FALSE)
	{
		/* 0x8A: メーカコード */
		ret = EENL_ExtEnlDevSetProperty(NULL, info->devMpcs, 0x8Au, 3u, (uchar*)g_eenlCodeNcc);
		if (ret != ENL_RESULT_SUCCESS)
		{
			DLOG_Error("mpcs 0x8A init error!:%ld", ret);
			/* error */
			rtn_flg = TRUE;
			ret = -1;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0x8C: 商品コード */
		ret = ENL_DEV_set_property(NULL, info->devMpcs, 0x8Cu, 12u, (uchar*)prod);
		if (ret != ENL_RESULT_SUCCESS)
		{
			DLOG_Error("mpcs 0x8C init error!:%ld", ret);
			/* error */
			rtn_flg = TRUE;
			ret = -1;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0x8D: 製造番号 */
		ret = ENL_DEV_set_property(NULL, info->devMpcs, 0x8Du, 12u, (uchar*)seri);
		if (ret != ENL_RESULT_SUCCESS)
		{
			DLOG_Error("mpcs 0x8D init error!:%ld", ret);
			/* error */
			rtn_flg = TRUE;
			ret = -1;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0x93: 遠隔操作設定 */
		/* ライブラリの初期値とする */
		/* EPC設定済フラグを更新しておく */
		EENL_SetEpcValidFlg(info->devMpcs, 0x93u);

		/* 0x97: 現在時刻設定 00:00 */
		/* ライブラリの初期値とする */

		/* 0x98: 現在年月日設定 2014/1/1 */
		ret = ENL_DEV_set_property(NULL, info->devMpcs, 0x98u, 4u, (uchar*)work1);
		if (ret != ENL_RESULT_SUCCESS)
		{
			DLOG_Error("mpcs 0x98 init error!:%ld", ret);
			/* error */
			rtn_flg = TRUE;
			ret = -1;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0xD0: 系統連系状態 */
		/* 独立 */
		data = 0x01;
		ret = ENL_DEV_set_property(NULL, info->devMpcs, 0xD0u, 1u, &data);
		if (ret != ENL_RESULT_SUCCESS)
		{
			DLOG_Error("mpcs 0xD0 init error!:%ld", ret);
			/* error */
			rtn_flg = TRUE;
			ret = -1;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	/* 0xE0: 積算電力量計測値（正方向） */
	/* ライブラリの初期値とする */

	/* 0xE3: 積算電力量計測値（逆方向） */
	/* ライブラリの初期値とする */

	/* 0xE7: 瞬時電力計測値 */
	/* ライブラリの初期値とする */

	if (rtn_flg == FALSE)
	{
		/* 0xE8: 接続機器リスト */
		eenl_MpcsCreate0xE8(work_E8);
		work_E8_pdc = (uchar)((work_E8[0] * 3) + 1);
		ret = EENL_ExtEnlDevSetProperty(NULL, info->devMpcs, 0xE8u, work_E8_pdc, (uchar*)work_E8);
		if (ret != ENL_RESULT_SUCCESS)
		{
			DLOG_Error("mpcs 0xE8 init error!:%ld", ret);
			/* error */
			rtn_flg = TRUE;
			ret = -1;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* PCSクラス(0xF5)  */
		/* 0xF5:瞬時電力計測値 */
		work_F5[0] = 0;				/* 売買電電力値 */
		work_F5[1] = 0;				/* 家庭消費電力値 */
		work_F5[2] = 0;				/* 外付太陽光発電電力値 */
		work_F5[3] = 0x7FFFFFFF;	/* 予約（0x7FFFFFFF） */
		ret = ENL_DEV_set_property(NULL, info->devMpcs, 0xF5u, 16u, (uchar*)work_F5);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0xF5 init error!:%ld", ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 使用後は元に戻す */
			memset(work_F5, 0x00, sizeof(work_F5));
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* PCSクラス(0xF6)  */
		/* 0xF6:積算電力量計測値 */
		work_F6[0] = 0u;				/* 順潮流方向の積算電力量 */
		work_F6[1] = 0u;				/* 逆潮流方向の積算電力量 */
		work_F6[2] = 0xFFFFFFFFu;	/* 予約（0xFFFFFFFF） */
		work_F6[3] = 0xFFFFFFFFu;	/* 予約（0xFFFFFFFF） */
		work_F6[4] = 0xFFFFFFFFu;	/* 予約（0xFFFFFFFF） */
		work_F6[5] = 0xFFFFFFFFu;	/* 予約（0xFFFFFFFF） */
		work_F6[6] = 0xFFFFFFFFu;	/* 予約（0xFFFFFFFF） */
		work_F6[7] = 0xFFFFFFFFu;	/* 予約（0xFFFFFFFF） */

		ret = ENL_DEV_set_property(NULL, info->devMpcs, 0xF6u, 32u, (uchar*)work_F6);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0xF6 init error!:%ld", ret);
			ret = -1;
		} else {
			/* 使用後は元に戻す */
			memset(work_F6, 0x00u, sizeof(work_F6));
		}
	} else {
		/* 処理無し */
	}

	DLOG_Debug("-----[create mpcs] end-----");

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:epcsデータの反映（マルチ入力PCS）						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info	デバイス情報								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_UpdateEpcsMpcs(EenlDeviceInfo_t* info)
{
	slong ret = 0;					/* 戻り値初期化 */
	uchar unitAddr = 1u;			/* アドレス */
	slong i = 0;					/* ループのカウンタ */
	EPCS_OPESTTS_INFO_T opeStts;	/* 動作情報構造体 */
	EPCS_SERIAL_NUM_T pcsSerial;	/* PCS機種名・製造番号情報構造体 */
	void* dev_type;					/* デバイスタイプ */

	DEBUG_TRCIN();

	DLOG_Debug("-----[epcs mpcs] update start-----");
	if (info == NULL)
	{
		DLOG_Error("-----[epcs mpcs] NULL-----");
		/* error */
		return -1;
	} else {
		/* 処理無し */
	}

	if (info->devMpcs == NULL)
	{
		/* マルチ入力PCSが無いなら */
		/* すべての機器構成でマルチ入力PCSクラスを作成するので */
		/* ライブラリでデバイスの作成に失敗しない限りはNULLにならない */
		DLOG_Debug("-----[epcs mpcs] update pass-----");
		/* errorにしない */
		return 0;
	} else {
		/* 処理無し */
	}

	dev_type = info->devMpcs;

	memset(&opeStts, 0x00, sizeof(opeStts));
	memset(&pcsSerial, 0x00, sizeof(pcsSerial));

	ret = epcs_sync_getOpeSttsInfo(&opeStts);
	if (ret != EPCS_OK)
	{
		DLOG_Error("getOpeSttsInfo error! : %ld", ret);
	} else {
		/* 処理無し */
	}

	/* 機種名・製造番号取得 */
	ret = epcs_sync_getPcsModelSerialNumber(unitAddr, &pcsSerial);
	if (ret != EPCS_OK)
	{
		DLOG_Error("getPcsModelSerialNumber error! : %ld", ret);
	} else {
		/* 処理無し */
	}

	DLOG_Debug("SerialNumber : ");
	for (i = 0; i < 17; i++) {
		DLOG_Debug("%c[%02x]", pcsSerial.pcsSerialNumber[i], pcsSerial.pcsSerialNumber[i]);
	}

	DLOG_Debug("ModelNumber : <%s>", pcsSerial.pcsModelName);
	/* ----- ECHONET Liteへ設定 ----- */
	/* 0x83 : 識別番号 */
	EENL_DevUpdate0x83(pcsSerial.pcsSerialNumber, dev_type);

	/* 0x8C : 商品コード */
	EENL_DevUpdate0x8C(pcsSerial.pcsModelName, dev_type);

	/* 0x8D : 製造番号 */
	EENL_DevUpdate0x8D(pcsSerial.pcsSerialNumber, dev_type);

	/* ***** [CMD21] ***** */
	/* ----- ECHONET Liteへ設定 ----- */
	/* 0x80 : 動作状態 */
	EENL_DevUpdate0x80(&opeStts, dev_type);

	/* 0xD0 : 系統連系状態 */
	eenl_MpcsUpdate0xD0(info, &opeStts);

	DLOG_Debug("-----[epcs mpcs] update end-----");

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:設定電文のチェック（マルチ入力PCS）					*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info	デバイス情報								*/
/*					:[in]　epc	設定対象EPC									*/
/*					:[in]　pdc	設定値のデータサイズ						*/
/*					:[in]　edt	設定値										*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_MpcsSetCheck(void* instance, uchar epc, uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_NOT_ACCEPT;	/* 戻り値初期化 */

	DEBUG_TRCIN();

	DLOG_Debug("-----mpcs set check-----");
	DLOG_Debug("epc:%02x  pdc:%02x  edt:%02x", epc, *pdc, *edt);

	switch (epc) {
	case 0x81:		/* 設置場所 */
		ret = EENL_DevSetCheck0x81(instance, pdc, edt);
		break;
	case 0x97:		/* 現在時刻設定 */
		ret = EENL_DevSetCheck0x97(pdc, edt);
		break;
	case 0x98:		/* 現在年月日設定 */
		ret = EENL_DevSetCheck0x98(pdc, edt);
		break;
	default:		/* その他 */
		DLOG_Debug("%02x not support", epc);
		ret = ENL_RESULT_NOT_ACCEPT;
		break;
	}

	DLOG_Debug("=> check : %ld", ret);
	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:マルチ入力PCSクラス 0xD0の設定処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xD0 	0xD0設定値								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_MpcsNotify0xD0(uchar val0xD0)
{
	slong ret = ENL_RESULT_SUCCESS;		/* 戻り値初期化 */
	EenlDeviceInfo_t* info = NULL;		/* デバイス情報 */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (NULL == info)
	{
		ret = ENL_RESULT_INVALID_PARAMETER;
		DLOG_Debug("%s leave.(%ld)", __FUNCTION__, ret);
		return ret;
	} else {
		/* 処理無し */
	}

	/* 設定値が決定している場合は、ENLに反映 */
	/* マルチ入力PCS0xD0 */
	if (info->devMpcs != NULL)
	{
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devMpcs, 0xD0u, val0xD0, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/* 成功 */
			DLOG_Debug("mpcs 0xD0 set success : 0x%02x", val0xD0);
			/* 状変アナウンスは無し */
		} else {
			/* 失敗 */
			DLOG_Error("mpcs 0xD0 set fail : 0x%02x(%ld)", val0xD0, ret);
			/* 異常でも処理続行 */
		}
	} else {
		/* 処理無し */
	}

	DLOG_Debug("%s leave.(%ld)", __FUNCTION__, ret);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:マルチ入力PCSクラス 0xE0の設定処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　ope 	動作情報構造体								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_MpcsTimer0xE0(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値初期化 */
	ulong val0xE0 = 0u;				/* 0xE0設定値 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devMpcs))
	{
		DLOG_Debug("%s leave.", __FUNCTION__);
		return ret;
	} else {
		/* 処理無し */
	}

	DLOG_Debug("ttlChargeEnergyInv : %ld", ope->ttlChargeEnergyInv);
	/* 範囲 0x000000000～0x3B9AC9FF(0～999,999,999) */
	/* epcsでECHONET規格の最大値超過がありえる */
	/* オーバーフロー時に0から再インクリメントとなるように補正する必要があるため、 */
	/* 常に規格上の最大値+1で剰余算する */
	val0xE0 = ope->ttlChargeEnergyInv % EENL_LIMIT_1000000000;

	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devMpcs, 0xE0u, val0xE0, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug("[timer mpcs] 0xE0 : 0x%08lx(%ld)", val0xE0, val0xE0);
		/* 状変アナウンスはなし */
	} else {
		DLOG_Error("[timer mpcs] 0xE0 pcs error!:%ld", ret);
		/* 異常でも処理続行 */
	}

	DLOG_Debug("%s leave.", __FUNCTION__);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:マルチ入力PCSクラス 0xE3の設定処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　ope 	動作情報構造体								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_MpcsTimer0xE3(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値初期化 */
	ulong val0xE3 = 0u;				/* 0xE3設定値 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devMpcs))
	{
		DLOG_Debug("%s leave.", __FUNCTION__);
		return ret;
	} else {
		/* 処理無し */
	}

	DLOG_Debug("ttlDischargeEnergyInv : %ld", ope->ttlDischargeEnergyInv);
	/* 範囲 0x000000000～0x3B9AC9FF(0～999,999,999) */
	/* epcsでECHONET規格の最大値超過がありえる */
	/* オーバーフロー時に0から再インクリメントとなるように補正する必要があるため、 */
	/* 常に規格上の最大値+1で剰余算する */
	val0xE3 = ope->ttlDischargeEnergyInv % EENL_LIMIT_1000000000;

	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devMpcs, 0xE3u, val0xE3, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug("[timer mpcs] 0xE3 : 0x%08lx(%ld)", val0xE3, val0xE3);
		/* 状変アナウンスはなし */
	} else {
		DLOG_Error("[timer mpcs] 0xE3 pcs error!:%ld", ret);
		/* 異常でも処理続行 */
	}

	DLOG_Debug("%s leave.", __FUNCTION__);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:マルチ入力PCSクラス 0xE7の設定処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　ope 	動作情報構造体								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_MpcsTimer0xE7(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値初期化 */
	ulong val0xE7 = 0u;				/* 0xE7設定値 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devMpcs))
	{
		DLOG_Debug("%s leave.", __FUNCTION__);
		return ret;
	} else {
		/* 処理無し */
	}

	DLOG_Debug("chargeDischargePwr : %ld", ope->chargeDischargePwr);
	val0xE7 = (ulong)ope->chargeDischargePwr;
	/* ECHONET Liteのほうが範囲が広いため範囲チェックはしない */

	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devMpcs, 0xE7u, val0xE7, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug("[timer mpcs] 0xE7 : 0x%08lx(%ld)", val0xE7, val0xE7);
		/* 状変アナウンスはなし */
	} else {
		DLOG_Error("[timer mpcs] 0xE7 pcs error!:%ld", ret);
		/* 異常でも処理続行 */
	}

	DLOG_Debug("%s leave.", __FUNCTION__);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:マルチ入力PCSクラス 0xD0の設定処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　ope 	動作情報構造体								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_MpcsTimer0xD0(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値初期化 */
	ulong val0xD0 = 0u;				/* 0xD0設定値 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devMpcs))
	{
		DLOG_Debug("%s leave.", __FUNCTION__);
		return ret;
	} else {
		/* 処理無し */
	}

	if (ope->pwrCut == EPCS_PWROUTAGE_YES)
	{
		/* 停電の場合 */
		val0xD0 = 0x01u;
	} else {
		/* 停電以外の場合 */
		val0xD0 = 0x00u;
	}

	/* 0xD0:系統連系状態（マルチ入力PCS）★ */
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devMpcs, 0xD0u, val0xD0, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug("[timer mpcs] 0xD0 : %d -> 0x%02lx", ope->pwrCut, val0xD0);
		/* 状変アナウンスはなし */
	} else {
		DLOG_Error("[timer mpcs] 0xD0 pcs error!:%ld", ret);
	}

	DLOG_Debug("%s leave.", __FUNCTION__);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:マルチ入力PCSクラス 0xF6の設定処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　ope 	動作情報構造体								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_MpcsTimer0xF6(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;			/* 戻り値 */
	slong func_ret = ENL_RESULT_SUCCESS;	/* 関数戻り値 */
	EenlDeviceInfo_t* info = NULL;			/* デバイス情報 */
	ulong work_F6[8] = { 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u };

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devMpcs))
	{
		DLOG_Debug("%s leave.", __FUNCTION__);
		return ret;
	} else {
		/* 処理無し */
	}

	/* 0xF6取得 */
	work_F6[0] = ope->ttlPurchasePwr;		/* 積算買電電力量(0.001kWh -> Wh) */
	work_F6[1] = ope->ttlSellingPwr;		/* 積算売電電力量(0.001kWh -> Wh) */
	work_F6[2] = 0xFFFFFFFFu;				/* 予約 */
	work_F6[3] = 0xFFFFFFFFu;				/* 予約 */
	work_F6[4] = 0xFFFFFFFFu;				/* 予約 */
	work_F6[5] = 0xFFFFFFFFu;				/* 予約 */
	work_F6[6] = 0xFFFFFFFFu;				/* 予約 */
	work_F6[7] = 0xFFFFFFFFu;				/* 予約 */

	/* 積算買電電力量(Wh)が0x00000000-0x3B9AC9FF(0-999,999,999)の範囲外なら */
	/* PGreliefでチェックに引っかかりますが、設定値の範囲がslongの最小値から最大値までなので偽になる事は無いが判断文としては残します。 */
	if (work_F6[0] > EENL_LIMIT_TTLPWR_MAX)
	{
		/* EPCへの設定反映を行わない */
		ret = ENL_RESULT_SUCCESS;
	} else {
		/* 積算売電電力量(Wh)が0x00000000-0x3B9AC9FF(0-999,999,999)の範囲外なら */
		/* PGreliefでチェックに引っかかりますが、設定値の範囲がslongの最小値から最大値までなので偽になる事は無いが判断文としては残します。 */
		if (work_F6[1] > EENL_LIMIT_TTLPWR_MAX)
		{
			/* EPCへの設定反映を行わない */
			ret = ENL_RESULT_SUCCESS;
		} else {
			work_F6[0] = EENL_BYTE_SWAP_32(work_F6[0]);
			work_F6[1] = EENL_BYTE_SWAP_32(work_F6[1]);
			work_F6[2] = EENL_BYTE_SWAP_32(work_F6[2]);
			work_F6[3] = EENL_BYTE_SWAP_32(work_F6[3]);
			work_F6[4] = EENL_BYTE_SWAP_32(work_F6[4]);
			work_F6[5] = EENL_BYTE_SWAP_32(work_F6[5]);
			work_F6[6] = EENL_BYTE_SWAP_32(work_F6[6]);
			work_F6[7] = EENL_BYTE_SWAP_32(work_F6[7]);

			/* 0xF6:積算電力量計測値 */
			func_ret = EENL_ExtEnlDevSetProperty(NULL, info->devMpcs, 0xF6u, 32u, (uchar*)work_F6);
			if (func_ret == ENL_RESULT_SUCCESS)
			{
				DLOG_Debug("[timer mpcs] 0xF6 : Set = [%ld]", func_ret);
				/* 状変アナウンスはなし */
			} else {
				DLOG_Error("[timer mpcs] 0xF6 pcs error!:%ld", func_ret);
				ret = func_ret;
			}
		}
	}

	DLOG_Debug("%s leave.", __FUNCTION__);

	DEBUG_TRCOUT();

	return ret;
}



/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*==========================================================================*/
/*	Description		:マルチ入力PCSクラス 0xE8(接続機器リスト)の初期化		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　work_E8 0xE8設定値								*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
static void eenl_MpcsCreate0xE8(uchar* work_E8)
{
	slong i = 0;					/* ループのカウンタ */
	slong work_E8_idx = 0;			/* 接続機器リストの添え字 */
	slong list_idx = 0;				/* EOJリストの添え字 */
	uchar work_E8_num = 0u;			/* 接続機器リスト数 */

	DEBUG_TRCIN();
	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* ノード、マルチ入力PCSは含めない */
	for (i = EENL_DEV_LIST_SO; i <= EENL_DEV_LIST_BT; i++)
	{
		if (g_eenlDevList[i] != NULL)
		{
			/* 接続機器リストの添え字を更新 */
			work_E8_idx = (work_E8_num * 3) + 1;
			/* 接続機器リストに各機器のEOJをコピー */
			memcpy(&work_E8[work_E8_idx], g_eenlEojList[list_idx], 3);
			/* 接続機器リスト数をインクリメント */
			work_E8_num++;
		} else {
			/* 処理無し */
		}
		/* EOJリストの添え字を更新 */
		list_idx++;
	}
	/* 接続機器リスト数を設定 */
	work_E8[0] = work_E8_num;

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:マルチ入力PCSクラス 0xD0(系統連系状態)のepcsデータの反映*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info	デバイス情報								*/
/*					:[in]　opeStts 動作情報構造体							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
static slong eenl_MpcsUpdate0xD0(EenlDeviceInfo_t* info, EPCS_OPESTTS_INFO_T* opeStts)
{
	slong ret = 0;			/* 戻り値初期化 */
	uchar val0xD0 = 0xFFu;	/* 0xD0設定値 */

	DEBUG_TRCIN();

	DLOG_Debug("pwrCut : %d", opeStts->pwrCut);

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	if (opeStts->pwrCut == EPCS_PWROUTAGE_YES)
	{
		/* 停電の場合 */
		val0xD0 = 0x01u;
	} else {
		/* 停電以外の場合 */
		val0xD0 = 0x00u;
	}

	/* 0xD0:系統連系状態（マルチ入力PCS）*/
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devMpcs, 0xD0u, val0xD0, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug("[epcs mpcs] 0xD0 : %d -> 0x%02x", opeStts->pwrCut, val0xD0);
		/* 状変アナウンスはなし */
	} else {
		DLOG_Error("mpcs 0xD0 pcs error!:%ld", ret);
	}

	DEBUG_TRCOUT();
	return ret;
}

