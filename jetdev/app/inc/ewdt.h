/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Chen.H											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/28												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:EWDT													*/
/*--------------------------------------------------------------------------*/
/*	Note			:WATCHDOG監視プロセスヘッダ									*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EWDT_H__
#define __EWDT_H__
#include "frm_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
/***********************************************************
* @def		EWDT_PROC_TIMEOUT_CNT
* @brief	常駐プロセスタイムアウトカウント(秒)
************************************************************/
#define EWDT_PROC_TIMEOUT_DEF	(60)
#define EWDT_OK					(0)
#define EWDT_ERROR				(-1)
/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
/***********************************************************
* @enum		EwdtMsgId_t
* @brief	ファンクションID
************************************************************/
typedef enum {
	EWDT_MSGID_WDT_START = 0,	/* WDT開始通知 */
	EWDT_MSGID_WDT_RES,			/* WDT応答通知 */
	EWDT_MSGID_TERM_REQ,		/* 終了要求 */
	EWDT_MSGID_TERM_RES			/* 終了応答通知 */
} EwdtMsgId_t;

/***********************************************************
* @enum		EwdtTermType_t
* @brief	終了種別
************************************************************/
typedef enum {
	EWDT_TERM_TYPE_NONE = 0,	/* 終了処理未実行 */
	EWDT_TERM_TYPE_REBOOT,		/* リブート */
	EWDT_TERM_TYPE_SHUTDOWN		/* 電源OFF */
} EwdtTermType_t;

/***********************************************************
* @struct	EwdtFncWdtRes_t
* @brief	WDT応答通知メッセージ構造
************************************************************/
typedef struct {
	FrmMsgHead_t Header;	/* メッセージヘッダ */
	uchar Timeout;			/* タイムアウト時間 */
} EwdtFncWdtRes_t;

/***********************************************************
* @struct	EwdtFncTermReq_t
* @brief	終了要求メッセージ構造
************************************************************/
typedef struct {
	FrmMsgHead_t Header;	/* メッセージヘッダ */
	EwdtTermType_t Type;	/* 終了種別 */
} EwdtFncTermReq_t;

/***********************************************************
* @struct	EwdtFncTermRes_t
* @brief	終了応答通知メッセージ構造
************************************************************/
typedef struct {
	FrmMsgHead_t Header;	/* メッセージヘッダ */
	EwdtTermType_t Type;	/* 終了種別 */
} EwdtFncTermRes_t;

/***********************************************************
* @enum		EwdtEvtEnum_t
* @brief	イベントID
************************************************************/
typedef enum {
	EWDT_EVT_WDT_REQ = 0,	/* WDT応答要求 */
	EWDT_EVT_TERM_REQ		/* 終了要求 */
} EwdtEvtEnum_t;

/***********************************************************
* @struct	EwdtEvtWdtReq_t
* @brief	WDT応答要求メッセージ構造
************************************************************/
typedef struct _EwdtEvtWdtReq_t{
	FrmMsgHead_t Header;	/* メッセージヘッダ */
} EwdtEvtWdtReq_t;

/***********************************************************
* @struct	EwdtEvtTermReq_t
* @brief	終了要求メッセージ構造
************************************************************/
typedef struct {
	FrmMsgHead_t Header;	/* メッセージヘッダ */
	EwdtTermType_t Type;	/* 終了種別 */
	ulong Sport;			/* 要求元SPORT */
} EwdtEvtTermReq_t;

/***********************************************************
* @union	EwdtFncMsg_t
* @brief	ファンクションメッセージ共用体
************************************************************/
typedef union {
	FrmMsgHead_t Header;			/* メッセージヘッダ */
	EwdtFncWdtRes_t FncWdtRes;		/* WDT応答通知メッセージ */
	EwdtFncTermReq_t FncTermReq;	/* 終了要求メッセージ */
	EwdtFncTermRes_t FncTermRes;	/* 終了応答通知メッセージ */
} EwdtFncMsg_t;

/***********************************************************
* @union	EwdtEvtUnion_t
* @brief	イベントメッセージ共用体
************************************************************/
typedef union {
	FrmMsgHead_t Header;			/* メッセージヘッダ */
	EwdtEvtWdtReq_t EvtWdtReq;		/* WDT応答要求メッセージ */
	EwdtEvtTermReq_t EvtTermReq;	/* 終了要求メッセージ */
} EwdtEvtUnion_t;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

extern void EWDT_Init(void);
extern void EWDT_Fin(void);
extern void EWDT_MsgProc(FrmMsgHead_t const *msg);
extern slong EWDT_Start(slong fact);
extern slong EWDT_Stop(void);

extern slong EWDT_AsyncSendWdtRes(uchar timeout);
extern slong EWDT_AsyncSendRebootReq(void);
extern slong EWDT_AsyncSendTermRes(EwdtTermType_t type);

#ifdef __cplusplus
}
#endif

#endif /*__EWDT_H__*/