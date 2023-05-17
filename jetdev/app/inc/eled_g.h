/****************************************************************************/
/*	Description		:LED制御プロセスヘッダ									*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Gu.CM												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/10												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __ELED_G_H__
#define __ELED_G_H__

#include "typedef.h"
#include "frm_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
typedef enum {
	ELED_PWRLED_ONING,
	ELED_PWRLED_RUNNING,
	ELED_PWRLED_OFFING,
	ELED_PWRLED_TESTRUN,
	ELED_PWRLED_SHUTDOWN,
	ELED_PWRLED_ERROR,
	ELED_PWRLED_MAX
} EledPwrLed_t;

typedef enum {
	ELED_FWLED_DOWNLOAD,
	ELED_FWLED_UPDATE,
	ELED_FWLED_FINISH,
	ELED_FWLED_ERR_E1BOARD,
	ELED_FWLED_ERR_INV,
	ELED_FWLED_ERR_PV,
	ELED_FWLED_ERR_BDC,
	ELED_FWLED_ERR_JB,
	ELED_FWLED_ERR_OTHER,
	ELED_FWLED_MAX
} EledFwLed_t;

/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern void ELED_Init(void);
extern void ELED_Fin(void);
extern void ELED_MsgProc(FrmMsgHead_t const *msg);
extern slong ELED_Start(slong fact);
extern slong ELED_Stop(void);

extern void ELED_PwrLedBlankStart(EledPwrLed_t pwrLed);
extern void ELED_PwrLedBlankStop(EledPwrLed_t pwrLed);
extern void ELED_FwLedBlankStart(EledFwLed_t fwLed);
extern void ELED_FwLedBlankStop(EledFwLed_t fwLed);

#endif /*__ELED_G_H__*/
