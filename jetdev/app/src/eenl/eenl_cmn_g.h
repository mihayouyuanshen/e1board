/****************************************************************************/
/*	Description		:EENLの共通処理 ヘッダ									*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC DX.zhao											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/05/08												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、EENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:無し													*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/

#ifndef __EENL_CMN_G_H__
#define __EENL_CMN_G_H__

#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/if.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include "typedef.h"
#include "lfw_g.h"
#include "task_g.h"
#include "common_g.h"
#include "dlog_g.h"
#include "enet_g.h"
#include "etim_g.h"
#include "esys_g.h"

#include "eenl_g.h"
#include "eenl_common_g.h"
#include "enet_define.h"
#include "eenl_device_g.h"								/* 機器オブジェクト ヘッダファイル */
#include "eenl_timer_g.h"								/* タイマー処理 ヘッダファイル */
#include "eenl_hems_history_g.h"						/* HEMS発電抑制 ヘッダファイル */
#include "eenl_cmnfile_g.h"								/* ファイル入出力 ヘッダファイル */
#include "eenl_evt_notify_g.h"							/* 状態変化通知 ヘッダファイル */
#include "eenl_node_g.h"								/* ノードプロファイル ヘッダファイル */
#include "eenl_bt_g.h"									/* 蓄電池クラス ヘッダファイル */
#include "eenl_solar_g.h"								/* 太陽光クラス ヘッダファイル */
#include "eenl_v2h_g.h"									/* V2Hクラス ヘッダファイル */
#include "eenl_mpcs_g.h"								/* マルチ入力PCSクラス ヘッダファイル */
#include "eenl_meter_g.h"								/* 分散型電源電力量メータクラス  ヘッダファイル*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/
extern const uchar g_eenlCodeNcc[3];					/* メーカコード ニチコン 0x00006C */

extern uchar g_eenlId[17];

extern uchar g_eenlBattery0x81;							/* 蓄電池 設置場所(0x81) */
extern uchar g_eenlSolar0x81;							/* 太陽光 設置場所(0x81) */
extern uchar g_eenlV2h0x81;								/* 車両充放電器 設置場所(0x81) */
extern uchar g_eenlMpcs0x81;							/* マルチ入力PCS 設置場所(0x81) */
extern ulong g_eenlBattery0xE7;							/* 蓄電池 充電量設定値(0xE7) */
extern ulong g_eenlBattery0xE8;							/* 蓄電池 放電量設定値(0xE8) */
extern ulong g_eenlBattery0xAA;							/* 蓄電池 AC充電量設定値(0xAA) */
extern ulong g_eenlBattery0xAB;							/* 蓄電池 AC放電量設定値(0xAB) */

extern uchar g_eenlPushedUp;							/* 押し上げ 0x42:なし(固定値)  */

extern uchar g_eenlMacAddr[6];							/* MACアドレス */

extern uchar g_eenlIfMode;								/* IFタイプ */
extern uchar g_eenlCreate;							    /* enl構築 0:未構築 1:構築済 */

extern EenlTestRunState_t g_eenlTestRunState;			/* 試運転の起動有無 */

extern uchar g_eenlOpeCtrl;								/* ENL充放電制御 (初期値:OFF) */

extern uchar g_eenlV2hConnectorLock;					/* 状態変化通知された時の「V2Hコネクタロック状態」を保持 */
extern uchar g_eenlOperation;							/* 状態変化通知された時の「CMD21:動作中」を保持 */
extern uchar g_eenlPwrCut;								/* 停電状態変更通知された時の「停電状態」を保持 */

extern uchar g_eenlKeepOpeMode;							/* 状態変化通知された時の運転モード保存用。2017/05/11 B-0076 0xF4状態変化通知 不具合対処 */

extern slong g_eenlReboot;								/* プロセス再起動判定用(ESYS_START_FACT_RESET=プロセス再起動)  */

extern EenlHemsHistory_t g_eenlHemsHistory;				/* HEMS発電抑制履歴 */

extern bool_t s_eenlSupReleaseK;						/* Release K対応判定用 */

extern void* g_eenlDevNode;								/* デバイスインスタンスアドレス(ノード) */
extern void* g_eenlDevBattery;							/* デバイスインスタンスアドレス(蓄電池) */
extern void* g_eenlDevSolar;							/* デバイスインスタンスアドレス(太陽光) */
extern void* g_eenlDevV2h;								/* デバイスインスタンスアドレス(車両充放電器) */
extern void* g_eenlDevMpcs;								/* デバイスインスタンスアドレス(マルチ入力PCS) */
extern void* g_eenlDevMeterE1;							/*	デバイスインスタンスアドレス(分散型電源電力量メータ)E1 */
extern void* g_eenlDevMeterC2;							/*	デバイスインスタンスアドレス(分散型電源電力量メータ)C2 */

extern void* g_eenlDevList[EENL_DEV_LIST_MAX];			/* デバイスインスタンスアドレス */

extern uchar g_eenlEojSolar[3];							/* 太陽光のEOJ */
extern uchar g_eenlEojBt[3];							/* 蓄電池のEOJ */
extern uchar* g_eenlEojList[2];							/* EOJリスト */

extern uchar g_eenlKeepV2hOpeMode;						/* 状態変化通知された時のV2Hの運転モード保存用。2017/12/13 ADAM修正計画No.231対処 */

#endif													/*__EENL_CMN_G_H__ */
