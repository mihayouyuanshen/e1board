/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Wang.M												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/20												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __ESYS_FSM_G_H__
#define __ESYS_FSM_G_H__
#include "frm_g.h"

/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define ESYS_FSM_RET_OK		((slong)0)
#define ESYS_FSM_RET_ERR	((slong)-1)

#define TRIGGER_INVALID		((ushort)0xFFFFu)
/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef enum {
	ESYS_ST_KEEP = -1,
	ESYS_ST_IDLE = 0,		/* First state */
	ESYS_ST_STARTING,
	ESYS_ST_RUNNING,
	ESYS_ST_STOPING,
	ESYS_ST_FW_UPDATE,
	ESYS_ST_MAX
} FsmState_t;

typedef enum {
	ESYS_TRIGGER_START = 0,
	ESYS_TRIGGER_START_FINISH,
	ESYS_TRIGGER_START_TIMEOUT,
	ESYS_TRIGGER_KEY_PRESS,
	ESYS_TRIGGER_POWER_CUT_ST_CHANGE,
	ESYS_TRIGGER_ERROR_HAPPEN,
	ESYS_TRIGGER_ERROR_CANCEL,
	ESYS_TRIGGER_SYSTEM_TERMINATE,
	ESYS_TRIGGER_MAX
} EsysTrigger_t;

typedef struct {
	ushort trigger;
	const FrmMsgHead_t *pMsg;
} FsmTrigerInfo_t;

typedef struct {
	FsmState_t targetState;
	void (*action)(const FsmTrigerInfo_t *);
} FsmTrans_t;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern void esys_FsmInit(FsmState_t initState);
extern void esys_FsmSetState(FsmState_t state);
extern slong esys_fsmRun(const FrmMsgHead_t *pMsg);


#endif /*__ESYS_FSM_G_H__*/
