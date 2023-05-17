/****************************************************************************/
/*	Description		:FW更新機能 設定ヘッダファイル							*/
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
#ifndef	__EOTA_SETTING_H__
#define	__EOTA_SETTING_H__

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <common_g.h>
#include <time.h>
#include <dirent.h>
#include <ctype.h> 

#include "eota_g.h"
#include "typedef.h"
#include "dlog_g.h"
#include "frm_g.h"
#include "eota_g.h"
#include "task_g.h"

/*--------------------------------------------------------------------------*/
/* マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define EOTA_UPD_CMD_DEL_FILE		((schar *)"rm -rf TODO\*.*")
										/* 既存の FW 更新ファイルコメント	*/
#define EOTA_UPD_CMD_IND_NAME		((schar *)"update_setup.txt")
										/* FW 更新指示ファイル				*/
#define EOTA_UPD_FILE_BOARD			((schar *)"BOARD_info.txt")
										/* 通信ボード FW 更新情報ファイル	*/
#define EOTA_UPD_FILE_INV			((schar *)"INV_info.txt")
										/* INV マイコン FW 更新情報ファイル	*/
#define EOTA_UPD_FILE_PV			((schar *)"PV_info.txt")
										/* PV マイコン FW 更新情報ファイル	*/
#define EOTA_UPD_FILE_BDC			((schar *)"BDC_info.txt")
										/* BDC マイコン FW 更新情報ファイル	*/
#define EOTA_UPD_FILE_JB			((schar *)"JB_info.txt")
										/* J.B. FW 更新情報ファイル			*/

/* 圧縮ファイル解凍後に FW 更新指示ファイル */
#define EOTA_PKG_TGZ_NAME_LEN		((uchar)17u)
										/* 圧縮ファイル名 サイズ			*/

#define EOTA_SETUP_DOWN_NO_MIN		((uchar)1u)
										/* 管理番号 最大値					*/
#define EOTA_SETUP_DOWN_NO_MAX		((ushort)65535u)
										/* 管理番号 最小値					*/
#define EOTA_SETUP_TYPE				((schar *)"fw_model_e1")
										/* fw_model_e1						*/

#define EOTA_STRCMP_INDENTICAL		((uchar)0u)
										/* 文字列同じ						*/
#define EOTA_SYSTEM_RET				((uchar)0u)
										/* system正常に実施					*/
#define EOTA_READ_LINE_ERR			((slong)-1)
										/* データ取得異常値					*/

#define EOTA_FW_VER_NUP_FILE_PATH	((schar *)"/home/test")
										/* for FW versionup TODO.			 */
#define EOTA_FW_USB_FILE_PATH		((schar *)"/home/test")
										/* USB パス TODO.					 */

/* 各配列サイズ */
#define EOTA_PKG_EXT_SIZE			((uchar)4u)
										/* 拡張子文字列サイズ				*/

#define	EOTA_TAR_FILE_DIR_LEN		((ushort)(4096))
										/* 圧縮ファイルのフルパスサイズ		*/
#define	EOTA_TAR_FILE_PATH_LEN		((ushort)(255))
										/* 圧縮ファイルのフォルダサイズ		*/

#define EOTA_SETUP_NO_SIZE			((uchar)5u)
										/* 管理番号サイズ					 */
#define EOTA_SETUP_TYPE_SIZE		((uchar)11u)
										/* fw_model_e1サイズ				 */
#define EOTA_SETUP_MODE_SIZE		((uchar)1u)
										/* MODEサイズ 						 */
#define EOTA_SETUP_TGT_SIZE			((uchar)1u)
										/* TGTサイズ 						 */
#define EOTA_SETUP_TIME_SIZE		((uchar)11u)
										/* TIMEサイズ 						 */
#define EOTA_SETUP_FILE_SIZE		((uchar)255u)
										/* ファイル名最大サイズ 			 */
#define EOTA_SETUP_VER_SIZE			((uchar)255u)
										/* バージョン最大サイズ 			 */
