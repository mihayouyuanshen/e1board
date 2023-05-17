/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Hao.JQ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/19												*/
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
#include "ctrl_home.h"
#include "ctrl_g.h"
#include "util_g.h"
#include "srv_home_g.h"
#include "frm_dispatcher_g.h"
#include "util_logger_g.h"
#include "frm_dispatcher_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
const ServiceHandle_t DS_HomeCtrlHandle[CTRL_HOME_HANDLE_MAX_NUM] = {
	{"get", "^(/home)", ctrl_GetHome}
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
extern void CTRL_Home_Handle(const RequestData_t *tmp_requestData_P)
{
	(void)CTRL_Distribute(tmp_requestData_P, DS_HomeCtrlHandle, MgetArrayMemCount_si(DS_HomeCtrlHandle));
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
static schar ctrl_GetHome(const RequestData_t *tmp_requestData_P)
{
	LOG_entry("ctrl_GetHome");

	schar tmp_ret_SC = -1;
	schar url[64];
	homeInfo tmp_dataResponse_t = {0};
	homeRequest tmp_dataRequest_t = {0};
	CtrlContent_t tmp_ctrContent_t = {0};
	schar *tmp_response_SCP = NULL;
	cJSON *tmp_responseJSon_P = NULL;
	cJSON *cxFile = NULL;

	if (NULL == tmp_requestData_P)
	{
		LOG_debug("Error(ctrl_GetHome): Param is NULL.\n");
		return -1;
	}
	else
	{
		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_INT;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "timeStamp", &tmp_ctrContent_t);
		if (0 == tmp_ret_SC)
		{
			tmp_dataRequest_t.timeStamp = tmp_ctrContent_t.ContentValueInt_SI;
		}
		else
		{
			LOG_debug("Error(ctrl_GetHome: get content(timeStamp) failed(%d).\n", tmp_ret_SC);
			return -1;
		}
		LOG_debug("%d\n", tmp_dataRequest_t.timeStamp);
		
		tmp_ret_SC = SRV_GetHome(&tmp_dataRequest_t, &tmp_dataResponse_t);
		if (0 == tmp_ret_SC)
		{
			tmp_responseJSon_P = cJSON_CreateObject();
			cxFile = cJSON_CreateObject();
			cxFile = cJSON_AddObjectToObject(tmp_responseJSon_P, "cxFile");
			if (NULL != tmp_responseJSon_P)
			{
				cJSON_AddStringToObject(tmp_responseJSon_P, "sysConfig", tmp_dataResponse_t.sysConfig);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "outputCtrlPct", tmp_dataResponse_t.outputCtrlPct);
				cJSON_AddStringToObject(tmp_responseJSon_P, "opMode", tmp_dataResponse_t.opMode);
				cJSON_AddStringToObject(tmp_responseJSon_P, "emerMode", tmp_dataResponse_t.emerMode);
				cJSON_AddStringToObject(tmp_responseJSon_P, "opSetReq", tmp_dataResponse_t.opSetReq);
				cJSON_AddStringToObject(tmp_responseJSon_P, "outputCtrlSt", tmp_dataResponse_t.outputCtrlSt);
				cJSON_AddStringToObject(tmp_responseJSon_P, "connLockSt", tmp_dataResponse_t.connLockSt);
				cJSON_AddStringToObject(tmp_responseJSon_P, "desigPowOpFlg", tmp_dataResponse_t.desigPowOpFlg);
				cJSON_AddStringToObject(tmp_responseJSon_P, "runStop", tmp_dataResponse_t.runStop);
				cJSON_AddStringToObject(tmp_responseJSon_P, "noticeAddFlg", tmp_dataResponse_t.noticeAddFlg);
				cJSON_AddStringToObject(tmp_responseJSon_P, "fwUpdNo", tmp_dataResponse_t.fwUpdNo);
				cJSON_AddStringToObject(tmp_responseJSon_P, "fwUpdMode", tmp_dataResponse_t.fwUpdMode);
				cJSON_AddStringToObject(tmp_responseJSon_P, "sbOpSt", tmp_dataResponse_t.sbOpSt);

				cJSON_AddNumberToObject(cxFile, "batteryCharging", tmp_dataResponse_t.testFile.batteryCharging);
				cJSON_AddNumberToObject(cxFile, "batteryDischarge", tmp_dataResponse_t.testFile.batteryDischarge);
				cJSON_AddNumberToObject(cxFile, "buyElectricityC2", tmp_dataResponse_t.testFile.buyElectricityC2);
				cJSON_AddNumberToObject(cxFile, "buyElectricityE1", tmp_dataResponse_t.testFile.buyElectricityE1);
				cJSON_AddNumberToObject(cxFile, "V2hCharging", tmp_dataResponse_t.testFile.V2hCharging);
				cJSON_AddNumberToObject(cxFile, "V2hDischarge", tmp_dataResponse_t.testFile.V2hDischarge);
				cJSON_AddNumberToObject(cxFile, "sunlightPvGen", tmp_dataResponse_t.testFile.sunlightPvGen);
				cJSON_AddNumberToObject(cxFile, "extSunlightPv", tmp_dataResponse_t.testFile.extSunlightPv);
				cJSON_AddNumberToObject(cxFile, "selfRelOperSwitch", tmp_dataResponse_t.testFile.selfRelOperSwitch);
				cJSON_AddNumberToObject(cxFile, "pvOutDischarge", tmp_dataResponse_t.testFile.pvOutDischarge);
				cJSON_AddNumberToObject(cxFile, "pvOutRet", tmp_dataResponse_t.testFile.pvOutRet);

				cJSON_AddStringToObject(tmp_responseJSon_P, "extPvExist", tmp_dataResponse_t.extPvExist);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "pvAvgPow", tmp_dataResponse_t.pvAvgPow);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "rcAvgPow", tmp_dataResponse_t.rcAvgPow);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "vehicleCdtAvgPow", tmp_dataResponse_t.vehicleCdtAvgPow);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "sbCdtAvgPow", tmp_dataResponse_t.sbCdtAvgPow);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "extPvAvgPow", tmp_dataResponse_t.extPvAvgPow);
				cJSON_AddStringToObject(tmp_responseJSon_P, "purPowDfv", tmp_dataResponse_t.purPowDfv);
				cJSON_AddStringToObject(tmp_responseJSon_P, "vehicleDcSt", tmp_dataResponse_t.vehicleDcSt);
				cJSON_AddStringToObject(tmp_responseJSon_P, "mainteStatus", tmp_dataResponse_t.mainteStatus);
				cJSON_AddStringToObject(tmp_responseJSon_P, "poFlg", tmp_dataResponse_t.poFlg);
				cJSON_AddStringToObject(tmp_responseJSon_P, "mainteOperation", tmp_dataResponse_t.mainteOperation);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "evBattLvl3", tmp_dataResponse_t.evBattLvl3);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "socFlg", tmp_dataResponse_t.socFlg);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "mainteSchedule", tmp_dataResponse_t.mainteSchedule);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "cTime", tmp_dataResponse_t.cTime);
				
				CTRL_setOutputJson(tmp_responseJSon_P);
			}
			else
			{
				LOG_debug("Error(ctrl_GetHome): CJson create failed.\n");
			}
		}
		else
		{
			LOG_debug("Error(ctrl_GetHome): Service handle failed(%d).\n", tmp_ret_SC);
		}
	}
	LOG_leave("ctrl_GetHome");

	// UTIL_SubStrByChar(tmp_requestData_P->Path_info_SCP, '?', url, 64);
	CTRL_writeLog(url, "op context");
	return tmp_ret_SC;
}

/*End Of File*/
