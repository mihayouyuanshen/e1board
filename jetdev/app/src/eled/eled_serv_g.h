/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Gu.CM												*/
/*--------------------------------------------------------------------------*/
/*	Date			::2023/03/16											*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __ELED_SERV_G_H__
#define __ELED_SERV_G_H__

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
	ELED_WORKMODE_PWRON,
	ELED_WORKMODE_PWRRUN,
	ELED_WORKMODE_PWROFF,
	ELED_WORKMODE_TESTRUN,
	ELED_WORKMODE_SHUTDOWN,
	ELED_WORKMODE_ERROR,

	ELED_WORKMODE_FWDOWN,
	ELED_WORKMODE_FWUPDATA,
	ELED_WORKMODE_FWFINISH,
	ELED_WORKMODE_FWE1ERR,
	ELED_WORKMODE_FWINVERR,
	ELED_WORKMODE_FWPVERR,
	ELED_WORKMODE_FWBDCERR,
	ELED_WORKMODE_FWJBERR,
	ELED_WORKMODE_FWOTHERR,
	ELED_WORKMODE_MAX
} EledWorkMode_t;

typedef enum{
	ELED_PRIORITY_POWER = 0u,
	ELED_PRIORITY_TESTRUN,
	ELED_PRIORITY_SHUTDOWN,
	ELED_PRIORITY_ERROR,
	ELED_PRIORITY_FW
}EledPriority_t;

/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern void ELED_ServInit(void);
extern void ELED_ProcChgMode(void);
extern void ELED_ProcDoFirstSeq(void);
extern void ELED_ProcDoNextSeq(void);
extern void ELED_ProcDoFirstStep(void);
extern void ELED_ProcDoNextStep(void);
extern void ELED_ProcStart(FrmMsgHead_t const *msg);
extern void ELED_EwdtRes(FrmMsgHead_t const *msg);
extern void ELED_EwdtTermReg(FrmMsgHead_t const *msg);
extern void ELED_OpenGpioStart(void);
extern void ELED_CloseGpioStop(void);
extern void ELED_LedOffStepProc(void);
extern void ELED_LedBlinkStop(EledWorkMode_t mode);

#endif /*__ELED_SERV_G_H__*/
