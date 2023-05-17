/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Hao.JQ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/22												*/
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
#include "srv_log_g.h"
#include "util_logger_g.h"
#include "ref_cjson_g.h"

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
/*	history			:2023/04/23 : DHC Hao.JQ : 新規作成						*/
/*==========================================================================*/
extern schar SRV_GetLog(timeSearchHis *tmp_dataResponse_P)
{
	schar tmp_ret_SC = 0;
	
	if(NULL == tmp_dataResponse_P)
	{
		LOG_debug("Error(SRV_GetLog): Param is NULL.\n" );
		return -1;
	}
	else
	{
		LOG_debug("HELLO WORLD!\n");

		memset(tmp_dataResponse_P, 0, sizeof(timeSearchHis));
		tmp_dataResponse_P->fixedSkdLastDay = 10;
		tmp_dataResponse_P->setDate = 0;
		tmp_dataResponse_P->cTime = 0;
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
/*	history			:2023/04/23 : DHC Hao.JQ : 新規作成						*/
/*==========================================================================*/
extern schar SRV_GetLogBs(const historyPrama *tmp_dataRequest_P, bsHistoryInfo *tmp_dataResponse_P, uint *listNum)
{
	schar tmp_ret_SC = 0;
	uint tmp_i = 0;
	*listNum = 2u;
	
	if((NULL == tmp_dataRequest_P) || (NULL == tmp_dataResponse_P))
	{
		LOG_debug("Error(SRV_GetLogBs): Param is NULL.\n" );
		return -1;
	}
	else
	{
		LOG_debug("HELLO WORLD!\n");

		memset(tmp_dataResponse_P, 0, sizeof(bsHistoryInfo));
		tmp_dataResponse_P->pvRpgAc = 10;
		tmp_dataResponse_P->pvRpgOth = 10;
		tmp_dataResponse_P->setDate = 0;
		tmp_dataResponse_P->cTime = 0;
		strcpy(tmp_dataResponse_P->sysConfig, "0");
		
		for(tmp_i = 0; tmp_i < *listNum; tmp_i++)
		{
			tmp_dataResponse_P->bH[tmp_i].rpgIn = 0;
			tmp_dataResponse_P->bH[tmp_i].rpgExt = 12;
			tmp_dataResponse_P->bH[tmp_i].salesElectricity = 5;
			tmp_dataResponse_P->bH[tmp_i].buyElectricity = 23;
			strcpy(tmp_dataResponse_P->bH[tmp_i].timeZone, "2023");
			strcpy(tmp_dataResponse_P->bH[tmp_i].timeZoneFlg, "1");
		}
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
/*	history			:2023/05/03 : DHC Zhang.WY : 新規作成					*/
/*==========================================================================*/
extern schar SRV_GetLogSd(const historyPrama *tmp_dataRequest_P, sbDcHistoryInfo *tmp_dataResponse_P, uint *listNum)
{
	schar tmp_ret_SC = 0;
	uint tmp_i = 0;
	*listNum = 2u;
	
	if((NULL == tmp_dataRequest_P) || (NULL == tmp_dataResponse_P))
	{
		LOG_debug("Error(SRV_GetLogSd): Param is NULL.\n" );
		return -1;
	}
	else
	{
		LOG_debug("HELLO WORLD!\n");

		memset(tmp_dataResponse_P, 0, sizeof(sbDcHistoryInfo));
		tmp_dataResponse_P->maxChgVal = 10;
		tmp_dataResponse_P->pvRpgOth = 10;
		tmp_dataResponse_P->setDate = 0;
		tmp_dataResponse_P->cTime = 0;
		strcpy(tmp_dataResponse_P->sysConfig, "0");
		
		for(tmp_i = 0; tmp_i < *listNum; tmp_i++)
		{
			tmp_dataResponse_P->sDH[tmp_i].chgPow = 0;
			tmp_dataResponse_P->sDH[tmp_i].disChgPow = 12;
			tmp_dataResponse_P->sDH[tmp_i].batteryLvl = 5;
			strcpy(tmp_dataResponse_P->sDH[tmp_i].timeZone, "2023");
			strcpy(tmp_dataResponse_P->sDH[tmp_i].timeZoneFlg, "2");
		}
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
/*	history			:2023/05/03 : DHC Zhang.WY : 新規作成					*/
/*==========================================================================*/
extern schar SRV_GetLogVd(const historyPrama *tmp_dataRequest_P, vehicleDcHisInfo *tmp_dataResponse_P, uint *listNum)
{
	schar tmp_ret_SC = 0;
	uint tmp_i = 0;
	*listNum = 2u;
	
	if((NULL == tmp_dataRequest_P) || (NULL == tmp_dataResponse_P))
	{
		LOG_debug("Error(SRV_GetLogVd): Param is NULL.\n" );
		return -1;
	}
	else
	{
		LOG_debug("HELLO WORLD!\n");

		memset(tmp_dataResponse_P, 0, sizeof(vehicleDcHisInfo));
		tmp_dataResponse_P->setDate = 3;
		tmp_dataResponse_P->cTime = 0;
		strcpy(tmp_dataResponse_P->sysConfig, "0");
		
		for(tmp_i = 0; tmp_i < *listNum; tmp_i++)
		{
			tmp_dataResponse_P->vDH[tmp_i].vehicleBatt1 = 0;
			tmp_dataResponse_P->vDH[tmp_i].chgPow = 7;
			tmp_dataResponse_P->vDH[tmp_i].disChgPow = 2;
			strcpy(tmp_dataResponse_P->vDH[tmp_i].timeZone, "2023");
			strcpy(tmp_dataResponse_P->vDH[tmp_i].timeZoneFlg, "1");
		}
	}
	return tmp_ret_SC;
}
/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*End Of File*/
