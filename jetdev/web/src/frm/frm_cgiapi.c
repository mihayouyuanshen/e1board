/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Zhang.WY											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/07												*/
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
#include "frm_cgiapi_g.h"
#include "ref/ref_cgic.h"
#include "typedef.h"
#include "util_logger_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
schar *FRM_CgiGetFormData(schar *poststr)
{
	uint len;
	schar *lenstr;
	lenstr = getenv("CONTENT_LENGTH");
	if (lenstr == NULL)
		LOG_error("Error parameters should be entered!\n");
	else
	{
		len = atoi(lenstr);
		fgets(poststr, len + 1, stdin);
	}

	return poststr;
}

schar *FRM_CgiGetQueryString(void)
{
	return getenv("QUERY_STRING");
}

void FRM_CgiCookieString(schar *name, schar *tmp_result_cp, int tmp_max_i)
{
	cgiCookieString(name, tmp_result_cp, tmp_max_i);
}

void FRM_CgiHeaderCookieSetString(schar *name, schar *tmp_value_cp, int tmp_secondsToLive_i, schar *tmp_path_cp, schar *tmp_domain_cp)
{
	cgiHeaderCookieSetString(name, tmp_value_cp, tmp_secondsToLive_i, tmp_path_cp, tmp_domain_cp);
}

void FRM_CgiHeaderContentType(schar *tmp_mimeType_scp)
{
	cgiHeaderContentType(tmp_mimeType_scp);
}

void FRM_CgiHeaderStatus(int status, char *statusMessage)
{
	cgiHeaderStatus(status, statusMessage);
}

/*End Of File*/