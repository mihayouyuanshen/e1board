/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Chen.H											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/30												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __ESYS_MOD_H__
#define __ESYS_MOD_H__
#include "typedef.h"
#include "frm_g.h"
#include "task_g.h"
#include "esys_g.h"
#include "einv_g.h"
#include "ekey_g.h"
#include "eled_g.h"
#include "eeep_g.h"
#include "enet_g.h"
#include "ermc_g.h"
#include "etest_g.h"
#include "etim_g.h"
#include "elog_g.h"
#include "edat_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
/* 启动阶段一（启动必要基本模块）模块数量 */
#define ESYS_START_STAGE_1_MOD_NUM				((ulong)6u)
/* 启动阶段二（启动依赖启动阶段一完成的模块）模块数量 */
#define ESYS_START_STAGE_2_MOD_NUM				((ulong)1u)
/* 启动阶段三（启动依赖启动阶段一、二完成的模块）模块数量 */
#define ESYS_START_STAGE_3_MOD_NUM				((ulong)1u)
/* 运行阶段模块数量 */
#define ESYS_RUNNING_STAGE_MOD_NUM	(MOD_NUM - ESYS_START_STAGE_1_MOD_NUM \
									- ESYS_START_STAGE_2_MOD_NUM \
									- ESYS_START_STAGE_3_MOD_NUM - (ulong)1u)
/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef struct {
	ulong modId;
	slong (*modStart)(slong fact);
	slong (*modStop)(void);
} ModSchedule_t;

enum {
	MOD_ESYS,
	MOD_EEEP,
	MOD_ETIM,
	MOD_EKEY,
	MOD_ELED,
	MOD_EDAT,
	MOD_EINV,
	MOD_ELOG,
	MOD_ERMC,
	MOD_ETEST,
	MOD_ENET,
	MOD_NUM
};
/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/
extern const ModDesc_t g_esysModMgrTbl[MOD_NUM];
extern const ModSchedule_t g_esysModSchTbl[MOD_NUM];

/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/


#endif /*__ESYS_MOD_H__*/
