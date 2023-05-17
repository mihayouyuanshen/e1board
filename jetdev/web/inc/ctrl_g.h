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
#ifndef __CONTROLLER_G_H__
#define __CONTROLLER_G_H__
#include "common_g.h"
#include "typedef.h"
#include "ref_cjson_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
#define CTRL_CONTENT_TYPE_STRING		((uchar)1u)
#define CTRL_CONTENT_TYPE_INT		((uchar)2u)
#define CTRL_CONTENT_STRING_MAX_LEN	((uchar)511u)

typedef struct{
	schar		*MethodContent_SCP;
	schar		*PathRegEx_SCP;
	schar		(*CtrlHandler_FN)(const RequestData_t *);
}ServiceHandle_t;

typedef struct{
	schar		ContentType_SC;
	schar		ContentStringMaxLen_SC;
	sint		ContentValueInt_SI;
	schar		ContentValueString[CTRL_CONTENT_STRING_MAX_LEN + 1];
}CtrlContent_t;


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern void CTRL_Initialize(void);
extern schar CTRL_Distribute( const RequestData_t *tmp_requestData_P, const ServiceHandle_t *tmp_serviceHandle_P, sint tmp_handleNUM_SI );
extern schar CTRL_IsMatchByPath(const schar *tmp_regEx_UCP, const schar *tmp_message_UCP);
extern schar CTRL_GetContentByFormdata(const uchar *tmp_formdata_UCP, const uchar *tmp_itemName_UCP, CtrlContent_t *tmp_content_P);
extern schar CTRL_GetContentByJsondata(const uchar *tmp_formdata_UCP, const uchar *tmp_itemName_UCP, CtrlContent_t *tmp_content_P);
extern void CTRL_setOutputJson(cJSON *tmp_responseJSon_P);
extern void CTRL_writeLog(schar *url, char *opInfo);
extern void CTRL_writeLogWithAuthenToken(char* authenToken, char *pictureId, char *opInfo);

#endif /*__CONTROLLER_G_H__*/