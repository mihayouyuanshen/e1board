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
#ifndef __EDAT_PRIVATE_H__
#define __EDAT_PRIVATE_H__

#include <edat_g.h>
#include "task_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef enum {
	EDAT_MSGID_START = 0,
	/* 设置对应类型错误 */
	EDAT_ASYNC_SET_BOARD_ERRCODE,
	EDAT_ASYNC_SET_PCS_ERRCODE,
	EDAT_ASYNC_SET_RMC_ERRCODE,
	/* 清除对应类型的错误 */
	EDAT_ASYNC_CLEAR_BOARD_ERROR,
	EDAT_ASYNC_CLEAR_PCS_ERROR,
	EDAT_ASYNC_CLEAR_RMC_ERROR,
	/* 清除对应类型的所有错误 */
	EDAT_ASYNC_CLEAR_ALL_BOARD_ERROR,
	EDAT_ASYNC_CLEAR_ALL_PCS_ERROR,
	EDAT_ASYNC_CLEAR_ALL_RMC_ERROR,
	/* 清除所有错误 */
	EDAT_ASYNC_CLEAR_ALL_ERROR,
	EDAT_MAX_FUNC_NUMS
} EdatMsgId_t;

typedef struct {
	FrmMsgHead_t header;
	slong fact;
} EdatStart_t;

typedef struct {
	FrmMsgHead_t header;
	slong type;										/* 错误类型 */
	schar errCode[EDAT_ERROR_CODE_LEN];				/* 错误代码 */
	schar errCodeRating;							/* 错误等级 */
} EdatDesErrInfo_t;

typedef struct {
	FrmMsgHead_t header;
	slong type;
} EdatClearAllDesErr_t;

typedef struct {
	FrmMsgHead_t header;
	slong type;
	schar errCode[EDAT_ERROR_CODE_LEN];
} EdatClearDesErr_t;

typedef struct {
	FrmMsgHead_t header;
} EdatClearAllErr_t;
/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/


#endif /*__EDAT_PRIVATE_H__*/
