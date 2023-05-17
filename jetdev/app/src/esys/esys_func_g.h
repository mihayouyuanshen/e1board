/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Chen.H												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/19												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __ESYS_FUNC_G_H__
#define __ESYS_FUNC_G_H__
#include "typedef.h"
#include "wdt_g.h"
#include "esys_g.h"
#include "enet_g.h"
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
extern void esys_SystemWakeup(void);
extern void esys_KeyEventProc(ushort msgId);
extern void esys_SystemWakeup(void);
extern void esys_StartFwUpdate(void);
extern void esys_EmergencyStop(void);
extern void esys_ShutDown(void);
extern void esys_SendWdtRes(void);
extern void esys_SendTermRes(WdtTermType_t type);
extern void esys_DebugLogProc(void);
extern void esys_PowerCutStChgProc(PowerCutOperation_t status);
extern void esys_ProcErrForWifi(const EdatErrInfo_t *errInfo);
extern void esys_ProcErrForLed(const EdatErrInfo_t *errInfo);
extern void esys_ClearErrForWifi(const EdatErrInfo_t *errInfo);
extern void esys_ClearErrForLed(const EdatErrInfo_t *errInfo);
extern void esys_ErrorHappenProc(const EdatErrInfo_t *errInfo);
extern void esys_ErrorCancelProc(const EdatErrInfo_t *errInfo);
extern void esys_ModStartRsltProc(ulong modId, ModActionResult_t result);
extern void esys_ModStopRsltProc(ulong modId, ModActionResult_t result);
extern void esys_LedRelatedErrDetected(void);
extern void esys_SetReadEEPromErr(void);
extern void esys_ClearReadEEPromErr(void);
#endif /*__ESYS_FUNC_G_H__*/
