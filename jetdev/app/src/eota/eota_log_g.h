/****************************************************************************/
/*	Description		:FW更新機能 ログヘッダファイル							*/
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
#ifndef	__EOTA_LOG_G_H__
#define	__EOTA_LOG_G_H__

//#include <elog.h>
#include <stdio.h>

#include "typedef.h"

/*--------------------------------------------------------------------------*/
/* マクロ定義																*/
/*--------------------------------------------------------------------------*/
/* FATAL */

/* エラー */
#define EOTA_FUN_PARAM_NULL				((schar *)"Function's Parameter is NULL.")
												/* 関数のパラメータエラー	*/
#define EOTA_FILE_NAME_SIZE_ERR			((schar *)"File Name size Error!.")
												/* ファイル名サイズエラー	*/
#define EOTA_FILE_NAME_ERR				((schar *)"File Name Error!.")
												/* ファイル名エラー			*/
#define EOTA_FILE_NAME_NUM_ERR			((schar *)"File Name Number Error!.")
												/* ファイル名半角英数字以外	*/
#define EOTA_FILE_NAME_EXT_ERR			((schar *)"File Name Ext Error!.")
												/* ファイル名拡張子エラー	*/
#define EOTA_FILE_GET_DATA_ERR			((schar *)"Get File Data Error!.")
												/* ファイルデータ取得エラー	*/

#define EOTA_OPEN_DIR_FAIL_ERR			((schar *)"Open Folder failed.")
												/* フォルダオープンエラー	*/
#define EOTA_OPEN_FILE_FAIL_ERR			((schar *)"Open File failed.")
												/* ファイルオープンエエラー	*/
#define EOTA_CLOSE_FILE_FAIL_ERR		((schar *)"Close File failed.")
												/* ファイルクローズエラー	*/
#define EOTA_READ_FILE_FAIL_ERR			((schar *)"Read File failed.")
												/* ファイル読み込むエラー	*/
#define EOTA_WRITE_FILE_FAIL_ERR		((schar *)"Write File failed.")
												/* ファイル書き込みエラー	*/

#define EOTA_TAR_ZCVF_ERR				((schar *)"Tar zcvf error.")
												/* ファイル解凍エラー		*/
#define EOTA_FILE_DIR_NO_ERR			((schar *)"Path plan data is not exist.")
												/* パス存在しない			*/
#define EOTA_FILE_DEL_L_ERR				((schar *)"Delete File error.")
												/* ファイル削除エラー		*/
#define EOTA_FILE_COPY_L_ERR			((schar *)"Copy File error.")
												/* ファイルコピーエラー		*/

#define EOTA_FILE_MD5_ERR				((schar *)"File Md5 error.")
												/* MD5チェック不正			*/

#define EOTA_FILE_DATA_L_ERR			((schar *)"File Data error.")
												/* ファイルデータ不正		*/

/* FW 更新指示ファイル */
#define EOTA_SETUP_NO_NOT_NUM_ERR		((schar *)"NO is not number error.")
												/* NO項目半角英数字以外		*/
#define EOTA_SETUP_NO_RANGE_ERR			((schar *)"NO's Range error.(1～65535)")
												/* NO項目範囲不正			*/
#define EOTA_SETUP_MODE_ERR				((schar *)"MODE's Value error.")
												/* MODE項目値不正			*/
#define EOTA_SETUP_TGT_NOT_NUM_ERR		((schar *)"TGT is not number error.")
												/* TGT項目半角英数字以外	*/
#define EOTA_SETUP_TGT_RANGE_ERR		((schar *)"TGT's Range error.(0～5)")
												/* TGT項目範囲不正			*/
#define EOTA_SETUP_TGT_REPEAT_ERR		((schar *)"TGT's Repeat error.")
												/* TGT項目重複定義			*/
#define EOTA_SETUP_TGT_ORDER_ERR		((schar *)"TGT's number discontinuous error.")
												/* TGT項目番号連続エラー	*/
#define EOTA_SETUP_TGT_ALL_Z_ERR		((schar *)"TGT's All 0 error.")
												/* TGT項目番号全て0エラー	*/
#define EOTA_SETUP_TIME_LEN_ERR			((schar *)"Time's Length error.")
												/* TIMEのサイズ不正			*/
#define EOTA_SETUP_TIME_FORM_ERR		((schar *)"Time's Format error.")
												/* TIMEのフォーマット不正	*/
#define EOTA_SETUP_TYPE_ERR				((schar *)"TYPE's Value error.")
												/* TYPE項目値不正			*/
/* NORMAL */
#define EOTA_UPD_VER_SAME				((schar *)"Update Version Same.")
												/* バージョンと同じ			*/

/* DEBUG */

/* VERBOSE */

/*--------------------------------------------------------------------------*/
/* 非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
/* TODO. */
typedef enum{
	ELOG_LV_FATAL = 0,			/* 致命的なレベル(続行不可)	*/
	ELOG_LV_ERROR,				/* エラー発生				*/
	ELOG_LV_NORMAL,				/* 通常処理					*/
	ELOG_LV_DEBUG,				/* デバッグ用途				*/
	ELOG_LV_VERBOSE				/* 冗長なログ				*/
}ELOG_LV_E;

/*--------------------------------------------------------------------------*/
/* 定数宣言、定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* 変数宣言、定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* プロトタイプ宣言															*/
/*--------------------------------------------------------------------------*/

#endif	/* __EOTA_LOG_G_H__ */
