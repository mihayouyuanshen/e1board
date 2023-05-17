/****************************************************************************/
/*	Description		:消息框架												*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC yufeng.yang										*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/22												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:frm_msg												*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "frm_msg_g.h"
#include "frm_msg.h"
#include "frm_g.h"
#include "dlog_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
const ModDesc_t s_frmMsgModDescMain = {
	0u, 0u, NULL, NULL, NULL
};

ModDescEx_t s_frmMsgModDescExMain = {
	&s_frmMsgModDescMain,
	0,
	0,
	NULL,
	0u,
	1u,
	{},
#ifdef ENABLE_STATISTIC
	0u,
	0u
#endif
};

ModDescEx_t *s_frmMsgModDescExTbl;
ushort s_frmMsgModDescExTblSize;
pthread_key_t s_frmMsgThPrivKey;
ulong s_frmMsgWorking;
PthreadEntity_t s_frmMsgPthreadPool[PTHREAD_POOL_MAX_SIZE];
pthread_mutex_t s_frmMsgPthreadPoolMutex;

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description		:初始化消息框架											*/
/*--------------------------------------------------------------------------*/
/*	param			:ModDesc_t const *modDescTbl:模块描述表					*/
/*--------------------------------------------------------------------------*/
/*	param			:ushort modDescTblSize:模块描述表大小					*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	-1:异常											*/
/*--------------------------------------------------------------------------*/
/*	detail			:根据模块描述表生成消息框架用模块管理表						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
slong FRM_MsgInit(ModDesc_t const *modDescTbl, ushort modDescTblSize)
{
	ushort i;
	ulong tmpSize;

	DEBUG_TRCIN();
	/* 引数チェック */
	if ((NULL != s_frmMsgModDescExTbl) || (NULL == modDescTbl))
	{
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		s_frmMsgModDescExTbl = malloc(modDescTblSize * sizeof(ModDescEx_t));
		s_frmMsgModDescExTblSize = modDescTblSize;

		for (i = 0u; i < s_frmMsgModDescExTblSize; ++i)
		{
			s_frmMsgModDescExTbl[i].modDesc = modDescTbl + i;
			s_frmMsgModDescExTbl[i].msqId = 0;
			s_frmMsgModDescExTbl[i].tid = (pthread_t)0;
			/* 初始化接收缓冲区大小 */
			tmpSize = s_frmMsgModDescExTbl[i].modDesc->rcvQMaxSize;
			if (0 == tmpSize)
			{
				tmpSize = MSG_MAX_SIZE_DEF;
			}
			else
			{
				/* 処理無し */
			}
			s_frmMsgModDescExTbl[i].rcvBufSize = tmpSize;
			s_frmMsgModDescExTbl[i].rcvBuf = malloc(tmpSize);
			s_frmMsgModDescExTbl[i].isWorking = 0u;
			pthread_mutex_init(&(s_frmMsgModDescExTbl[i].syncCallLock), NULL);
			/* 接收缓冲区用量统计 */
			#ifdef ENABLE_STATISTIC
			s_frmMsgModDescExTbl[i].rcvQMaxUse = 0u;
			s_frmMsgModDescExTbl[i].sndQMaxUse = 0u;
			#endif
		}
		/* 排他制御初始化 */
		pthread_mutex_init(&s_frmMsgPthreadPoolMutex, NULL);

		/* 创建线程私有数据Key */
		pthread_key_create(&s_frmMsgThPrivKey, NULL);
		/* 设定线程主线程私有数据 */
		pthread_setspecific(s_frmMsgThPrivKey, &s_frmMsgModDescExMain);
	}

	DEBUG_TRCOUT();
	return FRM_SUCCESS;
}

