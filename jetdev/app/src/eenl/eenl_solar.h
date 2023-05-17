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
#ifndef	__EENL_SOLAR_H__
#define	__EENL_SOLAR_H__

/* 関数宣言 */
static slong eenl_SolarUpdate0xE8(EenlDeviceInfo_t* info, EPCS_SPDATA_GET_T* spDataGet);
static slong eenl_SolarUpdate0xD0(EenlDeviceInfo_t* info, EPCS_OPESTTS_INFO_T* opeStts);
static slong eenl_SolarUpdate0xB0(EenlDeviceInfo_t* info);
static slong eenl_SolarUpdate0xB1(EenlDeviceInfo_t* info);

static slong eenl_SolarSetCheck0xA0(uchar* edt);
static slong eenl_SolarSetCheck0xC1(uchar* edt);

static slong eenl_SolarGetOutputCtrlHistry(uchar* val0xB0);

#endif	/* __EENL_SOLAR_H__ */
