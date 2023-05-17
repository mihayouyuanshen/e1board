/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Cao.HZ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/05/06												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "enet_cmn_g.h"
#include "enet_serv_g.h"
#include "enet_thread_g.h"
#include "enet_file_g.h"
#include "enet_g.h"
#include "enet_serv.h"
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
ushort g_enet_WlanLimitTime;
uchar g_enet_WlanLimitSignal;
EnetDeviceRunMode_t g_enet_WiFiRunMode = ENET_NORMAL_MODE;
/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
EnetProcElm_t s_enet_sendChgEvntProc[PROCESS_MAX];
/* 最後に設定された無線IF番号 */
uchar s_enet_CurrentIfNo = ENET_NETWORK_NONE;
/* ENET_NETWORK_TYPE_E変換テーブル、type→ネットワークインタフェース名 */
const schar s_enet_interfaceName[4][IFNAMSIZ] = {
		ENET_DEVICE_NAME_UNSET,
		ENET_DEVICE_NAME_ETH0,
		ENET_DEVICE_NAME_RA0 ,
		ENET_DEVICE_NAME_WLAN0
};
/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:リモコンにエラーコードを設定する						*/
/*--------------------------------------------------------------------------*/
/*	param			:schar* errCode エラーコード("BE000"など)				*/
/*--------------------------------------------------------------------------*/
/*	return			:無い													*/
/*--------------------------------------------------------------------------*/
/*	detail			:リモコンにエラーコードを設定する						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void ENET_SetRmcErrCode(schar* errCode)
{
	uchar errRate = ENET_ERRCODE_UNSET_ROUTER_COMM_RATE;
	DEBUG_TRCIN();
	/* 送信エラーコード */
	DLOG_Debug("set enot/elst errCode: %s.\n", errCode);

	/* エラーレート設定 */
	if(strncmp(errCode, ENET_ERRCODE_SET_ROUTER_COMM, 5) == 0)
	{
		errRate = ENET_ERRCODE_SET_ROUTER_COMM_RATE;
	}
	else if(strncmp(errCode, ENET_ERRCODE_UNSET_ROUTER_COMM, 5) == 0)
	{
		errRate = ENET_ERRCODE_UNSET_ROUTER_COMM_RATE;
	}
	else
	{
		DLOG_Error("err! unknown err code\n");
		return;
	}
	/* 送信エラーレート */
	DLOG_Debug("set enot/elst errRate: %c.\n", errRate);
	DLOG_Debug("set enot/elst %s(%c)\n", errCode, errRate);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:指定デバイス(NIC)のデフォルトゲートウェイを取得する	*/
/* 					/proc/net/routeのフォーマット解析を行い、値を取得する	*/
/*--------------------------------------------------------------------------*/
/*	param			:schar* ifaceName	指定デバイス名（e.g. "eth0"）		*/
/* 					EnetIpv4_t* gateway	取得データを格納する				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:指定デバイス(NIC)のデフォルトゲートウェイを取得する	*/
/* 					/proc/net/routeのフォーマット解析を行い、値を取得する	*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_GetDefaultGW(const schar* ifaceName, EnetIpv4_t* gateway)
{
	slong fncRet = ENET_ERR1;
	slong scanElements = 0;
	bool_t dapFlag = FALSE;
	FILE* fp;
	EnetRouteInfo_t rinf = { };
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	if (gateway == NULL)
	{
		DLOG_Error("Err! gateway is NULL.\n");
		return (fncRet);
	}
	gateway->addr32bit = 0; /* 未検出時はこの値(0.0.0.0)が結果となる */
	/* NULLチェック */
	if (ifaceName == NULL)
	{
		DLOG_Error("Err! ifaceName is NULL.\n");
		return (fncRet);
	}
	/* 空文字 */
	if (strnlen(ifaceName, IFNAMSIZ) <= 0)
	{
		DLOG_Debug("ifaceName is BLANK.\n");
		return (fncRet);
	}

	fp = CMN_FOPEN(ENET_PROC_NET_ROUTE, ENET_OPEN_TYPE_READ);
	if (fp == NULL)
	{
		fncRet = ENET_ERR1;
		DLOG_Error("Err! fopen : %s\n", ENET_PROC_NET_ROUTE);
		/* fopen失敗 */
		return (fncRet);
	}
	/* 行読み込み */
	while (FALSE == dapFlag && (fgets(buf, ENET_BUF_STR_LEN, fp)) != NULL)
	{
		memset(&rinf, 0, sizeof(rinf));
		scanElements = sscanf(buf, "%16s%lx%lx%lx%ld%ld%ld%lx%ld%ld%ld\n",
		rinf.iface, &rinf.dest, &rinf.gateway, &rinf.flgs, &rinf.ref,
		&rinf.use, &rinf.metric, &rinf.mask, &rinf.mtu, &rinf.win, &rinf.irtt);
		/* 変換個数不一致 */
		if (scanElements == ENET_ROUTE_INFO_ELEMENTS) 
		{
			if ((scanElements <= EOF) /* 読み込み失敗 */
			&& feof(fp)) /* ファイル終端 */
			{
				dapFlag = TRUE;
			}
			if(FALSE == dapFlag)
			{
				/* 対象のデバイスがupでない(=down) */
				if (rinf.flgs & RTF_UP)
				{
					if ((rinf.flgs == (RTF_UP | RTF_GATEWAY)) /* rinf.dest == 0*/
					&& strncmp(rinf.iface, ifaceName, IFNAMSIZ) == 0) /* デバイス名(eth0など)と一致 */
					{
						gateway->addr32bit = rinf.gateway;
						fncRet = ENET_SUCCESS;
						dapFlag = TRUE;
					}
				}
			}
		}
	}
	DLOG_Debug("gateway %d\n: ", (*gateway));
	CMN_FCLOSE(fp);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:指定デバイス(NIC)の指定したIPアドレス+サブネットマスク	*/
