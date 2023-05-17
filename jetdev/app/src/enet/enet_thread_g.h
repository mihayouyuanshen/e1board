/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Cao.HZ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/05/06												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __ENET_THREAD_G_H__
#define __ENET_THREAD_G_H__
/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern slong ENET_ExecCmdKillDhcp(void);
extern slong ENET_ExecDhcpThread(void);
extern slong ENET_ExecCmdKillHostapd(void);
extern slong ENET_ExecWlanSettingThread(void);
extern slong ENET_ExecScanEssidThread(ulong cPort);
extern slong ENET_SendPingThread(ulong cPort);
extern slong ENET_WatchIPThread(void);
extern void ENET_InitEnetThread(int argc, char *argv[]);
extern slong ENET_ExecHostapdThread(void);
extern void ENET_CreateInotifyThread(void);


#endif /*__ENET_THREAD_G_H__*/