#define EOTA_SETUP_LINE_L_SIZE		((ulong)10u)
										/* 一行目最大サイズ(:左) 			 */
#define EOTA_SETUP_LINE_R_SIZE		((ulong)1276u)
										/* 一行目最大サイズ(:右) 			 */

#define EOTA_DATANO_SIZE			((uchar)4u)
										/* DataNo							 */
#define EOTA_FW_VERSION_SIZE		((uchar)15u)
										/* FW Versionサイズ					 */
#define EOTA_FW_VERSION_JB_SIZE		((uchar)2u)
										/* FW Version J.B.サイズ			 */
#define EOTA_EEPROM_CHECKSUM_SIZE	((uchar)2u)
										/* Eeprom Checksumサイズ			 */
#define EOTA_FLASH_CHECK_SUM_SIZE	((uchar)2u)
										/* Flash Checksumサイズ				 */
#define EOTA_FW_DATA_SIZE_SIZE		((uchar)8u)
										/* FW データサイズのサイズ			 */
#define EOTA_FW_DATA_SIZE			((ushort)1024u)
										/* FW データサイズ					 */
#define EOTA_RESEVED_SIZE			((uchar)9u)
										/* Resevedのサイズ					 */
#define EOTA_CRC32_SIZE				((uchar)8u)
										/* CRT32のサイズ					 */

#define EOTA_CMD11_MESS_MAX_SIZE	((ulong)1036u)
										/* メッセージの最大サイズ 			 */

/* タイマー */
#define EOTA_TIME_INV_RES			((uchar)1u)
										/* INV通信一回応答タイム			 */
#define EOTA_TIME_INV_TIME_OUT		((ushort)270u)
										/* INV通信タイムアウト				 */

/* DataNo  */
#define EOTA_DATANO_INV_INQ			((ushort)0xFF00u)
										/* INVマイコン転送問い合わせ		 */
#define EOTA_DATANO_PV_INQ			((ushort)0xFF01u)
										/* PVマイコン転送問い合わせ			 */
#define EOTA_DATANO_BDC_INQ			((ushort)0xFF02u)
										/* BDCマイコン転送問い合わせ		 */
#define EOTA_DATANO_JB_INQ			((ushort)0xFF03u)
										/* J.B.転送問い合わせ				 */
#define EOTA_DATANO_FW_S			((ushort)0x0000u)
										/* FW 転送開始						 */
#define EOTA_DATANO_FW_E			((ushort)0xFFFFu)
										/* FW 転送終了						 */
#define EOTA_DATANO_FW_F			((ushort)0xFFF1u)
										/* FW 転送完了						 */
#define EOTA_DATANO_BUSY			((ushort)0xFF10u)
										/* Busy 確認						 */
#define EOTA_DATANO_FW_NUM_S		((ushort)0x0001u)
										/* FW 転送開始番号					 */
#define EOTA_DATANO_FW_NUM_E		((ushort)0x9999u)
										/* FW 転送終了番号					 */

#define EOTA_FIlE_READ_ERR 			((slong)-1)
										/* ファイル読み込むエラー			*/

#define EOTA_TIME_YEAR_START		((ushort)1900u)
												/* 1900年					*/

/*--------------------------------------------------------------------------------------------------*/
/* 公開型宣言、定義																					*/
/*--------------------------------------------------------------------------------------------------*/
/* 戻り値(処理流れ) */
typedef enum _eota_flow_ret_value_t
{
	EOTA_FLOW_OK = 0,				/* 0:OK						*/
	EOTA_FLOW_NG,					/* 1:NG						*/
} EOTA_FLOW_RET_VALUE_T;

/* 戻り値(INV送受信) */
typedef enum _eota_inv_ret_value_t
{
	EOTA_INV_OK = 0,				/* 0:OK						*/
	EOTA_INV_NG,					/* 1:NG						*/
} EOTA_INV_RET_VALUE_T;

