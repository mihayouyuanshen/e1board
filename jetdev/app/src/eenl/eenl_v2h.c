/****************************************************************************/
/*	Description		:ECHONET Lite application for device usage				*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC K.Jiang											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/20												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:eenl													*/
/*--------------------------------------------------------------------------*/
/*	Note			:E1														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(C) 2023. Nichicon Corporation. All Rights Reserved.	*/
/*					:(C) Ubiquitous Corp. 2001-2013. All rights reserved.	*/
/****************************************************************************/
#include "eenl_cmn_g.h"
#include "eenl_v2h.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
/* 状態変化通知された時のV2Hの運転モード保存用。
2017/12/13 ADAM修正計画No.231対処 */
ulong s_eenlErrst = 0;

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description		:車両充放電器デバイスの作成								*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*--------------------------------------------------------------------------*/
/*	return			:現在故障の発生状況有無									*/
/*					:(0:正常終了/-1:異常発生有)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:eenl_fileRead()を呼び出してから実行すること			*/
/*					:構築なので状変アナウンス通知は無し						*/
/*					:eenl_common.cで定義したプロパティリストの定義と		*/
/*					:初期化対象のepcの不一致が無いこと!!					*/
/*					:ニチコン向けM1/M2対応									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_CreateDeviceV2h(EenlDeviceInfo_t* info)
{
	slong ret = 0;
	ushort cnt = 0u;
	uchar data = 0x00u;
	uchar prod[16] = { 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u,
						0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u };
	uchar zero1b = 0x00u;
	uchar zero2b[2] = { 0x00u, 0x00u };
	uchar zero4b[4] = { 0x00u, 0x00u, 0x00u, 0x00u };
	uchar zero8b[8] = { 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u };
	uchar nowDate[4] = { 0x07u, 0xDEu, 0x01u, 0x01u };
	uchar space12[12] = { 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u,
						0x20u, 0x20u, 0x20u, 0x20u };
	slong workF5[4] = { 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u };
	uchar v2hId[17] = { 0xFEu, 0x00u, 0x00u, 0x00u, 0x40u, 0x20u, 0x20u, 0x20u, 0x20u,
						0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u };
	uchar merr[4] = { 0x00u, 0x00u, 0x00u, 0x00u };
	bool_t isErr = FALSE;

	/* 0xD7, 0xD9 は初期値をセットしない */
	/* 初期値有りの1バイトサイズのEPC定義 */
	uchar init1bEpc[4][2] = { { 0xC7u, 0xFFu },/* 0xFF 不定 */
						{ 0xCCu, 0x21u },		/* 0x21 DC_タイプAA（充放電可） */
						{ 0xDAu, 0x44u },		/* 0x44 待機 */
						{ 0xDBu, 0x01u } };	/* 0x01 独立 */

	/* ゼロ初期化する1バイトサイズのEPC定義 */
	/* 0xCDを初期化するとエラーになる */
	uchar zeroInit1bEpc[2] = { 0xC4u, 0xE4u };

	/* ゼロ初期化する2バイトサイズのEPC定義 */
	uchar zeroInit2bEpc[11] = { 0xC1u, 0xC3u, 0xD1u, 0xD2u, 0xD4u, 0xD5u,
								0xE3u, 0xE9u, 0xEDu, 0xEEu, 0xEFu };

	/* ゼロ初期化する4バイトサイズのEPC定義 */
	uchar zeroInit4bEpc[12] = { 0xC0u, 0xC2u, 0xC5u, 0xC6u, 0xCAu,
								0xCBu, 0xD0u, 0xD3u, 0xD6u, 0xD8u,
								0xE2u, 0xE7u };

	/* ゼロ初期化する8バイトサイズのEPC定義 */
	uchar zeroInit8bEpc[2] = { 0xC8u, 0xC9u };

	DEBUG_TRCIN();
	DLOG_Info("-----[create v2h] start-----");

	if (info == NULL)
	{
		/* パラメーターエラー */
		return -1;
	} else
	{
		/* 処理継続 */
	}

	/* 車両充放電器デバイスの作成 */
	/* 押し上げあり/押し上げなし 共通 */
	DLOG_Debug("create device storage v2h : ncc ext (K,N,S)");
	ret = ENL_create_device_electric_vehicle_charger(&info->devV2h,
		g_devicePropV2hNccN,
		g_devicePropV2hExtN,
		0x01u,
		ENL_INSTANCE_MODE_DEVICE);
	if (ret != ENL_RESULT_SUCCESS)
	{
		DLOG_Error("v2h device create error:%ld", ret);
		isErr = TRUE;
	} else
	{
		g_eenlDevV2h = info->devV2h;	/* インスタンスのアドレスを保持 */
		DLOG_Debug("v2h device created!!(instance:0x%08lX)",
			(ulong)g_eenlDevV2h);

		/* 0x80: 動作状態 */
		data = 0x31u;	/* off */
		DLOG_Debug("0x80 init: ncc");
		ret = ENL_DEV_set_property(NULL, info->devV2h, 0x80u, 1, &data);
		if (ret != ENL_RESULT_SUCCESS)
		{
			DLOG_Error("v2h 0x80 init error:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 0x81: 設置場所 */
			data = EENL_LOCATION_GARAGE; /* 車庫 */
			DLOG_Debug("0x81 init: ncc");
			ret = EENL_ExtEnlDevSetProperty(NULL, info->devV2h,
				0x81u, 1, &data);
			if (ret != ENL_RESULT_SUCCESS)
			{
				DLOG_Error("v2h 0x81 init error:%ld", ret);
				isErr = TRUE;
			} else
			{
				/* 前回の設置場所情報を反映 */
				/* 前回の設置場所情報は、*/
				/* eenl_fileRead()であらかじめグローバル変数に格納済み */
				DLOG_Debug("0x81 load value : 0x%02x", g_eenlV2h0x81);
				ret = EENL_ExtEnlDevSetProperty(NULL, info->devV2h,
					0x81u, 1u, &g_eenlV2h0x81);
				if (ret != ENL_RESULT_SUCCESS)
				{
					DLOG_Debug("v2h(ncc) 0x81 load init fail(%02x) : %ld",
						g_eenlV2h0x81, ret);
					/* 前回の情報が */
					/* 反映できなくても初期値で動作可能なので処理続行 */
				} else
				{
					/* 処理継続 */
					/* 状変アナウンスは無し */
				}

				/* 0x82: 規格バージョン AppendixI（ライブラリ自動生成） */

				/* 0x83: 識別番号 */
				DLOG_Debug("0x83 init: ncc");
				memcpy(&v2hId[1], &g_eenlCodeNcc[0], 3);

				/*車両充放電器の識別番号5バイト目は押し上げ状態 */
				v2hId[4] = g_eenlPushedUp;
				memcpy(&v2hId[5], &prod[0], 12);
				ret = EENL_ExtEnlDevSetProperty(NULL, info->devV2h,
					0x83u, 17u, (uchar*)v2hId);
				if (ret != ENL_RESULT_SUCCESS)
				{
					DLOG_Error("v2h 0x83 init error!:%ld", ret);
					isErr = TRUE;
				} else
				{
					/* 0x86: メーカ異常コード */
					DLOG_Debug("0x86 init: ncc");
					memcpy(&merr[1], &g_eenlCodeNcc[0], 3);

					ret = EENL_ExtEnlDevSetProperty(NULL, info->devV2h,
						0x86u, 4u, (uchar*)merr);
					if (ret != ENL_RESULT_SUCCESS)
					{
						DLOG_Error("v2h 0x86 init error!:%ld", ret);
						isErr = TRUE;
					} else
					{
						isErr = FALSE;
					}
				}
			}
		}
	}

	if (isErr == FALSE)
	{
		/* 0x88: 異常発生状態 */
		DLOG_Debug("0x88 init: ncc");
		data = 0x42u;	/* 異常発生無し */
		ret = ENL_DEV_set_property(NULL, info->devV2h, 0x88u, 1u, &data);
		if (ret != ENL_RESULT_SUCCESS)
		{
			DLOG_Error("v2h 0x88 init error!:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 状変アナウンスは無し */
			/* 0x8A: メーカコード */
			DLOG_Debug("0x8A init: ncc");
			ret = EENL_ExtEnlDevSetProperty(NULL, info->devV2h,
				0x8Au, 3u, (uchar*)g_eenlCodeNcc);
			if (ret != ENL_RESULT_SUCCESS)
			{
				DLOG_Error("v2h 0x8A init error!:%ld", ret);
				isErr = TRUE;
			} else
			{
				/* 0x8C: 商品コード */
				DLOG_Debug("0x8C init: ncc");
				ret = ENL_DEV_set_property(NULL, info->devV2h,
					0x8Cu, 12u, (uchar*)space12);
				if (ret != ENL_RESULT_SUCCESS)
				{
					DLOG_Error("v2h 0x8C init error!:%ld", ret);
					isErr = TRUE;
				} else
				{
					isErr = FALSE;
				}
			}
		}
	} else
	{
		/* 処理継続 */
	}

	if (isErr == FALSE)
	{
		/* 0x8Du: 製造番号 */
		DLOG_Debug("0x8Du init: ncc");
		ret = ENL_DEV_set_property(NULL, info->devV2h,
			0x8Du, 12u, (uchar*)space12);
		if (ret != ENL_RESULT_SUCCESS)
		{
			DLOG_Error("v2h 0x8Du init error!:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 0x93: 遠隔操作設定 */
			DLOG_Debug("0x93 init: ncc");
			data = EENL_REMOTE_OFF;	/* 公衆回線未経由 */
			ret = EENL_ExtEnlDevSetProperty(NULL, info->devV2h,
				0x93u, 1u, &data);
			if (ret != ENL_RESULT_SUCCESS)
			{
				DLOG_Error("v2h 0x93 init error!:%ld", ret);
				isErr = TRUE;
			} else
			{
				/* 0x97: 現在時刻設定 00:00 */
				DLOG_Debug("0x97 init: ncc");
				ret = ENL_DEV_set_property(NULL, info->devV2h,
					0x97u, 2u, (uchar*)zero2b);
				if (ret != ENL_RESULT_SUCCESS)
				{
					DLOG_Error("v2h 0x97 init error!:%ld", ret);
					isErr = TRUE;
				} else
				{
					/* 0x98: 現在年月日設定 2014/1/1 */
					DLOG_Debug("0x98 init: ncc");
					ret = ENL_DEV_set_property(NULL, info->devV2h,
						0x98u, 4u, (uchar*)nowDate);
					if (ret != ENL_RESULT_SUCCESS)
					{
						DLOG_Error("v2h 0x98 init error!:%ld", ret);
						isErr = TRUE;
					} else
					{
						isErr = FALSE;
					}
				}
			}
		}
	} else
	{
		/* 処理継続 */
	}

	if (isErr == FALSE)
	{
		/* 初期値有り(1バイト)のEPCをまとめて初期化 */
		DLOG_Debug("1byte init: data.%ld", sizeof(init1bEpc));
		for (cnt = 0u; cnt < (sizeof(init1bEpc) / 2)
			&& ret == ENL_RESULT_SUCCESS; cnt++)
		{
			DLOG_Debug("v2h 0x%02X init (1byte) <- 0x%02X",
				init1bEpc[cnt][0], init1bEpc[cnt][1]);

			ret = ENL_DEV_set_property(NULL, info->devV2h,
				init1bEpc[cnt][0], 1u, &init1bEpc[cnt][1]);
			if (ret != ENL_RESULT_SUCCESS)
			{
				DLOG_Error("v2h 0x%02X init (1byte) error!:%ld",
					init1bEpc[cnt][0], ret);
				isErr = TRUE;

			} else
			{
				/* 処理継続 */
			}
		}
	} else
	{
		/* 処理継続 */
	}

	if (isErr == FALSE)
	{
		/* 初期値0(1バイト)のEPCをまとめて初期化 */
		DLOG_Debug("1byte init: 0");
		for (cnt = 0u; cnt < sizeof(zeroInit1bEpc)
			&& ret == ENL_RESULT_SUCCESS; cnt++)
		{
			DLOG_Debug("v2h 0x%02X init zero (1bytes)",
				zeroInit1bEpc[cnt]);

			ret = ENL_DEV_set_property(NULL, info->devV2h,
				zeroInit1bEpc[cnt], 1u, &zero1b);
			if (ret != ENL_RESULT_SUCCESS)
			{
				DLOG_Error("v2h 0x%02X init zero (1bytes) error!:%ld",
					zeroInit1bEpc[cnt], ret);
				isErr = TRUE;
			} else
			{
				/* 処理継続 */
			}
		}
	} else
	{
		/* 処理継続 */
	}

	if (isErr == FALSE)
	{
		/* 初期値0(2バイト)のEPCをまとめて初期化 */
		DLOG_Debug("2byte init: 0");
		for (cnt = 0u; cnt < sizeof(zeroInit2bEpc)
			&& ret == ENL_RESULT_SUCCESS; cnt++)
		{
			DLOG_Debug("v2h 0x%02X init zero (2bytes)",
				zeroInit2bEpc[cnt]);

			ret = ENL_DEV_set_property(NULL, info->devV2h,
				zeroInit2bEpc[cnt], 2u, (uchar*)zero2b);
			if (ret != ENL_RESULT_SUCCESS)
			{
				DLOG_Error("v2h 0x%02X init zero (2bytes) error!:%ld",
					zeroInit2bEpc[cnt], ret);
				isErr = TRUE;
			} else
			{
				/* 処理継続 */
			}
		}
	} else
	{
		/* 処理継続 */
	}

	if (isErr == FALSE)
	{
		/* 初期値0(4バイト)のEPCをまとめて初期化 */
		DLOG_Debug("4byte init: 0");
		for (cnt = 0u; cnt < sizeof(zeroInit4bEpc)
			&& ret == ENL_RESULT_SUCCESS; cnt++)
		{
			DLOG_Debug("v2h 0x%02X init zero (4bytes)",
				zeroInit4bEpc[cnt]);

			ret = ENL_DEV_set_property(NULL, info->devV2h,
				zeroInit4bEpc[cnt], 4u, zero4b);
			if (ret != ENL_RESULT_SUCCESS)
			{
				DLOG_Error("v2h 0x%02X init zero (4bytes) error!:%ld",
					zeroInit4bEpc[cnt], ret);
				isErr = TRUE;
			} else
			{
				/* 処理継続 */
			}
		}
	} else
	{
		/* 処理継続 */
	}

	if (isErr == FALSE)
	{
		ret = EENL_ExtEnlDevSetProperty(NULL, info->devV2h,
			0xEBu, 4u, zero4b);
		if (ret != ENL_RESULT_SUCCESS)
		{
			DLOG_Error("v2h 0x%02X init zero (4bytes) error!:%ld", 0xEB, ret);
			isErr = TRUE;
		} else
		{
			ret = EENL_ExtEnlDevSetProperty(NULL, info->devV2h,
				0xECu, 4u, zero4b);
			if (ret != ENL_RESULT_SUCCESS)
			{
				DLOG_Error("v2h 0x%02X init zero (4bytes) error!:%ld",
					0xECu, ret);
				isErr = TRUE;
			} else
			{
				/* 初期値0(8バイト)のEPCをまとめて初期化 */
				DLOG_Debug("8byte init: 0");
				for (cnt = 0u; cnt < sizeof(zeroInit8bEpc)
					&& ret == ENL_RESULT_SUCCESS; cnt++)
				{
					DLOG_Debug("v2h 0x%02X init zero (8bytes)",
						zeroInit8bEpc[cnt]);

					ret = ENL_DEV_set_property(NULL, info->devV2h,
						zeroInit8bEpc[cnt], 8u, zero8b);
					if (ret != ENL_RESULT_SUCCESS)
					{
						DLOG_Error("v2h 0x%02X init zero (8bytes) error!:%ld",
							zeroInit8bEpc[cnt], ret);
						isErr = TRUE;
					} else
					{
						/* 処理継続 */
					}
				}
			}
		}
	} else
	{
		/* 処理継続 */
	}

	if (isErr == FALSE)
	{
		/********************************************/
		/*			ユーザ定義の設定開始			*/
		/********************************************/

		DLOG_Debug("0xF5 init: ncc");
		/* 0xF5:瞬時電力計測値 */
		workF5[0] = 0;				/* 売買電電力値 */
		workF5[1] = 0;				/* 家庭消費電力値 */
		workF5[2] = 0;				/* 外付太陽光発電電力値 */
		workF5[3] = 0x7FFFFFFF;		/* 予約（0x7FFFFFFF） */
		ret = ENL_DEV_set_property(NULL, info->devV2h,
			0xF5u, 16u, (uchar*)workF5);
		if (ret != ENL_RESULT_SUCCESS)
		{
			DLOG_Error("battery 0xF5 init error!:%ld", ret);
			ret = -1;
		} else
		{
			/* 使用後は元に戻す */
			memset(workF5, 0x00, sizeof(workF5));
			ret = 0;
		}
	} else
	{
		ret = -1;
	}

	DLOG_Debug("-----[create v2h] end-----");
	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:epcsデータの反映（車両充電器）							*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常/-2:epcs通信でエラー発生)			*/
/*--------------------------------------------------------------------------*/
/*	detail			:eenl_fileRead()を呼び出してから実行すること			*/
/*					:構築なので状変アナウンス通知は無し						*/
/*					:eenl_common.cで定義したプロパティリストの定義と		*/
/*					:初期化対象のepcの不一致が無いこと!!					*/
/*					:ニチコン向けM1/M2対応									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_UpdateEpcsV2h(EenlDeviceInfo_t* info)
{
	slong ret = 0;
	EPCS_V2H_SETINFO_GET_T v2hSet;
	void* devType;

	DEBUG_TRCIN();

	DLOG_Debug("-----[epcs v2h] update start-----");

	if (info == NULL)
	{
		DLOG_Error("getV2h Info error! : NULL");
		return -1;
	} else if (info->devV2h == NULL)
	{
		/* V2Hクラスなし */
		DLOG_Debug("-----[v2h] update pass-----");
		return 0;
	} else
	{
		/* 処理継続 */
	}

	devType = info->devV2h;

	memset(&v2hSet, 0x00, sizeof(EPCS_V2H_SETINFO_GET_T));

	/* epcsからV2H設定情報(非通信の現在状態)を取得 */
	ret = epcs_sync_getV2hSetInfoCurrent_NotComm(&v2hSet);
	if (ret != EPCS_OK)
	{
		DLOG_Error("getV2hSetInfo error! : %ld", ret);
	} else
	{
		/* 0x80: 動作状態→状態変化通知にて設定するため、ここでは設定しない。*/

		/* 0x83: 識別番号（車両充放電器） */
		ret = EENL_DevUpdate0x83(v2hSet.v2hSerialNumber, devType);

		/* 0x8C: 商品コード（車両充放電器） */
		ret = EENL_DevUpdate0x8C(v2hSet.v2hModelName, devType);

		/* 0x8Du: 製造番号（車両充放電器） */
		ret = EENL_DevUpdate0x8D(v2hSet.v2hSerialNumber, devType);

		/* 0xC0: 車載電池の放電可能容量値 1→周期処理にて設定するため、*/
		/* updateでは設定しない。*/

		/* 0xC5: 定格充電能力 */
		ret = eenl_V2hUpdate0xC5(info, &v2hSet);

		/* 0xC6: 定格放電能力 */
		ret = eenl_V2hUpdate0xC6(info, &v2hSet);

		/* 0xC8: 最小最大充電電力値 */
		ret = eenl_V2hUpdate0xC8(info, &v2hSet);

		/* 0xC9: 最小最大放電電力値 */
		ret = eenl_V2hUpdate0xC9(info, &v2hSet);

		/* 0xCA: 最小最大充電電流値 */
		ret = eenl_V2hUpdate0xCA(info);

		/* 0xCB: 最小最大放電電流値 */
		ret = eenl_V2hUpdate0xCB(info);

		/* 0xCC: 充放電器タイプ */
		ret = eenl_V2hUpdate0xCC(info, &v2hSet);

		/* 0xCE: 車載電池の充電可能容量値→周期処理にて設定するため、*/
		/* 0xD0: 車載電池の使用容量値1→周期処理にて設定するため、*/
		/* updateでは設定しない。 */

		/* 0xD2: 定格電圧 */
		ret = eenl_V2hUpdate0xD2(info);

		/* 0xD3: 瞬時充放電電力計測値→周期処理にて設定するため、*/
		/* 0xD4: 瞬時充放電電流計測値→周期処理にて設定するため、*/
		/* 0xD5: 瞬時充放電電圧計測値→周期処理にて設定するため、*/
		/* 0xD6: 積算放電電力量計測値→周期処理にて設定するため、*/
		/* 0xD8: 積算充電電力量計測値→周期処理にて設定するため、*/
		/* updateでは設定しない。*/
		/* 0xDB: 系統連系状態 →状態変化通知にて設定するため、*/
		/* ここでは設定しない。 */

		/* 0xE6: 車両ID */
		ret = eenl_V2hUpdate0xE6(info, &v2hSet);

		/* 0xEA: 放電量設定値 */
		ret = eenl_V2hUpdate0xEA(info, &v2hSet);
	}

	DEBUG_TRCOUT();
	return 0;
}

