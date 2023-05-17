/******************************************************************************/
/*    Description    :時刻管理ミドル                                           */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC Gu.CM                                               */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/04/03                                              */
/*----------------------------------------------------------------------------*/
/*    Module Name    :                                                        */
/*----------------------------------------------------------------------------*/
/*    Note           :                                                        */
/*----------------------------------------------------------------------------*/
/*    Copyright      :(c) 2020 NICHICON CORPORATION                           */
/******************************************************************************/
#ifndef __ETIM_UTL_H__
#define __ETIM_UTL_H__

#include <time.h>
#include "typedef.h"
/*----------------------------------------------------------------------------*/
/* マクロ定義                                                                  */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開型宣言、定義                                                          */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 定数宣言、定義                                                              */
/*--------------------------------------- ------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 変数宣言、定義                                                              */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* プロトタイプ宣言                                                            */
/*----------------------------------------------------------------------------*/
extern sint ETIM_StrpTime(const schar *dateStr, struct tm *rtm);
extern sint ETIM_ClockSetTime(clockid_t clkId, const struct timespec *tp);
extern sint ETIM_ClockGetTime(clockid_t clkId, struct timespec *tp);
extern sint ETIM_LocalTimeR(const time_t *timep, struct tm *result);
extern sint ETIM_MkTime(struct tm *ptm, time_t *pt);
extern sint ETIM_CheckDateRange(const struct tm *ptm);

#endif /*__ETIM_UTL_H__*/
