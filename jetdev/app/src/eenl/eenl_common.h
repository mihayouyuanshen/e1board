/****************************************************************************/
/*	Description		:EENLの共通処理 ヘッダ									*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC DX.zhao											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/05/08												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、EENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:無し													*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EENL_COMMON_H__
#define __EENL_COMMON_H__

/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/

static sint eenl_CreateSocketRecv(const schar* cp);
static sint eenl_CreateSocketRecv6(const schar* cp);
static sint eenl_CreateSocketSend(const schar* cp);
static sint eenl_CreateSocketSend6(const schar* cp);
static void eenl_UnregisterDeviceInfo(EenlDeviceInfo_t* _info);
static void eenl_SetSelectedInstance(void* instance);
static void* eenl_GetSelectedInstance(void);
static void eenl_ThreadSend(const schar* cp);
static bool_t eenl_ThreadSendSub(const ENL_MESSAGE* msg, const EenlAddr_t* addr);
/* thread function */
static void eenl_ThreadRecv(const schar* cp);
static void eenl_ThreadRecv6(const schar* cp);
static void eenl_ThreadTcpRecv(const schar* cp);
static void eenl_ThreadMulticast(void);
static bool_t eenl_GetEpcValidFlg(void* instance, uchar epc);

/* dispose device */
static void eenl_DisposeDevice(EenlDeviceInfo_t* info);

/* process message */
static void eenl_ProcessMessage(EenlAddr_t* addr, uchar* buf, slong len);

static bool_t eenl_IsStrDigit(schar* digitStr, slong strSize);
static slong eenl_CheckChargeTimeZoneUWfunc(EenlHourMinute_t* hourMinute);
static slong eenl_CheckChargeTimeZonefunc(EPCS_CHARGE_TIME_ZONE_T* epcsTimeZone);

#endif
