/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jingtong.li										*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/17												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __ENET_G_H__
#define __ENET_G_H__

#include <lfw_g.h>
#include <frm_g.h>
#include <typedef.h>
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define ENET_PROXY_ADDR_STR_LEN (47)  /* 画面遷移仕様より46文字+'\0' */
#define ENET_WLAN_SSID_STR_LEN  (33) /* ESSID名：最大32文字+'\0' */
#define ENET_WLAN_PSWR_STR_LEN  (64) /* 無線パスワード：最大63文字+'\0' */
#define ENET_WLAN_IF_NAME_STR_LEN (64) /* 無線LANインタフェース名：63文字+'\0' */

#define ENET_CHGEVNT_IP_ADDRESS    (ulong)(0x0001) /* 変化通知：IPアドレス */
#define ENET_CHGEVNT_IP_HANDLING   (ulong)(0x0002) /* 変化通知：IP自動/手動 */
#define ENET_CHGEVNT_NETWORK_TYPE  (ulong)(0x0004) /* 変化通知：ネットワーク種別 */
#define ENET_CHGEVNT_PROXY_SETTING (ulong)(0x0008) /* 変化通知：Proxy設定 */

/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef enum {
	ENET_MSGID_START = 0,
	ENET_THREAD_CANCEL,
	ENET_SYNC_CHKPROCESS, /* WDT応答 */
	/* ネットワーク種別（なし/有線/無線） */
	ENET_SYNC_GET_NETWORKTYPE, /* ネットワーク種別 取得 */
	ENET_SYNC_SET_NETWORKTYPE, /* ネットワーク種別 設定 */
	/* IPv4アドレス */
	ENET_SYNC_GET_IPADDR4, /* IPv4 取得 */
	ENET_SYNC_SET_IPADDR4, /* IPv4 設定 */
	ENET_SYNC_GET_IPADDR4SETTING, /* IPv4 手動設定値取得 */
	/* IPv6アドレス */
	ENET_SYNC_GET_IPADDR6, /* IPv6 取得 */
	ENET_SYNC_SET_IPADDR6, /* IPv6 設定 */
	/* プロキシサーバ */
	ENET_SYNC_GET_PROXY, /* プロキシ 取得 */
	ENET_SYNC_SET_PROXY, /* プロキシ 設定 */
	/* 無線LAN */
	ENET_ASYNC_SCAN_ESSID, /* 非同期essidスキャン */
	ENET_SYNC_SET_WLANSETTING, /* 無線LAN設定 設定 */
	ENET_SYNC_GET_WLANSETTING, /* 無線LAN設定 取得 */
	ENET_SYNC_GET_WLANSTATUS, /* 無線LAN状態(電波強度) 取得、未接続時は負数 */
	ENET_SYNC_SET_WLANUP, /* 無線LAN on/off */
	ENET_ASYNC_WRITE_WLANLIMIT, /* WLANLimit.txtを書 */
	ENET_ASYNC_RESTARTWIFI,
	/* ルータ疎通 */
	ENET_ASYNC_CHK_ROUTERCONNECTION, /* ルータ疎通 */
	/* 設定変化イベント通知 */
	ENET_SYNC_SET_CHGEVNTLISTENER, /* 設定変化イベント通知 登録 */
	ENET_SYNC_RMV_CHGEVNTLISTENER, /* 設定変化イベント通知 解除 */
	/* WLANインターフェース名取得 */
	ENET_SYNC_GET_WLAN_IF_NAME, /* WLANインタフェース名取得 */
	/* ターミネータ */
	ENET_MAX_FUNC_NUMS /* 関数の数 */
}EnetMsgId_t;

typedef enum {
	ENET_NETWORK_NONE = 0,
	ENET_NETWORK_WIRED,
	ENET_NETWORK_WIRELESS,
	/* ターミネータ */
	ENET_NETWORK_TYPE_MAX
} EnetNetworkType_t;

typedef enum {
	ENET_IPV4 = 0,
	ENET_IPV6,
	ENET_IP_TYPE_MAX
} EnetIpType_t;

typedef enum {
	ENET_NETTYPE_EXT_NONE = 0,		/* 拡張なし */
	ENET_NETTYPE_EXT_UPD_SETFILE,	/* 設定ファイル更新(デバイスの有無に関わらず設定ファイル更新する) */
	ENET_NETTYPE_EXT_MAX
} EnetNettypeExt_t;

typedef enum {
	ENET_IPSET_EXT_NONE = 0,	/* 拡張なし */
	ENET_IPSET_EXT_UPD_SETFILE,	/* 設定ファイル更新(設定ファイルのみ更新する) */
	ENET_IPSET_EXT_MAX
} EnetIpsetExt_t;

typedef enum {
	ENET_WLANSET_EXT_NONE = 0,		/* 拡張なし */
	ENET_WLANSET_EXT_UPD_SETFILE,	/* 設定ファイル更新(設定ファイルのみ更新する) */
	ENET_WLANSET_EXT_MAX
} EnetWlanSetExt_t;