/*					を取得する												*/
/*--------------------------------------------------------------------------*/
/*	param			:schar* ifaceName	指定デバイス名（e.g. "eth0"）		*/
/*					void* settings	取得データを格納する					*/
/*					EnetIpType_t ipType	取得するIP種別を指定する			*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:指定デバイス(NIC)の指定したIPアドレス+サブネットマスク	*/
/*					を取得する												*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_GetIpAddr(const schar* ifaceName, void* settings, const EnetIpType_t ipType)
{

	slong fncRet = ENET_ERR1;
	EnetIpv4Settings_t* v4 = NULL;
	EnetIpv6Settings_t* v6 = NULL;
	struct ifaddrs* ifa_list = NULL;
	struct ifaddrs* ifa = NULL;
	DEBUG_TRCIN();

	if (settings == NULL)
	{
		DLOG_Error("Err! settings is NULL.\n");
		/* 引数エラー */
		return (fncRet);
	}
	if (ifaceName == NULL)
	{
		/* NULLチェック */
		DLOG_Error("Err! ifaceName is NULL.\n");
		/* 引数エラー */
		return (fncRet);
	}
	/* IPv4/v6別、返却用引数初期化 */
	switch (ipType)
	{
		case ENET_IPV4:
			v4 = (EnetIpv4Settings_t*) settings;
			v4->ip.addr32bit = 0;
			v4->subnet.addr32bit = 0;
			v4->autoFlg = ENET_GetPrefIPv4AutoFlg();
			break;
		case ENET_IPV6:
			v6 = (EnetIpv6Settings_t*) settings;
			v6->ip.addr32bit[0] = 0;
			v6->ip.addr32bit[1] = 0;
			v6->ip.addr32bit[2] = 0;
			v6->ip.addr32bit[3] = 0;

			v6->subnet.addr32bit[0] = 0;
			v6->subnet.addr32bit[1] = 0;
			v6->subnet.addr32bit[2] = 0;
			v6->subnet.addr32bit[3] = 0;
			break;
		default:
			DLOG_Error("Err! ipType : %d\n", ipType);
			/* 引数エラー：IPv4/IPv6以外が指定された */
			return (fncRet);
	}
	/* 空文字 */
	if (strnlen(ifaceName, IFNAMSIZ) <= 0)
	{
		DLOG_Debug("ifaceName is BLANK.\n");
		return (fncRet); /* 引数エラー */
	}

	if (getifaddrs(&ifa_list) != 0) {
		fncRet = ENET_ERR2;
		DLOG_Error("Err! getifaddrs Failed.\n");
		/* インターフェースが取得できない */
		return (fncRet);
	}

	for (ifa = ifa_list; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (strncmp(ifa->ifa_name, ifaceName, IFNAMSIZ) == 0)
		{
			switch (ifa->ifa_addr->sa_family)
			{
				/* IPv4 */
				case AF_INET: 
				{
					if (ipType == ENET_IPV4)
					{
						v4->ip.addr32bit = ((struct sockaddr_in *) ifa->ifa_addr)->sin_addr.s_addr;
						v4->subnet.addr32bit = ((struct sockaddr_in *) ifa->ifa_netmask)->sin_addr.s_addr;
						DLOG_Debug("IP   %d\n : ", v4->ip.addr32bit);
						DLOG_Debug("IPv4   %d\n : ", v4->ip);
						DLOG_Debug("subnet %d\n : ", v4->subnet);
						fncRet = ENET_SUCCESS;
					}
					break;
				}
				/* IPv6 */
				case AF_INET6: 
				{
					if (ipType == ENET_IPV6)
					{
						v6->ip.addr32bit[0] = ((struct sockaddr_in6*) ifa->ifa_addr)->sin6_addr.s6_addr32[0];
						v6->ip.addr32bit[1] = ((struct sockaddr_in6*) ifa->ifa_addr)->sin6_addr.s6_addr32[1];
						v6->ip.addr32bit[2] = ((struct sockaddr_in6*) ifa->ifa_addr)->sin6_addr.s6_addr32[2];
						v6->ip.addr32bit[3] = ((struct sockaddr_in6*) ifa->ifa_addr)->sin6_addr.s6_addr32[3];

						v6->subnet.addr32bit[0] = ((struct sockaddr_in6*) ifa->ifa_netmask)->sin6_addr.s6_addr32[0];
						v6->subnet.addr32bit[1] = ((struct sockaddr_in6*) ifa->ifa_netmask)->sin6_addr.s6_addr32[1];
						v6->subnet.addr32bit[2] = ((struct sockaddr_in6*) ifa->ifa_netmask)->sin6_addr.s6_addr32[2];
						v6->subnet.addr32bit[3] = ((struct sockaddr_in6*) ifa->ifa_netmask)->sin6_addr.s6_addr32[3];
						fncRet = ENET_SUCCESS;
					}
					break;
				}
				/* IPv4/IPv6以外 */
				default:
					break;
			}
		}
	}
	freeifaddrs(ifa_list);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:現在の設定に従い、ネットワークを更新する				*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:現在の設定に従い、ネットワークを更新する				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_UpdateNetworkType(void)
{
	bool_t dapFlag = FALSE;
	slong fncRet = ENET_ERR1;
	schar  bufIfNameStr[IFNAMSIZ];									/* ネットワークIF名取得バッファ */
	EnetNetworkType_t nowType = ENET_GetPrefNetworkType();			/* 設定ファイルのネットワーク */
	schar* nowIfaceName = ENET_GetNetworkTypeStrTbl(nowType);		/* 現在のネットワークIF名 */
	DEBUG_TRCIN();

	slong bufIfNameCnt = ENET_NETWORK_WIRED;						/* IF名取得カウント(ネットワークDOWN用) */
	slong bufIfNameRet = ENET_SUCCESS;								/* IF名取得結果(ネットワークDOWN用) */

	memset(bufIfNameStr, 0x00, IFNAMSIZ);
	/* 設定更新のためkill */

/*	FRM_TimerSetOneShot(ENET_ONE_SHOT_ID_IP_WATCH, ENET_GetPrefIpWatchInterval(), NULL);*/
/*	FRM_TimerSetOneShot(ENET_ONE_SHOT_ID_SEND_PING, ENET_GetPrefSendPingInterval() + ENET_THREAD_START_DELAY, NULL);*/

	/* 有効にしたいネットワーク以外をDOWN */
	do{
		/* インターフェース名の取得 */
		memset(bufIfNameStr, 0x00, IFNAMSIZ);
		bufIfNameRet = ENET_GetInterfaceNameIndex(bufIfNameCnt, bufIfNameStr);
		DLOG_Debug("ENET_GetInterfaceNameIndex([in]%d,[out]%s):%d\n", bufIfNameCnt, bufIfNameStr, bufIfNameRet);

		/* インターフェース名が取得できなくなった場合 */
		if(bufIfNameRet != ENET_SUCCESS)
		{
			DLOG_Debug("break link down loop (%d).\n", bufIfNameCnt);
			dapFlag = TRUE;
		}
		if(FALSE == dapFlag)
		{
			/* インターフェース名称の取得に成功 */
			if((strnlen(bufIfNameStr, IFNAMSIZ) > 0))
			{
				/* デバイスの状態で判定 */
				if (enet_GetDeviceState(bufIfNameStr) == ENET_DEVICE_UP)
				{
					/* デバイスが動作中 */
					if (strncmp(bufIfNameStr, nowIfaceName, IFNAMSIZ) == 0)
					{
						/* 現在のネットワーク */
						DLOG_Debug("linked up already : %s\n", bufIfNameStr);	/* 既にデバイスが動作中 */
					}
					else
					{
						DLOG_Debug("link down : %s\n", bufIfNameStr);
						/* 最終的にUPにしたいデバイスでなければDOWNする */
						enet_SetLinkMode(bufIfNameStr, ENET_LINK_DOWN);
					}
				}
			}
			/* 取得位置のカウントアップ */
			bufIfNameCnt++;
		}
		DLOG_Debug("dapFlag : %d\n", dapFlag);
	}while(FALSE == dapFlag && bufIfNameRet == ENET_SUCCESS);
	/* ネットワーク有効とそれ以外 */
	switch (nowType)
	{
		/* $FALL-THROUGH$ */
		case ENET_NETWORK_WIRED:
		case ENET_NETWORK_WIRELESS:
		{
			/* ネットワークが有効 */
			DLOG_Debug("network enable : %s\n", nowIfaceName);
			fncRet = enet_SetLinkMode(nowIfaceName, ENET_LINK_UP);
			/* 設定中のネットワークをUP */
			DLOG_Debug("enet_SetLinkMode : [ %s ]\n", fncRet == ENET_SUCCESS?"OK":ENET_YELLOW"NG"ENET_RESET);
			/* LinkUp成功 */
			if (fncRet == ENET_SUCCESS)
			{
				/* 設定の復元 */
				ENET_InitIpAddr(nowType, ENET_IPV4);
			}
			break;
		}
		/* $FALL-THROUGH$ */
		case ENET_NETWORK_NONE:
		default:
		{
			/* ネットワークが無効 */
			DLOG_Debug("all network disable.\n");
			ENET_ExecCmdKillDhcp();
			/* ネットワークなしの場合、ルータPING疎通成功(エラー解除)を即時通知 */
			ENET_SetRmcErrCode(ENET_ERRCODE_UNSET_ROUTER_COMM);
			fncRet = ENET_SUCCESS;
			break;
		}
	}
	DLOG_Debug("fncRet : %s\n", fncRet==ENET_SUCCESS?"OK":ENET_YELLOW"NG"ENET_RESET);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:IPアドレスを初期化する									*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetNetworkType_t nowType	初期化対象のネットワーク種別*/
/*					EnetIpType_t ipType	初期化対象のIP種別					*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:IPアドレスを初期化する									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_InitIpAddr(const EnetNetworkType_t nowType, const EnetIpType_t ipType)
{
	slong fncRet = ENET_ERR1;
	EnetIpv4Settings_t v4bk = { };
#if 0
	EnetIpv6Settings_t v6bk = { }; /*(TBD) v6実装*/
#endif
	DEBUG_TRCIN();

	schar* ifaceName = ENET_GetNetworkTypeStrTbl(nowType);
	/* NULLチェック */
	if (ifaceName == NULL)
	{
		DLOG_Debug("network is down.\n");
		fncRet = ENET_SUCCESS;
		return (fncRet);
	}
	/* 空文字 */
	if (strnlen(ifaceName, IFNAMSIZ) <= 0)
	{
		DLOG_Debug("network is down.\n");
		fncRet = ENET_SUCCESS;
		return (fncRet);
	}
	/* 無線とそれ以外(前処理) */
	switch (ipType) 
	{
		case ENET_IPV4:
			/* 復元用設定値取得 */
			v4bk.autoFlg = ENET_GetPrefIPv4AutoFlg();
			/* 自動設定 */
			if (v4bk.autoFlg)
			{
				fncRet = ENET_ExecDhcpThread();
			}
			/* 手動設定 */
			else
			{
				ENET_ExecCmdKillDhcp();
				/* 復元用設定値取得 */
				ENET_GetPrefIPv4fmtIP(ENET_KEY_IPV4_ADDR_SELF, &v4bk.ip);
				ENET_GetPrefIPv4fmtIP(ENET_KEY_SUBNET, &v4bk.subnet);
				ENET_GetPrefIPv4fmtIP(ENET_KEY_GATEWAY, &v4bk.gateway);
				ENET_GetPrefIPv4fmtIP(ENET_KEY_DNS1, &v4bk.dns1);
				ENET_GetPrefIPv4fmtIP(ENET_KEY_DNS2, &v4bk.dns2);
				/* 手動設定実施、成功判定 */
				if (enet_SetIPv4Self(ifaceName, &v4bk) == ENET_SUCCESS)
				{
					fncRet = ENET_SUCCESS;
				}
				else 
				{
					/* 設定失敗 */
					DLOG_Error("Err! setting Fail.\n");
					v4bk.ip.addr32bit = 0;
					v4bk.subnet.addr32bit = 0;
					v4bk.gateway.addr32bit = 0;
					v4bk.dns1.addr32bit = 0;
					v4bk.dns2.addr32bit = 0;
					enet_SetIPv4Self(ifaceName, &v4bk);
				}
			}
			break;
		/* (TBD) v6実装 */
		case ENET_IPV6:
			break;
		default:
			DLOG_Error("Err! ipType : %d\n", ipType);
			/* 引数エラー：IPv4/IPv6以外が指定された */
			return (fncRet);
	}
	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:ESSIDスキャン完了時に実行し、呼び出し元に結果を送信する*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong cPort	通知先Cポート							*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:ESSIDスキャン完了時に実行し、呼び出し元に結果を送信する*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_ScanEssidLfwASyncRet(ulong cPort)
{
	slong fncRet = 0;
	slong retSize = sizeof(EnetFnc00_t);
	EnetFnc00_t fnc = { };
	/* 送信先のSポートに変換 */
	ulong sPort = CONVERT_CPORT2SPORT(cPort);
	DEBUG_TRCIN();

	/* 非同期スキャンの関数ID */
 	fnc.header.msgId = ENET_ASYNC_SCAN_ESSID;
	fnc.header.retCode = ENET_GetFileSize(ENET_FILE_AP_CSV);
	fncRet = FRM_MsgSendEvent(sPort, retSize, (void*) &fnc);

	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:ルータ疎通確認完了時に実行し、							*/
/*					呼び出し元に結果を送信する								*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong cPort	通知先Cポート							*/
/*					slong result	0/-1：疎通成功/疎通失敗					*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:ルータ疎通確認完了時に実行し、							*/
/*					呼び出し元に結果を送信する								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_ChkRouterConnectionLfwASyncRet(ulong cPort, slong result)
{

	slong fncRet = 0;
	EnetFnc00_t fnc = { };
	/* 送信先のSポートに変換 */
	ulong sPort = CONVERT_CPORT2SPORT(cPort);
	DEBUG_TRCIN();

	/* ルータ疎通の関数ID */
	fnc.header.msgId = ENET_ASYNC_CHK_ROUTERCONNECTION;
	fnc.header.retCode = result;

	if (cPort != 0)
	{
		DLOG_Debug("send port : %lu\n", sPort);
		fncRet = FRM_MsgSendEvent(sPort, sizeof(EnetFnc00_t), (void*) &fnc);
	}

	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:setChgEvntListener										*/
/*					で登録したプロセスに状態変化通知を送信する				*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong port	送信先ポート								*/
/*					ulong flag	送信対象イベントフラグ						*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:setChgEvntListener										*/
/*					で登録したプロセスに状態変化通知を送信する				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong enet_SendChgEvntLfwEvent(ulong port, ulong flag)
{
	slong fncRet = 0;
	EnetFncListenerPort_t fnc = { };
	DEBUG_TRCIN();

	DLOG_Debug("send port,flag : %lu,0x%02lX\n", port, flag);
	if (flag == 0)
	{
		DLOG_Debug("No matching event.\n");
		return (fncRet);
	}

	fnc.header.msgId = ENET_SYNC_SET_CHGEVNTLISTENER;

	fnc.port = port;
	if (flag & ENET_CHGEVNT_IP_ADDRESS)
	{
		/* IP変化通知 */
		fnc.flag = ENET_CHGEVNT_IP_ADDRESS;
		/* 相手のポートに送信 */
		fncRet = FRM_MsgSendEvent(port, sizeof(EnetFncListenerPort_t), (void*) &fnc);
		DLOG_Debug("IPAddress    lfwRet : %d\n", fncRet);
	}
	if (flag & ENET_CHGEVNT_IP_HANDLING)
	{
		/* IP自動/手動変化通知 */
		fnc.flag = ENET_CHGEVNT_IP_HANDLING;
		/* 相手のポートに送信 */
		fncRet = FRM_MsgSendEvent(port, sizeof(EnetFncListenerPort_t), (void*) &fnc);
		DLOG_Debug("IPHandling   lfwRet : %d\n", fncRet);
	}
	if (flag & ENET_CHGEVNT_NETWORK_TYPE)
	{
		/* ネットワーク種別変化通知 */
		fnc.flag = ENET_CHGEVNT_NETWORK_TYPE;
		/* 相手のポートに送信 */
		fncRet = FRM_MsgSendEvent(port, sizeof(EnetFncListenerPort_t), (void*) &fnc);
		DLOG_Debug("NetworkType  lfwRet : %d\n", fncRet);
	}
	if (flag & ENET_CHGEVNT_PROXY_SETTING)
	{
		/* Proxy設定変化通知 */
		fnc.flag = ENET_CHGEVNT_PROXY_SETTING;
		/* 相手のポートに送信 */
		fncRet = FRM_MsgSendEvent(port, sizeof(EnetFncListenerPort_t), (void*) &fnc);
		DLOG_Debug("ProxySetting lfwRet : %d\n", fncRet);
	}

	DEBUG_TRCOUT();
	return (fncRet);
}

//*==========================================================================*/
/*	Description		:無線LANのステータスを取得する							*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無線LANのステータスを取得する							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_GetWlanStatus(void)
{
	slong fncRet = ENET_ERR1; /* 電波強度、デバイスが存在しない場合は負数 */
	struct iwreq iwr = { };
	struct iw_statistics stat = { };
	sint skfd = 0;
	schar* ifaceName = ENET_GetNetworkTypeStrTbl(ENET_NETWORK_WIRELESS);
	DEBUG_TRCIN();

	switch (enet_GetDeviceState(ifaceName))
	{
		/* デバイスが存在しない */
		case ENET_DEVICE_NONE:
			break;
		/* LinkUp */
		case ENET_DEVICE_UP:
		 	/* 電波強度取得に進む */
			fncRet = 0;
			break;
		 /* LinkDown */
		case ENET_DEVICE_DOWN:
			/* デバイスは存在するので電波強度0 */
			fncRet = 0;
			break;
		default:
			break;
	}
	if(0 == fncRet)
	{
	#ifndef CROSSDEBUG
		skfd = socket(AF_INET, SOCK_DGRAM, 0);
	#endif

		if (skfd == 0)
		{
			DLOG_Error("Err! socket is NULL.\n");
			fncRet = ENET_ERR1;
		}
		else
		{
			memset(&iwr, 0, sizeof(iwr));
			memset(&stat, 0, sizeof(stat));
			strncpy(iwr.ifr_name, ifaceName, IFNAMSIZ);
			iwr.u.data.pointer = &stat;
			iwr.u.data.length = sizeof(stat);

			if (ioctl(skfd, SIOCGIWSTATS, &iwr) < 0)
			{

				DLOG_Error("Err! ioctl(SIOCGIWSTATS).\n");
				close(skfd);
				fncRet = ENET_ERR1;
			}
			else
			{
				DLOG_Debug("q'ty(lvl:SNR) : %3d(%4d:%2d)\n", stat.qual.qual,
				 stat.qual.level - 0x100, stat.qual.level - stat.qual.noise);
				/* dBmが取得できないときは未接続 */
				if (stat.qual.level == 0)
				{
					/* too small. */
					fncRet = 0;
				}
				else if (stat.qual.qual <= ENET_WLAN_STAT_MAX)
				{
					/* expected range. */
					fncRet = stat.qual.qual;
				}
				else
				{
					/* too large. */
					fncRet = ENET_WLAN_STAT_MAX;
				}
				close(skfd);
			}
		}
	}
	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:登録済みプロセスにイベント通知する						*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong flag	通知するイベントのフラグ					*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:登録済みプロセスにイベント通知する						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void ENET_SendChgEvnt(ulong flag)
{
	int i = 0;
	DEBUG_TRCIN();

	DLOG_Debug("send flag : 0x%02lX\n", flag);
	for (i = 0; i < PROCESS_MAX; i++)
	{
		if (s_enet_sendChgEvntProc[i].port != 0)
		{
			DLOG_Debug("send port : %lu\n", s_enet_sendChgEvntProc[i].port);
			enet_SendChgEvntLfwEvent(s_enet_sendChgEvntProc[i].port, (s_enet_sendChgEvntProc[i].flag & flag));
		}
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:プロセスを初期化する									*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:無い													*/
/*--------------------------------------------------------------------------*/
/*	detail			:プロセスを初期化する									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void ENET_ProcStart(slong fact)
{
	DEBUG_TRCIN();

	DLOG_Debug("enet init start...\n");
	/* プロセス再起動時 */
	if (ESYS_START_FACT_RESET == fact)
	{
		DLOG_Debug("enet reboot.\n");
		enet_InitSendChgEvntProcArr();
	}
	ENET_InitEnetThread(0, NULL);
	ENET_InitEnetFile(0, NULL);
	/* 設定ファイルに従いネットワークの起動 */
/*	ENET_UpdateNetworkType();*/
	ENET_ResetApDevice();
	ENET_CreateInotifyThread();
	ESYS_NotifyStartResult(ENET_SPORT, MOD_ACTION_RESULT_SUCCESS);
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:設定ファイルからPROXY設定を取得する					*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetProxy_t* proxy	取得データを格納する				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルからPROXY設定を取得する					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_GetProxy(EnetProxy_t* proxy)
{
	slong fncRet = ENET_SUCCESS;
	slong bufNum = 0;
	schar val[ENET_BUF_STR_LEN] = {0};
	DEBUG_TRCIN();

	memset(proxy->addr, 0, sizeof(proxy->addr));
	proxy->port = 0;
	proxy->enableFlg = 0;

	if (GET_PREF(ENET_KEY_PROXY_HOST, val, ENET_FILE_PROXY))
	{
		if(ENET_PROXY_ADDR_STR_LEN > strnlen(val, sizeof(val)))
		{
			memcpy(proxy->addr, val, sizeof(proxy->addr));
		}
		else
		{
			fncRet = ENET_ERR1;
		}		
	}
	else
	{
		fncRet = ENET_ERR1;
	}
	memset(val, 0, sizeof(val));

	if (GET_PREF(ENET_KEY_PROXY_PORT, val, ENET_FILE_PROXY))
	{
		bufNum = atoi(val);
		if ((bufNum < 0) /* ポート番号がゼロ未満 */
		|| (ENET_PROXY_PORT_MAX < bufNum)) /* ポート番号が最大値より大きい */
		{
			fncRet = ENET_ERR1;
			DLOG_Error("Err! too large(small) proxy port : %s\n", val);
		}
		else
		{
			proxy->port = bufNum;
		}
	}
	else
	{
		fncRet = ENET_ERR1;
	}
	memset(val, 0, sizeof(val));

	if (GET_PREF(ENET_KEY_PROXY_ENABLE, val, ENET_FILE_PROXY))
	{
		bufNum = atoi(val);
		switch (bufNum) 
		{
		/* $FALL-THROUGH$ */
		case ENET_TRUE:
		case ENET_FALSE:
			proxy->enableFlg = bufNum;
			break;
		default:
			proxy->enableFlg = ENET_FALSE;
			fncRet = ENET_ERR1;
			DLOG_Error("Err! Invalid proxy enableFlg : %s\n", val);
			break;
		}
	}
	else
	{
		fncRet = ENET_ERR1;
	}
	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:イベント通知登録を行い、呼び出し元に結果を送信する		*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetFncListenerPort_t* fncPort	プロセス間通信メッセージ*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:イベント通知登録を行い、呼び出し元に結果を送信する		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SetChgEvntListenerLfwRet(EnetFncListenerPort_t* fncPort)
{
	slong fncRet = 0;
	DEBUG_TRCIN();

	fncPort->header.retCode = enet_SetChgEvntListener(fncPort->port, fncPort->flag);
	fncRet = FRM_CallRet(fncPort->header.srcModId, sizeof(EnetFncListenerPort_t), (void*) fncPort);

	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:無線LANのインターフェース名を取得する					*/
/*--------------------------------------------------------------------------*/
/*	param			:schar* ifName											*/
/*					無線LANインターフェース名を格納するバッファのアドレス	*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無線LANのインターフェース名を取得する					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_GetWlanIfName(schar* ifName)
{
	slong	fncRet = ENET_ERR1;
	schar* ifaceName = ENET_GetNetworkTypeStrTbl(ENET_NETWORK_WIRELESS);
	DEBUG_TRCIN();

	if(NULL != ifName){
		memset(ifName, 0x00, ENET_WLAN_IF_NAME_STR_LEN);
		/* 取得した無線LANインタフェース名が空文字でなければコピー */
		if(0 != strnlen(ifaceName, ENET_WLAN_IF_NAME_STR_LEN))
		{
			strncpy(ifName, ifaceName, (ENET_WLAN_IF_NAME_STR_LEN-1));
			fncRet = ENET_SUCCESS;
		}
	}

	DEBUG_TRCOUT();
	return fncRet;
}
/*==========================================================================*/
/*	Description		:イベント通知登録解除する								*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong port	登録解除するポート							*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:イベント通知登録解除する								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_RmvChgEvntListener(ulong port)
{
	slong fncRet = ENET_SUCCESS;
	slong i = 0;
	DEBUG_TRCIN();

	for (i = 0; i < PROCESS_MAX; i++)
	{
		if (s_enet_sendChgEvntProc[i].port == port)
		{
			if (enet_RmvPrefChgEvntFlag(port))
			{
				DLOG_Debug("rmv port : %d\n", port);
				/* ポートクリア */
				s_enet_sendChgEvntProc[i].port = 0;
				/* フラグクリア */
				s_enet_sendChgEvntProc[i].flag = 0;
			}
			else
			{
				fncRet = ENET_ERR1;
			}
		}
	}

	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:ネットワーク種別設定を行い、呼び出し元に結果を送信する	*/
