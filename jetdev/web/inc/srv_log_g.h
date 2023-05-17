/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Hao.JQ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/22												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __SERVICE_LOG_G_H__
#define __SERVICE_LOG_G_H__
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
	uint	fixedSkdLastDay;
	uint	setDate;
	uint	cTime;
}timeSearchHis;

typedef struct 
{
	schar	dd[CTRL_USER_ROLE_LEN+1];
	schar	mm[CTRL_USER_ROLE_LEN+1];
	schar	yyyy[CTRL_USER_ROLE_LEN+1];
}historyPrama;

typedef struct 
{
	schar	timeZoneFlg[CTRL_USER_ROLE_LEN+1];
	schar	timeZone[CTRL_USER_ROLE_LEN+1];
	uint	rpgIn;
	uint	rpgExt;
	uint	salesElectricity;
	uint	buyElectricity;
}bsHistory;

typedef struct 
{
	schar	sysConfig[CTRL_USER_ROLE_LEN+1];
	uint	cTime;
	uint	setDate;
	uint	pvRpgAc;
	uint	pvRpgOth;
	bsHistory	bH[48];
}bsHistoryInfo;

typedef struct 
{
	schar	timeZoneFlg[CTRL_USER_ROLE_LEN+1];
	schar	timeZone[CTRL_USER_ROLE_LEN+1];
	uint	batteryLvl;
	uint	chgPow;
	uint	disChgPow;
}sbDcHistory;

typedef struct 
{
	schar	sysConfig[CTRL_USER_ROLE_LEN+1];
	uint	cTime;
	uint	setDate;
	uint	maxChgVal;
	uint	pvRpgOth;
	sbDcHistory	sDH[48];
}sbDcHistoryInfo;

typedef struct 
{
	schar	timeZoneFlg[CTRL_USER_ROLE_LEN+1];
	schar	timeZone[CTRL_USER_ROLE_LEN+1];
	uint	vehicleBatt1;
	uint	chgPow;
	uint	disChgPow;
}vehicleDcHistory;

typedef struct 
{
	schar	sysConfig[CTRL_USER_ROLE_LEN+1];
	uint	cTime;
	uint	setDate;
	vehicleDcHistory	vDH[48];
}vehicleDcHisInfo;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern schar SRV_GetLog(timeSearchHis *tmp_dataResponse_P);
extern schar SRV_GetLogBs(const historyPrama *tmp_dataRequest_P, bsHistoryInfo *tmp_dataResponse_P, uint *listNum);
extern schar SRV_GetLogSd(const historyPrama *tmp_dataRequest_P, sbDcHistoryInfo *tmp_dataResponse_P, uint *listNum);
extern schar SRV_GetLogVd(const historyPrama *tmp_dataRequest_P, vehicleDcHisInfo *tmp_dataResponse_P, uint *listNum);

#endif /*__SERVICE_LOG_G_H__*/
