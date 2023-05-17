/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jingtong.li										*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/20												*/
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
#include "edat_serv_g.h"
#include "dlog_g.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <esys_g.h>
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
EdatErrCodeMemory_t s_edatErrorCodeMemoryPool[EDAT_ERROR_CODE_MEMORY_MAX_CNT];
ulong s_edatErrorCodeMaxLoad = 0;
ulong s_edatErrorCodeCurLoad = 0;
ulong s_edatErrorCodeLoadMeasureCnt = 0;
EdatErrCodeNode_t *s_EDAT_ErrCodeListHead = NULL;
/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
const schar * s_edatPriTbl[EERR_PRI_MAX] = {
	"bi",		/*  Lo */
	"BI",		/*  |  */
	"bc",		/*  |  */
	"BC",		/*  |  */
	"bf",		/*  |  */
	"BF",		/*  |  */
	"be",		/* \|/ */
	"BE"		/*  Hi */
};

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
static void *edat_ErrMalloc(slong size);
static void edat_ErrFree(void *ptr);
/*==========================================================================*/
/*	Description		:设置类型为通信ボード的错误信息							*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatDesErrInfo_t *fncErrCode	错误信息				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:设置类型为通信ボード的错误信息							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/23 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong EDAT_SetBoardErrInfo(EdatDesErrInfo_t *fncErrCode)
{
	slong fncRet = EDAT_OK;
	DEBUG_TRCIN();

	edat_SetErrCode(fncErrCode->errCode, fncErrCode->errCodeRating, fncErrCode->type);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:设置类型为PCS的错误信息								*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatDesErrInfo_t *fncErrCode	错误信息				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:设置类型为PCS的错误信息								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/23 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong EDAT_SetPcsErrInfo(EdatDesErrInfo_t *fncErrCode)
{
	slong fncRet = EDAT_OK;
	DEBUG_TRCIN();

	edat_SetErrCode(fncErrCode->errCode, fncErrCode->errCodeRating, fncErrCode->type);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:设置类型为RMC的错误信息								*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatDesErrInfo_t *fncErrCode	错误信息				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:设置类型为RMC的错误信息								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/23 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong EDAT_SetRmcErrInfo(EdatDesErrInfo_t *fncErrCode)
{
	slong fncRet = EDAT_OK;
	DEBUG_TRCIN();

	edat_SetErrCode(fncErrCode->errCode, fncErrCode->errCodeRating, fncErrCode->type);
	
	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:清除链表中类型为通信ボード的指定错误(异步)				*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatClearDesErr_t *fncDesErr	指定错误				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:清除链表中类型为通信ボード的指定错误(异步)				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/23 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong EDAT_ClearBoardErrInfo(EdatClearDesErr_t *fncDesErr)
{
	slong fncRet = EDAT_OK;
	DEBUG_TRCIN();

	edat_ErrListErase(&s_EDAT_ErrCodeListHead, fncDesErr->errCode, fncDesErr->type);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:清除链表中类型为PCS的指定错误(异步)					*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatClearDesErr_t *fncDesErr	指定错误				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:清除链表中类型为PCS的指定错误(异步)					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/23 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong EDAT_ClearPcsErrInfo(EdatClearDesErr_t *fncDesErr)
{
	slong fncRet = EDAT_OK;
	DEBUG_TRCIN();

	edat_ErrListErase(&s_EDAT_ErrCodeListHead, fncDesErr->errCode, fncDesErr->type);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:清除链表中类型为RMC的指定错误(异步)					*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatClearDesErr_t *fncDesErr	指定错误				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:清除链表中类型为RMC的指定错误(异步)					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/23 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong EDAT_ClearRmcErrInfo(EdatClearDesErr_t *fncDesErr)
{
	slong fncRet = EDAT_OK;
	DEBUG_TRCIN();

	edat_ErrListErase(&s_EDAT_ErrCodeListHead, fncDesErr->errCode, fncDesErr->type);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:清除链表中存储的所有类型为通信ボード错误(异步)			*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatClearAllDesErr_t *fncErrInfo	错误信息			*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:清除链表中存储的所有类型为通信ボード错误(异步)			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/23 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong EDAT_ClearAllBoardErrInfo(EdatClearAllDesErr_t *fncErrInfo)
{
	slong fncRet = EDAT_OK;
	DEBUG_TRCIN();

	edat_ClearAllDesErrCode(&s_EDAT_ErrCodeListHead, fncErrInfo->type);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:清除链表中存储的所有类型为PCS错误(异步)				*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatClearAllDesErr_t *fncErrInfo	错误信息			*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:清除链表中存储的所有类型为PCS错误(异步)				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/23 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong EDAT_ClearAllPcsErrInfo(EdatClearAllDesErr_t *fncErrInfo)
{
	slong fncRet = EDAT_OK;
	DEBUG_TRCIN();

	edat_ClearAllDesErrCode(&s_EDAT_ErrCodeListHead, fncErrInfo->type);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:清除链表中存储的所有类型为RMC的错误(异步)				*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatClearAllDesErr_t *fncErrInfo	错误信息			*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:清除链表中存储的所有类型为RMC的错误(异步)				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/23 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong EDAT_ClearAllRmcErrInfo(EdatClearAllDesErr_t *fncErrInfo)
{
	slong fncRet = EDAT_OK;
	DEBUG_TRCIN();

	edat_ClearAllDesErrCode(&s_EDAT_ErrCodeListHead, fncErrInfo->type);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:清除存储的所有错误(异步)								*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatClearAllErr_t *fncErrCode	错误代码				*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:清除存储的所有错误(异步)								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/23 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong EDAT_ClearAllErrInfo(EdatClearAllErr_t *fncErrCode)
{
	slong fncRet = EDAT_OK;
	DEBUG_TRCIN();

	edat_ClearAllErrCode();

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:根据错误代码获取错误优先级								*/
/*--------------------------------------------------------------------------*/
/*	param			:schar const *errCode	错误代码						*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	错误优先级										*/
/*--------------------------------------------------------------------------*/
/*	detail			:根据错误代码获取错误优先级								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/18 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong edat_GetErrCodePri(schar const *errCode)
{
	slong i = 0;
	slong loopFlag = 1;
	slong errPri = EERR_PRI_INVALID;		/* 错误优先级 */

	DEBUG_TRCIN();

	while((i < EERR_PRI_MAX) && (loopFlag != 0))
	{
		if(strncmp(errCode, s_edatPriTbl[i], 2) == 0)
		{
			errPri = i;
			loopFlag = 0;
		}
		else
		{
			i++;
		}
	}
	DEBUG_TRCOUT();

	return errPri;
}

