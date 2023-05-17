/****************************************************************************/
/*	Description		:FW更新機能 ファイル操作ヘッダファイル					*/
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
#ifndef	__EOTA_FILE_H__
#define	__EOTA_FILE_H__

#include "typedef.h"

/*--------------------------------------------------------------------------*/
/* マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define EOTA_FILE_DOT				((schar *)".")
										/* .								*/
#define EOTA_FILE_DOUBLE_DOT		((schar *)"..")
										/* ..								*/

#define EOTA_FILE_DOT_LEN			((uchar)1u)
										/* . サイズ							*/
#define EOTA_FILE_DOUBLE_DOT_LEN	((uchar)2u)
										/* .. サイズ						*/

#define EOTA_D_TYPE_FILE			((uchar)8u)
										/* ディレクトリ種類:FILE			*/
#define EOTA_D_TYPE_LINK_FILE		((uchar)10u)
										/* ディレクトリ種類:LinkFILE		*/
#define EOTA_D_TYPE_DIR_US			((uchar)4u)
										/* ディレクトリ種類:ディレクトリ	*/

#define EOTA_FILE_EOF				((schar *)"EOF")
										/* EOF								*/
#define EOTA_CHAR_LIN_BREAK			((schar)'\n')
										/* \n								*/
#define EOTA_CHAR_NULL				((schar)'\0')
										/* \0								*/
#define EOTA_CHAR_TAB				((schar)'\t')
										/* \t								*/
#define EOTA_CHAR_SPACE				((schar)' ')
										/* 空白								*/
#define EOTA_CHAR_SEM				((schar)':')
										/* セミコロン						*/
#define EOTA_CHAR_DOU_QUO_MARK		((schar)'"')
										/* 二重引用符						*/
#define EOTA_CHAR_COMMA				((schar*)",")
										/* コンマ							*/
#define EOTA_OPEN_TYPE_READ			((schar*)"rb")
										/* ファイル読み込む					*/

#define EOTA_SETUP_NO				((schar *)"NO")
										/* NO								*/
#define EOTA_SETUP_TYPE_STR			((schar *)"TYPE")
										/* TYPE								*/
#define EOTA_SETUP_MODE				((schar *)"MODE")
										/* MODE								*/
#define EOTA_SETUP_TGT_BOARD		((schar *)"BOARD")
										/* TGT BOARD						*/
#define EOTA_SETUP_TGT_INV			((schar *)"INV")
										/* TGT INV							*/
#define EOTA_SETUP_TGT_PV			((schar *)"PV")
										/* TGT PV							*/
#define EOTA_SETUP_TGT_BDC			((schar *)"BDC")
										/* TGT BDC							*/
#define EOTA_SETUP_TGT_JB			((schar *)"J.B.")
										/* TGT JB							*/
#define EOTA_SETUP_TGT_TIME			((schar *)"TIME")
										/* TGT TIME							*/
#define EOTA_SETUP_TGT_BOARD_FW		((schar *)"BOARD_FW")
										/* TGT BOARD_FW						*/
#define EOTA_SETUP_TGT_INV_FW		((schar *)"INV_FW")
										/* TGT INV_FW						*/
#define EOTA_SETUP_TGT_PV_FW		((schar *)"PV_FW")
										/* TGT PV_FW						*/
#define EOTA_SETUP_TGT_BDC_FW		((schar *)"BDC_FW")
										/* TGT BDC_FW						*/
#define EOTA_SETUP_TGT_JB_FW		((schar *)"J.B._FW")
										/* TGT J.B._FW						*/

/*--------------------------------------------------------------------------*/
/* 非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
/* 圧縮ファイル有無 */
enum
{
	EOTA_TAR_FILE_OFF = 0,					/* 0:なし						*/
	EOTA_TAR_FILE_ON,						/* 1:あり						*/
};

/*--------------------------------------------------------------------------*/
/* 定数宣言、定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* 変数宣言、定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* プロトタイプ宣言															*/
/*--------------------------------------------------------------------------*/
/* トリム処理(左側) */
static uchar eota_LeftTrim(schar * Src, schar * Dst);
/* トリム処理 */
static uchar eota_Trim(const schar * Src, schar * Dst);
/* 設定BOARD_FW */
static uchar eota_SetBoardFW(EOTA_UPDATE_SETUP_T * UpdateSetup, schar * Data);
/* 設定INV_FW */
static uchar eota_SetInvFW(EOTA_UPDATE_SETUP_T * UpdateSetup, schar * Data);
/* 設定PV_FW */
static uchar eota_SetPVFW(EOTA_UPDATE_SETUP_T * UpdateSetup, schar * Data);
/* 設定BDC_FW */
static uchar eota_SetBDCFW(EOTA_UPDATE_SETUP_T * UpdateSetup, schar * Data);
/* 設定JB_FW */
static uchar eota_SetJBFW(EOTA_UPDATE_SETUP_T * UpdateSetup, schar * Data);
/* FW 更新指示ファイル情報保存 */
static uchar eota_InfoSave(
							const schar *	     Left,
							schar *	    		 Right,
							EOTA_UPDATE_SETUP_T * UpdateSetup);
/* 一行目のデータ */
uchar eota_parse_line(
					schar *	Buff,
					EOTA_UPDATE_SETUP_T * UpdateSetup);



#endif	/* __EOTA_FILE_H__ */
