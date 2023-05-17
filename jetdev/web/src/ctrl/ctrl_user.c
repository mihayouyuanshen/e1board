/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Sun.YF												*/
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
#include "common_g.h"
#include "ref_cjson_g.h"
#include "ctrl_user.h"
#include "ctrl_g.h"
#include "srv_user_g.h"
#include "frm_dispatcher_g.h"
#include "util_logger_g.h"
#include "frm_dispatcher_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
const ServiceHandle_t DS_UserCtrlHandle[CTRL_USER_HANDLE_MAX_NUM] = {
	{"get", "^(/usr/option/)([0-9a-zA-Z]{0,})", ctrl_User_GetSingle},
	{"get", "^(/usr/list/)([0-9a-zA-Z]{0,})", ctrl_User_GetList},
	{"post", "^(/usr/option/)([0-9a-zA-Z]{0,})", ctrl_User_ModifySingle}
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
/*	history 		:2023/03/08 : DHC Sun.YF : 新規作成						*/
/*==========================================================================*/
extern void CTRL_User_Handle(const RequestData_t *tmp_requestData_P)
{
	(void)CTRL_Distribute(tmp_requestData_P, DS_UserCtrlHandle, MgetArrayMemCount_si(DS_UserCtrlHandle));
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
/*	history 		:2023/03/08 : DHC Sun.YF : 新規作成						*/
/*==========================================================================*/
schar ctrl_User_GetSingle(const RequestData_t *tmp_requestData_P)
{
	schar tmp_ret_SC = -1;
	UserDataInfo_t tmp_dataRequset_t = {0};
	UserDataInfo_t tmp_dataResponse_t = {0};
	CtrlContent_t tmp_ctrContent_t = {0};
	schar *tmp_response_SCP = NULL;
	cJSON *tmp_responseJSon_P = NULL;

	if(NULL == tmp_requestData_P)
	{
		LOG_debug("Error(ctrl_User_GetSingle): Param is NULL.\n");
		return -1;
	}
	else
	{
		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "name", &tmp_ctrContent_t);
		if(0 == tmp_ret_SC)
		{
			strncpy(tmp_dataRequset_t.Name_SC, tmp_ctrContent_t.ContentValueString, CTRL_USER_NAME_LEN);
		}
		else
		{
			LOG_debug("Error(ctrl_User_GetSingle): get content(name) failed(%d).\n", tmp_ret_SC);
			return -1;
		}

		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_INT;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "age", &tmp_ctrContent_t);
		if(0 == tmp_ret_SC)
		{
			tmp_dataRequset_t.Age_UI = (uint)tmp_ctrContent_t.ContentValueInt_SI;
		}
		else
		{
			LOG_debug("Error(ctrl_User_GetSingle): get content(age) failed(%d).\n", tmp_ret_SC);
			return -1;
		}

		tmp_ret_SC = SRV_USER_GetSingle(&tmp_dataRequset_t, &tmp_dataResponse_t);
		if(0 == tmp_ret_SC)
		{
			tmp_responseJSon_P = cJSON_CreateObject();
			if(NULL != tmp_responseJSon_P)
			{
				cJSON_AddStringToObject(tmp_responseJSon_P, "name", tmp_dataResponse_t.Name_SC);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "age", tmp_dataResponse_t.Age_UI);
				cJSON_AddStringToObject(tmp_responseJSon_P, "role", tmp_dataResponse_t.Role_SC);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "grade", tmp_dataResponse_t.Grade_UI);
				tmp_response_SCP = cJSON_Print(tmp_responseJSon_P);
				if(NULL != tmp_response_SCP)
				{
					LOG_debug("%s\n", tmp_response_SCP);
				}
				else
				{
					LOG_debug("Error(ctrl_User_GetSingle): CJson print failed.\n");
				}
				cJSON_Delete(tmp_responseJSon_P);
			}
			else
			{
				LOG_debug("Error(ctrl_User_GetSingle): CJson create failed.\n");
			}
		}
		else
		{
			LOG_debug("Error(ctrl_User_GetSingle): Service handle failed(%d).\n", tmp_ret_SC);
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
/*	history 		:2023/03/08 : DHC Sun.YF : 新規作成						*/
/*==========================================================================*/
schar ctrl_User_DealwithResponseList(const UserDataInfoList_t *tmp_responseList_P)
{
	schar tmp_ret_SC = 0;
	schar tmp_loop_SC = 0;
	schar* tmp_response_SCP = NULL;
	cJSON *tmp_responseJSon_P = NULL;
	cJSON *tmp_singleJSon_P = NULL;
	const schar* tmp_userDataName[CTRL_USER_RESPONSE_LIST_MAX_NUM] = {
		"user01","user02","user03","user04","user05",
		"user06","user07","user08","user09","user10",
		};

	if(NULL == tmp_responseList_P)
	{
		LOG_debug("Error(ctrl_User_DealwithResponseList): Param is NULL.\n");
		return -1;
	}
	else
	{
		tmp_responseJSon_P = cJSON_CreateObject();
		if(NULL != tmp_responseJSon_P)
		{
			while((tmp_loop_SC < tmp_responseList_P->ListCurrentNum) && (0 == tmp_ret_SC))
			{
				tmp_singleJSon_P = cJSON_CreateObject();
				if(NULL != tmp_singleJSon_P)
				{
					cJSON_AddStringToObject(tmp_singleJSon_P, "name",
											tmp_responseList_P->DataList[tmp_loop_SC].Name_SC);
					cJSON_AddNumberToObject(tmp_singleJSon_P, "age",
											tmp_responseList_P->DataList[tmp_loop_SC].Age_UI);
					cJSON_AddStringToObject(tmp_singleJSon_P, "role",
											tmp_responseList_P->DataList[tmp_loop_SC].Role_SC);
					cJSON_AddNumberToObject(tmp_singleJSon_P, "grade",
											tmp_responseList_P->DataList[tmp_loop_SC].Grade_UI);
					cJSON_AddItemToObject(tmp_responseJSon_P, tmp_userDataName[tmp_loop_SC], tmp_singleJSon_P);
					tmp_loop_SC++;
				}
				else
				{
					LOG_debug("Error(ctrl_User_DealwithResponseList): CJson single failed(%d).\n", tmp_loop_SC);
					tmp_ret_SC = -1;
				}
			}

			if(0 == tmp_ret_SC)
			{
				tmp_response_SCP = cJSON_Print(tmp_responseJSon_P);
				if(NULL != tmp_response_SCP)
				{
					LOG_debug("%s\n", tmp_response_SCP);
					tmp_ret_SC = 0;
				}
				else
				{
					LOG_debug("Error(ctrl_User_DealwithResponseList): CJson print failed.\n");
					tmp_ret_SC = -1;
				}
			}
			else
			{
				LOG_debug("Error(ctrl_User_DealwithResponseList): CJson list failed.\n");
				tmp_ret_SC = -1;
			}
			cJSON_Delete(tmp_responseJSon_P);
		}
		else
		{
			LOG_debug("Error(ctrl_User_DealwithResponseList): CJson list create failed.\n");
			tmp_ret_SC = -1;
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
/*	history 		:2023/03/08 : DHC Sun.YF : 新規作成						*/
/*==========================================================================*/
schar ctrl_User_GetList(const RequestData_t *tmp_requestData_P)
{
	schar tmp_ret_SC = 0;
	UserDataListQuery_t tmp_listQuery_t = {0};
	UserDataInfoList_t tmp_dataListResponse_t = {0};
	CtrlContent_t tmp_ctrContent_t = {0};

	if(NULL == tmp_requestData_P)
	{
		LOG_debug("Error(ctrl_User_GetList): Param is NULL.\n");
		return -1;
	}
	else
	{
		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_INT;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "age", &tmp_ctrContent_t);
		if(0 == tmp_ret_SC)
		{
			tmp_listQuery_t.AgeMax_UI = (uint)tmp_ctrContent_t.ContentValueInt_SI;
		}
		else
		{
			LOG_debug("Error(ctrl_User_GetList): get content(age) failed(%d).\n", tmp_ret_SC);
			return -1;
		}

		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_INT;
		tmp_ret_SC = CTRL_GetContentByFormdata(tmp_requestData_P->Data_SCP, "age", &tmp_ctrContent_t);
		if(0 == tmp_ret_SC)
		{
			tmp_listQuery_t.AgeMin_UI = (uint)tmp_ctrContent_t.ContentValueInt_SI;
		}
		else
		{
			LOG_debug("Error(ctrl_User_GetList): get content(age) failed(%d).\n", tmp_ret_SC);
			return -1;
		}

		tmp_dataListResponse_t.ListMaxNum = CTRL_USER_RESPONSE_LIST_MAX_NUM;
		tmp_dataListResponse_t.ListCurrentNum = 0;
		tmp_ret_SC = SRV_USER_GetList(&tmp_listQuery_t, &tmp_dataListResponse_t);
		if(0 == tmp_ret_SC)
		{
			tmp_ret_SC = ctrl_User_DealwithResponseList(&tmp_dataListResponse_t);
		}
		else
		{
			LOG_debug("Error(ctrl_User_GetList): Service handle failed(%d).\n", tmp_ret_SC);
			tmp_ret_SC = -1;
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
/*	history 		:2023/03/08 : DHC Sun.YF : 新規作成						*/
/*==========================================================================*/
schar ctrl_User_ModifySingle(const RequestData_t *tmp_requestData_P)
{
	schar tmp_ret_SC = -1;
	UserDataInfo_t tmp_dataRequset_t = {0};
	UserDataInfo_t tmp_dataResponse_t = {0};
	CtrlContent_t tmp_ctrContent_t = {0};
	schar *tmp_response_SCP = NULL;
	cJSON *tmp_responseJSon_P = NULL;

	if(NULL == tmp_requestData_P)
	{
		LOG_debug("Error(ctrl_User_ModifySingle): Param is NULL.\n");
		return -1;
	}
	else
	{
		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentStringMaxLen_SC = CTRL_USER_NAME_LEN;
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
		tmp_ret_SC = CTRL_GetContentByJsondata(tmp_requestData_P->Data_SCP, "name", &tmp_ctrContent_t);
		if(0 == tmp_ret_SC)
		{
			strncpy(tmp_dataRequset_t.Name_SC, tmp_ctrContent_t.ContentValueString, CTRL_USER_NAME_LEN);
		}
		else
		{
			LOG_debug("Error(ctrl_User_ModifySingle): get content(name) failed(%d).\n", tmp_ret_SC);
			return -1;
		}

		memset(&tmp_ctrContent_t, 0, sizeof(CtrlContent_t));
		tmp_ctrContent_t.ContentType_SC = CTRL_CONTENT_TYPE_INT;
		tmp_ret_SC = CTRL_GetContentByJsondata(tmp_requestData_P->Data_SCP, "age", &tmp_ctrContent_t);
		if(0 == tmp_ret_SC)
		{
			tmp_dataRequset_t.Age_UI = (uint)tmp_ctrContent_t.ContentValueInt_SI;
		}
		else
		{
			LOG_debug("Error(ctrl_User_ModifySingle): get content(age) failed(%d).\n", tmp_ret_SC);
			return -1;
		}

		tmp_ret_SC = SRV_USER_ModifySingle(&tmp_dataRequset_t, &tmp_dataResponse_t);
		if(0 == tmp_ret_SC)
		{
			tmp_responseJSon_P = cJSON_CreateObject();
			if(NULL != tmp_responseJSon_P)
			{
				cJSON_AddStringToObject(tmp_responseJSon_P, "name", tmp_dataResponse_t.Name_SC);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "age", tmp_dataResponse_t.Age_UI);
				cJSON_AddStringToObject(tmp_responseJSon_P, "role", tmp_dataResponse_t.Role_SC);
				cJSON_AddNumberToObject(tmp_responseJSon_P, "grade", tmp_dataResponse_t.Grade_UI);
				tmp_response_SCP = cJSON_Print(tmp_responseJSon_P);
				if(NULL != tmp_response_SCP)
				{
					LOG_debug("%s\n", tmp_response_SCP);
				}
				else
				{
					LOG_debug("Error(ctrl_User_ModifySingle): CJson print failed.\n");
				}
				cJSON_Delete(tmp_responseJSon_P);
			}
			else
			{
				LOG_debug("Error(ctrl_User_ModifySingle): CJson create failed.\n");
			}
		}
		else
		{
			LOG_debug("Error(ctrl_User_ModifySingle): Service handle failed(%d).\n", tmp_ret_SC);
		}

	}
	return tmp_ret_SC;
}

/*End Of File*/
