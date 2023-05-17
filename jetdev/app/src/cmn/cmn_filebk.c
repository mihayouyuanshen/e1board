/******************************************************************************/
/*    Description    :バックアップ付きファイルオープン・クローズ                  */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC GU.CM                                              */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/03/22                                              */
/*----------------------------------------------------------------------------*/
/*    Module Name    :                                                        */
/*----------------------------------------------------------------------------*/
/*    Note           :                                                        */
/*----------------------------------------------------------------------------*/
/*    Copyright      :(c) 2020 NICHICON CORPORATION                           */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Include Header File                                                        */
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "dlog_g.h"
#include "common_g.h"
#include "cmn_filebk.h"

/*----------------------------------------------------------------------------*/
/* 公開変数定義                                                                */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開変数定義                                                              */
/*----------------------------------------------------------------------------*/
#define CMN_BK_EXT			".bk"					/* バックアップファイル拡張子 */
#define CMN_CRC_EXT			".crc"					/* CRCファイル拡張子 */
#define CMN_BK_PATH_LEN		(CMN_PATH_LEN + 5)		/* バックアップファイルPATH最大長 */

/* バックアップファイルPATH生成マクロ */
#define CMN_MK_BK_PATH(filePath, path) snprintf(path, CMN_BK_PATH_LEN, "%s%s", filePath, CMN_BK_EXT)
#define CMN_MK_CRC_PATH(filePath, path) snprintf(path, CMN_BK_PATH_LEN, "%s%s", filePath, CMN_CRC_EXT)

#define CMN_OP_FILE_FAIL			((ssize_t)-1)
#define CMN_OP_FILE_OK				((ssize_t)0)

#define CMN_CHECK_FILEPATH_USER		((sint)1)
#define CMN_CHECK_FILEPATH_NOT_USER	((sint)0)

#define CMN_RESTORE_BK_FAIL			((sint)-1)
#define CMN_RESTORE_BK_OK			((sint)1)
#define CMN_RESTORE_BK_NULL			((sint)0)

#define CMN_CREATE_BK_FAIL			((sint)-1)
#define CMN_CREATE_BK_OK			((sint)1)
#define CMN_CREATE_BK_NULL			((sint)0)

#define CMN_ERR						((sint)-1)
#define CMN_OK						((sint)0)

/*----------------------------------------------------------------------------*/
/* 公開関数定義                                                                */
/*----------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:バックアップ付きファイルオープン							*/
/*--------------------------------------------------------------------------*/
/*	param			:const schar *filePath	ファイルPATH					*/
/*					:sint flag	オープンフラグ								*/
/*					:mode_t mode	オープンモード							*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
sint CMN_OpenBk(const schar *filePath, sint flags, mode_t mode)
{
	sint rcd = 0;
	sint bkflg = 0;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == filePath)
	{
		return CMN_ERR;
	}

	/* バックアップ対象ディレクトリ以外は通常open */
	if (CMN_CHECK_FILEPATH_NOT_USER == cmn_ChkBkTarget(filePath))
	{
		rcd = open(filePath, flags, mode);
	}
	else
	{
		/* ファイルリストア */
		bkflg = cmn_FileRestore(filePath);

		/* モード判定 */
		if ((O_WRONLY == (flags & O_WRONLY)) || (O_RDWR == (flags & O_RDWR)))
		{
			/* バックアップが無ければ生成 */
			if (CMN_RESTORE_BK_NULL == bkflg)
			{
				cmn_CreateBkFile(filePath);
			}

			/* ファイルオープン */
			rcd = open(filePath, flags, mode);
		}
	}

	DEBUG_TRCOUT();

	return rcd;
}

