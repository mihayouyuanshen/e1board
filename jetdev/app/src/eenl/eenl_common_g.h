/****************************************************************************/
/*	Description		:EENLの共通処理 ヘッダ									*/
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
#ifndef __EENL_COMMON_G_H__
#define __EENL_COMMON_G_H__

/*--------------------------------------------------------------------------*/
/*	ユビキタス ヘッダファイル												*/
/*--------------------------------------------------------------------------*/
#include "../../inc/enl/enl.h"
#include "../../inc/enl/enl_device.h"
#include "../../inc/enl/enl_electric_vehicle_charger.h"
#include "../../inc/enl/enl_home_solar_power_generation.h"
#include "../../inc/enl/enl_multi_input_pcs.h"
#include "../../inc/enl/enl_storage_battery.h"
#include "../../inc/enl/enl_distributed_generator_electric_energy_meter.h"

/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#if defined(__CYGWIN__) && __CYGWIN__ != 0
#if defined(USE_EVENTFD) && USE_EVENTFD != 0
#undef USE_EVENTFD
#endif
#endif

#if defined(IGNORE_SELF) && IGNORE_SELF != 0
#include <sys/ioctl.h>
#include <net/if.h>
#endif

#if defined(USE_EVENTFD) && USE_EVENTFD != 0
#include <sys/eventfd.h>
#define EENL_NUM_FD_EVT			((uchar)1u)
#define EENL_IDX_FD_EVT_READ	((uchar)0u)
#define EENL_IDX_FD_EVT_WRITE	((uchar)0u)
#else
#define EENL_NUM_FD_EVT			((uchar)2u)
#define EENL_IDX_FD_EVT_READ	((uchar)0u)
#define EENL_IDX_FD_EVT_WRITE	((uchar)1u)
#endif

#define EENL_NULL_CHECK(A,schar) ((NULL == A) || ((NULL != A) && (NULL == schar)))	/* NULLチェック用 */
#define EENL_UNUSED_PARA(a) ((void)(a))												/* 未使用引数用 */


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	constants
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#define EENL_MAX_MESSAGE_LENGTH			((ushort)(4096u))

#define EENL_MULTICAST_ADDRESS			((schar*)("224.0.23.0"))
#define EENL_UDP_PORT					((ushort)(3610u))
#define	EENL_TCP_PORT					((ushort)(3610u))
#define EENL_MULTICAST_ADDRESS_V6		((schar*)("ff02::1"))

#define EENL_OFS						((uchar)(0xB5u - 0x42u))

#define	EENL_IP_TYPE_V4					((uchar)(4u))
#define	EENL_IP_TYPE_V6					((uchar)(6u))

#define	EENL_PROTO_TYPE_UDP				((uchar)(0u))
#define	EENL_PROTO_TYPE_TCP				((uchar)(1u))

#define	EENL_MULTICAST_LOOP				((uchar)(10u))
#define	EENL_THREAD_TIMEOUT				((uchar)(5u))

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	constants for API
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/* result of API */
#define EENL_RESULT_NO_DATA_UPDATE		((uchar)(2u))			/* When creating EPC setting data, it indicates that there is no ambiguity in the set data and there is no need to set the property. */
#define EENL_RESULT_DEVICE_NOT_FOUND	((schar)(-100))			/* デバイスなしエラー */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	constants for callback
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#define EENL_RESULT_COLLECT_AND_ACCEPT_BAT0xF4	((uchar)(101u))	/* EENL_RESULT_COLLECT_AND_ACCEPTは蓄電池クラスの0xFAと0xFBでも使用している為、蓄電池クラスの0xF4用に追加。 */

#define	EENL_PUSHEDUP_OFF				((uchar)(0x42u))		/* 押し上げなし */

#define EENL_OPEMODE_ECONOMY			((uchar)(0x01u))		/* 運転モード(売電モード) */
#define EENL_OPEMODE_GREEN				((uchar)(0x02u))		/* 運転モード(グリーンモード) */
#define EENL_OPEMODE_CHARGE				((uchar)(0x03u))		/* 運転モード(充電) */
#define EENL_OPEMODE_MENT				((uchar)(0x06u))		/* 運転モード(メンテナンスモード) */
#define EENL_OPEMODE_SPECIAL			((uchar)(0x07u))		/* 運転モード(特殊運転モード) */

