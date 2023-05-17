/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jiayu.li											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/14												*/
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
/*	Description		:全クラス 0x80の設定処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　val0x80			0x80設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了)											*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/13 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_AllNotify0x80(uchar val0x80)
{
	slong ret = ENL_RESULT_SUCCESS;	/* 戻り値 */
	void* dev_list[EENL_DEV_LIST_MAX] = { 0 };	/* デバイスリスト */

	DEBUG_TRCIN();

	/* デバイスリストの作成 */
	EENL_SetBroadcastDevList(dev_list);

	/* ブロードキャストで設定しないデバイスにNULLを設定する */
	dev_list[EENL_DEV_LIST_NODE] = NULL;

	if (val0x80 != 0x00)
	{
		/* 設定値が確定している場合は、ENLに反映 */
		EENL_BroadcastSetPropertyValue(dev_list, EENL_DEV_LIST_MAX, 0x80u, val0x80, 0u, EENL_ANNOUNCE_ON);
	} else {
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:epcsの変更通知イベント処理（動作モード）				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cmsg			状態変化通知イベント構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-2:引数不正)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/13 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsOpeMode(EenlEvtSttsChangeReq_t* cmsg)
{
	uchar val0xF0 = 0u;	/* 蓄電池クラスユーザ定義の0xF0(運転モード)格納用変数 */
	uchar val0xDA = 0u;	/* 蓄電池クラスの0xDA(運転モード設定)格納用変数 */
	uchar val_v2h = 0u;	/* 車両充放電器クラスの0xDA(運転モード設定)格納用変数 */
	slong ret = 0;	/* 戻り値 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */
	uchar val0xF4_0 = 0xFFu;	/* 蓄電池クラスユーザ定義の0xF4(自動運転設定)のメンテナンスモード状態格納用変数 */
	uchar val0xF4_1 = 0u;	/* 蓄電池クラスユーザ定義の0xF4(自動運転設定)の自動運転モード状態格納用変数 */

	DEBUG_TRCIN();

	if (cmsg == NULL)
	{
		/* 引数不正 */
		return -2;
	} else {
		/* 処理無し */
	}

	DLOG_Debug("value: 0x%02x(%d)", cmsg->opeMode, cmsg->opeMode);
	DLOG_Debug("<pushedUp>: 0x%02x", g_eenlPushedUp);

	/* 状態変化通知された時の運転モード保存 */
	g_eenlKeepOpeMode = cmsg->opeMode;

	DLOG_Debug("mode SK,KN,N1");
	val0xF0 = 0x00u;		/* 蓄電池 0xF0はサポート */
	val0xF4_0 = 0xFFu;		/* 蓄電池 0xF4は未サポート(ニチコン向けはサポート) */
	val0xF4_1 = 0x00u;		/* 蓄電池 0xF4は未サポート(ニチコン向けはサポート) */
	/* 押し上げ無し */
	DLOG_Debug("mode SK,KN,N1");
	if (EENL_SpOpemodeChk(cmsg->opeMode) == TRUE)
	{ 
		/* 特殊運転モードなら */
		val0xF0 = 0x07u;
		val0xDA = 0x40u;
		val0xF4_0 = 0x00u; /* メンテナンスモード状態を0x00 実行していないにする */
		val0xF4_1 = 0x00u;

		if (cmsg->opeMode == EPCS_OPEMODE_B)
		{	/* 車両充電 */
			val_v2h = 0x42u;
		} else if (cmsg->opeMode == EPCS_OPEMODE_C)
		{	/* 車両放電 */
			val_v2h = 0x43u;
		} else {
			val_v2h = 0x40u;
		}
	} else {
		switch (cmsg->opeMode) {
		case EPCS_OPEMODE_1:	/* 売電モード */
			val0xF0 = 0x01u;
			val0xDA = 0x46u;
			val0xF4_0 = 0x00u; /* メンテナンスモード状態を0x00 実行していないにする */
			val0xF4_1 = 0x01u;
			val_v2h = 0x40u;
			break;
		case EPCS_OPEMODE_2:	/* グリーンモード */
			val0xF0 = 0x02u;
			val0xDA = 0x46u;
			val0xF4_0 = 0x00u; /* メンテナンスモード状態を0x00 実行していないにする */
			val0xF4_1 = 0x02u;
			val_v2h = 0x40u;
			break;
		case EPCS_OPEMODE_4:	/* 充電 */
			val0xF0 = 0x03u;
			val0xDA = 0x42u;
			val0xF4_0 = 0x00u; /* メンテナンスモード状態を0x00 実行していないにする */
			val0xF4_1 = 0x00u;
			val_v2h = 0x40u;
			break;
		case EPCS_OPEMODE_5:	/* 放電 */
			val0xF0 = 0x04u;
			val0xDA = 0x43u;
			val0xF4_0 = 0x00u; /* メンテナンスモード状態を0x00 実行していないにする */
			val0xF4_1 = 0x00u;
			val_v2h = 0x40u;
			break;
		case EPCS_OPEMODE_6:	/* 待機 */
			val0xF0 = 0x05u;
			val0xDA = 0x44u;
			val0xF4_0 = 0x00u; /* メンテナンスモード状態を0x00 実行していないにする */
			val0xF4_1 = 0x00u;
			val_v2h = 0x44u;
			break;
		case EPCS_OPEMODE_7:	/* メンテナンスモード */
			val0xF0 = 0x06u;
			val0xDA = 0x40u;
			val0xF4_0 = 0x01u; /* メンテナンスモード状態を0x01 実行しているにする */
			val0xF4_1 = 0x00u;
			val_v2h = 0x40u;
			break;
		default:	/* その他 */
			val0xF0 = 0x00u;
			val0xDA = 0x00u;
			val0xF4_0 = 0xFFu;
			val0xF4_1 = 0x00u;
			break;
		}
	}

	DLOG_Debug("[0xF0]val0xF0=0x%02X. [0xDA]val0xDA=0x%02X. [0xF4]val0xF4_0=0x%02X.val0xF4_1=0x%02X.", val0xF0, val0xDA, val0xF4_0, val0xF4_1);
	info = EENL_GetDeviceInfoIndex(0);
	if (info == NULL)
	{
		/* ENL未構築の場合、ENL充放電制御チェックのみ実施する */
		/* ※val0xDAは「ニチコン/押し上げ不定」の設定となっている場合があるが、充電/放電の値は全仕向けで同じため、判定に使用可能。 */
		/* ※M1/M2のeenlでは「押し上げ不定」は「押し上げなし」と判定されるように実装されている。 */
		if (val0xDA != 0x00)
		{
			/* ENL充放電制御チェック */
			ret = EENL_CheckEnlOpectrl(val0xDA);
			if (ret != EENL_OK)
			{
				DLOG_Error("check ENL remote OpeMode ERROR!(ret=%ld)", ret);
			} else {
				/* 処理無し */
			}
		} else {
			/* 処理無し */
		}
	} else {
		if (info->devBattery != NULL)
		{
			/* 0xF0 */
			EENL_BtNotify0xF0(val0xF0);

			/* 0xDA */
			EENL_BtNotify0xDA(val0xDA);

			/* 0xF4 */
			EENL_BtNotify0xF4(val0xF4_0, val0xF4_1);
		} else {
			/* 処理無し */
		}
		/* batteryが有るなら */
		if (info->devV2h != NULL)
		{
			if (val_v2h != 0x00)
			{
				/* 設定値が決まっている場合、ENLに反映 */
				/* 0xDA */
				EENL_V2hNotify0xDA(val_v2h);
			} else {
				/* 処理無し */
			}
		} else {
			/* 処理無し */
		}
		/* V2Hが有るなら */
	}

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:epcsの変更通知イベント処理（非常時安心設定）			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cmsg			状態変化通知イベント構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了/-2:引数不正)					*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/13 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsEmgPer(EenlEvtSttsChangeReq_t* cmsg)
{
	/* HEMS制御対応：ニチコンの場合は0xF2(非常時安心設定)に反映させる */
	uchar val = 0u;			/* 変更後の非常時安心設定変数 */
	slong ret = 0;			/* 戻り値初期化 */

	DEBUG_TRCIN();

	if (cmsg == NULL)
	{
		/* 引数不正 */
		return -2;
	} else {
		/* 処理無し */
	}

	DLOG_Debug("value: %d", cmsg->emergencyPercent);

	val = cmsg->emergencyPercent;
	if (val > EENL_EMERGENCY_100)
	{
		/* 100以上 */
		val = EENL_EMERGENCY_100;
	} else {
		/* 処理無し */
	}

	DLOG_Debug("value: %d -> %d(0x%02x)", cmsg->emergencyPercent, val, val);

	/* 0-100の範囲内の場合 */
	/* ucharなので0以上確定 */
	if (val <= EENL_LIMIT_100)
	{
		EENL_BtNotify0xF2(val);
	} else {
		/* 範囲外の場合 */
		DLOG_Error("battery 0xF2(0xF9) out of range : %d", val);
		/* 異常 */
		ret = -1;
	}

	DLOG_Debug("<<<eenl_EvtEpcsEmgPer>>> end");

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:epcsの変更通知イベント処理（充放電時間帯）				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cmsg			状態変化通知イベント構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-2:引数不正)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/13 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsChargeTimeZone(EenlEvtSttsChangeReq_t* cmsg)
{
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */
	uchar mode = 0u;				/* モード変数 */

	DEBUG_TRCIN();

	if (cmsg == NULL)
	{
		/* 引数不正 */
		return -2;
	} else {
		/* 処理無し */
	}

	info = EENL_GetDeviceInfoIndex(0);
	if (info->devBattery == NULL)
	{ /* batteryが無いなら */
		DLOG_Debug("devBattery non");
		return 0;
	} else {
		/* 処理無し */
	}

	DLOG_Debug("timezoneOpeMode : %d", cmsg->timeZoneOpeMode);
	DLOG_Debug("chargeStartTime : %s:%s", cmsg->chargeTimeZone.chargeStartTime.hour,
		cmsg->chargeTimeZone.chargeStartTime.min);
	DLOG_Debug("chargeStopTime  : %s:%s", cmsg->chargeTimeZone.chargeStopTime.hour,
		cmsg->chargeTimeZone.chargeStopTime.min);
	DLOG_Debug("dischargeStartTime : %s:%s", cmsg->chargeTimeZone.dischargeStartTime.hour,
		cmsg->chargeTimeZone.dischargeStartTime.min);
	DLOG_Debug("dischargeStopTime  : %s:%s", cmsg->chargeTimeZone.dischargeStopTime.hour,
		cmsg->chargeTimeZone.dischargeStopTime.min);

	switch (cmsg->timeZoneOpeMode) {
	case 1:						/* 売電モード */
		mode = 1u;
		break;
	case 2:						/* グリーンモード */
		mode = 2u;
		break;
	default:					/* その他 */
		mode = 0u;
		break;
	}

	DLOG_Debug("mode : %d -> %d", cmsg->timeZoneOpeMode, mode);
	EENL_BtNotify0xF4_0xF8(cmsg, mode);

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:epcsの変更通知イベント処理（運転中状態）				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cmsg			状態変化通知イベント構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了/-2:引数不正)					*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/13 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsOpeStts(EenlEvtSttsChangeReq_t* cmsg)
{
	uchar val = 0u;			/* 0xCF変数 */
	uchar val2 = 0u;		/* 0x80変数 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */

	DEBUG_TRCIN();

	if (cmsg == NULL)
	{
		/* 引数不正 */
		return -2;
	} else {
		/* 処理無し */
	}

	info = EENL_GetDeviceInfoIndex(0);
	if (info == NULL)
	{
		DLOG_Error("info error! : null");
		return -1;
	} else {
		/* 処理無し */
	}

	DLOG_Debug("value: %d", cmsg->operation);
	val = 0x00u;
	val2 = 0x00u;
	switch (cmsg->operation) {
	case EPCS_OPE_STOP:			/* 停止 */
		val = 0x40u;
		val2 = 0x31u;
		break;
	case EPCS_OPE_WAIT:			/* 待機 */
		val = 0x44u;
		val2 = 0x30u;
		break;
	case EPCS_OPE_DISCHARGE:	/* 放電 */
		val = 0x43u;
		val2 = 0x30u;
		break;
	case EPCS_OPE_CHARGE:		/* 充電 */
		val = 0x42u;
		val2 = 0x30u;
		break;
	default:					/* その他　急速充電は非対応 */
		val = 0x00u;
		val2 = 0x00u;
		break;
	}

	/* グローバル情報更新 */
	g_eenlOperation = cmsg->operation;

	/* 0xCF */
	EENL_BtNotify0xCF(val);

	/* 0x80 */
	EENL_AllNotify0x80(val2);

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:epcsの変更通知イベント処理（停電状態）					*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cmsg			状態変化通知イベント構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-2:引数不正)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/13 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsPwroutage(EenlEvtSttsChangeReq_t* cmsg)
{
	uchar val = 0xFFu;		/* 0xDB */
	uchar val2 = 0xFFu;		/* 0xD0 */

	DEBUG_TRCIN();

	if (cmsg == NULL)
	{
		/* 引数不正 */
		return -2;
	} else {
		/* 処理無し */
	}

	DLOG_Debug("pwrCut : %d", cmsg->pwrCut);
	DLOG_Debug("operation : %d", cmsg->operation);

	if (cmsg->pwrCut == EPCS_PWROUTAGE_YES)
	{
		/* 停電の場合 */
		val = 0x01u;	/* 独立 */
		val2 = 0x01u;	/* 独立 */
	} else {
		val = 0x02u;	/* 系統連系（逆潮流不可） */
		val2 = 0x00u;	/* 系統連系（逆潮流可） */
	}

	/* グローバル情報更新 */
	g_eenlPwrCut = cmsg->pwrCut;

	/* V2Hの0xDB更新 */
	EENL_UpdateV2h0xDB();
	/* 失敗しても処理続行のため、戻り値は参照しない */

	/* 0xDB */
	EENL_BtNotify0xDB(val);
	/* 0xD0 */
	EENL_SolarNotify0xD0(val2);
	/* 0xD0 */
	EENL_MpcsNotify0xD0(val2);

	DEBUG_TRCOUT();
	return 0;
}

