/****************************************************************************/
/*	Description		:ECHONET Lite application for device usage				*/
/*--------------------------------------------------------------------------*/
/*	Author			:														*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/18												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:eenl													*/
/*--------------------------------------------------------------------------*/
/*	Note			:機器オブジェクト分散型電源電力量メータクラス(E1)		*/
/*					:base on Ubiquitous sample application v1.5				*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(C) 2014. Nichicon Corporation. All Rights Reserved.	*/
/*					:(C) Ubiquitous Corp. 2001-2013. All rights reserved.	*/
/****************************************************************************/
#include "eenl_cmn_g.h"
#include "eenl_meter.h"

#define TOTAL_COLLECT_DATE ((ushort)(100u))
#define TOTAL_ENERGY_SIZE ((ushort)(192u))
#define INSTENCE_CODE_CNT ((ushort)(2u))


/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
/* E1 */
ushort s_ttlPcsChargeCurrentDate = 0;
ushort s_ttlPcsChargeCurrentValue = 0;
uchar s_ttlPcsChargeEnergyHistoryList[TOTAL_COLLECT_DATE]
[TOTAL_ENERGY_SIZE] = { 0x00 };
ushort s_ttlPcsDischargeCurrentDate = 0;
ushort s_ttlPcsDischargeCurrentValue = 0;
uchar s_ttlPcsDischargeEnergyHistoryList[TOTAL_COLLECT_DATE]
[TOTAL_ENERGY_SIZE] = { 0x00 };
uchar s_ttlPcsChargeEnergy[11] = { 0x00 };
uchar s_ttlPcsDischargeEnergy[11] = { 0x00 };
/* C2 */
ushort s_ttlV2hChargeCurrentDate = 0;
ushort s_ttlV2hChargeCurrentValue = 0;
uchar s_ttlV2hChargeEnergyHistoryList[TOTAL_COLLECT_DATE]
[TOTAL_ENERGY_SIZE] = { 0x00 };
ushort s_ttlV2hDischargeCurrentDate = 0;
ushort s_ttlV2hDischargeCurrentValue = 0;
uchar s_ttlV2hDischargeEnergyHistoryList[TOTAL_COLLECT_DATE]
[TOTAL_ENERGY_SIZE] = { 0x00 };
uchar s_ttlV2hChargeEnergy[11] = { 0x00 };
uchar s_ttlV2hDischargeEnergy[11] = { 0x00 };

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description		:分散型電源電力量メータデバイスの作成					*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*					:ushort meterDevKind									*/
/*					:ushort meterDevCnt										*/
/*--------------------------------------------------------------------------*/
/*	return			:現在故障の発生状況有無									*/
/*					:(0:正常終了/-1:異常発生有)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:eenl_fileRead()を呼び出してから実行すること			*/
/*					:構築なので状変アナウンス通知は無し						*/
/*					:eenl_common.cで定義したプロパティリストの定義と		*/
/*					:初期化対象のepcの不一致が無いこと!!					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/18: DHC K.Jiang : 新規作成						*/
/*==========================================================================*/
slong EENL_CreateDeviceMeter(EenlDeviceInfo_t* info, ushort meterDevKind, ushort meterDevCnt)
{
	slong ret = 0;
	slong cnt = 0u;
	uchar data = 0x00u;
	bool_t isErr = FALSE;
	uchar meterId[17] = { 0xFEu, 0x00u, 0x00u, 0x6Cu, 0x40u, 0x20u,
						0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u,
						0x20u, 0x20u, 0x20u, 0x20u, 0x20 };
	uchar merr[4] = { 0x00u, 0x00u, 0x00u, 0x6C };
	uchar space12[12] = { 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u,
						0x20u, 0x20u, 0x20u, 0x20 };
	uchar zero2b[2] = { 0x00u, 0x00u };
	uchar zero3b[3] = { 0x00u, 0x00u, 0x00u };
	uchar zero7b[7] = { 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u };
	uchar nowDate[4] = { 0x07u, 0xDEu, 0x01u, 0x01u };
	ushort initShort = 0x0000u;
	ulong initLong = 0x00000000u;
	uchar initUsedEnergy[194] = { 0x00u };
	ushort usedEnergyCollectDate = 0x0000u;
	ulong usedEnergyCollectValue[48] = { 0xFFFFFFFEu };
	uchar initTimerUsedEnergy[11] = { 0x00u };
	ushort timerUsedEnergyYear = 0x0001u;
	uchar timerUsedEnergyDate[2] = { 0x01u, 0x01u };
	uchar timerUsedEnergyTime[3] = { 0x00u, 0x00u, 0x00u };
	ulong timerUsedEnergyValue = 0xFFFFFFFEu;
	uchar instenceCodeList[INSTENCE_CODE_CNT] = { 0x01u, 0x02u };
	void** devMeterInfo = NULL;
	ushort i = 0u;

	DEBUG_TRCIN();

	if (info == NULL)
	{
		/* パラメーターエラー */
		return -1;
	} else
	{
		/* 処理継続 */
	}

	/* 機器オブジェクト分散型電源電力量メータクラス (E1)/(C2) */
	if (meterDevKind == EPCS_SYS_CON_METER_E1)
	{
		*devMeterInfo = info->devMeterE1;
	} else if (meterDevKind == EPCS_SYS_CON_METER_C2)
	{
		*devMeterInfo = info->devMeterC2;
	} else
	{
		/* 処理継続 */
	}

	/* 分散型電源電力量メータデバイスの作成 */
	ret = ENL_create_device_distributed_generator_electric_energy_meter(
		devMeterInfo,
		g_devicePropMeter,
		NULL,
		instenceCodeList[meterDevCnt],
		ENL_INSTANCE_MODE_DEVICE);
	if (ret != ENL_RESULT_SUCCESS)
	{
		DLOG_Error("meter device create error!:%ld", ret);
		isErr = TRUE;
	} else
	{
		if (i == 0u)
		{
			g_eenlDevMeterE1 = *devMeterInfo;
			DLOG_Debug("meter device created.(instance:0x%08lX)",
				(ulong)g_eenlDevMeterE1);
		} else if (i == 1u)
		{
			g_eenlDevMeterC2 = *devMeterInfo;
			DLOG_Debug("meter device created.(instance:0x%08lX)",
				(ulong)g_eenlDevMeterC2);
		} else
		{
			/* 処理継続 */
		}

		/* 機器オブジェクトスーパークラス 設定*/
		/* 0x80: 動作状態 */
		data = 0x31u;
		ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
			0x80u, 1u, &data);
		if (ret != ENL_RESULT_SUCCESS)
		{
			DLOG_Error("meter 0x80 init error.:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 0x81: 設置場所 */
			data = 0x00u;
			ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
				0x81u, 1u, &data);
			if (ret != ENL_RESULT_SUCCESS)
			{
				DLOG_Error("meter 0x81 init error!:%ld", ret);
				isErr = TRUE;
			} else
			{
				/* 前回の設置場所情報を反映 */
				/* 前回の設置場所情報は、*/
				/* eenl_fileRead()であらかじめグローバル変数に格納済み */
				DLOG_Debug("0x81 load value : 0x%02x", s_eenlMeter0x81);
				ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
					0x81u, 1u, &s_eenlMeter0x81);
				if (ret != ENL_RESULT_SUCCESS)
				{
					DLOG_Debug("meter 0x81 load init fail(%02x) : %ld",
						s_eenlMeter0x81, ret);
					/* 前回の情報が反映できなくても初期値で */
					/* 動作可能なので処理続行 */
				} else
				{
					/* 処理継続 */
					/* 状変アナウンスは無し */
				}

				/* 0x82: 規格バージョン AppendixI  （ライブラリ自動生成）*/

				/* 0x83: 識別番号 */
				ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
					0x83u, 17u, (uchar*)meterId);
				if (ret != ENL_RESULT_SUCCESS)
				{
					DLOG_Error("meter 0x83 init error!:%ld", ret);
					isErr = TRUE;
				} else
				{
					/* 0x86: メーカ異常コード */
					ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
						0x86u, 4u, (uchar*)merr);
					if (ret != ENL_RESULT_SUCCESS)
					{
						DLOG_Error("meter 0x86 init error!:%ld", ret);
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
		data = 0x42u;	/* 異常発生無し */
		ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
			0x88u, 1u, &data);
		if (ret != ENL_RESULT_SUCCESS)
		{
			DLOG_Error("meter 0x88 init error!:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 状変アナウンスは無し */
			/* 0x8A: メーカコード */
			ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
				0x8Au, 3u,
				(uchar*)g_eenlCodeNcc);
			if (ret != ENL_RESULT_SUCCESS)
			{
				DLOG_Error("meter 0x8A init error!:%ld", ret);
				isErr = TRUE;
			} else
			{
				/* 0x8C: 商品コード */
				ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
					0x8Cu, 12u,
					(uchar*)space12);
				if (ret != ENL_RESULT_SUCCESS)
				{
					DLOG_Error("meter 0x8C init error!:%ld", ret);
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
		/* 0x8D: 製造番号 */
		ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
			0x8Du, 12u, (uchar*)space12);
		if (ret != ENL_RESULT_SUCCESS)
		{
			DLOG_Error("meter 0x8D init error!:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 0x93: 遠隔操作設定 */
			data = EENL_REMOTE_OFF;	/* 公衆回線未経由 */
			ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
				0x93u, 1u, &data);
			if (ret != ENL_RESULT_SUCCESS)
			{
				DLOG_Error("meter 0x93 init error!:%ld", ret);
				isErr = TRUE;
			} else
			{
				/* 0x97: 現在時刻設定 00:00 */
				ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
					0x97u, 2u, (uchar*)zero2b);
				if (ret != ENL_RESULT_SUCCESS)
				{
					DLOG_Error("meter 0x97 init error!:%ld", ret);
					isErr = TRUE;
				} else
				{
					/* 0x98: 現在年月日設定 2014/1/1 */
					ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
						0x98u, 4u, (uchar*)nowDate);
					if (ret != ENL_RESULT_SUCCESS)
					{
						DLOG_Error("meter 0x98 init error!:%ld", ret);
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

	/* 0xD0: 機器種別 */
	if (isErr == FALSE)
	{
		ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
			0xD0u, 3u, zero3b);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Error("meter 0xD0 init error.:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 処理継続 */
		}
	} else
	{
		/* 処理継続 */
	}

	/* 0xD1: 機器ID */
	if (isErr == FALSE)
	{
		ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
			0xD1u, 7u, zero7b);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Error("meter 0xD1 init error.:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 処理継続 */
		}
	} else
	{
		/* 処理継続 */
	}

	/* 0xD2: 公差階級 */
	if (isErr == FALSE)
	{
		data = 0x03u;
		ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
			0xD2u, 1u, &data);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Error("meter 0xD2 init error.:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 処理継続 */
		}
	} else
	{
		/* 処理継続 */
	}

	/* 0xD3: 積算電力量計測値履歴保持日数 */
	if (isErr == FALSE)
	{
		initShort = 0x0063u;
		ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
			0xD3u, 2u, (uchar*)&initShort);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Error("meter 0xD3 init error.:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 処理継続 */
		}
	} else
	{
		/* 処理継続 */
	}

	/* 0xD4: 積算電力量単位 */
	if (isErr == FALSE)
	{
		data = 0x03u;
		ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
			0xD4u, 1u, &data);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Error("meter 0xD4 init error.:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 処理継続 */
		}
	} else
	{
		/* 処理継続 */
	}

	/* 0xD5: 積算履歴収集日 */
	if (isErr == FALSE)
	{
		initShort = 0xFFFFu;
		ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
			0xD5u, 2u, (uchar*)&initShort);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Error("meter 0xD5 init error.:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 処理継続 */
		}
	} else
	{
		/* 処理継続 */
	}

	/* 0xDA: 現在時分秒設定 */
	if (isErr == FALSE)
	{
		ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
			0xDAu, 3u, zero3b);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Error("meter 0xDA init error.:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 処理継続 */
		}
	} else
	{
		/* 処理継続 */
	}

	/* 0xDB: 時刻同期状態 */
	if (isErr == FALSE)
	{
		data = 0x00u;
		ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
			0xDBu, 1u, &data);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Error("meter 0xDB init error.:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 処理継続 */
		}
	} else
	{
		/* 処理継続 */
	}

	/* 0xE0: 積算電力量計測値(交流入力) */
	if (isErr == FALSE)
	{
		ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
			0xE0u, 4u, (uchar*)&initLong);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Error("meter 0xE0 init error.:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 処理継続 */
		}
	} else
	{
		/* 処理継続 */
	}

	/* 0xE1: 積算電力量計測値履歴(交流入力) */
	if (isErr == FALSE)
	{
		memcpy(initUsedEnergy, (uchar*)&usedEnergyCollectDate, 2);
		memcpy(initUsedEnergy + 2, (uchar*)usedEnergyCollectValue, 192);
		ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
			0xE1u, 194u, initUsedEnergy);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Error("meter 0xE1 init error.:%ld", ret);
			for (cnt = 0u; cnt < TOTAL_COLLECT_DATE; cnt++)
			{
				memcpy(s_ttlPcsChargeEnergyHistoryList[cnt],
					(uchar*)usedEnergyCollectValue, 192);
				memcpy(s_ttlV2hChargeEnergyHistoryList[cnt],
					(uchar*)usedEnergyCollectValue, 192);
			}
			isErr = TRUE;
		} else
		{
			/* 処理継続 */
		}
	} else
	{
		/* 処理継続 */
	}

	/* 0xE2: 積算電力量計測値(交流出力) */
	if (isErr == FALSE)
	{
		ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
			0xE2u, 4u, (uchar*)&initLong);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Error("meter 0xE2 init error.:%ld", ret);
			for (cnt = 0u; cnt < TOTAL_COLLECT_DATE; cnt++)
			{
				memcpy(s_ttlPcsDischargeEnergyHistoryList[cnt],
					(uchar*)usedEnergyCollectValue, 192);
				memcpy(s_ttlV2hDischargeEnergyHistoryList[cnt],
					(uchar*)usedEnergyCollectValue, 192);
			}
			isErr = TRUE;
		} else
		{
			/* 処理継続 */
		}
	} else
	{
		/* 処理継続 */
	}

	/* 0xE3: 積算電力量計測値履歴(交流出力) */
	if (isErr == FALSE)
	{
		ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
			0xE3u, 194u, initUsedEnergy);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Error("meter 0xE3 init error.:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 処理継続 */
		}
	} else
	{
		/* 処理継続 */
	}

	/* 0xE6: 定時積算電力量計測値(交流入力) */
	if (isErr == FALSE)
	{
		timerUsedEnergyYear = (ushort)EENL_BYTE_SWAP_16(timerUsedEnergyYear);
		timerUsedEnergyValue = (ulong)EENL_BYTE_SWAP_32(timerUsedEnergyValue);
		memcpy(initTimerUsedEnergy, (uchar*)&timerUsedEnergyYear, 2);
		memcpy(initTimerUsedEnergy + 2, timerUsedEnergyDate, 2);
		memcpy(initTimerUsedEnergy + 4, timerUsedEnergyTime, 2);
		memcpy(initTimerUsedEnergy + 7, (uchar*)timerUsedEnergyValue, 4);
		ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
			0xE6u, 11u, initTimerUsedEnergy);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Error("meter 0xE6 init error.:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 処理継続 */
		}
	} else
	{
		/* 処理継続 */
	}

	/* 0xE7: 定時積算電力量計測値(交流出力) */
	if (isErr == FALSE)
	{
		ret = EENL_ExtEnlDevSetProperty(NULL, *devMeterInfo,
			0xE7u, 11u, initTimerUsedEnergy);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Error("meter 0xE7 init error.:%ld", ret);
			isErr = TRUE;
		} else
		{
			/* 処理継続 */
		}
	} else
	{
		/* 処理継続 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:epcsデータの反映（分散型電源電力量メータ）				*/
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
/*	history			:2023/04/19 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_UpdateDeviceMeter(EenlDeviceInfo_t* info)
{
	slong ret = 0;
	Temp_t meterSet;

	DEBUG_TRCIN();

	if (info == NULL)
	{
		DLOG_Error("get meter info error: NULL");
	} else if (info->devMeterE1 == NULL)
	{
		/* meterクラスなし */
		DLOG_Debug("-----[meter] update pass-----");
		return 0;
	} else
	{
		/* 処理継続 */
	}

	memset(&meterSet, 0x00, sizeof(Temp_t));

	/* INVからmeter設定情報を取得 INVかた新規*/
	/*ret = INV_XXX(&meterSet);*/

	if (ret != EPCS_OK)
	{
		DLOG_Error("get meter Set Info error! : %ld", ret);
	} else
	{
		/* 0x80 : 動作状態 →状態変化通知にて設定するため、ここでは設定しない。*/

		/* 0x83 : 識別番号（分散型電源電力量メータ） */
		/*ret = EENL_DevUpdate0x83(info, meterSet.XXX, devType);*/

		/* 0x8C : 商品コード（分散型電源電力量メータ） */
		/*ret = EENL_DevUpdate0x8C(info, meterSet.XXX, devType);*/

		/* 0x8D : 製造番号（分散型電源電力量メータ） */
		/*ret = EENL_DevUpdate0x8D(info, meterSet.XXX, devType);*/

		/* 0xD0 : 機器種別 */
		ret = eenl_E1MeterUpdate0xD0(info, &meterSet);
		ret = eenl_C2MeterUpdate0xD0(info, &meterSet);

		/* 0xD1 : 機器ID */
		ret = eenl_E1MeterUpdate0xD1(info, &meterSet);
		ret = eenl_C2MeterUpdate0xD1(info, &meterSet);

		/* 0xD2 : 公差階級 */
		ret = eenl_E1MeterUpdate0xD2(info, &meterSet);
		ret = eenl_C2MeterUpdate0xD2(info, &meterSet);

		/* 0xD3 : 積算電力量計測値履歴保持日数 */
		ret = eenl_E1MeterUpdate0xD3(info, &meterSet);
		ret = eenl_C2MeterUpdate0xD3(info, &meterSet);

		/* 0xD4 : 積算電力量単位 */
		ret = eenl_E1MeterUpdate0xD4(info, &meterSet);
		ret = eenl_C2MeterUpdate0xD4(info, &meterSet);

		/* 0xD5 : 積算履歴収集日 */
		ret = eenl_E1MeterUpdate0xD5(info, &meterSet);
		ret = eenl_C2MeterUpdate0xD5(info, &meterSet);

		/* 0xDB: 時刻同期状態 */
		ret = eenl_E1MeterUpdate0xDB(info, &meterSet);
		ret = eenl_C2MeterUpdate0xDB(info, &meterSet);
	}

	DEBUG_TRCOUT();
	return 0;
}



