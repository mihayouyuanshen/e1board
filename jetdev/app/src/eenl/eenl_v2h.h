/****************************************************************************/
/*	Description		:ECHONET Lite application for device usage				*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC K.Jiang											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/20												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:eenl													*/
/*--------------------------------------------------------------------------*/
/*	Note			:E1														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(C) 2023. Nichicon Corporation. All Rights Reserved.	*/
/*					:(C) Ubiquitous Corp. 2001-2013. All rights reserved.	*/
/****************************************************************************/

#ifndef	__EENL_V2H_H__
#define	__EENL_V2H_H__

/* 関数宣言 */
static slong eenl_V2hUpdate0xC5(EenlDeviceInfo_t* info, EPCS_V2H_SETINFO_GET_T* v2h_set);
static slong eenl_V2hUpdate0xC6(EenlDeviceInfo_t* info, EPCS_V2H_SETINFO_GET_T* v2h_set);
static slong eenl_V2hUpdate0xC8(EenlDeviceInfo_t* info, EPCS_V2H_SETINFO_GET_T* v2h_set);
static slong eenl_V2hUpdate0xC9(EenlDeviceInfo_t* info, EPCS_V2H_SETINFO_GET_T* v2h_set);
static slong eenl_V2hUpdate0xCA(EenlDeviceInfo_t* info);
static slong eenl_V2hUpdate0xCB(EenlDeviceInfo_t* info);
static slong eenl_V2hUpdate0xCC(EenlDeviceInfo_t* info, EPCS_V2H_SETINFO_GET_T* v2h_set);
static slong eenl_V2hUpdate0xD2(EenlDeviceInfo_t* info);
static slong eenl_V2hUpdate0xE6(EenlDeviceInfo_t* info, EPCS_V2H_SETINFO_GET_T* v2h_set);
static slong eenl_V2hUpdate0xEA(EenlDeviceInfo_t* info, EPCS_V2H_SETINFO_GET_T* v2h_set);

static slong eenl_V2hSetCheck0xCD(uchar* pdc, uchar* edt);
static slong eenl_V2hSetCheck0xD7(uchar* pdc, uchar* edt);
static slong eenl_V2hSetCheck0xD9(uchar* pdc, uchar* edt);
static slong eenl_V2hSetCheck0xDA(uchar* pdc, uchar* edt);
static slong eenl_V2hSetCheck0xE7(uchar epc, uchar* pdc, uchar* edt);
static slong eenl_V2hSetCheck0xEA(uchar epc, uchar* pdc, uchar* edt);
static slong eenl_V2hSetCheck0xEB(uchar epc, uchar* pdc, uchar* edt);
static slong eenl_V2hSetCheck0xEC(uchar epc, uchar* pdc, uchar* edt);

#endif	/*__EENL_V2H_H__ */
