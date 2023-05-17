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

#ifndef __EENL_METER_H__
#define __EENL_METER_H__
/*--------------------------------temp define----------------------------------------------------------*/
typedef struct
{
	EPCS_TIME_T currentTime;
	ulong chargeEnergy;
	ulong disChargeEnergy;
	ushort ttlDate;
	uchar NtpServerSyncStatus;
}Temp_t;


uchar s_eenlMeter0x81 = 0x00;		/* 分散型電源電力量メータ 設置場所(0x81) */

//typedef struct
//{
//
//};
/*--------------------------------temp define----------------------------------------------------------*/

/* 関数宣言 */
static slong eenl_E1MeterUpdate0xD0(EenlDeviceInfo_t* info, Temp_t* meterSet);
static slong eenl_C2MeterUpdate0xD0(EenlDeviceInfo_t* info, Temp_t* meterSet);
static slong eenl_E1MeterUpdate0xD1(EenlDeviceInfo_t* info, Temp_t* meterSet);
static slong eenl_C2MeterUpdate0xD1(EenlDeviceInfo_t* info, Temp_t* meterSet);
static slong eenl_E1MeterUpdate0xD2(EenlDeviceInfo_t* info, Temp_t* meterSet);
static slong eenl_C2MeterUpdate0xD2(EenlDeviceInfo_t* info, Temp_t* meterSet);
static slong eenl_E1MeterUpdate0xD3(EenlDeviceInfo_t* info, Temp_t* meterSet);
static slong eenl_C2MeterUpdate0xD3(EenlDeviceInfo_t* info, Temp_t* meterSet);
static slong eenl_E1MeterUpdate0xD4(EenlDeviceInfo_t* info, Temp_t* meterSet);
static slong eenl_C2MeterUpdate0xD4(EenlDeviceInfo_t* info, Temp_t* meterSet);
static slong eenl_E1MeterUpdate0xD5(EenlDeviceInfo_t* info, Temp_t* meterSet);
static slong eenl_C2MeterUpdate0xD5(EenlDeviceInfo_t* info, Temp_t* meterSet);
static slong eenl_E1MeterUpdate0xDB(EenlDeviceInfo_t* info, Temp_t* meterSet);
static slong eenl_C2MeterUpdate0xDB(EenlDeviceInfo_t* info, Temp_t* meterSet);

#endif	/*__EENL_METER_H__ */