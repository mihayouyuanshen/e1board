/****************************************************************************/
/*	Description		:オプションリモコン通信機能 共通ヘッダ					*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC keqi.liang											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/29												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ERMC													*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __ERMC_CMN_G_H__
#define __ERMC_CMN_G_H__

#include <typedef.h>

#include "frm_g.h"
#include "emdtr_g.h"

/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
/* プロセスアクション */
typedef enum {
	ERMC_PROC_ACTION_IDLE			= ((ushort)0u),							/* IDLE */
	ERMC_PROC_ACTION_RECV			= ((ushort)1u),							/* 受信 */
	ERMC_PROC_ACTION_WAIT			= ((ushort)2u),							/* 待ち */
	ERMC_PROC_ACTION_SEND			= ((ushort)3u),							/* 送信 */
	ERMC_PROC_ACTION_MAX			= ((ushort)4u)							/* MAX */
} ProcAction_t;

/* INVマイコン初期化通信完了通知構造体 */
typedef struct {
	FrmMsgHead_t					header;
} InvInitCommFinNotify_t;

/* OPリモコン通信起動要求構造体 */
typedef struct {
	FrmMsgHead_t					header;
	slong							fact;
} RmcCommStartReq_t;

/* OPリモコン通信停止要求構造体 */
typedef struct {
	FrmMsgHead_t					header;
} RmcCommStopReq_t;

/* プロセス状態変更要求 */
typedef struct {
	FrmMsgHead_t					header;
	ProcAction_t					nextAction;
} ProcStatusChangeReq_t;

/* (CMD24)情報取得完了通知構造体 */
typedef struct {
	FrmMsgHead_t					header;
	EmdtrOpemodeGet_t				dataInfo;
} Cmd24InfoGetFinNty_t;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
/* プロセス状態 */
typedef enum {
	ERMC_PROC_STTS_IDLE				= ((ushort)0u),							/* IDLE */
	ERMC_PROC_STTS_RECEIVING		= ((ushort)1u),							/* 電文受信処理中 */
	ERMC_PROC_STTS_WAITING			= ((ushort)2u),							/* 情報取得待ち中 */
	ERMC_PROC_STTS_SENDING			= ((ushort)3u),							/* 電文送信処理中 */
	ERMC_PROC_STTS_MAX				= ((ushort)4u)							/* MAX */
} ProcStatus_t;

/* メッセージID */
typedef enum {
	ERMC_INV_INIT_COMM_FIN_NTY		= ((ushort)0u),							/* INVマイコン初期化通信完了通知 */
	ERMC_RMC_COMM_START_REQ			= ((ushort)1u),							/* OPリモコン通信起動要求 */
	ERMC_RMC_COMM_STOP_REQ			= ((ushort)2u),							/* OPリモコン通信停止要求 */
	ERMC_PROC_STATUS_CHANGE_REQ		= ((ushort)3u),							/* プロセス状態変更要求 */
	ERMC_CMD24_INFO_GET_FIN_NTY		= ((ushort)4u)							/* (CMD24)情報取得完了通知 */
} MsgId_t;

/* タイマID */
typedef enum {
	ERMC_TIMER_ID_RMC_COMM			= ((ushort)0u),							/* OPリモコン通信タイマ */
	ERMC_TIMER_ID_MSG_RES			= ((ushort)10u)							/* 電文応答タイマ */
} TimerId_t;

/* プロセスイベント */
typedef enum {
	ERMC_CMD0_REQ_MSG_RECV_EVENT	= ((ushort)0u),							/* (CMD0)要求電文受信イベント */
	ERMC_CMD24_REQ_MSG_RECV_EVENT	= ((ushort)1u),							/* (CMD24)要求電文受信イベント */
	ERMC_CMD41_REQ_MSG_RECV_EVENT	= ((ushort)2u),							/* (CMD41)要求電文受信イベント */
	ERMC_RES_MSG_SEND_FIN_EVENT		= ((ushort)3u),							/* 応答電文送信終了イベント */
	ERMC_RMC_COMM_TIMER_TO_EVENT	= ((ushort)4u),							/* OPリモコン通信タイマタイムアウトイベント */
	ERMC_MSG_RES_TIMER_TO_EVENT		= ((ushort)5u)							/* 電文応答タイマタイムアウトイベント */
} ProcEvent_t;

/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/


#endif /*__ERMC_CMN_G_H__*/
