/****************************************************************************/
/*	Description		:FW更新機能 ファイル操作								*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC.zhangs												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/20												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:FW更新機能の機能名は、EOTAとす							*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 DHC CORPORATION								*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "eota_log_g.h"
#include "eota_check_g.h"
#include "eota_setting_g.h"

#include "eota_file.h"

/*--------------------------------------------------------------------------*/
/* 公開変数定義																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* 非公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* 公開関数定義																*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:	圧縮ファイル取得									*/
/*--------------------------------------------------------------------------*/
/*	param			:	ディレクトリ										*/
/* 						 圧縮ファイル										*/
/*--------------------------------------------------------------------------*/
/*	return			:	処理結果											*/
/* 						EOTA_FILE_OK			処理正常					*/
/*						EOTA_FILE_PARAM_ERR		チェック異常				*/
/*				 		EOTA_FILE_TAR_NOT_ERR	圧縮ファイルなし			*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW更新開始処理を実施する							*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_GetTarFile(const schar * BasePath,	/* [i] ディレクトリ    */
					  schar *       TarFile)	/* [o] 圧縮ファイル    */
{
	uchar	Ret			= EOTA_FILE_OK;			/* 戻り値 					*/
	DIR *	Dir			= NULL;					/* ディレクトリ				*/
	struct  dirent *Ptr	= NULL;					/* ディレクトリポイント		*/
	sint	StrcmpRet	= 0;					/* 文字列比較戻り値			*/
	ushort  Number		= 0u;					/* 半角英数字				*/
	ushort  TmpNumber	= 0u;					/* 一時的な半角英数字		*/
	uchar	CheckRet	= EOTA_CHECK_OK;		/* ファイルチェック戻り値 	*/
	uchar	TarFileFlg	= EOTA_TAR_FILE_OFF;	/* 圧縮ファイル有無			*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == BasePath) || (NULL == TarFile))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FILE_PARAM_ERR;
	}
	else
	{
		/* ディレクトリオープン */
		Dir = opendir(BasePath);
		if (NULL == Dir)
		{
			/* TODO. */
			// DLOG_Debug(EOTA_OPEN_DIR_FAIL_ERR, BasePath);
			Ret = EOTA_FILE_OPEN_DIR_ERR;
		}
		else
		{
			/* ファイルを読み込む */
			Ptr = readdir(Dir);
			while (NULL != Ptr)
			{
				/* .と比較 */
				StrcmpRet = strncmp(Ptr->d_name,
									EOTA_FILE_DOT, EOTA_FILE_DOT_LEN);
				if (EOTA_STRCMP_INDENTICAL != StrcmpRet)
				{
					/* ..と比較 */
					StrcmpRet = strncmp(Ptr->d_name,
										EOTA_FILE_DOUBLE_DOT,
										EOTA_FILE_DOUBLE_DOT_LEN);
					if (EOTA_STRCMP_INDENTICAL != StrcmpRet)
					{
						/* File */
						if (EOTA_D_TYPE_FILE == Ptr->d_type)
						{
							CheckRet = EOTA_CheckTarFile(Ptr->d_name, &TmpNumber);

							/* 圧縮ファイルが複数ある場合は
								xxx が最も大きいものを取得 */
							if ((EOTA_CHECK_OK == CheckRet)
								&& (Number <= TmpNumber))
							{
								Number = TmpNumber;
								/* 圧縮ファイル名出力 */
								strncpy(TarFile, Ptr->d_name,
										EOTA_PKG_TGZ_NAME_LEN);
								/* 圧縮ファイル有設定 */
								TarFileFlg = EOTA_TAR_FILE_ON;
							}
							else
							{
								/* 処理無し */
							}
						}
						/* link File */
						/* Dir */
						else
						{
							/* 処理無し */
						}
					}
					else
					{
						/* 処理無し */
					}
				}
				else
				{
					/* 処理無し */
				}

				Ptr=readdir(Dir);
			}
		}

		/* ディレクトリクローズ */
		closedir(Dir);

		/*「E1FW_PKG_vxxx.tgz」が存在しない  */
		if (EOTA_TAR_FILE_OFF == TarFileFlg)
		{
			Ret = EOTA_FILE_TAR_NOT_ERR;
		}
		else
		{
			/* 処理無し */
		}
	}

	DEBUG_TRCOUT();

    return Ret;
}