/*==========================================================================*/
/*	Description		:epcsの変更通知イベント処理（SOH）						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cmsg			状態変化通知イベント構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-2:引数不正)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsSoh(EenlEvtSttsChangeReq_t* cmsg)
{
	slong dat = 0;		/* 0xE5変数 */

	DEBUG_TRCIN();

	if (cmsg == NULL)
	{
		/* 引数不正 */
		return -2;
	} else {
		/* 処理無し */
	}

	DLOG_Debug("value: %d", cmsg->soh);
	dat = cmsg->soh;

	if (dat >= EENL_LIMIT_0 && dat <= EENL_LIMIT_1000)
	{
		/* 範囲内(0-1000) */
		/* epcsからは0.1%単位のデータで来るので、1%単位に変更 */
		dat = (slong)((dat + 5) / 10);
		/* 0xE5 */
		EENL_BtNotify0xE5(dat);
	} else {
		/* 範囲外 */
		/* 処理無し */
	}

	DEBUG_TRCOUT();
	return 0;
}

/*==========================================================================*/
/*	Description		:epcsの変更通知イベント処理（出力抑制状態(CMD81)通知）	*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cmsg			状態変化通知イベント構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-3:パラメータ不正)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsSuppressState(EenlEvtSttsChangeReq_t* cmsg)
{
	uchar work_D1 = 0;		/* 0xD1変数 */

	DEBUG_TRCIN();
	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* epcsから取得した出力抑制状態を変換する */
	/* 0x00:抑制なし       = 0x44:抑制未実施 */
	/* 0x01:電圧上昇抑制中 = 0x42:抑制中(出力制御以外) */
	/* 0x02:出力制御中     = 0x41:抑制中(出力制御) */
	switch (cmsg->suppressState) {
	case EPCS_PVGEN_NOTICE_SUPPRESS_NONE:   /* 0x00 抑制なし */
		work_D1 = 0x44u; /* 抑制未実施 */
		break;
	case EPCS_PVGEN_NOTICE_SUPPRESS_VOLT:   /* 0x01 電圧上昇抑制中 */
		work_D1 = 0x42u; /* 抑制中(出力制御以外) */
		break;
	case EPCS_PVGEN_NOTICE_SUPPRESS_OUTPUT: /* 0x02 出力制御中 */
		work_D1 = 0x41u; /* 抑制中(出力制御) */
		break;
	default:
		return ENL_RESULT_INVALID_PARAMETER;
	}

	/* 0xD1 */
	EENL_SolarNotify0xD1(work_D1);

	DEBUG_TRCOUT();
	return 0;
}

