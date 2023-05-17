/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Xu.Y												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/26												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __DLOG_G_H__
#define __DLOG_G_H__

#include "typedef.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
/* 默认log级别 */
#define DLOG_LOG_LEVEL_DEFAULT	(LOG_LEVEL_DEBUG)

#define DLOG_OK	((slong)0)
#define DLOG_ERR	((slong)-1)

#define DLOG_Error(fmt, ...)	DLOG_Log(LOG_LEVEL_ERROR, \
										"[ERR][%s:%d]" fmt "\n", \
										__FILE__, __LINE__, ##__VA_ARGS__)
#define DLOG_Warn(fmt, ...)		DLOG_Log(LOG_LEVEL_WARN, \
										"[WRN][%s:%d]" fmt "\n", \
										__FILE__, __LINE__, ##__VA_ARGS__)
#define DLOG_Info(fmt, ...)		DLOG_Log(LOG_LEVEL_INFO, \
										"[INF][%s:%d]" fmt "\n", \
										__FILE__, __LINE__, ##__VA_ARGS__)
#define DLOG_Debug(fmt, ...)	DLOG_Log(LOG_LEVEL_DEBUG, \
										"[DBG][%s:%d]" fmt "\n", \
										__FILE__, __LINE__, ##__VA_ARGS__)

#if (defined ENABLE_DEBUG && (0 != ENABLE_DEBUG))
#define DEBUG_TRCIN() DLOG_Log(LOG_LEVEL_DEBUG, \
								"[DBG]""(IN >>) %s : \n",__FUNCTION__)
#define DEBUG_TRCOUT() DLOG_Log(LOG_LEVEL_DEBUG, \
								"[DBG]""(OUT<<) %s : \n", __FUNCTION__)
#else
#define DEBUG_TRCIN()
#define DEBUG_TRCOUT()
#endif

/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef enum
{
	DLOG_RESULT_OK	= 0,
	DLOG_RESULT_NG,
	DLOG_RESULT_EPRM
} DlogResult_t;

typedef enum
{
	DLOG_OUT_MODE_FILE = 0,
	DLOG_OUT_MODE_CONSOLE,
	DLOG_OUT_MODE_MAX
} DlogOutMode_t;

typedef enum {
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_MAX
} DlogLogLevel_t;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern DlogResult_t DLOG_Init(const char *filePath, const char *filePfxName,
	ulong fileMaxSize, ulong fileNum, DlogOutMode_t outMode,
	ulong fileCrtDelay);
extern void DLOG_Log(DlogLogLevel_t level, const char *fmt, ...);

#endif /*__DLOG_G_H__*/
