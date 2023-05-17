/****************************************************************************/
/*	Description		:ノードプロファイルクラス 処理							*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC DX.zhao											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/01												*/
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


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description		:ノードプロファイルの作成								*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info	DEVICE情報									*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_CreateNode(EenlDeviceInfo_t* info)
{
	slong i = 0;							/* MACアドレス加工のループ用 */
	uchar edt = 0x00u;						/* プロパティ値データ */
	uchar macAddr[12u] = { 0u };			/* MACアドレス */
	slong retFun = -1;						/* 呼び出す関数戻り値 */
	slong retCode = EENL_RET_CODE_OK;		/* 戻り値 */

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == info) {
		DEBUG_TRCOUT();
		return EENL_RET_CODE_FAIL;
	} else {
		/* 無し */
	}

	/* ノードプロファイルの作成 */
	retFun = ENL_create_node_profile(&info->node,
		g_nodeProp, NULL, ENL_INSTANCE_MODE_DEVICE);
	if (retFun != ENL_RESULT_SUCCESS) {
		retCode = EENL_RET_CODE_FAIL;
		DLOG_Error("node profile create error!:%ld", retFun);
	} else {
		/* インスタンスのアドレスを保持 */
		g_eenlDevNode = info->node;
	}

	/* ノードプロファイルのプロパティ初期値設定 */
	if (EENL_RET_CODE_OK == retCode) {
		/* 0x80: 動作状態 */
		edt = 0x30u;	/* 起動中(0x30) */
		retFun = EENL_ExtEnlDevSetProperty(NULL, info->node, 0x80u, 1u, &edt);
		if (retFun != ENL_RESULT_SUCCESS) {
			retCode = EENL_RET_CODE_FAIL;
			DLOG_Error("node 0x80 init error!:%ld", retFun);
		} else {
			/* 無し */
		}
	} else {
		/* 無し */
	}

	/* 0x82: 規格バージョン */
	/* ライブラリで自動設定 : (Version 1.14  = 0x01,0x0E,0x01,0x00) */

	if (EENL_RET_CODE_OK == retCode) {
		/* 0x83: 識別番号 */
		/* メーカコード ニチコン 0x00006C */
		memcpy(&g_eenlId[1], &g_eenlCodeNcc[0], 3u);

		/* 取得したMACアドレスを加工 */
		for (i = 0; i < 6; i++) {
			/* 上位バイト */
			macAddr[2u * i] = (uchar)((g_eenlMacAddr[i] & 0xF0u) / (uchar)16u);
			/* 下位バイト */
			macAddr[2u * i + 1u] = (g_eenlMacAddr[i] & 0x0Fu);

			if (macAddr[2u * i] > 9u) {
				/* A-Fの場合 */
				macAddr[2u * i] = (uchar)(macAddr[2u * i] + (uchar)0x37u);
			} else {
				/* 0-9の場合 */
				macAddr[2u * i] = (uchar)(macAddr[2u * i] + (uchar)0x30u);
			}

			if (macAddr[2u * i + 1u] > 9u) {
				/* A-Fの場合 */
				macAddr[2u * i + 1u] = (uchar)(macAddr[2u * i + 1u] + (uchar)0x37u);
			} else {
				/* 0-9 */
				macAddr[2u * i + 1u] = (uchar)(macAddr[2u * i + 1u] + (uchar)0x30u);
			}
		}

		/* ノードプロファイルの識別番号5バイト目は0x30固定 */
		g_eenlId[4] = 0x30u;
		memcpy(&g_eenlId[5], &macAddr[0], 12u);
		retFun = EENL_ExtEnlDevSetProperty(NULL,
			info->node, 0x83u, 17u, (uchar*)g_eenlId);
		if (retFun != ENL_RESULT_SUCCESS) {
			retCode = EENL_RET_CODE_FAIL;
			DLOG_Error("node 0x83 init error!:%ld", retFun);
		} else {
			/* 無し */
		}
	} else {
		/* 無し */
	}

	if (EENL_RET_CODE_OK == retCode) {
		/* 0x88: 異常発生状態 */
		edt = 0x42u;		/* 異常発生なし=0x42 */
		retFun = EENL_ExtEnlDevSetProperty(NULL, info->node, 0x88u, 1u, &edt);
		if (retFun != ENL_RESULT_SUCCESS) {
			retCode = EENL_RET_CODE_FAIL;
			DLOG_Error("node 0x88 init error!:%ld", retFun);
		} else {
			/* 無し */
		}
	} else {
		/* 無し */
	}

	if (EENL_RET_CODE_OK == retCode) {
		/* 0x8A: メーカコード */
		retFun = EENL_ExtEnlDevSetProperty(NULL, info->node,
			0x8Au, 3u, (uchar*)g_eenlCodeNcc);
		if (retFun != ENL_RESULT_SUCCESS) {
			retCode = EENL_RET_CODE_FAIL;
			DLOG_Error("node 0x8A init error!:%ld", retFun);
		} else {
			/* 無し */
		}
	} else {
		/* 無し */
	}

	DEBUG_TRCOUT();
	return retCode;
}
