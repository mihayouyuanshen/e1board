/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Xu.Y												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/05/04												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EDAT_VAL_G_H__
#define __EDAT_VAL_G_H__

#include "typedef.h"
#include <pthread.h>
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
/* 発電所ID */
#define EDAT_VAL_PWRHOUSEID_LEN				((uchar)27u)
/* 固定スケジュール開始年月日 */
#define EDAT_VAL_FIXSHDDATE_LEN				((uchar)8u)
/* 郵便番号 */
#define	EDAT_VAL_POSTALNUM_LEN				((uchar)8u)
/* IPアドレス */
#define	EDAT_VAL_IPADDRESS_LEN				((uchar)16u)
/* PROXYアドレス */
#define	EDAT_VAL_PROXYADDRESS_LEN			((uchar)13u)
/* ポート番号 */
#define	EDAT_VAL_PORTNUMBER_LEN				((uchar)6u)
/* hhmm */
#define EDAT_VAL_CHARGTIME_LEN				((uchar)6u)
/* TIMEサイズ  */
#define EDAT_VAL_SETUP_TIME_LEN				((uchar)12u)
/* YYMMDD */
#define EDAT_VAL_DATE_LEN					((uchar)7u)
/* システム_機種名 */
#define EDAT_VAL_SYSMDLNM_LEN				((uchar)17u)
/* 通信ボード_機種名 */
#define EDAT_VAL_MDLNAME_LEN				((uchar)17u)
/* 通信ボード_製造番号 */
#define EDAT_VAL_SERNUM_LEN					((uchar)17u)
/* 通信ボード_FWバージョン */
#define EDAT_VAL_BRDFWV_LEN					((uchar)9u)
/* OPリモコン_FW */
#define EDAT_VAL_OPRMCTLFW_LEN				((uchar)9u)
/* ECHONET Lite */
#define EDAT_VAL_ECHOLITE_LEN				((uchar)2u)
/* 更新スケジュール取得日 */
#define EDAT_VAL_UPDESHDDATE_LEN			((uchar)15u)

#define EDAT_VAL_FLTTOUSHT(flt)				((ushort)(flt * (ushort)100u))
#define EDAT_VAL_USHTTOFLT(usht)			((float)(usht * (float)0.01))
/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef enum{
	EDAT_VAL_RESULT_OK = 0,
	EDAT_VAL_RESULT_NG
} EdatValResult_t;

/* INVマイコンへ送信しないデータ */
/* 構成 */
typedef struct{
	uchar	exterSunPwrOrnot;					/* 外付け太陽光発電有無 */
	uchar	sunBeyondPwrGenratDevice;			/* 太陽光以外の発電装置 */
	uchar	loadConnectType;					/* 負荷接続タイプ */
} EdatValConstitute_t;

/* 設定値 */
typedef struct{
	/* 押し上げ設定 */
	uchar	pushUpSet;
	/* 出力制御対象 */
	uchar	outputContrObject;
	/* 設備認定容量 */
	float	eqpmtApproveCap;
	/* 発電電力上限設定(上限クリップ) */
	float	genPwrUpperLimit;
	/* クリップ動作 */
	uchar	clipAction;
	/* FIT契約タイプ */
	uchar	fitContractType;
	/* 自家消費タイプ */
	uchar	selfConsumeType;
	/* 発電所ID */
	schar	pwrHouseId[EDAT_VAL_PWRHOUSEID_LEN];
	/* 電力会社サーバ */
	uchar	pwrCompanyServer;
} EdatValSetVal_t;

/* 太陽光発電設定 */
typedef struct{
	/* 固定スケジュール開始年月日 */
	schar	fixShdStartDate[EDAT_VAL_FIXSHDDATE_LEN];
	/* 外付け太陽光発電の定格 */
	float	sunPwrRated;
	/* PVパネル定格 */
	float	pvPanelRating;
} EdatValPV_t;

/* 機器情報 */
typedef struct{
	/* システム_機種名 */
	schar	sysModelName[EDAT_VAL_SYSMDLNM_LEN];
	/* 通信ボード_機種名 */
	schar	commModelName[EDAT_VAL_MDLNAME_LEN];
	/* 通信ボード_製造番号 */
	schar	commSerialNum[EDAT_VAL_SERNUM_LEN];
	/* 通信ボード_FWバージョン */
	schar	commBoardFwVer[EDAT_VAL_BRDFWV_LEN];
	/* OPリモコン_FW */
	schar	opRemoteCtlFw[EDAT_VAL_OPRMCTLFW_LEN];
	/* 太陽光発電設置場所 */
	uchar	sunPwrSetLocation;
	/* 蓄電池設置場所 */
	uchar	btSetLocation;
	/* V2Hスタンド設置場所 */
	uchar	v2hStandSetLocation;
	/* 電気給湯器 */
	uchar	electWaterHeating;
	/* 郵便番号 */
	schar	postalNum[EDAT_VAL_POSTALNUM_LEN];
	/* ECHONET Lite */
	schar	echonetLite[EDAT_VAL_ECHOLITE_LEN];
	/* OPリモコン有無 */
	uchar	opRemoteControlOrnot;
} EdatValMachineInf_t;