/*==========================================================================*/
/*	Description		:	圧縮ファイル解凍									*/
/*--------------------------------------------------------------------------*/
/*	param			:	圧縮ファイルのフルパス								*/
/*--------------------------------------------------------------------------*/
/*	return			:	処理結果											*/
/* 						EOTA_FILE_OK			処理正常					*/
/*				 		EOTA_FILE_PARAM_ERR		チェック異常				*/
/*						EOTA_FILE_DIR_NOT_ERR	パス存在しない				*/
/*						EOTA_FILE_TAR_FAIL_ERR	解凍失敗					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW更新開始処理を実施する							*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_TarunZip(const schar * TgzFilePath)	/* [i] 圧縮ファイルのパス	*/
{
	uchar	Ret					 	 = EOTA_FILE_OK;
												/* 戻り値					*/
	sint 	SystemRet  		 		 = -1;		/* systemコマンドの戻り値 	*/
	schar	Cmd[EOTA_TAR_FILE_DIR_LEN] = {0};	/* コマンド					*/
	sint	stat_ret				 = 0;		/* statの戻り値 			*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == TgzFilePath)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FILE_PARAM_ERR;
	}
	else
	{
		/* 処理無し */
	}

	/* ディレクトリ存在チェック */
	if (EOTA_FILE_OK == Ret)
	{
		/* ディレクトリ確認 */
		stat_ret = access(TgzFilePath, 0);

		/* 存在しない場合 */
		if (0 != stat_ret)
		{
			/* ディレクトリ作成 */
			DLOG_Debug("%s", EOTA_FILE_DIR_NO_ERR);
			Ret = EOTA_FILE_DIR_NOT_ERR;
		}
		else
		{
			/* 処理無し */
		}
	}
	else
	{
		/* 処理無し */
	}

	/* 解凍処理実施 */
	if (EOTA_FILE_OK == Ret)
	{
		/* 解凍コマンド */
		memset(Cmd, 0, EOTA_TAR_FILE_DIR_LEN);
		sprintf(Cmd, "tar -zxvf %s", TgzFilePath);

		/* 解凍処理 */
		SystemRet = system(Cmd);

		if (EOTA_SYSTEM_RET == SystemRet)
		{
			if ( EOTA_SYSTEM_RET != WEXITSTATUS(SystemRet) )
			{
				/* 解凍エラーログ出力 */
				DLOG_Debug("%s", EOTA_TAR_ZCVF_ERR);
				Ret = EOTA_FILE_TAR_FAIL_ERR;
			}
			else
			{
				Ret = EOTA_FILE_OK;
			}
		}
		else
		{
			/* 解凍エラーログ出力 */
			DLOG_Debug("%s", EOTA_TAR_ZCVF_ERR);
			Ret = EOTA_FILE_TAR_FAIL_ERR;
		}
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

    return Ret;
}

/*==========================================================================*/
/*	Description		:	FW 更新の圧縮ファイルダウンロード					*/
/*--------------------------------------------------------------------------*/
/*	param			:	ダウンロードの元パス								*/
/* 				 		ダウンロードの先パス								*/
/*--------------------------------------------------------------------------*/
/*	return			:	処理結果											*/
/* 						EOTA_FILE_OK:処理正常								*/
/*				 		EOTA_FILE_PARAM_ERR:チェック異常					*/
/*				 		EOTA_FILE_COPY_ERR:ファイルコピーエラー				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW 更新の圧縮ファイルをダウンロードする				*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_TarCopy(const schar * Src,			/* [i] ウンロードの元パス	*/
				   const schar * dst)			/* [i] ダウンロードの先パス	*/
{
	uchar	Ret			= EOTA_FILE_OK;	/* 戻り値 					*/
	schar	Cmd[2 * EOTA_TAR_FILE_DIR_LEN]
						= {0};			/* コマンド					*/
	sint 	SystemRet  = -1;			/* systemコマンドの戻り値	*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == Src) || (NULL == dst))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FILE_PARAM_ERR;
		return Ret;
	}
	else
	{
		/* 処理無し */
	}

	/* 有効パスを確認 */
	sprintf(Cmd, "copy -p %s %s", Src, dst);

	/* コマンド実施 */
	SystemRet = system(Cmd);
	if (EOTA_SYSTEM_RET == SystemRet)
	{
		if ( EOTA_SYSTEM_RET != WEXITSTATUS(SystemRet) )
		{
			/* ファイル削除エラーログ出力 */
			DLOG_Debug("%s", EOTA_FILE_COPY_L_ERR);
			Ret = EOTA_FILE_COPY_ERR;
		}
		else
		{
			/* 処理無し */
		}
	}
	else
	{
		/* ファイル削除エラーログ出力 */
		DLOG_Debug("%s", EOTA_FILE_COPY_L_ERR);
		Ret = EOTA_FILE_COPY_ERR;
	}

	DEBUG_TRCOUT();

    return Ret;
}

/*==========================================================================*/
/*	Description		:	FW 更新指示ファイル取得								*/
/*--------------------------------------------------------------------------*/
/*	param			:	FW 更新指示ファイルのパス							*/
/* 						FW 更新指示ファイル情報								*/
/*--------------------------------------------------------------------------*/
/*	return			:	処理結果											*/
/* 						EOTA_FILE_OK:処理正常								*/
/*						EOTA_FILE_PARAM_ERR:チェック異常					*/
/*						EOTA_FILE_OPEN_DIR_ERR:ディレクトリオープンエラー	*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW更新開始処理を実施する							*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_GetUpdateSetupFile(
		const schar * FilePath,				/* [i] 圧縮ファイルのパス       */
		EOTA_UPDATE_SETUP_T * UpdateSetup)	/* [o] FW 更新指示ファイル情報  */
{
	/* 変数初期化 */
	uchar	Ret		 	= EOTA_FILE_OK;			/* 戻り値				*/
	slong	read_ret	= 0;					/* データ取得の戻り値	*/
	schar *	Buff		= 0;					/* データ				*/
	size_t	size		= 0;					/* 取得データサイズ		*/
	sint	StrcmpRet	= 1;					/* 文字列比較戻り値		*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == FilePath) || (NULL == UpdateSetup))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FILE_PARAM_ERR;
	}
	else
	{
		/* 処理無し */
	}

	/* FW 更新指示ファイルを読み込む */
	if (EOTA_FILE_OK == Ret)
	{
		/* ファイルオープン */
		FILE *FilePoint = fopen(FilePath, "r");

		/* オープン失敗の場合、ERRを戻る */
		if (FilePoint == NULL)
		{
			/* TODO. */
			// DLOG_Error("%s",EOTA_OPEN_DIR_FAIL_ERR, FilePath);
			Ret = EOTA_FILE_OPEN_DIR_ERR;
		}
		else
		{
			/* EOF以外の場合、ループ実施 */
			while(EOTA_STRCMP_INDENTICAL != StrcmpRet)
			{
				/* 一行目のデータを取得 */
				read_ret = getline(&Buff, &size, FilePoint);

				/* 異常の場合、ループ終了 */
				if (EOTA_READ_LINE_ERR == read_ret)
				{
					if (NULL != Buff)
					{
						free(Buff);
						Buff = NULL;
					}
					else
					{
						/* 処理無し */
					}
					StrcmpRet = EOTA_STRCMP_INDENTICAL;
					DLOG_Error("%s",EOTA_FILE_GET_DATA_ERR);
				}
				else
				{
					StrcmpRet = strncmp(Buff, EOTA_FILE_EOF, strlen(Buff));

					if (EOTA_STRCMP_INDENTICAL != StrcmpRet)
					{
						/* 取得したデータを保存 */
						read_ret = eota_parse_line(Buff, UpdateSetup);

						/* 取得失敗、ループ終了 */
						if(EOTA_FILE_OK != read_ret)
						{
							DLOG_Error("%s",EOTA_FILE_GET_DATA_ERR);
							StrcmpRet = EOTA_STRCMP_INDENTICAL;
						}
						else
						{
							/* 処理無し */
						}

						if (Buff != NULL)
						{
							free(Buff);
							Buff = NULL;
						}
						else
						{
							/* 処理無し */
						}
					}
					else
					{
						/* 処理無し */
					}
				}
			}
			/* ファイルをクローズする */
			fclose(FilePoint);
		}
	}
	else
	{
		/* 処理無し */
	}
	DEBUG_TRCOUT();

    return Ret;
}

