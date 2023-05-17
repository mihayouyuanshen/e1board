/****************************************************************************/
/*	Description		:				*/
/*--------------------------------------------------------------------------*/
/*	Author			:Qi.JF													*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/27												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:EMDTR													*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#include "emdtr_g.h"
#include <string.h>
#include "ermc_g.h"
#include "edat_g.h"

/*--------------------------------------------------------------------------*/
/*	同期情報設定														*/
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
/*	history			:2023/04/27 : Qi.JF : 新規作成						*/
/*==========================================================================*/
void EMDTR_SetCurrentTime(EtimTm_t* timeInfo)
{
	/* 時刻管理機能のIF */
	ETIM_SyncSetTime(timeInfo);
}

/*--------------------------------------------------------------------------*/
/*															*/
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
/*	history			:2023/04/27 : Qi.JF : 新規作成						*/
/*==========================================================================*/
void EMDTR_GetCmd24DataInfo(void)
{
	EINV_ReqOpeMode();
}

/*--------------------------------------------------------------------------*/
/*																			*/
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
/*	history			:2023/04/27 : Qi.JF : 新規作成							*/
/*==========================================================================*/
void EMDTR_GetCmd41DataInfo(EmdtrCmd41DataInfo_t* data)
{
	/* 時刻管理機能のIF */
	ETIM_GetTime(&data->commBoardCurrentTime);

	EinvOpesttsInfo_t opesttsInfo;
	/* 送受信データ初期化 */
	memset(&opesttsInfo, 0x00, sizeof(opesttsInfo));
	EINV_GetOpeSttsInfo(&opesttsInfo);

	EinvOpemodeGet_t opeModeGet;
	memset(&opeModeGet, 0x00, sizeof(opeModeGet));
	EINV_GetOpeMode(&opeModeGet);
	
	EinvCalcAvg_t calcAvgValue;
	/* 送受信データ初期化 */
	memset(&calcAvgValue, 0x00, sizeof(calcAvgValue));
	EINV_GetCalcAvgValue(&calcAvgValue);

	/* *******************************疎通確認用設定******************************* */
	data->opesttsInfo.chargeDischargePwr = opesttsInfo.chargeDischargePwrAC;
	data->opesttsInfo.inverterOpeState = opesttsInfo.opeState;
	data->opesttsInfo.rsoc = opesttsInfo.rsoc;
	data->opesttsInfo.remainingBtW = opesttsInfo.remainingBt;
	data->opesttsInfo.soh = opesttsInfo.soh;
	data->opesttsInfo.operation = opesttsInfo.btOperation;
	data->opesttsInfo.pwrCut = opesttsInfo.pwrCut;
	strncpy(data->opesttsInfo.pcsErr, opesttsInfo.pcsErr, sizeof(data->opesttsInfo.pcsErr) - 1);
	strncpy(data->opesttsInfo.pcsErrRating, opesttsInfo.pcsErrRating, sizeof(data->opesttsInfo.pcsErrRating) - 1);
	//data->opesttsInfo.opeMode = opesttsInfo.opeMode;
	data->opesttsInfo.opeMode = (uchar)('7');
	data->opesttsInfo.pvGenPwr = calcAvgValue.pvGenPwrAvg;
	data->opesttsInfo.revPwrFlow = calcAvgValue.revPwrFlowAvg;
	data->opesttsInfo.chargeDischargePwr2 = calcAvgValue.btChargeDischPwrAvg;
	data->opesttsInfo.pvGenPwr2 = calcAvgValue.pvPwr2Avg;
	data->opesttsInfo.homeLoadPower = calcAvgValue.homeLoadPwr;
	data->opesttsInfo.suppressState=  opesttsInfo.suppressState;
	data->opesttsInfo.v2hConnectorLockState = opesttsInfo.carConnectorLockState;
	data->opesttsInfo.v2hState = opesttsInfo.carCharDiscOpratState;
	data->opesttsInfo.v2hOpeMode = opesttsInfo.emergencyMode;
	data->opesttsInfo.v2hRemainingCapacity1 = opesttsInfo.carRemainingCapacity1;
	data->opesttsInfo.v2hRemainingCapacity3 = opesttsInfo.carRemainingCapacity3;
	data->opesttsInfo.soc = opesttsInfo.uSOC;
	
	data->opesttsInfo.runModeSetFrom = opeModeGet.runModeSetFrom;
	data->opesttsInfo.splPowerRunFlag = opeModeGet.splPowerRunFlag;
}