#define EENL_0xF4_TIMESET		 		((uchar)(0x02u))		/* 時刻設定数(押し上げ無し) */
#define EENL_0xF4_TIMESET_PUSHEDUP		((uchar)(0x04u))		/* 時刻設定数(押し上げ有り) */

#define	EENL_CREATE_OK					((uchar)(1u))			/* enl構築済 */
#define	EENL_CREATE_NG					((uchar)(0u))			/* enl未構築 */
#define	EENL_REMOTE_OFF					((uchar)(0x41u))		/* 遠隔操作設定 未経由(0x41) */
#define	EENL_REMOTE_ON					((uchar)(0x42u))		/* 遠隔操作設定 経由(0x42) */

#define	EENL_LIMIT_1000000000			((ulong)(1000000000u))	/* 上限値1000000000 */
#define	EENL_LIMIT_999999999			((ulong)(999999999u))	/* 上限値999999999 */
#define	EENL_LIMIT_65533				((ushort)(65533u))		/* 上限値65533 */
#define	EENL_YEAR_LIMIT_MIN				((ushort)(2000u))		/* 現在年月日の下限値(2000年) */
#define	EENL_YEAR_LIMIT_MAX				((ushort)(2037u))		/* 現在年月日の上限値(2037年) */
#define	EENL_LIMIT_1000					((ushort)(1000u))		/* 上限値1000 */
#define	EENL_LIMIT_100					((uchar)(100u))			/* 上限値100 */
#define	EENL_LIMIT_23					((uchar)(23u))			/* 上限値23 */
#define	EENL_LIMIT_59					((uchar)(59u))			/* 上限値59 */
#define	EENL_RETRY_COUNT				((uchar)(120u))			/* リトライ回数上限 */
#define	EENL_LIMIT_0					((uchar)(0u))			/* 下限値0 */
#define	EENL_LIMIT_PCSERR				((ushort)(60u*60u))		/* PCSエラー上限回数(1秒に1回カウントし、上限に達したらエラーログを出力する) */
#define	EENL_EMERGENCY_0				((uchar)(0u))			/* 非常時安心設定 0% */
#define	EENL_EMERGENCY_30				((uchar)(30u))			/* 非常時安心設定 30% */
#define	EENL_EMERGENCY_70				((uchar)(70u))			/* 非常時安心設定 70% */
#define	EENL_EMERGENCY_100				((uchar)(100u))			/* 非常時安心設定 100% */
#define	EPCS_PCSERR_CONNECTIONFAILED	((schar*)("BF550"))		/* PCS通信不通 */
#define EENL_0xFB_SCHEDULE				((uchar)(48u))			/* 0xFB(運転計画)のスケジュール部のデータサイズ */
#define EENL_0xFB_CHGDCHGLIMIT			((uchar)(4u))			/* 0xFB(運転計画)の充放電残量部のデータサイズ */
#define EENL_CLASS_BATTERY				((uchar)(0u))			/* 蓄電池クラス */
#define EENL_CLASS_SOLAR				((uchar)(1u))			/* 太陽光クラス */
#define EENL_ANNO_NONEREQ				((uchar)(0u))			/* 状変アナウンス通知無し */
#define EENL_ANNO_REQ					((uchar)(1u))			/* 状変アナウンス通知有り */
#define	EENL_LIMIT_GENPWR_MIN			((s64)(0x80000001))		/* 外付太陽光発電電力値 下限値0x80000001(-2,147,483,647) */
#define	EENL_LIMIT_GENPWR_MAX			((s64)(0x7FFFFFFD))		/* 外付太陽光発電電力値 上限値0x7FFFFFFD( 2,147,483,645) */
#define	EENL_LIMIT_PWRFLOW_MIN			((s64)(0x80000001))		/* 売買電電力実効値     下限値0x80000001(-2,147,483,647) */
#define	EENL_LIMIT_PWRFLOW_MAX			((s64)(0x7FFFFFFD))		/* 売買電電力実効値     上限値0x7FFFFFFD( 2,147,483,645) */
#define	EENL_LIMIT_LOADPOWER_MIN		((s64)(0x80000001))		/* 家庭消費電力値       下限値0x80000001(-2,147,483,647) */
#define	EENL_LIMIT_LOADPOWER_MAX		((s64)(0x7FFFFFFD))		/* 家庭消費電力値       上限値0x7FFFFFFD( 2,147,483,645) */
#define	EENL_OUTPUTFLUCTUATESPDCHG_MIN	((uchar)(0x05u))		/* 出力増減速度 最小値 */
#define	EENL_OUTPUTFLUCTUATESPDCHG_MAX	((uchar)(0x0Au))		/* 出力増減速度 最大値 */
#define	EENL_LIMIT_TTLPWR_MIN			((u64)(0x00000000))		/* 積算買電電力量       下限値0x00000000( 0) */
#define	EENL_LIMIT_TTLPWR_MAX			((u64)(0x3B9AC9FF))		/* 積算売電電力量       上限値0x3B9AC9FF( 999,999,999) */

