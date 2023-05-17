/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC yufeng.yang										*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/27												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __ESYS_G_H__
#define __ESYS_G_H__
#include "typedef.h"
#include "frm_g.h"
#include "ekey_g.h"
#include "dlog_g.h"
#include "einv_g.h"
#include "edat_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define ESYS_OK						((slong)0)
#define ESYS_ERROR					((slong)-1)

/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef enum {
	MOD_ACTION_RESULT_SUCCESS = 0,
	MOD_ACTION_RESULT_FAILED
} ModActionResult_t;

typedef enum {
	ESYS_START_FACT_MIN = 0,
	ESYS_START_FACT_NORMAL = ESYS_START_FACT_MIN,
	ESYS_START_FACT_RESET,
	ESYS_START_FACT_FOR_TEST,
	ESYS_START_FACT_FWRST,
	ESYS_START_FACT_MAX = ESYS_START_FACT_FWRST
} EsysStartFact_t;
/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern void ESYS_Init();
extern void ESYS_MsgProc(FrmMsgHead_t const *msg);
extern void ESYS_Fin();
extern slong ESYS_Start(slong fact);
extern slong ESYS_Stop();
extern slong ESYS_CheckStartFactor(slong fact);
extern void ESYS_SetStartFactor(slong fact);
extern slong ESYS_GetStartFactor();

extern void ESYS_NotifyStartResult(ulong modId, ModActionResult_t state);
extern void ESYS_NotifyStopResult(ulong modId, ModActionResult_t state);
extern void ESYS_NotifyKeyEvent(EKEY_Btn_t key, EKEY_BtnEvent_t event);
extern void ESYS_InvNotifyPowerCutSt(PowerCutOperation_t status);
extern void ESYS_NotifyErrHappen(EdatErrInfo_t *errInfo);
extern void ESYS_NotifyErrCancel(EdatErrInfo_t *errInfo);
extern void ESYS_NotifyShutdown(void);
extern void ESYS_NotifyReadEEPromErr(void);
extern void ESYS_ClearReadEEPromErr(void);
#endif /*__ESYS_G_H__*/
