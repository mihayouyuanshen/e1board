/****************************************************************************/
/*	Description		:EENLの共通処理											*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC DX.zhao											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/30												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、EENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:無し													*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/
#include "eenl_cmn_g.h"
#include "eenl_common.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/

/***********************************************************
* @def		g_nodeProp
* @brief	ノードプロファイルで利用するプロパティのリスト
************************************************************/
uchar g_nodeProp[14] = {
	0x80u, 0x82u, 0x83u, 0x88u, 0x8Au, 0x9Du, 0x9Eu, 0x9Fu, 0xD3u, 0xD4u,
	0xD5u, 0xD6u, 0xD7u, 0x00u
};

/***********************************************************
* @def		g_devicePropSolarNccN
* @brief	家庭用太陽光デバイスで利用するプロパティのリスト（ニチコン用）
************************************************************/
uchar g_devicePropSolarNccN[33] = {
	0x80u, 0x81u, 0x82u, 0x83u, 0x86u, 0x88u, 0x8Au, 0x8Cu, 0x8Du, 0x97u,
	0x98u, 0x9Du, 0x9Eu, 0x9Fu, 0xA0u, 0xA2u, 0xB0u, 0xB1u,	0xB2u, 0xB3u,
	0xB4u, 0xC0u, 0xC1u, 0xC2u, 0xC3u, 0xD0u, 0xD1u, 0xE0u,	0xE1u, 0xE3u, 
	0xE8u, 0xF5u, 0x00u
};

/***********************************************************
* @def		g_devicePropSolarExtNccN
* @brief	家庭用太陽光デバイスで利用するユーザ定義プロパティ（ニチコン用）
************************************************************/
ENL_PROPERTY_DEFINE g_devicePropSolarExtNccN[2] = {
	{0xF5u,	ENL_TYPE_SIGNED_LONG ,	16u,		0u,	0xFFFFFFFFu,
		(ENL_ENABLE_ACCESS_RULE_GET)},
	{0x00u,	0x00u				   ,	0u,		0u,		  0x00u,	0x00u },
};

/***********************************************************
* @def		g_devicePropBatteryNccN
* @brief	蓄電池デバイスで利用するプロパティのリスト(ニチコン向け)
************************************************************/
uchar g_devicePropBatteryNccN[52] = {
	0x80u, 0x81u, 0x82u, 0x83u, 0x86u, 0x88u, 0x8Au, 0x8Cu, 0x8Du, 0x97u,
	0x98u, 0x9Du, 0x9Eu, 0x9Fu, 0xA0u, 0xA1u, 0xA2u, 0xA3u, 0xA4u, 0xA5u,
	0xA8u, 0xA9u, 0xAAu, 0xABu, 0xC1u, 0xC2u, 0xC8u, 0xC9u, 0xCFu, 0xD0u,
	0xD2u, 0xD3u, 0xD6u, 0xD8u, 0xDAu, 0xDBu, 0xDCu, 0xDDu, 0xE2u, 0xE4u,
	0xE5u, 0xE6u, 0xE7u, 0xE8u,	0xEBu, 0xECu, 0xF0u, 0xF2u, 0xF4u, 0xF5u,
	0xF8u, 0x00u
};


/***********************************************************
* @def		g_devicePropBatteryExtN
* @brief	蓄電池デバイスで利用するユーザ定義プロパティ(ニチコン向け押し上げ無し用)
************************************************************/
ENL_PROPERTY_DEFINE g_devicePropBatteryExtN[6] = {
	{0xF0u,	ENL_TYPE_UNSIGNED_CHAR,	1u,	0x00u,	0x08u,
		(ENL_ENABLE_ACCESS_RULE_GET | ENL_ENABLE_STATUS_CHANGE_ANNO)},
	{0xF2u,	ENL_TYPE_UNSIGNED_CHAR,	1u,	0x00u,	100u,
		(ENL_ENABLE_ACCESS_RULE_SET | ENL_ENABLE_ACCESS_RULE_GET
		| ENL_ENABLE_STATUS_CHANGE_ANNO)},
	{0xF4u,	ENL_TYPE_OTHER,			21u,	0x00u,		0xFFu,
		(ENL_ENABLE_ACCESS_RULE_SET | ENL_ENABLE_ACCESS_RULE_GET
		| ENL_ENABLE_STATUS_CHANGE_ANNO)},
	{0xF5u,	ENL_TYPE_UNSIGNED_LONG,	16u,	0x00u,	0xFFFFFFFFu,
	(ENL_ENABLE_ACCESS_RULE_GET)},
	{0xF8u,	ENL_TYPE_OTHER,			9u,	0x00u,		0xFFu,
		(ENL_ENABLE_ACCESS_RULE_SET | ENL_ENABLE_ACCESS_RULE_GET
		| ENL_ENABLE_STATUS_CHANGE_ANNO)},
	{0x00u,	0x00u				  , 0u,	0x00u,		0x00u,	0x00u },
};

/***********************************************************
* @def		g_devicePropV2hNccN
* @brief	車両充放電器デバイスで利用するプロパティのリスト（ニチコン案件ニチコン用）
************************************************************/
uchar g_devicePropV2hNccN[50] = {
	0x80u, 0x81u, 0x82u, 0x83u, 0x86u, 0x88u, 0x8Au, 0x8Cu, 0x8Du, 0x97u,
	0x98u, 0x9Du, 0x9Eu, 0x9Fu, 0xC0u, 0xC2u, 0xC4u, 0xC5u, 0xC6u, 0xC7u, 
	0xC8u, 0xC9u, 0xCAu, 0xCBu, 0xCCu, 0xCDu, 0xCEu, 0xCFu, 0xD0u, 0xD2u,
	0xD3u, 0xD6u, 0xD7u, 0xD8u, 0xD9u, 0xDAu, 0xDBu, 0xDCu, 0xDDu, 0xE1u,
	0xE2u, 0xE4u, 0xE6u, 0xE7u, 0xEAu, 0xEBu, 0xECu, 0xF5u, 0xF8u, 0x00u
};

/***********************************************************
* @def		g_devicePropV2hExtN
* @brief	車両充放電器デバイスで利用するユーザ定義プロパティ(ニチコン案件ニチコン用)
************************************************************/
ENL_PROPERTY_DEFINE g_devicePropV2hExtN[3] = {
	{0xF5u,	ENL_TYPE_UNSIGNED_LONG,	16u,	0x00u,	0xFFFFFFFFu,
		(ENL_ENABLE_ACCESS_RULE_GET)},
	{0xF8u,	ENL_TYPE_OTHER,			9u,	0x00u,		0xFFu,
		(ENL_ENABLE_ACCESS_RULE_SET | ENL_ENABLE_ACCESS_RULE_GET
		| ENL_ENABLE_STATUS_CHANGE_ANNO)},
	{0x00u,	0x00u				  , 0u,	0x00u,	0x00u,	0x00u },
};

/***********************************************************
* @def		g_devicePropMpcs
* @brief	マルチ入力PCSデバイスで利用するプロパティのリスト
************************************************************/
uchar g_devicePropMpcs[22] = {
	0x80u, 0x81u, 0x82u, 0x83u, 0x86u, 0x88u, 0x8Au, 0x8Cu, 0x8Du, 0x97u,
	0x98u, 0x9Du, 0x9Eu, 0x9Fu, 0xD0u, 0xE0u, 0xE3u, 0xE7u, 0xE8u, 0xF5u,
	0xF6u, 0x00u
};

/***********************************************************
* @def		g_devicePropMpcsExt
* @brief	マルチ入力PCSデバイスで利用するユーザ定義プロパティ
************************************************************/
ENL_PROPERTY_DEFINE g_devicePropMpcsExt[3] = {
	{0xF5u,	ENL_TYPE_SIGNED_LONG,	16u,	0x00u,	0xFFFFFFFFu,
		(ENL_ENABLE_ACCESS_RULE_GET)},
	{0xF6u,	ENL_TYPE_UNSIGNED_LONG,	32u,	0x00u,	0xFFFFFFFFu,
		(ENL_ENABLE_ACCESS_RULE_GET)},
	{0x00u,	0x00u				  , 0u,	0x00u,	0x00u,	0x00u },
};

/**********************************************************************
* @def		g_devicePropMeter
* @brief	分散型電源電力量メータデバイスで利用するプロパティのリスト
***********************************************************************/
uchar g_devicePropMeter[29] = { 
	0x80u, 0x81u, 0x82u, 0x83u, 0x86u, 0x88u, 0x8Au, 0x8Cu, 0x8Du, 0x97u,
	0x98u, 0x9Du, 0x9Eu, 0x9Fu, 0xD0u, 0xD1u, 0xD2u, 0xD3u, 0xD4u, 0xD5u,
	0xDAu, 0xDBu, 0xE0u, 0xE1u, 0xE2u, 0xE3u, 0xE6u, 0xE7u, 0x00u
};


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
sem_t s_eenlSem;									/* スレッド制御用 */
sint s_eenlFdEvt[EENL_NUM_FD_EVT] = { 0 };			/* descriptor */
sint s_eenlFdQuit[EENL_NUM_FD_EVT] = { 0 };			/* descriptor */
EenlDeviceInfo_t* s_eenlInfoHead = NULL;			/* device情報 */

struct sockaddr_in s_eenlBcAddr;					/* IPv4マルチキャストアドレス */
struct sockaddr_in6	s_eenlBcAddr6;					/* IPv6マルチキャストアドレス */

void* s_eenlSelectedInstance = NULL;				/* インスタンスハンドル */
ENL_MESSAGE* s_eenlLatestRequest = NULL;			/* 最近のリクエストのハンドル */

pthread_t s_eenlThSend;								/* 送信スレッド */
pthread_t s_eenlThRecv;								/* UDP 受信スレッド */
pthread_t s_eenlThRecv6;							/* UDP 受信スレッド IPv6 */
pthread_t s_eenlThRecvTcp;							/* TCP受信スレッド(IPv4/IPv6) */
pthread_t s_eenlThMcast;							/* マルチキャストスレッド */
slong s_eenlLoopOut = 0;							/* ストスレッド終了フラグ(0:終了しない、1:終了する) */

sint s_eenlRv4 = -1;								/* socket descriptor */
sint s_eenlSv4 = -1;								/* socket descriptor */
sint s_eenlRv6 = -1;								/* socket descriptor */
sint s_eenlSv6 = -1;								/* socket descriptor */

bool_t	s_eenlNodEpcValidFlg[0xFF + 1] = { 0 };		/* EPC設定済フラグ(ノード) EPCは0x00～0xFFまでの最大256個 */
bool_t	s_eenlBatEpcValidFlg[0xFF + 1] = { 0 };		/* EPC設定済フラグ(蓄電池) */
bool_t	s_eenlSolEpcValidFlg[0xFF + 1] = { 0 };		/* EPC設定済フラグ(太陽光) */
bool_t	s_eenlV2hEpcValidFlg[0xFF + 1] = { 0 };		/* EPC設定済フラグ(車両充放電器) */
bool_t	s_eenlMpcsEpcValidFlg[0xFF + 1] = { 0 };	/* EPC設定済フラグ(マルチ入力PCS) */
bool_t	s_eenlReqSetEnable = FALSE;					/* Set要求受付可能フラグ */



