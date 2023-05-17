/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Zhang.WY											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/26												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __SERVICE_CONFIG_G_H__
#define __SERVICE_CONFIG_G_H__
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
#define	SETTING_TABLE_LEN							((uchar)3u)

typedef struct 
{
	schar	sysConfig[CTRL_USER_ROLE_LEN + 1];
	schar	extPvExist[CTRL_USER_ROLE_LEN + 1];
	schar	othDevice[CTRL_USER_ROLE_LEN + 1];
	schar	clipAction[CTRL_USER_ROLE_LEN + 1];
	schar	fitType[CTRL_USER_ROLE_LEN + 1];
	schar	selfUseTyp[CTRL_USER_ROLE_LEN + 1];
	uint	setDate;
	uint	contractVal;
	uint	pvPanel;
	uint	eqCapacity;
	uint	rpgUl;
	schar	salesValid[CTRL_USER_ROLE_LEN + 1];
	schar	outputCtrlT[CTRL_USER_ROLE_LEN + 1];
	schar	powStation[CTRL_USER_ROLE_LEN + 1];
	uint	outputChgRate;
	schar	electricSrv[CTRL_USER_ROLE_LEN + 1];
	schar	pushUp[CTRL_USER_ROLE_LEN + 1];
	uint	pcsKwh;
	schar	orcExist[CTRL_USER_ROLE_LEN + 1];
	schar	loadConnType[CTRL_USER_ROLE_LEN + 1];
	uint	sysFrequency;
	uint	setValueTbl[SETTING_TABLE_LEN];
	uint	sysVpkLvlList[SETTING_TABLE_LEN];
	uint	sysVpkLvl;
	uint	sysVpkTimeList[SETTING_TABLE_LEN];
	uint	sysVpkTime;
	uint	sysTrchLvlList[SETTING_TABLE_LEN];
	uint	sysTrchLvl;
	uint	sysTrchTimeList[SETTING_TABLE_LEN];
	uint	sysTrchTime;
	uint	freqRiseLvlList[SETTING_TABLE_LEN];
	uint	freqRiseLvl;
	uint	freqRiseTimeList[SETTING_TABLE_LEN];
	uint	freqRiseTime;
	uint	freqDropLvlList[SETTING_TABLE_LEN];
	uint	freqDropLvl;
	uint	freqDropTimeList[SETTING_TABLE_LEN];
	uint	freqDropTime;
	uint	rtnTimeList[SETTING_TABLE_LEN];
	uint	rtnTime;
	uint	restraintValList[SETTING_TABLE_LEN];
	uint	restraintVal;
	uint	restraintStList[SETTING_TABLE_LEN];
	uint	restraintSt;
	uint	powFactorCtrlList[SETTING_TABLE_LEN];
	uint	powFactorCtrl;
	uint	singleOpLvlList[SETTING_TABLE_LEN];
	uint	singleOpLvl;
	uint	freqFbGainList[SETTING_TABLE_LEN];
	uint	freqFbGain;
	uint	allowFreqLvlList[SETTING_TABLE_LEN];
	uint	allowFreqLvl;
}configSetInfo;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern schar SRV_GetConfig(configSetInfo *tmp_dataResponse_P);

#endif /*__SERVICE_CONFIG_G_H__*/
