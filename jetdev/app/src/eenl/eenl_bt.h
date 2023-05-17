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

#ifndef	__EENL_BT_H__
#define	__EENL_BT_H__

/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/

static slong eenl_BtUpdate0xC8(EenlDeviceInfo_t* info, EPCS_REG_INFO_T* regInfo);
static slong eenl_BtUpdate0xC9(EenlDeviceInfo_t* info, EPCS_REG_INFO_T* regInfo);
static slong eenl_BtUpdate0xD0_0xE7_0xE8(
    EenlDeviceInfo_t* info, EPCS_REG_INFO_T* regInfo);
static slong eenl_BtUpdate0xD2(EenlDeviceInfo_t* info, EPCS_REG_INFO_T* regInfo);
static slong eenl_BtUpdate0xD3(EenlDeviceInfo_t* info, EPCS_OPESTTS_INFO_T* opeStts);
static slong eenl_BtUpdate0xD6(EenlDeviceInfo_t* info, EPCS_OPESTTS_INFO_T* opeStts);
static slong eenl_BtUpdate0xD8(EenlDeviceInfo_t* info, EPCS_OPESTTS_INFO_T* opeStts);
static slong eenl_BtUpdate0xE2(EenlDeviceInfo_t* info, EPCS_OPESTTS_INFO_T* opeStts);
static slong eenl_BtUpdate0xE4(EenlDeviceInfo_t* info, EPCS_OPESTTS_INFO_T* opeStts);
static slong eenl_BtUpdate0xDA(EenlDeviceInfo_t* info, EPCS_OPEMODE_GET_T* ope);
static slong eenl_BtUpdate0xDB(EenlDeviceInfo_t* info, EPCS_OPESTTS_INFO_T* opeStts);
static slong eenl_BtUpdate0xDC(EenlDeviceInfo_t* info, EPCS_REG_INFO_T* regInfo);
static slong eenl_BtUpdate0xDD(EenlDeviceInfo_t* info, EPCS_REG_INFO_T* regInfo);
static slong eenl_BtUpdate0xF0(EenlDeviceInfo_t* info, EPCS_OPEMODE_GET_T* ope);
static slong eenl_BtUpdate0xF2(EenlDeviceInfo_t* info, EPCS_OPEMODE_GET_T* ope);
static slong eenl_BtUpdate0xF4(EenlDeviceInfo_t* info, EPCS_OPEMODE_GET_T* ope);
static slong eenl_BtUpdate0xF8(EenlDeviceInfo_t* info, EPCS_OPEMODE_GET_T* ope);

static slong eenl_BtSetCheck0xAA(uchar* pdc, uchar* edt);
static slong eenl_BtSetCheck0xAB(uchar* pdc, uchar* edt);
static slong eenl_BtSetCheck0xC1(uchar* pdc, uchar* edt);
static slong eenl_BtSetCheck0xC2(uchar* pdc, uchar* edt);
static slong eenl_BtSetCheck0xDA(uchar* pdc, uchar* edt);
static slong eenl_BtSetCheck0xE7(uchar* pdc, uchar* edt);
static slong eenl_BtSetCheck0xE8(uchar* pdc, uchar* edt);
static slong eenl_BtSetCheck0xEB(uchar* pdc, uchar* edt);
static slong eenl_BtSetCheck0xEC(uchar* pdc, uchar* edt);
static slong eenl_BtSetCheck0xF2(uchar* pdc, uchar* edt);
static slong eenl_BtSetCheck0xF4(uchar* pdc, uchar* edt);
static slong eenl_BtSetCheck0xF8(uchar* pdc, uchar* edt);

static uchar eenl_BtUpdateRetVal0xDA(uchar opeMode);
static uchar eenl_BtUpdateRetVal0xF0(uchar opeMode);

static slong eenl_BtNotify0xF4CreateData(
    uchar* p_buff0xF4, uchar val0xF4_0, uchar val0xF4_1);
static slong eenl_BtNotify0xF4CreateDataMatch(
    const EenlEvtSttsChangeReq_t* cmsg, uchar* val0xF4, uchar val);

static slong eenl_Bt0xF4CreateDataMatch(uchar mode, uchar* val0xF4);


#endif	/* __EENL_BT_H__ */
