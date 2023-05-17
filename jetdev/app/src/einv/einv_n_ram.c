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

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>

#include "typedef.h"
#include "task_g.h"
#include "dlog_g.h"
#include "common_g.h"

#include "einv_g.h"
#include "einv_msg_g.h"
#include "einv_n_g.h"
#include "einv_n_comm_g.h"
#include "einv_n_ram_g.h"
#include "einv_n_ram.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
ushort			g_einvCommErrSeq;								/* INV通信不通エラーシーケンス */
bool_t			g_einvCommErrFlg;								/* INV通信不通確定フラグ */
bool_t			g_einvInitErrFlg;								/* INVマイコン初期化通信失敗確定フラグ */
bool_t			g_einvOpeStopFlg;								/* 運転停止 */

EinvCmd01T_t	g_einvCmd01_t;
EinvCmd01R_t	g_einvCmd01_r;

EinvCmd18T_t	g_einvCmd18_t;
EinvCmd19T_t	g_einvCmd19_t;
EinvCmd18R_t	g_einvCmd18_r;
EinvCmd19R_t	g_einvCmd19_r;

EinvCmd60T_t	g_einvCmd60_t;
EinvCmd60R_t	g_einvCmd60_r;

EinvCmd62T_t	g_einvCmd62_t;
EinvCmd62R_t	g_einvCmd62_r;

EinvCmd63T_t	g_einvCmd63_t;
EinvCmd63R_t	g_einvCmd63_r;

EinvCmd64T_t	g_einvCmd64_t;
EinvCmd65T_t	g_einvCmd65_t;
EinvCmd64R_t	g_einvCmd64_r;
EinvCmd65R_t	g_einvCmd65_r;

EinvCmd68T_t	g_einvCmd68_t;
EinvCmd69T_t	g_einvCmd69_t;
EinvCmd68R_t	g_einvCmd68_r;
EinvCmd69R_t	g_einvCmd69_r;

EinvCmd6AT_t	g_einvCmd6A_t;
EinvCmd6BT_t	g_einvCmd6B_t;
EinvCmd6AR_t	g_einvCmd6A_r;
EinvCmd6BR_t	g_einvCmd6B_r;

EinvCmd6DT_t	g_einvCmd6D_t;
EinvCmd6DR_t	g_einvCmd6D_r;

EinvCmd74T_t	g_einvCmd74_t;
EinvCmd75T_t	g_einvCmd75_t;
EinvCmd74R_t	g_einvCmd74_r;
EinvCmd75R_t	g_einvCmd75_r;

EinvCmd81T_t	g_einvCmd81_t;
EinvCmd81R_t	g_einvCmd81_r;

EinvCmd90T_t	g_einvCmd90_t;
EinvCmd90R_t	g_einvCmd90_r;

EinvCmd98T_t	g_einvCmd98_t;
EinvCmd98R_t	g_einvCmd98_r;
uchar g_btChargeMode;
uchar g_btDisChrgMode;

/* 管理用パラメータ */
uchar s_needTestRun = 0;							/* 試運転要否 */
schar g_sysModeName[ EINV_SYS_MODEL_NAME_LEN ];		/* システム機種名 */
EinvMaintenanceDates_t g_mainteDates;				/* メンテナンス実施指定日 */

EinvUpdate_t		g_einvUpDateFlag;					/*運転モード関連変数更新管理*/

ushort	g_einvCarChargeDischargeLimitTime;				/*車両充放電上限時間 */
ushort	g_einvCarChargeLimitTime;						/*車両充電上限時間 */
ushort	g_einvCarDischargeLimitTime;					/*車両放電上限時間*/
ushort	g_einvAutoOpChargeTimeScope;					/*自動運転モード充電時間帯*/
ushort	g_einvCarChargeTime;							/*車両充電時間*/
ushort	g_einvCarDisChargeTime;							/*車両放電時間*/

uchar	g_splPowerRunFlag;								/* V:指定電力運転フラグ */
sshort	g_pvGenPwrAvg;									/* PV発電電力(本製品以外のPV)平均値[W] */
ushort	g_pvPwr2Avg;									/* 太陽光発電電力(AC端計測値)平均値[W] */
sshort	g_revPwrFlowAvg;								/* 逆潮流電力平均値[W] */
slong	g_v2hInstantPowerAvg;							/* V2H瞬時充放電電力計測値(AC端計測値)平均値[W] */
slong	g_btChargeDischPwrAvg;							/* 蓄電池充放電電力(AC端計測値)平均値[W] */
slong	g_homeLoadPwr;									/* 家庭負荷電力[W] */
slong	g_pvPwrAvgSum;									/* 太陽光発電電力平均値sum[W] */
sshort	g_arrayPvGenPwr[10];							/* array PV発電電力(本製品以外のPV) */
ushort	g_arrayPvPwr2[10];								/* array 太陽光発電電力(AC端計測値) */
sshort	g_arrayRevPwrFlow[10];							/* array 逆潮流電力 */
slong	g_arrayBtChargeDischPwr[10];					/* array 蓄電池充放電電力(AC端計測値) */
slong	g_arrayV2hInstantPower[10];						/* array V2H瞬時充放電電力計測値(AC端計測値) */
uchar	g_indexPvGenPwr;								/* index PV発電電力(本製品以外のPV) */
uchar	g_indexPvPwr2;									/* index 太陽光発電電力(AC端計測値) */
uchar	g_indexRevPwrFlow;								/* index 逆潮流電力 */
uchar	g_indexBtChargeDischPwr;						/* index 蓄電池充放電電力(AC端計測値) */
uchar	g_indexV2hInstantPower;							/* index V2H瞬時充放電電力計測値(AC端計測値) */

EinvSysStruct_t	g_einvSysStruct;						/*システム構成*/
uchar	s_einvTestRunAvailable;							/*試運転可能*/
uchar	s_einvTestRunRequest;							/*試運転指示*/
TestRunSubFsmState_t s_einvTestRunSubFsmState;			/*試運転サブ状態保存変数*/
uchar 	s_einvTestRunCmd75SendFinish;					/*試運転CMD75発行成功保存変数*/
uchar 	s_einvTestRunCmd65SendFinish;					/*試運転CMD65発行成功保存変数*/
/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
/*自動運転時間切替時刻変数20230324*/
EinvTime_t	s_einvSellChargStartTime;					/*V:売電モード充電開始時刻*/
EinvTime_t	s_einvSellChargStopTime;					/*V:売電モード充電停止時刻*/
EinvTime_t	s_einvSellDisChargStartTime;				/*V:売電モード放電開始時刻*/
EinvTime_t	s_einvSellDisChargStopTime;					/*V:売電モード放電停止時刻*/
EinvTime_t	s_einvGreenChargStartTime;					/*V:グリーンモード充電開始時刻*/
EinvTime_t	s_einvGreenChargStopTime;					/*V:グリーンモード充電停止時刻*/
EinvTime_t	s_einvGreenDisChargStartTime;				/*V:グリーンモード放電開始時刻*/
EinvTime_t	s_einvGreenDisChargStopTime;				/*V:グリーンモード放電停止時刻*/
uchar s_einvCtlRunMode;									/*V:通信ボード内管理運転モード*/
uchar s_einvAutoRunMode;								/*V:自動運転モード*/
uchar s_einvDischargePriority;							/*V:同時放電モード優先度*/
uchar s_einvRunModeSetFrom;								/*V:運転モード設定依頼元*/
uchar s_einvBatteryRunmode;								/*V:蓄電池クラス_運転モード*/
uchar s_einvV2HRunmode;									/*V:V2Hクラス_運転モード*/
uchar s_einvPowerOffRunMode;							/*非常時運転モード*/
uchar s_einvOperationStartStop;							/*運転停止指示変数*/
uchar s_einvButton1;									/*V2Hボタン①状態保存変数*/
uchar s_einvButton2;									/*V2Hボタン②状態保存変数*/
uchar s_einvButton3;									/*V2Hボタン③状態保存変数*/
uchar s_einvBattStatus;									/*蓄電池運転状態保存変数*/
uchar s_einvMaintainStatus;								/*Vメンテナンス状態保存変数*/
EinvEnlSetFrom_t	s_einvEnlSetFrom;					/*ENL CLASS設置元保存*/
EinvMainStatus_t    s_einvMainStatus;					/*システム状態管理変数*/

const uchar s_einvCmd65RunmodeTbl[EINV_MANUALMODE_LEN] =
{
	EINV_OPEMODE_7,
	EINV_OPEMODE_8,
	EINV_OPEMODE_C,
	EINV_OPEMODE_D,
	EINV_OPEMODE_F,
	EINV_OPEMODE_6
};

/*表 2-14ENL に指令値から算出1*/
const uchar s_einvConvertEnlRunmodeToCtlRunmodeTbl[EINV_ENLSETFROM_LEN][EINV_BATTRUNMODE_LEN][EINV_V2HRUNMODE_LEN] =
{
	{	/*蓄電池が後から設定*/
		{EINV_COMMU_BOARD_MANAGE_MODE_6,		EINV_COMMU_BOARD_MANAGE_MODE_3,		EINV_COMMU_BOARD_MANAGE_MODE_3,		EINV_COMMU_BOARD_MANAGE_MODE_3},
		{EINV_COMMU_BOARD_MANAGE_MODE_2,			EINV_COMMU_BOARD_MANAGE_MODE_2,		EINV_COMMU_BOARD_MANAGE_MODE_2,		EINV_COMMU_BOARD_MANAGE_MODE_2},
		{EINV_COMMU_BOARD_MANAGE_MODE_5,			EINV_COMMU_BOARD_MANAGE_MODE_4,		EINV_COMMU_BOARD_MANAGE_MODE_7,				EINV_COMMU_BOARD_MANAGE_MODE_7},
		{EINV_AUTO,					EINV_AUTO,				EINV_AUTO,				EINV_AUTO}
	},

	{	/*V2Hが後から設定*/
		{EINV_COMMU_BOARD_MANAGE_MODE_6,		EINV_COMMU_BOARD_MANAGE_MODE_4,		EINV_COMMU_BOARD_MANAGE_MODE_3,		EINV_AUTO},
		{EINV_COMMU_BOARD_MANAGE_MODE_5,			EINV_COMMU_BOARD_MANAGE_MODE_4,		EINV_COMMU_BOARD_MANAGE_MODE_2,		EINV_AUTO},
		{EINV_COMMU_BOARD_MANAGE_MODE_5,			EINV_COMMU_BOARD_MANAGE_MODE_4,		EINV_COMMU_BOARD_MANAGE_MODE_7,				EINV_AUTO},
		{EINV_COMMU_BOARD_MANAGE_MODE_5,			EINV_COMMU_BOARD_MANAGE_MODE_4,		EINV_COMMU_BOARD_MANAGE_MODE_7,				EINV_AUTO}
	}
};

/*表 2-14ENL に指令値から算出2*/
const uchar s_einvConvertEnlRunmodeToCtlRunmodeTblI[EINV_ENLSETFROM_LEN][EINV_ENLRUNMODE_LEN] =
{
	{EINV_COMMU_BOARD_MANAGE_MODE_3,	EINV_COMMU_BOARD_MANAGE_MODE_2,	EINV_COMMU_BOARD_MANAGE_MODE_7,	EINV_AUTO},/*BATT*/
	{EINV_COMMU_BOARD_MANAGE_MODE_5,		EINV_COMMU_BOARD_MANAGE_MODE_4,	EINV_COMMU_BOARD_MANAGE_MODE_7,	EINV_AUTO} /*V2H*/
};

/*表 2-11 運転モード算出*/
const uchar s_einvConvertCtlRunmodeAndTimerEventToCmd65RunmodeTbl[EINV_AUTORUNMODE_LEN][EINV_TIMEREVENT_LEN] =
{
	{EINV_OPEMODE_3,	EINV_OPEMODE_4,	EINV_OPEMODE_0, EINV_OPEMODE_4},/*SELL*/
	{EINV_OPEMODE_3,	EINV_OPEMODE_4,	EINV_OPEMODE_1, EINV_OPEMODE_4} /*GREEN*/
};

/*表 2-33 充電上限・放電下限値送信条件*/
const uchar s_einvSysStructAndCmdSendTbl[EINV_SYSSTRUCT_LEN][EINV_CMDTYPE_LEN] =
{	/*CMD65*/			/*CMD75*/
	{EINV_NEED,			EINV_NEED},		/*0:太陽光+蓄電池+V2H*/	
	{EINV_NEED,			EINV_NONEED},	/*1:太陽光+蓄電池*/
	{EINV_NEED,			EINV_NEED},		/*2:太陽光+V2H*/
	{EINV_NEED,			EINV_NEED},		/*3:蓄電池+V2H*/
	{EINV_NEED,			EINV_NONEED},	/*4:蓄電池*/
	{EINV_NEED,			EINV_NEED},		/*5:V2H*/
	{EINV_NEED,			EINV_NONEED},	/*6:太陽光*/
	{EINV_INVALIDVALUE,	EINV_INVALIDVALUE}	/*-:未設定 */
};