/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description		:デバイスの登録											*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　new_info	デバイス情報							*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/30 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
void EENL_RegisterDeviceInfo(EenlDeviceInfo_t* new_info)
{
	DEBUG_TRCIN();

	EenlDeviceInfo_t** info = NULL;
	bool_t isLoopEnd = FALSE;

	info = &s_eenlInfoHead;

	while (*info != NULL && !isLoopEnd) {
		if (*info == new_info) {
			isLoopEnd = TRUE;
		} else {
			info = &(*info)->next;
		}
	}

	if (*info == NULL) {
		*info = new_info;
		new_info->next = NULL;
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:デバイスリストの選択									*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　idx	インデックス番号							*/
/*--------------------------------------------------------------------------*/
/*	return			:EenlDeviceInfo_t デバイス情報							*/
/*					 (NULL:デバイス情報無し/非NULL:デバイス情報あり			*/
/*--------------------------------------------------------------------------*/
/*	detail			:インデックス番号によりデバイス情報を検索する			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/30 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
EenlDeviceInfo_t* EENL_GetDeviceInfoIndex(slong idx)
{
	EenlDeviceInfo_t* info = NULL;

	DEBUG_TRCIN();

	info = s_eenlInfoHead;

	while (info != NULL && idx-- > 0) {
		info = info->next;
	}

	DEBUG_TRCOUT();
	return info;
}

/*==========================================================================*/
/*	Description		:デバイスリストの選択（インスタンスハンドル）			*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　instance	インスタンスハンドル					*/
/*--------------------------------------------------------------------------*/
/*	return			:EenlDeviceInfo_t デバイス情報							*/
/*					 (NULL:デバイス情報無し/非NULL:デバイス情報あり			*/
/*--------------------------------------------------------------------------*/
/*	detail			:インスタンスハンドルによりデバイス情報を検索する		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/30 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
EenlDeviceInfo_t* EENL_GetDeviceInfoInstance(void* instance)
{
	EenlDeviceInfo_t* info = NULL;
	bool_t isLoopEnd = FALSE;

	DEBUG_TRCIN();

	info = s_eenlInfoHead;

	while (info != NULL && !isLoopEnd) {
		if (info->node == instance) {
			isLoopEnd = TRUE;
		} else if (info->devBattery == instance) {
			isLoopEnd = TRUE;
		} else if (info->devSolar == instance) {
			isLoopEnd = TRUE;
		} else if (info->devV2h == instance) {
			isLoopEnd = TRUE;
		} else if (info->devMpcs == instance) {
			isLoopEnd = TRUE;
		} else {
			info = info->next;
		}
	}

	DEBUG_TRCOUT();
	return info;
}

/*==========================================================================*/
/*	Description		:全デバイスの登録解除									*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/30 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
void EENL_DisposeAllDevices(void)
{
	EenlDeviceInfo_t* info = NULL;
	EenlDeviceInfo_t* next = NULL;

	DEBUG_TRCIN();

	info = s_eenlInfoHead;

	while (info != NULL) {
		next = info->next;
		eenl_DisposeDevice(info);
		info = next;
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:マルチキャストへの登録									*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
void EENL_AddMulticast(void)
{
	struct addrinfo hints, * res = NULL;
	struct group_req greq;
	slong ret = 0;
	uchar enetIf = 0u;
	slong err = 0;
	schar wlanName[ENET_WLAN_IF_NAME_STR_LEN] = { 0 };

	DEBUG_TRCIN();
	DLOG_Debug("----- EENL_AddMulticast start -----");

	memset(wlanName, 0, sizeof(wlanName));

	err = ENET_SyncGetWlanIfName(wlanName);
	if (err == ENET_SUCCESS) {
		DLOG_Debug("get wlan name : %s \r\n", wlanName);
	} else {
		DLOG_Error("get wlan name ERROR : %ld[%s]", err, wlanName);
	}

	enetIf = ENET_SyncGetNetworkType();
	DLOG_Debug(" network Type: %d", enetIf);
	DLOG_Debug("ifname eth0 : %d", if_nametoindex("eth0"));
	DLOG_Debug("ifname wlan : %d", if_nametoindex(wlanName));

	/* for IPv4 Multicast */
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	err = getaddrinfo(EENL_MULTICAST_ADDRESS, "3610", &hints, &res);
	if (err == 0) {
		/* 正常 */

		if (enetIf == ENET_NETWORK_WIRED) {
			/* 有線の場合 */
			greq.gr_interface = if_nametoindex("eth0");
			memcpy(&greq.gr_group, res->ai_addr, res->ai_addrlen);

			/* マルチキャストからの脱退 */
			ret = setsockopt(s_eenlRv4, IPPROTO_IP, MCAST_LEAVE_GROUP,
				(schar*)&greq, sizeof(greq));
			if (ret == 0) {
				DLOG_Debug("ipv4 multicast(eth0) leave(%d): %ld",
					greq.gr_interface, ret);
			} else {
				DLOG_Debug("ipv4 multicast(eth0) leave : %d", errno);
			}

			/* マルチキャストへの参加 */
			ret = setsockopt(s_eenlRv4, IPPROTO_IP, MCAST_JOIN_GROUP,
				(schar*)&greq, sizeof(greq));
			if (ret == 0) {
				DLOG_Debug("ipv4 multicast(eth0) join(%d): %ld",
					greq.gr_interface, ret);
			} else {
				DLOG_Error("ipv4 multicast(eth0) join error : %d", errno);
			}

		} else if (enetIf == ENET_NETWORK_WIRELESS) {
			/* 無線の場合 */
			greq.gr_interface = if_nametoindex(wlanName);
			memcpy(&greq.gr_group, res->ai_addr, res->ai_addrlen);

			/* マルチキャストからの脱退 */
			ret = setsockopt(s_eenlRv4, IPPROTO_IP, MCAST_LEAVE_GROUP,
				(schar*)&greq, sizeof(greq));
			if (ret == 0) {
				DLOG_Debug("ipv4 multicast(wlan) leave(%d): %ld",
					greq.gr_interface, ret);
			} else {
				DLOG_Debug("ipv4 multicast(wlan) leave : %d", errno);
			}

			/* マルチキャストへの参加 */
			ret = setsockopt(s_eenlRv4, IPPROTO_IP, MCAST_JOIN_GROUP,
				(schar*)&greq, sizeof(greq));
			if (ret == 0) {
				DLOG_Debug("ipv4 multicast(wlan) join(%d): %ld",
					greq.gr_interface, ret);
			} else {
				DLOG_Error("ipv4 multicast(wlan) join error : %d", errno);
			}

		} else {
			/* 有線、無線以外の場合、何もしない */

		}

	} else {
		/* error */
		DLOG_Error("ipv4 getaddr error!: %ld", err);
	}

	freeaddrinfo(res);

	/* for IPv6 Multicast */
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;

	err = getaddrinfo(EENL_MULTICAST_ADDRESS_V6, "3610", &hints, &res);
	if (err == 0) {
		/* 正常 */

		if (enetIf == ENET_NETWORK_WIRED) {
			/* 有線の場合 */
			greq.gr_interface = if_nametoindex("eth0");
			memcpy(&greq.gr_group, res->ai_addr, res->ai_addrlen);

			/* マルチキャストからの脱退 */
			ret = setsockopt(s_eenlRv6, IPPROTO_IPV6, MCAST_LEAVE_GROUP,
				(schar*)&greq, sizeof(greq));
			if (ret == 0) {
				DLOG_Debug("ipv6 multicast(eth0) leave(%d): %ld",
					greq.gr_interface, ret);
			} else {
				DLOG_Debug("ipv6 multicast(eth0) leave : %d", errno);
			}

			/* マルチキャストへの参加 */
			ret = setsockopt(s_eenlRv6, IPPROTO_IPV6, MCAST_JOIN_GROUP,
				(schar*)&greq, sizeof(greq));
			if (ret == 0) {
				DLOG_Debug("ipv6 multicast(eth0) join(%d): %ld",
					greq.gr_interface, ret);
			} else {
				DLOG_Error("ipv6 multicast(eth0) join error : %d", errno);
			}

		} else if (enetIf == ENET_NETWORK_WIRELESS) {
			/* 無線の場合 */
			greq.gr_interface = if_nametoindex(wlanName);
			memcpy(&greq.gr_group, res->ai_addr, res->ai_addrlen);

			/* マルチキャストからの脱退 */
			ret = setsockopt(s_eenlRv6, IPPROTO_IPV6, MCAST_LEAVE_GROUP,
				(schar*)&greq, sizeof(greq));
			if (ret == 0) {
				DLOG_Debug("ipv6 multicast(wlan) leave(%d): %ld",
					greq.gr_interface, ret);
			} else {
				DLOG_Debug("ipv6 multicast(wlan) leave : %d", errno);
			}

			/* マルチキャストへの参加 */
			ret = setsockopt(s_eenlRv6, IPPROTO_IPV6, MCAST_JOIN_GROUP,
				(schar*)&greq, sizeof(greq));
			if (ret == 0) {
				DLOG_Debug("ipv6 multicast(wlan) join(%d): %ld",
					greq.gr_interface, ret);
			} else {
				DLOG_Error("ipv6 multicast(wlan) join error : %d", errno);
			}

		} else {
			/* 有線、無線以外の場合、何もしない */

		}

	} else {
		/* error */
		DLOG_Error("ipv6 getaddr error!: %ld", err);
	}

	freeaddrinfo(res);

	sleep(1);

#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
	system("cat /proc/net/igmp");
	system("cat /proc/net/igmp6");
#endif	/* EN_TRC_PRINTF */

	DLOG_Debug("----- EENL_AddMulticast end -----");
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:通信スレッドの死活チェック								*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/0以外:異常終了)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_ThreadCheck(void)
{
	slong ret = 0;

	DEBUG_TRCIN();

	ret = pthread_kill(s_eenlThSend, 0);
	if (ret == 0) {
		/* スレッドが正常動作している場合 */
		/* 何もしない */
	} else {
		/* 異常の場合 */
		pthread_join(s_eenlThSend, NULL);
		/* 送信スレッド */
		pthread_create(&s_eenlThSend, NULL,
			(void* (*)(void*))eenl_ThreadSend, NULL);
		ret = ret - 1;
	}

	ret = pthread_kill(s_eenlThRecv, 0);
	if (ret == 0) {
		/* スレッドが正常動作している場合 */
		/* 何もしない */
	} else {
		/* 異常の場合 */
		pthread_join(s_eenlThRecv, NULL);
		/* UDP 受信スレッド */
		pthread_create(&s_eenlThRecv, NULL,
			(void* (*)(void*))eenl_ThreadRecv, NULL);
		ret = ret - 2;
	}

	ret = pthread_kill(s_eenlThRecv6, 0);
	if (ret == 0) {
		/* スレッドが正常動作している場合 */
		/* 何もしない */
	} else {
		/* 異常の場合 */
		pthread_join(s_eenlThRecv6, NULL);
		/* UDP 受信スレッド IPv6 */
		pthread_create(&s_eenlThRecv6, NULL,
			(void* (*)(void*))eenl_ThreadRecv6, NULL);
		ret = ret - 4;
	}

	ret = pthread_kill(s_eenlThRecvTcp, 0);
	if (ret == 0) {
		/* スレッドが正常動作している場合 */
		/* 何もしない */
	} else {
		/* 異常の場合 */
		pthread_join(s_eenlThRecvTcp, NULL);
		/* TCP受信スレッド(IPv4/IPv6) */
		pthread_create(&s_eenlThRecvTcp, NULL,
			(void* (*)(void*))eenl_ThreadTcpRecv, NULL);
		ret = ret - 16;

	}

	ret = pthread_kill(s_eenlThMcast, 0);
	if (ret == 0) {
		/* スレッドが正常動作している場合 */
		/* 何もしない */
	} else {
		/* 異常の場合 */
		pthread_join(s_eenlThMcast, NULL);
		/* マルチキャスト設定 */
		pthread_create(&s_eenlThMcast, NULL,
			(void* (*)(void*))eenl_ThreadMulticast, NULL);
		ret = ret - 32;
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:初期化処理												*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
void EENL_Initialize(void)
{
	s_eenlInfoHead = NULL;
	s_eenlSelectedInstance = NULL;
	s_eenlLatestRequest = NULL;
	s_eenlLoopOut = 0;

	DEBUG_TRCIN();

	s_eenlBcAddr.sin_family = AF_INET;
	s_eenlBcAddr.sin_addr.s_addr = inet_addr(EENL_MULTICAST_ADDRESS);
	s_eenlBcAddr.sin_port = htons(EENL_UDP_PORT);

	s_eenlBcAddr6.sin6_family = AF_INET6;
	s_eenlBcAddr6.sin6_port = htons(EENL_UDP_PORT);
	inet_pton(AF_INET6, EENL_MULTICAST_ADDRESS_V6, &s_eenlBcAddr6.sin6_addr);

	sem_init(&s_eenlSem, 0, 1);


#if defined(USE_EVENTFD) && USE_EVENTFD != 0
	s_eenlFdEvt[0] = eventfd(0, 0);
	s_eenlFdQuit[0] = eventfd(0, 0);
#else
	pipe(s_eenlFdEvt);
	pipe(s_eenlFdQuit);
#endif


	/* 送信スレッド */
	pthread_create(&s_eenlThSend, NULL,
		(void* (*)(void*))eenl_ThreadSend, NULL);
	/* UDP受信スレッド IPv6 */
	pthread_create(&s_eenlThRecv6, NULL,
		(void* (*)(void*))eenl_ThreadRecv6, NULL);
	/* UDP受信スレッド */
	pthread_create(&s_eenlThRecv, NULL,
		(void* (*)(void*))eenl_ThreadRecv, NULL);
	/* TCP受信スレッド(IPv4/IPv6) */
	pthread_create(&s_eenlThRecvTcp, NULL,
		(void* (*)(void*))eenl_ThreadTcpRecv, NULL);
	/* マルチキャスト設定 */
	pthread_create(&s_eenlThMcast, NULL,
		(void* (*)(void*))eenl_ThreadMulticast, NULL);

	ENL_initialize();
	ENL_set_message_length(EENL_MAX_MESSAGE_LENGTH);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:終了化処理												*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
void EENL_Terminate(void)
{
	u64 val = 0u;

	DEBUG_TRCIN();

	s_eenlLoopOut = 1;
	write(s_eenlFdQuit[EENL_IDX_FD_EVT_WRITE], &val, sizeof(u64));

	pthread_join(s_eenlThSend, NULL);
	pthread_join(s_eenlThRecv, NULL);
	pthread_join(s_eenlThRecv6, NULL);
	pthread_join(s_eenlThRecvTcp, NULL);
	pthread_join(s_eenlThMcast, NULL);

	ENL_terminate();

#if defined(USE_EVENTFD) && USE_EVENTFD != 0
	close(s_eenlFdEvt[0]);
	close(s_eenlFdQuit[0]);
#else
	close(s_eenlFdEvt[0]);
	close(s_eenlFdEvt[1]);

	close(s_eenlFdQuit[0]);
	close(s_eenlFdQuit[1]);
#endif



	sem_destroy(&s_eenlSem);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:wait event処理											*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info	蓄電池DEVICE情報							*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
void EENL_EventWait(EenlDeviceInfo_t* info)
{
	DEBUG_TRCIN();

	pthread_mutex_lock(&info->mutex);

	while (info->event == EENL_EVENT_NONE) {
		pthread_cond_wait(&info->cond, &info->mutex);
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:イベント設定処理										*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*					:[in]　event	イベント								*/
/*					:[in]　epc		ECHONET Liteプロパティ					*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
void EENL_EventSet(EenlDeviceInfo_t* info, slong event, uchar epc)
{
	DEBUG_TRCIN();

	pthread_mutex_lock(&info->mutex);

	while (info->event != EENL_EVENT_NONE) {
		pthread_cond_wait(&info->cond, &info->mutex);
	}

	info->event = event;
	info->epc = epc;

	pthread_cond_broadcast(&info->cond);

	pthread_mutex_unlock(&info->mutex);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:イベントクリア処理										*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　info		蓄電池DEVICE情報						*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
void EENL_EvenClear(EenlDeviceInfo_t* info)
{
	DEBUG_TRCIN();

	info->event = EENL_EVENT_NONE;

	pthread_cond_broadcast(&info->cond);

	pthread_mutex_unlock(&info->mutex);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:メッセージ送信処理										*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　addr		アドレス								*/
/*					:[in]　msg		メッセージ								*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
void EENL_SendMessage(EenlAddr_t* addr, ENL_MESSAGE* msg)
{
	u64 val = 0u;

	DEBUG_TRCIN();

	sem_wait(&s_eenlSem);
	if (msg != NULL) {
		DLOG_Debug("EENL_SendMessage ESV:%02x , EPC::%02x",
			msg->data[10], msg->data[12]);
		val = (u64)(ulong)addr << 32 | (ulong)msg;
		write(s_eenlFdEvt[EENL_IDX_FD_EVT_WRITE], &val, sizeof(u64));
	} else {
		sem_post(&s_eenlSem);
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:最近のリクエストのハンドル取得							*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:メッセージ構造体										*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
ENL_MESSAGE* EENL_GetLatestRequest(void)
{
	DEBUG_TRCIN();
	DEBUG_TRCOUT();
	return s_eenlLatestRequest;
}

/*==========================================================================*/
/*	Description		:最近のリクエストのハンドル取得(callback function)		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　instance		インスタンスハンドル				*/
/*					:[in]　message		対象リクエストメッセージハンドル	*/
/*					:[in]　result		応答内容							*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
void ENL_callback_response(void* instance, ENL_MESSAGE* message, slong result)
{
	EenlDeviceInfo_t* info;
	DEBUG_TRCIN();

	DLOG_Debug("instance = 0x%08lx, message = 0x%08lx, result = %ld",
		(ulong)instance, (ulong)message, result);

	if (message == EENL_GetLatestRequest()) {
		info = EENL_GetDeviceInfoInstance(instance);
		if (info != NULL) {
			if (result == ENL_RESULT_ACCEPT) {
				EENL_EventSet(info, EENL_EVENT_ACCEPTED, 0);
			} else {
				EENL_EventSet(info, EENL_EVENT_REJECTED, 0);
			}
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:充放電時刻の有効値判定を行う							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　hourMinute	充放電開始停止時刻構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:有効値/1:無効値)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_ChargeDischargeTimeZoneCheck(EenlHourMinute_t* hourMinute)
{
	DEBUG_TRCIN();

	DLOG_Debug("chargetimeZone %02d:%02d-%02d:%02d", hourMinute->cStartHH,
		hourMinute->cStartMM, hourMinute->cStopHH, hourMinute->cStopMM);
	DLOG_Debug("dischargetimeZone %02d:%02d-%02d:%02d", hourMinute->dStartHH,
		hourMinute->dStartMM, hourMinute->dStopHH, hourMinute->dStopMM);

	if (
		((hourMinute->cStartHH > EENL_LIMIT_23)) ||
		((hourMinute->cStartMM > EENL_LIMIT_59)) ||
		((hourMinute->cStopHH > EENL_LIMIT_23)) ||
		((hourMinute->cStopMM > EENL_LIMIT_59)) ||
		((hourMinute->dStartHH > EENL_LIMIT_23)) ||
		((hourMinute->dStartMM > EENL_LIMIT_59)) ||
		((hourMinute->dStopHH > EENL_LIMIT_23)) ||
		((hourMinute->dStopMM > EENL_LIMIT_59))
		)
	{
		/*  パラメータ異常 */
		return 1;
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
	return 0;
}

/*==========================================================================*/
/*	Description		:EPC設定済フラグ初期化									*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
void EENL_InitEpcValidFlg(void)
{
	DEBUG_TRCIN();

	/* ノードプロファイル */
	memset(&s_eenlNodEpcValidFlg, FALSE, sizeof(s_eenlNodEpcValidFlg));
	/*  ENLライブラリで自動生成されるプロパティはデフォルトでGet可としておく */

	/*  規格Version情報 */
	s_eenlNodEpcValidFlg[0x82u] = TRUE;
	/*  状変アナウンスプロパティマップ */
	s_eenlNodEpcValidFlg[0x9Du] = TRUE;
	/*  Setプロパティマップ */
	s_eenlNodEpcValidFlg[0x9Eu] = TRUE;
	/*  Getプロパティマップ */
	s_eenlNodEpcValidFlg[0x9Fu] = TRUE;
	/*  自ノードインスタンス数 */
	s_eenlNodEpcValidFlg[0xD3u] = TRUE;
	/*  自ノードクラス数 */
	s_eenlNodEpcValidFlg[0xD4u] = TRUE;
	/*  インスタンスリスト通知 M1 B_0319対策 */
	s_eenlNodEpcValidFlg[0xD5u] = TRUE;
	/*  自ノードインスタンスリストS */
	s_eenlNodEpcValidFlg[0xD6u] = TRUE;
	/*  自ノードクラスリストS */
	s_eenlNodEpcValidFlg[0xD7u] = TRUE;

	/* 蓄電池クラス	*/
	memset(&s_eenlBatEpcValidFlg, FALSE, sizeof(s_eenlBatEpcValidFlg));
	/*  ENLライブラリで自動生成されるプロパティはデフォルトでGet可としておく */
	/*  規格Version情報 */
	s_eenlBatEpcValidFlg[0x82u] = TRUE;
	/*  状変アナウンスプロパティマップ */
	s_eenlBatEpcValidFlg[0x9Du] = TRUE;
	/*  Setプロパティマップ */
	s_eenlBatEpcValidFlg[0x9Eu] = TRUE;
	/*  Getプロパティマップ */
	s_eenlBatEpcValidFlg[0x9Fu] = TRUE;

	/* 太陽光クラス	*/
	memset(&s_eenlSolEpcValidFlg, FALSE, sizeof(s_eenlSolEpcValidFlg));
	/*  ENLライブラリで自動生成されるプロパティはデフォルトでGet可としておく */
	/*  規格Version情報 */
	s_eenlSolEpcValidFlg[0x82u] = TRUE;
	/*  状変アナウンスプロパティマップ */
	s_eenlSolEpcValidFlg[0x9Du] = TRUE;
	/*  Setプロパティマップ */
	s_eenlSolEpcValidFlg[0x9Eu] = TRUE;
	/*  Getプロパティマップ */
	s_eenlSolEpcValidFlg[0x9Fu] = TRUE;

	/* 車両充放電器クラス	*/
	memset(&s_eenlV2hEpcValidFlg, FALSE, sizeof(s_eenlV2hEpcValidFlg));
	/*  ENLライブラリで自動生成されるプロパティはデフォルトでGet可としておく */
	/*  規格Version情報 */
	s_eenlV2hEpcValidFlg[0x82u] = TRUE;
	/*  状変アナウンスプロパティマップ */
	s_eenlV2hEpcValidFlg[0x9Du] = TRUE;
	/*  Setプロパティマップ */
	s_eenlV2hEpcValidFlg[0x9Eu] = TRUE;
	/*  Getプロパティマップ */
	s_eenlV2hEpcValidFlg[0x9Fu] = TRUE;

	/* マルチ入力PCSクラス	*/
	memset(&s_eenlMpcsEpcValidFlg, FALSE, sizeof(s_eenlMpcsEpcValidFlg));
	/*  ENLライブラリで自動生成されるプロパティはデフォルトでGet可としておく */
	/*  規格Version情報 */
	s_eenlMpcsEpcValidFlg[0x82u] = TRUE;
	/*  状変アナウンスプロパティマップ */
	s_eenlMpcsEpcValidFlg[0x9Du] = TRUE;
	/*  Setプロパティマップ */
	s_eenlMpcsEpcValidFlg[0x9Eu] = TRUE;
	/*  Getプロパティマップ */
	s_eenlMpcsEpcValidFlg[0x9Fu] = TRUE;

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:Set要求受付可能フラグ設定								*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
void EENL_SetReqSetEnable(void)
{
	DEBUG_TRCIN();

	if (FALSE == s_eenlReqSetEnable) {
		DLOG_Debug("set request enable.");	/* 初回のみ出力 */
	} else {
		/* DO NOTHING */
	}

	s_eenlReqSetEnable = TRUE;	/*  Set要求受け付ける */

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:EPC設定済フラグ設定									*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　instance	インスタンスハンドル					*/
/*					:[in]　epc	プロパティ									*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
void EENL_SetEpcValidFlg(void* instance, uchar epc)
{
	DEBUG_TRCIN();

	if (NULL != instance) {
		/* ノード */
		if (instance == g_eenlDevNode) {
			s_eenlNodEpcValidFlg[epc] = TRUE;
			DLOG_Debug("set EPC valid flag. instance:0x%08lX(node), epc:%02X\n",
				(ulong)instance, epc);
		}
		/* 蓄電池 */
		else if (instance == g_eenlDevBattery) {
			s_eenlBatEpcValidFlg[epc] = TRUE;
			DLOG_Debug("set EPC valid flag. instance:0x%08lX(battery), epc:%02X\n",
				(ulong)instance, epc);
		}
		/* 太陽光 */
		else if (instance == g_eenlDevSolar) {
			s_eenlSolEpcValidFlg[epc] = TRUE;
			DLOG_Debug("set EPC valid flag. instance:0x%08lX(solar), epc:%02X\n",
				(ulong)instance, epc);
		}
		/* 車両充放電器 */
		else if (instance == g_eenlDevV2h) {
			s_eenlV2hEpcValidFlg[epc] = TRUE;
			DLOG_Debug("set EPC valid flag. instance:0x%08lX(v2h), epc:%02X\n",
				(ulong)instance, epc);
		}
		/* マルチ入力PCS */
		else if (instance == g_eenlDevMpcs) {
			s_eenlMpcsEpcValidFlg[epc] = TRUE;
			DLOG_Debug("set EPC valid flag. instance:0x%08lX(mpcs), epc:%02X\n",
				(ulong)instance, epc);
		} else {
			/* DO NOTHING */
			DLOG_Error("unknown device!");
		}
	} else {
		DLOG_Error("device is NULL!");
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:Get可能チェック										*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　instance	インスタンスハンドル					*/
/*					:[in]　epc プロパティコード								*/
/*--------------------------------------------------------------------------*/
/*	return			:取得結果												*/
/*					 (FALSE:Get不許可/TRUE:Get許可)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
bool_t EENL_ChkGetEnable(void* instance, uchar epc)
{
	bool_t	ret = FALSE;

	DEBUG_TRCIN();

	if (NULL != instance) {
		ret = eenl_GetEpcValidFlg(instance, epc);
	} else {
		DLOG_Error("device is NULL!");
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:Set可能チェック										*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　instance	インスタンスハンドル					*/
/*					:[in]　epc プロパティコード								*/
/*--------------------------------------------------------------------------*/
/*	return			:取得結果												*/
/*					 (FALSE:Set不許可/TRUE:Set許可)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
bool_t EENL_ChkSetEnable(void* instance, uchar epc)
{
	bool_t	ret = FALSE;
	bool_t	valid = FALSE;

	DEBUG_TRCIN();

	if (NULL != instance) {
		if ((instance == g_eenlDevNode) ||			/*  ノード */
			(instance == g_eenlDevBattery) ||		/*  蓄電池 */
			(instance == g_eenlDevSolar) ||			/*  太陽光 */
			(instance == g_eenlDevV2h) ||			/*  車両充放電器 */
			(instance == g_eenlDevMpcs))			/*  マルチ入力PCS */
		{
			/*  EPC設定済みかどうかを取得 */
			valid = eenl_GetEpcValidFlg(instance, epc);
		}
		/*  EPC設定済み or 初回の周期処理が終わっていればSet受付 */
		if ((TRUE == valid) || (TRUE == s_eenlReqSetEnable)) {
			ret = TRUE;
		} else {
			DLOG_Error("Not Ready!");
		}
	} else {
		DLOG_Error("device is NULL!");
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:プロパティ値設定のラッパー関数							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　message メッセージハンドル格納先					*/
/*					:[in]　instance インスタンスハンドル					*/
/*					:[in]　epc プロパティコード								*/
/*					:[in]　pdc プロパティデータ								*/
/*					:[in]　edt プロパティデータ								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS:正常終了)							*/
/*					 (ENL_RESULT_NOT_INITIALIZED:未初期化)					*/
/*					 (ENL_RESULT_INVALID_PARAMETER:パラメータ不正)			*/
/*				(ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致) */
/*					 (ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ)	*/
/*					 (ENL_RESULT_INVALID_SIZE:データサイズ超過)				*/
/*					 (ENL_RESULT_EVALUATION_FAILED:値域外データ)			*/
/*					 (ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗)			*/
/*					 (ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過)		*/
/*					 (ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗)			*/
/*					 (FALSE:Set不許可/TRUE:Set許可)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_ExtEnlDevSetProperty(ENL_MESSAGE** message,
	void* instance, uchar epc, uchar pdc, uchar* edt)
{
	slong	ret = ENL_RESULT_SUCCESS;

	DEBUG_TRCIN();

	ret = ENL_DEV_set_property(message, instance, epc, pdc, edt);
	if (ENL_RESULT_SUCCESS == ret) {
		EENL_SetEpcValidFlg(instance, epc);
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:プロパティ値設定(数値)のラッパー関数					*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　message メッセージハンドル格納先					*/
/*					:[in]　instance インスタンスハンドル					*/
/*					:[in]　epc プロパティコード								*/
/*					:[in]　value 設定値										*/
/*					:[in]　index インデクス(要素1では常に0)					*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS:正常終了)							*/
/*					 (ENL_RESULT_NOT_INITIALIZED:未初期化)					*/
/*					 (ENL_RESULT_INVALID_PARAMETER:パラメータ不正)			*/
/*				(ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致) */
/*					 (ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ)	*/
/*					 (ENL_RESULT_INVALID_SIZE:データサイズ超過)				*/
/*					 (ENL_RESULT_EVALUATION_FAILED:値域外データ)			*/
/*					 (ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗)			*/
/*					 (ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過)		*/
/*					 (ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗)			*/
/*					 (FALSE:Set不許可/TRUE:Set許可)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_ExtEnlDevSetPropertyValue(ENL_MESSAGE** message,
	void* instance, uchar epc, ulong value, uchar index)
{
	slong	ret = ENL_RESULT_SUCCESS;

	DEBUG_TRCIN();

	ret = ENL_DEV_set_property_value(message, instance, epc, value, index);
	if (ENL_RESULT_SUCCESS == ret) {
		EENL_SetEpcValidFlg(instance, epc);
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:指定されたepcをブロードキャスト設定する				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　devList デバイスクラスのリスト					*/
/*					:[in]　listcnt リストカウント							*/
/*					:[in]　epc プロパティコード								*/
/*					:[in]　valSize 設定値サイズ								*/
/*					:[in]　val 設定値										*/
/*					:[in]　ano_onoff 状変通知ON/OFF							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS:正常終了)							*/
/*					 (ENL_RESULT_NOT_INITIALIZED:未初期化)					*/
/*					 (ENL_RESULT_INVALID_PARAMETER:パラメータ不正)			*/
/*				(ENL_RESULT_INVALID_INSTANCE_MODE:インスタンスモード不一致) */
/*					 (ENL_RESULT_UNSUPPORTED_PROPERTY:管理対象外プロパティ)	*/
/*					 (ENL_RESULT_INVALID_SIZE:データサイズ超過)				*/
/*					 (ENL_RESULT_EVALUATION_FAILED:値域外データ)			*/
/*					 (ENL_RESULT_SEMAPHORE_ERROR:セマフォ取得失敗)			*/
/*					 (ENL_RESULT_OVER_MAX_LENGTH:最大メッセージ長超過)		*/
/*					 (ENL_RESULT_ALLOCATION_FAILED:メモリ確保失敗)			*/
/*					 (FALSE:Set不許可/TRUE:Set許可)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:デバイスクラスがNULLなら設定も状変通知も行いません。	*/
/*		全ての処理が正常の場合には戻り値にはENL_RESULT_SUCCESSが返りますが	*/
/*		途中でエラーが発生していた場合は、一番最後のエラーが返ります。		*/
/*		エラーをフォローしたい場合には使用しないでください。				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BroadcastSetProperty(
	void** devList,
	uchar	listcnt,
	uchar	epc,
	uchar	valSize,
	uchar* val,
	uchar	ano_onoff)
{
	slong ret = ENL_RESULT_SUCCESS;
	slong func_ret = ENL_RESULT_SUCCESS;
	uchar i = 0u;
	ENL_MESSAGE* msg = NULL;

	DEBUG_TRCIN();

	if ((NULL == devList) || (0u == listcnt)) {
		return ENL_RESULT_INVALID_PARAMETER;
	} else {
		/* Do nothing. */
	}

	if ((0u == valSize) || (NULL == val)) {
		return ENL_RESULT_INVALID_PARAMETER;
	} else {
		/* Do nothing. */
	}

	if ((EENL_ANNOUNCE_ON != ano_onoff)
		&& (EENL_ANNOUNCE_OFF != ano_onoff)) {
		return ENL_RESULT_INVALID_PARAMETER;
	} else {
		/* Do nothing. */
	}

	for (i = 0u; i < listcnt; i++) {
		if (NULL != devList[i]) {

			ret = EENL_ExtEnlDevSetProperty(
				&msg, devList[i], epc,
				valSize, (uchar*)val);
			DLOG_Debug("devList[%d]: %d Set = [%ld]",
				i, epc, ret);

			if (ENL_RESULT_SUCCESS == ret) {
				if (EENL_ANNOUNCE_ON == ano_onoff) {
					/* 状変アナウンス通知 */
					EENL_SendMessage(NULL, msg);
				} else {
					/* Do nothing. */
				}
			} else {
				DLOG_Debug("[ERROR]devList[%d]: %d Set = [%ld]",
					i, epc, ret);
				func_ret = ret;
			}

		} else {
			/* Do nothing. */
		}
	}

	DEBUG_TRCOUT();
	return func_ret;
}

/*==========================================================================*/
/*	Description		:指定されたepcをブロードキャスト設定する（数値版）		*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　devList デバイスクラスのリスト					*/
/*					:[in]　listcnt リストカウント							*/
/*					:[in]　epc プロパティコード								*/
/*					:[in]　val 設定値										*/
/*					:[in]　index 設定箇所									*/
/*					:[in]　ano_onoff 状変通知ON/OFF							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (ENL_RESULT_SUCCESS:正常終了)							*/
/*					 (ENL_RESULT_INVALID_PARAMETER:パラメータ不正)			*/
/*					 (eenl_ext_ENL_DEV_set_property_valueの戻り値)			*/
/*--------------------------------------------------------------------------*/
/*	detail			:デバイスクラスがNULLなら設定も状変通知も行いません。	*/
/*		全ての処理が正常の場合には戻り値にはENL_RESULT_SUCCESSが返りますが	*/
/*		途中でエラーが発生していた場合は、一番最後のエラーが返ります。		*/
/*		エラーをフォローしたい場合には使用しないでください。				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_BroadcastSetPropertyValue(
	void** devList,
	uchar	listcnt,
	uchar	epc,
	ulong	val,
	uchar	index,
	uchar	ano_onoff)
{
	slong ret = ENL_RESULT_SUCCESS;
	slong func_ret = ENL_RESULT_SUCCESS;
	uchar i = 0u;
	ENL_MESSAGE* msg = NULL;

	DEBUG_TRCIN();

	if ((NULL == devList) || (0u == listcnt)) {
		return ENL_RESULT_INVALID_PARAMETER;
	} else {
		/* Do nothing. */
	}

	if ((EENL_ANNOUNCE_ON != ano_onoff)
		&& (EENL_ANNOUNCE_OFF != ano_onoff)) {
		return ENL_RESULT_INVALID_PARAMETER;
	} else {
		/* Do nothing. */
	}

	for (i = 0u; i < listcnt; i++) {
		if (NULL != devList[i]) {

			ret = EENL_ExtEnlDevSetPropertyValue(
				&msg, devList[i], epc, val, index);
			DLOG_Debug("devList[%d]: %d Set = [%ld]", i, epc, ret);

			if (ENL_RESULT_SUCCESS == ret) {
				if (EENL_ANNOUNCE_ON == ano_onoff) {
					/* 状変アナウンス通知 */
					EENL_SendMessage(NULL, msg);
				} else {
					/* Do nothing. */
				}
			} else {
				DLOG_Debug("[ERROR]devList[%d]: %d Set = [%ld]",
					i, epc, ret);
				func_ret = ret;
			}

		} else {
			/* Do nothing. */
		}
	}

	DEBUG_TRCOUT();
	return func_ret;
}

/*==========================================================================*/
/*	Description		:現在日時の翌日をEtimTm_t型で取得する					*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　curDay 現在日時									*/
/*					:[in]　nextDay 現在日時の翌日							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (EENL_OK(0):正常終了)									*/
/*					 (EENL_ERR_SYSCALL(-2):システムコールエラー)			*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_GetNextDay(EtimTm_t* curDay, EtimTm_t* nextDay)
{
	time_t chg_sec; /* 秒変換 */
	struct tm cur_tm, * nxt_tm = NULL, result;
	slong ret = EENL_OK;

	DEBUG_TRCIN();

	/*  実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* 初期化 */
	memset(&cur_tm, 0, sizeof(cur_tm));
	memset(&result, 0, sizeof(result));

	/* 現在日時を翌日に編集 */
	/* 1900からの年数 */
	cur_tm.tm_year = (curDay->tmYear - 1900);
	/* 1月からの月数[0-11] */
	cur_tm.tm_mon = (curDay->tmMon - 1);
	/* 翌日 */
	cur_tm.tm_mday = (curDay->tmMday + 1);
	cur_tm.tm_hour = (curDay->tmHour);
	cur_tm.tm_min = (curDay->tmMin);
	cur_tm.tm_sec = (curDay->tmSec);

	/* 秒に変換 */
	chg_sec = mktime(&cur_tm);
	if (chg_sec == -1) {
		DLOG_Error("mktime error!");
		ret = EENL_ERR_SYSCALL;
	} else {
		/* 秒を年月日時分秒へ変換 */
		nxt_tm = localtime_r(&chg_sec, &result);
		if (nxt_tm == NULL) {
			DLOG_Error("localtime_r error!");
			ret = EENL_ERR_SYSCALL;
		} else {
			/* EtimTm_t型に編集 */
			nextDay->tmYear = result.tm_year + 1900;
			nextDay->tmMon = result.tm_mon + 1;
			nextDay->tmMday = result.tm_mday;
			nextDay->tmHour = result.tm_hour;
			nextDay->tmMin = result.tm_min;
			nextDay->tmSec = result.tm_sec;
		}
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:epcをブロードキャスト設定するためのデバイスリスト		*/
/*					を作成する												*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　devList デバイスリスト							*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:EENL_BroadcastSetProperty()、							*/
/*					EENL_BroadcastSetPropertyValue()で						*/
/*					使用するデバイスリストを作成する						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
void EENL_SetBroadcastDevList(void** devList)
{
	slong i = 0;
	slong list_size = 0;

	DEBUG_TRCIN();
	/*  実装上、引数がNULLになることは無いため、NULLチェックを省略する */

	/* 初期化 */
	for (i = EENL_DEV_LIST_NODE; i < EENL_DEV_LIST_MAX; i++) {
		devList[i] = NULL;
	}

	list_size = sizeof(g_eenlDevList);

	/* デバイスリストにグローバルの情報をコピー */
	memcpy(devList, g_eenlDevList, (size_t)list_size);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:充放電時間帯の入力が正しいかチェックする				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　hourMinute 充放電時間帯							*/
/*--------------------------------------------------------------------------*/
/*	return			:判定結果												*/
/*					 (TRUE:正常終了/FALSE:異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/13 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
bool_t EENL_CheckChargeTimeZone(EPCS_CHARGE_TIME_ZONE_T* epcsTimeZone)
{
	bool_t retCode = FALSE;				/* 戻り値 */
	slong retCodeFun = EENL_RET_CODE_OK;		/* 戻り値(呼び出す関数) */

	DEBUG_TRCIN();

	retCodeFun = eenl_CheckChargeTimeZonefunc(epcsTimeZone);
	if (EENL_RET_CODE_OK == retCodeFun) {
		retCode = TRUE;
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
	return retCode;
}

/*==========================================================================*/
/*	Description		:PCSエラーコードをHEMS_GW通知用エラーコードに変換する	*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　pcsErr 充放電時間帯								*/
/*					:[in]　pcsErrRating PCSエラーレーティング				*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC JY.li : 新規作成						*/
/*==========================================================================*/
void EENL_ChangeHemsErrCode(schar* pcsErr, schar* pcsErrRating)
{
	if (pcsErr != NULL) {
		if ((memcmp(pcsErr, "BI", 2) == 0) ||
			(memcmp(pcsErr, "be", 2) == 0) ||
			(memcmp(pcsErr, "bf", 2) == 0) ||
			(memcmp(pcsErr, "bc", 2) == 0) ||
			(memcmp(pcsErr, "bi", 2) == 0))
		{
			memcpy(pcsErr, EPCS_PCSERR_NONE, strlen(EPCS_PCSERR_NONE));

			if (pcsErrRating != NULL) {
				/* BE000:エラーフリー */
				*pcsErrRating = '0';
			} else {
				/* DO NOTHING */
			}
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}
}

/*==========================================================================*/
/*	Description		:ENL充放電制御チェック									*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　set_ope_mode	充放電制御							*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					(0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:引数チェックは行わない(呼び元で行うこと)				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_CheckEnlOpectrl(uchar set_ope_mode)
{
	slong ret = EENL_OK;
	slong epcsRet = EPCS_OK;
	EPCS_OPEMODE_SET_T ope;

	DEBUG_TRCIN();

	if (g_eenlOpeCtrl != EENL_OPECTRL_OFF) {
		/*  ENL充放電制御中の場合 */
		if (g_eenlOpeCtrl != set_ope_mode) {
			/*  運転モードが変更されたら(変更後が充電/放電以外の場合のみ)、ENL充放電制御を解除する */
			memset(&ope, 0x00, sizeof(ope));

			/*  ※充放電制御を解除する場合、setChangeFlagはEPCS_SET_LIMIT_CTRLのみを設定する。 */
			ope.setChangeFlag = EPCS_SET_LIMIT_CTRL;
			ope.remoteState = EPCS_REMOTE_ON;
			ope.limitCtrl = EPCS_LIMIT_CTRL_OFF;

			DLOG_Debug("change enl operation control = 0x%02x->0x%02x. set = 0x%02x.",
				g_eenlOpeCtrl, EENL_OPECTRL_OFF, set_ope_mode);

			epcsRet = epcs_sync_setOpeMode(&ope);
			if (epcsRet == EPCS_OK) {
				/*  グローバル変数を更新して、ファイルに書き出し */
				g_eenlOpeCtrl = EENL_OPECTRL_OFF;

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
				DLOG_Error("einv set enl operation control ERROR! (einv ret=%ld)",
					epcsRet);
				ret = EENL_ERR;
			}
		} else {
			/*  運転モード変更なしなら、何もしない(状態を維持) */
			DLOG_Debug("Not chagne enl operation control = 0x%02x. set = 0x%02x.",
				g_eenlOpeCtrl, set_ope_mode);
		}
	} else {
		/*  ENL充放電制御中でない場合、何もしない */
		DLOG_Debug("do Nothing. enl operation control = 0x%02x. set = 0x%02x.",
			g_eenlOpeCtrl, set_ope_mode);
	}

	DEBUG_TRCOUT();
	return ret;
}


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description		:受信ソケットの生成(UDP IPv4)							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cp	connectフラグ								*/
/*--------------------------------------------------------------------------*/
/*	return			:ソケットdescriptor										*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/30 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static sint eenl_CreateSocketRecv(const schar* cp)
{
	struct addrinfo hints, * res = NULL, hints2, * res2 = NULL;
	struct group_req greq, greq2;

	slong err = 0;
	slong one = 1;
	slong ret = 0;
	uchar enetIf = 0u;
	schar wlanName[ENET_WLAN_IF_NAME_STR_LEN] = { 0 };
	bool_t isError = FALSE;
	s_eenlRv4 = -1;

#if defined(IGNORE_SELF) && IGNORE_SELF != 0
#if defined(__CYGWIN__) && __CYGWIN__ != 0
	slong flag = 0;
#endif
#endif
	DEBUG_TRCIN();

	/* 未使用引数 */
	EENL_UNUSED_PARA(cp);

	/* for multicast */
	memset(&hints2, 0, sizeof(hints2));
	memset(wlanName, 0, sizeof(wlanName));

	hints2.ai_family = AF_INET;
	hints2.ai_socktype = SOCK_DGRAM;

	err = getaddrinfo(EENL_MULTICAST_ADDRESS, "3610", &hints2, &res2);
	if (err != 0) {
		/* error */
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
		perror("{perr}eenl_CreateSocketRecv mcast getaddrinfo");
#endif
		DLOG_Error("recv4 getaddr error!: %ld", err);
		isError = TRUE;
	} else {
		err = ENET_SyncGetWlanIfName(wlanName);
		if (err == ENET_SUCCESS) {
			DLOG_Debug("get wlan name : [%s]", wlanName);
		} else {
			DLOG_Debug("get wlan name ERROR : %ld[%s]", err, wlanName);
		}

		DLOG_Debug("ifname eth0 : %d", if_nametoindex("eth0"));
		DLOG_Debug("ifname wlan : %d", if_nametoindex(wlanName));

		greq.gr_interface = 0;		/* any */
		memcpy(&greq.gr_group, res2->ai_addr, res2->ai_addrlen);

		greq2.gr_interface = if_nametoindex(wlanName);
		memcpy(&greq2.gr_group, res2->ai_addr, res2->ai_addrlen);

		freeaddrinfo(res2);
	}

	if (!isError) {
		/* for enl通信 */
		memset(&hints, 0, sizeof(hints));

		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
		err = getaddrinfo(NULL, "3610", &hints, &res);
		if (err != 0) {
			/* error */
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
			perror("{perr}eenl_CreateSocketRecv getaddrinfo");
#endif
			DLOG_Error("recv4 getaddrinfo error!: %ld", err);
			isError = TRUE;
		} else {
			s_eenlRv4 = socket(res->ai_family, res->ai_socktype, 0);
			if (s_eenlRv4 < 0) {
				/* error */
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
				perror("{perr}eenl_CreateSocketRecv socket");
#endif
				DLOG_Error("recv4 socket create error!: %d", s_eenlRv4);
				isError = TRUE;
			} else {
				ret = setsockopt(s_eenlRv4, SOL_SOCKET, SO_REUSEADDR,
					&one, sizeof(one));
				if (ret != 0) {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
					perror("{perr}eenl_CreateSocketRecv reuse setsockopt");
#endif
					DLOG_Error("recv4 reuse set error!: %ld", ret);
					isError = TRUE;
				} else {
					ret = bind(s_eenlRv4, res->ai_addr, res->ai_addrlen);
					if (ret != 0) {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
						perror("{perr}eenl_CreateSocketRecv bind");
#endif
						DLOG_Error("recv4 bind error!: %ld", ret);
						isError = TRUE;
					} else {
						/* DO NOTHING */
					}
					DLOG_Debug("recv4 bind ok: %ld", ret);

				}
			}
		}
	} else {
		/* DO NOTHING */
	}

	if (!isError) {
		/* IPv4 multicast */
		enetIf = ENET_SyncGetNetworkType();
		if (enetIf == ENET_NETWORK_WIRELESS) {
			/* 無線 */
			ret = setsockopt(s_eenlRv4, IPPROTO_IP, MCAST_JOIN_GROUP,
				(schar*)&greq2, sizeof(greq2));
			if (ret == 0) {
				DLOG_Debug("ipv4 multicast set(%d): %ld",
					greq2.gr_interface, ret);
			} else {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
				perror("{perr}eenl_CreateSocketRecv multicast set(wlan)");
#endif
				DLOG_Debug("recv4 multicast set(wlan) error : %d", errno);
				/* 処理は継続 */
			}

		} else {
			/* その他：有線  eth0*/
			ret = setsockopt(s_eenlRv4, IPPROTO_IP, MCAST_JOIN_GROUP,
				(schar*)&greq, sizeof(greq));
			if (ret == 0) {
				DLOG_Debug("ipv4 multicast set(%d): %ld",
					greq.gr_interface, ret);
			} else {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
				perror("{perr}eenl_CreateSocketRecv multicast set(eth0)");
#endif
				DLOG_Debug("recv4 multicast set(eth0) error : %d", errno);
				/* 処理は継続 */
			}

		}

#if defined(IGNORE_SELF) && IGNORE_SELF != 0
#if defined(__CYGWIN__) && __CYGWIN__ != 0
		setsockopt(s_eenlRv4, IPPROTO_IP, IP_MULTICAST_LOOP,
			&flag, sizeof(flag));
#endif
#endif
	} else {
		/* DO NOTHING */
	}

	freeaddrinfo(res);

	if (isError) {
		s_eenlRv4 = -1;
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
	return s_eenlRv4;
}

/*==========================================================================*/
/*	Description		:受信ソケットの生成(UDP IPv6)							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cp	connectフラグ								*/
/*--------------------------------------------------------------------------*/
/*	return			:ソケットdescriptor										*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static sint eenl_CreateSocketRecv6(const schar* cp)
{
	struct addrinfo hints, * res = NULL, hints2, * res2 = NULL;
	struct group_req greq, greq2;

	slong err = 0;
	slong one = 1;
	slong ret = 0;
	schar wlanName[ENET_WLAN_IF_NAME_STR_LEN] = { 0 };
	bool_t isError = FALSE;
	s_eenlRv6 = -1;

#if defined(IGNORE_SELF) && IGNORE_SELF != 0
#if defined(__CYGWIN__) && __CYGWIN__ != 0
	slong flag = 0;
#endif
#endif
	DEBUG_TRCIN();

	/* 未使用引数 */
	EENL_UNUSED_PARA(cp);

	/* for multicast */
	memset(&hints2, 0, sizeof(hints2));
	memset(wlanName, 0, sizeof(wlanName));

	hints2.ai_family = AF_INET6;
	hints2.ai_socktype = SOCK_DGRAM;

	err = getaddrinfo(EENL_MULTICAST_ADDRESS_V6, "3610", &hints2, &res2);
	if (err != 0) {
		/* error */
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
		perror("{perr}eenl_CreateSocketRecv6 mcast6 getaddrinfo");
#endif
		DLOG_Error("recv6 getaddr error!: %ld", err);
		isError = TRUE;
	} else {
		err = ENET_SyncGetWlanIfName(wlanName);
		if (err == ENET_SUCCESS) {
			DLOG_Debug("get wlan name : [%s]", wlanName);
		} else {
			DLOG_Error("get wlan name ERROR : %ld[%s]", err, wlanName);
		}

		DLOG_Debug("ifname eth0 : %d", if_nametoindex("eth0"));
		DLOG_Debug("ifname wlan : %d", if_nametoindex(wlanName));

		greq.gr_interface = 0;		/* any */
		memcpy(&greq.gr_group, res2->ai_addr, res2->ai_addrlen);

		greq2.gr_interface = if_nametoindex(wlanName);
		memcpy(&greq2.gr_group, res2->ai_addr, res2->ai_addrlen);

		freeaddrinfo(res2);
	}

	if (!isError) {
		/* for enl通信 */
		memset(&hints, 0, sizeof(hints));

		hints.ai_family = AF_INET6;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
		err = getaddrinfo(NULL, "3610", &hints, &res);
		if (err != 0) {
			/* error */
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
			perror("{perr}eenl_CreateSocketRecv6 getaddrinfo");
#endif
			DLOG_Error("recv6 getaddrinfo error!: %ld", err);
			isError = TRUE;
		} else {
			s_eenlRv6 = socket(res->ai_family, res->ai_socktype, 0);
			if (s_eenlRv6 < 0) {
				/* error */
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
				perror("{perr}eenl_CreateSocketRecv6 socket");
#endif
				DLOG_Error("recv6 socket create error!: %d", s_eenlRv6);
				isError = TRUE;
			} else {
				ret = setsockopt(s_eenlRv6, SOL_SOCKET, SO_REUSEADDR,
					&one, sizeof(one));
				if (ret != 0) {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
					perror("{perr}eenl_CreateSocketRecv6 setsockopt");
#endif
					DLOG_Error("recv6 reuse set error!: %ld", ret);
					isError = TRUE;
				} else {
					ret = bind(s_eenlRv6, res->ai_addr, res->ai_addrlen);
					if (ret != 0) {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
						perror("{perr}eenl_CreateSocketRecv6 bind");
#endif
						DLOG_Error("recv6 bind error!: %ld", ret);
						isError = TRUE;
					} else {
						/* DO NOTHING */
					}
					DLOG_Debug("recv6 bind ok: %ld", ret);
				}
			}
		}
	} else {
		/* DO NOTHING */
	}

	if (!isError) {

		/* IPv6 multicast */
		/*  any */
		ret = setsockopt(s_eenlRv6, IPPROTO_IPV6, MCAST_JOIN_GROUP,
			(schar*)&greq, sizeof(greq));
		if (ret == 0) {
			DLOG_Debug("ipv6 multicast set(%d): %ld", greq.gr_interface, ret);
		} else {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
			perror("{perr}eenl_CreateSocketRecv6 multicast set(eth0)");
#endif
			DLOG_Debug("recv6 multicast set(eth0) error : %d", errno);
			/* 処理は継続 */
		}

		/* wlan */
		ret = setsockopt(s_eenlRv6, IPPROTO_IPV6, MCAST_JOIN_GROUP,
			(schar*)&greq2, sizeof(greq2));
		if (ret == 0) {
			DLOG_Debug("ipv6 multicast set(%d): %ld", greq2.gr_interface, ret);
		} else {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
			perror("{perr}eenl_CreateSocketRecv6 multicast set(wlan)");
#endif
			DLOG_Debug("recv6 multicast set(wlan) error : %d", errno);
			/* 処理は継続 */
		}

#if defined(IGNORE_SELF) && IGNORE_SELF != 0
#if defined(__CYGWIN__) && __CYGWIN__ != 0
		setsockopt(s_eenlRv6, IPPROTO_IP, IP_MULTICAST_LOOP,
			&flag, sizeof(flag));
#endif
#endif
	}

	freeaddrinfo(res);

	if (isError) {
		s_eenlRv6 = -1;
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
	return s_eenlRv6;
}

/*==========================================================================*/
/*	Description		:送信ソケットの生成(UDP IPv4)							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cp	connectフラグ								*/
/*--------------------------------------------------------------------------*/
/*	return			:ソケットdescriptor										*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static sint eenl_CreateSocketSend(const schar* cp)
{
	struct sockaddr_in addr;

#if defined(IGNORE_SELF) && IGNORE_SELF != 0
#if !defined(__CYGWIN__) || __CYGWIN__ == 0
	slong flag = 0;
#endif
#endif

	DEBUG_TRCIN();

	s_eenlSv4 = socket(AF_INET, SOCK_DGRAM, 0);		/* UDP */

	if (cp != NULL) {
		addr.sin_family = AF_INET;		/* IPv4 only */
		addr.sin_addr.s_addr = htons(INADDR_ANY);
		addr.sin_port = htons(EENL_UDP_PORT);

		if (s_eenlSv4 != -1) {
			connect(s_eenlSv4, (struct sockaddr*) & addr,
				sizeof(struct sockaddr_in));
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

#if defined(IGNORE_SELF) && IGNORE_SELF != 0
#if !defined(__CYGWIN__) || __CYGWIN__ == 0
	setsockopt(s_eenlSv4, IPPROTO_IP, IP_MULTICAST_LOOP, &flag, sizeof(flag));
#endif
#endif

	DEBUG_TRCOUT();
	return s_eenlSv4;
}

/*==========================================================================*/
/*	Description		:送信ソケットの生成(UDP IPv6)							*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cp	connectフラグ								*/
/*--------------------------------------------------------------------------*/
/*	return			:ソケットdescriptor										*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static sint eenl_CreateSocketSend6(const schar* cp)
{
	struct sockaddr_in6 addr;

#if defined(IGNORE_SELF) && IGNORE_SELF != 0
#if !defined(__CYGWIN__) || __CYGWIN__ == 0
	slong flag = 0;
#endif
#endif

	DEBUG_TRCIN();

	s_eenlSv6 = socket(AF_INET6, SOCK_DGRAM, 0);	/* UDP */

	if (cp != NULL) {
		addr.sin6_family = AF_INET6;		/* IPv6 only */
		addr.sin6_port = htons(EENL_UDP_PORT);
		addr.sin6_addr = in6addr_any;

		if (s_eenlSv6 != -1) {
			connect(s_eenlSv6, (struct sockaddr*) & addr,
				sizeof(struct sockaddr_in));
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

#if defined(IGNORE_SELF) && IGNORE_SELF != 0
#if !defined(__CYGWIN__) || __CYGWIN__ == 0
	setsockopt(s_eenlSv6, IPPROTO_IP, IP_MULTICAST_LOOP, &flag, sizeof(flag));
#endif
#endif

	DEBUG_TRCOUT();
	return s_eenlSv6;
}


/*==========================================================================*/
/*	Description		:デバイスの登録削除										*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　dst_info	デバイス情報							*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/30 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_UnregisterDeviceInfo(EenlDeviceInfo_t* dst_info)
{
	EenlDeviceInfo_t** info = NULL;
	void* inst = NULL;
	bool_t isLoopEnd = FALSE;

	DEBUG_TRCIN();

	inst = eenl_GetSelectedInstance();

	info = &s_eenlInfoHead;

	while (*info != NULL && !isLoopEnd) {
		if (*info == dst_info) {
			isLoopEnd = TRUE;
		} else {
			info = &(*info)->next;
		}
	}

	if (*info != NULL) {
		*info = dst_info->next;
		if (dst_info->node == inst || dst_info->node == inst) {
			eenl_SetSelectedInstance(NULL);
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:デバイスリストの選択設定								*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　instance	インスタンスハンドル					*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/30 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_SetSelectedInstance(void* instance)
{
	DEBUG_TRCIN();

	s_eenlSelectedInstance = instance;

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:インスタンスハンドルの取得								*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　instance	インスタンスハンドル					*/
/*--------------------------------------------------------------------------*/
/*	return			:インスタンスハンドル									*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/30 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void* eenl_GetSelectedInstance(void)
{
	DEBUG_TRCIN();

	DEBUG_TRCOUT();
	return s_eenlSelectedInstance;
}


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	thread function
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*==========================================================================*/
/*	Description		:メッセージの送信										*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cp	connectフラグ								*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_ThreadSend(const schar* cp)
{
	sint nfds = 0;
	u64 val = 0u;
	slong num = 0;
	EenlAddr_t* addr = NULL;
	ENL_MESSAGE* msg = NULL;
	bool_t isLoop = TRUE;
	fd_set rfds;
	fd_set efds;
	struct timeval tv;

	DEBUG_TRCIN();

	/* 未使用引数 */
	EENL_UNUSED_PARA(cp);
	DLOG_Debug("***** eenl_ThreadSend in *****");

	while (isLoop) {
		pthread_testcancel();
		FD_ZERO(&rfds);
		FD_SET(s_eenlFdEvt[EENL_IDX_FD_EVT_READ], &rfds);
		FD_SET(s_eenlFdQuit[EENL_IDX_FD_EVT_READ], &rfds);
		efds = rfds;
		if (s_eenlFdEvt[EENL_IDX_FD_EVT_READ]
			> s_eenlFdQuit[EENL_IDX_FD_EVT_READ]) {
			nfds = s_eenlFdEvt[EENL_IDX_FD_EVT_READ];
		} else {
			nfds = s_eenlFdQuit[EENL_IDX_FD_EVT_READ];
		}

		/* タイムアウト:5秒 */
		tv.tv_sec = EENL_THREAD_TIMEOUT;
		tv.tv_usec = 0;

		num = select(nfds + 1, &rfds, NULL, &efds, &tv);
		DLOG_Debug("send select: %ld [%ld,%ld]", num, tv.tv_sec, tv.tv_usec);
		if (num <= 0) {
			/* タイムアウト、エラー時、何もせずに次の処理へ */
			DLOG_Debug("send timeout");
		} else {
			if (FD_ISSET(s_eenlFdEvt[EENL_IDX_FD_EVT_READ], &rfds)) {
				read((s_eenlFdEvt[EENL_IDX_FD_EVT_READ]),
					&val, sizeof(u64));
				addr = (EenlAddr_t*)(ulong)(val >> 32);
				msg = (ENL_MESSAGE*)(ulong)val;

				isLoop = eenl_ThreadSendSub(msg, addr);
				sem_post(&s_eenlSem);
			} else {
				isLoop = FALSE;
			}
		}
		if (s_eenlLoopOut == 1) {
			DLOG_Debug("*****s_eenlLoopOut send2*****");
			pthread_testcancel();
			isLoop = FALSE;
		} else {
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:メッセージの送信eenl_ThreadSendの子関数				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　msg	メッセージ構造体							*/
/*	param			:[in]　addr	アドレス									*/
/*--------------------------------------------------------------------------*/
/*	return			:判定結果												*/
/*					 (TRUE:正常終了/FALSE:異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/017 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static bool_t eenl_ThreadSendSub(const ENL_MESSAGE* msg, const EenlAddr_t* addr)
{
	sint fd1 = -1;
	sint fd2 = -1;
	sint fd3 = -1;
	slong ret = 0;
	slong retSetsockopt = 0;
	slong retSendto = 0;
	slong i = 0;
	ENL_MESSAGE* next = NULL;
	schar nm[INET6_ADDRSTRLEN] = { 0 };
	bool_t retVal = TRUE;
	struct sockaddr_in adr4;
	struct sockaddr_in6 adr6;
	struct timeval stv;

	DEBUG_TRCIN();

	/* タイムアウト:3秒 */
	stv.tv_sec = 3;
	stv.tv_usec = 0;

	fd1 = eenl_CreateSocketSend(NULL);
	fd2 = eenl_CreateSocketSend6(NULL);

	while (msg != NULL) {
		pthread_testcancel();
		if (addr != NULL && (msg->flags & ENL_MESSAGE_BROADCAST) == 0) {
			if (addr->ipType == EENL_IP_TYPE_V6) {
				/* IPv6 */
				memset(&nm, 0, sizeof(nm));
				inet_ntop(AF_INET6, &(addr->v6addr.sin6_addr), nm, sizeof(nm));

				if (addr->protoType == EENL_PROTO_TYPE_TCP) {
					DLOG_Debug("++++ tcp send to %s ++++", nm);
				} else {
					DLOG_Debug("#### send (v6) to %s ####", nm);
				}

				adr6 = addr->v6addr;

				for (i = 0; i < (msg->length); i++) {
					DLOG_Debug("%02x ", msg->data[i]);
				}
				DLOG_Debug("\n");

				if (addr->protoType == EENL_PROTO_TYPE_TCP) {
					/* TCP */
					/* IPv6の他に、IPv4でも射影アドレスのためここに入ってくる */
					fd3 = addr->tcpSock;
					DLOG_Debug("tcpSock : %ld", fd3);

					if (fd3 >= 0) {
						/* 応答のタイムアウト時間を設定(3sec) */
						retSetsockopt = setsockopt(fd3, SOL_SOCKET,
							SO_SNDTIMEO, &stv, sizeof(stv));

						retSendto = sendto(fd3, msg->data,
							(size_t)(msg->length), 0,
							(struct sockaddr*) & adr6,
							sizeof(struct sockaddr_in6));
						close(fd3);
					} else {
						DLOG_Error("sendto(tcp) sock error : %ld", fd3);
					}

					if (fd3 >= 0 && retSetsockopt == 0) {
						DLOG_Debug("fd3 setsockopt ok");

					} else {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
						perror("{perr}fd3 setsockopt ");
#endif
						DLOG_Debug("fd3 setsockopt error: %ld", retSetsockopt);
					}

					if (fd3 >= 0 && retSendto >= 0) {
						DLOG_Debug("sendto(tcp) : %ld", retSendto);

					} else {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
						perror("{perr}sendto(tcp) ");
#endif
						DLOG_Error("sendto(tcp) error : %ld, %d",
							retSendto, errno);
					}

				} else {
					/* UDP */
					ret = sendto(fd2, msg->data, (size_t)(msg->length),
						0, (struct sockaddr*) & adr6,
						sizeof(struct sockaddr_in6));
					if (ret >= 0) {
						DLOG_Debug("sendto(v6) : %ld", ret);
					} else {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
						perror("{perr}sendto(v6) ");
#endif
						DLOG_Error("sendto(v6) error : %ld, %d", ret, errno);
					}
				}

			} else {
				/*  IPv4 */
				if (addr->protoType == EENL_PROTO_TYPE_TCP) {
					/* ここには入ってこないはずだが、一応実装★ */
					DLOG_Debug("++++ v4tcp send to %s ++++",
						inet_ntoa(addr->v4addr.sin_addr));
				} else {
					DLOG_Debug("#### send (v4) to %s ####",
						inet_ntoa(addr->v4addr.sin_addr));
				}

				adr4 = addr->v4addr;

				for (i = 0; i < (msg->length); i++) {
					DLOG_Debug("%02x ", msg->data[i]);
				}
				DLOG_Debug("\n");

				if (addr->protoType == EENL_PROTO_TYPE_TCP) {
					/* TCP */
					/* ここには入ってこないはずだが、一応実装★ */
					fd3 = addr->tcpSock;
					if (fd3 >= 0) {
						/* 応答のタイムアウト時間を設定(3sec) */
						retSetsockopt = setsockopt(fd3, SOL_SOCKET,
							SO_SNDTIMEO, &stv, sizeof(stv));

						retSendto = sendto(fd3, msg->data,
							(size_t)(msg->length), 0,
							(struct sockaddr*) & adr4,
							sizeof(struct sockaddr_in));
						close(fd3);
					} else {
						DLOG_Error("sendto(v4tcp) sock error : %ld", fd3);
					}

					if (fd3 >= 0 && retSetsockopt == 0) {
						DLOG_Debug("v4tcp fd3 setsockopt ok");

					} else {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
						perror("{perr}v4tcp fd3 setsockopt ");
#endif
						DLOG_Debug("v4tcp fd3 setsockopt error: %ld",
							retSetsockopt);
					}

					if (fd3 >= 0 && retSendto >= 0) {
						DLOG_Debug("sendto(v4tcp) : %ld", retSendto);
					} else {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
						perror("{perr}sendto(v4tcp) ");
#endif
						DLOG_Error("sendto(v4tcp) error : %ld, %d",
							retSendto, errno);
					}
				} else {
					/* UDP */
					ret = sendto(fd1, msg->data, (size_t)(msg->length),
						0, (struct sockaddr*) & adr4,
						sizeof(struct sockaddr_in));
					if (ret >= 0) {
						DLOG_Debug("sendto(v4) : %ld", ret);
					} else {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
						perror("{perr}sendto(v4) ");
#endif
						DLOG_Error("sendto(v4) error : %ld, %d", ret, errno);
					}
				}
			}

		} else {
			DLOG_Debug("#### send general broadcast message ####");
			DLOG_Debug("msg : %s (%d)", msg->data, msg->length);

			DLOG_Debug("#### send broadcast ####");
			for (i = 0; i < (msg->length); i++) {
				DLOG_Debug("%02x ", msg->data[i]);
			}
			DLOG_Debug("\n");

			/* IPv4 */
			ret = sendto(fd1, msg->data, (size_t)(msg->length), 0,
				(struct sockaddr*) & s_eenlBcAddr, sizeof(struct sockaddr_in));
			DLOG_Debug("sendto(v4) : %ld", ret);
			if (ret < 0) {
				DLOG_Error("sendto(broadcast v4) error : %d", errno);
			} else {
				/* DO NOTHING */
			}

			/* IPv6 */
			ret = sendto(fd2, msg->data, (size_t)(msg->length), 0,
				(struct sockaddr*) & s_eenlBcAddr6,
				sizeof(struct sockaddr_in6));
			DLOG_Debug("broadcast(v6) : %ld", ret);
			DLOG_Debug("sendto(v6) : %ld", ret);
			if (ret < 0) {
				DLOG_Error("sendto(broadcast v6) error : %d", errno);
			} else {
				/* DO NOTHING */
			}

		}
		ENL_get_next_message((ENL_MESSAGE*)msg, &next);
		if ((msg->flags & ENL_MESSAGE_REQUEST) == 0) {
			ENL_dispose_message((ENL_MESSAGE*)msg);
		} else {
			/* DO NOTHING */
		}

		msg = next;
		if (s_eenlLoopOut == 1) {
			DLOG_Debug("*****s_eenlLoopOut send1*****");
			pthread_testcancel();
			retVal = FALSE;
		} else {
			/* DO NOTHING */
		}
	}

	close(fd1);
	close(fd2);

	DEBUG_TRCOUT();
	return retVal;
}

/*==========================================================================*/
/*	Description		:メッセージの受信(UDP)									*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cp	connectフラグ								*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_ThreadRecv(const schar* cp)
{
	sint fd = -1;
	sint nfds = 0;
	slong len = 0;
	slong i = 0;
	slong num = 0;
	socklen_t addrLen = 0;
	uchar buf[EENL_MAX_MESSAGE_LENGTH] = { 0u };
	bool_t isLoop = TRUE;
	fd_set rfds, efds;
	EenlAddr_t addr;
	struct timeval tv;


	DEBUG_TRCIN();

	/* 未使用引数 */
	EENL_UNUSED_PARA(cp);

	DLOG_Debug("***** eenl_ThreadRecv in *****");
	fd = eenl_CreateSocketRecv(NULL);
	if (fd < 0) {
		DLOG_Debug("recv socket : %ld", fd);
	} else {
		DLOG_Debug("***** eenl_ThreadRecv in (%ld) *****", fd);

		while (isLoop) {
			pthread_testcancel();
			FD_ZERO(&rfds);
			FD_SET(fd, &rfds);
			FD_SET(s_eenlFdQuit[EENL_IDX_FD_EVT_READ], &rfds);
			efds = rfds;
			if (fd > s_eenlFdQuit[EENL_IDX_FD_EVT_READ]) {
				nfds = fd;
			} else {
				nfds = s_eenlFdQuit[EENL_IDX_FD_EVT_READ];
			}

			/* タイムアウト:5秒 */
			tv.tv_sec = EENL_THREAD_TIMEOUT;
			tv.tv_usec = 0;

			num = select(nfds + 1, &rfds, NULL, &efds, &tv);
			DLOG_Debug("recv(v4) select: %ld[%ld,%ld]",
				num, tv.tv_sec, tv.tv_usec);
			if (num <= 0) {
				/* タイムアウト、エラー時 */
				/* 何もせず次の処理へ */
				DLOG_Debug("recv(v4) timeout");
			} else {
				if (FD_ISSET(fd, &rfds)) {
					pthread_testcancel();
					addrLen = sizeof(addr.v4addr);
					addr.ipType = EENL_IP_TYPE_V4;
					addr.protoType = EENL_PROTO_TYPE_UDP;
					addr.tcpSock = -1;

					len = recvfrom(fd, buf, sizeof(buf), 0,
						(struct sockaddr*) & (addr.v4addr), &addrLen);
					if (len <= 0) {
						/* 何もせず次の処理へ */
						DLOG_Debug("recv(v4) recv: %ld", len);
					} else {
						DLOG_Debug("#### received (v4) from %s ####",
							inet_ntoa(addr.v4addr.sin_addr));

						for (i = 0;i < len; i++) {
							DLOG_Debug("%02x ", buf[i]);
						}
						DLOG_Debug("\n");

						DLOG_Debug("eenl_ThreadRecv ESV:%02x , EPC::%02x",
							buf[10], buf[12]);
						eenl_ProcessMessage(&addr, buf, len);
						pthread_testcancel();
					}
				} else {
					pthread_testcancel();
					isLoop = FALSE;
				}
			}
			if (s_eenlLoopOut == 1) {
				DLOG_Debug("*****s_eenlLoopOut recv(u)*****");
				pthread_testcancel();
				isLoop = FALSE;
			} else {
				/* DO NOTHING */
			}
		}

		close(fd);
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:メッセージの受信(UDP) IPv6								*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cp	connectフラグ								*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_ThreadRecv6(const schar* cp)
{
	sint fd2 = -1;
	sint nfds = 0;
	slong i = 0;
	slong num = 0;
	slong len = 0;
	socklen_t addrLen = 0;
	uchar buf[EENL_MAX_MESSAGE_LENGTH] = { 0u };
	schar nm[INET6_ADDRSTRLEN] = { 0 };
	bool_t isLoop = TRUE;
	EenlAddr_t addr;
	fd_set rfds, efds;
	struct timeval tv;

	DEBUG_TRCIN();

	/* 未使用引数 */
	EENL_UNUSED_PARA(cp);

	DLOG_Debug("***** eenl_ThreadRecv6 in *****");
	fd2 = eenl_CreateSocketRecv6(NULL);
	if (fd2 < 0) {
		DLOG_Debug("recv6 socket : %ld", fd2);
	} else {
		DLOG_Debug("***** eenl_ThreadRecv6 in (%ld) *****", fd2);

		while (isLoop) {
			pthread_testcancel();
			FD_ZERO(&rfds);
			FD_SET(fd2, &rfds);
			FD_SET(s_eenlFdQuit[EENL_IDX_FD_EVT_READ], &rfds);
			efds = rfds;
			if (fd2 > s_eenlFdQuit[EENL_IDX_FD_EVT_READ]) {
				nfds = fd2;
			} else {
				nfds = s_eenlFdQuit[EENL_IDX_FD_EVT_READ];
			}

			/* タイムアウト:5秒 */
			tv.tv_sec = EENL_THREAD_TIMEOUT;
			tv.tv_usec = 0;

			num = select(nfds + 1, &rfds, NULL, &efds, &tv);
			DLOG_Debug("recv6 select: %ld[%ld, %ld]",
				num, tv.tv_sec, tv.tv_usec);
			if (num <= 0) {
				/* タイムアウト、エラー時 */
				/* 何もせずに次の処理へ */
				DLOG_Debug("recv(v6) timeout");
			} else {
				if (FD_ISSET(fd2, &rfds)) {
					pthread_testcancel();
					addrLen = sizeof(addr.v6addr);
					addr.ipType = EENL_IP_TYPE_V6;
					addr.protoType = EENL_PROTO_TYPE_UDP;
					addr.tcpSock = -1;

					len = recvfrom(fd2, buf, sizeof(buf), 0,
						(struct sockaddr*) & (addr.v6addr), &addrLen);
					if (len <= 0) {
						/* 何もせずに次の処理へ */
						DLOG_Debug("recv(v6) recv: %ld", len);
					} else {
						/* IPv6アドレスを取得 */
						memset(&nm, 0, sizeof(nm));
						inet_ntop(AF_INET6, &addr.v6addr.sin6_addr,
							nm, sizeof(nm));

						DLOG_Debug("#### received (v6) from %s ####", nm);

						for (i = 0;i < len; i++) {
							DLOG_Debug("%02x ", buf[i]);
						}
						DLOG_Debug("\n");

						eenl_ProcessMessage(&addr, buf, len);
						pthread_testcancel();
					}
				} else {
					pthread_testcancel();
					isLoop = FALSE;
				}
			}

			if (s_eenlLoopOut == 1) {
				DLOG_Debug("*****s_eenlLoopOut recv(u6)*****");
				pthread_testcancel();
				isLoop = FALSE;
			} else {
				/* DO NOTHING */
			}
		}

		close(fd2);
	}
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:メッセージの受信(TCP IPv4/IPv6)						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　cp	connectフラグ								*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:IPv4は射影アドレスを使ってIPv6として扱う				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_ThreadTcpRecv(const schar* cp)
{
	sint fd = -1;
	uchar buf[EENL_MAX_MESSAGE_LENGTH] = { 0u };
	slong len = 0;
	socklen_t addrLen = 0;
	sint sock = -1;
	slong yes = 1;
	slong i = 0;
	schar nm[INET6_ADDRSTRLEN] = { 0 };
	slong ret = 0;
	bool_t isError = FALSE;
	bool_t isLoop = TRUE;
	struct sockaddr_in6 addr;
	EenlAddr_t client;

	DEBUG_TRCIN();

	/* 未使用引数 */
	EENL_UNUSED_PARA(cp);

	DLOG_Debug("+++++ eenl_ThreadTcpRecv in +++++");

	fd = socket(AF_INET6, SOCK_STREAM, 0);	/* TCP IPv6 */
	if (fd < 0) {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
		perror("{perr}tcprecv socket");
#endif
		DLOG_Error("+++++ tcprecv  socket error : %ld +++++", fd);
		isError = TRUE;
	} else {
		addr.sin6_family = AF_INET6;
		addr.sin6_addr = in6addr_any;
		addr.sin6_port = htons(EENL_TCP_PORT);

		ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
			(const schar*)&yes, sizeof(yes));
		if (ret < 0) {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
			perror("{perr}tcprecv reuse set");
#endif
			DLOG_Error("+++++ tcprecv  reuse set error : %ld +++++", ret);
			isError = TRUE;
		} else {
			ret = bind(fd, (struct sockaddr*) & addr, sizeof(addr));
			if (ret < 0) {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
				perror("{perr}tcprecv bind");
#endif
				DLOG_Error("+++++ tcprecv  bind error : %ld +++++", ret);
				isError = TRUE;
			} else {
				ret = listen(fd, 5);
				if (ret < 0) {
#if defined(EN_TRC_PRINTF) && EN_TRC_PRINTF != 0
					perror("{perr}tcprecv listen");
#endif
					DLOG_Error("+++++ tcprecv  listen error : %ld +++++", ret);
					isError = TRUE;
				} else {
					/* DO NOTHING */
				}

			}
		}
	}


	if (!isError) {
		DLOG_Debug("+++++ eenl_ThreadTcpRecv in +++++(%ld)", fd);

		while (isLoop) {
			pthread_testcancel();

			sock = accept(fd, (struct sockaddr*) & (client.v6addr),
				&addrLen);
			if (sock < 0) {
				/* 失敗、何もしない　*/
				pthread_testcancel();
				DLOG_Debug("tcprecv accept : %ld", sock);

			} else {
				len = recv(sock, buf, sizeof(buf), 0);
				if (len <= 0) {
					/* 失敗、何もしない　*/
					pthread_testcancel();
					DLOG_Debug("tcprecv recv : %ld", len);
				} else {
					/* IPv6アドレスを取得 */
					memset(&nm, 0, sizeof(nm));
					inet_ntop(AF_INET6, &client.v6addr.sin6_addr,
						nm, sizeof(nm));

					DLOG_Debug("++++ tcp received from %s ++++(%ld, %ld)",
						nm, fd, sock);
					for (i = 0; i < len; i++) {
						DLOG_Debug("%02x", buf[i]);
					}

					for (i = 0;i < len; i++) {
						DLOG_Debug("%02x ", buf[i]);
					}
					DLOG_Debug("\n");

					client.ipType = EENL_IP_TYPE_V6;
					client.protoType = EENL_PROTO_TYPE_TCP;
					client.tcpSock = sock;
					eenl_ProcessMessage(&client, buf, len);
					pthread_testcancel();

				}
				if (s_eenlLoopOut == 1) {
					DLOG_Debug("*****s_eenlLoopOut recv(t)*****");
					pthread_testcancel();
					isLoop = FALSE;
				} else {
					/* DO NOTHING */
				}
			}
			if (s_eenlLoopOut == 1) {
				DLOG_Debug("*****s_eenlLoopOut recv(t)*****");
				pthread_testcancel();
				isLoop = FALSE;
			} else {
				/* DO NOTHING */
			}
		}

		close(fd);
	}
	DEBUG_TRCOUT();
}


/*==========================================================================*/
/*	Description		:マルチキャストへの登録スレッド							*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_ThreadMulticast(void)
{
	struct addrinfo hints, * res = NULL;
	struct group_req greq;
	uchar enetIf = 0u;
	slong err = 0;
	schar wlanName[ENET_WLAN_IF_NAME_STR_LEN] = { 0 };

	DEBUG_TRCIN();

	memset(wlanName, 0, sizeof(wlanName));

	while (1)
	{
		DLOG_Debug("----- multicast loop -start- -----");

		enetIf = ENET_SyncGetNetworkType();
		if (enetIf == ENET_NETWORK_WIRED) {
			DLOG_Debug(" network Type: Wired(%d)", enetIf);
		} else if (enetIf == ENET_NETWORK_WIRELESS) {
			DLOG_Debug(" network Type: Wireless(%d)", enetIf);

			memset(wlanName, 0, sizeof(wlanName));

			err = ENET_SyncGetWlanIfName(wlanName);
			if (err == ENET_SUCCESS) {
				DLOG_Debug("get wlan name : %s", wlanName);
			} else {
				DLOG_Debug("get wlan name ERROR : %ld[%s] ", err, wlanName);
			}
		} else {
			DLOG_Debug(" network Type: %d", enetIf);
		}

		/* for IPv4 Multicast */
		memset(&hints, 0, sizeof(hints));

		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;

		err = getaddrinfo(EENL_MULTICAST_ADDRESS, "3610", &hints, &res);
		if (err == 0) {
			/* 正常 */

			if (enetIf == ENET_NETWORK_WIRED) {
				/* 有線の場合 */
				greq.gr_interface = if_nametoindex("eth0");
				memcpy(&greq.gr_group, res->ai_addr, res->ai_addrlen);

				/* マルチキャストからの脱退 */
				setsockopt(s_eenlRv4, IPPROTO_IP, MCAST_LEAVE_GROUP,
					(schar*)&greq, sizeof(greq));
				DLOG_Debug("ipv4 multicast(eth0) leave");

				/* マルチキャストへの参加 */
				setsockopt(s_eenlRv4, IPPROTO_IP, MCAST_JOIN_GROUP,
					(schar*)&greq, sizeof(greq));
				DLOG_Debug("ipv4 multicast(eth0) join");

			} else if (enetIf == ENET_NETWORK_WIRELESS) {
				/* 無線の場合 */
				greq.gr_interface = if_nametoindex(wlanName);
				memcpy(&greq.gr_group, res->ai_addr, res->ai_addrlen);

				/* マルチキャストからの脱退 */
				setsockopt(s_eenlRv4, IPPROTO_IP, MCAST_LEAVE_GROUP,
					(schar*)&greq, sizeof(greq));
				DLOG_Debug("ipv4 multicast(wlan) leave");

				/* マルチキャストへの参加 */
				setsockopt(s_eenlRv4, IPPROTO_IP, MCAST_JOIN_GROUP,
					(schar*)&greq, sizeof(greq));
				DLOG_Debug("ipv4 multicast(wlan) join");

			} else {
				/* 有線、無線以外の場合、何もしない */

			}

		} else {
			/* error */
			DLOG_Error("ipv4 getaddr error!: %ld", err);
		}

		freeaddrinfo(res);

		/* for IPv6 Multicast */
		memset(&hints, 0, sizeof(hints));

		hints.ai_family = AF_INET6;
		hints.ai_socktype = SOCK_DGRAM;

		err = getaddrinfo(EENL_MULTICAST_ADDRESS_V6, "3610", &hints, &res);
		if (err == 0) {
			/* 正常 */

			if (enetIf == ENET_NETWORK_WIRED) {
				/* 有線の場合 */
				greq.gr_interface = if_nametoindex("eth0");
				memcpy(&greq.gr_group, res->ai_addr, res->ai_addrlen);

				/* マルチキャストからの脱退 */
				setsockopt(s_eenlRv6, IPPROTO_IPV6, MCAST_LEAVE_GROUP,
					(schar*)&greq, sizeof(greq));
				DLOG_Debug("ipv6 multicast(eth0) leave");

				/* マルチキャストへの参加 */
				setsockopt(s_eenlRv6, IPPROTO_IPV6, MCAST_JOIN_GROUP,
					(schar*)&greq, sizeof(greq));
				DLOG_Debug("ipv6 multicast(eth0) join");

			} else if (enetIf == ENET_NETWORK_WIRELESS) {
				/* 無線の場合 */
				greq.gr_interface = if_nametoindex(wlanName);
				memcpy(&greq.gr_group, res->ai_addr, res->ai_addrlen);

				/* マルチキャストからの脱退 */
				setsockopt(s_eenlRv6, IPPROTO_IPV6, MCAST_LEAVE_GROUP,
					(schar*)&greq, sizeof(greq));
				DLOG_Debug("ipv6 multicast(wlan) leave");

				/* マルチキャストへの参加 */
				setsockopt(s_eenlRv6, IPPROTO_IPV6, MCAST_JOIN_GROUP,
					(schar*)&greq, sizeof(greq));
				DLOG_Debug("ipv6 multicast(wlan) join");

			} else {
				/* 有線、無線以外の場合、何もしない */

			}

		} else {
			/* error */
			DLOG_Error("ipv6 getaddr error!: %ld", err);
		}

		freeaddrinfo(res);

		DLOG_Debug("----- multicast loop -end- -----");

		sleep(EENL_MULTICAST_LOOP);
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:EPC設定済フラグ取得									*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　instance	インスタンスハンドル					*/
/*					:[in]　epc プロパティコード								*/
/*--------------------------------------------------------------------------*/
/*	return			:取得結果												*/
/*					 (FALSE:設定無し/TRUE:設定済み)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/01 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static bool_t eenl_GetEpcValidFlg(void* instance, uchar epc)
{
	bool_t	ret = FALSE;

	DEBUG_TRCIN();

	if (NULL != instance) {
		/* ノード */
		if (instance == g_eenlDevNode) {
			ret = s_eenlNodEpcValidFlg[epc];
			DLOG_Debug("instance:0x%08lX(node), epc:%02X flg:%d",
				(ulong)instance, epc, ret);
		}
		/* 蓄電池 */
		else if (instance == g_eenlDevBattery) {
			ret = s_eenlBatEpcValidFlg[epc];
			DLOG_Debug("instance:0x%08lX(battery), epc:%02X flg:%d",
				(ulong)instance, epc, ret);
		}
		/* 太陽光 */
		else if (instance == g_eenlDevSolar) {
			ret = s_eenlSolEpcValidFlg[epc];
			DLOG_Debug("instance:0x%08lX(solar), epc:%02X flg:%d",
				(ulong)instance, epc, ret);
		}
		/* 車両充放電器 */
		else if (instance == g_eenlDevV2h) {
			ret = s_eenlV2hEpcValidFlg[epc];
			DLOG_Debug("instance:0x%08lX(v2h), epc:%02X flg:%d",
				(ulong)instance, epc, ret);
		}
		/* マルチ入力PCS */
		else if (instance == g_eenlDevMpcs) {
			ret = s_eenlMpcsEpcValidFlg[epc];
			DLOG_Debug("instance:0x%08lX(mpcs), epc:%02X flg:%d",
				(ulong)instance, epc, ret);
		} else {
			/* DO NOTHING */
			DLOG_Error("unknown device!");
		}
	} else {
		DLOG_Error("device is NULL!");
	}

	DEBUG_TRCOUT();
	return ret;
}


/*==========================================================================*/
/*	Description		:dispose device											*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]info	デバイス情報								*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:functions take in the difference						*/
/*					:of device and controllerothers							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_DisposeDevice(EenlDeviceInfo_t* info)
{
	ENL_MESSAGE* msg = NULL;

	DEBUG_TRCIN();

	if (info != NULL) {
		eenl_UnregisterDeviceInfo(info);

		if (info->devBattery != NULL) {
			ENL_DEV_unregister_instance(&msg, info->node, info->devBattery);
			ENL_dispose_instance(info->devBattery);
			if (msg != NULL) {
				EENL_SendMessage(NULL, msg);
			} else {
				/* DO NOTHING */
			}
		} else {
			/* DO NOTHING */
		}

		if (info->devSolar != NULL) {
			ENL_DEV_unregister_instance(&msg, info->node, info->devSolar);
			ENL_dispose_instance(info->devSolar);
			if (msg != NULL) {
				EENL_SendMessage(NULL, msg);
			} else {
				/* DO NOTHING */
			}
		} else {
			/* DO NOTHING */
		}

		if (info->devV2h != NULL) {
			ENL_DEV_unregister_instance(&msg, info->node, info->devV2h);
			ENL_dispose_instance(info->devV2h);
			if (msg != NULL) {
				EENL_SendMessage(NULL, msg);
			} else {
				/* DO NOTHING */
			}
		} else {
			/* DO NOTHING */
		}

		ENL_dispose_instance(info->node);

		EENL_EventSet(info, EENL_EVENT_DISPOSE, 0);

		pthread_join(info->th, NULL);

		pthread_cond_destroy(&info->cond);
		pthread_mutex_destroy(&info->mutex);

		CMN_FREE(info);
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
}


/*==========================================================================*/
/*	Description		:process message										*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]addr	アドレス									*/
/*					:[in]buf	メッセージデータ							*/
/*					:[in]len	メッセージデータ長							*/
/*--------------------------------------------------------------------------*/
/*	return			:無し													*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static void eenl_ProcessMessage(EenlAddr_t* addr, uchar* buf, slong len)
{
	EenlDeviceInfo_t* info = NULL;
	ENL_MESSAGE* msg = NULL;

	DEBUG_TRCIN();

	info = EENL_GetDeviceInfoIndex(0);

	if (info != NULL) {
		ENL_DEV_process_message(&msg, info->node, buf, (sint)len);
		if (msg != NULL) {

			addr->v4addr.sin_port = htons(EENL_UDP_PORT);
			addr->v6addr.sin6_port = htons(EENL_UDP_PORT);
			EENL_SendMessage(addr, msg);
		} else {
			/* DO NOTHING */
		}
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
}