/*==========================================================================*/
/*	Description		:设置错误代码和错误等级									*/
/*--------------------------------------------------------------------------*/
/*	param			:schar const *errCode	错误代码						*/
/*					schar errCodeRating		错误等级						*/
/*					EerrType_t type			错误类型						*/
/*--------------------------------------------------------------------------*/
/*	return			:EDAT_OK(0)												*/
/*					EDAT_ERROR(-1)											*/
/*--------------------------------------------------------------------------*/
/*	detail			:设置错误代码和错误等级									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong edat_SetErrCode(schar const *errCode, schar errCodeRating, EerrType_t type)
{
	slong errPri;
	struct timeval tv;
	EdatErrCodeNode_t *node = NULL;
	DEBUG_TRCIN();

	errPri = edat_GetErrCodePri(errCode);
	if (EERR_PRI_INVALID == errPri)
	{
		DLOG_Error("get error code pri Failed!");
		return EDAT_ERROR;
	}
	if (NULL != s_EDAT_ErrCodeListHead)
	{
		node = edat_ErrListFind(s_EDAT_ErrCodeListHead, errCode, type);
	}

	if (NULL == node)
	{
		node = edat_ErrMalloc(sizeof(EdatErrCodeNode_t));
		node->pri = errPri;
		memcpy(node->errInfo.errCode, errCode, EDAT_ERROR_CODE_LEN);
		node->errInfo.errCodeRating = errCodeRating;
		node->errInfo.type = type;
		/* 错误时间 ms */
		gettimeofday(&tv, NULL);
		node->unixTime = (u64)tv.tv_sec * 1000 + (u64)tv.tv_usec / 1000;
		node->prev = NULL;
		node->next = NULL;
		/* insert to list */
		edat_ErrListInsert(&s_EDAT_ErrCodeListHead, node);
		ELOG_ErrEventLog(type, errCode, errCodeRating);
		ESYS_NotifyErrHappen(&node->errInfo);
	}
	DEBUG_TRCOUT();
	return EDAT_OK;
}

