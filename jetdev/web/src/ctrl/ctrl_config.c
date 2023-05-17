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
#include "ctrl_config.h"
#include "ctrl_g.h"
#include "util_g.h"
#include "srv_config_g.h"
#include "frm_dispatcher_g.h"
#include "util_logger_g.h"
#include "frm_dispatcher_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
const ServiceHandle_t DS_ConfigCtrlHandle[CTRL_CONFIG_HANDLE_MAX_NUM] = {
	{"get", "^(/config)", ctrl_GetConfig}
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
extern void CTRL_Config_Handle(const RequestData_t *tmp_requestData_P)
{
	(void)CTRL_Distribute(tmp_requestData_P, DS_ConfigCtrlHandle, MgetArrayMemCount_si(DS_ConfigCtrlHandle));
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
/*	history			:2023/04/27 : DHC Zhang.WY : 新規作成					*/
/*==========================================================================*/
static void ctrl_AddNumberToArray(cJSON * const object, const char * const name, const uint * array)
{
    cJSON* arrayJson = cJSON_CreateArray();
	arrayJson = cJSON_AddArrayToObject(object, name);	
	for(int i = 0; i < SETTING_TABLE_LEN; i++)
	{
		cJSON_AddNumberToObject(arrayJson, "", array[i]);
	}
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
static schar ctrl_GetConfig(const RequestData_t *tmp_requestData_P)
{
	LOG_entry("ctrl_GetConfig");

	schar tmp_ret_SC = -1;
	schar pictureId[32];
	schar url[64];
	configSetInfo tmp_dataResponse_t = {0};
	CtrlContent_t tmp_ctrContent_t = {0};
	schar *tmp_response_SCP = NULL;
	cJSON *tmp_responseJSon_P = NULL;
	cJSON *tmp_array = NULL;

	if(NULL == tmp_requestData_P)
	{
		LOG_debug("Error(ctrl_GetConfig): Param is NULL.\n");
		return -1;
	}
	else
	{
		tmp_ret_SC = SRV_GetConfig(&tmp_dataResponse_t);
		if(0 == tmp_ret_SC)
		{
			tmp_responseJSon_P = cJSON_CreateObject();
			if(NULL != tmp_responseJSon_P)
			{
				cJSON_AddNumberToObject(tmp_responseJSon_P, "setDate", tmp_dataResponse_t.setDate);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "contractVal", tmp_dataResponse_t.contractVal);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "pvPanel", tmp_dataResponse_t.pvPanel);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "eqCapacity", tmp_dataResponse_t.eqCapacity);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "rpgUl", tmp_dataResponse_t.rpgUl);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "outputChgRate", tmp_dataResponse_t.outputChgRate);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "pcsKwh", tmp_dataResponse_t.pcsKwh);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "sysFrequency", tmp_dataResponse_t.sysFrequency);

				cJSON_AddStringToObject(tmp_responseJSon_P, "sysConfig", tmp_dataResponse_t.sysConfig);
				cJSON_AddStringToObject(tmp_responseJSon_P, "extPvExist", tmp_dataResponse_t.extPvExist);
				cJSON_AddStringToObject(tmp_responseJSon_P, "othDevice", tmp_dataResponse_t.othDevice);
				cJSON_AddStringToObject(tmp_responseJSon_P, "clipAction", tmp_dataResponse_t.clipAction);
				cJSON_AddStringToObject(tmp_responseJSon_P, "fitType", tmp_dataResponse_t.fitType);
				cJSON_AddStringToObject(tmp_responseJSon_P, "selfUseTyp", tmp_dataResponse_t.selfUseTyp);
				cJSON_AddStringToObject(tmp_responseJSon_P, "salesValid", tmp_dataResponse_t.salesValid);
				cJSON_AddStringToObject(tmp_responseJSon_P, "outputCtrlT", tmp_dataResponse_t.outputCtrlT);
				cJSON_AddStringToObject(tmp_responseJSon_P, "powStation", tmp_dataResponse_t.powStation);
				cJSON_AddStringToObject(tmp_responseJSon_P, "electricSrv", tmp_dataResponse_t.electricSrv);
				cJSON_AddStringToObject(tmp_responseJSon_P, "pushUp", tmp_dataResponse_t.pushUp);
				cJSON_AddStringToObject(tmp_responseJSon_P, "orcExist", tmp_dataResponse_t.orcExist);
				cJSON_AddStringToObject(tmp_responseJSon_P, "loadConnType", tmp_dataResponse_t.loadConnType);

				ctrl_AddNumberToArray(tmp_responseJSon_P, "setValueTbl", tmp_dataResponse_t.setValueTbl);

				ctrl_AddNumberToArray(tmp_responseJSon_P, "sysVpkLvlList", tmp_dataResponse_t.sysVpkLvlList);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "sysVpkLvl", tmp_dataResponse_t.sysVpkLvl);
				ctrl_AddNumberToArray(tmp_responseJSon_P, "sysVpkTimeList", tmp_dataResponse_t.sysVpkTimeList);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "sysVpkTime", tmp_dataResponse_t.sysVpkTime);
				ctrl_AddNumberToArray(tmp_responseJSon_P, "sysTrchLvlList", tmp_dataResponse_t.sysTrchLvlList);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "sysTrchLvl", tmp_dataResponse_t.sysTrchLvl);
				ctrl_AddNumberToArray(tmp_responseJSon_P, "sysTrchTimeList", tmp_dataResponse_t.sysTrchTimeList);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "sysTrchTime", tmp_dataResponse_t.sysTrchTime);
				ctrl_AddNumberToArray(tmp_responseJSon_P, "freqRiseLvlList", tmp_dataResponse_t.freqRiseLvlList);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "freqRiseLvl", tmp_dataResponse_t.freqRiseLvl);
				ctrl_AddNumberToArray(tmp_responseJSon_P, "freqRiseTimeList", tmp_dataResponse_t.freqRiseTimeList);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "freqRiseTime", tmp_dataResponse_t.freqRiseTime);
				ctrl_AddNumberToArray(tmp_responseJSon_P, "freqDropLvlList", tmp_dataResponse_t.freqDropLvlList);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "freqDropLvl", tmp_dataResponse_t.freqDropLvl);
				ctrl_AddNumberToArray(tmp_responseJSon_P, "freqDropTimeList", tmp_dataResponse_t.freqDropTimeList);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "freqDropTime", tmp_dataResponse_t.freqDropTime);
				ctrl_AddNumberToArray(tmp_responseJSon_P, "rtnTimeList", tmp_dataResponse_t.rtnTimeList);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "rtnTime", tmp_dataResponse_t.rtnTime);
				ctrl_AddNumberToArray(tmp_responseJSon_P, "restraintValList", tmp_dataResponse_t.restraintValList);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "restraintVal", tmp_dataResponse_t.restraintVal);
				ctrl_AddNumberToArray(tmp_responseJSon_P, "restraintStList", tmp_dataResponse_t.restraintStList);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "restraintSt", tmp_dataResponse_t.restraintSt);
				ctrl_AddNumberToArray(tmp_responseJSon_P, "powFactorCtrlList", tmp_dataResponse_t.powFactorCtrlList);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "powFactorCtrl", tmp_dataResponse_t.powFactorCtrl);
				ctrl_AddNumberToArray(tmp_responseJSon_P, "singleOpLvlList", tmp_dataResponse_t.singleOpLvlList);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "singleOpLvl", tmp_dataResponse_t.singleOpLvl);
				ctrl_AddNumberToArray(tmp_responseJSon_P, "freqFbGainList", tmp_dataResponse_t.freqFbGainList);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "freqFbGain", tmp_dataResponse_t.freqFbGain);
				ctrl_AddNumberToArray(tmp_responseJSon_P, "allowFreqLvlList", tmp_dataResponse_t.allowFreqLvlList);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "allowFreqLvl", tmp_dataResponse_t.allowFreqLvl);

				CTRL_setOutputJson(tmp_responseJSon_P);
			}
			else
			{
				LOG_debug("Error(ctrl_GetConfig): CJson create failed.\n");
			}
		}
		else
		{
			LOG_debug("Error(ctrl_GetConfig): Service handle failed(%d).\n", tmp_ret_SC);
		}

	}
	LOG_leave("ctrl_GetConfig");
	UTIL_SubStrByChar(tmp_requestData_P->Path_info_SCP, '?', url, 64);
	memset(pictureId, '\0', 32);
	// CTRL_writeLog(url, pictureId, "op context");
	return tmp_ret_SC;
}

/*End Of File*/