/*==========================================================================*/
/*	Description		:运行消息框架											*/
/*--------------------------------------------------------------------------*/
/*	param			:void													*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	-1:异常									*/
/*--------------------------------------------------------------------------*/
/*	detail			:为模块创建消息队列、线程等资源，进行模块初始化、启动。		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
slong FRM_MsgLoop(void)
{
	ushort i;
	DEBUG_TRCIN();
	/* 为各模块创建消息队列 */
	for (i = 0u; i < s_frmMsgModDescExTblSize; ++i)
	{
		s_frmMsgModDescExTbl[i].msqId = 
			msgget((key_t)(s_frmMsgModDescExTbl[i].modDesc->modId),
					0666 | IPC_CREAT);
		if(-1 == s_frmMsgModDescExTbl[i].msqId)
		{
			DLOG_Debug("msgget failure");
		}
		else
		{
			/* 処理無し */
		}
	}

	/* 各模块初始化 */
	for (i = 0u; i < s_frmMsgModDescExTblSize; ++i)
	{
		if (s_frmMsgModDescExTbl[i].modDesc->modInit)
		{
			s_frmMsgModDescExTbl[i].modDesc->modInit();
		}
		else
		{
			/* 処理無し */
		}
	}

	/* 为各模块创建线程 */
	for (i = 0u; i < s_frmMsgModDescExTblSize; ++i)
	{
		pthread_create(&(s_frmMsgModDescExTbl[i].tid), NULL,
						frm_ModThreadMain, s_frmMsgModDescExTbl + i);
	}

	s_frmMsgWorking = 1u;
	while (s_frmMsgWorking)
	{
		usleep(100 * 1000); /* 100ms */
	}

	/* 取消所有线程 */
	for (i = 0u; i < s_frmMsgModDescExTblSize; ++i)
	{
		pthread_cancel(s_frmMsgModDescExTbl[i].tid);
		pthread_join(s_frmMsgModDescExTbl[i].tid, NULL);
	}

	/* 删除消息队列 */
	for (i = 0u; i < s_frmMsgModDescExTblSize; ++i)
	{
		msgctl(s_frmMsgModDescExTbl[i].msqId, IPC_RMID, 0);
	}

	/* 释放内存资源 */
	if (NULL != s_frmMsgModDescExTbl)
	{
		for (i = 0u; i < s_frmMsgModDescExTblSize; ++i)
		{
			if (NULL != s_frmMsgModDescExTbl[i].rcvBuf)
			{
				free(s_frmMsgModDescExTbl[i].rcvBuf);
				s_frmMsgModDescExTbl[i].rcvBuf = NULL;
			}
			else
			{
				/* 処理無し */
			}
		}
		free(s_frmMsgModDescExTbl);
		s_frmMsgModDescExTbl = NULL;
	}
	s_frmMsgModDescExTblSize = 0u;
	DEBUG_TRCOUT();
	return FRM_SUCCESS;
}

/*==========================================================================*/
/*	Description		:停止消息框架											*/
/*--------------------------------------------------------------------------*/
/*	param			:void													*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	-1:异常											*/
/*--------------------------------------------------------------------------*/
/*	detail			:停止消息框架运行										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
slong FRM_MsgFin(void)
{
	DEBUG_TRCIN();
	/* 停止消息框架 */
	s_frmMsgWorking = 0u;
	DEBUG_TRCOUT();
	return FRM_SUCCESS;
}

/*==========================================================================*/
/*	Description		:重启模块												*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong modId:模块ID									*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	-1:异常									*/
/*--------------------------------------------------------------------------*/
/*	detail			:WDT监测模块异常后，重启模块用							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
slong FRM_MsgModReboot(ulong modId)
{
	slong ret = FRM_ERROR;
	ushort i, targetMod;
	uchar foundMod = 0u;

	for (i = 0u; !foundMod && (i < s_frmMsgModDescExTblSize); ++i)
	{
		if (modId == s_frmMsgModDescExTbl[i].modDesc->modId)
		{
			targetMod = i;
			foundMod = 1u;
		}
		else
		{
			/* 処理無し */
		}
	}

	if (foundMod)
	{
		/* 取消目标模块的线程 */
		pthread_cancel(s_frmMsgModDescExTbl[targetMod].tid);
		pthread_join(s_frmMsgModDescExTbl[targetMod].tid, NULL);

		/* 重启目标线程 */
		pthread_create(&(s_frmMsgModDescExTbl[i].tid), NULL,
						frm_ModThreadMain, s_frmMsgModDescExTbl + targetMod);
		ret = FRM_SUCCESS;
	}
	else
	{
		/* 処理無し */
	}

	return ret;
}

/*==========================================================================*/
/*	Description		:异步消息发送											*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong destModId:目标模块ID								*/
/*--------------------------------------------------------------------------*/
/*	param			:ushort msgSize:消息长度								*/
/*--------------------------------------------------------------------------*/
/*	param			:void *msg:消息											*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	-1:异常											*/
/*--------------------------------------------------------------------------*/
/*	detail			:模块间发送异步消息										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
slong FRM_MsgSendAsync(ulong destModId, ushort msgSize, void *msg)
{
	DEBUG_TRCIN();
	/* 引数チェック */
	if (msg == NULL) {
		DLOG_Error("parameter NULL.");
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}
	DEBUG_TRCOUT();
	return frm_MsgSend(destModId, FRM_MSG_ASYNC, msgSize, msg);
}

