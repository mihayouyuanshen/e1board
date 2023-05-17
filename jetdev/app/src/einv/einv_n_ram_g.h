/****************************************************************************/
/*	Description		:INV通信ミドル グローバル変数管理						*/
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
#ifndef __EINV_N_RAM_G_H__
#define __EINV_N_RAM_G_H__


/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "typedef.h"
#include "etim_g.h"
#include "einv_g.h"
#include "einv_msg_g.h"
#include "einv_n_g.h"
#include "einv_n_comm_g.h"


/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/
extern ushort			g_einvCommErrSeq;							/* PCS通信エラーシーケンス */
extern bool_t			g_einvCommErrFlg;							/* PCS通信不通確定フラグ */
extern bool_t			g_einvInitErrFlg;							/* INVマイコン初期化通信失敗確定フラグ */
extern bool_t			g_einvOpeStopFlg;							/* 運転停止 */

extern EinvCmd01T_t	g_einvCmd01_t;
extern EinvCmd01R_t	g_einvCmd01_r;

extern EinvCmd18T_t	g_einvCmd18_t;
extern EinvCmd19T_t	g_einvCmd19_t;
extern EinvCmd18R_t	g_einvCmd18_r;
extern EinvCmd19R_t	g_einvCmd19_r;

extern EinvCmd60T_t	g_einvCmd60_t;
extern EinvCmd60R_t	g_einvCmd60_r;

extern EinvCmd62T_t	g_einvCmd62_t;
extern EinvCmd62R_t	g_einvCmd62_r;

extern EinvCmd63T_t	g_einvCmd63_t;
extern EinvCmd63R_t	g_einvCmd63_r;

extern EinvCmd64T_t	g_einvCmd64_t;
extern EinvCmd65T_t	g_einvCmd65_t;
extern EinvCmd64R_t	g_einvCmd64_r;
extern EinvCmd65R_t	g_einvCmd65_r;

extern EinvCmd68T_t	g_einvCmd68_t;
extern EinvCmd69T_t	g_einvCmd69_t;
extern EinvCmd68R_t	g_einvCmd68_r;
extern EinvCmd69R_t	g_einvCmd69_r;

extern EinvCmd6AT_t	g_einvCmd6A_t;
extern EinvCmd6BT_t	g_einvCmd6B_t;
extern EinvCmd6AR_t	g_einvCmd6A_r;
extern EinvCmd6BR_t	g_einvCmd6B_r;

extern EinvCmd6DT_t	g_einvCmd6D_t;
extern EinvCmd6DR_t	g_einvCmd6D_r;

extern EinvCmd74T_t	g_einvCmd74_t;
extern EinvCmd75T_t	g_einvCmd75_t;
extern EinvCmd74R_t	g_einvCmd74_r;
extern EinvCmd75R_t	g_einvCmd75_r;

extern EinvCmd81T_t	g_einvCmd81_t;
extern EinvCmd81R_t	g_einvCmd81_r;

extern EinvCmd90T_t	g_einvCmd90_t;
extern EinvCmd90R_t	g_einvCmd90_r;

extern EinvCmd98T_t	g_einvCmd98_t;
extern EinvCmd98R_t	g_einvCmd98_r;

extern uchar g_btChargeMode;
extern uchar g_btDisChrgMode;
extern uchar g_needTestRun;
extern schar g_sysModeName[ EINV_SYS_MODEL_NAME_LEN ];
extern EinvMaintenanceDates_t g_mainteDates;

extern EinvUpdate_t		g_einvUpDateFlag;						/*運転モード変数更新管理*/

