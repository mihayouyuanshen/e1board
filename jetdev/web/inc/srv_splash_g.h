/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Hao.JQ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/16												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __SERVICE_SPLASH_G_H__
#define __SERVICE_SPLASH_G_H__
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
	schar	cxDo[CTRL_USER_NAME_LEN + 1];
	ulong	cTime;
	schar	outputCtrlT[CTRL_USER_ROLE_LEN + 1];
	schar	ntpSrvComStat[CTRL_USER_ROLE_LEN + 1];
	schar	extPvExist[CTRL_USER_ROLE_LEN + 1];
	schar	mcErrFlag[CTRL_USER_ROLE_LEN + 1];
	schar	dcbSn[CTRL_USER_ROLE_LEN + 1];
	schar	fwUpd[CTRL_USER_ROLE_LEN + 1];
	schar	modelName[CTRL_USER_ROLE_LEN + 1];
	schar	sysSn[CTRL_USER_ROLE_LEN + 1];
	schar	vehicleDcModel[CTRL_USER_ROLE_LEN + 1];
	schar	vehicleDcSn[CTRL_USER_ROLE_LEN + 1];
	schar	authToken[CTRL_USER_ROLE_LEN + 1];
}startupComInfo;

typedef struct 
{
	schar	noticeToken[CTRL_USER_ROLE_LEN + 1];
	schar	custAplId[CTRL_USER_ROLE_LEN + 1];
	schar	serviceAplId[CTRL_USER_ROLE_LEN + 1];
	schar	hostModel[CTRL_USER_ROLE_LEN + 1];
	schar	appVer[CTRL_USER_ROLE_LEN + 1];
}startupRequest;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern schar SRV_GetSplash(const startupRequest *tmp_dataRequest_P, startupComInfo *tmp_dataResponse_P, schar *tmp_sessionId_t);
extern schar SRV_GetSplInfo(startupComInfo *tmp_dataResponse_P);

#endif /*__SERVICE_SPLASH_G_H__*/