/*==========================================================================*/
/*	Description		:文字列が数値文字列かチェックする						*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　digitStr 判定する文字列							*/
/*	param			:[in]　strSize 文字数									*/
/*--------------------------------------------------------------------------*/
/*	return			:判定結果												*/
/*					 (TRUE:数字/FALSE:非数字								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/13 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static bool_t eenl_IsStrDigit(schar* digitStr, slong strSize)
{
	/* 変数宣言 */
	slong i = 0;
	bool_t bRet = TRUE;

	DEBUG_TRCIN();

	/* 引数チェック */
	if (digitStr == NULL || strSize <= 0) {
		DEBUG_TRCOUT();
		return FALSE;
	}

	/* 判定ループ */
	for (i = 0; i < strSize; i++) {

		/* 数値チェック */
		if (!isdigit((slong) * (digitStr + i))) {

			/* 末端で\0の場合 */
			if (i == (strSize - 1) && *(digitStr + i) == '\0') {
				/* DO NOTHING */
			} else {
				bRet = FALSE;
			}
		} else {
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();
	return bRet;
}

/*==========================================================================*/
/*	Description		:充放電時間帯の入力が正しいかチェックする				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　hourMinute 充放電時刻構造体						*/
/*--------------------------------------------------------------------------*/
/*	return			:判定結果												*/
/*					 (EENL_RET_CODE_OK:正常終了/EENL_RET_CODE_FAIL:異常		*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/13 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_CheckChargeTimeZoneUWfunc(EenlHourMinute_t* hourMinute)
{
	/* 変数宣言 */
	ulong chargeStartTime = 0u;					/* 充電開始時刻 */
	ulong chargeStopTime = 0u;					/* 充電停止時刻 */
	ulong disCrgStartTime = 0u;					/* 放電開始時刻 */
	ulong disCrgStopTime = 0u;					/* 放電停止時刻 */
	ulong chargeTime = 0u;						/* 充電時間合計 */
	ulong disCrgTime = 0u;						/* 放電時間合計 */
	slong retCode = EENL_RET_CODE_OK;			/* 戻り値 */

	DEBUG_TRCIN();

	/* 時間上限チェック ※引数がアンサインドなので、下限チェックは不要 */
	if ((hourMinute->cStartHH >= CMN_HOUR_MAX)
		|| (hourMinute->cStopHH >= CMN_HOUR_MAX)
		|| (hourMinute->dStartHH >= CMN_HOUR_MAX)
		|| (hourMinute->dStopHH >= CMN_HOUR_MAX)
		|| (hourMinute->cStartMM >= CMN_MIN_MAX)
		|| (hourMinute->cStopMM >= CMN_MIN_MAX)
		|| (hourMinute->dStartMM >= CMN_MIN_MAX)
		|| (hourMinute->dStopMM >= CMN_MIN_MAX)) {
		DLOG_Error("ERR! TimeZone upper limit exceeded");
		DEBUG_TRCOUT();
		retCode = EENL_RET_CODE_FAIL;
	} else {
		/* 指定された時分を分に変換 */
		chargeStartTime = (ulong)((hourMinute->cStartHH * CMN_MIN_MAX)
			+ hourMinute->cStartMM);
		chargeStopTime = (ulong)((hourMinute->cStopHH * CMN_MIN_MAX)
			+ hourMinute->cStopMM);
		disCrgStartTime = (ulong)((hourMinute->dStartHH * CMN_MIN_MAX)
			+ hourMinute->dStartMM);
		disCrgStopTime = (ulong)((hourMinute->dStopHH * CMN_MIN_MAX)
			+ hourMinute->dStopMM);

		/* 変換値ログ出力 */
		DLOG_Debug("charge    start time = %02d:%02d -> %ld",
			hourMinute->cStartHH, hourMinute->cStartMM, chargeStartTime);
		DLOG_Debug("charge    stop  time = %02d:%02d -> %ld",
			hourMinute->cStopHH, hourMinute->cStopMM, chargeStopTime);
		DLOG_Debug("discharge start time = %02d:%02d -> %ld",
			hourMinute->dStartHH, hourMinute->dStartMM, disCrgStartTime);
		DLOG_Debug("discharge stop  time = %02d:%02d -> %ld",
			hourMinute->dStopHH, hourMinute->dStopMM, disCrgStopTime);

		/* 充電 時間帯チェック */
		if (chargeStopTime < chargeStartTime) {

			/* 日付をまたいだ指定と判断して、充電終了時間を+1440(24h*60)する */
			chargeStopTime = chargeStopTime + CMN_ADD_MIN_DAY;

			/* 放電開始（翌日分）と充電停止時間の比較 */
			if ((disCrgStartTime + CMN_ADD_MIN_DAY) <= chargeStopTime) {

				/* 日付をまたいだ充電時間と翌日の放電開始時間がかぶっているのでNG */
				DLOG_Debug("FALSE : Overlap time zone (1)");
				retCode = EENL_RET_CODE_FAIL;
			} else {
				/* DO NOTHING */
			}
		} else if (chargeStopTime == chargeStartTime) {
			/* 充電開始時間と充電停止時間が同じ場合 */

#ifdef WORK_KYC /* 京セラ案件固有 */
		/* 充電開始時間と充電停止時間の重複可能 */
		/* DO NOTHING */
#else	/* ニチコン案件固有 */
		/* 充電開始時間と充電停止時間の重複不可 */
			DLOG_Debug("FALSE : Overlap time zone (2)");
			retCode = EENL_RET_CODE_FAIL;
#endif
		} else {
			/* 充電開始よりも充電停止が後の場合 */
			/* DO NOTHING */
		}

		/* 放電 時間帯チェック */
		if (disCrgStopTime < disCrgStartTime) {

			/* 日付をまたいだ指定と判断して、放電終了時間を+1440(24h*60)する */
			disCrgStopTime = disCrgStopTime + CMN_ADD_MIN_DAY;

			/* 充電開始が放電停止以前だった場合 */
			if ((chargeStartTime + CMN_ADD_MIN_DAY) <= disCrgStopTime) {

				/* 日付をまたいだ放電時間と翌日の充電開始時間がかぶっているのでNG */
				DLOG_Debug("FALSE : Overlap time zone (3)");
				retCode = EENL_RET_CODE_FAIL;
			} else {
				/* DO NOTHING */
			}
		} else if (disCrgStopTime == disCrgStartTime) {
			/* 放電開始時間と放電停止時間が同じ場合、NG */
			DLOG_Debug("FALSE : Overlap time zone (4)");
			retCode = EENL_RET_CODE_FAIL;
		} else {
			/* 放電開始よりも放電停止が後の場合 */
			/* DO NOTHING */
		}

		/* 充放電時間帯のチェック */
		if (((chargeStartTime < disCrgStartTime)
			&& (disCrgStartTime <= chargeStopTime))
			|| ((chargeStartTime > disCrgStartTime)
				&& (disCrgStopTime >= chargeStartTime))
			|| ((chargeStartTime == disCrgStartTime)
				|| (chargeStartTime == disCrgStopTime))
			|| ((chargeStopTime == disCrgStartTime)
				|| (chargeStopTime == disCrgStopTime))) {
			/*重複あり */
			DLOG_Debug("FALSE : Overlap time zone (5)");
			retCode = EENL_RET_CODE_FAIL;

		} else {
			/* 充電時間算出 */
			chargeTime = chargeStopTime - chargeStartTime;

			/* 放電時間算出 */
			disCrgTime = disCrgStopTime - disCrgStartTime;

			/* 充放電時間合計(秒換算)のチェック */
			if (((chargeTime + disCrgTime) * 60) > CMN_REFERENCE_HOURSEC) {
				/* 充放電時間合計制限オーバー */
				DLOG_Error("ERR! Charge Discharge Time upper limit exceeded");
				retCode = EENL_RET_CODE_FAIL;
			} else {
				/* DO NOTHING */
			}
		}

	}

	DLOG_Debug("TRUE : No overlap");
	DEBUG_TRCOUT();
	return retCode;
}


