/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jingtong.li										*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/23												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include header File														*/
/*--------------------------------------------------------------------------*/
#include <limits.h>
#include "enet_cmn_g.h"
#include "enet_file_g.h"
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
extern ushort g_enet_WlanLimitTime;
extern uchar g_enet_WlanLimitSignal;
/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
#define ENET_DBG_MODE_STR(mode)	(mode==ENET_FILE_READ?"FILE_READ ":(mode==ENET_FILE_WRITE?"FILE_WRITE":"UNKNOWN   "))
/* ファイルアクセス排他用 */
pthread_mutex_t s_ENET_TouchFileMutex;
/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
static bool_t enet_MoveTxtFile(const schar* src, const schar* dst);
/*==========================================================================*/
/*	Description		:指定ファイルのサイズを取得する							*/
/*--------------------------------------------------------------------------*/
/*	param			:schar* src	ファイルパス								*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						-1	ファイルが存在しない							*/
/*						0以上	ファイルサイズ								*/
/*--------------------------------------------------------------------------*/
/*	detail			:指定ファイルのサイズを取得する							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_GetFileSize(const schar* src)
{
	FILE* fp;
	slong fncRet = ENET_ERR1;
	DEBUG_TRCIN();

	fp = fopen(src, ENET_OPEN_TYPE_READ_BIN);
	if (fp == NULL)
	{
		DLOG_Error("can't read fopen %s. file is NOT exist.\n", src);
		return (fncRet);
	}
	/* 末尾に移動 */
	fseek(fp, 0, SEEK_END);
	/* 末尾のファイル位置＝ファイルサイズ */
	fncRet = ftell(fp);
	if (fncRet < 0)
	{
		fncRet = ENET_ERR1;
	}
	fclose(fp);
	DLOG_Debug("fncRet : %d\n", fncRet);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:指定ファイルに対してキーに対応する値を取得・保存する	*/