typedef enum {
	ENET_TEST_MODE = 0,
	ENET_NORMAL_MODE,
} EnetDeviceRunMode_t;

typedef union { /* 255.255.255.255 */
	uchar octet[4];
	ulong addr32bit;
} EnetIpv4_t;

typedef struct {
	EnetIpv4_t ip;
	EnetIpv4_t subnet;
	EnetIpv4_t gateway;
	EnetIpv4_t dns1;
	EnetIpv4_t dns2;
	schar autoFlg; /* 1/0：自動/手動 */
} EnetIpv4Settings_t;

typedef union { /* ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff */
	uchar addr8bit[16]; /* ff */
	ushort addr16bit[8]; /* ffff */
	ulong addr32bit[4]; /* ffff:ffff */
} EnetIpv6_t;

typedef struct {
	EnetIpv6_t ip;
	EnetIpv6_t subnet;
} EnetIpv6Settings_t;

typedef struct {
	schar addr[ENET_PROXY_ADDR_STR_LEN]; /* 画面遷移仕様より46文字 */
	ushort port; /* 0-65535 */
	schar enableFlg; /* 1/0：有効/無効 */
} EnetProxy_t;

typedef struct {
	schar essid[ENET_WLAN_SSID_STR_LEN]; /* 最大32文字 */
	schar password[ENET_WLAN_PSWR_STR_LEN]; /* 最大63文字 */
} EnetWlanSettings_t;

typedef struct {
	FrmMsgHead_t header;
	ulong port;
	ulong flag;
} EnetFncListenerPort_t;
/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

extern EnetNetworkType_t ENET_SyncGetNetworkType(void); /* ネットワーク種別 取得 */
extern slong ENET_SyncSetNetworkType(EnetNetworkType_t); /* ネットワーク種別 設定 */
extern slong ENET_SyncSetNetworkTypeExt(EnetNetworkType_t, EnetNettypeExt_t); /* ネットワーク種別 設定(拡張) */
extern slong ENET_SyncGetIpAddr4(EnetIpv4Settings_t*); /* IPv4 取得 */
extern slong ENET_SyncSetIPAddr4(EnetIpv4Settings_t*); /* IPv4 設定 */
extern slong ENET_SyncSetIPAddr4Ext(EnetIpv4Settings_t*, EnetIpsetExt_t); /* IPv4 設定(拡張) */
extern slong ENET_SyncGetIpAddr4Setting(EnetIpv4Settings_t*); /* IPv4 手動設定値取得 */
extern slong ENET_SyncGetIpAddr6(EnetIpv6Settings_t*); /* IPv6 取得 */
extern slong ENET_SyncSetIpAddr6(EnetIpv6Settings_t*); /* IPv6 設定 */
#if 0
extern slong enet_sync_addIPAddr6(const EnetIpv6Settings_t*); /* IPv6 追加 */
extern slong enet_sync_rmvIPAddr6(const EnetIpv6Settings_t*); /* IPv6 削除 */
#endif
extern slong ENET_SyncGetProxy(EnetProxy_t*); /* プロキシ 取得 */
extern slong ENET_SyncSetProxy(EnetProxy_t*); /* プロキシ 設定 */
extern void ENET_AsyncScanEssid(void); /* ESSIDのスキャン */
extern slong ENET_SyncSetWlanSetting(EnetWlanSettings_t*); /* 無線LAN 設定反映 */
extern slong ENET_SyncSetWlanSettingExt(EnetWlanSettings_t*, EnetWlanSetExt_t); /* 無線LAN 設定反映(拡張) */
extern slong ENRT_SyncGetWlanSetting(EnetWlanSettings_t*); /* 無線LAN 設定取得 */
extern slong ENET_SyncGetWlanStatus(void); /* 無線LAN状態(電波強度) 取得、未接続時は負数 */
extern slong ENET_SyncSetWlanUp(bool_t); /* 無線LAN on/off */
extern void ENET_AsyncChkRouterConnection(void);/* ルータ疎通確認 */
extern slong ENET_SyncSetChgEvntListener(ulong, ulong); /* 設定変化イベント通知 登録、ENET_CHGEVNT_～参照 */
extern slong ENET_SyncRmvChgEvntListener(ulong); /* 設定変化イベント通知 解除 */
extern slong ENET_SyncGetWlanIfName(schar ifName[ENET_WLAN_IF_NAME_STR_LEN]); /* 無線LAN IF名取得 */
extern void ENET_AsyncWriteWlanLimit(slong mins); /* WLANLimit.txtを読み取る */
extern void ENET_AsyncRestartWiFi(void);
extern void ENET_AsyncSetDeviceRun(EnetDeviceRunMode_t mode);

extern void ENET_Init(void);
extern void ENET_Fin(void);
extern void ENET_MsgProc(FrmMsgHead_t const *msg);
extern slong ENET_Start(slong fact);
extern slong ENET_Stop(void);

#ifdef __cplusplus
}
#endif

#endif /*__ENET_G_H__*/