/*==========================================================================*/
/*	Description		:充放電時間帯の入力が正しいかチェックする				*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　epcsTimeZone 充放電時間帯						*/
/*--------------------------------------------------------------------------*/
/*	return			:判定結果												*/
/*					 (EENL_RET_CODE_OK:正常終了/EENL_RET_CODE_FAIL:異常		*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/13 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
static slong eenl_CheckChargeTimeZonefunc(EPCS_CHARGE_TIME_ZONE_T* epcsTimeZone)
{
	/* 変数宣言 */
	EenlHourMinute_t hourMinute;
	schar* savep = NULL;
	slong retCode = EENL_RET_CODE_OK;		/* 戻り値 */

	DEBUG_TRCIN();

	/* 引数チェック */
	if (epcsTimeZone == NULL) {
		DLOG_Error("Err! epcsTimeZone is NULL");
		DEBUG_TRCOUT();
		return EENL_RET_CODE_FAIL;
	} else {
		/* 充電時間帯数値判定 */
		if (!eenl_IsStrDigit(epcsTimeZone->chargeStartTime.hour, CMN_HOUR_LEN)
			|| !eenl_IsStrDigit(epcsTimeZone->chargeStartTime.min, CMN_MIN_LEN)
			|| !eenl_IsStrDigit(epcsTimeZone->chargeStopTime.hour, CMN_HOUR_LEN)
			|| !eenl_IsStrDigit(epcsTimeZone->chargeStopTime.min, CMN_MIN_LEN)) {
			DLOG_Error("Err! chargeStartTime non-numeric : %s%s - %s%s"
				, epcsTimeZone->chargeStartTime.hour,
				epcsTimeZone->chargeStartTime.min
				, epcsTimeZone->chargeStopTime.hour,
				epcsTimeZone->chargeStopTime.min);
			retCode = EENL_RET_CODE_FAIL;
		} else {
			/* 放電時間帯数値判定 */
			if (!eenl_IsStrDigit(epcsTimeZone->dischargeStartTime.hour, CMN_HOUR_LEN)
				|| !eenl_IsStrDigit(epcsTimeZone->dischargeStartTime.min, CMN_MIN_LEN)
				|| !eenl_IsStrDigit(epcsTimeZone->dischargeStopTime.hour, CMN_HOUR_LEN)
				|| !eenl_IsStrDigit(epcsTimeZone->dischargeStopTime.min, CMN_MIN_LEN)) {
				DLOG_Error("Err! dischargeStopTime non-numeric : %s%s - %s%s"
					, epcsTimeZone->dischargeStartTime.hour,
					epcsTimeZone->dischargeStartTime.min
					, epcsTimeZone->dischargeStopTime.hour,
					epcsTimeZone->dischargeStopTime.min);
				retCode = EENL_RET_CODE_FAIL;
			} else {
				/* 数値化 */
				/* 充電開始時刻(HH) */
				hourMinute.cStartHH = (uchar)strtol(epcsTimeZone->chargeStartTime.hour,
					&savep, CMN_STRTOL_BASE_DEC);
				/* 充電開始時刻(MM) */
				hourMinute.cStartMM = (uchar)strtol(epcsTimeZone->chargeStartTime.min,
					&savep, CMN_STRTOL_BASE_DEC);
				/* 充電停止時刻(HH) */
				hourMinute.cStopHH = (uchar)strtol(epcsTimeZone->chargeStopTime.hour,
					&savep, CMN_STRTOL_BASE_DEC);
				/* 充電停止時刻(MM) */
				hourMinute.cStopMM = (uchar)strtol(epcsTimeZone->chargeStopTime.min,
					&savep, CMN_STRTOL_BASE_DEC);

				/* 放電開始時刻(HH) */
				hourMinute.dStartHH = (uchar)strtol(epcsTimeZone->dischargeStartTime.hour,
					&savep, CMN_STRTOL_BASE_DEC);
				/* 放電開始時刻(MM) */
				hourMinute.dStartMM = (uchar)strtol(epcsTimeZone->dischargeStartTime.min,
					&savep, CMN_STRTOL_BASE_DEC);
				/* 放電停止時刻(HH) */
				hourMinute.dStopHH = (uchar)strtol(epcsTimeZone->dischargeStopTime.hour,
					&savep, CMN_STRTOL_BASE_DEC);
				/* 放電停止時刻(MM) */
				hourMinute.dStopMM = (uchar)strtol(epcsTimeZone->dischargeStopTime.min,
					&savep, CMN_STRTOL_BASE_DEC);
			}
		}
	}


	/* 重複判定 */
	if (EENL_RET_CODE_OK == retCode) {
		retCode = eenl_CheckChargeTimeZoneUWfunc(&hourMinute);
	} else {
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();
	return retCode;
}
