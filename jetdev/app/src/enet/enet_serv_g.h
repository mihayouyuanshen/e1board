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
#ifndef __ENET_SERV_G_H__
#define __ENET_SERV_G_H__
/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "enet_g.h"
#include "enet_cmn_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define ENET_KEY_IPV4_ADDR_AUTO "IPv4_auto" /* IPv4自動設定されたアドレス */
#define ENET_KEY_IPV4_ADDR_SELF "IPv4_self" /* IPv4手動設定したアドレス */
#define ENET_ERRCODE_SET_ROUTER_COMM 	 	"BC551"
#define ENET_ERRCODE_SET_ROUTER_COMM_RATE	'M' /* uchar */
#define ENET_ERRCODE_UNSET_ROUTER_COMM 		"BE000"
#define ENET_ERRCODE_UNSET_ROUTER_COMM_RATE	'0' /* uchar */
#define ENET_THREAD_START_DELAY (ushort)(2000u) /* 初回タイミングを遅らせてログの混在を防止 */
#define ENET_IP_WATCH_INTERVAL      (ushort)(30000u)
#define ENET_IP_WATCH_INTERVAL_MIN  (ushort)(10000u)
/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern EnetNetworkType_t ENET_GetPrefNetworkType(void);
extern bool_t ENET_SetPrefNetworkType(EnetNetworkType_t type);
extern schar* ENET_GetPrefNetworkTypeStr(void);
extern schar* ENET_GetNetworkTypeStrTbl(EnetNetworkType_t type);
extern bool_t ENET_SetPrefIPv4AutoFlg(schar autoFlg);
extern schar ENET_GetPrefIPv4AutoFlg(void);
extern bool_t ENET_SetPrefIPv4fmtIP(const schar* key, const EnetIpv4_t* val);
extern bool_t ENET_GetPrefIPv4fmtIP(const schar* key, EnetIpv4_t* val);
extern bool_t ENET_SetPrefWlanSetting(EnetWlanSettings_t* wlan);
extern bool_t ENET_GetPrefWlanSetting(EnetWlanSettings_t* wlan);
extern bool_t ENET_SetPrefPingTimeout(slong timeMs);
extern slong ENET_GetPrefPingTimeout(void);
extern bool_t ENET_SetPrefDhcpRetries(slong cnt);
extern slong ENET_GetPrefDhcpRetries(void);
extern bool_t ENET_SetPrefDhcpTimeout(slong timeSec);
extern slong ENET_GetPrefDhcpTimeout(void);
extern bool_t ENET_SetPrefDhcpTryagain(slong timeSec);
extern slong ENET_GetPrefDhcpTryagain(void);
extern bool_t ENET_SetPrefIpWatchInterval(slong timeMs);
extern slong ENET_GetPrefIpWatchInterval(void);
extern bool_t ENET_SetPrefSendPingInterval(slong timeMs);
extern slong ENET_GetPrefSendPingInterval(void);
extern bool_t ENET_SetPrefChgEvntFlag(ulong port, ulong flag);
extern ulong ENET_GetPrefChgEvntFlag(ulong port);
extern slong ENET_GetInterfaceNameIndex(slong index, schar* interfaceName);
extern void ENET_ResetApDevice(void);
extern void ENET_SetRmcErrCode(schar* errCode);
extern bool_t ENET_InitPrefHostapd(EnetWlanSettings_t* wlan);
extern ushort ENET_IcmpChksum(ushort *buf, slong bufsz);
extern slong ENET_GetDefaultGW(const schar* ifaceName, EnetIpv4_t* gateway);
extern slong ENET_GetIpAddr(const schar* ifaceName, void* settings, const EnetIpType_t ipType);
extern slong ENET_UpdateNetworkType(void);
extern slong ENET_InitIpAddr(const EnetNetworkType_t nowType, const EnetIpType_t ipType);
extern slong ENET_ScanEssidLfwASyncRet(ulong cPort); /* callback from Thread */
extern slong ENET_ChkRouterConnectionLfwASyncRet(ulong cPort, slong result); /* callback from Thread */
extern slong ENET_GetWlanStatus(void);
extern void ENET_SendChgEvnt(ulong flag);
extern void ENET_ProcStart(slong fact);
extern slong ENET_GetProxy(EnetProxy_t* proxy);
extern slong ENET_GetWlanIfName(schar* ifName);
extern slong ENET_RmvChgEvntListener(ulong port);
extern slong ENET_SetNetworkTypeLfwRet(EnetFncNetworkType_t* fncNType);
extern slong ENET_GetIpAddr4LfwRet(EnetIpv4Settings_t* fncIPv4);
extern slong ENET_SetIpAddr4LfwRet(EnetFncIpv4_t* fncIPv4);
extern slong ENET_GetIPAddr4SettingLfwRet(EnetIpv4Settings_t* fncIPv4);
extern slong ENET_GetIpAddr6LfwRet(EnetIpv6Settings_t* fncIPv6);
extern slong ENET_SetIpAddr6LfwRet(EnetFncIpv6_t* fncIPv6);
extern slong ENET_SetProxyLfwRet(EnetFncProxy_t* fncProxy);
extern void ENET_ScanEssidLfwASync(EnetFnc00_t* fnc);
extern slong ENET_SetWlanSettingLfwRet(EnetFncWlan_t* fncWlan);
extern slong ENET_SetWlanUpLfwRet(EnetFncBool_t* fncBool);
extern void ENET_ChkRouterConnectionLfwASync(EnetFnc00_t* fnc);
extern slong ENET_SetChgEvntListenerLfwRet(EnetFncListenerPort_t* fncPort);
extern slong ENET_GetWlanSetting(EnetWlanSettings_t* getWlan);

#endif /*__ENET_SERV_G_H__*/