/*--------------------------------------------------------------------------*/
/*	param			:ENET_FNC_NETWORK_TYPE_T* fncNType						*/
/*					プロセス間通信メッセージ								*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:ネットワーク種別設定を行い、呼び出し元に結果を送信する	*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SetNetworkTypeLfwRet(EnetFncNetworkType_t* fncNType)
{
	DEBUG_TRCIN();
	slong fncRet = 0;

	fncNType->header.retCode = enet_SetNetworkType(fncNType->type, fncNType->ext);
	fncRet = FRM_CallRet(fncNType->header.srcModId, sizeof(EnetFncNetworkType_t), (void*) fncNType);

	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:IPv4アドレスを取得し、呼び出し元に送信する				*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetFncIpv4_t* fncIPv4	プロセス間通信メッセージ		*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:IPv4アドレスを取得し、呼び出し元に送信する				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_GetIpAddr4LfwRet(EnetIpv4Settings_t* fncIPv4)
{

	slong fncRet = ENET_SUCCESS;
	schar* ifaceName = ENET_GetPrefNetworkTypeStr();
	DEBUG_TRCIN();
	
	fncRet = ENET_GetIpAddr(ifaceName, fncIPv4, ENET_IPV4);
	switch (fncRet)
	{
		case ENET_SUCCESS:
			DLOG_Debug("ENET_GetIpAddr success!\n");
			break;
		default:
			/* contain ENET_ERR1/ENET_ERR2 */
			DLOG_Error("ENET_GetIpAddr : %d\n", fncRet);
			/* ネットワークなしのときに通過 */
			break;
	}
	fncRet = ENET_GetDefaultGW(ifaceName, &fncIPv4->gateway);
	switch (fncRet)
	{
		case ENET_SUCCESS:
			DLOG_Debug("ENET_GetDefaultGW success!\n");
			break;
		default:
			/* contain ENET_ERR1/ENET_ERR2 */
			DLOG_Error("ENET_GetDefaultGW : %d\n", fncRet);
			break;
	}
	/* DNS取得 */
	fncRet = enet_GetDNS(fncIPv4);
	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:IPv4アドレス設定を行い、呼び出し元に結果を送信する		*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetFncIpv4_t* fncIPv4	プロセス間通信メッセージ		*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:IPv4アドレス設定を行い、呼び出し元に結果を送信する		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SetIpAddr4LfwRet(EnetFncIpv4_t* fncIPv4)
{
	slong fncRet = 0;
	schar noneTypeStr[IFNAMSIZ] = { 0 }; /* 空文字 */
	DEBUG_TRCIN();

	if (ENET_IPSET_EXT_UPD_SETFILE == fncIPv4->ext)
	{
		/* 設定ファイル更新指定の場合 */
		fncIPv4->header.retCode = enet_SetIPAddr(noneTypeStr, &fncIPv4->v4, ENET_IPV4); /* Typeに空文字を指定してファイル更新のみ実行 */
	}
	else
	{
		fncIPv4->header.retCode = enet_SetIPAddr(ENET_GetPrefNetworkTypeStr(), &fncIPv4->v4, ENET_IPV4);
	}
	fncRet = FRM_CallRet(fncIPv4->header.srcModId, sizeof(EnetFncIpv4_t), (void*) fncIPv4);
	
	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:IPv4アドレスの手動設定値を取得し、呼び出し元に送信する	*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetFncIpv4_t* fncIPv4	プロセス間通信メッセージ		*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常											*/
/*--------------------------------------------------------------------------*/
/*	detail			:IPv4アドレスの手動設定値を取得し、呼び出し元に送信する	*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_GetIPAddr4SettingLfwRet(EnetIpv4Settings_t* fncIPv4)
{
	DEBUG_TRCIN();
	slong fncRet = ENET_SUCCESS;

	if (!ENET_GetPrefIPv4fmtIP(ENET_KEY_IPV4_ADDR_SELF, &fncIPv4->ip))
	{
		DLOG_Debug("IN ipaddr");
		fncIPv4->ip.addr32bit = ENET_DEFAULT_IP;
	}
	if (!ENET_GetPrefIPv4fmtIP(ENET_KEY_SUBNET, &fncIPv4->subnet))
	{
		fncIPv4->subnet.addr32bit = ENET_DEFAULT_SUBNET;
	}
	if (!ENET_GetPrefIPv4fmtIP(ENET_KEY_GATEWAY, &fncIPv4->gateway))
	{
		fncIPv4->gateway.addr32bit = ENET_DEFAULT_GATEWAY;
	}
	if (!ENET_GetPrefIPv4fmtIP(ENET_KEY_DNS1, &fncIPv4->dns1))
	{
		fncIPv4->dns1.addr32bit = ENET_DEFAULT_DNS1;
	}
	if (!ENET_GetPrefIPv4fmtIP(ENET_KEY_DNS2, &fncIPv4->dns2))
	{
		fncIPv4->dns2.addr32bit = ENET_DEFAULT_DNS2;
	}
	fncIPv4->autoFlg = 0;

	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:IPv6アドレスを取得し、呼び出し元に送信する				*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetFncIpv6_t* fncIPv6	プロセス間通信メッセージ		*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:IPv6アドレスを取得し、呼び出し元に送信する				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_GetIpAddr6LfwRet(EnetIpv6Settings_t* fncIPv6)
{
	DEBUG_TRCIN();
	slong fncRet = ENET_SUCCESS;

	fncRet = ENET_GetIpAddr(ENET_GetPrefNetworkTypeStr(), fncIPv6, ENET_IPV6);
	switch (fncRet)
	{
		case ENET_SUCCESS:
			DLOG_Debug("ENET_GetIpAddr success!\n");
			break;
		default:
			/* contain ENET_ERR1/ENET_ERR2 */
			DLOG_Error("Err! ENET_GetIpAddr : %d\n", fncRet);
			break;
	}

	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:IPv6アドレス設定を行い、呼び出し元に結果を送信する		*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetFncIpv6_t* fncIPv6	プロセス間通信メッセージ		*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:IPv6アドレス設定を行い、呼び出し元に結果を送信する		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SetIpAddr6LfwRet(EnetFncIpv6_t* fncIPv6)
{
	slong fncRet = 0;
	DEBUG_TRCIN();

	fncIPv6->header.retCode = enet_SetIPAddr(ENET_GetPrefNetworkTypeStr(), &fncIPv6->v6, ENET_IPV6);
	fncRet = FRM_CallRet(fncIPv6->header.srcModId, sizeof(EnetFncIpv6_t), (void*) fncIPv6);
	
	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:Proxy設定を行い、呼び出し元に結果を送信する			*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetFncProxy_t* fncProxy	プロセス間通信メッセージ	*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:Proxy設定を行い、呼び出し元に結果を送信する			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SetProxyLfwRet(EnetFncProxy_t* fncProxy)
{
	DEBUG_TRCIN();
	slong fncRet = 0;

	fncProxy->header.retCode = enet_SetProxy(&fncProxy->proxy);
	fncRet = FRM_CallRet(fncProxy->header.srcModId, sizeof(EnetFncProxy_t), (void*) fncProxy);

	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:ESSIDのスキャンを別スレッドで行う						*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetFnc00_t* fnc	プロセス間通信メッセージ			*/
/*--------------------------------------------------------------------------*/
/*	return			:無い													*/
/*--------------------------------------------------------------------------*/
/*	detail			:ESSIDのスキャンを別スレッドで行う						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void ENET_ScanEssidLfwASync(EnetFnc00_t* fnc)
{
	DEBUG_TRCIN();
	ENET_ExecScanEssidThread(fnc->header.srcModId);
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:無線LAN設定を行い、呼び出し元に結果を送信する			*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetFncWlan_t* fncWlan	プロセス間通信メッセージ		*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無線LAN設定を行い、呼び出し元に結果を送信する			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SetWlanSettingLfwRet(EnetFncWlan_t* fncWlan)
{
	slong fncRet = 0;
	DEBUG_TRCIN();

	DLOG_Debug("wlan : %s,%s\n", fncWlan->wlan.essid, fncWlan->wlan.password);
	fncWlan->header.retCode = enet_SetWlanSetting(&fncWlan->wlan, fncWlan->ext);
	fncRet = FRM_CallRet(fncWlan->header.srcModId, sizeof(EnetFncWlan_t), (void*) fncWlan);
	
	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:無線LANのon/offを行い、呼び出し元に結果を送信する		*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetFncBool_t* fncBool	プロセス間通信メッセージ		*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無線LANのon/offを行い、呼び出し元に結果を送信する		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SetWlanUpLfwRet(EnetFncBool_t* fncBool)
{
	slong fncRet = 0;
	DEBUG_TRCIN();

	fncBool->header.retCode = enet_SetWlanUp(fncBool->boolean);
	fncRet = FRM_CallRet(fncBool->header.srcModId, sizeof(EnetFncBool_t), (void*) fncBool);
	
	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:ルータ疎通確認を実施する								*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetFnc00_t* fnc	プロセス間通信メッセージ			*/
/*--------------------------------------------------------------------------*/
/*	return			:無い													*/
/*--------------------------------------------------------------------------*/
/*	detail			:ルータ疎通確認を実施する								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void ENET_ChkRouterConnectionLfwASync(EnetFnc00_t* fnc)
{
	DEBUG_TRCIN();
	enet_ChkRouterConnection(fnc->header.srcModId);
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:hostapd用設定ファイルを生成する					*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetWlanSettings_t* wlan	無線LAN設定					*/
/*--------------------------------------------------------------------------*/
/*	return			:boo_t	処理結果										*/
/*						TRUE	正常	FALSE	異常						*/
/*--------------------------------------------------------------------------*/
/*	detail			:hostapd用設定ファイルを生成する					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
bool_t ENET_InitPrefHostapd(EnetWlanSettings_t* wlan)
{
	bool_t fncRet = TRUE;
	DEBUG_TRCIN();

	/* 空文字 */
	if (strnlen(wlan->essid, sizeof(wlan->essid)) <= 0)
	{
		/* 接続先不明 */
		DLOG_Error("Err! essid is BLANK.\n");
		/* エラー */
		return (fncRet);
	}

	/* 毎回削除して新規作成 */
	remove(ENET_FILE_WPACONF);
	
	fncRet &= SET_PREF("#HOSTAPD_CONFIGURE", "#", ENET_FILE_WPACONF);
	fncRet &= SET_PREF("ctrl_interface", ENET_HOSTAPD_CTRL_IF, ENET_FILE_WPACONF);
	fncRet &= SET_PREF("interface", "wlan0", ENET_FILE_WPACONF);
	fncRet &= SET_PREF("driver", "nl80211", ENET_FILE_WPACONF);
	fncRet &= SET_PREF("ssid", wlan->essid, ENET_FILE_WPACONF);
	fncRet &= SET_PREF("wpa_passphrase", wlan->password, ENET_FILE_WPACONF);
	fncRet &= SET_PREF("channel", "6", ENET_FILE_WPACONF);
	fncRet &= SET_PREF("wpa", "2", ENET_FILE_WPACONF);
	fncRet &= SET_PREF("hw_mode", "g", ENET_FILE_WPACONF);
	fncRet &= SET_PREF("wpa_key_mgmt", ENET_VAL_KEYMGMT, ENET_FILE_WPACONF);
	fncRet &= SET_PREF("wpa_pairwise", "CCMP", ENET_FILE_WPACONF);
	fncRet &= SET_PREF("max_num_sta", "1", ENET_FILE_WPACONF);
	
	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:設定ファイルにネットワーク種別を記録する				*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetNetworkType_t type	ネットワーク種別				*/
/*--------------------------------------------------------------------------*/
/*	return			:boo_t	処理結果										*/
/*						TRUE	正常	FALSE	異常						*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルにネットワーク種別を記録する				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
bool_t ENET_SetPrefNetworkType(EnetNetworkType_t type)
{
	bool_t fncRet = FALSE;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	if ((ENET_NETWORK_NONE > type) /* 閾値未満 */
	|| (type >= ENET_NETWORK_TYPE_MAX)) /* 最大値以上 */
	{
		DLOG_Error("Err! Invalid type : %d\n", type);
	}
	else
	{
		snprintf(buf, ENET_BUF_STR_LEN, "%d", type);
		if (SET_PREF(ENET_KEY_NETWORK_TYPE, buf, ENET_FILE_SETTING))
		{
			fncRet = TRUE;
		}
	}
	DLOG_Debug("set : %d\n", type);

	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:設定ファイルからネットワーク種別を取得する				*/
/*--------------------------------------------------------------------------*/
/*	param			:ENET_WLAN_SETTINGS_T* wlan	無線LAN設定					*/
/*--------------------------------------------------------------------------*/
/*	return			:EnetNetworkType_t	処理結果							*/
/*					ENET_NETWORK_NONE	ネットワークなし					*/
/*					ENET_NETWORK_WIRED	ネットワーク有線					*/
/*					ENET_NETWORK_WIRELESS	ネットワーク無線				*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルからネットワーク種別を取得する				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
EnetNetworkType_t ENET_GetPrefNetworkType(void)
{
	EnetNetworkType_t fncRet = ENET_NETWORK_NONE;
	schar val[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	GET_PREF(ENET_KEY_NETWORK_TYPE, val, ENET_FILE_SETTING);
	fncRet = atoi(val); /* 値なしの場合は0(ENET_NETWORK_NONE) */
	if ((ENET_NETWORK_NONE > fncRet) /* 閾値未満 */
	|| (fncRet >= ENET_NETWORK_TYPE_MAX)) /* 最大値以上 */
	{
		DLOG_Error("Err! Invalid type : %s\n", val);
		fncRet = ENET_NETWORK_NONE; /* 異常値はネットワークなし */
	}
	DLOG_Debug("get : %d\n", fncRet);

	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:														*/
/*				設定ファイルのネットワーク種別からデバイス名(NIC)を取得する	*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:schar*	処理結果										*/
/*						デバイス名（e.g. "eth0"）							*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*				設定ファイルのネットワーク種別からデバイス名(NIC)を取得する	*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
schar* ENET_GetPrefNetworkTypeStr(void)
{
	/* 設定ファイルに記録されたネットワーク種別 */
	EnetNetworkType_t type = ENET_NETWORK_NONE;
	schar* ifaceName;
	DEBUG_TRCIN();

	type		= ENET_GetPrefNetworkType();
	ifaceName	= enet_GetDeviceName(type);
	DLOG_Debug("ifaceName : %s\n", ifaceName);

	DEBUG_TRCOUT();
	return (ifaceName);
}
/*==========================================================================*/
/*	Description		:ネットワーク種別をデバイス名に変換する					*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetNetworkType_t type	ネットワーク種別				*/
/*--------------------------------------------------------------------------*/
/*	return			:schar*	処理結果										*/
/*						デバイス名（e.g. "eth0"）							*/
/*--------------------------------------------------------------------------*/
/*	detail			:ネットワーク種別をデバイス名に変換する					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
schar* ENET_GetNetworkTypeStrTbl(EnetNetworkType_t type)
{
	EnetNetworkType_t bufType = ENET_NETWORK_NONE;
	schar* ifaceName;
	DEBUG_TRCIN();

	/* ネットワークタイプ設定 */
	bufType = type;
	/* ネットワークタイプ範囲判定 */
	if((type < ENET_NETWORK_NONE)
	|| (ENET_NETWORK_TYPE_MAX <= type) )
	{
		/* 範囲外だった場合、未設定にする */
		bufType = ENET_NETWORK_NONE;
	}
	ifaceName	= enet_GetDeviceName(bufType);
	DLOG_Debug("type, ifaceName : %d,%s\n", type, ifaceName);

	DEBUG_TRCOUT();
	return (ifaceName);
}
/*==========================================================================*/
/*	Description		:indexで指定されたインターフェース名称を取得する		*/
/*--------------------------------------------------------------------------*/
/*	param			:slong	index			インデックス値					*/
/*					schar*	interfaceName	インターフェース名				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*					ENET_ERR1：範囲外のindexを指定							*/
/*					ENET_ERR2：格納先がNULL									*/
/*					ENET_SUCCESS：上記以外									*/
/*--------------------------------------------------------------------------*/
/*	detail			:indexで指定されたインターフェース名称を取得する		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_GetInterfaceNameIndex(slong index, schar* interfaceName)
{
	DEBUG_TRCIN();
	/* indexの値が範囲外の場合 */
	if((index < 0)
	|| (index >= sizeof(s_enet_interfaceName)/IFNAMSIZ))
	{
		/* ENET_ERR1を返却 */
		return ENET_ERR1;
	}

	/* 格納先の領域がNULLの場合 */
	if(NULL == interfaceName)
	{
		/* ENET_ERR2を返却 */
		return ENET_ERR2;
	}

	/* 格納先にインターフェース名称をコピー */
	memset(interfaceName, 0x00, IFNAMSIZ);
	memcpy(interfaceName, s_enet_interfaceName[index], IFNAMSIZ);
	DEBUG_TRCOUT();
	/* OK判定を返却 */
	return ENET_SUCCESS;
}

