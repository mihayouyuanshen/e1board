/****************************************************************************/
/*	Description		:INV通信ミドル 機能処理									*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Daip												*/
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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <poll.h>

#include "typedef.h"
#include "task_g.h"
#include "frm_g.h"
#include "common_g.h"
#include "dlog_g.h"
#include "esys_g.h"
#include "etim_g.h"
#include "edat_g.h"
#include "emdtr_g.h"

#include "einv_g.h"
#include "einv_msg_g.h"
#include "einv_ram_g.h"
#include "einv_utl_g.h"
#include "einv_n_g.h"
#include "einv_n_ram_g.h"
#include "einv_n_utl_g.h"
#include "einv_n_comm_g.h"
#include "einv_n_func_g.h"
#include "einv_n_func.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
const uchar s_einvMonthDayNumTbl[EINV_MONTHNUM_LEN][EINV_YEARTYPE_LEN] =
{
	{EINV_31DAY,	EINV_31DAY},
	{EINV_28DAY,	EINV_29DAY},
	{EINV_31DAY,	EINV_31DAY},
	{EINV_30DAY,	EINV_30DAY},
	{EINV_31DAY,	EINV_31DAY},
	{EINV_30DAY,	EINV_30DAY},
	{EINV_31DAY,	EINV_31DAY},
	{EINV_31DAY,	EINV_31DAY},
	{EINV_30DAY,	EINV_30DAY},
	{EINV_31DAY,	EINV_31DAY},
	{EINV_30DAY,	EINV_30DAY},
	{EINV_31DAY,	EINV_31DAY}
};


/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description	:メインスレッド初期化処理									*/
/*--------------------------------------------------------------------------*/
/*	param		:なし														*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	:処理結果											*/
/*				:		EPCS_OK				正常終了						*/
/*				:		EPCS_ERR_SYSCALL	システムコールエラー			*/
/*				:		EPCS_ERR_FAIL		処理失敗エラー					*/
/*--------------------------------------------------------------------------*/
/*	detail		:メインスレッド初期化処理を行う。							*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/04 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_n_InitMainThread(void)
{
	slong ret = EINV_OK;

	/* 開始ログ出力 */
	DEBUG_TRCIN();



	/* 終了ログ出力 */
	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:通信スレッド初期化処理										*/
/*--------------------------------------------------------------------------*/
/*	param		:fact		:起動要因										*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	:処理結果											*/
/*				:		EPCS_OK				正常終了						*/
/*				:		EPCS_ERR_SYSCALL	システムコールエラー			*/
/*				:		EPCS_ERR_FAIL		処理失敗エラー					*/
/*--------------------------------------------------------------------------*/
/*	detail		:通信スレッド初期化処理を行う。								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/04 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_n_InitCommThread(void)
{
	slong ret = EINV_OK;

	/* 開始ログ出力 */
	DEBUG_TRCIN();



	/* 終了ログ出力 */
	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:CMD01送信処理												*/
/*--------------------------------------------------------------------------*/
/*	param		:msg	:初期化要求メッセージ								*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	:処理結果											*/
/*				:		EPCS_OK				正常終了						*/
/*				:		EPCS_ERR_SYSCALL	システムコールエラー			*/
/*--------------------------------------------------------------------------*/
/*	detail		:CMD01送信処理処理を行う。									*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/14 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_n_InitSendCMD01(EinvMsgInitreqPar_t* msg)
{
	slong ret = EINV_OK;
	slong msgRet = FRM_SUCCESS;
	EinvCmdDataT_t cmdData;
	EinvMsgPar_t par;

	/* 開始ログ出力 */
	DEBUG_TRCIN();

	memset(&cmdData, 0, sizeof(cmdData));
	memset(&par, 0, sizeof(par));

	ret = einv_n_SendCMD01Wrapper(&cmdData, TRUE);
	if (ret == EINV_OK)
	{
		par.initReq.header.msgId = EINV_FID_INIT_CMD01_OK;
	} 
	else
	{
		par.initReq.header.msgId = EINV_FID_INIT_CMDNG;
	}
	par.initReq.fact = msg->fact;

	/* 非同期メッセージ送信 */
	msgRet = FRM_MsgSendAsync(EINV_SPORT, sizeof(par), (void*)&par);
	if(msgRet != FRM_SUCCESS)
	{
		DLOG_Error("FRM_MsgSendEvent fail\n");
		ret = EINV_ERR_SYSCALL;
	}
	else
	{
		/* 処理無し */
	}
	
	/* 終了ログ出力 */
	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:CMD60送信処理												*/
/*--------------------------------------------------------------------------*/
/*	param		:msg	:初期化要求メッセージ								*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	:処理結果											*/
/*				:		EPCS_OK				正常終了						*/
/*				:		EPCS_ERR_SYSCALL	システムコールエラー			*/
/*--------------------------------------------------------------------------*/
/*	detail		:CMD60送信処理処理を行う。									*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/14 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_n_InitSendCMD60(EinvMsgInitreqPar_t* msg)
{
	slong ret = EINV_OK;
	slong msgRet = FRM_SUCCESS;
	EinvCmdDataT_t cmdData;
	EinvMsgPar_t par;
	EinvSysStruct_t sysStruct;

	/* 開始ログ出力 */
	DEBUG_TRCIN();

	memset(&cmdData, 0, sizeof(cmdData));
	memset(&par, 0, sizeof(par));

	ret = einv_n_SendCMD60Wrapper(&cmdData, TRUE);
	if (ret == EINV_OK)
	{
		/* システム構成にV2Hが含まれていない場合は送信しない */
		if (g_einvCmd60_r.sysStruct == EINV_SYS_STRUCT_0				/* "0":太陽光+蓄電池+V2H */	
			|| g_einvCmd60_r.sysStruct == EINV_SYS_STRUCT_2				/* "2":太陽光+V2H */
			|| g_einvCmd60_r.sysStruct == EINV_SYS_STRUCT_3				/* "3":蓄電池+V2H */
			|| g_einvCmd60_r.sysStruct == EINV_SYS_STRUCT_5)			/* "5":V2H */
		{
			par.initReq.header.msgId = EINV_FID_INIT_CMD60_OK;
		} 
		else
		{
			par.initReq.header.msgId = EINV_FID_INIT_CMD90_OK;
		}
	} 
	else
	{
		par.initReq.header.msgId = EINV_FID_INIT_CMDNG;
	}
	par.initReq.fact = msg->fact;

	/* 非同期メッセージ送信 */
	msgRet = FRM_MsgSendAsync(EINV_SPORT, sizeof(par), (void*)&par);
	if(msgRet != FRM_SUCCESS)
	{
		DLOG_Error("FRM_MsgSendEvent fail\n");
		ret = EINV_ERR_SYSCALL;
	}
	else
	{
		/* 処理無し */
	}
	
	/* 終了ログ出力 */
	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:CMD90送信処理												*/
/*--------------------------------------------------------------------------*/
/*	param		:msg	:初期化要求メッセージ								*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	:処理結果											*/
/*				:		EPCS_OK				正常終了						*/
/*				:		EPCS_ERR_SYSCALL	システムコールエラー			*/
/*--------------------------------------------------------------------------*/
/*	detail		:CMD90送信処理処理を行う。									*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/14 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_n_InitSendCMD90(EinvMsgInitreqPar_t* msg)
{
	slong ret = EINV_OK;
	slong msgRet = FRM_SUCCESS;
	EinvCmdDataT_t cmdData;
	EinvMsgPar_t par;

	/* 開始ログ出力 */
	DEBUG_TRCIN();

	memset(&cmdData, 0, sizeof(cmdData));
	memset(&par, 0, sizeof(par));

	ret = einv_n_SendCMD90Wrapper(&cmdData, TRUE);
	if (ret == EINV_OK)
	{
		par.initReq.header.msgId = EINV_FID_INIT_CMD90_OK;
	} 
	else
	{
		par.initReq.header.msgId = EINV_FID_INIT_CMDNG;
	}
	par.initReq.fact = msg->fact;

	/* 非同期メッセージ送信 */
	msgRet = FRM_MsgSendAsync(EINV_SPORT, sizeof(par), (void*)&par);
	if(msgRet != FRM_SUCCESS)
	{
		DLOG_Error("FRM_MsgSendEvent fail\n");
		ret = EINV_ERR_SYSCALL;
	}
	else
	{
		/* 処理無し */
	}

	/* 終了ログ出力 */
	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:CMD64送信処理												*/