/*==========================================================================*/
/*	Description		:	既存の FW 更新ファイル削除							*/
/*--------------------------------------------------------------------------*/
/*	param			:	ディレクトリ										*/
/*--------------------------------------------------------------------------*/
/*	return			:	処理結果											*/
/* 						EOTA_FILE_OK:処理正常								*/
/*						EOTA_FILE_PARAM_ERR:チェック異常					*/
/*						EOTA_FILE_OPEN_DIR_ERR:ディレクトリオープンエラー	*/
/*						EOTA_FILE_DEL_ERR:削除失敗							*/
/*--------------------------------------------------------------------------*/
/*	detail			:	既存の FW 更新ファイルを削除する					*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_DelFWFile(const schar * BasePath) 		/* [i] ディレクトリ    */
{
	/* 変数初期化 */
	uchar	Ret		 	= EOTA_FILE_OK;			/* 戻り値					*/
	schar	Cmd[EOTA_TAR_FILE_DIR_LEN]
						= {0};					/* コマンド					*/
	sint 	SystemRet  = -1;					/* systemコマンドの戻り値	*/
	DIR *	Dir			= NULL;					/* ディレクトリ				*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == BasePath)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FILE_PARAM_ERR;
	}
	else
	{
		/* 有効パスを確認 */
		/* ディレクトリオープン */
		Dir = opendir(BasePath);
		if (NULL == Dir)
		{
			/* TODO. */
			// DLOG_Error("%s",EOTA_OPEN_DIR_FAIL_ERR, BasePath);
			Ret = EOTA_FILE_OPEN_DIR_ERR;
		}
		else
		{
			/* ディレクトリクローズ */
			closedir(Dir);

			sprintf(Cmd, "rm -rf %s/*", BasePath);

			/* コマンド実施 */
			SystemRet = system(Cmd);

			if (EOTA_SYSTEM_RET == SystemRet)
			{
				if ( EOTA_SYSTEM_RET != WEXITSTATUS(SystemRet) )
				{
					/* ファイル削除エラーログ出力 */
					DLOG_Debug("%s", EOTA_FILE_DEL_L_ERR);
					Ret = EOTA_FILE_DEL_ERR;
				}
				else
				{
					Ret = EOTA_FILE_OK;
				}
			}
			else
			{
				/* ファイル削除エラーログ出力 */
				DLOG_Debug("%s", EOTA_FILE_COPY_L_ERR);
				Ret = EOTA_FILE_COPY_ERR;
			}
		}
	}

	DEBUG_TRCOUT();

    return Ret;
}

/*==========================================================================*/
/*	Description		:	ファイルサイズ取得									*/
/*--------------------------------------------------------------------------*/
/*	param			:	ファイルパス										*/
/*--------------------------------------------------------------------------*/
/*	return			:	(ファイルサイズ/ファイル読み込むエラー)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ファイルサイズを取得する							*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
slong EOTA_GetFileSize(const schar * Path)		/* [i]ファイルパス */
{
	/* 変数初期化 */
	slong Filesize	= EOTA_FIlE_READ_ERR;	/* 戻り値					*/
	FILE* FilePoint;						/* ファイルポイント			*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == Path)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		return Filesize;
	}
	else
	{
		/* 処理無し */
	}

	/* ファイルをオーペン */
	FilePoint = CMN_FOPEN(Path, EOTA_OPEN_TYPE_READ);
	if (FilePoint == NULL)
	{
		/* TODO. */
		// DLOG_Error("%s",EOTA_OPEN_DIR_FAIL_ERR, Path);
		return Filesize;
	}
	else
	{
		/* 処理無し */
	}

	/* 終了位置を指定 */
	fseek(FilePoint, 0L, SEEK_END);
	/* ファイルサイズ(文字数)を取得 */
	Filesize = ftell(FilePoint);

	/* ファイルをクローズ */
	CMN_FCLOSE(FilePoint);

	DEBUG_TRCOUT();

	return Filesize;
}

