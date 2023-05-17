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

#ifndef	__EENL_V2H_G_H__
#define	__EENL_V2H_G_H__

/* 関数宣言 */
extern slong EENL_CreateDeviceV2h(EenlDeviceInfo_t* info);
extern slong EENL_UpdateEpcsV2h(EenlDeviceInfo_t* info);
extern slong EENL_V2hSetCheck(void* instance, uchar epc, uchar* pdc, uchar* edt);
extern slong EENL_GetV2hLocation(EenlGetLocation_t* fmsg);
extern slong EENL_SetV2hLocation(EenlSetLocation_t* fmsg);

extern slong EENL_V2hNotify0xC7(uchar val0xC7);
extern slong EENL_V2hNotify0xE7(ulong val0xE7);
extern slong EENL_V2hNotify0xEA(ulong val0xEA);
extern slong EENL_V2hNotify0xEB(ulong val0xEB);
extern slong EENL_V2hNotify0xEC(ulong val0xEC);
extern slong EENL_V2hNotify0xD2(ushort val0xD2);
extern slong EENL_V2hNotify0xDA(uchar val0xDA);

extern slong EENL_V2hTimer0xC2(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_V2hTimer0xC4(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_V2hTimer0xCF(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_V2hTimer0xD3(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_V2hTimer0xD6(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_V2hTimer0xD8(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_V2hTimer0xE2(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_V2hTimer0xE4(const EPCS_OPESTTS_INFO_T* ope);

/* E1 新規 */
extern slong EENL_V2hTimer0xDB(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_V2hTimer0xE1(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_V2hTimer0xCE(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_V2hTimer0xD0(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_V2hTimer0xC0(const EPCS_OPESTTS_INFO_T* ope);

/* TODO: 削除するかどうか 確認中*/
extern slong EENL_UpdateV2h0xDB(void);

extern void EENL_V2hTimerSetInfo(EenlDeviceInfo_t* info, const ENOT_ERRST_T* errst);

#endif	/*__EENL_V2H_G_H__ */
