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
#ifndef __ENET_FILE_G_H__
#define __ENET_FILE_G_H__
/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "typedef.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define ENET_OPEN_TYPE_READ "r+"
#define ENET_OPEN_TYPE_WRITE "w+"
#define ENET_OPEN_TYPE_READ_BIN "rb"

/* enet汎用設定ファイル */
#define ENET_FILE_SETTING "/user/enet/enet.inf"
/* for PROXY */
#define ENET_FILE_PROXY "/user/enet/proxy.inf"
/* for WLAN */
#define ENET_FILE_WLAN "/user/enet/wlan.inf"
#define ENET_FILE_WLAN_LIMIT "/user/enet/WLANLimit.txt"
#define ENET_FILE_WPACONF "/etc/hostapd.conf"
/* for IPアドレス変更通知 */
#define ENET_FILE_NOTIFY_PROC "/mnt/ram/enet_notifyproc.inf"

#define ENET_FILE_TEMP    "/user/enet/enet.tmp"
#define ENET_SETTING_ONLY_KEY (uchar)(1u) /* key=val形式、keyのみ */
#define ENET_SETTING_ELEMENTS (uchar)(2u) /* key=val形式、key,val存在 */

#define GET_PREF(key, val, file) ENET_RwSettingValue(key, val, ENET_FILE_READ, file)
#define SET_PREF(key, val, file) ENET_RwSettingValue(key, val, ENET_FILE_WRITE, file)

/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef enum {
	ENET_FILE_READ = 0, /* ファイルから読み込む */
	ENET_FILE_WRITE, /* ファイルに書き込む */
	/* ターミネータ */
	ENET_FILE_MODE_MAX
} EnetFileMode_t;
/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
slong ENET_GetFileSize(const schar* src);
bool_t ENET_RwSettingValue(const schar* key, schar* val, EnetFileMode_t mode, const schar* filePath);
void ENET_InitEnetFile(int argc, char *argv[]);
slong ENET_ReadWlanLimitFile(void);

#endif /*__ENET_FILE_G_H__*/
