/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jingtong.li										*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/14												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EDAT_G_H__
#define __EDAT_G_H__

#include <lfw_g.h>
#include <frm_g.h>
#include <typedef.h>
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define EDAT_ERROR_CODE_RAT_LEN				((uchar)1u)
#define EDAT_ERROR_CODE_LEN					((uchar)8u)

#define EDAT_OK								((slong)0)
#define EDAT_ERROR							((slong)-1)
/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef enum {
	EERR_TYPE_PCS,
	EERR_TYPE_BOARD,
	EERR_TYPE_RMC,
	EERR_TYPE_NO_ERROR,
	EERR_TYPE_MAX
} EerrType_t;

typedef struct {
	EerrType_t type;								/* 错误类型 */
	schar errCode[EDAT_ERROR_CODE_LEN];				/* 错误代码 */
	schar errCodeRating;							/* 错误等级 */
} EdatErrInfo_t;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern slong EDAT_SetBoardErr(schar const *errCode, schar errCodeRating);
extern slong EDAT_SetPcsErr(schar const *errCode, schar errCodeRating);
extern slong EDAT_SetRmcErr(schar const *errCode, schar errCodeRating);
extern slong EDAT_ClearBoardErr(schar const *errCode);
extern slong EDAT_ClearPcsErr(schar const *errCode);
extern slong EDAT_ClearRmcErr(schar const *errCode);
extern slong EDAT_ClearAllBoardErr(void);
extern slong EDAT_ClearAllPcsErr(void);
extern slong EDAT_ClearAllRmcErr(void);
extern slong EDAT_ClearAllErr(void);

extern slong EDAT_GetErrInfo(EdatErrInfo_t *info);
extern slong EDAT_GetBoardErrInfo(EdatErrInfo_t *info);
extern slong EDAT_GetPcsErrInfo(EdatErrInfo_t *info);
extern slong EDAT_GetRmcErrInfo(EdatErrInfo_t *info);

extern slong EDAT_FindErrByPrefix(EerrType_t type, const schar *prefix);

extern void EDAT_Init(void);
extern void EDAT_Fin(void);
extern void EDAT_MsgProc(FrmMsgHead_t const *msg);
extern slong EDAT_Start(slong fact);
extern slong EDAT_Stop(void);

/* init func */
void EDAT_ValFileReset(void);

/* 構成 */
extern uchar EDAT_ValIfConstGetExtSunPwrOrnot(void);
extern uchar EDAT_ValIfConstGetSunBydPwrGenDev(void);
extern uchar EDAT_ValIfConstGetLoadConnType(void);

extern void EDAT_ValIfConstSetExtSunPwrOrnot(uchar data);
extern void EDAT_ValIfConstSetSunBydPwrGenDev(uchar data);
extern void EDAT_ValIfConstSetLoadConnType(uchar data);

/* 設定値 */
extern uchar EDAT_ValIfSetValGetPushUpSet(void);
extern uchar EDAT_ValIfSetValGetOutputConObj(void);
extern ushort EDAT_ValIfSetValGetEptAppCap(void);
extern ushort EDAT_ValIfSetValGetGenPwrUpLmt(void);
extern uchar EDAT_ValIfSetValGetCliAction(void);
extern uchar EDAT_ValIfSetValGetFitCtatType(void);
extern uchar EDAT_ValIfSetValGetSelfCseType(void);
extern void  EDAT_ValIfSetValGetPwrHouseId(schar *data);
extern uchar EDAT_ValIfSetValGetPwrCompServer(void);

extern void EDAT_ValIfSetValSetPushUpSet(uchar data);
extern void EDAT_ValIfSetValSetOutputConObj(uchar data);
extern void EDAT_ValIfSetValSetEptAppCap(ushort data);
extern void EDAT_ValIfSetValSetGenPwrUpLmt(ushort data);
extern void EDAT_ValIfSetValSetCliAction(uchar data);
extern void EDAT_ValIfSetValSetFitCtatType(uchar data);
extern void EDAT_ValIfSetValSetSelfCseType(uchar data);
extern void EDAT_ValIfSetValSetPwrHouseId(schar *data);
extern void EDAT_ValIfSetValSetPwrCompServer(uchar data);