/*merge from sunJT end*/

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description	:グローバル変数の初期化										*/
/*--------------------------------------------------------------------------*/
/*	param		:なし														*/
/*--------------------------------------------------------------------------*/
/*	return		:なし														*/
/*--------------------------------------------------------------------------*/
/*	detail		:グローバル変数の初期化を行う。								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/27 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
void EINV_n_InitNEinvGrobal(void)
{
	DEBUG_TRCIN();

	g_einvCommErrSeq = 0;								/* INV通信不通エラーシーケンス */
	g_einvCommErrFlg = FALSE;							/* INV通信不通確定フラグ */
	g_einvInitErrFlg = FALSE;							/* INVマイコン初期化通信失敗確定フラグ */
	g_einvOpeStopFlg = FALSE;							/* 運転停止 */
	s_needTestRun = EINV_NONEED_TEST_RUN;				/* 試運転要否 */
	EINV_n_StrNCopy(g_sysModeName, EINV_SYS_MODEL_NAME_INIT, sizeof(g_sysModeName));	/* V:システム機種名 */
	memset(&g_mainteDates, 0, sizeof(g_mainteDates));	/* メンテナンス実施指定日 */

	memset(&g_einvCmd01_r, 0, sizeof(g_einvCmd01_r));
	memset(&g_einvCmd18_r, 0, sizeof(g_einvCmd18_r));
	memset(&g_einvCmd60_r, 0, sizeof(g_einvCmd60_r));
	memset(&g_einvCmd62_r, 0, sizeof(g_einvCmd62_r));
	memset(&g_einvCmd64_r, 0, sizeof(g_einvCmd64_r));
	memset(&g_einvCmd68_r, 0, sizeof(g_einvCmd68_r));
	memset(&g_einvCmd6A_r, 0, sizeof(g_einvCmd6A_r));
	memset(&g_einvCmd74_r, 0, sizeof(g_einvCmd74_r));
	/* memset(&g_einvCmd81_t, 0, sizeof(g_einvCmd81_t)); */
	memset(&g_einvCmd81_r, 0, sizeof(g_einvCmd81_r));
	memset(&g_einvCmd90_r, 0, sizeof(g_einvCmd90_r));
	memset(g_arrayPvGenPwr,			0, sizeof(g_arrayPvGenPwr));			/* arrayPV発電電力(本製品以外のPV) */
	memset(g_arrayPvPwr2, 			0, sizeof(g_arrayPvPwr2));				/* array太陽光発電電力(AC端計測値) */
	memset(g_arrayRevPwrFlow, 		0, sizeof(g_arrayRevPwrFlow));			/* array逆潮流電力 */
	memset(g_arrayV2hInstantPower, 	0, sizeof(g_arrayV2hInstantPower));		/* arrayV2H瞬時充放電電力計測値(AC端計測値) */
	memset(g_arrayBtChargeDischPwr, 0, sizeof(g_arrayBtChargeDischPwr));	/* array蓄電池充放電電力(AC端計測値) */
	g_indexPvGenPwr = 0;
	g_indexPvPwr2	= 0;
	g_indexRevPwrFlow = 0;
	g_indexBtChargeDischPwr = 0;
	g_indexV2hInstantPower = 0;

	/*****************************************************************************************************/
	EINV_n_StrNCopy(g_einvCmd01_r.modelFunctionName, EINV_MODEL_FUNCTION_NAME_INIT, sizeof(g_einvCmd01_r.modelFunctionName));	/* モデル機能名称 */
	/*****************************************************************************************************/
	g_einvCmd18_r.exterSunPwrOrnot = EINV_EXTER_SUN_PWR_0;								/* 外付け太陽光発電有無 */
	g_einvCmd18_r.sunBeyondPwrGenratDevice = EINV_SUN_BEYOND_PWR_GENRAT_0;				/* 太陽光以外の発電装置 */
	g_einvCmd18_r.loadConnectType = EINV_LOAD_CONNECT_TYPE_0;							/* 負荷接続タイプ */
	g_einvCmd18_r.pvPanelRating = EINV_PV_PANEL_RATING_INIT;							/* PVパネル定格 */
	g_einvCmd18_r.outputContrObject = EINV_OUTPUT_CONTR_OBJECT_NO;						/* 出力制御対象 */
	g_einvCmd18_r.eqpmtApproveCap = EINV_EQPMT_APPROVE_CAP_INIT;						/* 設備認定容量 */
	g_einvCmd18_r.genPwrUpperLimit = EINV_GEN_PWR_UPPER_LIMIT_INIT;						/* 発電電力上限設定(上限クリップ) */
	g_einvCmd18_r.fitContractType = EINV_FIT_CONTRACT_TYPE_0;							/* FIT契約タイプ */
	g_einvCmd18_r.selfConsumeType = EINV_SELF_CONSUME_TYPE_0;							/* 自家消費タイプ */
	EINV_n_StrNCopy(g_einvCmd18_r.pwrHouseId, EINV_POWER_HOUSE_INIT, sizeof(g_einvCmd18_r.pwrHouseId));							/* 発電所ID */
	g_einvCmd18_r.pwrCompanyServer = EINV_PWR_COMPANY_SERVER_0;							/* 電力会社サーバ */
	g_einvCmd18_r.commuBoardManageMode = EINV_COMMU_BOARD_MANAGE_MODE_0;				/* V:通信ボード内管理運転モード */
	g_einvCmd18_r.emergencyMode = EINV_EMGMODE_0;										/* 非常時運転モード */
	EINV_n_StrNCopy(g_einvCmd18_r.sellModeCharStartTime.hour, EINV_SELL_MODE_CHAR_START_HOUR_INIT, sizeof(g_einvCmd18_r.sellModeCharStartTime.hour));		/* 売電モード充電開始時刻(時) */
	EINV_n_StrNCopy(g_einvCmd18_r.sellModeCharStartTime.minute, EINV_SELL_MODE_CHAR_START_MINUTE_INIT, sizeof(g_einvCmd18_r.sellModeCharStartTime.minute));	/* 売電モード充電開始時刻(分) */
	EINV_n_StrNCopy(g_einvCmd18_r.sellModeCharStopTime.hour, EINV_SELL_MODE_CHAR_STOP_HOUR_INIT, sizeof(g_einvCmd18_r.sellModeCharStopTime.hour));			/* 売電モード充電停止時刻(時) */
	EINV_n_StrNCopy(g_einvCmd18_r.sellModeCharStopTime.minute, EINV_SELL_MODE_CHAR_STOP_MINUTE_INIT, sizeof(g_einvCmd18_r.sellModeCharStopTime.minute));	/* 売電モード充電停止時刻(分) */
	EINV_n_StrNCopy(g_einvCmd18_r.sellModeDisCStartTime.hour, EINV_SELL_MODE_DISC_START_HOUR_INIT, sizeof(g_einvCmd18_r.sellModeDisCStartTime.hour));		/* 売電モード放電開始時刻(時) */
	EINV_n_StrNCopy(g_einvCmd18_r.sellModeDisCStartTime.minute, EINV_SELL_MODE_DISC_START_MINUTE_INIT, sizeof(g_einvCmd18_r.sellModeDisCStartTime.minute));	/* 売電モード放電開始時刻(分) */
	EINV_n_StrNCopy(g_einvCmd18_r.sellModeDisCStopTime.hour, EINV_SELL_MODE_DISC_STOP_HOUR_INIT, sizeof(g_einvCmd18_r.sellModeDisCStopTime.hour));			/* 売電モード放電停止時刻(時) */
	EINV_n_StrNCopy(g_einvCmd18_r.sellModeDisCStopTime.minute, EINV_SELL_MODE_DISC_STOP_MINUTE_INIT, sizeof(g_einvCmd18_r.sellModeDisCStopTime.minute));	/* 売電モード放電停止時刻(分) */
	EINV_n_StrNCopy(g_einvCmd18_r.greenModeCharStartTime.hour, EINV_GREEN_MODE_CHAR_START_HOUR_INIT, sizeof(g_einvCmd18_r.greenModeCharStartTime.hour));	/* グリーンモード充電開始時刻(時) */
	EINV_n_StrNCopy(g_einvCmd18_r.greenModeCharStartTime.minute, EINV_GREEN_MODE_CHAR_START_MINUTE_INIT, sizeof(g_einvCmd18_r.greenModeCharStartTime.minute));/* グリーンモード充電開始時刻(分) */
	EINV_n_StrNCopy(g_einvCmd18_r.greenModeCharStopTime.hour, EINV_GREEN_MODE_CHAR_STOP_HOUR_INIT, sizeof(g_einvCmd18_r.greenModeCharStopTime.hour));		/* グリーンモード充電停止時刻(時) */
	EINV_n_StrNCopy(g_einvCmd18_r.greenModeCharStopTime.minute, EINV_GREEN_MODE_CHAR_STOP_MINUTE_INIT, sizeof(g_einvCmd18_r.greenModeCharStopTime.minute));	/* グリーンモード充電停止時刻(分) */
	EINV_n_StrNCopy(g_einvCmd18_r.greenModeDisCStartTime.hour, EINV_GREEN_MODE_DISC_START_HOUR_INIT, sizeof(g_einvCmd18_r.greenModeDisCStartTime.hour));	/* グリーンモード放電開始時刻(時) */
	EINV_n_StrNCopy(g_einvCmd18_r.greenModeDisCStartTime.minute, EINV_GREEN_MODE_DISC_START_MINUTE_INIT, sizeof(g_einvCmd18_r.greenModeDisCStartTime.minute));/* グリーンモード放電開始時刻(分) */
	EINV_n_StrNCopy(g_einvCmd18_r.greenModeDisCStopTime.hour, EINV_GREEN_MODE_DISC_STOP_HOUR_INIT, sizeof(g_einvCmd18_r.greenModeDisCStopTime.hour));		/* グリーンモード放電停止時刻(時) */
	EINV_n_StrNCopy(g_einvCmd18_r.greenModeDisCStopTime.minute, EINV_GREEN_MODE_DISC_STOP_MINUTE_INIT, sizeof(g_einvCmd18_r.greenModeDisCStopTime.minute));	/* グリーンモード放電停止時刻(分) */
	EINV_n_StrNCopy(g_einvCmd18_r.sunPwrRated, EINV_SUN_PWR_RATED_INIT, sizeof(g_einvCmd18_r.sunPwrRated));													/* 外付け太陽光発電の定格 */
	g_einvCmd18_r.sunPwrExtern = EINV_SUN_PWR_EXTERN_NO;								/* 外付け太陽光発電の有無 */
	g_einvCmd18_r.btCharUpperLimit = EINV_BT_UPPER_LIMIT_18_INIT;							/* 蓄電池充電上限設定値 */
	g_einvCmd18_r.btDiscLowerLimit = EINV_BT_LOWER_LIMIT_18_INIT;							/* 蓄電池放電下限設定値 */
	g_einvCmd18_r.carOpertPriority = EINV_CAR_OPERT_PRIORITY_YES;						/* 車両操作優先 */
	g_einvCmd18_r.carCharordiscBodyOprat = EINV_BODY_OPRAT_YES;							/* 車両充放電器本体操作 */
	g_einvCmd18_r.pwrcutDischar = EINV_PWRCUT_DISCHAR_YES;								/* 停電時放電動作 */
	g_einvCmd18_r.pwrcutDiscPrioritySet = EINV_PWRCUT_DISC_PRIORITY_BT;					/* 停電時放電優先設定 */
	g_einvCmd18_r.pwrcutCharPrioritySet = EINV_PWRCUT_CHAR_PRIORITY_CAR;				/* 停電時充電優先設定 */
	g_einvCmd18_r.carDiscLowerLimitSet = EINV_V2H_DISCHG_LOW_LMT_INIT;					/* 車両放電下限値設定 */
	g_einvCmd18_r.carCharUpperLimitSet = EINV_V2H_CHARGE_UP_LMT_INIT;					/* 車両充電上限値設定 */
	g_einvCmd18_r.carDiscOrnot = EINV_CAR_DISC_YES;										/* 車両放電有無 */
	g_einvCmd18_r.discharPrioritySet = EINV_DISCHAR_PRIORIT_BT;							/* 放電優先設定 */
	g_einvCmd18_r.pwrcutDiscLowerLimitSet = EINV_PWRCUT_V2H_DISC_LOW_INIT;				/* 停電時車両放電下限値設定 */
	g_einvCmd18_r.chargePrioritySet = EINV_CHAR_PRIORIT_CAR;							/* 充電優先設定 */
	g_einvCmd18_r.sunPwrSetLocation =  EINV_SUN_PWR_SET_LOCATION_0;						/* 太陽光発電設置場所 */
	g_einvCmd18_r.btSetLocation = EINV_BT_SET_LOCATION_0;								/* 蓄電池設置場所 */
	g_einvCmd18_r.v2hStandSetLocation = EINV_V2H_STAND_SET_LOCATION_0;					/* V2Hスタンド設置場所 */
	g_einvCmd18_r.electWaterHeating = EINV_ELECT_WATER_HEATING_NO;						/* 電気給湯器 */
	EINV_n_StrNCopy(g_einvCmd18_r.postalNum, EINV_POSTAL_NUM_INIT, sizeof(g_einvCmd18_r.postalNum));								/* 郵便番号 */
	g_einvCmd18_r.echonetLiteVer = EINV_ECHONET_LITE_VER_INIT;							/* ECHONET Lite_Ver */
	g_einvCmd18_r.opRemoteControlOrnot = EINV_OP_REMOTE_CONTROL_NO;						/* OPリモコン有無 */
	g_einvCmd18_r.btCharPwrMaxValueSet = EINV_BT_CHAR_PWR_MAX_SET_INIT;					/* 蓄電池最大充電電力設定 */
	g_einvCmd18_r.carCharPwrValueSet = EINV_CAR_CHAR_PWR_SET_INIT;						/* 車両最大充電電力設定 */
	g_einvCmd18_r.pwrcutOutSet = EINV_PWRCUT_VOLUNTARILY_OUT;							/* 停電時出力設定 */
	g_einvCmd18_r.ipAddrSet = EINV_IP_ADDRESS_SET_AUTO;									/* IPアドレス設定 */
	EINV_n_StrNCopy(g_einvCmd18_r.ipAddress, EINV_IP_ADDRESS_INIT, sizeof(g_einvCmd18_r.ipAddress));								/* IPアドレス */
	EINV_n_StrNCopy(g_einvCmd18_r.subnetMask, EINV_SUBNET_MASK_INIT, sizeof(g_einvCmd18_r.subnetMask));							/* サブネットマスク */
	EINV_n_StrNCopy(g_einvCmd18_r.defaultGateway, EINV_DEFAULT_GATEWAY_INIT, sizeof(g_einvCmd18_r.defaultGateway));					/* デフォルトゲートウェイ */
	EINV_n_StrNCopy(g_einvCmd18_r.poiorityDns, EINV_POIORITY_DNS_INIT, sizeof(g_einvCmd18_r.poiorityDns));							/* 優先DNS */
	EINV_n_StrNCopy(g_einvCmd18_r.replaceDns, EINV_REPLACE_DNS_INIT, sizeof(g_einvCmd18_r.replaceDns));							/* 代替DNS */
	g_einvCmd18_r.proxySet = EINV_PROXY_SET_OFF;										/* PROXY設定 */	
	EINV_n_StrNCopy(g_einvCmd18_r.proxyAddress, EINV_PROXY_ADDRESS_INIT, sizeof(g_einvCmd18_r.proxyAddress));						/* PROXYアドレス */
	EINV_n_StrNCopy(g_einvCmd18_r.portNumber, EINV_PORT_NUMBER_INIT, sizeof(g_einvCmd18_r.portNumber));							/* ポート番号 */
	g_einvCmd18_r.testrunBtChar = EINV_TESTRUN_BT_CHAR_0;								/* 試運転_蓄電池充電 */
	g_einvCmd18_r.testrunBtDisc = EINV_TESTRUN_BT_DISC_0;								/* 試運転_蓄電池放電 */
	g_einvCmd18_r.testrunBuyE1 = EINV_TESTRUN_BUY_E1_0;									/* 試運転_買電(E1-ACCT) */
	g_einvCmd18_r.testrunBuyC2 = EINV_TESTRUN_BUY_C2_0;									/* 試運転_買電(C2-ACCT) */
	g_einvCmd18_r.testrunCarChar = EINV_TESTRUN_CAR_CHAR_0;								/* 試運転_車両充電 */
	g_einvCmd18_r.testrunCarDisc = EINV_TESTRUN_CAR_DISC_0;								/* 試運転_車両放電 */
	g_einvCmd18_r.testrunInterSunPwrGerat = EINV_TESTRUN_INTER_SUN_PWR_GERAT_0;			/* 試運転_内部太陽光発電の動作 */
	g_einvCmd18_r.testrunExternSunPwrGerat = EINV_TESTRUN_EXTERN_SUN_PWR_GERAT_0;		/* 試運転_外付け太陽光発電の動作 */	
	g_einvCmd18_r.testrunSwitchPwrCutMode = EINV_TESTRUN_SWITCH_PWRCUT_MODE_0;			/* 試運転_切り替えモジュール動作(停電) */
	g_einvCmd18_r.testrunPutCutE1Disc = EINV_TESTRUN_PUTCUT_E1_DISC_0;					/* 試運転_停電時E1放電 */
	g_einvCmd18_r.testrunSwitchResetMode = EINV_TESTRUN_PUTCUT_SWITCH_RESET_MODE_0;		/* 試運転_切り替えモジュール動作(復帰) */
	g_einvCmd18_r.reserve = EINV_RESERVE_INIT;											/* 予約 */
	g_einvCmd18_r.clipAction = EINV_CLIP_ACTION_0;										/* クリップ動作 */
	EINV_n_StrNCopy(g_einvCmd18_r.mentanceDate.year, EINV_MENTANCE_DATE_YEAR_INIT, sizeof(g_einvCmd18_r.mentanceDate.year));				/* メンテ期間開始日 */
	EINV_n_StrNCopy(g_einvCmd18_r.mentanceDate.month, EINV_MENTANCE_DATE_MON_INIT, sizeof(g_einvCmd18_r.mentanceDate.month));				/* メンテ期間開始日 */
	EINV_n_StrNCopy(g_einvCmd18_r.mentanceDate.day, EINV_MENTANCE_DATE_DAY_INIT, sizeof(g_einvCmd18_r.mentanceDate.day));				/* メンテ期間開始日 */
	EINV_n_StrNCopy(g_einvCmd18_r.impltationDesignDay1.year, EINV_IMPLTATION_DESIGN_DAY1_YEAR_INIT, sizeof(g_einvCmd18_r.impltationDesignDay1.year));				/* 実施指定日1 */
	EINV_n_StrNCopy(g_einvCmd18_r.impltationDesignDay1.month, EINV_IMPLTATION_DESIGN_DAY1_MON_INIT, sizeof(g_einvCmd18_r.impltationDesignDay1.month));				/* 実施指定日1 */
	EINV_n_StrNCopy(g_einvCmd18_r.impltationDesignDay1.day, EINV_IMPLTATION_DESIGN_DAY1_DAY_INIT, sizeof(g_einvCmd18_r.impltationDesignDay1.day));				/* 実施指定日1 */
	EINV_n_StrNCopy(g_einvCmd18_r.impltationDesignDay2.year, EINV_IMPLTATION_DESIGN_DAY2_YEAR_INIT, sizeof(g_einvCmd18_r.impltationDesignDay2.year));				/* 実施指定日2 */
	EINV_n_StrNCopy(g_einvCmd18_r.impltationDesignDay2.month, EINV_IMPLTATION_DESIGN_DAY2_MON_INIT, sizeof(g_einvCmd18_r.impltationDesignDay2.month));				/* 実施指定日2 */
	EINV_n_StrNCopy(g_einvCmd18_r.impltationDesignDay2.day, EINV_IMPLTATION_DESIGN_DAY2_DAY_INIT, sizeof(g_einvCmd18_r.impltationDesignDay2.day));				/* 実施指定日2 */
	g_einvCmd18_r.executeMode = EINV_EXECUTE_MODE_USUAL;								/* 実行モード */
	g_einvCmd18_r.maintainResult = EINV_MAINTAIN_RESULT_NOT_IMPLMT;						/* メンテ結果 */
	/*****************************************************************************************************/
	EINV_n_StrNCopy(g_einvCmd60_r.sysMadeNum, EINV_SYS_MADE_NUM_INIT, sizeof(g_einvCmd60_r.sysMadeNum));							/* システム製造番号 */
	g_einvCmd60_r.btRateEltc = EINV_BT_RATE_ELTC_INIT;									/* 蓄電池定格電力量 */
	g_einvCmd60_r.pcsRateOut = EINV_PCS_RATE_OUT_INIT;									/* PCS定格出力 */
	g_einvCmd60_r.pcsRateVol = EINV_PCS_RATE_VOL_INIT;									/* PCS定格電圧 */
	g_einvCmd60_r.pcsRateVolAlone = EINV_PCS_RATE_VOL_ALONE_INIT;						/* PCS定格電圧_自立時 */
	EINV_n_StrNCopy(g_einvCmd60_r.setDate.year, EINV_SET_DATE_YEAR_INIT, sizeof(g_einvCmd60_r.setDate.year));						/* 設置日(年) */
	EINV_n_StrNCopy(g_einvCmd60_r.setDate.month, EINV_SET_DATE_MON_INIT, sizeof(g_einvCmd60_r.setDate.month));						/* 設置日(月) */
	EINV_n_StrNCopy(g_einvCmd60_r.setDate.day, EINV_SET_DATE_DAY_INIT, sizeof(g_einvCmd60_r.setDate.day));							/* 設置日(日) */
	EINV_n_StrNCopy(g_einvCmd60_r.invMcuFw, EINV_INV_MCU_FW_INIT, sizeof(g_einvCmd60_r.invMcuFw));									/* INVマイコン_FW */
	EINV_n_StrNCopy(g_einvCmd60_r.pvMcuFw, EINV_PV_MCU_FW_INIT, sizeof(g_einvCmd60_r.pvMcuFw));										/* PVマイコン_FW */
	EINV_n_StrNCopy(g_einvCmd60_r.bdcMcuFw, EINV_BDC_MCU_FW_INIT, sizeof(g_einvCmd60_r.bdcMcuFw));									/* BDCマイコン_FW */
	EINV_n_StrNCopy(g_einvCmd60_r.jbFw, EINV_JB_FW_INIT, sizeof(g_einvCmd60_r.jbFw));												/* J.B._FW */
	EINV_n_StrNCopy(g_einvCmd60_r.invMcuBootLoader, EINV_INV_MCU_BOOT_LOADER_INIT, sizeof(g_einvCmd60_r.invMcuBootLoader));			/* INVマイコン_ブートローダー */
	EINV_n_StrNCopy(g_einvCmd60_r.pvMcuBootLoader, EINV_PV_MCU_BOOT_LOADER_INIT, sizeof(g_einvCmd60_r.pvMcuBootLoader));			/* PVマイコン_ブートローダー */
	EINV_n_StrNCopy(g_einvCmd60_r.bdcMcuBootLoader, EINV_BDCMCUBOOTLOADER_INIT, sizeof(g_einvCmd60_r.bdcMcuBootLoader));			/* BDCマイコン_ブートローダー */
	g_einvCmd60_r.sysStruct = EINV_SYS_STRUCT_7;										/* システム構成 */
	EINV_n_StrNCopy(g_einvCmd60_r.btMadeNum, EINV_BT_MADE_NUM_INIT, sizeof(g_einvCmd60_r.btMadeNum));								/* 蓄電池ユニット製造番号 */
	g_einvCmd60_r.btRatedVol = EINV_BT_RATED_VOL_INIT;									/* 蓄電池定格電圧 */
	g_einvCmd60_r.btMaxChargePwr = EINV_BT_MAX_CHARGE_PWR_INIT;							/* 蓄電池最大充電電力値 */
	g_einvCmd60_r.btMaxDischarPwr = EINV_BT_MAX_DISCHAR_PWR_INIT;						/* 蓄電池最大放電電力値 */
	g_einvCmd60_r.btMaxChargePwrIndep = EINV_BT_MAX_CHARGE_PWR_INDEP_INIT;				/* 蓄電池最大充電電力値_自立時 */
	g_einvCmd60_r.btMaxDischarPwrIndep = EINV_BT_MAX_DISCHAR_PWR_INDEP_INIT;			/* 蓄電池最大放電電力値_自立時 */
	g_einvCmd60_r.sysFrequency = EINV_SYS_FREQUENCY_INIT;								/* 系統周波数(50Hz or 60Hz) */
	/*****************************************************************************************************/
	g_einvCmd64_r.opeMode = EINV_OPEMODE_6;												/* 運転モード */
	g_einvCmd64_r.emergencyMode =EINV_EMGMODE_0;										/* 非常時運転モード */
	g_einvCmd64_r.startOrStop =EINV_INV_STOP;											/* 運転／停止 */
	g_einvCmd64_r.btLowerLimit = EINV_BT_LOWER_LIMIT_INIT;								/* 蓄電池放電下限値設定 */
	g_einvCmd64_r.btUpperLimit = EINV_BT_UPPER_LIMIT_INIT;								/* 蓄電池充電上限値設定 */
	g_einvCmd64_r.testrunOutAtPwrCut =EINV_TEST_RUN_POWERCUT_OUT_NO;					/* 試運転時停電出力 */
	g_einvCmd64_r.testrunFlag =EINV_TEST_RUN_FLAG_NO;									/* 試運転指示フラグ */
	g_einvCmd64_r.chargePwrSet = EINV_CHARGE_PWR_SET_INIT;								/* 充電電力設定値(蓄電池) */
	g_einvCmd64_r.dischargePwrSet = EINV_DISCHG_PWR_SET_INIT;							/* 放電電力設定値(蓄電池) */
	/*****************************************************************************************************/
	g_einvCmd68_r.sysVoltDetectLevel = EINV_SYS_VOLT_DETECT_LEVEL_INIT;					/* 系統過電圧検出レベル */
	g_einvCmd68_r.sysVoltDetectTime = EINV_SYS_VOLT_DETECT_TIME_INIT;					/* 系統過電圧検出時間 */
	g_einvCmd68_r.sysUnderVoltDetectLevel = EINV_SYS_UNDER_VOLT_DETECT_LEVEL_INIT;		/* 系統不足電圧検出レベル */
	g_einvCmd68_r.sysUnderVoltDetectTime = EINV_SYS_UNDER_VOLT_DETECT_TIME_INIT;		/* 系統不足電圧検出時間 */
	g_einvCmd68_r.freRiseDetectLevel = EINV_FRE_RISE_DETECT_LEVEL_INIT;					/* 周波数上昇検出レベル */
	g_einvCmd68_r.freRiseDetectTime = EINV_FRE_RISE_DETECT_TIME_INIT;					/* 周波数上昇検出時間 */
	g_einvCmd68_r.freDropDetectLevel = EINV_FRE_DROP_DETECT_LEVEL_INIT;					/* 周波数低下検出レベル */
	g_einvCmd68_r.freDropDetectTime = EINV_FRE_DROP_DETECT_TIME_INIT;					/* 周波数低下検出時間 */
	g_einvCmd68_r.passIndOperDetect = EINV_PASS_IND_OPER_DETECT_INIT;					/* 受動的単独運転検出レベル */
	g_einvCmd68_r.resctTime = EINV_RESCT_TIME_INIT;										/* 復帰時間 */
	g_einvCmd68_r.changeTimeIncorDes = EINV_CHANGE_TIME_INCOR_DES_INIT;					/* 出力増減変化時間 */
	g_einvCmd68_r.volRsInhbValue = EINV_VOL_RS_INHB_VALUE_INIT;							/* 電圧上昇抑制値 */
	g_einvCmd68_r.volRsInhbattLimit = EINV_VOL_RS_INHBATT_LIMIT_INIT;					/* 電圧上昇抑制動作待機時限 */
	g_einvCmd68_r.cotPowerControl = EINV_COT_POWER_CONTROL_INIT;						/* 力率一定制御 */
	g_einvCmd68_r.freFeedbackGain = EINV_FRE_FEED_BACK_GAIN_INIT;						/* 周波数フィードバックゲイン */
	g_einvCmd68_r.freLevAllow = EINV_FRE_LEV_ALLOW_INIT;								/* 並列時許容周波数レベル */
	/*****************************************************************************************************/
	g_einvCmd6A_r.contaractPwrSet = EINV_CONTRACT_PWR_SET_INIT;							/* 契約電力設定値 */
	g_einvCmd6A_r.pwrcutOutSet = EINV_PWRCUT_VOLUNTARILY_OUT;							/* 停電時出力設定 */
	g_einvCmd6A_r.pwrcutOutStart = EINV_PWRCUT_OUT_STOP;								/* 停電時出力開始 */
	g_einvCmd6A_r.sellValidOrInvalid = EINV_SELL_INVALID;								/* 売電有効無効設定 */
	g_einvCmd6A_r.maintenanceIndicate = EINV_MAINTENANCE_INDICATE_STOP;					/* メンテ指示 */
	/*****************************************************************************************************/
	g_einvCmd74_r.carOpertPriority = EINV_CAR_OPERT_PRIORITY_YES;						/* 車両操作優先 */
	g_einvCmd74_r.carCharordiscBodyOprat = EINV_BODY_OPRAT_YES;							/* 車両充放電器本体操作 */
	g_einvCmd74_r.pwrcutDischar = EINV_PWRCUT_DISCHAR_YES;								/* 停電時放電動作 */
	g_einvCmd74_r.pwrcutDiscPrioritySet = EINV_PWRCUT_DISC_PRIORITY_BT;					/* 停電時放電優先設定 */
	g_einvCmd74_r.pwrcutCharPrioritySet = EINV_PWRCUT_CHAR_PRIORITY_CAR;				/* 停電時充電優先設定 */
	g_einvCmd74_r.connectLockOrUnlock = EINV_CONNECT_UNLOCK;							/* コネクタロック／解除 */
	g_einvCmd74_r.carDiscLowerLimitSet = EINV_V2H_DISCHG_LOW_LMT_INIT;					/* 車両放電下限値設定 */
	g_einvCmd74_r.carCharUpperLimitSet = EINV_V2H_CHARGE_UP_LMT_INIT;					/* 車両充電上限値設定 */
	g_einvCmd74_r.carDiscOrnot = EINV_CAR_DISC_YES;										/* 車両放電有無 */
	g_einvCmd74_r.discharPrioritySet = EINV_DISCHAR_PRIORIT_BT;							/* 放電優先設定 */
	g_einvCmd74_r.pwrcutDiscLowerLimitSet = EINV_PWRCUT_V2H_DISC_LOW_INIT;				/* 停電時車両放電下限値設定 */
	g_einvCmd74_r.chargePrioritySet = EINV_CHAR_PRIORIT_CAR;							/* 充電優先設定 */
	g_einvCmd74_r.carCharPwrValueSet = EINV_CAR_CHARGE_SET_INIT;						/* 車載電池充電電力設定値 */
	g_einvCmd74_r.carDischarPwrValueSet = EINV_CAR_DISCHG_SET_INIT;						/* 車載電池放電電力設定値 */
	/*****************************************************************************************************/
	g_einvCmd81_r.chargeDischargePwrAC = EINV_CHARGE_DISCHAR_PWR_AC_INIT;				/* PCS充放電電力（AC端計測値） */
	g_einvCmd81_r.opeState = EINV_PCS_OPESTATE_STOP;									/* PCS運転状態 */
	g_einvCmd81_r.ttlChargeEnergySysAC = EINV_TTL_CHAR_ENERGY_SYS_AC_INIT;				/* 積算PCS充電電力量(AC端計測値)(系統連系時) */
	g_einvCmd81_r.ttlDischargeEnergySysAC = EINV_TTL_DISCHAR_ENERGY_SYS_AC_INIT;		/* 積算PCS放電電力量(AC端計測値)(系統連系時) */
	g_einvCmd81_r.rsoc = EINV_RSOC_INIT;												/* rSOC */
	g_einvCmd81_r.remainingBt = EINV_REMAINING_BT_INIT;									/* 電池残量 */
	g_einvCmd81_r.btVolt = EINV_BT_VOLT_INIT;											/* 電池電圧 */
	g_einvCmd81_r.soh = EINV_SOH_INIT;													/* SOH */
	g_einvCmd81_r.btOperation = EINV_BATTERY_OPERATION_STOP;							/* 蓄電池運転動作状態 */
	g_einvCmd81_r.pwrCut = EINV_PWRCUT_0;												/* 停電 */
	g_einvCmd81_r.pvGenPwr = EINV_PV_GEN_PWR_INIT;										/* PV発電電力(本製品以外のPV) */
	g_einvCmd81_r.revPwrFlow = EINV_REV_PWR_FLOW_INIT;									/* 逆潮流電力 */
	g_einvCmd81_r.sysVolt = EINV_SYS_VOLT_INIT;											/* 系統電圧 */
	EINV_n_StrNCopy(g_einvCmd81_r.pcsErr, EINV_PCS_ERR_INIT, sizeof(g_einvCmd81_r.pcsErr));									/* PCSエラー */
	EINV_n_StrNCopy(g_einvCmd81_r.pcsErrRating, EINV_PCS_ERR_RATING_INIT, sizeof(g_einvCmd81_r.pcsErrRating));				/* PCSエラーレーティング */
	g_einvCmd81_r.opeMode = EINV_OPEMODE_6;												/* 運転モード */
	g_einvCmd81_r.chargeDischargePwrDC = EINV_CHAR_DISCHAR_PWR_DC_INIT;					/* DC蓄電池充放電電力2(コンバータ部) */
	g_einvCmd81_r.ttlChargeEnergyDC = EINV_TTL_CHAR_ENERGY_DC_INIT;						/* DC積算蓄電池充電電力量2(コンバータ部) */
	g_einvCmd81_r.ttlDischargeEnergyDC = EINV_TTL_DISCHAR_ENERGY_DC_INIT;				/* DC積算蓄電池放電電力量2(コンバータ部) */
	g_einvCmd81_r.ttlPvGenPwr = EINV_TTL_PV_GEN_PWR_INIT;								/* 積算PV発電電力量(本製品以外のPV) */
	g_einvCmd81_r.ttlPurchasePwr = EINV_TTL_PURCHASE_PWR_INIT;							/* 積算買電電力量 */
	g_einvCmd81_r.ttlSellingPwr = EINV_TTL_SELLING_PWR_INIT;							/* 積算売電電力量 */
	g_einvCmd81_r.genPwrLimit = EINV_GEN_PWR_LIMIT_INIT;								/* 発電電力制限値(kW) */
	g_einvCmd81_r.suppressState = EINV_SUPPRESS_STATE_0;								/* 出力抑制状態 */
	g_einvCmd81_r.carConnectorLockState = EINV_UNLOCK_STATE;							/* コネクタロック状態 */
	g_einvCmd81_r.carCharDiscOpratState = EINV_OPRATSTATE_0;							/* 車両充放電器運転動作状態 */
	g_einvCmd81_r.carDischargeableRemainingCapacity1 = EINV_CAR_DISCHAR_GEABLE_REMAIN_CAPACITY1_INIT;	/* 車載電池の放電可能残容量1 */
	g_einvCmd81_r.carDischargeableRemainingCapacity3 = EINV_CAR_DISCHAR_GEABLE_REMAIN_CAPACITY3_INIT;	/* 車載電池の放電可能残容量3 */
	g_einvCmd81_r.carChargeableRemainCap = EINV_CAR_CHAR_GEABLE_REMAIN_CAP_INIT;		/* 車載電池の充電可能残容量値 */
	g_einvCmd81_r.carAvailabilityStatus = EINV_STATUS_0XFF;								/* 車両接続充放電可否状態 */
	g_einvCmd81_r.v2hInstantPower = EINV_V2H_INSTANT_POWER_INIT;						/* V2H瞬時充放電電力計測値(AC端計測値) */
	g_einvCmd81_r.carTtlDischargePower = EINV_CAR_TTL_DISCHAR_POWER_INIT;				/* 車載電池の積算放電電力量計測値(AC端計測値)(トータル) */
	g_einvCmd81_r.carTtlChargePower = EINV_CAR_TTL_CHAR_POWER_INIT;						/* 車載電池の積算充電電力量計測値(AC端計測値)(トータル) */
	g_einvCmd81_r.emergencyMode = EINV_EMGMODE_0;										/* 非常時運転モード */
	g_einvCmd81_r.carRemainingCapacity1 = EINV_CAR_REMAIN_CAPACITY1_INIT;				/* 車載電池の電池残容量1 */
	g_einvCmd81_r.carRemainingCapacity3 = EINV_CAR_REMAIN_CAPACITY3_INIT;				/* 車載電池の電池残容量3 */
	g_einvCmd81_r.ttlChargeEnergyAloneAC = EINV_TTL_CHAR_ENERGY_ALONE_AC_INIT;			/* 積算PCS充電電力量(AC端計測値)(自立時) */
	g_einvCmd81_r.ttlDischargeEnergyAloneAC = EINV_TTL_DISCHAR_ENERGY_ALONE_AC_INIT;	/* 積算PCS放電電力量(AC端計測値)(自立時) */
	g_einvCmd81_r.pcsEfficiencyChar = EINV_PCS_EFFICIENCY_CHAR_INIT;					/* PCS効率(充電方向) */
	g_einvCmd81_r.pcsEfficiencyDischar = EINV_PCS_EFFICIENCY_DISCHAR_INIT;				/* PCS効率(放電方向) */
	g_einvCmd81_r.btCharDischarPwr = EINV_BT_CHAR_DISCHAR_PWR_INIT;						/* 蓄電池充放電電力(AC端計測値) */
	g_einvCmd81_r.pvPwr2 = EINV_PV_PWR2_INIT;											/* 太陽光発電電力(AC端計測値) */
	g_einvCmd81_r.ttlPvGenPwr2 = EINV_TTL_PV_GEN_PWR2_INIT;								/* 積算太陽光発電電力量(AC端計測値) */
	g_einvCmd81_r.cellVoltMIN = EINV_CELL_VOLT_MIN_INIT;								/* 最低セル電圧 */
	g_einvCmd81_r.cellAverageVolt = EINV_CELL_AVERAGE_VOLT_INIT;						/* 平均セル電圧 */
	g_einvCmd81_r.cellVoltMAX = EINV_CELL_VOLT_MAX_INIT;								/* 最高セル電圧 */
	g_einvCmd81_r.cellTempeMIN = EINV_CELL_TEMPE_MIN_INIT;								/* 最低セル温度 */
	g_einvCmd81_r.cellTempeMAX = EINV_CELL_TEMPE_MAX_INIT;								/* 最高セル温度 */
	g_einvCmd81_r.pvStringVolt1 = EINV_PV_STRING_VOLT1_INIT;							/* 太陽光発電ストリング電圧(1) */
	g_einvCmd81_r.pvStringVolt2 = EINV_PV_STRING_VOLT2_INIT;							/* 太陽光発電ストリング電圧(2) */
	g_einvCmd81_r.pvStringVolt3 = EINV_PV_STRING_VOLT3_INIT;							/* 太陽光発電ストリング電圧(3) */
	g_einvCmd81_r.pvStringVolt4 = EINV_PV_STRING_VOLT4_INIT;							/* 太陽光発電ストリング電圧(4) */
	g_einvCmd81_r.pvStringVolt5 = EINV_PV_STRING_VOLT5_INIT;							/* 太陽光発電ストリング電圧(5) */
	g_einvCmd81_r.testRunoutinfrom = EINV_TESTRUN_OFF;									/* 試運転状態通知 */
	g_einvCmd81_r.v2hOperationButton1 = EINV_V2H_OPERATION_BUTTON1_INIT;				/* V2H本体操作ボタン1 */
	g_einvCmd81_r.v2hOperationButton2 = EINV_V2H_OPERATION_BUTTON2_OFF;					/* V2H本体操作ボタン2 */
	g_einvCmd81_r.v2hOperationButton3 = EINV_V2H_OPERATION_BUTTON3_OFF;					/* V2H本体操作ボタン3 */
	g_einvCmd81_r.connectCarTypeOrManufacturer = EINV_TYPE_OR_MANUFACTURER_0;			/* 接続中の車種またはメーカー(特定車両判定) */
	g_einvCmd81_r.notChargeFlag = EINV_CHAR_FLAG_NOHAVE;								/* 充電不可フラグ */
	g_einvCmd81_r.notDisChargeFlag = EINV_DISCHAR_FLAG_NOHAVE;							/* 放電不可フラグ */
	g_einvCmd81_r.revCtCurrentU = EINV_REV_CT_CURRENT_U_INIT;							/* 逆潮CTの電流値(U相) */
	g_einvCmd81_r.revCtCurrentW = EINV_REV_CT_CURRENT_W_INIT;							/* 逆潮CTの電流値(W相) */
	g_einvCmd81_r.carbtDiscCapacityValue1 = EINV_CAR_BT_DISC_CAPACITY_VALUE1_INIT;		/* 車載電池の放電可能容量値1 */
	g_einvCmd81_r.carbtUseCapacityValue1 = EINV_CAR_BT_USE_CAPACITY_VALUE1_INIT;		/* 車載電池の使用容量値1 */
	g_einvCmd81_r.carbtCharCapacityValue = EINV_CAR_BT_CHAR_CAPACITY_VALUE_INIT;		/* 車載電池の充電可能容量値 */
	g_einvCmd81_r.btCharCopltNotice = EINV_CHAR_COPLT_NOTICE_NO;						/* 蓄電池充電完了通知 */
	g_einvCmd81_r.btDischarCopltNotice = EINV_DISCHAR_COPLT_NOTICE_NO;					/* 蓄電池放電完了通知 */
	g_einvCmd81_r.maintenanceBtState = EINV_BTSTATE_0;									/* メンテ電池状態 */
	/*****************************************************************************************************/
	EINV_n_StrNCopy(g_einvCmd90_r.carCharAndDiscModelName, EINV_CAR_CHAR_AND_DISC_MODEL_NAME_INIT, sizeof(g_einvCmd90_r.carCharAndDiscModelName));		/* 車両充放電器機種名 */
	EINV_n_StrNCopy(g_einvCmd90_r.carCharAndDiscMadeNum, EINV_CAR_CHAR_AND_DISC_MADE_NUM_INIT, sizeof(g_einvCmd90_r.carCharAndDiscMadeNum));			/* 車両充放電器製造番号 */
	g_einvCmd90_r.carCharAndDiscType = EINV_CAR_CHAR_AND_DISC_TYPE_INIT;				/* 車両充放電器タイプ */
	g_einvCmd90_r.carCharAndDiscRatedChar = EINV_CAR_CHAR_AND_DISC_RATED_CHAR_INIT;		/* 車両充放電器定格充電能力 */
	g_einvCmd90_r.carCharAndDiscRatedDisc = EINV_CAR_CHAR_AND_DISC_RATED_DISC_INIT;		/* 車両充放電器定格放電能力 */
	EINV_n_StrNCopy(g_einvCmd90_r.carId, EINV_CARID_INIT, sizeof(g_einvCmd90_r.carId));										/* 車両ID */
	EINV_n_StrNCopy(g_einvCmd90_r.evpcsFw, EINV_EVPCS_FW_INIT, sizeof(g_einvCmd90_r.evpcsFw));									/* EVPCS_FW */
	EINV_n_StrNCopy(g_einvCmd90_r.mcuFw, EINV_MCU_FW_INIT, sizeof(g_einvCmd90_r.mcuFw));										/* MCU_FW */
	EINV_n_StrNCopy(g_einvCmd90_r.c2CommuBoradFw, EINV_C2_COMMU_BORAD_FW_INIT, sizeof(g_einvCmd90_r.c2CommuBoradFw));					/* C2通信ボード_FW */
	/*****************************************************************************************************/
	/*merge from sunJT start*/
	/*運転モード変数更新管理*/
	g_einvUpDateFlag.CTLRUNMODE = EINV_CLR;
	g_einvUpDateFlag.AUTORUNMODE = EINV_CLR;
	g_einvUpDateFlag.SELLCHARGSTARTTIME = EINV_CLR;
	g_einvUpDateFlag.SELLCHARGSTOPTIME = EINV_CLR;
	g_einvUpDateFlag.SELLDISCHARGSTARTTIME = EINV_CLR;
	g_einvUpDateFlag.SELLDISCHARGSTOPTIME = EINV_CLR;
	g_einvUpDateFlag.GREENCHARGSTARTTIME = EINV_CLR;
	g_einvUpDateFlag.GREENCHARGSTOPTIME = EINV_CLR;
	g_einvUpDateFlag.GREENDISCHARGSTARTTIME = EINV_CLR;
	g_einvUpDateFlag.GREENDISCHARGSTOPTIME = EINV_CLR;
	g_einvUpDateFlag.RUNMODESETFROM = EINV_CLR;
	g_einvUpDateFlag.POWEROFFRUNMODE = EINV_CLR;
	g_einvUpDateFlag.BATTCLASSRUNMODE = EINV_CLR;
	g_einvUpDateFlag.V2HCLASSRUNMODE = EINV_CLR;
	g_einvUpDateFlag.DISCHARGEPRIORITY = EINV_CLR;
	g_einvUpDateFlag.ENLRUNMODEFROM = EINV_CLR;
	g_einvUpDateFlag.OPERATIONSTARTSTOP = EINV_CLR;
	g_einvUpDateFlag.MAINTAIN = EINV_CLR;
	g_einvUpDateFlag.MAINSTATUS = EINV_CLR;
	g_einvUpDateFlag.SYSSTRUCT = EINV_CLR;
	g_einvUpDateFlag.TESTRUNAVAILABLE = EINV_CLR;
	g_einvUpDateFlag.TESTRUNREQUEST = EINV_CLR;
	g_einvUpDateFlag.TESTRUNSTATUS = EINV_CLR;
	/*V:自動運転切替時刻初期化*/
	s_einvSellChargStartTime.hour = 1U;		// TODO
	s_einvSellChargStartTime.min = 0U;		// TODO
	s_einvSellChargStopTime.hour = 5U;		// TODO
	s_einvSellChargStopTime.min = 59U;		// TODO
	s_einvSellDisChargStartTime.hour = 6U;	// TODO
	s_einvSellDisChargStartTime.min = 0U;	// TODO
	s_einvSellDisChargStopTime.hour = 0U;	// TODO
	s_einvSellDisChargStopTime.min = 50U;		// TODO
	s_einvGreenChargStartTime.hour = 1U;		// TODO
	s_einvGreenChargStartTime.min = 0U;		// TODO
	s_einvGreenChargStopTime.hour = 1U;		// TODO
	s_einvGreenChargStopTime.min  = 1U;		// TODO
	s_einvGreenDisChargStartTime.hour = 1U;	// TODO
	s_einvGreenDisChargStartTime.min = 2U;	// TODO
	s_einvGreenDisChargStopTime.hour = 0U;	// TODO
	s_einvGreenDisChargStopTime.min = 50U;	// TODO
	s_einvRunModeSetFrom = EINV_SYSTEM;					/*V:運転モード設定依頼元:初期値システム*/
	s_einvBatteryRunmode = EINV_ENLIDLE;				/*V:蓄電池クラス_運転モード初期値：待機 */
	s_einvV2HRunmode = EINV_ENLIDLE;					/*V:V2Hクラス_運転モード初期値：待機*/
	s_einvCtlRunMode = EINV_COMMU_BOARD_MANAGE_MODE_1;						/*V:通信ボード内管理運転モード初期値：1:グリーンモード*/
	s_einvAutoRunMode = EINV_AUTOGREEN;					/*V:自動運転モード初期値：1:グリーンモード*/
	s_einvPowerOffRunMode = EINV_POWEROFFAUTOPOWERCTL;	/*LAN:停電時運転モード*/
	s_einvDischargePriority = EINV_ENLBATTERY;			/*V:同時放電モード優先度*/
	s_einvEnlSetFrom = EINV_INVALIDVALUE;				/*ENL CLASS設置元保存変数*/
	s_einvOperationStartStop = EINV_INVALIDVALUE;		/*運転開始停止指示変数初期値*/
	s_einvButton1 = EINV_INVALIDVALUE;					/*V2Hボタン①状態保存変数初期値：不定状態*/
	s_einvButton2 = EINV_INVALIDVALUE;					/*V2Hボタン②状態保存変数初期値：不定状態*/
	s_einvButton3 = EINV_INVALIDVALUE;					/*V2Hボタン③状態保存変数初期値：不定状態*/	
	s_einvBattStatus = EINV_INVALIDVALUE;				/*蓄電池運転状態保存変数初期値：不定状態*/
	
	s_einvMainStatus = EINV_INVALIDVALUE;				/*システム初期状態：不定状態*/
	s_einvMaintainStatus = EINV_INVALIDVALUE;			/*V:メンテ状態：不定状態*/
	
	g_einvSysStruct.sysStructAscii = EINV_INVALIDVALUE;	/*システム構成：不定状態*/
	g_einvSysStruct.sysStructNum = EINV_INVALIDVALUE;	/*システム構成：不定状態*/
	s_einvTestRunAvailable = EINV_INVALIDVALUE;  		/*試運転可能状態：不定状態*/
	s_einvTestRunRequest = EINV_INVALIDVALUE;  			/*試運転要求：不定状態*/
	s_einvTestRunSubFsmState = EINV_INVALIDVALUE;  		/*試運転サブ状態：不定状態*/
	s_einvTestRunCmd75SendFinish = EINV_INVALIDVALUE;	/*試運転CMD75発行状態：不定状態*/
	s_einvTestRunCmd65SendFinish = EINV_INVALIDVALUE;	/*試運転CMD65発行状態：不定状態*/
	/*merge from sunJT end*/
	/*****************************************************************************************************/
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description	:試運転要否判定処理											*/
/*--------------------------------------------------------------------------*/
/*	param		:なし														*/
/*--------------------------------------------------------------------------*/
/*	return		:なし														*/
/*--------------------------------------------------------------------------*/
/*	detail		:試運転要否判定処理を行う。									*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/17 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
void EINV_n_CalcNeedTestRun(void)
{
	sint fileFlag = -1;
	sint setDateYear = 0;
	sint setDateMonth = 0;
	sint setDateDay = 0;
	struct stat	st;

	DEBUG_TRCIN();

	s_needTestRun = EINV_NONEED_TEST_RUN;
	memset(&st, 0x00, sizeof(st));

	fileFlag = stat(EINV_TESTRUN_FINISH_FILE_PATH, &st);
	setDateYear = strcmp(g_einvCmd60_r.setDate.year, "--");
	setDateMonth = strcmp(g_einvCmd60_r.setDate.month, "--");
	setDateDay = strcmp(g_einvCmd60_r.setDate.day, "--");

	if (fileFlag == 0					/* 試運転済みファイルが無い */
		|| (setDateYear == 0				/* CMD60:設置日=”------" */
		&& setDateMonth == 0
		&& setDateDay == 0))
	{
		s_needTestRun = EINV_NEED_TEST_RUN;
	} 
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description	:システム機種名の算出処理									*/
/*--------------------------------------------------------------------------*/
/*	param		:なし														*/
/*--------------------------------------------------------------------------*/
/*	return		:なし														*/
/*--------------------------------------------------------------------------*/
/*	detail		:システム機種名の算出処理を行う。							*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/17 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
void EINV_n_CalcSysModeName(void)
{
	DEBUG_TRCIN();

	/* V:システム機種名初期化 */
	EINV_n_StrNCopy(g_sysModeName, EINV_SYS_MODEL_NAME_INIT, sizeof(g_sysModeName));

	if (g_einvCmd60_r.sysStruct == EINV_SYS_STRUCT_7)			/* "-":未設定 */
	{
		/* 初期値 */
	} 
	else if (g_einvCmd60_r.sysStruct == EINV_SYS_STRUCT_0		/* "0":太陽光+蓄電池+V2H */
		|| g_einvCmd60_r.sysStruct == EINV_SYS_STRUCT_1			/* "1":太陽光+蓄電池 */
		|| g_einvCmd60_r.sysStruct == EINV_SYS_STRUCT_3			/* "3":蓄電池+V2H */
		|| g_einvCmd60_r.sysStruct == EINV_SYS_STRUCT_4	)		/* "4":蓄電池 */
	{
		if (g_einvCmd60_r.btRateEltc == EINV_BT_RATE_ELTC_0768)
		{
			EINV_n_StrNCopy(g_sysModeName, EINV_SYS_MODEL_NAME_E1M1, sizeof(g_sysModeName));
		} 
		else if (g_einvCmd60_r.btRateEltc == EINV_BT_RATE_ELTC_0958)
		{
			EINV_n_StrNCopy(g_sysModeName, EINV_SYS_MODEL_NAME_E1L1, sizeof(g_sysModeName));
		} 
		else
		{
			/* 処理無し */
		}
	} 
	else if (g_einvCmd60_r.sysStruct == EINV_SYS_STRUCT_2		/* "2":太陽光+V2H */
		|| g_einvCmd60_r.sysStruct == EINV_SYS_STRUCT_5			/* "5":V2H */
		|| g_einvCmd60_r.sysStruct == EINV_SYS_STRUCT_6)		/* "6":太陽光 */
	{
		EINV_n_StrNCopy(g_sysModeName, EINV_SYS_MODEL_NAME_E1, sizeof(g_sysModeName));
	}
	else
	{
		/* 処理無し */
	}

	DLOG_Debug("g_sysModeName:%s", g_sysModeName);
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description	:PUART制御 初期化処理										*/
/*--------------------------------------------------------------------------*/
/*	param		:なし														*/
/*--------------------------------------------------------------------------*/
/*	return		:なし														*/
/*--------------------------------------------------------------------------*/
/*	detail		:PUART制御 初期化処理を行う。								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/17 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
void EINV_n_CalcMaintenanceDate(void)
{
	sint setDateYearInit = 0;
	sint setDateYearMax = 0;
	uchar setDateYear = 0U;
	slong i = EINV_SYS_MADE_NUM_LAST_DIGIT - 1;
	sint ret = 0;
	schar lastNum = 0;

	DEBUG_TRCIN();

	memset(&g_mainteDates, 0, sizeof(g_mainteDates));

	/* V:実施指定日1、V:実施指定日2は「年」「月」「日」に分けて算出を行う */
	setDateYearInit = strcmp(g_einvCmd60_r.setDate.year, "--");
	setDateYearMax = strcmp(g_einvCmd60_r.setDate.year, EINV_MAINTE_DATE_YEAR_MAX);
	/* 「年」 */
	if(setDateYearInit == 0)
	{
		EINV_n_StrNCopy(g_mainteDates.runAssignDate1.year, EINV_MAINTE_DATE_YEAR_MAX, sizeof(g_mainteDates.runAssignDate1.year));
		EINV_n_StrNCopy(g_mainteDates.runAssignDate2.year, EINV_MAINTE_DATE_YEAR_MAX, sizeof(g_mainteDates.runAssignDate2.year));
	}
	else if(setDateYearMax == 0)
	{
		EINV_n_StrNCopy(g_mainteDates.runAssignDate1.year, EINV_MAINTE_DATE_YEAR_MAX, sizeof(g_mainteDates.runAssignDate1.year));
		EINV_n_StrNCopy(g_mainteDates.runAssignDate2.year, EINV_MAINTE_DATE_YEAR_MAX, sizeof(g_mainteDates.runAssignDate2.year));
	}
	else
	{
		/* CMD60:設置日の「年」+1年 */
		setDateYear = (uchar)strtoul(g_einvCmd60_r.setDate.year, NULL, EINV_CMD_RES_BASE_DEC);
		setDateYear = setDateYear + EINV_MAINTE_DATE_YEAR_PLUS;
		/* V:実施予定日1=CMD60:設置日の「年」+1年 */
		snprintf(g_mainteDates.runAssignDate1.year, EINV_DATEYEAR_LEN, "%02u", setDateYear);
		/* V:実施予定日2=CMD60:設置日の「年」+1年 */
		snprintf(g_mainteDates.runAssignDate2.year, EINV_DATEYEAR_LEN, "%02u" ,setDateYear);
	}

	/* 「月」 */
	/* V:実施指定日1=6 */
	EINV_n_StrNCopy(g_mainteDates.runAssignDate1.month, EINV_ASSIG1_DATE_MON, sizeof(g_mainteDates.runAssignDate1.month));
	/* V:実施指定日2=9 */
	EINV_n_StrNCopy(g_mainteDates.runAssignDate2.month, EINV_ASSIG2_DATE_MON, sizeof(g_mainteDates.runAssignDate2.month));

	/* 「日」 */
	/* CMD60:システム製造番号の数字部分 下一桁目の数字 */
	ret = isdigit(g_einvCmd60_r.sysMadeNum[i]);
	if(ret != 0)
	{
		lastNum = g_einvCmd60_r.sysMadeNum[i];
	}
	else
	{
		DLOG_Error("g_einvCmd60_r.sysMadeNum[7] is not digit.\n");
	}

	switch(lastNum) {
	case 1:
		EINV_n_StrNCopy(g_mainteDates.runAssignDate1.day, EINV_MAINTE_DATE_DAY_1, sizeof(g_mainteDates.runAssignDate1.day));
		EINV_n_StrNCopy(g_mainteDates.runAssignDate2.day, EINV_MAINTE_DATE_DAY_1, sizeof(g_mainteDates.runAssignDate2.day));
		break;
	case 2:
		EINV_n_StrNCopy(g_mainteDates.runAssignDate1.day, EINV_MAINTE_DATE_DAY_2, sizeof(g_mainteDates.runAssignDate1.day));
		EINV_n_StrNCopy(g_mainteDates.runAssignDate2.day, EINV_MAINTE_DATE_DAY_2, sizeof(g_mainteDates.runAssignDate2.day));
		break;
	case 3:
		EINV_n_StrNCopy(g_mainteDates.runAssignDate1.day, EINV_MAINTE_DATE_DAY_3, sizeof(g_mainteDates.runAssignDate1.day));
		EINV_n_StrNCopy(g_mainteDates.runAssignDate2.day, EINV_MAINTE_DATE_DAY_3, sizeof(g_mainteDates.runAssignDate2.day));
		break;
	case 4:
		EINV_n_StrNCopy(g_mainteDates.runAssignDate1.day, EINV_MAINTE_DATE_DAY_4, sizeof(g_mainteDates.runAssignDate1.day));
		EINV_n_StrNCopy(g_mainteDates.runAssignDate2.day, EINV_MAINTE_DATE_DAY_4, sizeof(g_mainteDates.runAssignDate2.day));
		break;
	case 5:
		EINV_n_StrNCopy(g_mainteDates.runAssignDate1.day, EINV_MAINTE_DATE_DAY_5, sizeof(g_mainteDates.runAssignDate1.day));
		EINV_n_StrNCopy(g_mainteDates.runAssignDate2.day, EINV_MAINTE_DATE_DAY_5, sizeof(g_mainteDates.runAssignDate2.day));
		break;
	case 6:
		EINV_n_StrNCopy(g_mainteDates.runAssignDate1.day, EINV_MAINTE_DATE_DAY_6, sizeof(g_mainteDates.runAssignDate1.day));
		EINV_n_StrNCopy(g_mainteDates.runAssignDate2.day, EINV_MAINTE_DATE_DAY_6, sizeof(g_mainteDates.runAssignDate2.day));
		break;
	case 7:
		EINV_n_StrNCopy(g_mainteDates.runAssignDate1.day, EINV_MAINTE_DATE_DAY_7, sizeof(g_mainteDates.runAssignDate1.day));
		EINV_n_StrNCopy(g_mainteDates.runAssignDate2.day, EINV_MAINTE_DATE_DAY_7, sizeof(g_mainteDates.runAssignDate2.day));
		break;
	case 8:
		EINV_n_StrNCopy(g_mainteDates.runAssignDate1.day, EINV_MAINTE_DATE_DAY_8, sizeof(g_mainteDates.runAssignDate1.day));
		EINV_n_StrNCopy(g_mainteDates.runAssignDate2.day, EINV_MAINTE_DATE_DAY_8, sizeof(g_mainteDates.runAssignDate2.day));
		break;
	case 9:
		EINV_n_StrNCopy(g_mainteDates.runAssignDate1.day, EINV_MAINTE_DATE_DAY_9, sizeof(g_mainteDates.runAssignDate1.day));
		EINV_n_StrNCopy(g_mainteDates.runAssignDate2.day, EINV_MAINTE_DATE_DAY_9, sizeof(g_mainteDates.runAssignDate2.day));
		break;
	case 0:
		EINV_n_StrNCopy(g_mainteDates.runAssignDate1.day, EINV_MAINTE_DATE_DAY_0, sizeof(g_mainteDates.runAssignDate1.day));
		EINV_n_StrNCopy(g_mainteDates.runAssignDate2.day, EINV_MAINTE_DATE_DAY_0, sizeof(g_mainteDates.runAssignDate2.day));
		break;
	default:
		EINV_n_StrNCopy(g_mainteDates.runAssignDate1.day, EINV_MAINTE_DATE_DAY_OTHER, sizeof(g_mainteDates.runAssignDate1.day));
		EINV_n_StrNCopy(g_mainteDates.runAssignDate2.day, EINV_MAINTE_DATE_DAY_OTHER, sizeof(g_mainteDates.runAssignDate2.day));
		break;
	}

	/* V:メンテ期間開始日はV:実施指定日1とする */
	g_mainteDates.mainteStartDate = g_mainteDates.runAssignDate1;

	DEBUG_TRCOUT();
}