/* 設定情報 */
typedef struct{
	float	btCharPwrMaxValueSet;				/* 蓄電池最大充電電力設定 */
	float	carCharPwrValueSet;					/* 車両最大充電電力設定 */
	uchar	carType;							/* 車両タイプ */
	uchar	carChargLimit;						/* 車両充電上限 */
} EdatValSetInf_t;

/* ネットワーク設定 */
typedef struct{
	/* IPアドレス設定 */
	uchar	ipAddrSet;
	/* IPアドレス */
	schar	ipAddress[EDAT_VAL_IPADDRESS_LEN];
	/* サブネットマスク */
	schar	subnetMask[EDAT_VAL_IPADDRESS_LEN];
	/* デフォルトゲートウェイ */
	schar	defaultGateway[EDAT_VAL_IPADDRESS_LEN];
	/* 優先DNS */
	schar	poiorityDns[EDAT_VAL_IPADDRESS_LEN];
	/* 代替DNS */
	schar	replaceDns[EDAT_VAL_IPADDRESS_LEN];
	/* PROXY設定 */
	uchar	proxySet;
	/* PROXYアドレス */
	schar	proxyAddress[EDAT_VAL_PROXYADDRESS_LEN];
	/* ポート番号 */
	schar	portNumber[EDAT_VAL_PORTNUMBER_LEN];
} EdatValNetWorkSet_t;

/* 通信状態 */
typedef struct{
	/* 更新スケジュール取得日 */
	schar	updeShdGetDate[EDAT_VAL_UPDESHDDATE_LEN];
} EdatValCommSte_t;

/* 運転設定 */
typedef struct{
	/* 売電モード充電開始時刻 */
	schar	sellModeCharStartTime[EDAT_VAL_CHARGTIME_LEN];
	/* 売電モード充電停止時刻 */
	schar	sellModeCharStopTime[EDAT_VAL_CHARGTIME_LEN];
	/* 売電モード放電開始時刻 */
	schar	sellModeDisCStartTime[EDAT_VAL_CHARGTIME_LEN];
	/* 売電モード放電停止時刻 */
	schar	sellModeDisCStopTime[EDAT_VAL_CHARGTIME_LEN];
	/* グリーンモード充電開始時刻 */
	schar	greenModeCharStartTime[EDAT_VAL_CHARGTIME_LEN];
	/* グリーンモード充電停止時刻 */
	schar	greenModeCharStopTime[EDAT_VAL_CHARGTIME_LEN];
	/* グリーンモード放電開始時刻 */
	schar	greenModeDisCStartTime[EDAT_VAL_CHARGTIME_LEN];
	/* グリーンモード放電停止時刻 */
	schar	greenModeDisCStopTime[EDAT_VAL_CHARGTIME_LEN];
} EdatValRunSet_t;

/* ENL関連 */
typedef struct{
	uchar	btChargeStyle;					/* 蓄電池クラス_充電方式 */
	uchar	btDischargeStyle;				/* 蓄電池クラス_放電方式 */
	uchar	v2hChargeStyle;					/* V2Hクラス_充電方式 */
	uchar	v2hDischargeStyle;				/* V2Hクラス_放電方式 */
	ushort	btChargePwrSetValue;			/* 蓄電池クラス_充電電力設定値 */
	ushort	btDiscPwrSetValue;				/* 蓄電池クラス_放電電力設定値 */
	ushort	v2hChargePwrSetValue;			/* V2Hクラス_充電電力設定値 */
	ushort	v2hDischargePwrSetValue;		/* V2Hクラス_放電電力設定値 */
	ushort 	btACChargeQuatySetValue;		/* 蓄電池クラス_AC充電量設定値 */
	ushort 	btACDiscQuatySetValue;			/* 蓄電池クラス_AC放電量設定値 */
	ushort 	btDCChargeQuatySetValue;		/* 蓄電池クラス_DC充電量設定値 */
	ushort 	btDCDiscQuatySetValue;			/* 蓄電池クラス_DC放電量設定値 */
	ushort 	v2hACChargeQuatySetValue;		/* V2Hクラス_AC充電電力量 */
	ushort 	v2hACDiscQuatySetValue;			/* V2Hクラス_AC放電電力量 */
} EdatValEnl_t;

/* 見守り関連 */
typedef struct{
	uchar	actLogCollectionCycle;				/* 動作ログ収集周期 */
} EdatValMonitor_t;

/* FW更新関連 */
typedef struct{
	uchar	updateLed;							/* FW更新LEDフラグ */
	ushort	updeNo;								/* 更新 No */
	uchar	analyRes;							/* 解析結果 */
	schar	updTime[EDAT_VAL_SETUP_TIME_LEN];	/* FW更新実施時刻 */
	uchar	boardTgt;							/* BOARD_tgt */
	uchar	invTgt;								/* INV_tgt */
	uchar	pvTgt;								/* PV_tgt */
	uchar	bdcTgt;								/* BDC_tgt */
	uchar	jbTgt;								/* J.B._tgt */
} EdatValFWUpdate_t;

