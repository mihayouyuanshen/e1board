/******************************************************************************/
/*    Description    :                                                        */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC Jiang.Z                                             */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/05/04                                              */
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
#include "elog_g.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "typedef.h"
#include "common_g.h"
#include "dlog_g.h"
#include "etest_shell_g.h"
#include "edat_g.h"
/*----------------------------------------------------------------------------*/
/* 公開変数定義                                                                */
/*----------------------------------------------------------------------------*/
#define EDAT_CMD_TBL_SIZE			((uchar)10)
ETEST_SubCmdItem_t s_ETestEdatSubCmdTbl[EDAT_CMD_TBL_SIZE];
ETEST_CmdItem_t g_ETestEdatCmdItem = {
	"edat",
	"edat",
	s_ETestEdatSubCmdTbl,
	ARRAY_SIZE(s_ETestEdatSubCmdTbl)
};
/*----------------------------------------------------------------------------*/
/* 非公開変数定義                                                              */
/*----------------------------------------------------------------------------*/
void errorSetErrCodeTest(slong argc, schar *agrv[]);
void errorSetBoardErrCodeTest(slong argc, schar *argv[]);
void errorSetPcsErrCodeTest(slong argc, schar *argv[]);
void errorSetRmcErrCodeTest(slong argc, schar *argv[]);

void errorShowErrTest(slong argc, schar *agrv[]);
void errorClearErrTest(slong argc, schar *agrv[]);
void errorClearBoardErrTest(slong argc, schar *argv[]);
void errorClearPcsErrTest(slong argc, schar *argv[]);
void errorClearRmcErrTest(slong argc, schar *argv[]);

void etest_edatCrtCurFile(slong argc, schar *agrv[]);
void etest_edatGetAndSet(slong argc, schar *agrv[]);

#define EDAT_ERROR_CODE_LEN			((uchar)8u)
#define EDAT_VAL_PWRHOUSEID_LEN		((uchar)27)			/* 発電所ID */
#define EDAT_VAL_FIXSHDDATE_LEN		((uchar)8)			/* 固定スケジュール開始年月日 */
#define	EDAT_VAL_POSTALNUM_LEN		((uchar)8)			/* 郵便番号 */
#define	EDAT_VAL_IPADDRESS_LEN		((uchar)16)			/* IPアドレス */
#define	EDAT_VAL_PROXYADDRESS_LEN	((uchar)13)			/* PROXYアドレス */
#define	EDAT_VAL_PORTNUMBER_LEN		((uchar)6)			/* ポート番号 */
#define EDAT_VAL_CHARGTIME_LEN		((uchar)6)			/* hhmm */
#define EDAT_VAL_SETUP_TIME_LEN		((uchar)12u)
#define EDAT_VAL_DATE_LEN			((uchar)7)			/* YYMMDD */

/* システム_機種名 */
#define EDAT_VAL_SYSMDLNM_LEN				((uchar)17u)
/* 通信ボード_機種名 */
#define EDAT_VAL_MDLNAME_LEN				((uchar)17u)
/* 通信ボード_製造番号 */
#define EDAT_VAL_SERNUM_LEN					((uchar)17u)
/* 通信ボード_FWバージョン */
#define EDAT_VAL_BRDFWV_LEN					((uchar)9u)
/* OPリモコン_FW */
#define EDAT_VAL_OPRMCTLFW_LEN				((uchar)9u)
/* ECHONET Lite */
#define EDAT_VAL_ECHOLITE_LEN				((uchar)2u)
/* 更新スケジュール取得日 */
#define EDAT_VAL_UPDESHDDATE_LEN			((uchar)15u)

ETEST_SubCmdItem_t s_ETestEdatSubCmdTbl[EDAT_CMD_TBL_SIZE] = {
	{
		"adderr", "simulate generate error, usage: adderr errcode errrating", 
		errorSetErrCodeTest
	},
	{
		"clear", "clear errcode, usage: clear errcode", errorClearErrTest
	},
	{
		"addboarderr", 
		"simulate generate board error, usage: addboarderr errcode errrating", 
		errorSetBoardErrCodeTest
	},
	{
		"addpcserr", 
		"simulate generate pcs error, usage: addpcserr errcode errrating", 
		errorSetPcsErrCodeTest
	},
	{
		"addrmcerr", 
		"simulate generate rmc error, usage: addrmcerr errcode errrating", 
		errorSetRmcErrCodeTest
	},
	{
		"showerr", "show errcode, usage: showerr", errorShowErrTest
	},
	{
		"clearboarderr", "clear board errcode, usage: clearboarderr errcode", 
		errorClearBoardErrTest
	},
	{
		"clearpcserr", "clear pcs errcode, usage: clearpcserr errcode", 
		errorClearPcsErrTest
	},
	{
		"clearrmcerr", "clear rmc errcode, usage: clearrmcerr errcode", 
		errorClearRmcErrTest
	},
	{
		"showdat", "showdat, usage: show data set and get", etest_edatGetAndSet
	},
	{
		"crtfile", "crtfile, usage: create current file", etest_edatCrtCurFile
	}
};

/*----------------------------------------------------------------------------*/
/* 公開関数定義                                                                */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開関数定義                                                              */
/*----------------------------------------------------------------------------*/
void errorSetErrCodeTest(slong argc, schar *argv[])
{
	if (3u != argc)
	{
		printf("usage: adderr error code errrating\n");
		return;
	}
	EDAT_SetBoardErr(argv[1], *argv[2]);
}

void errorClearErrTest(slong argc, schar *argv[])
{
	if (argc <= 1u)
	{
		printf("usage: clear errorr code\n");
		return;
	}
	EDAT_ClearBoardErr(argv[1]);
}

void errorSetBoardErrCodeTest(slong argc, schar *argv[])
{
	if (3u != argc)
	{
		printf("usage: add board errorr cmd was entered incorrectly\n");
		return;
	}
	EDAT_SetBoardErr(argv[1], *argv[2]);
}

void errorSetPcsErrCodeTest(slong argc, schar *argv[])
{
	if (3u != argc)
	{
		printf("usage: add pcs errorr cmd was entered incorrectly\n");
		return;
	}
	EDAT_SetPcsErr(argv[1], *argv[2]);
}

void errorSetRmcErrCodeTest(slong argc, schar *argv[])
{
	if (3u != argc)
	{
		printf("usage: add rmc errorr cmd was entered incorrectly\n");
		return;
	}
	EDAT_SetRmcErr(argv[1], *argv[2]);
}

void errorShowErrTest(slong argc, schar *argv[])
{
	if (1u != argc)
	{
		printf("usage: show error\n");
		return;
	}
	EdatErrInfo_t info;
	EDAT_GetErrInfo(&info);
	printf("current error information:%s %c\n", info.errCode, info.errCodeRating);
}

void errorClearBoardErrTest(slong argc, schar *argv[])
{
	if (argc <= 1u)
	{
		printf("usage: clear board errorr cmd was entered incorrectly\n");
		return;
	}
	EDAT_ClearBoardErr(argv[1]);
}

void errorClearPcsErrTest(slong argc, schar *argv[])
{
	if (argc <= 1u)
	{
		printf("usage: clear pcs errorr cmd was entered incorrectly\n");
		return;
	}
	EDAT_ClearPcsErr(argv[1]);
}

void errorClearRmcErrTest(slong argc, schar *argv[])
{
	if (argc <= 1u)
	{
		printf("usage: clear rmc errorr cmd was entered incorrectly\n");
		return;
	}
	EDAT_ClearRmcErr(argv[1]);
}

