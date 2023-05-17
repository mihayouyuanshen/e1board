/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Cao.HZ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/28												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EEEP_G_H__
#define __EEEP_G_H__
/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "typedef.h"
#include "frm_g.h"
#include "task_g.h"
/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern void EEEP_Init();
extern void EEEP_MsgProc(FrmMsgHead_t const *msg);
extern void EEEP_Fin();
extern slong EEEP_Start(slong fact);
extern slong EEEP_Stop();

extern slong EEEP_GetMountFailTimes(uchar *buf);
extern slong EEEP_GetBK0FailTimes(uchar *buf);
extern slong EEEP_GetBK1FailTimes(uchar *buf);
extern slong EEEP_GetBoardName(uchar *buf);
extern slong EEEP_GetBoardSeries(uchar *buf);
extern slong EEEP_GetBK0FWVer(uchar *buf);
extern slong EEEP_GetBK1FWVer(uchar *buf);
extern slong EEEP_GetWireMac(uchar *buf);
extern slong EEEP_GetWirelessMac(uchar *buf);

extern void EEEP_SetBK0FWVer(uchar *buf);
extern void EEEP_SetBK1FWVer(uchar *buf);
extern void EEEP_SetWireMac(uchar *buf);
extern void EEEP_SetWirelessMac(uchar *buf);

#endif /*__EEEP_H__*/
