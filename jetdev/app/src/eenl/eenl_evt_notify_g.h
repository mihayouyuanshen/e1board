/****************************************************************************/
/*	Description		: ヘッダ												*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jiayu.li											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/17												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、EENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:無し													*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef	__EENL_EVT_NOTIFY_G_H__
#define	__EENL_EVT_NOTIFY_G_H__

extern slong EENL_AllNotify0x80(uchar val0x80);
extern slong EENL_EvtEpcsOpeMode(EenlEvtSttsChangeReq_t* cmsg);
extern slong EENL_EvtEpcsEmgPer(EenlEvtSttsChangeReq_t* cmsg);
extern slong EENL_EvtEpcsChargeTimeZone(EenlEvtSttsChangeReq_t* cmsg);
extern slong EENL_EvtEpcsOpeStts(EenlEvtSttsChangeReq_t* cmsg);
extern slong EENL_EvtEpcsPwroutage(EenlEvtSttsChangeReq_t* cmsg);
extern slong EENL_EvtEpcsSoh(EenlEvtSttsChangeReq_t* cmsg);
extern slong EENL_EvtEpcsSuppressState(EenlEvtSttsChangeReq_t* cmsg);
extern slong EENL_EvtEpcsResetPwrset(EenlEvtSttsChangeReq_t* cmsg);
extern slong EENL_EvtEpcsAcChargeWait(void);
extern slong EENL_EvtEpcsAcDischargeWait(void);
extern slong EENL_EvtEpcsOutputFluctuateSpdChg0xB3(EenlEvtSpChangeReq_t* spmsg);
extern slong EENL_EvtEpcsPwrFactCtrlSpdChg(EenlEvtSpChangeReq_t* spmsg);
extern slong EENL_EvtEpcsGenPwrCapSpdChg(EenlEvtSpChangeReq_t* spmsg);
extern slong EENL_EvtEpcsFitTypeSpdChg(EenlEvtSpChangeReq_t* spmsg);
extern slong EENL_EvtEpcsSelfConsumptionTypeSpdChg(EenlEvtSpChangeReq_t* spmsg);
extern slong EENL_EvtEpcsFacilityCertifiedCapacitySpdChg(EenlEvtSpChangeReq_t* spmsg);
extern slong EENL_EvtEguiTestrunStatus(EenlTestRun_t* fmsg);
extern slong EENL_EvtEpcsV2hConnectorLock(EenlEvtV2hSttsChangeReq_t* v2hsttsmsg);
extern slong EENL_EvtEpcsV2hAvailabilityCharge(EenlEvtV2hSttsChangeReq_t* v2hsttsmsg);
extern slong EENL_EvtEpcsV2hChargeAmount1(EenlEvtV2hSttingChangeReq_t* v2hsettingmsg);
extern slong EENL_EvtEpcsV2hPowerSetCharge(EenlEvtV2hSttingChangeReq_t* v2hsettingmsg);
extern slong EENL_EvtEpcsV2hPowerSetDischarge(EenlEvtV2hSttingChangeReq_t* v2hsettingmsg);
extern slong EENL_EvtEpcsV2hRatedVolt(EenlEvtV2hSttingChangeReq_t* v2hsettingmsg);
extern slong EENL_EvtEpcsV2hDischargeSetValue(EenlEvtV2hSttingChangeReq_t* v2hsettingmsg);
extern slong EENL_EvtEschNextAccessDate(EenlEvtSttingChangeReq_t* spmsg);

#endif	/* __EENL_EVT_NOTIFY_G_H__ */