/*==========================================================================*/
/*	Description		:設定ファイルにIPv4の自動設定フラグを記録する			*/
/*--------------------------------------------------------------------------*/
/*	param			:schar autoFlg	自動フラグ								*/
/*--------------------------------------------------------------------------*/
/*	return			:boo_t	処理結果										*/
/*						TRUE	正常	FALSE	異常						*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルにIPv4の自動設定フラグを記録する			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
bool_t ENET_SetPrefIPv4AutoFlg(schar autoFlg)
{
	bool_t fncRet = FALSE;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	/* 排他ロック開始 */
	
	switch (autoFlg)
	{
		case 0: /* $FALL-THROUGH$ */
		case 1:
			snprintf(buf, ENET_BUF_STR_LEN, "%d", autoFlg);
			break;
		/* 異常値は自動に倒す */
		default:
			DLOG_Error("Err! autoFlg change to AUTO : %d\n", autoFlg);
			snprintf(buf, ENET_BUF_STR_LEN, "%d", ENET_IPV4_IS_AUTO);
			break;
	}
	if (SET_PREF(ENET_KEY_IPV4_IS_AUTO, buf, ENET_FILE_SETTING))
	{
		DLOG_Debug("set : %s\n", buf);
		fncRet = TRUE;
	}
	else
	{
		DLOG_Error("Err! set : %s\n", buf);
	}
	/* 排他ロック解除 */
	

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		設定ファイルからIPv4の自動設定フラグを取得する			*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:	schar	処理結果									*/
/*						0	手動											*/
/*						1	自動											*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルからIPv4の自動設定フラグを取得する			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
schar ENET_GetPrefIPv4AutoFlg(void)
{
	/* 値なしの場合は自動設定 */
	schar fncRet = ENET_IPV4_IS_AUTO;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	GET_PREF(ENET_KEY_IPV4_IS_AUTO, buf, ENET_FILE_SETTING);
	/* 空文字 */
	if (strnlen(buf, ENET_BUF_STR_LEN) <= 0)
	{
		DLOG_Debug("first time.\n");
		/* 自動設定フラグ書き込み */
		ENET_SetPrefIPv4AutoFlg(ENET_IPV4_IS_AUTO);
	}
	else
	{
		/* 先頭文字を判定 */
		switch (buf[0])
		{
			case '0': /* $FALL-THROUGH$ */
			case '1':
				fncRet = (schar) atoi(buf);
				/* 変換後0/1有効値判定 */
				switch (fncRet)
				{
					case 0: /* $FALL-THROUGH$ */
					case 1:
						break;
					default: /* 数値だが0/1でない */
						DLOG_Error("Err! autoFlg change to AUTO : %d\n", fncRet);
						fncRet = ENET_IPV4_IS_AUTO;
						/* 自動設定フラグ書き込み */
						ENET_SetPrefIPv4AutoFlg(ENET_IPV4_IS_AUTO);
						break;
				}
				break;
			/* 先頭が0/1でない */
			default:
				DLOG_Error("Err! autoFlg change to AUTO : %s\n", buf);
				fncRet = ENET_IPV4_IS_AUTO;
				/* 自動設定フラグ書き込み */
				ENET_SetPrefIPv4AutoFlg(ENET_IPV4_IS_AUTO);
				break;
		}
	}
	DLOG_Debug("get : %s->%d\n", buf, fncRet);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルに指定keyのIPv4アドレスを記録する			*/
