/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Wang.M												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/13												*/
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
#include "ctrl_g.h"
#include "common_g.h"
#include "srv_splash_g.h"
#include "util_logger_g.h"
#include "ref_cjson_g.h"
#include "session_g.h"
#include "frm_cgiapi_g.h"
#include "stub_g.h"
#include "util_g.h"
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/

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
/*	history			:2023/04/13 : DHC Hao.JQ : 新規作成						*/
/*==========================================================================*/
extern schar SRV_GetSplash(const startupRequest *tmp_dataRequest_P, startupComInfo *tmp_dataResponse_P, schar *tmp_sessionId_t)
{
	sint ret = 0;
	schar tmp_ret_SC = 0;
	SessionUserInfo_t sessionUserInfo = {0};

	if ((NULL == tmp_dataRequest_P) || (NULL == tmp_dataResponse_P))
	{
		LOG_debug("Error(SRV_GetSplash): Param is NULL.\n");
		return -1;
	}
	else
	{
		LOG_debug("HELLO WORLD!\n");

		if ((NULL == tmp_dataRequest_P->custAplId) || (strcmp(tmp_dataRequest_P->custAplId, "") == 0))
		{
			strcpy(sessionUserInfo.appVersion, tmp_dataRequest_P->appVer);
			strcpy(sessionUserInfo.noticeToken, tmp_dataRequest_P->noticeToken);
			strcpy(sessionUserInfo.phoneType, tmp_dataRequest_P->hostModel);
			strcpy(sessionUserInfo.userId, tmp_dataRequest_P->serviceAplId);
			ret = FRM_SetSession(tmp_sessionId_t, &sessionUserInfo);
		}
		else if ((NULL == tmp_dataRequest_P->serviceAplId) || (strcmp(tmp_dataRequest_P->serviceAplId, "") == 0))
		{
			strcpy(sessionUserInfo.appVersion, tmp_dataRequest_P->appVer);
			strcpy(sessionUserInfo.noticeToken, tmp_dataRequest_P->noticeToken);
			strcpy(sessionUserInfo.phoneType, tmp_dataRequest_P->hostModel);
			strcpy(sessionUserInfo.userId, tmp_dataRequest_P->custAplId);
			ret = FRM_SetSession(tmp_sessionId_t, &sessionUserInfo);
		}
		else
		{
		}
		char local_ip[20] = {0};
		UTIL_GetLocalip(local_ip);
		FRM_CgiHeaderCookieSetString(AUTH_TOKEN, tmp_sessionId_t, 1200, "/", local_ip);

		memset(tmp_dataResponse_P, 0, sizeof(startupComInfo));
		strcpy(tmp_dataResponse_P->cxDo, "0");
		tmp_dataResponse_P->cTime = 230417101020;
		strcpy(tmp_dataResponse_P->outputCtrlT, "0");
		strcpy(tmp_dataResponse_P->ntpSrvComStat, "0");
		strcpy(tmp_dataResponse_P->extPvExist, "0");
		strcpy(tmp_dataResponse_P->mcErrFlag, "0");
		strcpy(tmp_dataResponse_P->dcbSn, "0");
		strcpy(tmp_dataResponse_P->fwUpd, "0");
		strcpy(tmp_dataResponse_P->modelName, "0");
		strcpy(tmp_dataResponse_P->sysSn, "0");
		strcpy(tmp_dataResponse_P->vehicleDcModel, "0");
		strcpy(tmp_dataResponse_P->vehicleDcSn, "0");
		strncpy(tmp_dataResponse_P->authToken, tmp_sessionId_t, SESSION_ID_LENGTH + 1);
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
/*	history			:2023/05/04 : DHC Zhang.WY : 新規作成					*/
/*==========================================================================*/
extern schar SRV_GetSplInfo(startupComInfo *tmp_dataResponse_P)
{
	schar tmp_ret_SC = 0;

	if (NULL == tmp_dataResponse_P)
	{
		LOG_debug("Error(SRV_GetSplInfo): Param is NULL.\n");
		return -1;
	}
	else
	{
		LOG_debug("HELLO WORLD!\n");

		memset(tmp_dataResponse_P, 0, sizeof(startupComInfo));
		strcpy(tmp_dataResponse_P->cxDo, "0");
		tmp_dataResponse_P->cTime = 230504101020;
		strcpy(tmp_dataResponse_P->outputCtrlT, "0");
		strcpy(tmp_dataResponse_P->ntpSrvComStat, "0");
		strcpy(tmp_dataResponse_P->extPvExist, "0");
		strcpy(tmp_dataResponse_P->mcErrFlag, "0");
		strcpy(tmp_dataResponse_P->dcbSn, "0");
		strcpy(tmp_dataResponse_P->fwUpd, "0");
		strcpy(tmp_dataResponse_P->modelName, "0");
		strcpy(tmp_dataResponse_P->sysSn, "0");
		strcpy(tmp_dataResponse_P->vehicleDcModel, "5");
		strcpy(tmp_dataResponse_P->vehicleDcSn, "4");
	}
	return tmp_ret_SC;
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*End Of File*/
