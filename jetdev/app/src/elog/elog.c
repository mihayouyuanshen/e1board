/****************************************************************************/
/*	Description		:ログ管理ミドル											*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Jiang.Z											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/05/10												*/
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
#include "elog_g.h"
#include <task_g.h>
#include <lfw_g.h>
#include <ewdt.h>

#include "elog_utl_g.h"
#include "elog_val_g.h"
#include "elog_cnf_g.h"
#include "elog_fnc_g.h"
#include "elog_thr_g.h"
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/

/* グローバル変数 */
pthread_mutex_t s_elogSysCmdMutx;			/* system()実行排他用 */


/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/10 : DHC Jiang.Z : 新規作成					*/
/*==========================================================================*/
void ELOG_Init(void)
{
	DEBUG_TRCIN();
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/10 : DHC Jiang.Z : 新規作成					*/
/*==========================================================================*/
void ELOG_MsgProc(FrmMsgHead_t const *msg)
{
	ElogUsbIs_t isUsbRet = ELOG_USBIS_NOENT;
	
	DEBUG_TRCIN();
	switch(msg->attr)
	{
		case FRM_MSG_EVENT:
			switch(msg->msgId)
			{
				case LFW_MSGID_WDT_REQ:
					/* TODO */
					break;
				case LFW_MSGID_WDT_RES:
					/* TODO */
					break;
				case LFW_MSGID_TIMER_TIMEOUT:
					elog_ProcTimeout(msg);
					break;
				default:
					break;
			}
		break;
		case FRM_MSG_ASYNC:
			switch(msg->msgId)
			{
				case ELOG_MSGID_START:
					elog_Start();
					ESYS_NotifyStartResult(ELOG_SPORT, 
											MOD_ACTION_RESULT_SUCCESS);
					break;
				case ELOG_LOG:
					ELOG_FncLogOutput((ElogFncLog_t *)msg);
					break;
				case ELOG_ASYNC_OPERATION:
					ELOG_FncOperation((ElogFncOpt_t *)msg);
					break;
				case ELOG_ASYNC_ERR_EVENT_LOG:
					ELOG_FncSaveErrorEventLog((ElogFncErrEvent_t *)msg);
					break;
				case ELOG_ASYNC_ACTION_LOG:
					ELOG_FncSaveActionLog((ElogFncAction_t *)msg);
					break;
				case ELOG_ASYNC_SOH_INFO_LOG:
					ELOG_FncSOHpercentage((ElogFncSOH_t *)msg);
					break;
				case ELOG_ASYNC_PWR_VOLT_HIST_LOG:
					ELOG_FncPowerVoltHist((ElogFncVoltHist_t *)msg);
					break;
				case ELOG_ASYNC_HEMS_HISTORY_LOG:
					ELOG_FncHEMSPowerEdge((ElogFncHemsHistory_t *)msg);
					break;
				case ELOG_ASYNC_TOTAL_PWR:
					ELOG_FncPowerTotal((ElogFncPowerTotal_t *)msg);
					break;
				case ELOG_ASYNC_OUTPUTCTRL:
					ELOG_FncOutputCtrl((ElogFncOutPutCtrl_t*)msg);
					break;
				case ELOG_ASYNC_PWR_DEAL_PWR:
					ELOG_FncSavePowerDealLog((ElogPowerDeal_t *)msg);
					break;
				case ELOG_ASYNC_ACCUMULATOR_PWR:
					ELOG_FncSaveAccumulatorLog((ElogAccumulator_t *)msg);
					break;
				case ELOG_ASYNC_VEHICLE_PWR:
					ELOG_FncSaveVehicleLog((ElogVehicle_t *)msg);
					break;
				case ELOG_ASYNC_DOWNLOAD_LOG:
					ELOG_UpdateResult((ElogFncDownload_t*)msg);
					break;
				case ELOG_ASYNC_RUN_INFOR_CMD65_LOG:
					ELOG_RunInformation65((ElogFncRunInfor65_t*)msg);
					break;
					case ELOG_ASYNC_RUN_INFOR_CMD75_LOG:
					ELOG_RunInformation75((ElogFncRunInfor75_t*)msg);
					break;
				case ELOG_ASYNC_SAVESDPLOG:
					ELOG_FncSaveSdpLog((ElogFncSdp_t *)msg);
					break;
				case ELOG_ASYNC_PRESSSDP:
					ELOG_FncCompressSdpLog((ElogFncSdpComp_t*)msg);
					break;
				case ELOG_ASYNC_PRESS_ACTION:
					ELOG_FncCompressActionLog((ElogFncActionComp_t*)msg);
					break;
				case ELOG_ASYNC_GETLOGLIST:
					ELOG_FncGetLogList((ElogFncGetLogList_t*)msg);
					break;
				case ELOG_ASYNC_DELETELOG:
					ELOG_FncDeleteLog((ElogFncDeleteLog_t*)msg);
					break;
				case ELOG_ASYNC_DELETEFILE:
				ELOG_FncDeleteFile((ElogFncDeleteFile_t*)msg);
					break;
				case ELOG_ASYNC_CANCELTHR:
				ELOG_PthreadCancel((ElogThrCancel_t*)msg);
					break;
			}
		break;
		default:
			DLOG_Info("--unknown attr : %d--\n",msg->attr);
		break;
	}
	DEBUG_TRCOUT();
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
/*	history			:2023/05/10 : DHC Jiang.Z : 新規作成					*/
/*==========================================================================*/
slong ELOG_Start(slong fact)
{
	ELOG_Start_t msg;
	DEBUG_TRCIN();

	msg.header.msgId = ELOG_MSGID_START;
	msg.Fact = fact;
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(msg), &msg);

	DEBUG_TRCOUT();
	return 0;
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
/*	history			:2023/05/10 : DHC Jiang.Z : 新規作成					*/
/*==========================================================================*/
slong ELOG_Stop(void)
{
	DEBUG_TRCIN();
	ESYS_NotifyStopResult(ELOG_SPORT, MOD_ACTION_RESULT_SUCCESS);
	DEBUG_TRCOUT();
	return 0;
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
/*	history			:2023/05/10 : DHC Jiang.Z : 新規作成					*/
/*==========================================================================*/
void ELOG_Fin(void)
{
	DEBUG_TRCIN();
	DEBUG_TRCOUT();
}



/* End Of File */