/*merge from sunJT start*/
/*==========================================================================*/
/*	Description		:	運転開始停止変数設置								*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	operationStartStopValue								*/
/*	param	out		:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	運転開始停止変数設置								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetOperationStartStopValue(uchar operationStartStopValue)
{
	DEBUG_TRCIN();
	
	if (s_einvOperationStartStop != operationStartStopValue)
	{
		s_einvOperationStartStop = operationStartStopValue;
		g_einvUpDateFlag.OPERATIONSTARTSTOP = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.OPERATIONSTARTSTOP = EINV_CLR;
	}

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	取得運転開始停止変数								*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	g_einvOperationStartStop							*/
/*--------------------------------------------------------------------------*/
/*	detail			:	取得運転開始停止変数								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
uchar EINV_n_GetOperationStartStopValue(void)
{
	uchar ret;
	
	DEBUG_TRCIN();

	ret = s_einvOperationStartStop;

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	管理運転モード変数設置								*/
/*--------------------------------------------------------------------------*/
/*	param			:	ctlRunMode管理運転変数設定値						*/
/*--------------------------------------------------------------------------*/
/*	return			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	管理運転モード変数設置								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetCtlRunModeValue(uchar ctlRunMode)
{
	DEBUG_TRCIN();
	
	if (s_einvCtlRunMode != ctlRunMode)
	{
		s_einvCtlRunMode = ctlRunMode;
		g_einvUpDateFlag.CTLRUNMODE = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.CTLRUNMODE = EINV_CLR;
	}
	
	DEBUG_TRCOUT();
	                     
}

/*==========================================================================*/
/*	Description		:	取得管理運転モード変数								*/
/*--------------------------------------------------------------------------*/
/*	param			:	void												*/
/*--------------------------------------------------------------------------*/
/*	return			:	g_einvCtlRunMode									*/
/*--------------------------------------------------------------------------*/
/*	detail			:	取得管理運転モード変数								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
uchar  EINV_n_GetCtlRunModeValue(void)
{
	uchar ret;
	
	DEBUG_TRCIN();

	ret = s_einvCtlRunMode;

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	自動運転モード変数設置								*/
/*--------------------------------------------------------------------------*/
/*	param			:	autoRunMode											*/
/*--------------------------------------------------------------------------*/
/*	return			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	自動運転モード変数設置								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetAutoRunModeValue(uchar autoRunMode)
{
	DEBUG_TRCIN();
	
	if (s_einvAutoRunMode != autoRunMode)
	{
		s_einvAutoRunMode = autoRunMode;
		g_einvUpDateFlag.AUTORUNMODE = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.AUTORUNMODE = EINV_CLR;
	}

	DEBUG_TRCOUT();
	
}
/*==========================================================================*/
/*	Description		:	取得自動運転モード変数								*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	ret													*/
/*--------------------------------------------------------------------------*/
/*	detail			:	取得自動運転モード変数								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
uchar EINV_n_GetAutoRunModeValue(void)
{
	uchar ret;
	
	DEBUG_TRCIN();
	ret = s_einvAutoRunMode;
	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	非常時運転モード設置								*/