/*==========================================================================*/
/*	Description		:設定電文のチェック（車両充放電器）						*/
/*--------------------------------------------------------------------------*/
/*	param			:uchar	epc			設定対象EPC No.						*/
/*					:uchar	pdc			設定対象PDC							*/
/*					:uchar	pdcSize		希望するPDCサイズ					*/
/*					:ulong	dat			設定値								*/
/*					:ulong	datMax		設定値データ最大値					*/
/*					:EPCS_V2H_SETINFO_SET_T									*/
/*					:	v2hSetInfoSet	V2H設定情報設定用構造体				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong		処理結果									*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:ニチコン向けM1対応										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong eenl_V2hSetInfoSetCheck(uchar epc, uchar pdc, uchar pdcSize, ulong dat,
	ulong datMax, EPCS_V2H_SETINFO_SET_T v2hSetInfoSet)
{
	slong ret = ENL_RESULT_ACCEPT;

	DEBUG_TRCIN();

	/* サイズチェック */
	if (pdc != pdcSize)
	{
		/* サイズが違う場合 */
		DLOG_Error("pdc error!(%d)", pdc);
		return ENL_RESULT_NOT_ACCEPT;
	} else
	{
		/* 処理継続 */
	}

	/* 最大値オーバーチェック */
	if (dat <= datMax)
	{
		/* 範囲内　何もしない */
		/* ulongなので0以上は確定 */
	} else
	{
		/* 範囲外 */
		DLOG_Error("dat error! %ld.(max value= %ld)", dat, datMax);
		return ENL_RESULT_NOT_ACCEPT;
	}

	/* 渡されたV2H設定情報設定用構造体をEPCSにSet。 */
	/* V2H設定情報設定 */
	ret = epcs_sync_setV2hSetInfo(&v2hSetInfoSet);
	if (ret == EPCS_OK)
	{
		/* 設定OK */
		DLOG_Debug("v2h 0x%2X epcs_sync_setV2hSetInfo ok!)", epc);
		ret = ENL_RESULT_ACCEPT;
	} else
	{
		/* 設定失敗 */
		DLOG_Error("v2h 0x%2X epcs_sync_setV2hSetInfo NG! :%ld ", epc, ret);
		ret = ENL_RESULT_NOT_ACCEPT;
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:設定電文のチェック（車両充放電器）						*/
/*--------------------------------------------------------------------------*/
/*	param			:void	*instance		インスタンスハンドル			*/
/*					:uchar	epc		設定対象EPC								*/
/*					:uchar	*pdc	設定値のデータサイズ					*/
/*					:uchar	*edt	設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:slong		処理結果									*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:ニチコン向けM1対応										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hSetCheck(void* instance, uchar epc, uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_NOT_ACCEPT;

	DEBUG_TRCIN();
	DLOG_Debug("-----v2h set check-----");
	DLOG_Debug("epc:%02x  pdc:0x%02x(%d)  edt:0x%02x(%d)",
		epc, *pdc, *pdc, *edt, *edt);

	switch (epc)
	{
	case 0x81:		/* 設置場所 */
		ret = EENL_DevSetCheck0x81(instance, pdc, edt);
		break;
	case 0x97:		/* 現在時刻設定 */
		ret = EENL_DevSetCheck0x97(pdc, edt);
		break;
	case 0x98:		/* 現在年月日設定 */
		ret = EENL_DevSetCheck0x98(pdc, edt);
		break;
	case 0xCD:		/* 車両接続確認 */
		ret = eenl_V2hSetCheck0xCD(pdc, edt);
		break;
	case 0xD7:		/* 積算放電電力量リセット設定 */
		ret = eenl_V2hSetCheck0xD7(pdc, edt);
		break;
	case 0xD9:		/* 積算充電電力量リセット設定 */
		ret = eenl_V2hSetCheck0xD9(pdc, edt);
		break;
	case 0xDA:		/* 運転モード設定 */
		ret = eenl_V2hSetCheck0xDA(pdc, edt);
		break;
	case 0xE7:		/* 充電量設定値1 */
		ret = eenl_V2hSetCheck0xE7(epc, pdc, edt);
		break;
	case 0xEA:		/* 放電量設定値 */
		ret = eenl_V2hSetCheck0xEA(epc, pdc, edt);
		break;
	case 0xEB:		/* 充電電力設定値 */
		ret = eenl_V2hSetCheck0xEB(epc, pdc, edt);
		break;
	case 0xEC:		/* 放電電力設定値 */
		ret = eenl_V2hSetCheck0xEC(epc, pdc, edt);
		break;
	default:
		/* その他 */
		DLOG_Debug("%02x not support", epc);
		ret = ENL_RESULT_NOT_ACCEPT;
		break;
	}

	DLOG_Debug("=> check : %ld", ret);
	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラスの系統連系(0xDB)更新処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:処理なし/0以外:ENLライブラリの戻り値)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:ニチコン向けM1/M2対応									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_UpdateV2h0xDB(void)
{
	uchar valV2h = 0xFFu;
	slong ret = EENL_OK;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);
	if (info == NULL)
	{
		DLOG_Error("info error! : null");
		ret = EENL_ERR;
	} else
	{
		if (g_eenlPwrCut == EPCS_PWROUTAGE_YES)
		{
			/* 停電の場合 */
			valV2h = 0x01u;		/*独立 */
		} else
		{
			/* 停電以外の場合 */
			valV2h = 0x02u;		/* 系統連系（逆潮流不可） */
		}

		if (valV2h != 0xFFu)
		{
			/* 設定値が決定している場合は、ENLに反映 */
			if (info->devV2h != NULL)
			{
				/* 車両充放電器0xDB */
				ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h,
					0xDBu, valV2h, 0u);
				if (ret == ENL_RESULT_SUCCESS)
				{
					/*成功 */
					DLOG_Debug("v2h 0xDB set success : 0x%02x", valV2h);
					/*状変アナウンスは無し */
				} else
				{
					/*失敗 */
					DLOG_Debug("v2h 0xDB set fail : 0x%02x(%ld)",
						valV2h, ret);
				}
			} else
			{
				/* 処理継続 */
			}
		} else
		{
			/* 処理継続 */
		}
	}

	DEBUG_TRCOUT()
		return ret;
}

