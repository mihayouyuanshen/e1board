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

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "common_g.h"
#include "ref_cjson_g.h"
#include "ctrl_modeset.h"
#include "ctrl_g.h"
#include "srv_modeset_g.h"
#include "frm_dispatcher_g.h"
#include "util_logger_g.h"
#include "frm_dispatcher_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
const ServiceHandle_t DS_ModesetCtrlHandle[CTRL_MODESET_HANDLE_MAX_NUM] = {
	{"get", "^(/modeSet)", ctrl_GetModeset},
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
/*	history			:2023/04/20 : DHC Hao.JQ : 新規作成						*/
/*==========================================================================*/
extern void CTRL_Modeset_Handle(const RequestData_t *tmp_requestData_P)
{
	(void)CTRL_Distribute(tmp_requestData_P, DS_ModesetCtrlHandle, MgetArrayMemCount_si(DS_ModesetCtrlHandle));
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
/*	history			:2023/04/20 : DHC Hao.JQ : 新規作成						*/
/*==========================================================================*/
static schar ctrl_GetModeset(const RequestData_t *tmp_requestData_P)
{
	schar tmp_ret_SC = -1;
	modeSet tmp_dataResponse_t = {0};
	//startupRequest tmp_dataRequest_t = {0};
	CtrlContent_t tmp_ctrContent_t = {0};
	schar *tmp_response_SCP = NULL;
	cJSON *tmp_responseJSon_P = NULL;

	if(NULL == tmp_requestData_P)
	{
		LOG_debug("Error(ctrl_GetModeset): Param is NULL.\n");
		return -1;
	}
	else
	{
		tmp_ret_SC = SRV_GetModeset(&tmp_dataResponse_t);
		if(0 == tmp_ret_SC)
		{
			tmp_responseJSon_P = cJSON_CreateObject();
			if(NULL != tmp_responseJSon_P)
			{
				cJSON_AddStringToObject(tmp_responseJSon_P, "opMode", tmp_dataResponse_t.opMode);
				cJSON_AddStringToObject(tmp_responseJSon_P, "emerMode", tmp_dataResponse_t.emerMode);
				cJSON_AddStringToObject(tmp_responseJSon_P, "salesChgIsb", tmp_dataResponse_t.salesChgIsb);
				cJSON_AddStringToObject(tmp_responseJSon_P, "salesChgToc", tmp_dataResponse_t.salesChgToc);
				cJSON_AddStringToObject(tmp_responseJSon_P, "salesDcIsb", tmp_dataResponse_t.salesDcIsb);
				cJSON_AddStringToObject(tmp_responseJSon_P, "salesDcToc", tmp_dataResponse_t.salesDcToc);
				cJSON_AddStringToObject(tmp_responseJSon_P, "grnChgIsb", tmp_dataResponse_t.grnChgIsb);
				cJSON_AddStringToObject(tmp_responseJSon_P, "grnChgToc", tmp_dataResponse_t.grnChgToc);
				cJSON_AddStringToObject(tmp_responseJSon_P, "grnDischgIsb", tmp_dataResponse_t.grnDischgIsb);
				cJSON_AddStringToObject(tmp_responseJSon_P, "grnDischgToc", tmp_dataResponse_t.grnDischgToc);
				cJSON_AddStringToObject(tmp_responseJSon_P, "poFlg", tmp_dataResponse_t.poFlg);
				cJSON_AddStringToObject(tmp_responseJSon_P, "sysConfig", tmp_dataResponse_t.sysConfig);
				cJSON_AddStringToObject(tmp_responseJSon_P, "runStop", tmp_dataResponse_t.runStop);
				cJSON_AddStringToObject(tmp_responseJSon_P, "connLockSt", tmp_dataResponse_t.connLockSt);
				CTRL_setOutputJson(tmp_responseJSon_P);
			}
			else
			{
				LOG_debug("Error(ctrl_GetModeset): CJson create failed.\n");
			}
		}
		else
		{
			LOG_debug("Error(ctrl_GetModeset): Service handle failed(%d).\n", tmp_ret_SC);
		}

	}
	return tmp_ret_SC;
}

/*End Of File*/
