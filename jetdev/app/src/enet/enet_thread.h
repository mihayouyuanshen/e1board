/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jingtong.li										*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/17												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __ENET_THREAD_H__
#define __ENET_THREAD_H__
/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "frm_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define EVENT_NUM 12
/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
static void* enet_ExecDhcpThreadMain(void* arg);
static void* enet_ExecScanEssidThreadMain(void* arg);
static void* enet_ExecHostapdThreadMain(void* arg);
static void* enet_ExecWlanSettingThreadMain(void* arg);
static void* enet_SendPingThreadMain(void* arg);
static void* enet_WatchIPThreadMain(void* arg);
static void* enet_Inotify(void* arg);
static void enet_CancelThread(FrmPthread_t thread);
static void enet_RmInsMod(void);

#endif /*__ENET_THREAD_H__*/
