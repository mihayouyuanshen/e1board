/****************************************************************************/
/*	Description		:ECHONET Lite application for device usage				*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC K.Jiang											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/18												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:eenl													*/
/*--------------------------------------------------------------------------*/
/*	Note			:分散型電源電力量メータクラス							*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(C) 2023. Nichicon Corporation. All Rights Reserved.	*/
/*					:(C) Ubiquitous Corp. 2001-2013. All rights reserved.	*/
/****************************************************************************/

#ifndef __EENL_METER_G_H__
#define __EENL_METER_G_H__

/* 関数宣言 */
extern slong EENL_CreateDeviceMeter(EenlDeviceInfo_t* info, 
	ushort meterDevKind, ushort meterDevCnt);
extern slong EENL_UpdateDeviceMeter(EenlDeviceInfo_t* info);

extern slong EENL_E1MeterTimer0xDA(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_C2MeterTimer0xDA(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_E1MeterTimer0xE0_0xE1_0xE6(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_C2MeterTimer0xE0_0xE1_0xE6(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_E1MeterTimer0xE2_0xE3_0xE7(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_C2MeterTimer0xE2_0xE3_0xE7(const EPCS_OPESTTS_INFO_T* ope);

#endif	/*__EENL_METER_G_H__ */