/*--------------------------------------------------------------------------*/
/*	param			:	powerOffRunMode										*/
/*--------------------------------------------------------------------------*/
/*	return			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	非常時運転モード設置								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetPowerOffRunModeValue(uchar powerOffRunMode)
{
	DEBUG_TRCIN();
	
	if (s_einvPowerOffRunMode != powerOffRunMode)
	{
		s_einvPowerOffRunMode = powerOffRunMode;
		g_einvUpDateFlag.POWEROFFRUNMODE = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.POWEROFFRUNMODE = EINV_CLR;
	}

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	取得非常時運転モード設置							*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	ret													*/
/*--------------------------------------------------------------------------*/
/*	detail			:	取得非常時運転モード設置							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
uchar EINV_n_GetPowerOffRunModeValue(void)
{
	uchar ret;
	
	DEBUG_TRCIN();
	ret = s_einvPowerOffRunMode;

	DEBUG_TRCOUT();
	return ret;
}
/*==========================================================================*/
/*	Description		:	ENL蓄電池運転モード変数設置							*/
/*--------------------------------------------------------------------------*/
/*	param			:	enlRunMode											*/
/*--------------------------------------------------------------------------*/
/*	return			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ENL蓄電池運転モード変数設置							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void  EINV_n_SetBatteryRunmodeValue(uchar enlRunMode)
{
	DEBUG_TRCIN();
	
	if (s_einvBatteryRunmode != enlRunMode)
	{
		s_einvBatteryRunmode = enlRunMode;
		g_einvUpDateFlag.BATTCLASSRUNMODE = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.BATTCLASSRUNMODE = EINV_CLR;
	}

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	ENL蓄電池運転モード変数取得							*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	g_einvBatteryRunmode								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ENL蓄電池運転モード変数取得							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
uchar EINV_n_GetBatteryRunmodeValue(void)
{
	uchar ret;
	
	DEBUG_TRCIN();

	ret = s_einvBatteryRunmode;

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	ENLV2H運転モード変数設置							*/
/*--------------------------------------------------------------------------*/
/*	param			:	enlRunMode											*/
/*--------------------------------------------------------------------------*/
/*	return			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ENLV2H運転モード変数設置							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void  EINV_n_SetV2HRunmodeValue(uchar enlRunMode)
{
	DEBUG_TRCIN();

	if (s_einvV2HRunmode != enlRunMode)
	{
		s_einvV2HRunmode = enlRunMode;
		g_einvUpDateFlag.V2HCLASSRUNMODE = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.V2HCLASSRUNMODE = EINV_CLR;
	}

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	ENLV2H運転モード変数設置取得						*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	g_einvV2HRunmode									*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ENLV2H運転モード変数設置取得						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
uchar EINV_n_GetV2HRunmodeValue(void)
{
	uchar ret;
	
	DEBUG_TRCIN();

	ret = s_einvV2HRunmode;

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	SELLモード充電開始時刻変数設置						*/
/*--------------------------------------------------------------------------*/
/*	param			:	sellChargStartTime									*/
/*--------------------------------------------------------------------------*/
/*	return			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	SELLモード充電開始時刻変数設置						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetSellChargStartTimeValue(const EinvTime_t* sellChargStartTime)
{
	DEBUG_TRCIN();
	
	if ((s_einvSellChargStartTime.hour != sellChargStartTime->hour) ||
		(s_einvSellChargStartTime.min != sellChargStartTime->min))
	{
		s_einvSellChargStartTime.hour = sellChargStartTime->hour;
		s_einvSellChargStartTime.min = sellChargStartTime->min;
		g_einvUpDateFlag.SELLCHARGSTARTTIME = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.SELLCHARGSTARTTIME = EINV_CLR;
	}
	
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	取得SELLモード充電開始時刻変数						*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	g_einvSellChargStartTime							*/
/*--------------------------------------------------------------------------*/
/*	detail			:	取得SELLモード充電開始時刻変数						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_GetSellChargStartTimeValue(EinvTime_t * sellChargStartTime)
{	
	
	DEBUG_TRCIN();

	sellChargStartTime->hour = s_einvSellChargStartTime.hour;
	sellChargStartTime->min  = s_einvSellChargStartTime.min;

	DEBUG_TRCOUT();

}
/*==========================================================================*/
/*	Description		:	SELLモード充電停止時刻変数設置						*/
/*--------------------------------------------------------------------------*/
/*	param			:	sellChargStopTime									*/
/*--------------------------------------------------------------------------*/
/*	return			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	SELLモード充電停止時刻変数設置						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetSellChargStopTimeValue(const EinvTime_t * sellChargStopTime)
{
	DEBUG_TRCIN();
	
	if ((s_einvSellChargStopTime.hour != sellChargStopTime->hour) ||
		(s_einvSellChargStopTime.min != sellChargStopTime->min))
	{
		s_einvSellChargStopTime.hour = sellChargStopTime->hour;
		s_einvSellChargStopTime.min = sellChargStopTime->min;
		g_einvUpDateFlag.SELLCHARGSTOPTIME = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.SELLCHARGSTOPTIME = EINV_CLR;
	}
	
	DEBUG_TRCOUT();
	                     
}
/*==========================================================================*/
/*	Description		:	SELLモード充電停止時刻変数取得						*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	g_einvSellChargStopTime								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	SELLモード充電停止時刻変数取得						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_GetSellChargStopTimeValue(EinvTime_t * sellChargStopTime)
{
	
	DEBUG_TRCIN();
	
	sellChargStopTime->hour = s_einvSellChargStopTime.hour;
	sellChargStopTime->min = s_einvSellChargStopTime.min;

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	SELLモード放電開始時刻変数設置						*/
/*--------------------------------------------------------------------------*/
/*	param			:	sellDisChargStartTime								*/
/*--------------------------------------------------------------------------*/
/*	return			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	SELLモード放電開始時刻変数設置						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetSellDisChargStartTimeValue(const EinvTime_t * sellDisChargStartTime)
{
	
	DEBUG_TRCIN();
	
	if ((s_einvSellDisChargStartTime.hour != sellDisChargStartTime->hour) ||
		(s_einvSellDisChargStartTime.min != sellDisChargStartTime->min))
	{
		s_einvSellDisChargStartTime.hour = sellDisChargStartTime->hour;
		s_einvSellDisChargStartTime.min = sellDisChargStartTime->min;
		g_einvUpDateFlag.SELLDISCHARGSTARTTIME = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.SELLDISCHARGSTARTTIME = EINV_CLR;
	}
	
	DEBUG_TRCOUT();
	                     
}
/*==========================================================================*/
/*	Description		:	SELLモード放電開始時刻変数取得						*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	ret													*/
/*--------------------------------------------------------------------------*/
/*	detail			:	SELLモード放電開始時刻変数取得						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_GetSellDisChargStartTimeValue(EinvTime_t * sellDisChargStartTime)
{
	
	DEBUG_TRCIN();

	sellDisChargStartTime->hour = s_einvSellDisChargStartTime.hour;
	sellDisChargStartTime->min = s_einvSellDisChargStartTime.min;
	
	DEBUG_TRCOUT();

}
/*==========================================================================*/
/*	Description		:	SELLモード放電停止時刻変数設置						*/
/*--------------------------------------------------------------------------*/
/*	param			:	sellDisChargStopTime								*/
/*--------------------------------------------------------------------------*/
/*	return			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	SELLモード放電停止時刻変数設置						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetSellDisChargStopTimeValue(const EinvTime_t * sellDisChargStopTime)
{
	DEBUG_TRCIN();
	
	if ((s_einvSellDisChargStopTime.hour != sellDisChargStopTime->hour) ||
		(s_einvSellDisChargStopTime.min != sellDisChargStopTime->min))
	{
		s_einvSellDisChargStopTime.hour = sellDisChargStopTime->hour;
		s_einvSellDisChargStopTime.min = sellDisChargStopTime->min;
		g_einvUpDateFlag.SELLDISCHARGSTOPTIME = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.SELLDISCHARGSTOPTIME = EINV_CLR;
	}
	
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	SELLモード放電停止時刻変数取得						*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	g_einvSellDisChargStopTime							*/
/*--------------------------------------------------------------------------*/
/*	detail			:	SELLモード放電停止時刻変数取得						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_GetSellDisChargStopTimeValue(EinvTime_t * sellDisChargStopTime)
{
	
	DEBUG_TRCIN();
	
	sellDisChargStopTime->hour = s_einvSellDisChargStopTime.hour;
	sellDisChargStopTime->min = s_einvSellDisChargStopTime.min;
	
	DEBUG_TRCOUT();

}
/*==========================================================================*/
/*	Description		:	GREENモード充電開始時刻変数設置						*/
/*--------------------------------------------------------------------------*/
/*	param			:	greenChargStartTime									*/
/*--------------------------------------------------------------------------*/
/*	return			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	GREENモード充電開始時刻変数設置						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetGreenChargStartTimeValue(const EinvTime_t * greenChargStartTime)
{
	DEBUG_TRCIN();
	
	if ((s_einvGreenChargStartTime.hour != greenChargStartTime->hour) ||
		(s_einvGreenChargStartTime.min != greenChargStartTime->min))
	{
		s_einvGreenChargStartTime.hour = greenChargStartTime->hour;
		s_einvGreenChargStartTime.min = greenChargStartTime->min;
		g_einvUpDateFlag.GREENCHARGSTARTTIME = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.GREENCHARGSTARTTIME = EINV_CLR;
	}
	
	DEBUG_TRCOUT();
	                     
}
/*==========================================================================*/
/*	Description		:	GREENモード充電開始時刻変数取得						*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	g_einvGreenChargStartTime							*/
/*--------------------------------------------------------------------------*/
/*	detail			:	GREENモード充電開始時刻変数取得						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_GetGreenChargStartTimeValue(EinvTime_t * greenChargStartTime)
{
	
	DEBUG_TRCIN();

	greenChargStartTime->hour = s_einvGreenChargStartTime.hour;
	greenChargStartTime->min = s_einvGreenChargStartTime.min;

	DEBUG_TRCOUT();

}
/*==========================================================================*/
/*	Description		:	GREENモード充電停止時刻変数設置						*/
/*--------------------------------------------------------------------------*/
/*	param			:	greenChargStopTime									*/
/*--------------------------------------------------------------------------*/
/*	return			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	GREENモード充電停止時刻変数設置						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetGreenChargStopTimeValue(const EinvTime_t * greenChargStopTime)
{
	DEBUG_TRCIN();
	
	if ((s_einvGreenChargStopTime.hour != greenChargStopTime->hour) ||
		(s_einvGreenChargStopTime.min != greenChargStopTime->min))
	{
		s_einvGreenChargStopTime.hour = greenChargStopTime->hour;
		s_einvGreenChargStopTime.min = greenChargStopTime->min;
		g_einvUpDateFlag.GREENCHARGSTOPTIME = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.GREENCHARGSTOPTIME = EINV_CLR;
	}

	DEBUG_TRCOUT();

}
/*==========================================================================*/
/*	Description		:	GREENモード充電停止時刻変数取得						*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	g_einvGreenChargStopTime							*/
/*--------------------------------------------------------------------------*/
/*	detail			:	GREENモード充電停止時刻変数取得						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_GetGreenChargStopTimeValue(EinvTime_t * greenChargStopTime)
{
	
	DEBUG_TRCIN();

	greenChargStopTime->hour = s_einvGreenChargStopTime.hour;
	greenChargStopTime->min = s_einvGreenChargStopTime.min;
	
	DEBUG_TRCOUT();
	
}
/*==========================================================================*/
/*	Description		:	GREENモード放電開始時刻変数設置						*/
/*--------------------------------------------------------------------------*/
/*	param			:	greenDisChargStartTime								*/
/*--------------------------------------------------------------------------*/
/*	return			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	GREENモード放電開始時刻変数設置						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetGreenDisChargStartTimeValue(const EinvTime_t * greenDisChargStartTime)
{
	
	DEBUG_TRCIN();
	
	if ((s_einvGreenDisChargStartTime.hour != greenDisChargStartTime->hour) ||
		(s_einvGreenDisChargStartTime.min != greenDisChargStartTime->min))
	{
		s_einvGreenDisChargStartTime.hour = greenDisChargStartTime->hour;
		s_einvGreenDisChargStartTime.min = greenDisChargStartTime->min;
		g_einvUpDateFlag.GREENDISCHARGSTARTTIME = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.GREENDISCHARGSTARTTIME = EINV_CLR;
	}

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	GREENモード放電開始時刻変数取得						*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	g_einvGreenDisChargStartTime						*/
/*--------------------------------------------------------------------------*/
/*	detail			:	GREENモード放電開始時刻変数取得						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_GetGreenDisChargStartTimeValue(EinvTime_t * greenDisChargStartTime)
{
	DEBUG_TRCIN();

	greenDisChargStartTime->hour = s_einvGreenDisChargStartTime.hour;
	greenDisChargStartTime->min = s_einvGreenDisChargStartTime.min;
	
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	GREENモード放電停止時刻変数設置						*/
/*--------------------------------------------------------------------------*/
/*	param			:	greenDisChargStopTime								*/
/*--------------------------------------------------------------------------*/
/*	return			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	GREENモード放電停止時刻変数設置						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetGreenDisChargStopTimeValue(const EinvTime_t * greenDisChargStopTime)
{
	DEBUG_TRCIN();
	
	if ((s_einvGreenDisChargStopTime.hour != greenDisChargStopTime->hour) ||
		(s_einvGreenDisChargStopTime.min != greenDisChargStopTime->min))
	{
		s_einvGreenDisChargStopTime.hour = greenDisChargStopTime->hour;
		s_einvGreenDisChargStopTime.min = greenDisChargStopTime->min;
		g_einvUpDateFlag.GREENDISCHARGSTOPTIME = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.GREENDISCHARGSTOPTIME = EINV_CLR;
	}
	
	DEBUG_TRCOUT();
	                     
}
/*==========================================================================*/
/*	Description		:	GREENモード放電停止時刻変数取得						*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	ret													*/
/*--------------------------------------------------------------------------*/
/*	detail			:	GREENモード放電停止時刻変数取得						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_GetGreenDisChargStopTimeValue(EinvTime_t * greenDisChargStopTime)
{	
	DEBUG_TRCIN();
	
	greenDisChargStopTime->hour = s_einvGreenDisChargStopTime.hour;
	greenDisChargStopTime->min = s_einvGreenDisChargStopTime.min;
	
	DEBUG_TRCOUT();
	
}
/*==========================================================================*/
/*	Description		:	設置運転モード設定依頼元							*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	runModeSetFromData:運転モード設定依頼元設定値		*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:   運転モード設定依頼元変更処理						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/11 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
void EINV_n_SetRunModeSetFromValue(uchar runModeSetFromData)
{
	                     
	DEBUG_TRCIN();
	
	if (s_einvRunModeSetFrom != runModeSetFromData)
	{
		/*運転モード設定依頼元変更処理*/
		s_einvRunModeSetFrom = runModeSetFromData;
		g_einvUpDateFlag.RUNMODESETFROM = EINV_SET;
	}
	else 
	{
		g_einvUpDateFlag.RUNMODESETFROM = EINV_CLR;

	}

	DEBUG_TRCOUT();
	
}
/*==========================================================================*/
/*	Description		:	運転モード設定依頼元取得							*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	g_einvRunModeSetFrom								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	運転モード設定依頼元取得							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
uchar EINV_n_GetRunModeSetFromValue(void)
{
	uchar ret;
	DEBUG_TRCIN();
	ret = s_einvRunModeSetFrom;
	DEBUG_TRCOUT();
	return ret;
}
/*==========================================================================*/
/*	Description		:	システム構成変数設置								*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	sysStructValue										*/
/*	param	out		:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	運転開始停止変数設置								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/27 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetSysStructValue(const EinvSysStruct_t * sysStructValue)
{
	DEBUG_TRCIN();
	
	if ((g_einvSysStruct.sysStructAscii != sysStructValue->sysStructAscii) ||
		(g_einvSysStruct.sysStructNum != sysStructValue->sysStructNum))
	{
		g_einvSysStruct.sysStructAscii = sysStructValue->sysStructAscii;
		g_einvSysStruct.sysStructNum = sysStructValue->sysStructNum;
		g_einvUpDateFlag.SYSSTRUCT = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.SYSSTRUCT = EINV_CLR;
	}

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	システム構成変数取得								*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	g_einvSysStruct										*/
/*--------------------------------------------------------------------------*/
/*	detail			:	システム構成変数取得								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/27 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_GetSysStructValue(EinvSysStruct_t * sysStructValue)
{
	
	DEBUG_TRCIN();

	sysStructValue->sysStructAscii = g_einvSysStruct.sysStructAscii;
	sysStructValue->sysStructNum = g_einvSysStruct.sysStructNum;

	DEBUG_TRCOUT();

}
/*==========================================================================*/
/*	Description		:	試運転可能否状態設置								*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	available											*/
/*						EINV_TESTRUN_UNAVAILABLE							*/
/*						EINV_TESTRUN_AVAILABLE								*/
/*	param	out		:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	試運転外部通信機接続状態設置						*/
/*						1台スマートフォン接続後EINV_TESTRUN_AVAILABLE設置	*/
/*						スマートフォン未接続或いはもう一台スマートフォン接続*/
/*						EINV_TESTRUN_UNAVAILABLE							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetTestRunAvailableValue(uchar available)
{
	DEBUG_TRCIN();
	
	if (s_einvTestRunAvailable != available)
	{
		s_einvTestRunAvailable = available;
		g_einvUpDateFlag.TESTRUNAVAILABLE = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.TESTRUNAVAILABLE = EINV_CLR;
	}

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	試運転可能否状態取得								*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	s_einvTestRunAvailable								*/
/*						EINV_TESTRUN_UNAVAILABLE							*/
/*						EINV_TESTRUN_AVAILABLE								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	システム構成変数取得								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/27 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
uchar EINV_n_GetTestRunAvailableValue(void)
{
	uchar ret;
	
	DEBUG_TRCIN();

	ret = s_einvTestRunAvailable;

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	試運転必要否状態設置								*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	need												*/
/*						EINV_NONEED_TEST_RUN								*/
/*						EINV_NEED_TEST_RUN									*/
/*	param	out		:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	試運転必要否状態設置								*/
/*					:	設置日=”------" OR	試運転済みファイルが無い NEED	*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/08 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetTestRunNeedValue(uchar need)
{
	DEBUG_TRCIN();
	
	if (s_needTestRun != need)
	{
		s_needTestRun = need;
		g_einvUpDateFlag.TESTRUNNEED = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.TESTRUNNEED = EINV_CLR;
	}

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	試運転必要否状態取得								*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	s_needTestRun										*/
/*						EINV_NONEED_TEST_RUN								*/
/*						EINV_NEED_TEST_RUN									*/
/*--------------------------------------------------------------------------*/
/*	detail			:	試運転必要否状態取得								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/08 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
uchar EINV_n_GetTestRunNeedValue(void)
{
	uchar ret;
	
	DEBUG_TRCIN();

	ret = s_needTestRun;

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	試運転指示設置										*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	request												*/
/*						EINV_TESTRUN_START			開始					*/
/*						EINV_TESTRUN_FIN			終了					*/
/*						EINV_TESTRUN_PROABORT		中止					*/
/*	param	out		:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	試運転指示設置										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetTestRunRequestValue(uchar request)
{
	DEBUG_TRCIN();
	
	if (s_einvTestRunRequest != request)
	{
		s_einvTestRunRequest = request;
		g_einvUpDateFlag.TESTRUNREQUEST = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.TESTRUNREQUEST = EINV_CLR;
	}

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	試運転指示取得										*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	s_einvTestRunRequest								*/
/*						EINV_TESTRUN_START			開始					*/
/*						EINV_TESTRUN_FIN			終了					*/
/*						EINV_TESTRUN_PROABORT		中止					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	試運転指示取得										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
uchar EINV_n_GetTestRunRequestValue(void)
{
	uchar ret;
	
	DEBUG_TRCIN();

	ret = s_einvTestRunRequest;

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	試運転サブ状態設置									*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	status												*/
/*						EINV_TESTRUN_IDLE = 0								*/
/*						EINV_TESTRUN_DO = 1									*/
/*						EINV_TESTRUN_FINISH = 2								*/
/*						EINV_TESTRUN_ABORT = 3								*/
/*	param	out		:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	試運転指示設置										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetTestRunSubStatusValue(TestRunSubFsmState_t status)
{
	DEBUG_TRCIN();
	
	if (s_einvTestRunSubFsmState != status)
	{
		s_einvTestRunSubFsmState = status;
		g_einvUpDateFlag.TESTRUNSTATUS = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.TESTRUNSTATUS = EINV_CLR;
	}

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	試運転サブ状態設置取得								*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	s_einvTestRunSubFsmState							*/
/*						EINV_TESTRUN_IDLE = 0								*/
/*						EINV_TESTRUN_DO = 1									*/
/*						EINV_TESTRUN_FINISH = 2								*/
/*						EINV_TESTRUN_ABORT = 3								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	試運転指示取得										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
TestRunSubFsmState_t EINV_n_GetTestRunSubStatusValue(void)
{
	TestRunSubFsmState_t ret;
	
	DEBUG_TRCIN();

	ret = s_einvTestRunSubFsmState;

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	試運転CMD75発行状態設置								*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	finishFlag											*/
/*						EINV_CLR = 0u										*/
/*						EINV_SET = 1u										*/
/*	param	out		:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	試運転CMD75発行状態設置								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetTestRunCmd75SendFinishValue(EinvFlag_t finishFlag)
{
	DEBUG_TRCIN();
	
	if (s_einvTestRunCmd75SendFinish != finishFlag)
	{
		s_einvTestRunCmd75SendFinish = finishFlag;
	}
	else
	{
		
	}

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	試運転CMD75発行状態取得								*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	無し												*/
/*	param	out		:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	ret													*/
/*						EinvFlag_t											*/
/*						EINV_CLR = 0u										*/
/*						EINV_SET = 1u										*/
/*--------------------------------------------------------------------------*/
/*	detail			:	試運転CMD75発行状態取得								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
EinvFlag_t EINV_n_GetTestRunCmd75SendFinishValue(void)
{
	EinvFlag_t ret;
	
	DEBUG_TRCIN();
	
	ret = s_einvTestRunCmd75SendFinish;

	DEBUG_TRCOUT();
	
	return ret;
}
/*==========================================================================*/
/*	Description		:	試運転CMD65発行状態設置								*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	finishFlag											*/
/*						EINV_CLR = 0u										*/
/*						EINV_SET = 1u										*/
/*	param	out		:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	試運転CMD65発行状態設置								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/06 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetTestRunCmd65SendFinishValue(EinvFlag_t finishFlag)
{
	DEBUG_TRCIN();
	
	if (s_einvTestRunCmd65SendFinish != finishFlag)
	{
		s_einvTestRunCmd65SendFinish = finishFlag;
	}
	else
	{
		
	}

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	試運転CMD65発行状態取得								*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	無し												*/
/*	param	out		:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	ret													*/
/*						EinvFlag_t											*/
/*						EINV_CLR = 0u										*/
/*						EINV_SET = 1u										*/
/*--------------------------------------------------------------------------*/
/*	detail			:	試運転CMD65発行状態取得								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/06 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
EinvFlag_t EINV_n_GetTestRunCmd65SendFinishValue(void)
{
	EinvFlag_t ret;
	
	DEBUG_TRCIN();
	
	ret = s_einvTestRunCmd65SendFinish;

	DEBUG_TRCOUT();
	
	return ret;
}
/*==========================================================================*/
/*	Description		:	ENL運転モード初期化									*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	無し												*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:   運転モード設定元ENLからその他運転設定元に遷移する場合*/
/*					:	BatteryRunmode:		待機							*/
/*					:	V2HRunmode: 		待機							*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/11 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
void EINV_n_InitEnlRunMode(void)
{
	DEBUG_TRCIN();
	if ((s_einvRunModeSetFrom != EINV_ENL) && 
		(g_einvUpDateFlag.RUNMODESETFROM == EINV_SET)
	)
	{
		/*運転モード設定元ENL以外の運転設定元に遷移する場合*/
		/*V:蓄電池クラス_運転モードと V:V2H クラス_運転モードは初期値とする。*/
		s_einvBatteryRunmode = EINV_ENLIDLE;
		s_einvV2HRunmode = EINV_ENLIDLE;
		s_einvEnlSetFrom = EINV_INVALIDVALUE;
		/*20230328 電力算出モジュールからいくつか変数初期化必要*/
		/*20230411*/
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	設置ENL運転モード設定依頼元							*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	enlRunMode:ENL運転モード設定依頼元設定値			*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:   設置ENL運転モード設定依頼元							*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/04 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
void  EINV_n_SetENLRunmodeFromValue(EinvEnlSetFrom_t setENLRunModeFrom)
{
	DEBUG_TRCIN();
	if (s_einvEnlSetFrom != setENLRunModeFrom)
	{
		//ENL運転モード設定依頼元変更処理
		s_einvEnlSetFrom = setENLRunModeFrom;
		
		g_einvUpDateFlag.ENLRUNMODEFROM = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.ENLRUNMODEFROM = EINV_CLR;

	}
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	取得設置ENL運転モード設定依頼元						*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	void												*/
/*	return			:	g_einvEnlSetFrom									*/
/*--------------------------------------------------------------------------*/
/*	detail			:   取得設置ENL運転モード設定依頼元						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/04 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
EinvEnlSetFrom_t EINV_n_GetENLRunmodeFromValue(void)
{
	EinvEnlSetFrom_t ret;
	DEBUG_TRCIN();
	ret = s_einvEnlSetFrom;
	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:	設置同時放電モード優先度							*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	enlRunMode:ENL設置同時放電モード優先度				*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:   設置ENL運転モード設定依頼元							*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/11 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
void  EINV_n_SetDischargePriorityValue(uchar setDischargePriority)
{
	DEBUG_TRCIN();
	if (s_einvDischargePriority != setDischargePriority)
	{
		s_einvDischargePriority = setDischargePriority;
		g_einvUpDateFlag.DISCHARGEPRIORITY = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.DISCHARGEPRIORITY = EINV_CLR;

	}
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	取得同時放電モード優先度							*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	void												*/
/*	return			:	g_einvDischargePriority								*/
/*--------------------------------------------------------------------------*/
/*	detail			:   取得設置ENL運転モード設定依頼元						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/11 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
uchar EINV_n_GetDischargePriorityValue(void)
{
	uchar ret;
	DEBUG_TRCIN();
	ret = s_einvDischargePriority;
	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:	設置メンテナンス状態								*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	maintainstatus:メンテナンス状態						*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:   設置メンテナンス状態								*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/06 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
void  EINV_n_SetMaintainStatusValue(uchar maintainStatus)
{
	DEBUG_TRCIN();
	if (s_einvMaintainStatus != maintainStatus)
	{
		s_einvMaintainStatus = maintainStatus;
		g_einvUpDateFlag.MAINTAIN = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.MAINTAIN = EINV_CLR;

	}
	DEBUG_TRCOUT();
	
}
/*==========================================================================*/
/*	Description		:	取得メンテナンス状態								*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	void												*/
/*	return			:	g_einvMaintainStatus								*/
/*--------------------------------------------------------------------------*/
/*	detail			:   取得メンテナンス状態								*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/06 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
uchar EINV_n_GetMaintainStatusValue(void)
{
	
	uchar ret;
	DEBUG_TRCIN();
	ret = s_einvMaintainStatus;
	DEBUG_TRCOUT();
	return ret;
}
/*==========================================================================*/
/*	Description		:	システム状態変数設置								*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	mainStatus:	SYS/MAINTAIN/TEST/BACKUP				*/
/*	param	out		:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	システム状態変数設置								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/19 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_SetSysStatusValue(EinvMainStatus_t mainStatus)
{ 
	DEBUG_TRCIN();
	if (s_einvMainStatus != mainStatus)
	{
		s_einvMainStatus = mainStatus;
		g_einvUpDateFlag.MAINSTATUS = EINV_SET;
	}
	else
	{
		g_einvUpDateFlag.MAINSTATUS = EINV_CLR;
	}
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:	取得運転開始停止変数								*/
/*--------------------------------------------------------------------------*/
/*	param			:	無い												*/
/*--------------------------------------------------------------------------*/
/*	return			:	g_einvOperationStartStop							*/
/*--------------------------------------------------------------------------*/
/*	detail			:	取得運転開始停止変数								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/04 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
EinvMainStatus_t EINV_n_GetSysStatusValue(void)
{
	EinvMainStatus_t ret;
	DEBUG_TRCIN();
	ret = s_einvMainStatus;
	DEBUG_TRCOUT();
	return ret;
}
/*==========================================================================*/
/*	Description		:	取得Vram更新状態管理変数の値						*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	void												*/
/*	return			:	g_einvUpDateFlag									*/
/*--------------------------------------------------------------------------*/
/*	detail			:	取得Vram更新状態管理変数の値						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
void EINV_n_GetUpDateFlag(EinvUpdate_t * updateFlag)
{
	
	DEBUG_TRCIN();
	
	* updateFlag = g_einvUpDateFlag;
	
	DEBUG_TRCOUT();
	
}
/*==========================================================================*/
/*	Description		:	Vram更新状態管理変数の値初期化する					*/
/*--------------------------------------------------------------------------*/
/*	param	out		:	void												*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	変数値変更通知、変更対応処理実行完了後クリアする	*/
/*					:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/19 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void  EINV_n_ClrUpDateFlag(void)
{
	DEBUG_TRCIN();
	g_einvUpDateFlag.CTLRUNMODE = EINV_CLR;
	g_einvUpDateFlag.AUTORUNMODE = EINV_CLR;
	g_einvUpDateFlag.SELLCHARGSTARTTIME = EINV_CLR;
	g_einvUpDateFlag.SELLCHARGSTOPTIME = EINV_CLR;
	g_einvUpDateFlag.SELLDISCHARGSTARTTIME = EINV_CLR;
	g_einvUpDateFlag.SELLDISCHARGSTOPTIME = EINV_CLR;
	g_einvUpDateFlag.GREENCHARGSTARTTIME = EINV_CLR;
	g_einvUpDateFlag.GREENCHARGSTOPTIME = EINV_CLR;
	g_einvUpDateFlag.GREENDISCHARGSTARTTIME = EINV_CLR;
	g_einvUpDateFlag.GREENDISCHARGSTOPTIME = EINV_CLR;
	g_einvUpDateFlag.RUNMODESETFROM = EINV_CLR;
	g_einvUpDateFlag.POWEROFFRUNMODE = EINV_CLR;
	g_einvUpDateFlag.BATTCLASSRUNMODE = EINV_CLR;
	g_einvUpDateFlag.V2HCLASSRUNMODE = EINV_CLR;
	g_einvUpDateFlag.DISCHARGEPRIORITY = EINV_CLR;
	g_einvUpDateFlag.ENLRUNMODEFROM = EINV_CLR;
	g_einvUpDateFlag.OPERATIONSTARTSTOP = EINV_CLR;
	g_einvUpDateFlag.MAINTAIN = EINV_CLR;
	g_einvUpDateFlag.MAINSTATUS = EINV_CLR;
	g_einvUpDateFlag.SYSSTRUCT = EINV_CLR;
	g_einvUpDateFlag.TESTRUNAVAILABLE = EINV_CLR;
	g_einvUpDateFlag.TESTRUNREQUEST = EINV_CLR;
	g_einvUpDateFlag.TESTRUNSTATUS = EINV_CLR;
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	CMD65コマンドメンバー再算出必要性判定				*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	無し												*/
/*	param	out		:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	   EINV_UPDATE: 1	再算出必要						*/
/*					:	   EINV_NOUPDATE: 0	再算出必要無し					*/
/*					:														*/	
/*	detail			:  運転モード設置関連パラメータ更新なら、				*/
/*					:  CMD65コマンドメンバー再算出必要						*/
/*					:  運転モード設置関連パラメータ変数更新しない場合、		*/
/*				    :  再算出不要											*/
/*					:  該当対象VRAM変数は運転モード、非常時運転モード、		*/
/*					:  運転/停止、自動運転時間帯							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/19 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
slong EINV_n_CalcCmd65Request(void)
{
	
	slong ret = EINV_NOUPDATE;
	DEBUG_TRCIN();
	
	if (
		(g_einvUpDateFlag.OPERATIONSTARTSTOP == EINV_SET) ||
		(g_einvUpDateFlag.CTLRUNMODE == EINV_SET) ||
		(g_einvUpDateFlag.POWEROFFRUNMODE == EINV_SET) ||
#if 0
		(g_einvUpDateFlag.DISCHARGEPRIORITY == EINV_SET) ||
		(g_einvUpDateFlag.BATTCLASSRUNMODE == EINV_SET) ||
#endif	
		(g_einvUpDateFlag.GREENCHARGSTARTTIME == EINV_SET) ||
		(g_einvUpDateFlag.GREENCHARGSTOPTIME == EINV_SET) ||
		(g_einvUpDateFlag.GREENDISCHARGSTARTTIME == EINV_SET) ||
		(g_einvUpDateFlag.GREENDISCHARGSTOPTIME == EINV_SET) ||
#if 0
	    (g_einvUpDateFlag.RUNMODESETFROM == EINV_SET) ||
		(g_einvUpDateFlag.AUTORUNMODE == EINV_SET) ||
#endif	
		(g_einvUpDateFlag.SELLCHARGSTARTTIME == EINV_SET) ||
		(g_einvUpDateFlag.SELLCHARGSTOPTIME == EINV_SET) ||
		(g_einvUpDateFlag.SELLDISCHARGSTARTTIME == EINV_SET) ||
		(g_einvUpDateFlag.SELLDISCHARGSTOPTIME == EINV_SET)
#if 0
		(g_einvUpDateFlag.ENLRUNMODEFROM == EINV_SET) ||
		(g_einvUpDateFlag.V2HCLASSRUNMODE == EINV_SET)
#endif	
	)
	{
		/*再算出必要ある*/
		ret = EINV_UPDATE;
	}
	else
	{
		/*再算出必要ない*/
		ret = EINV_NOUPDATE;
	}
	
	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:	CMD65コマンド運転モード算出する						*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	nowTime:カレント時刻								*/
