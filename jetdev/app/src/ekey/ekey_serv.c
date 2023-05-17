/****************************************************************************/
/*	Description		:key業務機能											*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Cao.HZ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/20												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:EKEY													*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include header File														*/
/*--------------------------------------------------------------------------*/
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include "frm_g.h"
#include "task_g.h"
#include "dlog_g.h"
#include "ekey_cmn_g.h"
#include "ekey_serv_g.h"
#include "ekey_serv.h"
#include "dgpio_g.h"
#include "esys_g.h"
/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
EKEY_BtnContext_t s_ekeyBtnContextTbl[EKEY_BTN_MAX] = {
	{EKEY_BTN_SW1,		EKEY_TIMERID_SW1,	DGPIO_PORT_KEY_SW1,		-1,	EKEY_STS_IDLE},
	{EKEY_BTN_SW2,		EKEY_TIMERID_SW2,	DGPIO_PORT_KEY_SW2,		-1,	EKEY_STS_IDLE},
	{EKEY_PIN_A8,		EKEY_TIMERID_A8,	DGPIO_PORT_KEY_A8,		-1,	EKEY_STS_IDLE},
	{EKEY_PIN_D10,		EKEY_TIMERID_D10,	DGPIO_PORT_KEY_D10,		-1,	EKEY_STS_IDLE},
	{EKEY_PIN_A14,		EKEY_TIMERID_A14,	DGPIO_PORT_KEY_A14,		-1,	EKEY_STS_IDLE},
	{EKEY_PIN_K17,		EKEY_TIMERID_K17,	DGPIO_PORT_KEY_K17,		-1,	EKEY_STS_IDLE}
};

