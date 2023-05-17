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
#ifndef __EDAT_VAL_H__
#define __EDAT_VAL_H__

#include "edat_val_g.h"
#include <stdio.h>
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
/* 構成 */
#define CONST_EXTERSUNPWRORNOT_KEY				"exterSunPwrOrnot"
#define CONST_SUNBYDPWRGENDEV_KEY				"sunBeyondPwrGenratDevice"
#define CONST_LOADCONNTYPE_KEY					"loadConnectType"

#define CONST_EXTERSUNPWRORNOT_DEFVAL			((uchar)0u)
#define CONST_SUNBYDPWRGENDEV_DEFVAL			((uchar)0u)
#define CONST_LOADCONNTYPE_DEFVAL				((uchar)0u)

/* 設定値 */
#define SETVAL_PUSHUPSET_KEY					"pushUpSet"
#define SETVAL_OUTPUTCTROBJ_KEY					"outputContrObject"
#define SETVAL_EQPMTAPPROVECAP_KEY				"eqpmtApproveCap"
#define SETVAL_GENPWRUPLMT_KEY					"genPwrUpperLimit"
#define SETVAL_CLIPACTION_KEY					"clipAction"
#define SETVAL_FITCONTTYPE_KEY					"fitContractType"
#define SETVAL_SELFCONSTYPE_KEY					"selfConsumeType"
#define SETVAL_PWEHOUSEID_KEY					"pwrHouseId"
#define SETVAL_PWRCOMPSERVER_KEY				"pwrCompanyServer"

#define SETVAL_PUSHUPSET_DEFVAL					((uchar)0u)
#define SETVAL_OUTPUTCTROBJ_DEFVAL				((uchar)0u)
#define SETVAL_EQPMTAPPROVECAP_DEFVAL			((float)5.9)
#define SETVAL_GENPWRUPLMT_DEFVAL				((float)5.9)
#define SETVAL_CLIPACTION_DEFVAL				((uchar)0u)
#define SETVAL_FITCONTTYPE_DEFVAL				((uchar)0u)
#define SETVAL_SELFCONSTYPE_DEFVAL				((uchar)0u)
#define SETVAL_PWEHOUSEID_DEFVAL				"-"
#define SETVAL_PWRCOMPSERVER_DEFVAL				((uchar)0u)

/* 太陽光発電設定 */
#define PV_FISSHDSTARTDATE_KEY					"fixShdStartDate"
#define PV_SUNPWRRATED_KEY						"sunPwrRated"
#define PV_PVPANELRATING_KEY					"pvPanelRating"

#define PV_FISSHDSTARTDATE_DEFVAL				"-"
#define PV_SUNPWRRATED_DEFVLA					((float)0)
#define PV_PVPANELRATING_DEFVLA					((float)0)

/* 機器情報 */
#define MACHINEINF_SYSMODELNAME_KEY				"sysModelName"
#define MACHINEINF_COMMMODELNAME_KEY			"commModelName"
#define MACHINEINF_COMMSERIALNUM_KEY			"commSerialNum"
#define MACHINEINF_COMMBOARDFWVER_KEY			"commBoardFwVer"
#define MACHINEINF_OPREMOTECTLFW_KEY			"opRemoteCtlFw"
#define MACHINEINF_SUNPWRSETLOCATION_KEY		"sunPwrSetLocation"
#define MACHINEINF_BTSETLOCATION_KEY			"btSetLocation"
#define MACHINEINF_V2HSTANDSETLOCATION_KEY		"v2hStandSetLocation"
#define MACHINEINF_ELECTWATERHEATING_KEY		"electWaterHeating"
#define MACHINEINF_POSTALNUM_KEY				"postalNum"
#define MACHINEINF_ECHONETLITEVER_KEY			"echonetLiteVer"
#define MACHINEINF_OPREMOTECONTROLORNOT_KEY		"opRemoteControlOrnot"

