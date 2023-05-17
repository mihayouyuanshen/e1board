/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Zhang.WY											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/17												*/
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

#include "cgi_svc.h"
#include "common_g.h"
#include "frm_cgiapi_g.h"
#include "filter_g.h"
#include "typedef.h"
#include "util_logger_g.h"
#include "frm_dispatcher_g.h"
#include "ctrl_g.h"
#include "util_g.h"
#include "test_g.h"
#include "ctrl_init_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
int cgiMain(void)
{
	CTRL_init();
	main_handleRequest();
	LOG_info("complete!");

	return 1;
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
int main_handleRequest(void)
{
	LOG_entry("main_handleRequest");
	/* 1. declare and init the request data structure, which will delivery to next module*/
	RequestData_t request_data;
	memset(&request_data, 0, sizeof(RequestData_t));
	/* 2. after analysis the request url and method, to build the request info */
	main_buildRequestInfo(&request_data);
	/* 3. go through the filter chain, to filter the request */
	sint tmp_filter_si = FRM_filter(&request_data);
	/* 4. if pass all the filter, dispatcher will dispatch the controller to handler the request */
	if (RET_SUCCESS_CODE == tmp_filter_si)
	{
		sint ret = FRM_Dispatch(&request_data);
		if (RET_ERROR_CODE == ret)
		{
			FRM_CgiHeaderStatus(404, "Can't find the controller");
		}
		else
		{
			/* 処理なし */
		}
	}
	else
	{
		LOG_error("filter error");
	}

	LOG_leave("main_handleRequest");

	return 1;
}

sint main_buildRequestInfo(RequestData_t *request_data)
{
	LOG_entry("main_buildRequestInfo");
	/* input parameters check */
	if (NULL == request_data)
	{
		MlogErrorParameterNull();
		LOG_leave("main_buildRequestInfo");

		return RET_ERROR_CODE;
	}

	schar requestRaw[REQUSET_URL_MAX_SIZE];
	schar request[REQUSET_URL_MAX_SIZE];

	McharArrayInit(request_data->Method_SCP, REQUSET_METHOD_MAX_SIZE);
	McharArrayInit(request, REQUSET_URL_MAX_SIZE);
/**********************************************************************/
/* Loacal test case, set the test data(TEST_FLG defined in common_g.h)*/
/* 1. data set fake value                                             */
/* 2. session just return "sess_abc"                                  */
/**********************************************************************/
// #ifdef TEST_FLG
// /* test GET request*/
// #if 0
// 	// strncpy(requestRaw, "/api/servicesMenu/outputCtrl", REQUSET_URL_MAX_SIZE);
// 	// strncpy(requestRaw, "/api/start/splash?noticeToken=XXX&&custAplId=&&serviceAplId=yyy&&hostModel=abc&&appVer=1001", REQUSET_URL_MAX_SIZE);
// 	// strncpy(requestRaw, "/api/polling/storageStatus", REQUSET_URL_MAX_SIZE);
// 	// strncpy(requestRaw, "/api/start/splashInfo", REQUSET_URL_MAX_SIZE);
// 	// strncpy(requestRaw, "/api/home?timeStamp=1200", REQUSET_URL_MAX_SIZE);
// 	// strncpy(requestRaw, "/api/modeSet", REQUSET_URL_MAX_SIZE);
// 	// strncpy(requestRaw, "/api/history_search", REQUSET_URL_MAX_SIZE);
// 	// strncpy(requestRaw, "/api/tradeHist?dd=21&&mm=04&&yyyy=2023", REQUSET_URL_MAX_SIZE);
// 	// strncpy(requestRaw, "/api/records?dd=21&&mm=04&&yyyy=2023", REQUSET_URL_MAX_SIZE);
// 	// strncpy(requestRaw, "/api/vehicleRecords?dd=21&&mm=04&&yyyy=2023", REQUSET_URL_MAX_SIZE);
// 	// strncpy(requestRaw, "/api/home/powerFlow", REQUSET_URL_MAX_SIZE);
// 	strncpy(requestRaw, "/api/notice?maxNum=100", REQUSET_URL_MAX_SIZE);
// 	// strncpy(requestRaw, "/api/config", REQUSET_URL_MAX_SIZE);
// 	strncpy(request_data->Method_SCP, "GET", REQUSET_METHOD_MAX_SIZE);

// /* test DELETE request */
// #elif 0
// 	strncpy(requestRaw, "/api/usr/option/123", REQUSET_URL_MAX_SIZE);
// 	strncpy(request_data->Method_SCP, "DELETE", REQUSET_METHOD_MAX_SIZE);

// /* test POST request */
// #else
// 	// strncpy(requestRaw, "/api/timeSet", REQUSET_URL_MAX_SIZE);
// 	// strncpy(request_data->Data_SCP, "{\"data\":{\"cTime\":\"2023\"}}", REQUEST_DATA_MAX_SIZE);
// 	strncpy(requestRaw, "/api/errorDisp/errClear", REQUSET_URL_MAX_SIZE);
// 	// strncpy(requestRaw, "/api/extCTPosConfirmIns", REQUSET_URL_MAX_SIZE);
// 	// strncpy(request_data->Data_SCP, "{\"data\":{\"againstCtConnSet\":\"01\"}}", REQUEST_DATA_MAX_SIZE);
// 	// strncpy(requestRaw, "/api/switchModuleCTUse", REQUSET_URL_MAX_SIZE);
// 	// strncpy(request_data->Data_SCP, "{\"data\":{\"againstCtConnSet\":\"02\"}}", REQUEST_DATA_MAX_SIZE);
// 	strncpy(request_data->Method_SCP, "POST", REQUSET_METHOD_MAX_SIZE);
// 	strncpy(request_data->Data_SCP, "{\"data\":{\"name\":\"lorry\", \"age\":\"23\"}}", REQUEST_DATA_MAX_SIZE);
// #endif

#ifdef TEST_FLG
	RequestName name;
	name = HOME;
	stub_getRequestURL(name, requestRaw, request_data);

/***************************************************/
/* deploy to httpd test case, get data from environment */
/***************************************************/

#else
	/* use rewrite to implement th RESTful, so need from REQEST URI to get raw URL*/
	memcpy(requestRaw, getenv("REQUEST_URI"), REQUSET_URL_MAX_SIZE);
	/* get the request method: GET/ POST/ PUT/ DELETE */
	memcpy(request_data->Method_SCP, getenv("REQUEST_METHOD"), REQUSET_METHOD_MAX_SIZE);

#endif
	/* get the request method: get/ post/ put/ delete, and change it to lower case */
	main_getRequesMethod(request_data->Method_SCP);
	/* get the pathinfo which DON'T contain QUERY STRING */
	main_getRequestPathInfo(requestRaw, request_data->Path_info_SCP);
	/* get the data from request url(GET), or body(POST/PUT) */
	main_getRequestData(request_data->Method_SCP, request_data->Path_info_SCP, request_data->Data_SCP);

	LOG_debug("[Request Data] method: %s, path_info: %s, data: %s",
			  request_data->Method_SCP, request_data->Path_info_SCP, request_data->Data_SCP);
	LOG_leave("main_buildRequestInfo");
}

sint main_getRequesMethod(schar *requestMethod)
{
	LOG_entry("main_getRequesMethod");
	if (NULL == requestMethod)
	{
		MlogErrorParameterNull();
		LOG_leave("main_getRequesMethod");

		return RET_ERROR_CODE;
	}
	sint ret = RET_SUCCESS_CODE;
	if (requestMethod == NULL)
	{
		LOG_error("method is null\n");
		return RET_ERROR_CODE;
	}
	else
	{
		UTL_StrToLower(requestMethod);
		LOG_debug("method is: %s\n", requestMethod);
	}

	LOG_leave("main_getRequesMethod");

	return ret;
}

sint main_getRequestPathInfo(const schar *requestRaw, schar *request)
{
	LOG_entry("main_getRequestPathInfo");
	if (NULL == requestRaw || NULL == request)
	{
		MlogErrorParameterNull();
		LOG_leave("main_getRequestPathInfo");
		return RET_ERROR_CODE;
	}

	sint ret = RET_SUCCESS_CODE;
	uint rawUrlLen = 0;

	LOG_debug("raw request: %s", requestRaw);
	/* use rewrite to implement th RESTful, so need from REQEST URI to get raw URL*/
	/* if the raw request is EMPTY */
	if ((requestRaw == NULL) || (0 == strlen(requestRaw)))
	{
		ret = RET_ERROR_CODE;
	}
	else
	{
		rawUrlLen = strlen(requestRaw);
		/* every request url is head of "/cig", which use to identity request should be redirect to CGI to handle */
		/* so get the path info should skip the first 4 char "/api" to get the rea path info */
		if (REQUSET_URL_MAX_SIZE >= (rawUrlLen - REQUEST_PREFIX_LENGTH))
		{

			strncpy(request, requestRaw + 4, rawUrlLen - REQUEST_PREFIX_LENGTH);
		}
		else
		{
			LOG_error("raw Request length exceed max length: %d", rawUrlLen);
			ret = RET_ERROR_CODE;
		}
	}

	LOG_leave("main_getRequesMethod");
	return ret;
}

sint main_getRequestData(const char *requestMethod, char *request, char *tmp_data_scp)
{
	if (NULL == requestMethod || NULL == request || NULL == tmp_data_scp)
	{
		MlogErrorParameterNull();
		return RET_ERROR_CODE;
	}

	schar *splitChar = "?";
	schar *tmp_str_scp;
	if (!strcmp(requestMethod, "post") || !strcmp(requestMethod, "put"))
	{
#ifndef TEST_FLG
		FRM_CgiGetFormData(tmp_data_scp);
#endif
		LOG_debug("form data is: %s", tmp_data_scp);
	}
	else
	{
		tmp_str_scp = strtok(request, splitChar);
		tmp_str_scp = strtok(NULL, splitChar);
		if (tmp_str_scp != NULL)
		{
			strncpy(tmp_data_scp, tmp_str_scp, REQUEST_DATA_MAX_SIZE);
		}
		else
		{
			/* 処理なし */
		}
	}

	return RET_SUCCESS_CODE;
}

/*End Of File*/