/* 戻り値(データ処理) */
typedef enum _eota_data_ret_value_t
{
	EOTA_DATA_OK = 0,				/* 0:OK						*/
	EOTA_DATA_NG,					/* 1:NG						*/
} EOTA_DATA_RET_VALUE_T;

/* 戻り値(チェック処理) */
typedef enum _eota_check_ret_value_t
{
	EOTA_CHECK_OK = 0,				  /* 0:OK					*/
	EOTA_CHECK_NG,					  /* 1:NG					*/
} EOTA_CHECK_RET_VALUE_T;

/*  戻り値(ファイル処理) */
typedef enum _eota_file_ret_value_t
{
	EOTA_FILE_OK = 0,				/* 0:OK						*/
	EOTA_FILE_PARAM_ERR,			/* 1:パラメータエラー		*/
	EOTA_FILE_OPEN_DIR_ERR,			/* 2:オープンエラー			*/
	EOTA_FILE_TAR_NOT_ERR,			/* 3:圧縮ファイルなし	  	*/
	EOTA_FILE_TAR_FAIL_ERR,			/* 4:解凍失敗	 		 	*/
	EOTA_FILE_DIR_NOT_ERR,			/* 6:ファイルのフォルダなし	*/
	EOTA_FILE_DATA_ERR,				/* 7:ファイルデータ不正		*/
	EOTA_FILE_DEL_ERR,				/* 8:削除失敗				*/
	EOTA_FILE_COPY_ERR,				/* 9:コピー失敗				*/
} EOTA_FILE_RET_VALUE_T;

/* メッセージID */
typedef enum _eota_msgid_t
{
	EOTA_MSGID_START = 0,			/* 0:FW更新開始				*/
	EOTA_MSGID_STOP,				/* 1:FW更新終了				*/
	EOTA_MSGID_CMD11_REQ,			/* 2:CMD11 受信 Data		*/
	EOTA_MSGID_CMD60_REQ,			/* 3:CMD60 受信 Data		*/
	EOTA_MSGID_SEND_CMD64,			/* 4:CMD64 を 1s 毎に送信	*/
	EOTA_MSGID_CMD64_TIMEOUT,		/* 5:CMD64 60s Timeout		*/
	EOTA_MSGID_CMD64_REQ,			/* 6:CMD60 受信 Data		*/
	EOTA_MSGID_CMD65_REQ,			/* 7:CMD60 受信 Data		*/
	EOTA_MSGID_USE_UPD,				/* 8:ユーザ更新時点			*/
} EOTA_MSGID_T;

/* FW 更新状態 */
typedef enum _eota_upd_t
{
	EOTA_UPD_WAIT = 0,				/* 0:待機					*/
	EOTA_UPD_DOWN,					/* 1:ダウンロード・解析		*/
	EOTA_UPD_START,					/* 2:FW 更新開始			*/
	EOTA_UPD_ING,					/* 3:FW 更新中				*/
	EOTA_UPD_END,					/* 4:FW 更新終了			*/
} EOTA_UPD_T;

/* FW 更新中状態 */
typedef enum _eota_upd_state_t
{
	EOTA_UPD_ING_START = 0,			/* 0:更新中 開始			*/
	EOTA_UPD_ING_CON,				/* 1:更新中 引き続き		*/
	EOTA_UPD_ING_INV_END,			/* 2:更新中 INV更新終了		*/
	EOTA_UPD_ING_ALL_END,			/* 3:更新中 すべての更新終了*/
} EOTA_UPD_STATE_T;

/* FW 更新 LED フラグ */
typedef enum _eota_upd_led_t
{
	EOTA_UPD_LED_OFF = 0,			/* 0:なし					*/
	EOTA_UPD_LED_ON,				/* 1:あり					*/
} EOTA_UPD_LED_T;

/* ダウンロード・解析対象 */
typedef enum _eota_upd_tgt_t
{
	EOTA_UPD_TGT_BOARD = 0,			/* 0:BOARD					*/
	EOTA_UPD_TGT_INV,				/* 1:INV					*/
	EOTA_UPD_TGT_PV,				/* 2:PV						*/
	EOTA_UPD_TGT_BDC,				/* 3:BDC					*/
	EOTA_UPD_TGT_J_B,				/* 4:J.B.					*/

	EOTA_UPD_TGT_LEN,				/* 5:サイズ					*/
} EOTA_UPD_TGT_T;

