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
#include <stdlib.h>
#include <string.h>
#include "ctrl_g.h"
#include "srv_config_g.h"
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
extern schar SRV_GetConfig(configSetInfo *tmp_dataResponse_P)
{
	schar tmp_ret_SC = 0;
	uint tmp_i = 0;
	
	if(NULL == tmp_dataResponse_P)
	{
		LOG_debug("Error(SRV_GetConfig): Param is NULL.\n" );
		return -1;
	}
	else
	{
		LOG_debug("HELLO WORLD!\n");

		memset(tmp_dataResponse_P, 0, sizeof(configSetInfo));
		strcpy(tmp_dataResponse_P->sysConfig, "0");
		strcpy(tmp_dataResponse_P->extPvExist, "0");
		strcpy(tmp_dataResponse_P->othDevice, "0");
		strcpy(tmp_dataResponse_P->clipAction, "0");
		strcpy(tmp_dataResponse_P->fitType, "0");
		strcpy(tmp_dataResponse_P->selfUseTyp, "0");
		strcpy(tmp_dataResponse_P->salesValid, "0");
		strcpy(tmp_dataResponse_P->outputCtrlT, "0");
		strcpy(tmp_dataResponse_P->powStation, "0");
		strcpy(tmp_dataResponse_P->electricSrv, "0");
		strcpy(tmp_dataResponse_P->pushUp, "0");
		strcpy(tmp_dataResponse_P->orcExist, "0");
		strcpy(tmp_dataResponse_P->loadConnType, "0");
		tmp_dataResponse_P->sysFrequency = 0;
		tmp_dataResponse_P->pcsKwh = 0;
		tmp_dataResponse_P->outputChgRate = 0;
		tmp_dataResponse_P->setDate = 0;
		tmp_dataResponse_P->contractVal = 0;
		tmp_dataResponse_P->pvPanel = 0;
		tmp_dataResponse_P->eqCapacity = 0;
		tmp_dataResponse_P->rpgUl = 0;

		tmp_dataResponse_P->sysVpkLvl = 0;
		tmp_dataResponse_P->sysVpkTime = 0;
		tmp_dataResponse_P->sysTrchLvl = 0;
		tmp_dataResponse_P->sysTrchTime = 0;
		tmp_dataResponse_P->freqRiseLvl = 0;
		tmp_dataResponse_P->freqRiseTime = 0;
		tmp_dataResponse_P->freqDropLvl = 0;
		tmp_dataResponse_P->freqDropTime = 0;
		tmp_dataResponse_P->rtnTime = 0;
		tmp_dataResponse_P->restraintVal = 0;
		tmp_dataResponse_P->restraintSt = 0;
		tmp_dataResponse_P->powFactorCtrl = 0;
		tmp_dataResponse_P->singleOpLvl = 0;
		tmp_dataResponse_P->freqFbGain = 0;
		tmp_dataResponse_P->allowFreqLvl = 0;
		
		for(tmp_i = 0; tmp_i < SETTING_TABLE_LEN; tmp_i++)
		{
			tmp_dataResponse_P->sysVpkLvlList[tmp_i] = tmp_i + 1;
			tmp_dataResponse_P->sysVpkTimeList[tmp_i] = tmp_i + 1;
			tmp_dataResponse_P->sysTrchLvlList[tmp_i] = tmp_i + 1;
			tmp_dataResponse_P->sysTrchTimeList[tmp_i] = tmp_i + 1;
			tmp_dataResponse_P->freqRiseLvlList[tmp_i] = tmp_i + 1;
			tmp_dataResponse_P->freqRiseTimeList[tmp_i] = tmp_i + 1;
			tmp_dataResponse_P->freqDropLvlList[tmp_i] = tmp_i + 1;
			tmp_dataResponse_P->freqDropTimeList[tmp_i] = tmp_i + 1;
			tmp_dataResponse_P->rtnTimeList[tmp_i] = tmp_i + 1;
			tmp_dataResponse_P->restraintValList[tmp_i] = tmp_i + 1;
			tmp_dataResponse_P->restraintStList[tmp_i] = tmp_i + 1;
			tmp_dataResponse_P->powFactorCtrlList[tmp_i] = tmp_i + 1;
			tmp_dataResponse_P->singleOpLvlList[tmp_i] = tmp_i + 1;
			tmp_dataResponse_P->freqFbGainList[tmp_i] = tmp_i + 1;
			tmp_dataResponse_P->allowFreqLvlList[tmp_i] = tmp_i + 1;

			tmp_dataResponse_P->setValueTbl[tmp_i] = tmp_i + 1;
		}
	}
	return tmp_ret_SC;
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*End Of File*/