/* メンテナンス関連 */
typedef struct{
	schar	mentanceDate[EDAT_VAL_DATE_LEN];			/* メンテ期間開始日 */
	schar	impltationDesignDay1[EDAT_VAL_DATE_LEN];	/* 実施指定日1 */
	schar	impltationDesignDay2[EDAT_VAL_DATE_LEN];	/* 実施指定日2 */
	uchar	executeMode;								/* 実行モード */
	uchar	maintainResult;								/* メンテ結果 */
} EdatValMaintenance_t;

/* 起動時にINVマイコンに送信するデータ */
/* CMD65 */
typedef struct{
	uchar	commBoardManageMode;		/* V:通信ボード内管理運転モード */
	uchar	emergencyMode;				/* 非常時運転モード */
	ushort	btUpperLimit;				/* 蓄電池充電上限値設定 */
	ushort	btLowerLimit;				/* 蓄電池放電下限値設定 */
} EdatValCmd65_t;

/* CMD75 */
typedef struct{
	uchar	carCharordiscBodyOprat;		/* 車両充放電器本体操作 */
	uchar	pwrcutDischar;				/* 停電時放電動作 */
	uchar	pwrcutDiscPrioritySet;		/* 停電時放電優先設定 */
	uchar	pwrcutCharPrioritySet;		/* 停電時充電優先設定 */
	ushort	carDiscLowerLimitSet;		/* 車両放電下限値設定 */
	ushort	carCharUpperLimitSet;		/* 車両充電上限値設定 */
	uchar	carDiscOrnot;				/* 車両放電有無 */
	uchar	discharPrioritySet;			/* 放電優先設定 */
	ushort	pwrcutDiscLowerLimitSet;	/* 停電時車両放電下限値設定 */
	uchar	chargePrioritySet;			/* 充電優先設定 */
} EdatValCmd75_t;

/* CMD6B */
typedef struct{
	ushort	contaractPwrSet;			/* 契約電力設定値 */
	uchar	pwrcutOutSet;				/* 停電時出力設定 */
	uchar	pwrcutOutStart;				/* 停電時出力開始 */
	uchar	sellValidOrInvalid;			/* 売電有効無効設定 */
} EdatValCmd6b_t;

/* 復元対象のデータ */
/* 整定値 */
typedef struct{
	ushort	sysVoltDetectLevel;			/* OVR(電圧) */
	ushort	sysVoltDetectTime;			/* OVR(秒) */
	ushort	sysUnderVoltDetectLevel;	/* UVR(電圧) */
	ushort	sysUnderVoltDetectTime;		/* UVR(秒) */
	uchar	freRiseDetectLevel;			/* OFR(周波数) */
	ushort	freRiseDetectTime;			/* OFR(秒) */
	uchar	freDropDetectLevel;			/* UFR(周波数) */
	ushort	freDropDetectTime;			/* UFR(秒) */
	ushort	resctTime;					/* 復帰時限 */
	ushort	volRsInhbValue;				/* 電圧上昇抑制値 */
	ushort	volRsInhbattLimit;			/* 電圧上昇抑制動作待機時限 */
	ushort	cotPowerControl;			/* 力率一定制御 */
	ushort	passIndOperDetect;			/* 受動的単独運転検出レベル */
	ushort	freFeedbackGain;			/* 周波数フィードバックゲイン */
	ushort	freLevAllow;				/* 並列時許容周波数レベル */
	ushort	changeTimeIncorDes;			/* 出力増減変化時間 */
} EdatValSetting_t;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/
extern EdatValConstitute_t		g_edat_Constitute;
extern EdatValSetVal_t			g_edat_SetVal;
extern EdatValPV_t				g_edat_Pv;
extern EdatValMachineInf_t		g_edat_MachineInf;
extern EdatValSetInf_t			g_edat_SetInf;
extern EdatValNetWorkSet_t		g_edat_NetWorkSet;
extern EdatValCommSte_t			g_edat_CommSte;
extern EdatValRunSet_t			g_edat_RunSet;
extern EdatValEnl_t				g_edat_Enl;
extern EdatValMonitor_t			g_edat_Monitor;
extern EdatValFWUpdate_t		g_edat_FwUpde;
extern EdatValMaintenance_t		g_edat_Maint;
extern EdatValCmd65_t			g_edat_Cmd65;
extern EdatValCmd75_t			g_edat_Cmd75;
extern EdatValCmd6b_t			g_edat_Cmd6b;
extern EdatValSetting_t			g_edat_Setting;

extern pthread_mutex_t g_edatMutexSet;
/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern slong EDAT_ValInit(void);
extern void EDAT_ValSetAll(void);

#endif /*__EDAT_VAL_G_H__*/