/* ダウンロード・解析結果 */
typedef enum _eota_upd_res_t
{
	EOTA_UPD_RES_NULL = 0,			/* 0:未実施					*/
	EOTA_UPD_RES_OK,				/* 1:OK						*/
	EOTA_UPD_RES_NG,				/* 2:NG						*/
} EOTA_UPD_RES_T;

/* 削除フラグ*/
typedef enum _eota_down_del_t
{
	EOTA_DOWN_DEL_FLG_OFF = 0,		/* 0:なし					*/
	EOTA_DOWN_DEL_FLG_ON,			/* 1:あり					*/
} EOTA_DOWN_DEL_T;

/* tgt */
typedef enum _eota_tgt_flg_t
{
	EOTA_TGT_FLG_OFF = 0,			/* 0:対象外					*/
	EOTA_TGT_FLG_ON,				/* 1:対象					*/
} EOTA_TGT_FLG_T;

/* FW 更新実施状態*/
typedef enum _eota_run_s_t
{
	EOTA_RUN_STATE_S = 0,			/* 0:Start					*/
	EOTA_RUN_STATE_E,				/* 1:End					*/
} EOTA_RUN_S_T;

/* 更新中止 */
typedef enum _eota_update_stop_t
{
	EOTA_UPDATE_STOP_OFF = 0,		/* 0:更新中止無し			*/
	EOTA_UPDATE_STOP_ON,			/* 1:更新中止				*/
} EOTA_UPDATE_STOP_T;

/* タイマー状態 */
typedef enum _eota_timer_state_t
{
	EOTA_TIME_NULL = 0,			 	/* 0:タイマー未実施			*/
	EOTA_TIME_ING,					/* 1:タイマー実施中			*/
} EOTA_TIMER_STATE_T;

/* タイマーID */
typedef enum _eota_timerid_t
{
	EOTA_TIMEID_INV_RES = 0,		/* 0:一回応答タイムID		*/
	EOTA_TIMEID_INV_TIME_OUT,		/* 1:通信タイムアウトID		*/
	EOTA_TIMEID_MODE_F_TIME,		/* 2:MODE:F更新タイム		*/
} EOTA_TIMERID_T;

/* BANK No */
typedef enum _eota_bank_no_t
{
	EOTA_BANK_NO_1 = 1,		 		/* 1:BANK1	 	    		*/
	EOTA_BANK_NO_2,		 			/* 2:BANK2					*/
} EOTA_BANK_NO_T;

/* FW 転送状態  */
typedef enum _eota_fw_tran_s_t
{
	EOTA_FW_TRAN_STATE_C = 0,	 	/* 0:FW 転送継続	 	 	*/
	EOTA_FW_TRAN_STATE_E,		 	/* 1:FW 転送終了	 	 	*/
} EOTA_FW_TRAN_ST_T;

/* ACK/NAK */
typedef enum _eota_ack_nak_info_t
{
	EOTA_NAK = 0,	 	  			/* 0:NAK	 	 			*/
	EOTA_ACK,		 	  			/* 1:ACK	 	 			*/
	EOTA_BUSY,		 	  			/* 2:Busy	 	 			*/
	EOTA_RE_SEND,		 			/* 3:再送要求	 	 		*/
	EOTA_F_RE_SEND,		 			/* 4:F:リトライ要求 	 	*/
} EOTA_ACK_NAK_INFO_T;

/* BOARD_FW情報 */
typedef struct _eota_board_fw_t
{
	schar Version[EOTA_SETUP_VER_SIZE + 1];		/* バージョン					*/
 	schar File[EOTA_SETUP_FILE_SIZE + 1];		/* ファイル名					*/
	schar Md5File[EOTA_SETUP_FILE_SIZE + 1];	/* MD5 ファイル					*/
} EOTA_BOARD_FW_T;

