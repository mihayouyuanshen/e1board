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

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "esys_mod.h"

#define ADD_MOD_MGR(mod) [MOD_##mod] = {mod##_SPORT, 0, mod##_Init, mod##_MsgProc, mod##_Fin}
#define ADD_MOD_SCH(mod) [MOD_##mod] = {mod##_SPORT, mod##_Start, mod##_Stop}
#define ADD_MOD(mod) {mod##_SPORT, mod##_Start, mod##_Stop}
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
const ModDesc_t g_esysModMgrTbl[MOD_NUM] = {
	ADD_MOD_MGR(ESYS),
	ADD_MOD_MGR(EINV),
	ADD_MOD_MGR(EEEP),
	ADD_MOD_MGR(ETIM),
	ADD_MOD_MGR(ELOG),
	ADD_MOD_MGR(EKEY),
	ADD_MOD_MGR(ELED),
	ADD_MOD_MGR(ETEST),
	ADD_MOD_MGR(ERMC),
	ADD_MOD_MGR(ENET),
	ADD_MOD_MGR(EDAT)
};

const ModSchedule_t g_esysModSchTbl[MOD_NUM] = {
	ADD_MOD_SCH(ESYS),
	ADD_MOD_SCH(EINV),
	ADD_MOD_SCH(EEEP),
	ADD_MOD_SCH(ETIM),
	ADD_MOD_SCH(ELOG),
	ADD_MOD_SCH(EKEY),
	ADD_MOD_SCH(ELED),
	ADD_MOD_SCH(ETEST),
	ADD_MOD_SCH(ERMC),
	ADD_MOD_SCH(ENET),
	ADD_MOD_SCH(EDAT)
};


const ModSchedule_t g_esysStartStage1ModTbl[ESYS_START_STAGE_1_MOD_NUM] = {
	ADD_MOD(EEEP),
};


const ModSchedule_t g_esysStartStage2ModTbl[ESYS_START_STAGE_2_MOD_NUM] = {

};

const ModSchedule_t g_esysStartStage3ModTbl[ESYS_START_STAGE_3_MOD_NUM] = {

};

const ModSchedule_t g_esysRunningRelatedModTbl[ESYS_RUNNING_STAGE_MOD_NUM] = {

};
/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*End Of File*/
