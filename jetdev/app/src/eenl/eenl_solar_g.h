/****************************************************************************/
/*	Description		:機器オブジェクト太陽光発電クラス 処理 ヘッダ			*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jiayu.li											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/11												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、EENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:無し													*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef	__EENL_SOLAR_G_H__
#define	__EENL_SOLAR_G_H__

/* 関数宣言 */
extern slong EENL_CreateDeviceSolarNcc(EenlDeviceInfo_t* info);
extern slong EENL_UpdateEpcsSolarNcc(EenlDeviceInfo_t* info);
extern slong EENL_UpdateEschSolar(EenlDeviceInfo_t* info);
extern slong EENL_SolarSetCheck(void* instance, uchar epc, uchar* pdc, uchar* edt);

extern slong EENL_SolarCallbackRequestRead(uchar epc, uchar* edt);

extern slong EENL_SolarNotify0xB1(uchar* val0xB1);
extern slong EENL_SolarNotify0xB3(ushort val0xB3);
extern slong EENL_SolarNotify0xB4(ushort val0xB4);
extern slong EENL_SolarNotify0xC0(uchar val0xC0);
extern slong EENL_SolarNotify0xC1(uchar val0xC1);
extern slong EENL_SolarNotify0xC2(uchar val0xC2);
extern slong EENL_SolarNotify0xC3(ushort val0xC3);
extern slong EENL_SolarNotify0xD0(uchar val0xD0);
extern slong EENL_SolarNotify0xD1(uchar val0xD1);

extern slong EENL_SolarTimer0xA0(const EPCS_PV_CLASS_T* pvClassInfo);
extern slong EENL_SolarTimer0xE0(const EPCS_PV_CLASS_T* pvClassInfo);
extern slong EENL_SolarTimer0xE1(const EPCS_PV_CLASS_T* pvClassInfo);
extern slong EENL_SolarTimer0xE3(const EPCS_PV_CLASS_T* pvClassInfo);
extern slong EENL_SolarTimer0xD0(const EPCS_PV_CLASS_T* pvClassInfo);
extern slong EENL_SolarTimer0xD1(const EPCS_PV_CLASS_T* pvClassInfo);

#endif	/* __EENL_SOLAR_G_H__ */
