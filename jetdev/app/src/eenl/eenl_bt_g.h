/****************************************************************************/
/*	Description		:機器オブジェクト蓄電池クラス ヘッダ					*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC DX.zhao											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/27												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、EENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:無し													*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/

#ifndef	__EENL_BT_G_H__
#define	__EENL_BT_G_H__

extern slong EENL_CreateDeviceBattery(EenlDeviceInfo_t* info);
extern slong EENL_CreateUserDefineEpcNcc(EenlDeviceInfo_t* info);
/* ミドル層の設定をENLプロパティに反映(試運転完了判定となった後に１回だけ呼び出して初期設定を行う）*/
extern slong EENL_UpdateEpcsBattery(EenlDeviceInfo_t* info, EPCS_REG_INFO_T regInfo);
extern slong EENL_BatterySetCheck(void* instance, uchar epc, uchar* pdc, uchar* edt);

extern slong EENL_SpOpemodeChk(uchar opemode);
extern slong EENL_ElectricMoveModeChk(uchar opemode);

extern slong EENL_BtNotify0xAA(uchar val0xAA);
extern slong EENL_BtNotify0xAB(uchar val0xAB);
extern slong EENL_BtNotify0xC1(uchar val0xC1);
extern slong EENL_BtNotify0xC2(uchar val0xC2);
extern slong EENL_BtNotify0xCF(uchar val0xCF);
extern slong EENL_BtNotify0xDA(uchar val0xDA);
extern slong EENL_BtNotify0xDB(uchar val0xDB);
extern slong EENL_BtNotify0xE5(slong val0xE5);
extern slong EENL_BtNotify0xE6(slong val0xE6);
extern slong EENL_BtNotify0xE7_0xE8(const EenlEvtSttsChangeReq_t* cmsg);
extern slong EENL_BtNotify0xF0(uchar val0xF0);
extern slong EENL_BtNotify0xF2(uchar val0xF2);
extern slong EENL_BtNotify0xF4(uchar val0xF4_0, uchar val0xF4_1);
extern slong EENL_BtNotify0xF4_0xF8(
    const EenlEvtSttsChangeReq_t* cmsg, uchar mode);
extern slong EENL_BtNotify0xF8(uchar* val0xF8);

extern slong EENL_BtTimer0xA0_0xA1_0xA2_0xA3_0xA4_0xA5_0xA8_0xA9(
    const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_BtTimer0xD3(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_BtTimer0xD6(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_BtTimer0xD8(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_BtTimer0xE2(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_BtTimer0xE4(const EPCS_OPESTTS_INFO_T* ope);
extern slong EENL_BtTimer0xE5(const EPCS_OPESTTS_INFO_T* ope);


#endif	/* __EENL_BT_G_H__ */
