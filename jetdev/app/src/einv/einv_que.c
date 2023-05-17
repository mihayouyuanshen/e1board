/****************************************************************************/
/*	Description		:INV通信ミドル　通信要求キュー処理						*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Dai.P												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/23												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:INV通信												*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "typedef.h"
#include "task_g.h"
#include "frm_g.h"
#include "common_g.h"
#include "dlog_g.h"

#include "einv_g.h"
#include "einv_msg_g.h"
#include "einv_ram_g.h"
#include "einv_que_g.h"
#include "einv_que.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
pthread_cond_t s_einvQueCond;
pthread_mutex_t s_einvQueMutex;
EINV_COMM_QUE_T *s_einvQueHead;
EINV_COMM_QUE_T *s_einvQueEnd;

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description	:通信要求キューの初期化処理									*/
/*--------------------------------------------------------------------------*/
/*	param		:なし														*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	処理結果											*/
/*				:		EINV_OK(0)		正常終了							*/
/*--------------------------------------------------------------------------*/
/*	detail		:通信要求キューの初期化処理を行う。							*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/30 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_InitCommReqQue(void)
{
	slong ret = EINV_OK;

	DEBUG_TRCIN();

	/* 条件変数の初期化 */
	pthread_cond_init(&s_einvQueCond, NULL);

	/* Mutexの初期化 */
	CMN_MutexInit(&s_einvQueMutex);

	/* キューの初期化 */
	s_einvQueHead = NULL;
	s_einvQueEnd = NULL;

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:通信要求キュー追加処理										*/
/*--------------------------------------------------------------------------*/
/*	param		:reqMsg		:メッセージ要求構造体							*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	処理結果											*/
/*				:		EINV_OK(0)				正常終了					*/
/*				:		EINV_ERR_SYSCALL(-1)	システムコールエラー		*/
/*				:		EINV_ERR_PARAM(-4)		パラメータエラー			*/
/*--------------------------------------------------------------------------*/
/*	detail		:通信要求キュー追加処理を行う。								*/
/*				:本関数はメインスレッドから呼ばれる。						*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/30 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_EnqCommReqQue(EinvMsgPar_t *reqMsg)
{
	slong ret = EINV_OK;
	EINV_COMM_QUE_T *queInfo = NULL;

	DEBUG_TRCIN();

	/* 引数確認 */
	if ( reqMsg == NULL ) 
	{
		DLOG_Error("reqMsg is NULL.\n");
		DEBUG_TRCOUT();
		return EINV_ERR_PARAM;
	}
	else
	{
		/* 処理無し */
	}

	/* キュー要素のメモリ確保 */
	queInfo = (EINV_COMM_QUE_T*)CMN_MALLOC(sizeof(EINV_COMM_QUE_T));
	if ( queInfo == NULL )
	{
		DLOG_Error("malloc() return NULL.\n");
		ret = EINV_ERR_SYSCALL;
	}
	else
	{
		queInfo->next = NULL;

		/* メッセージパラメータをコピー */
		memcpy(&(queInfo->reqMsg), reqMsg, sizeof(queInfo->reqMsg));

		// 排他ロック
		CMN_MutexLock(&s_einvQueMutex);

		/* キューの最後に要素を追加 */
		if(s_einvQueEnd == NULL)
		{
			s_einvQueHead = queInfo;
			s_einvQueEnd = queInfo;
		}
		else
		{
			s_einvQueEnd->next = queInfo;
			s_einvQueEnd = queInfo;
		}

		/* INV通信スレッドを起床させる。wait中でなければ何もおきない。 */
		pthread_cond_signal(&s_einvQueCond);

		/* 排他アンロック */
		CMN_MutexUnlock(&s_einvQueMutex);
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:通信要求キュー取り出し処理									*/
/*--------------------------------------------------------------------------*/
/*	param		:reqMsg		:メッセージ要求構造体							*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	処理結果											*/
/*				:		EINV_OK(0)				正常終了					*/
/*				:		EINV_ERR_PARAM(-4)		パラメータエラー			*/
/*--------------------------------------------------------------------------*/
/*	detail		:通信要求キュー取り出し処理を行う。							*/
/*				:本関数はINV通信スレッドから呼ばれる。						*/
/*				:キュー要素が空の場合はスレッドスリープする。				*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/30 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_DeqCommReqQue(EinvMsgPar_t *reqMsg)
{
	slong ret = EINV_OK;
	bool_t breakFlag = FALSE;
	EINV_COMM_QUE_T *queInfo = NULL;

	DEBUG_TRCIN();

	/* 引数確認 */
	if ( reqMsg == NULL ) 
	{
		DLOG_Error("reqMsg is NULL.\n");
		DEBUG_TRCOUT();
		return EINV_ERR_PARAM;
	}
	else
	{
		/* 処理無し */
	}

	/* 排他ロック */
	CMN_MutexLock(&s_einvQueMutex);

	while(!breakFlag){

		/* PCS通信スレッドのライフポイントをクリア */
		EINV_SetThreadLifePoint(0U);

		queInfo = s_einvQueHead;

		if(queInfo != NULL){

			/* キューの先頭の要素をコピー */
			memcpy(reqMsg, &queInfo->reqMsg, sizeof(EinvMsgPar_t));

			/* キューの先頭を次の要素に付け替える */
			s_einvQueHead = s_einvQueHead->next;
			if(s_einvQueHead == NULL)
			{
				s_einvQueEnd = NULL;
			}
			else
			{
				/* 処理無し */
			}

			/* メモリ解放 */
			CMN_FREE(queInfo);

			//DLOG_Debug("pop Que\n");
			breakFlag = TRUE;
		}
		else
		{
			/* キューに要素が追加されるまで通信スレッドをスリープ */
			//DLOG_Debug("thread Sleep\n");
			pthread_cond_wait(&s_einvQueCond, &s_einvQueMutex);
		}
	}

	/* 排他アンロック */
	CMN_MutexUnlock(&s_einvQueMutex);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:INV通信スレッドがスリープしていた場合に起床させる処理		*/
/*--------------------------------------------------------------------------*/
/*	param		:なし														*/
/*--------------------------------------------------------------------------*/
/*	return		:result	処理結果											*/
/*				:		EINV_OK			正常終了							*/
/*--------------------------------------------------------------------------*/
/*	detail		:INV通信スレッドがスリープしていた場合に起床させる処理		*/
/*				:を行う。													*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/30 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_WakeUpCommThread(void)
{
	slong ret = EINV_OK;

	DEBUG_TRCIN();

	/* 排他ロック */
	CMN_MutexLock(&s_einvQueMutex);
	/* INV通信スレッドを起床させる。wait中でなければ何もおきない。 */
	pthread_cond_signal(&s_einvQueCond);
	/* 排他アンロック */
	CMN_MutexUnlock(&s_einvQueMutex);

	DEBUG_TRCOUT();

	return ret;
}


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*End Of File*/