/*==========================================================================*/
/*	Description		:V2H設置場所情報の取得処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlFncU_t* fmsg										*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常/-2:引数不正)						*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_GetV2hLocation(EenlGetLocation_t* fmsg)
{
	slong r = 0;
	EenlGetLocationRet_t ret;
	EenlDeviceInfo_t* info = NULL;
	ulong loc = EENL_LOCATION_GARAGE;

	DEBUG_TRCIN();

	DLOG_Debug(" <<<eenl_get_V2h_location>>> start ");

	if (fmsg == NULL)
	{
		return -2;	/* error */
	} else
	{
		/* 処理継続 */
	}

	if (g_eenlCreate == EENL_CREATE_OK)
	{
		/* enl構築済の場合は、enlから情報を取得 */
		info = EENL_GetDeviceInfoIndex(0);
		if (info != NULL)
		{
			if (info->devV2h != NULL)
			{
				/* ENLライブラリから現在の情報を取得 */
				r = ENL_get_property_value(info->devV2h, 0x81, &loc, 0);
				if (r != ENL_RESULT_SUCCESS)
				{
					/* 失敗 */
					loc = EENL_LOCATION_GARAGE;
					DLOG_Debug(" get 0x81 value : %ld, 0x%02lx ", r, loc);
				} else
				{
					/* 成功 */
					/* 取得した値をそのまま利用するので、何もしない */
				}
			} else
			{
				/* V2Hが未生成の場合、初期状態(車庫)を返す */
				loc = EENL_LOCATION_GARAGE;
				DLOG_Debug(" v2h non. 0x81 value : 0x%02lx ", loc);
			}
		} else
		{
			/* V2hが見つからない場合、初期状態(車庫)を返す */
			loc = EENL_LOCATION_GARAGE;
			DLOG_Debug(" get 0x81 null : 0x%02lx ", loc);
		}
	} else
	{
		/* enl未構築の場合は、ファイルから直接取得 */
		EENL_FileInit();
		EENL_FileRead();

		/* グローバル変数に保存された設置場所情報を利用 */
		loc = g_eenlV2h0x81;
		DLOG_Debug(" get 0x81 file : 0x%02lx ", loc);
	}

	/* 設置場所の下位3ビットをマスクする */
	loc = loc & 0xF8u;

	/* 取得値の判定 */
	if (loc != EENL_LOCATION_GARAGE)
	{
		/* 車庫以外の場合 */
		/* GUI関連からの要求なら「その他」を返す */
		if ((fmsg->header.msgId == AGUI_SPORT) ||
			(fmsg->header.msgId == AGUI_CPORT) ||
			(fmsg->header.msgId == EGUI_SPORT) ||
			(fmsg->header.msgId == EGUI_CPORT))
		{
			DLOG_Debug(" change data : 0x%02lx -> 0x%02x ", loc, EENL_LOCATION_OTHERS);
			loc = EENL_LOCATION_OTHERS;
		}
		/* 見守りサーバー関連からの要求なら「屋外」を返す */
		else if ((fmsg->header.msgId == ENSV_SPORT) ||
			(fmsg->header.msgId == ENSV_CPORT) ||
			(fmsg->header.msgId == ESSV_SPORT) ||
			(fmsg->header.msgId == ESSV_CPORT) ||
			(fmsg->header.msgId == ENSV_SUB_SPORT) ||
			(fmsg->header.msgId == ENSV_SUB_CPORT) ||
			(fmsg->header.msgId == ENSC_SPORT) ||
			(fmsg->header.msgId == ENSC_CPORT))
		{
			DLOG_Debug(" change data : 0x%02lx -> 0x%02x ",
				loc, EENL_LOCATION_OUTSIDE);
			loc = EENL_LOCATION_OUTSIDE;
		} else
		{
			/* 処理継続 */
		}
	} else
	{
		/* 車庫の場合 */
		/* そのまま利用するので、何もしない */
	}

	/* 同期通信の応答を生成 */
	ret.header.retCode = 0;
	ret.location = loc;
	DLOG_Debug(" ret=0x%02x", ret.location);

	/* 同期通信用リターンを返す */
	FRM_CallRet(fmsg->header.srcModId, sizeof(EenlGetLocationRet_t), (void*)&ret);

	DLOG_Debug(" <<<eenl_get_location>>> end ");
	DEBUG_TRCOUT();
	return 0;
}

