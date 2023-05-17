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
#ifndef __ENET_SERV_H__
#define __ENET_SERV_H__
/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "enet_cmn_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define ENET_KEY_NETWORK_TYPE   "networkType" /* 現在のネットワーク種別 */
#define ENET_KEY_IPV4_IS_AUTO   "IPv4_isAuto" /* IPv4設定が自動 or 手動 */
#define ENET_KEY_SUBNET         "subnet"  /* 手動設定したサブネットマスク */
#define ENET_KEY_GATEWAY        "gateway" /* 手動設定したゲートウェイ */
#define ENET_KEY_DNS1           "dns1" /* 手動設定したDNS1 */
#define ENET_KEY_DNS2           "dns2" /* 手動設定したDNS2 */
#define ENET_KEY_PROXY_HOST     "HOST" /* Proxyのホストアドレス */
#define ENET_KEY_PROXY_PORT     "PORT" /* Proxyのポート番号 */
#define ENET_KEY_PROXY_ENABLE   "proxyEnable" /* Proxyが有効 or 無効 */
#define ENET_KEY_WLAN_ESSID     "ssid" /* 無線接続のESSID */
#define ENET_KEY_WLAN_PASS      "psk"  /* 無線接続のパスキー */
#define ENET_VAL_KEYMGMT        "WPA-PSK"
#define ENET_KEY_PING_TIMEOUT   "pingTimeout" /* ルータ疎通確認用Pingのタイムアウト時間 */
#define ENET_PING_TIMEOUT       (ushort)(1000u) /* ルータ疎通確認用Pingのタイムアウト時間[ms]兼最短送信時間 */
#define ENET_KEY_DHCP_RETRIES   "dhcpRetries"
#define ENET_KEY_DHCP_TIMEOUT   "dhcpTimeout"
#define ENET_KEY_DHCP_TRYAGAIN  "dhcpTryagain"
#define ENET_DHCP_RETRIES       (uchar)(3u)
#define ENET_DHCP_TIMEOUT       (uchar)(5u)
#define ENET_DHCP_TRYAGAIN      (uchar)(60u)
#define ENET_KEY_IP_WATCH_INTERVAL  "ipWatch"
#define ENET_KEY_SEND_PING_INTERVAL "sendPing"
#define ENET_SEND_PING_INTERVAL     (ushort)(30000u)
#define ENET_SEND_PING_INTERVAL_MIN (ushort)(10000u)

#define ENET_HOSTAPD_CTRL_IF "/var/run/hostapd"

#define ENET_DEFAULT_SUBNET  (ulong)(0x00FFffFF) /* 255.255.255.0 */
#define ENET_DEFAULT_IP      (ulong)(0xCB01A8C0) /* 192.168.1.203 */
#define ENET_DEFAULT_GATEWAY (ulong)(0x0101A8C0) /* 192.168.1.1 */
#define ENET_DEFAULT_DNS1    (ulong)(0x0101A8C0) /* 192.168.1.1 */
#define ENET_DEFAULT_DNS2    (ulong)(0x0101A8C0) /* 192.168.1.1 */
/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
/* 無線LANデバイス名オフセット */
typedef enum {
	ENET_WLAN_OFS_RA0 = 0,
	ENET_WLAN_OFS_WLAN0,
	ENET_WLAN_OFS_MAX
}EnetWlanOffset_t;
/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
static bool_t enet_RmvPrefChgEvntFlag(ulong port);
static slong enet_SendChgEvntLfwEvent(ulong port, ulong flag);
static EnetDeviceState_t enet_GetDeviceState(const schar* ifaceName);
static slong enet_SetLinkMode(const schar* ifaceName, const EnetLinkMode_t link);
static slong enet_SetWlanSetting(EnetWlanSettings_t* setWlan, EnetWlanSetExt_t ext);
static slong enet_SetWlanUp(bool_t up);
static slong enet_GetDNS(EnetIpv4Settings_t* v4);
static slong enet_SetDNS(EnetIpv4Settings_t* v4);
static slong enet_SetProxy(EnetProxy_t* proxy);
static slong enet_SetIPv4Self(const schar* ifaceName, EnetIpv4Settings_t* v4);
static slong enet_SetIPAddr(const schar* ifaceName, void* settings, const EnetIpType_t ipType);
static slong enet_SetNetworkType(const EnetNetworkType_t type, const EnetNettypeExt_t ext);
static slong enet_ChkRouterConnection(ulong cPort);
static slong enet_SetChgEvntListener(ulong port, ulong eventFlag);
static void enet_InitSendChgEvntProcArr(void);
static bool_t enet_GetDeviceExist(const schar* ifaceName);
static schar* enet_GetDeviceName(EnetNetworkType_t type);

#endif /*__ENET_SERV_H__*/
