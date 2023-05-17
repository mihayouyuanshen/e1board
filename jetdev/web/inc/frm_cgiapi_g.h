/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC ZHANG.WY											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/07												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __CGIAPI_G_H__
#define __CGIAPI_G_H__
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#include <typedef.h>

/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern schar *FRM_CgiGetFormData(schar *poststr);
extern schar* FRM_CgiGetQueryString(void);
extern void FRM_CgiCookieString(schar *tmp_name_cp, schar *tmp_result_cp, int tmp_max_i);
extern void FRM_CgiHeaderCookieSetString(schar *tmp_name_cp, schar *tmp_value_cp, int tmp_secondsToLive_i, schar *tmp_path_cp, schar *tmp_domain_cp);
extern void FRM_CgiHeaderContentType(schar *tmp_mimeType_scp);
extern void FRM_CgiHeaderStatus(int status, char *statusMessage);

#endif /*__CGIAPI_G_H__*/

