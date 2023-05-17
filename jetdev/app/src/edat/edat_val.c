/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Xu.Y												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/05/05												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <unistd.h>
#include <string.h>

#include "edat_val.h"
#include "edat_file_g.h"
#include "edat_g.h"
#include "dlog_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
EdatValConstitute_t		g_edat_Constitute;
EdatValSetVal_t			g_edat_SetVal;
EdatValPV_t				g_edat_Pv;
EdatValMachineInf_t		g_edat_MachineInf;
EdatValSetInf_t			g_edat_SetInf;
EdatValNetWorkSet_t		g_edat_NetWorkSet;
EdatValCommSte_t		g_edat_CommSte;
EdatValRunSet_t			g_edat_RunSet;
EdatValEnl_t			g_edat_Enl;
EdatValMonitor_t		g_edat_Monitor;
EdatValFWUpdate_t		g_edat_FwUpde;
EdatValMaintenance_t	g_edat_Maint;
EdatValCmd65_t			g_edat_Cmd65;
EdatValCmd75_t			g_edat_Cmd75;
EdatValCmd6b_t			g_edat_Cmd6b;
EdatValSetting_t		g_edat_Setting;

pthread_mutex_t g_edatMutexSet;
/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
const TableArr_t s_edat_GetArray[EDAT_VAL_ARRAY_LEN] = {
	edat_ValGetConst,
	edat_ValGetSetVal,
	edat_ValGetPv,
	edat_ValGetMachInf,
	edat_ValGetSetInf,
	edat_ValGetNetWorkSet,
	edat_ValGetCommSte,
	edat_ValGetRunSet,
	edat_ValGetEnl,
	edat_ValGetMonitor,
	edat_ValGetFwUpde,
	edat_ValGetMaint,
	edat_ValGetCmd65,
	edat_ValGetCmd75,
	edat_ValGetCmd6b,
	edat_ValGetSetting
};

const TableArr_t s_edat_SetArray[EDAT_VAL_ARRAY_LEN] = {
	edat_ValSetConst,
	edat_ValSetSetVal,
	edat_ValSetPv,
	edat_ValSetMachInf,
	edat_ValSetSetInf,
	edat_ValSetNetWorkSet,
	edat_ValSetCommSte,
	edat_ValSetRunSet,
	edat_ValSetEnl,
	edat_ValSetMonitor,
	edat_ValSetFwUpde,
	edat_ValSetMaint,
	edat_ValSetCmd65,
	edat_ValSetCmd75,
	edat_ValSetCmd6b,
	edat_ValSetSetting
};
/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:	EDAT数据初始化										*/
/*--------------------------------------------------------------------------*/
/*	param			:	void												*/
/*--------------------------------------------------------------------------*/
/*	return			:	EDAT_VAL_RESULT_OK				处理成功			*/
/*					:	EDAT_VAL_RESULT_NG				处理失败			*/
/*--------------------------------------------------------------------------*/
/*	detail			:	EDAT数据初始化										*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
slong EDAT_ValInit(void)
{
	slong result = EDAT_VAL_RESULT_OK;
	slong ret = 0;
	FILE *fp = NULL;
	schar curFilePathName[EDAT_VAL_FILE_PATH_NAME_LEN];
	schar defFilePathName[EDAT_VAL_FILE_PATH_NAME_LEN];

	DEBUG_TRCIN();

	memset(curFilePathName, 0, EDAT_VAL_FILE_PATH_NAME_LEN);
	memset(defFilePathName, 0, EDAT_VAL_FILE_PATH_NAME_LEN);

	snprintf(curFilePathName, sizeof(curFilePathName), "%s/%s",
		EDAT_VAL_CUR_FILE_PATH, EDAT_VAL_CUR_FILE_NAME);

	snprintf(defFilePathName, sizeof(defFilePathName), "%s/%s",
		EDAT_VAL_DEF_FILE_PATH, EDAT_VAL_DEF_FILE_NAME);

	pthread_mutex_init(&g_edatMutexSet, NULL);

	ret = access(curFilePathName, F_OK);
	if (ret)
	{
#if 0
		ret = access(defFilePathName, F_OK);
		if (!ret)
		{
			EDAT_ValFileReset();
		}
		else
		{
			/* TODO:默认文件不存在处理  */
			printf("file (%s) is not exists!", defFilePathName);
			result = EDAT_VAL_RESULT_NG;
		}
#else
		EDAT_ValFileReset();
