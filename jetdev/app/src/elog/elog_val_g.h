/****************************************************************************/
/*	Description		:ログ管理ミドル グローバル変数							*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Jiang.Z											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/05/09												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __ELOG_VAL_G_H__
#define __ELOG_VAL_G_H__

#include "typedef.h"
#include "elog_g.h"
#include "eeep_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define PROCESS_MAX				(ushort)146u			/* 最大プロセス数		*/
#define TASKNAME				(uchar)6u				/* 最大プロセス数		*/

/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef struct ElogVal{
	bool_t logEnabled[PROCESS_MAX];
	ElogLv_t logLevel[PROCESS_MAX];
	bool_t usbInProgress;
}ElogVat_t;
/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
extern ElogVat_t *s_elog;

/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern bool_t ELOG_ValGetEnable(slong msgid);
extern ElogLv_t ELOG_ValGetLevel(slong msgid);
extern slong ELOG_Shmget(void);
#ifndef ELOG_INTERFACE

typedef struct ElogTaskName{
	schar name[TASKNAME];		/* taskname */
	slong sport;				/* SPORT	*/
}ElogTaskName_t;

extern void ELOG_ValInit();
extern void ELOG_ValSetEnable(bool_t enable, slong msgid);
extern void ELOG_ValSetEnableAll(bool_t enable);
extern void ELOG_ValSetLevel(ElogLv_t level, slong msgid);
extern void ELOG_ValSetUsbInProgress(bool_t usbInProgress);
extern bool_t ELOG_ValGetUsbInProgress(void);
extern void ELOG_ValPrintVariable(void);
extern bool_t ELOG_ValGetEnableAll(void);

extern const ElogTaskName_t s_ElogTaskName[PROCESS_MAX];
extern schar s_elogRmcSerialNo[ELOG_SERIES_LEN + 1];
#define ELOG_RMC_SERIALNO_DEF "0000000000000000"
/* デバッグログ出力対象プロセス数*/
/*(s_ElogTaskName[]でタスク名/SPORTが"DUMMY"/0xFFFFFFFF以外に設定されている数) */
#define ELOG_OUTPUT_LOG_PROC_NUM ((uchar)36u)	

#endif /* not ELOG_INTERFACE */

#endif /*__ELOG_VAL_G_H__*/