/*==========================================================================*/
/*	Description		:异步Event发送											*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong destModId:目标模块ID								*/
/*--------------------------------------------------------------------------*/
/*	param			:ushort msgSize:消息长度								*/
/*--------------------------------------------------------------------------*/
/*	param			:void *msg:消息											*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	-1:异常											*/
/*--------------------------------------------------------------------------*/
/*	detail			:模块间发送异步Event									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
slong FRM_MsgSendEvent(ulong destModId, ushort msgSize, void *msg)
{
	DEBUG_TRCIN();
	/* 引数チェック */
	if (msg == NULL) {
		DLOG_Error("parameter NULL.");
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}
	DEBUG_TRCOUT();
	return frm_MsgSend(destModId, FRM_MSG_EVENT, msgSize, msg);
}

/*==========================================================================*/
/*	Description		:同步调用返回											*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong destModId:目标模块ID								*/
/*--------------------------------------------------------------------------*/
/*	param			:ushort msgSize:消息长度								*/
/*--------------------------------------------------------------------------*/
/*	param			:void *msg:消息											*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	-1:异常											*/
/*--------------------------------------------------------------------------*/
/*	detail			:框架中模块回复同步消息，使同步调用返回						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
slong FRM_CallRet(ulong destModId, ushort msgSize, void *msg)
{
	slong ret = -1;
	sint msqid = 0;
	ModDescEx_t *modDescEx;
	FrmMsgHead_t *msgHead;

	DEBUG_TRCIN();
	/* 引数チェック */
	if (msg == NULL) {
		DLOG_Error("parameter NULL.");
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}
	msqid = msgget((key_t)destModId, 0666 | IPC_CREAT);
	modDescEx = (ModDescEx_t *)pthread_getspecific(s_frmMsgThPrivKey);
	msgHead = (FrmMsgHead_t *)msg;
	msgHead->mType = MSG_TYPE_SSYNC;
	msgHead->srcModId = modDescEx->modDesc->modId;
	msgHead->destModId = destModId;
	msgHead->attr = FRM_CALL_RET;
	msgHead->msgSize = msgSize;
	msgHead->retCode = 0;
	if (msgsnd(msqid, msg, msgSize - 4, 0) == 0)
	{
		ret = 0;
	}
	else
	{
		DLOG_Debug("msgsnd failure");
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:同步调用												*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong destModId:目标模块ID								*/
/*--------------------------------------------------------------------------*/
/*	param			:ushort msgSize:消息长度								*/
/*--------------------------------------------------------------------------*/
/*	param			:void *msg:消息											*/
/*--------------------------------------------------------------------------*/
/*	param			:void *retMsg:回复消息									*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	-1:异常											*/
/*--------------------------------------------------------------------------*/
/*	detail			:框架中模块发送同步消息，并等待回复							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
slong FRM_CallSync(ulong destModId, ushort msgSize, void *msg, void *retMsg)
{
	slong ret = -1;
	sint msqid = 0;
	ProcessMutex_t mutexId;
	ModDescEx_t *modDescEx;
	FrmMsgHead_t *msgHead;

	DEBUG_TRCIN();
	/* 引数チェック */
	if ((msg == NULL) || (retMsg == NULL)) {
		DLOG_Error("parameter NULL.");
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}
	/* 排他制御ロック */
	mutexId = FRM_MutexCreate(destModId);
	FRM_MutexLock(mutexId);
	msqid = msgget((key_t)destModId, 0666 | IPC_CREAT);
	modDescEx = (ModDescEx_t *)pthread_getspecific(s_frmMsgThPrivKey);
	msgHead = (FrmMsgHead_t *)msg;
	msgHead->mType = MSG_TYPE_ASYNC;
	msgHead->srcModId = modDescEx->modDesc->modId;
	msgHead->destModId = destModId;
	msgHead->attr = FRM_CALL_SYNC;
	msgHead->msgSize = msgSize;
	msgHead->retCode = 0;
	if (msgsnd(msqid, msg, msgSize - 4, 0) == 0)
	{
		ret = 0;
	}
	else
	{
		DLOG_Debug("msgsnd failure");
	}

	ret = msgrcv(modDescEx->msqId, retMsg,
					MSG_MAX_SIZE_DEF, MSG_TYPE_SSYNC, 0);

	/* 排他制御アンロック */
	FRM_MutexUnLock(mutexId);
	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:创建线程												*/
/*--------------------------------------------------------------------------*/
/*	param			:FrmPthread_t *thread:线程ID							*/
/*--------------------------------------------------------------------------*/
/*	param			:const pthread_attr_t *attr:线程属性					*/
/*--------------------------------------------------------------------------*/
/*	param			:void *(*startRoutine)(void *):线程处理函数				*/
/*--------------------------------------------------------------------------*/
/*	param			:void *arg:传给处理函数的参数							*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	-1:异常											*/
/*--------------------------------------------------------------------------*/
/*	detail			:为框架中的模块创建线程									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
slong FRM_PthreadCreate(FrmPthread_t *thread,
						const pthread_attr_t *attr,
						void *(*startRoutine)(void *), void *arg)
{
	slong idx, ret = FRM_ERROR;
	slong foundIdx = -1;

	DEBUG_TRCIN();
	/* 引数チェック */
	if ((startRoutine == NULL) || (thread == NULL)) {
		DLOG_Error("parameter NULL.");
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 排他制御ロック */
		pthread_mutex_lock(&s_frmMsgPthreadPoolMutex);
		for (idx = 0; (idx < PTHREAD_POOL_MAX_SIZE) && (-1 == foundIdx); ++idx)
		{
			if (0 == s_frmMsgPthreadPool[idx].isUsing)
			{
				s_frmMsgPthreadPool[idx].isUsing = 1;
				s_frmMsgPthreadPool[idx].startRoutine = startRoutine;
				s_frmMsgPthreadPool[idx].arg = arg;
				s_frmMsgPthreadPool[idx].thPrivData = 
									pthread_getspecific(s_frmMsgThPrivKey);
				pthread_create(&(s_frmMsgPthreadPool[idx].tid),
								attr,
								frm_PthreadEntityMain,
								&s_frmMsgPthreadPool[idx]);
				foundIdx = idx;
				*thread = foundIdx;
				ret = FRM_SUCCESS;
			}
			else
			{
				/* 処理無し */
			}
		}
		/* 排他制御アンロック */
		pthread_mutex_unlock(&s_frmMsgPthreadPoolMutex);
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:取消线程												*/
/*--------------------------------------------------------------------------*/
/*	param			:FrmPthread_t thread:线程ID								*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:取消模块创建的线程										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
void FRM_PthreadCancel(FrmPthread_t thread)
{
	DEBUG_TRCIN();
	if (thread < PTHREAD_POOL_MAX_SIZE)
	{
		/* 取消线程 */
		pthread_cancel(s_frmMsgPthreadPool[thread].tid);
		/* 排他制御ロック */
		pthread_mutex_lock(&s_frmMsgPthreadPoolMutex);
		s_frmMsgPthreadPool[thread].isUsing = 0u;
		/* 排他制御アンロック */
		pthread_mutex_unlock(&s_frmMsgPthreadPoolMutex);
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:获取框架中线程私有数据的Key							*/
/*--------------------------------------------------------------------------*/
/*	param			:void													*/
/*--------------------------------------------------------------------------*/
/*	return			:pthread_key_t:私有数据Key								*/
/*--------------------------------------------------------------------------*/
/*	detail			:获取框架中线程私有数据的Key							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
pthread_key_t FRM_GetPriKey(void)
{
	DEBUG_TRCIN();
	DEBUG_TRCOUT();
	return s_frmMsgThPrivKey;
}

/*==========================================================================*/
/*	Description		:获取本模块ID											*/
/*--------------------------------------------------------------------------*/
/*	param			:void													*/
/*--------------------------------------------------------------------------*/
/*	return			:ulong:模块ID											*/
/*--------------------------------------------------------------------------*/
/*	detail			:获取本模块ID											*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
ulong FRM_MsgSelfModId(void)
{
	ModDescEx_t *modDescEx;

	DEBUG_TRCIN();
	/* 获取线程私有数据 */
	modDescEx = (ModDescEx_t *)pthread_getspecific(s_frmMsgThPrivKey);
	DEBUG_TRCOUT();
	return modDescEx->modDesc->modId;
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:发送消息												*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong destModId:目标模块ID								*/
/*--------------------------------------------------------------------------*/
/*	param			:ushort attr:消息属性									*/
/*--------------------------------------------------------------------------*/
/*	param			:ushort msgSize:消息长度								*/
/*--------------------------------------------------------------------------*/
/*	param			:void *msg:消息											*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	-1:异常											*/
/*--------------------------------------------------------------------------*/
/*	detail			:给目标消息队列发送消息									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Chen.H : 新規作成						*/
/*==========================================================================*/
static slong frm_MsgSend(ulong destModId,
						ushort attr, ushort msgSize, void *msg)
{
	slong ret = -1;
	sint msqid = -1;
	ushort i;
	uchar destFound = 0u;
	ModDescEx_t *modDescEx;
	FrmMsgHead_t *msgHead;

	DEBUG_TRCIN();
	/* 引数チェック */
	if (msg == NULL) {
		DLOG_Error("parameter NULL.");
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}
	for (i = 0u; (!destFound) && (i < s_frmMsgModDescExTblSize); ++i)
	{
		if (destModId == s_frmMsgModDescExTbl[i].modDesc->modId)
		{
			msqid = s_frmMsgModDescExTbl[i].msqId;
			destFound = 1u;
		}
		else
		{
			/* 処理無し */
		}
	}
	if (-1 == msqid)
	{
		/* 如果消息队列不存在，则进行创建 */
		msqid = msgget((key_t)destModId, 0666 | IPC_CREAT);
		DLOG_Debug("call msgget: %d", msqid);
	}
	else
	{
		/* 処理無し */
	}

	if (-1 != msqid)
	{
		modDescEx = (ModDescEx_t *)pthread_getspecific(s_frmMsgThPrivKey);
		msgHead = (FrmMsgHead_t *)msg;
		msgHead->mType = MSG_TYPE_ASYNC;
		msgHead->srcModId = modDescEx->modDesc->modId;
		msgHead->destModId = destModId;
		msgHead->attr = attr;
		msgHead->msgSize = msgSize;
		msgHead->retCode = 0;
		/* 消息发送 */
		if (msgsnd(msqid, msg, msgSize - 4, 0) == 0)
		{
			ret = 0;
		}
		else
		{
			DLOG_Debug("msgsnd failure");
		}
	}
	else
	{
		DLOG_Debug("msgget failure");
	}
	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:模块主线程入口											*/
/*--------------------------------------------------------------------------*/
/*	param			:void *param:模块信息									*/
/*--------------------------------------------------------------------------*/
/*	return			:void *													*/
/*--------------------------------------------------------------------------*/
/*	detail			:模块线程入口函数										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
static void *frm_ModThreadMain(void *param)
{
	DEBUG_TRCIN();
	/* 引数チェック */
	if (param == NULL) {
		DLOG_Error("parameter NULL.");
		DEBUG_TRCOUT();
		return NULL;
	}
	else
	{
		/* 処理無し */
	}
	ModDescEx_t *modDescEx = (ModDescEx_t *)param;
	FrmMsgHead_t *recvBuf = modDescEx->rcvBuf;
	ushort rcvQMaxSize = modDescEx->rcvBufSize;
	sint ret;

	modDescEx->isWorking = 1u;

	pthread_setspecific(s_frmMsgThPrivKey, param);
	while (modDescEx->isWorking)
	{
		ret = msgrcv(modDescEx->msqId,
					recvBuf, rcvQMaxSize, MSG_TYPE_ASYNC, 0);
		if ((-1 != ret) && (modDescEx->modDesc->modMsgProc))
		{
			modDescEx->modDesc->modMsgProc(recvBuf);
		}
		else
		{
			DLOG_Debug("msgrcv failure");
		}
	}

	DEBUG_TRCOUT();
	return NULL;
}

/*==========================================================================*/
/*	Description		:模块创建子线程入口										*/
/*--------------------------------------------------------------------------*/
/*	param			:void *arg:子线程参数信息								*/
/*--------------------------------------------------------------------------*/
/*	return			:void *													*/
/*--------------------------------------------------------------------------*/
/*	detail			:模块创建子线程入口函数									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
static void *frm_PthreadEntityMain(void *arg)
{
	PthreadEntity_t *tmp_pthreadEntity_P = (PthreadEntity_t *)arg;

	DEBUG_TRCIN();
	/* 自スレッドをDetach */
	pthread_detach(pthread_self());
	/* 引数チェック */
	if (arg == NULL) {
		DLOG_Error("parameter NULL.");
		DEBUG_TRCOUT();
		return NULL;
	}
	else
	{
		pthread_setspecific(s_frmMsgThPrivKey,
							tmp_pthreadEntity_P->thPrivData);
		tmp_pthreadEntity_P->startRoutine(tmp_pthreadEntity_P->arg);
	}

	DEBUG_TRCOUT();
	return NULL;
}

/*End Of File*/
