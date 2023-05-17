/****************************************************************************/
/*	Description		:INV通信ミドル 通信キュー処理							*/
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
#ifndef __EINV_QUE_G_H__
#define __EINV_QUE_G_H__


/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "typedef.h"
#include "einv_msg_g.h"


/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern slong EINV_InitCommReqQue(void);
extern slong EINV_EnqCommReqQue(EinvMsgPar_t *reqMsg);
extern slong EINV_DeqCommReqQue(EinvMsgPar_t *reqMsg);
extern slong EINV_WakeUpCommThread(void);


#endif /*__EINV_QUE_G_H__*/