FrmPthread_t s_ekeyTid;		/*スレッド識別変数*/
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
bool_t g_ekeyPollFlg;
/*==========================================================================*/
/*	Description		:サブスレッドを作成する									*/
/*--------------------------------------------------------------------------*/
/*	param			:void													*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:スレッドを作成											*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/21 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void EKEY_CreateSubThread(void)
{
	slong ret;
	DEBUG_TRCIN();
	ret = FRM_PthreadCreate(&s_ekeyTid, NULL, ekey_SubThreadMainLoop, NULL);
	if(0 == ret)
	{
		DLOG_Error("FRM_PthreadCreate(ekey_SubThreadMainLoop)\n");
	}
	else
	{
		/* 動作なし*/
	}
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:ボタン情報を送信する									*/
/*--------------------------------------------------------------------------*/
/*	param			:EKEY_Btn_t btn, DGPIO_Val_t gpioVal					*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:ボタンの状態を送信										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/21 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
static void ekey_SendBtnMsg(EKEY_Btn_t btn, DGPIO_Val_t gpioVal)
{
	EKEY_KeyOnOff_t msg;

	DEBUG_TRCIN();
	/* パラメータセット */
	msg.btn = btn;
	msg.gpioVal = gpioVal;
	msg.header.msgId = EKEY_MSGID_KEY_ONOFF;

	/* イベント通知登録プロセスへ通知 */
	FRM_MsgSendAsync(EKEY_SPORT, sizeof(msg), &msg);
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:ボタンOn/Offを処理										*/
/*--------------------------------------------------------------------------*/
/*	param			:FrmMsgHead_t *msg										*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:キー動作を処理											*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/21 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void EKEY_ProcKeyOnOff(FrmMsgHead_t const *msg)
{
	EKEY_KeyOnOff_t const *keyOnOffMsg = (EKEY_KeyOnOff_t const *)msg;
	EKEY_Trg_t trg = EKEY_TRG_KEY_PRESS;
	EKEY_BtnContext_t *keyContext = NULL;
	ulong i = 0u;
	bool_t findFlag = FALSE;

	DEBUG_TRCIN();
	if (DGPIO_VAL_HI == keyOnOffMsg->gpioVal)
	{
		trg = EKEY_TRG_KEY_RELEASE;
	}
	else
	{
		/* 動作なし*/
	}
	while((i < EKEY_BTN_MAX) && (!findFlag))
	{
		if (s_ekeyBtnContextTbl[i].btn == keyOnOffMsg->btn)
		{
			keyContext = s_ekeyBtnContextTbl + i;
			findFlag = TRUE;
		}
		else
		{
			/* 動作なし*/
		}
		i++;
	}
	ekey_RunStateMachine(keyContext, trg);
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:ボタンのタイムアウト処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:FrmMsgHead_t *msg										*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:キーを押してタイムアウト処理							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/21 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void EKEY_ProcKeyTimeout(FrmMsgHead_t const *msg)
{
	FrmTimeout_t const *timeoutMsg = (FrmTimeout_t const *)msg;
	EKEY_Trg_t trg = EKEY_TRG_TIMEOUT;
	EKEY_BtnContext_t *keyContext = NULL;
	ulong i = 0u;
	bool_t findFlag = FALSE;

	DEBUG_TRCIN();
	while((i < EKEY_BTN_MAX) && (!findFlag))
	{
		if (s_ekeyBtnContextTbl[i].timerId == timeoutMsg->timerId)
		{
			keyContext = s_ekeyBtnContextTbl + i;
			findFlag = TRUE;
		}
		else
		{
			/* 動作なし*/
		}
		i++;
	}
	ekey_RunStateMachine(keyContext, trg);
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:ボタンキャンセル処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:FrmMsgHead_t *msg										*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:ボタンキャンセル処理									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/21 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void EKEY_ProcKeyCancel(FrmMsgHead_t const *msg)
{
	EKEY_KeyCancel_t const *keyCancelMsg = (EKEY_KeyCancel_t const *)msg;
	EKEY_Trg_t trg = EKEY_TRG_CANCEL;
	EKEY_BtnContext_t *keyContext = NULL;
	ulong i = 0u;
	bool_t findFlag = FALSE;

	DEBUG_TRCIN();
	while((i < EKEY_BTN_MAX) && (!findFlag))
	{
		if (s_ekeyBtnContextTbl[i].btn == keyCancelMsg->btn)
		{
			keyContext = s_ekeyBtnContextTbl + i;
			findFlag = TRUE;
		}
		else
		{
			/* 動作なし*/
		}
		i++;
	}
	ekey_RunStateMachine(keyContext, trg);
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:ボタン状態機											*/
/*--------------------------------------------------------------------------*/
/*	param			:EKEY_BtnContext_t *keyContext, EKEY_Trg_t trg			*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:ボタン状態機											*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/21 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
static void ekey_RunStateMachine(EKEY_BtnContext_t *keyContext, EKEY_Trg_t trg)
{
	DEBUG_TRCIN();
	if (NULL != keyContext)
	{
		switch(keyContext->state)
		{
		case EKEY_STS_IDLE:
			switch(trg)
			{
			case EKEY_TRG_KEY_PRESS:
				switch (keyContext->btn)
				{
				case EKEY_BTN_SW1:
				case EKEY_BTN_SW2:
					/* start timer */
					FRM_TimerSetOneShot(keyContext->timerId, 3000, NULL);
					break;
				default:
					/* EKEY_BTNEVENT_PRESS */
					ekey_CallBack(keyContext->btn, EKEY_BTNEVENT_PRESS);
					break;
				}
				keyContext->state = EKEY_STS_PRESS;
				break;
			default:
				break;
			}
			break;
		case EKEY_STS_PRESS:
			switch(trg)
			{
			case EKEY_TRG_KEY_RELEASE:
				/* EKEY_BTNEVENT_RELEASE */
				ekey_CallBack(keyContext->btn, EKEY_BTNEVENT_RELEASE);
				/* kill timer */
				FRM_TimerCancelOneShot(keyContext->timerId);
				keyContext->state = EKEY_STS_IDLE;
				break;
			case EKEY_TRG_TIMEOUT:
				/* EKEY_BTNEVENT_LONG_PRESS */
				keyContext->state = EKEY_STS_LONG_PRESS;
				ekey_CallBack(keyContext->btn, EKEY_BTNEVENT_LONG_PRESS);
				break;
			case EKEY_TRG_CANCEL:
				/* kill timer */
				FRM_TimerCancelOneShot(keyContext->timerId);
				keyContext->state = EKEY_STS_IDLE;
				break;
			default:
				break;
			}
			break;
		case EKEY_STS_LONG_PRESS:
			switch(trg)
			{
			case EKEY_TRG_KEY_RELEASE:
				/* EKEY_BTNEVENT_LONG_RELEASE */
				ekey_CallBack(keyContext->btn, EKEY_BTNEVENT_LONG_RELEASE);
				keyContext->state = EKEY_STS_IDLE;
				break;
			case EKEY_TRG_CANCEL:
				keyContext->state = EKEY_STS_IDLE;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
	else
	{
		DLOG_Error("KEY not found.\n");
	}
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:ボタンの状態を確認するサブプロセス						*/
/*--------------------------------------------------------------------------*/
/*	param			:void* arg												*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:ボタンの状態を確認するサブプロセス						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/21 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
static void* ekey_SubThreadMainLoop(void* arg)
{
	ulong i;
	DGPIO_Val_t gpioVal;
	struct pollfd pollFds[EKEY_BTN_MAX];
	sint pollRet;
	slong readRet;

	DEBUG_TRCIN();
	g_ekeyPollFlg = TRUE;
	/* open port */
	memset(pollFds, 0, sizeof(pollFds));
	for(i = 0u; i < EKEY_BTN_MAX; i++)
	{
		s_ekeyBtnContextTbl[i].gpioFd = DGPIO_Open(s_ekeyBtnContextTbl[i].gpioPort);
		pollFds[i].fd = s_ekeyBtnContextTbl[i].gpioFd;
		pollFds[i].events = (POLLPRI);
	}

	while(g_ekeyPollFlg)
	{
		pollRet = poll(pollFds, EKEY_BTN_MAX, -1);

		if(pollRet > 0)
		{
			for(i = 0u; i < EKEY_BTN_MAX; i++)
			{
				if(pollFds[i].revents & (POLLPRI))
				{
					readRet = DGPIO_Read(s_ekeyBtnContextTbl[i].gpioFd, &gpioVal);
					if (0 == readRet)
					{
						ekey_SendBtnMsg(s_ekeyBtnContextTbl[i].btn, gpioVal);
					}
					else
					{
						DLOG_Error("DGPIO_Read failed\n");
					}
					lseek(s_ekeyBtnContextTbl[i].gpioFd, 0, SEEK_SET);
				}
				else
				{
					/*動作なし*/
				}
			}
		}
		else
		{
			/*動作なし*/
		}
	}

	/* close port */
	for(i = 0u; i < EKEY_BTN_MAX; i++)
	{
		DGPIO_Close(s_ekeyBtnContextTbl[i].gpioFd, s_ekeyBtnContextTbl[i].gpioPort);
		s_ekeyBtnContextTbl[i].gpioFd = -1;
	}

	ekey_StopPollLoop(s_ekeyTid);
	DEBUG_TRCOUT();
	return NULL;
}
/*==========================================================================*/
/*	Description		:他のmodに通知する										*/
/*--------------------------------------------------------------------------*/
/*	param			:EKEY_Btn_t key, EKEY_BtnEvent_t event					*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:他のmodに通知する										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void ekey_CallBack(EKEY_Btn_t key, EKEY_BtnEvent_t event)
{
	ESYS_NotifyKeyEvent(key, event);
}

/*==========================================================================*/
/*	Description		:pollサイクルを停止										*/
/*--------------------------------------------------------------------------*/
/*	param			:FrmPthread_t threadId									*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:pollサイクルを停止										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/26 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void ekey_StopPollLoop(FrmPthread_t threadId)
{
	EKEY_Stop_t ekeyStopMsg;
	DEBUG_TRCIN();
	ekeyStopMsg.header.msgId = EKEY_MSGID_STOP;
	ekeyStopMsg.tid = threadId;

	FRM_MsgSendAsync(EKEY_SPORT, sizeof(ekeyStopMsg), &ekeyStopMsg);
	DEBUG_TRCOUT();
}

/*End Of File*/
