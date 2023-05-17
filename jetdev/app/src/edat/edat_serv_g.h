/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Cao.HZ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/05/04												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EDAT_SERV_G_H__
#define __EDAT_SERV_G_H__
/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include "typedef.h"
#include "edat_serv.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern slong EDAT_FindErr(EerrType_t type, const schar *prefix);

extern slong EDAT_SetBoardErrInfo(EdatDesErrInfo_t* fncErrCode);
extern slong EDAT_SetPcsErrInfo(EdatDesErrInfo_t* fncErrCode);
extern slong EDAT_SetRmcErrInfo(EdatDesErrInfo_t* fncErrCode);

extern slong EDAT_ClearBoardErrInfo(EdatClearDesErr_t *fncDesErr);
extern slong EDAT_ClearPcsErrInfo(EdatClearDesErr_t *fncDesErr);
extern slong EDAT_ClearRmcErrInfo(EdatClearDesErr_t *fncDesErr);
extern slong EDAT_ClearAllBoardErrInfo(EdatClearAllDesErr_t *fncErrInfo);
extern slong EDAT_ClearAllPcsErrInfo(EdatClearAllDesErr_t *fncErrInfo);
extern slong EDAT_ClearAllRmcErrInfo(EdatClearAllDesErr_t *fncErrInfo);
extern slong EDAT_ClearAllErrInfo(EdatClearAllErr_t *fncErrCode);

extern slong EDAT_GetBoardErrCode(EdatErrInfo_t *errInfo);
extern slong EDAT_GetPcsErrCode(EdatErrInfo_t *errInfo);
extern slong EDAT_GetRmcErrCode(EdatErrInfo_t *errInfo);
extern slong EDAT_GetErrCode(EdatErrInfo_t *errInfo);
#endif /*__EDAT_SERV_G_H__*/
