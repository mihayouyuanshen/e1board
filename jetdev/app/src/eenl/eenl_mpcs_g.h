/****************************************************************************/
/*	Description		:機器オブジェクトマルチ入力PCSクラス ヘッダ				*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jiayu.li											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/12												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、EENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:無し													*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef	__EENL_MPCS_G_H__
#define	__EENL_MPCS_G_H__

extern slong EENL_CreateDeviceMpcs(EenlDeviceInfo_t* info);
extern slong EENL_UpdateEpcsMpcs(EenlDeviceInfo_t* info);
extern slong EENL_MpcsSetCheck(void* instance, uchar epc, uchar* pdc, uchar* edt);

extern slong EENL_MpcsNotify0xD0(uchar val0xD0);

extern slong EENL_MpcsTimer0xE0(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_MpcsTimer0xE3(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_MpcsTimer0xE7(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_MpcsTimer0xD0(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_MpcsTimer0xF6(const EPCS_OPESTTS_INFO_T* ope);

#endif	/* __EENL_MPCS_G_H__ */
