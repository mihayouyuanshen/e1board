/****************************************************************************/
/*	Description		:機器オブジェクト蓄電池クラス 処理						*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC DX.zhao											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/27												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、EENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:無し													*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/
#include "eenl_cmn_g.h"
#include "eenl_bt.h"

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
/*	Description		:蓄電池デバイスの作成									*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info	蓄電池DEVICE情報							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:eenl_fileRead()を呼び出してから実行すること			*/
/*					 構築なので状変アナウンス通知は無し						*/
/*					 eenl_common.cで定義したプロパティリストの定義と		*/
/*					 初期化対象のepcの不一致が無いこと!!					*/
/*					 ニチコン向けH1HEMSエンハンス対応						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_CreateDeviceBattery(EenlDeviceInfo_t* info)
{
	slong ret = 0;
	slong retCode = 0;
	uchar data = 0x00u;
	uchar seri[16] = { 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u,
						0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u };
	uchar prod[16] = { 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u,
						0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u, 0x20u };
	uchar work1[2] = { 0x00u, 0x00u };
	uchar work2[4] = { 0x07u, 0xDEu, 0x01u, 0x01u };
	uchar work3[4] = { 0x00u, 0x00u, 0x00u, 0x00u };

	ulong val0xC8[2] = { 0u, 3000u };
	ulong val0xC9[2] = { 0u, 3000u };
	ulong val0xDC[2] = { 0u, 3000u };
	ulong val0xDD[2] = { 0u, 3000u };

	uchar merr[4] = { 0x00u, 0x00u, 0x00u, 0x00u };

	ulong work_EB = 99990u;
	ulong work_EC = 99990u;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (info == NULL) {
		DEBUG_TRCOUT();
		return -1;
	} else {
		/* 蓄電池デバイスの作成 */
		/* 押し上げ無し */
		DLOG_Debug("create device storage battery : ncc, ext_ns");
		ret = ENL_create_device_storage_battery(&info->devBattery,
			g_devicePropBatteryNccN, g_devicePropBatteryExtN,
			0x01, ENL_INSTANCE_MODE_DEVICE);

		if (ret != ENL_RESULT_SUCCESS) {
			DLOG_Error("battery device create error!(ncc):%ld", ret);
			/* error */
			retCode = -1;
		} else {
			/*  インスタンスのアドレスを保持 */
			g_eenlDevBattery = info->devBattery;
			DLOG_Debug("battery device created!!(instance:0x%08lX)",
				(ulong)g_eenlDevBattery);

			/* プロパティの初期値設定(初期化の値は、EchonetLiteSDK取り扱い説明書に記載の初期値に従う) */
			/*  0x80: 動作状態 off */
			data = 0x31u;	/* off */
			ret = ENL_DEV_set_property(NULL, info->devBattery, 0x80u, 1u, &data);
			if (ret != ENL_RESULT_SUCCESS) {
				DLOG_Error("battery 0x80 init error!:%ld", ret);
				/* error */
				retCode = -1;
			} else {
				/*  状変アナウンスは無し */

				/*  0x81: 設置場所 */
				data = EENL_LOCATION_INSIDE;	/* 屋内 */
				ret = ENL_DEV_set_property(NULL, info->devBattery, 0x81u, 1u, &data);
				if (ret != ENL_RESULT_SUCCESS) {
					DLOG_Error("battery 0x81 init error!:%ld", ret);
					/* error */
					retCode = -1;
				} else {
					/*  状変アナウンスは無し */

					/* 前回の設置場所情報を取得して反映 */
					/*   前回の設置場所情報は、eenl_fileRead()であらかじめグローバル変数に格納済み */
					DLOG_Debug("0x81 load value : 0x%02x", g_eenlBattery0x81);
					ret = EENL_ExtEnlDevSetProperty(NULL, info->devBattery,
						0x81u, 1u, &g_eenlBattery0x81);
					if (ret != ENL_RESULT_SUCCESS) {
						DLOG_Error("battery 0x81 load init fail(%02x) : %ld",
							g_eenlBattery0x81, ret);
						/* 前回の情報が反映できなくても初期値で動作可能なので処理続行 */
					} else {
						/* DO NOTHING */
					}

					/*  0x82: 規格バージョン AppendixI  （ライブラリ自動生成） */

					/*  0x83: 識別番号 */
					memcpy(&g_eenlId[1], &g_eenlCodeNcc[0], 3);

					/*  蓄電池の識別番号5バイト目は押し上げ状態 */
					g_eenlId[4] = g_eenlPushedUp;
					memcpy(&g_eenlId[5], &prod[0], 12);
					ret = ENL_DEV_set_property(NULL, info->devBattery,
						0x83u, 17u, (uchar*)g_eenlId);
					if (ret != ENL_RESULT_SUCCESS) {
						DLOG_Error("battery 0x83 init error!:%ld", ret);
						/* error */
						retCode = -1;
					} else {
						/* DO NOTHING */
					}
				}
			}
		}
	}

	if (0 == retCode) {
		/*  0x86: メーカ異常コード */
		memcpy(&merr[1], &g_eenlCodeNcc[0], 3);

		ret = ENL_DEV_set_property(NULL, info->devBattery, 0x86u, 4u, (uchar*)merr);
		if (ret != ENL_RESULT_SUCCESS) {
			DLOG_Error("battery 0x86 init error!:%ld", ret);
			/* error */
			retCode = -1;
		} else {
			/*  状変アナウンスは無し */
			/*  0x88: 異常発生状態 */
			data = 0x42;	/* 異常発生無し */
			ret = ENL_DEV_set_property(NULL, info->devBattery, 0x88u, 1u, &data);
			if (ret != ENL_RESULT_SUCCESS) {
				DLOG_Error("battery 0x88 init error!:%ld", ret);
				/* error */
				retCode = -1;
			} else {
				/*  状変アナウンスは無し */
			}
		}
	} else {
		/* DO NOTHING */
	}


	if (0 == retCode) {
		DLOG_Debug("0x8C init: ncc");
		/*  0x8C: 商品コード */
		ret = ENL_DEV_set_property(NULL, info->devBattery, 0x8Cu, 12u, (uchar*)prod);
		if (ret != ENL_RESULT_SUCCESS) {
			DLOG_Error("battery 0x8C init error!:%ld", ret);
			/* error */
			retCode = -1;
		} else {
			DLOG_Debug("0x8D init: ncc");
			/*  0x8D: 製造番号 */
			ret = ENL_DEV_set_property(NULL, info->devBattery, 0x8Du, 12u, (uchar*)seri);
			if (ret != ENL_RESULT_SUCCESS) {
				DLOG_Error("battery 0x8D init error!:%ld", ret);
				/* error */
				retCode = -1;
			} else {
				/*  0x93: 遠隔操作設定 */
				data = EENL_REMOTE_OFF;	/* 公衆回線未経由 */
				ret = EENL_ExtEnlDevSetProperty(NULL, info->devBattery, 0x93u, 1u, &data);
				if (ret != ENL_RESULT_SUCCESS) {
					DLOG_Error("battery 0x93 init error!:%ld", ret);
					/* error */
					retCode = -1;
				} else {
					/*  0x97: 現在時刻設定 00:00 */
					ret = ENL_DEV_set_property(NULL, info->devBattery, 0x97u, 2u, (uchar*)work1);
					if (ret != ENL_RESULT_SUCCESS) {
						DLOG_Error("battery 0x97 init error!:%ld", ret);
						/* error */
						retCode = -1;
					} else {
						/* DO NOTHING */
					}
				}
			}
		}
	} else {
		/* DO NOTHING */
	}

	if (0 == retCode) {
		/*  0x98: 現在年月日設定 2014/1/1 */
		ret = ENL_DEV_set_property(NULL, info->devBattery, 0x98u, 4u, (uchar*)work2);
		if (ret != ENL_RESULT_SUCCESS) {
			DLOG_Error("battery 0x98 init error!:%ld", ret);
			/* error */
			retCode = -1;
		} else {
			/*  0xA0: AC 実効容量（充電） */
			ret = ENL_DEV_set_property(NULL, info->devBattery, 0xA0u, 4u, (uchar*)work3);
			if (ret != ENL_RESULT_SUCCESS) {
				DLOG_Error("battery 0xA0 init error!:%ld", ret);
				/* error */
				retCode = -1;
			} else {
				/*  0xA1: AC 実効容量（放電） */
				ret = ENL_DEV_set_property(NULL, info->devBattery, 0xA1u, 4u, (uchar*)work3);
				if (ret != ENL_RESULT_SUCCESS) {
					DLOG_Error("battery 0xA1 init error!:%ld", ret);
					/* error */
					retCode = -1;
				} else {
					/*  0xA2: AC 充電可能容量 */
					ret = ENL_DEV_set_property(NULL, info->devBattery, 0xA2u, 4u, (uchar*)work3);
					if (ret != ENL_RESULT_SUCCESS) {
						DLOG_Error("battery 0xA2 init error!:%ld", ret);
						/* error */
						retCode = -1;
					} else {
						/* DO NOTHING */
					}
				}
			}
		}
	} else {
		/* DO NOTHING */
	}


	if (0 == retCode) {
		/*  0xA3: AC 放電可能容量 */
		ret = ENL_DEV_set_property(NULL, info->devBattery, 0xA3u, 4u, (uchar*)work3);
		if (ret != ENL_RESULT_SUCCESS) {
			DLOG_Error("battery 0xA3 init error!:%ld", ret);
			/* error */
			retCode = -1;
		} else {
			/*  0xA4: AC 充電可能量 */
			ret = ENL_DEV_set_property(NULL, info->devBattery, 0xA4u, 4u, (uchar*)work3);
			if (ret != ENL_RESULT_SUCCESS) {
				DLOG_Error("battery 0xA4 init error!:%ld", ret);
				/* error */
				retCode = -1;
			} else {
				/*  0xA5: AC 放電可能量 */
				ret = ENL_DEV_set_property(NULL, info->devBattery, 0xA5u, 4u, (uchar*)work3);
				if (ret != ENL_RESULT_SUCCESS) {
					DLOG_Error("battery 0xA5 init error!:%ld", ret);
					/* error */
					retCode = -1;
				} else {
					/*  0xA8: AC 積算充電電力量計測値 */
					ret = ENL_DEV_set_property(NULL, info->devBattery, 0xA8u, 4u, (uchar*)work3);
					if (ret != ENL_RESULT_SUCCESS) {
						DLOG_Error("battery 0xA8 init error!:%ld", ret);
						/* error */
						retCode = -1;
					} else {
						/* DO NOTHING */
					}
				}
			}
		}
	} else {
		/* DO NOTHING */
	}


	if (0 == retCode) {
		/*  0xA9: AC 積算放電電力量計測値 */
		ret = ENL_DEV_set_property(NULL, info->devBattery, 0xA9u, 4u, (uchar*)work3);
		if (ret != ENL_RESULT_SUCCESS) {
			DLOG_Error("battery 0xA9 init error!:%ld", ret);
			/* error */
			retCode = -1;
		} else {
			/* 前回のAC充電量設定値情報を取得して反映 */
			/*   前回のAC充電量設定値情報は、eenl_fileRead()であらかじめグローバル変数に格納済み */
			if (g_eenlBattery0xAA == 0xFFFFFFFF) {
				/*  初期値(0x00000000:未設定)を設定しておく */
				ret = EENL_ExtEnlDevSetProperty(NULL, info->devBattery,
					0xAAu, 4u, (uchar*)work3);
				if (ret != ENL_RESULT_SUCCESS) {
					DLOG_Error("battery 0xAA init error!:%ld", ret);
					/* error */
					retCode = -1;
				} else {
					DLOG_Debug("battery 0xAA init work3 ret=%ld", ret);
				}
				/* 初期値はeinvには反映しない */
			} else {
				DLOG_Debug("0xAA load value : 0x%08lx", g_eenlBattery0xAA);
				ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
					0xAAu, g_eenlBattery0xAA, 0u);
				if (ret != ENL_RESULT_SUCCESS) {
					DLOG_Error("battery 0xAA load init error!:%ld", ret);
					/* 前回の情報が反映できなくても初期値で動作可能なので処理続行 */
				} else {
					/* DO NOTHING */
				}
				/* einvへの反映はイベント登録後 */
			}
		}
	} else {
		/* DO NOTHING */
	}


	if (0 == retCode) {
		/* 前回のAC放電量設定値情報を取得して反映 */
		/*   前回のAC放電量設定値情報は、eenl_fileRead()であらかじめグローバル変数に格納済み */
		if (g_eenlBattery0xAB == 0xFFFFFFFFu) {
			/*  初期値(0x00000000:未設定)を設定しておく */
			ret = EENL_ExtEnlDevSetProperty(NULL, info->devBattery,
				0xABu, 4u, (uchar*)work3);
			if (ret != ENL_RESULT_SUCCESS) {
				DLOG_Error("battery 0xAB init error!:%ld", ret);
				/* error */
				retCode = -1;
			} else {
				DLOG_Debug("battery 0xAB init work3 ret=%ld", ret);
			}
			/* 初期値はeinvには反映しない */
		} else {
			DLOG_Debug("0xAB load value : 0x%08lx", g_eenlBattery0xAB);
			ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
				0xABu, g_eenlBattery0xAB, 0u);
			if (ret != ENL_RESULT_SUCCESS) {
				DLOG_Error("battery 0xAB load init error!:%ld", ret);
				/* 前回の情報が反映できなくても初期値で動作可能なので処理続行 */
			} else {
				/* DO NOTHING */
			}
			/* einvへの反映はイベント登録後 */
		}
	} else {
		/* DO NOTHING */
	}


	if (0 == retCode) {
		/*  0xC8: 最小最大充電電力値 */
		ret = ENL_DEV_set_property_value(NULL, info->devBattery, 0xC8u, val0xC8[0], 0u);
		if (ret != ENL_RESULT_SUCCESS) {
			DLOG_Error("battery 0xC8[0] init error!:%ld", ret);
			/* error */
			retCode = -1;
		} else {
			ret = ENL_DEV_set_property_value(NULL, info->devBattery, 0xC8u, val0xC8[1], 1u);
			if (ret != ENL_RESULT_SUCCESS) {
				DLOG_Error("battery 0xC8[1] init error!:%ld", ret);
				/* error */
				retCode = -1;
			} else {
				/*  0xC9: 最小最大放電電力値 */
				ret = ENL_DEV_set_property_value(NULL, info->devBattery, 0xC9u, val0xC9[0], 0u);
				if (ret != ENL_RESULT_SUCCESS) {
					DLOG_Error("battery 0xC9[0] init error!:%ld", ret);
					/* error */
					retCode = -1;
				} else {
					ret = ENL_DEV_set_property_value(NULL, info->devBattery, 0xC9u, val0xC9[1], 1u);
					if (ret != ENL_RESULT_SUCCESS) {
						DLOG_Error("battery 0xC9[1] init error!:%ld", ret);
						/* error */
						retCode = -1;
					} else {
						/* DO NOTHING */
					}
				}
			}
		}
	} else {
		/* DO NOTHING */
	}


	if (0 == retCode) {
		/*  0xCF: 運転動作状態 */
		data = 0x44u;	/* 待機 */
		ret = ENL_DEV_set_property(NULL, info->devBattery, 0xCFu, 1u, &data);
		if (ret != ENL_RESULT_SUCCESS) {
			DLOG_Error("battery 0xCF init error!:%ld", ret);
			/* error */
			retCode = -1;
		} else {
			/*  状変アナウンスは無し */

			/*  0xD0: 定格電力量 0x00000000 */
			ret = ENL_DEV_set_property(NULL, info->devBattery, 0xD0u, 4u, (uchar*)work3);
			if (ret != ENL_RESULT_SUCCESS) {
				DLOG_Error("battery 0xD0 init error!:%ld", ret);
				/* error */
				retCode = -1;
			} else {
				/* 0xD2: 定格電圧 */
				ret = ENL_DEV_set_property(NULL, info->devBattery, 0xD2u, 2u, (uchar*)work1);
				if (ret != ENL_RESULT_SUCCESS) {
					DLOG_Error("battery 0xD2 init error!:%ld", ret);
					/* error */
					retCode = -1;
				} else {
					/*  0xD3: 瞬時充放電電力計測値 */
					ret = ENL_DEV_set_property(NULL, info->devBattery, 0xD3u, 4u, (uchar*)work3);
					if (ret != ENL_RESULT_SUCCESS) {
						DLOG_Error("battery 0xD3 init error!:%ld", ret);
						/* error */
						retCode = -1;
					} else {
						/* DO NOTHING */
					}
				}
			}
		}
	} else {
		/* DO NOTHING */
	}

	if (0 == retCode) {
		/*  0xD6: 積算放電電力量計測値（CMD21：積算放電電力量２（コンバータ部）） */
		ret = ENL_DEV_set_property(NULL, info->devBattery, 0xD6u, 4u, (uchar*)work3);
		if (ret != ENL_RESULT_SUCCESS) {
			DLOG_Error("battery 0xD6 init error!:%ld", ret);
			/* error */
			retCode = -1;
		} else {
			/*  0xD8: 積算充電電力量計測値（CMD21：積算充電電力量２（コンバータ部）） */
			ret = ENL_DEV_set_property(NULL, info->devBattery, 0xD8u, 4u, (uchar*)work3);
			if (ret != ENL_RESULT_SUCCESS) {
				DLOG_Error("battery 0xD8 init error!:%ld", ret);
				/* error */
				retCode = -1;
			} else {
				/*  0xDA: 運転モード設定 */
				data = 0x44;	/* 待機 */
				ret = ENL_DEV_set_property(NULL, info->devBattery, 0xDAu, 1u, &data);
				if (ret != ENL_RESULT_SUCCESS) {
					DLOG_Error("battery 0xDA init error!:%ld", ret);
					/* error */
					retCode = -1;
				} else {
					/*  状変アナウンスは無し */

					/*  0xDB: 系統連系状態 */
					data = 0x01;	/* 独立 */
					ret = ENL_DEV_set_property(NULL, info->devBattery, 0xDBu, 1u, &data);
					if (ret != ENL_RESULT_SUCCESS) {
						DLOG_Error("battery 0xDB init error!:%ld", ret);
						/* error */
						retCode = -1;
					} else {
						/* DO NOTHING */
					}
				}
			}
		}
	} else {
		/* DO NOTHING */
	}


	if (0 == retCode) {
		/*  0xDC: 最小最大充電電力値（独立時） */
		ret = ENL_DEV_set_property_value(NULL, info->devBattery, 0xDCu, val0xDC[0], 0u);
		if (ret != ENL_RESULT_SUCCESS) {
			DLOG_Error("battery 0xDC[0] init error!:%ld", ret);
			/* error */
			retCode = -1;
		} else {
			ret = ENL_DEV_set_property_value(NULL, info->devBattery, 0xDCu, val0xDC[1], 1u);
			if (ret != ENL_RESULT_SUCCESS) {
				DLOG_Error("battery 0xDC[1] init error!:%ld", ret);
				/* error */
				retCode = -1;
			} else {
				/*  0xDD: 最小最大放電電力値（独立時） */
				ret = ENL_DEV_set_property_value(NULL, info->devBattery, 0xDDu, val0xDD[0], 0u);
				if (ret != ENL_RESULT_SUCCESS) {
					DLOG_Error("battery 0xDD[0] init error!:%ld", ret);
					/* error */
					retCode = -1;
				} else {
					ret = ENL_DEV_set_property_value(NULL, info->devBattery, 0xDDu, val0xDD[1], 1u);
					if (ret != ENL_RESULT_SUCCESS) {
						DLOG_Error("battery 0xDD[1] init error!:%ld", ret);
						/* error */
						retCode = -1;
					} else {
						/* DO NOTHING */
					}
				}
			}
		}
	} else {
		/* DO NOTHING */
	}

	if (0 == retCode) {
		/*  0xE2: 蓄電残量１ */
		ret = ENL_DEV_set_property(NULL, info->devBattery, 0xE2u, 4u, (uchar*)work3);
		if (ret != ENL_RESULT_SUCCESS) {
			DLOG_Error("battery 0xE2 init error!:%ld", ret);
			/* error */
			retCode = -1;
		} else {
			/*  0xE4: 蓄電残量３ */
			data = 0x00;
			ret = ENL_DEV_set_property(NULL, info->devBattery, 0xE4u, 1u, &data);
			if (ret != ENL_RESULT_SUCCESS) {
				DLOG_Error("battery 0xE4 init error!:%ld", ret);
				/* error */
				retCode = -1;
			} else {
				/*  0xE5: 劣化状態 */
				data = 0x00;
				ret = ENL_DEV_set_property(NULL, info->devBattery, 0xE5u, 1u, &data);
				if (ret != ENL_RESULT_SUCCESS) {
					DLOG_Error("battery 0xE5 init error!:%ld", ret);
					/* error */
					retCode = -1;
				} else {
					/*  0xE6: 蓄電池タイプ */
					data = 0x04;	/* Li電池 */
					ret = EENL_ExtEnlDevSetProperty(NULL, info->devBattery, 0xE6u, 1u, &data);
					if (ret != ENL_RESULT_SUCCESS) {
						DLOG_Error("battery 0xE6 init error!:%ld", ret);
						/* error */
						retCode = -1;
					} else {
						/* DO NOTHING */
					}
				}
			}
		}
	} else {
		/* DO NOTHING */
	}


	if (0 == retCode) {
		/*  0xE7: 充電量設定値１ */
		ret = ENL_DEV_set_property(NULL, info->devBattery, 0xE7u, 4u, (uchar*)work3);
		if (ret != ENL_RESULT_SUCCESS) {
			DLOG_Error("battery 0xE7 init error!:%ld", ret);
			/* error */
			retCode = -1;
		} else {
			/* 前回の充電量設定値情報を取得して反映 */
			/*   前回の充電量設定値情報は、eenl_fileRead()であらかじめグローバル変数に格納済み */
			if (g_eenlBattery0xE7 == 0xFFFFFFFF) {
				/* 初期状態の場合は、処理しない */
				DLOG_Debug("battery 0xE7 init skip.");
			} else {
				DLOG_Debug("0xE7 load value : 0x%08lx", g_eenlBattery0xE7);
				ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
					0xE7u, g_eenlBattery0xE7, 0u);
				if (ret != ENL_RESULT_SUCCESS) {
					DLOG_Error("battery 0xE7 load init error!:%ld", ret);
					/* 前回の情報が反映できなくても初期値で動作可能なので処理続行 */
				} else {
					/* DO NOTHING */
				}
				/* einvへの反映はイベント登録 */
			}

			/*  0xE8: 放電量設定値１ */
			ret = ENL_DEV_set_property(NULL, info->devBattery, 0xE8u, 4u, (uchar*)work3);
			if (ret != ENL_RESULT_SUCCESS) {
				DLOG_Error("battery 0xE8 init error!:%ld", ret);
				/* error */
				retCode = -1;
			} else {
				/* DO NOTHING */
			}
		}
	} else {
		/* DO NOTHING */
	}


	if (0 == retCode) {
		/* 前回の放電量設定値情報を取得して反映 */
		/*   前回の放電量設定値情報は、eenl_fileRead()であらかじめグローバル変数に格納済み */
		if (g_eenlBattery0xE8 == 0xFFFFFFFF) {
			/* 初期状態の場合は、何もしない */
			DLOG_Debug("battery 0xE8 init skip.");
		} else {
			DLOG_Debug("0xE8 load value : 0x%08lx", g_eenlBattery0xE8);
			ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
				0xE8u, g_eenlBattery0xE8, 0u);
			if (ret != ENL_RESULT_SUCCESS) {
				DLOG_Error("battery 0xE8 load init error!:%ld", ret);
				/* 前回の情報が反映できなくても初期値で動作可能なので処理続行 */
			} else {
				/* DO NOTHING */
			}
			/* einvへの反映はイベント登録 */
		}

		/*  0xEB : 充電電力設定値[W] */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
			0xEBu, work_EB, 0u);
		if (ret != ENL_RESULT_SUCCESS) {
			DLOG_Error("battery 0xEB init error!:%ld", ret);
			/* error */
			retCode = -1;
		} else {
			/*  0xEC : 放電電力設定値[W] */
			ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
				0xECu, work_EC, 0u);
			if (ret != ENL_RESULT_SUCCESS) {
				DLOG_Error("battery 0xEC init error!:%ld", ret);
				/* error */
				retCode = -1;
			} else {
				/* DO NOTHING */
			}
		}
	} else {
		/* DO NOTHING */
	}

	if (0 == retCode) {
		/********************************************/
		/*			ユーザ定義の設定開始			*/
		/********************************************/

		/*  0xF0: ユーザ運転モード設定 */
		DLOG_Debug("0xF0 init");
		data = 0x05;	/* 待機 */
		ret = ENL_DEV_set_property(NULL, info->devBattery, 0xF0u, 1u, &data);
		if (ret != ENL_RESULT_SUCCESS) {
			DLOG_Error("battery 0xF0 init error!:%ld", ret);
			/* error */
			retCode = -1;
		} else {
			/* DO NOTHING */
		}

		EENL_CreateUserDefineEpcNcc(info);
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();

	return retCode;
}