extern uchar	g_splPowerRunFlag;		/* V:指定電力運転フラグ */
extern sshort	g_pvGenPwrAvg;			/* PV発電電力(本製品以外のPV)平均値 */
extern ushort	g_pvPwr2Avg;			/* 太陽光発電電力(AC端計測値)平均値 */
extern sshort	g_revPwrFlowAvg;		/* 逆潮流電力平均値 */
extern slong	g_v2hInstantPowerAvg;	/* V2H瞬時充放電電力計測値(AC端計測値)平均値 */
extern slong	g_btChargeDischPwrAvg;	/* 蓄電池充放電電力(AC端計測値)平均値 */
extern slong	g_homeLoadPwr;			/* 家庭負荷電力 */
extern slong	g_pvPwrAvgSum;			/* 太陽光発電電力平均値sum */
extern sshort	g_arrayPvGenPwr[10];		/* array PV発電電力(本製品以外のPV) */
extern ushort	g_arrayPvPwr2[10];			/* array 太陽光発電電力(AC端計測値) */
extern sshort	g_arrayRevPwrFlow[10];		/* array 逆潮流電力 */
extern slong	g_arrayBtChargeDischPwr[10];/* array 蓄電池充放電電力(AC端計測値) */
extern slong	g_arrayV2hInstantPower[10];	/* array V2H瞬時充放電電力計測値(AC端計測値) */
extern uchar	g_indexPvGenPwr;			/* index PV発電電力(本製品以外のPV) */
extern uchar	g_indexPvPwr2;				/* index 太陽光発電電力(AC端計測値) */
extern uchar	g_indexRevPwrFlow;			/* index 逆潮流電力 */
extern uchar	g_indexBtChargeDischPwr;	/* index 蓄電池充放電電力(AC端計測値) */
extern uchar	g_indexV2hInstantPower;		/* index V2H瞬時充放電電力計測値(AC端計測値) */