/*--------------------------------------------------------------------------*/
/*	param			:schar* key	キー										*/
/*					schar* val	書き込み時は指定の値を保存する				*/
/*							読み込み時は取得データを格納する				*/
/*					EnetFileMode_t mode	読み込み・書き込みを指定			*/
/*					schar* filePath	対象ファイル							*/
/*--------------------------------------------------------------------------*/
/*	return			:boo_t	処理結果										*/
/*						TRUE	正常	FALSE	異常						*/
/*--------------------------------------------------------------------------*/
/*	detail			:指定ファイルに対してキーに対応する値を取得・保存する	*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
bool_t ENET_RwSettingValue(const schar* key, schar* val, EnetFileMode_t mode, const schar* filePath)
{
	bool_t fncRet = FALSE;
	bool_t hasKey = FALSE;
	bool_t errFlag = FALSE;
	bool_t dapFlag = FALSE;
	FILE *fpr = NULL; /* 読み込みファイル */
	FILE *fpw = NULL; /* 書き込み一時ファイル */
	schar readLine[ENET_BUF_STR_LEN] = { };
	schar writeLine[ENET_BUF_STR_LEN] = { };
	slong scanElements = 0;
	EnetKeyAndVal_t keyAndVal = { };
	schar* tmpPath = ENET_FILE_TEMP;
	DEBUG_TRCIN();

	/* 排他ロック開始 */
	CMN_MutexLock(&s_ENET_TouchFileMutex);
	/* 指定ファイルオープン */
	switch (mode)
	{
		case ENET_FILE_READ:
			if ((fpr = fopen(filePath, ENET_OPEN_TYPE_READ)) == NULL)
			{
				DLOG_Error("can't read fopen %s. file is NOT exist.\n", filePath);
				errFlag = TRUE; /* 応答までジャンプ */
			}
			break;
		case ENET_FILE_WRITE: /* 以降、(fpw != NULL)であれば書き込みモード */
			if ((fpw = fopen(tmpPath, ENET_OPEN_TYPE_WRITE)) == NULL)
			{
				DLOG_Error("Err! write fopen %s\n", tmpPath);
				errFlag = TRUE; /* 応答までジャンプ */
			}
			if ((fpr = fopen(filePath, ENET_OPEN_TYPE_READ)) == NULL)
			{
				DLOG_Error("can't read fopen %s. file is NOT exist.\n", filePath);
				errFlag = TRUE; /* 応答までジャンプ */
			}
			break;
		default:
			errFlag = TRUE; /* 応答までジャンプ */
			break;
	}

	if(FALSE == errFlag)
	{
		while (FALSE == dapFlag && fpr != NULL /* 初回書き込みはループに入らない */
		&& (fgets(readLine, ENET_BUF_STR_LEN, fpr)) != NULL) /* 行読み込み */
		{
			memset(&keyAndVal, 0, sizeof(keyAndVal)); /* 読み取りkey,val初期化 */
			/* readLineのkey=valをsscanf */
			scanElements = sscanf(readLine, "%[^=]=%[^\n]\n", keyAndVal.key, keyAndVal.val);
			/* 変換個数一致→key=val形式 */
			if (scanElements == ENET_SETTING_ELEMENTS)
			{
				/* keyが完全一致('\0'まで) */
				if (strncmp(keyAndVal.key, key, strnlen(key, ENET_BUF_STR_LEN) + 1) == 0)
				{
					hasKey = TRUE;
					if (fpw == NULL)
					{
						DLOG_Debug("select! key/val : %s/%s\n", keyAndVal.key, keyAndVal.val);
						strncpy(val, keyAndVal.val, ENET_BUF_STR_LEN);
						dapFlag = TRUE;
					}
					else
					{
						DLOG_Debug("update! key/oldVal->newVal : %s/%s->%s\n", key, keyAndVal.val, val);
						snprintf(writeLine, ENET_BUF_STR_LEN, "%s=%s\n", key, val);
						fputs(writeLine, fpw);
					}
				}
				/* key対象外の行 */
				else
				{
					if (fpw != NULL)
					{
						fputs(readLine, fpw); /* 読み取り行をコピー */
					}
				}
			}
			/* keyのみ存在 */
			else if (scanElements == ENET_SETTING_ONLY_KEY)
			{
				DLOG_Debug("Key has no value. key : %s\n", keyAndVal.key);
				/* keyが完全一致('\0'まで) */
				if (strncmp(keyAndVal.key, key, strnlen(key, ENET_BUF_STR_LEN) + 1) == 0)
				{
					hasKey = TRUE;
					if (fpw == NULL)
					{
						DLOG_Debug("select! find only key : %s\n", keyAndVal.key);
						dapFlag = TRUE;
					}
					else
					{
						DLOG_Debug("update! key/val : %s/%s\n", key, val);
						snprintf(writeLine, ENET_BUF_STR_LEN, "%s=%s\n", key, val);
						fputs(writeLine, fpw);
					}
				}
				else
				{
					if (fpw != NULL)
					{
						fputs(readLine, fpw); /* 読み取り行をコピー */
					}
				}
			}
			else
			{
				DLOG_Debug("Key is Invalid format. key : %s\n", keyAndVal.key);
				if (fpw != NULL)
				{
					fputs(readLine, fpw); /* 読み取り行をコピー、コメント行等を想定 */
				}
			}
		}
		if (hasKey)
		{
			if (fpw == NULL)
			{
				fncRet = TRUE; /* 読み込みモードでキーを発見 */
			}
		}
		else
		{
			if (fpw == NULL)
			{
				DLOG_Debug("not found key : %s\n", key);
			}
			else
			{ 
				/* 上書き行がないため行追加 */
				snprintf(writeLine, ENET_BUF_STR_LEN, "%s=%s\n", key, val);
				fputs(writeLine, fpw);
			}
		}
	}

	/* 指定ファイルクローズ */
	if (fpr != NULL)
	{
		fclose(fpr);
	}
	if (fpw != NULL)
	{
		fclose(fpw);
		if (enet_MoveTxtFile(tmpPath, filePath))
		{
			fncRet = TRUE; /* 書き込みモードでファイル書き換え成功 */
		}
		else
		{
			DLOG_Debug("Fatal! write Failed : %s\n", filePath);
		}
	}

	/* 排他ロック解除 */
	CMN_MutexUnlock(&s_ENET_TouchFileMutex);
	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:プロセスを初期化する									*/