/*==========================================================================*/
/*	Description		:NCC仕向けのユーザ定義プロパティ初期設定				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info	蓄電池DEVICE情報							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:NCC対応ユーザ定義EPCは以下								*/
/*					 0xF0:ユーザ運転モード設定								*/
/*						(仕向け共通で別のところで初期設定する)				*/
/*					 0xF2:非常時安心設定									*/
/*					 0xF4:自動運転設定										*/
/*					 0xF5:瞬時電力計測値									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_CreateUserDefineEpcNcc(EenlDeviceInfo_t* info)
{
	slong ret = 0;
	slong retCode = 0;
	uchar data = 0x00u;
	uchar bs = 0u;
	uchar work_0xF4[EENL_PDC_USER_0xF4_PUSHEDUP] = { 0x00u , 0x01u , 0x02u ,
		0x01u , 0x17u , 0x05u , 0x06u , 0x3Bu , 0x0Au , 0x00u , 0x16u , 0x3Bu , 0x02u ,
		0x17u , 0x05u , 0x06u , 0x3Bu , 0x0Au , 0x00u , 0x16u , 0x3Bu , 0x07u , 0x17u ,
		0x05u , 0x06u , 0x3Bu , 0x0Au , 0x00u , 0x16u , 0x3Bu , 0x08u , 0x17u , 0x05u ,
		0x06u , 0x3Bu , 0x0Au , 0x00u , 0x16u , 0x3Bu };
	slong work_F5[4] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000 };


	DEBUG_TRCIN();

	if (NULL == info) {
		/* コール元でチェック済みのためありえない */
		retCode = -1;
	} else {
		/* ニチコンの場合 */
		/*  0xF2: 非常時安心設定 ★ */
		data = 0x00u;
		ret = ENL_DEV_set_property(NULL, info->devBattery, 0xF2u, 1u, &data);
		if (ret != ENL_RESULT_SUCCESS) {
			DLOG_Error("battery(n) 0xF2 init error!:%ld", ret);
			/* error */
			retCode = -1;
		} else {
			/*  ニチコンの場合  2017/01/23 ニチコン向けH1HEMSエンハンス対応 */
			/* 0xF4:自動運転設定 */
			/* 押し上げ無し */
			bs = EENL_PDC_USER_0xF4;
			work_0xF4[2] = EENL_0xF4_TIMESET;

			ret = ENL_DEV_set_property(NULL, info->devBattery,
				0xF4u, bs, (uchar*)work_0xF4);
			if (ret != ENL_RESULT_SUCCESS) {
				DLOG_Error("battery 0xF4 init error!:%ld", ret);
				/* error */
				retCode = -1;
			} else {
				/* 0xF5:瞬時電力計測値 */
				work_F5[0] = 0;		/* 売買電電力値 */
				work_F5[1] = 0;		/* 家庭消費電力値 */
				work_F5[2] = 0;		/* 外付太陽光発電電力値 */
				work_F5[3] = 0x7FFFFFFF;	/* 予約（0x7FFFFFFF） */
				ret = ENL_DEV_set_property(NULL, info->devBattery,
					0xF5u, 16u, (uchar*)work_F5);
				if (ret != ENL_RESULT_SUCCESS) {
					DLOG_Error("battery 0xF5 init error!:%ld", ret);
					/* error */
					retCode = -1;
				} else {
					/* DO NOTHING */
				}
			}
		}
	}

	DEBUG_TRCOUT();
	return retCode;
}

/*==========================================================================*/
/*	Description		:einvデータの反映（蓄電池）								*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　regInfo	蓄電池の登録情報						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:NCC対応ユーザ定義EPCは以下								*/
/*					 0xF0:ユーザ運転モード設定								*/
/*						(仕向け共通で別のところで初期設定する)				*/
/*					 0xF8:運転指示											*/
/*					 0xF9:残量設定											*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_UpdateEpcsBattery(EenlDeviceInfo_t* info, EPCS_REG_INFO_T regInfo)
{
	slong ret = 0;
	slong i = 0;
	uchar unitAddr = 1u;
	void* dev_type = NULL;
	EPCS_SERIAL_NUM_T pcsSerial;
	EPCS_OPESTTS_INFO_T opeStts;
	EPCS_CHARGE_TIME_ZONE_T chTmZn;
	EPCS_OPEMODE_GET_T ope;

	DEBUG_TRCIN();

	memset(&pcsSerial, 0x00, sizeof(pcsSerial));
	memset(&opeStts, 0x00, sizeof(opeStts));
	memset(&chTmZn, 0x00, sizeof(chTmZn));
	memset(&ope, 0x00, sizeof(ope));

	DLOG_Debug("-----[einv battery] start-----");

	if (info == NULL) {
		DLOG_Error("-----[einv battery] NULL-----");
		return -1;	/* error */
	} else {
		/* DO NOTHING */
	}

	if (info->devBattery == NULL) { /*  batteryが無いなら */
		DLOG_Debug("-----[einv battery] pass-----");
		return 0;	/* errorにしない */
	} else {
		dev_type = info->devBattery;

		/* 情報収集 */

		/*
			機種名・製造番号取得
			EPCS_SERIAL_NUM_T
				schar pcsModelName[EPCS_PCS_MODEL_NAME_LEN];	PCS機種名
				schar pcsSerialNumber[EPCS_PCS_SERIAL_NUM_LEN];	PCS製造番号
		*/
		ret = epcs_sync_getPcsModelSerialNumber(unitAddr, &pcsSerial);
		if (ret != EPCS_OK) {
			DLOG_Error("getPcsModelSerialNumber error! : %ld", ret);
		} else {
			/* DO NOTHING */
		}

		DLOG_Debug("SerialNumber :");
		for (i = 0; i < 17; i++) {
			DLOG_Debug("%c[%02x]", pcsSerial.pcsSerialNumber[i],
				pcsSerial.pcsSerialNumber[i]);
		}

		DLOG_Debug("SerialNumber: <%s>", pcsSerial.pcsSerialNumber);
		DLOG_Debug("ModelNumber : <%s>", pcsSerial.pcsModelName);

		/* 動作情報取得 */
		ret = epcs_sync_getOpeSttsInfo(&opeStts);
		if (ret != EPCS_OK) {
			DLOG_Error("getOpeSttsInfo error! : %ld", ret);
		} else {
			/* DO NOTHING */
		}

		DLOG_Debug("operation : %d", opeStts.operation);

		/* einv運転モード取得*/
		ret = epcs_sync_getOpeMode(&ope);
		if (ret != EPCS_OK) {
			DLOG_Error("getOpeMode error! : %ld", ret);
		} else {
			/* DO NOTHING */
		}


		/* イベント登録は関数化して集約 */

		/*  0xC8 : 蓄電池への充電電力の最小値および最大値を、それぞれW（AC)で示す */
		ret = eenl_BtUpdate0xC8(info, &regInfo);

		/*  0xC9 : 蓄電池への放電電力の最小値および最大値を、それぞれW（AC)で示す */
		ret = eenl_BtUpdate0xC9(info, &regInfo);

		/*  0xD0 : 定格電力量 */
		ret = eenl_BtUpdate0xD0_0xE7_0xE8(info, &regInfo);

		/*  0xD2: 定格電圧 */
		ret = eenl_BtUpdate0xD2(info, &regInfo);

		/*  ----- ECHONET Liteへ設定 ----- */
		/*  0x83 : 識別番号（蓄電池） */
		ret = EENL_DevUpdate0x83(pcsSerial.pcsSerialNumber, dev_type);

		/*  0x8C : 商品コード（蓄電池） */
		ret = EENL_DevUpdate0x8C(pcsSerial.pcsModelName, dev_type);

		/*  0x8D : 製造番号（蓄電池） */
		ret = EENL_DevUpdate0x8D(pcsSerial.pcsSerialNumber, dev_type);

		/*  ----- ECHONET Liteへ設定 ----- */
		/*  0x80 : 動作状態 */
		ret = EENL_DevUpdate0x80(&opeStts, dev_type);

		/*  0xD3 : 瞬時充放電電力 */
		ret = eenl_BtUpdate0xD3(info, &opeStts);

		/*  0xD6 : 積算放電電力量計測値（CMD21：積算放電電力量２（コンバータ部）） */
		ret = eenl_BtUpdate0xD6(info, &opeStts);

		/*  0xD8 : 積算充電電力量計測値（CMD21：積算充電電力量２（コンバータ部）） */
		ret = eenl_BtUpdate0xD8(info, &opeStts);

		/*  0xE2 : 蓄電残量１　Wh */
		ret = eenl_BtUpdate0xE2(info, &opeStts);

		/*  0xE4 : 蓄電残量３　% */
		ret = eenl_BtUpdate0xE4(info, &opeStts);

		/*  0xDB : 系統連系状態 */
		ret = eenl_BtUpdate0xDB(info, &opeStts);

		/*  0xDC: 最小最大充電電力値（独立時） */
		ret = eenl_BtUpdate0xDC(info, &regInfo);

		/*  0xDD: 最小最大放電電力値（独立時） */
		ret = eenl_BtUpdate0xDD(info, &regInfo);

		/*  0xEB : 充電電力設定値 */
		/*  ※ここでは設定しない。周期処理(eenl_timer)で設定する。 */

		/*  0xEC : 放電電力設定値 */
		/*  ※ここでは設定しない。周期処理(eenl_timer)で設定する。 */

		/*  ----- ECHONET Liteへ設定 ----- */
		/*  0xDA : 運転モード設定 */
		/*  0xF0 : ユーザ運転モード設定 */
		/*  0xF4 : 自動運転設定				京セラの場合のみ */
	/*
			値		仕向け
					KC					SK,KN,N1			SA,A1
			0x00	深夜電力活用		N/A					深夜電力活用
			0x01	太陽光売電優先		売電モード			売電モード
			0x02	ピークカット		グリーンモード		ピークカット
			0x03	太陽光充電			N/A					グリーンモード
			0x04	充電				充電				充電
			0x05	放電				放電				放電
			0x06	待機				待機				待機
			0x07	メンテナンスモード	メンテナンスモード	メンテナンスモード
	*/

	/*  0xF0 */
		ret = eenl_BtUpdate0xF0(info, &ope);

		/*  0xF4 */
		ret = eenl_BtUpdate0xF4(info, &ope);

		/*  0xDA */
		ret = eenl_BtUpdate0xDA(info, &ope);

		/* 0xF2 : 非常時安心設定 */
		ret = eenl_BtUpdate0xF2(info, &ope);

		/*  0xF8(運転指示) */
		ret = eenl_BtUpdate0xF8(info, &ope);
	}

	DEBUG_TRCOUT();
	return 0;
}

/*==========================================================================*/
/*	Description		:設定電文のチェック（蓄電池）							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in] instance	インスタンスハンドル					*/
/*					 [in] epc		設定対象EPC								*/
/*					 [in] pdc		設定値のデータサイズ					*/
/*					 [in] edt		設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 ENL_RESULT_ACCEPT:		正常終了						*/
/*					 ENL_RESULT_NOT_ACCEPT:	異常終了						*/
/*--------------------------------------------------------------------------*/
/*	detail			:ニチコン向けH1HEMSエンハンス対応						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BatterySetCheck(void* instance, uchar epc, uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_NOT_ACCEPT;

	DEBUG_TRCIN();

	DLOG_Debug("-----battery set check-----");
	DLOG_Debug("epc:%02x  pdc:0x%02x(%d)  edt:0x%02x(%d)",
		epc, *pdc, *pdc, *edt, *edt);

	switch (epc) {
	case 0x81u:
		/* 設置場所 */
		ret = EENL_DevSetCheck0x81(instance, pdc, edt);
		break;
	case 0x97u:
		/* 現在時刻設定 */
		ret = EENL_DevSetCheck0x97(pdc, edt);
		break;
	case 0x98u:
		/* 現在年月日設定 */
		ret = EENL_DevSetCheck0x98(pdc, edt);
		break;
	case 0xAAu:
		/* AC充電量設定値 */
		ret = eenl_BtSetCheck0xAA(pdc, edt);
		break;
	case 0xABu:
		/* AC放電量設定値 */
		ret = eenl_BtSetCheck0xAB(pdc, edt);
		break;
	case 0xC1u:
		/*  0xC1(充電方式) */
		ret = eenl_BtSetCheck0xC1(pdc, edt);
		break;
	case 0xC2u:
		/*  0xC2(放電方式) */
		ret = eenl_BtSetCheck0xC2(pdc, edt);
		break;
	case 0xDAu:
		/* 運転モード設定 */
		ret = eenl_BtSetCheck0xDA(pdc, edt);
		break;
	case 0xE7u:
		/* 充電量設定値１ */
		ret = eenl_BtSetCheck0xE7(pdc, edt);
		break;
	case 0xE8u:
		/* 放電量設定値１ */
		ret = eenl_BtSetCheck0xE8(pdc, edt);
		break;
	case 0xEBu:
		/*  充電電力設定値[W] */
		ret = eenl_BtSetCheck0xEB(pdc, edt);
		break;
	case 0xECu:
		/*  放電電力設定値設定[W] */
		ret = eenl_BtSetCheck0xEC(pdc, edt);
		break;
	case 0xF2u:
		/* ユーザ定義　非常時安心設定 */
		ret = eenl_BtSetCheck0xF2(pdc, edt);
		break;
	case 0xF4u:
		/* ユーザ定義　自動運転設定 */
		ret = eenl_BtSetCheck0xF4(pdc, edt);
		break;
	case 0xF8u:
		/*  0xF8(運転指示) */
		ret = eenl_BtSetCheck0xF8(pdc, edt);
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
/*	Description		:蓄電池クラス 特殊運転モード判定処理					*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　opemode		運転モード							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (true:特殊運転モードである/false:特殊運転モードでない)	*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_SpOpemodeChk(uchar opemode)
{
	slong ret = FALSE;

	DEBUG_TRCIN();

	switch (opemode) {
	case EPCS_OPEMODE_3:			/*  EVモード */
	case EPCS_OPEMODE_9:			/*  余剰充電・放電 */
	case EPCS_OPEMODE_A:			/*  余剰のみ充電 */
	case EPCS_OPEMODE_B:			/*  車両充電 */
	case EPCS_OPEMODE_C:			/*  車両放電 */
	case EPCS_OPEMODE_E:			/*  車両拡張充電 */
	case EPCS_OPEMODE_F:			/*  エレムーブ充電 */
	case EPCS_OPEMODE_G:			/*  エレムーブ放電 */
	case EPCS_OPEMODE_H:			/*  EVモード_余剰充電・放電 */
	case EPCS_OPEMODE_I:			/*  EVモード_余剰のみ充電 */
	case EPCS_OPEMODE_J:			/*  EVモード_エレムーブ充電 */
	case EPCS_OPEMODE_K:			/*  EVモード_車両充電 */
	case EPCS_OPEMODE_L:			/*  EVモード_待機(未使用) */
		ret = TRUE;					/*  特殊運転モードである */
		break;
	default:
		ret = FALSE;				/*  特殊運転モードでない */
		break;
	}

	DEBUG_TRCOUT();
	return(ret);
}

/*==========================================================================*/
/*	Description		:蓄電池クラス、車両充放電器クラス						*/
/*					 電力移動モード判定処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　opemode		運転モード							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (true:電力移動モードである/false:電力移動モードでない)	*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_ElectricMoveModeChk(uchar opemode)
{
	slong ret = FALSE;

	DEBUG_TRCIN();

	switch (opemode) {
	case EPCS_OPEMODE_3:			/*  EVモード */
	case EPCS_OPEMODE_E:			/*  車両拡張充電 */
	case EPCS_OPEMODE_F:			/*  エレムーブ充電 */
	case EPCS_OPEMODE_G:			/*  エレムーブ放電 */
	case EPCS_OPEMODE_H:			/*  EVモード_余剰充電・放電 */
	case EPCS_OPEMODE_I:			/*  EVモード_余剰のみ充電 */
	case EPCS_OPEMODE_J:			/*  EVモード_エレムーブ充電 */
	case EPCS_OPEMODE_K:			/*  EVモード_車両充電 */
	case EPCS_OPEMODE_L:			/*  EVモード_待機(未使用) */
		ret = TRUE;					/*  電力移動モードである */
		break;
	default:
		ret = FALSE;				/*  電力移動モードでない */
		break;
	}

	DEBUG_TRCOUT();
	return(ret);
}