/*--------------------------------------------------------------------------*/
/*	param		:msg	:IF要求メッセージ								*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	:処理結果											*/
/*						EINV_OK					正常終了					*/
/*						EINV_ERR_SYSCALL		システムコールエラー		*/
/*						EINV_ERR_PARAM			パラメータエラー			*/
/*						EINV_ERR_FAIL			通信異常					*/
/*--------------------------------------------------------------------------*/
/*	detail		:CMD64送信処理処理を行う。									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/27 : DHC Chongyang.Chen : 新規作成				*/
/*==========================================================================*/
slong EINV_n_SendCMD64(void)
{
	slong ret = EINV_OK;
	slong msgRet = FRM_SUCCESS;
	uchar einvAutoRunMode;
	EinvCmdDataT_t cmdData;
	EinvMsgPar_t par;
	EinvOpemodeGet_t data;
	EinvTime_t sellChargStartTime;
	EinvTime_t sellChargStopTime;
	EinvTime_t sellDisChargStartTime;
	EinvTime_t sellDisChargStopTime;
	EinvTime_t greenChargStartTime;
	EinvTime_t greenChargStopTime;
	EinvTime_t greenDisChargStartTime;
	EinvTime_t greenDisChargStopTime;

	/* 開始ログ出力 */
	DEBUG_TRCIN();

	memset(&cmdData, 0, sizeof(cmdData));
	memset(&par, 0, sizeof(par));
	memset(&data, 0, sizeof(data));

	ret = einv_n_SendCMD64Wrapper(&cmdData, FALSE);
	if (ret == EINV_OK)
	{
		/* 処理無し */
	}
	else
	{
		/* 処理無し */
	}

	data.opeMode = g_einvCmd64_r.opeMode;		/* 運転モード */
	data.emergencyMode = g_einvCmd64_r.emergencyMode;	/* 非常時運転モード */
	data.startOrStop = g_einvCmd64_r.startOrStop;	/* 運転／停止  */
	data.userLowerLimit = g_einvCmd64_r.btLowerLimit;	/* 蓄電池放電下限値設定 */
	data.userUpperLimit = g_einvCmd64_r.btUpperLimit;	/* 蓄電池充電上限値設定 */

	einvAutoRunMode = EINV_n_GetAutoRunModeValue();
	if (einvAutoRunMode == EINV_AUTOSELL)
	{
		EINV_n_GetSellChargStartTimeValue(&sellChargStartTime);
		EINV_n_GetSellChargStopTimeValue(&sellChargStopTime);
		EINV_n_GetSellDisChargStartTimeValue(&sellDisChargStartTime);
		EINV_n_GetSellDisChargStopTimeValue(&sellDisChargStopTime);

		EINV_n_ConvertTimeValueToAscii(&sellChargStartTime, &data.chargeTimeZone.chargeStartTime);	/* 売電モード充電開始時刻 */
		EINV_n_ConvertTimeValueToAscii(&sellChargStopTime, &data.chargeTimeZone.chargeStopTime);		/* 売電モード充電停止時刻 */
		EINV_n_ConvertTimeValueToAscii(&sellDisChargStartTime, &data.chargeTimeZone.dischargeStartTime);	/* 売電モード放電開始時刻 */
		EINV_n_ConvertTimeValueToAscii(&sellDisChargStopTime, &data.chargeTimeZone.dischargeStopTime);	/* 売電モード放電停止時刻 */
	}
	else if (einvAutoRunMode == EINV_AUTOGREEN)
	{
		EINV_n_GetGreenChargStartTimeValue(&greenChargStartTime);
		EINV_n_GetGreenChargStopTimeValue(&greenChargStopTime);
		EINV_n_GetGreenDisChargStartTimeValue(&greenDisChargStartTime);
		EINV_n_GetGreenDisChargStopTimeValue(&greenDisChargStopTime);

		EINV_n_ConvertTimeValueToAscii(&greenChargStartTime, &data.chargeTimeZone.chargeStartTime);	/* グリーンモード充電開始時刻 */
		EINV_n_ConvertTimeValueToAscii(&greenChargStopTime, &data.chargeTimeZone.chargeStopTime);	/* グリーンモード充電停止時刻 */
		EINV_n_ConvertTimeValueToAscii(&greenDisChargStartTime, &data.chargeTimeZone.dischargeStartTime);/* グリーンモード放電開始時刻 */
		EINV_n_ConvertTimeValueToAscii(&greenDisChargStopTime, &data.chargeTimeZone.dischargeStopTime);	/* グリーンモード放電停止時刻 */
	}

	EMDTR_NotifyCmd24DataInfo(&data);

	/* 終了ログ出力 */
	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:CMD65送信処理												*/
/*--------------------------------------------------------------------------*/
/*	param		:msg	:初期化要求メッセージ								*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	:処理結果											*/
/*				:		EPCS_OK				正常終了						*/
/*				:		EPCS_ERR_SYSCALL	システムコールエラー			*/
/*--------------------------------------------------------------------------*/
/*	detail		:CMD65送信処理処理を行う。									*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/14 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_n_InitSendCMD65(EinvMsgInitreqPar_t* msg)
{
	slong ret = EINV_OK;
	slong msgRet = FRM_SUCCESS;
	EinvCmdDataT_t cmdData;
	EinvMsgPar_t par;

	/* 開始ログ出力 */
	DEBUG_TRCIN();

	memset(&cmdData, 0, sizeof(cmdData));
	memset(&par, 0, sizeof(par));

	cmdData.cmd65_t.changeFlag = EINV_SET_CMD65_ALL;
	cmdData.cmd65_t.opeMode = EINV_OPEMODE_6;								/* 運転モード */				//TODO 「V:通信ボード内管理運転モード」と表 2-11から算出する
	cmdData.cmd65_t.emergencyMode = EINV_EMGMODE_0;							/* 非常時運転モード */			//TODO 別紙【設定値一覧】参照
	/* 通信ボードのFW更新後の再起動の場合 */
	if (msg->fact == ESYS_START_FACT_FWRST)
	{
		cmdData.cmd65_t.startOrStop = EINV_INV_START;						/* 運転 */
	} 
	else
	{
		cmdData.cmd65_t.startOrStop = EINV_INV_STOP;						/* 停止 */
	}
	cmdData.cmd65_t.btLowerLimit = EINV_BT_LOWER_LIMIT_INIT;				/* 蓄電池放電下限値設定 */		//TODO 別紙【設定値一覧】参照
	cmdData.cmd65_t.btUpperLimit = EINV_BT_UPPER_LIMIT_INIT;				/* 蓄電池充電上限値設定 */		//TODO 別紙【設定値一覧】参照
	cmdData.cmd65_t.testrunOutAtPwrCut = EINV_TEST_RUN_POWERCUT_OUT_NO;		/* 試運転時停電出力 */
	cmdData.cmd65_t.testrunFlag = EINV_TEST_RUN_FLAG_NO;					/* 試運転指示フラグ */
	cmdData.cmd65_t.chargePwrSet = EINV_CHARGE_PWR_SET_INIT;				/* 充電電力設定値(蓄電池) */	//TODO 2.3に従う
	cmdData.cmd65_t.dischargePwrSet = EINV_DISCHG_PWR_SET_INIT;				/* 放電電力設定値(蓄電池) */	//TODO 2.3に従う
	ret = einv_n_SendCMD65Wrapper(&cmdData, TRUE);
	if (ret == EINV_OK)
	{
		/* システム構成にV2Hが含まれていない場合は送信しない */
		if (g_einvCmd60_r.sysStruct == EINV_SYS_STRUCT_0				/* "0":太陽光+蓄電池+V2H */	
			|| g_einvCmd60_r.sysStruct == EINV_SYS_STRUCT_2				/* "2":太陽光+V2H */
			|| g_einvCmd60_r.sysStruct == EINV_SYS_STRUCT_3				/* "3":蓄電池+V2H */
			|| g_einvCmd60_r.sysStruct == EINV_SYS_STRUCT_5)			/* "5":V2H */
		{
			par.initReq.header.msgId = EINV_FID_INIT_CMD65_OK;
		} 
		else
		{
			par.initReq.header.msgId = EINV_FID_INIT_CMD75_OK;
		}
	} 
	else
	{
		par.initReq.header.msgId = EINV_FID_INIT_CMDNG;
	}
	par.initReq.fact = msg->fact;

	/* 非同期メッセージ送信 */
	msgRet = FRM_MsgSendAsync(EINV_SPORT, sizeof(par), (void*)&par);
	if(msgRet != FRM_SUCCESS)
	{
		DLOG_Error("FRM_MsgSendEvent fail\n");
		ret = EINV_ERR_SYSCALL;
	}
	else
	{
		/* 処理無し */
	}

	/* 終了ログ出力 */
	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description	:試運転CMD送信共通化処理									*/
/*--------------------------------------------------------------------------*/
/*	param		:msg	試運転発行コマンドインタフェース					*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	:処理結果											*/
/*				:		EPCS_OK				正常終了						*/
/*				:		EPCS_ERR_SYSCALL	システムコールエラー			*/
/*				:		EINV_ERR_SYS_CONF	システム構成エラー				*/
/*--------------------------------------------------------------------------*/
/*	detail		:試運転CMD送信共通化処理									*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/28 : DHC Sun.JiTao : 新規作成						*/
/*==========================================================================*/
slong EINV_n_TestRunSendCMD(EinvMsgTestRunreqPar_t* msg)
{
	slong ret = EINV_OK;
	slong msgRet = FRM_SUCCESS;
	EinvCmdDataT_t cmdData;
	EinvMsgPar_t par;

	/* 開始ログ出力 */
	DEBUG_TRCIN();

	memset(&cmdData, 1, sizeof(cmdData));
	memset(&par, 0, sizeof(par));
	
	if(msg->command == EINV_SENDCMD65)
	{
		/*CMD65パラメータ作成*/
		EINV_n_TestRunCalcCmd65Para(&cmdData.cmd65_t,(EinvCmdParaSetOption_t)msg->fact);
	}
	else if(msg->command == EINV_SENDCMD75)
	{
		/*CMD75パラメータ作成*/
		EINV_n_TestRunCalcCmd75Para(&cmdData.cmd75_t);
	}
	else{}
	
	
	ret = EINV_SendCMD(msg->command, &cmdData);
	if (ret == EINV_OK)
	{
		if(msg->command == EINV_SENDCMD65)
		{
			/*状態更新通知EINV_FID_TESTRUN_CMD65_OK*/
			par.header.msgId = EINV_FID_TESTRUN_CMD65_OK;
		}
		else if(msg->command == EINV_SENDCMD75)
		{
			/*状態更新通知EINV_FID_TESTRUN_CMD75_OK*/
			par.header.msgId = EINV_FID_TESTRUN_CMD75_OK;
		}
		else{}
	}
	else
	{
		par.header.msgId = EINV_FID_TESTRUN_CMD_NG;
	}

	/* 非同期メッセージ送信 */
	msgRet = FRM_MsgSendAsync(EINV_SPORT, sizeof(par), (void*)&par);
	if(msgRet != FRM_SUCCESS)
	{
		DLOG_Error("FRM_MsgSendEvent fail\n");
		ret = EINV_ERR_SYSCALL;
	}
	else
	{
		/* 処理無し */
	}
	/* 終了ログ出力 */
	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description	:CMD75送信処理												*/
/*--------------------------------------------------------------------------*/
/*	param		:msg	:初期化要求メッセージ								*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	:処理結果											*/
/*				:		EPCS_OK				正常終了						*/
/*				:		EPCS_ERR_SYSCALL	システムコールエラー			*/
/*--------------------------------------------------------------------------*/
/*	detail		:CMD75送信処理処理を行う。									*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/14 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_n_InitSendCMD75(EinvMsgInitreqPar_t* msg)
{
	slong ret = EINV_OK;
	slong msgRet = FRM_SUCCESS;
	EinvCmdDataT_t cmdData;
	EinvMsgPar_t par;

	/* 開始ログ出力 */
	DEBUG_TRCIN();

	memset(&cmdData, 0, sizeof(cmdData));
	memset(&par, 0, sizeof(par));

	cmdData.cmd75_t.changeFlag = EINV_SET_CMD75_ALL & (~EINV_SET_CMD75_CONNECT_LOCK_OR_UNLOCK);
	cmdData.cmd75_t.carOpertPriority = EINV_CAR_OPERT_PRIORITY_YES;				/* 車両操作優先 */
	cmdData.cmd75_t.carCharordiscBodyOprat = EINV_BODY_OPRAT_YES;				/* 車両充放電器本体操作 */		//TODO 別紙【設定値一覧】参照
	cmdData.cmd75_t.pwrcutDischar = EINV_PWRCUT_DISCHAR_YES;					/* 停電時放電動作 */			//TODO 別紙【設定値一覧】参照
	cmdData.cmd75_t.pwrcutDiscPrioritySet = EINV_PWRCUT_DISC_PRIORITY_BT;		/* 停電時放電優先設定 */		//TODO 別紙【設定値一覧】参照
	cmdData.cmd75_t.pwrcutCharPrioritySet = EINV_PWRCUT_CHAR_PRIORITY_CAR;		/* 停電時充電優先設定 */		//TODO 別紙【設定値一覧】参照
	cmdData.cmd75_t.carDiscLowerLimitSet = EINV_V2H_DISCHG_LOW_LMT_INIT;		/* 車両放電下限値設定 */		//TODO 別紙【設定値一覧】参照
	cmdData.cmd75_t.carCharUpperLimitSet = EINV_V2H_CHARGE_UP_LMT_INIT;			/* 車両充電上限値設定 */		//TODO 別紙【設定値一覧】参照
	cmdData.cmd75_t.carDiscOrnot = EINV_CAR_DISC_YES;							/* 車両放電有無 */				//TODO 別紙【設定値一覧】参照
	cmdData.cmd75_t.discharPrioritySet = EINV_DISCHAR_PRIORIT_BT;				/* 放電優先設定 */				//TODO 別紙【設定値一覧】参照
	cmdData.cmd75_t.pwrcutCarDiscLowLmtSet = EINV_PWRCUT_V2H_DISC_LOW_INIT;		/* 停電時車両放電下限値設定 */	//TODO 別紙【設定値一覧】参照
	cmdData.cmd75_t.chargePrioritySet = EINV_CHAR_PRIORIT_CAR;					/* 充電優先設定 */				//TODO 別紙【設定値一覧】参照
	cmdData.cmd75_t.carCharPwrValueSet = EINV_CAR_CHARGE_SET_INIT;				/* 車載電池充電電力設定値 */	//TODO 2.3に従う
	cmdData.cmd75_t.carDiscPwrValueSet = EINV_CAR_DISCHG_SET_INIT;				/* 車載電池放電電力設定値 */	//TODO 2.3に従う
	ret = einv_n_SendCMD75Wrapper(&cmdData, TRUE);
	if (ret == EINV_OK)
	{
		par.initReq.header.msgId = EINV_FID_INIT_CMD75_OK;
	} 
	else
	{
		par.initReq.header.msgId = EINV_FID_INIT_CMDNG;
	}
	par.initReq.fact = msg->fact;

	/* 非同期メッセージ送信 */
	msgRet = FRM_MsgSendAsync(EINV_SPORT, sizeof(par), (void*)&par);
	if(msgRet != FRM_SUCCESS)
	{
		DLOG_Error("FRM_MsgSendEvent fail\n");
		ret = EINV_ERR_SYSCALL;
	}
	else
	{
		/* 処理無し */
	}

	/* 終了ログ出力 */
	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:CMD6B送信処理												*/
/*--------------------------------------------------------------------------*/
/*	param		:msg	:初期化要求メッセージ								*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	:処理結果											*/
/*				:		EPCS_OK				正常終了						*/
/*				:		EPCS_ERR_SYSCALL	システムコールエラー			*/
/*--------------------------------------------------------------------------*/
/*	detail		:CMD6B送信処理処理を行う。									*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/14 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_n_InitSendCMD6B(EinvMsgInitreqPar_t* msg)
{
	slong ret = EINV_OK;
	slong msgRet = FRM_SUCCESS;
	EinvCmdDataT_t cmdData;
	EinvMsgPar_t par;

	/* 開始ログ出力 */
	DEBUG_TRCIN();

	memset(&cmdData, 0, sizeof(cmdData));
	memset(&par, 0, sizeof(par));

	cmdData.cmd6B_t.changeFlag = EINV_SET_CMD6B_ALL;
	cmdData.cmd6B_t.contractPwrSet = EINV_CONTRACT_PWR_SET_INIT;			/* 契約電力設定値 */		//TODO 別紙【設定値一覧】参照
	cmdData.cmd6B_t.pwrcutOutSet = EINV_PWRCUT_OUT_START;					/* 停電時出力設定 */		//TODO 別紙【設定値一覧】参照
	cmdData.cmd6B_t.pwrcutOutStart = EINV_PWRCUT_OUT_STOP;					/* 停電時出力開始 */		//TODO 別紙【設定値一覧】参照
	cmdData.cmd6B_t.sellValidOrInvalid = EINV_SELL_INVALID;					/* 売電有効無効設定 */		//TODO 別紙【設定値一覧】参照
	cmdData.cmd6B_t.maintenanceIndicate = EINV_MAINTENANCE_INDICATE_STOP;	/* メンテ指示 */
	ret = einv_n_SendCMD6BWrapper(&cmdData, FALSE);
	if (ret == EINV_OK)
	{
		par.initReq.header.msgId = EINV_FID_INIT_CMD6B_OK;
	} 
	else
	{
		par.initReq.header.msgId = EINV_FID_INIT_CMDNG;
	}
	par.initReq.fact = msg->fact;

	/* 非同期メッセージ送信 */
	msgRet = FRM_MsgSendAsync(EINV_SPORT, sizeof(par), (void*)&par);
	if(msgRet != FRM_SUCCESS)
	{
		DLOG_Error("FRM_MsgSendEvent fail\n");
		ret = EINV_ERR_SYSCALL;
	}
	else
	{
		/* 処理無し */
	}

	/* 終了ログ出力 */
	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:通信スレッド初期化終了処理									*/
/*--------------------------------------------------------------------------*/
/*	param		:なし														*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	:処理結果											*/
/*				:		EPCS_OK				正常終了						*/
/*--------------------------------------------------------------------------*/
/*	detail		:通信スレッド初期化終了処理を行う。							*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/17 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_n_InitFin(void)
{
	slong ret = EINV_OK;

	DEBUG_TRCIN();

	/* INV状態設定(接続確立) */
	EINV_SetInvStts(EINV_STTS_COMM_CONF);

	/* 試運転要否の算出 */
	EINV_n_CalcNeedTestRun();

	/* システム機種名の算出 */
	EINV_n_CalcSysModeName();

	/* ENLクラスの生成 */
	/* システム構成変換して、eenlでクラス生成 */

	/* メンテナンス実施指定日の算出 */
	EINV_n_CalcMaintenanceDate();

	/* INVマイコン初期化通信失敗処理 */
	if (g_einvInitErrFlg)
	{
		g_einvInitErrFlg = FALSE;
		EDAT_ClearBoardErr(EINV_ERROR_INIT);
		DLOG_Debug("EDAT_ClearBoardErr:%s", EINV_ERROR_INIT);
	}
	else
	{
		FRM_TimerCancelOneShot(EINV_TIM_ID_INIT_ERR);
		ESYS_NotifyStartResult(EINV_SPORT, MOD_ACTION_RESULT_SUCCESS);
	}

	/* ワンショットタイマ起動 */
	FRM_TimerSetOneShot(EINV_TIM_ID_CYCLE, EINV_CYCLE_TIME_MS, NULL);

	/* INVマイコン初期化通信完了通知 */
	EMDTR_NotifyInvInitCommFin();

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:周期通信処理												*/
/*--------------------------------------------------------------------------*/
/*	param		:なし														*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	処理結果											*/
/*				:		EPCS_OK				正常終了						*/
/*				:		EPCS_ERR_SYSCALL	システムコールエラー			*/
/*				:		EPCS_ERR_FAIL		処理失敗エラー					*/
/*--------------------------------------------------------------------------*/
/*	detail		:周期通信処理を行う。										*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/20 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_n_GetCycleOpeStateInfo(void)
{
	slong ret = EINV_OK;
	EinvCmdDataT_t cmdData;
	EtimTm_t pTime;
	EdatErrInfo_t errInfo;
	schar oldErr[EINV_PCSERR_LEN] = { 0 };
	schar oldErrRating = EINV_ERROR_FREE_RATE;
	uchar oldPowCut = EINV_PWRCUT_0;
	slong compareRet = 0;
	/* 通信ステータス取得 */
	EinvStts_t oldEinvStt = EINV_GetInvStts();

	DEBUG_TRCIN();

	EINV_n_StrNCopy(oldErr, g_einvCmd81_r.pcsErr, sizeof(oldErr));
	oldErrRating = g_einvCmd81_r.pcsErrRating[EINV_PCSERRRAT_1];
	oldPowCut = g_einvCmd81_r.pwrCut;

	EINV_n_NumYear2To4(EINV_PRE_MOMENT_YEAR_INIT, (ushort*)(&pTime.tmYear));
	pTime.tmMon = EINV_PRE_MOMENT_MON_INIT;
	pTime.tmMday = EINV_PRE_MOMENT_DAY_INIT;
	pTime.tmHour = EINV_PRE_MOMENT_HOUR_INIT;
	pTime.tmMin = EINV_PRE_MOMENT_MIN_INIT;
	pTime.tmSec = EINV_PRE_MOMENT_SEC_INIT;
	
	/* 現在時刻を取得 */
	ETIM_GetTime( &pTime );

	EINV_n_NumYear4To2((ushort)(pTime.tmYear), &cmdData.cmd81_t.preMoment.year);
	cmdData.cmd81_t.preMoment.month = (uchar)pTime.tmMon;
	cmdData.cmd81_t.preMoment.day = (uchar)pTime.tmMday;
	cmdData.cmd81_t.preMoment.hour = (uchar)pTime.tmHour;
	cmdData.cmd81_t.preMoment.minute = (uchar)pTime.tmMin;
	cmdData.cmd81_t.preMoment.second = (uchar)pTime.tmSec;
	cmdData.cmd81_t.genPwrLimit = EINV_GEN_PWR_LIMIT_INIT;
	EDAT_GetBoardErrInfo(&errInfo);
	EINV_n_StrNCopy(cmdData.cmd81_t.commuBoardError, errInfo.errCode, sizeof(cmdData.cmd81_t.commuBoardError));
	cmdData.cmd81_t.carCharDischar = EINV_CAR_CHAR_OR_DISC_LIMITED_0;
	ret = EINV_SendCMD(EINV_CMD81, &cmdData);
	if (ret != EINV_OK)
	{
		if ((oldEinvStt == EINV_STTS_COMM_CONF)
			|| (oldEinvStt == EINV_STTS_COMM_INIT_CONF))
		{
			/* エラーワンショットタイマー登録 */
			FRM_TimerSetOneShot((ushort)(EINV_TIM_ID_COMM_ERR + g_einvCommErrSeq), EINV_COMM_ERR_TIME, NULL);
		} 
		else
		{
			/* 処理無し */
		}
	}
	else
	{
		if (g_einvCommErrFlg)
		{
			g_einvCommErrFlg = FALSE;
			EDAT_ClearBoardErr(EINV_ERROR_COMM);
			DLOG_Debug("EDAT_ClearBoardErr:%s", EINV_ERROR_COMM);
		} 
		else
		{
			/* 通信不通状態だった場合 */
			if ((oldEinvStt == EINV_STTS_COMM_ERR)
				|| (oldEinvStt == EINV_STTS_COMM_INIT_ERR))
			{
				/* エラーワンショットタイマー解除 */
				FRM_TimerCancelOneShot((ushort)(EINV_TIM_ID_COMM_ERR + g_einvCommErrSeq));
				g_einvCommErrSeq++;
				g_einvCommErrSeq = (g_einvCommErrSeq % 10);
			}
			else
			{
				/* 処理無し */
			}
		}

		/* PCSエラーとPCSエラーレーティングを設定 */
		compareRet = strcmp(oldErr, g_einvCmd81_r.pcsErr);
		if (compareRet != 0 || oldErrRating != g_einvCmd81_r.pcsErrRating[EINV_PCSERRRAT_1])
		{
			compareRet = strcmp(g_einvCmd81_r.pcsErr, EINV_ERROR_FREE);

			if (compareRet != 0)
			{
				EDAT_SetPcsErr(g_einvCmd81_r.pcsErr, g_einvCmd81_r.pcsErrRating[EINV_PCSERRRAT_1]);
			} 
			else
			{
				EDAT_ClearAllPcsErr();
			}
		} 
		else
		{
			/* 処理無し */
		}

		/* CMD81:停電の動作変更通知 */
		if ((oldPowCut == EINV_PWRCUT_1 || oldPowCut == EINV_PWRCUT_2)
			&& (g_einvCmd81_r.pwrCut != EINV_PWRCUT_1 && g_einvCmd81_r.pwrCut != EINV_PWRCUT_2))
		{
			ESYS_InvNotifyPowerCutSt(OPERATION_PWRON);
		} 
		else if ((oldPowCut != EINV_PWRCUT_1 && oldPowCut != EINV_PWRCUT_2)
			&& (g_einvCmd81_r.pwrCut == EINV_PWRCUT_1 || g_einvCmd81_r.pwrCut == EINV_PWRCUT_2))
		{
			ESYS_InvNotifyPowerCutSt(OPERATION_PWRCUT);
		}
		else
		{
			/* 処理無し */
		}
	}
	
	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:	自動運転モード関連Timer起動停止処理					*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	timerEvent:充放電切替/6時間/外部通信機設置(NOEVENT)/*/
/*					:			   FEEDBACK									*/
/*					:	具体的にEinvTimerEvent_t参照						*/
/*--------------------------------------------------------------------------*/
/*	return			:	   EINV_OK: 0										*/
/*					:	   FRM_TIMER_E_	参照								*/
/*					:	   EINV_ERR_PARAM:-4								*/
/*					:	   ETIM_GetTime:戻り値								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	自動運転モードTimer起動停止処理						*/
/*					:	自動運転時間帯設置変更時呼出す						*/
/*					:	運転モード切替時呼出す								*/
/*					:	充放電時刻切替イベントより、Timer起動停止処理		*/
/*					:	運転モード切替伴う、運転モード切替タイマ起動停止制御*/
/*			:呼出しタイミング①EINV_n_OpModeSetFromSystemInit並び			*/
/*			:呼出しタイミング②EINV_n_OpModeSetFromExternalDevice並び		*/
/*			:呼出しタイミング③EINV_n_OpModeAutoUpdateByTimerEvent並び		*/
/*			:呼出しタイミング④EINV_n_OpModeAutoUpdateByCMD81ResV2h並び		*/
/*			:呼出しタイミング⑤EINV_n_OpModeAutoUpdateByCMD81ResManual並び	*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/19 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
slong EINV_n_AutoRunmodeTimerCtl(EinvTimerEvent_t timerEvent)
{
	slong ret = EINV_OK;
	EinvValidity_t		z;		/*関数入力引数範囲判断用*/
	EtimTm_t		ptime;
	uchar ctlRunMode = EINV_n_GetCtlRunModeValue();
	uchar operationStartStop = EINV_n_GetOperationStartStopValue();

	DEBUG_TRCIN();

	/*システム時間取得*/
	//ret = ETIM_GetTime(&ptime);
	if(ret != ETIM_OK)
	{
		/*システム時間取得失敗*/
		/*ERR CODE ETIMモジュール参照*/
		DLOG_Error( "ETIM_GetTime() error %ld\n", ret );
		DEBUG_TRCOUT();
		return ret;
	}
	else
	{
		/*システム時間取得成功*/
		z = EINV_n_CheckSysTimeValid(&ptime);
	}
	
	if(z == EINV_INVALID)
	{
		ret = EINV_ERR_PARAM;
		DLOG_Error( "EINV_n_autoRunmodeTimerCtl() error %ld\n", ret );
		DEBUG_TRCOUT();
		return ret;
	}
	else
	{
		/* 処理無し */
	}

	if( (ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_0) &&
		(g_einvUpDateFlag.CTLRUNMODE == EINV_SET)
	)
	{
		/*その他モードSELLモードへ遷移*/
		if(operationStartStop == EINV_OPSTART)
		{
			/*運転開始状態*/
			/*SELLモードタイマ起動*/
			ret = einv_n_SellModeTimerCtl(EINV_OPSTART,&ptime);
		}
		else
		{
			/*運転停止状態*/
			/*SELLモードタイマ停止*/
			ret = einv_n_SellModeTimerCtl(EINV_OPSTOP,NULL);
		}
		/*GREENモードタイマ停止*/
		ret = einv_n_GreenModeTimerCtl(EINV_OPSTOP,NULL);
	}
	else if( (ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_1) &&
			(g_einvUpDateFlag.CTLRUNMODE == EINV_SET)
	)
	{
		/*その他モードGreenモードへ遷移*/
		if(operationStartStop == EINV_OPSTART)
		{
			/*運転開始状態*/
			/*Greenモードタイマ起動*/
			ret = einv_n_GreenModeTimerCtl(EINV_OPSTART,&ptime);
		}
		else
		{
			/*運転停止状態*/
			/*Greenモードタイマ停止*/
			ret = einv_n_GreenModeTimerCtl(EINV_OPSTOP,NULL);
		}
		/*sellモードタイマ停止*/
		ret = einv_n_SellModeTimerCtl(EINV_OPSTOP,NULL);
	}
	else if ((ctlRunMode >= EINV_COMMU_BOARD_MANAGE_MODE_2) &&
		(operationStartStop <= EINV_COMMU_BOARD_MANAGE_MODE_7) &&
		(g_einvUpDateFlag.CTLRUNMODE == EINV_SET)
	)
	{
		/*その他モードから手動モードへ遷移*/
		ret = einv_n_GreenModeTimerCtl(EINV_OPSTOP,NULL);
		ret = einv_n_SellModeTimerCtl(EINV_OPSTOP,NULL);
	}
	else if (((ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_0) ||
		(ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_1)) &&
		(g_einvUpDateFlag.CTLRUNMODE == EINV_CLR) &&
		(operationStartStop == EINV_OPSTART) &&
		(g_einvUpDateFlag.OPERATIONSTARTSTOP == EINV_CLR)
		)
	{
		/*自動運転モード個別タイマ更新再起動*/
		/*自動運転モード時刻切替イベント到着、タイマ更新再起動*/
		ret = einv_n_SellGreenModeSingleTimerCtl(timerEvent,&ptime);
	}
	else if (((ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_0) ||
		(ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_1)) &&
		(g_einvUpDateFlag.CTLRUNMODE == EINV_CLR) &&
		(operationStartStop == EINV_OPSTOP) &&
		(g_einvUpDateFlag.OPERATIONSTARTSTOP == EINV_SET)
		)
	{
		/*自動運転モード、運転開始->停止*/
		/*sellモードタイマ停止*/
		/*Greenモードタイマ停止*/
		ret = einv_n_GreenModeTimerCtl(EINV_OPSTOP,NULL);
		ret = einv_n_SellModeTimerCtl(EINV_OPSTOP,NULL);
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();
	
	return ret;
}

/*==========================================================================*/
/*	Description		:	6 hour Timer関連起動停止処理						*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	無し												*/
/*--------------------------------------------------------------------------*/
/*	return			:	EINV_OK: 0											*/
/*					:	FRM_TIMER_E_	参照								*/
/*--------------------------------------------------------------------------*/
/*	detail			:前提条件メンテ状態≠2:メンテ中					   		*/
/*					:通常システム稼働中の状態遷移、システム稼働中からメンテ */
/*					:へ遷移時タイマ停止、試運転へ遷移時タイマ停止			*/
/*					:管理運転モード=7:待機 && 6 時間継続->自動運転モードへ	*/
/*					:呼出すタイミング①システム稼働中の管理運転モード変更後	*/
/*					:呼出すタイミング②システム稼働中からメンテ/試運転/復旧へ*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/19 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
slong EINV_n_SixHourTimerCtl(void)
{
	slong ret = EINV_OK;
	uchar ctlRunMode = EINV_n_GetCtlRunModeValue();
	uchar mainStatus = EINV_n_GetMaintainStatusValue();

	DEBUG_TRCIN();
	
	if ((ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_7) &&
		(mainStatus == EINV_SYS) &&
		(g_einvUpDateFlag.CTLRUNMODE == EINV_SET)
	)
	{
			/*その他運転モードから待機モードへ遷移する*/
			/*システム通常稼働中*/
			/*起動oneshottimer6H*/
			ret = FRM_TimerSetOneShot(EINV_TIM_ID_SIXHOUR, EINV_SIXHOUR, NULL);
	}
	else if((ctlRunMode != EINV_COMMU_BOARD_MANAGE_MODE_7) &&
			(g_einvUpDateFlag.CTLRUNMODE == EINV_SET) &&
			(mainStatus == EINV_SYS)
	)
	{
			/*待機モードからその他運転モードへ遷移する*/
			/*停止oneshottimer	6H*/
			ret = FRM_TimerCancelOneShot(EINV_TIM_ID_SIXHOUR);
	}
	else
	{
		/* 処理無し */
	}

	if((mainStatus != EINV_SYS) &&
	   (g_einvUpDateFlag.MAINSTATUS == EINV_SET)
	)
	{
			/*メンテ状態など他のMAIN状態へ遷移する場合に*/
			/*停止oneshottimer	6H*/
			ret = FRM_TimerCancelOneShot(EINV_TIM_ID_SIXHOUR);
	}
	
	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:通信不通エラー判定処理										*/
/*--------------------------------------------------------------------------*/
/*	param		:なし														*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	応答コード											*/
/*						EINV_OK					正常終了					*/
/*--------------------------------------------------------------------------*/
/*	detail		:通信不通エラー判定処理を行う。								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/28 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_n_SetInvCommErr(void)
{
	slong ret = EINV_OK;

	/* 通信ステータス取得 */
	EinvStts_t einvStt = EINV_GetInvStts();

	/* 開始ログ出力 */
	DEBUG_TRCIN();

	/* 通信不通状態だった場合 */
	if ((einvStt == EINV_STTS_COMM_ERR)
		|| (einvStt == EINV_STTS_COMM_INIT_ERR))
	{
		/* INV通信不通(BF650,4)を通知 */
		EDAT_SetBoardErr(EINV_ERROR_COMM, EINV_ERROR_COMM_RATE);
		g_einvCommErrFlg = TRUE;
	}
	else
	{
		ret = EINV_ERR_FAIL;
	}

	/* 終了ログ出力 */
	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description	:INVマイコン初期化通信失敗判定処理							*/
/*--------------------------------------------------------------------------*/
/*	param		:なし														*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	応答コード											*/
/*						EINV_OK					正常終了					*/
/*--------------------------------------------------------------------------*/
/*	detail		:INVマイコン初期化通信失敗判定処理を行う。					*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/28 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_n_SetInvInitErr(void)
{
	slong ret = EINV_OK;

	/* 開始ログ出力 */
	DEBUG_TRCIN();

	/* INVマイコン初期化通信失敗(bf650,C)を通知 */
	g_einvInitErrFlg = TRUE;
	EDAT_SetBoardErr(EINV_ERROR_INIT, EINV_ERROR_INIT_RATE);
	ESYS_NotifyStartResult(EINV_SPORT, MOD_ACTION_RESULT_FAILED);

	/* 終了ログ出力 */
	DEBUG_TRCOUT();
	return ret;
}


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description	:CMD01送信ラップ処理										*/
/*--------------------------------------------------------------------------*/
/*	param		:cmdData	:CMD01送信データ								*/
/*				:forceSend	:強制送信										*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	応答コード											*/
/*						EINV_OK					正常終了					*/
/*						EINV_ERR_NOT_COMM		通信未確認エラー			*/
/*						EINV_ERR_SYSCALL		システムコールエラー		*/
/*						EINV_ERR_PARAM			パラメータエラー			*/
/*						EINV_ERR_FAIL			通信異常					*/
/*--------------------------------------------------------------------------*/
/*	detail		:CMD01送信ラップ処理を行う。								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/05/09 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong einv_n_SendCMD01Wrapper(EinvCmdDataT_t* cmdData, bool_t forceSend)
{
	slong ret = EINV_OK;
	/* 通信ステータス取得 */
	EinvStts_t einvStt = EINV_GetInvStts();

	DEBUG_TRCIN();

	if ((einvStt == EINV_STTS_COMM_CONF)
		|| (einvStt == EINV_STTS_COMM_INIT_CONF)
		|| forceSend)
	{
		ret = EINV_SendCMD(EINV_CMD01, cmdData);
	}
	else
	{
		ret = EINV_ERR_NOT_COMM;
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:CMD60送信ラップ処理										*/
/*--------------------------------------------------------------------------*/
/*	param		:cmdData	:CMD60送信データ								*/
/*				:forceSend	:強制送信										*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	応答コード											*/
/*						EINV_OK					正常終了					*/
/*						EINV_ERR_NOT_COMM		通信未確認エラー			*/
/*						EINV_ERR_SYSCALL		システムコールエラー		*/
/*						EINV_ERR_PARAM			パラメータエラー			*/
/*						EINV_ERR_FAIL			通信異常					*/
/*--------------------------------------------------------------------------*/
/*	detail		:CMD60送信ラップ処理を行う。								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/05/09 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong einv_n_SendCMD60Wrapper(EinvCmdDataT_t* cmdData, bool_t forceSend)
{
	slong ret = EINV_OK;
	/* 通信ステータス取得 */
	EinvStts_t einvStt = EINV_GetInvStts();

	DEBUG_TRCIN();

	if ((einvStt == EINV_STTS_COMM_CONF)
		|| (einvStt == EINV_STTS_COMM_INIT_CONF)
		|| forceSend)
	{
		ret = EINV_SendCMD(EINV_CMD60, cmdData);
	}
	else
	{
		ret = EINV_ERR_NOT_COMM;
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:CMD90送信ラップ処理										*/
/*--------------------------------------------------------------------------*/
/*	param		:cmdData	:CMD90送信データ								*/
/*				:forceSend	:強制送信										*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	応答コード											*/
/*						EINV_OK					正常終了					*/
/*						EINV_ERR_NOT_COMM		通信未確認エラー			*/
/*						EINV_ERR_SYSCALL		システムコールエラー		*/
/*						EINV_ERR_PARAM			パラメータエラー			*/
/*						EINV_ERR_FAIL			通信異常					*/
/*--------------------------------------------------------------------------*/
/*	detail		:CMD90送信ラップ処理を行う。								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/05/09 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong einv_n_SendCMD90Wrapper(EinvCmdDataT_t* cmdData, bool_t forceSend)
{
	slong ret = EINV_OK;
	/* 通信ステータス取得 */
	EinvStts_t einvStt = EINV_GetInvStts();

	DEBUG_TRCIN();

	if ((einvStt == EINV_STTS_COMM_CONF)
		|| (einvStt == EINV_STTS_COMM_INIT_CONF)
		|| forceSend)
	{
		ret = EINV_SendCMD(EINV_CMD90, cmdData);
	}
	else
	{
		ret = EINV_ERR_NOT_COMM;
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:CMD64送信ラップ処理										*/
/*--------------------------------------------------------------------------*/
/*	param		:cmdData	:CMD64送信データ								*/
/*				:forceSend	:強制送信										*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	応答コード											*/
/*						EINV_OK					正常終了					*/
/*						EINV_ERR_NOT_COMM		通信未確認エラー			*/
/*						EINV_ERR_SYSCALL		システムコールエラー		*/
/*						EINV_ERR_PARAM			パラメータエラー			*/
/*						EINV_ERR_FAIL			通信異常					*/
/*--------------------------------------------------------------------------*/
/*	detail		:CMD64送信ラップ処理を行う。								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/05/09 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong einv_n_SendCMD64Wrapper(EinvCmdDataT_t* cmdData, bool_t forceSend)
{
	slong ret = EINV_OK;
	/* 通信ステータス取得 */
	EinvStts_t einvStt = EINV_GetInvStts();

	DEBUG_TRCIN();

	if ((einvStt == EINV_STTS_COMM_CONF)
		|| (einvStt == EINV_STTS_COMM_INIT_CONF)
		|| forceSend)
	{
		ret = EINV_SendCMD(EINV_CMD64, cmdData);
	}
	else
	{
		ret = EINV_ERR_NOT_COMM;
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:CMD65送信ラップ処理										*/
/*--------------------------------------------------------------------------*/
/*	param		:cmdData	:CMD65送信データ								*/
/*				:forceSend	:強制送信										*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	応答コード											*/
/*						EINV_OK					正常終了					*/
/*						EINV_ERR_NOT_COMM		通信未確認エラー			*/
/*						EINV_ERR_SYSCALL		システムコールエラー		*/
/*						EINV_ERR_PARAM			パラメータエラー			*/
/*						EINV_ERR_FAIL			通信異常					*/
/*--------------------------------------------------------------------------*/
/*	detail		:CMD65送信ラップ処理を行う。								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/05/09 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong einv_n_SendCMD65Wrapper(EinvCmdDataT_t* cmdData, bool_t forceSend)
{
	slong ret = EINV_OK;
	/* 通信ステータス取得 */
	EinvStts_t einvStt = EINV_GetInvStts();

	DEBUG_TRCIN();

	if ((einvStt == EINV_STTS_COMM_CONF)
		|| (einvStt == EINV_STTS_COMM_INIT_CONF)
		|| forceSend)
	{
		ret = EINV_SendCMD(EINV_CMD65, cmdData);
		if (ret == EINV_OK && g_einvCmd65_r.response == EINV_CODE_ACK)
		{
			if (g_einvOpeStopFlg && cmdData->cmd65_t.startOrStop == EINV_INV_START)
			{
				g_einvOpeStopFlg = FALSE;
				EDAT_ClearBoardErr(EINV_ERROR_OPSTOP);
				DLOG_Debug("EDAT_ClearBoardErr:%s", EINV_ERROR_OPSTOP);
			} 
			else if (!g_einvOpeStopFlg && cmdData->cmd65_t.startOrStop == EINV_INV_STOP)
			{
				g_einvOpeStopFlg = TRUE;
				EDAT_SetBoardErr(EINV_ERROR_OPSTOP, EINV_ERROR_OPSTOP_RATE);
			}
			else
			{
				/* 処理無し */
			}
		}
		else
		{
			/* 処理無し */
		}
	} 
	else
	{
		ret = EINV_ERR_NOT_COMM;
	}


	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:CMD75送信ラップ処理										*/
/*--------------------------------------------------------------------------*/
/*	param		:cmdData	:CMD75送信データ								*/
/*				:forceSend	:強制送信										*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	応答コード											*/
/*						EINV_OK					正常終了					*/
/*						EINV_ERR_NOT_COMM		通信未確認エラー			*/
/*						EINV_ERR_SYSCALL		システムコールエラー		*/
/*						EINV_ERR_PARAM			パラメータエラー			*/
/*						EINV_ERR_FAIL			通信異常					*/
/*--------------------------------------------------------------------------*/
/*	detail		:CMD75送信ラップ処理を行う。								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/05/09 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong einv_n_SendCMD75Wrapper(EinvCmdDataT_t* cmdData, bool_t forceSend)
{
	slong ret = EINV_OK;
	/* 通信ステータス取得 */
	EinvStts_t einvStt = EINV_GetInvStts();

	DEBUG_TRCIN();

	if ((einvStt == EINV_STTS_COMM_CONF)
		|| (einvStt == EINV_STTS_COMM_INIT_CONF)
		|| forceSend)
	{
		ret = EINV_SendCMD(EINV_CMD75, cmdData);
	}
	else
	{
		ret = EINV_ERR_NOT_COMM;
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:CMD6B送信ラップ処理										*/
/*--------------------------------------------------------------------------*/
/*	param		:cmdData	:CMD6B送信データ								*/
/*				:forceSend	:強制送信										*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	応答コード											*/
/*						EINV_OK					正常終了					*/
/*						EINV_ERR_NOT_COMM		通信未確認エラー			*/
/*						EINV_ERR_SYSCALL		システムコールエラー		*/
/*						EINV_ERR_PARAM			パラメータエラー			*/
/*						EINV_ERR_FAIL			通信異常					*/
/*--------------------------------------------------------------------------*/
/*	detail		:CMD6B送信ラップ処理を行う。								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/05/09 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong einv_n_SendCMD6BWrapper(EinvCmdDataT_t* cmdData, bool_t forceSend)
{
	slong ret = EINV_OK;
	/* 通信ステータス取得 */
	EinvStts_t einvStt = EINV_GetInvStts();

	DEBUG_TRCIN();

	if ((einvStt == EINV_STTS_COMM_CONF)
		|| (einvStt == EINV_STTS_COMM_INIT_CONF)
		|| forceSend)
	{
		ret = EINV_SendCMD(EINV_CMD6B, cmdData);
	}
	else
	{
		ret = EINV_ERR_NOT_COMM;
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:	POWERON時 運転モード設定処理						*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	usbModeSet	USB値で設定する場合、設定値格納			*/
/*	param	in		:	setOption	USB或いはデフォールト値設定指定			*/
/*	param	in/out	:	cmd65Data	CMD65構造体アクセス						*/
/*--------------------------------------------------------------------------*/
/*	return			:EINV_CMD65_SENDREQUEST	CMD65発行必要  					*/
/*					:EINV_CMD_SENDNOREQUEST	CMD発行必要無し	0 	EINV_OK		*/
/*					:EINV_CMD75_SENDREQUEST	CMD75発行必要					*/
/*					:EINV_CMD75_SENDREQUEST | EINV_CMD65_SENDREQUEST  同時	*/
/*					:EINV_ERR_PARAM:-4		関数パラメータ異常				*/
/*					:ETIM_GetTime:戻り値									*/
/*--------------------------------------------------------------------------*/
/*	detail			:	運転モード設置処理									*/
/*					:	システムPOWER ON時呼出す							*/
/*					:	システム復元後の設定値								*/
/*					:	USBから設定値あるなら、USB設定値で初期化			*/
/*					:	USBから設定値ないなら、default設定値で初期化		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/18 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
static slong  einv_n_OpModeSetFromSystemInit(const EinvUsbModeSet_t * usbModeSet,EinvSetOption_t setOption, EinvCmd65T_t* cmd65Data)
{
	uchar	cmd65RunMode;
	uchar   calcCmd65RunModeResult;
	slong ret = EINV_CMD_SENDNOREQUEST;
	slong subret = EINV_NOUPDATE;
	EinvTime_t	nowTime;
	EtimTm_t ptime;
	EinvUpdate_t upDateFlag;

	EinvValidity_t		x,y,m,n,o,p,a,b,c,d,z;/*関数入力引数範囲判断用*/
	
	DEBUG_TRCIN();

	/*自動運転切替時刻設定	初期化：表 2-10 管理用パラメータ デフォールト値*/
	EinvTime_t	sellChargeStartTime = {1u,0u};
	EinvTime_t	sellChargeStopTime = {5u,59u};
	EinvTime_t	sellDisChargeStartTime = {6u,0u};
	EinvTime_t	sellDisChargeStopTime = {0u,50u};
	EinvTime_t	greenChargeStartTime = {1u,0u};
	EinvTime_t	greenChargeStopTime = {1u,1u};
	EinvTime_t	greenDisChargeStartTime = {1u,2u};
	EinvTime_t	greenDisChargeStopTime = {0u,50u};
	
	/*システム時間取得*/
	//ret = ETIM_GetTime(&ptime);
	if(ret != ETIM_OK)
	{	
		/*システム時間取得失敗*/
		/*ERR CODE ETIMモジュール参照*/
		DLOG_Error( "ETIM_GetTime() error %ld\n", ret );
		DEBUG_TRCOUT();
		return ret;
	}
	else
	{
		/*システム時間取得成功*/
		z = EINV_n_CheckSysTimeValid(&ptime);
	}
	
	if((usbModeSet != NULL) && (setOption == EINV_USB))
	{
		/*パラメータチェック前処理*/
		a = EINV_n_CheckTimeValid(&usbModeSet->sellChargeStartTime);
		b = EINV_n_CheckTimeValid(&usbModeSet->sellChargeStopTime);
		c = EINV_n_CheckTimeValid(&usbModeSet->sellDisChargeStartTime);
		d = EINV_n_CheckTimeValid(&usbModeSet->sellDisChargeStopTime);
		m = EINV_n_CheckTimeValid(&usbModeSet->greenChargeStartTime);
		n = EINV_n_CheckTimeValid(&usbModeSet->greenChargeStopTime);
		o = EINV_n_CheckTimeValid(&usbModeSet->greenDisChargeStartTime);
		p = EINV_n_CheckTimeValid(&usbModeSet->greenDisChargeStopTime);
		x = EINV_n_CheckSetRunModeFromSmartPhone(usbModeSet->ctlRunMode);
		y = EINV_n_CheckPowerOffRunMode(usbModeSet->powerOffRunMode);
	}
	else
	{
		/* 処理無し */
	}

	if(((setOption == EINV_USB) && (usbModeSet == NULL)) ||
		((setOption == EINV_USB) && (usbModeSet != NULL) && ((a == EINV_INVALID)||(b == EINV_INVALID)||(c == EINV_INVALID)||
		(d == EINV_INVALID)||(m == EINV_INVALID)||(n == EINV_INVALID)||(o == EINV_INVALID)||(p == EINV_INVALID)||(x == EINV_INVALID)||(y == EINV_INVALID))) ||
		((setOption == EINV_DEFAULT) && (usbModeSet != NULL)) ||
		(cmd65Data == NULL) ||
		(z == EINV_INVALID)
	)
	{
		/*入力引数チェック*/
		ret = EINV_ERR_PARAM;
		DLOG_Error( "EINV_n_OpModeSetFromSystemInit() error %ld\n", ret );
		DEBUG_TRCOUT();
		return ret;
	}
	else
	{
		/* 処理無し */
	}
	
	/*一旦、初期状態：システム稼働状態にして置く*/
	EINV_n_SetSysStatusValue(EINV_SYS);

	if(setOption == EINV_USB)
	{
		/*自動運転切替時刻設定	初期化:USBファイル設定値*/
		sellChargeStartTime = usbModeSet->sellChargeStartTime;
		sellChargeStopTime = usbModeSet->sellChargeStopTime;
		sellDisChargeStartTime = usbModeSet->sellDisChargeStartTime;
		sellDisChargeStopTime = usbModeSet->sellDisChargeStopTime;
		greenChargeStartTime = usbModeSet->greenChargeStartTime;
		greenChargeStopTime = usbModeSet->greenChargeStopTime;
		greenDisChargeStartTime = usbModeSet->greenDisChargeStartTime;
		greenDisChargeStopTime = usbModeSet->greenDisChargeStopTime;
	}
	else
	{
		/* 処理無し */
	}
	if(setOption == EINV_USB)
	{
		/*管理運転モード設定　  初期化:USB指定*/
		EINV_n_SetCtlRunModeValue(usbModeSet->ctlRunMode);
		if((usbModeSet->ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_1) || (usbModeSet->ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_0))
		{
			EINV_n_SetAutoRunModeValue(usbModeSet->ctlRunMode);
		}
		else
		{
			/* 処理無し */
		}
	}
	else
	{
		/*管理運転モード設定　  初期化：グリーンモード*/
		EINV_n_SetCtlRunModeValue(EINV_COMMU_BOARD_MANAGE_MODE_1);
		/*自動運転モード設定	初期化：グリーンモード*/
		EINV_n_SetAutoRunModeValue(EINV_AUTOGREEN);
	}
	
	EINV_n_GetUpDateFlag(&upDateFlag);
	if(upDateFlag.CTLRUNMODE == EINV_SET)
	{
		/*管理運転モード変更時、管理運転モード設定元設定*/
		/*管理運転モード設定元  初期化：システム*/
		EINV_n_SetRunModeSetFromValue(EINV_SYSTEM);
	}
	else
	{
		/* 処理無し */
	}
	
	/*自動運転切替時刻設定	初期化：表 2-10 管理用パラメータ*/
	/*Greenモード時刻初期化*/
	EINV_n_SetGreenChargStartTimeValue(&greenChargeStartTime);
	EINV_n_SetGreenChargStopTimeValue(&greenChargeStopTime);
	EINV_n_SetGreenDisChargStartTimeValue(&greenDisChargeStartTime);
	EINV_n_SetGreenDisChargStopTimeValue(&greenDisChargeStopTime);
	/*Sellモード時刻初期化*/
	EINV_n_SetSellChargStartTimeValue(&sellChargeStartTime);
	EINV_n_SetSellChargStopTimeValue(&sellChargeStopTime);
	EINV_n_SetSellDisChargStartTimeValue(&sellDisChargeStartTime);
	EINV_n_SetSellDisChargStopTimeValue(&sellDisChargeStopTime);
	
	if(setOption == EINV_USB)
	{
		/*停電時運転モード設定	初期化：自動給電切換*/
		EINV_n_SetPowerOffRunModeValue(usbModeSet->powerOffRunMode);
	}
	else
	{
		/*停電時運転モード設定	初期化：自動給電切換*/
		EINV_n_SetPowerOffRunModeValue(EINV_POWEROFFAUTOPOWERCTL);
	}
	/*運転開始停止設定		初期化：運転停止*/
	EINV_n_SetOperationStartStopValue(EINV_OPSTOP);
	/*ENL系設定*/
	/*ENL系蓄電池クラス_運転モード初期値:待機*/
	EINV_n_SetBatteryRunmodeValue(EINV_ENLIDLE);
	/*V:V2Hクラス_運転モード初期値:待機*/
	EINV_n_SetV2HRunmodeValue(EINV_ENLIDLE);
	/*V:同時放電モード優先度*/
	EINV_n_SetDischargePriorityValue(EINV_ENLBATTERY);
	/*CMD65コマンドメンバー再算出判定*/
	subret = EINV_n_CalcCmd65Request();
	if(subret == EINV_UPDATE)
	{
		/*再算出必要*/

		/*運転モード算出*/
		
		/*カレント時刻取得*/
			
		/*時刻変換*/
		nowTime.hour = ptime.tmHour;
		nowTime.min = ptime.tmMin;
		
		/*カレントCMD65運転モード取得cmd65RunMode*/
		cmd65RunMode = cmd65Data->opeMode;
		/*CMD65運転モード算出処理*/
		subret = EINV_n_CalcCMD65Runmode(&nowTime,cmd65RunMode,&calcCmd65RunModeResult);
		if(subret == EINV_UPDATE)
		{
			/*カレントCMD65運転モード設置calcCmd65RunModeResult*/
			cmd65Data->opeMode = calcCmd65RunModeResult;
			/*CMD65コマンド変更フラッグ設定*/
			cmd65Data->changeFlag = cmd65Data->changeFlag | EINV_SET_CMD65_OPE_MODE;
			/*CMD65コマンド発行要求設置*/
			ret = ret | EINV_CMD65_SENDREQUEST;
		}
		else
		{
			/*カレントCMD65運転モード設置不要*/
			ret = ret | EINV_CMD_SENDNOREQUEST;
		}
		
	}
	else
	{
		ret = ret | EINV_CMD_SENDNOREQUEST;
	}
	
	DEBUG_TRCOUT();
	return ret;
	
}

/*==========================================================================*/
/*	Description		:	外部通信機から運転モード設定処理					*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	opModeSet:運転モード設置処理						*/
/*	param	in/out	:	cmd65Data:cmd65構造体アクセス						*/
/*--------------------------------------------------------------------------*/
/*	return			:EINV_CMD65_SENDREQUEST	CMD65発行必要  					*/
/*					:EINV_CMD_SENDNOREQUEST	CMD発行必要無し	EINV_OK			*/
/*					:EINV_CMD75_SENDREQUEST	CMD75発行必要					*/
/*					:EINV_CMD75_SENDREQUEST | EINV_CMD65_SENDREQUEST		*/
/*					:EINV_ERR_PARAM:-4		関数パラメータ異常				*/
/*					:EINV_ERR_SYS_CONF:-9	非対応システム構成エラー		*/
/*					:EINV_ERR_NOT_SUPPORT:-6	非サポートエラー			*/
/*					:その他、ETIM_モジュールエラーコード参照				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	外部通信機から運転モード設置処理					*/
/*					:	CMD65構造体メンバーの値変更							*/
/*					:	外部通信機器から運転モード設置時呼出す				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/19 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
static slong  einv_n_OpModeSetFromExternalDevice(const EinvOpModeSet_t * opModeSet, EinvCmd65T_t* cmd65Data)
{
	uchar	ctlRunMode;/*管理運転モード保存用*/
	EinvTime_t	nowTime;/*カレント時刻保存用*/
	uchar	cmd65RunMode;/*カレントCMD65運転モード保存用*/
	uchar   calcCmd65RunModeResult = EINV_INVALIDVALUE;	/*算出した或いは設定したCMD65運転モード保存用*/
	uchar	cmd65OpStartStop;/*カレントCMD65運転／停止保存用*/
	uchar   calcCmd65OpStartStopResult = EINV_INVALIDVALUE;;/*算出した或いは設定したCMD65運転／停止保存用*/
	uchar	cmd65PowerOffRunMode;/*カレントCMD65非常時運転モード保存用*/
	uchar   calcCmd65PowerOffRunModeResult = EINV_INVALIDVALUE;;/*算出した或いは設定したCMD65非常時運転モード保存用*/
	slong ret = EINV_CMD_SENDNOREQUEST;/*関数戻り値*/
	slong subret = EINV_ERR_PARAM;/*関数戻り値*/
	EinvValidity_t		x,y,z,m,n,o,p,a,b,c,d,e;/*関数入力引数範囲判断用*/
	EinvSupport_t	f;/*関数入力引数範囲判断用*/
	uchar 	sysStructure;	/*システム構成*/
	EinvEnlSetFromStatus_t enlSetFromStatus;/*ENL設定元初回更新判断用*/
	EinvEnlSetFrom_t enlSetFrom;	/*カレントENL設定元*/
	EtimTm_t ptime;
	EinvUpdate_t upDateFlag;
	
	DEBUG_TRCIN();

	/*システム時間取得*/
	//ret = ETIM_GetTime(&ptime);
	if(ret != ETIM_OK)
	{
		/*システム時間取得失敗*/
		/*ERR CODE ETIMモジュール参照*/
		DLOG_Error( "ETIM_GetTime() error %ld\n", ret );
		DEBUG_TRCOUT();
		return ret;
	}
	else
	{
		/*システム時間取得成功*/
		e = EINV_n_CheckSysTimeValid(&ptime);
	}
	
	/*その他モジュールからシステム構成変数sysStructure取得*/
	sysStructure = g_einvCmd60_r.sysStruct;
	ret = EINV_n_CheckSysStructure(sysStructure);
	if(ret == EINV_INVALID)
	{
		ret = EINV_ERR_SYS_CONF;
		DLOG_Error( "EINV_n_CheckSysStructure() error %ld\n", ret );
		DEBUG_TRCOUT();
		return ret;
	}
	else
	{
		/* 処理無し */
	}
	
	if(opModeSet != NULL)
	{
		/*外部通信機運転モード設定元設定値範囲チェック*/
		x = EINV_n_CheckRunModeSetFromExternal(opModeSet->runModeSetFrom);
		/*Enl運転モード設定値範囲チェック*/
		y = EINV_n_CheckRunModeExternalEnl(opModeSet->enlRunMode);
		/*OP運転モード設定値範囲チェック*/
		z = EINV_n_CheckSetRunModeFromOpRemote(opModeSet->runMode);
		/*smartphone運転モード設定値範囲チェック*/
		m = EINV_n_CheckSetRunModeFromSmartPhone(opModeSet->runMode);
		/*見守りサーバ運転モード設定値範囲チェック*/
		n = EINV_n_CheckSetRunmodeFromServer(opModeSet->runMode);
		/*運転開始停止設定値範囲チェック*/
		o = EINV_n_CheckOpStartStop(opModeSet->operationStartStopValue);
		/*非常時運転モードチェック*/
		p = EINV_n_CheckPowerOffRunMode(opModeSet->powerOffRunMode);
		/*充電放電時刻範囲チェック*/
		a = EINV_n_CheckTimeValid(&opModeSet->chargeStartStop.start);
		b = EINV_n_CheckTimeValid(&opModeSet->chargeStartStop.end);
		c = EINV_n_CheckTimeValid(&opModeSet->dischargeStartStop.start);
		d = EINV_n_CheckTimeValid(&opModeSet->dischargeStartStop.end);
	}
	else
	{
		/* 処理無し */
	}
	if((opModeSet == NULL) ||
		(x == EINV_INVALID) || 
		((opModeSet->runModeSetFrom == EINV_EXTERNAL_ENL) && ((opModeSet->enlSetFrom != EINV_ENLBATTERYSET) && (opModeSet->enlSetFrom != EINV_ENLV2HSET))) || 
		((opModeSet->runModeSetFrom == EINV_EXTERNAL_ENL) && (y == EINV_INVALID)) || 
		((opModeSet->runModeSetFrom == EINV_EXTERNAL_OP) && (z == EINV_INVALID)) || 
		((opModeSet->runModeSetFrom == EINV_EXTERNAL_SMARTPHONE) && (m == EINV_INVALID)) ||
		(((opModeSet->runModeSetFrom >= EINV_EXTERNAL_SERVER_CLIMATEALARM) && (opModeSet->runModeSetFrom <= EINV_EXTERNAL_SERVER_AIAUTOCTL)) && (n == EINV_INVALID)) ||
		(o == EINV_INVALID) ||
		(p == EINV_INVALID) ||
		(((opModeSet->runMode == EINV_COMMU_BOARD_MANAGE_MODE_0)||(opModeSet->runMode == EINV_COMMU_BOARD_MANAGE_MODE_1))&&((a == EINV_INVALID)||(b == EINV_INVALID)||(c == EINV_INVALID)||(d == EINV_INVALID))) ||
		(cmd65Data == NULL) ||
		(e == EINV_INVALID)
	)
	{
		/*入力パラメータチェック*/
		ret = EINV_ERR_PARAM;
		DLOG_Error( "EINV_n_opModeSetFromExternalDevice() error %ld\n", ret );
		DEBUG_TRCOUT();
		return ret;
	}
	else
	{
		/* 処理無し */
	}
	if((opModeSet->runModeSetFrom >= EINV_EXTERNAL_SERVER_CLIMATEALARM) && (opModeSet->runModeSetFrom <= EINV_EXTERNAL_SERVER_AIAUTOCTL))
	{
		/*見守りサーバから運転モード設定サポートかどうかチェック*/
		f = EINV_n_CheckSetRunmodeSupportOrNotFromServer(opModeSet->runMode,sysStructure);
		if(f == EINV_NOTSUPPORT)
		{
			ret = EINV_ERR_NOT_SUPPORT;
			DLOG_Error( "EINV_n_CheckSetRunmodeSupportOrNotFromServer() error %ld\n", ret );
			DEBUG_TRCOUT();
			return ret;
		}
		else
		{
			/* 処理無し */
		}
	}
	else
	{
		/* 処理無し */
	}
	if(opModeSet->runModeSetFrom == EINV_EXTERNAL_ENL)
	{
		/*ENL設定元の場合、V2H或いは蓄電池*/
		enlSetFrom = EINV_n_GetENLRunmodeFromValue();
		if(enlSetFrom == EINV_INVALIDVALUE)
		{
			enlSetFromStatus = EINV_INITSET;
		}
		else
		{
			enlSetFromStatus = EINV_NORMALSET;
		}

		EINV_n_SetENLRunmodeFromValue(opModeSet->enlSetFrom);

		/*ENL運転モード設定*/
		if(opModeSet->enlSetFrom == EINV_ENLBATTERYSET)
		{
			/*ENL BATT運転モード設定*/
			EINV_n_SetBatteryRunmodeValue(opModeSet->enlRunMode);
		}
		else
		{
			/*ENL V2H運転モード設定*/
			EINV_n_SetV2HRunmodeValue(opModeSet->enlRunMode);
		}
	}
	else
	{
		/* 処理無し */
	}
	
	if(opModeSet->runModeSetFrom == EINV_EXTERNAL_ENL)
	{
		/*ENL運転モードから管理運転モードを算出する*/
		ctlRunMode = EINV_n_CalcRunModeByEnl(enlSetFromStatus,opModeSet->enlRunMode);
		
		if(ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_6)
		{
			/*同時放電優先度設置*/
			if(opModeSet->enlSetFrom == EINV_ENLBATTERYSET){
				/*同時放電優先度設置：蓄電池*/
				EINV_n_SetDischargePriorityValue(EINV_ENLBATTERY);
			}	
			else{
				/*同時放電優先度設置:車両*/
				EINV_n_SetDischargePriorityValue(EINV_ENLCAR);
			}
		}
		else
		{
			/* 処理無し */
		}
	}
	else if(opModeSet->runModeSetFrom == EINV_EXTERNAL_OP)
	{
		/*OPリモコン設定*/
		ctlRunMode = EINV_n_ConvertOpRemoteRunModeValue(opModeSet->runMode);
	}
	else if(opModeSet->runModeSetFrom == EINV_EXTERNAL_SMARTPHONE)
	{
		/*SMARTPHONE設定*/
		ctlRunMode = opModeSet->runMode;
		
	}
	else if((opModeSet->runModeSetFrom >= EINV_EXTERNAL_SERVER_CLIMATEALARM) &&
		   (opModeSet->runModeSetFrom <= EINV_EXTERNAL_SERVER_AIAUTOCTL)
	)
	{
		/*見守りサーバ設定*/
		ctlRunMode = opModeSet->runMode;
	}
	else
	{
		/* 処理無し */
	}

	if( (ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_0) || 
		(ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_1)
	)
	{
		/*自動運転モード設定*/
		EINV_n_SetAutoRunModeValue(ctlRunMode);
		/*自動運転切替時刻設定*/
		if (ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_0)
		{
				/*SELL*/
				EINV_n_SetSellChargStartTimeValue(&(opModeSet->chargeStartStop.start));
				EINV_n_SetSellChargStopTimeValue(&(opModeSet->chargeStartStop.end));
				EINV_n_SetSellDisChargStartTimeValue(&(opModeSet->dischargeStartStop.start));
				EINV_n_SetSellDisChargStopTimeValue(&(opModeSet->dischargeStartStop.end));
		}
		else 
		{
				/*GREEN*/
				EINV_n_SetGreenChargStartTimeValue(&(opModeSet->chargeStartStop.start));
				EINV_n_SetGreenChargStopTimeValue(&(opModeSet->chargeStartStop.end));
				EINV_n_SetGreenDisChargStartTimeValue(&(opModeSet->dischargeStartStop.start));
				EINV_n_SetGreenDisChargStopTimeValue(&(opModeSet->dischargeStartStop.end));
		}
	}
	else
	{
		/*その他モード時刻設定不要*/
	}

	/*管理運転モード設定*/
	EINV_n_SetCtlRunModeValue(ctlRunMode);
	
	EINV_n_GetUpDateFlag(&upDateFlag);
	if(upDateFlag.CTLRUNMODE == EINV_SET)
	{
		/*管理運転モード変更時、管理運転モード設定元設定*/
		/*運転モード設定元設置*/
		if((opModeSet->runModeSetFrom >= EINV_EXTERNAL_ENL) &&
	  	(opModeSet->runModeSetFrom <= EINV_EXTERNAL_SERVER_AIAUTOCTL)
		)
		{
			/*運転モード設定元更新*/
			EINV_n_SetRunModeSetFromValue(opModeSet->runModeSetFrom);
		}
		else if((opModeSet->runModeSetFrom == EINV_EXTERNAL_SMARTPHONE) ||
				(opModeSet->runModeSetFrom == EINV_EXTERNAL_OP)
		)
		{
			EINV_n_SetRunModeSetFromValue(EINV_SMARTPHONEOP);
		}
		else
		{
			/* 処理無し */
		}
		
		/*運転モード設定元ENL以外の運転設定元に遷移する場合、ENL関連グローバル変数初期化*/
		EINV_n_InitEnlRunMode();
	}
	else
	{
		/* 処理無し */
	}
	
	/*停電時運転モード設定*/
	EINV_n_SetPowerOffRunModeValue(opModeSet->powerOffRunMode);
	/*運転開始停止設定*/
	EINV_n_SetOperationStartStopValue(opModeSet->operationStartStopValue);
	
	/*CMD65コマンドメンバー再算出判定*/
	subret = EINV_n_CalcCmd65Request();

	if(subret == EINV_UPDATE)
	{
			/*再算出必要であれば*/
			
			/*運転開始停止算出*/
			/*カレントCMD65運転開始停止取得 cmd65OpStartStop*/
			cmd65OpStartStop = cmd65Data->startOrStop;
			subret = EINV_n_CalcCMD65OpStartStop(cmd65OpStartStop,&calcCmd65OpStartStopResult);
			if(subret == EINV_UPDATE)
			{
				/*カレントCMD65運転開始停止設置calcCmd65OpStartStopResult*/
				cmd65Data->startOrStop = calcCmd65OpStartStopResult;
				cmd65Data->changeFlag = cmd65Data->changeFlag | EINV_SET_CMD65_START_OR_STOP;
				x = EINV_VALID;
			}
			else
			{
				/*カレントCMD65運転開始停止設置不要*/
				x = EINV_INVALID;
			}
			
			/*非常時運転モード算出*/
			/*カレントCMD65非常時運転モード取得cmd65PowerOffRunMode*/
			cmd65PowerOffRunMode = cmd65Data->emergencyMode;
			subret = EINV_n_CalcCMD65PoweroffRunMode(cmd65PowerOffRunMode,&calcCmd65PowerOffRunModeResult);
			if(subret == EINV_UPDATE)
			{
				/*カレントCMD65非常時運転モード設置calcCmd65PowerOffRunModeResult*/
				cmd65Data->emergencyMode = calcCmd65PowerOffRunModeResult;
				cmd65Data->changeFlag = cmd65Data->changeFlag | EINV_SET_CMD65_EMG_MODE;
				y = EINV_VALID;
			}
			else
			{
				/*カレントCMD65非常時運転モード設置不要*/
				y = EINV_INVALID;
			}
			
			/*運転モード算出*/
		
			/*カレント時刻取得*/
			/*時刻変換*/
			nowTime.hour = ptime.tmHour;
			nowTime.min  = ptime.tmMin;
			
			/*カレントCMD65運転モード取得cmd65RunMode*/
			cmd65RunMode = cmd65Data->opeMode;
			/*CMD65運転モード算出処理*/
			subret = EINV_n_CalcCMD65Runmode(&nowTime,cmd65RunMode,&calcCmd65RunModeResult);
			if(subret == EINV_UPDATE)
			{
				/*カレントCMD65運転モード設置calcCmd65RunModeResult*/
				cmd65Data->opeMode = calcCmd65RunModeResult;
				cmd65Data->changeFlag = cmd65Data->changeFlag | EINV_SET_CMD65_OPE_MODE;
				z = EINV_VALID;
			}
			else
			{
				/*カレントCMD65運転モード設置不要*/
				z = EINV_INVALID;
			}
			if((x == EINV_VALID) || (y == EINV_VALID) || (z == EINV_VALID))
			{
				ret = ret | EINV_CMD65_SENDREQUEST;
			}
			else
			{
				ret = ret | EINV_CMD_SENDNOREQUEST;
			}
	}
	else
	{
		ret = ret | EINV_CMD_SENDNOREQUEST;
	}
	
	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:	Timerイベント処理より、運転モード自動更新処理		*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	timerEvent			 :自動運転時刻切替タイマイベント*/
/*											 :待機モード持続6時間イベント   */
/*	param	in/out	:	cmd65Data:CMD65構造体アクセス						*/
/*--------------------------------------------------------------------------*/
/*	return			:EINV_CMD65_SENDREQUEST	CMD65発行必要  					*/
/*					:EINV_CMD_SENDNOREQUEST	CMD発行必要無し		EINV_OK		*/
/*					:EINV_CMD75_SENDREQUEST	CMD75発行必要					*/
/*					:EINV_CMD75_SENDREQUEST | EINV_CMD65_SENDREQUEST		*/
/*					:EINV_ERR_PARAM:-4		関数パラメータ異常				*/
/*					:その他、ETIM_モジュールエラーコード参照				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	自動運転タイマイベント処理 alarm 	自動運転時刻切替*/
/*					:	自動運転タイマイベント処理 oneshot	待機モード6時間	*/
/*					:	呼出すタイミング：タイマイベント対応箇所			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/19 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
static slong  einv_n_OpModeAutoUpdateByTimerEvent(EinvTimerEvent_t timerEvent, EinvCmd65T_t* cmd65Data)
{

	uchar			ctlRunMode;
	EinvTime_t 	nowTime;
	uchar			cmd65RunMode;
	uchar   		calcCmd65RunModeResult = EINV_INVALIDVALUE;
	slong 		ret = EINV_CMD_SENDNOREQUEST;
	slong 		subret = EINV_NOUPDATE;
	EtimTm_t		ptime;
	EinvValidity_t	z;			/*関数入力引数範囲判断用*/
	EinvUpdate_t 	upDateFlag;
	
	DEBUG_TRCIN();
	
	/*システム時間取得*/
	//ret = ETIM_GetTime(&ptime);
	if(ret != ETIM_OK)
	{
		/*システム時間取得失敗*/
		/*ERR CODE ETIMモジュール参照*/
		DLOG_Error( "ETIM_GetTime() error %ld\n", ret );
		DEBUG_TRCOUT();
		return ret;
	}
	else
	{
		/*システム時間取得成功*/
		z = EINV_n_CheckSysTimeValid(&ptime);
	}

	if((timerEvent > EINV_SIXHOURCONTINUE) || 
	   (cmd65Data == NULL) ||
	   (z == EINV_INVALID)
	)
	{
		ret = EINV_ERR_PARAM;
		DLOG_Error( "EINV_n_opModeAutoUpdateByTimerEvent() error %ld\n", ret );
		DEBUG_TRCOUT();
		return ret;
	}
	else
	{
		/* 処理無し */
	}

	if((timerEvent >= EINV_SELLCHARGINGSTART) && (timerEvent <= EINV_GREENDISCHARGSTOP))
	{
		/*自動運転タイマイベント処理 alarm 運転モード切替時刻*/
		
		/*カレントCMD65運転モード設定値取得cmd65RunMode*/
		cmd65RunMode = cmd65Data->opeMode;
		/*タイマイベントより、CMD65運転モード算出処理*/
		subret = EINV_n_CalcCMD65RunmodeByTimerEvent(timerEvent, cmd65RunMode,&calcCmd65RunModeResult);
		if (subret == EINV_UPDATE)
		{
			/*カレントCMD65運転モード設定値設定calcCmd65RunModeResult*/
			cmd65Data->opeMode = calcCmd65RunModeResult;
			cmd65Data->changeFlag = cmd65Data->changeFlag | EINV_SET_CMD65_OPE_MODE;
			ret = ret | EINV_CMD65_SENDREQUEST;
		}
		else
		{
			/*カレントCMD65運転モード設定値設定不要*/
			ret = ret | EINV_CMD_SENDNOREQUEST;
		}
	}
	else
	{
		/*EINV_SIXHOURCONTINUE*/
		/*oneshot Timer*/
		/*待機モード持続6時間イベント処理*/
		/*管理運転モードをカレント自動運転モードにする*/
		ctlRunMode = EINV_n_GetAutoRunModeValue();
		/*管理運転モード設定*/
		EINV_n_SetCtlRunModeValue(ctlRunMode);
		EINV_n_GetUpDateFlag(&upDateFlag);
		if(upDateFlag.CTLRUNMODE == EINV_SET)
		{
			/*管理運転モード変更時、管理運転モード設定元設定*/
			/*管理運転モード設定元  初期化：システム*/
			EINV_n_SetRunModeSetFromValue(EINV_SYSTEM);
		}
		else
		{
			/* 処理無し */
		}
		/*CMD65コマンドメンバー再算出判定*/
		subret = EINV_n_CalcCmd65Request();
		if (subret == EINV_UPDATE)
		{
			/*再算出必要であれば*/
			/*運転モード再算出*/

			/*カレント時刻取得*/
			/*時刻変換*/
			nowTime.hour = ptime.tmHour;
			nowTime.min = ptime.tmMin;
			/*カレントCMD65運転モード取得cmd65RunMode*/
			cmd65RunMode = cmd65Data->opeMode;
			/*CMD65運転モード算出処理*/
			subret = EINV_n_CalcCMD65Runmode(&nowTime,cmd65RunMode,&calcCmd65RunModeResult);
			if(subret == EINV_UPDATE)
			{
				/*カレントCMD65運転モード設置calcCmd65RunModeResult*/
				cmd65Data->opeMode = calcCmd65RunModeResult;
				cmd65Data->changeFlag = cmd65Data->changeFlag | EINV_SET_CMD65_OPE_MODE;
				ret = ret | EINV_CMD65_SENDREQUEST;
			}
			else
			{
				/*カレントCMD65運転モード設置不要*/
				ret = ret | EINV_CMD_SENDNOREQUEST;
			}
		}
		else
		{
			ret = ret | EINV_CMD_SENDNOREQUEST;
		}
	}
	
	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:	cmd81 responseから運転モード設定処理				*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	v2hStatus											*/
/*					:	connecterstatus:LOCK,UNLOCK							*/
/*					:	button1:ON,OFF状態									*/
/*					:	button2:ON,OFF状態									*/
/*					:	button3:ON,OFF状態									*/
/*					:	V2hOperatePossible:V2H操作可能/不可能状態			*/
/*	param	in/out	:	cmd65Data	CMD65構造体アクセス						*/
/*	param	in/out	:	cmd75Data	CMD75構造体アクセス						*/
/*--------------------------------------------------------------------------*/
/*	return			:EINV_CMD65_SENDREQUEST	CMD65発行必要  					*/
/*					:EINV_CMD_SENDNOREQUEST	CMD発行必要無し		EINV_OK		*/
/*					:EINV_CMD75_SENDREQUEST	CMD75発行必要					*/
/*					:EINV_CMD75_SENDREQUEST | EINV_CMD65_SENDREQUEST 両方   */
/*					:EINV_ERR_PARAM:-4		関数パラメータ異常				*/
/*					:その他、ETIM_モジュールエラーコード参照				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	cmd81 responseから運転モード設定処理				*/
/*					:	V2H状態変化より運転モード設定変更					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/19 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
static slong einv_n_OpModeAutoUpdateByCMD81ResV2h(const EinvV2hStatus_t * v2hStatus, EinvCmd65T_t* cmd65Data, EinvCmd75T_t* cmd75Data)
{
	uchar   ctlRunMode;
	uchar   autoRunMode;
	uchar	cmd65RunMode;
	uchar   calcCmd65RunModeResult = EINV_INVALIDVALUE;
	uchar   cmd75ConnecterLockStatus;
	uchar   calcCmd75ConnecterLockStatus = EINV_INVALIDVALUE;
	EinvTime_t  nowTime;
	slong ret = EINV_CMD_SENDNOREQUEST;
	slong subret = EINV_UPDATE;
	EtimTm_t		ptime;
	EinvValidity_t	z;/*関数入力引数範囲判断用*/
	EinvUpdate_t upDateFlag;
	
	DEBUG_TRCIN();
	
	/*システム時間取得*/
	//ret = ETIM_GetTime(&ptime);
	if(ret != ETIM_OK)
	{
		/*システム時間取得失敗*/
		/*ERR CODE ETIMモジュール参照*/
		DLOG_Error( "ETIM_GetTime() error %ld\n", ret );
		DEBUG_TRCOUT();
		return ret;
	}
	else
	{
		/*システム時間取得成功*/
		z = EINV_n_CheckSysTimeValid(&ptime);
	}
	
	if((v2hStatus == NULL) ||
	   (cmd65Data == NULL) ||
	   (cmd75Data == NULL) ||
	   ((v2hStatus->v2hOperatePossible != EINV_IMPOSSIBLE) && (v2hStatus->v2hOperatePossible != EINV_POSSIBLE)) ||
	   ((v2hStatus->button1 != EINV_OFF) && (v2hStatus->button1 != EINV_ON)) ||
	   ((v2hStatus->button2 != EINV_OFF) && (v2hStatus->button2 != EINV_ON)) ||
	   ((v2hStatus->button3 != EINV_OFF) && (v2hStatus->button3 != EINV_ON)) ||
	   ((v2hStatus->connecterStatus != EINV_UNLOCK) && (v2hStatus->connecterStatus != EINV_LOCK)) ||
	   (z == EINV_INVALID)
	)
	{
		ret = EINV_ERR_PARAM;
		DLOG_Error( "EINV_n_opModeAutoUpdateByCMD81ResV2h() error %ld\n", ret );
		DEBUG_TRCOUT();
		return ret;
	}
	
	/*CMD81応答より、CMD65運転モード更新判定、CMD75コネクタ状態算出*/
	subret = EINV_n_CalcRunmodeByCmd81res(v2hStatus,&calcCmd75ConnecterLockStatus,&ctlRunMode,&autoRunMode);
	if(subret == EINV_UPDATE)
	{	
		/*CMD81応答より、通信ボード内管理運転モード変化する場合*/
		/*自動運転モード変更可能性があるから設定実施*/
		EINV_n_SetAutoRunModeValue(autoRunMode);
		/*管理運転モード設定*/
		EINV_n_SetCtlRunModeValue(ctlRunMode);
		EINV_n_GetUpDateFlag(&upDateFlag);
		if(upDateFlag.CTLRUNMODE == EINV_SET)
		{
			/*管理運転モード変更時、管理運転モード設定元設定*/
			/*管理運転モード設定元	初期化：EINV_V2H*/
			EINV_n_SetRunModeSetFromValue(EINV_V2H);
			/*運転モード設定元ENL以外の運転設定元に遷移する場合*/
			EINV_n_InitEnlRunMode();
		}
		else
		{
			/* 処理無し */
		}
		/*CMD65コマンドメンバー再算出判定*/
		subret = EINV_n_CalcCmd65Request();
		if (subret == EINV_UPDATE)
		{
			/*再算出必要であれば*/
			/*①運転モード算出*/
		
			/*カレント時刻取得*/
			/*時刻変換*/
			nowTime.hour = ptime.tmHour;
			nowTime.min = ptime.tmMin;
			
			/*カレントCMD65運転モード取得cmd65RunMode*/
			cmd65RunMode = cmd65Data->opeMode;
			/*CMD65運転モード算出処理*/
			subret = EINV_n_CalcCMD65Runmode(&nowTime,cmd65RunMode,&calcCmd65RunModeResult);
			if(subret == EINV_UPDATE)
			{
				/*カレントCMD65運転モード設置calcCmd65RunModeResult*/
				cmd65Data->opeMode = calcCmd65RunModeResult;
				cmd65Data->changeFlag = cmd65Data->changeFlag | EINV_SET_CMD65_OPE_MODE;
				ret = ret | EINV_CMD65_SENDREQUEST;
			}
			else
			{
				ret = ret | EINV_CMD_SENDNOREQUEST;
			}
		}
		else
		{
			/*CMD65コマンドメンバー再算出判定、通信ボード内管理運転モード変化しない場合*/
			ret = ret | EINV_CMD_SENDNOREQUEST;
		}
	}
	else
	{
		/*CMD81応答より、通信ボード内管理運転モード変化しない場合*/
		/*カレントCMD65運転モード設置不要*/
		ret = ret | EINV_CMD_SENDNOREQUEST;
	}
	
	/*カレントCMD75コネクタロック状態変数取得cmd75ConnecterLockStatus結果格納*/
	cmd75ConnecterLockStatus = cmd75Data->connectLockOrUnlock;
	ret = EINV_n_CalcCMD75ConnecterStatus(cmd75ConnecterLockStatus,&calcCmd75ConnecterLockStatus);
	if(ret == EINV_UPDATE)
	{
		/*設置calcCmd75ConnecterLockStatus*/
		cmd75Data->connectLockOrUnlock = calcCmd75ConnecterLockStatus;
		cmd75Data->changeFlag = cmd75Data->changeFlag | EINV_SET_CMD75_CONNECT_LOCK_OR_UNLOCK;
		ret = ret | EINV_CMD75_SENDREQUEST;
	}
	else
	{
		ret = ret | EINV_CMD_SENDNOREQUEST;
	}
	
	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:	cmd81 responseから運転モード設定処理				*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	manualRunStatus手動運転モード状態保存変数			*/
/*					:	Chargestatus		UNFINISH = '0',FINISH = '1'		*/
/*					:	DisChargestatus		UNFINISH = '0',FINISH = '1'		*/
/*					:	Battstatus			STOP = '0',RUNNING = '1'		*/
/*					:	v2hRunStatus		'D':CHARGEDISCHARGEFINISHREADY	*/
/*					:	NonChargeable		CLR = '0',SET = '1'				*/
/*					:	NonDisChargeable	CLR = '0',SET = '1'				*/
/*	param	in/out	:	cmd65Data	CMD65構造体アクセス						*/
/*--------------------------------------------------------------------------*/
/*	return			:EINV_CMD65_SENDREQUEST	CMD65発行必要  					*/
/*					:EINV_CMD_SENDNOREQUEST	CMD発行必要無し					*/
/*					:EINV_CMD75_SENDREQUEST	CMD75発行必要無し				*/
/*					:EINV_CMD75_SENDREQUEST | EINV_CMD65_SENDREQUEST		*/
/*					:EINV_ERR_PARAM:-4		関数パラメータ異常				*/
/*					:その他、ETIM_モジュールエラーコード参照				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	cmd81 responseから運転モード設定処理				*/
/*					:	手動運転モードから待機モードへ遷移する判定			*/
/*					:	手動運転モードから手動運転モードへ遷移する判定		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/19 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
static slong einv_n_OpModeAutoUpdateByCMD81ResManual(const Einv_ManualRunStatus_t * manualRunStatus, EinvCmd65T_t* cmd65Data)
{
	uchar ctlRunMode;
	EinvTime_t nowTime;
	uchar	cmd65RunMode;
	uchar   calcCmd65RunModeResult = EINV_INVALIDVALUE;	
	slong ret = EINV_CMD_SENDNOREQUEST;
	slong subret = EINV_UPDATE;
	EtimTm_t		ptime;
	EinvValidity_t	z,x;/*関数入力引数範囲判断用*/
	EinvUpdate_t upDateFlag;
	
	DEBUG_TRCIN();
	
	/*システム時間取得*/
	//ret = ETIM_GetTime(&ptime);
	if(ret != ETIM_OK)
	{
		/*システム時間取得失敗*/
		/*ERR CODE ETIMモジュール参照*/
		DLOG_Error( "ETIM_GetTime() error %ld\n", ret );
		DEBUG_TRCOUT();
		return ret;
	}
	else
	{
		/*システム時間取得成功*/
		z = EINV_n_CheckSysTimeValid(&ptime);
	}
	
	if(manualRunStatus != NULL)
	{
		x = EINV_n_CheckV2HOpStatus(manualRunStatus->v2hRunStatus);
	}
	else
	{
		/* 処理無し */
	}

	if((manualRunStatus == NULL) ||
	   (cmd65Data == NULL) ||
	   ((manualRunStatus->chargeStatus != EINV_FINISH) && (manualRunStatus->chargeStatus != EINV_UNFINISH)) ||
	   ((manualRunStatus->disChargeStatus != EINV_FINISH) && (manualRunStatus->disChargeStatus != EINV_UNFINISH)) ||
	   ((manualRunStatus->nonChargeable != EINV_NONCHARGEDISCHARGE_FLAG_SET) && (manualRunStatus->nonChargeable != EINV_NONCHARGEDISCHARGE_FLAG_CLR)) ||
	   ((manualRunStatus->nonDisChargeable != EINV_NONCHARGEDISCHARGE_FLAG_SET) && (manualRunStatus->nonDisChargeable != EINV_NONCHARGEDISCHARGE_FLAG_CLR)) ||
	   ((manualRunStatus->battStatus != EINV_BATTSTOP) && (manualRunStatus->battStatus != EINV_BATTRUNNING)) ||
	   (x == EINV_INVALID) ||
	   (z == EINV_INVALID)
	)
	{
		ret = EINV_ERR_PARAM;
		DLOG_Error( "EINV_n_opModeAutoUpdateByCMD81ResManual() error %ld\n", ret );
		DEBUG_TRCOUT();
		return ret;
	}
	
	/*CMD81応答より、CMD65運転モード更新判定*/
	subret = EINV_n_CalcRunmodeByCmd81resmanual(manualRunStatus, &ctlRunMode);
	if(subret == EINV_UPDATE)
	{	
		/*運転モード設定*/
		EINV_n_SetCtlRunModeValue(ctlRunMode);
		
		EINV_n_GetUpDateFlag(&upDateFlag);
		if(upDateFlag.CTLRUNMODE == EINV_SET)
		{
			/*管理運転モード変更時、管理運転モード設定元設定*/
			/*管理運転モード設定元　初期化：システム*/
			EINV_n_SetRunModeSetFromValue(EINV_SYSTEM);
			/*運転モード設定元ENL以外の運転設定元に遷移する場合*/
			EINV_n_InitEnlRunMode();
		}
		else
		{
			/* 処理無し */
		}
		
		/*CMD65コマンドメンバー再算出判定*/
		subret = EINV_n_CalcCmd65Request();
		if (subret == EINV_UPDATE)
		{
			/*再算出必要であれば*/
			/*運転モード算出*/
		
			/*カレント時刻取得*/
			/*時刻変換*/
			nowTime.hour = ptime.tmHour;
			nowTime.min = ptime.tmMin;
			/*カレントCMD65運転モード取得cmd65RunMode*/
			cmd65RunMode = cmd65Data->opeMode;
			/*CMD65運転モード算出処理*/
			subret = EINV_n_CalcCMD65Runmode(&nowTime,cmd65RunMode,&calcCmd65RunModeResult);
			if(subret == EINV_UPDATE)
			{
				/*カレントCMD65運転モード設置calcCmd65RunModeResult*/
				cmd65Data->opeMode = calcCmd65RunModeResult;
				/*CMD65コマンド変更フラッグ設定*/
				cmd65Data->changeFlag = cmd65Data->changeFlag | EINV_SET_CMD65_OPE_MODE;
				/*CMD65コマンド発行要求設置*/
				ret = ret | EINV_CMD65_SENDREQUEST;
			}
			else
			{
				/*カレントCMD65運転モード設置不要*/
				ret = ret | EINV_CMD_SENDNOREQUEST;
			}
		}
		else
		{
			ret = EINV_CMD_SENDNOREQUEST;
		}
	}
	else
	{
		ret = EINV_CMD_SENDNOREQUEST;
	}
	
	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:	SELLモードタイマ起動停止処理						*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	opStartStop:start/stop指示							*/
/*					:	ptime:カレント時刻									*/
/*	return			:	EINV_OK:正常終了									*/
/*					:	FRM_TIMER_E_	参照								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	SELLモードタイマ起動停止処理						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/19 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
static slong einv_n_SellModeTimerCtl(uchar opStartStop,const EtimTm_t * ptime)
{
	slong ret = EINV_OK;
	FrmTime_t chargstart;
	FrmTime_t chargstop;
	FrmTime_t dischargstart;
	FrmTime_t dischargstop;
	EinvTime_t sellChargStartTime;
	EinvTime_t sellChargStopTime;
	EinvTime_t sellDisChargStartTime;
	EinvTime_t sellDisChargStopTime;

	DEBUG_TRCIN();

	EINV_n_GetSellChargStartTimeValue(&sellChargStartTime);
	EINV_n_GetSellChargStopTimeValue(&sellChargStopTime);
	EINV_n_GetSellDisChargStartTimeValue(&sellDisChargStartTime);
	EINV_n_GetSellDisChargStopTimeValue(&sellDisChargStopTime);
	if(opStartStop == EINV_OPSTART)
	{
			/*SELLCHARGSTART タイマ設置、起動*/
		einv_n_CalcFrmWorkTimerPara(&sellChargStartTime, ptime, &chargstart);
			ret = FRM_TimerSetAlarm(EINV_TIM_ID_SELLCHARGSTART, &chargstart, NULL);
			/*SELLCHARGSTOP タイマ設置、起動*/
		einv_n_CalcFrmWorkTimerPara(&sellChargStopTime, ptime, &chargstop);
			ret = FRM_TimerSetAlarm(EINV_TIM_ID_SELLCHARGSTOP, &chargstop, NULL);
			/*SELLDISCHARGSTART タイマ設置、起動*/
		einv_n_CalcFrmWorkTimerPara(&sellDisChargStartTime, ptime, &dischargstart);
			ret = FRM_TimerSetAlarm(EINV_TIM_ID_SELLDISCHARGSTART, &dischargstart, NULL);
			/*SELLDISCHARGSTOP タイマ設置、起動*/
		einv_n_CalcFrmWorkTimerPara(&sellDisChargStopTime, ptime, &dischargstop);
			ret = FRM_TimerSetAlarm(EINV_TIM_ID_SELLDISCHARGSTOP, &dischargstop, NULL);
	}	
	else
	{
		/*SELLモードタイマ停止*/
		ret = FRM_TimerCancelAlarm(EINV_TIM_ID_SELLCHARGSTART);
		ret = FRM_TimerCancelAlarm(EINV_TIM_ID_SELLCHARGSTOP);
		ret = FRM_TimerCancelAlarm(EINV_TIM_ID_SELLDISCHARGSTART);
		ret = FRM_TimerCancelAlarm(EINV_TIM_ID_SELLDISCHARGSTOP);
	}
	
	DEBUG_TRCOUT();
	
	return ret;
	
}

/*==========================================================================*/
/*	Description		:	GREENモードタイマ起動停止処理						*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	opStartStop:start/stop指示							*/
/*					:	ptime:カレント時刻									*/
/*	return			:	EINV_OK:正常終了									*/
/*					:	FRM_TIMER_E_	参照								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	GREENモードタイマ起動停止処理						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/18 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
static slong einv_n_GreenModeTimerCtl(uchar opStartStop,const EtimTm_t * ptime)
{
	slong ret = EINV_OK;
	FrmTime_t chargstart;
	FrmTime_t chargstop;
	FrmTime_t dischargstart;
	FrmTime_t dischargstop;
	EinvTime_t greenChargStartTime;
	EinvTime_t greenChargStopTime;
	EinvTime_t greenDisChargStartTime;
	EinvTime_t greenDisChargStopTime;

	DEBUG_TRCIN();

	EINV_n_GetGreenChargStartTimeValue(&greenChargStartTime);
	EINV_n_GetGreenChargStopTimeValue(&greenChargStopTime);
	EINV_n_GetGreenDisChargStartTimeValue(&greenDisChargStartTime);
	EINV_n_GetGreenDisChargStopTimeValue(&greenDisChargStopTime);

	if(opStartStop == EINV_OPSTART)
	{
			/*GREENCHARGSTART タイマ設置、起動*/
			einv_n_CalcFrmWorkTimerPara(&greenChargStartTime,ptime,&chargstart);
			ret = FRM_TimerSetAlarm(EINV_TIM_ID_GREENCHARGSTART, &chargstart, NULL);
			/*GREENCHARGSTOP タイマ設置、起動*/
			einv_n_CalcFrmWorkTimerPara(&greenChargStopTime,ptime,&chargstop);
			ret = FRM_TimerSetAlarm(EINV_TIM_ID_GREENCHARGSTOP, &chargstop, NULL);
			/*GREENDISCHARGSTART タイマ設置、起動*/
			einv_n_CalcFrmWorkTimerPara(&greenDisChargStartTime,ptime,&dischargstart);
			ret = FRM_TimerSetAlarm(EINV_TIM_ID_GREENDISCHARGSTART, &dischargstart, NULL);
			/*GREENDISCHARGSTOP タイマ設置、起動*/
			einv_n_CalcFrmWorkTimerPara(&greenDisChargStopTime,ptime,&dischargstop);
			ret = FRM_TimerSetAlarm(EINV_TIM_ID_GREENDISCHARGSTOP, &dischargstop, NULL);
	}	
	else
	{
			/*Greenモードタイマ停止*/
			ret = FRM_TimerCancelAlarm(EINV_TIM_ID_GREENCHARGSTART);
			ret = FRM_TimerCancelAlarm(EINV_TIM_ID_GREENCHARGSTOP);
			ret = FRM_TimerCancelAlarm(EINV_TIM_ID_GREENDISCHARGSTART);
			ret = FRM_TimerCancelAlarm(EINV_TIM_ID_GREENDISCHARGSTOP);
	}
	
	DEBUG_TRCOUT();
	
	return ret;
}

/*==========================================================================*/
/*	Description		:	GREEN,GREENモード個別タイマ時刻更新処理				*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	timerEvent:自動運転時刻切替イベントor外部通信機設置 */
/*					:	ptime:カレント時刻									*/
/*	return			:	EINV_OK:正常終了									*/
/*					:	FRM_TIMER_E_	参照								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	SELL,GREENモード個別タイマ時刻更新処理				*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/18 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
static slong einv_n_SellGreenModeSingleTimerCtl(EinvTimerEvent_t timerEvent,const EtimTm_t * ptime)
{
	slong ret = EINV_OK;
	FrmTime_t chargstart;
	FrmTime_t chargstop;
	FrmTime_t dischargstart;
	FrmTime_t dischargstop;
	EinvTime_t sellChargStartTime;
	EinvTime_t sellChargStopTime;
	EinvTime_t sellDisChargStartTime;
	EinvTime_t sellDisChargStopTime;
	EinvTime_t greenChargStartTime;
	EinvTime_t greenChargStopTime;
	EinvTime_t greenDisChargStartTime;
	EinvTime_t greenDisChargStopTime;

	DEBUG_TRCIN();

	EINV_n_GetSellChargStartTimeValue(&sellChargStartTime);
	EINV_n_GetSellChargStopTimeValue(&sellChargStopTime);
	EINV_n_GetSellDisChargStartTimeValue(&sellDisChargStartTime);
	EINV_n_GetSellDisChargStopTimeValue(&sellDisChargStopTime);
	EINV_n_GetGreenChargStartTimeValue(&greenChargStartTime);
	EINV_n_GetGreenChargStopTimeValue(&greenChargStopTime);
	EINV_n_GetGreenDisChargStartTimeValue(&greenDisChargStartTime);
	EINV_n_GetGreenDisChargStopTimeValue(&greenDisChargStopTime);
	if(timerEvent == EINV_EXTERNALSET)
	{
		/*外部通信機からタイマ個別設置*/
		if (g_einvUpDateFlag.GREENCHARGSTARTTIME == EINV_SET)
		{
			/*GREENCHARGSTART タイマ設置、起動*/
			einv_n_CalcFrmWorkTimerPara(&greenChargStartTime,ptime,&chargstart);
			ret = FRM_TimerSetAlarm(EINV_TIM_ID_GREENCHARGSTART, &chargstart, NULL);
		
		}
		if (g_einvUpDateFlag.GREENCHARGSTOPTIME == EINV_SET)
		{
			/*GREENCHARGSTOP タイマ設置、起動*/
			einv_n_CalcFrmWorkTimerPara(&greenChargStopTime,ptime,&chargstop);
			ret = FRM_TimerSetAlarm(EINV_TIM_ID_GREENCHARGSTOP, &chargstop, NULL);
		}
		if (g_einvUpDateFlag.GREENDISCHARGSTARTTIME == EINV_SET)
		{
			/*GREENDISCHARGSTART タイマ設置、起動*/
			einv_n_CalcFrmWorkTimerPara(&greenDisChargStartTime,ptime,&dischargstart);
			ret = FRM_TimerSetAlarm(EINV_TIM_ID_GREENDISCHARGSTART, &dischargstart, NULL);
		}
		if (g_einvUpDateFlag.GREENDISCHARGSTOPTIME == EINV_SET)
		{
			/*GREENDISCHARGSTOP タイマ設置、起動*/
			einv_n_CalcFrmWorkTimerPara(&greenDisChargStopTime,ptime,&dischargstop);
			ret = FRM_TimerSetAlarm(EINV_TIM_ID_GREENDISCHARGSTOP, &dischargstop, NULL);
		}
		if (g_einvUpDateFlag.SELLCHARGSTARTTIME == EINV_SET)
		{
			/*SELLCHARGSTART タイマ設置、起動*/
			einv_n_CalcFrmWorkTimerPara(&sellChargStartTime,ptime,&chargstart);
			ret = FRM_TimerSetAlarm(EINV_TIM_ID_SELLCHARGSTART, &chargstart, NULL);
		}
		if (g_einvUpDateFlag.SELLCHARGSTOPTIME == EINV_SET)
		{
			/*SELLCHARGSTOP タイマ設置、起動*/
			einv_n_CalcFrmWorkTimerPara(&sellChargStopTime,ptime,&chargstop);
			ret = FRM_TimerSetAlarm(EINV_TIM_ID_SELLCHARGSTOP, &chargstop, NULL);
		}
		if (g_einvUpDateFlag.SELLDISCHARGSTARTTIME == EINV_SET)
		{
			/*SELLDISCHARGSTART タイマ設置、起動*/
			einv_n_CalcFrmWorkTimerPara(&sellDisChargStartTime,ptime,&dischargstart);
			ret = FRM_TimerSetAlarm(EINV_TIM_ID_SELLDISCHARGSTART, &dischargstart, NULL);
		}
		if (g_einvUpDateFlag.SELLDISCHARGSTOPTIME == EINV_SET)
		{
			/*SELLDISCHARGSTOP タイマ設置、起動*/
			einv_n_CalcFrmWorkTimerPara(&sellDisChargStopTime,ptime,&dischargstop);
			ret = FRM_TimerSetAlarm(EINV_TIM_ID_SELLDISCHARGSTOP, &dischargstop, NULL);
		}
	}
	else if(timerEvent == EINV_SELLCHARGINGSTART)
	{
		/*SELLCHARGSTART タイマ設置、起動*/
		einv_n_UpdateFrmWorkTimerPara(&sellChargStartTime,ptime,&chargstart);
		ret = FRM_TimerSetAlarm(EINV_TIM_ID_SELLCHARGSTART, &chargstart, NULL);
	}
	else if(timerEvent == EINV_SELLCHARGINGSTOP)
	{
		/*SELLCHARGSTOP タイマ設置、起動*/
		einv_n_UpdateFrmWorkTimerPara(&sellChargStopTime,ptime,&chargstop);
		ret = FRM_TimerSetAlarm(EINV_TIM_ID_SELLCHARGSTOP, &chargstop, NULL);
	}
	else if(timerEvent == EINV_SELLDISCHARGSTART)
	{
		/*SELLDISCHARGSTART タイマ設置、起動*/
		einv_n_UpdateFrmWorkTimerPara(&sellDisChargStartTime,ptime,&dischargstart);
		ret = FRM_TimerSetAlarm(EINV_TIM_ID_SELLDISCHARGSTART, &dischargstart, NULL);
	}
	else if(timerEvent == EINV_SELLDISCHARGSTOP)
	{
		/*SELLDISCHARGSTOP タイマ設置、起動*/
		einv_n_UpdateFrmWorkTimerPara(&sellDisChargStopTime,ptime,&dischargstop);
		ret = FRM_TimerSetAlarm(EINV_TIM_ID_SELLDISCHARGSTOP, &dischargstop, NULL);
	}
	else if(timerEvent == EINV_GREENCHARGINGSTART)
	{
		/*GREENCHARGSTART タイマ設置、起動*/
		einv_n_UpdateFrmWorkTimerPara(&greenChargStartTime,ptime,&chargstart);
		ret = FRM_TimerSetAlarm(EINV_TIM_ID_GREENCHARGSTART, &chargstart, NULL);
		
	}
	else if(timerEvent == EINV_GREENCHARGINGSTOP)
	{
		/*GREENCHARGSTOP タイマ設置、起動*/
		einv_n_UpdateFrmWorkTimerPara(&greenChargStopTime,ptime,&chargstop);
		ret = FRM_TimerSetAlarm(EINV_TIM_ID_GREENCHARGSTOP, &chargstop, NULL);
	}
	else if(timerEvent == EINV_GREENDISCHARGSTART)
	{
		/*GREENDISCHARGSTART タイマ設置、起動*/
		einv_n_UpdateFrmWorkTimerPara(&greenDisChargStartTime,ptime,&dischargstart);
		ret = FRM_TimerSetAlarm(EINV_TIM_ID_GREENDISCHARGSTART, &dischargstart, NULL);	
	}
	else if(timerEvent == EINV_GREENDISCHARGSTOP)
	{
		/*GREENDISCHARGSTOP タイマ設置、起動*/
		einv_n_UpdateFrmWorkTimerPara(&greenDisChargStopTime,ptime,&dischargstop);
		ret = FRM_TimerSetAlarm(EINV_TIM_ID_GREENDISCHARGSTOP, &dischargstop, NULL);
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();
	
	return ret;
}

