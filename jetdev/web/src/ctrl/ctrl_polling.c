/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Hao.JQ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/17												*/
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
#include "ctrl_polling.h"
#include "ctrl_g.h"
#include "srv_polling_g.h"
#include "frm_dispatcher_g.h"
#include "util_logger_g.h"
#include "frm_dispatcher_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
const ServiceHandle_t DS_PollingCtrlHandle[CTRL_POLLING_HANDLE_MAX_NUM] = {
	{"get", "^(/polling/storageStatus)", ctrl_GetPolling},
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
/*	history			:2023/04/17 : DHC Hao.JQ : 新規作成						*/
/*==========================================================================*/
extern void CTRL_Polling_Handle(const RequestData_t *tmp_requestData_P)
{
	(void)CTRL_Distribute(tmp_requestData_P, DS_PollingCtrlHandle, MgetArrayMemCount_si(DS_PollingCtrlHandle));
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
/*	history			:2023/04/17 : DHC Hao.JQ : 新規作成						*/
/*==========================================================================*/
static schar ctrl_GetPolling(const RequestData_t *tmp_requestData_P)
{
	schar tmp_ret_SC = -1;
	sbSysOpStatus tmp_dataResponse_t = {0};
	//startupRequest tmp_dataRequest_t = {0};
	CtrlContent_t tmp_ctrContent_t = {0};
	schar *tmp_response_SCP = NULL;
	cJSON *tmp_responseJSon_P = NULL;

	if(NULL == tmp_requestData_P)
	{
		LOG_debug("Error(ctrl_GetPolling): Param is NULL.\n");
		return -1;
	}
	else
	{
		tmp_ret_SC = SRV_GetPolling(&tmp_dataResponse_t);
		if(0 == tmp_ret_SC)
		{
			tmp_responseJSon_P = cJSON_CreateObject();
			if(NULL != tmp_responseJSon_P)
			{
				cJSON_AddStringToObject(tmp_responseJSon_P, "e1Err", tmp_dataResponse_t.e1Err);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "writeDate", tmp_dataResponse_t.writeDate);
				cJSON_AddStringToObject(tmp_responseJSon_P, "e1ErrRtg", tmp_dataResponse_t.e1ErrRtg);
				cJSON_AddStringToObject(tmp_responseJSon_P, "poFlg", tmp_dataResponse_t.poFlg);
				cJSON_AddStringToObject(tmp_responseJSon_P, "fwUpd", tmp_dataResponse_t.fwUpd);
				cJSON_AddStringToObject(tmp_responseJSon_P, "sysSn", tmp_dataResponse_t.sysSn);
				
				CTRL_setOutputJson(tmp_responseJSon_P);
			}
			else
			{
				LOG_debug("Error(ctrl_GetPolling): CJson create failed.\n");
			}
		}
		else
		{
			LOG_debug("Error(ctrl_GetPolling): Service handle failed(%d).\n", tmp_ret_SC);
		}

	}
	return tmp_ret_SC;
}

/*End Of File*/