#define	EENL_F4_OPEMODE_RESET_ON		((uchar)(1u))			/* メンテナンス状態書き換えフラグON */
#define	EENL_F4_OPEMODE_RESET_OFF		((uchar)(0u))			/* メンテナンス状態書き換えフラグOFF */

#define	EENL_PCS_ACK					((uchar)(49u))			/* RS485通信仕様のACK(ascii code '1') PCS応答の判定に使用 */
#define	EENL_PCS_NAK					((uchar)(48u))			/* RS485通信仕様のNAK(ascii code '0') PCS応答の判定に使用 */
#define	EENL_ANNOUNCE_ON				((uchar)(1u))			/* アナウンス通知有り */
#define	EENL_ANNOUNCE_OFF				((uchar)(0u))			/* アナウンス通知無し */

#define	EENL_OPECTRL_CHARGE				((uchar)(0x42u))		/* ENL充放電制御 充電 (0xDAと同じ値) */
#define	EENL_OPECTRL_DISCHARGE			((uchar)(0x43u))		/* ENL充放電制御 放電 (0xDAと同じ値) */
#define EENL_OPECTRL_OFF				((uchar)(0x99u))		/* ENL充放電制御 OFF */

#define	EENL_0x93_SET_ON				((uchar)(1u))			/* 遠隔操作(0x93)を公衆回線未経由(0x41)に戻すフラグON */
#define	EENL_0x93_SET_OFF				((uchar)(0u))			/* 遠隔操作(0x93)を公衆回線未経由(0x41)に戻すフラグOFF */

#define EENL_BYTE1(x)					( x        & 0xFFu)
#define EENL_BYTE2(x)					((x >>  8u) & 0xFFu)
#define EENL_BYTE3(x)					((x >> 16u) & 0xFFu)
#define EENL_BYTE4(x)					((x >> 24u) & 0xFFu)

#define EENL_BYTE_SWAP_16(x)			((uint16_t)( EENL_BYTE1(x) << 8u | EENL_BYTE2(x) ))
#define EENL_BYTE_SWAP_32(x)			((uint32_t)( EENL_BYTE1(x) << 24u | EENL_BYTE2(x) << 16u | EENL_BYTE3(x) << 8u | EENL_BYTE4(x) ))

#define	EENL_V2HTYPE_11					((schar*)("11"))		/* 車両充放電器タイプ */
#define	EENL_V2HTYPE_12					((schar*)("12"))		/* 車両充放電器タイプ */
#define	EENL_V2HTYPE_13					((schar*)("13"))		/* 車両充放電器タイプ */
#define	EENL_V2HTYPE_21					((schar*)("21"))		/* 車両充放電器タイプ */
#define	EENL_V2HTYPE_22					((schar*)("22"))		/* 車両充放電器タイプ */
#define	EENL_V2HTYPE_23					((schar*)("23"))		/* 車両充放電器タイプ */
#define	EENL_V2HTYPE_31					((schar*)("31"))		/* 車両充放電器タイプ */
#define	EENL_V2HTYPE_32					((schar*)("32"))		/* 車両充放電器タイプ */
#define	EENL_V2HTYPE_33					((schar*)("33"))		/* 車両充放電器タイプ */
#define	EENL_V2HTYPE_41					((schar*)("41"))		/* 車両充放電器タイプ */
#define	EENL_V2HTYPE_42					((schar*)("42"))		/* 車両充放電器タイプ */
#define	EENL_V2HTYPE_43					((schar*)("43"))		/* 車両充放電器タイプ */
#define	EENL_V2HTYPE_51					((schar*)("51"))		/* 車両充放電器タイプ */
#define	EENL_V2HTYPE_52					((schar*)("52"))		/* 車両充放電器タイプ */
#define	EENL_V2HTYPE_53					((schar*)("53"))		/* 車両充放電器タイプ */

