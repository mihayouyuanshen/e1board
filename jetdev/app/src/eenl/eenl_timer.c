/****************************************************************************/
/*	Description		:ECHONET Lite application for device usage				*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC K.jiang											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/25												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、EENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:無し													*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/
#include "eenl_cmn_g.h"
#include "eenl_timer.h"

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
/*	Description		:定期処理用スレッド										*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:ニチコン向けH1HEMSエンハンス対応						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC K.jiang : 新規作成					*/
/*==========================================================================*/
void EENL_Timer(void)
{
	EenlDeviceInfo_t* info = NULL;		/* device informationを格納 */
	EtimTm_t pTime;						/* 時刻設定・取得データ情報を格納 */
	EPCS_PV_CLASS_T	pvClassInfo;		/* 太陽光発電クラス情報を格納 */
	EPCS_OPESTTS_INFO_T ope;			/* 動作情報を格納 */
	EPCS_SERIAL_NUM_T pcsSerial;		/* PCS機種名・製造番号情報を格納 */
	ENOT_ERRST_T errst;					/* エラー発生状態(リモコン内部エラー) */
	slong retGetOpeSttsInfo = EPCS_OK;	/* getOpeSttsInfoの関数戻り値 */
	slong r = 0;						/* epcs,eschのI/F呼び出し結果格納 */
	slong ret = 0;						/* getPcsModelSerialNumberの関数戻り値 */
	slong retGetSolarInf = 0;			/* getPvClassInfoの関数戻り値 */
	schar hhmm[2] = { 0, 0 };			/* 現在時刻 */
	schar yyyymmdd[4] = { 0, 0, 0, 0 };	/* 現在日付 */
	uchar unitAddr = 1u;				/* ユニットアドレス */
	ulong cntPcsErrGetOpeSttsInfo = 0u;	/* epcs提供I/Fからのエラー応答回数(epcs_sync_getOpeSttsInfo) */
	ulong cntPcsErrGetPvClassInfo = 0u;	/* epcs提供I/Fからのエラー応答回数(epcs_sync_getPvClassInfo) */
	uchar seri[17];
	/* schar yyyymmdd_temp[4] = {0, 0, 0, 0}; */
	/* ESCH_HEMS_OPEPLAN_T hems_operationplan; */	/* ミドルの情報格納用(HEMS制御スケジュール) */
	/* ESCH_HEMS_ACTLOG_T hems_actlog; */			/* ミドルの情報格納用(HEMS制御動作履歴) */

	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);
	if (info != NULL)
	{
		while (1)
		{
			/* -------------------------------------------------------------------- */
			/* [etim]現在時刻 */
			memset(&pTime, 0x00, sizeof(pTime));

			/* etimから現在時刻を取得 */
			r = ETIM_GetTime(&pTime);
			if (r == 0)
			{
				/* 正常 */
				/* 現在時刻 */
				hhmm[0] = (schar)(pTime.tmHour);
				hhmm[1] = (schar)(pTime.tmMin);
				/* 現在年月日 */
				yyyymmdd[0] = (schar)((pTime.tmYear & 0xFF00) / 256);
				yyyymmdd[1] = (schar)(pTime.tmYear & 0x00FF);
				yyyymmdd[2] = (schar)(pTime.tmMon);
				yyyymmdd[3] = (schar)(pTime.tmMday);

				/* 0x97 : 現在時刻設定 */
				/* 0x98 : 現在年月日設定 */
				eenl_AllTimer0x97_0x98(yyyymmdd, hhmm);
			} else
			{
				/* etim異常 */
				/* 失敗しても処理続行 */
			}

			/* -------------------------------------------------------------------- */
			/* [epcs]機種名・製造番号取得 */
			memset(&pcsSerial, 0x00, sizeof(pcsSerial));
			ret = epcs_sync_getPcsModelSerialNumber(unitAddr, &pcsSerial);
			if (ret == EPCS_OK)
			{
				/* 製造番号 */
				memcpy(&seri[0], pcsSerial.pcsSerialNumber, 17);

				/* ----- ECHONET Liteへ設定 ----- */
				/* 0x83 : 識別番号（蓄電池、太陽光、マルチ入力PCS） */
				eenl_pcsTimer0x83(seri);

				/* 0x8D : 製造番号（蓄電池、太陽光、マルチ入力PCS） 2017/10/30 修正計画No.190対応 */
				eenl_pcsTimer0x8D(seri);
			} else
			{
				/* 製造番号取得に失敗したら、識別番号(0x83)と製造番号(0x8D)は更新しない */
				/* 処理に失敗しても処理続行 */
			}

			/* -------------------------------------------------------------------- */
			/* [epcs]動作状態 opeStts */
			memset(&ope, 0x00, sizeof(ope));
			memset(&errst, 0, sizeof(ENOT_ERRST_T));

			retGetOpeSttsInfo = epcs_sync_getOpeSttsInfo(&ope);

			/* メーカ異常コード0x86、異常発生状態0x88、異常内容0x89 */
			eenl_AllTimer0x86_0x88_0x89(retGetOpeSttsInfo, &ope, &errst);

			/* V2H設定情報 */
			EENL_V2hTimerSetInfo(info, &errst);

			if (retGetOpeSttsInfo == EPCS_OK)
			{

				/* 0xA0～0xA5, 0xA8, 0xA9, 0xEC */
				EENL_BtTimer0xA0_0xA1_0xA2_0xA3_0xA4_0xA5_0xA8_0xA9(&ope);

				/* 瞬時充放電電力計測値:0xD3 */
				EENL_BtTimer0xD3(&ope);

				/* 積算放電電力量計測値:0xD6（CMD21：積算放電電力量２（コンバータ部）） */
				EENL_BtTimer0xD6(&ope);

				/* 積算充電電力量計測値:0xD8（CMD21：積算充電電力量２（コンバータ部）） */
				EENL_BtTimer0xD8(&ope);

				/* 系統連系状態:蓄電池0xDB */
				/* 系統連系状態:太陽光0xD0 */
				/* 系統連系状態:マルチ入力PCS0xD0 */
				eenl_pcsTimer0xDB_0xD0(&ope);

				/* 0xE2 : 蓄電残量１　Wh */
				EENL_BtTimer0xE2(&ope);

				/* 0xE4 : 蓄電残量３　% */
				EENL_BtTimer0xE4(&ope);

				/* 0xE5 : 劣化状態(SOH) */
				EENL_BtTimer0xE5(&ope);

				/************************************************/
				/*	電気自動車充放電器クラスのプロパティ更新	*/
				/************************************************/

				/* 0xC2 : 車載電池の放電可能残容量 1 */
				EENL_V2hTimer0xC2(&ope);

				/* 0xC4 : 車載電池の放電可能残容量 3 */
				EENL_V2hTimer0xC4(&ope);

				/* 0xCF : 車載電池の充電可能残容量値 */
				EENL_V2hTimer0xCF(&ope);

				/* 0xD3 : 瞬時充放電電力計測値 */
				EENL_V2hTimer0xD3(&ope);

				/* 0xD6 : 積算放電電力量計測値 */
				EENL_V2hTimer0xD6(&ope);

				/* 0xD8 : 積算充電電力量計測値 */
				EENL_V2hTimer0xD8(&ope);

				/* 0xE2 : 車載電池の電池残容量 1 */
				EENL_V2hTimer0xE2(&ope);

				/* 0xE4 : 車載電池の電池残容量 3 */
				EENL_V2hTimer0xE4(&ope);

				/* 0xDB : 系統連系状態 */
				EENL_V2hTimer0xDB(&ope);

				/* 0xE1 : 運転動作状態 */
				EENL_V2hTimer0xE1(&ope);

				/* 0xCE : 車載電池の充電可能容量値 */
				EENL_V2hTimer0xCE(&ope);

				/* 0xD0 : 車載電池の使用容量値 1 */
				EENL_V2hTimer0xD0(&ope);

				/* 0xC0 : 車載電池の放電可能容量値１ */
				EENL_V2hTimer0xC0(&ope);

				/*******************************************/
				/*	マルチ入力PCSクラスのプロパティ更新	   */
				/*******************************************/

				/* 0xE0:積算電力量計測値(正方向) */
				EENL_MpcsTimer0xE0(&ope);

				/* 0xE3:積算電力量計測値(逆方向) */
				EENL_MpcsTimer0xE3(&ope);

				/* 0xE7:瞬時電力計測値 */
				EENL_MpcsTimer0xE7(&ope);

				/* 0xD0:系統連系状態 */
				EENL_MpcsTimer0xD0(&ope);

				/* 0xF6:積算電力量計測値 */
				EENL_MpcsTimer0xF6(&ope);

				/****************************************************/
				/*	分散型電源電力量メータクラスのプロパティ更新	*/
				/****************************************************/

				/* 0xDA:現在時分秒設定 */
				EENL_E1MeterTimer0xDA(&ope);

				/* 0xDA:現在時分秒設定 */
				EENL_C2MeterTimer0xDA(&ope);

				/* E1 meter */
				/* 0xE0:積算電力量計測値(交流入力) */
				/* 0xE1:積算電力量計測値履歴(交流入力) */
				/* 0xE6:定時積算電力量計測値(交流入力) */
				EENL_E1MeterTimer0xE0_0xE1_0xE6(&ope);

				/* E1 meter */
				/* 0xE2:積算電力量計測値(交流出力) */
				/* 0xE3:積算電力量計測値履歴(交流出力) */
				/* 0xE7:定時積算電力量計測値(交流出力) */
				EENL_E1MeterTimer0xE2_0xE3_0xE7(&ope);

				/* C2 meter */
				/* 0xE0:積算電力量計測値(交流入力) */
				/* 0xE1:積算電力量計測値履歴(交流入力) */
				/* 0xE6:定時積算電力量計測値(交流入力) */
				EENL_C2MeterTimer0xE0_0xE1_0xE6(&ope);

				/* C2 meter */
				/* 0xE2:積算電力量計測値(交流出力) */
				/* 0xE3:積算電力量計測値履歴(交流出力) */
				/* 0xE7:定時積算電力量計測値(交流出力) */
				EENL_C2MeterTimer0xE2_0xE3_0xE7(&ope);

				/* エラーカウントをクリア */
				cntPcsErrGetOpeSttsInfo = 0u;
			}
			/* 処理失敗 */
			else
			{
				/* 初回のエラー発生 */
				if (cntPcsErrGetOpeSttsInfo == 0u)
				{
					/* 処理無し */
				}
				/* 初回以外 */
				else
				{
					/* エラー継続回数の上限を超えた場合 */
					if (cntPcsErrGetOpeSttsInfo > EENL_LIMIT_PCSERR)
					{
						/* エラーカウントを0に戻す */
						cntPcsErrGetOpeSttsInfo = 0u;
					} else
					{
						/* 処理無し */
					}
				}
				cntPcsErrGetOpeSttsInfo++;
				/* 失敗しても処理続行 */
			}

			/*******************************************/
			/*		太陽光クラスのプロパティ更新	   */
			/*******************************************/
			if (info->devSolar != NULL)
			{
				/* epcsから太陽光関連の情報を取得★ */
				/*ｘ
				 * epcs整定値No取得(epcs_sync_getPvClassInfo(&pvClassInfo))で取得する
				 */
				retGetSolarInf = epcs_sync_getPvClassInfo(&pvClassInfo);

				if (retGetSolarInf == EPCS_OK)
				{
					/* 0xA0: 出力制御設定1 発電電力制限値(CMD21)[W] */
					EENL_SolarTimer0xA0(&pvClassInfo);

					/* 0xE0: 瞬時発電電力計測値[W]★太陽光発電電力(CMD21)[W] */
					EENL_SolarTimer0xE0(&pvClassInfo);

					/* 0xE1: 積算発電電力量計測値[0.001kWh]★積算太陽光発電電力量(CMD21)[Wh] */
					EENL_SolarTimer0xE1(&pvClassInfo);

					/* 0xE3: 積算売電電力量計測値★積算売電電力量(CMD21)[Wh] */
					EENL_SolarTimer0xE3(&pvClassInfo);

					/* 0xD0: 系統連系状態 */
					EENL_SolarTimer0xD0(&pvClassInfo);

					/* 0xD1: 出力抑制状態 */
					EENL_SolarTimer0xD1(&pvClassInfo);

					/* エラーカウントをクリア */
					cntPcsErrGetPvClassInfo = 0u;
				}
				/* 取得失敗 */
				else
				{
					/* 初回のエラー発生 */
					if (cntPcsErrGetPvClassInfo == 0u)
					{
						/* 処理無し */
					}
					/* 初回以外 */
					else
					{
						/* エラー継続回数の上限を超えた場合 */
						if (cntPcsErrGetPvClassInfo > EENL_LIMIT_PCSERR)
						{
							/* エラーカウントを0に戻す */
							cntPcsErrGetPvClassInfo = 0u;
						} else
						{
							/* 処理無し */
						}
					}
					cntPcsErrGetPvClassInfo++;
					/* 失敗しても処理続行 */
				}
				/* 0xF0: 発電状態通知   0xF0はイベント通知無し */
				/* 0xF1: 出力増減速度   0xF1はイベント通知無し */
			/* -------------------------------------------------------------------- */
			} /* devSolarが有るなら */
			else
			{
				/* 処理無し */
			}

			/* 0xF5: 瞬時電力計測値 */
			if (EPCS_OK == retGetOpeSttsInfo)
			{
				eenl_AllTimer0xF5(&ope);
			} else
			{
				/* 処理無し */
			}

			/* -------------------------------------------------------------------- */
			/* 設置場所	★★★ */
			eenl_AllTimer0x81();

			sleep(EENL_TIMER);

			/* Set要求受付可能にフラグ更新 */
			EENL_SetReqSetEnable();

		}	/* while */
	} else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();
}


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description		:蓄電池・太陽光・V2Hクラス 0x81の設定処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常発生有)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:構築なので状変アナウンス通知は無し						*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC K.jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_AllTimer0x81(void)
{
	slong ret = ENL_RESULT_SUCCESS;					/* 戻り値 */
	slong i = 0;									/* ループのカウンタ */
	ENL_MESSAGE* msg = NULL;						/* メッセージ */
	void* devList[EENL_DEV_LIST_MAX] = { NULL };	/* デバイスリスト */
	/* 0x81リスト */
	uchar place0x81[EENL_DEV_LIST_MAX] = { 0, g_eenlMpcs0x81,
											g_eenlSolar0x81,
											g_eenlBattery0x81,
											g_eenlV2h0x81 };

	DEBUG_TRCIN();

	/* デバイスリストの作成 */
	EENL_SetBroadcastDevList(devList);

	/* 設定しないデバイスにNULLを設定する */
	devList[EENL_DEV_LIST_NODE] = NULL;

	/*
	 *	EENL_BroadcastSetPropertyValueを使用したいところだが、
	 *	設置場所は各クラスで個別に持っているため、それぞれで設定する
	 */
	for (i = 0; i < EENL_DEV_LIST_MAX; i++)
	{
		if (devList[i] != NULL)
		{
			ret = EENL_ExtEnlDevSetPropertyValue(&msg, devList[i], 0x81u, place0x81[i], 0u);
			if (ret == ENL_RESULT_SUCCESS)
			{
				/* 状変アナウンス通知 */
				EENL_SendMessage(NULL, msg);
			} else
			{
				/* 処理なし */
			}
		} else
		{
			/* 処理なし */
		}
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池・太陽光・V2H・マルチ入力PCSクラス				*/
/*					:0x86/0x88/0x89の設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　getOpeSttsInfoRet								*/
/*									epcs_sync_getOpeSttsInfoの戻り値		*/
/*					:[in]　ope		動作情報構造体							*/
/*					:[in]　errst	リモコン内部エラー状態					*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常発生有)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC K.jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_AllTimer0x86_0x88_0x89(slong getOpeSttsInfoRet, 
	const EPCS_OPESTTS_INFO_T* ope, const ENOT_ERRST_T* errst)
{
	uchar val0x88 = 0xFFu;							/* 0x88設定値 */
	uchar val0x86[9] = { 0u };						/* 0x86設定値リスト */
	ushort val0x89 = 0x0000u;						/* 0x89設定値 */
	slong ret = ENL_RESULT_SUCCESS;					/* 戻り値 */
	void* devList[EENL_DEV_LIST_MAX] = { NULL };	/* デバイスリスト */

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* デバイスリストの作成 */
	EENL_SetBroadcastDevList(devList);

	/* ブロードキャストで設定しないデバイスにNULLを設定する */
	devList[EENL_DEV_LIST_NODE] = NULL;

	if (EPCS_OK == getOpeSttsInfoRet)
	{
		val0x88 = 0xFFu;
		val0x89 = 0xFFFFu;
		memset(&val0x86, 0x00, sizeof(val0x86));

		val0x86[0] = 5u;
		memcpy(&val0x86[1], &g_eenlCodeNcc[0], 3);

		/* BF550が発生している場合、最新のエラーはBF550に決まる */
		if ((errst->errStateNow & ENOT_ERRSTATE_PCS) == ENOT_ERRSTATE_PCS)
		{
			val0x88 = 0x41u;
			val0x89 = 0x03FFu;

			/* メーカ異常コード:0x86 */
			/* PCS通信不通コード(BF550)をコピー */
			memcpy(&val0x86[4], EPCS_PCSERR_CONNECTIONFAILED, 5);
		} else
		{	/* BF550が起きていない場合はCMD21のPCSエラーを参照する */
			/* 異常発生状態:0x88 */
			/* PCSエラーコードをHEMS_GW通知用エラーコードに置き換え */
			EENL_ChangeHemsErrCode((schar*)ope->pcsErr, NULL);

			/* BE000か判定 */
			if (strncmp(ope->pcsErr, EPCS_PCSERR_NONE, 5) == 0)
			{
				/* BE000の場合、異常なし */
				val0x88 = 0x42u;
				val0x89 = 0x0000u;

				/* メーカ異常コード:0x86 */
				/* エラーなしコード(BE000)をコピー */
				memcpy(&val0x86[4], EPCS_PCSERR_NONE, 5);
			} else
			{
				/* BE000以外の場合 */
				if (ope->pcsErr[0] == 'B' && ope->pcsErr[1] == 'C')
				{
					/* BCxxxの場合、異常なし */
					val0x88 = 0x42u;
					val0x89 = 0x0000u;

					/* メーカ異常コード:0x86 */
					/* エラーなしコード(BE000)をコピー */
					memcpy(&val0x86[4], EPCS_PCSERR_NONE, 5);
				} else
				{
					/* BE000、BCxxx以外の場合、異常あり */
					val0x88 = 0x41u;
					val0x89 = 0x03FFu;

					/* メーカ異常コード:0x86 */
					/* PCSのエラーコード(Bxxxx)をコピー */
					memcpy(&val0x86[4], ope->pcsErr, 5);
				}
			}
		}

		/* ENLへの設定 */
		if (val0x88 != 0xFFu)
		{
			/* 0x88: 異常発生状態 */
			EENL_BroadcastSetProperty(devList, EENL_DEV_LIST_MAX, 0x88u, 1u, &val0x88, EENL_ANNOUNCE_ON);
		} else
		{
			/* 処理なし */
		}

		if (val0x86[0] != '0')
		{
			/* 0x86 : メーカ異常コード */
			EENL_BroadcastSetProperty(devList, EENL_DEV_LIST_MAX, 0x86u, 9u, val0x86, EENL_ANNOUNCE_OFF);
		} else
		{
			/* 処理なし */
		}

		if (val0x89 != 0xFFFFu)
		{
			/* 0x89 : 異常内容 */
			EENL_BroadcastSetPropertyValue(devList, EENL_DEV_LIST_MAX, 0x89u, val0x89, 0u, EENL_ANNOUNCE_ON);
		} else
		{
			/* 処理なし */
		}
	} else
	{
		/* BF550が発生している場合、最新のエラーはBF550に決まる */
		if ((errst->errStateNow & ENOT_ERRSTATE_PCS) == ENOT_ERRSTATE_PCS)
		{
			val0x88 = 0x41u;
			val0x89 = 0x03FFu;
			val0x86[0] = 5u;

			memcpy(&val0x86[1], &g_eenlCodeNcc[0], 3);

			/* メーカ異常コード:0x86 */
			/* PCS通信不通コード(BF550)をコピー */
			memcpy(&val0x86[4], EPCS_PCSERR_CONNECTIONFAILED, 5);

			/* 0x88: 異常発生状態 */
			EENL_BroadcastSetProperty(
				devList, EENL_DEV_LIST_MAX, 0x88u, 1u, &val0x88, EENL_ANNOUNCE_ON);

			/* 0x86 : メーカ異常コード */
			EENL_BroadcastSetProperty(devList, EENL_DEV_LIST_MAX, 0x86u, 9u, val0x86, EENL_ANNOUNCE_OFF);

			/* 0x89 : 異常内容 */
			EENL_BroadcastSetPropertyValue(devList, EENL_DEV_LIST_MAX, 0x89u, val0x89, 0u, EENL_ANNOUNCE_ON);
		} else
		{
			/* 処理なし */
		}
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池・太陽光・V2H・マルチ入力PCSクラス				*/
/*					:									0x97/0x98の設定処理	*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　yyyymmdd		現在日付							*/
/*					:[in]　hhmm			現在時刻							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常発生有)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC K.jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_AllTimer0x97_0x98(const schar* yyyymmdd, const schar* hhmm)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値 */
	void* devList[EENL_DEV_LIST_MAX] = { NULL };				/* デバイスリスト */

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* ENLへ設定 */
	/* デバイスリストの作成 */
	EENL_SetBroadcastDevList(devList);

	/* ブロードキャストで設定しないデバイスにNULLを設定する */
	devList[EENL_DEV_LIST_NODE] = NULL;

	/*
	 *	0x97:状変なし
	 *	0x98:状変なし
	 */
	EENL_BroadcastSetProperty(devList, EENL_DEV_LIST_MAX, 0x97u, 2u, (uchar*)hhmm, EENL_ANNOUNCE_OFF);

	EENL_BroadcastSetProperty(devList, EENL_DEV_LIST_MAX, 0x98u, 4u, (uchar*)yyyymmdd, EENL_ANNOUNCE_OFF);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池・太陽光・V2Hクラス 0xF5の設定処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　ope		動作情報構造体							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常発生有)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC K.jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_AllTimer0xF5(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;					/* 戻り値 */
	EenlDeviceInfo_t* info = NULL;					/* device information */
	/* 0xF5設定値リスト */
	slong workF5[4] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	/* バイスリスト */
	void* devList[4] = { NULL, NULL, NULL, NULL };

	DEBUG_TRCIN();

	if (NULL == ope)
	{
		return ret;
	} else
	{
		/* 処理無し */
	}

	info = EENL_GetDeviceInfoIndex(0);
	if (NULL == info)
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		/* ノードは使わないので設定しない */
		devList[0] = info->devBattery;
		devList[1] = info->devSolar;
		devList[2] = info->devV2h;
		devList[3] = info->devMpcs;

		/* ニチコン仕向けのみ */
		/* 売買電電力実効値(W)   <-- PV 発電電力(本製品以外のPV)[W] */
		/* 発電時：正の値、電力消費時：負の値 */
		workF5[0] = ope->revPwrFlow;			
		/* 家庭消費電力値(0.01kW -> W) */
		workF5[1] = (slong)(ope->homeLoadPower * 10);
		/* 外付け太陽光発電電力値(W) */
		workF5[2] = ope->pvGenPwr;
		/* 予約 */
		workF5[3] = 0x7fffffff;
		/* 売買電電力実効値(W)が */
		/* 0x80000001～0x7FFFFFFD(-2,147,483,647～2,147,483,645) */
		/* の範囲外なら */
		/* PGreliefでチェックに引っかかりますが、*/
		/* 設定値の範囲がslongの最小値から最大値までなので偽になる事は */
		/* 無いが判断文としては残します */
		if (ope->revPwrFlow < (slong)EENL_LIMIT_PWRFLOW_MIN 
			|| ope->revPwrFlow >(slong)EENL_LIMIT_PWRFLOW_MAX)
		{
			/* EPCへの設定反映を行わない */
			ret = ENL_RESULT_SUCCESS;
		} else
		{
			/* 家庭消費電力値(W)が */
			/* 0x80000001～0x7FFFFFFD(-2,147,483,647～2,147,483,645) */
			/* の範囲外なら */
			/* PGreliefでチェックに引っかかりますが、*/
			/* 設定値の範囲がslongの最小値から最大値までなので偽になる事は */
			/* 無いが判断文としては残します */
			if (workF5[1] < (slong)EENL_LIMIT_LOADPOWER_MIN 
				|| workF5[1] > (slong)EENL_LIMIT_LOADPOWER_MAX)
			{
				/* EPCへの設定反映を行わない */
				ret = ENL_RESULT_SUCCESS;
			} else
			{
				/* 外付太陽光発電電力値(W)が0x80000001～0x7FFFFFFD */
				/* (-2,147,483,647～2,147,483,645)の範囲外なら */
				/* PGreliefでチェックに引っかかりますが、*/
				/* 設定値の範囲がslongの最小値から最大値までなので偽になる事は */
				/* 無いが判断文としては残します */
				if (ope->pvGenPwr < (slong)EENL_LIMIT_GENPWR_MIN 
					|| ope->pvGenPwr >(slong)EENL_LIMIT_GENPWR_MAX)
				{
					/* EPCへの設定反映を行わない */
					ret = ENL_RESULT_SUCCESS;
				} else
				{
					if (workF5[1] < 0)
					{
						/* 家庭消費電力値(W)が負値だったら0にする */
						workF5[1] = 0;
					} else
					{
						/* 処理無し */
					}

					workF5[0] = (slong)EENL_BYTE_SWAP_32(workF5[0]);
					workF5[1] = (slong)EENL_BYTE_SWAP_32(workF5[1]);
					workF5[2] = (slong)EENL_BYTE_SWAP_32(workF5[2]);
					workF5[3] = (slong)EENL_BYTE_SWAP_32(workF5[3]);

					/* 0xF5:状変なし */
					EENL_BroadcastSetProperty(devList, 4u, 0xF5u, 
						16u, (uchar*)workF5, EENL_ANNOUNCE_OFF);
				}
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池・太陽光・マルチ入力PCSクラス 0x83の設定処理		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pcsSerialNumber	PCSシリアル番号					*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常発生有)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC K.jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_pcsTimer0x83(const uchar* pcsSerialNumber)
{
	slong i = 0;									/* ループのカウンタ */
	uchar pdc = 17u;								/* 0x83設定値サイズ */
	uchar edt[17] = { 0u };							/* 0x83設定値リスト */
	slong ret = ENL_RESULT_SUCCESS;					/* 戻り値 */
	void* devList[EENL_DEV_LIST_MAX] = { NULL };	/* デバイスリスト */

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/*
	*	蓄電池クラスが作成されていない場合、edtに値が設定されない対応
	*	ライブラリからGetする値を存在するデバイスから取得する
	*/
	memset(&edt[0], 0x00, sizeof(edt));

	/* ノード、V2Hは含めない */
	for (i = EENL_DEV_LIST_MPCS; i < EENL_DEV_LIST_V2H 
		&& g_eenlDevList[i] == NULL; i++)
	{
		/* 現在の設定情報を取得 */
		ret = ENL_get_property(g_eenlDevList[i], 0x83, &pdc, edt);

		/* 押し上げ情報を設定（5バイト目） */
		edt[4] = g_eenlPushedUp;

		/* 製造番号を設定（6バイト目以降に12バイト分） */
		memcpy(&edt[5], pcsSerialNumber, 12);
	}

	/*デバイスリストの作成 */
	EENL_SetBroadcastDevList(devList);

	/*ブロードキャストで設定しないデバイスにNULLを設定する */
	devList[EENL_DEV_LIST_NODE] = NULL;
	devList[EENL_DEV_LIST_V2H] = NULL;

	/* 0x83:状変なし */
	EENL_BroadcastSetProperty(devList, EENL_DEV_LIST_MAX, 
		0x83, 17, (uchar*)edt, EENL_ANNOUNCE_OFF);

	/*※V2Hの押し上げ情報と製造番号の更新は実施しない */
	/* M1/M2では押し上げなし固定のため、更新不要 */
	/* 製造番号はコネクタロック時に更新するため、周期処理では更新しない */

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池・太陽光・マルチ入力PCSクラス 0x8Dの設定処理		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pcsSerialNumber	PCSシリアル番号					*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常発生有)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC K.jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_pcsTimer0x8D(const uchar* pcsSerialNumber)
{
	slong ret = ENL_RESULT_SUCCESS;					/* 戻り値 */
	void* devList[EENL_DEV_LIST_MAX] = { NULL };	/* デバイスリスト */

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* デバイスリストの作成 */
	EENL_SetBroadcastDevList(devList);

	/* ブロードキャストで設定しないデバイスにNULLを設定する */
	devList[EENL_DEV_LIST_NODE] = NULL;
	devList[EENL_DEV_LIST_V2H] = NULL;

	/* 蓄電池/太陽光/マルチ入力PCS 製造番号 : 0x8D */
	EENL_BroadcastSetProperty(devList, EENL_DEV_LIST_MAX, 
		0x8D, EENL_PDC_0x8D, (uchar*)pcsSerialNumber, EENL_ANNOUNCE_OFF);

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池・太陽光・マルチ入力PCSクラス 0xDB/0xD0の設定処理*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　ope	動作情報構造体								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					:(0:正常終了/0以外:異常発生有)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					:ENL_RESULT_SUCCESSで返す								*/
/*					:内部で使用するためNULLになり得ないのと、				*/
/*					:静的解析対応のため										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC K.jiang : 新規作成					*/
/*==========================================================================*/
static slong eenl_pcsTimer0xDB_0xD0(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;					/* 戻り値 */
	uchar val = 0xFFu;								/* 0xDB設定値 */
	uchar val2 = 0xFFu;								/* 0xD0設定値 */
	void* devList[EENL_DEV_LIST_MAX] = { NULL };	/* デバイスリスト */
	EenlDeviceInfo_t* info = NULL;					/* device information */

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (NULL == info)
	{
		ret = ENL_RESULT_SUCCESS;
	} else
	{
		/* デバイスリストの作成 */
		EENL_SetBroadcastDevList(devList);

		/* ブロードキャストで設定しないデバイスにNULLを設定する */
		devList[EENL_DEV_LIST_NODE] = NULL;
		devList[EENL_DEV_LIST_BT] = NULL;
		devList[EENL_DEV_LIST_V2H] = NULL;

		val = 0xFFu;
		val2 = 0xFFu;
		if (ope->pwrCut == EPCS_PWROUTAGE_YES)
		{
			/* 停電の場合 */
			val = 0x01u;		/* 独立 */
			val2 = 0x01u;	/* 独立 */
		} else
		{
			val = 0x02u;		/* 系統連系（逆潮流不可） */
			val2 = 0x00u;	/* 系統連系（逆潮流可） */
		}

		if (val != 0xFFu)
		{
			/* 設定値が決定している場合は、ENLに反映 */
			/* 蓄電池0xDB */
			if (info->devBattery != NULL)
			{
				ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
					0xDB, val, 0);
				/* 成功 */
				/* 状変アナウンスは無し */
				/* 失敗 */
				/* 異常でも処理続行 */
			} else
			{
				/* 処理無し */
			}
		} else
		{
			/* 処理無し */
		}

		if (val2 != 0xFF)
		{
			/* 設定値が決定している場合は、ENLに反映 */
			/* 太陽光0xD0、マルチ入力PCS0xD0 */
			EENL_BroadcastSetPropertyValue(devList, EENL_DEV_LIST_MAX, 
				0xD0, val2, 0, EENL_ANNOUNCE_OFF);
		} else
		{
			/* 処理無し */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

