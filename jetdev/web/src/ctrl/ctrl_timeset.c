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
#include "ctrl_timeset.h"
#include "ctrl_g.h"
#include "util_g.h"
#include "srv_timeset_g.h"
#include "frm_dispatcher_g.h"
#include "util_logger_g.h"
#include "frm_dispatcher_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
const ServiceHandle_t DS_TimesetCtrlHandle[CTRL_TIMESET_HANDLE_MAX_NUM] = {
	{"post", "^(/timeSet)", ctrl_GetTimeset}
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
extern void CTRL_Timeset_Handle(const RequestData_t *tmp_requestData_P)
{
	(void)CTRL_Distribute(tmp_requestData_P, DS_TimesetCtrlHandle, MgetArrayMemCount_si(DS_TimesetCtrlHandle));
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
static schar ctrl_GetTimeset(const RequestData_t *tmp_requestData_P)
{
	LOG_entry("ctrl_GetTimeset");

	schar tmp_ret_SC = -1;
	schar pictureId[32];
	schar url[64];
	uint ackFlg;
	uint cTime;
	CtrlContent_t tmp_ctrContent_t = {0};
	schar *tmp_response_SCP = NULL;
	cJSON *tmp_responseJSon_P = NULL;

	if(NULL == tmp_requestData_P)
	{
		LOG_debug("Error(ctrl_GetTimeset): Param is NULL.\n");
		return -1;
	}
	else
	{
		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_INT;
		tmp_ret_SC = CTRL_GetContentByJsondata(tmp_requestData_P->Data_SCP, "cTime", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			cTime = tmp_ctrContent_t.ContentValueInt_SI;
		}
		else
		{
			LOG_debug("Error(ctrl_GetTimeset: get content(cTime) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%d\n", cTime);

		tmp_ret_SC = SRV_GetTimeset(&cTime, &ackFlg);
		if(0 == tmp_ret_SC)
		{
			tmp_responseJSon_P = cJSON_CreateObject();
			if(NULL != tmp_responseJSon_P)
			{
				cJSON_AddNumberToObject(tmp_responseJSon_P, "ackFlg", ackFlg);
				
				CTRL_setOutputJson(tmp_responseJSon_P);
			}
			else
			{
				LOG_debug("Error(ctrl_GetTimeset): CJson create failed.\n");
			}
		}
		else
		{
			LOG_debug("Error(ctrl_GetTimeset): Service handle failed(%d).\n", tmp_ret_SC);
		}

	}
	LOG_leave("ctrl_GetTimeset");
	UTIL_SubStrByChar(tmp_requestData_P->Path_info_SCP, '?', url, 64);
	memset(pictureId, '\0', 32);
	// CTRL_writeLog(url, pictureId, "op context");
	return tmp_ret_SC;
}

/*End Of File*/