/*==========================================================================*/
/*	Description		:清除所有错误											*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:清除所有错误											*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong edat_ClearAllErrCode(void)
{
	EdatErrCodeNode_t* head = s_EDAT_ErrCodeListHead;
	EdatErrCodeNode_t* node;
	DEBUG_TRCIN();
	while(head)
	{
		node = head;
		head = node->next;
		edat_ErrFree(node);
	}
	s_EDAT_ErrCodeListHead = NULL;

	DEBUG_TRCOUT();
	return EDAT_OK;
}

/*==========================================================================*/
/*	Description		:清除所有指定类型的错误									*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatErrCodeNode_t** head								*/
/*					EerrType_t type	错误类型								*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:清除所有指定类型的错误									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/23 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong edat_ClearAllDesErrCode(EdatErrCodeNode_t** head, EerrType_t type)
{
	slong clearSucFlag = EDAT_OK;
	EdatErrCodeNode_t* node;
	node = *head;
	DEBUG_TRCIN();

	while(NULL != node)
	{
		if(type == node->errInfo.type)
		{
			if(node == *head)
			{
				*head = node->next;
			}
			else
			{
				node->prev->next = node->next;
				if(NULL != node->next)
				{
					node->next->prev = node->prev;
				}
			}
			edat_ErrFree(node);
		}
		else
		{
			node = node->next;
		}
	}

	DEBUG_TRCOUT();
	return (clearSucFlag);
}

/*==========================================================================*/
/*	Description		:将错误信息按照优先级高到低的顺序存储					*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatErrCodeNode_t** head	头结点						*/
/*					EdatErrCodeNode_t* node		新结点						*/
/*--------------------------------------------------------------------------*/
/*	return			:無い													*/
/*--------------------------------------------------------------------------*/
/*	detail			:将错误信息按照优先级高到低的顺序存储					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void edat_ErrListInsert(EdatErrCodeNode_t** head, EdatErrCodeNode_t* node)
{
	schar insertFlag = 0;
	EdatErrCodeNode_t* curNode;
	DEBUG_TRCIN();
	if (NULL == *head)
	{
		/* 空链表 */
		*head = node;
	}
	else
	{
		curNode = *head;
		/* 优先级比链表中优先级高 直接插入 */
		if(node->pri > curNode->pri)
		{
			node->next = curNode;
			curNode->prev = node;
			*head = node;
		}
		/* 要插入结点的优先级等于或小于链表中优先级 */
		else
		{
			/* 循环查找优先级比插入结点的优先级小的结点*/
			while((NULL != curNode->next) && (1 != insertFlag))
			{
				if(node->pri > curNode->pri)
				{
					node->prev = curNode->prev;
					node->next = curNode;
					curNode->prev = node;
					curNode->prev->next = node;
					insertFlag = 1;
				}
				else
				{
					curNode = curNode->next;
				}
			}
			/* 没查到 直接插在尾部 */
			if(curNode->next == NULL)
			{
				curNode->next = node;
				node->prev = curNode;
			}
		}
	}
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:将指定的错误消除										*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatErrCodeNode_t** head	头结点						*/
/*					schar const *errCode		错误代码					*/
/*					EerrType_t type				错误类型					*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:将指定的错误消除										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong edat_ErrListErase(EdatErrCodeNode_t** head, schar const *errCode, EerrType_t type)
{
	EdatErrCodeNode_t const *node;
	EdatErrInfo_t errInfo;
	slong eraRes = EDAT_ERROR;
	DEBUG_TRCIN();

	/* 查找需要删除的errcode结点 */
	node = edat_ErrListFind(s_EDAT_ErrCodeListHead, errCode, type);
	/* 删除符合条件结点 */
	if (NULL != node)
	{
		errInfo.type = type;
		errInfo.errCodeRating = node->errInfo.errCodeRating;
		memcpy(errInfo.errCode, node->errInfo.errCode, EDAT_ERROR_CODE_LEN);

		if(node == *head)
		{
			*head = node->next;
		}
		else
		{
			node->prev->next = node->next;
			if(NULL != node->next)
			{
				node->next->prev = node->prev;
			}
		}
		edat_ErrFree(node);
		ESYS_NotifyErrCancel(&errInfo);
		eraRes = EDAT_OK;
	}
	else
	{
		/* DO NOTING */
	}
	DEBUG_TRCOUT();
	return eraRes;
}