/*==========================================================================*/
/*	Description		:V2H設置場所情報の設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:fmsg メッセージ										*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常/-2:引数不正)						*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_SetV2hLocation(EenlSetLocation_t* fmsg)
{
	slong r = 0;
	EenlDeviceInfo_t* info = NULL;
	ENL_MESSAGE* msg;

	DEBUG_TRCIN();
	DLOG_Debug(" <<<eenl_set_V2h_location>>> start ");

	if (fmsg == NULL)
	{
		/* 引数不正の場合、処理終了 */
		return -2;
	} else
	{
		/* 処理継続 */
	}

	DLOG_Debug(" location=0x%02x", fmsg->location);
	/* 設定値の判定 */
	switch (fmsg->location) {
	case EENL_LOCATION_GARAGE:		/* 車庫 */
		/* breakなし */
	case EENL_LOCATION_OTHERS:		/* その他 */
		/* 設定可能 */

		if (g_eenlCreate == EENL_CREATE_OK)
		{
			/* enl構築済みの場合 */
			DLOG_Debug(" enl_create : ok ");

			info = EENL_GetDeviceInfoIndex(0);
			if (info != NULL)
			{
				/* 車両充電器 */
				if (info->devV2h != NULL)
				{
					r = EENL_ExtEnlDevSetPropertyValue(&msg, info->devV2h,
						0x81u, fmsg->location, 0u);
					if (r == ENL_RESULT_SUCCESS)
					{
						/* 成功 */
						DLOG_Debug(" v2h location set success : 0x%02x",
							fmsg->location);
						/* 状変アナウンスを通知 */
						if (msg != NULL)
						{
							EENL_SendMessage(NULL, msg);
							DLOG_Debug("#### send broadcast message (v2h:0x81) ####");

							/* グローバル変数に変更情報を格納 */
							g_eenlV2h0x81 = fmsg->location;
							/* 情報を保存 */
							EENL_FileWrite();

							DLOG_Debug(" v2h location change(N) call : 0x%02x",
								fmsg->location);
							if (fmsg->location == EENL_LOCATION_GARAGE)
							{
								/* 車庫 EENL_LOCATION_GARAGE */
								ensv_async_notify_V2hLocationChanged(ENSV_LOCATION_INSIDE);
							} else
							{
								/* その他 EENL_LOCATION_OTHERS */
								ensv_async_notify_V2hLocationChanged(ENSV_LOCATION_OUTSIDE);
							}

						} else
						{
							/* 失敗 */
							DLOG_Debug(" message not send");
							/* 処理は続行 */
						}
					} else
					{
						/* 失敗 */
						DLOG_Error(" v2h location set error : 0x%02x",
							fmsg->location);
						/* 処理は続行 */
					}
				} /* v2hがあるなら */
				else
				{
					DLOG_Debug(" v2h not found");
					/* 処理は続行 */
				}
			} else
			{
				DLOG_Error(" info = NULL");
				/* 処理は続行 */
			}
		} else
		{
			/* enl未構築の場合、ファイルに直接保存 */
			if (g_eenlV2h0x81 != fmsg->location)
			{
				DLOG_Debug(" enl_create : file ");
				EENL_FileInit();
				/* グローバル変数に設置場所を保存 */
				g_eenlV2h0x81 = fmsg->location;
				EENL_FileWrite();

				/* 見守りサーバへの通知を行う */
				DLOG_Debug(" v2h location change(N) call : 0x%02x",
					fmsg->location);
				if (fmsg->location == EENL_LOCATION_GARAGE)
				{
					/* 車庫 EENL_LOCATION_GARAGE */
					ensv_async_notify_V2hLocationChanged(ENSV_LOCATION_INSIDE);
				} else
				{
					/* その他 EENL_LOCATION_OTHERS */
					ensv_async_notify_V2hLocationChanged(ENSV_LOCATION_OUTSIDE);
				}
			} else
			{
				/* 処理継続 */
			}
		}
		break;
	default:
		/* 設定不可 */
		DLOG_Error(" location parameter error %02x", fmsg->location);
		/* 処理は続行 */
		break;
	}	/* fmsg->location */

	/* 非同期で呼ばれるので、応答は返さない */


	DLOG_Debug(" <<<eenl_set_V2h_location>>> end ");
	DEBUG_TRCOUT();
	return 0;
}