/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス 0xDA						*/
/*					:現在時分秒設定(E1)										*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_E1MeterTimer0xDA(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	uchar val0xDA[5] = { 0x00u, ':', 0x00u, ':', 0x00u };
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */
	/* For build Start */
	DLOG_Debug("For build: %s", (char*)ope);
	/* For build End */
	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devMeterE1))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		/*TODO:CMD81から、0xE2を取得する。*/
		/*memcpy(val0xE2, &(ope->currentTime[3]), 1);*/
		/*memcpy(val0xE2 + 2, &(ope->currentTime[4]), 1);*/
		/*memcpy(val0xE2 + 4, &(ope->currentTime[5]), 1);*/

		ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterE1,
			0xDAu, 5u, val0xDA);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Debug(" [timer meter E1] 0xDA success: %ld", ret);
			/* 状変アナウンスはなし */
		} else
		{
			DLOG_Error(" [timer meter E1] 0xDA error:%ld", ret);
			/* 異常でも処理続行 */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス 0xDA						*/
/*					:現在時分秒設定(C2)										*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_C2MeterTimer0xDA(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	uchar val0xDA[5] = { 0x00u, ':', 0x00u, ':', 0x00u };
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */
	/* For build Start */
	DLOG_Debug("For build: %s", (char*)ope);
	/* For build End */
	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devMeterC2))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		/*TODO:CMD81から、0xE2を取得する。*/
		/*memcpy(val0xE2, &(ope->currentTime[3]), 1);*/
		/*memcpy(val0xE2 + 2, &(ope->currentTime[4]), 1);*/
		/*memcpy(val0xE2 + 4, &(ope->currentTime[5]), 1);*/

		ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterC2,
			0xDAu, 5u, val0xDA);
		if (ret == ENL_RESULT_SUCCESS)
		{
			DLOG_Debug(" [timer meter C2] 0xDA success: %ld", ret);
			/* 状変アナウンスはなし */
		} else
		{
			DLOG_Error(" [timer meter C2] 0xDA error:%ld", ret);
			/* 異常でも処理続行 */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}


