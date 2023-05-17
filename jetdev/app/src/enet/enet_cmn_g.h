/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Cao.HZ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/05/08												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __ENET_CMN_G_H__
#define __ENET_CMN_G_H__
/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "lfw_g.h"
#include "frm_g.h"
#include "task_g.h"
#include "typedef.h"
#include "common_g.h"
#include "ewdt.h"
#include "etim_g.h"
#include "enet_g.h"
#include "esys_g.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/net.h>
#include <linux/wireless.h>
#include <linux/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/inotify.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>


#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <net/route.h>
#include <ifaddrs.h>
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define ENET_TRUE  (uchar)(1u)
#define ENET_FALSE (uchar)(0u)

#define ENET_SUCCESS (uchar)(0u)
#define ENET_ERR1 (schar)(-1)
#define ENET_ERR2 (schar)(-2)
#define ENET_ERR3 (schar)(-3)

#define ENET_PROC_NET_ROUTE "/proc/net/route"
#define ENET_ROUTE_INFO_ELEMENTS (uchar)(11u)

#define ENET_WLAN_STAT_STR_LEN  (uchar)(15u) /* 無線状態出力csv最大文字列長 */
#define ENET_WLAN_STAT_MAX      (uchar)(100u) /* 無線LAN状態(電波強度)最大値 */
#define ENET_PROXY_PORT_MAX     (ushort)(65535u) /* PROXYポート最大値 */
#define ENET_IPV4_IS_AUTO (uchar)(1u) /* IPv4が自動設定 */
#define ENET_WDT_TIMEOUT (uchar)(10u) /* プロセス監視タイムアウト */
#define ENET_FILE_AP_CSV  "/mnt/ram/enet_accesspoint.csv" /* scanEssid結果 */

/* for DNS */
#define ENET_ETC_RESOLV_CONF "/etc/resolv.conf"
#define ENET_NAMESERVER "nameserver"
#define ENET_DNS_INFO_ELEMENTS (uchar)(5u)
#define ENET_DNS_RESOLVER_LEN  (uchar)(10u)

#define ENET_BUF_STR_LEN    (ushort)(256u) /* 汎用バッファサイズ */ 
#define ENET_CHAR_NULL      '\0' /* 0x00 */

/* デバイス名称 */
#define ENET_DEVICE_NAME_UNSET	""
#define ENET_DEVICE_NAME_ETH0	"eth0"
#define ENET_DEVICE_NAME_RA0	"ra0"
#define ENET_DEVICE_NAME_WLAN0	"wlan0"

/* ログ出力用 */
#define ENET_RESET   "\033[0m"
#define ENET_BLACK   "\033[30m" /* Black */
#define ENET_RED     "\033[31m" /* Red */
#define ENET_GREEN   "\033[32m" /* Green */
#define ENET_YELLOW  "\033[33m" /* Yellow */
#define ENET_BLUE    "\033[34m" /* Blue */
#define ENET_MAGENTA "\033[35m" /* Magenta */
#define ENET_CYAN    "\033[36m" /* Cyan */
#define ENET_WHITE   "\033[37m" /* White */
#define ENET_BOLDBLACK   "\033[1m\033[30m" /* Bold Black */
#define ENET_BOLDRED     "\033[1m\033[31m" /* Bold Red */
#define ENET_BOLDGREEN   "\033[1m\033[32m" /* Bold Green */
#define ENET_BOLDYELLOW  "\033[1m\033[33m" /* Bold Yellow */
#define ENET_BOLDBLUE    "\033[1m\033[34m" /* Bold Blue */
#define ENET_BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define ENET_BOLDCYAN    "\033[1m\033[36m" /* Bold Cyan */
#define ENET_BOLDWHITE   "\033[1m\033[37m" /* Bold White */

#define DEBUG_PRINT(lvl, fmt, ...)

#ifdef PRINT_FATAL
#define LOGF(fmt, ...) printf(ENET_RED"(FATAL__) %4d:%s : " fmt""ENET_RESET, __LINE__, __FUNCTION__, ## __VA_ARGS__); fflush(stdout);
#else
#define LOGF(fmt, ...)
#endif

#ifdef PRINT_ERROR
#define LOGE(fmt, ...) printf(ENET_YELLOW"(ERROR__) %4d:%s : " fmt""ENET_RESET, __LINE__, __FUNCTION__, ## __VA_ARGS__); fflush(stdout);
#else
#define LOGE(fmt, ...)
#endif

#ifdef PRINT_NORMAL
#define LOGN(fmt, ...) printf("(NORMAL_) %4d:%s : " fmt, __LINE__, __FUNCTION__, ## __VA_ARGS__); fflush(stdout);
#else
#define LOGN(fmt, ...)
#endif