/*==========================================================================*/
/*	Description		:epcsの変更通知イベント処理（充放電電力設定値リセット通知）*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cmsg			状態変化通知イベント構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-2:パラメータ不正)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsResetPwrset(EenlEvtSttsChangeReq_t* cmsg)
{
	DEBUG_TRCIN();

	if (NULL == cmsg)
	{
		/* 引数不正 */
		return -2;
	} else {
		/* 処理無し */
	}

	/* 0xE7 */
	/* 0xE8 */
	EENL_BtNotify0xE7_0xE8(cmsg);

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:epcsの変更通知イベント処理（AC充電量設定値待機動作通知）*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了)											*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsAcChargeWait(void)
{
	slong ret = 0;		/* 関数戻り値初期化 */

	DEBUG_TRCIN();

	/* グローバル変数を更新してファイル保存 */
	g_eenlBattery0xAA = 0;
	EENL_FileWrite();

	DLOG_Debug("save file ok!");
	/* EPCSに反映 */
	/* AC充電量を0(初期値)に戻す */
	ret = epcs_sync_setAcChargePwr(0u);
	if (ret == EPCS_OK)
	{
		/* 設定OK */
		DLOG_Debug("epcs reset ac charge ok!");
	} else {
		/* 設定失敗 */
		DLOG_Error("epcs reset ac charge NG!(%ld)", ret);
		/* 処理は継続 */
	}

	/* ENLライブラリのAC充電量設定値(0xAA)も0(初期値)に戻す */
	EENL_BtNotify0xAA(0u);

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:epcsの変更通知イベント処理（AC放電量設定値待機動作通知）*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了)											*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsAcDischargeWait(void)
{
	slong ret = 0;		/* 関数戻り値初期化 */

	DEBUG_TRCIN();

	/* グローバル変数を更新してファイル保存 */
	g_eenlBattery0xAB = 0;
	EENL_FileWrite();

	DLOG_Debug("save file ok!");
	/* EPCSに反映 */
	/* AC放電量を0(初期値)に戻す */
	ret = epcs_sync_setAcDischargePwr(0u);
	if (ret == EPCS_OK)
	{
		/* 設定OK */
		DLOG_Debug("epcs reset ac discharge ok!");
	} else {
		/* 設定失敗 */
		DLOG_Error("epcs reset ac discharge NG!(%ld)", ret);
		/* 処理は継続 */
	}

	/* ENLライブラリのAC充電量設定値(0xAB)も0(初期値)に戻す */
	EENL_BtNotify0xAB(0u);

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:EPCSからの出力増減速度変化通知イベント処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　spmsg	設定変更通知イベントメッセージ構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-3:パラメータ異常)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsOutputFluctuateSpdChg0xB3(EenlEvtSpChangeReq_t* spmsg)
{
	ulong work_B3 = 0u;		/* 0xB3引数 */
	ushort val0xB3 = 0u;	/* 0xB3変数 */

	DEBUG_TRCIN();
	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* 分から秒に換算 */
	work_B3 = (ulong)(spmsg->otp * 60);

	/* 範囲チェック */
	/* 整定値の規格上では30分以上の値はあるが、出力制御が30分単位なので、 */
	/* 最大でも30(分)*60(秒)=1800(秒)となるが、規格上の範囲はチェックしておく */
	if (work_B3 > EENL_LIMIT_65533)
	{
		/* 範囲外 */
		return ENL_RESULT_INVALID_PARAMETER;
	} else {
		/* 処理無し */
	}

	val0xB3 = (ushort)work_B3;

	/* 0xB3 */
	EENL_SolarNotify0xB3(val0xB3);

	DEBUG_TRCOUT();
	return 0;
}

