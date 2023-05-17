/******************************************************************************/
/*    Description    :共通ヘッダ                                               */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC GU.CM                                               */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/03/22                                              */
/*----------------------------------------------------------------------------*/
/*    Module Name    :                                                        */
/*----------------------------------------------------------------------------*/
/*    Note           :                                                        */
/*----------------------------------------------------------------------------*/
/*    Copyright      :(c) 2020 NICHICON CORPORATION                           */
/******************************************************************************/
#ifndef __COMMON_G_H__
#define __COMMON_G_H__

#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
/*----------------------------------------------------------------------------*/
/* マクロ定義                                                                  */
/*----------------------------------------------------------------------------*/
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

/* 共有リソース取得・解放 */
#ifdef MALLOC_DEBUG
#define CMN_MALLOC(size) CMN_LogMalloc(size, __FILE__, __LINE__)
#define CMN_FREE(p) {if (NULL != p) {CMN_LogFree(p, __FILE__, __LINE__); p = NULL;}}
#else
#define CMN_MALLOC(size) malloc(size)
#define CMN_FREE(p) {if (NULL != p) {free(p); p = NULL;}}
#endif

#define CMN_FOPEN(path, mode) CMN_FopenBk(path, mode)
#define CMN_FCLOSE(fp) {if ((fp) != NULL){CMN_FcloseBk(fp); (fp) = NULL;}}
#define CMN_OPEN(path, flags, mode) CMN_OpenBk(path, flags, mode)
#define CMN_CLOSE(fd) {if ((fd) >= 0){CMN_CloseBk(fd);}}

#define	CMN_PATH_LEN			((uchar)128u)	/* PATH最大長 */

/*----------------------------------------------------------------------------*/
/* 非公開型宣言、定義                                                          */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 定数宣言、定義                                                              */
/*--------------------------------------- ------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 変数宣言、定義                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* プロトタイプ宣言                                                            */
/*----------------------------------------------------------------------------*/

/* 共有リソース取得・解放 */
void *CMN_LogMalloc(size_t size, schar *file, ulong line);
void CMN_LogFree(void *addr, schar *file, ulong line);

/* 32bit CRC計算 */ 
ulong CMN_Crc32(uchar *buf, ulong len);
ulong CMN_Crc32Type2(uchar *buf, ulong len, ulong c);

ulong CMN_Crc32L2Buf(ulong crc, uchar *buf);
ulong CMN_Crc32Buf2L(uchar *buf);
/* システムコールラッパー */
sint CMN_MutexInit(pthread_mutex_t *pmutex);
sint CMN_MutexLock(pthread_mutex_t *pmutex);
sint CMN_MutexUnlock(pthread_mutex_t *pmutex);

/* バックアップ付きファイルオープン・クローズ */
sint CMN_OpenBk(const schar *filePath, sint flags, mode_t mode);
sint CMN_CloseBk(sint fd);
FILE* CMN_FopenBk(const schar *filePath, const schar* mode);
sint CMN_FcloseBk(FILE* fp);
#endif /*__COMMON_G_H__*/
