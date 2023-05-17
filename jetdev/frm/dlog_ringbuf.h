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
#ifndef __DLOG_RINGBUF_H__
#define __DLOG_RINGBUF_H__

#include "dlog_ringbuf_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define GET_MIN(a, b)			((a) < (b) ? (a) : (b))
#define DLOG_RBF_DEF_SIZE		1

/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
static slong dlog_RingbufRounddownPowOfTwo(slong mask);
static slong dlog_RingbufCap(DlogRingbuf_t *self);
static slong dlog_RingbufLen(DlogRingbuf_t *self);
static slong dlog_RingbufAvail(DlogRingbuf_t *self);
static void dlog_RingbufCopyIn(DlogRingbuf_t *self, const void *src,
	slong copySize, slong off);
static void dlog_RingbufCopyOut(DlogRingbuf_t *self, void *dst,
	slong copySize, slong off);
static slong dlog_RingbufOutPeek(DlogRingbuf_t *self, void *buf,
	slong bufSize);



#endif /*__DLOG_RINGBUF_H__*/