/*==========================================================================*/
/*	Description		:EPCSからの力率一定制御変化通知イベント処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　spmsg	設定変更通知イベントメッセージ構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了)											*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsPwrFactCtrlSpdChg(EenlEvtSpChangeReq_t* spmsg)
{
	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* 範囲チェックはepcsで実施済み*/

	/* 0xC0*/
	EENL_SolarNotify0xC0(spmsg->pwrFactCtrl);

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:EPCSからの発電電力上限変化通知イベント処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　spmsg	設定変更通知イベントメッセージ構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了)											*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsGenPwrCapSpdChg(EenlEvtSpChangeReq_t* spmsg)
{
	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* 範囲チェックはepcsで実施済み */

	/* 0xB4 */
	EENL_SolarNotify0xB4(spmsg->genPwrCap);

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:EPCSからのFIT契約タイプ変化通知イベント処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　spmsg	設定変更通知イベントメッセージ構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了)											*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsFitTypeSpdChg(EenlEvtSpChangeReq_t* spmsg)
{
	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* 範囲チェックはepcsで実施済み */

	/* 0xC1 */
	EENL_SolarNotify0xC1(spmsg->fitType);

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:EPCSからの自家消費タイプ変化通知イベント処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　spmsg	設定変更通知イベントメッセージ構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了)											*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsSelfConsumptionTypeSpdChg(EenlEvtSpChangeReq_t* spmsg)
{
	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* 範囲チェックはepcsで実施済み */

	/* 0xC2 */
	EENL_SolarNotify0xC2(spmsg->selfConsumptionType);

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:EPCSからの設備認定容量変化通知イベント処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　spmsg	設定変更通知イベントメッセージ構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了)											*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsFacilityCertifiedCapacitySpdChg(EenlEvtSpChangeReq_t* spmsg)
{
	ulong work_C3 = 0u;		/* 0xC3引数 */
	ushort val0xC3 = 0u;	/* 0xC3変数 */

	DEBUG_TRCIN();
	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* epcsから0.01kW単位で取得するので、Wに変換する */
	/* 0590[0.01kW]→5900[W] */
	/*
	 *	ADAM修正計画No.408
	 *	0.01kW単位ではなく、W単位でもつ
	*/
	work_C3 = spmsg->facilityCertifiedCapacity;

	/* 範囲チェック */
	if (work_C3 > EENL_LIMIT_65533)
	{
		/* 65533より大きい場合、65533に制限 */
		val0xC3 = EENL_LIMIT_65533;
	} else {
		/* 範囲内なので、そのまま利用 */
		val0xC3 = (ushort)work_C3;
	}

	/* 0xC3 */
	EENL_SolarNotify0xC3(val0xC3);

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:試運転状態設定											*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　fmsg			状態変化通知イベント構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-2:引数不正)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEguiTestrunStatus(EenlTestRun_t* fmsg)
{
	slong ret = 0;		/* 関数戻り値初期化 */

	DEBUG_TRCIN();

	if (fmsg == NULL)
	{
		/* 引数不正 */
		return -2;
	} else {
		/* 処理無し */
	}

	/* 試運転状態を保存 */
	if (fmsg->state == EENL_TESTRUN_START)
	{
		g_eenlTestRunState = EENL_TESTRUN_START;
	} else {
		g_eenlTestRunState = EENL_TESTRUN_STOP;
	}

	DLOG_Debug("test run state = [%d](stt_testrun:%d)", g_eenlTestRunState, fmsg->state);
#if EENL_VPP_RAMFILE
	/* RAMファイルに書き込み(ファイルがない時の作成は呼び出し先で実施) */
	ret = EENL_CmnFileWrite(EENL_TESTRUNSTATE_FILE, &g_eenlTestRunState, sizeof(g_eenlTestRunState));
	if (ret == 0)
	{
		DLOG_Debug("write ram file. test run state = %d.", g_eenlTestRunState);
	} else {
		/* 書き込めなくても処理は続行 */
		DLOG_Error("can NOT write ram file! test run state = %d.", g_eenlTestRunState);
	}
#endif

	/* 非同期のため、応答は不要 */
	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:epcsのV2Hコネクタロック状態変化通知処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　v2hsttsmsg	V2H状態変化イベントメッセージ構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了/-2:引数不正)					*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsV2hConnectorLock(EenlEvtV2hSttsChangeReq_t* v2hsttsmsg)
{
	slong ret = 0;		/* 戻り値初期化 */
	EenlDeviceInfo_t* info = NULL;		/* デバイス情報 */
	slong subret = 0;		/* 関数戻り値初期化 */
	bool_t rtn_flg = FALSE;	/* 戻り値フラグ */

	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);
	if (info->devV2h == NULL)
	{ /* v2hが無いなら */
		DLOG_Debug("devV2h non");
		return -1;
	} else {
		/* 処理無し */
	}

	if (v2hsttsmsg == NULL)
	{
		DLOG_Error("v2hsttsmsg NULL");
		/* 引数不正 */
		return -2;
	} else {
		/* 処理無し */
	}

	/* コネクタロック状態 */
	if ((v2hsttsmsg->v2hConnectorLockState == EPCS_V2H_CONNECTOR_LOCK) || (v2hsttsmsg->v2hConnectorLockState == EPCS_V2H_CONNECTOR_UNLOCK))
	{	/* ロックと解除 */
		/* グローバル情報更新 */
		g_eenlV2hConnectorLock = v2hsttsmsg->v2hConnectorLockState;
		DLOG_Error("connector lock update v2h calling.");
		/* CMD30関連EPCにPCS情報を反映 */
		subret = EENL_UpdateEpcsV2h(info);
		DLOG_Error("connector lock update v2h called. ret = %ld", subret);
	} else {
		/* 範囲外 */
		DLOG_Error("V2H v2hConnectorLockState parameter error (%d)", v2hsttsmsg->v2hConnectorLockState);
		rtn_flg = TRUE;
		ret = -2;
	}

	if (rtn_flg == FALSE)
	{
		DLOG_Debug("<<<EENL_EvtEpcsV2hConnectorLock>>> end");
	} else {
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:epcsのV2H車両接続・充放電可否状態変化通知処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　v2hsttsmsg	V2H状態変化イベントメッセージ構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了/-2:引数不正)					*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsV2hAvailabilityCharge(EenlEvtV2hSttsChangeReq_t* v2hsttsmsg)
{
	slong ret = 0;		/* 戻り値初期化 */

	DEBUG_TRCIN();

	if (v2hsttsmsg == NULL)
	{
		DLOG_Error("v2hsttsmsg NULL");
		/* 引数不正 */
		return -2;
	} else {
		/* 処理無し */
	}

	/* 車両接続・充放電可否状態 */
	if ((v2hsttsmsg->v2hAvailabilityStatus == 0xFF) ||		/* 0xFF：不定 */
		(v2hsttsmsg->v2hAvailabilityStatus == 0x30) ||		/* 0x30：車両未接続 */
		(v2hsttsmsg->v2hAvailabilityStatus == 0x40) ||		/* 0x40：車両接続・充電不可・放電不可 */
		(v2hsttsmsg->v2hAvailabilityStatus == 0x41) ||		/* 0x41：車両接続・充電可・放電不可 */
		(v2hsttsmsg->v2hAvailabilityStatus == 0x42) ||		/* 0x42：車両接続・充電不可・放電可 */
		(v2hsttsmsg->v2hAvailabilityStatus == 0x43))
	{	/* 0x43：車両接続・充電可・放電可 */
		/* 設定範囲内なら0xC7をSet */
		/* 0xC7 */
		EENL_V2hNotify0xC7(v2hsttsmsg->v2hAvailabilityStatus);
	} else {
		DLOG_Error("V2H 0xC7 v2hAvailabilityStatus parameter error (%d)", v2hsttsmsg->v2hAvailabilityStatus);
		ret = -1;
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:epcsの変更通知イベント処理（車載電池充電量設定値1変更通知処理）*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　v2hsttsmsg	V2H設定変更イベントメッセージ構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了/-2:引数不正)					*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsV2hChargeAmount1(EenlEvtV2hSttingChangeReq_t* v2hsettingmsg)
{
	ulong val = 0u;		/* 0xE7変数 */
	ulong get_val = 0u;		/* 現在の充電量設定値1(0xE7)変数 */
	slong ret = 0;		/* 関数戻り値 */
	EenlDeviceInfo_t* info = NULL;		/* デバイス情報 */

	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);
	if (info->devV2h == NULL)
	{ /* v2hが無いなら */
		DLOG_Error("devV2h non");
		return -1;
	} else {
		/* 処理無し */
	}

	if (v2hsettingmsg == NULL)
	{
		DLOG_Error("v2hsttsmsg NULL");
		/* 引数不正 */
		return -2;
	} else {
		/* 処理無し */
	}
	val = v2hsettingmsg->v2hChargeAmountSet1;
	/* 範囲内(0-999,999,999) */
	if (val > 0x3B9AC9FF)
	{
		DLOG_Error("V2H 0xE7 out of range : %ld", val);
		/* 異常終了 */
		return -1;
	} else {
		/* 処理無し */
	}

	/* 現在のENLの値と比較 */
	/* 現在の充電量設定値1(0xE7)を取得 */
	ret = ENL_get_property_value(info->devV2h, 0xE7u, &get_val, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		if (val != get_val)
		{/* 現在値と異なっていたら */
			DLOG_Debug("V2H 0xE7 value not same.val=0x%08lX.get_val=0x%08lX", val, get_val);
			/* 他のプロセスからの通知とする。 */
			v2hsettingmsg->changedProcessMsgId = 0;
		} else {
			/* 処理無し */
		}
	} else {
		DLOG_Error("V2H 0xE7 get_property_value error=%ld", ret);
		/* 失敗しても続行。 */
	}

	/* 0xE7 */
	EENL_V2hNotify0xE7(val);

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:epcsの変更通知イベント処理（車載電池充電電力設定値変更通知処理）*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　v2hsttsmsg	V2H設定変更イベントメッセージ構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了/-2:引数不正)					*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsV2hPowerSetCharge(EenlEvtV2hSttingChangeReq_t* v2hsettingmsg)
{
	ulong val = 0u;		/* 0xEB変数 */

	DEBUG_TRCIN();

	if (v2hsettingmsg == NULL)
	{
		DLOG_Error("v2hsttsmsg NULL");
		/* 引数不正 */
		return -2;
	} else {
		/* 処理無し */
	}

	val = v2hsettingmsg->v2hPowerSetCharge;
	/* 範囲内(0-999,999,999) */
	if (val > 0x3B9AC9FF)
	{
		DLOG_Error("V2H 0xEB out of range : %ld", val);
		/* 異常 */
		return -1;
	} else {
		/* 処理無し */
	}

	/* 0xEB */
	EENL_V2hNotify0xEB(val);

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:epcsの変更通知イベント処理（車載電池放電電力設定値変更通知処理）*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　v2hsttsmsg	V2H設定変更イベントメッセージ構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了/-2:引数不正)					*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsV2hPowerSetDischarge(EenlEvtV2hSttingChangeReq_t* v2hsettingmsg)
{
	ulong val = 0u;		/* 0xEC変数 */

	DEBUG_TRCIN();

	if (v2hsettingmsg == NULL)
	{
		DLOG_Error("v2hsttsmsg NULL");
		/* 引数不正 */
		return -2;
	} else {
		/* 処理無し */
	}

	val = v2hsettingmsg->v2hPowerSetDiscCharge;
	/* 範囲内(0-999,999,999) */
	if (val > 0x3B9AC9FF)
	{
		DLOG_Error("V2H 0xEC out of range : %ld", val);
		/* 異常 */
		return -1;
	} else {
		/* 処理無し */
	}

	/* 0xEC */
	EENL_V2hNotify0xEC(val);

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:epcsの変更通知イベント処理（車両充放電器定格電圧変更通知処理）*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　v2hsttsmsg	V2H設定変更イベントメッセージ構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了/-2:引数不正)					*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsV2hRatedVolt(EenlEvtV2hSttingChangeReq_t* v2hsettingmsg)
{
	ushort val = 0u;		/* 0xD2変数 */
	ulong get_val = 0u;		/* 現在の車両充放電器定格電圧(0xD2)変数 */
	slong ret = 0;			/* 関数戻り値 */
	EenlDeviceInfo_t* info = NULL;	/* デバイス情報 */

	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);
	if (info->devV2h == NULL)
	{/* v2hが無いなら */
		DLOG_Debug("devV2h non");
		return -1;
	} else {
		/* 処理無し */
	}

	if (v2hsettingmsg == NULL)
	{
		DLOG_Error("v2hsttsmsg NULL");
		/* 引数不正 */
		return -2;
	} else {
		/* 処理無し */
	}

	val = v2hsettingmsg->v2hReratedVolt;
	/* 範囲内(0-32,766) */
	if (val > 0x7FFE)
	{
		DLOG_Error("V2H 0xD2 out of range : %d", val);
		return -1;
	} else {
		/* 処理無し */
	}

	/* 現在のENLの値と比較 */
	/* 現在の車両充放電器定格電圧(0xD2)を取得 */
	ret = ENL_get_property_value(info->devV2h, 0xD2, &get_val, 0);
	if (ret == ENL_RESULT_SUCCESS)
	{
		if (val != (ushort)get_val)
		{/* 現在値と異なっていたら */
			DLOG_Debug("V2H 0xD2 value not same.val=0x%04X.get_val=0x%04X", val, (ushort)get_val);
			/* 他のプロセスからの通知とする */
			v2hsettingmsg->changedProcessMsgId = 0;
		} else {
			/* 処理無し */
		}
	} else {
		DLOG_Error("V2H 0xD2 get_property_value error=%ld", ret);
		/* 他失敗しても続行 */
	}

	/* 0xD2 */
	EENL_V2hNotify0xD2(val);

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:epcsの変更通知イベント処理（車載電池放電量設定値変更通知処理）*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　v2hsttsmsg	V2H設定変更イベントメッセージ構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常終了)								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEpcsV2hDischargeSetValue(EenlEvtV2hSttingChangeReq_t* v2hsettingmsg)
{
	ulong val = 0u;		/* 0xEA変数 */
	ulong get_val = 0u;	/* 現在の車載電池放電量設定値(0xEA)変数 */
	slong ret = 0;		/* 関数戻り値 */
	EenlDeviceInfo_t* info = NULL;		/* デバイス情報 */

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	info = EENL_GetDeviceInfoIndex(0);
	if (info->devV2h == NULL)
	{/* v2hが無いなら */
		DLOG_Debug("devV2h non");
		return -1;
	} else {
		/* 処理無し */
	}

	/* 範囲チェックはしない(epcsと同じ範囲 0-999,999,999) */

	val = v2hsettingmsg->v2hDischargeSetValue;

	/* 現在のENLの値と比較 */
	/* 現在の車載電池放電量設定値(0xEA)を取得 */
	ret = ENL_get_property_value(info->devV2h, 0xEAu, &get_val, 0u);
	if (ret == ENL_RESULT_SUCCESS)
	{
		if (val != get_val)
		{/* 現在値と異なっていたら */
			DLOG_Debug("V2H 0xEA value not same.val=0x%08lX.get_val=0x%08lX", val, (ulong)get_val);
			/* 他のプロセスからの通知とする */
			v2hsettingmsg->changedProcessMsgId = 0;
		} else {
			/* 処理無し */
		}
	} else {
		DLOG_Error("V2H 0xEA get_property_value error=%ld", ret);
		/* 失敗しても続行 */
	}

	/* 0xEA */
	EENL_V2hNotify0xEA(val);

	DEBUG_TRCOUT();

	return 0;
}

