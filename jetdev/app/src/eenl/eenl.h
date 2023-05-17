/****************************************************************************/
/*	Description		:ENL通信ミドル 非公開ヘッダ								*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC DX.zhao											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/29												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、ENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:ECHONET Lite通信で行う処理								*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/

#ifndef __EENL_H__		/* 二重インクルード防止 */
#define __EENL_H__

/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開プロトタイプ宣言													*/
/*--------------------------------------------------------------------------*/
static void eenl_ThreadDevice(EenlDeviceInfo_t* info);
static void eenl_CreateDevice(EPCS_REG_INFO_T regInfo);
static void eenl_InitDevList(void);
#if 0
static slong eenl_SetEpcsEvent(EenlDeviceInfo_t* info);
static void eenl_SetEschEvent(EenlDeviceInfo_t* info);
#endif
static slong eenl_SetEpcsBattery(EenlDeviceInfo_t* info);
static void eenl_EvtSttsChangeReq(EenlEvtSttsChangeReq_t *fmsg);
static void eenl_EvtSpChangeReq(EenlEvtSpChangeReq_t* fmsg);
static void eenl_EvtV2hSttsChangeReq(EenlEvtV2hSttsChangeReq_t* fmsg);
static void eenl_EvtV2hSettingChangeReq(EenlEvtV2hSttingChangeReq_t* fmsg);
static void eenl_EschReq(EenlEvtSttingChangeReq_t* fmsg);
static void eenl_MsgProcSync(FrmMsgHead_t const* msg);
static void eenl_MsgProcAsync(FrmMsgHead_t const* msg);
static void eenl_MsgProcEvent(FrmMsgHead_t const* msg);
static void eenl_ConfirmEnetEvent(FrmMsgHead_t const* msg);
static void eenl_ConfirmTestRunEvent(FrmMsgHead_t const* msg);
static void eenl_ExecuteEnetEvent(FrmMsgHead_t const* msg);
static slong eenl_GetLocation(EenlGetLocation_t* fmsg);
static slong eenl_SetLocation(EenlSetLocation_t* fmsg);
static void eenl_ConfirmInit(void);
static void eenl_ConfirmStart(void);
static void eenl_ConfirmStartMsgProc(FrmMsgHead_t const* msg);
static void eenl_ExecuteInit(void);
static void eenl_ExecuteStartMsgProc(FrmMsgHead_t const* msg);


#endif	/* __EENL_H__ */


