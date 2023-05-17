/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Zhang.WY											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/24												*/
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
#include "ctrl_list.h"
#include "ctrl_g.h"
#include "util_g.h"
#include "srv_list_g.h"
#include "frm_dispatcher_g.h"
#include "util_logger_g.h"
#include "frm_dispatcher_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
const ServiceHandle_t DS_NoticeCtrlHandle[CTRL_NOTICE_HANDLE_MAX_NUM] = {
	{"get", "^(/notice)", ctrl_GetNotice},
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
/*	history			:2023/04/25 : DHC Zhang.WY : 新規作成					*/
/*==========================================================================*/
extern void CTRL_Notice_Handle(const RequestData_t *tmp_requestData_P)
{
	(void)CTRL_Distribute(tmp_requestData_P, DS_NoticeCtrlHandle, MgetArrayMemCount_si(DS_NoticeCtrlHandle));
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
/*	history			:2023/04/25 : DHC Zhang.WY : 新規作成					*/
/*==========================================================================*/
static schar ctrl_GetNotice(const RequestData_t *tmp_requestData_P)
{
	LOG_entry("ctrl_GetNotice");
	uint maxNum = 0;
	uint listNum = 0;
	uint i = 0;
	schar tmp_ret_SC = -1;
	schar url[64];
	noticeList tmp_dataResponse_t[100];
	CtrlContent_t tmp_ctrContent_t = {0};
	schar *tmp_response_SCP = NULL;
	cJSON *tmp_responseJSon_P = NULL;
	cJSON *tmp_array1 = NULL;
	cJSON *tmp_array2 = NULL;

	if (NULL == tmp_requestData_P)
	{
		LOG_debug("Error(ctrl_GetNotice): Param is NULL.\n");
		return -1;
	}
	else
	{
		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_INT;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "maxNum", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			maxNum = tmp_ctrContent_t.ContentValueInt_SI;
		}
		else
		{
			LOG_debug("Error(ctrl_GetNotice: get content(maxNum) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%d\n", maxNum);

		tmp_ret_SC = SRV_GetNotice(&maxNum, tmp_dataResponse_t, &listNum);
		if (0 == tmp_ret_SC)
		{
			tmp_responseJSon_P = cJSON_CreateObject();
			cJSON* arrayJson1 = cJSON_CreateArray();
			cJSON* arrayJson2 = cJSON_CreateArray();
			arrayJson1 = cJSON_AddArrayToObject(tmp_responseJSon_P, "noticeInfo");
			arrayJson2 = cJSON_AddArrayToObject(tmp_responseJSon_P, "fwUpdInfo");
			if (NULL != tmp_responseJSon_P)
			{
				for(i = 0; i < listNum; i++)
				{
					tmp_array1 = cJSON_CreateObject();
					cJSON_AddStringToObject(tmp_array1, "err", tmp_dataResponse_t->noteInfo[i].err);
					cJSON_AddStringToObject(tmp_array1, "errRtg", tmp_dataResponse_t->noteInfo[i].errRtg);
					cJSON_AddNumberToObject(tmp_array1, "writeDate", tmp_dataResponse_t->noteInfo[i].writeDate);
					cJSON_AddItemToArray(arrayJson1, tmp_array1);

					tmp_array2 = cJSON_CreateObject();
					cJSON_AddStringToObject(tmp_array2, "err", tmp_dataResponse_t->fwInfo[i].err);
					cJSON_AddStringToObject(tmp_array2, "fwUpdMsg", tmp_dataResponse_t->fwInfo[i].fwUpdMsg);
					cJSON_AddNumberToObject(tmp_array2, "writeDate", tmp_dataResponse_t->fwInfo[i].writeDate);
					cJSON_AddItemToArray(arrayJson2, tmp_array2);

				}
				
				CTRL_setOutputJson(tmp_responseJSon_P);
			}
			else
			{
				LOG_debug("Error(ctrl_GetNotice): CJson create failed.\n");
			}
		}
		else
		{
			LOG_debug("Error(ctrl_GetNotice): Service handle failed(%d).\n", tmp_ret_SC);
		}
	}
	LOG_leave("ctrl_GetNotice");

	UTIL_SubStrByChar(tmp_requestData_P->Path_info_SCP, '?', url, 64);
	CTRL_writeLog(url, "op context");

	return tmp_ret_SC;
}
/*End Of File*/