/* INV_FW情報 */
typedef struct _eota_inv_fw_t
{
	schar Version[EOTA_SETUP_VER_SIZE + 1];		/* バージョン					*/
	schar Bank1File[EOTA_SETUP_FILE_SIZE + 1];	/* BANK1 の FW ファイル名		*/
	schar Bank1Md5File[EOTA_SETUP_FILE_SIZE + 1];
												/* MD5 ファイル					*/
	schar Bank2File[EOTA_SETUP_FILE_SIZE + 1];	/* BANK2 の FW ファイル名		*/
	schar Bank2Md5File[EOTA_SETUP_FILE_SIZE + 1];
												/* MD5 ファイル					*/
} EOTA_INV_FW_T;

/* PV_FW情報 */
typedef struct _eota_pv_fw_t
{
	schar Version[EOTA_SETUP_VER_SIZE + 1];		/* バージョン					*/
	schar Bank1File[EOTA_SETUP_FILE_SIZE + 1];	/* BANK1 の FW ファイル名		*/
	schar Bank1Md5File[EOTA_SETUP_FILE_SIZE + 1];
												/* MD5 ファイル					*/
	schar Bank2File[EOTA_SETUP_FILE_SIZE + 1];	/* BANK2 の FW ファイル名		*/
	schar Bank2Md5File[EOTA_SETUP_FILE_SIZE + 1];
												/* MD5 ファイル					*/
} EOTA_PV_FW_T;

/* BDC_FW情報 */
typedef struct _eota_bdc_fw_t
{
	schar Version[EOTA_SETUP_VER_SIZE + 1];		/* バージョン					*/
	schar Bank1File[EOTA_SETUP_FILE_SIZE + 1];	/* BANK1 の FW ファイル名		*/
	schar Bank1Md5File[EOTA_SETUP_FILE_SIZE + 1];
												/* MD5 ファイル					*/
	schar Bank2File[EOTA_SETUP_FILE_SIZE + 1];	/* BANK2 の FW ファイル名		*/
	schar Bank2Md5File[EOTA_SETUP_FILE_SIZE + 1];
												/* MD5 ファイル					*/
} EOTA_BDC_FW_T;

/* J.B._FW情報 */
typedef struct _eota_jb_fw_t
{
	schar Version[EOTA_SETUP_VER_SIZE + 1];		/* バージョン					*/
 	schar File[EOTA_SETUP_FILE_SIZE + 1];		/* ファイル名					*/
	schar Md5File[EOTA_SETUP_FILE_SIZE + 1];	/* MD5 ファイル					*/
	schar EepromChecksum[EOTA_SETUP_FILE_SIZE + 1];
												/* EEPROM Checksum				*/
	schar FalshChekusum[EOTA_SETUP_FILE_SIZE + 1];
												/* Flash Checksum				*/
} EOTA_JB_FW_T;

/* FW 更新指示ファイル */
typedef struct _eota_update_setup_t
{
	schar No[EOTA_SETUP_NO_SIZE + 1];			/* 管理番号						*/
	schar Type[EOTA_SETUP_TYPE_SIZE + 1];		/* TYPE							*/
	schar Mode[EOTA_SETUP_MODE_SIZE + 1];		/* MODE							*/
	schar TgtBoard[EOTA_SETUP_TGT_SIZE + 1];	/* BOARD						*/
	schar TgtInv[EOTA_SETUP_TGT_SIZE + 1];		/* INV							*/
	schar TgtPv[EOTA_SETUP_TGT_SIZE + 1];		/* PV							*/
	schar TgtBdc[EOTA_SETUP_TGT_SIZE + 1];		/* BDC							*/
	schar TgtJb[EOTA_SETUP_TGT_SIZE + 1];		/* J.B.							*/
	schar Time[EOTA_SETUP_TIME_SIZE + 1];		/* TIME							*/

	EOTA_BOARD_FW_T BoardFw;					/* BOARD_FW						*/
	EOTA_INV_FW_T	InvFw;						/* INV_FW						*/
	EOTA_PV_FW_T	PvFw;						/* PV_FW						*/
	EOTA_BDC_FW_T	BdcFw;						/* BDC_FW						*/
	EOTA_JB_FW_T	JbFw;						/* J.B._FW						*/
} EOTA_UPDATE_SETUP_T;

