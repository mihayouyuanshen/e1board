/****************************************************************************/
/*	Description		:INV通信ミドル 機能処理									*/
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
#ifndef __EINV_N_FUNC_G_H__
#define __EINV_N_FUNC_G_H__


/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "typedef.h"
#include "einv_g.h"
#include "einv_msg_g.h"
#include "einv_n_g.h"


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
extern slong EINV_n_InitMainThread(void);
extern slong EINV_n_InitCommThread(void);
extern slong EINV_n_InitSendCMD01(EinvMsgInitreqPar_t* msg);
extern slong EINV_n_InitSendCMD60(EinvMsgInitreqPar_t* msg);
extern slong EINV_n_InitSendCMD90(EinvMsgInitreqPar_t* msg);
extern slong EINV_n_InitSendCMD65(EinvMsgInitreqPar_t* msg);
extern slong EINV_n_InitSendCMD75(EinvMsgInitreqPar_t* msg);
extern slong EINV_n_InitSendCMD6B(EinvMsgInitreqPar_t* msg);
extern slong EINV_n_InitFin(void);
extern slong EINV_n_GetCycleOpeStateInfo(void);
extern slong EINV_n_AutoRunmodeTimerCtl(EinvTimerEvent_t timerEvent);
extern slong EINV_n_SixHourTimerCtl(void);
extern slong EINV_n_SetInvCommErr(void);
extern slong EINV_n_SetInvInitErr(void);
extern slong EINV_n_SendCMD64(void);
extern slong EINV_n_GetOpeMode(EinvOpemodeGet_t *opeModeGet);
extern slong EINV_n_GetOpeSttsInfo(EinvOpesttsInfo_t *opeSttsInfo);
extern slong EINV_n_GetCalcAvgValue(EinvCalcAvg_t *calcAvgValueGet);
#endif /*__EINV_N_FUNC_G_H__*/
