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
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "ref_cjson_g.h"
#include "common_g.h"
#include "util_logger_g.h"
#include "frm_dispatcher_g.h"
#include "ctrl_g.h"
#include "frm_cgiapi_g.h"
#include "session_g.h"

#include "ctrl_user.h"
#include "ctrl_splash.h"
#include "ctrl_polling.h"
#include "ctrl_home.h"
#include "ctrl_modeset.h"
#include "ctrl_log.h"
#include "ctrl_list.h"
#include "ctrl_timeset.h"
#include "ctrl_errclr.h"
#include "ctrl_extct.h"
#include "ctrl_switch.h"
#include "ctrl_config.h"
#include "frm_cgiapi_g.h"
#include "ctrl_serviemenu.h"
#include "stub_g.h"

#define FIRST_LEVEL_SPLITOR "&"
#define SECOND_LEVEL_SPLITOR "="

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
typedef struct
{
	schar url[REQUSET_URL_MAX_SIZE];
	schar pictureId[32];
} NamePicIdMapping;

const NamePicIdMapping namePicIdMappings[17] = {
	{"/servicesMenu/outputCtrl", "26-1"},
	{"/start/splash", "1-1"},
	{"/start/splashInfo", "1-1"},
	{"/polling/storageStatus", "public"},
	{"/home", "6-1"},
	{"/modeSet", "7-1"},
	{"/history_search", "8-2"},
	{"/tradeHist", "8-2"},
	{"/records", "8-2"},
	{"/vehicleRecords", "8-2"},
	{"/home/powerFlow", "6-1"},
	{"/notice", "25-1"},
	{"/config", "t2"},
	{"/timeSet", "105-2"},
	{"/errorDisp/errClear", "101-1"},
	{"/extCTPosConfirmIns", "t2-2"},
	{"/switchModuleCTUse", "t2-3"}
};

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
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
void CTRL_Initialize(void)
{
	LOG_debug("CTRL_Initialize.\n");
	FRM_DispAddController("usr", CTRL_User_Handle);
	FRM_DispAddController("start", CTRL_Start_Handle);
	FRM_DispAddController("polling", CTRL_Polling_Handle);
	FRM_DispAddController("home", CTRL_Home_Handle);
	FRM_DispAddController("modeSet", CTRL_Modeset_Handle);
	FRM_DispAddController("history_search", CTRL_Log_Handle);
	FRM_DispAddController("tradeHist", CTRL_Log_Handle);
	FRM_DispAddController("records", CTRL_Log_Handle);
	FRM_DispAddController("vehicleRecords", CTRL_Log_Handle);
	FRM_DispAddController("servicesMenu", CTRL_Servicemenu_Handle);
	FRM_DispAddController("notice", CTRL_Notice_Handle);
	FRM_DispAddController("timeSet", CTRL_Timeset_Handle);
	FRM_DispAddController("errorDisp", CTRL_Errclr_Handle);
	FRM_DispAddController("extCTPosConfirmIns", CTRL_Extct_Handle);
	FRM_DispAddController("switchModuleCTUse", CTRL_Switch_Handle);
	FRM_DispAddController("config", CTRL_Config_Handle);
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
schar CTRL_Distribute(const RequestData_t *tmp_requestData_P,
					  const ServiceHandle_t *tmp_serviceHandle_P,
					  sint tmp_handleNum_SI)
{
	schar tmp_loop_SC = 0;
	schar tmp_find_SC = 0;
	schar tmp_ret_SC = 0;
	schar tmp_cmp_SC = 0;
	schar tmp_match_SC = 0;

	if ((NULL == tmp_requestData_P) || (NULL == tmp_serviceHandle_P))
	{
		LOG_debug("Error(CTRL_Distribute): Param is NULL.\n");
		return -1;
	}
	else
	{
		while ((tmp_loop_SC < tmp_handleNum_SI) && (0 == tmp_find_SC))
		{
			tmp_cmp_SC = strncmp(tmp_serviceHandle_P[tmp_loop_SC].MethodContent_SCP,
								 tmp_requestData_P->Method_SCP,
								 strlen(tmp_serviceHandle_P[tmp_loop_SC].MethodContent_SCP));
			if (0 == tmp_cmp_SC)
			{
				tmp_match_SC = CTRL_IsMatchByPath(tmp_serviceHandle_P[tmp_loop_SC].PathRegEx_SCP,
												  tmp_requestData_P->Path_info_SCP);
				if (0 == tmp_match_SC)
				{
					tmp_find_SC = 1;
					if (NULL != tmp_serviceHandle_P[tmp_loop_SC].CtrlHandler_FN)
					{
						tmp_ret_SC = tmp_serviceHandle_P[tmp_loop_SC].CtrlHandler_FN(tmp_requestData_P);
					}
					else
					{
						LOG_debug("Error(CTRL_Distribute): Ctrl handle NULL.\n");
						tmp_ret_SC = -2;
					}
				}
				else
				{
					tmp_loop_SC = tmp_loop_SC + 1;
				}
			}
			else
			{
				tmp_loop_SC = tmp_loop_SC + 1;
			}
		}

		if (0 == tmp_find_SC)
		{
			LOG_debug("CTRL_Distribute: NO MATCH.\n");
		}
		else
		{
			LOG_debug("CTRL_Distribute: Service handle retrun %d.\n", tmp_ret_SC);
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
schar CTRL_IsMatchByPath(const schar *tmp_regEx_UCP, const schar *tmp_message_UCP)
{
	schar tmp_ret_SC = 0;
	regex_t preg = {0};
	size_t nmatch = 1;
	regmatch_t pmatch[1] = {0};

	if ((NULL == tmp_regEx_UCP) || (NULL == tmp_message_UCP))
	{
		LOG_debug("Error(ctrl_IsMatchByPath): Param is NULL.\n");
		return -1;
	}
	else
	{
		tmp_ret_SC = regcomp(&preg, tmp_regEx_UCP, REG_EXTENDED);
		if (0 == tmp_ret_SC)
		{
			tmp_ret_SC = regexec(&preg, tmp_message_UCP, nmatch, pmatch, REG_NOTEOL);
			regfree(&preg);
		}
		else
		{
			LOG_debug("Error(ctrl_IsMatchByPath): regcomp FAILED.\n");
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

schar CTRL_GetContentByFormdata(const uchar *tmp_formdata_UCP,
								const uchar *tmp_itemName_UCP, CtrlContent_t *tmp_content_P)
{
	schar tmp_cmp_SC = 0;
	schar tmp_ret_SC = 0;
	schar tmp_formdata_SC[REQUEST_DATA_MAX_SIZE + 1] = {0};
	schar *firstLevelSplitor = "&";
	schar *token = NULL;

	if ((NULL == tmp_formdata_UCP) || (NULL == tmp_itemName_UCP) || (NULL == tmp_content_P))
	{
		LOG_debug("Error(CTRL_GetContentByFormdata): Param is NULL.\n");
		return -1;
	}
	else
	{
		strncpy(tmp_formdata_SC, tmp_formdata_UCP, REQUEST_DATA_MAX_SIZE);
		tmp_ret_SC = -1;
		char *formdata_rightvalue;
		token = strtok_r(tmp_formdata_SC, FIRST_LEVEL_SPLITOR, &formdata_rightvalue);
		while (NULL != token)
		{
			char *rightValue;
			char *leftValue = strtok_r(token, SECOND_LEVEL_SPLITOR, &rightValue);
			tmp_cmp_SC = strncmp(leftValue, tmp_itemName_UCP, strlen(tmp_itemName_UCP));
			if (0 == tmp_cmp_SC)
			{
				if (NULL != token)
				{
					if (CTRL_CONTENT_TYPE_STRING == tmp_content_P->ContentType_SC)
					{
						strncpy(tmp_content_P->ContentValueString,
								rightValue, tmp_content_P->ContentStringMaxLen_SC);
						tmp_ret_SC = 0;
					}
					else if (CTRL_CONTENT_TYPE_INT == tmp_content_P->ContentType_SC)
					{
						tmp_content_P->ContentValueInt_SI = atoi(rightValue);
						tmp_ret_SC = 0;
					}
					else
					{
						LOG_debug("Error(CTRL_GetStringByItemName): Get String TYPE wrong.\n");
					}
					token = NULL;
				}
				else
				{
					LOG_debug("Error(CTRL_GetStringByItemName): Get String fail.\n");
				}
			}
			else
			{
				token = strtok_r(NULL, firstLevelSplitor, &formdata_rightvalue);
			}
		}
	}
	return tmp_ret_SC;
}

/*==========================================================================*/
/*	Description 	:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history 		:2023/03/08 : DHC Sun.YF : 新規作成 					*/
/*==========================================================================*/
schar CTRL_GetContentByJsondata(const uchar *tmp_Jsondata_UCP,
								const uchar *tmp_itemName_UCP, CtrlContent_t *tmp_content_P)
{
	schar tmp_ret_SC = 0;
	cJSON *tmp_cJson_p = NULL;
	cJSON *tmp_content_p = NULL;

	if ((NULL == tmp_Jsondata_UCP) || (NULL == tmp_itemName_UCP) || (NULL == tmp_content_P))
	{
		LOG_debug("Error(CTRL_GetContentByJsondata): Param is NULL.\n");
		return -1;
	}
	else
	{
		tmp_cJson_p = cJSON_Parse(tmp_Jsondata_UCP);
		/* JSON字符串外面会包一层“data”节点，从data节点中获取目标数据 */
		cJSON* dataJSON = cJSON_GetObjectItem(tmp_cJson_p, "data");
		if (NULL != tmp_cJson_p)
		{
			tmp_content_p = cJSON_GetObjectItem(dataJSON, tmp_itemName_UCP);
			if (NULL != tmp_content_p)
			{
				if (CTRL_CONTENT_TYPE_STRING == tmp_content_P->ContentType_SC)
				{
					strncpy(tmp_content_P->ContentValueString,
							tmp_content_p->valuestring, tmp_content_P->ContentStringMaxLen_SC);
					tmp_ret_SC = 0;
				}
				else if (CTRL_CONTENT_TYPE_INT == tmp_content_P->ContentType_SC)
				{
					tmp_content_P->ContentValueInt_SI = atoi(tmp_content_p->valuestring);
					tmp_ret_SC = 0;
				}
				else
				{
					LOG_debug("Error(CTRL_GetStringByItemName): Get String TYPE wrong.\n");
					tmp_ret_SC = -1;
				}
				cJSON_Delete(tmp_cJson_p);
			}
			else
			{
				LOG_debug("Error(CTRL_GetStringByItemName): cJSON_GetObjectItem failed.\n");
				tmp_ret_SC = -1;
			}
		}
		else
		{
			LOG_debug("Error(CTRL_GetStringByItemName): cJSON_Parse failed.\n");
			tmp_ret_SC = -1;
		}
	}
	return tmp_ret_SC;
}

void CTRL_setOutputJson(cJSON *tmp_responseJSon_P)
{
	schar *tmp_response_SCP = NULL;

	cJSON *responseJSon = cJSON_CreateObject();
	cJSON_AddStringToObject(responseJSon, "inspectionCode", "");
	cJSON_AddStringToObject(responseJSon, "errorRating", "");
	cJSON_AddStringToObject(responseJSon, "msg", "");
	cJSON *dataNode = cJSON_CreateObject();
	cJSON_AddItemReferenceToObject(responseJSon, "data", tmp_responseJSon_P);

	FRM_CgiHeaderContentType("text/json");
	tmp_response_SCP = cJSON_Print(responseJSon);

	if (NULL != tmp_response_SCP)
	{
		printf("%s\n", tmp_response_SCP);
	}
	else
	{
		LOG_debug("Error(ctrl_GetSplash): CJson print failed.\n");
	}

	cJSON_Delete(tmp_responseJSon_P);
}

void CTRL_writeLog(schar *url, char *opInfo)
{
#ifndef RELEASE
	char endpiontInfo[512];
	schar tmp_auth_token_cp[AUTH_TOKEN_SIZE];
	schar pictureId[32];
	memset(pictureId, '\0', 32);
	int arrayLength = sizeof(namePicIdMappings) / sizeof(NamePicIdMapping);
	int i;
	FRM_CgiCookieString(AUTH_TOKEN, tmp_auth_token_cp, AUTH_TOKEN_SIZE);
	SessionUserInfo_t sessionUserInfo;
	FRM_GetSession(tmp_auth_token_cp, &sessionUserInfo);
	sprintf(endpiontInfo, "%s, %s, %s", sessionUserInfo.userId, sessionUserInfo.phoneType, sessionUserInfo.appVersion);

	for (int i = 0; i < arrayLength; i++)
	{
		if (strncmp(url, namePicIdMappings[i].url, 64) == 0)
		{
			strncpy(pictureId, namePicIdMappings[i].pictureId, 32);
		}
	}

	stub_Writelog(endpiontInfo, pictureId, opInfo);
#endif
}

void CTRL_writeLogWithAuthenToken(char *authenToken, char *pictureId, char *opInfo)
{
	char endpiontInfo[512];
	SessionUserInfo_t sessionUserInfo;
	FRM_GetSession(authenToken, &sessionUserInfo);
	sprintf(endpiontInfo, "%s, %s, %s", sessionUserInfo.userId, sessionUserInfo.phoneType, sessionUserInfo.appVersion);

	stub_Writelog(endpiontInfo, pictureId, opInfo);
}


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*End Of File*/