#define MACHINEINF_SYSMODELNAME_DEFVAL			"-"
#define MACHINEINF_COMMMODELNAME_DEFVAL			"-"
#define MACHINEINF_COMMSERIALNUM_DEFVAL			"-"
#define MACHINEINF_COMMBOARDFWVER_DEFVAL		"-"
#define MACHINEINF_OPREMOTECTLFW_DEFVAL			"-"
#define MACHINEINF_SUNPWRSETLOCATION_DEFVAL		((uchar)0u)
#define MACHINEINF_BTSETLOCATION_DEFVAL			((uchar)0u)
#define MACHINEINF_V2HSTANDSETLOCATION_DEFVAL	((uchar)0u)
#define MACHINEINF_ELECTWATERHEATING_DEFVAL		((uchar)0u)
#define MACHINEINF_POSTALNUM_DEFVAL				"-"
#define MACHINEINF_ECHONETLITEVER_DEFVAL		"Q"
#define MACHINEINF_OPREMOTECONTROLORNOT_DEFVAL	((uchar)0u)

/* 設定情報 */
#define SETINF_BTCHARPWRMAXVALSET_KEY			"btCharPwrMaxValueSet"
#define SETINF_CARCHARPWRVALSET_KEY				"carCharPwrValueSet"
#define SETINF_CARTYPE_KEY						"carType"
#define SETINF_CARCHARGLIMIT_KEY				"carChargLimit"

#define SETINF_BTCHARPWRMAXVALSET_DEFVAL		((float)2.0)
#define SETINF_CARCHARPWRVALSET_DEFVAL			((float)5.9)
#define SETINF_CARTYPE_DEFVAL					((uchar)0u)
#define SETINF_CARCHARGLIMIT_DEFVAL				((uchar)2u)

/* ネットワーク設定 */
#define NETWORKSET_IPADDRSET_KEY				"ipAddrSet"
#define NETWORKSET_IPADDRESS_KEY				"ipAddress"
#define NETWORKSET_SUBNETMASK_KEY				"subnetMask"
#define NETWORKSET_DEFAULTGATEWAY_KEY			"defaultGateway"
#define NETWORKSET_POIORITYDNS_KEY				"poiorityDns"
#define NETWORKSET_REPLACEDNS_KEY				"replaceDns"
#define NETWORKSET_PROXYSET_KEY					"proxySet"
#define NETWORKSET_PROXYADDRESS_KEY				"proxyAddress"
#define NETWORKSET_PORTNUMBER_KEY				"portNumber"

#define NETWORKSET_IPADDRSET_DEFVAL				((uchar)0u)
#define NETWORKSET_IPADDRESS_DEFVAL				"192.168.1.203"
#define NETWORKSET_SUBNETMASK_DEFVAL			"255.255.255.0"
#define NETWORKSET_DEFAULTGATEWAY_DEFVAL		"192.168.1.1"
#define NETWORKSET_POIORITYDNS_DEFVAL			"192.168.1.1"
#define NETWORKSET_REPLACEDNS_DEFVAL			"192.168.1.1"
#define NETWORKSET_PROXYSET_DEFVAL				((uchar)0u)
#define NETWORKSET_PROXYADDRESS_DEFVAL			"-"
#define NETWORKSET_PORTNUMBER_DEFVAL			"-"

/* 通信状態 */
#define COMMSTE_UPDESHDGETDATE_KEY				"updeShdGetDate"

#define COMMSTE_UPDESHDGETDATE_DEFVAL			"-"