/*==========================================================================*/
/*	Description		:蓄電池クラス 0xAAの設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xAA			0xAA設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtNotify0xAA(uchar val0xAA)
{
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;
	ENL_MESSAGE* msg = NULL;

	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);
	if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		ret = EENL_ExtEnlDevSetPropertyValue(&msg, info->devBattery,
			0xAAu, val0xAA, 0u);
		if (ret == ENL_RESULT_SUCCESS) {
			DLOG_Debug("0xAA : %d", val0xAA);
			if (msg != NULL) {
				/* 状変アナウンス通知 */
				EENL_SendMessage(NULL, msg);
				DLOG_Debug("#### send broadcast message (battery:0xAA) ####");
			} else {
				/* DO NOTHING */
			}
		} else {
			DLOG_Error("0xAA set error!");
			/*  処理は継続 */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xABの設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xAB			0xAB設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtNotify0xAB(uchar val0xAB)
{
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;
	ENL_MESSAGE* msg = NULL;

	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);
	if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		ret = EENL_ExtEnlDevSetPropertyValue(&msg, info->devBattery,
			0xABu, val0xAB, 0u);
		if (ret == ENL_RESULT_SUCCESS) {
			DLOG_Debug("0xAB : %d", val0xAB);
			if (msg != NULL) {
				/* 状変アナウンス通知 */
				EENL_SendMessage(NULL, msg);
				DLOG_Debug("#### send broadcast message (battery:0xAB) ####");
			} else {
				/* DO NOTHING */
			}
		} else {
			DLOG_Error("0xAB set error!");
			/*  処理は継続 */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}


/*==========================================================================*/
/*	Description		:蓄電池クラス 0xC1の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xC1			0xC1設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtNotify0xC1(uchar val0xC1)
{
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;
	ENL_MESSAGE* msg = NULL;

	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);
	if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		ret = EENL_ExtEnlDevSetPropertyValue(&msg, info->devBattery,
			0xC1u, val0xC1, 0u);
		if (ret == ENL_RESULT_SUCCESS) {
			DLOG_Debug("0xC1 : %d", val0xC1);
			if (msg != NULL) {
				/* 状変アナウンス通知 */
				EENL_SendMessage(NULL, msg);
				DLOG_Debug("#### send broadcast message (battery:0xC1) ####");
			} else {
				/* DO NOTHING */
			}
		} else {
			DLOG_Error("0xC1 set error!");
			/*  処理は継続 */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xC2の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xC2			0xC2設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtNotify0xC2(uchar val0xC2)
{
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;
	ENL_MESSAGE* msg = NULL;

	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);
	if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		ret = EENL_ExtEnlDevSetPropertyValue(&msg, info->devBattery,
			0xC2u, val0xC2, 0u);
		if (ret == ENL_RESULT_SUCCESS) {
			DLOG_Debug("0xC2 : %d", val0xC2);
			if (msg != NULL) {
				/* 状変アナウンス通知 */
				EENL_SendMessage(NULL, msg);
				DLOG_Debug("#### send broadcast message (battery:0xC2) ####");
			} else {
				/* DO NOTHING */
			}
		} else {
			DLOG_Error("0xC2 set error!");
			/*  処理は継続 */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xCFの設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xCF			0xCF設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtNotify0xCF(uchar val0xCF)
{
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;
	ENL_MESSAGE* msg = NULL;

	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);
	if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		if (val0xCF != 0x00) {
			/* 設定値が確定している場合は、ENLに反映 */
			/* 0xCF */
			ret = EENL_ExtEnlDevSetPropertyValue(&msg, info->devBattery,
				0xCFu, val0xCF, 0u);
			if (ret == ENL_RESULT_SUCCESS) {
				/* 成功 */
				DLOG_Debug("battery 0xCF set success : 0x%02x", val0xCF);

				if (msg != NULL) {
					/*  状変アナウンスを通知 */
					EENL_SendMessage(NULL, msg);
					DLOG_Debug("#### send broadcast message (battery:0xCF) ####");
				} else {
					/* DO NOTHING */
				}
			} else {
				DLOG_Error("battery 0xCF set fail : 0x%02x(%ld)", val0xCF, ret);
			}
		} else {
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xDAの設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xDA			0xDA設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtNotify0xDA(uchar val0xDA)
{
	slong ret = ENL_RESULT_SUCCESS;
	slong eenl_ret = EENL_OK;
	EenlDeviceInfo_t* info = NULL;
	ENL_MESSAGE* msg = NULL;

	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);
	if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		if (val0xDA != 0x00u) {
			/*  ENL充放電制御チェック */
			eenl_ret = EENL_CheckEnlOpectrl(val0xDA);
			if (EENL_OK != eenl_ret) {
				DLOG_Error("check ENL remote OpeMode ERROR!(ret=%ld)", eenl_ret);
			} else {
				/* DO NOTHING */
			}

			/*  設定値が決まっている場合、ENLに反映 */
			/*  0xDA */
			ret = EENL_ExtEnlDevSetPropertyValue(&msg, info->devBattery,
				0xDAu, val0xDA, 0u);
			if (ret == ENL_RESULT_SUCCESS) {
				/* 成功 */
				DLOG_Debug("battery 0xDA set success : 0x%02x", val0xDA);

				if (msg != NULL) {
					/*  状変アナウンスを通知 */
					EENL_SendMessage(NULL, msg);
					DLOG_Debug("#### send broadcast message (battery:0xDA) ####");
				} else {
					/* DO NOTHING */
				}
			} else {
				/* 失敗 */
				DLOG_Error("battery 0xDA set fail : 0x%02x(%ld)", val0xDA, ret);
			}
		} else {
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}


/*==========================================================================*/
/*	Description		:蓄電池 0xDBの設定処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xDB	0xD8設定値								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtNotify0xDB(uchar val0xDB)
{
	slong					ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);
	if (NULL == info) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		if (val0xDB != 0xFF) {
			/*  設定値が決定している場合は、ENLに反映 */
			/*  蓄電池0xDB */
			if (info->devBattery != NULL) {
				ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
					0xDBu, val0xDB, 0u);
				if (ret == ENL_RESULT_SUCCESS) {
					/* 成功 */
					DLOG_Debug("[timer battery] 0xDB set success : 0x%02x",
						val0xDB);

					/* 状変アナウンスは無し */

				} else {
					/* 失敗 */
					DLOG_Error("[timer battery] 0xDB set fail : 0x%02x(%ld)",
						val0xDB, ret);
					/* 異常でも処理続行 */
				}
			} else {
				/* DO NOTHING */
			}
		} else {
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xE5の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xE5			0xE5設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtNotify0xE5(slong val0xE5)
{
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);
	if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		/*  0xE5 */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
			0xE5u, (ulong)val0xE5, 0u);
		if (ret == ENL_RESULT_SUCCESS) {
			/* 成功 */
			DLOG_Debug("battery 0xE5 set success : 0x%02lx(%ld)",
				val0xE5, val0xE5);

			/* 状変アナウンスは無し */

		} else {
			/* 失敗 */
			DLOG_Error("battery 0xE5 set fail : 0x%02lx(%ld) (%ld)",
				val0xE5, val0xE5, ret);
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xE6の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xE6			0xE6設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtNotify0xE6(slong val0xE6)
{
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);
	if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		/*  0xE6 */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
			0xE6u, (ulong)val0xE6, 0u);
		if (ret == ENL_RESULT_SUCCESS) {
			/* 成功 */
			DLOG_Debug("battery 0xE6 set success : 0x%02lx(%ld)",
				val0xE6, val0xE6);

			/* 状変アナウンスは無し */

		} else {
			/* 失敗 */
			DLOG_Error("battery 0xE6 set fail : 0x%02lx(%ld) (%ld)",
				val0xE6, val0xE6, ret);
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xE7/0xE8の設定処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cmsg			状態変化通知イベント構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:0xE7/0xE8は一旦0xD0をライブラリから取得して値を		*/
/*					 設定するため、											*/
/*					 0xE7/0xE8用の設定値を引数でもらわずにこの中で			*/
/*					 設定値を加工する										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtNotify0xE7_0xE8(const EenlEvtSttsChangeReq_t* cmsg)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong getval = 0u;
	EenlDeviceInfo_t* info = NULL;


	DEBUG_TRCIN();

	if (NULL == cmsg) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		info = EENL_GetDeviceInfoIndex(0);
		if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
			ret = ENL_RESULT_INVALID_PARAMETER;
		} else {
			/*  定格容量(0xD0)を取得 */
			ret = ENL_get_property_value(info->devBattery, 0xD0u, &getval, 0u);
			if (ret != ENL_RESULT_SUCCESS) {
				/* 失敗 */
				DLOG_Error("battery get 0xD0 error!:%ld", ret);
			} else {
				/* グローバル変数を更新してファイル保存 */
				g_eenlBattery0xE7 = getval;
				g_eenlBattery0xE8 = getval;
				EENL_FileWrite();

				DLOG_Debug("save file ok! (%ld, 0x%08lx)", getval, getval);

				/* einvに反映 */
				/*  充電電力量を定格容量に戻す (単位は[Wh]で変換不要) */
				ret = epcs_sync_setChargePwr(getval);
				if (ret == EPCS_OK) {
					/* 設定OK */
					DLOG_Debug("einv reset charge ok! (%ld, %08lx)", getval, getval);
				} else {
					/* 設定失敗 */
					DLOG_Error("einv reset charge NG! :%ld (%ld, %08lx)",
						ret, getval, getval);
					/*  処理は継続 */
				}

				/*  放電電力量を定格容量に戻す (単位は[Wh]で変換不要) */
				ret = epcs_sync_setDischargePwr(getval);
				if (ret == EPCS_OK) {
					/* 設定OK */
					DLOG_Debug("einv reset discharge ok! (%ld, %08lx)",
						getval, getval);
				} else {
					/* 設定失敗 */
					DLOG_Error("einv reset discharge NG! :%ld (%ld, %08lx)",
						ret, getval, getval);
					/*  処理は継続 */
				}

				/* ENLライブラリの充電量設定値1(0xE7)も定格容量に戻す */
				ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
					0xE7u, getval, 0u);
				if (ret == ENL_RESULT_SUCCESS) {
					DLOG_Debug("0xE7 : 0x%lx(%ld) ", getval, getval);
				} else {
					DLOG_Error("0xE7 set error!");
					/*  処理は継続 */
				}

				/* ENLライブラリの放電量設定値1(0xE8)も定格容量に戻す */
				ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
					0xE8u, getval, 0u);
				if (ret == ENL_RESULT_SUCCESS) {
					DLOG_Debug("0xE8 : 0x%lx(%ld)", getval, getval);

				} else {
					DLOG_Error("0xE8 set error!");
					/*  処理は継続 */
				}
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xF0の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xF0			0xF0設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtNotify0xF0(uchar val0xF0)
{
	slong					ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;
	ENL_MESSAGE* msg;


	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);
	if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		if (val0xF0 != 0x00u) {
			/*  設定値が決まっている場合、ENLに反映 */
			/*  0xF0 */
			ret = EENL_ExtEnlDevSetPropertyValue(&msg, info->devBattery,
				0xF0u, val0xF0, 0u);
			if (ret == ENL_RESULT_SUCCESS) {
				/* 成功 */
				DLOG_Debug("battery 0xF0 set success : 0x%02x", val0xF0);

				if (msg != NULL) {
					/*  状変アナウンスを通知 */
					EENL_SendMessage(NULL, msg);
					DLOG_Debug("#### send broadcast message (battery:0xF0) ####");
				} else {
					/* DO NOTHING */
				}
			} else {
				/* 失敗 */
				DLOG_Error("battery 0xF0 set fail : 0x%02x(%ld)", val0xF0, ret);
			}
		} else {
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xF2 or 0xF9の設定処理					*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xF2			0xF2設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtNotify0xF2(uchar val0xF2)
{
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;
	ENL_MESSAGE* msg = NULL;

	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);
	if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		/*  ニチコン仕向けは0xF2(非常時安心設定) */
		ret = EENL_ExtEnlDevSetPropertyValue(&msg, info->devBattery,
				0xF2u, val0xF2, 0u);

		/*  Setプロパティ失敗 */
		if (ret == ENL_RESULT_SUCCESS) {
			/*  状変アナウンス通知が作成されている場合はENLに送信する */
			if (msg != NULL) {
				/*  状変アナウンスを通知 */
				EENL_SendMessage(NULL, msg);
				DLOG_Debug("#### send broadcast message (battery:0xF2(0xF9)) ####");
			} else {
				/* DO NOTHING */
			}
		} else {
			/*  反映に失敗した場合はログ出しして抜ける */
			DLOG_Debug("battery 0xF2(0xF9) set failed.");
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xF4の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xF4_0	付属設定値							*/
/*					 [in]　val0xF4_1	付属設定値							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtNotify0xF4(uchar val0xF4_0, uchar val0xF4_1)
{
	slong ret = ENL_RESULT_SUCCESS;
	uchar buff0xF4[EENL_PDC_USER_0xF4_PUSHEDUP] = { 0u };	/*  蓄電池クラスユーザ定義の0xF4(自動運転設定)格納用変数 */
	EenlDeviceInfo_t* info = NULL;
	ENL_MESSAGE* msg = NULL;
	uchar bs = 0u;


	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);
	if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		if (val0xF4_0 != 0xFFu) {
			/*  設定値が決まっている場合、ENLに反映 */
			ret = eenl_BtNotify0xF4CreateData(buff0xF4, val0xF4_0, val0xF4_1);
			if (ENL_RESULT_SUCCESS == ret) {
				/* 押し上げ無し */
				bs = EENL_PDC_USER_0xF4;
				
				ret = EENL_ExtEnlDevSetProperty(&msg, info->devBattery,
					0xF4u, bs, buff0xF4);
				if (ret == ENL_RESULT_SUCCESS) {
					/* 成功 */
					DLOG_Debug("battery 0xF4 set success : 0x%02x %02x",
						buff0xF4[0], buff0xF4[1]);

					if (msg != NULL) {
						/*  状変アナウンスを通知 */
						EENL_SendMessage(NULL, msg);
						DLOG_Debug("#### send broadcast message (battery:0xF4) ####");
					} else {
						/* DO NOTHING */
					}

				} else {
					/* 失敗 */
					DLOG_Error("battery 0xF4 set fail : %ld", ret);
				}
			} else {
				/* DO NOTHING */
			}

		} else {
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}


/*==========================================================================*/
/*	Description		:蓄電池クラス 0xF4 0xF8の設定処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cmsg	状態変化通知構造体							*/
/*					 [in]　mode	モード										*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtNotify0xF4_0xF8(const EenlEvtSttsChangeReq_t* cmsg,
	uchar mode)
{
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;
	ENL_MESSAGE* msg = NULL;
	uchar bs = 0u;
	uchar val0xF4[EENL_PDC_USER_0xF4_PUSHEDUP] = { 0u };


	DEBUG_TRCIN();

	if (NULL == cmsg) {
		DEBUG_TRCOUT();
		return ENL_RESULT_INVALID_PARAMETER;
	} else {
		/* DO NOTHING */
	}

	info = EENL_GetDeviceInfoIndex(0);
	if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		/* DO NOTHING */
	}

	/*  正常の場合 */
	if (ENL_RESULT_SUCCESS == ret) {
		/*  0xF4 */
		if (mode != 0) {
			/* 0xF4 */
			ret = eenl_BtNotify0xF4CreateDataMatch(cmsg, val0xF4, mode);
			if (ENL_RESULT_SUCCESS == ret) {

				/* 更新がある場合 */
				/* 押し上げ無し */
				bs = EENL_PDC_USER_0xF4;
				
				ret = EENL_ExtEnlDevSetProperty(&msg, info->devBattery,
					0xF4u, bs, val0xF4);
				if (ret == ENL_RESULT_SUCCESS) {
					/* 成功 */
					DLOG_Debug("battery 0xF4 set success ");

					if (msg != NULL) {
						/* 状変アナウンス通知 */
						EENL_SendMessage(NULL, msg);
						DLOG_Debug("#### send broadcast message (battery:0xF4) ####");
					} else {
						/* DO NOTHING */
					}
				} else {
					/* 失敗 */
					DLOG_Error("battery 0xF4 set fail : %ld", ret);
					/* 失敗でも処理続行 */
				}

			} else {
				/* DO NOTHING */
			}
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xF8の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0xF8	0xF8設定値								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtNotify0xF8(uchar* val0xF8)
{
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;
	ENL_MESSAGE* msg = NULL;


	DEBUG_TRCIN();

	if (NULL == val0xF8) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		info = EENL_GetDeviceInfoIndex(0);
		if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
			ret = ENL_RESULT_INVALID_PARAMETER;
		} else {
			ret = EENL_ExtEnlDevSetProperty(&msg, info->devBattery,
				0xF8u, EENL_PDC_USER_0xF8, val0xF8);
			if (ret == ENL_RESULT_SUCCESS) {
				/* 成功 */
				DLOG_Debug("battery 0xF8 set success.0xF8[0]=%d", val0xF8[0]);
				if (msg != NULL) {
					/* 状変アナウンス通知(状変アナウンス通知に対応している場合のみ送信される) */
					EENL_SendMessage(NULL, msg);
					DLOG_Debug("#### send broadcast message (battery:0xF8) ####");
				} else {
					/* DO NOTHING */
				}
			} else {
				/* 失敗 */
				DLOG_Error("battery 0xF8 ENLLib error!:%ld", ret);
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xA0/0xA1/0xA2/0xA3/0xA4/0xA5/0xA8/0xA9	*/
/*					 の設定処理												*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　ope	動作情報構造体								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					 ENL_RESULT_SUCCESSで返す								*/
/*					 内部で使用するためNULLになり得ないのと、				*/
/*					 静的解析対応のため、									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtTimer0xA0_0xA1_0xA2_0xA3_0xA4_0xA5_0xA8_0xA9(
	const EPCS_OPESTTS_INFO_T* ope)
{
	ulong cnt = 0u;
	slong ret = ENL_RESULT_SUCCESS;
	ulong setval = 0u;
	uchar setepc[8] = { 0xA0u, 0xA1u, 0xA2u, 0xA3u, 0xA4u, 0xA5u, 0xA8u, 0xA9u };
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	if (NULL == ope) {
		/* DO NOTHING */
	} else {
		info = EENL_GetDeviceInfoIndex(0);
		if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
			/* DO NOTHING */
		} else {
			/* AC 実効容量（充電）:0xA0 */
			/* AC 実効容量（放電）:0xA1 */
			/* AC 充電可能容量:0xA2 */
			/* AC 放電可能容量:0xA3 */
			/* AC 充電可能量:0xA4 */
			/* AC 放電可能量:0xA5 */
			/* AC 積算充電電力量計測値:0xA8 */
			/* AC 積算放電電力量計測値:0xA9 */
			/* 充電電力設定値:0xEB →更新しない(Get要求時にeinvから設定値を取得) */
			/* 放電電力設定値:0xEC →更新しない(Get要求時にeinvから設定値を取得) */
			for (cnt = 0; cnt < sizeof(setepc); cnt++) {

				switch (setepc[cnt]) {
				case 0xA0u:
					setval = ope->acChargeEffectiveCapacity;
					DLOG_Debug("acChargeEffectiveCapacity : %ld", setval);
					break;
				case 0xA1u:
					setval = ope->acDischargeEffectiveCapacity;
					DLOG_Debug("acDischargeEffectiveCapacity : %ld", setval);
					break;
				case 0xA2u:
					setval = ope->acChargePossibleCapacity;
					DLOG_Debug("acChargePossibleCapacity : %ld", setval);
					break;
				case 0xA3u:
					setval = ope->acDishargePossibleCapacity;
					DLOG_Debug("acDishargePossibleCapacity : %ld", setval);
					break;
				case 0xA4u:
					setval = ope->acChargePossibleAmount;
					DLOG_Debug("acChargePossibleAmount : %ld", setval);
					break;
				case 0xA5u:
					setval = ope->acDishargePossibleAmount;
					DLOG_Debug("acDishargePossibleAmount : %ld", setval);
					break;
				case 0xA8u:
					/* 範囲 0x000000000～0x3B9AC9FF(0～999,999,999) */
					/* einvでECHONET規格の最大値超過がありえる */
					/* オーバーフロー時に0から再インクリメントとなるように補正する必要があるため、 */
					/* 常に規格上の最大値+1で剰余算する */
					/*  単位は異なるが格納値は同じ(Wh = 0.001kWh) */
					setval = ope->ttlChargeEnergy % EENL_LIMIT_1000000000;
					DLOG_Debug("ttlChargeEnergy : %ld", setval);
					break;
				case 0xA9u:
					/* 範囲 0x000000000～0x3B9AC9FF(0～999,999,999) */
					/* einvでECHONET規格の最大値超過がありえる */
					/* オーバーフロー時に0から再インクリメントとなるように補正する必要があるため 、 */
					/* 常に規格上の最大値+1で剰余算する */
					/*  単位は異なるが格納値は同じ(Wh = 0.001kWh) */
					setval = ope->ttlDischargeEnergy % EENL_LIMIT_1000000000;
					DLOG_Debug("ttlDischargeEnergy : %ld", setval);
					break;
				default:
					/* ここに到達するのは不具合以外あり得ない */
					DLOG_Error("0x%X eenl unexpected error!", setepc[cnt]);
					break;
				}

				ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
					setepc[cnt], setval, 0u);
				if (ret == ENL_RESULT_SUCCESS) {
					switch (setepc[cnt]) {
					case 0xA0u:
						DLOG_Debug("[AC charge effective capacity] 0x%X limit: none 0x%08lx(%ld)",
							setepc[cnt], setval, setval);
						break;
					case 0xA1u:
						DLOG_Debug("[AC discharge effective capacity] 0x%X limit: none 0x%08lx(%ld)",
							setepc[cnt], setval, setval);
						break;
					case 0xA2u:
						DLOG_Debug("[AC charge possible capacity] 0x%X limit: none 0x%08lx(%ld)",
							setepc[cnt], setval, setval);
						break;
					case 0xA3u:
						DLOG_Debug("[AC discharge possible capacity] 0x%X limit: none 0x%08lx(%ld)",
							setepc[cnt], setval, setval);
						break;
					case 0xA4u:
						DLOG_Debug("[AC charge possible amount] 0x%X limit: none 0x%08lx(%ld)",
							setepc[cnt], setval, setval);
						break;
					case 0xA5u:
						DLOG_Debug("[AC discharge possible amount] 0x%X limit: none 0x%08lx(%ld)",
							setepc[cnt], setval, setval);
						break;
					case 0xA8u:
						DLOG_Debug("[AC charge energy] 0x%X limit: none 0x%08lx(%ld)",
							setepc[cnt], setval, setval);
						break;
					case 0xA9u:
						DLOG_Debug("[AC discharge energy] 0x%X limit: none 0x%08lx(%ld)",
							setepc[cnt], setval, setval);
						break;
					default:
						/* ここに到達するのは不具合以外あり得ない */
						DLOG_Error("0x%X eenl unexpected error!", setepc[cnt]);
						break;
					}
					/* 状変アナウンスはなし */

				} else {
					switch (setepc[cnt]) {
					case 0xA0:
						DLOG_Debug("[AC charge effective capacity] 0x%X pcs error!:%ld",
							setepc[cnt], ret);
						break;
					case 0xA1u:
						DLOG_Debug("[AC discharge effective capacity] 0x%X pcs error!:%ld",
							setepc[cnt], ret);
						break;
					case 0xA2u:
						DLOG_Debug("[AC charge possible capacity] 0x%X pcs error!:%ld",
							setepc[cnt], ret);
						break;
					case 0xA3u:
						DLOG_Debug("[AC discharge possible capacity] 0x%X pcs error!:%ld",
							setepc[cnt], ret);
						break;
					case 0xA4u:
						DLOG_Debug("[AC charge possible amount] 0x%X pcs error!:%ld",
							setepc[cnt], ret);
						break;
					case 0xA5u:
						DLOG_Debug("[AC discharge possible amount] 0x%X pcs error!:%ld",
							setepc[cnt], ret);
						break;
					case 0xA8u:
						DLOG_Debug("[AC charge energy] 0x%X pcs error!:%ld",
							setepc[cnt], ret);
						break;
					case 0xA9u:
						DLOG_Debug("[AC discharge energy] 0x%X pcs error!:%ld",
							setepc[cnt], ret);
						break;
					default:
						/* ここに到達するのは不具合以外あり得ない */
						DLOG_Error("0x%X eenl unexpected error!", setepc[cnt]);
						break;
					}
					/* 異常でも処理続行 */
				}
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xD3の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　ope	動作情報構造体								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					 ENL_RESULT_SUCCESSで返す								*/
/*					 内部で使用するためNULLになり得ないのと、				*/
/*					 静的解析対応のため、									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtTimer0xD3(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	slong val0xD3 = 0;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	if (NULL == ope) {
		/* DO NOTHING */
	} else {
		info = EENL_GetDeviceInfoIndex(0);
		if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
			/* DO NOTHING */
		} else {
			DLOG_Debug("chargeDischargePwr2 : %ld", ope->chargeDischargePwr2);

			val0xD3 = ope->chargeDischargePwr2;
			DLOG_Debug("[timer battery] 0xD3 limit: none (%ld)", val0xD3);

			ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
				0xD3u, (ulong)val0xD3, 0u);
			if (ret == ENL_RESULT_SUCCESS) {
				DLOG_Debug("[timer battery] 0xD3 : 0x%08lx(%ld)", val0xD3, val0xD3);

				/* 状変アナウンスはなし */

			} else {
				DLOG_Error("[timer battery] 0xD3 pcs error!:%ld", ret);
				/* 異常でも処理続行 */
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xD6の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　ope	動作情報構造体								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					 ENL_RESULT_SUCCESSで返す								*/
/*					 内部で使用するためNULLになり得ないのと、				*/
/*					 静的解析対応のため、									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtTimer0xD6(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong val0xD6 = 0u;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	if (NULL == ope) {
		/* DO NOTHING */
	} else {
		info = EENL_GetDeviceInfoIndex(0);
		if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
			/* DO NOTHING */
		} else {
			DLOG_Debug("ttlDischargeEnergy2 : %ld", ope->ttlDischargeEnergy2);

			val0xD6 = ope->ttlDischargeEnergy2;
			if (val0xD6 > EENL_LIMIT_999999999) {
				/* 999999999より大きい場合 */
				while (val0xD6 > EENL_LIMIT_999999999) {
					val0xD6 = val0xD6 - 1000000000u;
				}
				DLOG_Debug("rotate : %ld -> 0x%08lx(%ld)", ope->ttlDischargeEnergy2,
					val0xD6, val0xD6);

			} else {
				/* 999999999以下の場合 */
			}

			ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
				0xD6u, val0xD6, 0u);
			if (ret == ENL_RESULT_SUCCESS) {
				DLOG_Debug("[timer battery] 0xD6 : 0x%04lx(%ld)", val0xD6, val0xD6);

				/* 状変アナウンスはなし */

			} else {
				DLOG_Error("[timer battery] 0xD6 pcs error!:%ld", ret);
				/* 異常でも処理続行 */
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xD8の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　ope	動作情報構造体								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					 ENL_RESULT_SUCCESSで返す								*/
/*					 内部で使用するためNULLになり得ないのと、				*/
/*					 静的解析対応のため、									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtTimer0xD8(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong val0xD8 = 0u;
	EenlDeviceInfo_t* info = NULL;


	DEBUG_TRCIN();

	if (NULL == ope) {
		/* DO NOTHING */
	} else {
		info = EENL_GetDeviceInfoIndex(0);
		if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
			/* DO NOTHING */
		} else {
			DLOG_Debug("ttlChargeEnergy2 : %ld", ope->ttlChargeEnergy2);

			val0xD8 = ope->ttlChargeEnergy2;

			if (val0xD8 > EENL_LIMIT_999999999) {
				/* 999999999より大きい場合 */
				while (val0xD8 > EENL_LIMIT_999999999) {
					val0xD8 = val0xD8 - 1000000000u;
				}
				DLOG_Debug("rotate : %ld -> 0x%08lx(%ld)", ope->ttlChargeEnergy2,
					val0xD8, val0xD8);
			} else {
				/* 999999999以下の場合 */
			}

			ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
				0xD8u, val0xD8, 0u);
			if (ret == ENL_RESULT_SUCCESS) {
				DLOG_Debug("[timer battery] 0xD8 : 0x%04lx(%ld)", val0xD8, val0xD8);

				/* 状変アナウンスはなし */

			} else {
				DLOG_Error("[timer battery] 0xD8 pcs error!:%ld", ret);
				/* 異常でも処理続行 */
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xE2の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　ope	動作情報構造体								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					 ENL_RESULT_SUCCESSで返す								*/
/*					 内部で使用するためNULLになり得ないのと、				*/
/*					 静的解析対応のため、									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtTimer0xE2(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong val0xE2 = 0u;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	if (NULL == ope) {
		/* DO NOTHING */
	} else {
		info = EENL_GetDeviceInfoIndex(0);
		if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
			/* DO NOTHING */
		} else {
			DLOG_Debug("remainingBtW : %ld", ope->remainingBtW);

			/*  pcsからのデータは0.1kWh単位なので、Wh単位に変換 */
			val0xE2 = (ulong)(ope->remainingBtW * 100);

			ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
				0xE2u, val0xE2, 0u);
			if (ret == ENL_RESULT_SUCCESS) {
				DLOG_Debug("[timer battery] 0xE2 : %ld -> %ld",
					ope->remainingBtW, val0xE2);

				/* 状変アナウンスはなし */

			} else {
				DLOG_Error("[timer battery] 0xE2 pcs error!:%ld", ret);
				/* 異常でも処理続行 */
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xE4の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　ope	動作情報構造体								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					 ENL_RESULT_SUCCESSで返す								*/
/*					 内部で使用するためNULLになり得ないのと、				*/
/*					 静的解析対応のため、									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtTimer0xE4(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong val0xE4 = 0u;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	if (NULL == ope) {
		/* DO NOTHING */
	} else {
		info = EENL_GetDeviceInfoIndex(0);
		if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
			/* DO NOTHING */
		} else {
			DLOG_Debug("soc : %ld", ope->soc);

			/*  pcsからのデータは0.1%単位なので、%単位に変換 */
			val0xE4 = (ulong)((ope->soc + 5u) / 10u);

			ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
				0xE4u, val0xE4, 0u);
			if (ret == ENL_RESULT_SUCCESS) {
				DLOG_Debug("[timer battery] 0xE4 : %ld -> %ld",
					ope->soc, val0xE4);

				/* 状変アナウンスはなし */

			} else {
				DLOG_Error("[timer battery] 0xE4 pcs error!:%ld", ret);
				/* 異常でも処理続行 */
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xE5の設定処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　ope	動作情報構造体								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:timerのパラメタチェックでエラーの場合は、				*/
/*					 ENL_RESULT_SUCCESSで返す								*/
/*					 内部で使用するためNULLになり得ないのと、				*/
/*					 静的解析対応のため、									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BtTimer0xE5(const EPCS_OPESTTS_INFO_T* ope)
{
	slong ret = ENL_RESULT_SUCCESS;
	ulong val0xE5 = 0u;
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	if (NULL == ope) {
		/* DO NOTHING */
	} else {
		info = EENL_GetDeviceInfoIndex(0);
		if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
			/* DO NOTHING */
		} else {
			DLOG_Debug("soh : %ld", ope->soh);

			/*  pcsからのデータは0.1%単位なので、%単位に変換 */
			val0xE5 = (ulong)((ope->soh + 5u) / 10u);

			ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
				0xE5u, val0xE5, 0u);
			if (ret == ENL_RESULT_SUCCESS) {
				DLOG_Debug("[timer battery] 0xE5 : %ld -> %ld", ope->soh, val0xE5);

				/* 状変アナウンスはなし */

			} else {
				DLOG_Error("[timer battery] 0xE5 pcs error!:%ld", ret);
				/* 異常でも処理続行 */
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*==========================================================================*/
/*	Description		:蓄電池クラス 0xC8(最大/最小充電電力値)の反映処理		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　regInfo	蓄電池の登録情報						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS(0):正常終了/0未満:異常(enl.h参照))	*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtUpdate0xC8(EenlDeviceInfo_t* info, EPCS_REG_INFO_T* regInfo)
{
	slong	ret = 0;
	u64	val0xC8[2] = { 0u, 0u };

	DEBUG_TRCIN();

	/*  PCSは0.01kW単位なので、W単位に変換 */
	val0xC8[0] = (u64)regInfo->chargePowerValueMin * (u64)10;
	if (val0xC8[0] > (u64)EENL_LIMIT_999999999) {
		val0xC8[0] = (u64)EENL_LIMIT_999999999;
	} else {
		/* DO NOTHING */
	}

	val0xC8[1] = (u64)regInfo->chargePowerValueMax * (u64)10;
	if (val0xC8[1] > (u64)EENL_LIMIT_999999999) {
		val0xC8[1] = (u64)EENL_LIMIT_999999999;
	} else {
		/* DO NOTHING */
	}

	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
		0xC8u, (ulong)val0xC8[0], 0u);
	if (ret == ENL_RESULT_SUCCESS) {
		DLOG_Debug("[einv battery] 0xC8 : Min 0x%llx(%lld)",
			val0xC8[0], val0xC8[0]);
	} else {
		DLOG_Error("battery 0xC8 Min set error!:%ld", ret);
	}

	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
		0xC8u, (ulong)val0xC8[1], 1u);
	if (ret == ENL_RESULT_SUCCESS) {
		DLOG_Debug("[einv battery] 0xC8 : Max 0x%llx(%lld)",
			val0xC8[1], val0xC8[1]);
	} else {
		DLOG_Error("battery 0xC8 Max set error!:%ld", ret);
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xC9(最大/最小放電電力値)の設定処理		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　regInfo	蓄電池の登録情報						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS(0):正常終了/0未満:異常(enl.h参照))	*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtUpdate0xC9(EenlDeviceInfo_t* info, EPCS_REG_INFO_T* regInfo)
{
	slong ret = 0;
	u64	val0xC9[2] = { 0u, 0u };

	DEBUG_TRCIN();

	/*  PCSは0.01kW単位なので、W単位に変換 */
	val0xC9[0] = (u64)regInfo->dischargePowerValueMin * (u64)10;
	if (val0xC9[0] > (u64)EENL_LIMIT_999999999) {
		val0xC9[0] = (u64)EENL_LIMIT_999999999;
	} else {
		/* DO NOTHING */
	}

	val0xC9[1] = (u64)regInfo->dischargePowerValueMax * (u64)10;
	if (val0xC9[1] > (u64)EENL_LIMIT_999999999) {
		val0xC9[1] = (u64)EENL_LIMIT_999999999;
	} else {
		/* DO NOTHING */
	}

	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
		0xC9u, (ulong)val0xC9[0], 0u);
	if (ret == ENL_RESULT_SUCCESS) {
		DLOG_Debug("[einv battery] 0xC9 : Min 0x%llx(%lld)",
			val0xC9[0], val0xC9[0]);
	} else {
		DLOG_Error("battery 0xC9 Min set error!:%ld", ret);
	}

	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
		0xC9u, (ulong)val0xC9[1], 1u);
	if (ret == ENL_RESULT_SUCCESS) {
		DLOG_Debug("[einv battery] 0xC9 : Max 0x%llx(%lld)",
			val0xC9[1], val0xC9[1]);
	} else {
		DLOG_Error("battery 0xC9 Max set error!:%ld", ret);
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xD0(定格電力量)の設定処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　regInfo	蓄電池の登録情報						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS(0):正常終了/0未満:異常(enl.h参照))	*/
/*--------------------------------------------------------------------------*/
/*	detail			:infoの情報については上位から渡されたものをそのまま		*/
/*					 使用し、本関数内で再取得はしない						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtUpdate0xD0_0xE7_0xE8(EenlDeviceInfo_t* info,
	EPCS_REG_INFO_T* regInfo)
{
	slong	ret = 0;
	ulong	val0xD0 = 0u;

	DEBUG_TRCIN();

	/*  PCSの定格電力量は0.1kWh単位なので、Wh単位に変更 */
	val0xD0 = (ulong)(regInfo->btCapacity * 100u);

	if (val0xD0 > EENL_LIMIT_999999999) {
		/* 999999999より大きい場合、999999999に制限 */
		val0xD0 = EENL_LIMIT_999999999;
	} else {
		/* DO NOTHING */
	}

	DLOG_Debug("btCapacity : 0x%08lx (0x%02x)", val0xD0, regInfo->btCapacity);

	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
		0xD0u, val0xD0, 0u);
	if (ret == ENL_RESULT_SUCCESS) {
		DLOG_Debug("[einv battery] 0xD0 : 0x%lx(%ld)", val0xD0, val0xD0);

	} else {
		DLOG_Error("battery 0xD0 pcs error!:%ld", ret);
	}

	if (g_eenlBattery0xE7 == 0xFFFFFFFF) {
		/*  0xE7が初期状態の場合、定格電力量を設定 */

		/* 0xE7 */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
			0xE7u, val0xD0, 0u);
		if (ret == ENL_RESULT_SUCCESS) {
			DLOG_Debug("[einv battery] 0xE7 : 0x%lx(%ld)", val0xD0, val0xD0);

		} else {
			DLOG_Error("battery 0xE7 pcs error!:%ld", ret);
		}
		/* グローバル変数を更新 */
		g_eenlBattery0xE7 = val0xD0;

		/* 情報をローカルに保存 */
		EENL_FileWrite();

	} else {
		/* 初期状態以外の場合は、何もしない */
	}

	if (g_eenlBattery0xE8 == 0xFFFFFFFFu) {
		/*  0xE8が初期状態の場合、定格電力量を設定 */

		/* 0xE8 */
		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
			0xE8u, val0xD0, 0u);
		if (ret == ENL_RESULT_SUCCESS) {
			DLOG_Debug("[einv battery] 0xE8 : 0x%lx(%ld)", val0xD0, val0xD0);

		} else {
			DLOG_Error("battery 0xE8 pcs error!:%ld", ret);
		}
		/* グローバル変数を更新 */
		g_eenlBattery0xE8 = val0xD0;

		/* 情報をローカルに保存 */
		EENL_FileWrite();

	} else {
		/* 初期状態以外の場合は、何もしない */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xD2(定格電圧)の反映処理					*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　regInfo	蓄電池の登録情報						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS(0):正常終了/0未満:異常(enl.h参照))	*/