/*==========================================================================*/
/*	Description		:ESCHからの次回アクセス日時変化通知イベント処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　v2hsttsmsg			設定変更通知イベント構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了)											*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/14 : DHC jiayu.li : 新規作成					*/
/*==========================================================================*/
slong EENL_EvtEschNextAccessDate(EenlEvtSttingChangeReq_t* spmsg)
{
	uchar work_B1[EENL_PDC_SOL_0xB1] = { 0u };/* 0xB1変数 */
	ESCH_DATE_TIME_T nextDate;				/* esch日付型情報 */
	ESCH_DATE_TIME_T cmpSchDate;			/* esch日付型情報 */

	DEBUG_TRCIN();

	/* 実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* 初期化 */
	memset(&work_B1, 0x00u, sizeof(work_B1));
	memset(&nextDate, 0x00, sizeof(nextDate));
	memset(&cmpSchDate, 0x00, sizeof(cmpSchDate));
	memcpy(&nextDate, &spmsg->nextAccessDate, sizeof(nextDate));

	/* 次回アクセス日時の有無判定 */
	if ((memcmp(&nextDate, &cmpSchDate, sizeof(nextDate))) == 0)
	{
		/* 次回アクセス日時が取得できていない場合(ALL 0)は全て0xFF */
		memset(&work_B1, 0xFFu, sizeof(work_B1));
		DLOG_Debug("Next Access Date is none");
	} else {
		/* 次回アクセス日時が取得できている場合 */
		/* EPC用のデータ設定 */
		work_B1[0] = (uchar)((nextDate.year & 0xFF00) >> 8);
		work_B1[1] = (uchar)(nextDate.year & 0x00FF);
		work_B1[2] = nextDate.month;
		work_B1[3] = nextDate.day;
		work_B1[4] = nextDate.hour;
		work_B1[5] = nextDate.minute;
		work_B1[6] = nextDate.second;
	}

	/* 0xB1 */
	EENL_SolarNotify0xB1(work_B1);

	DEBUG_TRCOUT();

	return 0;
}


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
