/****************************************************************************/
/*	Description		:キーモジュール業務										*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Cao.HZ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/20												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EKEY_SERV_H__
#define __EKEY_SERV_H__
/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "typedef.h"
/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
typedef enum {
	EKEY_TIMERID_SW1 = 0u,
	EKEY_TIMERID_SW2,
	EKEY_TIMERID_A8,
	EKEY_TIMERID_D10,
	EKEY_TIMERID_A14,
	EKEY_TIMERID_K17,
} EKEY_TimerId_t;

typedef enum {
	EKEY_TRG_KEY_PRESS = 0u,
	EKEY_TRG_KEY_RELEASE,
	EKEY_TRG_TIMEOUT,
	EKEY_TRG_CANCEL,
	EKEY_TRG_MAX
} EKEY_Trg_t;

typedef enum {
	EKEY_STS_IDLE = 0u,
	EKEY_STS_PRESS,
	EKEY_STS_LONG_PRESS,
	EKEY_STS_MAX
} EKEY_State_t;

/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef struct {
	EKEY_Btn_t btn;
	EKEY_TimerId_t timerId;
	DGPIO_Port_t gpioPort;
	sint gpioFd;
	EKEY_State_t state;
} EKEY_BtnContext_t;

/*--------------------------------------------------------------------------*/
/*	非公開プロトタイプ宣言													*/
/*--------------------------------------------------------------------------*/
static void ekey_CallBack(EKEY_Btn_t key, EKEY_BtnEvent_t event);
static void ekey_SendBtnMsg(EKEY_Btn_t btn, DGPIO_Val_t gpioVal);
static void* ekey_SubThreadMainLoop(void* arg);
static void ekey_RunStateMachine(EKEY_BtnContext_t *keyContext, EKEY_Trg_t trg);
static void ekey_StopPollLoop(FrmPthread_t threadId);
#endif /*__EKEY_SERV_H__*/