/*==========================================================================*/
/*	Description		:バックアップ付きファイルクローズ							*/
/*--------------------------------------------------------------------------*/
/*	param			:sint fd	ファイルディスクリプタ						*/
/*--------------------------------------------------------------------------*/
/*	return			:sint													*/
/*					(0:成功/-1:失敗)										*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
sint CMN_CloseBk(sint fd)
{
	schar linkPath[CMN_BK_PATH_LEN];
	schar filePath[CMN_PATH_LEN + 1];
	schar bkPath[CMN_BK_PATH_LEN];
	ssize_t sz = 0;
	sint rcd = 0;

	DEBUG_TRCIN();

	/* バッファ初期化 */
	memset(linkPath, 0x00, CMN_BK_PATH_LEN);
	memset(filePath, 0x00, (CMN_PATH_LEN + 1));

	/* ファイルを同期 */
	fsync(fd);

	/* ファイルPATH生成 */
	snprintf(linkPath, CMN_BK_PATH_LEN, "/proc/%d/fd/%d", getpid(), fd);
	sz = readlink(linkPath, filePath, CMN_PATH_LEN);
	if (CMN_OP_FILE_FAIL != sz)
	{
		/* バックアップ対象ディレクトリのファイルならバックアップ削除 */
		if (CMN_CHECK_FILEPATH_USER == cmn_ChkBkTarget(filePath))
		{
			/* バックアップファイル削除 */
			memset(bkPath, 0x00, CMN_BK_PATH_LEN);
			CMN_MK_BK_PATH(filePath, bkPath);
			remove(bkPath);
			/* CRCファイル削除 */
			memset(bkPath, 0x00, CMN_BK_PATH_LEN);
			CMN_MK_CRC_PATH(filePath, bkPath);
			remove(bkPath);
		}
	}

	/* ファイルクローズ */
	rcd = close(fd);

	DEBUG_TRCOUT();

	return rcd;
}

/*==========================================================================*/
/*	Description		:バックアップ付きファイルオープン							*/
/*--------------------------------------------------------------------------*/
/*	param			:sint fd	ファイルディスクリプタ						*/
/*--------------------------------------------------------------------------*/
/*	return			:FILE *													*/
/*					(!NULL:成功/NULL:失敗)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
FILE* CMN_FopenBk(const schar *filePath, const schar* mode)
{
	FILE *rcd = NULL;
	sint bkflg = 0;

	DEBUG_TRCIN();

	if (NULL == filePath) 
	{
		return NULL;
	}

	if (NULL == mode)
	{
		return NULL;
	}

	/* バックアップ対象ディレクトリ以外なら通常fopen */
	if (CMN_CHECK_FILEPATH_NOT_USER == cmn_ChkBkTarget(filePath)) 
	{
		rcd = fopen(filePath, mode);
	}
	else
	{
		/* ファイルリストア */
		bkflg = cmn_FileRestore(filePath);

		/* モード判定 */
		if ((NULL != strstr(mode, "w")) || (NULL != strstr(mode, "a")))
		{
			/* バックアップが無ければ生成 */
			if (CMN_RESTORE_BK_NULL == bkflg)
			{
				cmn_CreateBkFile(filePath);
			}
		}

		/* ファイルオープン */
		rcd = fopen(filePath, mode);
	}

	DEBUG_TRCOUT();

	return rcd;
}

/*==========================================================================*/
/*	Description		:バックアップ付きファイルクローズ							*/
/*--------------------------------------------------------------------------*/
/*	param			:sint fd	ファイルディスクリプ						*/
/*--------------------------------------------------------------------------*/
/*	return			:FILE *													*/
/*					(!NULL:成功/NULL:失敗)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
sint CMN_FcloseBk(FILE* fp)
{
	sint fd = 0;
	schar linkPath[CMN_BK_PATH_LEN];
	schar filePath[CMN_PATH_LEN + 1];
	schar bkPath[CMN_BK_PATH_LEN];
	ssize_t sz = 0;
	sint rcd = 0;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == fp)
	{
		return CMN_ERR;
	}

	/* バッファ初期化 */
	memset(linkPath, 0x00, CMN_BK_PATH_LEN);
	memset(filePath, 0x00, (CMN_PATH_LEN + 1));

	/* ファイルディスクリプタ取得 */
	fd = fileno(fp);

	/* ファイルを同期 */
	fflush(fp);
	fsync(fd);

	/* ファイルPATH生成 */
	snprintf(linkPath, CMN_BK_PATH_LEN, "/proc/%d/fd/%d", getpid(), fd);
	sz = readlink(linkPath, filePath, CMN_PATH_LEN);
	if (CMN_OP_FILE_FAIL != sz)
	{
		/* バックアップ対象ディレクトリのファイルならバックアップ削除 */
		if (CMN_CHECK_FILEPATH_USER == cmn_ChkBkTarget(filePath)) 
		{
			/* バックアップファイル削除 */
			memset(bkPath, 0x00, CMN_BK_PATH_LEN);
			CMN_MK_BK_PATH(filePath, bkPath);
			remove(bkPath);
			/* CRCファイル削除 */
			memset(bkPath, 0x00, CMN_BK_PATH_LEN);
			CMN_MK_CRC_PATH(filePath, bkPath);
			remove(bkPath);
		}
	}

	/* ファイルクローズ */
	rcd = fclose(fp);

	DEBUG_TRCOUT();

	return rcd;
}

