/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Zhang.WY											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/08												*/
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
#include <sys/time.h>

#include "common_g.h"
#include "ref_cjson_g.h"
#include "ctrl_user.h"
#include "ctrl_g.h"
#include "util_g.h"
#include "srv_user_g.h"
#include "frm_dispatcher_g.h"
#include "util_logger_g.h"
#include "frm_dispatcher_g.h"

#include "ctrl_serviemenu.h"
#include "srv/srv_servicemenu.h"
#include "stub_g.h"
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
const ServiceHandle_t DS_ServiceMenuCtrlHandleRegister[1] = {
	{"get", "/servicesMenu/outputCtrl", ctrl_Servicemenu_outputCtrl}
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
/*	history 		:2023/03/08 : DHC Zhang.WY : 新規作成					*/
/*==========================================================================*/
extern void CTRL_Servicemenu_Handle(const RequestData_t *tmp_requestData_P)
{
	(void)CTRL_Distribute(tmp_requestData_P, DS_ServiceMenuCtrlHandleRegister, MgetArrayMemCount_si(DS_ServiceMenuCtrlHandleRegister));
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
/*	history 		:2023/03/08 : DHC Zhang.WY : 新規作成					*/
/*==========================================================================*/
static schar ctrl_Servicemenu_outputCtrl(const RequestData_t *tmp_requestData_P)
{
	LOG_entry("ctrl_Servicemenu_outputCtrl");
	schar pictureId[32];
	schar url[64];
	schar tmp_ret_SC = RET_SUCCESS_CODE;
	char outputFlag[4];
	schar service_ret = WEB_GetOutputFlag(outputFlag);

	if (RET_SUCCESS_CODE == service_ret)
	{
		cJSON *tmp_responseJSon_P = cJSON_CreateObject();
		if (NULL != tmp_responseJSon_P)
		{
			cJSON_AddStringToObject(tmp_responseJSon_P, "outputCtrlT", outputFlag);
			CTRL_setOutputJson(tmp_responseJSon_P);
		}
	}
	else
	{
		/* TODO return the error response */
	}

	LOG_leave("ctrl_Servicemenu_outputCtrl");

	UTIL_SubStrByChar(tmp_requestData_P->Path_info_SCP, '?', url, 64);
	memset(pictureId, '\0', 32);
	// CTRL_writeLog(url, pictureId, "op context");

	return tmp_ret_SC;
}

/*End Of File*/