/*==========================================================================*/
/*	Description		:查找符合条件的结点										*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatErrCodeNode_t *head	头结点						*/
/*					schar const *errCode	错误代码						*/
/*					EerrType_t type	错误类型								*/
/*--------------------------------------------------------------------------*/
/*	return			:EdatErrCodeNode_t*	符合条件结点						*/
/*--------------------------------------------------------------------------*/
/*	detail			:查找符合条件的结点										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
EdatErrCodeNode_t* edat_ErrListFind(EdatErrCodeNode_t *head, schar const *errCode, EerrType_t type)
{
	schar findFlag = 0;
	EdatErrCodeNode_t* node = head;

	DEBUG_TRCIN();
	while ((node != NULL) && (findFlag != 1))
	{
		if(0 == strcmp(errCode, node->errInfo.errCode))
		{
			if(type == node->errInfo.type)
			{
				findFlag = 1;
			}
		}
		else
		{
			node = node->next;
		}
	}
	DEBUG_TRCOUT();

	return node;
}

/*==========================================================================*/
/*	Description		:查找符合条件的结点										*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatErrCodeNode_t *head	头结点						*/
/*					EerrType_t type	错误类型								*/
/*--------------------------------------------------------------------------*/
/*	return			:EdatErrCodeNode_t*	符合条件结点						*/
/*--------------------------------------------------------------------------*/
/*	detail			:查找符合条件的结点										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
EdatErrCodeNode_t* edat_DesErrListFind(EdatErrCodeNode_t *head, EerrType_t type)
{
	schar findFlag = 0;
	EdatErrCodeNode_t* node = head;

	DEBUG_TRCIN();
	while ((node != NULL) && (findFlag != 1))
	{
		if(type == node->errInfo.type)
		{
			findFlag = 1;
		}
		else
		{
			node = node->next;
		}
	}
	DEBUG_TRCOUT();

	return node;
}

/*==========================================================================*/
/*	Description		:根据错误前缀查找错误									*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:根据错误前缀查找错误									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong EDAT_FindErr(EerrType_t type, const schar *prefix)
{
	slong findSuc = EDAT_ERROR;
	EdatErrCodeNode_t* node = s_EDAT_ErrCodeListHead;
	DEBUG_TRCIN();

	while ((node != NULL) && (EDAT_OK != findSuc))
	{
		if(0 == strncmp(prefix, node->errInfo.errCode, 2) && type == node->errInfo.type)
		{
			findSuc = EDAT_OK;
		}
		else
		{
			node = node->next;
		}
	}

	DEBUG_TRCOUT();
	return (findSuc);
}

/*==========================================================================*/
/*	Description		:获取优先级最高错误										*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatErrInfo_t *errInfo	错误信息						*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:获取优先级最高错误										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong EDAT_GetErrCode(EdatErrInfo_t *errInfo)
{
	slong getErrFlag = EDAT_OK;
	DEBUG_TRCIN();

	if(NULL == s_EDAT_ErrCodeListHead)
	{
		errInfo->type = EERR_TYPE_NO_ERROR;
		memcpy(errInfo->errCode, EDAT_NO_ERROR, EDAT_ERROR_CODE_LEN);
		errInfo->errCodeRating = EDAT_NO_ERROR_RATING;
	}
	else
	{
		errInfo->type = s_EDAT_ErrCodeListHead->errInfo.type;
		memcpy(errInfo->errCode, s_EDAT_ErrCodeListHead->errInfo.errCode, EDAT_ERROR_CODE_LEN);
		errInfo->errCodeRating = s_EDAT_ErrCodeListHead->errInfo.errCodeRating;
	}

	DEBUG_TRCOUT();
	return (getErrFlag);
}

/*==========================================================================*/
/*	Description		:获取优先级最高的通信ボード错误							*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatErrInfo_t *errInfo	错误信息						*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:获取优先级最高的通信ボード错误							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong EDAT_GetBoardErrCode(EdatErrInfo_t *errInfo)
{
	EdatErrCodeNode_t* node;
	slong getErrFlag = EDAT_OK;
	DEBUG_TRCIN();

	node = edat_DesErrListFind(s_EDAT_ErrCodeListHead, EERR_TYPE_BOARD);
	if(NULL != node)
	{
		errInfo->type = node->errInfo.type;
		memcpy(errInfo->errCode, node->errInfo.errCode, EDAT_ERROR_CODE_LEN);
		errInfo->errCodeRating = node->errInfo.errCodeRating;
	}
	else
	{
		errInfo->type = EERR_TYPE_NO_ERROR;
		memcpy(errInfo->errCode, EDAT_NO_ERROR, EDAT_ERROR_CODE_LEN);
		errInfo->errCodeRating = EDAT_NO_ERROR_RATING;
	}

	DEBUG_TRCOUT();
	return (getErrFlag);
}

/*==========================================================================*/
/*	Description		:获取优先级最高的PCS错误								*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatErrInfo_t *errInfo	错误信息						*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:获取优先级最高的PCS错误								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong EDAT_GetPcsErrCode(EdatErrInfo_t *errInfo)
{
	EdatErrCodeNode_t* node;
	slong getErrFlag = EDAT_OK;
	DEBUG_TRCIN();

	node = edat_DesErrListFind(s_EDAT_ErrCodeListHead, EERR_TYPE_PCS);
	if(NULL != node)
	{
		errInfo->type = node->errInfo.type;
		memcpy(errInfo->errCode, node->errInfo.errCode, EDAT_ERROR_CODE_LEN);
		errInfo->errCodeRating = node->errInfo.errCodeRating;
	}
	else
	{
		errInfo->type = EERR_TYPE_NO_ERROR;
		memcpy(errInfo->errCode, EDAT_NO_ERROR, EDAT_ERROR_CODE_LEN);
		errInfo->errCodeRating = EDAT_NO_ERROR_RATING;
	}

	DEBUG_TRCOUT();
	return (getErrFlag);
}

/*==========================================================================*/
/*	Description		:获取优先级最高的RMC错误								*/
/*--------------------------------------------------------------------------*/
/*	param			:EdatErrInfo_t *errInfo	错误信息						*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:获取优先级最高的RMC错误								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong EDAT_GetRmcErrCode(EdatErrInfo_t *errInfo)
{
	EdatErrCodeNode_t* node;
	slong getErrFlag = EDAT_OK;
	DEBUG_TRCIN();
	
	node = edat_DesErrListFind(s_EDAT_ErrCodeListHead, EERR_TYPE_RMC);
	if(NULL != node)
	{
		errInfo->type = node->errInfo.type;
		memcpy(errInfo->errCode, node->errInfo.errCode, EDAT_ERROR_CODE_LEN);
		errInfo->errCodeRating = node->errInfo.errCodeRating;
	}
	else
	{
		errInfo->type = EERR_TYPE_NO_ERROR;
		memcpy(errInfo->errCode, EDAT_NO_ERROR, EDAT_ERROR_CODE_LEN);
		errInfo->errCodeRating = EDAT_NO_ERROR_RATING;
	}

	DEBUG_TRCOUT();
	return (getErrFlag);
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:申请Error内存											*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong size:内存大小									*/
/*--------------------------------------------------------------------------*/
/*	return			:void *													*/
/*--------------------------------------------------------------------------*/
/*	detail			:从内存池申请Error内存									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
static void *edat_ErrMalloc(slong size)
{
	slong i;
	void *ptr = NULL;
	DEBUG_TRCIN();

	if (size <= sizeof(EdatErrCodeNode_t))
	{
		for (i = 0; (i < EDAT_ERROR_CODE_MEMORY_MAX_CNT) && (NULL == ptr); i++)
		{
			if (!s_edatErrorCodeMemoryPool[i].isUsing)
			{
				s_edatErrorCodeMemoryPool[i].isUsing = 1;
				ptr = &s_edatErrorCodeMemoryPool[i];
				s_edatErrorCodeCurLoad++;
				if (s_edatErrorCodeMaxLoad < s_edatErrorCodeCurLoad)
				{
					s_edatErrorCodeMaxLoad = s_edatErrorCodeCurLoad;
				}
				else
				{
					/* 処理無し */
				}
				s_edatErrorCodeLoadMeasureCnt++;
				s_edatErrorCodeLoadMeasureCnt = s_edatErrorCodeLoadMeasureCnt % EDAT_ERROR_CODE_LOAD_MEASURE_DIV;
				if (0 == s_edatErrorCodeLoadMeasureCnt)
				{
					DLOG_Debug("MemoryPool Load:Current[%lu] Max[%lu]", s_edatErrorCodeCurLoad, s_edatErrorCodeMaxLoad);
				}
				else
				{
					/* 処理無し */
				}
			}
			else
			{
				/* 処理無し */
			}
		}
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();
	return ptr;
}

/*==========================================================================*/
/*	Description		:释放Error内存											*/
/*--------------------------------------------------------------------------*/
/*	param			:void *ptr:Errorr内存指针								*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:释放Error内存											*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/20 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
static void edat_ErrFree(void *ptr)
{
	EdatErrCodeMemory_t *pMemory = (EdatErrCodeMemory_t *)ptr;
	DEBUG_TRCIN();

	if (pMemory)
	{
		pMemory->isUsing = 0;
		s_edatErrorCodeCurLoad--;
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();
}

/*End Of File*/