/* 運転設定 */
#define RUNSET_SELLMODECHARSTARTTIME_KEY		"sellModeCharStartTime"
#define RUNSET_SELLMODECHARSTOPTIME_KEY			"sellModeCharStopTime"
#define RUNSET_SELLMODEDISCSTARTTIME_KEY		"sellModeDisCStartTime"
#define RUNSET_SELLMODEDISCSTOPTIME_KEY			"sellModeDisCStopTime"
#define RUNSET_GREENMODECHARSTARTTIME_KEY		"greenModeCharStartTime"
#define RUNSET_GREENMODECHARSTOPTIME_KEY		"greenModeCharStopTime"
#define RUNSET_GREENMODEDISCSTARTTIME_KEY		"greenModeDisCStartTime"
#define RUNSET_GREENMODEDISCSTOPTIME_KEY		"greenModeDisCStopTime"

#define RUNSET_SELLMODECHARSTARTTIME_DEFVAL		"01:00"
#define RUNSET_SELLMODECHARSTOPTIME_DEFVAL		"05:59"
#define RUNSET_SELLMODEDISCSTARTTIME_DEFVAL		"06:00"
#define RUNSET_SELLMODEDISCSTOPTIME_DEFVAL		"00:50"
#define RUNSET_GREENMODECHARSTARTTIME_DEFVAL	"01:00"
#define RUNSET_GREENMODECHARSTOPTIME_DEFVAL		"01:01"
#define RUNSET_GREENMODEDISCSTARTTIME_DEFVAL	"01:02"
#define RUNSET_GREENMODEDISCSTOPTIME_DEFVAL		"00:50"

/* ENL関連 */
#define ENL_BTCHARGESTYLE_KEY					"btChargeStyle"
#define ENL_BTDISCHARGESTYLE_KEY				"btDischargeStyle"
#define ENL_V2HCHARGESTYLE_KEY					"v2hChargeStyle"
#define ENL_V2HDISCHARGESTYLE_KEY				"v2hDischargeStyle"
#define ENL_BTCHARGEPWRSETVALUE_KEY				"btChargePwrSetValue"
#define ENL_BTDISCPWRSETVALUE_KEY				"btDiscPwrSetValue"
#define ENL_V2HCHARGEPWRSETVALUE_KEY			"v2hChargePwrSetValue"
#define ENL_V2HDISCHARGEPWRSETVALUE_KEY			"v2hDischargePwrSetValue"
#define ENL_BTACCHARGEQUATYSETVALUE_KEY			"btACChargeQuatySetValue"
#define ENL_BTACDISCQUATYSETVALUE_KEY			"btACDiscQuatySetValue"
#define ENL_BTDCCHARGEQUATYSETVALUE_KEY			"btDCChargeQuatySetValue"
#define ENL_BTDCDISCQUATYSETVALUE_KEY			"btDCDiscQuatySetValue"
#define ENL_V2HACCHARGEQUATYSETVALUE_KEY		"v2hACChargeQuatySetValue"
#define ENL_V2HACDISCQUATYSETVALUE_KEY			"v2hACDiscQuatySetValue"

#define ENL_BTCHARGESTYLE_DEFVAL				((uchar)0u)
#define ENL_BTDISCHARGESTYLE_DEFVAL				((uchar)0u)
#define ENL_V2HCHARGESTYLE_DEFVAL				((uchar)0u)
#define ENL_V2HDISCHARGESTYLE_DEFVAL			((uchar)0u)
#define ENL_BTCHARGEPWRSETVALUE_DEFVAL			((ushort)0u)
#define ENL_BTDISCPWRSETVALUE_DEFVAL			((ushort)0u)
#define ENL_V2HCHARGEPWRSETVALUE_DEFVAL			((ushort)0u)
#define ENL_V2HDISCHARGEPWRSETVALUE_DEFVAL		((ushort)0u)
#define ENL_BTACCHARGEQUATYSETVALUE_DEFVAL		((ushort)0u)
#define ENL_BTACDISCQUATYSETVALUE_DEFVAL		((ushort)0u)
#define ENL_BTDCCHARGEQUATYSETVALUE_DEFVAL		((ushort)0u)
#define ENL_BTDCDISCQUATYSETVALUE_DEFVAL		((ushort)0u)
#define ENL_V2HACCHARGEQUATYSETVALUE_DEFVAL		((ushort)0u)
#define ENL_V2HACDISCQUATYSETVALUE_DEFVAL		((ushort)0u)