#endif
	}
	else
	{
		/* file exists */
		fp = fopen(curFilePathName, "r");
		if (NULL == fp)
		{
			printf("open (%s) failed!", curFilePathName);
			result = EDAT_VAL_RESULT_NG;
		}
		else
		{
			edat_ValGetAll(fp);
			fclose(fp);
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	重置当前dat配置文件									*/
/*--------------------------------------------------------------------------*/
/*	param			:	void												*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	重置当前dat配置文件									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
void EDAT_ValFileReset(void)
{
	FILE *fp = NULL;
	schar curFilePathName[EDAT_VAL_FILE_PATH_NAME_LEN];
	schar defFilePathName[EDAT_VAL_FILE_PATH_NAME_LEN];

	DEBUG_TRCIN();

	memset(curFilePathName, 0, EDAT_VAL_FILE_PATH_NAME_LEN);
	memset(defFilePathName, 0, EDAT_VAL_FILE_PATH_NAME_LEN);

	snprintf(curFilePathName, sizeof(curFilePathName), "%s/%s",
		EDAT_VAL_CUR_FILE_PATH, EDAT_VAL_CUR_FILE_NAME);

	snprintf(defFilePathName, sizeof(defFilePathName), "%s/%s",
		EDAT_VAL_DEF_FILE_PATH, EDAT_VAL_DEF_FILE_NAME);

	EDAT_IniFileCopy(curFilePathName, defFilePathName);

	fp = fopen(curFilePathName, "r");
	if (NULL == fp)
	{
		printf("open (%s) failed!", curFilePathName);
	}
	else
	{
		edat_ValGetAll(fp);
		fclose(fp);
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	将内存数据写入到文件								*/
/*--------------------------------------------------------------------------*/
/*	param			:	void												*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将内存数据写入到文件								*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
void EDAT_ValSetAll(void)
{
	FILE *fp = NULL;
	slong loopCnt = 0;
	schar curFilePathName[EDAT_VAL_FILE_PATH_NAME_LEN];

	DEBUG_TRCIN();

	memset(curFilePathName, 0, EDAT_VAL_FILE_PATH_NAME_LEN);

	snprintf(curFilePathName, sizeof(curFilePathName), "%s/%s",
		EDAT_VAL_CUR_FILE_PATH, EDAT_VAL_CUR_FILE_NAME);

	fp = fopen(curFilePathName, "w");
	if (NULL == fp)
	{
		printf("open (%s) faied", curFilePathName);
	}
	else
	{
		for (loopCnt = 0; loopCnt < EDAT_VAL_ARRAY_LEN; loopCnt++)
		{
			s_edat_SetArray[loopCnt](fp);
		}
		fflush(fp);
		fsync(fileno(fp));
		fclose(fp);
	}

	DEBUG_TRCOUT();
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:	读取文件数据到内存									*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	读取文件数据到内存									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValGetAll(FILE *fp)
{
	slong loopCnt	= 0;

	DEBUG_TRCIN();

	for (loopCnt = 0; loopCnt < EDAT_VAL_ARRAY_LEN; loopCnt++)
	{
		s_edat_GetArray[loopCnt](fp);
		/* fseek(fp, 0, SEEK_SET); */
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	从文件中读取 g_edat_Constitute数据					*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	从文件中读取 g_edat_Constitute数据					*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValGetConst(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_GetIniCharValue(fp, CONST_EXTERSUNPWRORNOT_KEY,
		&g_edat_Constitute.exterSunPwrOrnot, CONST_EXTERSUNPWRORNOT_DEFVAL);

	EDAT_GetIniCharValue(fp,CONST_SUNBYDPWRGENDEV_KEY,
		&g_edat_Constitute.sunBeyondPwrGenratDevice,
		CONST_SUNBYDPWRGENDEV_DEFVAL);

	EDAT_GetIniCharValue(fp, CONST_LOADCONNTYPE_KEY,
		&g_edat_Constitute.loadConnectType, CONST_LOADCONNTYPE_DEFVAL);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	将g_edat_Constitute数据写入到文件中					*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将g_edat_Constitute数据写入到文件中					*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValSetConst(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_SetIniCharValue(fp, CONST_EXTERSUNPWRORNOT_KEY,
		g_edat_Constitute.exterSunPwrOrnot);

	EDAT_SetIniCharValue(fp, CONST_SUNBYDPWRGENDEV_KEY,
		g_edat_Constitute.sunBeyondPwrGenratDevice);

	EDAT_SetIniCharValue(fp, CONST_LOADCONNTYPE_KEY,
		g_edat_Constitute.loadConnectType);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	从文件中读取 g_edat_SetVal数据						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	从文件中读取 g_edat_SetVal数据						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValGetSetVal(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_GetIniCharValue(fp, SETVAL_PUSHUPSET_KEY,
		&g_edat_SetVal.pushUpSet, SETVAL_PUSHUPSET_DEFVAL);

	EDAT_GetIniCharValue(fp, SETVAL_OUTPUTCTROBJ_KEY,
		&g_edat_SetVal.outputContrObject, SETVAL_OUTPUTCTROBJ_DEFVAL);

	EDAT_GetIniFloatValue(fp, SETVAL_EQPMTAPPROVECAP_KEY,
		&g_edat_SetVal.eqpmtApproveCap, SETVAL_EQPMTAPPROVECAP_DEFVAL);

	EDAT_GetIniFloatValue(fp, SETVAL_GENPWRUPLMT_KEY,
		&g_edat_SetVal.genPwrUpperLimit, SETVAL_GENPWRUPLMT_DEFVAL);
	
	EDAT_GetIniCharValue(fp, SETVAL_CLIPACTION_KEY,
		&g_edat_SetVal.clipAction, SETVAL_CLIPACTION_DEFVAL);
	
	EDAT_GetIniCharValue(fp, SETVAL_FITCONTTYPE_KEY,
		&g_edat_SetVal.fitContractType, SETVAL_FITCONTTYPE_DEFVAL);

	EDAT_GetIniCharValue(fp, SETVAL_SELFCONSTYPE_KEY,
		&g_edat_SetVal.selfConsumeType, SETVAL_SELFCONSTYPE_DEFVAL);

	EDAT_GetIniStringValue(fp, SETVAL_PWEHOUSEID_KEY,
		g_edat_SetVal.pwrHouseId, SETVAL_PWEHOUSEID_DEFVAL);

	EDAT_GetIniCharValue(fp, SETVAL_PWRCOMPSERVER_KEY,
		&g_edat_SetVal.pwrCompanyServer, SETVAL_PWRCOMPSERVER_DEFVAL);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	将g_edat_SetVal数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将g_edat_SetVal数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValSetSetVal(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_SetIniCharValue(fp, SETVAL_PUSHUPSET_KEY,
		g_edat_SetVal.pushUpSet);

	EDAT_SetIniCharValue(fp, SETVAL_OUTPUTCTROBJ_KEY,
		g_edat_SetVal.outputContrObject);

	EDAT_SetIniFloatValue(fp, SETVAL_EQPMTAPPROVECAP_KEY,
		g_edat_SetVal.eqpmtApproveCap);

	EDAT_SetIniFloatValue(fp, SETVAL_GENPWRUPLMT_KEY,
		g_edat_SetVal.genPwrUpperLimit);

	EDAT_SetIniCharValue(fp, SETVAL_CLIPACTION_KEY,
		g_edat_SetVal.clipAction);

	EDAT_SetIniCharValue(fp, SETVAL_FITCONTTYPE_KEY,
		g_edat_SetVal.fitContractType);

	EDAT_SetIniCharValue(fp, SETVAL_SELFCONSTYPE_KEY,
		g_edat_SetVal.selfConsumeType);

	EDAT_SetIniStringValue(fp, SETVAL_PWEHOUSEID_KEY,
		g_edat_SetVal.pwrHouseId);

	EDAT_SetIniCharValue(fp, SETVAL_PWRCOMPSERVER_KEY,
		g_edat_SetVal.pwrCompanyServer);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	从文件中读取 g_edat_Pv数据							*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	从文件中读取 g_edat_Pv数据							*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValGetPv(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_GetIniStringValue(fp, PV_FISSHDSTARTDATE_KEY,
		g_edat_Pv.fixShdStartDate, PV_FISSHDSTARTDATE_DEFVAL);
	
	EDAT_GetIniFloatValue(fp, PV_SUNPWRRATED_KEY,
		&g_edat_Pv.sunPwrRated, PV_SUNPWRRATED_DEFVLA);

	EDAT_GetIniFloatValue(fp, PV_PVPANELRATING_KEY,
		&g_edat_Pv.pvPanelRating, PV_PVPANELRATING_DEFVLA);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	将g_edat_Pv数据写入到文件中							*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将g_edat_Pv数据写入到文件中							*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/06 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValSetPv(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_SetIniStringValue(fp, PV_FISSHDSTARTDATE_KEY,
		g_edat_Pv.fixShdStartDate);
	
	EDAT_SetIniFloatValue(fp, PV_SUNPWRRATED_KEY,
		g_edat_Pv.sunPwrRated);

	EDAT_SetIniFloatValue(fp, PV_PVPANELRATING_KEY,
		g_edat_Pv.pvPanelRating);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	从文件中读取 g_edat_MachineInf数据					*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	从文件中读取 g_edat_MachineInf数据					*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValGetMachInf(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_GetIniStringValue(fp, MACHINEINF_SYSMODELNAME_KEY,
		g_edat_MachineInf.sysModelName,
		MACHINEINF_SYSMODELNAME_DEFVAL);

	EDAT_GetIniStringValue(fp, MACHINEINF_COMMMODELNAME_KEY,
		g_edat_MachineInf.commModelName,
		MACHINEINF_COMMMODELNAME_DEFVAL);

	EDAT_GetIniStringValue(fp, MACHINEINF_COMMSERIALNUM_KEY,
		g_edat_MachineInf.commSerialNum,
		MACHINEINF_COMMSERIALNUM_DEFVAL);

	EDAT_GetIniStringValue(fp, MACHINEINF_COMMBOARDFWVER_KEY,
		g_edat_MachineInf.commBoardFwVer,
		MACHINEINF_COMMBOARDFWVER_DEFVAL);

	EDAT_GetIniStringValue(fp, MACHINEINF_OPREMOTECTLFW_KEY,
		g_edat_MachineInf.opRemoteCtlFw,
		MACHINEINF_OPREMOTECTLFW_DEFVAL);

	EDAT_GetIniCharValue(fp, MACHINEINF_SUNPWRSETLOCATION_KEY,
		&g_edat_MachineInf.sunPwrSetLocation,
		MACHINEINF_SUNPWRSETLOCATION_DEFVAL);

	EDAT_GetIniCharValue(fp, MACHINEINF_BTSETLOCATION_KEY,
		&g_edat_MachineInf.btSetLocation,
		MACHINEINF_BTSETLOCATION_DEFVAL);

	EDAT_GetIniCharValue(fp, MACHINEINF_V2HSTANDSETLOCATION_KEY,
		&g_edat_MachineInf.v2hStandSetLocation,
		MACHINEINF_V2HSTANDSETLOCATION_DEFVAL);

	EDAT_GetIniCharValue(fp, MACHINEINF_ELECTWATERHEATING_KEY,
		&g_edat_MachineInf.electWaterHeating,
		MACHINEINF_ELECTWATERHEATING_DEFVAL);

	EDAT_GetIniStringValue(fp, MACHINEINF_POSTALNUM_KEY,
		g_edat_MachineInf.postalNum,
		MACHINEINF_POSTALNUM_DEFVAL);

	EDAT_GetIniStringValue(fp, MACHINEINF_ECHONETLITEVER_KEY,
		g_edat_MachineInf.echonetLite,
		MACHINEINF_ECHONETLITEVER_DEFVAL);

	EDAT_GetIniCharValue(fp, MACHINEINF_OPREMOTECONTROLORNOT_KEY,
		&g_edat_MachineInf.opRemoteControlOrnot,
		MACHINEINF_OPREMOTECONTROLORNOT_DEFVAL);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	将g_edat_MachineInf数据写入到文件中					*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将g_edat_MachineInf数据写入到文件中					*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/06 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValSetMachInf(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_SetIniStringValue(fp, MACHINEINF_SYSMODELNAME_KEY,
		g_edat_MachineInf.sysModelName);

	EDAT_SetIniStringValue(fp, MACHINEINF_COMMMODELNAME_KEY,
		g_edat_MachineInf.commModelName);

	EDAT_SetIniStringValue(fp, MACHINEINF_COMMSERIALNUM_KEY,
		g_edat_MachineInf.commSerialNum);

	EDAT_SetIniStringValue(fp, MACHINEINF_COMMBOARDFWVER_KEY,
		g_edat_MachineInf.commBoardFwVer);

	EDAT_SetIniStringValue(fp, MACHINEINF_OPREMOTECTLFW_KEY,
		g_edat_MachineInf.opRemoteCtlFw);

	EDAT_SetIniCharValue(fp, MACHINEINF_SUNPWRSETLOCATION_KEY,
		g_edat_MachineInf.sunPwrSetLocation);

	EDAT_SetIniCharValue(fp, MACHINEINF_BTSETLOCATION_KEY,
		g_edat_MachineInf.btSetLocation);

	EDAT_SetIniCharValue(fp, MACHINEINF_V2HSTANDSETLOCATION_KEY,
		g_edat_MachineInf.v2hStandSetLocation);

	EDAT_SetIniCharValue(fp, MACHINEINF_ELECTWATERHEATING_KEY,
		g_edat_MachineInf.electWaterHeating);

	EDAT_SetIniStringValue(fp, MACHINEINF_POSTALNUM_KEY,
		g_edat_MachineInf.postalNum);

	EDAT_SetIniStringValue(fp, MACHINEINF_ECHONETLITEVER_KEY,
		g_edat_MachineInf.echonetLite);

	EDAT_SetIniCharValue(fp, MACHINEINF_OPREMOTECONTROLORNOT_KEY,
		g_edat_MachineInf.opRemoteControlOrnot);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	从文件中读取 g_edat_SetInf数据						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	从文件中读取 g_edat_SetInf数据						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValGetSetInf(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_GetIniFloatValue(fp, SETINF_BTCHARPWRMAXVALSET_KEY,
		&g_edat_SetInf.btCharPwrMaxValueSet,
		SETINF_BTCHARPWRMAXVALSET_DEFVAL);

	EDAT_GetIniFloatValue(fp, SETINF_CARCHARPWRVALSET_KEY,
		&g_edat_SetInf.carCharPwrValueSet,
		SETINF_CARCHARPWRVALSET_DEFVAL);

	EDAT_GetIniCharValue(fp, SETINF_CARTYPE_KEY,
		&g_edat_SetInf.carType,
		SETINF_CARTYPE_DEFVAL);

	EDAT_GetIniCharValue(fp, SETINF_CARCHARGLIMIT_KEY,
		&g_edat_SetInf.carChargLimit,
		SETINF_CARCHARGLIMIT_DEFVAL);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	将g_edat_SetInf数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将g_edat_SetInf数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/06 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValSetSetInf(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_SetIniFloatValue(fp, SETINF_BTCHARPWRMAXVALSET_KEY,
		g_edat_SetInf.btCharPwrMaxValueSet);

	EDAT_SetIniFloatValue(fp, SETINF_CARCHARPWRVALSET_KEY,
		g_edat_SetInf.carCharPwrValueSet);

	EDAT_SetIniCharValue(fp, SETINF_CARTYPE_KEY,
		g_edat_SetInf.carType);

	EDAT_SetIniCharValue(fp, SETINF_CARCHARGLIMIT_KEY,
		g_edat_SetInf.carChargLimit);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	从文件中读取 g_edat_NetWorkSet数据					*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	从文件中读取 g_edat_NetWorkSet数据					*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValGetNetWorkSet(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_GetIniCharValue(fp, NETWORKSET_IPADDRSET_KEY,
		&g_edat_NetWorkSet.ipAddrSet,
		NETWORKSET_IPADDRSET_DEFVAL);

	EDAT_GetIniStringValue(fp, NETWORKSET_IPADDRESS_KEY,
		g_edat_NetWorkSet.ipAddress,
		NETWORKSET_IPADDRESS_DEFVAL);

	EDAT_GetIniStringValue(fp, NETWORKSET_SUBNETMASK_KEY,
		g_edat_NetWorkSet.subnetMask,
		NETWORKSET_SUBNETMASK_DEFVAL);

	EDAT_GetIniStringValue(fp, NETWORKSET_DEFAULTGATEWAY_KEY,
		g_edat_NetWorkSet.defaultGateway,
		NETWORKSET_DEFAULTGATEWAY_DEFVAL);

	EDAT_GetIniStringValue(fp, NETWORKSET_POIORITYDNS_KEY,
		g_edat_NetWorkSet.poiorityDns,
		NETWORKSET_POIORITYDNS_DEFVAL);

	EDAT_GetIniStringValue(fp, NETWORKSET_REPLACEDNS_KEY,
		g_edat_NetWorkSet.replaceDns,
		NETWORKSET_REPLACEDNS_DEFVAL);

	EDAT_GetIniCharValue(fp, NETWORKSET_PROXYSET_KEY,
		&g_edat_NetWorkSet.proxySet,
		NETWORKSET_PROXYSET_DEFVAL);

	EDAT_GetIniStringValue(fp, NETWORKSET_PROXYADDRESS_KEY,
		g_edat_NetWorkSet.proxyAddress,
		NETWORKSET_PROXYADDRESS_DEFVAL);

	EDAT_GetIniStringValue(fp, NETWORKSET_PORTNUMBER_KEY,
		g_edat_NetWorkSet.portNumber,
		NETWORKSET_PORTNUMBER_DEFVAL);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	将g_edat_NetWorkSet数据写入到文件中					*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将g_edat_NetWorkSet数据写入到文件中					*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/06 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValSetNetWorkSet(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_SetIniCharValue(fp, NETWORKSET_IPADDRSET_KEY,
		g_edat_NetWorkSet.ipAddrSet);

	EDAT_SetIniStringValue(fp, NETWORKSET_IPADDRESS_KEY,
		g_edat_NetWorkSet.ipAddress);

	EDAT_SetIniStringValue(fp, NETWORKSET_SUBNETMASK_KEY,
		g_edat_NetWorkSet.subnetMask);

	EDAT_SetIniStringValue(fp, NETWORKSET_DEFAULTGATEWAY_KEY,
		g_edat_NetWorkSet.defaultGateway);

	EDAT_SetIniStringValue(fp, NETWORKSET_POIORITYDNS_KEY,
		g_edat_NetWorkSet.poiorityDns);

	EDAT_SetIniStringValue(fp, NETWORKSET_REPLACEDNS_KEY,
		g_edat_NetWorkSet.replaceDns);

	EDAT_SetIniCharValue(fp, NETWORKSET_PROXYSET_KEY,
		g_edat_NetWorkSet.proxySet);

	EDAT_SetIniStringValue(fp, NETWORKSET_PROXYADDRESS_KEY,
		g_edat_NetWorkSet.proxyAddress);

	EDAT_SetIniStringValue(fp, NETWORKSET_PORTNUMBER_KEY,
		g_edat_NetWorkSet.portNumber);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	从文件中读取 g_edat_CommSte数据						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	从文件中读取 g_edat_CommSte数据						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValGetCommSte(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_GetIniStringValue(fp, COMMSTE_UPDESHDGETDATE_KEY,
		g_edat_CommSte.updeShdGetDate,
		COMMSTE_UPDESHDGETDATE_DEFVAL);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	将g_edat_CommSte数据写入到文件中					*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将g_edat_CommSte数据写入到文件中					*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/06 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValSetCommSte(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_SetIniStringValue(fp, COMMSTE_UPDESHDGETDATE_KEY,
		g_edat_CommSte.updeShdGetDate);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	从文件中读取 g_edat_RunSet数据						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	从文件中读取 g_edat_RunSet数据						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValGetRunSet(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_GetIniStringValue(fp, RUNSET_SELLMODECHARSTARTTIME_KEY,
		g_edat_RunSet.sellModeCharStartTime,
		RUNSET_SELLMODECHARSTARTTIME_DEFVAL);

	EDAT_GetIniStringValue(fp, RUNSET_SELLMODECHARSTOPTIME_KEY,
		g_edat_RunSet.sellModeCharStopTime,
		RUNSET_SELLMODECHARSTOPTIME_DEFVAL);

	EDAT_GetIniStringValue(fp, RUNSET_SELLMODEDISCSTARTTIME_KEY,
		g_edat_RunSet.sellModeDisCStartTime,
		RUNSET_SELLMODEDISCSTARTTIME_DEFVAL);

	EDAT_GetIniStringValue(fp, RUNSET_SELLMODEDISCSTOPTIME_KEY,
		g_edat_RunSet.sellModeDisCStopTime,
		RUNSET_SELLMODEDISCSTOPTIME_DEFVAL);

	EDAT_GetIniStringValue(fp, RUNSET_GREENMODECHARSTARTTIME_KEY,
		g_edat_RunSet.greenModeCharStartTime,
		RUNSET_GREENMODECHARSTARTTIME_DEFVAL);

	EDAT_GetIniStringValue(fp, RUNSET_GREENMODECHARSTOPTIME_KEY,
		g_edat_RunSet.greenModeCharStopTime,
		RUNSET_GREENMODECHARSTOPTIME_DEFVAL);

	EDAT_GetIniStringValue(fp, RUNSET_GREENMODEDISCSTARTTIME_KEY,
		g_edat_RunSet.greenModeDisCStartTime,
		RUNSET_GREENMODEDISCSTARTTIME_DEFVAL);

	EDAT_GetIniStringValue(fp, RUNSET_GREENMODEDISCSTOPTIME_KEY,
		g_edat_RunSet.greenModeDisCStopTime,
		RUNSET_GREENMODEDISCSTOPTIME_DEFVAL);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	将g_edat_RunSet数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将g_edat_RunSet数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/06 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValSetRunSet(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_SetIniStringValue(fp, RUNSET_SELLMODECHARSTARTTIME_KEY,
		g_edat_RunSet.sellModeCharStartTime);

	EDAT_SetIniStringValue(fp, RUNSET_SELLMODECHARSTOPTIME_KEY,
		g_edat_RunSet.sellModeCharStopTime);

	EDAT_SetIniStringValue(fp, RUNSET_SELLMODEDISCSTARTTIME_KEY,
		g_edat_RunSet.sellModeDisCStartTime);

	EDAT_SetIniStringValue(fp, RUNSET_SELLMODEDISCSTOPTIME_KEY,
		g_edat_RunSet.sellModeDisCStopTime);

	EDAT_SetIniStringValue(fp, RUNSET_GREENMODECHARSTARTTIME_KEY,
		g_edat_RunSet.greenModeCharStartTime);

	EDAT_SetIniStringValue(fp, RUNSET_GREENMODECHARSTOPTIME_KEY,
		g_edat_RunSet.greenModeCharStopTime);

	EDAT_SetIniStringValue(fp, RUNSET_GREENMODEDISCSTARTTIME_KEY,
		g_edat_RunSet.greenModeDisCStartTime);

	EDAT_SetIniStringValue(fp, RUNSET_GREENMODEDISCSTOPTIME_KEY,
		g_edat_RunSet.greenModeDisCStopTime);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	从文件中读取 g_edat_Enl数据							*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	从文件中读取 g_edat_Enl数据							*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValGetEnl(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_GetIniCharValue(fp, ENL_BTCHARGESTYLE_KEY,
		&g_edat_Enl.btChargeStyle, ENL_BTCHARGESTYLE_DEFVAL);

	EDAT_GetIniCharValue(fp, ENL_BTDISCHARGESTYLE_KEY,
		&g_edat_Enl.btDischargeStyle, ENL_BTDISCHARGESTYLE_DEFVAL);

	EDAT_GetIniCharValue(fp, ENL_V2HCHARGESTYLE_KEY,
		&g_edat_Enl.v2hChargeStyle, ENL_V2HCHARGESTYLE_DEFVAL);

	EDAT_GetIniCharValue(fp, ENL_V2HDISCHARGESTYLE_KEY,
		&g_edat_Enl.v2hDischargeStyle, ENL_V2HDISCHARGESTYLE_DEFVAL);

	EDAT_GetIniShortValue(fp, ENL_BTCHARGEPWRSETVALUE_KEY,
		&g_edat_Enl.btChargePwrSetValue, ENL_BTCHARGEPWRSETVALUE_DEFVAL);

	EDAT_GetIniShortValue(fp, ENL_BTDISCPWRSETVALUE_KEY,
		&g_edat_Enl.btDiscPwrSetValue, ENL_BTDISCPWRSETVALUE_DEFVAL);

	EDAT_GetIniShortValue(fp, ENL_V2HCHARGEPWRSETVALUE_KEY,
		&g_edat_Enl.v2hChargePwrSetValue,
		ENL_V2HCHARGEPWRSETVALUE_DEFVAL);

	EDAT_GetIniShortValue(fp, ENL_V2HDISCHARGEPWRSETVALUE_KEY,
		&g_edat_Enl.v2hDischargePwrSetValue,
		ENL_V2HDISCHARGEPWRSETVALUE_DEFVAL);

	EDAT_GetIniShortValue(fp, ENL_BTACCHARGEQUATYSETVALUE_KEY,
		&g_edat_Enl.btACChargeQuatySetValue,
		ENL_BTACCHARGEQUATYSETVALUE_DEFVAL);

	EDAT_GetIniShortValue(fp, ENL_BTACDISCQUATYSETVALUE_KEY,
		&g_edat_Enl.btACDiscQuatySetValue, ENL_BTACDISCQUATYSETVALUE_DEFVAL);

	EDAT_GetIniShortValue(fp, ENL_BTDCCHARGEQUATYSETVALUE_KEY,
		&g_edat_Enl.btDCChargeQuatySetValue,
		ENL_BTDCCHARGEQUATYSETVALUE_DEFVAL);

	EDAT_GetIniShortValue(fp, ENL_BTDCDISCQUATYSETVALUE_KEY,
		&g_edat_Enl.btDCDiscQuatySetValue, ENL_BTDCDISCQUATYSETVALUE_DEFVAL);

	EDAT_GetIniShortValue(fp, ENL_V2HACCHARGEQUATYSETVALUE_KEY,
		&g_edat_Enl.v2hACChargeQuatySetValue,
		ENL_V2HACCHARGEQUATYSETVALUE_DEFVAL);

	EDAT_GetIniShortValue(fp, ENL_V2HACDISCQUATYSETVALUE_KEY,
		&g_edat_Enl.v2hACDiscQuatySetValue,
		ENL_V2HACDISCQUATYSETVALUE_DEFVAL);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	将g_edat_Enl数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将g_edat_Enl数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/06 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValSetEnl(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_SetIniCharValue(fp, ENL_BTCHARGESTYLE_KEY,
		g_edat_Enl.btChargeStyle);

	EDAT_SetIniCharValue(fp, ENL_BTDISCHARGESTYLE_KEY,
		g_edat_Enl.btDischargeStyle);

	EDAT_SetIniCharValue(fp, ENL_V2HCHARGESTYLE_KEY,
		g_edat_Enl.v2hChargeStyle);

	EDAT_SetIniCharValue(fp, ENL_V2HDISCHARGESTYLE_KEY,
		g_edat_Enl.v2hDischargeStyle);

	EDAT_SetIniShortValue(fp, ENL_BTCHARGEPWRSETVALUE_KEY,
		g_edat_Enl.btChargePwrSetValue);

	EDAT_SetIniShortValue(fp, ENL_BTDISCPWRSETVALUE_KEY,
		g_edat_Enl.btDiscPwrSetValue);

	EDAT_SetIniShortValue(fp, ENL_V2HCHARGEPWRSETVALUE_KEY,
		g_edat_Enl.v2hChargePwrSetValue);

	EDAT_SetIniShortValue(fp, ENL_V2HDISCHARGEPWRSETVALUE_KEY,
		g_edat_Enl.v2hDischargePwrSetValue);

	EDAT_SetIniShortValue(fp, ENL_BTACCHARGEQUATYSETVALUE_KEY,
		g_edat_Enl.btACChargeQuatySetValue);

	EDAT_SetIniShortValue(fp, ENL_BTACDISCQUATYSETVALUE_KEY,
		g_edat_Enl.btACDiscQuatySetValue);

	EDAT_SetIniShortValue(fp, ENL_BTDCCHARGEQUATYSETVALUE_KEY,
		g_edat_Enl.btDCChargeQuatySetValue);

	EDAT_SetIniShortValue(fp, ENL_BTDCDISCQUATYSETVALUE_KEY,
		g_edat_Enl.btDCDiscQuatySetValue);

	EDAT_SetIniShortValue(fp, ENL_V2HACCHARGEQUATYSETVALUE_KEY,
		g_edat_Enl.v2hACChargeQuatySetValue);

	EDAT_SetIniShortValue(fp, ENL_V2HACDISCQUATYSETVALUE_KEY,
		g_edat_Enl.v2hACDiscQuatySetValue);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	从文件中读取 g_edat_Monitor数据						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	从文件中读取 g_edat_Monitor数据						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValGetMonitor(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_GetIniCharValue(fp, MONITOR_ACTLOGCOLLCYC_KEY,
		&g_edat_Monitor.actLogCollectionCycle, MONITOR_ACTLOGCOLLCYC_DEF_VAL);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	将g_edat_Monitor数据写入到文件中					*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将g_edat_Monitor数据写入到文件中					*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/06 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValSetMonitor(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_SetIniCharValue(fp, MONITOR_ACTLOGCOLLCYC_KEY,
		g_edat_Monitor.actLogCollectionCycle);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	从文件中读取 g_edat_FwUpde数据						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	从文件中读取 g_edat_FwUpde数据						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValGetFwUpde(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_GetIniCharValue(fp, FWUPDE_UPDATELED_KEY,
		&g_edat_FwUpde.updateLed, FWUPDE_UPDATELED_DEFVAL);

	EDAT_GetIniShortValue(fp, FWUPDE_UPDENO_KEY,
		&g_edat_FwUpde.updeNo, FWUPDE_UPDENO_DEFVAL);

	EDAT_GetIniCharValue(fp, FWUPDE_ANALYRES_KEY,
		&g_edat_FwUpde.analyRes, FWUPDE_ANALYRES_DEFVAL);

	EDAT_GetIniStringValue(fp, FWUPDE_UPDTIME_KEY,
		g_edat_FwUpde.updTime, FWUPDE_UPDTIME_DEFVAL);

	EDAT_GetIniCharValue(fp, FWUPDE_BOARDTGT_KEY,
		&g_edat_FwUpde.boardTgt, FWUPDE_BOARDTGT_DEFVAL);

	EDAT_GetIniCharValue(fp, FWUPDE_INVTGT_KEY,
		&g_edat_FwUpde.invTgt, FWUPDE_INVTGT_DEFVAL);

	EDAT_GetIniCharValue(fp, FWUPDE_PVTGT_KEY,
		&g_edat_FwUpde.pvTgt, FWUPDE_PVTGT_DEFVAL);

	EDAT_GetIniCharValue(fp, FWUPDE_BDCTGT_KEY,
		&g_edat_FwUpde.bdcTgt, FWUPDE_BDCTGT_DEFVAL);

	EDAT_GetIniCharValue(fp, FWUPDE_JBTGT_KEY,
		&g_edat_FwUpde.jbTgt, FWUPDE_JBTGT_DEFVAL);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	将g_edat_FwUpde数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将g_edat_FwUpde数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/06 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValSetFwUpde(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_SetIniCharValue(fp, FWUPDE_UPDATELED_KEY, g_edat_FwUpde.updateLed);

	EDAT_SetIniShortValue(fp, FWUPDE_UPDENO_KEY, g_edat_FwUpde.updeNo);

	EDAT_SetIniCharValue(fp, FWUPDE_ANALYRES_KEY, g_edat_FwUpde.analyRes);

	EDAT_SetIniStringValue(fp, FWUPDE_UPDTIME_KEY, g_edat_FwUpde.updTime);

	EDAT_SetIniCharValue(fp, FWUPDE_BOARDTGT_KEY, g_edat_FwUpde.boardTgt);

	EDAT_SetIniCharValue(fp, FWUPDE_INVTGT_KEY, g_edat_FwUpde.invTgt);

	EDAT_SetIniCharValue(fp, FWUPDE_PVTGT_KEY, g_edat_FwUpde.pvTgt);

	EDAT_SetIniCharValue(fp, FWUPDE_BDCTGT_KEY, g_edat_FwUpde.bdcTgt);

	EDAT_SetIniCharValue(fp, FWUPDE_JBTGT_KEY, g_edat_FwUpde.jbTgt);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	从文件中读取 g_edat_Maint数据						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	从文件中读取 g_edat_Maint数据						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValGetMaint(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_GetIniStringValue(fp, MAINT_MENTANCEDATE_KEY,
		g_edat_Maint.mentanceDate, MAINT_MENTANCEDATE_DEFVAL);

	EDAT_GetIniStringValue(fp, MAINT_IMPLTATIONDESIGNDAY1_KEY,
		g_edat_Maint.impltationDesignDay1, MAINT_IMPLTATIONDESIGNDAY1_DEFVAL);

	EDAT_GetIniStringValue(fp, MAINT_IMPLTATIONDESIGNDAY2_KEY,
		g_edat_Maint.impltationDesignDay2, MAINT_IMPLTATIONDESIGNDAY2_DEFVAL);

	EDAT_GetIniCharValue(fp, MAINT_EXECUTEMODE_KEY,
		&g_edat_Maint.executeMode, MAINT_EXECUTEMODE_DEFVAL);

	EDAT_GetIniCharValue(fp, MAINT_MAINTAINRESULT_KEY,
		&g_edat_Maint.maintainResult, MAINT_MAINTAINRESULT_DEFVAL);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	将g_edat_Maint数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将g_edat_Maint数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/06 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValSetMaint(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_SetIniStringValue(fp, MAINT_MENTANCEDATE_KEY,
		g_edat_Maint.mentanceDate);

	EDAT_SetIniStringValue(fp, MAINT_IMPLTATIONDESIGNDAY1_KEY,
		g_edat_Maint.impltationDesignDay1);

	EDAT_SetIniStringValue(fp, MAINT_IMPLTATIONDESIGNDAY2_KEY,
		g_edat_Maint.impltationDesignDay2);

	EDAT_SetIniCharValue(fp, MAINT_EXECUTEMODE_KEY,
		g_edat_Maint.executeMode);

	EDAT_SetIniCharValue(fp, MAINT_MAINTAINRESULT_KEY,
		g_edat_Maint.maintainResult);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	从文件中读取 g_edat_Cmd65数据						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	从文件中读取 g_edat_Cmd65数据						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValGetCmd65(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_GetIniCharValue(fp, CMD65_COMMBOARDMANAGEMODE_KEY,
		&g_edat_Cmd65.commBoardManageMode, CMD65_COMMBOARDMANAGEMODE_DEFVAL);

	EDAT_GetIniCharValue(fp, CMD65_EMERGENCYMODE_KEY,
		&g_edat_Cmd65.emergencyMode, CMD65_EMERGENCYMODE_DEFVAL);

	EDAT_GetIniShortValue(fp, CMD65_BTUPPERLIMIT_KEY,
		&g_edat_Cmd65.btUpperLimit, CMD65_BTUPPERLIMIT_DEFVAL);

	EDAT_GetIniShortValue(fp, CMD65_BTLOWERLIMIT_KEY,
		&g_edat_Cmd65.btUpperLimit, CMD65_BTLOWERLIMIT_DEFVAL);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	将g_edat_Cmd65数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将g_edat_Cmd65数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/06 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValSetCmd65(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_SetIniCharValue(fp, CMD65_COMMBOARDMANAGEMODE_KEY,
		g_edat_Cmd65.commBoardManageMode);

	EDAT_SetIniCharValue(fp, CMD65_EMERGENCYMODE_KEY,
		g_edat_Cmd65.emergencyMode);

	EDAT_SetIniShortValue(fp, CMD65_BTUPPERLIMIT_KEY,
		g_edat_Cmd65.btUpperLimit);

	EDAT_SetIniShortValue(fp, CMD65_BTLOWERLIMIT_KEY,
		g_edat_Cmd65.btLowerLimit);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	从文件中读取 g_edat_Cmd75数据						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	从文件中读取 g_edat_Cmd75数据						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValGetCmd75(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_GetIniCharValue(fp, CMD75_CARCHARORDISCBODYOPRAT_KEY,
		&g_edat_Cmd75.carCharordiscBodyOprat,
		CMD75_CARCHARORDISCBODYOPRAT_DEFVAL);

	EDAT_GetIniCharValue(fp, CMD75_PWRCUTDISCHAR_KEY,
		&g_edat_Cmd75.pwrcutDischar,
		CMD75_PWRCUTDISCHAR_DEFVAL);

	EDAT_GetIniCharValue(fp, CMD75_PWRCUTDISCPRIORITYSET_KEY,
		&g_edat_Cmd75.pwrcutDiscPrioritySet,
		CMD75_PWRCUTDISCPRIORITYSET_DEFVAL);

	EDAT_GetIniCharValue(fp, CMD75_PWRCUTCHARPRIORITYSET_KEY,
		&g_edat_Cmd75.pwrcutCharPrioritySet,
		CMD75_PWRCUTCHARPRIORITYSET_DEFVAL);

	EDAT_GetIniShortValue(fp, CMD75_CARDISCLOWERLIMITSET_KEY,
		&g_edat_Cmd75.carDiscLowerLimitSet,
		CMD75_CARDISCLOWERLIMITSET_DEFVAL);

	EDAT_GetIniShortValue(fp, CMD75_CARCHARUPPERLIMITSET_KEY,
		&g_edat_Cmd75.carCharUpperLimitSet,
		CMD75_CARCHARUPPERLIMITSET_DEFVAL);

	EDAT_GetIniCharValue(fp, CMD75_CARDISCORNOT_KEY,
		&g_edat_Cmd75.carDiscOrnot,
		CMD75_CARDISCORNOT_DEFVAL);

	EDAT_GetIniCharValue(fp, CMD75_DISCHARPRIORITYSET_KEY,
		&g_edat_Cmd75.discharPrioritySet,
		CMD75_DISCHARPRIORITYSET_DEFVAL);

	EDAT_GetIniShortValue(fp, CMD75_PWRCUTDISCLOWERLIMITSET_KEY,
		&g_edat_Cmd75.pwrcutDiscLowerLimitSet,
		CMD75_PWRCUTDISCLOWERLIMITSET_DEFVAL);

	EDAT_GetIniCharValue(fp, CMD75_CHARGEPRIORITYSET_KEY,
		&g_edat_Cmd75.chargePrioritySet,
		CMD75_CHARGEPRIORITYSET_DEFVAL);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	将g_edat_Cmd75数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将g_edat_Cmd75数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/06 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValSetCmd75(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_SetIniCharValue(fp, CMD75_CARCHARORDISCBODYOPRAT_KEY,
		g_edat_Cmd75.carCharordiscBodyOprat);

	EDAT_SetIniCharValue(fp, CMD75_PWRCUTDISCHAR_KEY,
		g_edat_Cmd75.pwrcutDischar);

	EDAT_SetIniCharValue(fp, CMD75_PWRCUTDISCPRIORITYSET_KEY,
		g_edat_Cmd75.pwrcutDiscPrioritySet);

	EDAT_SetIniCharValue(fp, CMD75_PWRCUTCHARPRIORITYSET_KEY,
		g_edat_Cmd75.pwrcutCharPrioritySet);

	EDAT_SetIniShortValue(fp, CMD75_CARDISCLOWERLIMITSET_KEY,
		g_edat_Cmd75.carDiscLowerLimitSet);

	EDAT_SetIniShortValue(fp, CMD75_CARCHARUPPERLIMITSET_KEY,
		g_edat_Cmd75.carCharUpperLimitSet);

	EDAT_SetIniCharValue(fp, CMD75_CARDISCORNOT_KEY,
		g_edat_Cmd75.carDiscOrnot);

	EDAT_SetIniCharValue(fp, CMD75_DISCHARPRIORITYSET_KEY,
		g_edat_Cmd75.discharPrioritySet);

	EDAT_SetIniShortValue(fp, CMD75_PWRCUTDISCLOWERLIMITSET_KEY,
		g_edat_Cmd75.pwrcutDiscLowerLimitSet);

	EDAT_SetIniCharValue(fp, CMD75_CHARGEPRIORITYSET_KEY,
		g_edat_Cmd75.chargePrioritySet);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	从文件中读取 g_edat_Cmd6b数据						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	从文件中读取 g_edat_Cmd6b数据						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValGetCmd6b(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_GetIniShortValue(fp, CMD6B_CONTARACTPWRSET_KEY,
		&g_edat_Cmd6b.contaractPwrSet,
		CMD6B_CONTARACTPWRSET_DEFVAL);

	EDAT_GetIniCharValue(fp, CMD6B_PWRCUTOUTSET_KEY,
		&g_edat_Cmd6b.pwrcutOutSet,
		CMD6B_PWRCUTOUTSET_DEFVAL);

	EDAT_GetIniCharValue(fp, CMD6B_PWRCUTOUTSTART_KEY,
		&g_edat_Cmd6b.pwrcutOutStart,
		CMD6B_PWRCUTOUTSTART_DEFVAL);

	EDAT_GetIniCharValue(fp, CMD6B_SELLVALIDORINVALID_KEY,
		&g_edat_Cmd6b.sellValidOrInvalid,
		CMD6B_SELLVALIDORINVALID_DEFVAL);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	将g_edat_Cmd6b数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将g_edat_Cmd6b数据写入到文件中						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/06 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValSetCmd6b(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_SetIniShortValue(fp, CMD6B_CONTARACTPWRSET_KEY,
		g_edat_Cmd6b.contaractPwrSet);

	EDAT_SetIniCharValue(fp, CMD6B_PWRCUTOUTSET_KEY,
		g_edat_Cmd6b.pwrcutOutSet);

	EDAT_SetIniCharValue(fp, CMD6B_PWRCUTOUTSTART_KEY,
		g_edat_Cmd6b.pwrcutOutStart);

	EDAT_SetIniCharValue(fp, CMD6B_SELLVALIDORINVALID_KEY,
		g_edat_Cmd6b.sellValidOrInvalid);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	从文件中读取 g_edat_Setting数据						*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	从文件中读取 g_edat_Setting数据						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/05 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValGetSetting(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_GetIniShortValue(fp, SETTING_SYSVOLTDETECTLEVEL_KEY,
		&g_edat_Setting.sysVoltDetectLevel,
		SETTING_SYSVOLTDETECTLEVEL_DEFVAL);

	EDAT_GetIniShortValue(fp, SETTING_SYSVOLTDETECTTIME_KEY,
		&g_edat_Setting.sysVoltDetectTime,
		SETTING_SYSVOLTDETECTTIME_DEFVAL);

	EDAT_GetIniShortValue(fp, SETTING_SYSUNDERVOLTDETECTLEVEL_KEY,
		&g_edat_Setting.sysUnderVoltDetectLevel,
		SETTING_SYSUNDERVOLTDETECTLEVEL_DEFVAL);

	EDAT_GetIniShortValue(fp, SETTING_SYSUNDERVOLTDETECTTIME_KEY,
		&g_edat_Setting.sysUnderVoltDetectTime,
		SETTING_SYSUNDERVOLTDETECTTIME_DEFVAL);

	EDAT_GetIniCharValue(fp, SETTING_FRERISEDETECTLEVEL_KEY,
		&g_edat_Setting.freRiseDetectLevel,
		SETTING_FRERISEDETECTLEVEL_DEFVAL);

	EDAT_GetIniShortValue(fp, SETTING_FRERISEDETECTTIME_KEY,
		&g_edat_Setting.freRiseDetectTime,
		SETTING_FRERISEDETECTTIME_DEFVAL);

	EDAT_GetIniCharValue(fp, SETTING_FREDROPDETECTLEVEL_KEY,
		&g_edat_Setting.freDropDetectLevel,
		SETTING_FREDROPDETECTLEVEL_DEFVAL);

	EDAT_GetIniShortValue(fp, SETTING_FREDROPDETECTTIME_KEY,
		&g_edat_Setting.freDropDetectTime,
		SETTING_FREDROPDETECTTIME_DEFVAL);

	EDAT_GetIniShortValue(fp, SETTING_RESCTTIME_KEY,
		&g_edat_Setting.resctTime,
		SETTING_RESCTTIME_DEFVAL);

	EDAT_GetIniShortValue(fp, SETTING_VOLRSINHBVALUE_KEY,
		&g_edat_Setting.volRsInhbValue,
		SETTING_VOLRSINHBVALUE_DEFVAL);

	EDAT_GetIniShortValue(fp, SETTING_VOLRSINHBATTLIMIT_KEY,
		&g_edat_Setting.volRsInhbattLimit,
		SETTING_VOLRSINHBATTLIMIT_DEFVAL);

	EDAT_GetIniShortValue(fp, SETTING_COTPOWERCONTROL_KEY,
		&g_edat_Setting.cotPowerControl,
		SETTING_COTPOWERCONTROL_DEFVAL);

	EDAT_GetIniShortValue(fp, SETTING_PASSINDOPERDETECT_KEY,
		&g_edat_Setting.passIndOperDetect,
		SETTING_PASSINDOPERDETECT_DEFVAL);

	EDAT_GetIniShortValue(fp, SETTING_FREFEEDBACKGAIN_KEY,
		&g_edat_Setting.freFeedbackGain,
		SETTING_FREFEEDBACKGAIN_DEFVAL);

	EDAT_GetIniShortValue(fp, SETTING_FRELEVALLOW_KEY,
		&g_edat_Setting.freLevAllow,
		SETTING_FRELEVALLOW_DEFVAL);

	EDAT_GetIniShortValue(fp, SETTING_CHANGETIMEINCORDES_KEY,
		&g_edat_Setting.changeTimeIncorDes,
		SETTING_CHANGETIMEINCORDES_DEFVAL);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	将g_edat_Setting数据写入到文件中					*/
/*--------------------------------------------------------------------------*/
/*	param			:	fp							文件指针				*/
/*--------------------------------------------------------------------------*/
/*	return			:	void												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	将g_edat_Setting数据写入到文件中					*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/05/06 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void edat_ValSetSetting(FILE *fp)
{
	DEBUG_TRCIN();

	EDAT_SetIniShortValue(fp, SETTING_SYSVOLTDETECTLEVEL_KEY,
		g_edat_Setting.sysVoltDetectLevel);

	EDAT_SetIniShortValue(fp, SETTING_SYSVOLTDETECTTIME_KEY,
		g_edat_Setting.sysVoltDetectTime);

	EDAT_SetIniShortValue(fp, SETTING_SYSUNDERVOLTDETECTLEVEL_KEY,
		g_edat_Setting.sysUnderVoltDetectLevel);

	EDAT_SetIniShortValue(fp, SETTING_SYSUNDERVOLTDETECTTIME_KEY,
		g_edat_Setting.sysUnderVoltDetectTime);

	EDAT_SetIniCharValue(fp, SETTING_FRERISEDETECTLEVEL_KEY,
		g_edat_Setting.freRiseDetectLevel);

	EDAT_SetIniShortValue(fp, SETTING_FRERISEDETECTTIME_KEY,
		g_edat_Setting.freRiseDetectTime);

	EDAT_SetIniCharValue(fp, SETTING_FREDROPDETECTLEVEL_KEY,
		g_edat_Setting.freDropDetectLevel);

	EDAT_SetIniShortValue(fp, SETTING_FREDROPDETECTTIME_KEY,
		g_edat_Setting.freDropDetectTime);

	EDAT_SetIniShortValue(fp, SETTING_RESCTTIME_KEY,
		g_edat_Setting.resctTime);

	EDAT_SetIniShortValue(fp, SETTING_VOLRSINHBVALUE_KEY,
		g_edat_Setting.volRsInhbValue);

	EDAT_SetIniShortValue(fp, SETTING_VOLRSINHBATTLIMIT_KEY,
		g_edat_Setting.volRsInhbattLimit);

	EDAT_SetIniShortValue(fp, SETTING_COTPOWERCONTROL_KEY,
		g_edat_Setting.cotPowerControl);

	EDAT_SetIniShortValue(fp, SETTING_PASSINDOPERDETECT_KEY,
		g_edat_Setting.passIndOperDetect);

	EDAT_SetIniShortValue(fp, SETTING_FREFEEDBACKGAIN_KEY,
		g_edat_Setting.freFeedbackGain);

	EDAT_SetIniShortValue(fp, SETTING_FRELEVALLOW_KEY,
		g_edat_Setting.freLevAllow);

	EDAT_SetIniShortValue(fp, SETTING_CHANGETIMEINCORDES_KEY,
		g_edat_Setting.changeTimeIncorDes);

	DEBUG_TRCOUT();
}

/*End Of File*/
