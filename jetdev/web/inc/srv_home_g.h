/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Hao.JQ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/19												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __SERVICE_HOME_G_H__
#define __SERVICE_HOME_G_H__
#include "ctrl_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
#define	CTRL_USER_NAME_LEN							((uchar)31u)
#define	CTRL_USER_ROLE_LEN							((uchar)31u)
#define	CTRL_USER_RESPONSE_LIST_MAX_NUM				((uchar)10u)

typedef struct 
{
	uint	batteryCharging;
	uint	batteryDischarge;
	uint	buyElectricityE1;
	uint	buyElectricityC2;
	uint	V2hCharging;
	uint	V2hDischarge;
	uint	sunlightPvGen;
	uint	extSunlightPv;
	uint	selfRelOperSwitch;
	uint	pvOutDischarge;
	uint	pvOutRet;
}cxFile;

typedef struct 
{
	schar	sysConfig[CTRL_USER_NAME_LEN + 1];
	schar	opMode[CTRL_USER_ROLE_LEN + 1];
	schar	emerMode[CTRL_USER_ROLE_LEN + 1];
	schar	opSetReq[CTRL_USER_ROLE_LEN + 1];
	schar	outputCtrlSt[CTRL_USER_ROLE_LEN + 1];
	uint	outputCtrlPct;
	schar	connLockSt[CTRL_USER_ROLE_LEN + 1];
	schar	desigPowOpFlg[CTRL_USER_ROLE_LEN + 1];
	schar	runStop[CTRL_USER_ROLE_LEN + 1];
	schar	noticeAddFlg[CTRL_USER_ROLE_LEN + 1];
	schar	fwUpdNo[CTRL_USER_ROLE_LEN + 1];
	schar	fwUpdMode[CTRL_USER_ROLE_LEN + 1];
	cxFile	testFile;
	schar	sbOpSt[CTRL_USER_ROLE_LEN + 1];
	uint	pvAvgPow;
	uint	extPvAvgPow;
	schar	extPvExist[CTRL_USER_ROLE_LEN + 1];
	uint	rcAvgPow;
	uint	vehicleCdtAvgPow;
	uint	sbCdtAvgPow;
	schar	purPowDfv[CTRL_USER_ROLE_LEN + 1];
	schar	vehicleDcSt[CTRL_USER_ROLE_LEN + 1];
	schar	mainteStatus[CTRL_USER_ROLE_LEN + 1];
	schar	poFlg[CTRL_USER_ROLE_LEN + 1];
	schar	mainteOperation[CTRL_USER_ROLE_LEN + 1];
	uint	evBattLvl3;
	uint	socFlg;
	uint	mainteSchedule;
	uint	cTime;
}homeInfo;

typedef struct 
{
	ulong	timeStamp;
}homeRequest;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern schar SRV_GetHome(const homeRequest *tmp_dataRequest_P, homeInfo *tmp_dataResponse_P);
#endif /*__SERVICE_HOME_G_H__*/
