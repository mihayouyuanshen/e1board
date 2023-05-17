/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Sun.YF												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/08												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __SERVICE_G_H__
#define __SERVICE_G_H__
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
	schar	Name_SC[CTRL_USER_NAME_LEN + 1];
	uint	Age_UI;
	schar	Role_SC[CTRL_USER_ROLE_LEN + 1];
	uint	Grade_UI;
}UserDataInfo_t;

typedef struct
{
	schar	Name_SC[CTRL_USER_NAME_LEN + 1];
	uint	AgeMax_UI;
	uint	Age_UI;
	uint	AgeMin_UI;
	schar	Role_SC[CTRL_USER_ROLE_LEN + 1];
	uint	GradeMax_UI;
	uint	Grade_UI;
	uint	GradeMin_UI;
}UserDataListQuery_t;

typedef struct
{
	sint			ListCurrentNum;
	sint			ListMaxNum;
	UserDataInfo_t	DataList[CTRL_USER_RESPONSE_LIST_MAX_NUM];
}UserDataInfoList_t;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
schar SRV_USER_GetSingle(const UserDataInfo_t *tmp_dataRequest_P, UserDataInfo_t *tmp_dataResponse_P);
schar SRV_USER_GetList(const UserDataListQuery_t *tmp_listQuery_P, UserDataInfoList_t *tmp_dataResponse_P);
schar SRV_USER_ModifySingle(const UserDataInfo_t *tmp_dataRequest_P, UserDataInfo_t *tmp_dataResponse_P);

#endif /*__SERVICE_G_H__*/