/* 送信情報(CMD11) */
typedef struct _eota_cmd11_req_info_t
{
	uchar	DataNo[EOTA_DATANO_SIZE];			/* DataNo						*/
	uchar	FWVersion[EOTA_FW_VERSION_SIZE];	/* FW Version					*/
	uchar	FWVersionJB[EOTA_FW_VERSION_JB_SIZE];
												/* FW Version(J.B.)				*/
	uchar	EepromChecksum[EOTA_EEPROM_CHECKSUM_SIZE];
												/* Eeprom Checksum				*/
	uchar	FlashChecksum[EOTA_FLASH_CHECK_SUM_SIZE];
												/* Flash Checksum				*/
	uchar	Reseved[EOTA_RESEVED_SIZE];			/* Reseved						*/
	uchar	FWDataSize[EOTA_FW_DATA_SIZE_SIZE];	/* FW データサイズ				*/
	uchar	CRC32[EOTA_CRC32_SIZE];				/* CRC-32						*/
	uchar	FWData[EOTA_FW_DATA_SIZE];			/* FW Data						*/
}EOTA_CMD11_REQ_INFO_T;

/* 受信情報(CMD11) */
typedef struct _eota_cmd11_res_info_t
{
	uchar	DataNo[EOTA_DATANO_SIZE];			/* DataNo						*/
	uchar	BankNo;								/* BANK No						*/
	uchar	FwTranState;						/* FW 転送状態					*/
	uchar	AckNak;								/* ACK/NAK						*/
}EOTA_CMD11_RES_INFO_T;

/* タイム情報 */
typedef struct _eota_time_info_t
{
	uchar	InvTimeRes;							/* 一回応答タイム状態			*/
	uchar	InvTimeOut;							/* 通信タイムアウト状態			*/
} EOTA_TIME_INFO_T;

/* INV更新情報 */
typedef struct _eota_inv_upd_info_t
{
	schar Version[EOTA_SETUP_VER_SIZE + 1];		/* バージョン					*/
	schar Bank1File[EOTA_SETUP_FILE_SIZE + 1];	/* BANK1 の FW ファイル名		*/
	schar Bank2File[EOTA_SETUP_FILE_SIZE + 1];	/* BANK2 の FW ファイル名		*/
	slong Bank1FileSize;						/* BANK1 の ファイルサイズ		*/
	slong Bank2FileSize;						/* BANK2 の ファイルサイズ		*/
} EOTA_INV_UPD_INFO_T;

/* PV更新情報 */
typedef struct _eota_pv_upd_info_t
{
	schar Version[EOTA_SETUP_VER_SIZE + 1];		/* バージョン					*/
	schar Bank1File[EOTA_SETUP_FILE_SIZE + 1];	/* BANK1 の FW ファイル名		*/
	schar Bank2File[EOTA_SETUP_FILE_SIZE + 1];	/* BANK2 の FW ファイル名		*/
	slong Bank1FileSize;						/* BANK1 の ファイルサイズ		*/
	slong Bank2FileSize;						/* BANK2 の ファイルサイズ		*/
} EOTA_PV_UPD_INFO_T;

/* BDC更新情報 */
typedef struct _eota_bdc_upd_info_t
{
	schar Version[EOTA_SETUP_VER_SIZE + 1];		/* バージョン					*/
	schar Bank1File[EOTA_SETUP_FILE_SIZE + 1];	/* BANK1 の FW ファイル名		*/
	schar Bank2File[EOTA_SETUP_FILE_SIZE + 1];	/* BANK2 の FW ファイル名		*/
	slong Bank1FileSize;						/* BANK1 の ファイルサイズ		*/
	slong Bank2FileSize;						/* BANK2 の ファイルサイズ		*/
} EOTA_BDC_UPD_INFO_T;