/*	param	in		:	cmd65RunMode:カレントCMD65運転モード				*/
/*	param	out		:	calcCmd65RunModeResult:CMD65運転モード計算結果格納	*/
/*--------------------------------------------------------------------------*/
/*					:	 1:EINV_UPDATE			CMD65運転モード更新			*/
/*					:	 0:EINV_NOUPDATE		CMD65運転モード不更新		*/
/*--------------------------------------------------------------------------*/
/*	detail			:	CMD65コマンド運転モード算出する						*/
/*					:	表 2-10 運転モード算出								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/19 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
slong EINV_n_CalcCMD65Runmode(const EinvTime_t * nowTime,uchar cmd65RunMode,uchar * calcCmd65RunModeResult)
{
	slong	ret = EINV_UPDATE;
	EinvFlag_t m, n;
	EinvTimeScope_t  sellChargStartStopTime;
	EinvTimeScope_t  sellDisChargStartStopTime;
	EinvTimeScope_t  greenChargStartStopTime;
	EinvTimeScope_t  greenDisChargStartStopTime;
	uchar einvCtlRunModeIndex;

	DEBUG_TRCIN();

	if ((s_einvCtlRunMode >= EINV_COMMU_BOARD_MANAGE_MODE_2) && (s_einvCtlRunMode <= EINV_COMMU_BOARD_MANAGE_MODE_7))
	{
		/*手動モードCMD65運転モード算出*/
		einvCtlRunModeIndex = s_einvCtlRunMode - EINV_COMMU_BOARD_MANAGE_MODE_2;
		* calcCmd65RunModeResult = s_einvCmd65RunmodeTbl[einvCtlRunModeIndex];
	}
	else if(s_einvCtlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_0)
	{
		sellChargStartStopTime.start = s_einvSellChargStartTime;
		sellChargStartStopTime.end = s_einvSellChargStopTime;
		sellDisChargStartStopTime.start = s_einvSellDisChargStartTime;
		sellDisChargStartStopTime.end = s_einvSellDisChargStopTime;

		m = EINV_n_CheckTimeScope(nowTime, &sellChargStartStopTime);
		if (m == EINV_SET)
		{
			* calcCmd65RunModeResult = EINV_OPEMODE_3;
		}
		n = EINV_n_CheckTimeScope(nowTime, &sellDisChargStartStopTime);
		if (n == EINV_SET)
		{
			* calcCmd65RunModeResult = EINV_OPEMODE_0;
		}
		if ((m != EINV_SET) && (n != EINV_SET))
		{
			* calcCmd65RunModeResult = EINV_OPEMODE_4;
		}
	}
	else if(s_einvCtlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_1)
	{
		greenChargStartStopTime.start = s_einvGreenChargStartTime;
		greenChargStartStopTime.end = s_einvGreenChargStopTime;
		greenDisChargStartStopTime.start = s_einvGreenDisChargStartTime;
		greenDisChargStartStopTime.end = s_einvGreenDisChargStopTime;

		m = EINV_n_CheckTimeScope(nowTime, &greenChargStartStopTime);
		if (m == EINV_SET)
		{
			* calcCmd65RunModeResult = EINV_OPEMODE_3;
			
		}
		n = EINV_n_CheckTimeScope(nowTime, &greenDisChargStartStopTime);
		if (n == EINV_SET)
		{
			* calcCmd65RunModeResult = EINV_OPEMODE_1;
		}
		if ((m != EINV_SET) && (n != EINV_SET))
		{
			* calcCmd65RunModeResult = EINV_OPEMODE_4;
		}
	}
	else
	{
		/* 処理無し */
	}
	
	if((cmd65RunMode == * calcCmd65RunModeResult) || (* calcCmd65RunModeResult == EINV_INVALIDVALUE))
	{
		/*CMD65コマンドへ書込み必要無し*/
		ret = EINV_NOUPDATE;
	}
	else
	{
		/*CMD65コマンドへ書込み必要ある*/
		ret = EINV_UPDATE;
	}
	
	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:	CMD65コマンド非常時運転モード算出する				*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	cmd65PowerOffRunMode:CMD65非常時運転モード			*/
