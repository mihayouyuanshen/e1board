/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Hao.JQ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/20												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __SERVICE_MODESET_G_H__
#define __SERVICE_MODESET_G_H__
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
	schar	opMode[CTRL_USER_ROLE_LEN + 1];
	schar	emerMode[CTRL_USER_ROLE_LEN + 1];
	schar	salesChgIsb[CTRL_USER_ROLE_LEN + 1];
	schar	salesChgToc[CTRL_USER_ROLE_LEN + 1];
	schar	salesDcIsb[CTRL_USER_ROLE_LEN + 1];
	schar	salesDcToc[CTRL_USER_ROLE_LEN + 1];
	schar	grnChgIsb[CTRL_USER_ROLE_LEN + 1];
	schar	grnChgToc[CTRL_USER_ROLE_LEN + 1];
	schar	grnDischgIsb[CTRL_USER_ROLE_LEN + 1];
	schar	grnDischgToc[CTRL_USER_ROLE_LEN + 1];
	schar	poFlg[CTRL_USER_ROLE_LEN + 1];
	schar	sysConfig[CTRL_USER_ROLE_LEN + 1];
	schar	runStop[CTRL_USER_ROLE_LEN + 1];
	schar	connLockSt[CTRL_USER_ROLE_LEN + 1];
}modeSet;

typedef struct 
{
	schar	opMode[CTRL_USER_ROLE_LEN + 1];
	schar	emerMode[CTRL_USER_ROLE_LEN + 1];
	uint	salesChgIsb;
	uint	salesChgToc;
	uint	salesDcIsb;
	uint	salesDcToc;
	uint	grnChgIsb;
	uint	grnChgToc;
	uint	grnDischgIsb;
	uint	grnDischgToc;
}modeSetPara;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern schar SRV_GetModeset(modeSet *tmp_dataResponse_P);

#endif /*__SERVICE_MODESET_G_H__*/