/*--------------------------------------------------------------------------*/
/*	detail			:infoの情報については上位から渡されたものをそのまま		*/
/*					 使用し、本関数内で再取得はしない						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtUpdate0xD2(EenlDeviceInfo_t* info, EPCS_REG_INFO_T* regInfo)
{
	slong	ret = 0;
	ushort	val0xD2 = 0u;

	DEBUG_TRCIN();

	/*  PCSは0.1V単位なので、V単位に変換 */
	 /*  四捨五入の為に0.5加算 */
	val0xD2 = (ushort)((regInfo->btRatedVolt + 5u) / 10u);
	if (val0xD2 > 0x7FFEu) {
		val0xD2 = 0x7FFEu;
	} else {
		/* DO NOTHING */
	}

	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
		0xD2u, (ushort)val0xD2, 0u);
	/* infoがNULLの場合メモリ不正なのでプロセス強制終了するが意図通り */
	if (ret == ENL_RESULT_SUCCESS) {
		DLOG_Debug("[einv battery] 0xD2 : 0x%X (pcs: 0x%X)",
			val0xD2, regInfo->btRatedVolt);
	} else {
		DLOG_Error("battery 0xD2 pcs error!:%ld", ret);
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xD3(瞬時充放電電力)の反映処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　opeStts	PCSの動作状態							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS(0):正常終了/0未満:異常(enl.h参照))	*/
/*--------------------------------------------------------------------------*/
/*	detail			:infoの情報については上位から渡されたものをそのまま		*/
/*					 使用し、本関数内で再取得はしない						*/
/*					 仕向け差分修正済み										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtUpdate0xD3(EenlDeviceInfo_t* info, EPCS_OPESTTS_INFO_T* opeStts)
{
	slong	ret = 0;
	slong	val0xD3 = 0;

	DEBUG_TRCIN();
	DLOG_Debug("chargeDischargePwr2 : %ld", opeStts->chargeDischargePwr2);

	info = EENL_GetDeviceInfoIndex(0);
	if (info != NULL) {
		val0xD3 = opeStts->chargeDischargePwr2;
		DLOG_Debug("[einv battery] 0xD3 limit: none (%ld)", val0xD3);

		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
			0xD3u, (ulong)val0xD3, 0u);
		if (ret == ENL_RESULT_SUCCESS) {
			DLOG_Debug("[einv battery] 0xD3 : %08lx", val0xD3);

			/* 状変アナウンスはなし */

		} else {
			DLOG_Debug("battery 0xD3 pcs error!:%ld", ret);
		}
	} else {
		DLOG_Debug("battery 0xD3 pcs error!: null");
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xD6(積算放電電力量測定値)の反映処理		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　opeStts	PCSの動作状態							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS(0):正常終了/0未満:異常(enl.h参照))	*/
/*--------------------------------------------------------------------------*/
/*	detail			:infoの情報については上位から渡されたものをそのまま		*/
/*					 使用し、本関数内で再取得はしない						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtUpdate0xD6(EenlDeviceInfo_t* info, EPCS_OPESTTS_INFO_T* opeStts)
{
	slong	ret = 0;
	ulong val0xD6 = 0u;

	DLOG_Debug("ttlDischargeEnergy2 : %ld", opeStts->ttlDischargeEnergy2);
	DEBUG_TRCIN();

	/* 	ulong ttlDischargeEnergy2; */
	/*  積算放電電力量[Wh] */
	val0xD6 = opeStts->ttlDischargeEnergy2;

	if (val0xD6 > EENL_LIMIT_999999999) {
		/*  999999999より大きい場合、ローテーション処理 */
		while (val0xD6 > EENL_LIMIT_999999999) {
			val0xD6 = val0xD6 - 1000000000u;
		}
		DLOG_Debug("rotate : %ld -> 0x%08lx(%ld)",
			opeStts->ttlDischargeEnergy2, val0xD6, val0xD6);

	} else {
		/*  999999999以下の場合 */

	}

	info = EENL_GetDeviceInfoIndex(0);
	if (info != NULL) {

		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
			0xD6u, val0xD6, 0u);
		if (ret == ENL_RESULT_SUCCESS) {
			DLOG_Debug("[einv battery] 0xD6 : 0x%08lx(%ld)",
				val0xD6, val0xD6);

			/* 状変アナウンスはなし */

		} else {
			DLOG_Debug("battery 0xD6 pcs error!:%ld", ret);
		}
	} else {
		DLOG_Debug("battery 0xD6 pcs error!: null");
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xD8(瞬時充放電電力)の反映処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　opeStts	PCSの動作状態							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS(0):正常終了/0未満:異常(enl.h参照))	*/
/*--------------------------------------------------------------------------*/
/*	detail			:infoの情報については上位から渡されたものをそのまま		*/
/*					 使用し、本関数内で再取得はしない						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtUpdate0xD8(EenlDeviceInfo_t* info, EPCS_OPESTTS_INFO_T* opeStts)
{
	slong	ret = 0;
	ulong val0xD8 = 0u;

	DEBUG_TRCIN();
	DLOG_Debug("ttlChargeEnergy2 : %ld", opeStts->ttlChargeEnergy2);


	/*  ulong ttlChargeEnergy2;	 */
	/*  積算充電電力量[Wh] */
	val0xD8 = opeStts->ttlChargeEnergy2;
	if (val0xD8 > EENL_LIMIT_999999999) {
		/* 999999999より大きい場合、ローテーション処理 */
		while (val0xD8 > EENL_LIMIT_999999999) {
			val0xD8 = val0xD8 - 1000000000u;
		}
		DLOG_Debug("rotate : %ld -> 0x%lx(%ld)", opeStts->ttlChargeEnergy2,
			val0xD8, val0xD8);

	} else {
		/* 999999999以下の場合 */

	}

	info = EENL_GetDeviceInfoIndex(0);
	if (info != NULL) {

		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
			0xD8u, val0xD8, 0u);
		if (ret == ENL_RESULT_SUCCESS) {
			DLOG_Debug("[einv battery] 0xD8 : 0x%08lx(%ld)",
				val0xD8, val0xD8);

			/* 状変アナウンスはなし */

		} else {
			DLOG_Error("battery 0xD8 pcs error!:%ld", ret);
		}
	} else {
		DLOG_Error("battery 0xD8 pcs error!: null");
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xE2(蓄電残量1 "Wh")の反映処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　opeStts	PCSの動作状態							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS(0):正常終了/0未満:異常(enl.h参照))	*/
/*--------------------------------------------------------------------------*/
/*	detail			:infoの情報については上位から渡されたものをそのまま		*/
/*					 使用し、本関数内で再取得はしない						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtUpdate0xE2(EenlDeviceInfo_t* info, EPCS_OPESTTS_INFO_T* opeStts)
{
	slong	ret = 0;
	ulong	val0xE2 = 0u;

	DLOG_Debug("remainingBtW : %ld", opeStts->remainingBtW);
	DEBUG_TRCIN();

	/*  ushort remainingBtW;*/
	/* 電池残量[0.1kWh] */
	/*  pcsからのデータは0.1kWh単位なので、Wh単位に変換 */
	val0xE2 = opeStts->remainingBtW * 100u;

	info = EENL_GetDeviceInfoIndex(0);
	if (info != NULL) {

		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
			0xE2u, val0xE2, 0u);
		if (ret == ENL_RESULT_SUCCESS) {
			DLOG_Debug("[einv battery] 0xE2 : %ld -> %ld",
				opeStts->remainingBtW, val0xE2);

			/* 状変アナウンスはなし */

		} else {
			DLOG_Error("battery 0xE2 pcs error!:%ld", ret);
		}
	} else {
		DLOG_Error("battery 0xE2 pcs error!: null");
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xE4(蓄電残量3 "%")の反映処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　opeStts	PCSの動作状態							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS(0):正常終了/0未満:異常(enl.h参照))	*/
/*--------------------------------------------------------------------------*/
/*	detail			:infoの情報については上位から渡されたものをそのまま		*/
/*					 使用し、本関数内で再取得はしない						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtUpdate0xE4(EenlDeviceInfo_t* info, EPCS_OPESTTS_INFO_T* opeStts)
{
	slong	ret = 0;
	ulong	val0xE4 = 0u;

	DEBUG_TRCIN();
	DLOG_Debug("soc : %ld", opeStts->soc);

	/*  ushort soc;		 */
	/*  SOC値[0.1%] */
	/*  pcsからのデータは0.1%単位なので、%単位に変換 */
	val0xE4 = (opeStts->soc + 5u) / 10u;

	info = EENL_GetDeviceInfoIndex(0);
	if (info != NULL) {

		ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
			0xE4u, val0xE4, 0u);
		if (ret == ENL_RESULT_SUCCESS) {
			DLOG_Debug("[einv battery] 0xE4 : %ld -> %ld",
				opeStts->soc, val0xE4);

			/* 状変アナウンスはなし */

		} else {
			DLOG_Error("battery 0xE4 pcs error!:%ld", ret);
		}
	} else {
		DLOG_Error("battery 0xE4 pcs error!: null");
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xDA(ユーザ運転モード設定)の反映処理		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　ope		PCSの運転モード							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS(0):正常終了/0未満:異常(enl.h参照))	*/
/*--------------------------------------------------------------------------*/
/*	detail			:infoの情報については上位から渡されたものをそのまま		*/
/*					 使用し、本関数内で再取得はしない						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtUpdate0xDA(EenlDeviceInfo_t* info, EPCS_OPEMODE_GET_T* ope)
{
	slong ret = 0;
	ENL_MESSAGE* msg = NULL;
	uchar val_0xDA = 0xFFu;		/*  0xDA(運転モード設定) */

	DEBUG_TRCIN();

	/* 運転モードから設定値を判定 */
	val_0xDA = eenl_BtUpdateRetVal0xDA(ope->opeMode);
	if (val_0xDA != 0xFFu) {
		/*  設定値が決定している場合は、ENLに反映 */

		info = EENL_GetDeviceInfoIndex(0);
		if (info != NULL) {

			/* 0xDA */
			ret = EENL_ExtEnlDevSetPropertyValue(&msg, info->devBattery,
				0xDAu, val_0xDA, 0u);
			if (ret == ENL_RESULT_SUCCESS) {
				DLOG_Debug("[einv battery k] 0xDA : %02x -> 0x%02x",
					ope->opeMode, val_0xDA);

				if (msg != NULL) {
					/* 状変アナウンス通知 */
					EENL_SendMessage(NULL, msg);
					DLOG_Debug("#### send broadcast message (battery:0xDA) ####");
				} else {
					/* DO NOTHING */
				}

			} else {
				DLOG_Error("battery 0xDA pcs error!:%ld", ret);
			}

		} else {
			DLOG_Error("battery 0xDA pcs error!: null");
		}

	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xDB(系統連系状態)の反映処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　opeStts	PCSの動作状態							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS(0):正常終了/0未満:異常(enl.h参照))	*/