/*--------------------------------------------------------------------------*/
/*																			*/
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
/*	history			:2023/04/27 : Qi.JF : 新規作成							*/
/*==========================================================================*/
void EMDTR_SetBoardError(schar const* errCode, schar errCodeRating)
{
	/* データエラー管理機能のIF */
	EDAT_SetBoardErr(errCode, errCodeRating);
}

/*--------------------------------------------------------------------------*/
/*																			*/
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
/*	history			:2023/04/27 : Qi.JF : 新規作成							*/
/*==========================================================================*/
void EMDTR_ClearBoardError(schar const* errCode)
{
	EDAT_ClearBoardErr(errCode);
}

/*--------------------------------------------------------------------------*/
/*																			*/
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
/*	history			:2023/04/27 : Qi.JF : 新規作成							*/
/*==========================================================================*/
void EMDTR_NotifyCmd24DataInfo(EinvOpemodeGet_t* data)
{
	EmdtrOpemodeGet_t openMode;

	/* 送受信データ初期化 */
	memset(&openMode, 0x00, sizeof(openMode));

	if(NULL != data)
	{
		openMode.acLowerLimit = data->userLowerLimit;
		openMode.acUpperLimit = data->userUpperLimit;
		openMode.emergencyMode = data->emergencyMode;
		openMode.opeMode = data->opeMode;
		openMode.startOrStop = data->startOrStop;
		strncpy(openMode.chargeTimeZone.chargeStartTime.hour, data->chargeTimeZone.chargeStartTime.hour, sizeof(openMode.chargeTimeZone.chargeStartTime.hour) - 1);
		strncpy(openMode.chargeTimeZone.chargeStartTime.min, data->chargeTimeZone.chargeStartTime.minute, sizeof(openMode.chargeTimeZone.chargeStartTime.min) - 1);
		strncpy(openMode.chargeTimeZone.chargeStopTime.hour, data->chargeTimeZone.chargeStopTime.hour, sizeof(openMode.chargeTimeZone.chargeStopTime.hour) - 1);
		strncpy(openMode.chargeTimeZone.chargeStopTime.min, data->chargeTimeZone.chargeStopTime.minute, sizeof(openMode.chargeTimeZone.chargeStopTime.min) - 1);
		strncpy(openMode.chargeTimeZone.dischargeStartTime.hour, data->chargeTimeZone.dischargeStartTime.hour, sizeof(openMode.chargeTimeZone.dischargeStartTime.hour) - 1);
		strncpy(openMode.chargeTimeZone.dischargeStartTime.min, data->chargeTimeZone.dischargeStartTime.minute, sizeof(openMode.chargeTimeZone.dischargeStartTime.min) - 1);
		strncpy(openMode.chargeTimeZone.dischargeStopTime.hour, data->chargeTimeZone.dischargeStopTime.hour, sizeof(openMode.chargeTimeZone.dischargeStopTime.hour) - 1);
		strncpy(openMode.chargeTimeZone.dischargeStopTime.min, data->chargeTimeZone.dischargeStopTime.minute, sizeof(openMode.chargeTimeZone.dischargeStopTime.min) - 1);
	}
	ERMC_NotifyCmd24DataInfo(&openMode);
}


void EMDTR_NotifyInvInitCommFin()
{
	ERMC_NotifyInvInitCommFin();
}
