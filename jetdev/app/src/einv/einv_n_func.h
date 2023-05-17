/****************************************************************************/
/*	Description		:INV通信ミドル 機能処理									*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Dai.P												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/23												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:INV通信												*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EINV_N_FUNC_H__
#define __EINV_N_FUNC_H__


/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
/*関数RET値の定義*/
#define EINV_CMD_SENDNOREQUEST		((uchar)0x00u)
#define EINV_CMD65_SENDREQUEST		((uchar)0x01u)
#define EINV_CMD75_SENDREQUEST		((uchar)0x02u)

#define EINV_MONTHNUM_LEN				((uchar)12u)
#define EINV_YEARTYPE_LEN				((uchar)2u)

#define EINV_28DAY						((uchar)28u)
#define EINV_29DAY						((uchar)29u)
#define EINV_30DAY						((uchar)30u)
#define EINV_31DAY						((uchar)31u)

#define EINV_4MOD						((uchar)4u)
#define EINV_100MOD						((uchar)100u)
#define EINV_400MOD						((uchar)400u)

#define EINV_12MONTH					((uchar)12u)
#define EINV_1MONTH						((uchar)1u)

#define EINV_ONEMONTH					((uchar)1u)
#define EINV_ONEYEAR					((uchar)1u)
#define EINV_ONEDAY						((uchar)1u)


/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef struct _einv_opmodeset_t{
	uchar 					runMode;					/*運転モード*/
	uchar 					operationStartStopValue;	/*運転開始停止*/
	uchar 					powerOffRunMode;			/*非常時運転モード*/
	EinvTimeScope_t			chargeStartStop;			/*充電開始停止時刻*/	
	EinvTimeScope_t			dischargeStartStop;			/*放電開始停止時刻*/
	uchar			 		runModeSetFrom;				/*運転モード設定元*/
	EinvEnlSetFrom_t		enlSetFrom;					/*ENL運転モード設定元*/
	uchar					enlRunMode;					/*ENL運転モード*/
}EinvOpModeSet_t;

typedef struct _einv_usbmodeset_t{
	uchar 					ctlRunMode;					/*管理運転モード*/
	uchar 					powerOffRunMode;			/*非常時運転モード*/
	EinvTime_t				sellChargeStartTime;		/*売電モード充電開始時刻*/
	EinvTime_t				sellChargeStopTime;			/*売電モード充電停止時刻*/
	EinvTime_t				sellDisChargeStartTime;		/*売電モード放電開始時刻*/
	EinvTime_t				sellDisChargeStopTime;		/*売電モード放電停止時刻*/
	EinvTime_t				greenChargeStartTime;		/*グリーンモード充電開始時刻*/
	EinvTime_t				greenChargeStopTime;		/*グリーンモード充電停止時刻*/
	EinvTime_t				greenDisChargeStartTime;	/*グリーンモード放電開始時刻*/
	EinvTime_t				greenDisChargeStopTime;		/*グリーンモード放電停止時刻*/
}EinvUsbModeSet_t;


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/

typedef enum {
	EINV_DEFAULT = 0u,
	EINV_USB,
} EinvSetOption_t;


/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
static slong einv_n_SendCMD01Wrapper(EinvCmdDataT_t* cmdData, bool_t forceSend);
static slong einv_n_SendCMD60Wrapper(EinvCmdDataT_t* cmdData, bool_t forceSend);
static slong einv_n_SendCMD90Wrapper(EinvCmdDataT_t* cmdData, bool_t forceSend);
static slong einv_n_SendCMD64Wrapper(EinvCmdDataT_t* cmdData, bool_t forceSend);
static slong einv_n_SendCMD65Wrapper(EinvCmdDataT_t* cmdData, bool_t forceSend);
static slong einv_n_SendCMD75Wrapper(EinvCmdDataT_t* cmdData, bool_t forceSend);
static slong einv_n_SendCMD6BWrapper(EinvCmdDataT_t* cmdData, bool_t forceSend);
static slong  einv_n_OpModeSetFromSystemInit(const EinvUsbModeSet_t * usbModeSet,EinvSetOption_t setOption, EinvCmd65T_t* cmd65Data);
static slong  einv_n_OpModeSetFromExternalDevice(const EinvOpModeSet_t * opModeSet, EinvCmd65T_t* cmd65Data);
static slong  einv_n_OpModeAutoUpdateByTimerEvent(EinvTimerEvent_t timerEvent, EinvCmd65T_t* cmd65Data);
static slong  einv_n_OpModeAutoUpdateByCMD81ResV2h(const EinvV2hStatus_t * v2hStatus, EinvCmd65T_t* cmd65Data, EinvCmd75T_t* cmd75Data);
static slong  einv_n_OpModeAutoUpdateByCMD81ResManual(const Einv_ManualRunStatus_t * manualRunStatus, EinvCmd65T_t* cmd65Data);
static slong einv_n_SellModeTimerCtl(uchar opStartStop,const EtimTm_t * ptime);
static slong einv_n_GreenModeTimerCtl(uchar opStartStop,const EtimTm_t * ptime);
static slong einv_n_SellGreenModeSingleTimerCtl(EinvTimerEvent_t timerEvent,const EtimTm_t * ptime);
static void  einv_n_CalcFrmWorkTimerPara(const EinvTime_t * time,const EtimTm_t * ptime,FrmTime_t * frmtime);
static void  einv_n_UpdateFrmWorkTimerPara(const EinvTime_t * time,const EtimTm_t * ptime,FrmTime_t * frmtime);
#endif /*__EINV_N_FUNC_H__*/
