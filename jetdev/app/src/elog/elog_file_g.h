/****************************************************************************/
/*	Description		:ログ管理ミドル メッセージ処理								*/
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
#ifndef __ELOG_FILE_G_H__
#define __ELOG_FILE_G_H__
#include "typedef.h"
#include "elog_g.h"
#include "elog_val_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define ELOG_CONF_FILENAME		"/mnt/user/elog/elog.inf"
#define ELOG_BUF_SIZE			((uchar)24u)

/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern bool_t ELOG_FileReadConfig(void);
extern bool_t ELOG_FileWriteConfig(void);


#endif /* __ELOG_FILE_G_H__ */
