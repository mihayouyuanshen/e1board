/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Hao.JQ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/23												*/
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
#include "ctrl_log.h"
#include "ctrl_g.h"
#include "srv_log_g.h"
#include "frm_dispatcher_g.h"
#include "util_logger_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
const ServiceHandle_t DS_LogCtrlHandle[CTRL_LOG_HANDLE_MAX_NUM] = {
	{"get", "^(/history_search)", ctrl_GetLog},
	{"get", "^(/tradeHist)", ctrl_GetLogBs},
	{"get", "^(/records)", ctrl_GetLogSd},
	{"get", "^(/vehicleRecords)", ctrl_GetLogVd}
};

/*--------------------------------------------------------------------------*/
/*	公開関数定義													
		*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/23 : DHC Hao.JQ : 新規作成						*/
/*==========================================================================*/
extern void CTRL_Log_Handle(const RequestData_t *tmp_requestData_P)
{
	(void)CTRL_Distribute(tmp_requestData_P, DS_LogCtrlHandle, MgetArrayMemCount_si(DS_LogCtrlHandle));
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/23 : DHC Hao.JQ : 新規作成						*/
/*==========================================================================*/
static schar ctrl_GetLogBs(const RequestData_t *tmp_requestData_P)
{
	LOG_entry("ctrl_GetLogBs");

	uint listNum = 0;
	uint i = 0;
	schar tmp_ret_SC = -1;
	bsHistoryInfo tmp_dataResponse_t = {0};
	historyPrama tmp_dataRequest_t = {0};
	CtrlContent_t tmp_ctrContent_t = {0};
	schar *tmp_response_SCP = NULL;
	cJSON *tmp_responseJSon_P = NULL;
	cJSON *tmp_array = NULL;

	if(NULL == tmp_requestData_P)
	{
		LOG_debug("Error(ctrl_GetLogBs): Param is NULL.\n");
		return -1;
	}
	else
	{
		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "dd", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			strncpy(tmp_dataRequest_t.dd, tmp_ctrContent_t.ContentValueString, CTRL_USER_NAME_LEN);
		}
		else
		{
			LOG_debug("Error(ctrl_GetLogBs: get content(dd) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%s\n", tmp_dataRequest_t.dd);

		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "mm", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			strncpy(tmp_dataRequest_t.mm, tmp_ctrContent_t.ContentValueString, CTRL_USER_NAME_LEN);
		}
		else
		{
			LOG_debug("Error(ctrl_GetLogBs: get content(mm) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%s\n", tmp_dataRequest_t.mm);

		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "yyyy", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			strncpy(tmp_dataRequest_t.yyyy, tmp_ctrContent_t.ContentValueString, CTRL_USER_NAME_LEN);
		}
		else
		{
			LOG_debug("Error(ctrl_GetLogBs: get content(yyyy) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%s\n", tmp_dataRequest_t.yyyy);

		tmp_ret_SC = SRV_GetLogBs(&tmp_dataRequest_t, &tmp_dataResponse_t, &listNum);
		if(0 == tmp_ret_SC)
		{
			tmp_responseJSon_P = cJSON_CreateObject();
			cJSON* arrayJson = cJSON_CreateObject();
			arrayJson = cJSON_AddArrayToObject(tmp_responseJSon_P, "bsHistory");
			if(NULL != tmp_responseJSon_P)
			{
				cJSON_AddNumberToObject(tmp_responseJSon_P, "pvRpgAc", tmp_dataResponse_t.pvRpgAc);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "cTime", tmp_dataResponse_t.cTime);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "setDate", tmp_dataResponse_t.setDate);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "pvRpgOth", tmp_dataResponse_t.pvRpgOth);
				cJSON_AddStringToObject(tmp_responseJSon_P, "sysConfig", tmp_dataResponse_t.sysConfig);
				for(i = 0; i < listNum; i++)
				{
					tmp_array = cJSON_CreateObject();
					cJSON_AddStringToObject(tmp_array, "timeZoneFlg", tmp_dataResponse_t.bH[i].timeZoneFlg);
					cJSON_AddStringToObject(tmp_array, "timeZone", tmp_dataResponse_t.bH[i].timeZone);
					cJSON_AddNumberToObject(tmp_array, "rpgExt", tmp_dataResponse_t.bH[i].rpgExt);
					cJSON_AddNumberToObject(tmp_array, "rpgIn", tmp_dataResponse_t.bH[i].rpgIn);
					cJSON_AddNumberToObject(tmp_array, "salesElectricity", tmp_dataResponse_t.bH[i].salesElectricity);
					cJSON_AddNumberToObject(tmp_array, "buyElectricity", tmp_dataResponse_t.bH[i].buyElectricity);
					cJSON_AddItemToArray(arrayJson, tmp_array);
				}

				CTRL_setOutputJson(tmp_responseJSon_P);
			}
			else
			{
				LOG_debug("Error(ctrl_GetLogBs): CJson create failed.\n");
			}
		}
		else
		{
			LOG_debug("Error(ctrl_GetLogBs): Service handle failed(%d).\n", tmp_ret_SC);
		}

	}
	return tmp_ret_SC;
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/24 : DHC Hao.JQ : 新規作成						*/
/*==========================================================================*/
static schar ctrl_GetLog(const RequestData_t *tmp_requestData_P)
{
	schar tmp_ret_SC = -1;
	timeSearchHis tmp_dataResponse_t = {0};
	//startupRequest tmp_dataRequest_t = {0};
	CtrlContent_t tmp_ctrContent_t = {0};
	schar *tmp_response_SCP = NULL;
	cJSON *tmp_responseJSon_P = NULL;

	if(NULL == tmp_requestData_P)
	{
		LOG_debug("Error(ctrl_GetLog): Param is NULL.\n");
		return -1;
	}
	else
	{
		tmp_ret_SC = SRV_GetLog(&tmp_dataResponse_t);
		if(0 == tmp_ret_SC)
		{
			tmp_responseJSon_P = cJSON_CreateObject();
			if(NULL != tmp_responseJSon_P)
			{
				cJSON_AddNumberToObject(tmp_responseJSon_P, "fixedSkdLastDay", tmp_dataResponse_t.fixedSkdLastDay);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "cTime", tmp_dataResponse_t.cTime);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "setDate", tmp_dataResponse_t.setDate);
				
				CTRL_setOutputJson(tmp_responseJSon_P);
			}
			else
			{
				LOG_debug("Error(ctrl_GetLog): CJson create failed.\n");
			}
		}
		else
		{
			LOG_debug("Error(ctrl_GetLog): Service handle failed(%d).\n", tmp_ret_SC);
		}

	}
	return tmp_ret_SC;
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
/*	history			:2023/05/03 : DHC Zhang.WY : 新規作成					*/
/*==========================================================================*/
static schar ctrl_GetLogSd(const RequestData_t *tmp_requestData_P)
{
	LOG_entry("ctrl_GetLogSd");

	uint listNum = 0;
	uint i = 0;
	schar tmp_ret_SC = -1;
	sbDcHistoryInfo tmp_dataResponse_t = {0};
	historyPrama tmp_dataRequest_t = {0};
	CtrlContent_t tmp_ctrContent_t = {0};
	schar *tmp_response_SCP = NULL;
	cJSON *tmp_responseJSon_P = NULL;
	cJSON *tmp_array = NULL;

	if(NULL == tmp_requestData_P)
	{
		LOG_debug("Error(ctrl_GetLogSd): Param is NULL.\n");
		return -1;
	}
	else
	{
		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "dd", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			strncpy(tmp_dataRequest_t.dd, tmp_ctrContent_t.ContentValueString, CTRL_USER_NAME_LEN);
		}
		else
		{
			LOG_debug("Error(ctrl_GetLogSd: get content(dd) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%s\n", tmp_dataRequest_t.dd);

		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "mm", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			strncpy(tmp_dataRequest_t.mm, tmp_ctrContent_t.ContentValueString, CTRL_USER_NAME_LEN);
		}
		else
		{
			LOG_debug("Error(ctrl_GetLogSd: get content(mm) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%s\n", tmp_dataRequest_t.mm);

		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "yyyy", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			strncpy(tmp_dataRequest_t.yyyy, tmp_ctrContent_t.ContentValueString, CTRL_USER_NAME_LEN);
		}
		else
		{
			LOG_debug("Error(ctrl_GetLogSd: get content(yyyy) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%s\n", tmp_dataRequest_t.yyyy);

		tmp_ret_SC = SRV_GetLogSd(&tmp_dataRequest_t, &tmp_dataResponse_t, &listNum);
		if(0 == tmp_ret_SC)
		{
			tmp_responseJSon_P = cJSON_CreateObject();
			cJSON* arrayJson = cJSON_CreateObject();
			arrayJson = cJSON_AddArrayToObject(tmp_responseJSon_P, "sbDcHistory");
			if(NULL != tmp_responseJSon_P)
			{
				cJSON_AddNumberToObject(tmp_responseJSon_P, "maxChgVal", tmp_dataResponse_t.maxChgVal);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "cTime", tmp_dataResponse_t.cTime);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "setDate", tmp_dataResponse_t.setDate);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "pvRpgOth", tmp_dataResponse_t.pvRpgOth);
				cJSON_AddStringToObject(tmp_responseJSon_P, "sysConfig", tmp_dataResponse_t.sysConfig);
				for(i = 0; i < listNum; i++)
				{
					tmp_array = cJSON_CreateObject();
					cJSON_AddStringToObject(tmp_array, "timeZoneFlg", tmp_dataResponse_t.sDH[i].timeZoneFlg);
					cJSON_AddStringToObject(tmp_array, "timeZone", tmp_dataResponse_t.sDH[i].timeZone);
					cJSON_AddNumberToObject(tmp_array, "batteryLvl", tmp_dataResponse_t.sDH[i].batteryLvl);
					cJSON_AddNumberToObject(tmp_array, "chgPow", tmp_dataResponse_t.sDH[i].chgPow);
					cJSON_AddNumberToObject(tmp_array, "disChgPow", tmp_dataResponse_t.sDH[i].disChgPow);

					cJSON_AddItemToArray(arrayJson, tmp_array);
				}

				CTRL_setOutputJson(tmp_responseJSon_P);
			}
			else
			{
				LOG_debug("Error(ctrl_GetLogSd): CJson create failed.\n");
			}
		}
		else
		{
			LOG_debug("Error(ctrl_GetLogSd): Service handle failed(%d).\n", tmp_ret_SC);
		}

	}
	return tmp_ret_SC;
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
/*	history			:2023/05/03 : DHC Zhang.WY : 新規作成					*/
/*==========================================================================*/
static schar ctrl_GetLogVd(const RequestData_t *tmp_requestData_P)
{
	LOG_entry("ctrl_GetLogVd");

	uint listNum = 0;
	uint i = 0;
	schar tmp_ret_SC = -1;
	vehicleDcHisInfo tmp_dataResponse_t = {0};
	historyPrama tmp_dataRequest_t = {0};
	CtrlContent_t tmp_ctrContent_t = {0};
	schar *tmp_response_SCP = NULL;
	cJSON *tmp_responseJSon_P = NULL;
	cJSON *tmp_array = NULL;

	if(NULL == tmp_requestData_P)
	{
		LOG_debug("Error(ctrl_GetLogVd): Param is NULL.\n");
		return -1;
	}
	else
	{
		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "dd", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			strncpy(tmp_dataRequest_t.dd, tmp_ctrContent_t.ContentValueString, CTRL_USER_NAME_LEN);
		}
		else
		{
			LOG_debug("Error(ctrl_GetLogVd: get content(dd) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%s\n", tmp_dataRequest_t.dd);

		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "mm", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			strncpy(tmp_dataRequest_t.mm, tmp_ctrContent_t.ContentValueString, CTRL_USER_NAME_LEN);
		}
		else
		{
			LOG_debug("Error(ctrl_GetLogVd: get content(mm) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%s\n", tmp_dataRequest_t.mm);

		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "yyyy", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			strncpy(tmp_dataRequest_t.yyyy, tmp_ctrContent_t.ContentValueString, CTRL_USER_NAME_LEN);
		}
		else
		{
			LOG_debug("Error(ctrl_GetLogVd: get content(yyyy) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%s\n", tmp_dataRequest_t.yyyy);

		tmp_ret_SC = SRV_GetLogVd(&tmp_dataRequest_t, &tmp_dataResponse_t, &listNum);
		if(0 == tmp_ret_SC)
		{
			tmp_responseJSon_P = cJSON_CreateObject();
			cJSON* arrayJson = cJSON_CreateObject();
			arrayJson = cJSON_AddArrayToObject(tmp_responseJSon_P, "vehicleDcHistory");
			if(NULL != tmp_responseJSon_P)
			{
				cJSON_AddNumberToObject(tmp_responseJSon_P, "cTime", tmp_dataResponse_t.cTime);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "setDate", tmp_dataResponse_t.setDate);
				cJSON_AddStringToObject(tmp_responseJSon_P, "sysConfig", tmp_dataResponse_t.sysConfig);
				for(i = 0; i < listNum; i++)
				{
					tmp_array = cJSON_CreateObject();
					cJSON_AddStringToObject(tmp_array, "timeZoneFlg", tmp_dataResponse_t.vDH[i].timeZoneFlg);
					cJSON_AddStringToObject(tmp_array, "timeZone", tmp_dataResponse_t.vDH[i].timeZone);
					cJSON_AddNumberToObject(tmp_array, "vehicleBatt1", tmp_dataResponse_t.vDH[i].vehicleBatt1);
					cJSON_AddNumberToObject(tmp_array, "chgPow", tmp_dataResponse_t.vDH[i].chgPow);
					cJSON_AddNumberToObject(tmp_array, "disChgPow", tmp_dataResponse_t.vDH[i].disChgPow);

					cJSON_AddItemToArray(arrayJson, tmp_array);
				}

				CTRL_setOutputJson(tmp_responseJSon_P);
			}
			else
			{
				LOG_debug("Error(ctrl_GetLogVd): CJson create failed.\n");
			}
		}
		else
		{
			LOG_debug("Error(ctrl_GetLogVd): Service handle failed(%d).\n", tmp_ret_SC);
		}

	}
	return tmp_ret_SC;
}

/*End Of File*/
