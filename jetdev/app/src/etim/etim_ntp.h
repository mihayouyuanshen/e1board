/******************************************************************************/
/*    Description    :時刻管理ミドル                                           */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC Gu.CM                                               */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/04/04                                              */
/*----------------------------------------------------------------------------*/
/*    Module Name    :                                                        */
/*----------------------------------------------------------------------------*/
/*    Note           :                                                        */
/*----------------------------------------------------------------------------*/
/*    Copyright      :(c) 2020 NICHICON CORPORATION                           */
/******************************************************************************/
#ifndef __ETIM_NTP_H__
#define __ETIM_NTP_H__

#include "typedef.h"
#include "etim_g.h"
/*----------------------------------------------------------------------------*/
/* マクロ定義                                                                  */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開型宣言、定義                                                          */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 定数宣言、定義                                                              */
/*--------------------------------------- ------------------------------------*/
/* ETIM接続先NTPサーバー */
typedef enum {
	ETIM_NTP_DEFAULT = 0,			/* デフォルトNTPサーバー	*/
	ETIM_NTP_SPECIFY,		 		/* 電力会社指定NTPサーバー	*/
	ETIM_NTP_INVALID = 0xFFFF		/* 無効値(初期値)			*/
} EtimNtpServerE_t;

/*----------------------------------------------------------------------------*/
/* 変数宣言、定義                                                              */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* プロトタイプ宣言                                                            */
/*----------------------------------------------------------------------------*/
extern sint ETIM_NtpSync(const schar* pCmd);
extern sint ETIM_MakeNtpTime(void);
extern sint ETIM_NtpSetNextTime(sint offset);
extern void *ETIM_WatchNtp(void* arg);
extern sint ETIM_StartNtpSync(const EtimNtpInfo_t* pInfo);
extern void ETIM_InitNtpServerInfo(void);
extern sint ETIM_SetNtpServerInfo(const EtimNtpInfo_t* pInfo);
extern sint ETIM_GetNtpServerInfo(EtimNtpInfo_t* pInfo, schar* pCmd);
extern sint ETIM_WriteNtpServerInfo(const EtimNtpInfo_t* pInfo);
extern sint ETIM_ReadNtpServerInfo(EtimNtpInfo_t* pInfo);
extern sint ETIM_SetSyncTime(const EtimNtpInfo_t* pInfo);
extern sint ETIM_CheckRetry(const EtimNtpInfo_t* pInfo, const ulong retry, ulong* pInt, bool_t* pRet);
extern void ETIM_UpdateSyncState(void);
extern void ETIM_RecoverSyncState(void);
extern EtimNtpState_t ETIM_GetSyncState(void);
extern void ETIM_SetSyncState(EtimNtpState_t syncState);
extern sint ETIM_GetNextTime(const time_t *nowTime, time_t *nextTime);
bool_t ETIM_IsChangeNtpServerInfo(const EtimNtpInfo_t* before, const EtimNtpInfo_t* after);

#endif /*__ETIM_NTP_H__*/
