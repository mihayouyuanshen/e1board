/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Gu.CM												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/05/09												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __ELED_SERV_H__
#define __ELED_SERV_H__

#include "typedef.h"
#include "eled_serv_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
typedef struct eledPriNode {
	EledPriority_t	pri;
	EledWorkMode_t	mode;
	struct eledPriNode *prev;
	struct eledPriNode *next;
}eledPriNode_t;

typedef struct eledPriMemoryPool
{
	eledPriNode_t node;
	uchar isUsing;
}eledPriMemoryPool_t;


/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
static void eled_ProcDoCurrentStep(void);
static void eled_SendTriggerMsg(ushort msgId);
static void eled_FileWrite(sint fd, schar *val, sint len);
static void eled_setLedOn(sint ledFd);
static void eled_setLedOff(sint ledFd);
static void eled_LedOnStepProc(void);
static void eled_OgLedOnStepProc(void);
static void eled_GrLedOnStepProc(void);
static void eled_ChgModePreProc(void);
static void eled_PriListErase(EledWorkMode_t mode);
static void eled_LedPriListInsert(EledWorkMode_t mode, EledPriority_t pri);
static bool_t eled_PriListFind(EledWorkMode_t mode);
static void eled_PriFree(void *ptr);
static void *eled_PriMalloc(slong size);
static void eled_PrtList(void);

#endif /*__ELED_SERV_H__*/