/*	param	out		:	calcCmd65PowerOffRunModeResult:CMD65非常時運転モード*/
/*						計算結果格納										*/
/*--------------------------------------------------------------------------*/
/*					:	 1:EINV_UPDATE										*/
/*					:	 0:EINV_NOUPDATE									*/
/*--------------------------------------------------------------------------*/
/*	detail			:	CMD65コマンド非常時運転モード算出する				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/19 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
slong EINV_n_CalcCMD65PoweroffRunMode(uchar cmd65PowerOffRunMode,uchar * calcCmd65PowerOffRunModeResult)
{
	slong	ret = EINV_UPDATE;
	
	DEBUG_TRCIN();
	
	if(s_einvPowerOffRunMode == cmd65PowerOffRunMode)
	{
		ret = EINV_NOUPDATE;
	}
	else
	{
		* calcCmd65PowerOffRunModeResult = s_einvPowerOffRunMode;
		ret = EINV_UPDATE;
	}
	
	DEBUG_TRCOUT();
	
	return ret;
}
/*==========================================================================*/
/*	Description		:	CMD75コマンドコネクタ状態変更判定					*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	cmd75ConnecterStatus:カレントCMD75コネクタ状態		*/
/*	param	out		:	calccmd75ConnecterStatusResult:CMD75コネクタ状態	*/
/*						計算結果格納										*/
/*--------------------------------------------------------------------------*/
/*					:	 1:EINV_UPDATE			コネクタ状態変更必要		*/
/*					:	 0:EINV_NOUPDATE		コネクタ状態変更必要無し	*/
/*--------------------------------------------------------------------------*/
/*	detail			:	CMD75コマンドコネクタ状態変更判定					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
slong EINV_n_CalcCMD75ConnecterStatus(uchar cmd75ConnecterStatus,uchar * calcCmd75ConnecterStatusResult)
{
	slong	ret = EINV_UPDATE;
	
	DEBUG_TRCIN();
	
	if((* calcCmd75ConnecterStatusResult == cmd75ConnecterStatus) || (* calcCmd75ConnecterStatusResult == EINV_INVALIDVALUE))
	{
		/*算出結果カレントコネクタ状態一致或いはコネクタ状態変更無し*/
		/*CMD75構造体メンバー修正必要無し*/
		ret = EINV_NOUPDATE;
	}
	else
	{
		/*CMD75構造体メンバー修正必要*/
		ret = EINV_UPDATE;
	}
	
	DEBUG_TRCOUT();
	
	return ret;
}
/*==========================================================================*/
/*	Description		:	CMD65コマンド運転/停止メンバー算出する				*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	cmd65OpStartStop:CMD65運転開始停止					*/
/*	param	out		:	calcCmd65OpStartStopResult:CMD65運転開始停止		*/
/*						計算結果格納										*/
/*--------------------------------------------------------------------------*/
/*					:	 1:EINV_UPDATE										*/
/*					:	 0:EINV_NOUPDATE									*/
/*--------------------------------------------------------------------------*/
/*	detail			:	CMD65コマンド運転開始停止算出する					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/11 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
slong EINV_n_CalcCMD65OpStartStop(uchar cmd65OpStartStop,uchar * calcCmd65OpStartStopResult)
{
	slong	ret = EINV_UPDATE;
	
	DEBUG_TRCIN();
	
	if(s_einvOperationStartStop == cmd65OpStartStop)
	{
		ret = EINV_NOUPDATE;
	}
	else
	{
		* calcCmd65OpStartStopResult = s_einvOperationStartStop;
		ret = EINV_UPDATE;
	}
	
	DEBUG_TRCOUT();
	
	return ret;
}
/*==========================================================================*/
/*	Description		:	自動運転モードタイマイベント処理					*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	timerEvent:自動運転タイマイベント					*/
/*					:	cmd65RunMode:カレントCMD65運転モード保存変数		*/
/*	param	out		:	calcCmd65RunModeResult:CMD65運転モード算出結果格納	*/
/*--------------------------------------------------------------------------*/
/*	return			:	1:EINV_UPDATE										*/
/*					:	0:EINV_NOUPDATE										*/
/*--------------------------------------------------------------------------*/
/*	detail			:	CMD65コマンド運転モード算出する						*/
/*					:	自動運転モード時タイマイベントより、				*/
/*					:	CMD65コマンド運転モード算出							*/
/*					:	表 2-11 運転モード算出								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/19 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
slong  EINV_n_CalcCMD65RunmodeByTimerEvent(EinvTimerEvent_t timerEvent,uchar	cmd65RunMode,uchar * calcCmd65RunModeResult)
{
	slong	ret = EINV_NOUPDATE;
	uchar einvCtlRunModeIndex;
	
	DEBUG_TRCIN();
	
	/*自動運転モードのみ*/
	einvCtlRunModeIndex = s_einvCtlRunMode - '0';
 	
	if(timerEvent >= EINV_TIMEREVENTOFFSET)	
	{
		timerEvent = timerEvent - EINV_TIMEREVENTOFFSET;
	}
	else
	{
		/* 処理無し */
	}
	
	* calcCmd65RunModeResult = s_einvConvertCtlRunmodeAndTimerEventToCmd65RunmodeTbl[einvCtlRunModeIndex][timerEvent];

	
	if (* calcCmd65RunModeResult != cmd65RunMode) 
	{
		ret = EINV_UPDATE;
	}
	else
	{
		ret = EINV_NOUPDATE;
	}

	DEBUG_TRCOUT();
	
	return ret;
}