/*==========================================================================*/
/*	Description		:	ファイルデータ取得									*/
/*--------------------------------------------------------------------------*/
/*	param			:	ファイルパス										*/
/* 						開始位置											*/
/* 						出力データサイズ									*/
/* 						出力データ											*/
/*--------------------------------------------------------------------------*/
/*	return			:	処理結果											*/
/* 						EOTA_FILE_OK:処理正常								*/
/*						EOTA_FILE_PARAM_ERR:チェック異常					*/
/*						EOTA_FILE_OPEN_DIR_ERR:ディレクトリオープンエラー	*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ファイルサイズを取得する							*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar EOTA_GetFileData(const schar * Path,			/* [i]ファイルパス		*/
					   slong Offset,				/* [i]開始位置			*/
					   size_t * Len,				/* [o]出力データサイズ	*/
					   uchar *  Data				/* [o]出力データ		*/
					 )
{
	/* 変数初期化 */
	uchar	Ret		 	= EOTA_FILE_OK;				/* 戻り値				*/
	sint	FileRet		= 0;						/* ファイル操作の戻り値	*/
	sint	GetcRet		= 0;						/* fgetcの戻り値		*/
	ulong 	Loop		= 0u;						/* ループ数				*/
	FILE*	FilePoint;								/* ファイルポイント		*/

	DEBUG_TRCIN();


	/* パラメータチェック */
	if ((NULL == Path) || (NULL == Data) || (NULL == Len))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		return EOTA_FILE_PARAM_ERR;
	}
	else
	{
		/* 処理無し */
	}

	/* ファイルをオーペン */
	FilePoint = CMN_FOPEN(Path, EOTA_OPEN_TYPE_READ);
	if (FilePoint == NULL)
	{
		/* TODO. */
		// DLOG_Error("%s",EOTA_OPEN_DIR_FAIL_ERR, Path);
		Ret = EOTA_FILE_OPEN_DIR_ERR;
	}
	else
	{
		/* 処理無し */
	}

	if (EOTA_FILE_OK == Ret)
	{
		/* 開始位置を指定 */
		fseek(FilePoint, Offset, SEEK_SET);

		/* EOF取得 */
		FileRet = feof(FilePoint);
		/* バイナリデータ取得 */
		while( ! FileRet )
		{
			/* 1024 バイトずつに分割した */
			if ( EOTA_FW_DATA_SIZE > Loop)
			{
				GetcRet = fgetc(FilePoint);
				if (EOTA_FIlE_READ_ERR == GetcRet)
				{
					FileRet = 1;
				}
				else
				{
					Data[Loop] = (uchar)GetcRet;
					*Len += 1;

					FileRet = feof(FilePoint);
					Loop += 1;
				}
			}
			else
			{
				FileRet = 1;
			}
		}

		/* ファイルをクローズ */
		CMN_FCLOSE(FilePoint);
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*--------------------------------------------------------------------------*/
/* 非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:	トリム処理(左側)									*/
/*--------------------------------------------------------------------------*/
/*	param			:	変換元データ										*/
/*				 		変換先データ										*/
/*--------------------------------------------------------------------------*/
/*	return			:	処理結果											*/
/* 						EOTA_FILE_OK		処理正常						*/
/*						EOTA_FILE_PARAM_ERR	チェック異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	トリム処理を実施する								*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar eota_LeftTrim(schar * Src,			/* [i]元データ		*/
			   	    schar * Dst)			/* [o]変換先データ	*/
{
	uchar	Ret		= EOTA_FILE_OK;		/* 戻り値 		*/
	ulong	flag	= 0u;				/* フラグ		*/
	ulong	index	= 0u;				/* インデックス	*/
	size_t	size	= strlen(Src);		/* サイズ		*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == Src) || (NULL == Dst))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FILE_PARAM_ERR;
	}
	else
	{
		/* 処理無し */
	}

	if (EOTA_FILE_OK == Ret)
	{
		/* \nの場合 */
		if ((0 < size) && (EOTA_CHAR_LIN_BREAK == Src[size - 1]))
		{
			Src[size - 1] = EOTA_CHAR_NULL;
		}
		else
		{
			/* 処理無し */
		}

		/* トリム元文字列数分繰り返す */
		for(size_t size = 0; size < strlen(Src); size++)
		{
			if (flag == 0)
			{
				/* 空白とTABは不要 */
				if ((EOTA_CHAR_SPACE == Src[size]) || (EOTA_CHAR_TAB == Src[size]))
				{
					/* 処理無し */
				}
				else
				{
					Dst[index] = Src[size];
					flag = 1;
				}
			}
			else
			{
				Dst[index] = Src[size];
			}

			index++;
		}
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	トリム処理											*/
/*--------------------------------------------------------------------------*/
/*	param			:	変換元データ										*/
/*--------------------------------------------------------------------------*/
/*	return			:	処理結果											*/
/* 						EOTA_FILE_OK:処理正常								*/
/*						EOTA_FILE_PARAM_ERR:チェック異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	トリム処理を実施する								*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar eota_Trim(const schar * Src,						/* [i]元データ		*/
		  		 schar * Dst)							/* [o]変換先データ	*/
{
	uchar	Ret		= EOTA_FILE_OK;		/* 戻り値 		*/
	ulong 	index 	= 0u;				/* インデックス	*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == Src) || (NULL == Dst))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FILE_PARAM_ERR;
	}
	else
	{
		/* トリム元文字列数分繰り返す */
		for(size_t size = 0; size < strlen(Src); size++)
		{
			/* 空白、二重引用符、\nは不要 */
			if (	(EOTA_CHAR_SPACE == Src[size])
				||	(EOTA_CHAR_DOU_QUO_MARK == Src[size])
				||	(EOTA_CHAR_LIN_BREAK == Src[size]))
			{
				/* 処理無し */
			}
			else
			{
				Dst[index] = Src[size];
				index++;
			}
		}
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	設定BOARD_FW										*/
/*--------------------------------------------------------------------------*/
/*	param			:	FW 更新指示ファイル情報								*/
/* 				 		データ												*/
/*--------------------------------------------------------------------------*/
/*	return			:	処理結果											*/
/* 						EOTA_FILE_OK:処理正常								*/
/*						EOTA_FILE_PARAM_ERR:チェック異常					*/
/*						EOTA_FILE_DATA_ERR:ファイルデータ不正)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	BOARD_FWを設定する									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar eota_SetBoardFW(
		EOTA_UPDATE_SETUP_T * UpdateSetup,	/* [o] FW 更新指示ファイル情報	*/
		schar * Data)						/* [i] データ					*/
{
	uchar	Ret			= EOTA_FILE_OK;		/* 戻り値 				*/
	schar * Token		= NULL;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == UpdateSetup)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FILE_PARAM_ERR;
	}
	else
	{
		Token = strtok(Data, EOTA_CHAR_COMMA);

		/* バージョン */
		if (NULL != Token)
		{
			memcpy(UpdateSetup->BoardFw.Version, Token, strlen(Token));

			Token = strtok(NULL, EOTA_CHAR_COMMA);

			/* ファイル名 */
			if (NULL != Token)
			{
				memcpy(UpdateSetup->BoardFw.File, Token, strlen(Token));

				Token = strtok(NULL, EOTA_CHAR_COMMA);

				/* MD5 ファイル */
				if (NULL != Token)
				{
					memcpy(UpdateSetup->BoardFw.Md5File, Token, strlen(Token));
				}
				else
				{
					DLOG_Error("%s",EOTA_FILE_DATA_L_ERR);
					Ret = EOTA_FILE_DATA_ERR;
				}
			}
			else
			{
				/* 処理無し */
			}
		}
		else
		{
			/* 処理無し */
		}
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	設定INV_FW											*/
/*--------------------------------------------------------------------------*/
/*	param			:	FW 更新指示ファイル情報								*/
/* 				 		データ												*/
/*--------------------------------------------------------------------------*/
/*	return			:	処理結果											*/
/* 						EOTA_FILE_OK:処理正常								*/
/*						EOTA_FILE_PARAM_ERR:チェック異常					*/
/*						EOTA_FILE_DATA_ERR:ファイルデータ不正				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	INV_FWを設定する									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar eota_SetInvFW(
		EOTA_UPDATE_SETUP_T * UpdateSetup,	/* [o] FW 更新指示ファイル情報	*/
		schar * Data)						/* [i] データ					*/
{
	uchar	Ret			= EOTA_FILE_OK;		/* 戻り値 				*/
	schar *  Token		= NULL;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == UpdateSetup)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FILE_PARAM_ERR;
	}
	else
	{
		Token = strtok(Data, EOTA_CHAR_COMMA);
		/* バージョン */
		if (NULL != Token)
		{
			memcpy(UpdateSetup->InvFw.Version, Token, strlen(Token));

			Token = strtok(NULL, EOTA_CHAR_COMMA);

			/* BANK1 の FW ファイル名 */
			if (NULL != Token)
			{
				memcpy(UpdateSetup->InvFw.Bank1File, Token, strlen(Token));

				Token = strtok(NULL, EOTA_CHAR_COMMA);

				/* MD5 ファイル */
				if (NULL != Token)
				{
					memcpy(UpdateSetup->InvFw.Bank1Md5File,
							Token,
							strlen(Token));

					Token = strtok(NULL, EOTA_CHAR_COMMA);

					/* BANK2 の FW ファイル名 */
					if (NULL != Token)
					{
						memcpy(UpdateSetup->InvFw.Bank2File,
								Token,
								strlen(Token));

						Token = strtok(NULL, EOTA_CHAR_COMMA);

						/* MD5 ファイル */
						if (NULL != Token)
						{
							memcpy(UpdateSetup->InvFw.Bank2Md5File,
									Token,
									strlen(Token));
						}
						else
						{
							DLOG_Error("%s",EOTA_FILE_DATA_L_ERR);
							Ret = EOTA_FILE_DATA_ERR;
						}
					}
					else
					{
						/* 処理無し */
					}
				}
				else
				{
					DLOG_Error("%s",EOTA_FILE_DATA_L_ERR);
					Ret = EOTA_FILE_DATA_ERR;
				}
			}
			else
			{
				/* 処理無し */
			}
		}
		else
		{
			/* 処理無し */
		}
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	設定PV_FW											*/
/*--------------------------------------------------------------------------*/
/*	param			:	FW 更新指示ファイル情報								*/
/* 				 		データ												*/
/*--------------------------------------------------------------------------*/
/*	return			:	処理結果											*/
/* 						EOTA_FILE_OK:処理正常								*/
/*						EOTA_FILE_PARAM_ERR:チェック異常					*/
/*						EOTA_FILE_DATA_ERR:ファイルデータ不正				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	PV_FWを設定する										*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar eota_SetPVFW(
		EOTA_UPDATE_SETUP_T * UpdateSetup,	/* [o] FW 更新指示ファイル情報	*/
		schar * Data)						/* [i] データ					*/
{
	uchar	Ret			= EOTA_FILE_OK;		/* 戻り値 				*/
	schar *  Token		= NULL;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == UpdateSetup)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FILE_PARAM_ERR;
	}
	else
	{
		Token = strtok(Data, EOTA_CHAR_COMMA);
		/* バージョン */
		if (NULL != Token)
		{
			memcpy(UpdateSetup->PvFw.Version, Token, strlen(Token));

			Token = strtok(NULL, EOTA_CHAR_COMMA);

			/* BANK1 の FW ファイル名 */
			if (NULL != Token)
			{
				memcpy(UpdateSetup->PvFw.Bank1File, Token, strlen(Token));
				Token = strtok(NULL, EOTA_CHAR_COMMA);

				/* MD5 ファイル */
				if (NULL != Token)
				{
					memcpy(UpdateSetup->PvFw.Bank1Md5File,
							Token,
							strlen(Token));

					Token = strtok(NULL, EOTA_CHAR_COMMA);

					/* BANK2 の FW ファイル名 */
					if (NULL != Token)
					{
						memcpy(UpdateSetup->PvFw.Bank2File,
								Token,
								strlen(Token));

						Token = strtok(NULL, EOTA_CHAR_COMMA);

						/* MD5 ファイル */
						if (NULL != Token)
						{
							memcpy(UpdateSetup->PvFw.Bank2Md5File,
									Token,
									strlen(Token));
						}
						else
						{
							DLOG_Error("%s",EOTA_FILE_DATA_L_ERR);
							Ret = EOTA_FILE_DATA_ERR;
						}
					}
					else
					{
						/* 処理無し */
					}
				}
				else
				{
					DLOG_Error("%s",EOTA_FILE_DATA_L_ERR);
					Ret = EOTA_FILE_DATA_ERR;
				}
			}
			else
			{
				/* 処理無し */
			}
		}
		else
		{
			/* 処理無し */
		}
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	設定BDC_FW											*/
/*--------------------------------------------------------------------------*/
/*	param			:	FW 更新指示ファイル情報								*/
/* 				 		データ												*/
/*--------------------------------------------------------------------------*/
/*	return			:	処理結果											*/
/* 						EOTA_FILE_OK:処理正常								*/
/*						EOTA_FILE_PARAM_ERR:チェック異常					*/
/*						EOTA_FILE_DATA_ERR:ファイルデータ不正				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	BDC_FWを設定する									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar eota_SetBDCFW(
		EOTA_UPDATE_SETUP_T * UpdateSetup,	/* [o] FW 更新指示ファイル情報	*/
		schar * Data)						/* [i] データ					*/
{
	uchar	Ret			= EOTA_FILE_OK;				/* 戻り値 				*/
	schar *  Token		= NULL;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == UpdateSetup)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FILE_PARAM_ERR;
	}
	else
	{
		Token = strtok(Data, EOTA_CHAR_COMMA);
		/* バージョン */
		if (NULL != Token)
		{
			memcpy(UpdateSetup->BdcFw.Version, Token, strlen(Token));

			Token = strtok(NULL, EOTA_CHAR_COMMA);

			/* BANK1 の FW ファイル名 */
			if (NULL != Token)
			{
				memcpy(UpdateSetup->BdcFw.Bank1File, Token, strlen(Token));

				Token = strtok(NULL, EOTA_CHAR_COMMA);

				/* MD5 ファイル */
				if (NULL != Token)
				{
					memcpy(UpdateSetup->BdcFw.Bank1Md5File,
							Token,
							strlen(Token));

					Token = strtok(NULL, EOTA_CHAR_COMMA);

					/* BANK2 の FW ファイル名 */
					if (NULL != Token)
					{
						memcpy(UpdateSetup->BdcFw.Bank2File,
								Token,
								strlen(Token));

						Token = strtok(NULL, EOTA_CHAR_COMMA);

						/* MD5 ファイル */
						if (NULL != Token)
						{
							memcpy(UpdateSetup->BdcFw.Bank2Md5File,
									Token,
									strlen(Token));
						}
						else
						{
							DLOG_Error("%s",EOTA_FILE_DATA_L_ERR);
							Ret = EOTA_FILE_DATA_ERR;
						}
					}
					else
					{
						/* 処理無し */
					}
				}
				else
				{
					DLOG_Error("%s",EOTA_FILE_DATA_L_ERR);
					Ret = EOTA_FILE_DATA_ERR;
				}
			}
			else
			{
				/* 処理無し */
			}
		}
		else
		{
			/* 処理無し */
		}
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	設定JB_FW											*/
/*--------------------------------------------------------------------------*/
/*	param			:	FW 更新指示ファイル情報								*/
/* 				 		データ												*/
/*--------------------------------------------------------------------------*/
/*	return			:	処理結果											*/
/* 						EOTA_FILE_OK:処理正常								*/
/*						EOTA_FILE_PARAM_ERR:チェック異常					*/
/*						EOTA_FILE_DATA_ERR:ファイルデータ不正				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	JB_FWを設定する										*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar eota_SetJBFW(
		EOTA_UPDATE_SETUP_T * UpdateSetup,	/* [o] FW 更新指示ファイル情報	*/
		schar * Data)						/* [i] データ					*/
{
	uchar	Ret			= EOTA_FILE_OK;		/* 戻り値 				*/
	schar *  Token		= NULL;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == UpdateSetup)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FILE_PARAM_ERR;
	}
	else
	{
		Token = strtok(Data, EOTA_CHAR_COMMA);
		/* バージョン */
		if (NULL != Token)
		{
			memcpy(UpdateSetup->JbFw.Version, Token, strlen(Token));

			Token = strtok(NULL, EOTA_CHAR_COMMA);

			/* ファイル名 */
			if (NULL != Token)
			{
				memcpy(UpdateSetup->JbFw.File, Token, strlen(Token));

				Token = strtok(NULL, EOTA_CHAR_COMMA);

				/* MD5 ファイル */
				if (NULL != Token)
				{
					memcpy(UpdateSetup->JbFw.Md5File, Token, strlen(Token));

					Token = strtok(NULL, EOTA_CHAR_COMMA);

					/* EEPROM Checksum */
					if (NULL != Token)
					{
						memcpy(UpdateSetup->JbFw.EepromChecksum,
								Token,
								strlen(Token));

						Token = strtok(NULL, EOTA_CHAR_COMMA);

						/* Flash Checksum */
						if (NULL != Token)
						{
							memcpy(UpdateSetup->JbFw.FalshChekusum,
									Token,
									strlen(Token));
						}
						else
						{
							/* 処理無し */
						}
					}
					else
					{
						/* 処理無し */
					}
				}
				else
				{
					DLOG_Error("%s",EOTA_FILE_DATA_L_ERR);
					Ret = EOTA_FILE_DATA_ERR;
				}
			}
			else
			{
				/* 処理無し */
			}
		}
		else
		{
			/* 処理無し */
		}
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	FW 更新指示ファイル情報保存							*/
/*--------------------------------------------------------------------------*/
/*	param			:	左側値(:)											*/
/* 				 		右側値(:)											*/
/* 				 		FW 更新指示ファイル情報								*/
/*--------------------------------------------------------------------------*/
/*	return			:	処理結果											*/
/* 						EOTA_FILE_OK:処理正常								*/
/*						EOTA_FILE_PARAM_ERR:チェック異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	FW 更新指示ファイル情報を保存する					*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar eota_InfoSave(
		const schar *	     Left,			/* [i] :左側値					*/
		schar *	    		 Right,			/* [i] :右側値					*/
		EOTA_UPDATE_SETUP_T * UpdateSetup)	/* [o] FW 更新指示ファイル情報  */
{
	/* 変数初期化 */
	uchar	Ret			= EOTA_FILE_OK;				/* 戻り値 				*/
	sint	StrcmpRet	= EOTA_STRCMP_INDENTICAL;	/* 文字列比較戻り値		*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == UpdateSetup)
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FILE_PARAM_ERR;
	}
	else
	{
		/* NO */
		StrcmpRet = strncmp(Left, EOTA_SETUP_NO, strlen(Left));
		if (EOTA_STRCMP_INDENTICAL == StrcmpRet)
		{
			memcpy(UpdateSetup->No, Right, strlen(Right));
			return Ret;
		}
		else
		{
			/* 処理無し */
		}

		/* TYPE */
		StrcmpRet = strncmp(Left, EOTA_SETUP_TYPE_STR, strlen(Left));
		if (EOTA_STRCMP_INDENTICAL == StrcmpRet)
		{
			memcpy(UpdateSetup->Type, Right, strlen(Right));
			return Ret;
		}
		else
		{
			/* 処理無し */
		}

		/* MODE */
		StrcmpRet = strncmp(Left, EOTA_SETUP_MODE, strlen(Left));
		if (EOTA_STRCMP_INDENTICAL == StrcmpRet)
		{
			memcpy(UpdateSetup->Mode, Right, strlen(Right));
			return Ret;
		}
		else
		{
			/* 処理無し */
		}

		/* BOARD */
		StrcmpRet = strncmp(Left, EOTA_SETUP_TGT_BOARD, strlen(Left));
		if (EOTA_STRCMP_INDENTICAL == StrcmpRet)
		{
			memcpy(UpdateSetup->TgtBoard, Right, strlen(Right));
			return Ret;
		}
		else
		{
			/* 処理無し */
		}

		/* INV */
		StrcmpRet = strncmp(Left, EOTA_SETUP_TGT_INV, strlen(Left));
		if (EOTA_STRCMP_INDENTICAL == StrcmpRet)
		{
			memcpy(UpdateSetup->TgtInv, Right, strlen(Right));
			return Ret;
		}
		else
		{
			/* 処理無し */
		}

		/* PV */
		StrcmpRet = strncmp(Left, EOTA_SETUP_TGT_PV, strlen(Left));
		if (EOTA_STRCMP_INDENTICAL == StrcmpRet)
		{
			memcpy(UpdateSetup->TgtPv, Right, strlen(Right));
			return Ret;
		}
		else
		{
			/* 処理無し */
		}

		/* BDC */
		StrcmpRet = strncmp(Left, EOTA_SETUP_TGT_BDC, strlen(Left));
		if (EOTA_STRCMP_INDENTICAL == StrcmpRet)
		{
			memcpy(UpdateSetup->TgtBdc, Right, strlen(Right));
			return Ret;
		}
		else
		{
			/* 処理無し */
		}

		/* J.B. */
		StrcmpRet = strncmp(Left, EOTA_SETUP_TGT_JB, strlen(Left));
		if (EOTA_STRCMP_INDENTICAL == StrcmpRet)
		{
			memcpy(UpdateSetup->TgtJb, Right, strlen(Right));
			return Ret;
		}
		else
		{
			/* 処理無し */
		}

		/* TIME */
		StrcmpRet = strncmp(Left, EOTA_SETUP_TGT_TIME, strlen(Left));
		if (EOTA_STRCMP_INDENTICAL == StrcmpRet)
		{
			memcpy(UpdateSetup->Time, Right, strlen(Right));
			return Ret;
		}
		else
		{
			/* 処理無し */
		}

		/* BOARD_FW */
		StrcmpRet = strncmp(Left, EOTA_SETUP_TGT_BOARD_FW, strlen(Left));
		if (EOTA_STRCMP_INDENTICAL == StrcmpRet)
		{
			Ret = eota_SetBoardFW(UpdateSetup, Right);
			return Ret;
		}
		else
		{
			/* 処理無し */
		}

		/* INV_FW */
		StrcmpRet = strncmp(Left, EOTA_SETUP_TGT_INV_FW, strlen(Left));
		if (EOTA_STRCMP_INDENTICAL == StrcmpRet)
		{
			Ret = eota_SetInvFW(UpdateSetup, Right);
			return Ret;
		}
		else
		{
			/* 処理無し */
		}

		/* PV_FW */
		StrcmpRet = strncmp(Left, EOTA_SETUP_TGT_PV_FW, strlen(Left));
		if (EOTA_STRCMP_INDENTICAL == StrcmpRet)
		{
			Ret = eota_SetPVFW(UpdateSetup, Right);
			return Ret;
		}
		else
		{
			/* 処理無し */
		}

		/* BDC_FW */
		StrcmpRet = strncmp(Left, EOTA_SETUP_TGT_BDC_FW, strlen(Left));
		if (EOTA_STRCMP_INDENTICAL == StrcmpRet)
		{
			Ret = eota_SetBDCFW(UpdateSetup, Right);
			return Ret;
		}
		else
		{
			/* 処理無し */
		}

		/* J.B._FW */
		StrcmpRet = strncmp(Left, EOTA_SETUP_TGT_JB_FW, strlen(Left));
		if (EOTA_STRCMP_INDENTICAL == StrcmpRet)
		{
			Ret = eota_SetJBFW(UpdateSetup, Right);
			return Ret;
		}
		else
		{
			/* 処理無し */
		}
	}

	DEBUG_TRCOUT();

	return Ret;
}

