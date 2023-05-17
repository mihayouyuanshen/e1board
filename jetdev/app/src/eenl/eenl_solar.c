/****************************************************************************/
/*	Description		:機器オブジェクト太陽光発電クラス 処理					*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jiayu.li											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/06												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、EENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:無し													*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/
#include "eenl_cmn_g.h"
#include "eenl_solar.h"

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
/*	Description		:太陽光発電デバイスの作成と登録（ニチコン用）			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info	デバイス情報								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/06 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_CreateDeviceSolarNcc(EenlDeviceInfo_t* info)
{
	slong ret = 0;					/* 戻り値を初期化する*/
	uchar data = 0x00u;				/* 設定値 */
	bool_t rtn_flg = FALSE;			/* 戻り値フラグ */
	uchar prod[16] = { 0x20u, 0x20u, 0x20u, 0x20u,0x20u, 0x20u, 0x20u, 0x20u,0x20u, 0x20u, 0x20u, 0x20u,0x20u, 0x20u, 0x20u, 0x20u };
	uchar seri[16] = { 0x20u, 0x20u, 0x20u, 0x20u,0x20u, 0x20u, 0x20u, 0x20u,0x20u, 0x20u, 0x20u, 0x20u,0x20u, 0x20u, 0x20u, 0x20u };
	uchar work1[2] = { 0x00u, 0x00u };
	uchar work2[4] = { 0x07u, 0xDEu, 0x01u, 0x01u };
	uchar work3[4] = { 0x00u, 0x00u, 0x00u, 0x00u };
	uchar work4[2] = { 0x00u, 0x00u };
	uchar work5[3] = { 0x00u, 0x00u, 0x00u };
	uchar merr[4] = { 0x00u, 0x00u, 0x00u, 0x00u };
	uchar work_B0[100] = { 0x00u };
	uchar work_B1[7] = { 0x00u };
	slong work_F5[4] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	ushort mode_0xE8 = 0u;
	ushort mode_0xA2 = 0u;

	DEBUG_TRCIN();

	DLOG_Debug("-----[create solar(ncc)] start-----");
	if (info == NULL)
	{
		/* error */
		return -1;
	} else {
		/* 処理無し */
	}

	/* 太陽光デバイス */
	/* ニチコン仕向けの場合 */
	ret = ENL_create_device_home_solar_power_generation(&info->devSolar, g_devicePropSolarNccN, g_devicePropSolarExtNccN, 0x01u, ENL_INSTANCE_MODE_DEVICE);
	if (ret != ENL_RESULT_SUCCESS)
	{
		/* error */
		DLOG_Error("solar(ncc) device create error!:%ld", ret);
		rtn_flg = TRUE;
		ret = -1;
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/*  インスタンスのアドレスを保持 */
		g_eenlDevSolar = info->devSolar;
		DLOG_Debug("solar device created!!(instance:0x%08lX)", (ulong)g_eenlDevSolar);
		/* プロパティの初期値設定 */
		/*  0x80: 動作状態 */
		/* 「off」設定 */
		data = 0x31;
		ret = ENL_DEV_set_property(NULL, info->devSolar, 0x80u, 1u, &data);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0x80 init error!(%02x) : %ld", data, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0x81: 設置場所 */
		/* 室内 */
		data = EENL_LOCATION_INSIDE;
		ret = ENL_DEV_set_property(NULL, info->devSolar, 0x81u, 1u, &data);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0x81 init error!(%02x) : %ld", data, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 前回の設置場所情報を反映 */
			/* 前回の設置場所情報は、eenl_fileRead()であらかじめグローバル変数に格納済み */
			DLOG_Debug("0x81 load value : 0x%02x", g_eenlSolar0x81);
			ret = EENL_ExtEnlDevSetProperty(NULL, info->devSolar, 0x81u, 1u, &g_eenlSolar0x81);
			if (ret != ENL_RESULT_SUCCESS)
			{
				DLOG_Debug("solar(ncc) 0x81 load init fail(%02x) : %ld", g_eenlSolar0x81, ret);
				/* 前回の情報が反映できなくても初期値で動作可能なので処理続行 */
			} else {
				/* 処理無し */
			}
		}
		/* 状変アナウンスは無し */
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0x83: 識別番号 */
		memcpy(&g_eenlId[1], &g_eenlCodeNcc[0], 3);
		memcpy(&g_eenlId[5], &prod[0], 12);
		ret = ENL_DEV_set_property(NULL, info->devSolar, 0x83u, 17u, (uchar*)g_eenlId);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0x83 init error!(%s) : %ld", g_eenlId, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0x86: メーカ異常コード★ */
		memcpy(&merr[1], &g_eenlCodeNcc[0], 3);
		ret = ENL_DEV_set_property(NULL, info->devSolar, 0x86u, 4u, (uchar*)merr);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0x86 init error!(%02x %02x %02x %02x) : %ld", merr[0], merr[1], merr[2], merr[3], ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0x88: 異常発生状態 */
		/* 異常なし */
		data = 0x42;
		ret = ENL_DEV_set_property(NULL, info->devSolar, 0x88u, 1u, &data);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0x88 init error!(%02x) : %ld", data, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
		/* 状変アナウンスは無し */
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0x8A: メーカコード */
		/* ニチコンの場合 */
		memcpy(&work5[0], &g_eenlCodeNcc[0], 3);

		ret = EENL_ExtEnlDevSetProperty(NULL, info->devSolar, 0x8Au, 3u, (uchar*)work5);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0x8A init error!(%s) : %ld", work5, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0x8C: 商品コード */
		ret = ENL_DEV_set_property(NULL, info->devSolar, 0x8Cu, 12u, (uchar*)prod);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0x8C init error!(%s) : %ld", prod, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0x8D: 製造番号 */
		ret = ENL_DEV_set_property(NULL, info->devSolar, 0x8Du, 12u, (uchar*)seri);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0x8D init error!(%s) : %ld", seri, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0x93: 遠隔操作設定★ */
		/* 公衆回線未経由 */
		data = EENL_REMOTE_OFF;
		ret = EENL_ExtEnlDevSetProperty(NULL, info->devSolar, 0x93u, 1u, &data);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0x93 init error!(%02x) : %ld", data, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0x97: 現在時刻設定★ */
		ret = ENL_DEV_set_property(NULL, info->devSolar, 0x97u, 2u, (uchar*)work1);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0x97 init error!(%s) : %ld", work1, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0x98: 現在年月日設定★ */
		ret = ENL_DEV_set_property(NULL, info->devSolar, 0x98u, 4u, (uchar*)work2);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0x98 init error!(%s) : %ld", work2, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	/* 0xA0: 出力制御設定１ */
	/* 初期化しない(ライブラリの初期値とする) */

	if (rtn_flg == FALSE)
	{
		/* 0xA2: 余剰買取制御機能設定 */
		/* 0xA2の現在の設定状態を取得 */
		ret = ENL_get_operation_mode(info->devSolar, 0xA2u, &mode_0xA2);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0xA2 get_operation_mode error! : %ld", ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0xA2からSetを無効化★ */
		mode_0xA2 = (ushort)(mode_0xA2 - ENL_ENABLE_ACCESS_RULE_SET);

		/*0xA2の設定状態を更新 */
		ret = ENL_set_operation_mode(info->devSolar, 0xA2u, mode_0xA2);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0xA2 set_operation_mode error! : %ld", ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0x41(有効)固定 */
		data = 0x41;
		ret = EENL_ExtEnlDevSetProperty(NULL, info->devSolar, 0xA2u, 1u, &data);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0xA2 init error!(%02x) : %ld", data, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0xB0: 出力制御スケジュール */
		memset(work_B0, 0xFFu, sizeof(work_B0));
		ret = ENL_DEV_set_property(NULL, info->devSolar, 0xB0u, 100u, (uchar*)work_B0);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0xB0 init error!(%s) : %ld", work_B0, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0xB1: 次回アクセス日時 */
		memset(work_B1, 0xFFu, sizeof(work_B1));
		ret = ENL_DEV_set_property(NULL, info->devSolar, 0xB1u, 7u, (uchar*)work_B1);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0xB1 init error!(%s) : %ld", work_B1, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
		/* 状変アナウンスは無し */
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0xB2: 余剰買取制御機能タイプ */
		data = 0x41; /* 0x41(有効)固定 */
		ret = EENL_ExtEnlDevSetProperty(NULL, info->devSolar, 0xB2u, 1u, &data);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0xB2 init error!(%02x) : %ld", data, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}
	/* 0xB3: 出力変化時間設定値 */
	/* ライブラリの初期値とする */

	/* 0xB4: 上限クリップ設定値 */
	/* ライブラリの初期値とする */

	/* 0xC0: 運転力率設定値 */
	/* ライブラリの初期値とする */

	/* 0xC1: FIT契約タイプ */
	/* ライブラリの初期値とする */

	/* 0xC2: 自家消費タイプ */
	/* ライブラリの初期値とする */

	/* 0xC3: 設備認定容量 */
	/* ライブラリの初期値とする */

	/* 0xD0: 系統連系状態★ */
	/* ライブラリの初期値とする */

	if (rtn_flg == FALSE)
	{
		/* 0xD1: 出力抑制状態*/
		/* 出力抑制無し */
		data = 0x44;
		ret = ENL_DEV_set_property(NULL, info->devSolar, 0xD1u, 1u, &data);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0xD1 init error!(%02x) : %ld", data, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0xE0: 瞬時発電電力計測値 */
		ret = ENL_DEV_set_property(NULL, info->devSolar, 0xE0u, 2u, (uchar*)work1);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0xE0 init error!(%s) : %ld", work1, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0xE1: 積算発電電力量計測値 */
		ret = ENL_DEV_set_property(NULL, info->devSolar, 0xE1u, 4u, (uchar*)work3);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0xE1 init error!(%s) : %ld", work3, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0xE3: 積算売買電力量計測値 */
		ret = ENL_DEV_set_property(NULL, info->devSolar, 0xE3u, 4u, (uchar*)work3);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0xE3 init error!(%s) : %ld", work3, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0xE8: 定格発電電力値（系統連系時） */

		/* 0xE8の現在の設定状態を取得 */
		ret = ENL_get_operation_mode(info->devSolar, 0xE8u, &mode_0xE8);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0xE8 get_operation_mode error! : %ld", ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 0xE8からSetを無効化★ */
		mode_0xE8 = (ushort)(mode_0xE8 - ENL_ENABLE_ACCESS_RULE_SET);

		/* 0xE8の設定状態を更新 */
		ret = ENL_set_operation_mode(info->devSolar, 0xE8, mode_0xE8);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0xE8 set_operation_mode error! : %ld", ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		work4[0] = 0x00; work4[1] = 0x59;
		ret = ENL_DEV_set_property(NULL, info->devSolar, 0xE8u, 2u, (uchar*)work4);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0xE8 init error!(%s) : %ld", work4, ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 太陽光クラスEPC(0xF5)  */
		/* 0xF5:瞬時電力計測値 */
		work_F5[0] = 0;		/* 売買電電力値 */
		work_F5[1] = 0;		/* 家庭消費電力値 */
		work_F5[2] = 0;		/* 外付太陽光発電電力値 */
		work_F5[3] = 0x7FFFFFFF;	/* 予約（0x7FFFFFFF） */
		ret = ENL_DEV_set_property(NULL, info->devSolar, 0xF5u, 16u, (uchar*)work_F5);
		if (ret != ENL_RESULT_SUCCESS)
		{
			/* error */
			DLOG_Error("solar(ncc) 0xF5 init error!:%ld", ret);
			ret = -1;
			rtn_flg = TRUE;
		} else {
			memset(work_F5, 0x00, sizeof(work_F5));	/* 使用後は元に戻す */
			ret = 0;
		}
	} else {
		/* 処理無し */
	}

	DLOG_Debug("-----[create solar(ncc)] end-----");

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:epcsデータの反映（太陽光）								*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info	デバイス情報								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/07 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_UpdateEpcsSolarNcc(EenlDeviceInfo_t* info)
{
	EPCS_OPESTTS_INFO_T opeStts;			/* ope 動作情報構造体 */
	EPCS_SERIAL_NUM_T pcsSerial;			/* PCS機種名・製造番号情報構造体 */
	EPCS_SPDATA_GET_T spDataGet;			/* 整定値情報 */
	uchar unitAddr = 1u;					/* unitアドレス */
	slong ret = 0;							/* 戻り値を初期化する*/
	slong i = 0;							/* ループのカウンタ */
	void* dev_type;							/* デバイスタイプ */

	DEBUG_TRCIN();

	DLOG_Debug("-----[epcs solar] update start-----");
	if (info == NULL)
	{
		/* error */
		DLOG_Error("-----[epcs solar] NULL-----");
		return -1;
	} else {
		/* 処理無し */
	}

	if (info->devSolar == NULL)
	{
		/* solarが無いなら */
		DLOG_Debug("-----[epcs solar] update pass-----");
		/* errorにしない */
		return 0;
	} else {
		/* 処理無し */
	}

	/* 太陽光発電 */
	dev_type = info->devSolar;

	/* イベント登録は関数化して集約 */
	memset(&opeStts, 0x00, sizeof(opeStts));
	memset(&pcsSerial, 0x00, sizeof(pcsSerial));
	memset(&spDataGet, 0x00, sizeof(spDataGet));

	/* ope 動作取得 */
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

	/* 整定値・設定値取得 */
	ret = epcs_sync_getSpData(&spDataGet);
	if (ret != EPCS_OK)
	{
		DLOG_Error("getSpData error! : %ld", ret);
	} else {
		/* 処理無し */
	}

	DLOG_Debug("SerialNumber : ");
	for (i = 0; i < 17; i++) {
		DLOG_Debug("%c[%02x] ", pcsSerial.pcsSerialNumber[i], pcsSerial.pcsSerialNumber[i]);
	}

	DLOG_Debug("ModelNumber : <%s>", pcsSerial.pcsModelName);
	/* ----- ECHONET Liteへ設定 ----- */
	/* 0x83 : 識別番号（太陽光） */
	ret = EENL_DevUpdate0x83(pcsSerial.pcsSerialNumber, dev_type);

	/* 0x8C : 商品コード（太陽光） */
	ret = EENL_DevUpdate0x8C(pcsSerial.pcsModelName, dev_type);

	/* 0x8D : 製造番号（太陽光） */
	ret = EENL_DevUpdate0x8D(pcsSerial.pcsSerialNumber, dev_type);

	/* 0xE8 : 定格発電電力値(系統連系時) */
	ret = eenl_SolarUpdate0xE8(info, &spDataGet);

	/* ***** [CMD21] ***** */
	/* ----- ECHONET Liteへ設定 ----- */
	/* 0x80 : 動作状態 */
	ret = EENL_DevUpdate0x80(&opeStts, dev_type);

	/* 0xD0:系統連系状態 */
	/* 停電状態 */
	ret = eenl_SolarUpdate0xD0(info, &opeStts);

	DLOG_Debug("-----[epcs solar] update end-----");

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:epcsデータの反映（太陽光）								*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info	デバイス情報								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/07 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_UpdateEschSolar(EenlDeviceInfo_t* info)
{
	slong ret = 0;					/* 戻り値を初期化する*/
	uchar outputCtrlOnOff = 0u;		/* 対象外 */
	bool_t rtn_flg = FALSE;			/* 戻り値フラグ */

	DLOG_Debug("-----[esch solar] update start-----");

	if (info == NULL)
	{
		/* error */
		DLOG_Error("-----[esch solar] NULL-----");
		return -1;
	} else {
		/* 処理無し */
	}

	/* 出力制御対象/対象外取得 */
	ret = esch_sync_getOutputCtrlOnOff(&outputCtrlOnOff);
	if (ret != ESCH_OK)
	{
		DLOG_Error("getOutputCtrlOnOff error! : %ld", ret);
		if (ret == ESCH_ERR_SYSCALL)
		{
			/* システムコールエラーの場合はEPCに値を設定しない */
			/* 0xB0 出力制御スケジュールはここでEPC設定済フラグを更新しておかないと、 */
			/* リターン後に更新契機がないため常に不可応答になってしまう */
			EENL_SetEpcValidFlg(info->devSolar, 0xB0u);
			DLOG_Error("solar 0xB0 not set!");
			DLOG_Error("solar 0xB1 not set!");
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 出力制御対象判定 */
		if (outputCtrlOnOff == ESCH_OUTPUT_CTRL_ON)
		{
			/* 対象 */
			/* 0xB0: 出力制御スケジュール */
			eenl_SolarUpdate0xB0(info);

			/* 0xB1: 次回アクセス日時 */
			eenl_SolarUpdate0xB1(info);
		} else {
			/* 対象外 */
			/* create時に0xFFを設定しているため、EPC設定済フラグを更新しておく */
			EENL_SetEpcValidFlg(info->devSolar, 0xB0u);
			EENL_SetEpcValidFlg(info->devSolar, 0xB1u);
			DLOG_Debug("outputCtrlOnOff is off : %02x", outputCtrlOnOff);
		}
	} else {
		/* 処理無し */
	}

	DLOG_Debug("-----[esch solar] update end-----");

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:設定電文のチェック（太陽光）							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　instance	インスタンスハンドル					*/
/*					:[in]　epc		設定対象EPC								*/
/*					:[in]　pdc		設定値のデータサイズ					*/
/*					:[in]　edt		設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/07 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SolarSetCheck(void* instance, uchar epc, uchar* pdc, uchar* edt)
{
	/*
	 * 住宅用太陽光発電クラスの設定要求に対する電文がEchonetLite規格の電文構成の通りになっているかをチェックする
	 * チェックNG時は対応不可の応答を返す
	 */
	DEBUG_TRCIN();

	slong ret = ENL_RESULT_NOT_ACCEPT;		/* 戻り値を初期化する*/

	DLOG_Debug("-----solar set check-----");
	DLOG_Debug("epc:%02x  pdc:%02x  edt:%02x", epc, *pdc, *edt);

	switch (epc) {
		/* 設定可能なプロパティはパラメータのチェックを行ってACCEPTかNOT_ACCEPTかの判断を行う */
	case 0x81:		/* 設置場所 */
		ret = EENL_DevSetCheck0x81(instance, pdc, edt);
		break;
	case 0x97:		/* 現在時刻設定 */
		ret = EENL_DevSetCheck0x97(pdc, edt);
		break;
	case 0x98:		/* 現在年月日設定 */
		ret = EENL_DevSetCheck0x98(pdc, edt);
		break;
	case 0xC1:		/* FIT契約タイプ */
		ret = eenl_SolarSetCheck0xC1(edt);
		break;
		/* 太陽光クラス EPC(0xA0)*/
	case 0xA0:		/* 出力制御設定1 */
		ret = eenl_SolarSetCheck0xA0(edt);
		break;
		/* その他は全てSet不可能なプロパティなのでENL_RESULT_NOT_ACCEPTをセットする */
	default:  /* その他 */
		DLOG_Debug("%02x not accept", epc);
		ret = ENL_RESULT_NOT_ACCEPT;	/* 設定不可 */
		break;
	}

	DLOG_Debug("=> check : %ld", ret);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光デバイスのGetのコールバック処理					*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　epc		設定対象EPC								*/
/*					:[in]　edt		設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/07 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SolarCallbackRequestRead(uchar epc, uchar* edt)
{
	slong ret = ENL_RESULT_ACCEPT;	/* 戻り値を初期化する*/
	uchar outputCtrlOnOff = 0u; 	/* 対象外 */
	bool_t rtn_flg = FALSE;			/* 戻り値フラグ */

	DEBUG_TRCIN();

	if ((epc == 0xB0) || (epc == 0xB1))
	{
		/* 出力制御スケジュールまたは次回アクセス日時のGet要求なら */
		/* 出力制御対象/対象外取得 */
		ret = esch_sync_getOutputCtrlOnOff(&outputCtrlOnOff);
		if (ret != ESCH_OK)
		{
			DLOG_Error("getOutputCtrlOnOff error! : %ld", ret);
			if (ret == ESCH_ERR_SYSCALL)
			{
				/* システムコールエラーの場合は不可応答を返す */
				DLOG_Error("solar 0xB0 not set!");
				DLOG_Error("solar 0xB1 not set!");
				ret = ENL_RESULT_NOT_ACCEPT;
				rtn_flg = TRUE;
			} else {
				/* 処理無し */
			}
		} else {
			/* 処理無し */
		}

		if (rtn_flg == FALSE)
		{
			/* 出力制御対象判定 */
			if (outputCtrlOnOff == ESCH_OUTPUT_CTRL_ON)
			{
				/* 対象 */
				/* 対象の場合は出力制御スケジュールを取得する */
				/* 次回アクセス日時は状変で設定済みのため処理なし */
				if (epc == 0xB0)
				{
					ret = eenl_SolarGetOutputCtrlHistry(edt);
					/* エラーでも0xFFを設定しているため、retの値は確認しない */
				} else {
					/* 処理無し */
				}
			} else {
				/* 対象外 */
				/* 対象外の場合はALL 0xFFを設定する */
				if (epc == 0xB0)
				{
					memset(edt, 0xFFu, EENL_PDC_SOL_0xB0);
				} else if (epc == 0xB1) {
					memset(edt, 0xFFu, EENL_PDC_SOL_0xB1);
				} else {
					/* 処理無し */
				}
			}
			ret = ENL_RESULT_ACCEPT;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xB0(出力制御スケジュール)のeschデータの反映*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		デバイス情報							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/07 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
static slong eenl_SolarUpdate0xB0(EenlDeviceInfo_t* info)
{
	slong ret = 0;		/* 戻り値を初期化する*/
	uchar val0xB0[EENL_PDC_SOL_0xB0];	/* 太陽光クラス0xB0(出力制御スケジュール) */

	DEBUG_TRCIN();
	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* 初期化*/
	memset(&val0xB0, 0x00u, sizeof(val0xB0));

	/* 出力制御スケジュール取得 */
	ret = eenl_SolarGetOutputCtrlHistry(val0xB0);
	if (ret != EENL_OK)
	{
		/* create時に0xFFを設定しているため、EPC設定済フラグを更新しておく */
		EENL_SetEpcValidFlg(info->devSolar, 0xB0u);
		DLOG_Error("solar getOutputCtrlHistry error! : %ld", ret);
	} else {
		/* 0xB0:出力制御スケジュール（太陽光） */
		ret = EENL_ExtEnlDevSetProperty(NULL, info->devSolar, 0xB0u, EENL_PDC_SOL_0xB0, val0xB0);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Debug("[esch solar] 0xB0");
			/*状変アナウンスはなし */
		} else {
			DLOG_Error("solar 0xB0 error!:%ld", ret);
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xB1(次回アクセス日時)のeschデータの反映	*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		デバイス情報							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/07 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
static slong eenl_SolarUpdate0xB1(EenlDeviceInfo_t* info)
{
	slong ret = 0;			/* 戻り値を初期化する*/
	ENL_MESSAGE* msg = NULL;			/* メッセージ */
	ESCH_DATE_TIME_T nextDate, cmpSchDate;	/* esch日付型情報 */
	uchar val0xB1[EENL_PDC_SOL_0xB1];		/* 太陽光クラス0xB1(次回アクセス日時) */
	bool_t rtn_flg = FALSE;		/* 戻り値フラグ */

	DEBUG_TRCIN();
	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* 初期化 */
	memset(&val0xB1, 0x00u, sizeof(val0xB1));
	memset(&nextDate, 0x00, sizeof(nextDate));
	memset(&cmpSchDate, 0x00, sizeof(cmpSchDate));

	/* 次回アクセス日時取得 */
	ret = esch_sync_getNextAccessDate(&nextDate);
	if (ret != ESCH_OK)
	{
		/* create時に0xFFを設定しているため、EPC設定済フラグを更新しておく */
		EENL_SetEpcValidFlg(info->devSolar, 0xB1u);
		DLOG_Error("getNextAccessDate error! : %ld", ret);
		rtn_flg = TRUE;
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 次回アクセス日時の有無判定 */
		if ((memcmp(&nextDate, &cmpSchDate, sizeof(nextDate))) == 0)
		{
			/* 次回アクセス日時が取得できていない場合(ALL 0) */
			/* create時に0xFFを設定しているため、EPC設定済フラグを更新しておく */
			EENL_SetEpcValidFlg(info->devSolar, 0xB1u);
			DLOG_Debug("Next Access Date is none");
			ret = 0;
		} else {
			/* EPC用のデータ設定 */
			val0xB1[0] = (uchar)((nextDate.year & 0xFF00) >> 8);
			val0xB1[1] = (uchar)(nextDate.year & 0x00FF);
			val0xB1[2] = nextDate.month;
			val0xB1[3] = nextDate.day;
			val0xB1[4] = nextDate.hour;
			val0xB1[5] = nextDate.minute;
			val0xB1[6] = nextDate.second;

			/* 0xB1:次回アクセス日時（太陽光） */
			ret = EENL_ExtEnlDevSetProperty(&msg, info->devSolar, 0xB1u, EENL_PDC_SOL_0xB1, val0xB1);
			if (ret == ENL_RESULT_SUCCESS)
			{
				DLOG_Debug("[esch solar] 0xB1 : [%04d/%02d/%02d %02d:%02d:%02d]", ((val0xB1[0] << 8) + val0xB1[1]), val0xB1[2], val0xB1[3], val0xB1[4], val0xB1[5], val0xB1[6]);
				if (msg != NULL)
				{
					/* 状変アナウンス通知 */
					EENL_SendMessage(NULL, msg);
					DLOG_Debug("#### send broadcast message (solar:0xB1) ####");
				} else {
					/* 処理無し */
				}
			} else {
				DLOG_Error("solar 0xB1 error!:%ld", ret);
			}
		}
	} else {
		/* 処理無し */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xB1の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xB1			0xB1設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SolarNotify0xB1(uchar* val0xB1)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値初期化 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */
	ENL_MESSAGE* msg = NULL;		/* メッセージ */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する*/

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devSolar))
	{
		ret = ENL_RESULT_INVALID_PARAMETER;
		DLOG_Debug("\t\t %s leave.(%ld)", __FUNCTION__, ret);
		return ret;
	} else {
		/* 処理無し */
	}

	/* 0xB1 */
	ret = EENL_ExtEnlDevSetProperty(&msg, info->devSolar, 0xB1u, EENL_PDC_SOL_0xB1, val0xB1);
	if (ret == ENL_RESULT_SUCCESS)
	{
		/* 成功 */
		DLOG_Debug("solar 0xB1 set success : [%04d/%02d/%02d %02d:%02d:%02d]",
			((val0xB1[0] << 8) + val0xB1[1]), val0xB1[2], val0xB1[3], val0xB1[4], val0xB1[5], val0xB1[6]);
		if (msg != NULL)
		{
			/* 状変アナウンス通知 */
			EENL_SendMessage(NULL, msg);
			DLOG_Debug("#### send broadcast message (solar:0xB1) ####");
		} else {
			/* 処理無し */
		}
	} else {
		/* 失敗 */
		DLOG_Error("solar 0xB1 ENLLib error!:%ld", ret);
	}

	DLOG_Debug("%s leave.(%ld)", __FUNCTION__, ret);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xB3の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xB3			0xB3設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SolarNotify0xB3(ushort val0xB3)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値初期化 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devSolar))
	{
		ret = ENL_RESULT_INVALID_PARAMETER;
		DLOG_Debug("%s leave.(%ld)", __FUNCTION__, ret);
		return ret;
	} else {
		/* 処理無し */
	}

	/* 0xB3*/
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devSolar, 0xB3u, val0xB3, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		/* 成功 */
		DLOG_Debug("solar 0xB3 set success : 0x%04x(%d)", val0xB3, val0xB3);
		/* 状変アナウンスは無し */
	} else {
		/* 失敗 */
		DLOG_Error("solar 0xB3 ENLLib error!:%ld", ret);
	}

	DLOG_Debug("%s leave.(%ld)", __FUNCTION__, ret);

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xB4の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xB4			0xB4設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SolarNotify0xB4(ushort val0xB4)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値初期化 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devSolar))
	{
		ret = ENL_RESULT_INVALID_PARAMETER;
		DLOG_Debug("%s leave.(%ld)", __FUNCTION__, ret);
		return ret;
	} else {
		/* 処理無し */
	}

	/* 0xB4 */
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devSolar, 0xB4u, val0xB4, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		/* 成功 */
		DLOG_Debug("solar 0xB4 set success : 0x%04x(%d)", val0xB4, val0xB4);
		/* 状変アナウンスは無し */
	} else {
		/* 失敗 */
		DLOG_Error("solar 0xB4 ENLLib error!:%ld", ret);
	}

	DLOG_Debug("\t\t %s leave.(%ld)", __FUNCTION__, ret);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xC0の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xC0			0xC0設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SolarNotify0xC0(uchar val0xC0)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値初期化 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devSolar))
	{
		ret = ENL_RESULT_INVALID_PARAMETER;
		DLOG_Debug("%s leave.(%ld)", __FUNCTION__, ret);
		return ret;
	} else {
		/* 処理無し */
	}

	/* 0xC0 */
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devSolar, 0xC0u, val0xC0, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		/* 成功 */
		DLOG_Debug("solar 0xC0 set success : 0x%02x(%d)", val0xC0, val0xC0);
		/*状変アナウンスは無し */
	} else {
		/* 失敗 */
		DLOG_Error("solar 0xC0 ENLLib error!:%ld", ret);
	}

	DLOG_Debug("%s leave.(%ld)", __FUNCTION__, ret);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xC1の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xC1			0xC1設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SolarNotify0xC1(uchar val0xC1)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値初期化 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devSolar))
	{
		ret = ENL_RESULT_INVALID_PARAMETER;
		DLOG_Debug("%s leave.(%ld)", __FUNCTION__, ret);
		return ret;
	} else {
		/* 処理無し */
	}

	/* 0xC1 */
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devSolar, 0xC1u, val0xC1, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		/* 成功 */
		DLOG_Debug("solar 0xC1 set success : 0x%02x(%d)", val0xC1, val0xC1);
		/* 状変アナウンスは無し */
	} else {
		/* 失敗 */
		DLOG_Error("solar 0xC1 ENLLib error!:%ld", ret);
	}

	DLOG_Debug("%s leave.(%ld)", __FUNCTION__, ret);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xC2の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xC2			0xC2設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SolarNotify0xC2(uchar val0xC2)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値初期化 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devSolar))
	{
		ret = ENL_RESULT_INVALID_PARAMETER;
		DLOG_Debug("%s leave.(%ld)", __FUNCTION__, ret);
		return ret;
	} else {
		/* 処理無し */
	}

	/* 0xC2 */
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devSolar, 0xC2u, val0xC2, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		/* 成功 */
		DLOG_Debug("solar 0xC2 set success : 0x%02x(%d)", val0xC2, val0xC2);
		/*状変アナウンスは無し */
	} else {
		/* 失敗 */
		DLOG_Error("solar 0xC2 ENLLib error!:%ld", ret);
	}

	DLOG_Debug("%s leave.(%ld)", __FUNCTION__, ret);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xC3の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xC3			0xC3設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SolarNotify0xC3(ushort val0xC3)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値初期化 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devSolar))
	{
		ret = ENL_RESULT_INVALID_PARAMETER;
		DLOG_Debug("%s leave.(%ld)", __FUNCTION__, ret);
		return ret;
	} else {
		/* 処理無し */
	}

	/* 0xC3 */
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devSolar, 0xC3u, val0xC3, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		/* 成功*/
		DLOG_Debug("solar 0xC3 set success : 0x%04x(%d)", val0xC3, val0xC3);
		/* 状変アナウンスは無し */
	} else {
		/* 失敗 */
		DLOG_Error("solar 0xC3 ENLLib error!:%ld", ret);
	}

	DLOG_Debug("%s leave.(%ld)", __FUNCTION__, ret);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xD0の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xD0			0xD0設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SolarNotify0xD0(uchar val0xD0)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値初期化 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	info = EENL_GetDeviceInfoIndex(0);
	if (NULL == info)
	{
		DLOG_Debug("%s leave.", __FUNCTION__);
		return ENL_RESULT_INVALID_PARAMETER;
	} else {
		/* 処理無し */
	}

	if (val0xD0 != 0xFF)
	{
		/* 設定値が決定している場合は、ENLに反映*/
		/* 太陽光0xD0*/
		if (info->devSolar != NULL)
		{
			ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devSolar, 0xD0u, val0xD0, 0u);
			if (ret == ENL_RESULT_SUCCESS)
			{
				/*成功*/
				DLOG_Debug("[timer solar] 0xD0 set success : 0x%02x", val0xD0);
				/*状変アナウンスは無し*/
			} else {
				/*失敗*/
				DLOG_Error("[timer solar] 0xD0 set fail : 0x%02x(%ld)", val0xD0, ret);
				/*異常でも処理続行*/
			}
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	DLOG_Debug("%s leave.", __FUNCTION__);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xD1の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xD1			0xD1設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SolarNotify0xD1(uchar val0xD1)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値初期化 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */


	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devSolar))
	{
		ret = ENL_RESULT_INVALID_PARAMETER;
		DLOG_Debug("%s leave.(%ld)", __FUNCTION__, ret);
		return ret;
	} else {
		/* 処理無し */
	}

	/* 0xD1 */
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devSolar, 0xD1u, val0xD1, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		/*成功 */
		DLOG_Debug("solar 0xD1 set success : 0x%02x(%d)", val0xD1, val0xD1);
		/*状変アナウンスは無し */
	} else {
		/*失敗 */
		DLOG_Error("solar 0xD1 ENLLib error!:%ld", ret);
	}

	DLOG_Debug("%s leave.(%ld)", __FUNCTION__, ret);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xA0の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pvClassInfo	太陽光発電クラス情報				*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SolarTimer0xA0(const EPCS_PV_CLASS_T* pvClassInfo)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値初期化 */
	uchar val0xA0 = 0u;				/* 出力制御設定[%]用 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devSolar))
	{
		DLOG_Debug("%s leave.", __FUNCTION__);
		return ret;
	} else {
		/* 処理無し */
	}

	/* 範囲チェックはepcsで実施済み */
	/* 太陽光発電クラス情報の出力制御設定[%]を設定 */
	val0xA0 = pvClassInfo->otpCtrlSettingPer;

	/* 0xA0 太陽光 */
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devSolar, 0xA0u, val0xA0, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		/*成功 */
		DLOG_Debug("[timer solar] 0xA0 pcs success: %d", val0xA0);
		/*状変アナウンスは無し */
	} else {
		DLOG_Error("[timer solar] 0xA0 pcs error! : %d(%ld)", val0xA0, ret);
		/*失敗しても処理続行 */
	}

	DLOG_Debug("%s leave.", __FUNCTION__);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xE0の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pvClassInfo	ope 動作情報構造体					*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SolarTimer0xE0(const EPCS_PV_CLASS_T* pvClassInfo)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値初期化 */
	slong val2 = 0;					/* 瞬時発電電力計測値[W] */
	slong val0xE0 = 0;				/* 0xE0設定 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	if (NULL == pvClassInfo)
	{
		DLOG_Debug("%s leave.", __FUNCTION__);
		return ret;
	} else {
		/* 処理無し */
	}

	info = EENL_GetDeviceInfoIndex(0);
	if ((NULL == info) || ((NULL != info) && (NULL == info->devSolar)))
	{
		DLOG_Debug("%s leave.", __FUNCTION__);
		return ret;
	} else {
		/* 処理無し */
	}

	val2 = pvClassInfo->pvGenPwr2;
	if (val2 > EENL_LIMIT_65533)
	{
		/*65533より大きい場合、規格に規定されているオーバーフロー値(0xFFFF)に設定 */
		val0xE0 = 0xFFFF;
		DLOG_Debug("[timer solar] 0xE0 %ld -> %ld(0x%lx) overflow", val2, val0xE0, val0xE0);
	} else if (val2 < 0) {
		/*0より小さい場合、規格に規定されているアンダーフロー値(0xFFFE)に設定 */
		val0xE0 = 0xFFFE;
		DLOG_Debug("[timer solar] 0xE0 %ld -> %ld(0x%lx) underflow", val2, val0xE0, val0xE0);
	} else {
		/*範囲内の場合、そのまま利用 */
		val0xE0 = val2;
		DLOG_Debug("[timer solar] 0xE0 %ld(0x%lx) no-change", val0xE0, val0xE0);
	}
	DLOG_Debug("[timer solar] 0xE0 %ld -> %ld(0x%lx)", val2, val0xE0, val0xE0);
	/* 0xE0 太陽光 */
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devSolar, 0xE0u, (ulong)val0xE0, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		/* 成功 */
		DLOG_Debug("[timer solar] 0xE0 pcs success: %ld", val0xE0);
		/* 状変アナウンスは無し */
	} else {
		DLOG_Error("[timer solar] 0xE0 pcs error! : %ld(%ld)", val0xE0, ret);
		/* 失敗しても処理続行 */
	}

	DLOG_Debug("%s leave.", __FUNCTION__);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xE1の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pvClassInfo	ope 動作情報構造体					*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SolarTimer0xE1(const EPCS_PV_CLASS_T* pvClassInfo)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値初期化 */
	ulong val2 = 0u;				/* 積算発電電力量計測値[Wh] */
	ulong val0xE1 = 0u;				/* 0xE1設定 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	if (NULL == pvClassInfo)
	{
		DLOG_Debug("%s leave.", __FUNCTION__);
		return ret;
	} else {
		/* 処理無し */
	}

	info = EENL_GetDeviceInfoIndex(0);
	if ((NULL == info) || ((NULL != info) && (NULL == info->devSolar)))
	{
		DLOG_Debug("%s leave.", __FUNCTION__);
		return ret;
	} else {
		/* 処理無し */
	}

	val2 = pvClassInfo->ttlPvGenPwr2;
	val0xE1 = val2;
	if (val0xE1 > EENL_LIMIT_999999999)
	{
		/* 999999999より大きい場合、積算値をローテーション */
		while (val0xE1 > EENL_LIMIT_999999999) {
			/* 999999999より大きい間、繰り返す */
			val0xE1 = val0xE1 - 1000000000;
			DLOG_Debug("rotate %ld -> 0x%08lx(%ld)", val2, val0xE1, val0xE1);
		}
		DLOG_Debug("[timer solar] 0xE1 %ld -> 0x%08lx(%ld) rotate", val2, val0xE1, val0xE1);
	} else {
		/* 999999999以下の場合 */
		/* 何もしない */
		/* val0xE1はunsigned型のため負数は無し */
	}
	DLOG_Debug("[timer solar] 0xE1 %ld -> 0x%08lx(%ld)", val2, val0xE1, val0xE1);
	/* 0xE1 太陽光 */
	info = EENL_GetDeviceInfoIndex(0);
	if (info != NULL)
	{
		/* 0xE1 */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devSolar, 0xE1u, val0xE1, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/* 成功 */
			DLOG_Debug("[timer solar] 0xE1 pcs success: %ld", val0xE1);
			DLOG_Debug("[timer solar] 0xE1 success: %ld", val0xE1);
			/* 状変アナウンスは無し */
		} else {
			DLOG_Error("[timer solar] 0xE1 pcs error! : %ld(%ld)", val0xE1, ret);
			/* 失敗しても処理続行 */
		}
	} else {
		DLOG_Error("[timer solar] 0xE1 pcs error! : null");
		/*失敗しても処理続行 */
	}

	DLOG_Debug("%s leave.", __FUNCTION__);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xE3の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pvClassInfo	ope 動作情報構造体					*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SolarTimer0xE3(const EPCS_PV_CLASS_T* pvClassInfo)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値初期化 */
	ulong val2 = 0u;				/* 積算売電電力量計測値[Wh] */
	ulong val0xE3 = 0u;				/* 0xE3設定 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	if (NULL == pvClassInfo)
	{
		DLOG_Debug("%s leave.", __FUNCTION__);
		return ret;
	} else {
		/* 処理無し */
	}

	info = EENL_GetDeviceInfoIndex(0);
	if ((NULL == info) || ((NULL != info) && (NULL == info->devSolar)))
	{
		DLOG_Debug("%s leave.", __FUNCTION__);
		return ret;
	} else {
		/* 処理無し */
	}

	val2 = pvClassInfo->ttlSellingPwr;
	val0xE3 = val2;
	if (val0xE3 > EENL_LIMIT_999999999) {
		/* 999999999より大きい場合、積算値をローテーション */
		while (val0xE3 > EENL_LIMIT_999999999) {
			/* 999999999より大きい間、繰り返す */
			val0xE3 = val0xE3 - 1000000000;
			DLOG_Debug("rotate %ld -> 0x%08lx(%ld)", val2, val0xE3, val0xE3);
		}
		DLOG_Debug("[timer solar] 0xE3 %ld -> 0x%08lx(%ld) rotate", val2, val0xE3, val0xE3);
	} else {
		/* 999999999以下の場合 */
		/* 処理無し */
		/* val0xE3はunsigned型のため負数は無し */
	}
	DLOG_Debug("[timer solar] 0xE3 %ld -> 0x%08lx(%ld)", val2, val0xE3, val0xE3);
	/* 0xE3 太陽光 */
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devSolar, 0xE3u, val0xE3, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		/* 成功 */
		DLOG_Debug("[timer solar] 0xE3 pcs success: %ld", val0xE3);
		DLOG_Debug("[timer solar] 0xE3 success: %ld", val0xE3);
		/* 状変アナウンスは無し */
	} else {
		DLOG_Error("[timer solar] 0xE3 pcs error! : %ld(%ld)", val0xE3, ret);
		/* 失敗しても処理続行 */
	}

	DLOG_Debug("%s leave.", __FUNCTION__);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xD0の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pvClassInfo	ope 動作情報構造体					*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SolarTimer0xD0(const EPCS_PV_CLASS_T* pvClassInfo)
{
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */
	slong ret = 0;					/* 戻り値 */
	uchar val0xD0 = 0u;				/* 0xD0設定 */

	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	if (NULL == pvClassInfo)
	{
		DLOG_Debug("%s leave.", __FUNCTION__);
		return ret;
	} else {
		/* 処理無し */
	}
	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devSolar))
	{
		DLOG_Debug("%s leave.", __FUNCTION__);
		return ret;
	} else {
		/* 処理無し */
	}

	DLOG_Debug("pwrCut : %d", pvClassInfo->pwrCut);
	if (pvClassInfo->pwrCut == EPCS_PWROUTAGE_YES)
	{
		/* 停電の場合 */
		val0xD0 = 0x01u;
	} else {
		/* 停電以外の場合 */
		val0xD0 = 0x00u;
	}

	if (val0xD0 != 0xFF)
	{
		/*0xD0:系統連系状態（太陽光）★ */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devSolar, 0xD0u, val0xD0, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Debug("[timer solar] 0xD0 : %d -> 0x%02x", pvClassInfo->pwrCut, val0xD0);
			/*状変アナウンスはなし */
		} else {
			DLOG_Error("[timer solar] 0xD0 pcs error!:%ld", ret);
		}
	} else {
		/* エラーにしない処理続行 */
		DLOG_Error("[timer solar] 0xD0 not set!: %d", val0xD0);
	}

	DLOG_Debug("%s leave.", __FUNCTION__);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xD1の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pvClassInfo	ope 動作情報構造体					*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_SolarTimer0xD1(const EPCS_PV_CLASS_T* pvClassInfo)
{
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */
	slong ret = 0;					/* 戻り値 */
	uchar val0xD1 = 0u;				/* 0xD0設定 */
	bool_t rtn_flag = FALSE;		/* 戻り値フラグ */


	DEBUG_TRCIN();

	DLOG_Debug("%s enter.", __FUNCTION__);

	if (NULL == pvClassInfo)
	{
		DLOG_Debug("%s leave.", __FUNCTION__);
		return ret;
	} else {
		/* 処理無し */
	}
	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devSolar))
	{
		DLOG_Debug("%s leave.", __FUNCTION__);
		return ret;
	} else {
		/* 処理無し */
	}

	/* einvから取得した出力抑制状態を変換する */
	/* 0x00:抑制なし       = 0x44:抑制未実施 */
	/* 0x01:電圧上昇抑制中 = 0x42:抑制中(出力制御以外) */
	/* 0x02:出力制御中     = 0x41:抑制中(出力制御) */
	switch (pvClassInfo->otpSuppressState)
	{
	case EPCS_PVGEN_NOTICE_SUPPRESS_NONE:	/* 0x00 抑制なし */
		val0xD1 = 0x44u; /* 抑制未実施 */
		break;
	case EPCS_PVGEN_NOTICE_SUPPRESS_VOLT:	/* 0x01 電圧上昇抑制中 */
		val0xD1 = 0x42u; /* 抑制中(出力制御以外) */
		break;
	case EPCS_PVGEN_NOTICE_SUPPRESS_OUTPUT:	/* 0x02 出力制御中 */
		val0xD1 = 0x41u; /* 抑制中(出力制御) */
		break;
	default:
		rtn_flag = TRUE;
		ret = -1;
		break;
	}

	if (rtn_flag == FALSE)
	{
		/*0xD0:出力抑制状態★ */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devSolar, 0xD1u, val0xD1, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/* 成功 */
			DLOG_Debug("[timer solar]  0xD1 pcs success : 0x%02x(%d)", val0xD1, val0xD1);
			/* 状変アナウンスは無し */
		} else {
			/* 失敗 */
			DLOG_Error("[timer solar]  0xD1 psc error!:%ld", ret);
		}
	} else
	{
		/* 処理無し */
	}

	DLOG_Debug("%s leave.", __FUNCTION__);

	DEBUG_TRCOUT();

	return ret;
}


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*==========================================================================*/
/*	Description		:太陽光クラス 0xE8(定格発電電力値(系統連系時))のepcsデータの反映*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info			デバイス情報						*/
/*					:[in]　spDataGet	整定値情報							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/07 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
static slong eenl_SolarUpdate0xE8(EenlDeviceInfo_t* info, EPCS_SPDATA_GET_T* spDataGet)
{
	slong ret = 0;			/* 戻り値初期化 */
	ulong work_E8 = 0u;		/* work_E8初期化 */
	ushort val0xE8 = 0u;	/* val0xE8初期化 */

	DEBUG_TRCIN();
	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* epcsから0.01kW単位で取得するので、Wに変換する */
	/* 0590[0.01kW]→5900[W] */
	work_E8 = (ulong)(spDataGet->pcsRatedPwr * 10);

	/* 範囲チェック */
	if (work_E8 > EENL_LIMIT_65533)
	{
		/*65533より大きい場合、65533に制限 */
		val0xE8 = EENL_LIMIT_65533;
		DLOG_Debug("[epcs solar] 0xE8 %ld -> %d(0x%x) limit over", work_E8, val0xE8, val0xE8);
	} else {
		/*範囲内なので、そのまま利用 */
		val0xE8 = (ushort)work_E8;
	}

	/*0xE8:定格発電電力値(系統連系時) */
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devSolar, 0xE8u, val0xE8, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		/*状変アナウンスはなし */
		DLOG_Debug("[epcs solar] 0xE8 : %d", val0xE8);
	} else {
		/* エラーにしない処理続行 */
		DLOG_Error("solar 0xE8 pcs error!:%ld", ret);
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xD0(系統連系状態)のepcsデータの反映		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info			デバイス情報						*/
/*					:[in]　opeStts		ope 動作情報構造体					*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/07 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
static slong eenl_SolarUpdate0xD0(EenlDeviceInfo_t* info, EPCS_OPESTTS_INFO_T* opeStts)
{
	slong ret = 0;			/* 戻り値初期化 */
	uchar val0xD0 = 0xFFu;	/* 系統連系状態変数初期化 */

	DEBUG_TRCIN();

	DLOG_Debug("pwrCut : %d", opeStts->pwrCut);
	if (opeStts->pwrCut == EPCS_PWROUTAGE_YES)
	{
		/* 停電の場合 */
		val0xD0 = 0x01u;
	} else {
		/* 停電以外の場合 */
		val0xD0 = 0x00u;
	}

	if (val0xD0 != 0xFF)
	{
		/* 0xD0:系統連系状態（太陽光）*/
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devSolar, 0xD0u, val0xD0, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Debug("[epcs solar(ncc)] 0xD0 : %d -> 0x%02x", opeStts->pwrCut, val0xD0);
			/* 状変アナウンスはなし */
		} else {
			DLOG_Error("solar(ncc) 0xD0 pcs error!:%ld", ret);
		}
	} else {
		/* エラーにしない処理続行 */
		DLOG_Error("solar(ncc) 0xD0 not set!: %d", val0xD0);
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xA0(出力制御設定1)の設定電文チェック		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　edt			設定値								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
static slong eenl_SolarSetCheck0xA0(uchar* edt)
{
	slong ret = ENL_RESULT_ACCEPT;	/* 戻り値初期化 */
	slong ret2 = 0;					/* epcs設定関数戻り値初期化 */
	uchar dat = 0u;					/* 設定値変数初期化 */

	DEBUG_TRCIN();

	dat = *edt;
	DLOG_Debug("solar 0xA0 : 0x%02x(%d)", dat, dat);

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* 範囲チェックはしない */
	/* 規格上 0x00～0x64(0～100%) */

	/* epcsへ設定する */
	ret2 = epcs_sync_setRestrictedPvGenPwr(EPCS_RESTRICTED_TYPE_OUTPUT_CTRL, dat);
	if (ret2 != EPCS_OK)
	{
		ret = ENL_RESULT_NOT_ACCEPT;
		DLOG_Debug("solar 0xA0 epcs_sync_setRestrictedPvGenPwr error %ld", ret2);
	} else {
		/* 処理無し */
	}

	/* HEMS発電抑制履歴に記録(epcsへの設定成否を問わず記録) */
	EENL_ExtUpdateHemsHistory(dat);

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:太陽光クラス 0xC1(FIT契約タイプ)の設定電文チェック		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　edt			設定値								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
static slong eenl_SolarSetCheck0xC1(uchar* edt)
{
	slong ret = ENL_RESULT_ACCEPT;	/* 戻り値初期化 */
	slong ret2 = 0;					/* epcs設定関数戻り値初期化 */
	EPCS_PVINFO_SET_T pvInfoSet;	/* 太陽光発電情報設定構造体 */
	uchar dat = 0u;					/* 設定値変数初期化 */

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* 初期化 */
	memset(&pvInfoSet, 0x00, sizeof(pvInfoSet));

	dat = *edt;
	DLOG_Debug("solar 0xC1 : 0x%02x", dat);

	/* 範囲チェックはしない */
	/* 規格上 0x41～0x43 */

	pvInfoSet.changeFlg = EPCS_SET_PVINFO_FIT;
	pvInfoSet.fitType = dat;
	ret2 = epcs_sync_setPvInfoNotComm(&pvInfoSet);
	if (ret2 != EPCS_OK)
	{
		ret = ENL_RESULT_NOT_ACCEPT;
		DLOG_Debug("solar 0xC1 epcs_sync_setPvInfoNotComm error %ld", ret2);
	} else {
		/* 処理無し */
	}

	DEBUG_TRCOUT();
	return ret;
}


/*==========================================================================*/
/*	Description		:太陽光クラス 0xB0(出力制御スケジュール)の取得処理		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xB0	設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
static slong eenl_SolarGetOutputCtrlHistry(uchar* val0xB0)
{
	slong ret = EENL_OK;					/* 戻り値初期化 */
	slong func_ret = EENL_OK;				/* 関数戻り値初期化 */
	bool_t set_flg = FALSE;					/* スケジュール有無フラグ */
	EtimTm_t curDatetime, nextDatetime;					/* 時刻設定・取得データ */
	ESCH_DATE_TIME_T targetDate, nexttargetDate;		/* esch日付型情報 */
	ESCH_DAILY_SCHED_DATA_T dailySchData, nextSchData;	/* 出力制御日間スケジュール情報 */
	bool_t rtn_flg = FALSE;					/* 戻り値フラグ */

	DEBUG_TRCIN();
	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* 初期化 */
	memset(&curDatetime, 0x00, sizeof(curDatetime));
	memset(&nextDatetime, 0x00, sizeof(nextDatetime));
	memset(&targetDate, 0x00, sizeof(targetDate));
	memset(&nexttargetDate, 0x00, sizeof(nexttargetDate));
	memset(&dailySchData, 0x00, sizeof(dailySchData));
	memset(&nextSchData, 0x00, sizeof(nextSchData));
	memset(val0xB0, 0xFFu, EENL_PDC_SOL_0xB0);	/* 引数で受け取った領域を0xFF(データなし)で初期化 */

	/* 現在日時(当日)を取得 */
	func_ret = ETIM_GetTime(&curDatetime);
	if (func_ret != ETIM_OK)
	{
		/* 日付が取得できない場合は0xFFを返す */
		DLOG_Error("getTime error! : %ld", ret);
		ret = EENL_ERR;
		rtn_flg = TRUE;
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 現在日時から翌日を取得 */
		func_ret = EENL_GetNextDay(&curDatetime, &nextDatetime);
		if (func_ret != EENL_OK)
		{
			/* 日付が取得できない場合は0xFFを返す */
			DLOG_Error("get nextDay error! : %ld", ret);
			ret = EENL_ERR;
			rtn_flg = TRUE;
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	if (rtn_flg == FALSE)
	{
		/* 取得した日時をesch日付型にキャスト */
		/*（esch日付型のほうが小さいが日付なので問題なし） */
		/* 当日分をesch日付型にキャスト */
		targetDate.year = (ushort)curDatetime.tmYear;
		targetDate.month = (uchar)curDatetime.tmMon;
		targetDate.day = (uchar)curDatetime.tmMday;
		targetDate.hour = (uchar)curDatetime.tmHour;
		targetDate.minute = (uchar)curDatetime.tmMin;
		targetDate.second = (uchar)curDatetime.tmSec;

		/* 翌日分をesch日付型にキャスト */
		nexttargetDate.year = (ushort)nextDatetime.tmYear;
		nexttargetDate.month = (uchar)nextDatetime.tmMon;
		nexttargetDate.day = (uchar)nextDatetime.tmMday;
		nexttargetDate.hour = (uchar)nextDatetime.tmHour;
		nexttargetDate.minute = (uchar)nextDatetime.tmMin;
		nexttargetDate.second = (uchar)nextDatetime.tmSec;

		/* 出力制御履歴取得 */
		/* 当日分を取得 */
		func_ret = esch_sync_getOutputCtrlHistory(&targetDate, &dailySchData);
		if (func_ret == ESCH_OK)
		{
			/* 当日のスケジュール有無フラグを更新 */
			set_flg = TRUE;
		} else {
			/* 取得に失敗またはデータがない場合は0xFFを設定する */
			memset(&dailySchData, 0xFF, sizeof(dailySchData));
			DLOG_Debug("getOutputCtrlHistory error! : %ld[%04d/%02d/%02d]", ret, targetDate.year, targetDate.month, targetDate.day);
		}

		/* 翌日分を取得 */
		func_ret = esch_sync_getOutputCtrlHistory(&nexttargetDate, &nextSchData);
		if (func_ret == ESCH_OK)
		{
			/* 翌日のスケジュール有無フラグを更新 */
			set_flg = TRUE;
		} else {
			/* 取得に失敗またはデータがない場合は0xFFを設定する */
			memset(&nextSchData, 0xFF, sizeof(nextSchData));
			DLOG_Debug("getOutputCtrlHistory error! : %ld[%04d/%02d/%02d]", ret, nexttargetDate.year, nexttargetDate.month, nexttargetDate.day);
		}

		/* EPC用のデータ設定 */
		/* YYYYMMDD + 出力制御率[%] */
		if (set_flg == TRUE)
		{
			/* 現在または翌日のデータがある場合 */
			val0xB0[0] = (uchar)((targetDate.year & 0xFF00) >> 8);
			val0xB0[1] = (uchar)(targetDate.year & 0x00FF);
			val0xB0[2] = targetDate.month;
			val0xB0[3] = targetDate.day;
			memcpy(&val0xB0[4], &dailySchData.schedData, sizeof(dailySchData.schedData));
			memcpy(&val0xB0[52], &nextSchData.schedData, sizeof(nextSchData.schedData));
		} else {
			/* 処理無し */
		}
	} else {
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return ret;
}