/*==========================================================================*/
/*	Description		:	alarmtimerパラメータ算出							*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	time:自動運転運転モード切替時刻値 					*/
/*					:	ptime:カレント時刻									*/
/*					:	frmtime:alarmtimerパラメータ						*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/18 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
static void einv_n_CalcFrmWorkTimerPara(const EinvTime_t * time, const EtimTm_t * ptime, FrmTime_t * frmtime)
{
	uint c, s;
	
	DEBUG_TRCIN();
	
	c = ptime->tmHour * EINV_MIN_ONE_HOUR + ptime->tmMin;/*カレント時刻*/
	s = time->hour * EINV_MIN_ONE_HOUR + time->min;		/*自動運転切替時刻*/
	
	if((c > s) || (c == s))
	{
		/*設定時刻がカレント時刻の前*/
		/*設定時刻が既に経過した、24HOUR後通知*/
		einv_n_UpdateFrmWorkTimerPara(time,ptime,frmtime);
	}
	else
	{
		/*設定時刻がカレント時刻の後ろ*/
		/*設定時刻がまだ*/
		frmtime->year = ptime->tmYear;
		frmtime->mon =  ptime->tmMon;
		frmtime->day =  ptime->tmMday;
		frmtime->hour = time->hour;
		frmtime->min  = time->min;
		frmtime->sec  = 0u;
	}
	
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	alarmtimerパラメータ算出	24HOUR後の年月日算出	*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	time:自動運転運転モード切替時刻値 					*/
/*					:	ptime:カレント時刻									*/
/*					:	frmtime:alarmtimerパラメータ						*/
/*	return			:	無し												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	24HOUR後の年月日算出								*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/18 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
static void einv_n_UpdateFrmWorkTimerPara(const EinvTime_t * time,const EtimTm_t * ptime,FrmTime_t * frmtime)
{
	uchar leap = EINV_CLR;
	uchar day;
	
	DEBUG_TRCIN();
	
	frmtime->year = ptime->tmYear;
	frmtime->mon =  ptime->tmMon;
	frmtime->day =  ptime->tmMday;
	frmtime->hour = time->hour;
	frmtime->min  = time->min;
	frmtime->sec  = 0u;
	
	if(((frmtime->year % EINV_4MOD == 0u) && (frmtime->year % EINV_100MOD != 0u)) ||
	  ((frmtime->year % EINV_100MOD == 0u) && (frmtime->year % EINV_400MOD == 0u))
	)
	{
		/*閏年*/
		leap = EINV_SET;
	}
	else
	{
		leap = EINV_CLR;
	}
	
	day = s_einvMonthDayNumTbl[frmtime->mon-EINV_ONEMONTH][leap];
	
	/*月末チャック*/
	if(frmtime->day == day)
	{	/*月末*/
		/*24hour後の年月日*/
		/*年末チャック*/
		if(frmtime->mon == EINV_12MONTH)
		{
			/*12月->1月*/
			frmtime->mon = EINV_1MONTH;
			/*次の年になる*/
			frmtime->year = frmtime->year + EINV_ONEYEAR;
		}
		else
		{
			frmtime->mon = frmtime->mon + EINV_ONEMONTH;
		}
		frmtime->day = EINV_ONEDAY;
	}
	else
	{
		frmtime->day = frmtime->day + EINV_ONEDAY;
	}
	
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	取得運転モード設定依頼元,指定電力運転フラグ			*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Chongyang.Chen : 新規作成				*/
/*==========================================================================*/
slong EINV_n_GetOpeMode(EinvOpemodeGet_t *opeModeGet)
{
	slong ret = EINV_OK;

	DEBUG_TRCIN();

	opeModeGet->runModeSetFrom  = EINV_n_GetRunModeSetFromValue();	/* V:運転モード設定依頼元 */
	opeModeGet->splPowerRunFlag = g_splPowerRunFlag;				/* V:指定電力運転フラグ */

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:	取得CMD41表示用シグナル処理値						*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/05 : DHC Chongyang.Chen : 新規作成				*/
/*==========================================================================*/
slong EINV_n_GetCalcAvgValue(EinvCalcAvg_t *calcAvgValueGet)
{
	slong ret = EINV_OK;

	DEBUG_TRCIN();

	EINV_n_CalcDisplayAvgValue();

	calcAvgValueGet->pvPwr2Avg			 = g_pvPwr2Avg;			/* 太陽光発電電力(AC端計測値)平均値 */
	calcAvgValueGet->pvGenPwrAvg		 = g_pvGenPwrAvg;		/* PV発電電力(本製品以外のPV)平均値 */
	calcAvgValueGet->revPwrFlowAvg		 = g_revPwrFlowAvg;		/* 逆潮流電力平均値 */
	calcAvgValueGet->v2hInstantPowerAvg	 = g_v2hInstantPowerAvg;/* V2H瞬時充放電電力計測値(AC端計測値)平均値 */
	calcAvgValueGet->btChargeDischPwrAvg = g_btChargeDischPwrAvg;/* 蓄電池充放電電力(AC端計測値)平均値 */
	calcAvgValueGet->homeLoadPwr	 	 = g_homeLoadPwr; 		/* 家庭負荷電力 */
	calcAvgValueGet->pvPwrAvgSum		 = g_pvPwrAvgSum;		/* 太陽光発電電力平均値sum */

	DLOG_Debug("calcAvgValueGet->pvPwr2Avg           = %u",calcAvgValueGet->pvPwr2Avg);
	DLOG_Debug("calcAvgValueGet->pvGenPwrAvg         = %d",calcAvgValueGet->pvGenPwrAvg);
	DLOG_Debug("calcAvgValueGet->revPwrFlowAvg       = %d",calcAvgValueGet->revPwrFlowAvg);
	DLOG_Debug("calcAvgValueGet->v2hInstantPowerAvg  = %ld",calcAvgValueGet->v2hInstantPowerAvg);
	DLOG_Debug("calcAvgValueGet->btChargeDischPwrAvg = %ld",calcAvgValueGet->btChargeDischPwrAvg);
	DLOG_Debug("calcAvgValueGet->homeLoadPwr         = %ld",calcAvgValueGet->homeLoadPwr);
	DLOG_Debug("calcAvgValueGet->pvPwrAvgSum         = %ld",calcAvgValueGet->pvPwrAvgSum);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:	動作情報取得										*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Chongyang.Chen : 新規作成				*/
/*==========================================================================*/
slong EINV_n_GetOpeSttsInfo(EinvOpesttsInfo_t *opeSttsInfo)
{
	slong ret = EINV_OK;
	ushort 	usoc;

	DEBUG_TRCIN();

	ret = EINV_n_SocConvtRtoU(&usoc, g_einvCmd81_r.rsoc);
	if (ret == EINV_OK)
	{
		opeSttsInfo->uSOC = usoc;
	}
	else
	{
		/* 処理無し */
	}
	opeSttsInfo->chargeDischargePwrAC	= g_einvCmd81_r.chargeDischargePwrAC;	/* PCS充放電電力（AC端計測値）[W] */
	opeSttsInfo->opeState 				= g_einvCmd81_r.opeState;				/* PCS運転状態 */			
	opeSttsInfo->rsoc					= g_einvCmd81_r.rsoc;					/* rSOC[1%] */
	opeSttsInfo->remainingBt			= g_einvCmd81_r.remainingBt;			/* 電池残量[0.1kWh] */
	opeSttsInfo->soh					= g_einvCmd81_r.soh;					/* SOH[1%] */				
	opeSttsInfo->btOperation 			= g_einvCmd81_r.btOperation;			/* 蓄電池運転動作状態 */
	opeSttsInfo->pwrCut 				= g_einvCmd81_r.pwrCut;					/* 停電 */
	opeSttsInfo->opeMode 				= g_einvCmd81_r.opeMode;				/* 運転モード */
	/* opeSttsInfo->opeMode				= EINV_n_GetCtlRunModeValue(); */			/* 管理運転モード */
	opeSttsInfo->suppressState			= g_einvCmd81_r.suppressState;			/* 出力抑制状態 */
	opeSttsInfo->carConnectorLockState 	= g_einvCmd81_r.carConnectorLockState;	/* コネクタロック状態 */
	opeSttsInfo->carCharDiscOpratState 	= g_einvCmd81_r.carCharDiscOpratState;	/* 車両充放電器運転動作状態 */
	opeSttsInfo->emergencyMode 			= g_einvCmd81_r.emergencyMode;			/* 非常時運転モード */
	opeSttsInfo->carRemainingCapacity1 	= g_einvCmd81_r.carRemainingCapacity1;	/* 車載電池の電池残容量1[wh] */
	opeSttsInfo->carRemainingCapacity3 	= g_einvCmd81_r.carRemainingCapacity3;	/* 車載電池の電池残容量3[%] */

	DEBUG_TRCOUT();

	return ret;
}
/*End Of File*/