/*----------------------------------------------------------------------------*/
/* 非公開関数定義                                                              */
/*----------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:ファイル読み出し										*/
/*--------------------------------------------------------------------------*/
/*	param			:sint fd		ファイルディスクリプタ					*/
/*					:ulong len		対象データバイト数						*/
/*					:size_t size	読み出しサイズ							*/
/*--------------------------------------------------------------------------*/
/*	return			:ssize_t												*/
/*					(0<=:読み出したサイズ/-1:読み出し失敗)					*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
ssize_t cmn_ReadFile(sint fd, uchar* buf, size_t size)
{
	ssize_t rdSz = 0;
	ssize_t rdTotal = 0;
	uchar *rdBuf = NULL;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == buf) 
	{
		return CMN_ERR;
	}

	if (0 == size) 
	{
		return 0;
	}

	rdBuf = buf;

	while(((size_t)rdTotal < size) && (0 < rdSz)) 
	{
		rdSz = read(fd, rdBuf, (size - (size_t)rdTotal));
		if (0 > rdSz) 
		{
			rdTotal = CMN_OP_FILE_FAIL;
		} 
		else if (0 == rdSz) 
		{
			/* do nothing */
		} 
		else
		{
			rdTotal += rdSz;
			rdBuf += rdSz;
		}
	}

	DEBUG_TRCOUT();

	return rdTotal;
}

/*==========================================================================*/
/*	Description		:ファイル書き込み										*/
/*--------------------------------------------------------------------------*/
/*	param			:sint fd		ファイルディスクリプタ					*/
/*					:ulong len		書き込みデータバッファ					*/
/* 					:size_t size	書き込みサイズ							*/
/*--------------------------------------------------------------------------*/
/*	return			:ssize_t												*/
/*					(0<=:読み出したサイズ/-1:読み出し失敗)					*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/09 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
ssize_t cmn_WriteFile(sint fd, uchar* buf, size_t size)
{
	ssize_t wrSz = 0;
	ssize_t wrTotal = 0;
	uchar* wrBuf = NULL;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == buf)
	{
		return CMN_ERR;
	}

	if (0 == size)
	{
		return 0;
	}

	wrBuf = buf;

	while(((size_t)wrTotal < size) && (0 < wrSz))
	{
		wrSz = write(fd, wrBuf, (size - (size_t)wrTotal));
		if (0 > wrSz)
		{
			wrTotal = CMN_OP_FILE_FAIL;
		}
		else if (0 == wrSz)
		{
			/* do nothing */
		}
		else
		{
			wrTotal += wrSz;
			wrBuf += wrSz;
		}
	}

	DEBUG_TRCOUT();

	return wrTotal;
}

