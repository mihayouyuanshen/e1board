/****************************************************************************/
/*	Description		:キーモジュール業務（公開）								*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Cao.HZ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/20												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EKEY_SERV_G_H__
#define __EKEY_SERV_G_H__
/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "typedef.h"
#include "frm_g.h"
/*--------------------------------------------------------------------------*/
/*	公開プロトタイプ宣言													*/
/*--------------------------------------------------------------------------*/
extern void EKEY_CreateSubThread(void);
extern void EKEY_ProcKeyOnOff(FrmMsgHead_t const *msg);
extern void EKEY_ProcKeyTimeout(FrmMsgHead_t const *msg);
extern void EKEY_ProcKeyCancel(FrmMsgHead_t const *msg);
#endif /*__EKEY_SERV_H__*/
