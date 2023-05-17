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

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "common_g.h"
#include "ref_cjson_g.h"
#include "ctrl_extct.h"
#include "ctrl_g.h"
#include "util_g.h"
#include "srv_extct_g.h"
#include "frm_dispatcher_g.h"
#include "util_logger_g.h"
#include "frm_dispatcher_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
const ServiceHandle_t DS_ExtctCtrlHandle[CTRL_EXTCT_HANDLE_MAX_NUM] = {
	{"post", "^(/extCTPosConfirmIns)", ctrl_GetExtct}
};

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:method_name														*/
/*--------------------------------------------------------------------------*/
/*	param			:parameter_list														*/
/*--------------------------------------------------------------------------*/
/*	return			:return_type														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/26 : DHC Zhang.WY : 新規作成					*/
/*==========================================================================*/
extern void CTRL_Extct_Handle(const RequestData_t *tmp_requestData_P)
{
	(void)CTRL_Distribute(tmp_requestData_P, DS_ExtctCtrlHandle, MgetArrayMemCount_si(DS_ExtctCtrlHandle));
}

/*==========================================================================*/
/*	Description		:method_name														*/
/*--------------------------------------------------------------------------*/
/*	param			:parameter_list														*/
/*--------------------------------------------------------------------------*/
/*	return			:return_type														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/26 : DHC Zhang.WY : 新規作成					*/
/*==========================================================================*/
static schar ctrl_GetExtct(const RequestData_t *tmp_requestData_P)
{
	LOG_entry("ctrl_GetExtct");

	schar tmp_ret_SC = -1;
	schar pictureId[32];
	schar url[64];
	schar ackFlg;
	schar againstCtConnSet[256];
	CtrlContent_t tmp_ctrContent_t = {0};
	schar *tmp_response_SCP = NULL;
	cJSON *tmp_responseJSon_P = NULL;

	if(NULL == tmp_requestData_P)
	{
		LOG_debug("Error(ctrl_GetExtct): Param is NULL.\n");
		return -1;
	}
	else
	{
		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
		tmp_ret_SC = CTRL_GetContentByJsondata(tmp_requestData_P->Data_SCP, "againstCtConnSet", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			strncpy(againstCtConnSet, tmp_ctrContent_t.ContentValueString, CTRL_USER_NAME_LEN);
		}
		else
		{
			LOG_debug("Error(ctrl_GetExtct: get content(againstCtConnSet) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%s\n", againstCtConnSet);

		tmp_ret_SC = SRV_GetExtct(againstCtConnSet, &ackFlg);
		if(0 == tmp_ret_SC)
		{
			tmp_responseJSon_P = cJSON_CreateObject();
			if(NULL != tmp_responseJSon_P)
			{
				cJSON_AddStringToObject(tmp_responseJSon_P, "ackFlg", &ackFlg);
				
				CTRL_setOutputJson(tmp_responseJSon_P);
			}
			else
			{
				LOG_debug("Error(ctrl_GetExtct): CJson create failed.\n");
			}
		}
		else
		{
			LOG_debug("Error(ctrl_GetExtct): Service handle failed(%d).\n", tmp_ret_SC);
		}

	}
	LOG_leave("ctrl_GetExtct");
	UTIL_SubStrByChar(tmp_requestData_P->Path_info_SCP, '?', url, 64);
	memset(pictureId, '\0', 32);
	// CTRL_writeLog(url, pictureId, "op context");
	return tmp_ret_SC;
}

/*End Of File*/
