/******************************************************************************/
/*    Description    :ログ管理ミドル I/F関数                                    */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC Jiang.Z                                             */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/03/31                                              */
/*----------------------------------------------------------------------------*/
/*    Module Name    :                                                        */
/*----------------------------------------------------------------------------*/
/*    Note           :                                                        */
/*----------------------------------------------------------------------------*/
/*    Copyright      :(c) 2020 NICHICON CORPORATION                           */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Include Header File                                                        */
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <task_g.h>
#include "typedef.h"
#include "elog_g.h"
#include "elog_fnc_g.h"
#include "elog_utl_g.h"
#include "elog_val_g.h"

/*----------------------------------------------------------------------------*/
/* 公開変数定義                                                                */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開変数定義                                                              */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 公開関数定義                                                                */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開関数定義                                                              */
/*----------------------------------------------------------------------------*/



/*============================================================================*/
/*    Description    :ログ出力                                                 */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*                   :0:Sucess -1:Failed                                      */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/10  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_Log(ElogLv_t level,char *fmt, ...)
{
	ElogLv_t slevel;
	bool_t enable;
	ElogFncLog_t par;
	va_list arg;
	DEBUG_TRCIN();

	/* ログレベル・OnOff状態共有メモリ取得 */
	ELOG_Shmget();

	/* OnOff状態取得 */
	enable = ELOG_ValGetEnable(FRM_MsgSelfModId());
	if(enable)
	{
		/* ログレベル設定取得 */
		slevel = ELOG_ValGetLevel(FRM_MsgSelfModId());
		if(level <= slevel)
		{
			par.header.msgId = ELOG_LOG;
			par.level = level;
			va_start(arg, fmt);
			vsnprintf((char*)par.string, sizeof(par.string),(char *)fmt, arg);
			va_end(arg);

			FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
		}
		else
		{
			/*Do nothing*/
		}
	}
	else
	{
		/*Do nothing*/
	}
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :操作log                                                 */
/*----------------------------------------------------------------------------*/
/*    param          :slong ButtonID:操作されたボタンID                        */
/*----------------------------------------------------------------------------*/
/*    param          :slong PictureID:操作された画面のID                       */
/*----------------------------------------------------------------------------*/
/*    param          :schar ObjectType:操作对象                                */
/*----------------------------------------------------------------------------*/
/*    param          :schar *opt:操作内容                                      */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/29  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_AsyncOperationLog(slong ButtonID, slong PictureID, schar ObjectType, schar *opt)
{
	ElogFncOpt_t par;

	DEBUG_TRCIN();
	par.header.msgId = ELOG_ASYNC_OPERATION;

	par.ButtonID = ButtonID;
	par.PictureID = PictureID;
	par.ObjectType = ObjectType;
	strncpy((schar *)(par.opt),(schar *)opt, sizeof(par.opt) - 1);
	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);

	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :PCS的错误ログ設定                                        */
/*----------------------------------------------------------------------------*/
/*                   :schar *currErrCode:现在的错误码                          */
/*----------------------------------------------------------------------------*/
/*                   :schar *errLevel:错误码等级                               */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/20  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_ErrEventLog(ElogDiffErrType_t type, schar *currErrCode, schar errLevel)
{
	ElogFncErrEvent_t par;

	DEBUG_TRCIN();
	/* 引数確認 */

	if(currErrCode == NULL)
	{
		DEBUG_TRCOUT();
		return;
	}
	else
	{
		if(errLevel == NULL)
		{
			DEBUG_TRCOUT();
			return;
		}
		else
		{
			/*Do nothing*/
		}
	}

	par.header.msgId = ELOG_ASYNC_ERR_EVENT_LOG;

	strncpy((schar *)(par.currErrCode), (schar *)currErrCode,
			sizeof(par.currErrCode) - 1);
	par.errLevel = errLevel;
	par.type = type;

	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :动作log设定                                              */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/06  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_ActionLog(schar *actionLog)
{
	ElogFncAction_t par;
	DEBUG_TRCIN();

	/* 引数確認 */
	if(actionLog == NULL)
	{
		DEBUG_TRCOUT();
		return;
	}
	else
	{
		/*Do nothing*/
	}

	par.header.msgId = ELOG_ASYNC_ACTION_LOG;

	strncpy((schar *)(par.actionLog), (schar *)actionLog,
			sizeof(par.actionLog) - 1);
	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}
/*============================================================================*/
/*    Description    :SDPログ圧縮要求                                          */
/*----------------------------------------------------------------------------*/
/*    param          :bool_t del:是否删除文件                                  */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/28  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_AsyncCompressActionLog(bool_t del)
{
	ElogFncActionComp_t par;

	DEBUG_TRCIN();
	par.header.msgId = ELOG_ASYNC_PRESS_ACTION;
	par.del = del;

	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}
/*============================================================================*/
/*    Description    :soh推移log设定                                           */
/*----------------------------------------------------------------------------*/
/*    param          :ushort percentage:推移百分比                             */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/30  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_SOHInfoLog(ushort percentage)
{
	ElogFncSOH_t par;
	/* 引数確認 */
	DEBUG_TRCIN();

	par.header.msgId = ELOG_ASYNC_SOH_INFO_LOG;
	par.percentage = percentage;
	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}
/*============================================================================*/
/*    Description    :电压上升抑制log                                          */
/*----------------------------------------------------------------------------*/
/*    param          :schar state:抑制状态                                     */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/30  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_PowerVoltHistLog(schar state)
{
	ElogFncVoltHist_t par;
	/* 引数確認 */
	DEBUG_TRCIN();

	par.header.msgId = ELOG_ASYNC_PWR_VOLT_HIST_LOG;
	par.state = state;
	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}
/*============================================================================*/
/*    Description    :HEMS发电抑制log                                          */
/*----------------------------------------------------------------------------*/
/*    param          :ushort percentage:发电抑制值                             */
/*----------------------------------------------------------------------------*/
/*                   :schar state:发电抑制状态                                 */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/30  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_HemsHistoryLog(ushort percentage, schar state)
{
	ElogFncHemsHistory_t par;
	/* 引数確認 */
	DEBUG_TRCIN();

	par.header.msgId = ELOG_ASYNC_HEMS_HISTORY_LOG;
	par.percentage = percentage;
	par.state = state;
	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}
/*============================================================================*/
/*    Description    :分布式AC累计输入输出log设定                               */
/*----------------------------------------------------------------------------*/
/*    param          :ulong PowerTotal:总电量累计                              */
/*----------------------------------------------------------------------------*/
/*    param          :schar type:AC种类                                       */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/30  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_PowerTotalLog(ulong PowerTotal, ElogFileNumType_t type)
{
	ElogFncPowerTotal_t par;
	/* 引数確認 */
	DEBUG_TRCIN();

	par.header.msgId = ELOG_ASYNC_TOTAL_PWR;
	par.PowerTotal = PowerTotal;
	par.type = type;
	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :出力制御ログ                                             */
/*----------------------------------------------------------------------------*/
/*    param          :sshort percentage:出力制御率                             */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/20  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_OutPutCtrlLog(sshort percentage)
{
	ElogFncOutPutCtrl_t par;
	/* 引数確認 */
	DEBUG_TRCIN();

	par.header.msgId = ELOG_ASYNC_OUTPUTCTRL;
	par.percentage = percentage;
	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}
/*============================================================================*/
/*    Description    :发电/买卖电log                                           */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/20  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_PowerDealLog(void)
{
	ElogPowerDeal_t par;
	/* 引数確認 */
	DEBUG_TRCIN();

	par.header.msgId = ELOG_ASYNC_PWR_DEAL_PWR;
	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}
/*============================================================================*/
/*    Description    :蓄电池充放电log                                          */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/20  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_AccumulatorLog(void)
{
	ElogAccumulator_t par;
	/* 引数確認 */
	DEBUG_TRCIN();

	par.header.msgId = ELOG_ASYNC_ACCUMULATOR_PWR;
	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :车辆充放电log                                            */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/20  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_VehicleLog(void)
{
	ElogVehicle_t par;
	/* 引数確認 */
	DEBUG_TRCIN();

	par.header.msgId = ELOG_ASYNC_VEHICLE_PWR;
	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :cmd65运转信息                                            */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/20  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_RunInforCMD65Log(ElogFncRunInfor65_t *runInfor)
{
	ElogFncRunInfor par;
	DEBUG_TRCIN();

	/* 引数確認 */
	if(runInfor == NULL)
	{
		DEBUG_TRCOUT();
		return;
	}
	else
	{
		/*Do nothing*/
	}

	par.header.msgId = ELOG_ASYNC_RUN_INFOR_CMD65_LOG;
	par.runCmd65 = *runInfor;
	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}
/*============================================================================*/
/*    Description    :cmd75运转信息                                            */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/20  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_RunInforCMD75Log(ElogFncRunInfor75_t *runInfor)
{
	ElogFncRunInfor par;
	DEBUG_TRCIN();

	/* 引数確認 */
	if(runInfor == NULL)
	{
		DEBUG_TRCOUT();
		return;
	}
	else
	{
		/*Do nothing*/
	}

	par.header.msgId = ELOG_ASYNC_RUN_INFOR_CMD75_LOG;
	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}
/*============================================================================*/
/*    Description    :SDPログ設定                                              */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/28  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_AsyncSaveSdpLog(schar *string)
{
	ElogFncSdp_t par;

	DEBUG_TRCIN();
	/* 引数確認 */
	if(string == NULL)
	{
		DEBUG_TRCOUT();
		return;
	}
	else
	{
		/*Do nothing*/
	}

	par.header.msgId = ELOG_ASYNC_SAVESDPLOG;
	strncpy((schar *)(par.string), (schar *)string, sizeof(par.string) - 1);

	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :SDPログ圧縮要求                                          */
/*----------------------------------------------------------------------------*/
/*    param          :bool_t del:原文件是否删除                                */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/28  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_AsyncCompressSdpLog(bool_t del)
{
	ElogFncSdpComp_t par;

	DEBUG_TRCIN();

	par.header.msgId = ELOG_ASYNC_PRESSSDP;

	par.del = del;

	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :ログ一覧取得                                             */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_AsyncGetLogList(ElogType_t type, uchar *begin, uchar *end)
{

	ElogFncGetLogList_t par;
	DEBUG_TRCIN();

	/* 引数確認 */
	if(begin == NULL)
	{
		DEBUG_TRCOUT();
		return ELOG_ERR_PARAM;
	}
	else
	{
		if(end == NULL)
		{
			DEBUG_TRCOUT();
			return ELOG_ERR_PARAM;
		}
		else
		{
			/*Do nothing*/
		}
	}

	par.header.msgId = ELOG_ASYNC_GETLOGLIST;
	par.type = type;
	strncpy((schar *)par.begin, (schar *)begin, sizeof(par.begin) - 1);
	strncpy((schar *)par.end, (schar *)end, sizeof(par.end) - 1);

	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);

	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :ログ領域空き容量取得                                      */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_SyncGetFreeLogSpace(ulong *pFreeSpace)
{

	ElogFncFreeSpace_t par;
	DEBUG_TRCIN();
	/* 引数確認 */
	if(pFreeSpace == NULL)
	{
		DEBUG_TRCOUT();
		return ELOG_ERR_PARAM;
	}
	else
	{
		/*Do nothing*/
	}
	par.header.msgId = ELOG_SYNC_GETLOGFREE;

	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);

	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :ログ削除要求                                             */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_AsyncDeleteLog(ElogType_t type, uchar *begin, uchar *end)
{

	ElogFncDeleteLog_t par;
	DEBUG_TRCIN();

	/* 引数確認 */
	if(begin == NULL)
	{
		DEBUG_TRCOUT();
		return ELOG_ERR_PARAM;
	}
	else
	{
		/*Do nothing*/
	}
	if(end == NULL)
	{
		DEBUG_TRCOUT();
		return ELOG_ERR_PARAM;
	}
	else
	{
		/*Do nothing*/
	}

	par.header.msgId = ELOG_ASYNC_DELETELOG;
	par.type = type;
	strncpy((schar *)par.begin, (schar *)begin, sizeof(par.begin) - 1);
	strncpy((schar *)par.end, (schar *)end, sizeof(par.end) - 1);

	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :指定ログファイル削除要求                                  */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_AsyncDeleteFile(ElogDeleteFileType_t type, uchar line)
{
	ElogFncDeleteFile_t par;
	DEBUG_TRCIN();

	par.header.msgId = ELOG_ASYNC_DELETEFILE;
	par.type = type;
	par.line = line;
	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :全プロセスログOn/Off設定                                  */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_AsyncSetEnableAll(bool_t onOff)
{

	ElogFncOnOff_t par;
	DEBUG_TRCIN();

	par.header.msgId = ELOG_ASYNC_SETENABLEALL;

	par.onOff = onOff;
	par.msgID = 0;		/* dummy */

	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :ログOn/Off取得                                           */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/11  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_AsyncGetEnableAll(bool_t *onOff)
{
	ElogFncOnOff_t par;
	DEBUG_TRCIN();

	/* 引数確認 */
	if(onOff == NULL)
	{
		DEBUG_TRCOUT();
		return;
	}
	else
	{
		/*Do nothing*/
	}

	*onOff = FALSE;

	par.header.msgId = ELOG_ASYNC_GETENABLEALL;

	par.onOff = FALSE;	/* dummy */
	par.msgID = 0;		/* dummy */

	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :ダウンロードログ出力                                      */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/30  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_AsyncDownloadLog(schar* string)
{
	ElogFncDownload_t par;
	DEBUG_TRCIN();

	/* 引数確認 */
	if(string == NULL)
	{
		DEBUG_TRCOUT();
		return;
	}
	else
	{
		/*Do nothing*/
	}

	par.header.msgId = ELOG_ASYNC_DOWNLOAD_LOG;
	strncpy((schar*)(par.string), (schar*)string, ELOG_NDL_STRMAX);
	par.string[ELOG_NDL_STRMAX - 1] = '\0';
	/* ELOGプロセス呼び出し */
	FRM_MsgSendAsync(ELOG_SPORT, sizeof(par), &par);
	DEBUG_TRCOUT();
}



/* End Of File */