/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jingtong.li										*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/14												*/
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
#include <limits.h>
#include "edat_file.h"
#include "edat_file_g.h"
#include "dlog_g.h"
#include <ctype.h>
#include <string.h>
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void EDAT_GetIniShortValue(FILE * fp, schar * key, ushort * value, ushort def)
{
	schar retS[VALUELENGTH] = {0};
	slong ret;
	ret = EDAT_IniparserLoad(fp, key, retS);
	if(EDAT_RET_OK == ret)
	{
		*value = (ushort)atol(retS);
		DLOG_Debug("Key match.");
	}
	else
	{
		*value = def;
		DLOG_Debug("Key not match. Return default.");
	}
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void EDAT_GetIniStringValue(FILE * fp, schar * key, schar * value, schar * def)
{
	schar retS[VALUELENGTH] = {0};
	slong ret;
	slong len = 0;
	ret = EDAT_IniparserLoad(fp, key, retS);
	if(EDAT_RET_OK == ret)
	{
		len = strlen(retS) + 1;
		strncpy(value, retS, len);
		DLOG_Debug("Key match.");
	}
	else
	{
		len = strlen(def) + 1;
		strncpy(value, def, len);
		DLOG_Debug("Key not match. Return default.");
	}
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/05 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void EDAT_GetIniCharValue(FILE * fp, schar * key, uchar * value, uchar def)
{
	schar retS[VALUELENGTH] = {0};
	slong ret;
	ret = EDAT_IniparserLoad(fp, key, retS);
	if(EDAT_RET_OK == ret)
	{
		*value = (uchar)atol(retS);
		DLOG_Debug("Key match.");
	}
	else
	{
		*value = def;
		DLOG_Debug("Key not match. Return default.");
	}
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/05 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void EDAT_GetIniFloatValue(FILE * fp, schar * key, float * value, float def)
{
	schar retS[VALUELENGTH] = {0};
	slong ret;
	ret = EDAT_IniparserLoad(fp, key, retS);
	if(EDAT_RET_OK == ret)
	{
		*value = (float)atof(retS);
		DLOG_Debug("Key match.");
	}
	else
	{
		*value = def;
		DLOG_Debug("Key not match. Return default.");
	}
}
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void EDAT_SetIniShortValue(FILE * fp, schar * key, ushort value)
{
	schar transBuf[VALUELENGTH] = {0};
	snprintf(transBuf, sizeof(transBuf), "%u", value);
	EDAT_IniDumpLine(fp, key, transBuf);
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void EDAT_SetIniStringValue(FILE * fp, schar * key, schar * value)
{
	EDAT_IniDumpLine(fp, key, value);
}
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/05 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void EDAT_SetIniCharValue(FILE * fp, schar * key, uchar value)
{
	schar transBuf[VALUELENGTH] = {0};
	snprintf(transBuf, sizeof(transBuf), "%u", value);
	EDAT_IniDumpLine(fp, key, transBuf);
}
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/05 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void EDAT_SetIniFloatValue(FILE * fp, schar * key, float value)
{
	schar transBuf[VALUELENGTH] = {0};
	snprintf(transBuf, sizeof(transBuf), "%f", value);
	EDAT_IniDumpLine(fp, key, transBuf);
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
slong EDAT_IniparserLoad(const FILE *fp, const schar *key, schar *value)
{
	schar tmp_line[LINELENGTH] = {0};
	schar tmp_key[KEYLENGTH] = {0};
	schar tmp_val[VALUELENGTH] = {0};
	slong last = 0;
	slong lineno = 0;
	slong len;
	slong ret = EDAT_RET_NG;

	if(NULL == fp)
	{
		DLOG_Error("EDAT_IniparserLoad para ERROR");
		return ret;
	}
	while(fgets(tmp_line + last, LINELENGTH - last, fp) != NULL)
	{
		lineno++;
		len = (slong)strlen(tmp_line)-1;
		if(len > 0)
		{
			/* Safety check against buffer overflows */
			if(tmp_line[len]!='\n' && !feof(fp))
			{
				DLOG_Error("input line too long");
				return ret;
			}
			/* Get rid of \n and spaces at end of line */
			while((len >= 0) &&
					((tmp_line[len]=='\n') || (isspace(tmp_line[len]))))
			{
				tmp_line[len] = 0;
				len--;
			}
			if(len < 0)
			{ /* Line was entirely \n and/or spaces */
				len = 0;
			}
			/* Detect multi-line */
			if(tmp_line[len]=='\\')
			{
				/* Multi-line value */
				last = len;
			}
			else
			{
				last = 0;
				switch(edat_IniparserLine(tmp_line, tmp_key, tmp_val))
				{
					case LINE_EMPTY:
					case LINE_COMMENT:
					break;
					case LINE_VALUE:
					if(0 == strcmp(tmp_key,key))
					{
						strncpy(value, tmp_val, strlen(tmp_val) + 1);
						ret = EDAT_RET_OK;
					}
					break ;
					case LINE_ERROR:
					break;
					default:
					break ;
				}
				memset(tmp_line, 0, LINELENGTH);
				memset(tmp_key, 0, KEYLENGTH);
				memset(tmp_val, 0, VALUELENGTH);
			}
		}
	}
	fseek(fp, 0, SEEK_SET);
	return ret;
}
/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
LineStatus edat_IniparserLine(const schar *input_line, schar *key,
									 schar *value)
{
	LineStatus sta;
	schar tmp_line[LINELENGTH] = {0};
	size_t len;

	len = strlen(input_line) + 1;
	memcpy(tmp_line, input_line, len);

	sta = LINE_UNPROCESSED;
	if(len < 1)
	{
		/* Empty line */
		sta = LINE_EMPTY;
	}
	else if(tmp_line[0] == '#' || tmp_line[0] == ';')
	{
		/* Comment line */
		sta = LINE_COMMENT;
	}
	else if(sscanf(tmp_line, "%[^=] = \"%[^\"]\"", key, value) == 2
			 || sscanf(tmp_line, "%[^=] = '%[^\']'", key, value) == 2)
	{
		/* Usual key=value with quotes, with or without comments */
		edat_Strstrip(key);
		/* Don't strip spaces from values surrounded with quotes */
		sta = LINE_VALUE;
	}
	else if(sscanf(tmp_line, "%[^=] = %[^;#]", key, value) == 2)
	{
		/* Usual key=value without quotes, with or without comments */
		edat_Strstrip(key);
		edat_Strstrip(value);
		/*
		 * sscanf cannot handle '' or "" as empty values
		 * this is done here
		 */
		if(!strcmp(value, "\"\"") || (!strcmp(value, "''")))
		{
			value[0] = 0;
		}
		sta = LINE_VALUE;
	}
	else if(sscanf(tmp_line, "%[^=] = %[;#]", key, value)==2
		 || sscanf(tmp_line, "%[^=] %[=]", key, value) == 2)
	{
		/*
		 * Special cases:
		 * key=
		 * key=;
		 * key=#
		 */
		edat_Strstrip(key);
		value[0] = 0;
		sta = LINE_VALUE;
	}
	else
	{
		/* Generate syntax error */
		sta = LINE_ERROR;
	}
	return sta;
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/04 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
ulong edat_Strstrip(schar *s)
{
	schar *last = NULL;
	schar *dest = s;
	bool_t hasSpace = TRUE;

	if(s == NULL)
	{
 		return 0;
	}

	last = s + strlen(s);
	while(isspace((slong)*s) && *s) 
	{
		s++;
	}
	while(last > s && hasSpace)
	{
	if(!isspace((slong)*(last-1)))
		{
			hasSpace = FALSE;
		}
		last--;
	}
	*last = (schar)0;

	memmove(dest, s, last - s + 1);
	return last - s;
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/05 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void EDAT_IniDumpLine(const FILE *fp, const schar *key, const schar *value)
{
	if(NULL == key || NULL == value || NULL == fp)
	{
		return;
	}
	fprintf(fp, "%s = %s\n", key, value);
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/05 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void EDAT_IniFileCopy(const schar *dest, const schar *src)
{
	FILE *fpr = NULL;/* 読み込みファイル */
	FILE *fpw = NULL;/* 書き込みファイル */
	schar readLine[LINELENGTH] = {0};
	DEBUG_TRCIN();

	if((fpr = fopen(src, "r")) == NULL)
	{
		DLOG_Error("can't read fopen %s. file is NOT exist.", src);
		return (FALSE);
	}
	fpw = fopen(dest, "w+");

	/* 行読み込み */
	while((fgets(readLine, LINELENGTH, fpr)) != NULL)
	{
		/* 読み込み行を書き込み */
		fputs(readLine, fpw);
	}
	fclose(fpr);
	fclose(fpw);

	DEBUG_TRCOUT();
}
/*End Of File*/
