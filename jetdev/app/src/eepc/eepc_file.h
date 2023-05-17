/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Xu.Y												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/07												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EEPC_FILE_H__
#define __EEPC_FILE_H__

#include "eepc_com.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
extern EepcFuncResult_t EEPC_FileFncChkAllFiles(void);
extern EepcFuncResult_t EEPC_FileReadTestSrvConnect(void);
extern EepcFuncResult_t EEPC_FileDetailChkUseSrvFile(EepcFileUseSrv_t *useSrv, bool_t *chgFlg);
extern EepcFuncResult_t EEPC_FileSetUsrSrvInf(EepcSetSrv_t *setSrv);
extern EepcFuncResult_t EEPC_FileUpdateComInf(EepcComInf_t *comInf);
extern EepcFuncResult_t EEPC_FileCrtRegChgEvtData(EepcRegChg_t *regChg, schar *writeFilePath);
extern EepcFuncResult_t EEPC_FileCrtClrChgEvtData(EepcClrChg_t *clrChg, schar *writeFilePath);
extern EepcFuncResult_t EEPC_FileUpdateSrvMngFile(void);
/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/
ProcessMutex_t g_eepcMutexComSrv;
ProcessMutex_t g_eepcMutexFileSrvMng;
ProcessMutex_t g_eepcMutexFileComHist;

/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/


#endif /*__EEPC_FILE_H__*/
