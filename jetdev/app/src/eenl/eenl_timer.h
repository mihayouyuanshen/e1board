/****************************************************************************/
/*	Description		:ECHONET Lite application for device usage				*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC K.Jiang											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/18												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:eenl													*/
/*--------------------------------------------------------------------------*/
/*	Note			:E1														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(C) 2023. Nichicon Corporation. All Rights Reserved.	*/
/*					:(C) Ubiquitous Corp. 2001-2013. All rights reserved.	*/
/****************************************************************************/

#ifndef	__EENL_TIMER_H__
#define	__EENL_TIMER_H__

static slong eenl_AllTimer0x81(void);
static slong eenl_AllTimer0x86_0x88_0x89(slong getOpeSttsInfoRet, 
	const EPCS_OPESTTS_INFO_T* ope, const ENOT_ERRST_T* errst);
static slong eenl_AllTimer0x97_0x98(const schar* yyyymmdd, const schar* hhmm);
static slong eenl_AllTimer0xF5(const EPCS_OPESTTS_INFO_T* ope);

static slong eenl_pcsTimer0x83(const uchar* pcsSerialNumber);
static slong eenl_pcsTimer0x8D(const uchar* pcsSerialNumber);
static slong eenl_pcsTimer0xDB_0xD0(const EPCS_OPESTTS_INFO_T* ope);

#endif	/* __EENL_TIMER_H__ */
