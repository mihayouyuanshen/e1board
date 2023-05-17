/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Chen.H											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/11												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __ESYS_CMN_G_H__
#define __ESYS_CMN_G_H__
#include "frm_g.h"
#include "esys_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef enum {
	ESYS_MSGID_START = 0,
	ESYS_MSGID_POWER_CUT_ST_CHANGE,
	ESYS_MSGID_ERROR_HAPPEN,
	ESYS_MSGID_ERROR_CANCEL,
	ESYS_MSGID_START_RESULT_NOTIFY,
	ESYS_MSGID_STOP_RESULT_NOTIFY,
	ESYS_MSGID_READ_EEP_ERR_HAPPEN,
	ESYS_MSGID_READ_EEP_ERR_CLEAR,
	ESYS_MSGID_KEY1_PRESSED,
	ESYS_MSGID_KEY1_LONG_PRESSED,
	ESYS_MSGID_KEY2_PRESSED,
	ESYS_MSGID_KEY2_LONG_PRESSED,
	ESYS_MSGID_SW1_ON,
	ESYS_MSGID_SW1_OFF,
	ESYS_MSGID_SW2_ON,
	ESYS_MSGID_SW2_OFF,
	ESYS_MSGID_SW3_ON,
	ESYS_MSGID_SW3_OFF,
	ESYS_MSGID_SW4_ON,
	ESYS_MSGID_SW4_OFF,
	ESYS_MSGID_MAX
} EsysMsgId_t;

/* 按键事件通知消息结构体 */
typedef struct {
	FrmMsgHead_t header;
} EsysKeyNotify_t;

/* 停电状态改变消息结构体 */
typedef struct {
	FrmMsgHead_t header;
	PowerCutOperation_t status;
} EsysPowerCutSt_t;

/* 错误改变消息结构体 */
typedef struct {
	FrmMsgHead_t header;
	EdatErrInfo_t errInfo;
} EsysMsgErrChg_t;

/* 模块动作结果通知消息结构体 */
typedef struct {
	FrmMsgHead_t header;
	ulong modId;
	ModActionResult_t result;
} EsysActRsltNotify_t;

typedef union {
	FrmMsgHead_t header;
} EsysMsgUnion_t;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/


#endif /*__ESYS_CMN_G_H__*/
