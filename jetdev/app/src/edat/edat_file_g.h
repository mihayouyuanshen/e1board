/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Cao.HZ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/05/04												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EDAT_FILE_G_H__
#define __EDAT_FILE_G_H__
/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include "typedef.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define LINELENGTH		(ushort)(512)
#define KEYLENGTH		(ushort)(32)
#define VALUELENGTH		(ushort)(256)
/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
enum{
	EDAT_RET_OK,
	EDAT_RET_NG
};
/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern slong EDAT_IniparserLoad(const FILE *fp, const schar *key, schar *value);
extern void EDAT_IniDumpLine(const FILE *fp, const schar *key, const schar *value);
extern void EDAT_IniFileCopy(const schar *dest, const schar *src);

extern void EDAT_GetIniShortValue(FILE * fp, schar * key, ushort * value, ushort def);
extern void EDAT_GetIniStringValue(FILE * fp, schar * key, schar * value, schar * def);
extern void EDAT_GetIniCharValue(FILE * fp, schar * key, uchar * value, uchar def);
extern void EDAT_GetIniFloatValue(FILE * fp, schar * key, float * value, float def);

extern void EDAT_SetIniShortValue(FILE * fp, schar * key, ushort value);
extern void EDAT_SetIniStringValue(FILE * fp, schar * key, schar * value);
extern void EDAT_SetIniCharValue(FILE * fp, schar * key, uchar value);
extern void EDAT_SetIniFloatValue(FILE * fp, schar * key, float value);

#endif /*__EDAT_FILE_G_H__*/