/*--------------------------------------------------------------------------*/
/*	detail			:infoの情報については上位から渡されたものをそのまま		*/
/*					 使用し、本関数内で再取得はしない						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtUpdate0xDB(EenlDeviceInfo_t* info, EPCS_OPESTTS_INFO_T* opeStts)
{
	slong	ret = 0;
	uchar	val0xDB = 0xFFu;

	DEBUG_TRCIN();
	DLOG_Debug("pwrCut : %d", opeStts->pwrCut);

	/*  uchar pwrCut; */
	/*  停電状態 */
	if (opeStts->pwrCut == EPCS_PWROUTAGE_YES) {
		/*  停電の場合 */
		val0xDB = 0x01u;

	} else {
		/*  停電以外の場合 */
		val0xDB = 0x02u;
	}

	if (val0xDB != 0xFFu) {
		/*  設定値が決定している場合は、ENLに反映 */

		info = EENL_GetDeviceInfoIndex(0);
		if (info != NULL) {

			/* 0xDB:系統連系状態（蓄電池） */
			ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
				0xDBu, val0xDB, 0u);

			if (ret == ENL_RESULT_SUCCESS) {
				DLOG_Debug("[einv battery] 0xDB : %d -> 0x%02x",
					opeStts->pwrCut, val0xDB);

				/* 状変アナウンスはなし */

			} else {
				DLOG_Error("battery 0xDB pcs error!:%ld", ret);
			}
		} else {
			DLOG_Error("battery 0xDB pcs error!: null");
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xDC(独立時 最小最大充電電力用)の反映処理	*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　regInfo	蓄電池の登録情報						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS(0):正常終了/0未満:異常(enl.h参照))	*/
/*--------------------------------------------------------------------------*/
/*	detail			:infoの情報については上位から渡されたものをそのまま		*/
/*					 使用し、本関数内で再取得はしない						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtUpdate0xDC(EenlDeviceInfo_t* info, EPCS_REG_INFO_T* regInfo)
{
	slong ret = 0;
	u64 val0xDC[2] = { 0u, 0u };

	DEBUG_TRCIN();

	/*  PCSは0.01kW単位なので、W単位に変換 */
	val0xDC[0] = (u64)regInfo->chargePowerValueMinCut * 10u;
	if (val0xDC[0] > (u64)EENL_LIMIT_999999999) {
		val0xDC[0] = (u64)EENL_LIMIT_999999999;
	} else {
		/* DO NOTHING */
	}

	val0xDC[1] = (u64)regInfo->chargePowerValueMaxCut * 10u;
	if (val0xDC[1] > (u64)EENL_LIMIT_999999999) {
		val0xDC[1] = (u64)EENL_LIMIT_999999999;
	} else {
		/* DO NOTHING */
	}

	/* infoがNULLの場合メモリ不正なのでプロセス強制終了するが意図通り */
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
		0xDCu, (ulong)val0xDC[0], 0u);
	if (ret == ENL_RESULT_SUCCESS) {
		DLOG_Debug("[einv battery] 0xDC[0] : 0x%llx(%lld)",
			val0xDC[0], val0xDC[0]);

	} else {
		DLOG_Error("battery 0xDC[0] pcs error!:%ld", ret);
	}

	/* infoがNULLの場合メモリ不正なのでプロセス強制終了するが意図通り */
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
		0xDCu, (ulong)val0xDC[1], 1);
	if (ret == ENL_RESULT_SUCCESS) {
		DLOG_Debug("[einv battery] 0xDC[1] : 0x%llx(%lld)",
			val0xDC[1], val0xDC[1]);

	} else {
		DLOG_Error("battery 0xDC[1] pcs error!:%ld", ret);
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xDD(最小最大放電電力量)の反映処理		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　regInfo	蓄電池の登録情報						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS(0):正常終了/0未満:異常(enl.h参照))	*/
/*--------------------------------------------------------------------------*/
/*	detail			:infoの情報については上位から渡されたものをそのまま		*/
/*					 使用し、本関数内で再取得はしない						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtUpdate0xDD(EenlDeviceInfo_t* info, EPCS_REG_INFO_T* regInfo)
{
	slong ret = 0;
	u64 val0xDD[2] = { 0u, 0u };

	DEBUG_TRCIN();

	/*  PCSは0.01kW単位なので、W単位に変換 */
	val0xDD[0] = (u64)regInfo->dischargePowerValueMinCut * 10u;
	if (val0xDD[0] > (u64)EENL_LIMIT_999999999) {
		val0xDD[0] = (u64)EENL_LIMIT_999999999;
	} else {
		/* DO NOTHING */
	}

	val0xDD[1] = (u64)regInfo->dischargePowerValueMaxCut * 10u;
	if (val0xDD[1] > (u64)EENL_LIMIT_999999999) {
		val0xDD[1] = (u64)EENL_LIMIT_999999999;
	} else {
		/* DO NOTHING */
	}

	/* infoがNULLの場合メモリ不正なのでプロセス強制終了するが意図通り */
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
		0xDDu, (ulong)val0xDD[0], 0u);
	if (ret == ENL_RESULT_SUCCESS) {
		DLOG_Debug("[einv battery] 0xDD[0] : 0x%llx(%lld)",
			val0xDD[0], val0xDD[0]);
	} else {
		DLOG_Error("battery 0xDD[0] pcs error!:%ld", ret);
	}

	/* infoがNULLの場合メモリ不正なのでプロセス強制終了するが意図通り */
	ret = EENL_ExtEnlDevSetPropertyValue(NULL, info->devBattery,
		0xDDu, (ulong)val0xDD[1], 1u);
	if (ret == ENL_RESULT_SUCCESS) {
		DLOG_Debug("[einv battery] 0xDD[1] : 0x%llx(%lld)",
			val0xDD[1], val0xDD[1]);

	} else {
		DLOG_Error("battery 0xDD[1] pcs error!:%ld", ret);
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xF0(ユーザ運転モード設定)の反映処理		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　ope		PCSの運転モード							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS(0):正常終了/0未満:異常(enl.h参照))	*/
/*--------------------------------------------------------------------------*/
/*	detail			:infoの情報については上位から渡されたものをそのまま		*/
/*					 使用し、本関数内で再取得はしない						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtUpdate0xF0(EenlDeviceInfo_t* info, EPCS_OPEMODE_GET_T* ope)
{
	slong ret = 0;
	ENL_MESSAGE* msg = NULL;
	uchar val_0xF0 = 0xFFu;		/*  0xF0(ユーザ運転モード設定) */

	DLOG_Debug("mode : KN  pushedUp : no  %d", ope->opeMode);
	DEBUG_TRCIN();

	/* 運転モードから設定値を判定 */
	val_0xF0 = eenl_BtUpdateRetVal0xF0(ope->opeMode);
	DLOG_Debug("opeMode : %d -> 0x%02x", ope->opeMode, val_0xF0);

	if (val_0xF0 != 0xFFu) {
		/* 設定値が決定している場合には、ENLに反映 */

		info = EENL_GetDeviceInfoIndex(0);
		if (info != NULL) {

			/* 0xF0 */
			ret = EENL_ExtEnlDevSetPropertyValue(&msg, info->devBattery,
				0xF0u, val_0xF0, 0u);
			if (ret == ENL_RESULT_SUCCESS) {
				/* 成功 */
				DLOG_Debug("[einv battery] 0xF0 : %02x -> 0x%02x",
					ope->opeMode, val_0xF0);

				if (msg != NULL) {
					/* 状変アナウンス通知 */
					EENL_SendMessage(NULL, msg);
					DLOG_Debug("#### send broadcast message (battery:0xF0) ####");
				} else {
					/* DO NOTHING */
				}
			} else {
				/* 失敗 */
				DLOG_Error("battery 0xF0 pcs error!:%ld", ret);
			}

		} else {
			DLOG_Error("battery 0xF0 pcs error!: null");
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xF2(非常時安心設定)の反映処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　ope		PCSの運転モード							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS(0):正常終了/0未満:異常(enl.h参照))	*/
/*--------------------------------------------------------------------------*/
/*	detail			:infoの情報については上位から渡されたものをそのまま		*/
/*					 使用し、本関数内で再取得はしない						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtUpdate0xF2(EenlDeviceInfo_t* info, EPCS_OPEMODE_GET_T* ope)
{
	slong ret = 0;
	ENL_MESSAGE* msg = NULL;
	uchar val0xF2 = 0xFFu;
	uchar val = ope->emergencyPercent;

	DEBUG_TRCIN();
	DLOG_Debug("emergencyPercent : %d", ope->emergencyPercent);

	if (val > EENL_EMERGENCY_100) {
		/* 100以上 */
		val0xF2 = EENL_EMERGENCY_100;
	} else {
		val0xF2 = val;/*  そのまま */
	}

	if (val0xF2 != 0xFFu) {
		/* 設定値が決定している場合は、ENLに反映 */

		info = EENL_GetDeviceInfoIndex(0);
		if (info != NULL) {
			/* 0xF2 */
			DLOG_Debug("[einv battery] 0xF2) : 0x%02x", val0xF2);
			ret = EENL_ExtEnlDevSetPropertyValue(&msg, info->devBattery,
				0xF2u, val0xF2, 0u);

			if (ret == ENL_RESULT_SUCCESS) {
				/* 成功 */
				DLOG_Debug("[einv battery] 0xF2(0xF9) : %d -> 0x%02x",
					ope->emergencyPercent, val0xF2);
				if (msg != NULL) {
					/* 状変アナウンス通知 */
					EENL_SendMessage(NULL, msg);
					DLOG_Debug("#### send broadcast message (battery:0xF2(0xF9)) ####");
				} else {
					/* DO NOTHING */
				}
			} else {
				/* 失敗 */
				DLOG_Error("battery 0xF2(0xF9) pcs error!:%ld", ret);
			}

		} else {
			DLOG_Error("battery 0xF2(0xF9) pcs error!: null");
		}

	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xF4(自動運転設定)の反映処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　ope		PCSの運転モード							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS(0):正常終了/0未満:異常(enl.h参照))	*/
/*--------------------------------------------------------------------------*/
/*	detail			:infoの情報については上位から渡されたものをそのまま		*/
/*					 使用し、本関数内で再取得はしない						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtUpdate0xF4(EenlDeviceInfo_t* info, EPCS_OPEMODE_GET_T* ope)
{
	slong	ret = 0;
	ENL_MESSAGE* msg = NULL;
	uchar	bs = 0u;
	uchar	mode = 0u;
	uchar	val_0xF0 = 0xFFu;		/*  0xF0(ユーザ運転モード設定) */
	uchar	val_0xF4[39] = { 0u };		/*  0xF4(自動運転モード設定) */
	slong	loop = 0;
	EPCS_CHARGE_TIME_ZONE_T chTmZn;
	DEBUG_TRCIN();


	memset(&val_0xF4, 0x00, sizeof(val_0xF4));
	memset(&chTmZn, 0x00, sizeof(chTmZn));

	/* 運転モードから設定値を判定 */
	val_0xF0 = eenl_BtUpdateRetVal0xF0(ope->opeMode);

	/* 設定値が決定している場合には、ENLに反映 */
	info = EENL_GetDeviceInfoIndex(0);
	if (val_0xF0 != 0xFFu && info != NULL) {
		/* 0xF4 */
		/* 現在の状態を取得 */
		bs = EENL_PDC_USER_0xF4;

		ret = ENL_get_property(info->devBattery, 0xF4u, &bs, val_0xF4);
		if (ret == ENL_RESULT_SUCCESS) {
			/* 成功の場合、取得した値に上書きで変更情報を設定 */

			if (val_0xF0 == 0x06) {
				/* メンテナンスモードの場合 */
				val_0xF4[0] = 0x01;

			} else {
				/* メンテナンスモード以外の場合 */
				val_0xF4[0] = 0x00;
			}

			for (mode = 1u; mode < 5u; ++mode) {
				/* 運転モードごとの充放電時間帯を設定する */
				eenl_Bt0xF4CreateDataMatch(mode, val_0xF4);
			}

			/* 0xF4 Set */
			ret = EENL_ExtEnlDevSetProperty(&msg, info->devBattery,
				0xF4u, bs, val_0xF4);
			if (ret == ENL_RESULT_SUCCESS) {
				/* 成功 */
				DLOG_Debug("[einv battery] 0xF4 (%d) ok", bs);

					
				for (loop = 0; loop < bs; loop++) {
					DLOG_Debug("%02ld : 0x%02x", loop, val_0xF4[loop]);
				}

				if (msg != NULL) {
					/* 状変アナウンス通知 */
					EENL_SendMessage(NULL, msg);
					DLOG_Debug("#### send broadcast message (battery:0xF4) ####");

				} else {
					/* DO NOTHING */
				}

			} else {
				/* 失敗 */
				DLOG_Error("battery 0xF4 set error!:%ld", ret);
			}

		} else {
			/* 失敗 */
			DLOG_Error("battery 0xF4 get error!:%ld", ret);
		}
	} else {
		if (NULL == info) {
			DLOG_Error("battery opeMode Get error!: null");
		} else {
			/* DO NOTHING */
		}
	}


	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xF8(運転指示)の反映処理					*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					 [in]　ope		PCSの運転モード							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS(0):正常終了/0未満:異常(enl.h参照))	*/
