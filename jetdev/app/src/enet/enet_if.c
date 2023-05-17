/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jingtong.li										*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/23												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include header File														*/
/*--------------------------------------------------------------------------*/
#include "dlog_g.h"
#include "enet_serv_g.h"
#include "enet_cmn_g.h"
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
extern EnetDeviceRunMode_t g_enet_WiFiRunMode;
/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:ネットワーク種別を取得する								*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:ENET_NETWORK_NONE(0)	ネットワークなし				*/
/*					ENET_NETWORK_WIRED(1)	有線							*/
/*					ENET_NETWORK_WIRELESS(2)	無線						*/
/*--------------------------------------------------------------------------*/
/*	detail			:ネットワーク種別を取得する								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
EnetNetworkType_t ENET_SyncGetNetworkType(void)
{
	EnetNetworkType_t ret = ENET_NETWORK_NONE;
	DEBUG_TRCIN();

	ret = ENET_GetPrefNetworkType();

	DEBUG_TRCOUT();
	return (ret);
}

/*==========================================================================*/
/*	Description		:ネットワーク種別を設定する								*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetNetworkType_t	networkType	設定するネットワーク種別*/
/*					EnetNettypeExt_t	ext			拡張設定				*/
/*--------------------------------------------------------------------------*/
/*	return			:INT	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:ネットワーク種別を設定する								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncSetNetworkTypeExt(EnetNetworkType_t networkType, EnetNettypeExt_t ext)
{
	/* 処理結果 */
	slong lfwResult = ENET_SUCCESS;
	slong fncRet = ENET_ERR1;
	EnetFncNetworkType_t prm;
	EnetFncNetworkType_t ret;
	DEBUG_TRCIN();

	/* ATTENTION */
	prm.header.msgId = ENET_SYNC_SET_NETWORKTYPE;
	prm.type = networkType;
	prm.ext = ext;
	lfwResult = FRM_CallSync(ENET_SPORT, sizeof(EnetFncNetworkType_t), &prm, &ret);

	if (0 <= lfwResult)
	{
		/* success */
		fncRet = ret.header.retCode;
	}

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:ネットワーク種別を設定する								*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetNetworkType_t networkType	設定するネットワーク種別*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:ネットワーク種別を設定する								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncSetNetworkType(EnetNetworkType_t networkType)
{
	slong fncRet;
	DEBUG_TRCIN();
	
	fncRet = ENET_SyncSetNetworkTypeExt(networkType, ENET_NETTYPE_EXT_NONE);
	
	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:IPv4 取得												*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetIpv4Settings_t *									*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:IPv4 取得												*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncGetIpAddr4(EnetIpv4Settings_t *retIPv4)
{
	/* 処理結果 */
	slong fncRet = ENET_ERR1;
	DEBUG_TRCIN();

	if (retIPv4 == NULL)
	{
		return (fncRet);
	}

	fncRet = ENET_GetIpAddr4LfwRet(retIPv4);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:IPv4 設定												*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetIpv4Settings_t	*setIPv4	IPv4設定				*/
/*					EnetIpsetExt_t	ext		拡張設定						*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:IPv4 設定												*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncSetIPAddr4Ext(EnetIpv4Settings_t *setIPv4, EnetIpsetExt_t ext)
{
	/* 処理結果 */
	slong lfwResult = ENET_SUCCESS;
	slong fncRet = ENET_ERR1;
	/* メッセージ送受信用変数宣言 */
	EnetFncIpv4_t prm;
	EnetFncIpv4_t ret;
	DEBUG_TRCIN();

	if (setIPv4 == NULL)
	{
		return (fncRet);
	}
	if (ENET_IPSET_EXT_MAX <= ext)
	{
		return (fncRet);
	}
	/* ATTENTION */
	prm.header.msgId = ENET_SYNC_SET_IPADDR4;
	/* additional param */
	prm.v4 = *setIPv4;
	prm.ext = ext;
	lfwResult = FRM_CallSync(ENET_SPORT, sizeof(EnetFncIpv4_t), &prm, &ret);
	if (0 <= lfwResult)
	{
		/* success */
		fncRet = ret.header.retCode;
	}

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:IPv4 設定												*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetIpv4Settings_t										*/
/*--------------------------------------------------------------------------*/
/*	return			:slong													*/
/*--------------------------------------------------------------------------*/
/*	detail			:IPv4 設定												*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncSetIPAddr4(EnetIpv4Settings_t *setIPv4)
{
	slong fncRet;
	DEBUG_TRCIN();

	fncRet = ENET_SyncSetIPAddr4Ext(setIPv4, ENET_IPSET_EXT_NONE);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:IPv4 手動設定値取得									*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetIpv4Settings_t *									*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:IPv4 手動設定値取得									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncGetIpAddr4Setting(EnetIpv4Settings_t *retIPv4)
{
	/* 処理結果 */
	slong fncRet = ENET_ERR1;
	DEBUG_TRCIN();

	if (retIPv4 == NULL)
	{
		return (fncRet);
	}
	fncRet = ENET_GetIPAddr4SettingLfwRet(retIPv4);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:IPv6アドレスを取得する									*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetIpv6Settings_t *retIPv6	取得した値を格納		*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:IPv6アドレスを取得する									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncGetIpAddr6(EnetIpv6Settings_t *retIPv6)
{
	/* 処理結果 */
	slong fncRet = ENET_ERR1;
	DEBUG_TRCIN();

	if (retIPv6 == NULL)
	{
		return (fncRet);
	}
	fncRet = ENET_GetIpAddr6LfwRet(retIPv6);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:する													*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetIpv6Settings_t *setIPv6	IPv6設定				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:する													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncSetIpAddr6(EnetIpv6Settings_t *setIPv6)
{
	/* 処理結果 */
	slong lfwResult = ENET_SUCCESS;
	slong fncRet = ENET_ERR1;
	/* メッセージ送受信用変数宣言 */
	EnetFncIpv6_t prm;
	EnetFncIpv6_t ret;
	DEBUG_TRCIN();

	if (setIPv6 == NULL)
	{
		return (fncRet);
	}
	/* ATTENTION */
	prm.header.msgId = ENET_SYNC_SET_IPADDR6;
	/* additional param */
	prm.v6 = *setIPv6;
	lfwResult = FRM_CallSync(ENET_SPORT, sizeof(EnetFncIpv6_t), &prm, &ret);
	if (0 <= lfwResult)
	{
		/* success */
		fncRet = ret.header.retCode;
	}

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:する													*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetProxy_t *retProxy		取得データを格納する		*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:する													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncGetProxy(EnetProxy_t *retProxy)
{
	/* 処理結果 */
	slong fncRet = ENET_ERR1;
	DEBUG_TRCIN();

	if (retProxy == NULL)
	{
		return (fncRet);
	}
	fncRet = ENET_GetProxy(retProxy);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:する													*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetProxy_t *setProxy		Proxy設定					*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:する													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncSetProxy(EnetProxy_t *setProxy)
{
	/* 処理結果 */
	slong lfwResult = ENET_SUCCESS;
	slong fncRet = ENET_ERR1;
	EnetFncProxy_t prm;
	EnetFncProxy_t ret;
	DEBUG_TRCIN();

	if (setProxy == NULL)
	{
		return (fncRet);
	}
	/* ATTENTION */
	prm.header.msgId = ENET_SYNC_SET_PROXY;
	/* additional param */
	prm.proxy = *setProxy;
	lfwResult = FRM_CallSync(ENET_SPORT, sizeof(EnetFncProxy_t), &prm, &ret);
	if (0 <= lfwResult)	
	{
		/* success */
		fncRet = ret.header.retCode;
	}

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:無線LANのESSIDをスキャンする							*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:無い													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無線LANのESSIDをスキャンする							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void ENET_AsyncScanEssid(void)
{
	/* メッセージ送受信用変数宣言 */
	EnetFnc00_t prm;
	DEBUG_TRCIN();

	memset(&prm, 0, sizeof(EnetFnc00_t));
	/* パラメータヘッダ設定 */
	prm.header.msgId = ENET_ASYNC_SCAN_ESSID; /* ATTENTION */

	/* プロセス呼び出し */
	FRM_MsgSendAsync(ENET_SPORT, sizeof(EnetFnc00_t), &prm);
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:無線LANを設定する										*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetWlanSettings_t* setWlan	無線LAN設定				*/
/*					EnetWlanSetExt_t ext			拡張設定				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無線LANを設定する										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncSetWlanSettingExt(EnetWlanSettings_t *setWlan, EnetWlanSetExt_t ext)
{
	/* 処理結果 */
	slong lfwResult = ENET_SUCCESS;
	slong fncRet = ENET_ERR1;
	/* メッセージ送受信用変数宣言 */
	EnetFncWlan_t prm;
	EnetFncWlan_t ret;
	DEBUG_TRCIN();

	if (setWlan == NULL)
	{
		return (fncRet);
	}
	if (ENET_WLANSET_EXT_MAX <= ext)
	{
		return (fncRet);
	}
	/* ATTENTION */
	prm.header.msgId = ENET_SYNC_SET_WLANSETTING;
	/* additional param */
	prm.wlan = *setWlan;
	prm.ext = ext;
	lfwResult = FRM_CallSync(ENET_SPORT, sizeof(EnetFncWlan_t), &prm, &ret);
	if (0 <= lfwResult)	
	{
		/* success */
		fncRet = ret.header.retCode;
	}

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:無線LANを設定する										*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetWlanSettings_t* setWlan	無線LAN設定				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無線LANを設定する										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncSetWlanSetting(EnetWlanSettings_t *setWlan)
{
	slong fncRet;
	DEBUG_TRCIN();

	fncRet = ENET_SyncSetWlanSettingExt(setWlan, ENET_WLANSET_EXT_NONE);
	
	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:無線LAN設定を取得する									*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetWlanSettings_T* getWlan	取得データを格納する	*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無線LAN設定を取得する									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncGetWlanSetting(EnetWlanSettings_t *getWlan)
{
	/* 処理結果 */
	slong fncRet = ENET_ERR1;
	DEBUG_TRCIN();

	if (getWlan == NULL)
	{
		return (fncRet);
	}
	fncRet = ENET_GetWlanSetting(getWlan);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:無線LANのステータスを取得する							*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:slong		処理結果									*/
/*					0~100	電波強度										*/
/*					-1		無線LANデバイス未検出							*/
/*--------------------------------------------------------------------------*/
/*	detail			:無線LANのステータスを取得する							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncGetWlanStatus(void)
{
	/* 処理結果 */
	slong fncRet = ENET_ERR1;
	DEBUG_TRCIN();

	fncRet = ENET_GetWlanStatus();

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:無線LANをON/OFF設定する								*/
/*--------------------------------------------------------------------------*/
/*	param			:bool_t up	TRUEのときON、FALSEのときOFFにする			*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無線LANをON/OFF設定する								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncSetWlanUp(bool_t up)
{
	/* 処理結果 */
	slong lfwResult = ENET_SUCCESS;
	slong fncRet = ENET_ERR1;
	EnetFncBool_t prm;
	EnetFncBool_t ret;
	DEBUG_TRCIN();
	
	/* ATTENTION */
	prm.header.msgId = ENET_SYNC_SET_WLANUP;
	/* additional param */
	prm.boolean = up;
	/* プロセス呼び出し */
	lfwResult = FRM_CallSync(ENET_SPORT, sizeof(EnetFncBool_t), &prm, &ret);
	if (0 <= lfwResult)	
	{
		/* success */
		fncRet = ret.header.retCode;
	}

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:ルータとPING疎通確認する								*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:無い													*/
/*--------------------------------------------------------------------------*/
/*	detail			:ルータとPING疎通確認する								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void ENET_AsyncChkRouterConnection(void)
{
	/* 処理結果 */
	/* メッセージ送受信用変数宣言 */
	EnetFnc00_t prm;
	DEBUG_TRCIN();

	/* パラメータ格納メモリ確保 */
	memset(&prm, 0, sizeof(EnetFnc00_t));
	/* パラメータヘッダ設定 */ 
	prm.header.msgId = ENET_ASYNC_CHK_ROUTERCONNECTION; /* ATTENTION */

	/* プロセス呼び出し */
	FRM_MsgSendAsync(ENET_SPORT, sizeof(EnetFnc00_t), &prm);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:イベント通知登録する									*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong	addSPort	登録するポート番号					*/
/*					ulong	eventFlag	登録するイベントフラグ				*/
/*										ENET_CHGEVNT_～を参照				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:イベント通知登録する									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncSetChgEvntListener(ulong addSPort, ulong eventFlag)
{
	/* 処理結果 */
	slong lfwResult = ENET_SUCCESS;
	slong fncRet = ENET_ERR1;
	/* メッセージ送受信用変数宣言 */
	EnetFncListenerPort_t prm;
	EnetFncListenerPort_t ret;
	DEBUG_TRCIN();

 	/* ATTENTION */
	prm.header.msgId = ENET_SYNC_SET_CHGEVNTLISTENER;
	/* additional param */
	prm.port = addSPort;
	prm.flag = eventFlag;
	lfwResult = FRM_CallSync(ENET_SPORT, sizeof(EnetFncListenerPort_t), &prm, &ret);
	if (0 <= lfwResult)
	{
		/* success */
		fncRet = ret.header.retCode;
	}

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:イベント通知登録を解除する								*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong	rmvSPort	登録解除するポート番号				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:イベント通知登録を解除する								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncRmvChgEvntListener(ulong rmvSPort)
{
	/* 処理結果 */
	slong fncRet = ENET_ERR1;
	DEBUG_TRCIN();

	fncRet = ENET_RmvChgEvntListener(rmvSPort);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:無線LANインタフェース名を取得する						*/
/*--------------------------------------------------------------------------*/
/*	param			:schar* ifName											*/
/*					インタフェース名を格納するバッファのアドレス			*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:無線LANインタフェース名を取得する						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SyncGetWlanIfName(schar ifName[ENET_WLAN_IF_NAME_STR_LEN])
{
	/* 処理結果 */
	slong fncRet = ENET_ERR1;
	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == ifName)
	{
		return ENET_ERR1;
	}
	fncRet = ENET_GetWlanIfName(ifName);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/06 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void ENET_AsyncWriteWlanLimit(slong mins)
{
	EnetFnc00_t msg;
	DEBUG_TRCIN();
	msg.header.msgId = ENET_ASYNC_WRITE_WLANLIMIT;
	msg.param = mins;
	FRM_MsgSendAsync(ENET_SPORT, sizeof(msg), &msg);
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/10 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void ENET_AsyncRestartWiFi(void)
{
	EnetFnc00_t msg;
	DEBUG_TRCIN();
	msg.header.msgId = ENET_ASYNC_RESTARTWIFI;
	FRM_MsgSendAsync(ENET_SPORT, sizeof(msg), &msg);
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/10 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void ENET_AsyncSetDeviceRun(EnetDeviceRunMode_t mode)
{
	g_enet_WiFiRunMode = mode;
	EnetFnc00_t msg;
	DEBUG_TRCIN();
	msg.header.msgId = ENET_ASYNC_RESTARTWIFI;
	FRM_MsgSendAsync(ENET_SPORT, sizeof(msg), &msg);
	DEBUG_TRCOUT();
}
/*End Of File*/