/*==========================================================================*/
/*	Description		:ファイルリストア										*/
/*--------------------------------------------------------------------------*/
/*	param			:const schar *filePath	対象ファイルPATH				*/
/*--------------------------------------------------------------------------*/
/*	return			:sint													*/
/*					(0:バックアップファイル無し/1:バックアップファイル有り/		*/
/*					-1:リストア失敗)										*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
sint cmn_FileRestore(const schar *filePath)
{
	size_t strLen = 0;
	sint rcd = 0;
	sint ret = 0;
	schar crcPath[CMN_BK_PATH_LEN];
	struct stat stCrc;
	uchar crcEnable = 0;
	schar bkPath[CMN_BK_PATH_LEN];
	struct stat stBk;
	uchar bkEnable = 0;
	ulong crcBk = 0;
	ulong crcCalc = 0;
	sint fd = -1;
	uchar *tmpBuf = NULL;
	ssize_t rdSz = 0;
	ssize_t wrSz = 0;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == filePath)
	{
		return CMN_RESTORE_BK_NULL;
	}

	strLen = strnlen(filePath, (CMN_PATH_LEN + 1));
	if (strLen < CMN_PATH_LEN)
	{
		memset(&stCrc, 0x00, sizeof(stCrc));
		memset(&stBk, 0x00, sizeof(stBk));

		/* バックアップファイルPATH生成 */
		memset(bkPath, 0x00, CMN_BK_PATH_LEN);
		CMN_MK_BK_PATH(filePath, bkPath);
		memset(crcPath, 0x00, CMN_BK_PATH_LEN);
		CMN_MK_CRC_PATH(filePath, crcPath);

		/* CRCファイル存在チェック */
		ret = stat(crcPath, &stCrc);
		if (0 == ret)
		{
			/* CRCファイル読み出し */
			fd = open(crcPath, O_RDONLY);
			if (0 <= fd)
			{
				rdSz = cmn_ReadFile(fd, (uchar*)&crcBk, sizeof(ulong));
				if (rdSz == sizeof(ulong))
				{
					crcEnable = 1;
				}
				close(fd);
				fd = (-1);
			}
		}

		/* CRCファイル有りならバックアップファイル読み出し */
		if (1 == crcEnable)
		{
			/* バックアップファイル存在チェック */
			ret = stat(bkPath, &stBk);
			if (0 == ret)
			{
				if (0 == stBk.st_size)
				{
					if (0 == crcBk)
					{
						bkEnable = 1;
					}
				}
				else
				{
					fd = open(bkPath, O_RDONLY);
					if (0 <= fd)
					{
						tmpBuf = (uchar *)malloc((ulong)stBk.st_size);
						if (NULL == tmpBuf)
						{
							close(fd);
							fd = -1;
						}
						else
						{
							rdSz = cmn_ReadFile(fd, tmpBuf, (ulong)stBk.st_size);
							if (rdSz == stBk.st_size)
							{
								crcCalc = CMN_Crc32(tmpBuf, (ulong)stBk.st_size);
								if (crcCalc == crcBk) 
								{
									bkEnable = 1;
								}
							}
							else
							{
								close(fd);
								fd = -1;
							}
						}
					}
					else
					{
						/* do nothing */
					}
				}
			}
			else
			{
			/* do nothing */
			}
		}

		/* バックアップ有りならファイルリストア */
		if (1 == bkEnable)
		{
			/* 対象ファイルオープン */
			fd = open(filePath, (O_WRONLY | O_TRUNC));
			if (0 <= fd)
			{
				if (NULL != tmpBuf)
				{
					/* バックアップから読み出したデータを書き込み */
					wrSz = cmn_WriteFile(fd, tmpBuf, (ulong)stBk.st_size);
					if (wrSz != stBk.st_size)
					{
						DLOG_Error("[cmn_FileRestore] write Fail\n");
						rcd = CMN_RESTORE_BK_FAIL;	/* リストア失敗 */
					}
					else
					{
						rcd = CMN_RESTORE_BK_OK;	/* リストア成功 */
					}
				}
				else
				{
					rcd = CMN_RESTORE_BK_OK;	/* リストア成功 */
				}
				close(fd);
				fd = (-1);
			}
			else
			{
				DLOG_Error("[cmn_FileRestore] open Fail\n");
				rcd = CMN_RESTORE_BK_FAIL;	/* リストア失敗 */
			}
		}
		else
		{
			rcd = CMN_RESTORE_BK_NULL;	/* 有効バックアップ無し */
		}

		/* リストア失敗 or 有効バックアップ無しはバックアップ削除 */
		if (CMN_RESTORE_BK_OK != rcd)
		{
			remove(bkPath);
			remove(crcPath);
		}
		/* リストア失敗はファイル削除 */
		if (CMN_RESTORE_BK_FAIL == rcd)
		{
			remove(filePath);
		}

		/* バッファ解放 */
		if (NULL != tmpBuf)
		{
			free(tmpBuf);
			tmpBuf = NULL;
		}
	}
	else
	{
		rcd = CMN_RESTORE_BK_NULL;
	}


	DEBUG_TRCOUT();

	return rcd;
}

