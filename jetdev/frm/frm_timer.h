/****************************************************************************/
/*	Description		:Timer框架私有头文件									*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC yufeng.yang										*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/14												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:frm_timer												*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __FRM_TIMER_H__
#define __FRM_TIMER_H__

#include "frm_g.h"
#include "frm_msg_g.h"

/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define MOD_TIMER					((ulong)ETIMER_SPORT)
#define FRM_TIMER_MEMORY_MAX_CNT	((ulong)100u)
#define FRM_TIMER_LOAD_MEASURE_DIV	((ulong)10u)

/* 监控Timer的时间间隔 */
#define FRM_TIMER_INTERVAL_MS		((slong)10)
/* 1日の秒数：3600sec * 24H */
#define	FRM_TIMER_DAYSEC_SZ			((slong)86400)
/* 1日の秒数へ変換 */
#define	FRM_TIMER_DAYSEC(n)			((slong)(n) % FRM_TIMER_DAYSEC_SZ)
/* msec->nsec変換 */
#define	FRM_TIMER_M_NSEC(n)			((slong)(n)*1000000)
/* msec->usec変換 */
#define	FRM_TIMER_M_USEC(n)			((slong)(n)*1000)


/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef struct frm_timer_reg_t {
	ulong destModId;				/* 返信メッセージID */
	ulong id;						/* ワンショットタイマー登録ID */
	FrmTimerOpt opt;
	struct timespec expectTime;		/* ワンショットタイマー予定時間 */
	struct frm_timer_reg_t *prev;	/* 前リストのポインタ */
	struct frm_timer_reg_t *next;	/* 次リストのポインタ */
} FrmTimerReg_t;

typedef struct frm_alarm_reg_t {
	ulong destModId;				/* 返信メッセージID */
	ulong id;						/* アラーム登録ID */
	FrmTimerOpt opt;
	struct timespec expectTime;		/* アラーム予定時刻 */
	struct frm_alarm_reg_t *prev;	/* 前リストのポインタ */
	struct frm_alarm_reg_t *next;	/* 次リストのポインタ */
} FrmAlarmReg_t;

typedef union {
	FrmTimerReg_t timerReg;
	FrmAlarmReg_t alarmReg;
} FrmTimerRegEntity_t;

typedef struct {
	FrmTimerRegEntity_t regEntity;
	uchar isUsing;
} FrmTimerMemory_t;

/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
static slong frm_FrmTime2Tm(const FrmTime_t *pTime, struct tm *ptm);
static slong frm_CheckDateRange(const struct tm *ptm);
static void *frm_WatchThread(void *arg);
static void *frm_TimerMalloc(ulong size);
static void frm_TimerFree(void *ptr);
#endif /*__FRM_TIMER_H__*/