/* J.B.更新情報 */
typedef struct _eota_jb_upd_info_t
{
	schar Version[EOTA_SETUP_VER_SIZE + 1];		/* バージョン					*/
 	schar File[EOTA_SETUP_FILE_SIZE + 1];		/* ファイル名					*/
	schar EepromChecksum[EOTA_SETUP_FILE_SIZE + 1];
												/* EEPROM Checksum				*/
	schar FalshChekusum[EOTA_SETUP_FILE_SIZE + 1];
												/* Flash Checksum				*/
	slong FileSize;								/* ファイルサイズ				*/
} EOTA_JB_UPD_INFO_T;

/* 更新状態情報 */
typedef struct _eota_upd_state_info_t
{
	uchar	AnalyRes;							/* V:解析結果					*/
	uchar	BoardTgt;							/* V:BOARD_tgt					*/
	uchar	InvTgt;								/* V:INV_tgt					*/
	uchar	PvTgt;								/* V:PV_tgt 					*/
	uchar	BdcTgt;								/* V:BDC_tgt					*/
	uchar	JbTgt;								/* V:J.B._tgt					*/
	uchar	DownDelFlg;							/* V:削除フラグ					*/
	uchar	UpdMode;							/* V:更新 Mode					*/
	uchar	UpdateState;						/* V:FW 更新状態				*/
	uchar	UpdateLed;							/* V:FW 更新 LED フラグ			*/
	uchar	DownAnalyTgt;						/* V:ダウンロード・解析対象		*/

	uchar	DownSrc;							/* ダウンロード経由				*/
	uchar	fwRunState;							/* FW 更新実施状態(開始・終了)	*/
	uchar	UpdateStop;							/* 更新中止フラグ				*/

	uchar	UpdProjectState;					/* 更新部品状態   				*/
												/* 00000001(INV転送完了)		*/
												/* 00000010(PV転送完了)			*/
	uchar	UpdIngState;						/* 更新中状態					*/

} EOTA_UPD_STATE_INFO_T;

/* 更新用保存情報 */
typedef struct _eota_upd_save_info_t
{
	schar	Dir[EOTA_TAR_FILE_DIR_LEN + 1];		/* 更新ファイルフォルダ			*/
	schar	UpdTime[EOTA_SETUP_TIME_SIZE + 1];	/* V:FW 更新実施時刻			*/
	ushort	UpdNo;								/* V:更新 No					*/
} EOTA_UPD_SAVE_INFO_T;

/* 転送中情報 */
typedef struct _eota_send_ing_info_t
{
	uchar	UpdTgt;								/* 転送中Tgt					*/
	slong	UpdFileOffset;						/* 転送中ファイルoffset			*/
}EOTA_SEND_ING_INFO_T;

/* FW 更新情報 */
typedef struct _eota_upd_info_t
{
	EOTA_UPD_STATE_INFO_T 	UpdStateInfo;		/* 更新状態情報					*/
	EOTA_UPD_SAVE_INFO_T	UpdSaveInfo;		/* 更新用保存情報				*/
	EOTA_SEND_ING_INFO_T	SendIngInfo;		/* 転送中情報					*/

	EOTA_INV_UPD_INFO_T		InvInfo;			/* INV更新情報					*/
	EOTA_PV_UPD_INFO_T		PvInfo;				/* PV更新情報					*/
	EOTA_BDC_UPD_INFO_T		BdcInfo;			/* BDC更新情報					*/
	EOTA_JB_UPD_INFO_T		JbInfo;				/* J.B.更新情報					*/

	EOTA_TIME_INFO_T		TimeInfo;			/* タイム情報					*/
	EOTA_CMD11_REQ_INFO_T	Message;			/* 前回のメッセージ				*/
} EOTA_UPD_INFO_T;





#endif	/* __ESYS_LOCAL_H__ */