#ifdef PRINT_DEBUG
#define LOGD(fmt, ...) printf(ENET_MAGENTA"(DEBUG__) %4d:%s : " fmt""ENET_RESET, __LINE__, __FUNCTION__, ## __VA_ARGS__); fflush(stdout);
#else
#define LOGD(fmt, ...)
#endif

#ifdef PRINT_VERBOSE
#define LOGV(fmt, ...) printf(ENET_BLUE"(VERBOSE) %4d:%s : " fmt""ENET_RESET, __LINE__, __FUNCTION__, ## __VA_ARGS__); fflush(stdout);
#else
#define LOGV(fmt, ...)
#endif

#ifdef PRINT_FNCIO
#define FNC_IN   LOGD(">>>\n")
#define FNC_OUT  LOGD("<<<\n")
#else
#define FNC_IN  /* do nothing */
#define FNC_OUT /* do nothing */
#endif

#define LOGIP(fmt, addr) \
	LOGN(fmt"%d.%d.%d.%d\n", addr.octet[0], addr.octet[1], addr.octet[2], addr.octet[3])

#define IPV62STR(addr, buf, bufSize) inet_ntop(AF_INET6, addr.addr8bit, buf, bufSize)
/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef struct {
	FrmMsgHead_t header;
	slong param;
} EnetFnc00_t;

typedef struct {
	FrmMsgHead_t header;
	bool_t boolean;
} EnetFncBool_t;

typedef struct {
	FrmMsgHead_t header; /* メッセージヘッダ */
	EnetNetworkType_t type;
	EnetNettypeExt_t ext;
} EnetFncNetworkType_t;

typedef struct {
	FrmMsgHead_t header;
	EnetIpv4Settings_t v4;
	EnetIpsetExt_t ext;
} EnetFncIpv4_t;

typedef struct {
	FrmMsgHead_t header;
	EnetIpv6Settings_t v6;
} EnetFncIpv6_t;

typedef struct {
	FrmMsgHead_t header;
	EnetProxy_t proxy;
} EnetFncProxy_t;

typedef struct {
	FrmMsgHead_t header;
	EnetWlanSettings_t wlan;
	EnetWlanSetExt_t ext;
} EnetFncWlan_t;

typedef struct {
	FrmMsgHead_t header;
	schar ifName[ENET_WLAN_IF_NAME_STR_LEN];
} EnetFncWlanIfName_t;

typedef struct {
	FrmMsgHead_t header;
	slong fact;
}EnetStart_t;

typedef struct {
	FrmMsgHead_t header;
	FrmPthread_t thread;
}EnetThdCancel_t;

typedef union {
	FrmMsgHead_t header;
	EnetFnc00_t fnc00;
	EnetFncBool_t fncBool;
	EnetFncNetworkType_t fncNetType;
	EnetFncIpv4_t fncIPv4;
	EnetFncIpv6_t fncIPv6;
	EnetFncProxy_t fncProxy;
	EnetFncWlan_t fncWlan;
	EnetFncListenerPort_t fncPort;
	EnetFncWlanIfName_t fncWlanIfName;
} EnetFnc_t;

typedef struct {
	schar key[ENET_BUF_STR_LEN];
	schar val[ENET_BUF_STR_LEN];
} EnetKeyAndVal_t;

typedef struct {
	ulong port;
	ulong flag;
} EnetProcElm_t;

typedef struct {
	schar iface[IFNAMSIZ];
	ulong dest;
	ulong gateway;
	slong flgs;
	slong ref;
	slong use;
	slong metric;
	ulong mask;
	slong mtu;
	slong win;
	slong irtt;
} EnetRouteInfo_t;

typedef struct {
	schar resolver[11];
	slong octet[4];
} EnetDnsInfo_t;
/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
typedef enum {
	ENET_LINK_DOWN = 0, /* リンクDown */
	ENET_LINK_UP, /* リンクUP */
	/* ターミネータ */
	ENET_LINK_MODE_MAX
} EnetLinkMode_t;

typedef enum {
	ENET_DEVICE_NONE = 0, /* デバイス未検出 */
	ENET_DEVICE_UP, /* デバイスUP(動作中) */
	ENET_DEVICE_DOWN, /* リンクDown(停止中) */
	/* ターミネータ */
	ENET_DEVICE_STATE_MAX
} EnetDeviceState_t;

typedef enum {
	ENET_ONE_SHOT_ID_IP_WATCH  = 0, /* IP監視間隔 */
	ENET_ONE_SHOT_ID_SEND_PING, /* ルータ疎通確認間隔 */
	/* ターミネータ */
	ENET_ONE_SHOT_ID_MAX,
	ENET_ONE_SHOT_ID_WLANLMT,
} EnetOneShotId_t;

#endif /*__ENET_CMN_G_H__*/