/* 太陽光発電設定 */
extern void EDAT_ValIfPvGetFixShdSrtDate(schar *data);
extern ushort EDAT_ValIfPvGetSunPwrRated(void);
extern ushort EDAT_ValIfPvGetPvPanelRating(void);

extern void EDAT_ValIfPvSetFixShdSrtDate(schar *data);
extern void EDAT_ValIfPvSetSunPwrRated(ushort data);
extern void EDAT_ValIfPvSetPvPanelRating(ushort data);

/* 機器情報 */
extern void EDAT_ValIfMhcInfGetSysMdlName(schar *data);
extern void EDAT_ValIfMhcInfGetComMdlName(schar *data);
extern void EDAT_ValIfMhcInfGetComSerialNum(schar *data);
extern void EDAT_ValIfMhcInfGetComBoardFwVer(schar *data);
extern void EDAT_ValIfMhcInfGetOpRmtCtlFw(schar *data);
extern uchar EDAT_ValIfMhcInfGetSunPweSetLctn(void);
extern uchar EDAT_ValIfMhcInfGetBtSetLctn(void);
extern uchar EDAT_ValIfMhcInfGetV2hStdSetLctn(void);
extern uchar EDAT_ValIfMhcInfGetElectWaterHeat(void);
extern void EDAT_ValIfMhcInfGetPostNum(schar *data);
extern void EDAT_ValIfMhcInfGetEchonetLite(schar *data);
extern uchar EDAT_ValIfMhcInfGetOpRmtCtlOrnot(void);

extern void EDAT_ValIfMhcInfSetSysMdlName(schar *data);
extern void EDAT_ValIfMhcInfSetComMdlName(schar *data);
extern void EDAT_ValIfMhcInfSetComSerialNum(schar *data);
extern void EDAT_ValIfMhcInfSetComBoardFwVer(schar *data);
extern void EDAT_ValIfMhcInfSetOpRmtCtlFw(schar *data);
extern void EDAT_ValIfMhcInfSetSunPweSetLctn(uchar data);
extern void EDAT_ValIfMhcInfSetBtSetLctn(uchar data);
extern void EDAT_ValIfMhcInfSetV2hStdSetLctn(uchar data);
extern void EDAT_ValIfMhcInfSetElectWaterHeat(uchar data);
extern void EDAT_ValIfMhcInfSetPostNum(schar *data);
extern void EDAT_ValIfMhcInfSetEchonetLite(schar *data);
extern void EDAT_ValIfMhcInfSetOpRmtCtlOrnot(uchar data);

/* 設定情報 */
extern ushort EDAT_ValIfSetInfGetBtCharPwrMaxValSet(void);
extern ushort EDAT_ValIfSetInfGetCarCharPwrValSet(void);
extern uchar EDAT_ValIfSetInfGetCarType(void);
extern uchar EDAT_ValIfSetInfGetCarCharLmt(void);

extern void EDAT_ValIfSetInfSetBtCharPwrMaxValSet(ushort data);
extern void EDAT_ValIfSetInfSetCarCharPwrValSet(ushort data);
extern void EDAT_ValIfSetInfSetCarType(uchar data);
extern void EDAT_ValIfSetInfSetCarCharLmt(uchar data);

/* ネットワーク設定 */
extern uchar EDAT_ValIfNwsGetIpAddrSet(void);
extern void EDAT_ValIfNwsGetIpAddress(schar *data);
extern void EDAT_ValIfNwsGetSubMask(schar *data);
extern void EDAT_ValIfNwsGetDftGateway(schar *data);
extern void EDAT_ValIfNwsGetPtyDns(schar *data);
extern void EDAT_ValIfNwsGetRleDns(schar *data);
extern uchar EDAT_ValIfNwsGetProxySet(void);
extern void EDAT_ValIfNwsGetPxyAddress(schar *data);
extern void EDAT_ValIfNwsGetPortNum(schar *data);