/*--------------------------------------------------------------------------*/
/*	param			:int argc	プロセス起動時の引数						*/
/*					char *argv[]	プロセス起動時の引数					*/
/*--------------------------------------------------------------------------*/
/*	return			:無い													*/
/*--------------------------------------------------------------------------*/
/*	detail			:プロセスを初期化する									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void ENET_InitEnetFile(int argc, char *argv[])
{
	DEBUG_TRCIN();

	/* Mutexの初期化 */
	CMN_MutexInit(&s_ENET_TouchFileMutex);

	DEBUG_TRCOUT();
}
/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:テキストファイルをコピーする							*/
/*--------------------------------------------------------------------------*/
/*	param			:schar* src	コピー元ファイル							*/
/*					schar* dst	コピー先ファイル							*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:テキストファイルをコピーする							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
static bool_t enet_CopyTxtFile(const schar* src, const schar* dst)
{
	FILE *fpr = NULL; 						/* 読み込みファイル */
	FILE *fpw = NULL; 						/* 書き込みファイル */
	schar readLine[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	if ((fpr = fopen(src, ENET_OPEN_TYPE_READ)) == NULL)
	{
		DLOG_Error("can't read fopen %s. file is NOT exist.", src);
		return (FALSE);
	}
	if ((fpw = fopen(dst, ENET_OPEN_TYPE_WRITE)) == NULL)
	{
		DLOG_Error("Err! write fopen %s", dst);
		fclose(fpr);
		return (FALSE);
	}

	/* 行読み込み */
	while ((fgets(readLine, ENET_BUF_STR_LEN, fpr)) != NULL)
	{
		/* 読み込み行を書き込み */
		fputs(readLine, fpw);
	}
	fclose(fpr);
	fclose(fpw);

	DEBUG_TRCOUT();
	return (TRUE);
}

/*==========================================================================*/
/*	Description		:テキストファイルを移動する								*/
/*--------------------------------------------------------------------------*/
/*	param			:schar* src	移動元ファイル								*/
/*					schar* dst	移動先ファイル								*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:テキストファイルを移動する								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
static bool_t enet_MoveTxtFile(const schar* src, const schar* dst)
{
	bool_t fncRet = FALSE;
	DEBUG_TRCIN();

	if (enet_CopyTxtFile(src, dst))
	{
		fncRet = TRUE;
		if (remove(src) != 0)
		{
			DLOG_Debug("can't remove %s. Remaining files.\n", src);
		}
	}

	DEBUG_TRCOUT();
	return (fncRet);
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
/*	history			:2023/04/07 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
slong ENET_ReadWlanLimitFile(void)
{
	FILE *file = NULL;
	schar ch[ENET_BUF_STR_LEN] = {0};
	slong len = 0;
	slong ret = 0;
	slong i = 0;
	slong WLANlimitTime = -1;

	DEBUG_TRCIN();
	if((file = fopen(ENET_FILE_WLAN_LIMIT, "r")) == NULL)
	{
		DEBUG_TRCOUT();
		return ENET_ERR1;
	}
	fgets(ch, sizeof(ch), file);
	len = strlen(ch);
	if(ch[len-1] == '\n')
	{
		len--;
	}	
	if(len > 3 || len == 0)
	{
		ret = -1;
		DLOG_Error("WlanLimitFile has more than 3 char or no char!");
	}
	else
	{
		for(i = 0; i < len; ++i)
		{
			if(ch[i] >= '0' && ch[i] <= '9')
			{
				/*do nothing*/
			}
			else
			{
				ret = -1;
				DLOG_Error("WlanLimitFile has illegal char [%c]", ch[i]);
			}
		}
	}
	if(0 == ret)
	{
		WLANlimitTime = atol(ch);
		if(0 == WLANlimitTime)
		{
			g_enet_WlanLimitSignal = 1;
		}
		else
		{
			g_enet_WlanLimitSignal = 0;
		}
		g_enet_WlanLimitTime = WLANlimitTime;
	}
	else
	{
		g_enet_WlanLimitTime = 0;
		g_enet_WlanLimitSignal = 1;
	}
	fclose(file);
	DEBUG_TRCOUT();
	return ENET_SUCCESS;
}

/* End Of File */