/* 構成 */
void etest_edatExtSunPwrOrnot(void)
{
	uchar exepRes = 0u;
	uchar testVal;

	testVal = EDAT_ValIfConstGetExtSunPwrOrnot();

	DLOG_Debug("Get exterSunPwrOrnot defval:%d", testVal);

	EDAT_ValIfConstSetExtSunPwrOrnot(exepRes);
	testVal = EDAT_ValIfConstGetExtSunPwrOrnot();
	DLOG_Debug("Get exterSunPwrOrnot : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set exterSunPwrOrnot success");
	}
	else
	{
		DLOG_Debug("Set exterSunPwrOrnot fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatSunBydPwrGenDev(void)
{
	uchar exepRes = 0u;
	uchar testVal;

	testVal = EDAT_ValIfConstGetSunBydPwrGenDev();

	DLOG_Debug("Get sunBeyondPwrGenratDevice defval:%d", testVal);

	EDAT_ValIfConstSetSunBydPwrGenDev(exepRes);
	testVal = EDAT_ValIfConstGetSunBydPwrGenDev();
	DLOG_Debug("Get sunBeyondPwrGenratDevice : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set sunBeyondPwrGenratDevice success");
	}
	else
	{
		DLOG_Debug("Set sunBeyondPwrGenratDevice fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatLoadConnType(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfConstGetLoadConnType();

	DLOG_Debug("Get loadConnectType defval:%d", testVal);

	EDAT_ValIfConstSetLoadConnType(exepRes);
	testVal = EDAT_ValIfConstGetLoadConnType();
	DLOG_Debug("Get loadConnectType : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set loadConnectType success");
	}
	else
	{
		DLOG_Debug("Set loadConnectType fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

/* 設定値 */
void etest_edatPushUpSet(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfSetValGetPushUpSet();

	DLOG_Debug("Get pushUpSet defval:%d", testVal);

	EDAT_ValIfSetValSetPushUpSet(exepRes);
	testVal = EDAT_ValIfSetValGetPushUpSet();
	DLOG_Debug("Get pushUpSet : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set pushUpSet success");
	}
	else
	{
		DLOG_Debug("Set pushUpSet fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatOutputConObj(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfSetValGetOutputConObj();

	DLOG_Debug("Get outputContrObject defval:%d", testVal);

	EDAT_ValIfSetValSetOutputConObj(exepRes);
	testVal = EDAT_ValIfSetValGetOutputConObj();
	DLOG_Debug("Get outputContrObject : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set outputContrObject success");
	}
	else
	{
		DLOG_Debug("Set outputContrObject fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatCliAction(void)
{
	uchar exepRes = 0u;
	uchar testVal;

	testVal = EDAT_ValIfSetValGetCliAction();

	DLOG_Debug("Get clipAction defval:%d", testVal);

	EDAT_ValIfSetValSetCliAction(exepRes);
	testVal = EDAT_ValIfSetValGetCliAction();
	DLOG_Debug("Get clipAction : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set clipAction success");
	}
	else
	{
		DLOG_Debug("Set clipAction fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatFitCtatType(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfSetValGetFitCtatType();

	DLOG_Debug("Get fitContractType defval:%d", testVal);

	EDAT_ValIfSetValSetFitCtatType(exepRes);
	testVal = EDAT_ValIfSetValGetFitCtatType();
	DLOG_Debug("Get fitContractType : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set fitContractType success");
	}
	else
	{
		DLOG_Debug("Set fitContractType fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatSelfCseType(void)
{
	uchar exepRes = 2u;
	uchar testVal;

	testVal = EDAT_ValIfSetValGetSelfCseType();

	DLOG_Debug("Get selfConsumeType defval:%d", testVal);

	EDAT_ValIfSetValSetSelfCseType(exepRes);
	testVal = EDAT_ValIfSetValGetSelfCseType();
	DLOG_Debug("Get selfConsumeType : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set selfConsumeType success");
	}
	else
	{
		DLOG_Debug("Set selfConsumeType fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatPwrCompServer(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfSetValGetPwrCompServer();

	DLOG_Debug("Get pwrCompanyServer defval:%d", testVal);

	EDAT_ValIfSetValSetPwrCompServer(exepRes);
	testVal = EDAT_ValIfSetValGetPwrCompServer();
	DLOG_Debug("Get pwrCompanyServer : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set pwrCompanyServer success");
	}
	else
	{
		DLOG_Debug("Set pwrCompanyServer fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

/* 機器情報 */
void etest_edatSunPwrSetLctn(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfMhcInfGetSunPweSetLctn();

	DLOG_Debug("Get sunPwrSetLocation defval:%d", testVal);

	EDAT_ValIfMhcInfSetSunPweSetLctn(exepRes);
	testVal = EDAT_ValIfMhcInfGetSunPweSetLctn();
	DLOG_Debug("Get sunPwrSetLocation : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set sunPwrSetLocation success");
	}
	else
	{
		DLOG_Debug("Set sunPwrSetLocation fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatBtSetLctn(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfMhcInfGetBtSetLctn();

	DLOG_Debug("Get btSetLocation defval:%d", testVal);

	EDAT_ValIfMhcInfSetBtSetLctn(exepRes);
	testVal = EDAT_ValIfMhcInfGetBtSetLctn();
	DLOG_Debug("Get btSetLocation : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set btSetLocation success");
	}
	else
	{
		DLOG_Debug("Set btSetLocation fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatV2hStdSetLctn(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfMhcInfGetV2hStdSetLctn();

	DLOG_Debug("Get v2hStandSetLocation defval:%d", testVal);

	EDAT_ValIfMhcInfSetV2hStdSetLctn(exepRes);
	testVal = EDAT_ValIfMhcInfGetV2hStdSetLctn();
	DLOG_Debug("Get v2hStandSetLocation : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set v2hStandSetLocation success");
	}
	else
	{
		DLOG_Debug("Set v2hStandSetLocation fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatElectWaterHeat(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfMhcInfGetElectWaterHeat();

	DLOG_Debug("Get electWaterHeating defval:%d", testVal);

	EDAT_ValIfMhcInfSetElectWaterHeat(exepRes);
	testVal = EDAT_ValIfMhcInfGetElectWaterHeat();
	DLOG_Debug("Get electWaterHeating : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set electWaterHeating success");
	}
	else
	{
		DLOG_Debug("Set electWaterHeating fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatOpRmtCtlOrnot(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfMhcInfGetOpRmtCtlOrnot();

	DLOG_Debug("Get opRemoteControlOrnot defval:%d", testVal);

	EDAT_ValIfMhcInfSetOpRmtCtlOrnot(exepRes);
	testVal = EDAT_ValIfMhcInfGetOpRmtCtlOrnot();
	DLOG_Debug("Get opRemoteControlOrnot : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set opRemoteControlOrnot success");
	}
	else
	{
		DLOG_Debug("Set opRemoteControlOrnot fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

/* 設定情報 */
void etest_edatCarType(void)
{
	uchar exepRes = 0u;
	uchar testVal;

	testVal = EDAT_ValIfSetInfGetCarType();

	DLOG_Debug("Get carType defval:%d", testVal);

	EDAT_ValIfSetInfSetCarType(exepRes);
	testVal = EDAT_ValIfSetInfGetCarType();
	DLOG_Debug("Get carType : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set carType success");
	}
	else
	{
		DLOG_Debug("Set carType fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatCarCharLmt(void)
{
	uchar exepRes = 4u;
	uchar testVal;

	testVal = EDAT_ValIfSetInfGetCarCharLmt();

	DLOG_Debug("Get carChargLimit defval:%d", testVal);

	EDAT_ValIfSetInfSetCarCharLmt(exepRes);
	testVal = EDAT_ValIfSetInfGetCarCharLmt();
	DLOG_Debug("Get carChargLimit : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set carChargLimit success");
	}
	else
	{
		DLOG_Debug("Set carChargLimit fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatIpAddrSet(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfNwsGetIpAddrSet();

	DLOG_Debug("Get ipAddrSet defval:%d", testVal);

	EDAT_ValIfNwsSetIpAddrSet(exepRes);
	testVal = EDAT_ValIfNwsGetIpAddrSet();
	DLOG_Debug("Get ipAddrSet : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set ipAddrSet success");
	}
	else
	{
		DLOG_Debug("Set ipAddrSet fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatProxySet(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfNwsGetProxySet();

	DLOG_Debug("Get proxySet defval:%d", testVal);

	EDAT_ValIfNwsSetProxySet(exepRes);
	testVal = EDAT_ValIfNwsGetProxySet();
	DLOG_Debug("Get proxySet : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set proxySet success");
	}
	else
	{
		DLOG_Debug("Set proxySet fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

/* ENL関連 */
void etest_edatBtCharStyle(void)
{
	uchar exepRes = 0u;
	uchar testVal;

	testVal = EDAT_ValIfEnlGetBtCharStyle();

	DLOG_Debug("Get btChargeStyle defval:%d", testVal);

	EDAT_ValIfEnlSetBtCharStyle(exepRes);
	testVal = EDAT_ValIfEnlGetBtCharStyle();
	DLOG_Debug("Get btChargeStyle : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set btChargeStyle success");
	}
	else
	{
		DLOG_Debug("Set btChargeStyle fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatBtDisCStyle(void)
{
	uchar exepRes = 0u;
	uchar testVal;

	testVal = EDAT_ValIfEnlGetBtDisCStyle();

	DLOG_Debug("Get btDischargeStyle defval:%d", testVal);

	EDAT_ValIfEnlSetBtDisCStyle(exepRes);
	testVal = EDAT_ValIfEnlGetBtDisCStyle();
	DLOG_Debug("Get btDischargeStyle : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set btDischargeStyle success");
	}
	else
	{
		DLOG_Debug("Set btDischargeStyle fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatV2hCharStyle(void)
{
	uchar exepRes = 0u;
	uchar testVal;

	testVal = EDAT_ValIfEnlGetV2hCharStyle();

	DLOG_Debug("Get v2hChargeStyle defval:%d", testVal);

	EDAT_ValIfEnlSetV2hCharStyle(exepRes);
	testVal = EDAT_ValIfEnlGetV2hCharStyle();
	DLOG_Debug("Get v2hChargeStyle : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set v2hChargeStyle success");
	}
	else
	{
		DLOG_Debug("Set v2hChargeStyle fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatV2hDisCStyle(void)
{
	uchar exepRes = 0u;
	uchar testVal;

	testVal = EDAT_ValIfEnlGetV2hDisCStyle();

	DLOG_Debug("Get v2hDischargeStyle defval:%d", testVal);

	EDAT_ValIfEnlSetV2hDisCStyle(exepRes);
	testVal = EDAT_ValIfEnlGetV2hDisCStyle();
	DLOG_Debug("Get v2hDischargeStyle : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set v2hDischargeStyle success");
	}
	else
	{
		DLOG_Debug("Set v2hDischargeStyle fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

/* 見守り関連 */
void etest_edatActLogCollCyc(void)
{
	uchar exepRes = 30u;
	uchar testVal;

	testVal = EDAT_ValIfMtrGetActLogCollCyc();

	DLOG_Debug("Get actLogCollectionCycle defval:%d", testVal);

	EDAT_ValIfMtrSetActLogCollCyc(exepRes);
	testVal = EDAT_ValIfMtrGetActLogCollCyc();
	DLOG_Debug("Get actLogCollectionCycle : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set actLogCollectionCycle success");
	}
	else
	{
		DLOG_Debug("Set actLogCollectionCycle fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

/* FW更新関連 */
void etest_edatUpdeLed(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfFwUpdeGetUpdeLed();

	DLOG_Debug("Get updateLed defval:%d", testVal);

	EDAT_ValIfFwUpdeSetUpdeLed(exepRes);
	testVal = EDAT_ValIfFwUpdeGetUpdeLed();
	DLOG_Debug("Get updateLed : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set updateLed success");
	}
	else
	{
		DLOG_Debug("Set updateLed fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatAnalyRes(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfFwUpdeGetAnalyRes();

	DLOG_Debug("Get analyRes defval:%d", testVal);

	EDAT_ValIfFwUpdeSetAnalyRes(exepRes);
	testVal = EDAT_ValIfFwUpdeGetAnalyRes();
	DLOG_Debug("Get analyRes : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set analyRes success");
	}
	else
	{
		DLOG_Debug("Set analyRes fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatBoardTgt(void)
{
	uchar exepRes = 0u;
	uchar testVal;

	testVal = EDAT_ValIfFwUpdeGetBoardTgt();

	DLOG_Debug("Get boardTgt defval:%d", testVal);

	EDAT_ValIfFwUpdeSetBoardTgt(exepRes);
	testVal = EDAT_ValIfFwUpdeGetBoardTgt();
	DLOG_Debug("Get boardTgt : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set boardTgt success");
	}
	else
	{
		DLOG_Debug("Set boardTgt fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatInvTgt(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfFwUpdeGetInvTgt();

	DLOG_Debug("Get invTgt defval:%d", testVal);

	EDAT_ValIfFwUpdeSetInvTgt(exepRes);
	testVal = EDAT_ValIfFwUpdeGetInvTgt();
	DLOG_Debug("Get invTgt : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set invTgt success");
	}
	else
	{
		DLOG_Debug("Set invTgt fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatPvTgt(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfFwUpdeGetPvTgt();

	DLOG_Debug("Get pvTgt defval:%d", testVal);

	EDAT_ValIfFwUpdeSetPvTgt(exepRes);
	testVal = EDAT_ValIfFwUpdeGetPvTgt();
	DLOG_Debug("Get pvTgt : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set pvTgt success");
	}
	else
	{
		DLOG_Debug("Set pvTgt fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatBdcTgt(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfFwUpdeGetBdcTgt();

	DLOG_Debug("Get bdcTgt defval:%d", testVal);

	EDAT_ValIfFwUpdeSetBdcTgt(exepRes);
	testVal = EDAT_ValIfFwUpdeGetBdcTgt();
	DLOG_Debug("Get bdcTgt : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set bdcTgt success");
	}
	else
	{
		DLOG_Debug("Set bdcTgt fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatJbTgt(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfFwUpdeGetJbTgt();

	DLOG_Debug("Get jbTgt defval:%d", testVal);

	EDAT_ValIfFwUpdeSetJbTgt(exepRes);
	testVal = EDAT_ValIfFwUpdeGetJbTgt();
	DLOG_Debug("Get jbTgt : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set jbTgt success");
	}
	else
	{
		DLOG_Debug("Set jbTgt fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

/* メンテナンス関連 */
void etest_edatExecuteMode(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfMaintGetExecuteMode();

	DLOG_Debug("Get executeMode defval:%d", testVal);

	EDAT_ValIfMaintSetExecuteMode(exepRes);
	testVal = EDAT_ValIfMaintGetExecuteMode();
	DLOG_Debug("Get executeMode : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set executeMode success");
	}
	else
	{
		DLOG_Debug("Set executeMode fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatMaintRes(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfMaintGetMaintRes();

	DLOG_Debug("Get maintainResult defval:%d", testVal);

	EDAT_ValIfMaintSetMaintRes(exepRes);
	testVal = EDAT_ValIfMaintGetMaintRes();
	DLOG_Debug("Get maintainResult : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set maintainResult success");
	}
	else
	{
		DLOG_Debug("Set maintainResult fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

/* CMD65 */
void etest_edatCommBoardMgeMode(void)
{
	uchar exepRes = 0u;
	uchar testVal;

	testVal = EDAT_ValIfCmd65GetCommBoardMgeMode();

	DLOG_Debug("Get commBoardManageMode defval:%d", testVal);

	EDAT_ValIfCmd65SetCommBoardMgeMode(exepRes);
	testVal = EDAT_ValIfCmd65GetCommBoardMgeMode();
	DLOG_Debug("Get commBoardManageMode : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set commBoardManageMode success");
	}
	else
	{
		DLOG_Debug("Set commBoardManageMode fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatEmergencyMode(void)
{
	uchar exepRes = 0u;
	uchar testVal;

	testVal = EDAT_ValIfCmd65GetEmergencyMode();

	DLOG_Debug("Get emergencyMode defval:%d", testVal);

	EDAT_ValIfCmd65SetEmergencyMode(exepRes);
	testVal = EDAT_ValIfCmd65GetEmergencyMode();
	DLOG_Debug("Get emergencyMode : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set emergencyMode success");
	}
	else
	{
		DLOG_Debug("Set emergencyMode fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

/* CMD75 */
void etest_edatCarCharBodyOprat(void)
{
	uchar exepRes = 4u;
	uchar testVal;

	testVal = EDAT_ValIfCmd75GetCarCharBodyOprat();

	DLOG_Debug("Get carCharordiscBodyOprat defval:%d", testVal);

	EDAT_ValIfCmd75SetCarCharBodyOprat(exepRes);
	testVal = EDAT_ValIfCmd75GetCarCharBodyOprat();
	DLOG_Debug("Get carCharordiscBodyOprat : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set carCharordiscBodyOprat success");
	}
	else
	{
		DLOG_Debug("Set carCharordiscBodyOprat fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatPwrcutDischar(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfCmd75GetPwrcutDischar();

	DLOG_Debug("Get pwrcutDischar defval:%d", testVal);

	EDAT_ValIfCmd75SetPwrcutDischar(exepRes);
	testVal = EDAT_ValIfCmd75GetPwrcutDischar();
	DLOG_Debug("Get pwrcutDischar : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set pwrcutDischar success");
	}
	else
	{
		DLOG_Debug("Set pwrcutDischar fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatPwrcutDiscPrySet(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfCmd75GetPwrcutDiscPrySet();

	DLOG_Debug("Get pwrcutDiscPrioritySet defval:%d", testVal);

	EDAT_ValIfCmd75SetPwrcutDiscPrySet(exepRes);
	testVal = EDAT_ValIfCmd75GetPwrcutDiscPrySet();
	DLOG_Debug("Get pwrcutDiscPrioritySet : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set pwrcutDiscPrioritySet success");
	}
	else
	{
		DLOG_Debug("Set pwrcutDiscPrioritySet fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatPwrcutCharPrySet(void)
{
	uchar exepRes = 0u;
	uchar testVal;

	testVal = EDAT_ValIfCmd75GetPwrcutCharPrySet();

	DLOG_Debug("Get pwrcutCharPrioritySet defval:%d", testVal);

	EDAT_ValIfCmd75SetPwrcutCharPrySet(exepRes);
	testVal = EDAT_ValIfCmd75GetPwrcutCharPrySet();
	DLOG_Debug("Get pwrcutCharPrioritySet : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set pwrcutCharPrioritySet success");
	}
	else
	{
		DLOG_Debug("Set pwrcutCharPrioritySet fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatCarDiscOrnot(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfCmd75GetCarDiscOrnot();

	DLOG_Debug("Get carDiscOrnot defval:%d", testVal);

	EDAT_ValIfCmd75SetCarDiscOrnot(exepRes);
	testVal = EDAT_ValIfCmd75GetCarDiscOrnot();
	DLOG_Debug("Get carDiscOrnot : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set carDiscOrnot success");
	}
	else
	{
		DLOG_Debug("Set carDiscOrnot fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatDiscPrySet(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfCmd75GetDiscPrySet();

	DLOG_Debug("Get discharPrioritySet defval:%d", testVal);

	EDAT_ValIfCmd75SetDiscPrySet(exepRes);
	testVal = EDAT_ValIfCmd75GetDiscPrySet();
	DLOG_Debug("Get discharPrioritySet : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set discharPrioritySet success");
	}
	else
	{
		DLOG_Debug("Set discharPrioritySet fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatCharPrySet(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfCmd75GetCharPrySet();

	DLOG_Debug("Get chargePrioritySet defval:%d", testVal);

	EDAT_ValIfCmd75SetCharPrySet(exepRes);
	testVal = EDAT_ValIfCmd75GetCharPrySet();
	DLOG_Debug("Get chargePrioritySet : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set chargePrioritySet success");
	}
	else
	{
		DLOG_Debug("Set chargePrioritySet fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

/* CMD6B */
void etest_edatPwrcutOutSet(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfCmd6bGetPwrcutOutSet();

	DLOG_Debug("Get pwrcutOutSet defval:%d", testVal);

	EDAT_ValIfCmd6bSetPwrcutOutSet(exepRes);
	testVal = EDAT_ValIfCmd6bGetPwrcutOutSet();
	DLOG_Debug("Get pwrcutOutSet : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set pwrcutOutSet success");
	}
	else
	{
		DLOG_Debug("Set pwrcutOutSet fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatPwrcutOutStart(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfCmd6bGetPwrcutOutStart();

	DLOG_Debug("Get pwrcutOutStart defval:%d", testVal);

	EDAT_ValIfCmd6bSetPwrcutOutStart(exepRes);
	testVal = EDAT_ValIfCmd6bGetPwrcutOutStart();
	DLOG_Debug("Get pwrcutOutStart : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set pwrcutOutStart success");
	}
	else
	{
		DLOG_Debug("Set pwrcutOutStart fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatSellValOrInval(void)
{
	uchar exepRes = 1u;
	uchar testVal;

	testVal = EDAT_ValIfCmd6bGetSellValOrInval();

	DLOG_Debug("Get sellValidOrInvalid defval:%d", testVal);

	EDAT_ValIfCmd6bSetSellValOrInval(exepRes);
	testVal = EDAT_ValIfCmd6bGetSellValOrInval();
	DLOG_Debug("Get sellValidOrInvalid : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set sellValidOrInvalid success");
	}
	else
	{
		DLOG_Debug("Set sellValidOrInvalid fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

/* 整定値 */
void etest_edatFreRiseDetLvl(void)
{
	uchar exepRes = 100u;
	uchar testVal;

	testVal = EDAT_ValIfStgGetFreRiseDetLvl();

	DLOG_Debug("Get freRiseDetectLevel defval:%d", testVal);

	EDAT_ValIfStgSetFreRiseDetLvl(exepRes);
	testVal = EDAT_ValIfStgGetFreRiseDetLvl();
	DLOG_Debug("Get freRiseDetectLevel : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set freRiseDetectLevel success");
	}
	else
	{
		DLOG_Debug("Set freRiseDetectLevel fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatFreDropDetLvl(void)
{
	uchar exepRes = 100u;
	uchar testVal;

	testVal = EDAT_ValIfStgGetFreDropDetLvl();

	DLOG_Debug("Get freDropDetectLevel defval:%d", testVal);

	EDAT_ValIfStgSetFreDropDetLvl(exepRes);
	testVal = EDAT_ValIfStgGetFreDropDetLvl();
	DLOG_Debug("Get freDropDetectLevel : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set freDropDetectLevel success");
	}
	else
	{
		DLOG_Debug("Set freDropDetectLevel fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatBtCharPwrSetVal(void)
{
	ushort exepRes = 50000u;
	ushort testVal;

	testVal = EDAT_ValIfEnlGetBtCharPwrSetVal();

	DLOG_Debug("Get btChargePwrSetValue defval:%d", testVal);

	EDAT_ValIfEnlSetBtCharPwrSetVal(exepRes);
	testVal = EDAT_ValIfEnlGetBtCharPwrSetVal();
	DLOG_Debug("Get btChargePwrSetValue : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set btChargePwrSetValue success");
	}
	else
	{
		DLOG_Debug("Set btChargePwrSetValue fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatBtDisCPweSetVal(void)
{
	ushort exepRes = 14000u;
	ushort testVal;

	testVal = EDAT_ValIfEnlGetBtDisCPweSetVal();

	DLOG_Debug("Get btDiscPwrSetValue defval:%d", testVal);

	EDAT_ValIfEnlSetBtDisCPweSetVal(exepRes);
	testVal = EDAT_ValIfEnlGetBtDisCPweSetVal();
	DLOG_Debug("Get btDiscPwrSetValue : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set btDiscPwrSetValue success");
	}
	else
	{
		DLOG_Debug("Set btDiscPwrSetValue fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatV2hCharPwrSetVal(void)
{
	ushort exepRes = 50000u;
	ushort testVal;

	testVal = EDAT_ValIfEnlGetV2hCharPwrSetVal();

	DLOG_Debug("Get v2hChargePwrSetValue defval:%d", testVal);

	EDAT_ValIfEnlSetV2hCharPwrSetVal(exepRes);
	testVal = EDAT_ValIfEnlGetV2hCharPwrSetVal();
	DLOG_Debug("Get v2hChargePwrSetValue : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set v2hChargePwrSetValue success");
	}
	else
	{
		DLOG_Debug("Set v2hChargePwrSetValue fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatV2hDisCPweSetVal(void)
{
	ushort exepRes = 1000u;
	ushort testVal;

	testVal = EDAT_ValIfEnlGetV2hDisCPweSetVal();

	DLOG_Debug("Get v2hDischargePwrSetValue defval:%d", testVal);

	EDAT_ValIfEnlSetV2hDisCPweSetVal(exepRes);
	testVal = EDAT_ValIfEnlGetV2hDisCPweSetVal();
	DLOG_Debug("Get v2hDischargePwrSetValue : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set v2hDischargePwrSetValue success");
	}
	else
	{
		DLOG_Debug("Set v2hDischargePwrSetValue fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatBtAcCharQtySetVal(void)
{
	ushort exepRes = 1000u;
	ushort testVal;

	testVal = EDAT_ValIfEnlGetBtAcCharQtySetVal();

	DLOG_Debug("Get btACChargeQuatySetValue defval:%d", testVal);

	EDAT_ValIfEnlSetBtAcCharQtySetVal(exepRes);
	testVal = EDAT_ValIfEnlGetBtAcCharQtySetVal();
	DLOG_Debug("Get btACChargeQuatySetValue : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set btACChargeQuatySetValue success");
	}
	else
	{
		DLOG_Debug("Set btACChargeQuatySetValue fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatBtAcDisCQtySetVal(void)
{
	ushort exepRes = 1000u;
	ushort testVal;

	testVal = EDAT_ValIfEnlGetBtAcDisCQtySetVal();

	DLOG_Debug("Get btACDiscQuatySetValue defval:%d", testVal);

	EDAT_ValIfEnlSetBtAcDisCQtySetVal(exepRes);
	testVal = EDAT_ValIfEnlGetBtAcDisCQtySetVal();
	DLOG_Debug("Get btACDiscQuatySetValue : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set btACDiscQuatySetValue success");
	}
	else
	{
		DLOG_Debug("Set btACDiscQuatySetValue fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatBtDcCharQtySetVal(void)
{
	ushort exepRes = 1000u;
	ushort testVal;

	testVal = EDAT_ValIfEnlGetBtDcCharQtySetVal();

	DLOG_Debug("Get btDCChargeQuatySetValue defval:%d", testVal);

	EDAT_ValIfEnlSetBtDcCharQtySetVal(exepRes);
	testVal = EDAT_ValIfEnlGetBtDcCharQtySetVal();
	DLOG_Debug("Get btDCChargeQuatySetValue : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set btDCChargeQuatySetValue success");
	}
	else
	{
		DLOG_Debug("Set btDCChargeQuatySetValue fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatBtDcDiscQtySetVal(void)
{
	ushort exepRes = 1000u;
	ushort testVal;

	testVal = EDAT_ValIfEnlGetBtDcDiscQtySetVal();

	DLOG_Debug("Get btDCDiscQuatySetValue defval:%d", testVal);

	EDAT_ValIfEnlSetBtDcDiscQtySetVal(exepRes);
	testVal = EDAT_ValIfEnlGetBtDcDiscQtySetVal();
	DLOG_Debug("Get btDCDiscQuatySetValue : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set btDCDiscQuatySetValue success");
	}
	else
	{
		DLOG_Debug("Set btDCDiscQuatySetValue fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatV2hDcCharQtySetVal(void)
{
	ushort exepRes = 1000u;
	ushort testVal;

	testVal = EDAT_ValIfEnlGetV2hDcCharQtySetVal();

	DLOG_Debug("Get v2hACChargeQuatySetValue defval:%d", testVal);

	EDAT_ValIfEnlSetV2hDcCharQtySetVal(exepRes);
	testVal = EDAT_ValIfEnlGetV2hDcCharQtySetVal();
	DLOG_Debug("Get v2hACChargeQuatySetValue : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set v2hACChargeQuatySetValue success");
	}
	else
	{
		DLOG_Debug("Set v2hACChargeQuatySetValue fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatV2hDcDiscQtySetVal(void)
{
	ushort exepRes = 1000u;
	ushort testVal;

	testVal = EDAT_ValIfEnlGetV2hDcDiscQtySetVal();

	DLOG_Debug("Get v2hACDiscQuatySetValue defval:%d", testVal);

	EDAT_ValIfEnlSetV2hDcDiscQtySetVal(exepRes);
	testVal = EDAT_ValIfEnlGetV2hDcDiscQtySetVal();
	DLOG_Debug("Get v2hACDiscQuatySetValue : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set v2hACDiscQuatySetValue success");
	}
	else
	{
		DLOG_Debug("Set v2hACDiscQuatySetValue fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatUpdeNo(void)
{
	ushort exepRes = 1000u;
	ushort testVal;

	testVal = EDAT_ValIfFwUpdeGetUpdeNo();

	DLOG_Debug("Get updeNo defval:%d", testVal);

	EDAT_ValIfFwUpdeSetUpdeNo(exepRes);
	testVal = EDAT_ValIfFwUpdeGetUpdeNo();
	DLOG_Debug("Get updeNo : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set updeNo success");
	}
	else
	{
		DLOG_Debug("Set updeNo fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatBtUpLmt(void)
{
	ushort exepRes = 100u;
	ushort testVal;

	testVal = EDAT_ValIfCmd65GetBtUpLmt();

	DLOG_Debug("Get btUpperLimit defval:%d", testVal);

	EDAT_ValIfCmd65SetBtUpLmt(exepRes);
	testVal = EDAT_ValIfCmd65GetBtUpLmt();
	DLOG_Debug("Get btUpperLimit : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set btUpperLimit success");
	}
	else
	{
		DLOG_Debug("Set btUpperLimit fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatbtLowLmt(void)
{
	ushort exepRes = 30u;
	ushort testVal;

	testVal = EDAT_ValIfCmd65GetBtLowLmt();

	DLOG_Debug("Get btLowerLimit defval:%d", testVal);

	EDAT_ValIfCmd65SetBtLowLmt(exepRes);
	testVal = EDAT_ValIfCmd65GetBtLowLmt();
	DLOG_Debug("Get btLowerLimit : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set btLowerLimit success");
	}
	else
	{
		DLOG_Debug("Set btLowerLimit fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatCarDiscLowLmtSet(void)
{
	ushort exepRes = 40u;
	ushort testVal;

	testVal = EDAT_ValIfCmd75GetCarDiscLowLmtSet();

	DLOG_Debug("Get carDiscLowerLimitSet defval:%d", testVal);

	EDAT_ValIfCmd75SetCarDiscLowLmtSet(exepRes);
	testVal = EDAT_ValIfCmd75GetCarDiscLowLmtSet();
	DLOG_Debug("Get carDiscLowerLimitSet : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set carDiscLowerLimitSet success");
	}
	else
	{
		DLOG_Debug("Set carDiscLowerLimitSet fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatCarCharUpLmtSet(void)
{
	ushort exepRes = 80u;
	ushort testVal;

	testVal = EDAT_ValIfCmd75GetCarCharUpLmtSet();

	DLOG_Debug("Get carCharUpperLimitSet defval:%d", testVal);

	EDAT_ValIfCmd75SetCarCharUpLmtSet(exepRes);
	testVal = EDAT_ValIfCmd75GetCarCharUpLmtSet();
	DLOG_Debug("Get carCharUpperLimitSet : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set carCharUpperLimitSet success");
	}
	else
	{
		DLOG_Debug("Set carCharUpperLimitSet fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatPwrDiscLowLmtSet(void)
{
	ushort exepRes = 20u;
	ushort testVal;

	testVal = EDAT_ValIfCmd75GetPwrDiscLowLmtSet();

	DLOG_Debug("Get pwrcutDiscLowerLimitSet defval:%d", testVal);

	EDAT_ValIfCmd75SetPwrDiscLowLmtSet(exepRes);
	testVal = EDAT_ValIfCmd75GetPwrDiscLowLmtSet();
	DLOG_Debug("Get pwrcutDiscLowerLimitSet : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set pwrcutDiscLowerLimitSet success");
	}
	else
	{
		DLOG_Debug("Set pwrcutDiscLowerLimitSet fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatContPwrSet(void)
{
	ushort exepRes = 20u;
	ushort testVal;

	testVal = EDAT_ValIfCmd6bGetContPwrSet();

	DLOG_Debug("Get contaractPwrSet defval:%d", testVal);

	EDAT_ValIfCmd6bSetContPwrSet(exepRes);
	testVal = EDAT_ValIfCmd6bGetContPwrSet();
	DLOG_Debug("Get contaractPwrSet : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set contaractPwrSet success");
	}
	else
	{
		DLOG_Debug("Set contaractPwrSet fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatSysVolDetLvl(void)
{
	ushort exepRes = 1000u;
	ushort testVal;

	testVal = EDAT_ValIfStgGetSysVolDetLvl();

	DLOG_Debug("Get sysVoltDetectLevel defval:%d", testVal);

	EDAT_ValIfStgSetSysVolDetLvl(exepRes);
	testVal = EDAT_ValIfStgGetSysVolDetLvl();
	DLOG_Debug("Get sysVoltDetectLevel : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set sysVoltDetectLevel success");
	}
	else
	{
		DLOG_Debug("Set sysVoltDetectLevel fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatSysVolDetTime(void)
{
	ushort exepRes = 1000u;
	ushort testVal;

	testVal = EDAT_ValIfStgGetSysVolDetTime();

	DLOG_Debug("Get sysVoltDetectTime defval:%d", testVal);

	EDAT_ValIfStgSetSysVolDetTime(exepRes);
	testVal = EDAT_ValIfStgGetSysVolDetTime();
	DLOG_Debug("Get sysVoltDetectTime : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set sysVoltDetectTime success");
	}
	else
	{
		DLOG_Debug("Set sysVoltDetectTime fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatSysUdrVolDetLvl(void)
{
	ushort exepRes = 1000u;
	ushort testVal;

	testVal = EDAT_ValIfStgGetSysUdrVolDetLvl();

	DLOG_Debug("Get sysUnderVoltDetectLevel defval:%d", testVal);

	EDAT_ValIfStgSetSysUdrVolDetLvl(exepRes);
	testVal = EDAT_ValIfStgGetSysUdrVolDetLvl();
	DLOG_Debug("Get sysUnderVoltDetectLevel : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set sysUnderVoltDetectLevel success");
	}
	else
	{
		DLOG_Debug("Set sysUnderVoltDetectLevel fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatSysUdrVolDetTime(void)
{
	ushort exepRes = 1000u;
	ushort testVal;

	testVal = EDAT_ValIfStgGetSysUdrVolDetTime();

	DLOG_Debug("Get sysUnderVoltDetectTime defval:%d", testVal);

	EDAT_ValIfStgSetSysUdrVolDetTime(exepRes);
	testVal = EDAT_ValIfStgGetSysUdrVolDetTime();
	DLOG_Debug("Get sysUnderVoltDetectTime : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set sysUnderVoltDetectTime success");
	}
	else
	{
		DLOG_Debug("Set sysUnderVoltDetectTime fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatFreRiseDetTime(void)
{
	ushort exepRes = 1000u;
	ushort testVal;

	testVal = EDAT_ValIfStgGetFreRiseDetTime();

	DLOG_Debug("Get freRiseDetectTime defval:%d", testVal);

	EDAT_ValIfStgSetFreRiseDetTime(exepRes);
	testVal = EDAT_ValIfStgGetFreRiseDetTime();
	DLOG_Debug("Get freRiseDetectTime : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set freRiseDetectTime success");
	}
	else
	{
		DLOG_Debug("Set freRiseDetectTime fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatFreDropDetTime(void)
{
	ushort exepRes = 1000u;
	ushort testVal;

	testVal = EDAT_ValIfStgGetFreDropDetTime();

	DLOG_Debug("Get freDropDetectTime defval:%d", testVal);

	EDAT_ValIfStgSetFreDropDetTime(exepRes);
	testVal = EDAT_ValIfStgGetFreDropDetTime();
	DLOG_Debug("Get freDropDetectTime : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set freDropDetectTime success");
	}
	else
	{
		DLOG_Debug("Set freDropDetectTime fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatResctTime(void)
{
	ushort exepRes = 2000u;
	ushort testVal;

	testVal = EDAT_ValIfStgGetResctTime();

	DLOG_Debug("Get resctTime defval:%d", testVal);

	EDAT_ValIfStgSetResctTime(exepRes);
	testVal = EDAT_ValIfStgGetResctTime();
	DLOG_Debug("Get resctTime : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set resctTime success");
	}
	else
	{
		DLOG_Debug("Set resctTime fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatVolRsInhbVal(void)
{
	ushort exepRes = 100u;
	ushort testVal;

	testVal = EDAT_ValIfStgGetVolRsInhbVal();

	DLOG_Debug("Get volRsInhbValue defval:%d", testVal);

	EDAT_ValIfStgSetVolRsInhbVal(exepRes);
	testVal = EDAT_ValIfStgGetVolRsInhbVal();
	DLOG_Debug("Get volRsInhbValue : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set volRsInhbValue success");
	}
	else
	{
		DLOG_Debug("Set volRsInhbValue fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatVolRsInhbLmt(void)
{
	ushort exepRes = 210u;
	ushort testVal;

	testVal = EDAT_ValIfStgGetVolRsInhbLmt();

	DLOG_Debug("Get volRsInhbattLimit defval:%d", testVal);

	EDAT_ValIfStgSetVolRsInhbLmt(exepRes);
	testVal = EDAT_ValIfStgGetVolRsInhbLmt();
	DLOG_Debug("Get volRsInhbattLimit : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set volRsInhbattLimit success");
	}
	else
	{
		DLOG_Debug("Set volRsInhbattLimit fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatCotPwrCtl(void)
{
	ushort exepRes = 90u;
	ushort testVal;

	testVal = EDAT_ValIfStgGetCotPwrCtl();

	DLOG_Debug("Get cotPowerControl defval:%d", testVal);

	EDAT_ValIfStgSetCotPwrCtl(exepRes);
	testVal = EDAT_ValIfStgGetCotPwrCtl();
	DLOG_Debug("Get cotPowerControl : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set cotPowerControl success");
	}
	else
	{
		DLOG_Debug("Set cotPowerControl fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatPassIndOperDet(void)
{
	ushort exepRes = 4u;
	ushort testVal;

	testVal = EDAT_ValIfStgGetPassIndOperDet();

	DLOG_Debug("Get passIndOperDetect defval:%d", testVal);

	EDAT_ValIfStgSetPassIndOperDet(exepRes);
	testVal = EDAT_ValIfStgGetPassIndOperDet();
	DLOG_Debug("Get passIndOperDetect : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set passIndOperDetect success");
	}
	else
	{
		DLOG_Debug("Set passIndOperDetect fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatFreFfbkGain(void)
{
	ushort exepRes = 2000u;
	ushort testVal;

	testVal = EDAT_ValIfStgGetFreFfbkGain();

	DLOG_Debug("Get freFeedbackGain defval:%d", testVal);

	EDAT_ValIfStgSetFreFfbkGain(exepRes);
	testVal = EDAT_ValIfStgGetFreFfbkGain();
	DLOG_Debug("Get freFeedbackGain : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set freFeedbackGain success");
	}
	else
	{
		DLOG_Debug("Set freFeedbackGain fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatFreLevAllow(void)
{
	ushort exepRes = 20u;
	ushort testVal;

	testVal = EDAT_ValIfStgGetFreLevAllow();

	DLOG_Debug("Get freLevAllow defval:%d", testVal);

	EDAT_ValIfStgSetFreLevAllow(exepRes);
	testVal = EDAT_ValIfStgGetFreLevAllow();
	DLOG_Debug("Get freLevAllow : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set freLevAllow success");
	}
	else
	{
		DLOG_Debug("Set freLevAllow fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatChgTimeIncDes(void)
{
	ushort exepRes = 6u;
	ushort testVal;

	testVal = EDAT_ValIfStgGetChgTimeIncDes();

	DLOG_Debug("Get changeTimeIncorDes defval:%d", testVal);

	EDAT_ValIfStgSetChgTimeIncDes(exepRes);
	testVal = EDAT_ValIfStgGetChgTimeIncDes();
	DLOG_Debug("Get changeTimeIncorDes : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set changeTimeIncorDes success");
	}
	else
	{
		DLOG_Debug("Set changeTimeIncorDes fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatEptAppCap(void)
{
	ushort exepRes = 50000u;
	ushort testVal;

	testVal = EDAT_ValIfSetValGetEptAppCap();

	DLOG_Debug("Get eqpmtApproveCap defval:%d", testVal);

	EDAT_ValIfSetValSetEptAppCap(exepRes);
	testVal = EDAT_ValIfSetValGetEptAppCap();
	DLOG_Debug("Get eqpmtApproveCap : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set eqpmtApproveCap success");
	}
	else
	{
		DLOG_Debug("Set eqpmtApproveCap fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatGenPwrUpLmt(void)
{
	ushort exepRes = 50000u;
	ushort testVal;

	testVal = EDAT_ValIfSetValGetGenPwrUpLmt();

	DLOG_Debug("Get genPwrUpperLimit defval:%d", testVal);

	EDAT_ValIfSetValSetGenPwrUpLmt(exepRes);
	testVal = EDAT_ValIfSetValGetGenPwrUpLmt();
	DLOG_Debug("Get genPwrUpperLimit : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set genPwrUpperLimit success");
	}
	else
	{
		DLOG_Debug("Set genPwrUpperLimit fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatSunPwrRated(void)
{
	ushort exepRes = 50000u;
	ushort testVal;

	testVal = EDAT_ValIfPvGetSunPwrRated();

	DLOG_Debug("Get sunPwrRated defval:%d", testVal);

	EDAT_ValIfPvSetSunPwrRated(exepRes);
	testVal = EDAT_ValIfPvGetSunPwrRated();
	DLOG_Debug("Get sunPwrRated : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set sunPwrRated success");
	}
	else
	{
		DLOG_Debug("Set sunPwrRated fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatPvPanelRating(void)
{
	ushort exepRes = 20000u;
	ushort testVal;

	testVal = EDAT_ValIfPvGetPvPanelRating();

	DLOG_Debug("Get pvPanelRating defval:%d", testVal);

	EDAT_ValIfPvSetPvPanelRating(exepRes);
	testVal = EDAT_ValIfPvGetPvPanelRating();
	DLOG_Debug("Get pvPanelRating : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set pvPanelRating success");
	}
	else
	{
		DLOG_Debug("Set pvPanelRating fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatBtCharPwrMaxValSet(void)
{
	ushort exepRes = 50000u;
	ushort testVal;

	testVal = EDAT_ValIfSetInfGetBtCharPwrMaxValSet();

	DLOG_Debug("Get btCharPwrMaxValueSet defval:%d", testVal);

	EDAT_ValIfSetInfSetBtCharPwrMaxValSet(exepRes);
	testVal = EDAT_ValIfSetInfGetBtCharPwrMaxValSet();
	DLOG_Debug("Get btCharPwrMaxValueSet : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set btCharPwrMaxValueSet success");
	}
	else
	{
		DLOG_Debug("Set btCharPwrMaxValueSet fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatCarCharPwrValSet(void)
{
	ushort exepRes = 50000u;
	ushort testVal;

	testVal = EDAT_ValIfSetInfGetCarCharPwrValSet();

	DLOG_Debug("Get btChargePwrSetValue defval:%d", testVal);

	EDAT_ValIfSetInfSetCarCharPwrValSet(exepRes);
	testVal = EDAT_ValIfSetInfGetCarCharPwrValSet();
	DLOG_Debug("Get carCharPwrValueSet : %d", testVal);

	if(exepRes == testVal)
	{
		DLOG_Debug("Set carCharPwrValueSet success");
	}
	else
	{
		DLOG_Debug("Set carCharPwrValueSet fail exepRes : %d testVal : %d", exepRes, testVal);
	}
}

void etest_edatPwrHouseId(void)
{
	schar *exepRes = "123456789";
	schar testVal[EDAT_VAL_PWRHOUSEID_LEN];

	EDAT_ValIfSetValGetPwrHouseId(testVal);
	DLOG_Debug("Get pwrHouseId defval:%s", testVal);

	EDAT_ValIfSetValSetPwrHouseId(exepRes);
	EDAT_ValIfSetValGetPwrHouseId(testVal);
	DLOG_Debug("Get pwrHouseId : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set pwrHouseId success");
	}
	else
	{
		DLOG_Debug("Set pwrHouseId fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatFixShdSrtDate(void)
{
	schar *exepRes = "230506";
	schar testVal[EDAT_VAL_FIXSHDDATE_LEN];

	EDAT_ValIfPvGetFixShdSrtDate(testVal);
	DLOG_Debug("Get fixShdStartDate defval:%s", testVal);

	EDAT_ValIfPvSetFixShdSrtDate(exepRes);
	EDAT_ValIfPvGetFixShdSrtDate(testVal);
	DLOG_Debug("Get fixShdStartDate : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set fixShdStartDate success");
	}
	else
	{
		DLOG_Debug("Set fixShdStartDate fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatPostNum(void)
{
	schar *exepRes = "1234567";
	schar testVal[EDAT_VAL_POSTALNUM_LEN];

	EDAT_ValIfMhcInfGetPostNum(testVal);
	DLOG_Debug("Get postalNum defval:%s", testVal);

	EDAT_ValIfMhcInfSetPostNum(exepRes);
	EDAT_ValIfMhcInfGetPostNum(testVal);
	DLOG_Debug("Get postalNum : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set postalNum success");
	}
	else
	{
		DLOG_Debug("Set postalNum fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatIpAddress(void)
{
	schar *exepRes = "1921681200";
	schar testVal[EDAT_VAL_PWRHOUSEID_LEN];

	EDAT_ValIfNwsGetIpAddress(testVal);
	DLOG_Debug("Get ipAddress defval:%s", testVal);

	EDAT_ValIfNwsSetIpAddress(exepRes);
	EDAT_ValIfNwsGetIpAddress(testVal);
	DLOG_Debug("Get ipAddress : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set ipAddress success");
	}
	else
	{
		DLOG_Debug("Set ipAddress fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatSubMask(void)
{
	schar *exepRes = "2552552551";
	schar testVal[EDAT_VAL_PWRHOUSEID_LEN];

	EDAT_ValIfNwsGetSubMask(testVal);
	DLOG_Debug("Get subnetMask defval:%s", testVal);

	EDAT_ValIfNwsSetSubMask(exepRes);
	EDAT_ValIfNwsGetSubMask(testVal);
	DLOG_Debug("Get subnetMask : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set subnetMask success");
	}
	else
	{
		DLOG_Debug("Set subnetMask fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatDftGateway(void)
{
	schar *exepRes = "19216812";
	schar testVal[EDAT_VAL_PWRHOUSEID_LEN];

	EDAT_ValIfNwsGetDftGateway(testVal);
	DLOG_Debug("Get defaultGateway defval:%s", testVal);

	EDAT_ValIfNwsSetDftGateway(exepRes);
	EDAT_ValIfNwsGetDftGateway(testVal);
	DLOG_Debug("Get defaultGateway : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set defaultGateway success");
	}
	else
	{
		DLOG_Debug("Set defaultGateway fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatPtyDns(void)
{
	schar *exepRes = "19216813";
	schar testVal[EDAT_VAL_PWRHOUSEID_LEN];

	EDAT_ValIfNwsGetPtyDns(testVal);
	DLOG_Debug("Get poiorityDns defval:%s", testVal);

	EDAT_ValIfNwsSetPtyDns(exepRes);
	EDAT_ValIfNwsGetPtyDns(testVal);
	DLOG_Debug("Get poiorityDns : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set poiorityDns success");
	}
	else
	{
		DLOG_Debug("Set poiorityDns fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatRleDns(void)
{
	schar *exepRes = "19216814";
	schar testVal[EDAT_VAL_PWRHOUSEID_LEN];

	EDAT_ValIfNwsGetRleDns(testVal);
	DLOG_Debug("Get replaceDns defval:%s", testVal);

	EDAT_ValIfNwsSetRleDns(exepRes);
	EDAT_ValIfNwsGetRleDns(testVal);
	DLOG_Debug("Get replaceDns : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set replaceDns success");
	}
	else
	{
		DLOG_Debug("Set replaceDns fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatPxyAddress(void)
{
	schar *exepRes = "123456789";
	schar testVal[EDAT_VAL_PWRHOUSEID_LEN];

	EDAT_ValIfNwsGetPxyAddress(testVal);
	DLOG_Debug("Get proxyAddress defval:%s", testVal);

	EDAT_ValIfNwsSetPxyAddress(exepRes);
	EDAT_ValIfNwsGetPxyAddress(testVal);
	DLOG_Debug("Get proxyAddress : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set proxyAddress success");
	}
	else
	{
		DLOG_Debug("Set proxyAddress fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatSysMdlName(void)
{
	schar *exepRes = "sysModelName";
	schar testVal[EDAT_VAL_SYSMDLNM_LEN];

	EDAT_ValIfMhcInfGetSysMdlName(testVal);
	DLOG_Debug("Get sysModelName defval:%s", testVal);

	EDAT_ValIfMhcInfSetSysMdlName(exepRes);
	EDAT_ValIfMhcInfGetSysMdlName(testVal);
	DLOG_Debug("Get sysModelName : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set sysModelName success");
	}
	else
	{
		DLOG_Debug("Set sysModelName fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}
void etest_edatCommMdlName(void)
{
	schar *exepRes = "commModelName";
	schar testVal[EDAT_VAL_MDLNAME_LEN];

	EDAT_ValIfMhcInfGetComMdlName(testVal);
	DLOG_Debug("Get commModelName defval:%s", testVal);

	EDAT_ValIfMhcInfSetComMdlName(exepRes);
	EDAT_ValIfMhcInfGetComMdlName(testVal);
	DLOG_Debug("Get commModelName : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set commModelName success");
	}
	else
	{
		DLOG_Debug("Set commModelName fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}
void etest_edatCommSerialNum(void)
{
	schar *exepRes = "commSerialNum";
	schar testVal[EDAT_VAL_SERNUM_LEN];

	EDAT_ValIfMhcInfGetComSerialNum(testVal);
	DLOG_Debug("Get commSerialNum defval:%s", testVal);

	EDAT_ValIfMhcInfSetComSerialNum(exepRes);
	EDAT_ValIfMhcInfGetComSerialNum(testVal);
	DLOG_Debug("Get commSerialNum : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set commSerialNum success");
	}
	else
	{
		DLOG_Debug("Set commSerialNum fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}
void etest_edatCommBoardFwVer(void)
{
	schar *exepRes = "comBFv";
	schar testVal[EDAT_VAL_BRDFWV_LEN];

	EDAT_ValIfMhcInfGetComBoardFwVer(testVal);
	DLOG_Debug("Get commBoardFwVer defval:%s", testVal);

	EDAT_ValIfMhcInfSetComBoardFwVer(exepRes);
	EDAT_ValIfMhcInfGetComBoardFwVer(testVal);
	DLOG_Debug("Get commBoardFwVer : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set commBoardFwVer success");
	}
	else
	{
		DLOG_Debug("Set commBoardFwVer fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}
void etest_edatOpRmtCtrlFw(void)
{
	schar *exepRes = "opRmtFw";
	schar testVal[EDAT_VAL_OPRMCTLFW_LEN];

	EDAT_ValIfMhcInfGetOpRmtCtlFw(testVal);
	DLOG_Debug("Get opRemoteCtlFw defval:%s", testVal);

	EDAT_ValIfMhcInfSetOpRmtCtlFw(exepRes);
	EDAT_ValIfMhcInfGetOpRmtCtlFw(testVal);
	DLOG_Debug("Get opRemoteCtlFw : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set opRemoteCtlFw success");
	}
	else
	{
		DLOG_Debug("Set opRemoteCtlFw fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}
void etest_edatEchonetLite(void)
{
	schar *exepRes = "Q";
	schar testVal[EDAT_VAL_ECHOLITE_LEN];

	EDAT_ValIfMhcInfGetEchonetLite(testVal);
	DLOG_Debug("Get echonetLiteVer defval:%s", testVal);

	EDAT_ValIfMhcInfSetEchonetLite(exepRes);
	EDAT_ValIfMhcInfGetEchonetLite(testVal);
	DLOG_Debug("Get echonetLiteVer : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set echonetLiteVer success");
	}
	else
	{
		DLOG_Debug("Set echonetLiteVer fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}
void etest_edatUpdeShdDate(void)
{
	schar *exepRes = "20230510130105";
	schar testVal[EDAT_VAL_UPDESHDDATE_LEN];

	EDAT_ValIfComSteGetUpdeShdDate(testVal);
	DLOG_Debug("Get updeShdGetDate defval:%s", testVal);

	EDAT_ValIfComSteSetUpdeShdDate(exepRes);
	EDAT_ValIfComSteGetUpdeShdDate(testVal);
	DLOG_Debug("Get updeShdGetDate : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set updeShdGetDate success");
	}
	else
	{
		DLOG_Debug("Set updeShdGetDate fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatPortNum(void)
{
	schar *exepRes = "1234";
	schar testVal[EDAT_VAL_PWRHOUSEID_LEN];

	EDAT_ValIfNwsGetPortNum(testVal);
	DLOG_Debug("Get portNumber defval:%s", testVal);

	EDAT_ValIfNwsSetPortNum(exepRes);
	EDAT_ValIfNwsGetPortNum(testVal);
	DLOG_Debug("Get portNumber : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set portNumber success");
	}
	else
	{
		DLOG_Debug("Set portNumber fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatSellModeCharStartTime(void)
{
	schar *exepRes = "02:00";
	schar testVal[EDAT_VAL_CHARGTIME_LEN];

	EDAT_ValIfRunSetGetSellModeCharStartTime(testVal);
	DLOG_Debug("Get sellModeCharStartTime defval:%s", testVal);

	EDAT_ValIfRunSetSetSellModeCharStartTime(exepRes);
	EDAT_ValIfRunSetGetSellModeCharStartTime(testVal);
	DLOG_Debug("Get sellModeCharStartTime : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set sellModeCharStartTime success");
	}
	else
	{
		DLOG_Debug("Set sellModeCharStartTime fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatSellModeCharStopTime(void)
{
	schar *exepRes = "06:00";
	schar testVal[EDAT_VAL_CHARGTIME_LEN];

	EDAT_ValIfRunSetGetSellModeCharStopTime(testVal);
	DLOG_Debug("Get sellModeCharStopTime defval:%s", testVal);

	EDAT_ValIfRunSetSetSellModeCharStopTime(exepRes);
	EDAT_ValIfRunSetGetSellModeCharStopTime(testVal);
	DLOG_Debug("Get sellModeCharStopTime : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set sellModeCharStopTime success");
	}
	else
	{
		DLOG_Debug("Set sellModeCharStopTime fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatSellModeDisCStartTime(void)
{
	schar *exepRes = "07:00";
	schar testVal[EDAT_VAL_CHARGTIME_LEN];

	EDAT_ValIfRunSetGetSellModeDisCStartTime(testVal);
	DLOG_Debug("Get sellModeDisCStartTime defval:%s", testVal);

	EDAT_ValIfRunSetSetSellModeDisCStartTime(exepRes);
	EDAT_ValIfRunSetGetSellModeDisCStartTime(testVal);
	DLOG_Debug("Get sellModeDisCStartTime : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set sellModeDisCStartTime success");
	}
	else
	{
		DLOG_Debug("Set sellModeDisCStartTime fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatSellModeDiscStopTime(void)
{
	schar *exepRes = "01:00";
	schar testVal[EDAT_VAL_CHARGTIME_LEN];

	EDAT_ValIfRunSetGetSellModeDiscStopTime(testVal);
	DLOG_Debug("Get sellModeDisCStopTime defval:%s", testVal);

	EDAT_ValIfRunSetSetSellModeDiscStopTime(exepRes);
	EDAT_ValIfRunSetGetSellModeDiscStopTime(testVal);
	DLOG_Debug("Get sellModeDisCStopTime : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set sellModeDisCStopTime success");
	}
	else
	{
		DLOG_Debug("Set sellModeDisCStopTime fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatGenModeCharStartTime(void)
{
	schar *exepRes = "02:00";
	schar testVal[EDAT_VAL_CHARGTIME_LEN];

	EDAT_ValIfRunSetGetGenModeCharStartTime(testVal);
	DLOG_Debug("Get greenModeCharStartTime defval:%s", testVal);

	EDAT_ValIfRunSetSetGenModeCharStartTime(exepRes);
	EDAT_ValIfRunSetGetGenModeCharStartTime(testVal);
	DLOG_Debug("Get greenModeCharStartTime : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set greenModeCharStartTime success");
	}
	else
	{
		DLOG_Debug("Set greenModeCharStartTime fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatGenModeCharStopTime(void)
{
	schar *exepRes = "01:30";
	schar testVal[EDAT_VAL_CHARGTIME_LEN];

	EDAT_ValIfRunSetGetGenModeCharStopTime(testVal);
	DLOG_Debug("Get greenModeCharStopTime defval:%s", testVal);

	EDAT_ValIfRunSetSetGenModeCharStopTime(exepRes);
	EDAT_ValIfRunSetGetGenModeCharStopTime(testVal);
	DLOG_Debug("Get greenModeCharStopTime : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set greenModeCharStopTime success");
	}
	else
	{
		DLOG_Debug("Set greenModeCharStopTime fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatGenModeDisCStartTime(void)
{
	schar *exepRes = "02:02";
	schar testVal[EDAT_VAL_CHARGTIME_LEN];

	EDAT_ValIfRunSetGetGenModeDisCStartTime(testVal);
	DLOG_Debug("Get greenModeDisCStartTime defval:%s", testVal);

	EDAT_ValIfRunSetSetGenModeDisCStartTime(exepRes);
	EDAT_ValIfRunSetGetGenModeDisCStartTime(testVal);
	DLOG_Debug("Get greenModeDisCStartTime : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set greenModeDisCStartTime success");
	}
	else
	{
		DLOG_Debug("Set greenModeDisCStartTime fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatGenModeDiscStopTime(void)
{
	schar *exepRes = "01:00";
	schar testVal[EDAT_VAL_CHARGTIME_LEN];

	EDAT_ValIfRunSetGetGenModeDiscStopTime(testVal);
	DLOG_Debug("Get greenModeDisCStopTime defval:%s", testVal);

	EDAT_ValIfRunSetSetGenModeDiscStopTime(exepRes);
	EDAT_ValIfRunSetGetGenModeDiscStopTime(testVal);
	DLOG_Debug("Get greenModeDisCStopTime : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set greenModeDisCStopTime success");
	}
	else
	{
		DLOG_Debug("Set greenModeDisCStopTime fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatUpdeTime(void)
{
	schar *exepRes = "01:00";
	schar testVal[EDAT_VAL_SETUP_TIME_LEN];

	EDAT_ValIfFwUpdeGetUpdeTime(testVal);
	DLOG_Debug("Get updTime defval:%s", testVal);

	EDAT_ValIfFwUpdeSetUpdeTime(exepRes);
	EDAT_ValIfFwUpdeGetUpdeTime(testVal);
	DLOG_Debug("Get updTime : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set updTime success");
	}
	else
	{
		DLOG_Debug("Set updTime fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatMentDate(void)
{
	schar *exepRes = "230505";
	schar testVal[EDAT_VAL_DATE_LEN];

	EDAT_ValIfMaintGetMentDate(testVal);
	DLOG_Debug("Get mentanceDate defval:%s", testVal);

	EDAT_ValIfMaintSetMentDate(exepRes);
	EDAT_ValIfMaintGetMentDate(testVal);
	DLOG_Debug("Get mentanceDate : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set mentanceDate success");
	}
	else
	{
		DLOG_Debug("Set mentanceDate fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatImpDesDay1(void)
{
	schar *exepRes = "230505";
	schar testVal[EDAT_VAL_PWRHOUSEID_LEN];

	EDAT_ValIfMaintGetImpDesDay1(testVal);
	DLOG_Debug("Get impltationDesignDay1 defval:%s", testVal);

	EDAT_ValIfMaintSetImpDesDay1(exepRes);
	EDAT_ValIfMaintGetImpDesDay1(testVal);
	DLOG_Debug("Get impltationDesignDay1 : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set impltationDesignDay1 success");
	}
	else
	{
		DLOG_Debug("Set impltationDesignDay1 fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatImpDesDay2(void)
{
	schar *exepRes = "230505";
	schar testVal[EDAT_VAL_PWRHOUSEID_LEN];

	EDAT_ValIfMaintGetImpDesDay2(testVal);
	DLOG_Debug("Get impltationDesignDay2 defval:%s", testVal);

	EDAT_ValIfMaintSetImpDesDay2(exepRes);
	EDAT_ValIfMaintGetImpDesDay2(testVal);
	DLOG_Debug("Get impltationDesignDay2 : %s", testVal);

	if(0 == strcmp(exepRes, testVal))
	{
		DLOG_Debug("Set impltationDesignDay2 success");
	}
	else
	{
		DLOG_Debug("Set impltationDesignDay2 fail exepRes : %s testVal : %s", exepRes, testVal);
	}
}

void etest_edatCrtCurFile(slong argc, schar *agrv[])
{
	schar *strExepRes = "192.168.1.100";
	uchar charExepRes = 10;
	ushort ushortExepRes = 1000;
	ushort floatExepRes = 9999;

	schar strTestVal[EDAT_VAL_IPADDRESS_LEN];
	uchar charTestVal;
	ushort ushortTestVal;
	ushort floatTestVal;

	EDAT_ValIfNwsSetIpAddress(strExepRes);
	EDAT_ValIfNwsGetIpAddress(strTestVal);

	EDAT_ValIfMhcInfSetSunPweSetLctn(charExepRes);
	charTestVal = EDAT_ValIfMhcInfGetSunPweSetLctn();

	EDAT_ValIfPvSetSunPwrRated(ushortExepRes);
	ushortTestVal = EDAT_ValIfPvGetSunPwrRated();

	EDAT_ValIfSetValSetEptAppCap(floatExepRes);
	floatTestVal = EDAT_ValIfSetValGetEptAppCap();

	if ((0 == strcmp(strExepRes, strTestVal)) && (charExepRes == charTestVal) &&
		(ushortExepRes == ushortTestVal) && (floatExepRes == floatTestVal))
	{
		DLOG_Debug("Create Current File success");
	}
	else
	{
		DLOG_Debug("Create Current File faile");
		DLOG_Debug("strExepRes:%s, strTestVal%s", strExepRes, strTestVal);
		DLOG_Debug("charExepRes:%d, charTestVal%d", charExepRes, charTestVal);
		DLOG_Debug("ushortExepRes:%d, ushortTestVal%d", ushortExepRes, ushortTestVal);
		DLOG_Debug("floatExepRes:%d, floatTestVal%d", floatExepRes, floatTestVal);
	}
}


void etest_edatGetAndSet(slong argc, schar *agrv[])
{
	/* Test exterSunPwrOrnot */
	etest_edatExtSunPwrOrnot();

	/* Test sunBeyondPwrGenratDevice */
	etest_edatSunBydPwrGenDev();

	/* Test loadConnectType */
	etest_edatLoadConnType();

	/* Test pushUpSet */
	etest_edatPushUpSet();

	/* Test outputContrObject */
	etest_edatOutputConObj();

	/* Test eqpmtApproveCap */
	etest_edatEptAppCap();

	/* Test genPwrUpperLimit */
	etest_edatGenPwrUpLmt();

	/* Test clipAction */
	etest_edatCliAction();

	/* Test fitContractType */
	etest_edatFitCtatType();

	/* Test selfConsumeType */
	etest_edatSelfCseType();

	/* Test pwrHouseId */
	etest_edatPwrHouseId();

	/* Test pwrCompanyServer */
	etest_edatPwrCompServer();

	/* Test fixShdStartDate */
	etest_edatFixShdSrtDate();

	/* Test sunPwrRated */
	etest_edatSunPwrRated();

	/* Test pvPanelRating */
	etest_edatPvPanelRating();

	/* Test sysModelName */
	etest_edatSysMdlName();

	/* Test commModelName */
	etest_edatCommMdlName();

	/* Test commSerialNum */
	etest_edatCommSerialNum();

	/* Test commBoardFwVer */
	etest_edatCommBoardFwVer();

	/* Test opRemoteCtlFw */
	etest_edatOpRmtCtrlFw();

	/* Test sunPwrSetLocation */
	etest_edatSunPwrSetLctn();

	/* Test btSetLocation */
	etest_edatBtSetLctn();

	/* Test v2hStandSetLocation */
	etest_edatV2hStdSetLctn();

	/* Test electWaterHeating */
	etest_edatElectWaterHeat();

	/* Test postalNum */
	etest_edatPostNum();

	/* Test echonetLiteVer */
	etest_edatEchonetLite();

	/* Test opRemoteControlOrnot */
	etest_edatOpRmtCtlOrnot();

	/* Test btCharPwrMaxValueSet */
	etest_edatBtCharPwrMaxValSet();

	/* Test carCharPwrValueSet */
	etest_edatCarCharPwrValSet();

	/* Test carType */
	etest_edatCarType();

	/* Test carChargLimit */
	etest_edatCarCharLmt();

	/* Test ipAddrSet */
	etest_edatIpAddrSet();

	/* Test ipAddress */
	etest_edatIpAddress();

	/* Test subnetMask */
	etest_edatSubMask();

	/* Test defaultGateway */
	etest_edatDftGateway();

	/* Test poiorityDns */
	etest_edatPtyDns();

	/* Test replaceDns */
	etest_edatRleDns();

	/* Test proxySet */
	etest_edatProxySet();

	/* Test proxyAddress */
	etest_edatPxyAddress();

	/* Test portNumber */
	etest_edatPortNum();

	/* Test updeShdGetDate */
	etest_edatUpdeShdDate();

	/* Test sellModeCharStartTime */
	etest_edatSellModeCharStartTime();

	/* Test sellModeCharStopTime */
	etest_edatSellModeCharStopTime();

	/* Test sellModeDisCStartTime */
	etest_edatSellModeDisCStartTime();

	/* Test sellModeDisCStopTime */
	etest_edatSellModeDiscStopTime();

	/* Test greenModeCharStartTime */
	etest_edatGenModeCharStartTime();

	/* Test greenModeCharStopTime */
	etest_edatGenModeCharStopTime();

	/* Test greenModeDisCStartTime */
	etest_edatGenModeDisCStartTime();

	/* Test greenModeDisCStopTime */
	etest_edatGenModeDiscStopTime();

	/* Test btChargeStyle */
	etest_edatBtCharStyle();

	/* Test btDischargeStyle */
	etest_edatBtDisCStyle();

	/* Test v2hChargeStyle */
	etest_edatV2hCharStyle();

	/* Test v2hDischargeStyle */
	etest_edatV2hDisCStyle();

	/* Test btChargePwrSetValue */
	etest_edatBtCharPwrSetVal();

	/* Test btDiscPwrSetValue */
	etest_edatBtDisCPweSetVal();

	/* Test v2hChargePwrSetValue */
	etest_edatV2hCharPwrSetVal();

	/* Test v2hDischargePwrSetValue */
	etest_edatV2hDisCPweSetVal();

	/* Test btACChargeQuatySetValue */
	etest_edatBtAcCharQtySetVal();

	/* Test btACDiscQuatySetValue */
	etest_edatBtAcDisCQtySetVal();

	/* Test btDCChargeQuatySetValue */
	etest_edatBtDcCharQtySetVal();

	/* Test btDCDiscQuatySetValue */
	etest_edatBtDcDiscQtySetVal();

	/* Test v2hACChargeQuatySetValue */
	etest_edatV2hDcCharQtySetVal();

	/* Test v2hACDiscQuatySetValue */
	etest_edatV2hDcDiscQtySetVal();

	/* Test actLogCollectionCycle */
	etest_edatActLogCollCyc();

	/* Test updateLed */
	etest_edatUpdeLed();

	/* Test updeNo */
	etest_edatUpdeNo();

	/* Test analyRes */
	etest_edatAnalyRes();

	/* Test updTime */
	etest_edatUpdeTime();

	/* Test boardTgt */
	etest_edatBoardTgt();

	/* Test invTgt */
	etest_edatInvTgt();

	/* Test pvTgt */
	etest_edatPvTgt();

	/* Test bdcTgt */
	etest_edatBdcTgt();

	/* Test jbTgt */
	etest_edatJbTgt();

	/* Test mentanceDate */
	etest_edatMentDate();

	/* Test impltationDesignDay1 */
	etest_edatImpDesDay1();

	/* Test impltationDesignDay2 */
	etest_edatImpDesDay2();

	/* Test executeMode */
	etest_edatExecuteMode();

	/* Test maintainResult */
	etest_edatMaintRes();

	/* Test commBoardManageMode */
	etest_edatCommBoardMgeMode();

	/* Test emergencyMode */
	etest_edatEmergencyMode();

	/* Test btUpperLimit */
	etest_edatBtUpLmt();

	/* Test btLowerLimit */
	etest_edatbtLowLmt();

	/* Test carCharordiscBodyOprat */
	etest_edatCarCharBodyOprat();

	/* Test pwrcutDischar */
	etest_edatPwrcutDischar();

	/* Test pwrcutDiscPrioritySet */
	etest_edatPwrcutDiscPrySet();

	/* Test pwrcutCharPrioritySet */
	etest_edatPwrcutCharPrySet();

	/* Test carDiscLowerLimitSet */
	etest_edatCarDiscLowLmtSet();

	/* Test carCharUpperLimitSet */
	etest_edatCarCharUpLmtSet();

	/* Test carDiscOrnot */
	etest_edatCarDiscOrnot();

	/* Test discharPrioritySet */
	etest_edatDiscPrySet();

	/* Test pwrcutDiscLowerLimitSet */
	etest_edatPwrDiscLowLmtSet();

	/* Test chargePrioritySet */
	etest_edatCharPrySet();

	/* Test contaractPwrSet */
	etest_edatContPwrSet();

	/* Test pwrcutOutSet */
	etest_edatPwrcutOutSet();

	/* Test pwrcutOutStart */
	etest_edatPwrcutOutStart();

	/* Test sellValidOrInvalid */
	etest_edatSellValOrInval();

	/* Test sysVoltDetectLevel */
	etest_edatSysVolDetLvl();

	/* Test sysVoltDetectTime */
	etest_edatSysVolDetTime();

	/* Test sysUnderVoltDetectLevel */
	etest_edatSysUdrVolDetLvl();

	/* Test sysUnderVoltDetectTime */
	etest_edatSysUdrVolDetTime();

	/* Test freRiseDetectLevel */
	etest_edatFreRiseDetLvl();

	/* Test freRiseDetectTime */
	etest_edatFreRiseDetTime();

	/* Test freDropDetectLevel */
	etest_edatFreDropDetLvl();

	/* Test freDropDetectTime */
	etest_edatFreDropDetTime();

	/* Test resctTime */
	etest_edatResctTime();

	/* Test volRsInhbValue */
	etest_edatVolRsInhbVal();

	/* Test volRsInhbattLimit */
	etest_edatVolRsInhbLmt();

	/* Test cotPowerControl */
	etest_edatCotPwrCtl();

	/* Test passIndOperDetect */
	etest_edatPassIndOperDet();

	/* Test freFeedbackGain */
	etest_edatFreFfbkGain();

	/* Test freLevAllow */
	etest_edatFreLevAllow();

	/* Test changeTimeIncorDes */
	etest_edatChgTimeIncDes();
}
/* End Of File */