/* 見守り関連 */
#define MONITOR_ACTLOGCOLLCYC_KEY				"actLogCollectionCycle"

#define MONITOR_ACTLOGCOLLCYC_DEF_VAL			((uchar)10u)

/* FW更新関連 */
#define FWUPDE_UPDATELED_KEY					"updateLed"
#define FWUPDE_UPDENO_KEY						"updeNo"
#define FWUPDE_ANALYRES_KEY						"analyRes"
#define FWUPDE_UPDTIME_KEY						"updTime"
#define FWUPDE_BOARDTGT_KEY						"boardTgt"
#define FWUPDE_INVTGT_KEY						"invTgt"
#define FWUPDE_PVTGT_KEY						"pvTgt"
#define FWUPDE_BDCTGT_KEY						"bdcTgt"
#define FWUPDE_JBTGT_KEY						"jbTgt"

#define FWUPDE_UPDATELED_DEFVAL					((uchar)0u)
#define FWUPDE_UPDENO_DEFVAL					((ushort)0u)
#define FWUPDE_ANALYRES_DEFVAL					((uchar)0u)
#define FWUPDE_UPDTIME_DEFVAL					"-"
#define FWUPDE_BOARDTGT_DEFVAL					((uchar)0u)
#define FWUPDE_INVTGT_DEFVAL					((uchar)0u)
#define FWUPDE_PVTGT_DEFVAL						((uchar)0u)
#define FWUPDE_BDCTGT_DEFVAL					((uchar)0u)
#define FWUPDE_JBTGT_DEFVAL						((uchar)0u)

/* メンテナンス関連 */
#define MAINT_MENTANCEDATE_KEY					"mentanceDate"
#define MAINT_IMPLTATIONDESIGNDAY1_KEY			"impltationDesignDay1"
#define MAINT_IMPLTATIONDESIGNDAY2_KEY			"impltationDesignDay2"
#define MAINT_EXECUTEMODE_KEY					"executeMode"
#define MAINT_MAINTAINRESULT_KEY				"maintainResult"

#define MAINT_MENTANCEDATE_DEFVAL				"000000"
#define MAINT_IMPLTATIONDESIGNDAY1_DEFVAL		"000000"
#define MAINT_IMPLTATIONDESIGNDAY2_DEFVAL		"000000"
#define MAINT_EXECUTEMODE_DEFVAL				((uchar)0u)
#define MAINT_MAINTAINRESULT_DEFVAL				((uchar)0u)

/* CMD65 */
#define CMD65_COMMBOARDMANAGEMODE_KEY			"commBoardManageMode"
#define CMD65_EMERGENCYMODE_KEY					"emergencyMode"
#define CMD65_BTUPPERLIMIT_KEY					"btUpperLimit"
#define CMD65_BTLOWERLIMIT_KEY					"btLowerLimit"

#define CMD65_COMMBOARDMANAGEMODE_DEFVAL		((uchar)0u)
#define CMD65_EMERGENCYMODE_DEFVAL				((uchar)0u)
#define CMD65_BTUPPERLIMIT_DEFVAL				((ushort)100u)
#define CMD65_BTLOWERLIMIT_DEFVAL				((ushort)0u)

/* CMD75 */