/*==========================================================================*/
/*	Description		:	一行目のデータ										*/
/*--------------------------------------------------------------------------*/
/*	param			:	一行目のデータ										*/
/* 						 FW 更新指示ファイル情報							*/
/*--------------------------------------------------------------------------*/
/*	return			:	処理結果											*/
/* 						EOTA_FILE_OK:処理正常								*/
/*						EOTA_FILE_PARAM_ERR:チェック異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	一行目のデータを取得する							*/
/*--------------------------------------------------------------------------*/
/*	history 		:	2023/03/20 : DHC.zhangs : 新規作成					*/
/*==========================================================================*/
uchar eota_parse_line(
		schar *	Buff,						/* [i] 一行目のデータ           */
		EOTA_UPDATE_SETUP_T * UpdateSetup)	/* [o] FW 更新指示ファイル情報  */
{
	/* 変数初期化 */
	uchar	Ret		 			 				 = EOTA_FILE_OK;
											/* 戻り値				*/
    schar 	tmp_left[EOTA_SETUP_LINE_L_SIZE]	 = {0};
											/* :左側値(tmp)			*/
    schar	tmp_right[EOTA_SETUP_LINE_R_SIZE]	 = {0};
											/* :右側値(tmp)			*/
    schar	tmp_data[EOTA_SETUP_LINE_L_SIZE + EOTA_SETUP_LINE_R_SIZE]	 = {0};
											/* tmpデータ			*/
    schar	Left[EOTA_SETUP_LINE_L_SIZE]		 = {0};
											/* :左側値				*/
    schar	Right[EOTA_SETUP_LINE_R_SIZE]		 = {0};
											/* :右側値				*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == Buff) || (NULL == UpdateSetup))
	{
		/* パラメータはNULLの場合、エラーログを出力する */
		DLOG_Error("%s",EOTA_FUN_PARAM_NULL);
		Ret = EOTA_FILE_PARAM_ERR;
	}
	else
	{
		/* 処理無し */
	}

	/* トリム処理(左側)を実施 */
	if(EOTA_FILE_OK == Ret)
	{
		/* 空白など文字列を削除 */
		Ret = eota_LeftTrim(Buff, tmp_data);

		/* セミコロンのみの場合、この行目を処理しない */
		if ((EOTA_FILE_OK != Ret)
			|| ((0 < strlen(tmp_data)) && (EOTA_CHAR_SEM == tmp_data[0])))
		{
		    /* 処理無し */
		}
		else
		{
			/* 文字列数分繰り返す */
			for(size_t index = 0; index < strlen(tmp_data); index++)
			{
				if (EOTA_CHAR_SEM == tmp_data[index])
				{
					memcpy(tmp_left, tmp_data, index);
					memcpy(tmp_right,
							&tmp_data[index + 1],
							strlen(tmp_data) - index - 1);
					Ret = eota_Trim(tmp_left, Left);

					if (EOTA_FILE_OK == Ret)
					{
						Ret = eota_Trim(tmp_right, Right);

						if (EOTA_FILE_OK == Ret)
						{
							/* リストにてデータ保存 */
							Ret = eota_InfoSave(Left, Right, UpdateSetup);

							if (EOTA_FILE_OK != Ret)
							{
								index = strlen(tmp_data);
							}
							else
							{
								/* 処理無し */
							}
						}
						else
						{
							/* 処理無し */
						}
					}
					else
					{
						/* 処理無し */
					}
				}
				else
				{
					/* 処理無し */
				}
			}
		}
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return Ret;
}