/*==========================================================================*/
/*	Description		:	ENL運転モードより管理運転モード算出処理				*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	enlSetFromStatus:初回設定/初回以降設定				*/
/*	param	in		:	enlRunMode:enl運転モード設定						*/
/*	param	out		:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	ctlRunMode:管理運転モード算出結果					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ENL運転モードより管理運転モード算出処理				*/
/*					:	表 2-14ENL に指令値から算出							*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/19 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
uchar EINV_n_CalcRunModeByEnl(EinvEnlSetFromStatus_t enlSetFromStatus,uchar enlRunMode)
{
	uchar ctlRunMode;
	
	uchar enlRunModeIndex;
	uchar enlBatteryRunmodeIndex;
	uchar enlV2HRunmodeIndex;
	uchar enlSetFromIndex;
	
	DEBUG_TRCIN();

	enlRunModeIndex = enlRunMode - '1';
    enlBatteryRunmodeIndex = s_einvBatteryRunmode - '1';
	enlV2HRunmodeIndex = s_einvV2HRunmode - '1';
	enlSetFromIndex = s_einvEnlSetFrom;

	if(enlSetFromStatus == EINV_INITSET)
	{
		/*初回設定*/
		ctlRunMode = s_einvConvertEnlRunmodeToCtlRunmodeTblI[enlSetFromIndex][enlRunModeIndex];
	}
	else
	{
		/*初回以降設定*/
		if(g_einvUpDateFlag.ENLRUNMODEFROM == EINV_CLR)
		{
			/*ENL運転モード設定元変更無い*/
			ctlRunMode = s_einvConvertEnlRunmodeToCtlRunmodeTblI[enlSetFromIndex][enlRunModeIndex];
		}
		else
		{
			/*ENL運転モード設定元変更ある*/
			ctlRunMode = s_einvConvertEnlRunmodeToCtlRunmodeTbl[enlSetFromIndex][enlBatteryRunmodeIndex][enlV2HRunmodeIndex];
		}
	}

	if(ctlRunMode == EINV_AUTO)
	{
		ctlRunMode = s_einvAutoRunMode;
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return ctlRunMode;
}
/*==========================================================================*/
/*	Description		:	cmd81 responseから運転モード算出処理				 */
/*-------------------------------------------------------------------------- */
/*	param	in		:	v2hStatus:v2hのconnecter状態/button状態保存変数		 */
/*	param	out		:	cmd75ConnecterLockStatus:CMD75 LOCK/UNLOCK状態変数	 */
/*	param	out		:	runMode:v2hのconnecter状態/button状態より			 */
/*						管理運転モード算出結果格納							 */
/*	param	out		:	autoRunMode:v2hのconnecter状態/button状態より		 */
/*						自動運転モード算出結果格納							 */
/*-------------------------------------------------------------------------- */
/*	return			: 														 */
/*					:	1:EINV_UPDATE 			管理運転モード更新			 */
/*					:	0:EINV_NOUPDATE			管理運転モード不更新		 */
/*-------------------------------------------------------------------------- */
/*	detail			:	cmd81 responseから運転モード算出処理				 */
/*					:	V2H状態変化より管理運転モード、自動運転モード算出	 */
/*					:	V2H状態変化よりCMD75コネクタロックアンロック算出	 */
/*					:	表 2-17V2H 指令値から 								 */
/*-------------------------------------------------------------------------- */
/*	history			:2023/04/19 : DHC Sun.JT : 新規作成						 */
/*========================================================================== */
slong EINV_n_CalcRunmodeByCmd81res(const EinvV2hStatus_t * v2hStatus, uchar * cmd75ConnecterLockStatus, uchar * runMode, uchar * autoRunMode)
{
	EinvFlag_t i, j, k;
	
	slong ret = EINV_NOUPDATE;
	
	DEBUG_TRCIN();
	
	/*管理運転モードと自動運転モードを保存しておく*/
	/*判定条件より管理運転モードと自動運転モード変更しない場合、算出結果は従来の値にする*/
	*runMode = s_einvCtlRunMode;
	*autoRunMode = s_einvAutoRunMode;
	
	if(v2hStatus->v2hOperatePossible == EINV_POSSIBLE)
	{
	
		/*ボタン状態変更場合、ボタン状態保存して置く、エッジON->OFF判定*/
		i = EINV_n_CheckButtonStatus(v2hStatus->button1, &s_einvButton1);
		j = EINV_n_CheckButtonStatus(v2hStatus->button2, &s_einvButton2);
		k = EINV_n_CheckButtonStatus(v2hStatus->button3, &s_einvButton3);

		if (s_einvCtlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_0)
		{
			/*自動運転SELLモード*/
			if (v2hStatus->connecterStatus == EINV_UNLOCK)
			{
				if ((i == EINV_SET) && (j == EINV_CLR) && (k == EINV_CLR))
				{
					/*設定＆更新判断*/
					*runMode = EINV_COMMU_BOARD_MANAGE_MODE_4;
					*autoRunMode = EINV_AUTOSELL;
					ret = EINV_UPDATE;
				}
				else
				{
					ret = EINV_NOUPDATE;
				}
			}
			else
			{
				if ((i == EINV_SET) && (j == EINV_CLR) && (k == EINV_CLR))
				{
					/*設定＆更新判断*/
					*runMode = EINV_COMMU_BOARD_MANAGE_MODE_4;
					*autoRunMode = EINV_AUTOSELL;
					ret = EINV_UPDATE;
				}
				else if ((i == EINV_CLR) && (j == EINV_SET) && (k == EINV_CLR))
				{
					/*CMD75 コネクタロック/解除 -> アンロック(0)*/
					/*CMD75 コネクタロック/解除　更新判定*/
					/*更新ならば、　SendCmd75Request->SET*/
					/*未更新ならば、SendCmd75Request->CLR*/
					/*CMD75 コネクタロック/解除更新インタフェース20230328*/
					* cmd75ConnecterLockStatus = EINV_UNLOCK;
					ret = EINV_NOUPDATE;
				}
				else if ((i == EINV_CLR) && (j == EINV_CLR) && (k == EINV_SET))
				{
					/*CMD75 コネクタロック/解除 -> アンロック(0)*/
					/*CMD75 コネクタロック/解除　更新判定*/
					/*更新ならば、　SendCmd75Request->SET*/
					/*未更新ならば、SendCmd75Request->CLR*/
					/*CMD75 コネクタロック/解除更新インタフェース20230328*/
					* cmd75ConnecterLockStatus = EINV_UNLOCK;
					ret = EINV_NOUPDATE;
				}
				else 
				{
					ret = EINV_NOUPDATE;
				}
			}
		}
		else if (s_einvCtlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_1)
		{
			/*自動運転GREENモード*/
			if (v2hStatus->connecterStatus == EINV_UNLOCK)
			{
				if ((i == EINV_SET) && (j == EINV_CLR) && (k == EINV_CLR))
				{
					/*設定＆更新判断*/
					*runMode = EINV_COMMU_BOARD_MANAGE_MODE_4;
					*autoRunMode = EINV_AUTOGREEN;
					ret = EINV_UPDATE;
				}
				else
				{
					ret = EINV_NOUPDATE;
				}
			}
			else
			{
				if ((i == EINV_SET) && (j == EINV_CLR) && (k == EINV_CLR))
				{
					/*設定＆更新判断*/
					*runMode = EINV_COMMU_BOARD_MANAGE_MODE_4;
					*autoRunMode = EINV_AUTOGREEN;
					ret = EINV_UPDATE;
				}
				else if ((i == EINV_CLR) && (j == EINV_SET) && (k == EINV_CLR))
				{
					/*CMD75 コネクタロック/解除 -> アンロック(0)*/
					/*CMD75 コネクタロック/解除　更新判定*/
					/*更新ならば、　SendCmd75Request->SET*/
					/*未更新ならば、SendCmd75Request->CLR*/
					/*CMD75 コネクタロック/解除更新インタフェース20230328*/
					* cmd75ConnecterLockStatus = EINV_UNLOCK;
					ret = EINV_NOUPDATE;
				}
				else if ((i == EINV_CLR) && (j == EINV_CLR) && (k == EINV_SET))
				{
					/*CMD75 コネクタロック/解除 -> アンロック(0)*/
					/*CMD75 コネクタロック/解除　更新判定*/
					/*更新ならば、　SendCmd75Request->SET*/
					/*未更新ならば、SendCmd75Request->CLR*/
					/*CMD75 コネクタロック/解除更新インタフェース20230328*/
					* cmd75ConnecterLockStatus = EINV_UNLOCK;
					ret = EINV_NOUPDATE;
				}
				else
				{
					ret = EINV_NOUPDATE;
				}
			}
		}
		else if (s_einvCtlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_2)
		{
			/*蓄電池充電モード*/
			if (v2hStatus->connecterStatus == EINV_UNLOCK)
			{
				if ((i == EINV_SET) && (j == EINV_CLR) && (k == EINV_CLR))
				{
					/*設定＆更新判断*/
					*runMode = EINV_COMMU_BOARD_MANAGE_MODE_4;
					ret = EINV_UPDATE;
				}
				else
				{
					ret = EINV_NOUPDATE;
				}
			}
			else
			{
				if ((i == EINV_SET) && (j == EINV_CLR) && (k == EINV_CLR))
				{
					/*設定＆更新判断*/
					*runMode = EINV_COMMU_BOARD_MANAGE_MODE_4;
					ret = EINV_UPDATE;
				}
				else if ((i == EINV_CLR) && (j == EINV_SET) && (k == EINV_CLR))
				{
					/*CMD75 コネクタロック/解除 -> アンロック(0)*/
					/*CMD75 コネクタロック/解除　更新判定*/
					/*更新ならば、　SendCmd75Request->SET*/
					/*未更新ならば、SendCmd75Request->CLR*/
					/*CMD75 コネクタロック/解除更新インタフェース20230328*/
					* cmd75ConnecterLockStatus = EINV_UNLOCK;
					ret = EINV_NOUPDATE;
				}
				else if ((i == EINV_CLR) && (j == EINV_CLR) && (k == EINV_SET))
				{
					/*CMD75 コネクタロック/解除 -> アンロック(0)*/
					/*CMD75 コネクタロック/解除　更新判定*/
					/*更新ならば、　SendCmd75Request->SET*/
					/*未更新ならば、SendCmd75Request->CLR*/
					/*CMD75 コネクタロック/解除更新インタフェース20230328*/
					* cmd75ConnecterLockStatus = EINV_UNLOCK;
					ret = EINV_NOUPDATE;
				}
				else
				{
					ret = EINV_NOUPDATE;
				}
			}
		}
		else if (s_einvCtlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_4)
		{
			/*車両充電モード*/
			if (v2hStatus->connecterStatus == EINV_UNLOCK)
			{
				ret = EINV_NOUPDATE;
			}
			else
			{
				if ((i == EINV_SET) && (j == EINV_CLR) && (k == EINV_CLR))
				{
					/*設定＆更新判断*/
					*runMode = s_einvAutoRunMode;
					/*CMD75 コネクタロック/解除 -> アンロック(0)*/
					/*CMD75 コネクタロック/解除　更新判定*/
					/*更新ならば、　SendCmd75Request->SET*/
					/*未更新ならば、SendCmd75Request->CLR*/
					/*CMD75 コネクタロック/解除更新インタフェース20230328*/
					* cmd75ConnecterLockStatus = EINV_UNLOCK;
					ret = EINV_UPDATE;

				}
				else if ((i == EINV_CLR) && (j == EINV_SET) && (k == EINV_CLR))
				{
					*runMode = s_einvAutoRunMode;
					/*CMD75 コネクタロック/解除 -> アンロック(0)*/
					/*CMD75 コネクタロック/解除　更新判定*/
					/*更新ならば、　SendCmd75Request->SET*/
					/*未更新ならば、SendCmd75Request->CLR*/
					/*CMD75 コネクタロック/解除更新インタフェース20230328*/
					* cmd75ConnecterLockStatus = EINV_UNLOCK;
					ret = EINV_UPDATE;
				}
				else if ((i == EINV_CLR) && (j == EINV_CLR) && (k == EINV_SET))
				{
					*runMode = s_einvAutoRunMode;
					/*CMD75 コネクタロック/解除 -> アンロック(0)*/
					/*CMD75 コネクタロック/解除　更新判定*/
					/*更新ならば、　SendCmd75Request->SET*/
					/*未更新ならば、SendCmd75Request->CLR*/
					/*CMD75 コネクタロック/解除更新インタフェース20230328*/
					* cmd75ConnecterLockStatus = EINV_UNLOCK;
					ret = EINV_UPDATE;
				}
				else
				{
					ret = EINV_NOUPDATE;
				}
			}
		}
		else
		{
			/*その以外のモード*/
			if (v2hStatus->connecterStatus == EINV_UNLOCK)
			{
				if ((i == EINV_SET) && (j == EINV_CLR) && (k == EINV_CLR))
				{
					/*設定＆更新判断*/
					*runMode = EINV_COMMU_BOARD_MANAGE_MODE_4;
					ret = EINV_UPDATE;
				}
				else if ((i == EINV_CLR) && (j == EINV_SET) && (k == EINV_CLR))
				{
					*runMode = s_einvAutoRunMode;
					ret = EINV_UPDATE;
				}
				else if ((i == EINV_CLR) && (j == EINV_CLR) && (k == EINV_SET))
				{
					*runMode = s_einvAutoRunMode;
					ret = EINV_UPDATE;
				}
				else
				{
					ret = EINV_NOUPDATE;
				}
			}
			else
			{
				if ((i == EINV_SET) && (j == EINV_CLR) && (k == EINV_CLR))
				{
					/*設定＆更新判断*/
					*runMode = EINV_COMMU_BOARD_MANAGE_MODE_4;
					ret = EINV_UPDATE;

				}
				else if ((i == EINV_CLR) && (j == EINV_SET) && (k == EINV_CLR))
				{
					*runMode = s_einvAutoRunMode;
					/*CMD75 コネクタロック/解除 -> アンロック(0)*/
					/*CMD75 コネクタロック/解除　更新判定*/
					/*更新ならば、　SendCmd75Request->SET*/
					/*未更新ならば、SendCmd75Request->CLR*/
					/*CMD75 コネクタロック/解除更新インタフェース20230328*/
					* cmd75ConnecterLockStatus = EINV_UNLOCK;
					ret = EINV_UPDATE;
				}
				else if ((i == EINV_CLR) && (j == EINV_CLR) && (k == EINV_SET))
				{
					*runMode = s_einvAutoRunMode;
					/*CMD75 コネクタロック/解除 -> アンロック(0)*/
					/*CMD75 コネクタロック/解除　更新判定*/
					/*更新ならば、　SendCmd75Request->SET*/
					/*未更新ならば、SendCmd75Request->CLR*/
					/*CMD75 コネクタロック/解除更新インタフェース20230328*/
					* cmd75ConnecterLockStatus = EINV_UNLOCK;
					ret = EINV_UPDATE;
				}
				else
				{
					ret = EINV_NOUPDATE;
				}
			}
		}
	}
	else
	{
		ret = EINV_NOUPDATE;
	}
	
	DEBUG_TRCOUT();
	
	return ret;
	
}
/*==========================================================================*/
/*	Description		:	cmd81 responseから運転モード設定処理				*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	manualRunStatus		手動運転状態保存変数保存		*/
/*	param	out		:	runMode				管理運転モード算出結果格納		*/
/*--------------------------------------------------------------------------*/
/*	return			:  														*/
/*					:	1:EINV_UPDATE 運転モード更新						*/
/*					:	0:EINV_NOUPDATE 運転モード更新無し					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	手動運転モードから待機モードへ遷移する判定			*/
/*					:	その他、手動運転モード間の状態遷移					*/
/*					:	仕様書2.2.1.1.5										*/
/*					:	通常稼働中のみ判定実施								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
slong EINV_n_CalcRunmodeByCmd81resmanual(const Einv_ManualRunStatus_t * manualRunStatus, uchar * runMode)
{
	slong ret = EINV_NOUPDATE;
	
	DEBUG_TRCIN();
	
	/*算出結果より管理運転モード変更しない場合、EINV_NOUPDATE戻す、算出結果が従来と同じ値にする*/
	*runMode = s_einvCtlRunMode;
	
	if(s_einvMainStatus == EINV_SYS)
	{
		if( ((s_einvCtlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_2) && 
			((manualRunStatus->chargeStatus == EINV_FINISH) || ((manualRunStatus->battStatus != s_einvBattStatus) && (s_einvBattStatus == EINV_BATTRUNNING)))
			) ||
			((s_einvCtlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_3) && 
			((manualRunStatus->disChargeStatus == EINV_FINISH) || ((manualRunStatus->battStatus != s_einvBattStatus) && (s_einvBattStatus == EINV_BATTRUNNING)))
			) ||
			((s_einvCtlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_4) && 
			((manualRunStatus->nonChargeable == EINV_NONCHARGEDISCHARGE_FLAG_SET) || (manualRunStatus->v2hRunStatus == EINV_OPRATSTATE_D))
			) ||
			((s_einvCtlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_5) && 
			((manualRunStatus->nonDisChargeable == EINV_NONCHARGEDISCHARGE_FLAG_SET) || (manualRunStatus->v2hRunStatus == EINV_OPRATSTATE_D))
			)
		)
		{
			*runMode = EINV_COMMU_BOARD_MANAGE_MODE_7;
			ret = EINV_UPDATE;
		}
		else if ((s_einvCtlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_6) &&
			((manualRunStatus->nonDisChargeable == EINV_NONCHARGEDISCHARGE_FLAG_SET) || (manualRunStatus->v2hRunStatus == EINV_OPRATSTATE_D))
		)
		{
			*runMode = EINV_COMMU_BOARD_MANAGE_MODE_3;
			ret = EINV_UPDATE;
		}
		else if ((s_einvCtlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_6) &&
			((manualRunStatus->disChargeStatus == EINV_FINISH) || ((manualRunStatus->battStatus != s_einvBattStatus) && (s_einvBattStatus == EINV_BATTRUNNING)))
		)
		{
			*runMode = EINV_COMMU_BOARD_MANAGE_MODE_5;
			ret = EINV_UPDATE;
		}
		else {ret = EINV_NOUPDATE;}
		
		/*蓄電池運転動作状態保存*/
		s_einvBattStatus = manualRunStatus->battStatus;
	}
	else
	{
		ret = EINV_NOUPDATE;
	}
	
	DEBUG_TRCOUT();
	
	return ret;

}
/*merge from sunJT end*/
/*==========================================================================*/
/*	Description	:充電放電下限値送信判定										*/
/*--------------------------------------------------------------------------*/
/*	param		:cmdType:コマンド種類CMD65(0) or CMD75(1)					*/
/*--------------------------------------------------------------------------*/
/*	return		:EINV_NONEED(0)												*/
/*				:EINV_NEED(1)												*/
/*				:EINV_ERR_PARAM(-4)											*/
/*--------------------------------------------------------------------------*/
/*	detail		:充電放電下限値送信判定										*/
/*	detail		:表 2-33 充電上限・放電下限値送信条件						*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/27 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
uchar EINV_n_CalcNeedSendCmd(uchar cmdType)
{	
	slong ret = EINV_NONEED;
	
	DEBUG_TRCIN();
	
	if(((cmdType != EINV_SENDCMD65) && (cmdType != EINV_SENDCMD75)) ||
		(g_einvSysStruct.sysStructAscii == EINV_INVALIDVALUE) || 
		(g_einvSysStruct.sysStructNum == EINV_INVALIDVALUE)
	)
	{
		ret = EINV_ERR_PARAM;
		return ret;
	}
	else
	{
		
	}
	ret = s_einvSysStructAndCmdSendTbl[g_einvSysStruct.sysStructNum][cmdType];

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description	:試運転時送信するCMD65構造体値算出							*/
/*--------------------------------------------------------------------------*/
/*	param		:cmd65Para:EinvCmd65T_t構造体ポインタ						*/
/*--------------------------------------------------------------------------*/
/*	return		:無し														*/
/*--------------------------------------------------------------------------*/
/*	detail		:試運転時送信するCMD65構造体値算出							*/
/*	detail		:表 2-33 充電上限・放電下限値送信条件						*/
/*	detail		:2.5.2 試運転中												*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/05/06 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void  EINV_n_TestRunCalcCmd65Para(EinvCmd65T_t * cmd65Para,EinvCmdParaSetOption_t cmd65ParaSetOption)
{
	if(cmd65ParaSetOption == EINV_TESTRUNSTART){
		/*運転モード*/
		cmd65Para->opeMode 		= EINV_OPEMODE_6;								/* 運転モード:待機*/
		/*運転開始停止*/
		cmd65Para->startOrStop  = EINV_INV_STOP;								/* 停止 */
		/*試運転開始指示*/
		cmd65Para->testrunFlag 	= EINV_TEST_RUN_FLAG_YES;						/* 試運転指示フラグ 試運転開始*/
		cmd65Para->btLowerLimit = 0u;											/* 蓄電池放電下限値設定 */
		cmd65Para->btUpperLimit = EINV_BT_UPPER_LIMIT_INIT;						/* 蓄電池充電上限値設定 */
		cmd65Para->chargePwrSet = EINV_CHARGE_PWR_SET_INIT;						/* 充電電力設定値(蓄電池) */
		cmd65Para->dischargePwrSet = EINV_DISCHG_PWR_SET_INIT;					/* 放電電力設定値(蓄電池) */
	}
	else if(cmd65ParaSetOption == EINV_TESTRUNSTOP)
	{
		/*試運転終了指示*/
		cmd65Para->testrunFlag 	= EINV_TEST_RUN_FLAG_NO;						/* 試運転指示フラグ 停止*/
	}
	else
	{
		/*試運転中止指示*/
		cmd65Para->testrunFlag 	= EINV_TEST_RUN_FLAG_NO;						/* 試運転指示フラグ 停止*/
		cmd65Para->testrunOutAtPwrCut = EINV_TEST_RUN_POWERCUT_OUT_NO;			/* 試運転時停電出力 連系出力 */
		cmd65Para->opeMode 		= EINV_OPEMODE_6;								/* 運転モード:待機*/
		cmd65Para->startOrStop  = EINV_INV_STOP;								/* 停止 */
	}
	
	/*CMD65 changeflag算出 && CMD65発行コマンドバックアップ*/
	EINV_n_TestRunCalcCmd65ChangeFlagAndBackUpCmd65Para(cmd65Para);
}

