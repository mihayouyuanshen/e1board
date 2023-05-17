/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jingtong.li										*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/23												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include header File														*/
/*--------------------------------------------------------------------------*/
#include "enet_cmn_g.h"
#include "enet_serv_g.h"
#include "enet_thread_g.h"
#include "enet_file_g.h"
#include "esys_g.h"
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
/*	Description		:プロセスを初期化する									*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:無い													*/
/*--------------------------------------------------------------------------*/
/*	detail			:プロセスを初期化する									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void ENET_Init(void)
{
	DEBUG_TRCIN();
	/*TODO:*/
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:無い													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void ENET_Fin(void)
{
	DEBUG_TRCIN();
	/*TODO:*/
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:無い													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void ENET_MsgProc(FrmMsgHead_t const *msg)
{
	DEBUG_TRCIN();

	switch (msg->attr) 
	{
		case FRM_CALL_SYNC:
			/* header.MsgIDで各処理に分岐 */
			switch (msg->msgId)
			{
				/* ネットワーク種別 設定 */
				case ENET_SYNC_SET_NETWORKTYPE:
				{
					ENET_SetNetworkTypeLfwRet((EnetFncNetworkType_t*) msg);
					break;
				}
				/* IPv4 設定 */
				case ENET_SYNC_SET_IPADDR4:
				{
					ENET_SetIpAddr4LfwRet((EnetFncIpv4_t*)msg);
					break;
				}
				/* IPv6 設定 */
				case ENET_SYNC_SET_IPADDR6:
				{
					ENET_SetIpAddr6LfwRet((EnetFncIpv6_t*)msg);
					break;
				}
				/* プロキシ 設定 */
				case ENET_SYNC_SET_PROXY:
				{
					ENET_SetProxyLfwRet((EnetFncProxy_t*)msg);
					break;
				}
				/* 無線接続 設定反映 */
				case ENET_SYNC_SET_WLANSETTING:
				{
					ENET_SetWlanSettingLfwRet((EnetFncWlan_t*)msg);
					break;
				}
				/* 無線LAN on/off */
				case ENET_SYNC_SET_WLANUP:
				{
					ENET_SetWlanUpLfwRet((EnetFncBool_t*)msg);
					break;
				}
				/* 設定変化イベント通知 登録 */
				case ENET_SYNC_SET_CHGEVNTLISTENER:
				{
					ENET_SetChgEvntListenerLfwRet((EnetFncListenerPort_t*)msg);
					break;
				}
			}
			break;
		case FRM_MSG_ASYNC:
			/* header.MsgIDで各処理に分岐 */
			switch (msg->msgId)
			{
				case ENET_MSGID_START:
				{
					EnetStart_t const* procStart = (EnetStart_t const*)msg;
					ENET_ProcStart(procStart->fact);
					break;
				}
				/* ESSIDスキャン */
				case ENET_ASYNC_SCAN_ESSID:
				{
					ENET_ScanEssidLfwASync((EnetFnc00_t*) msg);
					break;
				}
				/* ルータ接続確認 */
				case ENET_ASYNC_CHK_ROUTERCONNECTION:
				{
					ENET_ChkRouterConnectionLfwASync((EnetFnc00_t*) msg);
					break;
				}
				case ENET_ASYNC_RESTARTWIFI:
				{
					ENET_ResetApDevice();
					break;
				}
				case ENET_THREAD_CANCEL:
				{
					EnetThdCancel_t const* thdmsg = (EnetThdCancel_t const*)msg;
					FRM_PthreadCancel(thdmsg->thread);
					break;
				}
				default:
					break;
			}
			break;
		case FRM_MSG_EVENT:
			switch (msg->msgId)
			{
#if 0
				/* WDT応答要求 */
				case LFW_MSGID_WDT_REQ:
					/* 生存応答 */
					EWDT_AsyncSendWdtRes(ENET_WDT_TIMEOUT);
					break;
				/* 終了要求 */
				case LFW_MSGID_TERM_REQ:
				{
					EwdtEvtTermReq_t const *wdtMsg = (EwdtEvtTermReq_t const *)msg;	/*TODO:*/
					FRM_TimerCancelOneShot(ENET_ONE_SHOT_ID_IP_WATCH);
					FRM_TimerCancelOneShot(ENET_ONE_SHOT_ID_SEND_PING);
					/* 終了応答 */
					EWDT_AsyncSendTermRes(wdtMsg->Type);
					break;
				}
#endif
				case LFW_MSGID_TIMER_TIMEOUT:
				{
					FrmTimeout_t const *timeoutMsg = (FrmTimeout_t const *)msg;
					switch (timeoutMsg->timerId)
					{
						case ENET_ONE_SHOT_ID_IP_WATCH:
							/* IP監視スレッド起動 */
							ENET_WatchIPThread();
							break;
						case ENET_ONE_SHOT_ID_SEND_PING:
							/* ルータ疎通確認スレッド起動 */
							ENET_SendPingThread(0);
							break;
						case ENET_ONE_SHOT_ID_WLANLMT:
							DLOG_Debug("wifi timer out.\n");
							ENET_ExecCmdKillHostapd();
							break;
						default:
							break;
					}
				}
				default:
					break;
			}
			break;
		default:
			break;
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:無い													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_Start(slong fact)
{
	EnetStart_t msg;
	slong msgSize = sizeof(EnetStart_t);
	DEBUG_TRCIN();

	msg.header.msgId = ENET_MSGID_START;
	msg.fact = fact;
	FRM_MsgSendAsync(ENET_SPORT, msgSize, &msg);

	DEBUG_TRCOUT();
	return 0;
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:無い													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_Stop(void)
{
	DEBUG_TRCIN();
	ESYS_NotifyStopResult(ENET_SPORT, MOD_ACTION_RESULT_SUCCESS);
	DEBUG_TRCOUT();
}

/*End Of File*/
