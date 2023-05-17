/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Hao.JQ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/19												*/
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
#include "srv_home_g.h"
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
/*	history			:2023/04/20 : DHC Hao.JQ : 新規作成						*/
/*==========================================================================*/
extern schar SRV_GetHome(const homeRequest *tmp_dataRequest_P, homeInfo *tmp_dataResponse_P)
{
	schar tmp_ret_SC = 0;
	
	if((NULL == tmp_dataRequest_P) || (NULL == tmp_dataResponse_P))
	{
		LOG_debug("Error(SRV_GetHome): Param is NULL.\n" );
		return -1;
	}
	else
	{
		LOG_debug("HELLO WORLD!\n");

		memset(tmp_dataResponse_P, 0, sizeof(homeInfo));
		strcpy(tmp_dataResponse_P->sysConfig, "0");
		tmp_dataResponse_P->outputCtrlPct = 50;
		strcpy(tmp_dataResponse_P->opMode, "0");
		strcpy(tmp_dataResponse_P->emerMode, "0");
		strcpy(tmp_dataResponse_P->opSetReq, "0");
		strcpy(tmp_dataResponse_P->outputCtrlSt, "0");
		strcpy(tmp_dataResponse_P->connLockSt, "0");
		strcpy(tmp_dataResponse_P->desigPowOpFlg, "0");
		strcpy(tmp_dataResponse_P->runStop, "0");
		strcpy(tmp_dataResponse_P->noticeAddFlg, "0");
		strcpy(tmp_dataResponse_P->fwUpdNo, "0");
		strcpy(tmp_dataResponse_P->fwUpdMode, "0");
		strcpy(tmp_dataResponse_P->sbOpSt, "0");
		
		(tmp_dataResponse_P->testFile).batteryCharging = 0;
		(tmp_dataResponse_P->testFile).batteryDischarge = 1;
		(tmp_dataResponse_P->testFile).buyElectricityE1 = 0;
		(tmp_dataResponse_P->testFile).buyElectricityC2 = 1;
		(tmp_dataResponse_P->testFile).V2hCharging = 0;
		(tmp_dataResponse_P->testFile).V2hDischarge = 2;
		(tmp_dataResponse_P->testFile).sunlightPvGen = 0;
		(tmp_dataResponse_P->testFile).extSunlightPv = 1;
		(tmp_dataResponse_P->testFile).selfRelOperSwitch = 1;
		(tmp_dataResponse_P->testFile).pvOutDischarge = 2;
		(tmp_dataResponse_P->testFile).pvOutRet = 0;

		strcpy(tmp_dataResponse_P->extPvExist, "0");
		tmp_dataResponse_P->pvAvgPow = 100;
		tmp_dataResponse_P->extPvAvgPow = 500;
		tmp_dataResponse_P->rcAvgPow = 233;
		tmp_dataResponse_P->vehicleCdtAvgPow = 666;
		tmp_dataResponse_P->sbCdtAvgPow = 99;
		strcpy(tmp_dataResponse_P->purPowDfv, "0");
		strcpy(tmp_dataResponse_P->vehicleDcSt, "0");
		strcpy(tmp_dataResponse_P->mainteStatus, "0");
		strcpy(tmp_dataResponse_P->poFlg, "0");
		strcpy(tmp_dataResponse_P->mainteOperation, "0");
		tmp_dataResponse_P->evBattLvl3 = 0;
		tmp_dataResponse_P->socFlg = 0;
		tmp_dataResponse_P->mainteSchedule = 0;
		tmp_dataResponse_P->cTime = 0;
	}
	return tmp_ret_SC;
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*End Of File*/