/*==========================================================================*/
/*	Description	:試運転時送信するCMD65構造体changeflagメンバー値算出		*/
/*				 試運転時送信するCMD65構造体値バックアップ 					*/
/*--------------------------------------------------------------------------*/
/*	param		:cmd65Para:EinvCmd65T_t構造体ポインタ						*/
/*--------------------------------------------------------------------------*/
/*	return		:無し														*/
/*	Description	:試運転時送信するCMD65構造体changeflagメンバー値算出		*/
/*				 試運転時送信するCMD65構造体値バックアップ 					*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/05/06 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void  EINV_n_TestRunCalcCmd65ChangeFlagAndBackUpCmd65Para(EinvCmd65T_t * cmd65Para)
{
	
	if((cmd65Para->opeMode != g_einvCmd65_t.opeMode) && (cmd65Para->opeMode != EINV_UCHARMAXVALUE))
	{
		g_einvCmd65_t.opeMode = cmd65Para->opeMode;
		cmd65Para->changeFlag = cmd65Para->changeFlag | EINV_SET_CMD65_OPE_MODE;
	}
	else
	{
		
	}
	if((cmd65Para->emergencyMode != g_einvCmd65_t.emergencyMode) && (cmd65Para->emergencyMode != EINV_UCHARMAXVALUE))
	{
		g_einvCmd65_t.emergencyMode = cmd65Para->emergencyMode;
		cmd65Para->changeFlag = cmd65Para->changeFlag | EINV_SET_CMD65_EMG_MODE;
	}
	else
	{
		
	}
	if((cmd65Para->startOrStop != g_einvCmd65_t.startOrStop) && (cmd65Para->startOrStop != EINV_UCHARMAXVALUE))
	{
		g_einvCmd65_t.startOrStop = cmd65Para->startOrStop;
		cmd65Para->changeFlag = cmd65Para->changeFlag | EINV_SET_CMD65_START_OR_STOP;
	}
	else
	{
		
	}
	if((cmd65Para->btLowerLimit != g_einvCmd65_t.btLowerLimit) && (cmd65Para->btLowerLimit != EINV_USHORTMAXVALUE))
	{
		g_einvCmd65_t.btLowerLimit = cmd65Para->btLowerLimit;
		cmd65Para->changeFlag = cmd65Para->changeFlag | EINV_SET_CMD65_BT_LOWER_LIMIT;
	}
	else
	{
		
	}
	if((cmd65Para->btUpperLimit != g_einvCmd65_t.btUpperLimit) && (cmd65Para->btUpperLimit != EINV_USHORTMAXVALUE))
	{
		g_einvCmd65_t.btUpperLimit = cmd65Para->btUpperLimit;
		cmd65Para->changeFlag = cmd65Para->changeFlag | EINV_SET_CMD65_BT_UPPER_LIMIT;
	}
	else
	{
		
	}
	if((cmd65Para->testrunOutAtPwrCut != g_einvCmd65_t.testrunOutAtPwrCut) && (cmd65Para->testrunOutAtPwrCut != EINV_UCHARMAXVALUE))
	{
		g_einvCmd65_t.testrunOutAtPwrCut = cmd65Para->testrunOutAtPwrCut;
		cmd65Para->changeFlag = cmd65Para->changeFlag | EINV_SET_CMD65_TEST_RUN_OUT;
	}
	else
	{
		
	}
	if((cmd65Para->testrunFlag != g_einvCmd65_t.testrunFlag) && (cmd65Para->testrunFlag != EINV_UCHARMAXVALUE))
	{
		g_einvCmd65_t.testrunFlag = cmd65Para->testrunFlag;
		cmd65Para->changeFlag = cmd65Para->changeFlag | EINV_SET_CMD65_TEST_RUN_FLAG;
	}
	else
	{
		
	}
	if((cmd65Para->chargePwrSet != g_einvCmd65_t.chargePwrSet) && (cmd65Para->chargePwrSet != EINV_USHORTMAXVALUE))
	{
		g_einvCmd65_t.chargePwrSet = cmd65Para->chargePwrSet;
		cmd65Para->changeFlag = cmd65Para->changeFlag | EINV_SET_CMD65_CHR_PWR_SET;
	}
	else
	{
		
	}
	if((cmd65Para->dischargePwrSet != g_einvCmd65_t.dischargePwrSet) && (cmd65Para->dischargePwrSet != EINV_USHORTMAXVALUE))
	{
		g_einvCmd65_t.dischargePwrSet = cmd65Para->dischargePwrSet;
		cmd65Para->changeFlag = cmd65Para->changeFlag | EINV_SET_CMD65_DISCHR_PWR_SET;
	}
	else
	{
		
	}
}
/*==========================================================================*/
/*	Description	:試運転時送信するCMD75構造体値算出							*/
/*--------------------------------------------------------------------------*/
/*	param		:cmd75Para:EinvCmd75T_t構造体ポインタ						*/
/*--------------------------------------------------------------------------*/
/*	return		:無し														*/
/*--------------------------------------------------------------------------*/
/*	detail		:試運転時送信するCMD75構造体値算出							*/
/*	detail		:表 2-33 充電上限・放電下限値送信条件						*/
/*	detail		:2.5.2 試運転中												*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/05/05 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void EINV_n_TestRunCalcCmd75Para(EinvCmd75T_t * cmd75Para)
{
	cmd75Para->carDiscLowerLimitSet = 0u;							/* 車両放電下限値設定 */
	cmd75Para->carCharUpperLimitSet = EINV_V2H_CHARGE_UP_LMT_INIT;	/* 車両充電上限値設定 */
	cmd75Para->carCharPwrValueSet = EINV_CHARGE_PWR_SET_INIT;		/* 充電電力設定値(車両) */
	cmd75Para->carDiscPwrValueSet = EINV_DISCHG_PWR_SET_INIT;		/* 放電電力設定値(車両) */
	
	EINV_n_TestRunCalcCmd75ChangeFlagAndBackUpCmd75Para(cmd75Para);
}
/*==========================================================================*/
/*	Description	:試運転時送信するCMD75構造体changeflagメンバー値算出		*/
/*				 試運転時送信するCMD75構造体値バックアップ 					*/
/*--------------------------------------------------------------------------*/
/*	param		:cmd75Para:EinvCmd75T_t構造体ポインタ						*/
/*--------------------------------------------------------------------------*/
/*	return		:無し														*/
/*	Description	:試運転時送信するCMD75構造体changeflagメンバー値算出		*/
/*				 試運転時送信するCMD75構造体値バックアップ 					*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/05/06 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
void  EINV_n_TestRunCalcCmd75ChangeFlagAndBackUpCmd75Para(EinvCmd75T_t * cmd75Para)
{
	
	if((cmd75Para->carOpertPriority != g_einvCmd75_t.carOpertPriority) && (cmd75Para->carOpertPriority != EINV_UCHARMAXVALUE))
	{
		g_einvCmd75_t.carOpertPriority = cmd75Para->carOpertPriority;
		cmd75Para->changeFlag = cmd75Para->changeFlag | EINV_SET_CMD75_CAR_OPERT_PRIORITY;
	}
	else
	{
		
	}
	if((cmd75Para->carCharordiscBodyOprat != g_einvCmd75_t.carCharordiscBodyOprat) && (cmd75Para->carCharordiscBodyOprat != EINV_UCHARMAXVALUE))
	{
		g_einvCmd75_t.carCharordiscBodyOprat = cmd75Para->carCharordiscBodyOprat;
		cmd75Para->changeFlag = cmd75Para->changeFlag | EINV_SET_CMD75_CAR_CHARORDISC_BODY_OPRAT;
	}
	else
	{
		
	}
	if((cmd75Para->pwrcutDischar != g_einvCmd75_t.pwrcutDischar) && (cmd75Para->pwrcutDischar != EINV_UCHARMAXVALUE))
	{
		g_einvCmd75_t.pwrcutDischar = cmd75Para->pwrcutDischar;
		cmd75Para->changeFlag = cmd75Para->changeFlag | EINV_SET_CMD75_PWRCUT_DISCHAR;
	}
	else
	{
		
	}
	if((cmd75Para->pwrcutDiscPrioritySet != g_einvCmd75_t.pwrcutDiscPrioritySet) && (cmd75Para->pwrcutDiscPrioritySet != EINV_UCHARMAXVALUE))
	{
		g_einvCmd75_t.pwrcutDiscPrioritySet = cmd75Para->pwrcutDiscPrioritySet;
		cmd75Para->changeFlag = cmd75Para->changeFlag | EINV_SET_CMD75_PWRCUT_DISC_PRIORITY_SET;
	}
	else
	{
		
	}
	if((cmd75Para->pwrcutCharPrioritySet != g_einvCmd75_t.pwrcutCharPrioritySet) && (cmd75Para->pwrcutCharPrioritySet != EINV_UCHARMAXVALUE))
	{
		g_einvCmd75_t.pwrcutCharPrioritySet = cmd75Para->pwrcutCharPrioritySet;
		cmd75Para->changeFlag = cmd75Para->changeFlag | EINV_SET_CMD75_PWRCUT_CHAR_PRIORITY_SET;
	}
	else
	{
		
	}
	if((cmd75Para->connectLockOrUnlock != g_einvCmd75_t.connectLockOrUnlock) && (cmd75Para->connectLockOrUnlock != EINV_UCHARMAXVALUE))
	{
		g_einvCmd75_t.connectLockOrUnlock = cmd75Para->connectLockOrUnlock;
		cmd75Para->changeFlag = cmd75Para->changeFlag | EINV_SET_CMD75_CONNECT_LOCK_OR_UNLOCK;
	}
	else
	{
		
	}
	if((cmd75Para->carDiscLowerLimitSet != g_einvCmd75_t.carDiscLowerLimitSet) && (cmd75Para->carDiscLowerLimitSet != EINV_USHORTMAXVALUE))
	{
		g_einvCmd75_t.carDiscLowerLimitSet = cmd75Para->carDiscLowerLimitSet;
		cmd75Para->changeFlag = cmd75Para->changeFlag | EINV_SET_CMD75_CAR_DISC_LOWERLIMIT_SET;
	}
	else
	{
		
	}
	if((cmd75Para->carCharUpperLimitSet != g_einvCmd75_t.carCharUpperLimitSet) && (cmd75Para->carCharUpperLimitSet != EINV_USHORTMAXVALUE))
	{
		g_einvCmd75_t.carCharUpperLimitSet = cmd75Para->carCharUpperLimitSet;
		cmd75Para->changeFlag = cmd75Para->changeFlag | EINV_SET_CMD75_CAR_CHAR_UPPERLIMIT_SET;
	}
	else
	{
		
	}
	if((cmd75Para->carDiscOrnot != g_einvCmd75_t.carDiscOrnot) && (cmd75Para->carDiscOrnot != EINV_UCHARMAXVALUE))
	{
		g_einvCmd75_t.carDiscOrnot = cmd75Para->carDiscOrnot;
		cmd75Para->changeFlag = cmd75Para->changeFlag | EINV_SET_CMD75_CAR_DISC_ORNOT;
	}
	else
	{
		
	}
	if((cmd75Para->discharPrioritySet != g_einvCmd75_t.discharPrioritySet) && (cmd75Para->discharPrioritySet != EINV_UCHARMAXVALUE))
	{
		g_einvCmd75_t.discharPrioritySet = cmd75Para->discharPrioritySet;
		cmd75Para->changeFlag = cmd75Para->changeFlag | EINV_SET_CMD75_DISCHAR_PRIORITY_SET;
	}
	else
	{
		
	}
	if((cmd75Para->pwrcutCarDiscLowLmtSet != g_einvCmd75_t.pwrcutCarDiscLowLmtSet) && (cmd75Para->pwrcutCarDiscLowLmtSet != EINV_USHORTMAXVALUE))
	{
		g_einvCmd75_t.pwrcutCarDiscLowLmtSet = cmd75Para->pwrcutCarDiscLowLmtSet;
		cmd75Para->changeFlag = cmd75Para->changeFlag | EINV_SET_CMD75_PWRCUT_DISC_LOWERLIMIT_SET;
	}
	else
	{
		
	}
	if((cmd75Para->chargePrioritySet != g_einvCmd75_t.chargePrioritySet) && (cmd75Para->chargePrioritySet != EINV_UCHARMAXVALUE))
	{
		g_einvCmd75_t.chargePrioritySet = cmd75Para->chargePrioritySet;
		cmd75Para->changeFlag = cmd75Para->changeFlag | EINV_SET_CMD75_CHARGE_PRIORITY_SET;
	}
	else
	{
		
	}
	if((cmd75Para->carCharPwrValueSet != g_einvCmd75_t.carCharPwrValueSet) && (cmd75Para->carCharPwrValueSet != EINV_USHORTMAXVALUE))
	{
		g_einvCmd75_t.carCharPwrValueSet = cmd75Para->carCharPwrValueSet;
		cmd75Para->changeFlag = cmd75Para->changeFlag | EINV_SET_CMD75_BTCHAR_PWR_VALUE_SET;
	}
	else
	{
		
	}
	if((cmd75Para->carDiscPwrValueSet != g_einvCmd75_t.carDiscPwrValueSet) && (cmd75Para->carDiscPwrValueSet != EINV_USHORTMAXVALUE))
	{
		g_einvCmd75_t.carDiscPwrValueSet = cmd75Para->carDiscPwrValueSet;
		cmd75Para->changeFlag = cmd75Para->changeFlag | EINV_SET_CMD75_BTDISCHAR_PWR_VALUE_SET;
	}
	else
	{
		
	}
	
}
/*==========================================================================*/
/*	Description	:CMD81試運転状態通知										*/
/*--------------------------------------------------------------------------*/
/*	param		:cmd81Para:EinvCmd81R_t構造体ポインタ						*/
/*--------------------------------------------------------------------------*/
/*	return		:testRunOutIn:試運転状態通知								*/
/*--------------------------------------------------------------------------*/
/*	detail		:試運転状態通知												*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/27 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
uchar EINV_n_GetCmd81TestRunOutInfromValue(void)
{
	return g_einvCmd81_r.testRunoutinfrom;
}
/*merge from sunJT end*/

/*==========================================================================*/
/*	Description		:	算出CMD41表示用シグナル処理値						*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/06 : DHC Chongyang.Chen : 新規作成				*/
/*==========================================================================*/
void EINV_n_CalcDisplayAvgValue(void)
{
	uchar i = 0;
	bool_t breakFlag = FALSE;
	slong	ttlPvGenPwr = 0;
	ulong 	ttlPvPwr2 = 0;
	slong	ttlRevPwrFlow = 0;
	slong	ttlBtChargeDischPwr = 0;
	slong	ttlV2hInstantPower = 0;

	DEBUG_TRCIN();
	
	/* PV発電電力(本製品以外のPV)平均値 */
	while (!breakFlag) 
	{
		if(0 == g_arrayPvGenPwr[i])
		{
			breakFlag = TRUE;
		}
		ttlPvGenPwr = ttlPvGenPwr + g_arrayPvGenPwr[i];
		i++;
		if(i > 9)
		{
			breakFlag = TRUE;
		}
	}
	
	if(1 == i)
	{
		g_pvGenPwrAvg = 0;
	}
	else if( (10==i) && (0!=g_arrayPvGenPwr[i-1]) )
	{
		g_pvGenPwrAvg = ttlPvGenPwr/i;
	}
	else
	{
		g_pvGenPwrAvg = ttlPvGenPwr/(i-1);
	}
	
	/* 太陽光発電電力(AC端計測値)平均値 */
	i = 0;
	breakFlag = FALSE;
	while (!breakFlag) 
	{
		if(0 == g_arrayPvPwr2[i])
		{
			breakFlag = TRUE;
		}
		ttlPvPwr2 = ttlPvPwr2 + g_arrayPvPwr2[i];
		i++;
		if(i > 9)
		{
			breakFlag = TRUE;
		}
	}
	
	if(1 == i)
	{
		g_pvPwr2Avg = 0;
	}
	else if( (10==i) && (0!=g_arrayPvPwr2[i-1]) )
	{
		g_pvPwr2Avg = ttlPvPwr2/i;
	}
	else
	{
		g_pvPwr2Avg = ttlPvPwr2/(i-1);
	}

	/* 逆潮流電力平均値 */
	i = 0;
	breakFlag = FALSE;
	while (!breakFlag) 
	{
		if(0 == g_arrayRevPwrFlow[i])
		{
			breakFlag = TRUE;
		}
		ttlRevPwrFlow = ttlRevPwrFlow + g_arrayRevPwrFlow[i];
		i++;
		if(i > 9)
		{
			breakFlag = TRUE;
		}
	}
	
	if(1 == i)
	{
		g_revPwrFlowAvg = 0;
	}
	else if( (10==i) && (0!=g_arrayRevPwrFlow[i-1]) )
	{
		g_revPwrFlowAvg = ttlRevPwrFlow/i;
	}
	else
	{
		g_revPwrFlowAvg = ttlRevPwrFlow/(i-1);
	}

	/* 蓄電池充放電電力(AC端計測値)平均値 */
	i = 0;
	breakFlag = FALSE;
	while (!breakFlag) 
	{
		if(0 == g_arrayBtChargeDischPwr[i])
		{
			breakFlag = TRUE;
		}
		ttlBtChargeDischPwr = ttlBtChargeDischPwr + g_arrayBtChargeDischPwr[i];
		i++;
		if(i > 9)
		{
			breakFlag = TRUE;
		}
	}
	
	if(1 == i)
	{
		g_btChargeDischPwrAvg = 0;
	}
	else if( (10==i) && (0!=g_arrayBtChargeDischPwr[i-1]) )
	{
		g_btChargeDischPwrAvg = ttlBtChargeDischPwr/i;
	}
	else
	{
		g_btChargeDischPwrAvg = ttlBtChargeDischPwr/(i-1);
	}

	/* V2H瞬時充放電電力計測値(AC端計測値)平均値 */	
	i = 0;
	breakFlag = FALSE;	
	while (!breakFlag) 
	{
		if(0 == g_arrayV2hInstantPower[i])
		{
			breakFlag = TRUE;
		}
		ttlV2hInstantPower = ttlV2hInstantPower + g_arrayV2hInstantPower[i];
		i++;
		if(i > 9)
		{
			breakFlag = TRUE;
		}
	}
	
	if(1 == i)
	{
		g_v2hInstantPowerAvg = 0;
	}
	else if( (10==i) && (0!=g_arrayV2hInstantPower[i-1]) )
	{
		g_v2hInstantPowerAvg = ttlV2hInstantPower/i;
	}
	else
	{
		g_v2hInstantPowerAvg = ttlV2hInstantPower/(i-1);
	}

	/* 太陽光+PV発電電力平均値sum */
	g_pvPwrAvgSum = g_pvPwr2Avg + g_pvGenPwrAvg;

	/* 家庭負荷電力[W] */
	g_homeLoadPwr = ( g_pvPwrAvgSum - g_revPwrFlowAvg - (g_v2hInstantPowerAvg+g_btChargeDischPwrAvg) )/10;
	
	DEBUG_TRCOUT();
}

/*End Of File*/