/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(E1) 0xE0 0xE1 0xE6		*/
/*					:積算電力量計測値(交流入力)								*/
/*					:積算電力量計測値履歴(交流入力)							*/
/*					:定時積算電力量計測値(交流入力)							*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/

slong EENL_E1MeterTimer0xE0_0xE1_0xE6(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong val0xE0 = 0u;
	uchar val0xE1[EINV_METER_ENERGER_HISTORY_LEN] = { 0x00u };
	uchar val0xE6[EINV_METER_ENERGER_LEN] = { 0x00u };
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devMeterE1))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		/* 範囲 0x000000000～0x3B9AC9FF(0～999,999,999) */
		/* epcsでECHONET規格の最大値超過がありえる */
		/* オーバーフロー時に0から再インクリメントとなるように */
		/* 補正する必要があるため、*/
		/* 常に規格上の最大値+1で剰余算する */
		val0xE0 = ope->pcsTtlChargeEnergy % EENL_LIMIT_1000000000;

		/* 分散型電源電力量メータ0xE0 */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devMeterE1,
			0xE0u, val0xE0, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/*成功 */
			DLOG_Debug("E1 meter 0xE0 set success : 0x%lx(%ld)", 
				val0xE0, val0xE0);
			/* 状変アナウンスは無し */
		} else
		{
			/* 処理継続 */
		}

		/* 分散型電源電力量メータ0xE1 */
		memcpy(val0xE1, ope->pcsTtlChargeEnergyHistory, 
			EINV_METER_ENERGER_HISTORY_LEN);
		ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterE1, 
			0xE1u, EINV_METER_ENERGER_HISTORY_LEN, val0xE1);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/*成功 */
			DLOG_Debug("E1 meter 0xE1 set success : 0x%s",
				val0xE1);
			/* 状変アナウンスは無し */
		} else
		{
			/* 失敗 */
			DLOG_Debug("E1 meter 0xE1 set fail ret: %ld", ret);
			/* 処理継続 */
		}

		/* 分散型電源電力量メータ0xE6 */
		memcpy(val0xE6, ope->pcsTtlTimeChargeEnergy,
			EINV_METER_ENERGER_HISTORY_LEN);
		ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterE1,
			0xE6u, EINV_METER_ENERGER_LEN, val0xE6);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/*成功 */
			DLOG_Debug("E1 meter 0xE6 set success : 0x%s",
				val0xE1);
			/* 状変アナウンスは無し */
		} else
		{
			/* 失敗 */
			DLOG_Debug("E1 meter 0xE6 set fail ret: %ld", ret);
			/* 処理継続 */
		}
	}

	DEBUG_TRCOUT();
	return ENL_RESULT_SUCCESS;
}

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(E1) 0xE0 0xE1 0xE6		*/
/*					:積算電力量計測値(交流入力)								*/
/*					:積算電力量計測値履歴(交流入力)							*/
/*					:定時積算電力量計測値(交流入力)							*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/

slong EENL_C2MeterTimer0xE0_0xE1_0xE6(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong val0xE0 = 0u;
	uchar val0xE1[EINV_METER_ENERGER_HISTORY_LEN] = { 0x00u };
	uchar val0xE6[EINV_METER_ENERGER_LEN] = { 0x00u };
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devMeterC2))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		/* 範囲 0x000000000～0x3B9AC9FF(0～999,999,999) */
		/* epcsでECHONET規格の最大値超過がありえる */
		/* オーバーフロー時に0から再インクリメントとなるように */
		/* 補正する必要があるため、*/
		/* 常に規格上の最大値+1で剰余算する */
		val0xE0 = ope->v2hTtlChargeEnergy % EENL_LIMIT_1000000000;

		/* 分散型電源電力量メータ0xE0 */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devMeterC2,
			0xE0u, val0xE0, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/*成功 */
			DLOG_Debug("C2 meter 0xE0 set success : 0x%lx(%ld)",
				val0xE0, val0xE0);
			/* 状変アナウンスは無し */
		} else
		{
			/* 処理継続 */
		}

		/* 分散型電源電力量メータ0xE1 */
		memcpy(val0xE1, ope->v2hTtlChargeEnergyHistory,
			EINV_METER_ENERGER_HISTORY_LEN);
		ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterC2,
			0xE1u, EINV_METER_ENERGER_HISTORY_LEN, val0xE1);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/*成功 */
			DLOG_Debug("C2 meter 0xE1 set success : 0x%s",
				val0xE1);
			/* 状変アナウンスは無し */
		} else
		{
			/* 失敗 */
			DLOG_Debug("C2 meter 0xE1 set fail ret: %ld", ret);
			/* 処理継続 */
		}

		/* 分散型電源電力量メータ0xE6 */
		memcpy(val0xE6, ope->v2hTtlTimeChargeEnergy,
			EINV_METER_ENERGER_HISTORY_LEN);
		ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterC2,
			0xE6u, EINV_METER_ENERGER_LEN, val0xE6);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/*成功 */
			DLOG_Debug("C2 meter 0xE6 set success : 0x%s",
				val0xE1);
			/* 状変アナウンスは無し */
		} else
		{
			/* 失敗 */
			DLOG_Debug("C2 meter 0xE6 set fail ret: %ld", ret);
			/* 処理継続 */
		}
	}

	DEBUG_TRCOUT();
	return ENL_RESULT_SUCCESS;
}

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(E1) 0xE2 0xE3 0xE7		*/
/*					:積算電力量計測値(交流出力)								*/
/*					:積算電力量計測値履歴(交流出力)							*/
/*					:定時積算電力量計測値(交流出力)							*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_E1MeterTimer0xE2_0xE3_0xE7(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong val0xE2 = 0u;
	uchar val0xE3[EINV_METER_ENERGER_HISTORY_LEN] = { 0x00u };
	uchar val0xE7[EINV_METER_ENERGER_LEN] = { 0x00u };
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devMeterE1))
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		/* 範囲 0x000000000～0x3B9AC9FF(0～999,999,999) */
		/* epcsでECHONET規格の最大値超過がありえる */
		/* オーバーフロー時に0から再インクリメントとなるように */
		/* 補正する必要があるため、*/
		/* 常に規格上の最大値+1で剰余算する */
		val0xE2 = ope->pcsTtlDischargeEnergy % EENL_LIMIT_1000000000;

		/* 分散型電源電力量メータ0xE2 */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devMeterE1,
			0xE2u, val0xE2, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/*成功 */
			DLOG_Debug("E1 meter 0xE2 set success : 0x%lx(%ld)",
				val0xE2, val0xE2);
			/* 状変アナウンスは無し */
		}
		else
		{
			/* 処理継続 */
		}

		/* 分散型電源電力量メータ0xE3 */
		memcpy(val0xE3, ope->pcsTtlDischargeEnergyHistory,
			EINV_METER_ENERGER_HISTORY_LEN);
		ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterE1,
			0xE3u, EINV_METER_ENERGER_HISTORY_LEN, val0xE3);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/*成功 */
			DLOG_Debug("E1 meter 0xE3 set success : 0x%s",
				val0xE3);
			/* 状変アナウンスは無し */
		}
		else
		{
			/* 失敗 */
			DLOG_Debug("E1 meter 0xE3 set fail ret: %ld", ret);
			/* 処理継続 */
		}

		/* 分散型電源電力量メータ0xE7 */
		memcpy(val0xE7, ope->pcsTtlTimeDischargeEnergy,
			EINV_METER_ENERGER_HISTORY_LEN);
		ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterE1,
			0xE7u, EINV_METER_ENERGER_LEN, val0xE7);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/*成功 */
			DLOG_Debug("E1 meter 0xE7 set success : 0x%s",
				val0xE7);
			/* 状変アナウンスは無し */
		}
		else
		{
			/* 失敗 */
			DLOG_Debug("E1 meter 0xE7 set fail ret: %ld", ret);
			/* 処理継続 */
		}
	}

	DEBUG_TRCOUT();
	return ENL_RESULT_SUCCESS;
}

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(C2) 0xE2 0xE3 0xE7		*/
/*					:積算電力量計測値(交流出力)								*/
/*					:積算電力量計測値履歴(交流出力)							*/
/*					:定時積算電力量計測値(交流出力)							*/
/*--------------------------------------------------------------------------*/
/*	param			:const EPCS_OPESTTS_INFO_T*	ope 動作情報構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
slong EENL_C2MeterTimer0xE2_0xE3_0xE7(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong val0xE2 = 0u;
	uchar val0xE3[EINV_METER_ENERGER_HISTORY_LEN] = { 0x00u };
	uchar val0xE7[EINV_METER_ENERGER_LEN] = { 0x00u };
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (EENL_NULL_CHECK(info, info->devMeterC2))
	{
		ret = ENL_RESULT_SUCCESS;
	}
	else
	{
		/* 範囲 0x000000000～0x3B9AC9FF(0～999,999,999) */
		/* epcsでECHONET規格の最大値超過がありえる */
		/* オーバーフロー時に0から再インクリメントとなるように */
		/* 補正する必要があるため、*/
		/* 常に規格上の最大値+1で剰余算する */
		val0xE2 = ope->v2hTtlDischargeEnergy % EENL_LIMIT_1000000000;

		/* 分散型電源電力量メータ0xE2 */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devMeterC2,
			0xE2u, val0xE2, 0u);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/*成功 */
			DLOG_Debug("C2 meter 0xE2 set success : 0x%lx(%ld)",
				val0xE2, val0xE2);
			/* 状変アナウンスは無し */
		}
		else
		{
			/* 処理継続 */
		}

		/* 分散型電源電力量メータ0xE3 */
		memcpy(val0xE3, ope->v2hTtlDischargeEnergyHistory,
			EINV_METER_ENERGER_HISTORY_LEN);
		ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterC2,
			0xE3u, EINV_METER_ENERGER_HISTORY_LEN, val0xE3);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/*成功 */
			DLOG_Debug("C2 meter 0xE3 set success : 0x%s",
				val0xE3);
			/* 状変アナウンスは無し */
		}
		else
		{
			/* 失敗 */
			DLOG_Debug("C2 meter 0xE3 set fail ret: %ld", ret);
			/* 処理継続 */
		}

		/* 分散型電源電力量メータ0xE7 */
		memcpy(val0xE7, ope->v2hTtlTimeDischargeEnergy,
			EINV_METER_ENERGER_HISTORY_LEN);
		ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterC2,
			0xE7u, EINV_METER_ENERGER_LEN, val0xE7);
		if (ret == ENL_RESULT_SUCCESS)
		{
			/*成功 */
			DLOG_Debug("C2 meter 0xE7 set success : 0x%s",
				val0xE7);
			/* 状変アナウンスは無し */
		}
		else
		{
			/* 失敗 */
			DLOG_Debug("C2 meter 0xE7 set fail ret: %ld", ret);
			/* 処理継続 */
		}
	}

	DEBUG_TRCOUT();
	return ENL_RESULT_SUCCESS;
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(0xD0)更新処理(E1)			*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:処理なし/0以外:ENLライブラリの戻り値)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_E1MeterUpdate0xD0(EenlDeviceInfo_t* info, Temp_t* meterSet)
{
	slong ret = 0;
	uchar value[3] = { 0x00u };

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */
	/* TODO: 機器種別(0xDO) を取得する。*/
	/*memcpy(value, meterSet.XXX, 3);*/
	/* For build Start */
	DLOG_Debug("For build: %s", (char*)meterSet);
	/* For build End */

	ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterE1, 0xD0u, 3u, value);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [meter E1] 0xD0 update success:%ld", ret);
	} else
	{
		DLOG_Error("[meter E1] 0xD0 update error:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(0xD0)更新処理(C2)			*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:処理なし/0以外:ENLライブラリの戻り値)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_C2MeterUpdate0xD0(EenlDeviceInfo_t* info, Temp_t* meterSet)
{
	slong ret = 0;
	uchar value[3] = { 0x00u };

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */
	/* TODO: 機器種別(0xDO) を取得する。*/
	/*memcpy(value, meterSet.XXX, 3);*/
	/* For build Start */
	DLOG_Debug("For build: %s", (char*)meterSet);
	/* For build End */

	ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterC2, 0xD0u, 3u, value);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [meter C2] 0xD0 update success:%ld", ret);
	} else
	{
		DLOG_Error("[meter C2] 0xD0 update error:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(0xD1)更新処理(E1)			*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:処理なし/0以外:ENLライブラリの戻り値)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_E1MeterUpdate0xD1(EenlDeviceInfo_t* info, Temp_t* meterSet)
{
	slong ret = 0;
	uchar value[7] = { 0x00u };

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */
	/* TODO: 機器ID(0xD1) を取得する。*/
	/*memcpy(value, meterSet.XXX, 7);*/
	/* For build Start */
	DLOG_Debug("For build: %s", (char*)meterSet);
	/* For build End */
	ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterE1, 0xD1u, 7u, value);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [meter E1] 0xD1 update success:%ld", ret);
	} else
	{
		DLOG_Error("[meter E1] 0xD1 update error:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(0xD1)更新処理(C2)			*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:処理なし/0以外:ENLライブラリの戻り値)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_C2MeterUpdate0xD1(EenlDeviceInfo_t* info, Temp_t* meterSet)
{
	slong ret = 0;
	uchar value[7] = { 0x00u };

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */
	/* TODO: 機器ID(0xD1) を取得する。*/
	/*memcpy(value, meterSet.XXX, 7);*/
	/* For build Start */
	DLOG_Debug("For build: %s", (char*)meterSet);
	/* For build End */
	ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterC2, 0xD1u, 7u, value);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [meter C2] 0xD1 update success:%ld", ret);
	} else
	{
		DLOG_Error("[meter C2] 0xD1 update error:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(0xD2)更新処理(E1)			*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:処理なし/0以外:ENLライブラリの戻り値)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_E1MeterUpdate0xD2(EenlDeviceInfo_t* info, Temp_t* meterSet)
{
	slong ret = 0;
	uchar value = 0x00u;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */
	/* TODO: 公差階級(0xD2) を取得する。*/
	/*memcpy(value, meterSet.XXX, 1);*/
	/* For build Start */
	DLOG_Debug("For build: %s", (char*)meterSet);
	/* For build End */
	ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterE1, 0xD2u, 1u, &value);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [meter E1] 0xD2 update success:%ld", ret);
	} else
	{
		DLOG_Error("[meter E1] 0xD2 update error:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(0xD2)更新処理(C2)			*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:処理なし/0以外:ENLライブラリの戻り値)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_C2MeterUpdate0xD2(EenlDeviceInfo_t* info, Temp_t* meterSet)
{
	slong ret = 0;
	uchar value = 0x00u;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */
	/* TODO: 公差階級(0xD2) を取得する。*/
	/*memcpy(value, meterSet.XXX, 1);*/
	/* For build Start */
	DLOG_Debug("For build: %s", (char*)meterSet);
	/* For build End */
	ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterC2, 0xD2u, 1u, &value);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [meter C2] 0xD2 update success:%ld", ret);
	} else
	{
		DLOG_Error("[meter C2] 0xD2 update error:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(0xD3)更新処理(E1)			*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:処理なし/0以外:ENLライブラリの戻り値)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_E1MeterUpdate0xD3(EenlDeviceInfo_t* info, Temp_t* meterSet)
{
	slong ret = 0;
	ushort value = 0x0063u;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */
	/* TODO: 積算電力量計測値履歴保持日数(0xD3) を取得する。*/
	/* For build Start */
	DLOG_Debug("For build: %s", (char*)meterSet);
	/* For build End */
	value = (ushort)EENL_BYTE_SWAP_16(value);

	ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterE1,
		0xD3u, 2u, (uchar*)&value);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [meter E1] 0xD3 update success:%ld", ret);
	} else
	{
		DLOG_Error("[meter E1] 0xD3 update error:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(0xD3)更新処理(C2)			*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:処理なし/0以外:ENLライブラリの戻り値)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_C2MeterUpdate0xD3(EenlDeviceInfo_t* info, Temp_t* meterSet)
{
	slong ret = 0;
	ushort value = 0x0063u;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */
	/* TODO: 積算電力量計測値履歴保持日数(0xD3) を取得する。*/
	/* For build Start */
	DLOG_Debug("For build: %s", (char*)meterSet);
	/* For build End */
	value = (ushort)EENL_BYTE_SWAP_16(value);

	ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterC2,
		0xD3u, 2u, (uchar*)&value);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [meter C2] 0xD3 update success:%ld", ret);
	} else
	{
		DLOG_Error("[meter C2] 0xD3 update error:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(0xD4)更新処理(E1)			*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:処理なし/0以外:ENLライブラリの戻り値)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_E1MeterUpdate0xD4(EenlDeviceInfo_t* info, Temp_t* meterSet)
{
	slong ret = 0;
	uchar value = 0x00u;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */
	/* TODO: 積算電力量単位(0xD4) を取得する。*/
	/*memcpy(value, meterSet.XXX, 1);*/
	/* For build Start */
	DLOG_Debug("For build: %s", (char*)meterSet);
	/* For build End */
	ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterE1, 0xD4u, 1u, &value);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [meter E1] 0xD4 update success:%ld", ret);
	} else
	{
		DLOG_Error("[meter E1] 0xD4 update error:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(0xD4)更新処理(C2)			*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:処理なし/0以外:ENLライブラリの戻り値)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_C2MeterUpdate0xD4(EenlDeviceInfo_t* info, Temp_t* meterSet)
{
	slong ret = 0;
	uchar value = 0x00u;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */
	/* TODO: 積算電力量単位(0xD4) を取得する。*/
	/*memcpy(value, meterSet.XXX, 1);*/
	/* For build Start */
	DLOG_Debug("For build: %s", (char*)meterSet);
	/* For build End */
	ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterC2, 0xD4u, 1u, &value);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [meter C2] 0xD4 update success:%ld", ret);
	} else
	{
		DLOG_Error("[meter C2] 0xD4 update error:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(0xD5)更新処理(E1)			*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:処理なし/0以外:ENLライブラリの戻り値)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_E1MeterUpdate0xD5(EenlDeviceInfo_t* info, Temp_t* meterSet)
{
	slong ret = 0;
	ushort value = 0xFFFFu;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */
	/* TODO: 積算履歴収集日(0xD5) を取得する。*/
	/*value = meterSet.XXX;*/
	/* For build Start */
	DLOG_Debug("For build: %s", (char*)meterSet);
	/* For build End */
	if (value > 0x0063u)
	{
		value = 0x0063u;
	} else
	{
		/* 処理続行 */
	}

	value = (ushort)EENL_BYTE_SWAP_16(value);
	ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterE1,
		0xD5u, 2u, (uchar*)&value);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [meter E1] 0xD5 update success:%ld", ret);
	} else
	{
		DLOG_Error("[meter E1] 0xD5 update error:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(0xD5)更新処理(C2)			*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:処理なし/0以外:ENLライブラリの戻り値)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_C2MeterUpdate0xD5(EenlDeviceInfo_t* info, Temp_t* meterSet)
{
	slong ret = 0;
	ushort value = 0xFFFFu;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */
	/* TODO: 積算履歴収集日(0xD5) を取得する。*/
	/*value = meterSet.XXX;*/
	/* For build Start */
	DLOG_Debug("For build: %s", (char*)meterSet);
	/* For build End */
	if (value > 0x0063u)
	{
		value = 0x0063u;
	} else
	{
		/* 処理続行 */
	}

	value = (ushort)EENL_BYTE_SWAP_16(value);
	ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterC2,
		0xD5u, 2u, (uchar*)&value);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [meter C2] 0xD5 update success:%ld", ret);
	} else
	{
		DLOG_Error("[meter C2] 0xD5 update error:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(0xDB)更新処理(E1)			*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:処理なし/0以外:ENLライブラリの戻り値)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_E1MeterUpdate0xDB(EenlDeviceInfo_t* info, Temp_t* meterSet)
{
	slong ret = 0;
	uchar value = 0x00;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */
	/* TODO: 時刻同期状態(0xDB) を取得する。*/
	/*memcpy(value, meterSet.XXX, 1);*/
	/* For build Start */
	DLOG_Debug("For build: %s", (char*)meterSet);
	/* For build End */
	ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterE1, 0xDBu, 1u, &value);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [meter E1] 0xDB update success:%ld", ret);
	} else
	{
		DLOG_Error("[meter E1] 0xDB update error:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:分散型電源電力量メータクラス(0xDB)更新処理(C2)			*/
/*--------------------------------------------------------------------------*/
/*	param			:EenlDeviceInfo_t* info									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:処理なし/0以外:ENLライブラリの戻り値)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC K.Jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_C2MeterUpdate0xDB(EenlDeviceInfo_t* info, Temp_t* meterSet)
{
	slong ret = 0;
	uchar value = 0x00u;

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */
	/* TODO: 時刻同期状態(0xDB) を取得する。*/
	/*memcpy(value, meterSet.XXX, 1);*/
	/* For build Start */
	DLOG_Debug("For build: %s", (char*)meterSet);
	/* For build End */
	ret = EENL_ExtEnlDevSetProperty(NULL, info->devMeterC2, 0xDBu, 1u, &value);
	if (ret == ENL_RESULT_SUCCESS)
	{
		DLOG_Debug(" [meter C2] 0xDB update success:%ld", ret);
	} else
	{
		DLOG_Error("[meter C2] 0xDB update error:%ld", ret);
		/* 処理続行 */
	}

	DEBUG_TRCOUT();
	return ret;
}