extern void EDAT_ValIfNwsSetIpAddrSet(uchar data);
extern void EDAT_ValIfNwsSetIpAddress(schar *data);
extern void EDAT_ValIfNwsSetSubMask(schar *data);
extern void EDAT_ValIfNwsSetDftGateway(schar *data);
extern void EDAT_ValIfNwsSetPtyDns(schar *data);
extern void EDAT_ValIfNwsSetRleDns(schar *data);
extern void EDAT_ValIfNwsSetProxySet(uchar data);
extern void EDAT_ValIfNwsSetPxyAddress(schar *data);
extern void EDAT_ValIfNwsSetPortNum(schar *data);

/* 通信状態 */
extern void EDAT_ValIfComSteGetUpdeShdDate(schar *data);

extern void EDAT_ValIfComSteSetUpdeShdDate(schar *data);

/* 運転設定 */
extern void EDAT_ValIfRunSetGetSellModeCharStartTime(schar *data);
extern void EDAT_ValIfRunSetGetSellModeCharStopTime(schar *data);
extern void EDAT_ValIfRunSetGetSellModeDisCStartTime(schar *data);
extern void EDAT_ValIfRunSetGetSellModeDiscStopTime(schar *data);
extern void EDAT_ValIfRunSetGetGenModeCharStartTime(schar *data);
extern void EDAT_ValIfRunSetGetGenModeCharStopTime(schar *data);
extern void EDAT_ValIfRunSetGetGenModeDisCStartTime(schar *data);
extern void EDAT_ValIfRunSetGetGenModeDiscStopTime(schar *data);

extern void EDAT_ValIfRunSetSetSellModeCharStartTime(schar *data);
extern void EDAT_ValIfRunSetSetSellModeCharStopTime(schar *data);
extern void EDAT_ValIfRunSetSetSellModeDisCStartTime(schar *data);
extern void EDAT_ValIfRunSetSetSellModeDiscStopTime(schar *data);
extern void EDAT_ValIfRunSetSetGenModeCharStartTime(schar *data);
extern void EDAT_ValIfRunSetSetGenModeCharStopTime(schar *data);
extern void EDAT_ValIfRunSetSetGenModeDisCStartTime(schar *data);
extern void EDAT_ValIfRunSetSetGenModeDiscStopTime(schar *data);

/* ENL関連 */
extern uchar EDAT_ValIfEnlGetBtCharStyle(void);
extern uchar EDAT_ValIfEnlGetBtDisCStyle(void);
extern uchar EDAT_ValIfEnlGetV2hCharStyle(void);
extern uchar EDAT_ValIfEnlGetV2hDisCStyle(void);
extern ushort EDAT_ValIfEnlGetBtCharPwrSetVal(void);
extern ushort EDAT_ValIfEnlGetBtDisCPweSetVal(void);
extern ushort EDAT_ValIfEnlGetV2hCharPwrSetVal(void);
extern ushort EDAT_ValIfEnlGetV2hDisCPweSetVal(void);
extern ushort EDAT_ValIfEnlGetBtAcCharQtySetVal(void);
extern ushort EDAT_ValIfEnlGetBtAcDisCQtySetVal(void);
extern ushort EDAT_ValIfEnlGetBtDcCharQtySetVal(void);
extern ushort EDAT_ValIfEnlGetBtDcDiscQtySetVal(void);
extern ushort EDAT_ValIfEnlGetV2hDcCharQtySetVal(void);
extern ushort EDAT_ValIfEnlGetV2hDcDiscQtySetVal(void);

extern void EDAT_ValIfEnlSetBtCharStyle(uchar data);
extern void EDAT_ValIfEnlSetBtDisCStyle(uchar data);
extern void EDAT_ValIfEnlSetV2hCharStyle(uchar data);
extern void EDAT_ValIfEnlSetV2hDisCStyle(uchar data);
extern void EDAT_ValIfEnlSetBtCharPwrSetVal(ushort data);
extern void EDAT_ValIfEnlSetBtDisCPweSetVal(ushort data);
extern void EDAT_ValIfEnlSetV2hCharPwrSetVal(ushort data);
extern void EDAT_ValIfEnlSetV2hDisCPweSetVal(ushort data);
extern void EDAT_ValIfEnlSetBtAcCharQtySetVal(ushort data);
extern void EDAT_ValIfEnlSetBtAcDisCQtySetVal(ushort data);
extern void EDAT_ValIfEnlSetBtDcCharQtySetVal(ushort data);
extern void EDAT_ValIfEnlSetBtDcDiscQtySetVal(ushort data);
extern void EDAT_ValIfEnlSetV2hDcCharQtySetVal(ushort data);
extern void EDAT_ValIfEnlSetV2hDcDiscQtySetVal(ushort data);