/*==========================================================================*/
/*	Description		:V2Hクラス 0xC7の設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:uchar val 0xC7設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hNotify0xC7(uchar val0xC7)
{
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;
	ENL_MESSAGE* msg = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else
	{
		/* 0xC7 */
		ret = EENL_ExtEnlDevSetProperty(&msg, info->devV2h,
			0xC7u, 1u, &val0xC7);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/* 成功 */
			DLOG_Debug("V2H 0xC7 set success : 0x%02x(%d)",
				val0xC7, val0xC7);
			if (msg != NULL)
			{
				/* 状変アナウンス通知 */
				EENL_SendMessage(NULL, msg);
				DLOG_Debug("#### send broadcast message (v2h:0xC7) ####");
			} else
			{
				/* 処理続行 */
			}
		} else
		{
			/* 失敗 */
			DLOG_Error("V2H 0xC7 set fail : 0x%02x(%d) (%ld)",
				val0xC7, val0xC7, ret);
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xE7の設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong val0xE7 0xE7設定値								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hNotify0xE7(ulong val0xE7)
{
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else
	{
		/* 0xE7 */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h,
			0xE7u, val0xE7, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/*成功 */
			DLOG_Debug("V2H 0xE7 set success : 0x%02lx(%ld)",
				val0xE7, val0xE7);
			/* 変アナウンスは無し */
		} else
		{
			/* 失敗 */
			DLOG_Error("V2H 0xE7 set fail: 0x%02lx(%ld) (%ld)",
				val0xE7, val0xE7, ret);
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xEAの設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong val0xEA 0xEA設定値								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hNotify0xEA(ulong val0xEA)
{
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else
	{
		/* 0xEA */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h,
			0xEAu, val0xEA, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/* 成功 */
			DLOG_Debug("V2H 0xEA set success : 0x%08lx(%ld)",
				val0xEA, val0xEA);
			/* 状変アナウンスは無し */
		} else
		{
			/* 失敗 */
			DLOG_Error("V2H 0xEA set fail: 0x%08lx(%ld) (%ld)",
				val0xEA, val0xEA, ret);
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xEBの設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong val0xEB 0xEB設定値								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hNotify0xEB(ulong val0xEB)
{
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else
	{
		/* 0xEB */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h,
			0xEBu, val0xEB, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/* 成功 */
			DLOG_Debug("V2H 0xEB set success : 0x%02lx(%ld)",
				val0xEB, val0xEB);
			/* 変アナウンスは無し */
		} else
		{
			/* 失敗 */
			DLOG_Error("V2H 0xEB set fail: 0x%02lx(%ld) (%ld)",
				val0xEB, val0xEB, ret);
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xECの設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong val0xEC 0xEC設定値								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hNotify0xEC(ulong val0xEC)
{
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else
	{
		/* 0xEC */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h,
			0xECu, val0xEC, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/* 成功 */
			DLOG_Debug("V2H 0xEC set success: 0x%02lx(%ld)",
				val0xEC, val0xEC);
			/* 状変アナウンスは無し */
		} else
		{
			/* 失敗 */
			DLOG_Error("V2H 0xEC set fail: 0x%02lx(%ld) (%ld)",
				val0xEC, val0xEC, ret);
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xD2の設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong val0xD2 0xD2設定値								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hNotify0xD2(ushort val0xD2)
{
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else
	{
		/* 0xD2 */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h,
			0xD2u, val0xD2, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/* 成功 */
			DLOG_Debug("V2H 0xD2 set success: 0x%02x(%d)",
				val0xD2, val0xD2);
			/* 状変アナウンスは無し */
		} else
		{
			/* 失敗 */
			DLOG_Error("V2H 0xD2 set fail: 0x%02x(%d) (%ld)",
				val0xD2, val0xD2, ret);
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xDAの設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong val0xDA 0xDA設定値								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hNotify0xDA(uchar val0xDA)
{
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;
	ENL_MESSAGE* msg = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else
	{
		/* 0xDA */
		ret = EENL_ExtEnlDevSetPropertyValue(&msg, info->devV2h,
			0xDAu, val0xDA, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/* 成功 */
			DLOG_Debug("V2H 0xDA set success: 0x%02x(%d)",
				val0xDA, val0xDA);
			if (msg != NULL)
			{
				/* 状変アナウンス通知 */
				EENL_SendMessage(NULL, msg);
				DLOG_Debug("#### send broadcast message (v2h:0xDA) ####");
			}
		} else
		{
			/* 失敗 */
			DLOG_Error("V2H 0xDA set fail: 0x%02x(%d) (%ld)",
				val0xDA, val0xDA, ret);
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xC2の設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hTimer0xC2(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong val0xC2 = 0u;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		DLOG_Debug(" v2hDischargeableRemainingCapacity1 : %ld",
			ope->v2hDischargeableRemainingCapacity1);
		/* 車載電池の放電可能残容量１ [Wh]  範囲：0～999,999,999 [Wh] */
		val0xC2 = ope->v2hDischargeableRemainingCapacity1;

		/* 範囲チェック。0x000000000 - 0x3B9AC9FF(0 - 999,999,999) */
		if (val0xC2 > 0x3B9AC9FFu)
		{
			val0xC2 = 0x3B9AC9FFu;
		} else
		{
			/* 処理続行 */
		}

		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h,
			0xC2u, val0xC2, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Debug(" [timer v2h] 0xC2 : %ld", val0xC2);
			/* 状変アナウンスはなし */
		} else
		{
			DLOG_Error(" [timer v2h] 0xC2 pcs error!:%ld", ret);
			/* 異常でも処理続行 */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xC4の設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hTimer0xC4(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	uchar val0xC4 = 0u;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		DLOG_Debug(" v2hDischargeableRemainingCapacity3 : %d",
			ope->v2hDischargeableRemainingCapacity3);
		/* 車載電池の放電可能残容量３ [％]  範囲：0～100[％] */
		val0xC4 = ope->v2hDischargeableRemainingCapacity3;

		/* 範囲チェック。0x00 - 0x64(0～100) */
		if (val0xC4 > 0x64u)
		{
			val0xC4 = 0x64u;
		} else
		{
			/* 処理続行 */
		}
		ret = EENL_ExtEnlDevSetProperty(NULL, info->devV2h,
			0xC4u, 1u, &val0xC4);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Debug(" [timer v2h] 0xC4 : %d", val0xC4);
			/* 状変アナウンスはなし */
		} else
		{
			DLOG_Error(" [timer v2h] 0xC4 pcs error!:%ld", ret);
			/* 異常でも処理続行 */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xCFの設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hTimer0xCF(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong val0xCF = 0u;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		DLOG_Debug(" v2hChargeableRemainCap : %ld",
			ope->v2hChargeableRemainCap);
		/* 車載電池の充電可能残容量値[Wh]  範囲：0～999,999,999[Wh] */
		val0xCF = ope->v2hChargeableRemainCap;

		/* 範囲チェックはしない(epcsと同じ範囲 0-999,999,999) */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h,
			0xCFu, val0xCF, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Debug(" [timer v2h] 0xCF : %ld", val0xCF);
			/* 状変アナウンスはなし */
		} else
		{
			DLOG_Error(" [timer v2h] 0xCF pcs error!:%ld", ret);
			/* 異常でも処理続行 */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xD3の設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hTimer0xD3(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	slong val0xD3 = 0;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		DLOG_Debug(" v2hInstantPower : %ld", ope->v2hInstantPower);
		val0xD3 = ope->v2hInstantPower;

		/* slongのフル値が範囲なのでチェックしない。 */
		/* 充電時:0x000000001～0x3B9AC9FF(1～999,999,999) */
		/* 放電時:0xFFFFFFFF～0xC4653601(-1～-999,999,999) */
		/* 超える場合:0x7FFFFFFF (オーバーフローコード) */
		/* 未満の場合:0x80000000 (アンダーフローコード) */
		/* 充放電していない時:0 */
		DLOG_Debug(" [timer v2h] 0xD3 limit: none (%ld)", val0xD3);
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h, 
			0xD3u, (ulong)val0xD3, 0u);

		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Debug(" [timer v2h] 0xD3 : %ld", val0xD3);
			/* 状変アナウンスはなし */
		} else
		{
			DLOG_Error(" [timer v2h] 0xD3 pcs error!:%ld", ret);
			/* 異常でも処理続行 */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xD6の設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hTimer0xD6(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong val0xD6 = 0u;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		DLOG_Debug(" v2hTtlDischargePower : %ld",
			ope->v2hTtlDischargePower);

		/* 範囲 0x000000000 - 0x3B9AC9FF(0 - 999,999,999) */
		/* epcsでECHONET規格の最大値超過がありえる */
		/* オーバーフロー時に0から再インクリメントとなるように補正する必要があるため、 */
		/* 常に規格上の最大値+1で剰余算する */
		val0xD6 = ope->v2hTtlDischargePower % EENL_LIMIT_1000000000;

		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h,
			0xD6u, val0xD6, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Debug(" [timer v2h] 0xD6 : %ld", val0xD6);
			/* 状変アナウンスはなし */
		} else
		{
			DLOG_Error(" [timer v2h] 0xD6 pcs error!:%ld", ret);
			/* 異常でも処理続行 */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xD8の設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hTimer0xD8(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong val0xD8 = 0u;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		DLOG_Debug(" v2hTtlChargePower : %ld", ope->v2hTtlChargePower);

		/* 範囲 0x000000000 - 0x3B9AC9FF(0 - 999,999,999) */
		/* epcsでECHONET規格の最大値超過がありえる */
		/* オーバーフロー時に0から再インクリメントとなるように補正する必要があるため、*/
		/* 常に規格上の最大値+1で剰余算する */
		val0xD8 = ope->v2hTtlChargePower % EENL_LIMIT_1000000000;

		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h,
			0xD8u, val0xD8, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Debug(" [timer v2h] 0xD8 : %ld", val0xD8);
			/* 状変アナウンスはなし */
		} else
		{
			DLOG_Error(" [timer v2h] 0xD8 pcs error!:%ld", ret);
			/* 異常でも処理続行 */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xE2の設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hTimer0xE2(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong val0xE2 = 0u;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		DLOG_Debug(" v2hRemainingCapacity1 : %ld",
			ope->v2hRemainingCapacity1);
		val0xE2 = ope->v2hRemainingCapacity1;

		/* 範囲チェック。0x000000000 - 0x3B9AC9FF(0 - 999,999,999) */
		if (val0xE2 > 0x3B9AC9FFu)
		{
			val0xE2 = 0x3B9AC9FFu;
		} else
		{
			/* 処理続行 */
		}

		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h,
			0xE2u, val0xE2, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Debug(" [timer v2h] 0xE2 : %ld", val0xE2);
			/*状変アナウンスはなし */
		} else
		{
			DLOG_Error(" [timer v2h] 0xE2 pcs error!:%ld", ret);
			/* 異常でも処理続行 */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xE4の設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hTimer0xE4(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	uchar val0xE4 = 0u;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		DLOG_Debug(" v2hRemainingCapacity3 : %d", ope->v2hRemainingCapacity3);
		/* 車載電池の放電可能残容量３ [％]  範囲：0～100[％] */
		val0xE4 = ope->v2hRemainingCapacity3;

		/* 範囲チェック。0x00 - 0x64(0～100) */
		if (val0xE4 > 0x64u)
		{
			val0xE4 = 0x64u;
		} else
		{
			/* 処理続行 */
		}

		ret = EENL_ExtEnlDevSetProperty(NULL, info->devV2h, 0xE4u, 1u, &val0xE4);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Debug(" [timer v2h] 0xE4 : %d", val0xE4);
			/* 状変アナウンスはなし */
		} else
		{
			DLOG_Error(" [timer v2h] 0xE4 pcs error!:%ld", ret);
			/* 異常でも処理続行 */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xDBの設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hTimer0xDB(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	uchar pwrCutStatus = EPCS_PWROUTAGE_NO;
	uchar val0xDB = 0u;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		DLOG_Debug(" pwrCut : %d", ope->pwrCut);
		pwrCutStatus = ope->pwrCut;

		if (pwrCutStatus == EPCS_PWROUTAGE_YES)
		{
			/* 停電の場合 */
			/* 独立 */
			val0xDB = 0x01u;
		} else
		{
			/* 停電以外の場合 */
			/* 系統連系（逆潮流不可）*/
			val0xDB = 0x02u;
		}

		/* 車両充放電器0xDB */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h,
			0xDBu, val0xDB, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/* 成功 */
			DLOG_Debug("v2h 0xDB set success : 0x%02x", val0xDB);
			/* 状変アナウンスは無し */
		} else
		{
			/* 失敗 */
			DLOG_Debug("v2h 0xDB set fail : 0x%02x(%ld)", val0xDB, ret);
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xE1の設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hTimer0xE1(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	uchar v2hState = EPCS_V2H_STATE_ERR_STOP;
	uchar val0xE1 = 0u;
	EenlDeviceInfo_t* info = NULL;
	ENL_MESSAGE* msg;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		DLOG_Debug(" v2hState : %d", ope->v2hState);
		v2hState = ope->v2hState;

		if (v2hState == EPCS_V2H_STATE_CHARGE)
		{
			/* 充電中の場合 */
			val0xE1 = 0x42;
		} else if (v2hState == EPCS_V2H_STATE_DISCHARGE)
		{
			/* 放電中の場合 */
			val0xE1 = 0x43;
		} else if (v2hState == EPCS_V2H_STATE_WAIT)
		{
			/* 待機の場合 */
			val0xE1 = 0x44u;
		} else if (v2hState == EPCS_V2H_STATE_ERR_STOP)
		{
			/* 停止の場合 */
			val0xE1 = 0x47u;
		} else
		{
			/* その他場合 */
			val0xE1 = 0x40u;
		}

		/* 車両充放電器0xE1 */
		ret = EENL_ExtEnlDevSetPropertyValue(&msg, info->devV2h,
			0xE1u, val0xE1, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/* 成功 */
			DLOG_Debug("v2h 0xE1 set success : 0x%02x", val0xE1);
			/* 状変アナウンスを通知 */
			EENL_SendMessage(NULL, msg);
			DLOG_Debug("#### send broadcast message (v2h:0xE1) ####");
		} else
		{
			/*失敗 */
			DLOG_Debug("v2h 0xDB set fail : 0x%02x(%ld)", val0xE1, ret);
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xCEの設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hTimer0xCE(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong val0xCE = 0u;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		DLOG_Debug(" v2hChargeableCapValue : %ld",
			ope->v2hChargeableCapValue);
		val0xCE = ope->v2hChargeableCapValue;
		/* 範囲チェックはしない(epcsと同じ範囲 0-999,999,999) */

		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h,
			0xCEu, val0xCE, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/*成功 */
			DLOG_Debug(" [epcs v2h] 0xCE: 0x%lx(%ld)", val0xCE, val0xCE);
			/*状変アナウンスは無し */
		} else
		{
			/*失敗 */
			DLOG_Error("v2h 0xCE pcs error!:%ld", ret);
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xD0の設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hTimer0xD0(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong val0xD0 = 0u;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		DLOG_Debug(" v2hUsedCapacity1 : %ld", ope->v2hUsedCapacity1);
		val0xD0 = ope->v2hUsedCapacity1;
		/* 範囲チェックはしない(epcsと同じ範囲 0-999,999,999) */

		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h,
			0xD0u, val0xD0, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/*成功 */
			DLOG_Debug(" [epcs v2h] 0xD0 : 0x%lx(%ld)", val0xD0, val0xD0);
			/*状変アナウンスは無し */
		} else
		{
			/*失敗 */
			DLOG_Error("v2h 0xD0 pcs error!:%ld", ret);
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス 0xC0の設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_V2hTimer0xC0(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong val0xC0 = 0u;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devV2h))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		DLOG_Debug(" v2hDischargeCapacity1 : %ld",
			ope->v2hDischargeCapacity1);
		val0xC0 = ope->v2hDischargeCapacity1;
		/* 範囲チェックはしない(epcsと同じ範囲 0-999,999,999) */

		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h,
			0xC0u, val0xC0, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/*成功 */
			DLOG_Debug(" [epcs v2h] 0xC0 : 0x%lx(%ld)", val0xC0, val0xC0);
			/*状変アナウンスは無し */
		} else
		{
			/*失敗 */
			DLOG_Error("v2h 0xC0 pcs error!:%ld", ret);
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:V2Hクラス V2H設定処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t*	info  デバイス情報					*/
/*					:const ENOT_ERRST_T*	errst リモコン内部エラー状態	*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
void EENL_V2hTimerSetInfo(EenlDeviceInfo_t* info, const ENOT_ERRST_T* errst)
{
	/*リモコン内部エラー状態 */
	ulong prevErrState = s_eenlErrst & ENOT_ERRSTATE_PCS;
	ulong curErrState = errst->errState & ENOT_ERRSTATE_PCS;
	slong ret = 0;

	if (prevErrState != curErrState)
	{
		/* 正常⇒PCS通信不通 or PCS通信不通⇒通信回復 */
		ret = EENL_UpdateEpcsV2h(info);
		s_eenlErrst = errst->errState;
		if (ret != 0)
		{
			DLOG_Error(" [timer v2h] setInfo error!:%ld", ret);
		} else
		{
			/* 処理続行 */
		}
	} else
	{
		/* 処理続行 */
	}
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xC5								*/
/*					:(定格充電能力)のepcsデータの反映						*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t			*info						*/
/*					:EPCS_V2H_SETINFO_GET_T		v2hSet						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hUpdate0xC5(EenlDeviceInfo_t* info, EPCS_V2H_SETINFO_GET_T* v2hSet)
{
	slong ret = 0;
	ulong valUl = 0u;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	valUl = v2hSet->v2hRatedCharge;
	/* 範囲チェック。0x000000000～0x3B9AC9FF(0～999,999,999) */
	if (valUl > 0x3B9AC9FFu)
	{
		valUl = 0x3B9AC9FFu;
	} else
	{
		/* 処理続行 */
	}

	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h, 0xC5u, valUl, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [epcs v2h] 0xC5 : 0x%lx(%ld)", valUl, valUl);
	} else
	{
		DLOG_Error("v2h 0xC5 pcs error!:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xC6								*/
/*					:(定格充電能力)のepcsデータの反映						*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t			*info						*/
/*					:EPCS_V2H_SETINFO_GET_T		v2hSet						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hUpdate0xC6(EenlDeviceInfo_t* info, EPCS_V2H_SETINFO_GET_T* v2hSet)
{
	slong ret = 0;
	ulong valUl = 0u;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	valUl = v2hSet->v2hRatedDischarge;
	/* 範囲チェック。0x000000000～0x3B9AC9FF(0～999,999,999) */
	if (valUl > 0x3B9AC9FFu)
	{
		valUl = 0x3B9AC9FFu;
	} else
	{
		/* 処理続行 */
	}

	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h, 0xC6u, valUl, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [epcs v2h] 0xC6 : 0x%lx(%ld)", valUl, valUl);
	} else
	{
		DLOG_Error("v2h 0xC6 pcs error!:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xC8								*/
/*					:(最小最大充電電力値)のepcsデータの反映					*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t			*info						*/
/*					:EPCS_V2H_SETINFO_GET_T		v2hSet						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hUpdate0xC8(EenlDeviceInfo_t* info, EPCS_V2H_SETINFO_GET_T* v2hSet)
{
	slong ret = 0;
	ulong valUl = 0u;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* 最小値 ０W固定値 */
	valUl = 0u;

	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h, 0xC8u, valUl, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [epcs v2h] 0xC8 : [min] 0x%lx(%ld)", valUl, valUl);
	} else
	{
		DLOG_Error("v2h 0xC8 [min] pcs error!:%ld", ret);
		/* 処理続行 */
	}

	/* 最大値 定格充電能力 */
	valUl = v2hSet->v2hRatedCharge;

	/* 範囲チェック。0x000000000～0x3B9AC9FF(0～999,999,999) */
	if (valUl > 0x3B9AC9FFu)
	{
		valUl = 0x3B9AC9FFu;
	} else
	{
		/* 処理続行 */
	}

	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h, 0xC8u, valUl, 1u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [epcs v2h] 0xC8 : [max] 0x%lx(%ld)", valUl, valUl);
	} else
	{
		DLOG_Error("v2h 0xC8 [max] pcs error!:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xC9								*/
/*					:(最小最大放電電力値)のepcsデータの反映					*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t			*info						*/
/*					:EPCS_V2H_SETINFO_GET_T		v2hSet						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hUpdate0xC9(EenlDeviceInfo_t* info, EPCS_V2H_SETINFO_GET_T* v2hSet)
{
	slong ret = 0;
	ulong valUl = 0u;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* 最小値 0W固定値 */
	valUl = 0u;

	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h, 0xC9, valUl, 0);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [epcs v2h] 0xC9 : [min] 0x%lx(%ld)", valUl, valUl);
	} else
	{
		DLOG_Error("v2h 0xC9 [min] pcs error!:%ld", ret);
		/* 処理続行 */
	}

	/* 最大値 定格放電能力 */
	valUl = v2hSet->v2hRatedDischarge;

	/* 範囲チェック。0x000000000～0x3B9AC9FF(0～999,999,999) */
	if (valUl > 0x3B9AC9FFu)
	{
		valUl = 0x3B9AC9FFu;
	} else
	{
		/* 処理続行 */
	}

	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h, 0xC9u, valUl, 1u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [epcs v2h] 0xC9 : [max] 0x%lx(%ld)", valUl, valUl);
	} else
	{
		DLOG_Error("v2h 0xC9 [max] pcs error!:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xCA								*/
/*					:(最小最大充電電流値)のepcsデータの反映					*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t			*info						*/
/*					:EPCS_V2H_SETINFO_GET_T		v2hSet						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hUpdate0xCA(EenlDeviceInfo_t* info)
{
	slong ret = 0;

	/* 最小値=0A、最大値=30A */
	ushort val0xCA[2] = { 0u, 30u };

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	val0xCA[0] = (ushort)EENL_BYTE_SWAP_16(val0xCA[0]); /* BYTE SWAP */
	val0xCA[1] = (ushort)EENL_BYTE_SWAP_16(val0xCA[1]); /* BYTE SWAP */

	ret = EENL_ExtEnlDevSetProperty(NULL, info->devV2h, 0xCAu, 4u, (uchar*)val0xCA);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [epcs v2h] 0xCA : [min] 0x%x(%d)", val0xCA[0], val0xCA[0]);
		DLOG_Debug(" [epcs v2h] 0xCA : [max] 0x%x(%d)", val0xCA[1], val0xCA[1]);
	} else
	{
		DLOG_Error("v2h 0xCA pcs error!:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xCB								*/
/*					:(最小最大放電電流値)のepcsデータの反映					*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t			*info						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hUpdate0xCB(EenlDeviceInfo_t* info)
{
	slong ret = 0;

	/* 最小値=0A、最大値=30A */
	ushort val0xCB[2] = { 0u, 30u };

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	val0xCB[0] = (ushort)EENL_BYTE_SWAP_16(val0xCB[0]); /* BYTE SWAP */
	val0xCB[1] = (ushort)EENL_BYTE_SWAP_16(val0xCB[1]); /* BYTE SWAP */

	ret = EENL_ExtEnlDevSetProperty(NULL, info->devV2h, 0xCBu, 4u, (uchar*)val0xCB);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [epcs v2h] 0xCB : [min] 0x%x(%d)", val0xCB[0], val0xCB[0]);
		DLOG_Debug(" [epcs v2h] 0xCB : [max] 0x%x(%d)", val0xCB[1], val0xCB[1]);
	} else
	{
		DLOG_Error("v2h 0xCB pcs error!:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xCC								*/
/*					:(充放電器タイプ)のepcsデータの反映						*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t			*info						*/
/*					:EPCS_V2H_SETINFO_GET_T		v2hSet						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hUpdate0xCC(EenlDeviceInfo_t* info, EPCS_V2H_SETINFO_GET_T* v2hSet)
{
	slong ret = 0;
	uchar valUb = 0u;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	if (strcmp(v2hSet->v2hType, EENL_V2HTYPE_11) == 0)
	{
		valUb = 0x11u;
	} else if (strcmp(v2hSet->v2hType, EENL_V2HTYPE_12) == 0)
	{
		valUb = 0x12u;
	} else if (strcmp(v2hSet->v2hType, EENL_V2HTYPE_13) == 0)
	{
		valUb = 0x13u;
	} else if (strcmp(v2hSet->v2hType, EENL_V2HTYPE_21) == 0)
	{
		valUb = 0x21u;
	} else if (strcmp(v2hSet->v2hType, EENL_V2HTYPE_22) == 0)
	{
		valUb = 0x22u;
	} else if (strcmp(v2hSet->v2hType, EENL_V2HTYPE_23) == 0)
	{
		valUb = 0x23u;
	} else if (strcmp(v2hSet->v2hType, EENL_V2HTYPE_31) == 0)
	{
		valUb = 0x31u;
	} else if (strcmp(v2hSet->v2hType, EENL_V2HTYPE_32) == 0)
	{
		valUb = 0x32u;
	} else if (strcmp(v2hSet->v2hType, EENL_V2HTYPE_33) == 0)
	{
		valUb = 0x33u;
	} else if (strcmp(v2hSet->v2hType, EENL_V2HTYPE_41) == 0)
	{
		valUb = 0x41u;
	} else if (strcmp(v2hSet->v2hType, EENL_V2HTYPE_42) == 0)
	{
		valUb = 0x42u;
	} else if (strcmp(v2hSet->v2hType, EENL_V2HTYPE_43) == 0)
	{
		valUb = 0x43u;
	} else if (strcmp(v2hSet->v2hType, EENL_V2HTYPE_51) == 0)
	{
		valUb = 0x51u;
	} else if (strcmp(v2hSet->v2hType, EENL_V2HTYPE_52) == 0)
	{
		valUb = 0x52u;
	} else if (strcmp(v2hSet->v2hType, EENL_V2HTYPE_53) == 0)
	{
		valUb = 0x53u;
	} else
	{
		valUb = 0xFFu;
		DLOG_Error("v2h 0xCC v2hType error!:0x%x", valUb);
		/* そのまま続行 */
	}
	if (valUb != 0xFFu)
	{
		ret = EENL_ExtEnlDevSetProperty(NULL, info->devV2h, 0xCCu, 1u, &valUb);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Debug(" [epcs v2h] 0xCC : (0x%s)", v2hSet->v2hType);
		} else
		{
			DLOG_Error("v2h 0xCC pcs error!:%ld", ret);
			/* 処理続行 */
		}
	} else
	{
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xD2								*/
/*					:(定格電圧)のepcsデータの反映							*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t			*info						*/
/*					:EPCS_V2H_SETINFO_GET_T		v2hSet						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hUpdate0xD2(EenlDeviceInfo_t* info)
{
	slong ret = 0;
	ushort valUs = 200u;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	valUs = (ushort)EENL_BYTE_SWAP_16(valUs); /* BYTE SWAP */
	ret = EENL_ExtEnlDevSetProperty(NULL, info->devV2h, 0xD2u, 2u, (uchar*)&valUs);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [epcs v2h] 0xD2 : 0x%x(%d)", valUs, valUs);
	} else
	{
		DLOG_Error("v2h 0xD2 pcs error!:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xE6								*/
/*					:(車両ID)のepcsデータの反映								*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t			*info						*/
/*					:EPCS_V2H_SETINFO_GET_T		v2hSet						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hUpdate0xE6(EenlDeviceInfo_t* info, EPCS_V2H_SETINFO_GET_T* v2hSet)
{
	slong ret = 0;
	uchar pdc = 0u;
	uchar val0xE6[EENL_PDC_V2H_0xE6 + 1] = { 0x00u }; /* NULL止めするため+1する */

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	val0xE6[0] = v2hSet->vehicleIdLen;
	memcpy(&val0xE6[1], &v2hSet->vehicleId, EENL_PDC_V2H_0xE6 - 1);

	/*範囲チェックはepcsで実施済み */

	/*車両IDデータサイズを+1して全体のデータサイズをpdcに格納 */
	pdc = (uchar)(val0xE6[0] + 1u);

	ret = EENL_ExtEnlDevSetProperty(NULL, info->devV2h, 0xE6u, pdc, val0xE6);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [epcs v2h] 0xE6 id len: [%d]", val0xE6[0]);
		DLOG_Debug(" [epcs v2h] 0xE6 id: [%s]", &val0xE6[1]);
	} else
	{
		DLOG_Error("v2h 0xE6 pcs error!:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xEA								*/
/*					:(放電量設定値)のepcsデータの反映						*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t			*info						*/
/*					:EPCS_V2H_SETINFO_GET_T		v2hSet						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hUpdate0xEA(EenlDeviceInfo_t* info, EPCS_V2H_SETINFO_GET_T* v2hSet)
{
	slong ret = 0;
	ulong valUl = 0u;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	valUl = v2hSet->v2hDischargeSetValue;
	/* 範囲チェックはしない(epcsと同じ範囲 0-999,999,999) */
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devV2h, 0xEAu, valUl, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [epcs v2h] 0xEA : 0x%lx(%ld)", valUl, valUl);
	} else
	{
		DLOG_Error("v2h 0xEA pcs error!:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xCD								*/
/*					:(車両接続確認)の設定電文チェック						*/
/*--------------------------------------------------------------------------*/
/*	param			:uchar* pdc	設定値のデータサイズ						*/
/*					:uchar* edt	設定値										*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hSetCheck0xCD(uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_ACCEPT;
	uchar connConf = 0x10u;
	schar pcsRes = 0;
	slong ret2 = 0;

	DEBUG_TRCIN();

	if (*pdc != 1u)
	{
		/* サイズが1バイト以外の場合 */
		DLOG_Error(" pdc error!(%d)", *pdc);
		ret = ENL_RESULT_NOT_ACCEPT;
	} else if (*edt != 0x10u)
	{	/* 0x10:接続確認のみ */
		/* 0x10以外なら */
		DLOG_Error(" edt error!(%d)", *edt);
		ret = ENL_RESULT_NOT_ACCEPT;
	} else
	{
		/* V2H車両接続確認 */
		ret2 = epcs_sync_chkV2hConnConf(connConf, &pcsRes);
		if (ret2 == EPCS_OK)
		{
			/* 設定OK */
			DLOG_Debug(" v2h 0xC7 epcs_sync_chkV2hConnConf ok! connConf=%d, pcsRes=%d)", connConf, pcsRes);
			if (pcsRes == EENL_PCS_NAK)
			{
				DLOG_Error(" v2h 0xCD pcsRes = NAK!(%d). V2hConnConf NG! ", pcsRes);
				ret = ENL_RESULT_NOT_ACCEPT;
			}
		} else
		{
			/* 設定失敗 */
			DLOG_Error(" v2h 0xCD epcs_sync_chkV2hConnConf NG! :%ld ", ret2);
			ret = ENL_RESULT_NOT_ACCEPT;
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xD7								*/
/*					:(積算放電電力量リセット設定)の設定電文チェック			*/
/*--------------------------------------------------------------------------*/
/*	param			:uchar	*pdc	設定値のデータサイズ					*/
/*					:uchar	*edt	設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hSetCheck0xD7(uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_ACCEPT;
	EPCS_V2H_TTL_ENERGY_RESET_T ttlEnergyReset;
	slong ret2 = 0;

	DEBUG_TRCIN();

	if (*pdc != 1u)
	{
		/*サイズが1バイト以外の場合 */
		DLOG_Error(" pdc error!(%d)", *pdc);
		ret = ENL_RESULT_NOT_ACCEPT;
	} else if (*edt != 0x00u)
	{
		/* 0x00:リセットのみ */
		/* 0x00以外なら */
		DLOG_Error(" edt error!(%d)", *edt);
		ret = ENL_RESULT_NOT_ACCEPT;
	} else
	{
		memset(&ttlEnergyReset, 0x00, sizeof(EPCS_V2H_TTL_ENERGY_RESET_T));
		ttlEnergyReset.setChangeFlag = EPCS_SET_V2H_TTL_DISCHARGE_RESET;
		ttlEnergyReset.v2hTtlDishargeEnergyReset = EPCS_V2H_TTL_ENERGY_RESET_YES;
		ttlEnergyReset.v2hTtlChargeEnergyReset = EPCS_V2H_TTL_ENERGY_RESET_NO;
		DLOG_Debug(" v2h 0xD7 setChangeFlag=%d", ttlEnergyReset.setChangeFlag);
		DLOG_Debug(" v2h 0xD7 Disharge=%d", ttlEnergyReset.v2hTtlDishargeEnergyReset);
		DLOG_Debug(" v2h 0xD7 Charge=%d", ttlEnergyReset.v2hTtlChargeEnergyReset);

		/* V2H積算情報リセット */
		ret2 = epcs_sync_setV2hTtlEnergyReset(&ttlEnergyReset);
		if (ret2 == EPCS_OK)
		{
			/* 設定OK */
			DLOG_Debug(" v2h 0xD7 epcs_sync_setV2hTtlEnergyReset ok! Disharge=%d, Charge=%d)",
				ttlEnergyReset.v2hTtlDishargeEnergyReset, ttlEnergyReset.v2hTtlChargeEnergyReset);
		} else
		{
			/* 設定失敗 */
			DLOG_Error(" v2h 0xD7 epcs_sync_setV2hTtlEnergyReset NG! :%ld ", ret2);
			ret = ENL_RESULT_NOT_ACCEPT;
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xD9								*/
/*					:(積算充電電力量リセット設定)の設定電文チェック			*/
/*--------------------------------------------------------------------------*/
/*	param			:uchar	*pdc	設定値のデータサイズ					*/
/*					:uchar	*edt	設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hSetCheck0xD9(uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_ACCEPT;
	EPCS_V2H_TTL_ENERGY_RESET_T ttlEnergyReset;
	slong ret2 = 0;

	DEBUG_TRCIN();

	if (*pdc != 1u)
	{
		/* サイズが1バイト以外の場合 */
		DLOG_Error(" pdc error!(%d)", *pdc);
		ret = ENL_RESULT_NOT_ACCEPT;
	} else if (*edt != 0x00u)
	{
		/* 0x00:リセットのみ */
		/* 0x00以外なら */
		DLOG_Error(" edt error!(%d)", *edt);
		ret = ENL_RESULT_NOT_ACCEPT;
	} else
	{
		memset(&ttlEnergyReset, 0x00, sizeof(EPCS_V2H_TTL_ENERGY_RESET_T));
		ttlEnergyReset.setChangeFlag = EPCS_SET_V2H_TTL_CHARGE_RESET;
		ttlEnergyReset.v2hTtlDishargeEnergyReset = EPCS_V2H_TTL_ENERGY_RESET_NO;
		ttlEnergyReset.v2hTtlChargeEnergyReset = EPCS_V2H_TTL_ENERGY_RESET_YES;
		DLOG_Debug(" v2h 0xD9 setChangeFlag=%d", ttlEnergyReset.setChangeFlag);
		DLOG_Debug(" v2h 0xD9 Disharge=%d", ttlEnergyReset.v2hTtlDishargeEnergyReset);
		DLOG_Debug(" v2h 0xD9 Charge=%d", ttlEnergyReset.v2hTtlChargeEnergyReset);
		/* V2H積算情報リセット */
		ret2 = epcs_sync_setV2hTtlEnergyReset(&ttlEnergyReset);
		if (ret2 == EPCS_OK)
		{
			/* 設定OK */
			DLOG_Debug(" v2h 0xD9 epcs_sync_setV2hTtlEnergyReset ok! Disharge=%d, Charge=%d)",
				ttlEnergyReset.v2hTtlDishargeEnergyReset,
				ttlEnergyReset.v2hTtlChargeEnergyReset);
		} else
		{
			/* 設定失敗 */
			DLOG_Error(" v2h 0xD9 epcs_sync_setV2hTtlEnergyReset NG! :%ld ", ret2);
			ret = ENL_RESULT_NOT_ACCEPT;
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xDA								*/
/*					:(運転モード設定)の設定電文チェック						*/
/*--------------------------------------------------------------------------*/
/*	param			:uchar	*pdc	設定値のデータサイズ					*/
/*					:uchar	*edt	設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hSetCheck0xDA(uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_NOT_ACCEPT;
	EPCS_OPEMODE_SET_T ope;
	slong ret2 = 0;

	DEBUG_TRCIN();

	if (*pdc != 1u)
	{
		/* サイズが1バイト以外の場合 */
		DLOG_Error(" pdc error!(%d)", *pdc);
		ret = ENL_RESULT_NOT_ACCEPT;
	} else
	{
		ret = ENL_RESULT_ACCEPT;
		memset(&ope, 0x00, sizeof(EPCS_OPEMODE_GET_T));
		switch (*edt)
		{
		case 0x42:
			/* 充電 */
			/* 車両充放電器運転モード */
			/* 車両充放電器運転モードフラグ設定値 */
			/* 車両充電 */
			ope.setChangeFlag = EPCS_SET_OPE_MODE | EPCS_SET_LIMIT_CTRL;
			ope.opeMode = EPCS_OPEMODE_B;
			break;
		case 0x43:
			/* 放電 */
			/* 車両充放電器運転モード */
			/*車両充放電器運転モードフラグ設定値 */
			/* 車両放電 */
			ope.setChangeFlag = EPCS_SET_OPE_MODE | EPCS_SET_LIMIT_CTRL;
			ope.opeMode = EPCS_OPEMODE_C;
			break;
		case 0x44:
			/* 待機 */
			/* 車両充放電器運転モード */
			/* 車両充放電器運転モードフラグ設定値 */
			/* 待機 */
			ope.setChangeFlag = EPCS_SET_OPE_MODE | EPCS_SET_LIMIT_CTRL;
			ope.opeMode = EPCS_OPEMODE_6;
			break;
		default:
			/* 充電、放電、待機以外なら不可応答 */
			DLOG_Error(" edt error!(%d)", *edt);
			ret = ENL_RESULT_NOT_ACCEPT;
			break;
		}
		ope.remoteState = EPCS_REMOTE_ON;
		ope.limitCtrl = EPCS_LIMIT_CTRL_UNDEF;
		/* ※setChangeFlagにEPCS_SET_LIMIT_CTRLを指定しない場合も、*/
		/* 無効値を設定しておく。*/
	}
	if (ret == ENL_RESULT_ACCEPT)
	{
		DLOG_Debug(" v2h 0xDA v2hOpeMode=%d remoteState=%d",
			ope.opeMode, ope.remoteState);
		ret2 = epcs_sync_setOpeMode(&ope);
		if (ret2 == EPCS_OK)
		{
			/* 設定OK */
			DLOG_Debug(" v2h 0xDA epcs_sync_setV2hOpeMode ok!)");
		} else
		{
			/* 設定失敗 */
			DLOG_Error(" v2h 0xDA epcs_sync_setV2hOpeMode NG! :%ld ", ret2);
			ret = ENL_RESULT_NOT_ACCEPT;
		}
	} else
	{
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xE7								*/
/*					:(充電量設定値1)の設定電文チェック						*/
/*--------------------------------------------------------------------------*/
/*	param			:uchar	epc		設定対象EPC								*/
/*					:uchar	*pdc	設定値のデータサイズ					*/
/*					:uchar	*edt	設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hSetCheck0xE7(uchar epc, uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_NOT_ACCEPT;
	/* 希望するPDCサイズ */
	uchar pdcSize = 4u;
	/* セットするデータ値用 */
	ulong dat = 0u;
	/* セットするデータの最大値 */
	ulong datMax = 0x3B9AC9FFu;
	/* セットするV2H設定情報設定用構造体 */
	EPCS_V2H_SETINFO_SET_T v2hSetInfoSet;

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	DEBUG_TRCIN();

	dat = (ulong)((ulong)((*edt) * 256u * 256u * 256u)
		+ (ulong)((*(edt + 1u)) * 256u * 256u)
		+ (ulong)((*(edt + 2u)) * 256u)
		+ (ulong)((*(edt + 3u))));
	memset(&v2hSetInfoSet, 0x00, sizeof(EPCS_V2H_SETINFO_SET_T));
	/* 車載電池充電量設定値１ */
	v2hSetInfoSet.setChangeFlag = EPCS_SET_V2H_CHARGE_AMOUNT_SET_1;
	/* 車載電池充電量設定値１ */
	v2hSetInfoSet.v2hChargeAmountSet1 = dat;

	DLOG_Debug(" v2h 0xE7 setChangeFlag=%ld",
		v2hSetInfoSet.setChangeFlag);
	DLOG_Debug(" v2h 0xE7 v2hChargeAmountSet1=%ld",
		v2hSetInfoSet.v2hChargeAmountSet1);

	/* 設定電文のチェック（V2H設定情報設定用構造体用） */
	ret = eenl_V2hSetInfoSetCheck(epc, *pdc, pdcSize,
		(ulong)dat, (ulong)datMax, v2hSetInfoSet);

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xEA								*/
/*					:(放電量設定値)の設定電文チェック						*/
/*--------------------------------------------------------------------------*/
/*	param			:uchar	epc		設定対象EPC								*/
/*					:uchar	*pdc	設定値のデータサイズ					*/
/*					:uchar	*edt	設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hSetCheck0xEA(uchar epc, uchar* pdc, uchar* edt)
{
	slong ret;
	/* 希望するPDCサイズ */
	uchar pdcSize = 4u;
	/* セットするデータ値用 */
	ulong dat = 0;
	/* セットするデータの最大値 */
	ulong datMax = 0x3B9AC9FFu;
	/* セットするV2H設定情報設定用構造体 */
	EPCS_V2H_SETINFO_SET_T v2hSetInfoSet;

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	DEBUG_TRCIN();

	dat = (ulong)((ulong)((*edt) << 24u) | (ulong)((*(edt + 1u)) << 16u)
		| (ulong)((*(edt + 2u)) << 8u)
		| (ulong)(*(edt + 3u)));
	memset(&v2hSetInfoSet, 0x00, sizeof(EPCS_V2H_SETINFO_SET_T));
	/* 車載電池放電量設定値 */
	v2hSetInfoSet.setChangeFlag = EPCS_SET_V2H_DISCHARGE_SET_VALUE;
	/* 車載電池放電量設定値 */
	v2hSetInfoSet.v2hDischargeSetValue = dat;

	DLOG_Debug(" v2h 0xEA setChangeFlag=%ld",
		v2hSetInfoSet.setChangeFlag);
	DLOG_Debug(" v2h 0xEA v2hCurrentSetCharge=%ld",
		v2hSetInfoSet.v2hDischargeSetValue);

	/* 設定電文のチェック（V2H設定情報設定用構造体用） */
	ret = eenl_V2hSetInfoSetCheck(epc, *pdc, pdcSize,
		(ulong)dat, (ulong)datMax, v2hSetInfoSet);

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xEB								*/
/*					:(充電電力設定値)の設定電文チェック						*/
/*--------------------------------------------------------------------------*/
/*	param			:uchar	epc		設定対象EPC								*/
/*					:uchar	*pdc	設定値のデータサイズ					*/
/*					:uchar	*edt	設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hSetCheck0xEB(uchar epc, uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_ACCEPT;
	/* 希望するPDCサイズ */
	uchar pdcSize = 4u;
	/* セットするデータ値用 */
	ulong dat = 0u;
	/* セットするデータの最大値 */
	ulong datMax = 0x3B9AC9FFu;
	/* セットするV2H設定情報設定用構造体 */
	EPCS_V2H_SETINFO_SET_T v2hSetInfoSet;
	slong epcsRet = 0;

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	DEBUG_TRCIN();

	if (g_eenlTestRunState == EENL_TESTRUN_START)
	{
		/* 試運転中は不可応答 */
		DLOG_Debug(" v2h 0xEB detect test run start! flag=%d ",
			g_eenlTestRunState);
		ret = ENL_RESULT_NOT_ACCEPT;
	} else
	{
		/* 2021/10/13 ADAM修正計画No.117対処 */
		EPCS_OPEMODE_GET_T ope;
		memset(&ope, 0x00, sizeof(EPCS_OPEMODE_GET_T));
		epcsRet = epcs_sync_getOpeMode(&ope);
		/* opeの取得が出来て、ope.opeModeが電力移動モードまたは */
		/* opeの取得が出来きなくて、g_eenlKeepOpeModeが電力移動モードなら */
		DLOG_Debug("epcsRet=%ld/ope.opeMode=%d/g_eenlKeepOpeMode=%d",
			epcsRet, ope.opeMode, g_eenlKeepOpeMode);
		if (((epcsRet == EPCS_OK)
			&& (EENL_ElectricMoveModeChk(ope.opeMode) == TRUE))
			|| ((epcsRet != EPCS_OK)
				&& (EENL_ElectricMoveModeChk(g_eenlKeepOpeMode) == TRUE)))
		{
			/* epcs_sync_getOpeMode()が失敗した時のエラー回避の予備の判定 */
			/*電力移動モードでは不可応答 */
			DLOG_Debug(" v2h 0xEB opeMode = Electric_Move_Mode.");

			dat = (ulong)((ulong)((*edt) * 256u * 256u * 256u)
				+ (ulong)((*(edt + 1u)) * 256u * 256u)
				+ (ulong)((*(edt + 2u)) * 256u)
				+ (ulong)((*(edt + 3u))));
			memset(&v2hSetInfoSet, 0x00, sizeof(EPCS_V2H_SETINFO_SET_T));
			/* 車載電池充電電力設定値 */
			v2hSetInfoSet.setChangeFlag = EPCS_SET_V2H_POWER_SET_CHARGE;
			/* 車載電池充電電力設定値 */
			v2hSetInfoSet.v2hPowerSetCharge = dat;

			DLOG_Debug(" v2h 0xEB setChangeFlag=%ld",
				v2hSetInfoSet.setChangeFlag);
			DLOG_Debug(" v2h 0xEB v2hPowerSetCharge=%ld",
				v2hSetInfoSet.v2hPowerSetCharge);

			/* 設定電文のチェック（V2H設定情報設定用構造体用） */
			ret = eenl_V2hSetInfoSetCheck(epc, *pdc, pdcSize,
				(ulong)dat, (ulong)datMax, v2hSetInfoSet);
		} else
		{
			/* 処理続行 */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:車両充放電器クラス 0xEC								*/
/*					:(放電電力設定値)の設定電文チェック						*/
/*--------------------------------------------------------------------------*/
/*	param			:uchar	epc		設定対象EPC								*/
/*					:uchar	*pdc	設定値のデータサイズ					*/
/*					:uchar	*edt	設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_V2hSetCheck0xEC(uchar epc, uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_ACCEPT;
	/* 希望するPDCサイズ */
	uchar pdcSize = 4u;
	/* セットするデータ値用 */
	ulong dat = 0u;
	/* セットするデータの最大値 */
	ulong datMax = 0x3B9AC9FFu;
	/* セットするV2H設定情報設定用構造体 */
	EPCS_V2H_SETINFO_SET_T v2hSetInfoSet;
	slong epcsRet = 0;

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	DEBUG_TRCIN();

	if (g_eenlTestRunState == EENL_TESTRUN_START)
	{
		/* 試運転中は不可応答 */
		DLOG_Debug(" v2h 0xEC detect test run start! flag=%d ",
			g_eenlTestRunState);
		ret = ENL_RESULT_NOT_ACCEPT;
	} else if (g_eenlPwrCut == EPCS_PWROUTAGE_YES)
	{
		/* 停電中は不可応答 */
		DLOG_Debug(" v2h 0xEC detect power outage! flag=%d ", g_eenlPwrCut);
		ret = ENL_RESULT_NOT_ACCEPT;
	} else
	{
		/* 2021/10/13 ADAM修正計画No.117対処 */
		EPCS_OPEMODE_GET_T ope;
		memset(&ope, 0x00, sizeof(EPCS_OPEMODE_GET_T));

		epcsRet = epcs_sync_getOpeMode(&ope);

		/* opeの取得が出来て、ope.opeModeが電力移動モードまたは */
		/* opeの取得が出来きなくて、g_eenlKeepOpeModeが電力移動モードなら */
		DLOG_Debug("epcsRet=%ld/ope.opeMode=%d/g_eenlKeepOpeMode=%d",
			epcsRet, ope.opeMode, g_eenlKeepOpeMode);

		if (((epcsRet == EPCS_OK)
			&& (EENL_ElectricMoveModeChk(ope.opeMode) == TRUE))
			|| ((epcsRet != EPCS_OK)
				&& (EENL_ElectricMoveModeChk(g_eenlKeepOpeMode) == TRUE)))
		{
			/* epcs_sync_getOpeMode()が失敗した時のエラー回避の予備の判定 */
			/* 電力移動モードでは不可応答 */
			DLOG_Debug(" v2h 0xEC opeMode = Electric_Move_Mode.");

			dat = (ulong)((ulong)((*edt) * 256u * 256u * 256u)
				+ (ulong)((*(edt + 1u)) * 256u * 256u)
				+ (ulong)((*(edt + 2u)) * 256u)
				+ (ulong)((*(edt + 3u))));
			memset(&v2hSetInfoSet, 0x00, sizeof(EPCS_V2H_SETINFO_SET_T));
			/* 車載電池放電電力設定値 */
			v2hSetInfoSet.setChangeFlag = EPCS_SET_V2H_POWER_SET_DISCHARGE;
			/* 車載電池放電電力設定値 */
			v2hSetInfoSet.v2hPowerSetDiscCharge = dat;

			DLOG_Debug(" v2h 0xEC setChangeFlag=%ld",
				v2hSetInfoSet.setChangeFlag);
			DLOG_Debug(" v2h 0xEC v2hPowerSetDiscCharge=%ld",
				v2hSetInfoSet.v2hPowerSetDiscCharge);

			/* 設定電文のチェック（V2H設定情報設定用構造体用）*/
			ret = eenl_V2hSetInfoSetCheck(epc, *pdc, pdcSize,
				(ulong)dat, (ulong)datMax, v2hSetInfoSet);
		} else
		{
			/* 処理続行 */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}
