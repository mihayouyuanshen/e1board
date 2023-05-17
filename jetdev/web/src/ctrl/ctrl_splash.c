/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Hao.JQ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/12												*/
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
#include "ctrl_splash.h"
#include "ctrl_g.h"
#include "util_g.h"
#include "srv_splash_g.h"
#include "frm_dispatcher_g.h"
#include "util_logger_g.h"
#include "frm_dispatcher_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
const ServiceHandle_t DS_SplashCtrlHandle[CTRL_SPLASH_HANDLE_MAX_NUM] = {
	{"get", "^(/start/splashInfo)", ctrl_GetSplInfo},
	{"get", "^(/start/splash)", ctrl_GetSplash}
	
};

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
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
/*	history			:2023/04/13 : DHC Wang.M : 新規作成						*/
/*==========================================================================*/
extern void CTRL_Start_Handle(const RequestData_t *tmp_requestData_P)
{
	(void)CTRL_Distribute(tmp_requestData_P, DS_SplashCtrlHandle, MgetArrayMemCount_si(DS_SplashCtrlHandle));
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
/*	history			:2023/04/12 : DHC Wang.M : 新規作成						*/
/*==========================================================================*/
static schar ctrl_GetSplash(const RequestData_t *tmp_requestData_P)
{
	LOG_entry("ctrl_GetSplash");

	schar tmp_ret_SC = -1;
	schar tmp_sessionId_t[17];
	startupComInfo tmp_dataResponse_t = {0};
	startupRequest tmp_dataRequest_t = {0};
	CtrlContent_t tmp_ctrContent_t = {0};
	schar *tmp_response_SCP = NULL;
	cJSON *tmp_responseJSon_P = NULL;

	if (NULL == tmp_requestData_P)
	{
		LOG_debug("Error(ctrl_GetSplash): Param is NULL.\n");
		return -1;
	}
	else
	{
		memset(&tmp_ctrContent_t, '\0', sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "noticeToken", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			strncpy(tmp_dataRequest_t.noticeToken, tmp_ctrContent_t.ContentValueString, CTRL_USER_NAME_LEN);
		}
		else
		{
			LOG_debug("Error(ctrl_GetSplash: get content(noticeToken) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%s\n", tmp_dataRequest_t.noticeToken);

		memset(&tmp_ctrContent_t, '\0', sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "custAplId", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			strncpy(tmp_dataRequest_t.custAplId, tmp_ctrContent_t.ContentValueString, CTRL_USER_NAME_LEN);
		}
		else
		{
			LOG_debug("Error(ctrl_GetSplash): get content(custAplId) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%s\n", tmp_dataRequest_t.custAplId);

		memset(&tmp_ctrContent_t, '\0', sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "serviceAplId", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			strncpy(tmp_dataRequest_t.serviceAplId, tmp_ctrContent_t.ContentValueString, CTRL_USER_NAME_LEN);
		}
		else
		{
			LOG_debug("Error(ctrl_GetSplash): get content(serviceAplId) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%s\n", tmp_dataRequest_t.serviceAplId);

		memset(&tmp_ctrContent_t, '\0', sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "hostModel", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			strncpy(tmp_dataRequest_t.hostModel, tmp_ctrContent_t.ContentValueString, CTRL_USER_NAME_LEN);
		}
		else
		{
			LOG_debug("Error(ctrl_GetSplash): get content(hostModel) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%s\n", tmp_dataRequest_t.hostModel);

		memset(&tmp_ctrContent_t, '\0', sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "appVer", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			strncpy(tmp_dataRequest_t.appVer, tmp_ctrContent_t.ContentValueString, CTRL_USER_NAME_LEN);
		}
		else
		{
			LOG_debug("Error(ctrl_GetSplash): get content(appVer) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%s\n", tmp_dataRequest_t.appVer);

		memset(&tmp_dataResponse_t.cxDo, '\0', sizeof(tmp_dataResponse_t.cxDo));
		
		memset(&tmp_dataResponse_t.outputCtrlT, '\0', sizeof(tmp_dataResponse_t.outputCtrlT));
		memset(&tmp_dataResponse_t.ntpSrvComStat, '\0', sizeof(tmp_dataResponse_t.ntpSrvComStat));
		memset(&tmp_dataResponse_t.extPvExist, '\0', sizeof(tmp_dataResponse_t.extPvExist));
		memset(&tmp_dataResponse_t.mcErrFlag, '\0', sizeof(tmp_dataResponse_t.mcErrFlag));
		memset(&tmp_dataResponse_t.dcbSn, '\0', sizeof(tmp_dataResponse_t.dcbSn));
		memset(&tmp_dataResponse_t.fwUpd, '\0', sizeof(tmp_dataResponse_t.fwUpd));
		memset(&tmp_dataResponse_t.modelName, '\0', sizeof(tmp_dataResponse_t.modelName));
		memset(&tmp_dataResponse_t.sysSn, '\0', sizeof(tmp_dataResponse_t.sysSn));
		memset(&tmp_dataResponse_t.vehicleDcModel, '\0', sizeof(tmp_dataResponse_t.vehicleDcModel));
		memset(&tmp_dataResponse_t.vehicleDcSn, '\0', sizeof(tmp_dataResponse_t.vehicleDcSn));
		memset(&tmp_dataResponse_t.authToken, '\0', sizeof(tmp_dataResponse_t.authToken));

		tmp_ret_SC = SRV_GetSplash(&tmp_dataRequest_t, &tmp_dataResponse_t, tmp_sessionId_t);
		if (0 == tmp_ret_SC)
		{
			tmp_responseJSon_P = cJSON_CreateObject();
			if (NULL != tmp_responseJSon_P)
			{
				cJSON_AddStringToObject(tmp_responseJSon_P, "cxDo", tmp_dataResponse_t.cxDo);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "cTime", tmp_dataResponse_t.cTime);
				cJSON_AddStringToObject(tmp_responseJSon_P, "outputCtrlT", tmp_dataResponse_t.outputCtrlT);
				cJSON_AddStringToObject(tmp_responseJSon_P, "ntpSrvComStat", tmp_dataResponse_t.ntpSrvComStat);
				cJSON_AddStringToObject(tmp_responseJSon_P, "extPvExist", tmp_dataResponse_t.extPvExist);
				cJSON_AddStringToObject(tmp_responseJSon_P, "mcErrFlag", tmp_dataResponse_t.mcErrFlag);
				cJSON_AddStringToObject(tmp_responseJSon_P, "dcbSn", tmp_dataResponse_t.dcbSn);
				cJSON_AddStringToObject(tmp_responseJSon_P, "fwUpd", tmp_dataResponse_t.fwUpd);
				cJSON_AddStringToObject(tmp_responseJSon_P, "modelName", tmp_dataResponse_t.modelName);
				cJSON_AddStringToObject(tmp_responseJSon_P, "sysSn", tmp_dataResponse_t.sysSn);
				cJSON_AddStringToObject(tmp_responseJSon_P, "vehicleDcModel", tmp_dataResponse_t.vehicleDcModel);
				cJSON_AddStringToObject(tmp_responseJSon_P, "vehicleDcSn", tmp_dataResponse_t.vehicleDcSn);
				cJSON_AddStringToObject(tmp_responseJSon_P, "authToken", tmp_dataResponse_t.authToken);
				
				CTRL_setOutputJson(tmp_responseJSon_P);
			}
			else
			{
				LOG_debug("Error(ctrl_GetSplash): CJson create failed.\n");
			}
		}
		else
		{
			LOG_debug("Error(ctrl_GetSplash): Service handle failed(%d).\n", tmp_ret_SC);
		}
	}

	LOG_leave("ctrl_GetSplash");

	CTRL_writeLogWithAuthenToken(tmp_sessionId_t, "1-1", "op context");

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
/*	history			:2023/05/04 : DHC Zhang.WY : 新規作成					*/
/*==========================================================================*/
static schar ctrl_GetSplInfo(const RequestData_t *tmp_requestData_P)
{
	LOG_entry("ctrl_GetSplInfo");

	schar tmp_ret_SC = -1;
	schar url[64];
	startupComInfo tmp_dataResponse_t = {0};
	CtrlContent_t tmp_ctrContent_t = {0};
	schar *tmp_response_SCP = NULL;
	cJSON *tmp_responseJSon_P = NULL;

	if (NULL == tmp_requestData_P)
	{
		LOG_debug("Error(ctrl_GetSplInfo): Param is NULL.\n");
		return -1;
	}
	else
	{
		tmp_ret_SC = SRV_GetSplInfo(&tmp_dataResponse_t);
		if (0 == tmp_ret_SC)
		{
			tmp_responseJSon_P = cJSON_CreateObject();
			if (NULL != tmp_responseJSon_P)
			{
				cJSON_AddStringToObject(tmp_responseJSon_P, "cxDo", tmp_dataResponse_t.cxDo);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "cTime", tmp_dataResponse_t.cTime);
				cJSON_AddStringToObject(tmp_responseJSon_P, "outputCtrlT", tmp_dataResponse_t.outputCtrlT);
				cJSON_AddStringToObject(tmp_responseJSon_P, "ntpSrvComStat", tmp_dataResponse_t.ntpSrvComStat);
				cJSON_AddStringToObject(tmp_responseJSon_P, "extPvExist", tmp_dataResponse_t.extPvExist);
				cJSON_AddStringToObject(tmp_responseJSon_P, "mcErrFlag", tmp_dataResponse_t.mcErrFlag);
				cJSON_AddStringToObject(tmp_responseJSon_P, "dcbSn", tmp_dataResponse_t.dcbSn);
				cJSON_AddStringToObject(tmp_responseJSon_P, "fwUpd", tmp_dataResponse_t.fwUpd);
				cJSON_AddStringToObject(tmp_responseJSon_P, "modelName", tmp_dataResponse_t.modelName);
				cJSON_AddStringToObject(tmp_responseJSon_P, "sysSn", tmp_dataResponse_t.sysSn);
				cJSON_AddStringToObject(tmp_responseJSon_P, "vehicleDcModel", tmp_dataResponse_t.vehicleDcModel);
				cJSON_AddStringToObject(tmp_responseJSon_P, "vehicleDcSn", tmp_dataResponse_t.vehicleDcSn);

				CTRL_setOutputJson(tmp_responseJSon_P);
			}
			else
			{
				LOG_debug("Error(ctrl_GetSplInfo): CJson create failed.\n");
			}
		}
		else
		{
			LOG_debug("Error(ctrl_GetSplInfo): Service handle failed(%d).\n", tmp_ret_SC);
		}
	}
	LOG_leave("ctrl_GetSplInfo");
	UTIL_SubStrByChar(tmp_requestData_P->Path_info_SCP, '?', url, 64);
	CTRL_writeLog(url, "op context");
	return tmp_ret_SC;
}


/*End Of File*/
