/****************************************************************************/
/*	Description		:機器オブジェクトスーパークラス 処理 ヘッダ				*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jiayu.li											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/06												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、EENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:無し													*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef	__EENL_DEVICE_G_H__
#define	__EENL_DEVICE_G_H__

#include<eenl_cmn_g.h>

slong EENL_DevUpdate0x80(EPCS_OPESTTS_INFO_T* opeStts, void* dev_type);
slong EENL_DevUpdate0x83(schar* seri_no, void* dev_type);
slong EENL_DevUpdate0x8C(schar* mod_name, void* dev_type);
slong EENL_DevUpdate0x8D(schar* seri_no, void* dev_type);

slong EENL_DevSetCheck0x81(void* instance, uchar* pdc, uchar* edt);
slong EENL_DevSetCheck0x97(uchar* pdc, uchar* edt);
slong EENL_DevSetCheck0x98(uchar* pdc, uchar* edt);
#endif	/* __EENL_DEVICE_G_H__ */
