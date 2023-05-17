/****************************************************************************/
/*	Description		:INV通信ミドル ユーティリティ							*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Dai.P												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/23												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:INV通信												*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EINV_N_UTL_G_H__
#define __EINV_N_UTL_G_H__


/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "typedef.h"
#include "common_g.h"
#include "einv_g.h"
#include "einv_msg_g.h"
#include "einv_n_g.h"


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
extern slong EINV_n_CheckComplete( sint fd ) ;
extern slong EINV_GetSysConfBitAssign(uchar cmdSysConf, ushort* bitSysconf);
extern slong EINV_GetSysConfCharacter(ushort bitSysconf, uchar* cmdSysConf);
extern slong EINV_n_StrYear2To4(schar* year2, schar* year4);
extern slong EINV_n_StrYear4To2(schar* year4, schar* year2);
extern slong EINV_n_NumYear2To4(uchar year2, ushort* year4);
extern slong EINV_n_NumYear4To2(ushort year4, uchar* year2);
extern	uchar EINV_n_ConvertSysStructureFromAsciiToNum(uchar sysStructure);
extern  EinvValidity_t EINV_n_CheckV2HOpStatus(uchar V2HOpStatus);
extern	EinvValidity_t EINV_n_CheckSetRunmodeFromServer(uchar ctlRunMode);
extern  EinvSupport_t  EINV_n_CheckSetRunmodeSupportOrNotFromServer(uchar ctlRunMode, uchar sysStructure);
extern	EinvValidity_t EINV_n_CheckSysStructure(uchar sysStructure);
extern	EinvValidity_t EINV_n_CheckSetRunModeFromSmartPhone(uchar ctlRunMode);
extern  EinvValidity_t EINV_n_CheckSetRunModeFromOpRemote(uchar ctlRunMode);
extern	uchar EINV_n_ConvertOpRemoteRunModeValue(uchar opRunMode);
extern	EinvValidity_t EINV_n_CheckRunModeSetFromExternal(uchar ctlRunModeSetFrom);
extern  EinvValidity_t EINV_n_CheckRunModeExternalEnl(uchar enlRunMode);
extern  EinvValidity_t EINV_n_CheckOpStartStop(uchar opStartStop);
extern  EinvValidity_t EINV_n_CheckPowerOffRunMode(uchar powerOffRunMode);
extern  slong  EINV_n_CheckTimeValid(const EinvTime_t* time);
extern  slong EINV_n_CheckSysTimeValid(const EtimTm_t* time);
extern  slong  EINV_n_ConvertTimeAsciiToValue(const EinvTimeStr_t* timeAscii, EinvTime_t* timeNum);
extern	slong  EINV_n_ConvertTimeValueToAscii(const EinvTime_t* timeNum, EinvTimeStr_t* timeAscii);
extern slong EINV_n_CheckTimeScope(const EinvTime_t* time, const EinvTimeScope_t* timeScope);
extern EinvFlag_t EINV_n_CheckButtonStatus(uchar buttonStatus, uchar* buttonPrestatus);
extern slong EINV_n_SocConvtRtoU(ushort* DEST, const ushort RSOC);
extern slong EINV_n_SocConvtUtoR(ushort* DEST, const ushort USOC);
extern schar* EINV_n_StrNCopy(schar* dest, const schar* src, size_t destSize);
#endif /*__EINV_N_UTL_G_H__*/