/*--------------------------------------------------------------------------*/
/*	param			:schar* key	保存キー									*/
/*					EnetIpv4_t* val	保存アドレス							*/
/*--------------------------------------------------------------------------*/
/*	return			:boo_t	処理結果										*/
/*						TRUE	正常	FALSE	異常						*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルに指定keyのIPv4アドレスを記録する			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
bool_t ENET_SetPrefIPv4fmtIP(const schar* key, const EnetIpv4_t* val)
{
	bool_t fncRet = FALSE;
	schar buf[ENET_BUF_STR_LEN] = { };
	/* 排他ロック開始 */
	
	DEBUG_TRCIN();

	snprintf(buf, ENET_BUF_STR_LEN, "%lu", val->addr32bit);
	if (SET_PREF(key, buf, ENET_FILE_SETTING))
	{
		DLOG_Debug("set : %s\n", buf);
		fncRet = TRUE;
	}
	else
	{
		DLOG_Error("Err! set : %s\n", buf);
	}
	/* 排他ロック解除 */
	

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルから指定keyのIPv4アドレスを取得する		*/
/*--------------------------------------------------------------------------*/
/*	param			:schar* key	取得キー									*/
/*					EnetIpv4_t* val	取得データを格納する					*/
/*--------------------------------------------------------------------------*/
/*	return			:boo_t	処理結果										*/
/*						TRUE	正常	FALSE	異常						*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルから指定keyのIPv4アドレスを取得する		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
bool_t ENET_GetPrefIPv4fmtIP(const schar* key, EnetIpv4_t* val)
{
	bool_t fncRet = FALSE;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	if (GET_PREF(key, buf, ENET_FILE_SETTING))
	{
		/* UWのため、atollで変換 */
		val->addr32bit = atoll(buf);
		DLOG_Debug("get : %s\n", buf);
		fncRet = TRUE;
	}
	else
	{
		DLOG_Debug("get NG. can't find key.\n");
	}

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルに無線の設定								*/
/*					(ESSID,password,暗号化方式)を記録する					*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetWlanSettings_t* wlan	無線LAN設定					*/
/*--------------------------------------------------------------------------*/
/*	return			:boo_t	処理結果										*/
/*						TRUE	正常	FALSE	異常						*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルに無線の設定								*/
/*					(ESSID,password,暗号化方式)を記録する					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
bool_t ENET_SetPrefWlanSetting(EnetWlanSettings_t* wlan)
{
	bool_t fncRet = FALSE;
	DEBUG_TRCIN();

	DLOG_Debug("set : %s,%s\n", wlan->essid, wlan->password);
	/* 排他ロック開始 */
	
	if (SET_PREF(ENET_KEY_WLAN_ESSID, wlan->essid, ENET_FILE_WLAN)
	&& SET_PREF(ENET_KEY_WLAN_PASS, wlan->password, ENET_FILE_WLAN))
	{
		DLOG_Debug("set : %s/%s\n", wlan->essid, wlan->password);
		fncRet = TRUE; /* すべて記録できたら成功 */
	}
	/* 排他ロック解除 */
	

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルから無線の設定								*/
/*					(ESSID,password,暗号化方式)を取得する					*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetWlanSettings_t* wlan	取得データを格納する		*/
/*--------------------------------------------------------------------------*/
/*	return			:boo_t	処理結果										*/
/*						TRUE	正常	FALSE	異常						*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルから無線の設定								*/
/*					(ESSID,password,暗号化方式)を取得する					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
bool_t ENET_GetPrefWlanSetting(EnetWlanSettings_t* wlan)
{
	bool_t fncRet = FALSE;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	memset(buf, 0, sizeof(buf));
	if (GET_PREF(ENET_KEY_WLAN_ESSID, buf, ENET_FILE_WLAN))
	{
		/* null止め用-1 */
		strncpy(wlan->essid, buf, sizeof(wlan->essid) - 1);
		memset(buf, 0, sizeof(buf));
		/* null止め用-1 */
		if (GET_PREF(ENET_KEY_WLAN_PASS, buf, ENET_FILE_WLAN))
		{
			strncpy(wlan->password, buf, sizeof(wlan->password) - 1);
			fncRet = TRUE; /* すべて取得できたら成功 */
		}
		else
		{
			DLOG_Debug("Err! password get NG.\n");
		}
	}
	else
	{
		DLOG_Debug("Err! essid get NG.\n");
	}

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルにPINGのタイムアウト時間を記録する			*/
/*--------------------------------------------------------------------------*/
/*	param			:slong timeMs	タイムアウト時間(ミリ秒)				*/
/*--------------------------------------------------------------------------*/
/*	return			:boo_t	処理結果										*/
/*						TRUE	正常	FALSE	異常						*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルにPINGのタイムアウト時間を記録する			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
bool_t ENET_SetPrefPingTimeout(slong timeMs)
{
	bool_t fncRet = FALSE;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();
	
	/* 排他ロック開始 */
	
	snprintf(buf, ENET_BUF_STR_LEN, "%ld", timeMs);
	if (SET_PREF(ENET_KEY_PING_TIMEOUT, buf, ENET_FILE_SETTING))
	{
		DLOG_Debug("set : %s\n", buf);
		fncRet = TRUE;
	}
	else
	{
		DLOG_Error("Err! set : %s\n", buf);
	}
	/* 排他ロック解除 */
	

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルからPINGのタイムアウト時間を取得する		*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						タイムアウト時間									*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルからPINGのタイムアウト時間を取得する		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_GetPrefPingTimeout(void)
{
	/* 値なしの場合は初期値 */
	slong fncRet = ENET_PING_TIMEOUT;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	if (GET_PREF(ENET_KEY_PING_TIMEOUT, buf, ENET_FILE_SETTING))
	{
		fncRet = atoi(buf);
		/* PINGの最短タイムアウト時間より短い */
		if (fncRet < ENET_PING_TIMEOUT)
		{
			fncRet = ENET_PING_TIMEOUT;
			ENET_SetPrefPingTimeout(ENET_PING_TIMEOUT);
			DLOG_Debug("init : %d\n", fncRet);
		}
		DLOG_Debug("get : %s->%d\n", buf, fncRet);
	}
	else
	{
		DLOG_Debug("first time.\n");
		ENET_SetPrefPingTimeout(ENET_PING_TIMEOUT);
	}

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルにDHCPのリトライ回数を記録する				*/
/*--------------------------------------------------------------------------*/
/*	param			:slong cnt	リトライ回数								*/
/*--------------------------------------------------------------------------*/
/*	return			:boo_t	処理結果										*/
/*						TRUE	正常	FALSE	異常						*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルにDHCPのリトライ回数を記録する				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
bool_t ENET_SetPrefDhcpRetries(slong cnt)
{
	bool_t fncRet = FALSE;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	/* 排他ロック開始 */
	
	snprintf(buf, ENET_BUF_STR_LEN, "%ld", cnt);
	if (SET_PREF(ENET_KEY_DHCP_RETRIES, buf, ENET_FILE_SETTING))
	{
		DLOG_Debug("set : %s\n", buf);
		fncRet = TRUE;
	}
	else
	{
		DLOG_Error("Err! set : %d\n", buf);
	}
	/* 排他ロック解除 */
	

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルからDHCPのリトライ回数を取得する			*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						リトライ回数										*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルからDHCPのリトライ回数を取得する			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_GetPrefDhcpRetries(void)
{
	/* 値なしの場合は初期値 */
	slong fncRet = ENET_DHCP_RETRIES;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	if (GET_PREF(ENET_KEY_DHCP_RETRIES, buf, ENET_FILE_SETTING))
	{
		fncRet = atoi(buf);
		/* 0以下は初期値を復元 */
		if (fncRet <= 0)
		{
			fncRet = ENET_DHCP_RETRIES;
			ENET_SetPrefDhcpRetries(ENET_DHCP_RETRIES);
			DLOG_Debug("init : %d\n", fncRet);
		}
		DLOG_Debug("get : %s->%d\n", buf, fncRet);
	}
	else
	{
		DLOG_Debug("first time.\n");
		ENET_SetPrefDhcpRetries(ENET_DHCP_RETRIES);
	}

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルにDHCPのタイムアウト時間を記録する			*/
/*--------------------------------------------------------------------------*/
/*	param			:slong timeSec	タイムアウト時間(秒)					*/
/*--------------------------------------------------------------------------*/
/*	return			:boo_t	処理結果										*/
/*						TRUE	正常	FALSE	異常						*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルにDHCPのタイムアウト時間を記録する			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
bool_t ENET_SetPrefDhcpTimeout(slong timeSec)
{
	bool_t fncRet = FALSE;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	/* 排他ロック開始 */
	
	snprintf(buf, ENET_BUF_STR_LEN, "%ld", timeSec);
	if (SET_PREF(ENET_KEY_DHCP_TIMEOUT, buf, ENET_FILE_SETTING))
	{
		DLOG_Debug("set : %s\n", buf);
		fncRet = TRUE;
	}
	else
	{
		DLOG_Error("Err! set : %s\n", buf);
	}
	/* 排他ロック解除 */
	

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルからDHCPのタイムアウト時間を取得する		*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						タイムアウト時間									*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルからDHCPのタイムアウト時間を取得する		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_GetPrefDhcpTimeout(void)
{
	/* 値なしの場合は初期値 */
	slong fncRet = ENET_DHCP_TIMEOUT;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	if (GET_PREF(ENET_KEY_DHCP_TIMEOUT, buf, ENET_FILE_SETTING))
	{
		fncRet = atoi(buf);
		/* 0以下は初期値を復元 */
		if (fncRet <= 0)
		{
			fncRet = ENET_DHCP_TIMEOUT;
			ENET_SetPrefDhcpTimeout(ENET_DHCP_TIMEOUT);
			DLOG_Debug("init : %d\n", fncRet);
		}
		DLOG_Debug("get : %s->%d\n", buf, fncRet);
	}
	else
	{
		DLOG_Debug("first time.\n");
		ENET_SetPrefDhcpTimeout(ENET_DHCP_TIMEOUT);
	}

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルにDHCPの取得再開間隔を記録する				*/
/*--------------------------------------------------------------------------*/
/*	param			:slong timeSec	取得再開間隔(秒)						*/
/*--------------------------------------------------------------------------*/
/*	return			:boo_t	処理結果										*/
/*						TRUE	正常	FALSE	異常						*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルにDHCPの取得再開間隔を記録する				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
bool_t ENET_SetPrefDhcpTryagain(slong timeSec)
{
	bool_t fncRet = FALSE;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	/* 排他ロック開始 */
	
	snprintf(buf, ENET_BUF_STR_LEN, "%ld", timeSec);
	if (SET_PREF(ENET_KEY_DHCP_TRYAGAIN, buf, ENET_FILE_SETTING))
	{
		DLOG_Debug("set : %s\n", buf);
		fncRet = TRUE;
	}
	else
	{
		DLOG_Error("Err! set : %s\n", buf);
	}
	/* 排他ロック解除 */
	

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルからDHCPの取得再開間隔を取得する			*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						取得再開間隔										*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルからDHCPの取得再開間隔を取得する			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_GetPrefDhcpTryagain(void)
{
	/* 値なしの場合は初期値 */
	slong fncRet = ENET_DHCP_TRYAGAIN;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	if (GET_PREF(ENET_KEY_DHCP_TRYAGAIN, buf, ENET_FILE_SETTING))
	{
		fncRet = atoi(buf);
		/* 0以下は初期値を復元 */
		if (fncRet <= 0)
		{
			fncRet = ENET_DHCP_TRYAGAIN;
			ENET_SetPrefDhcpTryagain(ENET_DHCP_TRYAGAIN);
			DLOG_Debug("init : %d\n", fncRet);
		}
		DLOG_Debug("get : %s\n", buf);
	}
	else
	{
		DLOG_Debug("first time.\n");
		ENET_SetPrefDhcpTryagain(ENET_DHCP_TRYAGAIN);
	}

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルにIP監視間隔を記録する						*/
/*--------------------------------------------------------------------------*/
/*	param			:slong timeMs	IP監視間隔(ミリ秒)						*/
/*--------------------------------------------------------------------------*/
/*	return			:boo_t	処理結果										*/
/*						TRUE	正常	FALSE	異常						*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルにIP監視間隔を記録する						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
bool_t ENET_SetPrefIpWatchInterval(slong timeMs)
{
	bool_t fncRet = FALSE;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	/* 排他ロック開始 */
	
	snprintf(buf, ENET_BUF_STR_LEN, "%ld", timeMs);
	if (SET_PREF(ENET_KEY_IP_WATCH_INTERVAL, buf, ENET_FILE_SETTING))
	{
		DLOG_Debug("set : %s\n", buf);
		fncRet = TRUE;
	}
	else
	{
		DLOG_Error("Err! set : %s\n", buf);
	}
	/* 排他ロック解除 */
	

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルからIP監視間隔を取得する					*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						IP監視間隔											*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルからIP監視間隔を取得する					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_GetPrefIpWatchInterval(void)
{
	/* 値なしの場合は初期値 */
	slong fncRet = ENET_IP_WATCH_INTERVAL;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	if (GET_PREF(ENET_KEY_IP_WATCH_INTERVAL, buf, ENET_FILE_SETTING))
	{
		fncRet = atoi(buf);
		/* 許容最短時間より短い */
		if (fncRet < ENET_IP_WATCH_INTERVAL_MIN)
		{
			fncRet = ENET_IP_WATCH_INTERVAL;
			ENET_SetPrefIpWatchInterval(ENET_IP_WATCH_INTERVAL);
			DLOG_Debug("init : %d\n", fncRet);
		}
		DLOG_Debug("get : %s->%d\n", buf, fncRet);
	}
	else
	{
		DLOG_Debug("first time.\n");
		ENET_SetPrefIpWatchInterval(ENET_IP_WATCH_INTERVAL);
	}

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルにルータ疎通確認間隔を記録する				*/
/*--------------------------------------------------------------------------*/
/*	param			:slong timeMs	ルータ疎通確認間隔(ミリ秒)				*/
/*--------------------------------------------------------------------------*/
/*	return			:boo_t	処理結果										*/
/*						TRUE	正常	FALSE	異常						*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルにルータ疎通確認間隔を記録する				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
bool_t ENET_SetPrefSendPingInterval(slong timeMs)
{
	bool_t fncRet = FALSE;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	/* 排他ロック開始 */
	
	snprintf(buf, ENET_BUF_STR_LEN, "%ld", timeMs);
	if (SET_PREF(ENET_KEY_SEND_PING_INTERVAL, buf, ENET_FILE_SETTING))
	{
		DLOG_Debug("set : %s\n", buf);
		fncRet = TRUE;
	}
	else
	{
		DLOG_Error("Err! set : %s\n", buf);
	}
	/* 排他ロック解除 */
	

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルからルータ疎通確認間隔を取得する			*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						ルータ疎通確認間隔									*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルからルータ疎通確認間隔を取得する			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_GetPrefSendPingInterval(void)
{
	/* 値なしの場合は初期値 */
	slong fncRet = ENET_SEND_PING_INTERVAL;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	if (GET_PREF(ENET_KEY_SEND_PING_INTERVAL, buf, ENET_FILE_SETTING))
	{
		fncRet = atoi(buf);
		/* 許容最短時間より短い */
		if (fncRet < ENET_SEND_PING_INTERVAL_MIN)
		{
			fncRet = ENET_SEND_PING_INTERVAL;
			ENET_SetPrefSendPingInterval(ENET_IP_WATCH_INTERVAL);
			DLOG_Debug("init : %d\n", fncRet);
		}
		DLOG_Debug("get : %s->%d\n", buf, fncRet);
	}
	else
	{
		DLOG_Debug("first time.\n");
		ENET_SetPrefSendPingInterval(ENET_IP_WATCH_INTERVAL);
	}

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:														*/
/*				設定ファイルにポート番号と対応するイベントフラグを記録する	*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong port	ポート番号									*/
/*					ulong flag	イベントフラグ								*/
/*--------------------------------------------------------------------------*/
/*	return			:boo_t	処理結果										*/
/*						TRUE	正常	FALSE	異常						*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*				設定ファイルにポート番号と対応するイベントフラグを記録する	*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
bool_t ENET_SetPrefChgEvntFlag(ulong port, ulong flag)
{
	bool_t fncRet = FALSE;
	schar bufKey[ENET_BUF_STR_LEN] = { }; /* ポート番号 */
	schar bufVal[ENET_BUF_STR_LEN] = { }; /* イベントフラグ */
	DEBUG_TRCIN();

	/* 排他ロック開始 */
	
	snprintf(bufKey, ENET_BUF_STR_LEN, "%lu", port);
	snprintf(bufVal, ENET_BUF_STR_LEN, "%lu", flag);
	/* ポート番号=イベントフラグで記録 */
	if (SET_PREF(bufKey, bufVal, ENET_FILE_NOTIFY_PROC))
	{
		DLOG_Debug("set port,flag : %s,%s\n", bufKey, bufVal);
		fncRet = TRUE;
	}
	else
	{
		DLOG_Error("Err! set port,flag : %s,%s\n", bufKey, bufVal);
	}
	/* 排他ロック解除 */
	

	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:指定ポートに対応するイベントフラグを取得する			*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong port	ポート番号									*/
/*--------------------------------------------------------------------------*/
/*	return			:ulong	処理結果										*/
/*						イベントフラグ										*/
/*--------------------------------------------------------------------------*/
/*	detail			:指定ポートに対応するイベントフラグを取得する			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
ulong ENET_GetPrefChgEvntFlag(ulong port)
{
	ulong fncRet = 0; /* 保存されたフラグ */
	slong getVal = 0; /* ファイルからの取得値格納、閾値判定用 */
	schar bufKey[ENET_BUF_STR_LEN] = { };
	schar bufVal[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	snprintf(bufKey, ENET_BUF_STR_LEN, "%lu", port);
	if (GET_PREF(bufKey, bufVal, ENET_FILE_NOTIFY_PROC))
	{
		getVal = atoll(bufVal);
		if (0 < getVal)
		{
			fncRet = getVal;
		}
	}

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
void ENET_ResetApDevice(void)
{
	DEBUG_TRCIN();
	ENET_ReadWlanLimitFile();
	ENET_ExecCmdKillHostapd();
	if(ENET_NORMAL_MODE == g_enet_WiFiRunMode)
	{
		if(g_enet_WlanLimitTime > 0)
		{
			ENET_ExecWlanSettingThread();
			FRM_TimerSetOneShot(ENET_ONE_SHOT_ID_WLANLMT, g_enet_WlanLimitTime * (60 * 1000), NULL);
			DLOG_Debug("WIFI set timer %d", g_enet_WlanLimitTime);
		}
		else
		{
			DLOG_Debug("WlanLimitTime = 0!");
		}
	}
	else
	{
		ENET_ExecWlanSettingThread();
		FRM_TimerCancelOneShot(ENET_ONE_SHOT_ID_WLANLMT);
		DLOG_Debug("WIFI keep running");
	}
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:ICMPヘッダ用チェックサムを算出する						*/
/*--------------------------------------------------------------------------*/
/*	param			:ushort *buf	チェックサム算出対象参照				*/
/*					slong bufsz	チェックサム算出対象サイズ					*/
/*--------------------------------------------------------------------------*/
/*	return			:ushort	計算結果										*/
/*--------------------------------------------------------------------------*/
/*	detail			:ICMPヘッダ用チェックサムを算出する						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
ushort ENET_IcmpChksum(ushort *buf, slong bufsz)
{
	ulong sum = 0;
	while (bufsz > 1)
	{
		sum += *buf;
		buf++;
		bufsz -= 2;
	}
	if (bufsz == 1)
	{
		sum += *(uchar*) buf;
	}
	sum = (sum & 0xffff) + (sum >> 16);
	sum = (sum & 0xffff) + (sum >> 16);
	return (~sum);
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
/*	history			:2023/05/08 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
slong ENET_GetWlanSetting(EnetWlanSettings_t* getWlan)
{
	slong fncRet = ENET_ERR1;
	DEBUG_TRCIN();

	if (ENET_GetPrefWlanSetting(getWlan))
	{
		fncRet = ENET_SUCCESS;
		/* 現在値取得成功 */
		DLOG_Debug("ENET_GetPrefWlanSetting : OK.\n");
		DLOG_Debug("wlan : %s,%s\n", getWlan->essid, getWlan->password);
	}
	else
	{
		/* 設定値取得失敗 */
		DLOG_Error("Err! ENET_GetPrefWlanSetting : NG.\n");
	}

	DEBUG_TRCOUT();
	return (fncRet);
}
/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description		:指定デバイス(NIC)の状態をチェックする					*/
/*--------------------------------------------------------------------------*/
/*	param			:schar* ifaceName	指定デバイス名（e.g. "eth0"）		*/
/*--------------------------------------------------------------------------*/
/*	return			:EnetDeviceState_t	処理結果							*/
/*--------------------------------------------------------------------------*/
/*	detail			:指定デバイス(NIC)の状態をチェックする					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
EnetDeviceState_t enet_GetDeviceState(const schar* ifaceName)
{
	EnetDeviceState_t fncRet = ENET_DEVICE_NONE;
	sint fd = 0;
	struct ifreq req;
	DEBUG_TRCIN();

#ifndef CROSSDEBUG
	fd = socket(AF_INET, SOCK_DGRAM, 0); /*TODO:*/
#endif
	if (fd == 0)
	{
		DLOG_Error("Err! socket is NULL.\n");
		return (fncRet);
	}
	strncpy(req.ifr_name, ifaceName, IFNAMSIZ - 1);
	if (ioctl(fd, SIOCGIFFLAGS, &req))
	{
		/* デバイスが存在しない */
		DLOG_Debug("ioctl can't get ifaceName : %s\n", ifaceName);
	}
	else 
	{
		if (req.ifr_flags & IFF_UP)
		{
			/* LinkUp */
			fncRet = ENET_DEVICE_UP;
		}
		else
		{
			/* LinkDown */
			fncRet = ENET_DEVICE_DOWN;
		}
		DLOG_Debug("%6s : %s\n", ifaceName, (fncRet == ENET_DEVICE_UP) ? "UP" : "DOWN");
	}
	close(fd);

	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:指定デバイス(NIC)の動作状態(ネットワークのUP/DOWN)		*/
/*					を切り替える											*/
/*--------------------------------------------------------------------------*/
/*	param			:schar* ifaceName	指定デバイス名（e.g. "eth0"）		*/
/*					EnetLinkMode_t link	動作状態							*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:指定デバイス(NIC)の動作状態(ネットワークのUP/DOWN)		*/
/*					を切り替える											*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong enet_SetLinkMode(const schar* ifaceName, const EnetLinkMode_t link)
{
	slong fncRet = ENET_ERR1;
	sint fd = 0;
	struct ifreq req;
	DEBUG_TRCIN();

#ifndef CROSSDEBUG
	fd = socket(AF_INET, SOCK_DGRAM, 0); /*TODO:*/
#endif
	if (fd == 0)
	{
		DLOG_Error("Err! socket is NULL.\n");
		return (fncRet);
	}
	strncpy(req.ifr_name, ifaceName, IFNAMSIZ - 1);
	if (ioctl(fd, SIOCGIFFLAGS, &req))
	{
		DLOG_Error("Err! ioctl get ifaceName : %s\n", ifaceName);
		close(fd);
	}
	else
	{
		switch (link)
		{
			case ENET_LINK_UP:
				req.ifr_flags |= IFF_UP;
				break;
			case ENET_LINK_DOWN:
				req.ifr_flags &= ~IFF_UP;
				break;
			default:
				DLOG_Error("Err! Invalid specified : %d\n", link);
				close(fd);
				return (fncRet);
		}

		if (ioctl(fd, SIOCSIFFLAGS, &req))
		{
			DLOG_Error("Err! ioctl set ifaceName : %s\n", ifaceName);
		}
		else
		{
			DLOG_Debug("ifaceName/link : %s/%d\n", ifaceName, link);
			fncRet = ENET_SUCCESS;
		}
		close(fd);
	}
	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:無線LANを設定する										*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetWlanSettings_t*	setWlan	無線LAN設定				*/	
/*					EnetWlanSetExt_t		setWlan	拡張設定				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無線LANを設定する										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong enet_SetWlanSetting(EnetWlanSettings_t* setWlan, EnetWlanSetExt_t ext)
{

	slong fncRet = ENET_ERR1;
	EnetWlanSettings_t wlanbk = { };
	DEBUG_TRCIN();

	DLOG_Debug("wlan : %s,%s\n", setWlan->essid, setWlan->password);
	if(ENET_WLANSET_EXT_MAX <= ext)
	{
		/* 拡張設定範囲外 */
		return (fncRet);
	}
	/* 現在の設定値をバックアップ */
	if (ENET_GetPrefWlanSetting(&wlanbk))
	{
		/* 現在値取得成功 */
		DLOG_Debug("ENET_GetPrefWlanSetting OK.\n");
		/* 設定値を更新 */
		if (ENET_SetPrefWlanSetting(setWlan))
		{
			/* 更新成功 */
			DLOG_Debug("ENET_SetPrefWlanSetting OK.\n");
			fncRet = ENET_SUCCESS;
		}
		else 
		{
			/* 更新失敗 */
			DLOG_Error("Err! ENET_SetPrefWlanSetting NG.\n");
			/* 設定値を復元 */
			if (ENET_SetPrefWlanSetting(&wlanbk))
			{
				/* 設定ファイル復元成功 */
				DLOG_Debug("setting restore : OK\n");
			}
			else
			{
				/* 復元失敗 */
				DLOG_Debug("Fatal! setting restore : NG\n");
			}
		}
	}
	else
	{
		/* 設定値のバックアップ取得失敗 */
		DLOG_Error("Err! ENET_GetPrefWlanSetting : NG\n");
	}
	/* 設定ファイル更新指定（設定値復元でのみ使用）でなければ */
	if(ext != ENET_WLANSET_EXT_UPD_SETFILE)
	{
		if (ENET_GetPrefNetworkType() == ENET_NETWORK_WIRELESS
		&& fncRet == ENET_SUCCESS)
		{
			ENET_UpdateNetworkType();
		}
	}
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
slong enet_SetWlanUp(bool_t up)
{
	slong fncRet = ENET_ERR1;
	EnetLinkMode_t link = ENET_LINK_UP;
	DEBUG_TRCIN();

	if (up)
	{
		link = ENET_LINK_UP;
	}
	else
	{
		link = ENET_LINK_DOWN;
	}
	fncRet = enet_SetLinkMode(ENET_GetNetworkTypeStrTbl(ENET_NETWORK_WIRELESS), link);
	DLOG_Debug("enet_SetLinkMode : [ %s ]\n", fncRet == ENET_SUCCESS?"OK":ENET_YELLOW"NG"ENET_RESET);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:DNS情報をファイル「/etc/resolv.conf」より取得する。	*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetIpv4Settings_t* v4	取得データを格納する			*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:DNS情報をファイル「/etc/resolv.conf」より取得する。	*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong enet_GetDNS(EnetIpv4Settings_t* v4)
{
	slong fncRet = 0; /* 検出したDNSの数(0～2) */
	slong scanElements = 0;
	FILE* fp;
	bool_t hasDNS1 = FALSE;
	bool_t dapFlag = FALSE;
	EnetDnsInfo_t dinf = { };
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	if (v4 == NULL)
	{
		DLOG_Error("Err! v4 is NULL\n");
		return (fncRet);
	}
	/* 返却用引数の初期化 */
	v4->dns1.addr32bit = 0;
	v4->dns2.addr32bit = 0;
	fp = CMN_FOPEN(ENET_ETC_RESOLV_CONF, ENET_OPEN_TYPE_READ);
	if (fp == NULL)
	{
		DLOG_Error("Err! fopen : %s\n", ENET_ETC_RESOLV_CONF);
		return (fncRet);
	}
	/* 行読み込み */
	while (FALSE == dapFlag && (fgets(buf, ENET_BUF_STR_LEN, fp)) != NULL)
	{
		/* バッファ初期化 */
		memset(&dinf, 0, sizeof(dinf));
		/* フォーマット：nameserver 192.168.123.234 */
		scanElements = sscanf(buf, "%10s %ld.%ld.%ld.%ld\n", dinf.resolver,
		&dinf.octet[0], &dinf.octet[1], &dinf.octet[2], &dinf.octet[3]);
		/* 変換個数不一致 */
		if (scanElements == ENET_DNS_INFO_ELEMENTS)
		{
			/* 読み込み失敗 */
			if ((scanElements <= EOF)
			&& feof(fp)) 
			{
				/* ファイル終端 */
				dapFlag = TRUE;
			}
			if(FALSE == dapFlag)
			{
				/* resolverがネームサーバと一致 */
				if (strncmp(dinf.resolver, ENET_NAMESERVER, ENET_DNS_RESOLVER_LEN) == 0)
				{
					if (hasDNS1)
					{
						v4->dns2.octet[0] = (uchar) dinf.octet[0];
						v4->dns2.octet[1] = (uchar) dinf.octet[1];
						v4->dns2.octet[2] = (uchar) dinf.octet[2];
						v4->dns2.octet[3] = (uchar) dinf.octet[3];
						DLOG_Debug("DNS2 : ", v4->dns2);
						fncRet++;
						/* DNS1とDNS2を検出 */
						dapFlag = TRUE;
					}
					else
					{
						v4->dns1.octet[0] = (uchar) dinf.octet[0];
						v4->dns1.octet[1] = (uchar) dinf.octet[1];
						v4->dns1.octet[2] = (uchar) dinf.octet[2];
						v4->dns1.octet[3] = (uchar) dinf.octet[3];
						DLOG_Debug("DNS1 : ", v4->dns1);
						fncRet++;
						/* DNS1を検出 */
						hasDNS1 = TRUE;
					}
				}
			}
		}
	}
	DLOG_Debug("found DNS : %d\n", fncRet);
	CMN_FCLOSE(fp);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:DNS情報をファイル「/etc/resolv.conf」に設定する		*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetIpv4Settings_t* v4	DNS設定							*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:DNS情報をファイル「/etc/resolv.conf」に設定する		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong enet_SetDNS(EnetIpv4Settings_t* v4)
{
	slong fncRet = ENET_ERR1;
	schar buf[ENET_BUF_STR_LEN] = { };
	FILE* fp;
	DEBUG_TRCIN();

	if (v4 == NULL)
	{
		DLOG_Error("Err! v4 is NULL\n");
		return (fncRet);
	}
	fp = CMN_FOPEN(ENET_ETC_RESOLV_CONF, ENET_OPEN_TYPE_WRITE);
	if (fp == NULL)
	{
		DLOG_Error("Err! fopen : %s\n", ENET_ETC_RESOLV_CONF);
		return (fncRet);
	}

	snprintf(buf, ENET_BUF_STR_LEN, "%s %d.%d.%d.%d\n", ENET_NAMESERVER,
	v4->dns1.octet[0], v4->dns1.octet[1], v4->dns1.octet[2], v4->dns1.octet[3]);
	fputs(buf, fp);
	/* バッファ初期化 */
	memset(buf, 0, ENET_BUF_STR_LEN);
	snprintf(buf, ENET_BUF_STR_LEN, "%s %d.%d.%d.%d\n", ENET_NAMESERVER,
	v4->dns2.octet[0], v4->dns2.octet[1], v4->dns2.octet[2], v4->dns2.octet[3]);
	fputs(buf, fp);

	fncRet = ENET_SUCCESS;
	DLOG_Debug("DNS1 : %d\n", v4->dns1);
	DLOG_Debug("DNS2 : %d\n", v4->dns2);
	CMN_FCLOSE(fp);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルにPROXY設定を保存する						*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetProxy_t* proxy	Proxy設定							*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルにPROXY設定を保存する						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong enet_SetProxy(EnetProxy_t* proxy)
{
	slong fncRet = ENET_ERR1;
	schar* key;
	schar val[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();
	/* PROXY有効/無効フラグの値判定 */
	switch (proxy->enableFlg)
	{
		/* $FALL-THROUGH$ */
		case ENET_TRUE:
		case ENET_FALSE:
			break;
		default:
			proxy->enableFlg = ENET_FALSE;
			DLOG_Error("Err! Invalid proxy enableFlg : %d\n", proxy->enableFlg);
			return (fncRet);
	}

	memset(val, 0, sizeof(val));
	snprintf(val, ENET_BUF_STR_LEN, "%d", proxy->enableFlg);
	key = ENET_KEY_PROXY_ENABLE;
	SET_PREF(key, val, ENET_FILE_PROXY);
	memset(val, 0, sizeof(val));
	snprintf(val, ENET_BUF_STR_LEN, "%d", proxy->port);
	key = ENET_KEY_PROXY_PORT;
	SET_PREF(key, val, ENET_FILE_PROXY);
	key = ENET_KEY_PROXY_HOST;
	SET_PREF(key, proxy->addr, ENET_FILE_PROXY);
	/* Proxy変化通知 */
	ENET_SendChgEvnt(ENET_CHGEVNT_PROXY_SETTING);
	fncRet = ENET_SUCCESS;

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:手動IPv4アドレスを設定反映する							*/
/*--------------------------------------------------------------------------*/
/*	param			:schar* ifaceName	指定デバイス名（e.g. "eth0"）		*/
/*					EnetIpv4Settings_t* v4	IPv4アドレス設定				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:手動IPv4アドレスを設定反映する							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong enet_SetIPv4Self(const schar* ifaceName, EnetIpv4Settings_t* v4)
{
	slong fncRet = ENET_ERR1;
	sint fd = 0;
	struct ifreq ifr;
	struct sockaddr_in sin;
	struct rtentry rtent = { };
	struct sockaddr_in *p_sin;
	EnetIpv4_t gateway = { };
	DEBUG_TRCIN();

#ifndef CROSSDEBUG
	fd = socket(AF_INET, SOCK_DGRAM, 0);
#endif
	if (fd == 0)
	{
		DLOG_Error("Err! socket is NULL.\n");
		return (fncRet);
	}

	memset(&ifr, 0, sizeof(ifr));
	/* set ifaceName */
	strncpy(ifr.ifr_name, ifaceName, IFNAMSIZ - 1);
	/* IPv4 */
	sin.sin_family = AF_INET;

	/* set new ip address */
	sin.sin_addr = *((struct in_addr*) &v4->ip.addr32bit);
	memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));
	if (ioctl(fd, SIOCSIFADDR, &ifr))
	{
		DLOG_Error("Err! ioctl(SIOCSIFADDR).\n");
		close(fd);
		return (fncRet);
	}

	/* set new Netmask */
	if(v4->ip.addr32bit != 0)
	{
		sin.sin_addr = *((struct in_addr*) &v4->subnet.addr32bit);
		memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));
		if (ioctl(fd, SIOCSIFNETMASK, &ifr))
		{
			DLOG_Error("Err! ioctl(SIOCSIFNETMASK).\n");
			close(fd);
			return (fncRet);
		}
	}

	if (enet_SetDNS(v4) != ENET_SUCCESS)
	{
		close(fd);
		return (fncRet);
	}

	/* dest address */
	p_sin = (struct sockaddr_in *) &rtent.rt_dst;
	p_sin->sin_family = AF_INET;
	p_sin->sin_addr.s_addr = 0;

	/* mask address */
	p_sin = (struct sockaddr_in *) &rtent.rt_genmask;
	p_sin->sin_family = AF_INET;
	p_sin->sin_addr.s_addr = 0;

	/* gateway address */
	p_sin = (struct sockaddr_in *) &rtent.rt_gateway;
	p_sin->sin_family = AF_INET;

	/* metric +1 for binary compatibility! */
	rtent.rt_metric = 1;
	/* is a gateway */
	rtent.rt_flags = (RTF_UP | RTF_GATEWAY);

	/* delete routing table entry */
	if (ENET_GetDefaultGW(ifaceName, &gateway) == ENET_SUCCESS)
	{
		/* gatewayが存在したら */
		if (0 < gateway.addr32bit)
		{
			DLOG_Debug("rmv gateway : %d\n", gateway);
			p_sin->sin_addr.s_addr = gateway.addr32bit;
			/* 既存のgatewayを削除 */
			if (ioctl(fd, SIOCDELRT, &rtent) < 0)
			{
				DLOG_Error("Err! ioctl(SIOCDELRT).\n");
				close(fd);
				return (fncRet);
			}
		}
	}

	/* add routing table entry */
	if(v4->ip.addr32bit != 0)
	{
		p_sin->sin_addr.s_addr = v4->gateway.addr32bit;
		DLOG_Debug("add gateway : %d\n", v4->gateway);
		/* gatewayを追加 */
		if (ioctl(fd, SIOCADDRT, &rtent) < 0)
		{
			DLOG_Error("Err! ioctl(SIOCADDRT).\n");
			close(fd);
			return (fncRet);
		}
	}

	fncRet = ENET_SUCCESS;
	close(fd);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:IPv4(自動＋手動)/IPv6アドレスを設定する				*/
/*--------------------------------------------------------------------------*/
/*	param			:schar* ifaceName	指定デバイス名（e.g. "eth0"）		*/
/*					void* settings	IPアドレス設定							*/
/*					EnetIpType_t ipType	設定するIP種別を指定する			*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:IPv4(自動＋手動)/IPv6アドレスを設定する				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong enet_SetIPAddr(const schar* ifaceName, void* settings, const EnetIpType_t ipType)
{
	slong fncRet = ENET_ERR1;
	EnetIpv4Settings_t* v4 = NULL;
	EnetIpv6Settings_t* v6 = NULL;
	EnetIpv4Settings_t v4bk = { };
#if 0
	EnetIpv6Settings_t v6bk = {}; //(TBD) v6実装
#endif
	bool_t chgOnlySetting = FALSE;
	DEBUG_TRCIN();

	if (settings == NULL)
	{
		DLOG_Error("Err! settings is NULL.\n");
		/* 引数エラー */
		return (fncRet);
	}
	/* NULLチェック */
	if (ifaceName == NULL)
	{
		DLOG_Error("Err! ifaceName is NULL.\n");
		/* 引数エラー */
		return (fncRet);
	}
	/* 空文字、設定ファイルのみ変更 */
	if (strnlen(ifaceName, IFNAMSIZ) <= 0)
	{
		DLOG_Debug("ifaceName is BLANK. changeOnlySetting.\n");
		chgOnlySetting = TRUE;
	}

	switch (ipType)
	{
		case ENET_IPV4:
			v4 = (EnetIpv4Settings_t*) settings;
			DLOG_Info("v4->autoFlg : %d\n", v4->autoFlg);
			DLOG_Info("v4->ip      %d\n : ", v4->ip);
			DLOG_Info("v4->subnet  %d\n : ", v4->subnet);
			DLOG_Info("v4->gateway %d\n : ", v4->gateway);
			DLOG_Info("v4->dns1    %d\n : ", v4->dns1);
			DLOG_Info("v4->dns2    %d\n : ", v4->dns2);

			/* 巻き戻し用設定値取得 */
			v4bk.autoFlg = ENET_GetPrefIPv4AutoFlg();
			if (!ENET_SetPrefIPv4AutoFlg(v4->autoFlg))
			{
				/* 書き込み異常 */
				break;
			}
			if (v4->autoFlg)
			{
				/* 未使用変数の初期化 */
				v4->ip.addr32bit = 0;
				v4->subnet.addr32bit = 0;
				v4->gateway.addr32bit = 0;
				v4->dns1.addr32bit = 0;
				v4->dns2.addr32bit = 0;
				if (chgOnlySetting)
				{
					fncRet = ENET_SUCCESS;
					break;
				}
				else
				{
					fncRet = ENET_ExecDhcpThread();
				}
			}
			else
			{
				/* 手動設定 */
				ENET_ExecCmdKillDhcp();
				/* 設定値更新 */
				if (ENET_SetPrefIPv4fmtIP(ENET_KEY_IPV4_ADDR_SELF, &v4->ip)
				&& ENET_SetPrefIPv4fmtIP(ENET_KEY_SUBNET, &v4->subnet)
				&& ENET_SetPrefIPv4fmtIP(ENET_KEY_GATEWAY, &v4->gateway)
				&& ENET_SetPrefIPv4fmtIP(ENET_KEY_DNS1, &v4->dns1)
				&& ENET_SetPrefIPv4fmtIP(ENET_KEY_DNS2, &v4->dns2)
				)
				{
					/* 全アドレスの記録に成功 */
					DLOG_Debug("setting write OK.\n");
					if (chgOnlySetting)
					{
						fncRet = ENET_SUCCESS;
						break;
					}
					if (enet_SetIPv4Self(ifaceName, v4) == ENET_SUCCESS)
					{
						/* 設定成功 */
						fncRet = ENET_SUCCESS;
					}
					else
					{
						/* 設定失敗 */
						DLOG_Error("Err! setting Fail.\n");
						v4->ip.addr32bit = 0;
						v4->subnet.addr32bit = 0;
						v4->gateway.addr32bit = 0;
						v4->dns1.addr32bit = 0;
						v4->dns2.addr32bit = 0;
						enet_SetIPv4Self(ifaceName, v4);
					}
				}
				else
				{
					/* 書き込み異常 */
					DLOG_Error("Err! setting write NG.\n");
				}
			}
			if (v4bk.autoFlg != ENET_GetPrefIPv4AutoFlg())
			{
				/* 自動/手動変化通知 */
				ENET_SendChgEvnt(ENET_CHGEVNT_IP_HANDLING);
			}
			break;
		case ENET_IPV6:
			/* (TBD) v6実装 */
			v6 = (EnetIpv6Settings_t*) settings;
			/* 手動設定のみ */
			v6->ip.addr32bit[0] = 0;
			v6->ip.addr32bit[1] = 0;
			v6->ip.addr32bit[2] = 0;
			v6->ip.addr32bit[3] = 0;

			v6->subnet.addr32bit[0] = 0;
			v6->subnet.addr32bit[1] = 0;
			v6->subnet.addr32bit[2] = 0;
			v6->subnet.addr32bit[3] = 0;
			break;
		default:
			DLOG_Error("Err! ipType : %d\n", ipType);
			/* 引数エラー：IPv4/IPv6以外が指定された */
			break;
	}

	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:ネットワーク種別を設定する								*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetNetworkType_t	type	ネットワーク種別			*/
/*					EnetNettypeExt_t	ext		拡張設定					*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:ネットワーク種別を設定する								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong enet_SetNetworkType(const EnetNetworkType_t type, const EnetNettypeExt_t ext)
{
	slong fncRet = ENET_ERR1;
	EnetNetworkType_t typebk = ENET_NETWORK_NONE;
	DEBUG_TRCIN();

	/* 閾値未満 */
	if ((ENET_NETWORK_NONE > type)
	|| (type >= ENET_NETWORK_TYPE_MAX)) /* 最大値以上 */
	{
		DLOG_Error("Err! Invalid type : %d\n", type);
		/* ネットワークタイプ範囲外 */
		return (fncRet);
	}
	/* 有効範囲外 */
	if (ENET_NETTYPE_EXT_MAX <= ext)
	{
		DLOG_Error("Err! Invalid ext : %d\n", ext);
		/* 拡張設定範囲外 */
		return (fncRet);
	}
	/* 設定ファイル更新指定（設定値復元でのみ使用）でなければ */
	if (ext != ENET_NETTYPE_EXT_UPD_SETFILE)
	{
		/* ネットワーク無効以外はデバイスチェック */
		if (type != ENET_NETWORK_NONE)
		{
			/* デバイスが存在しない */
			if (enet_GetDeviceState(ENET_GetNetworkTypeStrTbl(type)) == ENET_DEVICE_NONE)
			{
				DLOG_Error("Err! device not detected.\n");
				/* デバイスが検出されない（主に無線） */
				return (fncRet);
			}
		}
	}
	/* 復元用に設定値取得 */
	typebk = ENET_GetPrefNetworkType();
	if (type == typebk)
	{
		/* ネットワーク変更なし、処理は成功扱い */
		fncRet = ENET_SUCCESS;
		DLOG_Error("Err! not changed network.\n");
		return (fncRet);
	}
	if (ENET_SetPrefNetworkType(type))
	{
		/* 設定ファイル更新成功 */
		DLOG_Debug("set type : %d\n", type);
		/* 設定ファイル更新指定(設定値復元でのみ使用)でなければ */
		if (ext != ENET_NETTYPE_EXT_UPD_SETFILE)
		{
			/* 設定ファイルに従い実際のネットワークに反映 */
			if (ENET_UpdateNetworkType() == ENET_SUCCESS)
			{
				/* ネットワーク切り替え成功 */
				DLOG_Debug("change network : OK.\n");
				fncRet = ENET_SUCCESS;
				/* ネットワーク種別変化通知 */
				ENET_SendChgEvnt(ENET_CHGEVNT_NETWORK_TYPE);
				if (type == ENET_NETWORK_NONE)
				{
					/* 切断でIPをリセットするためIP監視スレッド起動 */
					ENET_WatchIPThread();
				}
			}
			else {
				/* ネットワーク切り替え失敗 */
				DLOG_Error("Err! change network : NG.\n");
				if (ENET_SetPrefNetworkType(typebk))
				{
					/* 設定ファイル書き戻し成功 */
					DLOG_Debug("setting restore : OK\n");
					if (ENET_UpdateNetworkType() == ENET_SUCCESS)
					{
						/* 元のネットワーク復元成功 */
						DLOG_Debug("network rollback : OK\n");
					}
					else
					{
						/* 元のネットワーク復元失敗 */
						DLOG_Warn("Fatal! network rollback : NG\n");
					}
				}
				else
				{
					/* 設定ファイル書き戻し失敗 */
					DLOG_Warn("Fatal! setting restore : NG\n");
				}
			}
		}
		else
		{
			/* 設定ファイル更新指定の場合は、設定ファイル更新できればOK */
			fncRet = ENET_SUCCESS;
		}
	}
	else
	{
		/* 設定ファイル更新失敗 */
		DLOG_Error("Err! set type : %d\n", type);
	}

	DEBUG_TRCIN();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:ルータとの疎通確認スレッドを実行する					*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong cPort											*/
/*					結果通知するプロセスのCポート、0のとき通知しない		*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:ルータとの疎通確認スレッドを実行する					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong enet_ChkRouterConnection(ulong cPort)
{
	slong fncRet = 0;
	DEBUG_TRCIN();

	if (ENET_GetPrefNetworkType() == ENET_NETWORK_WIRELESS)
	{
		DLOG_Debug("ENET_AsyncScanEssid IN: OK\n");
		ENET_AsyncScanEssid();
	}
	/* gatewayにping */
	fncRet = ENET_SendPingThread(cPort);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:イベント通知登録する									*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong port	登録するポート								*/
/*					ulong eventFlag	登録するイベント						*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:イベント通知登録する									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong enet_SetChgEvntListener(ulong port, ulong eventFlag)
{
	slong fncRet = ENET_ERR1;
	slong i = 0;
	/* 登録済み判定 */
	bool_t existPort = FALSE;
	/* 対応フラグフィルタ */
	ulong supportFlag = 0;
	/* 有効イベントフラグ */
	ulong validFlag = 0;
	DEBUG_TRCIN();

	supportFlag = (ENET_CHGEVNT_IP_ADDRESS
	| ENET_CHGEVNT_IP_HANDLING
	| ENET_CHGEVNT_NETWORK_TYPE
	| ENET_CHGEVNT_PROXY_SETTING);
	/* 対応するフラグのみ抽出、有効イベントとする */
	validFlag = eventFlag & supportFlag;
	DLOG_Debug("flag input,valid,port : 0x%02lX,0x%02lX,%lu\n", eventFlag, validFlag, port);
	if (validFlag == 0)
	{
		DLOG_Error("Err! There is no corresponding event[port] : 0x%02lX[%lu]\n", eventFlag, port);
		return (fncRet);
	}

	for (i = 0; i < PROCESS_MAX; i++)
	{
		/* 登録済み判定ループ */
		if (s_enet_sendChgEvntProc[i].port == port)
		{
			/* 登録済みポート検出 */
			DLOG_Debug("already registered : %d\n", port);
			if (ENET_SetPrefChgEvntFlag(port, validFlag))
			{
				/* 既存フラグ上書き保存 */
				DLOG_Debug("flag old->new : 0x%02lX->0x%02lX,\n", s_enet_sendChgEvntProc[i].flag, validFlag);
				/* 既存フラグ上書き */
				s_enet_sendChgEvntProc[i].flag = validFlag;
				fncRet = ENET_SUCCESS;
			}
			existPort = TRUE;
			/* 登録済み */
			fncRet = ENET_SUCCESS;
			/* 重複登録が発生してもフラグに不整合を起こさないよう全体走査 */
		}
	}
	if (!existPort)
	{
		/* 未登録なら追加 */
		for (i = 0; i < PROCESS_MAX; i++)
		{
			/* ポート追加用空き領域走査 */
			if (s_enet_sendChgEvntProc[i].port == 0)
			{
				if (ENET_SetPrefChgEvntFlag(port, validFlag))
				{
					DLOG_Debug("set port,flag : %d,0x%02lX\n", port, validFlag);
					s_enet_sendChgEvntProc[i].port = port;
					/* ポートを追加して */
					s_enet_sendChgEvntProc[i].flag = validFlag;
					/* フラグを設定 */
					fncRet = ENET_SUCCESS;
				}
			}
		}
	}
	if (fncRet == ENET_SUCCESS)
	{
		/* イベントの登録時通知 */
		enet_SendChgEvntLfwEvent(port, validFlag);
	}

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:設定ファイルから通知登録済みのプロセスを取得する		*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:無い													*/
/*--------------------------------------------------------------------------*/
/*	detail			:設定ファイルから通知登録済みのプロセスを取得する		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/16 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void enet_InitSendChgEvntProcArr(void)
{
	/* 登録済み確認するプロセスのポート番号(始端) */
	slong i = LFW_SPORT_MIN;
	/* プロセス配列のインデックス */
	slong j = 0;
	/* 登録済み確認するプロセスのポート番号(終端) */
	slong maxProcPort = (LFW_SPORT_MIN + PROCESS_MAX);
	ulong flag = 0;
	DEBUG_TRCIN();

	if (ENET_GetFileSize(ENET_FILE_NOTIFY_PROC) <= 0)
	{
		DLOG_Debug("file not found : %s\n", ENET_FILE_NOTIFY_PROC);
		DEBUG_TRCOUT();
	}
	else
	{
		for (i = LFW_SPORT_MIN; i < maxProcPort; i++)
		{
			/* プロセスのポート番号(始端～終端) */
			flag = ENET_GetPrefChgEvntFlag(i);
			if (flag != 0)
			{
				if (j < PROCESS_MAX)
				{
					DLOG_Debug("find port,flag : %d,0x%02lX\n", i, flag);
					s_enet_sendChgEvntProc[j].port = i;
					s_enet_sendChgEvntProc[j].flag = flag;
					j++;
				}
			}
		}
	}
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:指定されたデバイス名のデバイスが存在するか確認する		*/
/*--------------------------------------------------------------------------*/
/*	param			:schar*		ifaceName	デバイス名						*/
/*--------------------------------------------------------------------------*/
/*	return			:boo_t	処理結果										*/
/*						TRUE	デバイスの存在が確認された					*/
/*						FALSE	デバイスの存在が確認できなかった			*/
/*--------------------------------------------------------------------------*/
/*	detail			:指定されたデバイス名のデバイスが存在するか確認する		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
bool_t enet_GetDeviceExist(const schar* ifaceName)
{
	bool_t fncRet = FALSE;
	sint fd = 0;
	slong retIoctl = 0;
	struct ifreq req;
	DEBUG_TRCIN();
	
	/* 引数チェック */
	if(ifaceName == NULL)
	{
		DLOG_Error("Err! ifaceName is NULL.\n");
		return (fncRet);
	}

	/* 初期化 */
	memset(&req.ifr_name, 0x00, IFNAMSIZ);

#ifndef CROSSDEBUG
	fd = socket(AF_INET, SOCK_DGRAM, 0);
#endif
	if (fd < 0)
	{
		DLOG_Error("Err! socket is NULL.\n");
		return (fncRet);
	}
	strncpy(req.ifr_name, ifaceName, IFNAMSIZ-1);
	retIoctl = ioctl(fd, SIOCGIFFLAGS, &req);

	DLOG_Debug("ioctl(%s):%d\n",ifaceName, retIoctl);
	if (retIoctl < 0)
	{
		fncRet = FALSE;
	}
	else
	{
		fncRet = TRUE;
	}
	close(fd);
	DLOG_Debug("return(%s)\n",(fncRet==TRUE?"TRUE":"FALSE"));

	DEBUG_TRCOUT();
	return (fncRet);
}
/*==========================================================================*/
/*	Description		:デバイス名を設定する									*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetNetworkType_t type	ネットワーク種別				*/
/*					schar*	ifaceName				デバイス名				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*--------------------------------------------------------------------------*/
/*	detail			:デバイス名を設定する									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
schar* enet_GetDeviceName(EnetNetworkType_t type)
{
	/* ローカル変数宣言 */
	schar* ifaceName = NULL;
	DEBUG_TRCIN();
	
	/* ネットワークタイプ範囲判定 */
	if((type < ENET_NETWORK_NONE)
	|| (ENET_NETWORK_TYPE_MAX <= type) )
	{
		type = ENET_NETWORK_NONE;	/* 範囲外だった場合、未設定にする */
	}

	/* ネットワーク種別が"無線"の場合 */
	if(type == ENET_NETWORK_WIRELESS)
	{
		/* 排他ロック開始 */
		
		/* 最後に設定されたデバイス番号のチェック */
		if ((s_enet_CurrentIfNo < ENET_NETWORK_WIRELESS)
		||  ((ENET_NETWORK_WIRELESS + ENET_WLAN_OFS_MAX) <= s_enet_CurrentIfNo))
		{
			/* 無線デバイス以外の場合、未設定にする */
			DLOG_Debug("not wirless s_enet_CurrentIfNo : %d\n", s_enet_CurrentIfNo);
			s_enet_CurrentIfNo = ENET_NETWORK_NONE;
		}

		/* 設定されたデバイス名のデバイスが存在しない場合 */
		if(enet_GetDeviceExist(s_enet_interfaceName[s_enet_CurrentIfNo]) == FALSE)
		{
			/* 存在する無線デバイスを検索、番号を設定 */
			if(enet_GetDeviceExist(ENET_DEVICE_NAME_RA0) == TRUE)
			{
				s_enet_CurrentIfNo	= ENET_NETWORK_WIRELESS + ENET_WLAN_OFS_RA0;
			}
			else if(enet_GetDeviceExist(ENET_DEVICE_NAME_WLAN0) == TRUE)
			{
				s_enet_CurrentIfNo	= ENET_NETWORK_WIRELESS + ENET_WLAN_OFS_WLAN0;
			}
			else
			{
				/* DO NOTHING */
			}
		}

		/* デバイス名称を設定 */
		DLOG_Debug("set s_enet_CurrentIfNo : %d\n", s_enet_CurrentIfNo);
		ifaceName = s_enet_interfaceName[s_enet_CurrentIfNo];
		/* 排他ロック終了 */
		
	}
	else
	{
		/* ネットワーク種別が"未設定"または"有線"の場合 */
		ifaceName = s_enet_interfaceName[type];
	}
	DLOG_Debug("type, ifaceName : %d, %s\n", type, ifaceName);

	DEBUG_TRCOUT();
	return ifaceName;
}
/*==========================================================================*/
/*	Description		:														*/
/*				設定ファイルからポート番号に対応するイベントフラグを削除する*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong port	ポート番号									*/
/*					ulong flag	イベントフラグ								*/
/*--------------------------------------------------------------------------*/
/*	return			:boo_t	処理結果										*/
/*						TRUE	正常	FALSE	異常						*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*				設定ファイルからポート番号に対応するイベントフラグを削除する*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
bool_t enet_RmvPrefChgEvntFlag(ulong port)
{
	bool_t fncRet = FALSE;
	schar bufKey[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	/* 排他ロック開始 */
	
	snprintf(bufKey, ENET_BUF_STR_LEN, "%lu", port);
	/* ポート番号=""で記録 */
	if (SET_PREF(bufKey, "", ENET_FILE_NOTIFY_PROC))
	{
		DLOG_Debug("rmv port : %s\n", bufKey);
		fncRet = TRUE;
	}
	else
	{
		DLOG_Error("Err! rmv : %s\n", bufKey);
	}
	/* 排他ロック解除 */
	

	DEBUG_TRCOUT();
	return (fncRet);
}



/*End Of File*/