#define CMD75_CARCHARORDISCBODYOPRAT_KEY		"carCharordiscBodyOprat"
#define CMD75_PWRCUTDISCHAR_KEY					"pwrcutDischar"
#define CMD75_PWRCUTDISCPRIORITYSET_KEY			"pwrcutDiscPrioritySet"
#define CMD75_PWRCUTCHARPRIORITYSET_KEY			"pwrcutCharPrioritySet"
#define CMD75_CARDISCLOWERLIMITSET_KEY			"carDiscLowerLimitSet"
#define CMD75_CARCHARUPPERLIMITSET_KEY			"carCharUpperLimitSet"
#define CMD75_CARDISCORNOT_KEY					"carDiscOrnot"
#define CMD75_DISCHARPRIORITYSET_KEY			"discharPrioritySet"
#define CMD75_PWRCUTDISCLOWERLIMITSET_KEY		"pwrcutDiscLowerLimitSet"
#define CMD75_CHARGEPRIORITYSET_KEY				"chargePrioritySet"

#define CMD75_CARCHARORDISCBODYOPRAT_DEFVAL		((uchar)1u)
#define CMD75_PWRCUTDISCHAR_DEFVAL				((uchar)1u)
#define CMD75_PWRCUTDISCPRIORITYSET_DEFVAL		((uchar)0u)
#define CMD75_PWRCUTCHARPRIORITYSET_DEFVAL		((uchar)1u)
#define CMD75_CARDISCLOWERLIMITSET_DEFVAL		((ushort)30u)
#define CMD75_CARCHARUPPERLIMITSET_DEFVAL		((ushort)100u)
#define CMD75_CARDISCORNOT_DEFVAL				((uchar)1u)
#define CMD75_DISCHARPRIORITYSET_DEFVAL			((uchar)0u)
#define CMD75_PWRCUTDISCLOWERLIMITSET_DEFVAL	((ushort)10u)
#define CMD75_CHARGEPRIORITYSET_DEFVAL			((uchar)1u)

/* CMD6B */
#define CMD6B_CONTARACTPWRSET_KEY				"contaractPwrSet"
#define CMD6B_PWRCUTOUTSET_KEY					"pwrcutOutSet"
#define CMD6B_PWRCUTOUTSTART_KEY				"pwrcutOutStart"
#define CMD6B_SELLVALIDORINVALID_KEY			"sellValidOrInvalid"

#define CMD6B_CONTARACTPWRSET_DEFVAL			((ushort)9999u)
#define CMD6B_PWRCUTOUTSET_DEFVAL				((uchar)1u)
#define CMD6B_PWRCUTOUTSTART_DEFVAL				((uchar)0u)
#define CMD6B_SELLVALIDORINVALID_DEFVAL			((uchar)0u)

/* 整定値 */
#define SETTING_SYSVOLTDETECTLEVEL_KEY			"sysVoltDetectLevel"
#define SETTING_SYSVOLTDETECTTIME_KEY			"sysVoltDetectTime"
#define SETTING_SYSUNDERVOLTDETECTLEVEL_KEY		"sysUnderVoltDetectLevel"
#define SETTING_SYSUNDERVOLTDETECTTIME_KEY		"sysUnderVoltDetectTime"
#define SETTING_FRERISEDETECTLEVEL_KEY			"freRiseDetectLevel"
#define SETTING_FRERISEDETECTTIME_KEY			"freRiseDetectTime"
#define SETTING_FREDROPDETECTLEVEL_KEY			"freDropDetectLevel"
#define SETTING_FREDROPDETECTTIME_KEY			"freDropDetectTime"
#define SETTING_RESCTTIME_KEY					"resctTime"
#define SETTING_VOLRSINHBVALUE_KEY				"volRsInhbValue"
#define SETTING_VOLRSINHBATTLIMIT_KEY			"volRsInhbattLimit"
#define SETTING_COTPOWERCONTROL_KEY				"cotPowerControl"
#define SETTING_PASSINDOPERDETECT_KEY			"passIndOperDetect"
#define SETTING_FREFEEDBACKGAIN_KEY				"freFeedbackGain"
#define SETTING_FRELEVALLOW_KEY					"freLevAllow"
#define SETTING_CHANGETIMEINCORDES_KEY			"changeTimeIncorDes"

