/****************************************************************************/
/*	Description		:				*/
/*--------------------------------------------------------------------------*/
/*	Author			:Qi.JF													*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/27												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:EMDTR													*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#include <typedef.h>
#include "etim_g.h"
#include "einv_g.h"

#ifndef __EMDTR_G_H__
#define __EMDTR_G_H__


#define	EMDTR_CMN_HOUR_LEN			((ulong)3u)				/* 時の文字列長(文字列終端あり) */
#define	EMDTR_CMN_MIN_LEN			((ulong)3u)				/* 分の文字列長(文字列終端あり) */
#define	EMDTR_PCS_ERR_LEN			((ulong)6u)				/* PCSエラーの最大文字列長 */
#define	EMDTR_PCSERRRATING_LEN		((ulong)3u)				/* PCSエラーレーティングの最大文字列長 */


/* 時刻情報構造体 */
typedef struct {
	schar	hour[EMDTR_CMN_HOUR_LEN];							/* 時（“00”～“23”） */
	schar	min[EMDTR_CMN_MIN_LEN];								/* 分（“00”～“59”） */
}EmdtrTime_t;



/* 充放電時間帯情報構造体 */
typedef struct {
	EmdtrTime_t		chargeStartTime;						/* 充電開始時刻 */
	EmdtrTime_t		chargeStopTime;							/* 充電停止時刻 */
	EmdtrTime_t		dischargeStartTime;						/* 放電開始時刻 */
	EmdtrTime_t		dischargeStopTime;						/* 放電停止時刻 */
}EmdtrChargeTimeZone_t;


/* 動作情報構造体 */
typedef struct {
	/* ★以下はJET対象★ */
	slong	chargeDischargePwr;								/* AC 充放電電力（インバータ部）[W] */
	uchar	inverterOpeState;				/* インバータ動作状態 */
	ushort	rsoc;											/* rSOC値[0.1%] */
	ushort	soc;											/* SOC値[0.1%] */
	uchar	operation;										/* 蓄電池運転動作状態[EPCS011-001] */
	uchar	pwrCut;											/* 停電状態 */
	sshort	pvGenPwr;										/* PV 発電電力(本製品以外のPV)[W] 発電時：正の値、電力消費時：負の値 */
	sshort	revPwrFlow;										/* 逆潮流電力[W] */
	uchar	opeMode;										/* 運転モード */
	slong	chargeDischargePwr2;							/* DC 充放電電力2（コンバータ部）[W] */
	ushort	pvGenPwr2;										/* 太陽光発電電力（内蔵PV）[W] */
	slong	homeLoadPower;									/* 家庭負荷電力[W] */
                    
	ushort	remainingBtW;									/* 電池残量[0.1kWh] */
	ushort	soh;											/* SOH値[0.1%] */
	ushort	pvSelfInputPwr;									/* PV 自立入力電力[W] */
	schar	pcsErr[EMDTR_PCS_ERR_LEN];						/* PCSエラー */
	schar	pcsErrRating[EMDTR_PCSERRRATING_LEN];			/* PCSエラーレーティング */
	uchar	suppressState;									/* 出力抑制状態 */
	uchar	v2hConnectorLockState;							/* コネクタロック状態 */
	uchar	v2hState;										/* 車両充放電器状態 */
	slong	v2hInstantPower;								/* 車両充放電器瞬時充放電電力計測値 [W] */
	uchar	v2hOpeMode;										/* 車両充放電器運転モード(CMD81:非常時運転モード) */
	ulong	v2hRemainingCapacity1;							/* 車載電池の電池残容量1 [Wh] */
	uchar	v2hRemainingCapacity3;							/* 車載電池の電池残容量3 [％] */
	uchar	runModeSetFrom;									/* V:運転モード設定元 */
	uchar	splPowerRunFlag;								/* V:指定電力運転フラグ */
}EmdtrOpesttsInfo_t;

/*  E1ソース新規必要な構造体定義 */
typedef struct {
	EtimTm_t			commBoardCurrentTime;				/* 通信ボード現在時刻 ★JET対象★ */
	schar				commBoardCommState;					/* 通信ボード通信状態 */
	schar				commBoardCommConfirmState[3];		/* 通信ボード通信確認状態 */
	schar				remoteOpeState;						/* リモート運転状態 */
	schar				timeSyncState;						/* 時刻同期状態 */
	ushort				suppressPer;						/* 抑制率％ */
	schar				representOpeMode[2];				/* 表示用運転モード */
	EmdtrOpesttsInfo_t 	opesttsInfo;						/* 動作情報構造体 */
} EmdtrCmd41DataInfo_t;

typedef struct {
	uchar						opeMode;					/* 運転モード */
	uchar						emergencyMode;				/* 非常時運転モード(CMD64:非常時運転モード) */
	uchar						startOrStop;				/* 運転/停止 ★JET対象★ */
	uchar						acUpperLimit;				/* 蓄電池充電上限値設定 */
	uchar						acLowerLimit;				/* 蓄電池放電下限値設定 */
	EmdtrChargeTimeZone_t	chargeTimeZone;				/* 時 */
	uchar						sysConfig;					/* システム構成 */
	uchar						poFlg;						/* 停電 */
}EmdtrOpemodeGet_t;


//ermc to emdtr
extern void EMDTR_SetCurrentTime(EtimTm_t* timeInfo);
extern void EMDTR_GetCmd24DataInfo(void);
extern void EMDTR_GetCmd41DataInfo(EmdtrCmd41DataInfo_t* data);
extern void EMDTR_SetBoardError(schar const* errCode, schar errCodeRating);
extern void EMDTR_ClearBoardError(schar const* errCode);


//inv to emdtr
extern void EMDTR_NotifyCmd24DataInfo(EinvOpemodeGet_t* data1);
extern void EMDTR_NotifyInvInitCommFin();

#endif
