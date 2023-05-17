/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Zhang.WY											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/20												*/
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
#include <string.h>

#include "common_g.h"
#include "filter_g.h"
#include "util_logger_g.h"
#include "frm_cgiapi_g.h"
#include "session_g.h"
#include "util_g.h"

#define AUTH_ERROR "Token Authentication Error"
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
/*	Description		:void													*/
/*--------------------------------------------------------------------------*/
/*	param			:authen_filter											*/
/*--------------------------------------------------------------------------*/
/*	return			:RequestData_t* requestData								*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/20 : DHC Zhang.WY : 新規作成					*/
/*==========================================================================*/
sint FILTER_authenticate(const RequestData_t *requestData)
{
	LOG_entry("authen_filter");

	sint ret = RET_ERROR_CODE;
	schar auth_token[AUTH_TOKEN_SIZE];
	if (0 == strncmp(requestData->Method_SCP, "get", 3))
	{
		sint CHAR_LENGTH = 15;
		schar tmpChars[CHAR_LENGTH];
		UTIL_SubStrByChar(requestData->Path_info_SCP, '?', tmpChars, CHAR_LENGTH);

		strncpy(tmpChars, requestData->Path_info_SCP, CHAR_LENGTH);
		/* if the request is start/splash, will not authentication, just return pass */
		if (0 == strcmp(tmpChars, "/start/splash"))
		{
			return RET_SUCCESS_CODE;
		}
	}
	FRM_CgiCookieString(AUTH_TOKEN, auth_token, AUTH_TOKEN_SIZE);
#ifdef TEST_SKIP_AUTH
	ret = RET_SUCCESS_CODE;
#else
	/* if find authentication token in cookie */
	sint tmp_length = strlen(auth_token);
	if (tmp_length > 0)
	{
		ret = FRM_CheckSession(auth_token);
		if (ret != RET_SUCCESS_CODE)
		{
			FRM_CgiHeaderStatus(401, AUTH_ERROR);
		}
		else
		{
		}
	}
	else
	{
		FRM_CgiHeaderStatus(401, AUTH_ERROR);
	}
#endif
	LOG_leave("authen_filter");
	return ret;
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*End Of File*/