/* 見守り関連 */
extern uchar EDAT_ValIfMtrGetActLogCollCyc(void);

extern void EDAT_ValIfMtrSetActLogCollCyc(uchar data);

/* FW更新関連 */
extern uchar EDAT_ValIfFwUpdeGetUpdeLed(void);
extern ushort EDAT_ValIfFwUpdeGetUpdeNo(void);
extern uchar EDAT_ValIfFwUpdeGetAnalyRes(void);
extern void EDAT_ValIfFwUpdeGetUpdeTime(schar *data);
extern uchar EDAT_ValIfFwUpdeGetBoardTgt(void);
extern uchar EDAT_ValIfFwUpdeGetInvTgt(void);
extern uchar EDAT_ValIfFwUpdeGetPvTgt(void);
extern uchar EDAT_ValIfFwUpdeGetBdcTgt(void);
extern uchar EDAT_ValIfFwUpdeGetJbTgt(void);

extern void EDAT_ValIfFwUpdeSetUpdeLed(uchar data);
extern void EDAT_ValIfFwUpdeSetUpdeNo(ushort data);
extern void EDAT_ValIfFwUpdeSetAnalyRes(uchar data);
extern void EDAT_ValIfFwUpdeSetUpdeTime(schar *data);
extern void EDAT_ValIfFwUpdeSetBoardTgt(uchar data);
extern void EDAT_ValIfFwUpdeSetInvTgt(uchar data);
extern void EDAT_ValIfFwUpdeSetPvTgt(uchar data);
extern void EDAT_ValIfFwUpdeSetBdcTgt(uchar data);
extern void EDAT_ValIfFwUpdeSetJbTgt(uchar data);

/* メンテナンス関連 */
extern void EDAT_ValIfMaintGetMentDate(schar *data);
extern void EDAT_ValIfMaintGetImpDesDay1(schar *data);
extern void EDAT_ValIfMaintGetImpDesDay2(schar *data);
extern uchar EDAT_ValIfMaintGetExecuteMode(void);
extern uchar EDAT_ValIfMaintGetMaintRes(void);

extern void EDAT_ValIfMaintSetMentDate(schar *data);
extern void EDAT_ValIfMaintSetImpDesDay1(schar *data);
extern void EDAT_ValIfMaintSetImpDesDay2(schar *data);
extern void EDAT_ValIfMaintSetExecuteMode(uchar data);
extern void EDAT_ValIfMaintSetMaintRes(uchar data);

/* CMD65 */
extern uchar EDAT_ValIfCmd65GetCommBoardMgeMode(void);
extern uchar EDAT_ValIfCmd65GetEmergencyMode(void);
extern ushort EDAT_ValIfCmd65GetBtUpLmt(void);
extern ushort EDAT_ValIfCmd65GetBtLowLmt(void);

extern void EDAT_ValIfCmd65SetCommBoardMgeMode(uchar data);
extern void EDAT_ValIfCmd65SetEmergencyMode(uchar data);
extern void EDAT_ValIfCmd65SetBtUpLmt(ushort data);
extern void EDAT_ValIfCmd65SetBtLowLmt(ushort data);

/* CMD75 */
extern uchar EDAT_ValIfCmd75GetCarCharBodyOprat(void);
extern uchar EDAT_ValIfCmd75GetPwrcutDischar(void);
extern uchar EDAT_ValIfCmd75GetPwrcutDiscPrySet(void);
extern uchar EDAT_ValIfCmd75GetPwrcutCharPrySet(void);
extern ushort EDAT_ValIfCmd75GetCarDiscLowLmtSet(void);
extern ushort EDAT_ValIfCmd75GetCarCharUpLmtSet(void);
extern uchar EDAT_ValIfCmd75GetCarDiscOrnot(void);
extern uchar EDAT_ValIfCmd75GetDiscPrySet(void);
extern ushort EDAT_ValIfCmd75GetPwrDiscLowLmtSet(void);
extern uchar EDAT_ValIfCmd75GetCharPrySet(void);

