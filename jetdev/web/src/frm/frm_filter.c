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
#include "frm_filter.h"
#include "filter_g.h"
#include "frm_cgiapi_g.h"
#include "string.h"
#include "ref/ref_cgic.h"
#include "session_g.h"
#include "util_logger_g.h"
#include "common_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
sint (*const filter_handlers[])(const RequestData_t *request_data) =
	{
		FILTER_authenticate};

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
sint FRM_filter(const RequestData_t *request_data)
{
	LOG_entry("FRM_filter");
	sint i = 0;
	sint ret = RET_SUCCESS_CODE;
	if ((NULL == request_data))
	{
		MlogErrorParameterNull();
	}
	else
	{
		/* filter is chain, node on the chain will be execute in order, encounter error will return directly */
		/* multi-contian should use "()" */
		while ((i < MgetArrayMemCount_si(filter_handlers)) && (RET_SUCCESS_CODE == ret))
		{
			ret = filter_handlers[i](request_data);
			i++;
		}
	}

	LOG_leave("FRM_filter");
	return ret;
}

/*End Of File*/