/****************************************/
/* PDC(=プロパティ値のデータサイズ)定義 */
/****************************************/
#define	EENL_PDC_0x83					((uchar)(17u))
#define	EENL_PDC_0x8C					((uchar)(12u))
#define	EENL_PDC_0x8D					((uchar)(12u))
#define EENL_PDC_USER_0xF8				((uchar)(9u))			/* 蓄電池クラスユーザ定義0xF8(運転指示) */
#define EENL_PDC_USER_0xF9				((uchar)(2u))			/* 蓄電池クラスユーザ定義0xF9(残量設定) */
#define EENL_PDC_USER_0xF4				((uchar)(21u))			/* 蓄電池クラスユーザ定義0xF4(自動運転設定　押し上げ無し時) */
#define EENL_PDC_USER_0xF4_PUSHEDUP		((uchar)(39u))			/* 蓄電池クラスユーザ定義0xF4(自動運転設定　押し上げ有り時) */
#define EENL_PDC_USER_0xFA				((uchar)(6u))			/* 蓄電池クラスユーザ定義0xFA(HEMS制御状態) */
#define EENL_PDC_USER_0xFB				((uchar)(61u))			/* 蓄電池クラスユーザ定義0xFB(運転計画) */
#define EENL_PDC_USER_0xFC				((uchar)(96u))			/* 蓄電池クラスユーザ定義0xFA(運転履歴) */
#define EENL_PDC_SOL_0xB0				((uchar)(100u))			/* 太陽光クラス0xB0(出力制御スケジュール) */
#define EENL_PDC_SOL_0xB1				((uchar)(7u))			/* 太陽光クラス0xB1(次回アクセス日時) */
#define EENL_PDC_V2H_0xE6				((uchar)(25u))			/* 車両充放電器クラス0xE6(車両ID) */

#define CMN_STRTOL_BASE_DEC				((uchar)(10))			/* 数値変換(10進数) */
#define CMN_HOUR_MAX					((uchar)(24))
#define CMN_MIN_MAX						((uchar)(60))
#define CMN_DAYSEC						((ulong)(86400))		/* 1日の秒数( 24H * 3600sec ) */
#define CMN_TIME_ZONE_LEN				((uchar)(2))

#define CMN_HOUR_SEC(n)					((ulong)((n)*3600))		/* 1時間の秒数 */
#define CMN_MIN_SEC(n)					((ulong)((n)*60))		/* 1分の秒数  */
#define CMN_SEC_HOUR(n)					((ulong)((n)/3600))		/* 秒数から時間へ変換  */
#define CMN_ADD_MIN_DAY					((ushort)(CMN_HOUR_MAX * CMN_MIN_MAX))
#define	CMN_REFERENCE_HOURSEC			((ulong)(85800))		/* 充放電時間の最大値(23時間50分) */



/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	structures
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*  IPv4/v6通信用構造体 */
typedef struct {
	uchar					ipType;
	uchar					protoType;
	struct sockaddr_in		v4addr;
	struct sockaddr_in6		v6addr;
	sint					tcpSock;
} EenlAddr_t;


/* device information */
typedef struct EenlDeviceInfo_t {
	void* node;			/* ノードプロファイル */
	void* devBattery;	/* 蓄電池 */
	void* devSolar;		/* 太陽光発電 */
	void* devV2h;		/* 電気自動車充放電器 */
	void* devMpcs;		/* マルチ入力PCSク */
	void* devMeterE1;	/* 分散型電源電力量メータクラスE1 */
	void* devMeterC2;	/* 分散型電源電力量メータクラスC2 */
	EenlAddr_t addr;
	pthread_t th;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	slong event;
	uchar epc;
	struct EenlDeviceInfo_t* next;
} EenlDeviceInfo_t;

/* 充放電時間帯 */
typedef struct {
	uchar cStartHH;		/* 充電開始時刻[時]	*/
	uchar cStartMM;		/* 充電開始時刻[分] */
	uchar cStopHH;		/* 充電停止時刻[時] */
	uchar cStopMM;		/* 充電停止時刻[分] */
	uchar dStartHH;		/* 放電開始時刻[時] */
	uchar dStartMM;		/* 放電開始時刻[分] */
	uchar dStopHH;		/* 放電停止時刻[時] */
	uchar dStopMM;		/* 放電停止時刻[分] */

} EenlHourMinute_t;


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	event codes
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
enum {
	EENL_EVENT_NONE,
	EENL_EVENT_DISPOSE,

	EENL_EVENT_READ,
	EENL_EVENT_WRITE,

	EENL_EVENT_BATTERY_READ,
	EENL_EVENT_BATTERY_WRITE,