extern void EDAT_ValIfCmd75SetCarCharBodyOprat(uchar data);
extern void EDAT_ValIfCmd75SetPwrcutDischar(uchar data);
extern void EDAT_ValIfCmd75SetPwrcutDiscPrySet(uchar data);
extern void EDAT_ValIfCmd75SetPwrcutCharPrySet(uchar data);
extern void EDAT_ValIfCmd75SetCarDiscLowLmtSet(ushort data);
extern void EDAT_ValIfCmd75SetCarCharUpLmtSet(ushort data);
extern void EDAT_ValIfCmd75SetCarDiscOrnot(uchar data);
extern void EDAT_ValIfCmd75SetDiscPrySet(uchar data);
extern void EDAT_ValIfCmd75SetPwrDiscLowLmtSet(ushort data);
extern void EDAT_ValIfCmd75SetCharPrySet(uchar data);

/* CMD6B */
extern ushort EDAT_ValIfCmd6bGetContPwrSet(void);
extern uchar EDAT_ValIfCmd6bGetPwrcutOutSet(void);
extern uchar EDAT_ValIfCmd6bGetPwrcutOutStart(void);
extern uchar EDAT_ValIfCmd6bGetSellValOrInval(void);

extern void EDAT_ValIfCmd6bSetContPwrSet(ushort data);
extern void EDAT_ValIfCmd6bSetPwrcutOutSet(uchar data);
extern void EDAT_ValIfCmd6bSetPwrcutOutStart(uchar data);
extern void EDAT_ValIfCmd6bSetSellValOrInval(uchar data);

/* 整定値 */
extern ushort EDAT_ValIfStgGetSysVolDetLvl(void);
extern ushort EDAT_ValIfStgGetSysVolDetTime(void);
extern ushort EDAT_ValIfStgGetSysUdrVolDetLvl(void);
extern ushort EDAT_ValIfStgGetSysUdrVolDetTime(void);
extern uchar EDAT_ValIfStgGetFreRiseDetLvl(void);
extern ushort EDAT_ValIfStgGetFreRiseDetTime(void);
extern uchar EDAT_ValIfStgGetFreDropDetLvl(void);
extern ushort EDAT_ValIfStgGetFreDropDetTime(void);
extern ushort EDAT_ValIfStgGetResctTime(void);
extern ushort EDAT_ValIfStgGetVolRsInhbVal(void);
extern ushort EDAT_ValIfStgGetVolRsInhbLmt(void);
extern ushort EDAT_ValIfStgGetCotPwrCtl(void);
extern ushort EDAT_ValIfStgGetPassIndOperDet(void);
extern ushort EDAT_ValIfStgGetFreFfbkGain(void);
extern ushort EDAT_ValIfStgGetFreLevAllow(void);
extern ushort EDAT_ValIfStgGetChgTimeIncDes(void);

extern void EDAT_ValIfStgSetSysVolDetLvl(ushort data);
extern void EDAT_ValIfStgSetSysVolDetTime(ushort data);
extern void EDAT_ValIfStgSetSysUdrVolDetLvl(ushort data);
extern void EDAT_ValIfStgSetSysUdrVolDetTime(ushort data);
extern void EDAT_ValIfStgSetFreRiseDetLvl(uchar data);
extern void EDAT_ValIfStgSetFreRiseDetTime(ushort data);
extern void EDAT_ValIfStgSetFreDropDetLvl(uchar data);
extern void EDAT_ValIfStgSetFreDropDetTime(ushort data);
extern void EDAT_ValIfStgSetResctTime(ushort data);
extern void EDAT_ValIfStgSetVolRsInhbVal(ushort data);
extern void EDAT_ValIfStgSetVolRsInhbLmt(ushort data);
extern void EDAT_ValIfStgSetCotPwrCtl(ushort data);
extern void EDAT_ValIfStgSetPassIndOperDet(ushort data);
extern void EDAT_ValIfStgSetFreFfbkGain(ushort data);
extern void EDAT_ValIfStgSetFreLevAllow(ushort data);
extern void EDAT_ValIfStgSetChgTimeIncDes(ushort data);

#endif /*__EDAT_G_H__*/