/*--------------------------------------------------------------------------*/
/*	detail			:infoの情報については上位から渡されたものをそのまま		*/
/*					 使用し、本関数内で再取得はしない						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtUpdate0xF8(EenlDeviceInfo_t* info, EPCS_OPEMODE_GET_T* ope)
{
	slong ret = 0;
	ENL_MESSAGE* msg = NULL;
	uchar val2 = 0x00u;				/*  0xF0(ユーザ運転モード設定) */
	uchar workF8[EENL_PDC_USER_0xF8] = { 0u };	/*  蓄電池クラスの0xF8(運転指示）格納用変数 */
	uchar mode = EENL_OPEMODE_ECONOMY;		/*  運転モード */
	slong chk = 0;				/*  パラメータ判定結果 */
	EenlHourMinute_t hourMinute;			/* 充放電時刻 */
	EPCS_CHARGE_TIME_ZONE_T chTmZn;

	DEBUG_TRCIN();

	memset(&chTmZn, 0x00, sizeof(chTmZn));
	memset(&workF8, 0x00, sizeof(workF8));

	/*  val2(=0xF0(ユーザ運転モード設定)が"売電モード"又は"グリーンモード"の場合 */
	if ((val2 == EENL_OPEMODE_ECONOMY) || (val2 == EENL_OPEMODE_GREEN)) {
		/*  運転モードに現在の自動運転モードを設定する */
		mode = ope->opeMode;
	} else if ((val2 >= EENL_OPEMODE_CHARGE) && (val2 <= EENL_OPEMODE_SPECIAL)) {
		/*  val2が手動運転モード又はメンテナンスモード又は特殊運転モードの場合 */
		/*  運転モードに前回の自動運転モードを設定する */
		mode = ope->beforeAutoOpeMode;
	} else {
		/*  定義範囲外の値を取得した場合 */
		/*  リモコンの初期値(売電モード)を設定 */
		mode = EENL_OPEMODE_ECONOMY;
	}

	/*  自動運転モードの充放電時刻を取得 */
	ret = epcs_sync_getChargeTimeZone(mode, &chTmZn);
	if (ret != EPCS_OK) {
		/*  処理続行(M1 B_0326対策) */
		DLOG_Error("getChargeTimeZone error!(%d) : %ld", mode, ret);
	} else {
		/* DO NOTHING */
	}

	hourMinute.cStartHH = (uchar)atoi(chTmZn.chargeStartTime.hour);
	hourMinute.cStartMM = (uchar)atoi(chTmZn.chargeStartTime.min);
	hourMinute.cStopHH = (uchar)atoi(chTmZn.chargeStopTime.hour);
	hourMinute.cStopMM = (uchar)atoi(chTmZn.chargeStopTime.min);
	hourMinute.dStartHH = (uchar)atoi(chTmZn.dischargeStartTime.hour);
	hourMinute.dStartMM = (uchar)atoi(chTmZn.dischargeStartTime.min);
	hourMinute.dStopHH = (uchar)atoi(chTmZn.dischargeStopTime.hour);
	hourMinute.dStopMM = (uchar)atoi(chTmZn.dischargeStopTime.min);

	/*  時分の有効範囲判定 */
	chk = EENL_ChargeDischargeTimeZoneCheck(&hourMinute);


	/*  パラメータに問題が無い場合は、ENLに反映 */
	if (chk == 0) {
		workF8[0u] = mode;
		workF8[1u] = (uchar)(hourMinute.cStartHH);
		workF8[2u] = (uchar)(hourMinute.cStartMM);
		workF8[3u] = (uchar)(hourMinute.cStopHH);
		workF8[4u] = (uchar)(hourMinute.cStopMM);
		workF8[5u] = (uchar)(hourMinute.dStartHH);
		workF8[6u] = (uchar)(hourMinute.dStartMM);
		workF8[7u] = (uchar)(hourMinute.dStopHH);
		workF8[8u] = (uchar)(hourMinute.dStopMM);

		info = EENL_GetDeviceInfoIndex(0);
		if (info != NULL) {
			ret = EENL_ExtEnlDevSetProperty(&msg, info->devBattery,
				0xF8u, EENL_PDC_USER_0xF8, (uchar*)workF8);
			if (ret == ENL_RESULT_SUCCESS) {
				if (msg != NULL) {
					/* 状変アナウンス通知(状変アナウンス通知に対応している場合のみ送信される) */
					EENL_SendMessage(NULL, msg);
					DLOG_Debug("#### send broadcast message (battery:0xF8) ####");
				} else {
					/* DO NOTHING */
				}
			} else {
				/* 失敗 */
				DLOG_Error("battery 0xF8 ENLLib error!:%ld", ret);
				/*  処理続行(M1 B_0326対策) */
			}
		} else {
			DLOG_Error("battery 0xF8 ENLinfo index error! ");
			/*  処理続行(M1 B_0326対策) */
		}
	} else {
		/*  パラメータ異常 */
		/*  処理続行(M1 B_0326対策) */
		/*  Do nothing */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xAA(AC充電電力設定値)の設定処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pdc		設定値のデータサイズ					*/
/*					 [in]　edt		設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_ACCEPT:正常終了/ENL_RESULT_NOT_ACCEPT:異常	*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtSetCheck0xAA(uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_NOT_ACCEPT;
	slong chk = 0;
	ulong dat = 0u;

	DEBUG_TRCIN();

	if (*pdc != 4u) {
		/* サイズが4バイト以外の場合 */
		chk = 1;

		DLOG_Error("pdc error!(%d)", *pdc);

	} else {
		/* サイズが4バイトの場合 */
		/*  範囲は 0x00000000 - 0x3B9AC9FF */
		dat = (ulong)((*edt * 256u * 256u * 256u) +
			(*(edt + 1u) * 256u * 256u) + (*(edt + 2u) * 256u) + (*(edt + 3u)));

		if (dat <= 0x3B9AC9FFu) {
			/* 範囲内　何もしない */
			/* ulongなので0以上は確定 */
		} else {
			/* 範囲外 */
			chk++;
		}

		if (chk == 0) {
			/* EPCSに反映 (単位は[Wh]で変換不要) */
			ret = epcs_sync_setAcChargePwr(dat);
			if (ret == EPCS_OK) {
				/* 設定OK */
				DLOG_Debug("einv set ok! (%ld, 0x%08lx)", dat, dat);

				/* ファイルに保存 */
				g_eenlBattery0xAA = dat;
				EENL_FileWrite();

				DLOG_Debug("0xAA save ok! (%ld, 0x%08lx)", dat, dat);
			} else {
				/* 設定失敗 */
				DLOG_Error("einv set NG! :%ld (%ld, 0x%08lx)", ret, dat, dat);
				chk++;
			}
		} else {
			DLOG_Error("einv NOT set (%ld, 0x%08lx)", dat, dat);
			/* セットせず、処理は続行 */
		}
	}
	DLOG_Debug("chk : %ld", chk);

	if (chk > 0) {
		ret = ENL_RESULT_NOT_ACCEPT;
	} else {
		ret = ENL_RESULT_ACCEPT;
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xAB(AC放電量設定値)の設定処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pdc		設定値のデータサイズ					*/
/*					 [in]　edt		設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_ACCEPT:正常終了/ENL_RESULT_NOT_ACCEPT:異常	*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtSetCheck0xAB(uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_NOT_ACCEPT;
	slong chk = 0;
	ulong dat = 0u;

	DEBUG_TRCIN();

	if (*pdc != 4u) {
		/* サイズが4バイト以外の場合 */
		chk = 1;

		DLOG_Error("pdc error!(%d)", *pdc);

	} else {
		/* サイズが4バイトの場合 */
		/*  範囲は 0x00000000 - 0x3B9AC9FF */
		dat = (ulong)((*edt * 256u * 256u * 256u) + (*(edt + 1) * 256u * 256u)
			+ (*(edt + 2u) * 256u) + (*(edt + 3u)));

		if (dat <= 0x3B9AC9FF) {
			/* 範囲内　何もしない */
			/* ulongなので0以上は確定 */
		} else {
			/* 範囲外 */
			chk++;
		}

		if (chk == 0) {
			/* 範囲内で比較用変数より小さい場合は、まずeinvに反映 (単位は[Wh]で変換不要) */
			ret = epcs_sync_setAcDischargePwr(dat);
			if (ret == EPCS_OK) {
				/* 設定OK */
				DLOG_Debug("einv set ok! (%ld, 0x%08lx)", dat, dat);

				/* ファイルに保存 */
				g_eenlBattery0xAB = dat;
				EENL_FileWrite();

				DLOG_Debug("0xAB save ok! (%ld, 0x%08lx)", dat, dat);
			} else {
				/* 設定失敗 */
				DLOG_Error("einv set NG! :%ld (%ld, 0x%08lx)", ret, dat, dat);
				chk++;
			}
		} else {
			DLOG_Error("einv NOT set (%ld, 0x%08lx)", dat, dat);
			/* セットせず、処理は続行 */
		}
	}
	DLOG_Debug("chk : %ld", chk);

	if (chk > 0) {
		ret = ENL_RESULT_NOT_ACCEPT;
	} else {
		ret = ENL_RESULT_ACCEPT;
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xC1(充電方式)の設定処理					*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pdc		設定値のデータサイズ					*/
/*					 [in]　edt		設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_ACCEPT:正常終了/ENL_RESULT_NOT_ACCEPT:異常	*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtSetCheck0xC1(uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_NOT_ACCEPT;
	slong chk = 0;

	DEBUG_TRCIN();

	if (*pdc != 1u) {
		/* サイズが1バイト以外の場合 */
		chk = 1;

		DLOG_Error("pdc error!(%d)", *pdc);

	} else {
		/* サイズが1バイトの場合 */
		/*  範囲は 最大充電電力充電=0x01 or 指定電力充電 =0x03 */
		if ((0x01u == *edt) || (0x03u == *edt)) {
			/* 範囲内　何もしない */
		} else {
			/* 範囲外 */
			chk++;
		}

		if (chk == 0) {
			/* 範囲内の場合はINVに反映 */
			/* TODO★★★INVへの設定インタフェースを変更する必要ret = epcs_sync_setChargeMode(*edt);*/
			if (ret == EPCS_OK) {
				/* 設定OK */
				DLOG_Debug("einv set ok! (%d, 0x%02x)", *edt, *edt);
			} else {
				/* 設定失敗 */
				DLOG_Error("einv set NG! :%ld (%d, 0x%02x)", ret, *edt, *edt);
				chk++;
			}
		} else {
			DLOG_Error("einv NOT set (%d, 0x%02x)", *edt, *edt);
			/* セットせず、処理は続行 */
		}
	}
	DLOG_Debug("chk : %ld", chk);

	if (chk > 0) {
		ret = ENL_RESULT_NOT_ACCEPT;
	} else {
		ret = ENL_RESULT_ACCEPT;
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xC2(放電方式)の設定処理					*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pdc		設定値のデータサイズ					*/
/*					 [in]　edt		設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_ACCEPT:正常終了/ENL_RESULT_NOT_ACCEPT:異常	*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtSetCheck0xC2(uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_NOT_ACCEPT;
	slong chk = 0;

	DEBUG_TRCIN();

	if (*pdc != 1u) {
		/* サイズが1バイト以外の場合 */
		chk = 1;

		DLOG_Error("pdc error!(%d)", *pdc);

	} else {
		/* サイズが1バイトの場合 */
		/*  範囲は 最大放電電力放電=0x01 or 負荷追従放電 =0x02 or 指定電力放電 =0x03 */
		if ((0x01u == *edt) || (0x02u == *edt) || (0x03u == *edt)) {
			/* 範囲内　何もしない */
		} else {
			/* 範囲外 */
			chk++;
		}

		if (chk == 0) {
			/* 範囲内の場合はINVに反映 */
			/* TODO★★★INVへの設定インタフェースを変更する必要ret = epcs_sync_setDischargeMode(*edt);*/
			if (ret == EPCS_OK) {
				/* 設定OK */
				DLOG_Debug("einv set ok! (%d, 0x%02x)", *edt, *edt);
			} else {
				/* 設定失敗 */
				DLOG_Error("einv set NG! :%ld (%d, 0x%02x)", ret, *edt, *edt);
				chk++;
			}
		} else {
			DLOG_Error("einv NOT set (%d, 0x%02x)", *edt, *edt);
			/* セットせず、処理は続行 */
		}
	}
	DLOG_Debug("chk : %ld", chk);

	if (chk > 0) {
		ret = ENL_RESULT_NOT_ACCEPT;
	} else {
		ret = ENL_RESULT_ACCEPT;
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xDA(運転モード)の設定処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pdc		設定値のデータサイズ					*/
/*					 [in]　edt		設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_ACCEPT:正常終了/ENL_RESULT_NOT_ACCEPT:異常	*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtSetCheck0xDA(uchar* pdc, uchar* edt)
{
	DLOG_Debug("epc 0xDA : %02x", *edt);

	slong ret = ENL_RESULT_NOT_ACCEPT;
	uchar val = 0u;
	slong getOpemode = 0;
	slong setOpeMode = 0;
	/* 前回の運転モードをPCSから取得 */
	EPCS_OPEMODE_GET_T ope_get;

	EPCS_OPEMODE_SET_T ope;

	DEBUG_TRCIN();
	EENL_UNUSED_PARA(pdc);

	memset(&ope_get, 0x00, sizeof(ope_get));
	getOpemode = epcs_sync_getOpeMode(&ope_get);

	switch (*edt) {
	case 0x42u: {	/* 充電 */
		ret = ENL_RESULT_ACCEPT;		/* 設定可 */
		val = 4u;	/* 充電 */
		break;
	}
	case 0x43u: {	/* 放電 */
		ret = ENL_RESULT_ACCEPT;		/* 設定可 */
		val = 5u;	/* 放電 */
		break;
	}
	case 0x44u: {	/* 待機 */
		ret = ENL_RESULT_ACCEPT;		/* 設定可 */
		val = 6u;	/* 待機 */
		break;
	}
	case 0x46u: {		/* 自動 */
		ret = ENL_RESULT_ACCEPT;		/* 設定可 */
		if (getOpemode == EPCS_OK) {
			/* 取得できた場合、前回の自動運転モードを設定 */
			if (ope_get.beforeAutoOpeMode == EPCS_AUTO_OPE_UNDEF) {
				/* 前回値がない場合 */
				val = 1u;	/* 売電モード */
			} else {
				/* 前回値がある場合 */
				val = ope_get.beforeAutoOpeMode;
			}
			DLOG_Debug("(N) beforeAutoOpeMode : %d , %d",
				ope_get.beforeAutoOpeMode, val);
			ret = ENL_RESULT_ACCEPT;	/* 設定可 */

		} else {
			/* 取得できなかった場合 */
			DLOG_Error("getOpeMode error! : %ld", ret);
			val = 1u;	/* 売電モード */
			ret = ENL_RESULT_NOT_ACCEPT;	/* 設定不可 */
		}

		break;
	}
	default: {		/* その他 */
		ret = ENL_RESULT_NOT_ACCEPT;	/* 設定不可 */
		break;
	}
	}

	if (ret == ENL_RESULT_ACCEPT) {
		/* 設定可の場合 */

		/* einvへ反映 */

		memset(&ope, 0x00, sizeof(ope));
		ope.setChangeFlag = EPCS_SET_OPE_MODE | EPCS_SET_LIMIT_CTRL;
		ope.remoteState = EPCS_REMOTE_ON;
		ope.opeMode = val;

		if ((*edt == 0x42u) || (*edt == 0x43u)) {
			ope.limitCtrl = EPCS_LIMIT_CTRL_ON;
		} else {
			/* 運転モード変更通知でOFFするため、ここではOFFしない */
			ope.limitCtrl = EPCS_LIMIT_CTRL_UNDEF;
		}

		setOpeMode = epcs_sync_setOpeMode(&ope);
		if (setOpeMode == EPCS_OK) {
			/* 設定成功 */
			DLOG_Debug("einv set ok! (0x%02x, %d, rem=%d, lim=%d)",
				*edt, val, ope.remoteState, ope.limitCtrl);

			if ((*edt == 0x42u) || (*edt == 0x43u)) {
				/*  運転モードが充電 or 放電に変更されたら、ENL充放電制御中に設定する */
				g_eenlOpeCtrl = *edt;

				ret = EENL_CmnFileWrite(EENL_OPECTRL_FILE,
					&g_eenlOpeCtrl, sizeof(g_eenlOpeCtrl));
				if (ret == EENL_OK) {
					DLOG_Debug("write enl operation control file = 0x%02x",
						g_eenlOpeCtrl);
				} else {
					DLOG_Error("write enl operation control file ERROR! = 0x%02x (ret=%ld)",
						g_eenlOpeCtrl, ret);
				}
			} else {
				/*  ENL充放電制御のOFFは、通知で処理する(ここではOFFにしない) */
			}
		} else {
			/* 設定失敗 */
			DLOG_Error("einv set NG! : %ld(0x%02x, %d)", setOpeMode, *edt, val);
			ret = ENL_RESULT_NOT_ACCEPT;
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xE7(充電量設定値1)の設定処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pdc		設定値のデータサイズ					*/
/*					 [in]　edt		設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_ACCEPT:正常終了/ENL_RESULT_NOT_ACCEPT:異常	*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtSetCheck0xE7(uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_NOT_ACCEPT;
	slong chk = 0;
	ulong dat = 0u;

	DEBUG_TRCIN();

	if (*pdc != 4u) {
		/* サイズが4バイト以外の場合 */
		chk = 1;

		DLOG_Error("pdc error!(%d)", *pdc);

	} else {
		/* サイズが4バイトの場合 */
		/*  範囲は 0x00000000 - 0x3B9AC9FF */
		dat = (ulong)((*edt * 256u * 256u * 256u) + (*(edt + 1) * 256u * 256u)
			+ (*(edt + 2u) * 256u) + (*(edt + 3u)));

		if (dat <= 0x3B9AC9FFu) {
			/* 範囲内　何もしない */
			/* ulongなので0以上は確定 */
		} else {
			/* 範囲外 */
			chk++;
		}

		if (chk == 0) {
			/* EPCSに反映 (単位は[Wh]で変換不要) */
			ret = epcs_sync_setChargePwr(dat);
			if (ret == EPCS_OK) {
				/* 設定OK */
				DLOG_Debug("einv set ok! (%ld, 0x%08lx)", dat, dat);

				/* ファイルに保存 */
				g_eenlBattery0xE7 = dat;
				EENL_FileWrite();

				DLOG_Debug("0xE7 save ok! (%ld, 0x%08lx)", dat, dat);
			} else {
				/* 設定失敗 */
				DLOG_Error("einv set NG! :%ld (%ld, 0x%08lx)", ret, dat, dat);
				chk++;
			}
		} else {
			DLOG_Error("einv NOT set (%ld, 0x%08lx)", dat, dat);
			/* セットせず、処理は続行 */
		}

	}

	DLOG_Debug("chk : %ld", chk);

	if (chk > 0) {
		ret = ENL_RESULT_NOT_ACCEPT;
	} else {
		ret = ENL_RESULT_ACCEPT;
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xE8(放放電量設定値1)の設定処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pdc		設定値のデータサイズ					*/
/*					 [in]　edt		設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_ACCEPT:正常終了/ENL_RESULT_NOT_ACCEPT:異常	*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtSetCheck0xE8(uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_NOT_ACCEPT;
	slong chk = 0;
	ulong dat = 0u;

	DEBUG_TRCIN();

	if (*pdc != 4u) {
		/* サイズが4バイト以外の場合 */
		chk = 1;

		DLOG_Error("pdc error!(%d)", *pdc);

	} else {
		/* サイズが4バイトの場合 */
		dat = (ulong)((*edt * 256u * 256u * 256u) + (*(edt + 1u) * 256u * 256u)
			+ (*(edt + 2u) * 256u) + (*(edt + 3u)));

		if (dat <= 0x3B9AC9FFu) {
			/* 範囲内　何もしない */
			/* ulongなので0以上は確定 */
		} else {
			/* 範囲外 */
			chk++;
		}

		if (chk == 0) {
			/* einvに反映 (単位は[Wh]で変換不要) */
			ret = epcs_sync_setDischargePwr(dat);
			if (ret == EPCS_OK) {
				/* 設定OK */
				DLOG_Debug("einv set ok! (%ld, %08lx)", dat, dat);

				/* ファイルに保存 */
				g_eenlBattery0xE8 = dat;
				EENL_FileWrite();

				DLOG_Debug("0xE8 save ok! (%ld, 0x%08lx)", dat, dat);
			} else {
				/* 設定失敗 */
				DLOG_Error("einv set NG! :%ld (%ld, %08lx)", ret, dat, dat);
				chk++;
			}
		} else {
			DLOG_Error("einv NOT set (%ld, 0x%08lx)", dat, dat);
			/* セットせず、処理は続行 */
		}

	}

	DLOG_Debug("chk : %ld", chk);

	if (chk > 0) {
		ret = ENL_RESULT_NOT_ACCEPT;
	} else {
		ret = ENL_RESULT_ACCEPT;
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xEB(充電電力設定値)の設定処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pdc		設定値のデータサイズ					*/
/*					 [in]　edt		設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_ACCEPT:正常終了/ENL_RESULT_NOT_ACCEPT:異常	*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtSetCheck0xEB(uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_NOT_ACCEPT;
	slong chk = 0;
	ulong dat = 0u;
	ulong val0xEB = 0u;
	EPCS_OPEMODE_GET_T ope;
	memset(&ope, 0x00, sizeof(ope));

	DEBUG_TRCIN();

	if (*pdc != 4u) {
		/* サイズが4バイト以外の場合 */
		chk = 1;
		DLOG_Error("pdc error!(%d)", *pdc);

	} else {
		/* サイズが4バイトの場合 */
		dat = (ulong)((*edt * 256u * 256u * 256u) + (*(edt + 1u) * 256u * 256u)
			+ (*(edt + 2u) * 256u) + (*(edt + 3u)));
		if (dat <= 0x3B9AC9FFu) {
			/* 範囲内　何もしない */
			/* ulongなので0以上は確定 */
		} else {
			/* 範囲外 */
			chk++;
		}

		if (g_eenlTestRunState == EENL_TESTRUN_START) {
			/* 試運転中は不可応答 */
			DLOG_Debug("detect test run start! flag=%d (%ld, 0x%08lx)",
				g_eenlTestRunState, dat, dat);
			chk++;
		} else {
			/* DO NOTHING */
		}

		/*  運転モード取得 */
		ret = epcs_sync_getOpeMode(&ope);
		/*  opeの取得が出来て、ope.opeModeが電力移動モードまたは */
		/*  opeの取得が出来きなくて、g_eenlKeepOpeModeが電力移動モードなら */
		DLOG_Debug("ret=%ld/ope.opeMode=%d/g_eenlKeepOpeMode=%d",
			ret, ope.opeMode, g_eenlKeepOpeMode);

		/*  epcs_sync_getOpeMode()が失敗した時のエラー回避の予備の判定 */
		if (((ret == EPCS_OK) && (EENL_ElectricMoveModeChk(ope.opeMode) == TRUE)) ||
			((ret != EPCS_OK) && (EENL_ElectricMoveModeChk(g_eenlKeepOpeMode) == TRUE))) {
			/* 電力移動モード中は不可応答 */
			chk++;
			DLOG_Debug("BT 0xEB opeMode = Electric_Move_Mode.%ld", chk);
		} else {
			/* DO NOTHING */
		}

		if (chk == 0) {
			/*  範囲：0 ～ 99990[W] ※10未満は切り捨て */
			val0xEB = (dat / 10u) * 10u;
			if (val0xEB > 99990u) {
				/*  PCS側のリミットを越えている場合はリミットする */
				val0xEB = 99990u;
			} else {
				/* DO NOTHING */
			}

			/*  充電電力設定値設定 */
			ret = epcs_sync_setRestrictedChargePwr(val0xEB);
			if (ret == EPCS_OK) {
				/* 設定OK */
				DLOG_Debug("einv set ok! (%ld, %08lx)", val0xEB, val0xEB);
			} else {
				/* 設定失敗 */
				DLOG_Error("einv set NG! :%ld (%ld, %08lx)", ret, val0xEB, val0xEB);
				chk++;
			}
		} else {
			/* DO NOTHING */
		}
	}
	DLOG_Debug("chk : %ld", chk);

	if (chk > 0) {
		ret = ENL_RESULT_NOT_ACCEPT;
	} else {
		ret = ENL_RESULT_ACCEPT;
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xEC(放電電力設定値)の設定処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pdc		設定値のデータサイズ					*/
/*					 [in]　edt		設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_ACCEPT:正常終了/ENL_RESULT_NOT_ACCEPT:異常	*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtSetCheck0xEC(uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_NOT_ACCEPT;
	slong chk = 0;
	ulong dat = 0u;
	ulong val0xEC = 0u;
	EPCS_OPEMODE_GET_T ope;
	memset(&ope, 0x00, sizeof(ope));

	DEBUG_TRCIN();

	if (*pdc != 4u) {
		/* サイズが4バイト以外の場合 */
		chk = 1;
		DLOG_Error("pdc error!(%d)", *pdc);

	} else {
		/* サイズが4バイトの場合 */
		dat = (ulong)((*edt * 256u * 256u * 256u) + (*(edt + 1) * 256u * 256u)
			+ (*(edt + 2u) * 256u) + (*(edt + 3u)));
		if (dat <= 0x3B9AC9FF) {
			/* 範囲内　何もしない */
			/* ulongなので0以上は確定 */
		} else {
			/* 範囲外 */
			chk++;
		}

		if (g_eenlTestRunState == EENL_TESTRUN_START) {
			/* 試運転中は不可応答 */
			DLOG_Debug("detect test run start! flag=%d (%ld, 0x%08lx)",
				g_eenlTestRunState, dat, dat);
			chk++;
		} else {
			/* DO NOTHING */
		}

		/*  運転モード取得 */
		ret = epcs_sync_getOpeMode(&ope);
		/*  opeの取得が出来て、ope.opeModeが電力移動モードまたは */
		/*  opeの取得が出来きなくて、g_eenlKeepOpeModeが電力移動モードなら */
		DLOG_Debug("ret=%ld/ope.opeMode=%d/g_eenlKeepOpeMode=%d",
			ret, ope.opeMode, g_eenlKeepOpeMode);

		/*  epcs_sync_getOpeMode()が失敗した時のエラー回避の予備の判定 */
		if (((ret == EPCS_OK)
			&& (EENL_ElectricMoveModeChk(ope.opeMode) == TRUE)) ||
			((ret != EPCS_OK)
				&& (EENL_ElectricMoveModeChk(g_eenlKeepOpeMode) == TRUE))) {
			/* 電力移動モード中は不可応答 */
			chk++;
			DLOG_Debug("BT 0xEC opeMode = Electric_Move_Mode.%ld", chk);
		} else {
			/* DO NOTHING */
		}

		if (chk == 0) {
			/*  範囲：0 ～ 99990[W] ※10未満は切り捨て */
			val0xEC = (dat / 10u) * 10u;
			if (val0xEC > 99990u) {
				/*  PCS側のリミットを越えている場合はリミットする */
				val0xEC = 99990u;
			} else {
				/* DO NOTHING */
			}

			/*  放電電力設定値設定 */
			ret = epcs_sync_setRestrictedDischargePwr(val0xEC);
			if (ret == EPCS_OK) {
				/* 設定OK */
				DLOG_Debug("einv set ok! (%ld, %08lx)", val0xEC, val0xEC);
			} else {
				/* 設定失敗 */
				DLOG_Error("einv set NG! :%ld (%ld, %08lx)", ret, val0xEC, val0xEC);
				chk++;
			}
		} else {
			/* DO NOTHING */
		}
	}
	DLOG_Debug("chk : %ld", chk);

	if (chk > 0) {
		ret = ENL_RESULT_NOT_ACCEPT;
	} else {
		ret = ENL_RESULT_ACCEPT;
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xF2(ユーザ定義 非常時安心設定)の設定処理	*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pdc		設定値のデータサイズ					*/
/*					 [in]　edt		設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_ACCEPT:正常終了/ENL_RESULT_NOT_ACCEPT:異常	*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtSetCheck0xF2(uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_NOT_ACCEPT;
	slong setOpeMode = 0;
	EPCS_OPEMODE_SET_T ope;

	DEBUG_TRCIN();
	EENL_UNUSED_PARA(pdc);

	switch (*edt) {
	case 0u:
		/* 0 */
		ret = ENL_RESULT_ACCEPT;
		break;
	case 20u:
		/* 20 */
		ret = ENL_RESULT_ACCEPT;
		break;
	case 30u:
		/* 30 */
		ret = ENL_RESULT_ACCEPT;
		break;
	case 40u:
		/* 40 */
		ret = ENL_RESULT_ACCEPT;
		break;
	case 50u:
		/* 50 */
		ret = ENL_RESULT_ACCEPT;
		break;
	case 60u:
		/* 60 */
		ret = ENL_RESULT_ACCEPT;
		break;
	case 70u:
		/* 70 */
		ret = ENL_RESULT_ACCEPT;
		break;
	case 100u:
		/* 100 */
		ret = ENL_RESULT_ACCEPT;
		break;
	default:
		/* その他 */
		/* 設定不可 */
		ret = ENL_RESULT_NOT_ACCEPT;
		break;
	}

	/* einvへ反映 */
	if (ret == ENL_RESULT_ACCEPT) {


		memset(&ope, 0x00, sizeof(ope));
		ope.setChangeFlag = EPCS_SET_EMG_PER;

		ope.remoteState = EPCS_REMOTE_ON;

		ope.limitCtrl = EPCS_LIMIT_CTRL_UNDEF;
		/*  ※setChangeFlagにEPCS_SET_LIMIT_CTRLを指定しない場合も、無効値を設定しておく。 */

		ope.emergencyPercent = *edt;

		setOpeMode = epcs_sync_setOpeMode(&ope);
		if (setOpeMode == EPCS_OK) {
			/* 設定成功 */
			DLOG_Debug("einv set ok! (0x%02x, %d)", *edt, *edt);
		} else {
			/* 設定失敗 */
			DLOG_Error("einv set NG! : %ld(0x%02x, %d)", setOpeMode, *edt, *edt);
			ret = ENL_RESULT_NOT_ACCEPT;
		}

	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xF4(ユーザ定義 自動運転)の設定処理		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pdc		設定値のデータサイズ					*/
/*					 [in]　edt		設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_ACCEPT:正常終了/ENL_RESULT_NOT_ACCEPT:異常	*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtSetCheck0xF4(uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_ACCEPT;
	slong loop = EENL_0xF4_TIMESET;
	slong cnt = 0;
	slong r = 0;
	slong getOpemode = 0;	/*  2017/05/11 B-0076 0xF4状態変化通知 不具合対処 */

	uchar val0xDA = 0u;
	uchar autoOpe = 0u;
	uchar timeSetNum = 0u;

	uchar s_mode = 0u;
	uchar f4_ope_reset_flg = EENL_F4_OPEMODE_RESET_OFF; /*  メンテナンス状態書き換えフラグ    2017/05/11 B-0076 0xF4状態変化通知 不具合対処 */
	uchar f4_ope_reset_val = 0u;							/*  メンテナンス状態書き換え値        2017/05/16 B-0076 0xF4状態変化通知 不具合対処 */

	bool_t overlap = 0;
	bool_t isLoopEnd = FALSE;

	EPCS_CHARGE_TIME_ZONE_T	s_tz;
	EPCS_OPEMODE_SET_T		ope;
	EPCS_OPEMODE_GET_T		opeGet;
	EPCS_OPESTTS_INFO_T		opeSttsInfo;                /*  2017/05/11 B-0076 0xF4状態変化通知 不具合対処 */

	DEBUG_TRCIN();
	
	if (*pdc != 0x15u) {
		/* サイズが21バイト以外の場合 */
		ret = ENL_RESULT_NOT_ACCEPT;
		DLOG_Error("pdc 21 error!(%d)", *pdc);

	} else {
		/* 1バイト目		メンテナンスモード状態(Getのみ) */
		/*               1バイト目のチェックは他のパラメータがOKだった時に行う。*/
		/* 最後にチェックする。2017/05/11 B-0076 0xF4状態変化通知 不具合対処 */

		/* 2バイト目		自動運転モード */
		autoOpe = *(edt + 1u);
		if ((0x01u != autoOpe) && (0x02u != autoOpe)) {
			DLOG_Error("edt(1) error! (%x)", autoOpe);
			ret = ENL_RESULT_NOT_ACCEPT;
		} else {
			/* DO NOTHING */
		}

		/* 3バイト目		時刻設定数 2固定 */
		timeSetNum = *(edt + 2u);
		if (EENL_0xF4_TIMESET != timeSetNum) {
			/* 2以外の場合 */
			DLOG_Error("edt(2) error! (%x)", timeSetNum);
			ret = ENL_RESULT_NOT_ACCEPT;
		} else {
			/* DO NOTHING */
		}
	}

	if (ENL_RESULT_NOT_ACCEPT != ret) {
		/* 受理応答 */
		/*  2017/05/11 B-0076 0xF4状態変化通知 不具合対処 */
		/* 1バイト目のチェック */
		memset(&opeSttsInfo, 0x0, sizeof(opeSttsInfo));
		/* 運転モードをPCSから取得 */
		getOpemode = epcs_sync_getOpeSttsInfo(&opeSttsInfo);
		DLOG_Debug("getOpemode=%ld/opeSttsInfo.opeMode=%d/g_eenlKeepOpeMode=%d",
			getOpemode, opeSttsInfo.opeMode, g_eenlKeepOpeMode);
		/*  opeの取得が出来て、opeSttsInfo.opeModeがメンテナンスモードでないまたは */
		/*  opeの取得が出来きなくて、g_eenlKeepOpeModeがメンテナンスモードでないなら */
		/*  epcs_sync_getOpeSttsInfo()が失敗した時のエラー回避の予備の判定 */
		if (((getOpemode == EPCS_OK) && (opeSttsInfo.opeMode != EPCS_OPEMODE_7)) ||
			((getOpemode != EPCS_OK) && (g_eenlKeepOpeMode != EPCS_OPEMODE_7))) {
			/*  1バイト目(メンテナンスモード状態)に値がセットされていたら */
			if (*edt != 0x00u) {
				/*  メンテナンス状態書き換え値 */
				f4_ope_reset_val = 0x00u;
				DLOG_Debug("opemode value. edt=%d", *edt);
				/*  メンテナンス状態書き換えフラグ OFF=換えない/ON=換える */
				f4_ope_reset_flg = EENL_F4_OPEMODE_RESET_ON;
			} else {
				DLOG_Debug("opemode reset pass. edt=%d", *edt);
			}
		} else if (((getOpemode == EPCS_OK) &&
			(opeSttsInfo.opeMode == EPCS_OPEMODE_7)) ||
			((getOpemode != EPCS_OK) && (g_eenlKeepOpeMode == EPCS_OPEMODE_7))) {
			/*  opeの取得が出来て、opeSttsInfo.opeModeがメンテナンスモードまたは */
			/*  opeの取得が出来きなくて、g_eenlKeepOpeModeがメンテナンスモードなら */
			/*  epcs_sync_getOpeSttsInfo()が失敗した時のエラー回避の予備の判定 */

			/*  1バイト目(メンテナンスモード状態)に値がセットされていたら */
			if (*edt != 0x01u) {
				/*  メンテナンス状態書き換え値 */
				f4_ope_reset_val = 0x01u;
				DLOG_Debug("opemode value. edt=%d", *edt);
				/*  メンテナンス状態書き換えフラグ OFF=換えない/ON=換える */
				f4_ope_reset_flg = EENL_F4_OPEMODE_RESET_ON;
			} else {
				DLOG_Debug("opemode reset pass. edt=%d", *edt);
			}
		} else {
			/*  opeの取得が出来なかったら */
			DLOG_Error("opemode get error! ret=%ld", getOpemode);
			/*  メンテナンスモードが不明でもエラーにはしない。 */
			/*  ENL_RESULT_ACCEPTで返れば、状態変化通知で再度メンテナンスモードを書き換える処理を実施されます。 */
		}
	} else {
		/* 不可応答 */
	}

	/* einvへ設定 */
	if (ENL_RESULT_NOT_ACCEPT != ret) {

		/* 充放電時刻の設定 */
		for (cnt = 0; (cnt < loop && !isLoopEnd); cnt++) {

			memset(&s_tz, 0x00, sizeof(s_tz));

			switch (*(edt + 3u + 9u * cnt)) {
			case 0x01u:
				/* 売電モード */
				s_mode = 1u;
				break;
			case 0x02u:
				/* グリーンモード */
				s_mode = 2u;
				break;
			case 0x07u:
				/* 深夜電力活用 */
				s_mode = 0u;
				break;
			case 0x08u:
				/* ピークカット */
				s_mode = 2u;
				break;
			default:
				ret = ENL_RESULT_NOT_ACCEPT;
				break;
			}

			if (ENL_RESULT_NOT_ACCEPT != ret) {

				snprintf(s_tz.chargeStartTime.hour, CMN_HOUR_LEN, "%02d", (*(edt + 4 + 9 * cnt)));
				snprintf(s_tz.chargeStartTime.min, CMN_MIN_LEN, "%02d", (*(edt + 5 + 9 * cnt)));
				snprintf(s_tz.chargeStopTime.hour, CMN_HOUR_LEN, "%02d", (*(edt + 6 + 9 * cnt)));
				snprintf(s_tz.chargeStopTime.min, CMN_MIN_LEN, "%02d", (*(edt + 7 + 9 * cnt)));
				snprintf(s_tz.dischargeStartTime.hour, CMN_HOUR_LEN, "%02d", (*(edt + 8 + 9 * cnt)));
				snprintf(s_tz.dischargeStartTime.min, CMN_MIN_LEN, "%02d", (*(edt + 9 + 9 * cnt)));
				snprintf(s_tz.dischargeStopTime.hour, CMN_HOUR_LEN, "%02d", (*(edt + 10 + 9 * cnt)));
				snprintf(s_tz.dischargeStopTime.min, CMN_MIN_LEN, "%02d", (*(edt + 11 + 9 * cnt)));

				/* 時間の有効範囲と重複をチェックする */
				overlap = EENL_CheckChargeTimeZone(&s_tz);
				if (FALSE != overlap) {
					r = epcs_sync_setChargeTimeZone(s_mode, &s_tz);
					if (r == EPCS_OK) {
						/* 設定成功 */
						DLOG_Debug("einv setChargeTimeZone(%ld) ok! : %d", cnt, s_mode);
					} else {
						/* 設定失敗 */
						DLOG_Error("einv setChargeTimeZone(%ld) NG! (%ld) : %d", cnt, r, s_mode);
						ret = ENL_RESULT_NOT_ACCEPT;
						isLoopEnd = TRUE;
					}

				} else {
					ret = ENL_RESULT_NOT_ACCEPT;
					isLoopEnd = TRUE;
				}

			} else {
				/* ENL_RESULT_NOT_ACCEPTなのでforを抜ける */
				isLoopEnd = TRUE;
			}
		}	/* for */

		/* 自動運転モードの設定 */
		switch (*(edt + 1u)) {
		case 0x01:		/* 売電モード */
			s_mode = 1u;
			break;
		case 0x02u:		/* グリーンモード */
			s_mode = 2u;
			break;
		case 0x07u:		/* 深夜電力活用 */
			s_mode = 0u;
			break;
		case 0x08u:		/* ピークカット */
			s_mode = 2u;
			break;
		default:
			ret = ENL_RESULT_NOT_ACCEPT;
			break;
		}

		/* 現在の運転モードを取得 */
		memset(&opeGet, 0x00, sizeof(opeGet));

		if (ENL_RESULT_NOT_ACCEPT != ret) {
			r = epcs_sync_getOpeMode(&opeGet);
			if (r == EPCS_OK) {

				/*  opeGet.opeModeが0～3なら。 PGReleaf対処。2017/05/17 */
				if ((opeGet.opeMode <= 3u)
					|| (opeGet.opeMode == 7u)
					|| (opeGet.opeMode == 8u)) {
					val0xDA = 0x46u;	/* 自動 */

				} else {
					val0xDA = 0u;	/* 自動以外 */

				}

				DLOG_Debug("-------------------------------------------");
				DLOG_Debug(" now run mode : %d -> 0x%02x",
					opeGet.opeMode, val0xDA);
				DLOG_Debug("-------------------------------------------");

				/* 値が取得できた場合 */
				if (val0xDA == 0x46u) {
					/* 現在、自動運転モードの場合 */

					/* einvに自動運転モードを設定（即時反映） */
					memset(&ope, 0x00, sizeof(ope));
					ope.setChangeFlag = EPCS_SET_OPE_MODE;

					ope.remoteState = EPCS_REMOTE_ON;

					ope.limitCtrl = EPCS_LIMIT_CTRL_UNDEF;
					/*  ※setChangeFlagにEPCS_SET_LIMIT_CTRLを指定しない場合も、無効値を設定しておく。 */
					/*    充放電制御の解除は運転モード変更通知で行うため、ここではOFFしない。 */

					ope.opeMode = s_mode;

					r = epcs_sync_setOpeMode(&ope);
					if (r == EPCS_OK) {
						/* 設定成功 */
						DLOG_Debug("einv setOpeMode ok! : [%d]", s_mode);
					} else {
						/* 設定失敗 */
						DLOG_Error("einv setOpeMode NG! (%ld) : [%d]",
							r, s_mode);
						ret = ENL_RESULT_NOT_ACCEPT;
					}

				} else {
					/* 現在、自動運転モード以外の場合 */
					/* epcsへ自動運転モードの予約設定 */
					r = epcs_sync_setAutoOpeModeReserve(s_mode);
					if (r == EPCS_OK) {
						/* 設定成功 */
						DLOG_Debug("einv setAutoOpeModeReserve ok! : [%d]",
							s_mode);

					} else {
						/* 設定失敗 */
						DLOG_Error("einv setAutoOpeModeReserve NG! (%ld) : [%d]",
							r, s_mode);
						ret = ENL_RESULT_NOT_ACCEPT;
					}
				}

			} else {
				/* 値が取得できなかった場合 */
				DLOG_Error("einv getOpeMode error! (%ld)", r);
				ret = ENL_RESULT_NOT_ACCEPT;
			}
		}	/* ENL_RESULT_ACCEPT */
		/*  最後までチェックしてENL_RESULT_ACCEPTなら */
			/*  パラメータのエラーが無ければ1バイト目のチェックをする。 */
		if (ret == ENL_RESULT_ACCEPT) {
			/* メンテナンス状態書き換えフラグがONなら、*/
			/* 1バイト目(メンテナンスモード状態)の値を書き換える。 */
			/*  メンテナンス状態書き換えフラグ OFF=換えない/ON=換える */
			if (f4_ope_reset_flg == EENL_F4_OPEMODE_RESET_ON) {
				/*  メンテナンスモード状態を書き戻す。 */
				*edt = f4_ope_reset_val;
				DLOG_Debug("opemode value reset. edt=%d", *edt);
				/*  戻り値をEENL_RESULT_COLLECT_AND_ACCEPT_BAT0xF4にする。 */
				ret = EENL_RESULT_COLLECT_AND_ACCEPT_BAT0xF4;
			} else {
				/* DO NOTHING */
			}
		} else {
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xF8(運転指示)の設定処理					*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pdc		設定値のデータサイズ					*/
/*					 [in]　edt		設定値									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_ACCEPT:正常終了/ENL_RESULT_NOT_ACCEPT:異常	*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtSetCheck0xF8(uchar* pdc, uchar* edt)
{
	slong ret = ENL_RESULT_ACCEPT;
	slong setOpeMode = 0;
	bool_t overlap = FALSE;

	EPCS_CHARGE_TIME_ZONE_T	s_tz;
	EPCS_OPEMODE_SET_T		ope;

	DEBUG_TRCIN();

	/*  0xF8のSetを受信したら操作音を鳴らす */
	/* TODO★★★ ebuz_async_soundTouchBuz();  操作音 */

	/*  データサイズが規格外の場合 */
	if (*pdc != 9u) {
		ret = ENL_RESULT_NOT_ACCEPT;
		DLOG_Debug("Set 0xF8 pdc size err");
	} else {
		/* DO NOTHING */
	}

	/*  売電モード、グリーンモード以外の場合 */
	if ((*edt < EENL_OPEMODE_ECONOMY) || (*edt > EENL_OPEMODE_GREEN)) {
		ret = ENL_RESULT_NOT_ACCEPT;
		DLOG_Debug("Set 0xF8 opemode err");
	} else {
		/* DO NOTHING */
	}

	if (ENL_RESULT_NOT_ACCEPT != ret) {
		memset(&ope, 0x00, sizeof(ope));
		memset(&s_tz, 0, sizeof(s_tz));

		/*  運転モードと充放電時刻の更新フラグをON */
		ope.setChangeFlag = EPCS_SET_OPE_MODE | EPCS_SET_TIME_ZONE;

		/*  即時反映 */
		ope.remoteState = EPCS_REMOTE_ON;

		ope.limitCtrl = EPCS_LIMIT_CTRL_UNDEF;
		/*  ※setChangeFlagにEPCS_SET_LIMIT_CTRLを指定しない場合も、無効値を設定しておく。 */

		ope.opeMode = *edt;
		snprintf(s_tz.chargeStartTime.hour, CMN_HOUR_LEN, "%02d", (*(edt + 1)));
		snprintf(s_tz.chargeStartTime.min, CMN_MIN_LEN, "%02d", (*(edt + 2)));
		snprintf(s_tz.chargeStopTime.hour, CMN_HOUR_LEN, "%02d", (*(edt + 3)));
		snprintf(s_tz.chargeStopTime.min, CMN_MIN_LEN, "%02d", (*(edt + 4)));
		snprintf(s_tz.dischargeStartTime.hour, CMN_HOUR_LEN, "%02d", (*(edt + 5)));
		snprintf(s_tz.dischargeStartTime.min, CMN_MIN_LEN, "%02d", (*(edt + 6)));
		snprintf(s_tz.dischargeStopTime.hour, CMN_HOUR_LEN, "%02d", (*(edt + 7)));
		snprintf(s_tz.dischargeStopTime.min, CMN_MIN_LEN, "%02d", (*(edt + 8)));

		/* 時間の有効範囲と重複をチェックする */
		overlap = EENL_CheckChargeTimeZone(&s_tz);
		if (FALSE != overlap) {
			ope.chargeTimeZone = s_tz;

			setOpeMode = epcs_sync_setOpeMode(&ope);
			if (setOpeMode == EPCS_OK) {
				/* 設定成功 */
				DLOG_Debug("0xF8 einv set ok! ");
			} else {
				/* 設定失敗 */
				DLOG_Error("0xF8 einv set NG! : %ld", setOpeMode);
				/* ENLへの設定をできないように応答 */
				ret = ENL_RESULT_NOT_ACCEPT;
			}
		} else {
			ret = ENL_RESULT_NOT_ACCEPT;
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xDAのデータ設定処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　opeMode			運転モード						*/
/*--------------------------------------------------------------------------*/
/*	return			:0xDAに設定する値										*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static uchar eenl_BtUpdateRetVal0xDA(uchar opeMode)
{
	uchar val_0xDA = 0xFFu;	/* 不定値で初期化 */
	DEBUG_TRCIN();

	/*  押し上げなし */
	if (EENL_SpOpemodeChk(opeMode) == TRUE) { /* 特殊運転モードなら */
		val_0xDA = 0x40u;
	} else {
		switch (opeMode) {
		case EPCS_OPEMODE_1:
			/* 売電モード */
			val_0xDA = 0x46u;
			break;
		case EPCS_OPEMODE_2:
			/* グリーンモード */
			val_0xDA = 0x46u;
			break;
		case EPCS_OPEMODE_4:
			/* 充電 */
			val_0xDA = 0x42u;
			break;
		case EPCS_OPEMODE_5:
			/* 放電 */
			val_0xDA = 0x43u;
			break;
		case EPCS_OPEMODE_6:
			/* 待機 */
			val_0xDA = 0x44u;
			break;
		case EPCS_OPEMODE_7:
			/* メンテナンスモード */
			val_0xDA = 0x40u;
			break;
			/* T3で削除 */
#if 0
			case EPCS_OPEMODE_8:		/* 特殊運転モード */
				val_0xDA = 0x40u;		/*  0x40=その他 */
				break;
#endif
		default:
			/* その他 */
			val_0xDA = 0xFFu;
			/*  不定値 */
			break;
		}
	}

	DEBUG_TRCOUT();
	return val_0xDA;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xF0のデータ設定処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　opeMode			運転モード						*/
/*--------------------------------------------------------------------------*/
/*	return			:0xDAに設定する値										*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static uchar eenl_BtUpdateRetVal0xF0(uchar opeMode)
{
	uchar val_0xF0 = 0xFFu;	/* 不定値で初期化 */

	DEBUG_TRCIN();

	/*  押し上げなし */
	/* 特殊運転モードなら */
	if (EENL_SpOpemodeChk(opeMode) == TRUE) {
		val_0xF0 = 0x07u;
	} else {
		switch (opeMode) {
		case EPCS_OPEMODE_1:
			/* 売電モード */
			val_0xF0 = 0x01u;
			break;
		case EPCS_OPEMODE_2:
			/* グリーンモード */
			val_0xF0 = 0x02u;
			break;
		case EPCS_OPEMODE_4:
			/* 充電 */
			val_0xF0 = 0x03u;
			break;
		case EPCS_OPEMODE_5:
			/* 放電 */
			val_0xF0 = 0x04u;
			break;
		case EPCS_OPEMODE_6:
			/* 待機 */
			val_0xF0 = 0x05u;
			break;
		case EPCS_OPEMODE_7:
			/* メンテナンスモード */
		val_0xF0 = 0x06u;
			break;
			/* T3で削除 */
#if 0
			case EPCS_OPEMODE_8:	/* 特殊運転モード */
				val_0xF0 = 0x07u;
				break;
#endif
		default:
			/* その他 */
			val_0xF0 = 0xFFu;
			/*  不定値 */
			break;
		}
	}

	DEBUG_TRCOUT();
	return val_0xF0;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xF4のデータ設定処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　p_buff0xF4			0xF4設定値					*/
/*					 [in]　val0xF4_0			付属設定値					*/
/*					 [in]　val0xF4_1			付属設定値					*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:本関数の戻り値がENL_RESULT_SUCCESSのときのみ			*/
/*					 0xF4を設定してください。								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_BtNotify0xF4CreateData(
	uchar* p_buff0xF4,
	uchar	val0xF4_0,
	uchar	val0xF4_1
)
{
	uchar bs = EENL_PDC_USER_0xF4;
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;


	DEBUG_TRCIN();

	if (NULL == p_buff0xF4) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		info = EENL_GetDeviceInfoIndex(0);
		if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
			ret = ENL_RESULT_INVALID_PARAMETER;
		} else {
			memset(p_buff0xF4, 0x00u, EENL_PDC_USER_0xF4_PUSHEDUP);

			/* 0xF4 Set */

			/* 0xF4 */
			/* 押し上げ無し */
			bs = EENL_PDC_USER_0xF4;

			ret = ENL_get_property(info->devBattery, 0xF4u, &bs, p_buff0xF4);
			if (ret == ENL_RESULT_SUCCESS) {
				/* 正常 */
				/*  取得した情報を元に上書きする */

				/* 1バイト目:メンテナンスモード状態 */
				p_buff0xF4[0] = val0xF4_0;

				/* 2バイト目:自動運転モード */
				/* 押し上げ無し */
				if ((val0xF4_1 == 0x01u) || (val0xF4_1 == 0x02u)) {
					/* 自動運転モードの場合、設定を反映 */
					p_buff0xF4[1] = val0xF4_1;
				} else {
					/* 手動運転モードは反映しない */
					/* ENL_RESULT_SUCCESSを返して、Setは行う */
				}

			} else {
				/* Get失敗時は更新なしで続行 */
				DLOG_Error("battery 0xF4 get fail : %ld", ret);
				ret = EENL_RESULT_NO_DATA_UPDATE;
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xF4のデータ設定処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cmsg			状態変化通知構造体					*/
/*					 [out] val0xF4		0xF4設定値							*/
/*					 [in]　val			付属設定値							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_NOT_INITIALIZED:未初期化					*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータ不正			*/
/*				ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致	*/
/*					  ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ	*/
/*					  ENL_RESULT_UNSUPPORTED_TYPE:適用対象外プロパティ		*/
/*					  ENL_RESULT_EVALUATION_FAILED:値域外データ				*/
/*					  ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗			*/
/*					  ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過		*/
/*					  ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗			*/
/*--------------------------------------------------------------------------*/
/*	detail			:本関数の戻り値がENL_RESULT_SUCCESSのときのみ			*/
/*					 0xF4を設定してください。								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong eenl_BtNotify0xF4CreateDataMatch(
	const EenlEvtSttsChangeReq_t* cmsg,
	uchar* val0xF4,
	uchar						val
)
{
	uchar pos = 0u;
	uchar bs = EENL_PDC_USER_0xF4;
	slong stah = 0;
	slong stam = 0;
	slong stoh = 0;
	slong stom = 0;
	slong dstah = 0;
	slong dstam = 0;
	slong dstoh = 0;
	slong dstom = 0;
	slong ret = ENL_RESULT_SUCCESS;
	EenlDeviceInfo_t* info = NULL;


	DEBUG_TRCIN();

	if (NULL == cmsg || NULL == val0xF4) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		info = EENL_GetDeviceInfoIndex(0);
		if ((NULL == info) || ((NULL != info) && (NULL == info->devBattery))) {
			ret = ENL_RESULT_INVALID_PARAMETER;
		} else {
			/* 押し上げ無し */
			bs = EENL_PDC_USER_0xF4;

			memset(val0xF4, 0x00, bs);

			stah = atoi(cmsg->chargeTimeZone.chargeStartTime.hour);
			stam = atoi(cmsg->chargeTimeZone.chargeStartTime.min);
			stoh = atoi(cmsg->chargeTimeZone.chargeStopTime.hour);
			stom = atoi(cmsg->chargeTimeZone.chargeStopTime.min);
			dstah = atoi(cmsg->chargeTimeZone.dischargeStartTime.hour);
			dstam = atoi(cmsg->chargeTimeZone.dischargeStartTime.min);
			dstoh = atoi(cmsg->chargeTimeZone.dischargeStopTime.hour);
			dstom = atoi(cmsg->chargeTimeZone.dischargeStopTime.min);

			/* 0xF4 Set */
			ret = ENL_get_property(info->devBattery, 0xF4u, &bs, val0xF4);
			if (ENL_RESULT_SUCCESS == ret) {

				switch (val) {
				case 1u:
					/* 売電モード		*/
					pos = 3u;
					break;
				case 2u:
					/* グリーンモード	*/
					pos = 12u;
					break;
				default:
					/* その他（更新なし） */
					ret = EENL_RESULT_NO_DATA_UPDATE;
					break;
				}

				if (ENL_RESULT_SUCCESS == ret) {
					val0xF4[pos] = val;
					val0xF4[pos + 1u] = (uchar)stah;
					val0xF4[pos + 2u] = (uchar)stam;
					val0xF4[pos + 3u] = (uchar)stoh;
					val0xF4[pos + 4u] = (uchar)stom;
					val0xF4[pos + 5u] = (uchar)dstah;
					val0xF4[pos + 6u] = (uchar)dstam;
					val0xF4[pos + 7u] = (uchar)dstoh;
					val0xF4[pos + 8u] = (uchar)dstom;
				} else {
					/* DO NOTHING */
				}

			} else {
				/* Get失敗時は更新なしで続行 */
				DLOG_Error("battery 0xF4 get fail : %ld", ret);
				ret = EENL_RESULT_NO_DATA_UPDATE;
			}
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:蓄電池クラス 0xF4のデータ設定処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　mode			運転モード							*/
/*					 [in]　val0xF4		0xF4設定値							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS:正常終了/							*/
/*					  ENL_RESULT_INVALID_PARAMETER:パラメータエラー/		*/
/*					  EENL_RESULT_NO_DATA_UPDATE:更新なし					*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_Bt0xF4CreateDataMatch(
	uchar	mode,
	uchar* val0xF4
)
{
	uchar pos = 0u;
	uchar val = 0u;
	slong stah = 0;
	slong stam = 0;
	slong stoh = 0;
	slong stom = 0;
	slong dstah = 0;
	slong dstam = 0;
	slong dstoh = 0;
	slong dstom = 0;
	slong ret = ENL_RESULT_SUCCESS;
	EPCS_CHARGE_TIME_ZONE_T	chTmZn;

	DEBUG_TRCIN();

	if (NULL == val0xF4) {
		ret = ENL_RESULT_INVALID_PARAMETER;
	} else {
		memset(&chTmZn, 0x00, sizeof(chTmZn));
		ret = epcs_sync_getChargeTimeZone(mode, &chTmZn);
		if (ret != EPCS_OK) {
			DLOG_Error("battery 0xF4 get timezone(eco) error!:%ld", ret);
			/* 処理続行(M1 B_0326対策) */
		} else {
			/* DO NOTHING */
		}

		DLOG_Debug("<eco> getChargeTimeZone");
		DLOG_Debug(" [eco]");
		DLOG_Debug(" chargeStartTime");
		DLOG_Debug("   hour=\"%s\"strlen=%ld", chTmZn.chargeStartTime.hour,
			strlen(chTmZn.chargeStartTime.hour));
		DLOG_Debug("   min=\"%s\"strlen=%ld", chTmZn.chargeStartTime.min,
			strlen(chTmZn.chargeStartTime.min));
		DLOG_Debug(" chargeStopTime");
		DLOG_Debug("   hour=\"%s\"strlen=%ld", chTmZn.chargeStopTime.hour,
			strlen(chTmZn.chargeStopTime.hour));
		DLOG_Debug("   min=\"%s\"strlen=%ld", chTmZn.chargeStopTime.min,
			strlen(chTmZn.chargeStopTime.min));
		DLOG_Debug(" dischargeStartTime");
		DLOG_Debug("   hour=\"%s\"strlen=%ld", chTmZn.dischargeStartTime.hour,
			strlen(chTmZn.dischargeStartTime.hour));
		DLOG_Debug("   min=\"%s\"strlen=%ld", chTmZn.dischargeStartTime.min,
			strlen(chTmZn.dischargeStartTime.min));
		DLOG_Debug(" dischargeStopTime");
		DLOG_Debug("   hour=\"%s\"strlen=%ld", chTmZn.dischargeStopTime.hour,
			strlen(chTmZn.dischargeStopTime.hour));
		DLOG_Debug("   min=\"%s\"strlen=%ld", chTmZn.dischargeStopTime.min,
			strlen(chTmZn.dischargeStopTime.min));

		stah = atoi(chTmZn.chargeStartTime.hour);
		stam = atoi(chTmZn.chargeStartTime.min);
		stoh = atoi(chTmZn.chargeStopTime.hour);
		stom = atoi(chTmZn.chargeStopTime.min);
		dstah = atoi(chTmZn.dischargeStartTime.hour);
		dstam = atoi(chTmZn.dischargeStartTime.min);
		dstoh = atoi(chTmZn.dischargeStopTime.hour);
		dstom = atoi(chTmZn.dischargeStopTime.min);

		/* 0xF4 Create */
		switch (mode) {
		case 1u:
			/* 売電モード	*/
			pos = 3u;
			/* 売電モード */
			val = 0x01u;
			break;
		case 2u:
			/* グリーンモード	*/
			pos = 12u;
			/* グリーンモード */
			val = 0x02u;
			break;
		default:
			/* その他（更新なし） */
			ret = EENL_RESULT_NO_DATA_UPDATE;
			break;
		}

		if (ENL_RESULT_SUCCESS == ret) {
			val0xF4[pos] = val;
			val0xF4[pos + 1u] = (uchar)stah;
			val0xF4[pos + 2u] = (uchar)stam;
			val0xF4[pos + 3u] = (uchar)stoh;
			val0xF4[pos + 4u] = (uchar)stom;
			val0xF4[pos + 5u] = (uchar)dstah;
			val0xF4[pos + 6u] = (uchar)dstam;
			val0xF4[pos + 7u] = (uchar)dstoh;
			val0xF4[pos + 8u] = (uchar)dstom;
		} else {
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();
	return ret;
}