#define SETTING_SYSVOLTDETECTLEVEL_DEFVAL		((ushort)115u)
#define SETTING_SYSVOLTDETECTTIME_DEFVAL		((ushort)1000u)
#define SETTING_SYSUNDERVOLTDETECTLEVEL_DEFVAL	((ushort)80u)
#define SETTING_SYSUNDERVOLTDETECTTIME_DEFVAL	((ushort)1000u)
#define SETTING_FRERISEDETECTLEVEL_DEFVAL		((uchar)2u)
#define SETTING_FRERISEDETECTTIME_DEFVAL		((ushort)1000u)
#define SETTING_FREDROPDETECTLEVEL_DEFVAL		((uchar)5u)
#define SETTING_FREDROPDETECTTIME_DEFVAL		((ushort)2000u)
#define SETTING_RESCTTIME_DEFVAL				((ushort)300u)
#define SETTING_VOLRSINHBVALUE_DEFVAL			((ushort)1090u)
#define SETTING_VOLRSINHBATTLIMIT_DEFVAL		((ushort)200u)
#define SETTING_COTPOWERCONTROL_DEFVAL			((ushort)95u)
#define SETTING_PASSINDOPERDETECT_DEFVAL		((ushort)3u)
#define SETTING_FREFEEDBACKGAIN_DEFVAL			((ushort)1000u)
#define SETTING_FRELEVALLOW_DEFVAL				((ushort)10u)
#define SETTING_CHANGETIMEINCORDES_DEFVAL		((ushort)5u)


#define EDAT_VAL_ARRAY_LEN						(16)

typedef void (*TableArr_t)(FILE *fp);

#define EDAT_VAL_FILE_PATH_NAME_LEN				((ushort)512u)

#define EDAT_VAL_CUR_FILE_PATH					"/user/edat"
#define EDAT_VAL_CUR_FILE_NAME					"curDataFile.ini"

#define EDAT_VAL_DEF_FILE_PATH					"/user/edat"
#define EDAT_VAL_DEF_FILE_NAME					"defDataFile.ini"

/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
static void edat_ValGetAll(FILE *fp);
static void edat_ValGetConst(FILE *fp);
static void edat_ValGetSetVal(FILE *fp);
static void edat_ValGetPv(FILE *fp);
static void edat_ValGetMachInf(FILE *fp);
static void edat_ValGetSetInf(FILE *fp);
static void edat_ValGetNetWorkSet(FILE *fp);
static void edat_ValGetCommSte(FILE *fp);
static void edat_ValGetRunSet(FILE *fp);
static void edat_ValGetEnl(FILE *fp);
static void edat_ValGetMonitor(FILE *fp);
static void edat_ValGetFwUpde(FILE *fp);
static void edat_ValGetMaint(FILE *fp);
static void edat_ValGetCmd65(FILE *fp);
static void edat_ValGetCmd75(FILE *fp);
static void edat_ValGetCmd6b(FILE *fp);
static void edat_ValGetSetting(FILE *fp);

static void edat_ValSetConst(FILE *fp);
static void edat_ValSetSetVal(FILE *fp);
static void edat_ValSetPv(FILE *fp);
static void edat_ValSetMachInf(FILE *fp);
static void edat_ValSetSetInf(FILE *fp);
static void edat_ValSetNetWorkSet(FILE *fp);
static void edat_ValSetCommSte(FILE *fp);
static void edat_ValSetRunSet(FILE *fp);
static void edat_ValSetEnl(FILE *fp);
static void edat_ValSetMonitor(FILE *fp);
static void edat_ValSetFwUpde(FILE *fp);
static void edat_ValSetMaint(FILE *fp);
static void edat_ValSetCmd65(FILE *fp);
static void edat_ValSetCmd75(FILE *fp);
static void edat_ValSetCmd6b(FILE *fp);
static void edat_ValSetSetting(FILE *fp);

#endif /*__EDAT_VAL_H__*/
