/******************************************************************************/
/*    Description    :時刻管理ミドル                                           */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC Gu.CM                                              */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/04/03                                              */
/*----------------------------------------------------------------------------*/
/*    Module Name    :                                                        */
/*----------------------------------------------------------------------------*/
/*    Note           :                                                        */
/*----------------------------------------------------------------------------*/
/*    Copyright      :(c) 2020 NICHICON CORPORATION                           */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Include Header File                                                        */
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "common_g.h"
#include "ewdt.h"
//#include "esch.h"
#include "etim_g.h"
#include "etim_msg.h"
#include "etim_ntp.h"
#include "etim_rtc.h"
#include "etim_utl.h"
#include "task_g.h"
#include "etim_serv.h"
#include "dlog_g.h"
#include "esys_g.h"

/*----------------------------------------------------------------------------*/
/* 公開変数定義                                                                */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開変数定義                                                              */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 公開関数定義                                                                */
/*----------------------------------------------------------------------------*/
/*============================================================================*/
/*    Description    :TIM模块初始化                                            */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/15  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void ETIM_Init(void)
{
	DEBUG_TRCIN();

	ETIM_SetSyncState(ETIM_STATE_NTP_SYNC_INIT);
	ESYS_NotifyStartResult(ELED_SPORT, MOD_ACTION_RESULT_SUCCESS);

	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :TIM模块消息处理                                          */
/*----------------------------------------------------------------------------*/
/*    param          :FrmMsgHead_t const *msg 消息                             */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/15  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void ETIM_MsgProc(FrmMsgHead_t const *msg)
{
	sint ret;
	sint resFlg = 1;
	EtimRetCodeE_t returnCode;
	EtimMsgRspData_t rsp;

	DEBUG_TRCIN();

	switch(msg->attr)
	{
	case LFW_CALL_SYNC:
		switch (msg->msgId)
		{
		case ETIM_MSGID_SETTIME_REQ:
		{
			EtimSetTime_t const *setTimeData = (EtimSetTime_t const *)msg;
			ret = ETIM_SetTime(&setTimeData->time);
			if (ETIM_OK == ret)
			{
				/* eschへ時刻同期イベントを送信 任务管理*/
				//esch_async_notifyTimeSync();
				/* RTCへ反映 */
				ret = ETIM_SetRtc();
				if (ret != ETIM_OK)
				{
					DLOG_Error("etim_rtcSet() error.\n");
				}
			}
			break;
		}
		case ETIM_MSGID_PING_NTP:
			ret = ETIM_PingNtpsvr((sint)msg->destModId);
			if (ETIM_OK == ret)
			{
				resFlg = 0;		/* 起動成功なら応答は後でスレッドから返信する */
			}
			break;
		default:
			DLOG_Error("unknown sync functionId = %ld\n", msg->msgId);
			resFlg = 0;
			break;
		}

		if(1 == resFlg)
		{
			memset(&rsp, 0x00, sizeof(rsp));
			returnCode = (ret == ETIM_OK) ? ETIM_OK : ETIM_E_OTHER;
			rsp.header.retCode = (slong)returnCode;
			ret = FRM_CallRet(msg->srcModId, sizeof(rsp), (void *)&rsp);
			if(-1 == ret)
			{
				DLOG_Error("response FRM_CallRet() error.(functionId=%ld)\n", msg->msgId);
			}
		}
		break;
	case LFW_MSG_ASYNC:
		switch (msg->msgId)
		{
		case ETIM_MSGID_STARTSYNC_REQ:
		{
			EtimStartNtpSync_t const *ntpSysnMsg = (EtimStartNtpSync_t const *)msg;
			ret = ETIM_StartNtpSync((EtimNtpInfo_t const *)&ntpSysnMsg->ntpInfo);
			break;
		}
		case ETIM_MSGID_START:
		{
			EtimStart_t const *startMsg = (EtimStart_t const *)msg;
			ETIM_ProcStart(startMsg->fact);
			break;
		}
		case ETIM_MSGID_NTP_SYNC_TREG:
		{
			NtpStateMsg_t const *ntpStateMsg = (NtpStateMsg_t const *)msg;
			ETIM_NtpSyncStateProc(ntpStateMsg->trigger);
			break;
		}
		default:
			DLOG_Error("unknown async functionId = %ld\n", msg->msgId);
			break;
		}
		break;
	case LFW_MSG_EVENT:
		switch (msg->msgId)
		{
		case EWDT_EVT_WDT_REQ:
			ETIM_WdtReq();
			//ewdt_async_sendWdtRes(ETIM_WDT_TIMEOUT);
			break;
		case EWDT_EVT_TERM_REQ:
			ETIM_TemrReq();
			//ewdt_async_sendTermRes(msg->term_req.type);
			break;
		case LFW_MSGID_TIMER_TIMEOUT:
		{
			FrmTimeout_t const *timeoutMsg = (FrmTimeout_t const *)msg;
			if(ETIM_NTP_SYNC_TIMER_ID == timeoutMsg->timerId)
			{
				Etim_NtpSyncTimer();
				ETIM_NtpSyncStateProc(ETIM_STATE_NTP_SYNC_INIT);
			}
			else if(ETIM_RETRY_TIMER_ID == timeoutMsg->timerId)
			{
				ETIM_NtpSyncStateProc(ETIM_STATE_NTP_SYNC_INIT);
			}
			else if(ETIM_NTP_SYNC_JUDGE_TIMER_ID == timeoutMsg->timerId)
			{
				ETIM_NtpSyncStateProc(ETIM_STATE_NTP_SYNC_NG);
			}
			break;
		}
		default:
			DLOG_Error("unknown event function = %ld\n", msg->msgId);
			break;
		}
		break;
	default:
		DLOG_Error("Message Error\n");
		break;
	}

	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :TIM模块资源释放                                          */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/true:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/16  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
void ETIM_Fin(void)
{
	/* TODO */
}

/*============================================================================*/
/*    Description    : 起動時処理                                              */
/*----------------------------------------------------------------------------*/
/*    param          : wdtFlag WDTからの再起動フラグ(1の場合再起動)        */
/*----------------------------------------------------------------------------*/
/*    return         : slong                                                  */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/13  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
slong ETIM_Start(slong fact)
{
	EtimStart_t msg;

	DEBUG_TRCIN();

	msg.header.msgId = ETIM_MSGID_START;
	msg.fact = (sint)fact;
	FRM_MsgSendAsync(ETIM_SPORT, sizeof(msg), &msg);

	DEBUG_TRCOUT();

	return 0;
}

/*============================================================================*/
/*    Description    :TIM模块停止                                              */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :slong                                                   */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/16  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
slong ETIM_Stop(void)
{

	ESYS_NotifyStopResult(ELED_SPORT, MOD_ACTION_RESULT_SUCCESS);

	return 0;
}

/*----------------------------------------------------------------------------*/
/* 非公開関数定義                                                              */
/*----------------------------------------------------------------------------*/

/* End Of File */