/*==========================================================================*/
/*	Description		:バックアップファイル生成								*/
/*--------------------------------------------------------------------------*/
/*	param			:const schar *filePath	対象ファイルPATH				*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void cmn_CreateBkFile(const schar *filePath)
{
	size_t strLen = 0;
	sint ret = 0;
	sint fdsrc = -1;
	sint fdbk = -1;
	sint fdcrc = -1;
	schar bkPath[CMN_BK_PATH_LEN];
	schar crcPath[CMN_BK_PATH_LEN];
	struct stat st;
	uchar *tmpBuf = NULL;
	ssize_t rdSz = 0;
	ssize_t wrSz = 0;
	ulong crcCalc = 0;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == filePath)
	{
		return;
	}

	strLen = strnlen(filePath, (CMN_PATH_LEN + 1));
	if (strLen < CMN_PATH_LEN)
	{
		/* バックアップファイルPATH生成 */
		memset(bkPath, 0x00, CMN_BK_PATH_LEN);
		memset(crcPath, 0x00, CMN_BK_PATH_LEN);
		CMN_MK_BK_PATH(filePath, bkPath);
		CMN_MK_CRC_PATH(filePath, crcPath);
	
		/* バックアップ元ファイル存在確認 */
		ret = stat(filePath, &st);
		if (0 == ret)
		{
			if (0 == st.st_size)
			{
				fdbk = open(bkPath, (O_CREAT | O_TRUNC | O_WRONLY), (S_IRWXU | S_IRWXG));
				if (0 <= fdbk)
				{
					/* CRCファイルオープン */
					fdcrc = open(crcPath, (O_CREAT | O_TRUNC | O_WRONLY), (S_IRWXU | S_IRWXG));
					if (0 <= fdcrc)
					{
						crcCalc = 0;	/* 空ファイルは0を書き込み */
						wrSz = cmn_WriteFile(fdcrc, (uchar*)&crcCalc, sizeof(ulong));
						fsync(fdcrc);
						close(fdcrc);
						fdcrc = (-1);
					}
					fsync(fdbk);
					close(fdbk);
					fdbk = (-1);
				}
			}
			else
			{
				fdsrc = open(filePath, O_RDONLY);
				if (0 <= fdsrc)
				{
					tmpBuf = (uchar*)malloc((ulong)st.st_size);
					if (NULL == tmpBuf)
					{
						close(fdsrc);
						fdsrc = -1;
					}
					else
					{
						rdSz = cmn_ReadFile(fdsrc, tmpBuf, (ulong)st.st_size);
						if (rdSz == st.st_size)
						{
							fdbk = open(bkPath, (O_CREAT | O_WRONLY | O_TRUNC), (S_IRWXU | S_IRWXG));
							if (0 <= fdbk)
							{
								wrSz = cmn_WriteFile(fdbk, tmpBuf, (ulong)st.st_size);
								if (wrSz == st.st_size)
								{
									/* CRC算出 */
									crcCalc = CMN_Crc32(tmpBuf, (ulong)rdSz);
									/* CRCファイルオープン */
									fdcrc = open(crcPath, (O_CREAT | O_TRUNC | O_WRONLY), (S_IRWXU | S_IRWXG));
									if (0 <= fdcrc)
									{
										/* CRC書き込み */
										wrSz = cmn_WriteFile(fdcrc, (uchar*)&crcCalc, sizeof(ulong));
										fsync(fdcrc);
										close(fdcrc);
										fdcrc = -1;
									}
								}
								fsync(fdbk);
								close(fdbk);
								fdbk = -1;
							}
						}
					}
				}
				else
				{
					/* do nothing */
				}
			}
		}
		else
		{
			/* do nothing */
		}
	}
	else
	{
		/* do nothing */
	}
	
	
	close(fdsrc);
	fdsrc = -1;
	free(tmpBuf);
	tmpBuf = NULL;

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:バックアップ対象ファイルチェック							*/
/*--------------------------------------------------------------------------*/
/*	param			:const schar *filePath	ファイルPATH					*/
/*--------------------------------------------------------------------------*/
/*	return			:sint													*/
/*					(0:バックアップ非対称/1:バックアップ対象)					*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
sint cmn_ChkBkTarget(const schar *filePath)
{
	sint rcd = CMN_CHECK_FILEPATH_NOT_USER;

	DEBUG_TRCIN();

	/* パラメータ異常は非対称 */
	if (NULL == filePath)
	{
		rcd = CMN_CHECK_FILEPATH_NOT_USER;
	}
	else if (0 == memcmp("/mnt/user/egra/", filePath, 15))	/* /mnt/user/egra/ は非対称 */
	{
		rcd = CMN_CHECK_FILEPATH_NOT_USER;
	}
	else if (0 == memcmp("/mnt/user/", filePath, 10))	/* /mnt/user/以下は対象 */
	{
		rcd = CMN_CHECK_FILEPATH_USER;
	}
#ifndef WORK_KYC	/* N社案件用 */
	else if(memcmp("/mnt/log/sdp/", filePath, 13) == 0)	/* /mnt/log/sdp/以下は対象 */
	{
		rcd = CMN_CHECK_FILEPATH_USER;
	}
#endif
	else	/* /mnt/user/以外は非対称 */
	{
		rcd = CMN_CHECK_FILEPATH_NOT_USER;
	}

	DEBUG_TRCOUT();

	return rcd;
}

/* End Of File */