/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern void EINV_n_InitNEinvGrobal(void);
extern void EINV_n_CalcNeedTestRun(void);
extern void EINV_n_CalcSysModeName(void);
extern void EINV_n_CalcMaintenanceDate(void);
/*merge from sunJT START*/
extern void  EINV_n_SetOperationStartStopValue(uchar operationStartStopValue);
extern uchar EINV_n_GetOperationStartStopValue(void);
extern void  EINV_n_SetCtlRunModeValue(uchar ctlRunMode);
extern uchar EINV_n_GetCtlRunModeValue(void);
extern void  EINV_n_SetAutoRunModeValue(uchar autoRunMode);
extern uchar EINV_n_GetAutoRunModeValue(void);
extern void  EINV_n_SetPowerOffRunModeValue(uchar powerOffRunMode);
extern uchar EINV_n_GetPowerOffRunModeValue(void);
extern void  EINV_n_SetBatteryRunmodeValue(uchar enlRunMode);
extern uchar EINV_n_GetBatteryRunmodeValue(void);
extern void  EINV_n_SetV2HRunmodeValue(uchar enlRunMode);
extern uchar EINV_n_GetV2HRunmodeValue(void);
extern void  EINV_n_SetMaintainStatusValue(uchar maintainStatus);
extern uchar EINV_n_GetMaintainStatusValue(void);
extern void  EINV_n_SetDischargePriorityValue(uchar setDischargePriority);
extern uchar EINV_n_GetDischargePriorityValue(void);
extern void EINV_n_SetSellChargStartTimeValue(const EinvTime_t * sellChargStartTime);
extern void EINV_n_GetSellChargStartTimeValue(EinvTime_t * sellChargStartTime);
extern void EINV_n_SetSellChargStopTimeValue(const EinvTime_t * sellChargStopTime);
extern void EINV_n_GetSellChargStopTimeValue(EinvTime_t * sellChargStopTime);
extern void EINV_n_SetSellDisChargStartTimeValue(const EinvTime_t * sellDisChargStartTime);
extern void EINV_n_GetSellDisChargStartTimeValue(EinvTime_t * sellDisChargStartTime);
extern void EINV_n_SetSellDisChargStopTimeValue(const EinvTime_t * sellDisChargStopTime);
extern void EINV_n_GetSellDisChargStopTimeValue(EinvTime_t * sellDisChargStopTime);
extern void EINV_n_SetGreenChargStartTimeValue(const EinvTime_t * greenChargStartTime);
extern void EINV_n_GetGreenChargStartTimeValue(EinvTime_t * greenChargStartTime);
extern void EINV_n_SetGreenChargStopTimeValue(const EinvTime_t * greenChargStopTime);
extern void EINV_n_GetGreenChargStopTimeValue(EinvTime_t * greenChargStopTime);
extern void EINV_n_SetGreenDisChargStartTimeValue(const EinvTime_t * greenDisChargStartTime);
extern void EINV_n_GetGreenDisChargStartTimeValue(EinvTime_t * greenDisChargStartTime);
extern void EINV_n_SetGreenDisChargStopTimeValue(const EinvTime_t * greenDisChargStopTime);
extern void EINV_n_GetGreenDisChargStopTimeValue(EinvTime_t * greenDisChargStopTime);
extern void  EINV_n_SetRunModeSetFromValue(uchar runModeSetFromData);
extern uchar EINV_n_GetRunModeSetFromValue(void);
extern void  EINV_n_SetENLRunmodeFromValue(EinvEnlSetFrom_t setENLRunModeFrom);
extern EinvEnlSetFrom_t EINV_n_GetENLRunmodeFromValue(void);
extern void  EINV_n_SetTestRunAvailableValue(uchar available);
extern uchar EINV_n_GetTestRunAvailableValue(void);
extern void  EINV_n_SetTestRunRequestValue(uchar request);
extern uchar EINV_n_GetTestRunRequestValue(void);
extern void EINV_n_SetTestRunSubStatusValue(TestRunSubFsmState_t status);
extern TestRunSubFsmState_t EINV_n_GetTestRunSubStatusValue(void);
extern void EINV_n_SetTestRunCmd75SendFinishValue(EinvFlag_t finishFlag);
extern EinvFlag_t EINV_n_GetTestRunCmd75SendFinishValue(void);
extern void EINV_n_SetTestRunCmd65SendFinishValue(EinvFlag_t finishFlag);
extern EinvFlag_t EINV_n_GetTestRunCmd65SendFinishValue(void);
extern void EINV_n_GetUpDateFlag(EinvUpdate_t * updateFlag);
extern void  EINV_n_ClrUpDateFlag(void);
extern void  EINV_n_SetSysStatusValue(EinvMainStatus_t mainStatus);
extern  EinvMainStatus_t EINV_n_GetSysStatusValue(void);
extern	void  EINV_n_SetSysStructValue(const EinvSysStruct_t * SysStructValue);
extern	void  EINV_n_GetSysStructValue(EinvSysStruct_t * sysStructValue);
extern  void  EINV_n_SetTestRunNeedValue(uchar need);
extern	uchar EINV_n_GetTestRunNeedValue(void);
extern slong EINV_n_CalcCmd65Request(void);
extern slong EINV_n_CalcCMD65Runmode(const EinvTime_t * nowTime,uchar cmd65RunMode,uchar * calcCmd65RunModeResult);
extern slong EINV_n_CalcCMD65PoweroffRunMode(uchar cmd65PowerOffRunMode,uchar * calcCmd65PowerOffRunModeResult);
extern slong EINV_n_CalcCMD75ConnecterStatus(uchar cmd75ConnecterStatus,uchar * calcCmd75ConnecterStatusResult);
extern slong EINV_n_CalcCMD65OpStartStop(uchar cmd65OpStartStop,uchar * calcCmd65OpStartStopResult);
extern slong EINV_n_CalcCMD65RunmodeByTimerEvent(EinvTimerEvent_t timerEvent,uchar	cmd65RunMode,uchar * calcCmd65RunModeResult);
extern uchar EINV_n_CalcRunModeByEnl(EinvEnlSetFromStatus_t enlSetFromStatus,uchar enlRunMode);
extern slong EINV_n_CalcRunmodeByCmd81res(const EinvV2hStatus_t * v2hStatus, uchar * cmd75ConnecterLockStatus, uchar * runMode, uchar * autoRunMode);
extern slong EINV_n_CalcRunmodeByCmd81resmanual(const Einv_ManualRunStatus_t * manualRunStatus, uchar * runMode);
extern uchar EINV_n_CalcNeedSendCmd(uchar cmdType);
extern void  EINV_n_TestRunCalcCmd65Para(EinvCmd65T_t * cmd65Para,EinvCmdParaSetOption_t cmd65ParaSetOption);
extern void  EINV_n_TestRunCalcCmd65ChangeFlagAndBackUpCmd65Para(EinvCmd65T_t * cmd65Para);
extern void  EINV_n_TestRunCalcCmd75Para(EinvCmd75T_t * cmd75Para);
extern void  EINV_n_TestRunCalcCmd75ChangeFlagAndBackUpCmd75Para(EinvCmd75T_t * cmd75Para);
extern uchar EINV_n_GetCmd81TestRunOutInfromValue(void);
/*merge from sunJT END*/
extern void EINV_n_CalcDisplayAvgValue(void);

#endif /*__EINV_N_RAM_G_H__*/
