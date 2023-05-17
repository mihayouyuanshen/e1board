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
#ifndef __EDAT_SERV_H__
#define __EDAT_SERV_H__

#include "edat_msg.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define EERR_PRI_INVALID					((slong)-1)
#define EERR_PRI_MAX						((uchar)8u)
#define ENABLE_STATISTIC					((schar)1)
#define EDAT_ERROR_CODE_MEMORY_MAX_CNT		((ulong)100u)
#define EDAT_ERROR_CODE_LOAD_MEASURE_DIV	((ulong)10u)
#define EDAT_NO_ERROR						"BE000"
#define EDAT_NO_ERROR_RATING				'0'
/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef struct edat_errcode_node_t {
	slong pri;										/* 错误优先级 */
	u64 unixTime;									/* 错误发生时间 */
	EdatErrInfo_t errInfo;							/* 错误信息 */
	struct edat_errcode_node_t *prev;				/* 结点前继 */
	struct edat_errcode_node_t *next;				/* 结点后继 */
} EdatErrCodeNode_t;

typedef struct {
	EdatErrCodeNode_t regEntity;
	uchar isUsing;
} EdatErrCodeMemory_t;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern slong edat_SetErrCode(schar const *errCode, schar errCodeRating, EerrType_t type);
extern slong edat_ClearAllErrCode(void);
extern slong edat_ClearAllDesErrCode(EdatErrCodeNode_t** head, EerrType_t type);
extern slong edat_GetErrCodePri(schar const *errCode);

extern void edat_ErrListInsert(EdatErrCodeNode_t** head, EdatErrCodeNode_t* node);
extern slong edat_ErrListErase(EdatErrCodeNode_t** head, schar const *errCode, EerrType_t type);
extern EdatErrCodeNode_t* edat_ErrListFind(EdatErrCodeNode_t *head, schar const *errCode, EerrType_t type);

#endif /*__EDAT_SERV_H__*/
