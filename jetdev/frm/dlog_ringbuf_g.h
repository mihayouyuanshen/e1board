/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Xu.Y												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/27												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __DLOG_RINGBUF_G_H__
#define __DLOG_RINGBUF_G_H__

#include "typedef.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef struct
{
	uchar	*data;
	slong	in;
	slong	out;
	slong	mask;
	slong	size;
} DlogRingbuf_t;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern void DLOG_RingbufInit(DlogRingbuf_t *self, slong itemSize,
	void *buf, slong bufSize);
extern slong DLOG_RingbufIn(DlogRingbuf_t *self, const void *buf,
	slong bufSize);
extern slong DLOG_RingbufOut(DlogRingbuf_t *self, const void *buf,
	slong bufSize);
extern bool_t DLOG_RingbufIsFull(DlogRingbuf_t *self);
extern bool_t DLOG_RingbufIsEmpty(DlogRingbuf_t *self);

#endif /*__DLOG_RINGBUF_G_H__*/