	EENL_EVENT_SOLAR_READ,
	EENL_EVENT_SOLAR_WRITE,

	EENL_EVENT_V2H_READ,
	EENL_EVENT_V2H_WRITE,

	EENL_EVENT_MPCS_READ,
	EENL_EVENT_MPCS_WRITE,

	EENL_EVENT_E1METER_READ,
	EENL_EVENT_E1METER_WRITE,

	EENL_EVENT_C2METER_READ,
	EENL_EVENT_C2METER_WRITE,

	EENL_EVENT_NOTIFIED,
	EENL_EVENT_UPDATED,

	EENL_EVENT_ACCEPTED,
	EENL_EVENT_REJECTED
};

/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/

/* property list of node profile */
extern uchar g_nodeProp[14];

/* property list of device */
extern uchar g_devicePropSolarNccN[33];
extern uchar g_devicePropBatteryNccN[52];
extern uchar g_devicePropV2hNccN[50];
extern uchar g_devicePropMpcs[22];
extern uchar g_devicePropMeter[29];
extern ENL_PROPERTY_DEFINE g_devicePropSolarExtNccN[2];
extern ENL_PROPERTY_DEFINE g_devicePropBatteryExtN[6];
extern ENL_PROPERTY_DEFINE g_devicePropV2hExtN[3];
extern ENL_PROPERTY_DEFINE g_devicePropMpcsExt[3];

/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/

/* device management */
extern void EENL_RegisterDeviceInfo(EenlDeviceInfo_t* info);
extern EenlDeviceInfo_t* EENL_GetDeviceInfoIndex(slong idx);
extern EenlDeviceInfo_t* EENL_GetDeviceInfoInstance(void* instance);
extern void EENL_DisposeAllDevices(void);

extern void EENL_AddMulticast(void);
extern slong EENL_ThreadCheck(void);

/* system setup */
extern void EENL_Initialize(void);
extern void EENL_Terminate(void);

/* eventing */
extern void EENL_EventWait(EenlDeviceInfo_t* info);
extern void EENL_EventSet(EenlDeviceInfo_t* info, slong event, uchar epc);
extern void EENL_EvenClear(EenlDeviceInfo_t* info);

/* messaging */
extern void EENL_SendMessage(EenlAddr_t* addr, ENL_MESSAGE* msg);

extern ENL_MESSAGE* EENL_GetLatestRequest(void);

/*  充放電時刻の有効値判定 */
extern slong EENL_ChargeDischargeTimeZoneCheck(EenlHourMinute_t* hourMinute);

/*  プロパティ設定のラッパー関数 */
extern slong EENL_ExtEnlDevSetProperty(ENL_MESSAGE** message,
	void* instance, uchar epc, uchar pdc, uchar* edt);
extern slong EENL_ExtEnlDevSetPropertyValue(ENL_MESSAGE** message,
	void* instance, uchar epc, ulong value, uchar index);

/*  フラグ制御関連：EPC設定済フラグ、Set要求受付可能フラグ */
extern void EENL_InitEpcValidFlg(void);
extern void EENL_SetEpcValidFlg(void* instance, uchar epc);

extern void EENL_SetReqSetEnable(void);
extern bool_t EENL_ChkGetEnable(void* instance, uchar epc);
extern bool_t EENL_ChkSetEnable(void* instance, uchar epc);

/*  Release K */
extern slong EENL_GetNextDay(EtimTm_t* curDay, EtimTm_t* nextDay);
extern void EENL_SetBroadcastDevList(void** devList);

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	functions take in the difference
		of device and controllerothers
	eenl.c
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* set property */
extern slong EENL_BroadcastSetProperty(
	void** devList,
	uchar	listcnt,
	uchar	epc,
	uchar	valSize,
	uchar* val,
	uchar	ano_onoff);
extern slong EENL_BroadcastSetPropertyValue(
	void** devList,
	uchar	listcnt,
	uchar	epc,
	ulong	val,
	uchar	index,
	uchar	ano_onoff);

extern bool_t EENL_CheckChargeTimeZone(EPCS_CHARGE_TIME_ZONE_T* epcsTimeZone);
extern void EENL_ChangeHemsErrCode(schar* pcsErr, schar* pcsErrRating);
extern slong EENL_CheckEnlOpectrl(uchar set_ope_mode);

/* APIのコールバック関数 */
extern void ENL_callback_response(void* instance, ENL_MESSAGE* message, slong result);

#endif
