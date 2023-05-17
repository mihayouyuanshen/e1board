/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Xu.Y												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/07												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EEPC_COM_H__
#define __EEPC_COM_H__

#include "eepc_g.h"
#include "ewdt.h"
#include "mtx.h"
#include "task_g.h"
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define EEPC_TEST_CMD_LEN					((ushort)525u)

#define EEPC_LINE_END1_LEN					((uchar)1u)
#define EEPC_LINE_END2_LEN					((uchar)2u)

#define EEPC_FILE_END_SYMR					'\r'
#define EEPC_FILE_END_SYMN					'\n'
#define EEPC_FILE_WRITE_NUMBER				((uchar)1u)

#define EEPC_SRV_MNG_BANKNO_LINE			((uchar)0u)
#define EEPC_SRV_MNG_VER_LINE				((uchar)1u)
#define EEPC_SRV_MNG_CNT_LINE				((uchar)2u)
#define EEPC_SRV_MNG_INF_LINE				((uchar)3u)
#define EEPC_SRV_MNG_BNK_NO_DATA_LEN		((uchar)1u)

#define EEPC_SRV_URL_COM_LINE_ID	((uchar)0u)
#define EEPC_SRV_URL_VER_LINE_ID	((uchar)1u)
#define EEPC_SRV_URL_CNT_LINE_ID	((uchar)2u)
#define EEPC_SRV_URL_INF_LINE_ID	((uchar)3u)

#define EEPC_RTCER_BNK_NO_0					((uchar)0u)
#define EEPC_RTCER_BNK_NO_1					((uchar)1u)

#define	EEPC_RTRY_TIM_TBL_CNT				((uchar)10u)			/* リトライ時刻情報テーブルの数					*/
#define	EEPC_MAC_ADDR_LEN					((uchar)12u)			/* MACアドレスの文字列サイズ					*/
#define	EEPC_MAJOR_VER_LEN					((uchar)3u)				/* メジャーバージョンの最大桁数					*/
#define	EEPC_MINER_VER_LEN					((uchar)2u)				/* マイナーバージョンの最大桁数					*/
#define	EEPC_COM_TIMEOUT_SEC				((uchar)60u)			/* 通信タイムアウト時間							*/
#define	EEPC_MSG_BUF_SIZE					((ushort)(2u * 1024u))	/* メッセージ送受信バッファサイズ				*/
#define	EEPC_LOCAL_TIME_LEN					((uchar)15u)			/* 現在時刻文字列の文字数						*/
#define	EEPC_CHK_NUM_U_INT					((ulong)0xFFFFFFFFu)	/* データ型チェック値(unsigned int型)			*/
#define	EEPC_SRV_FIEXD_MAX_LEN				((uchar)1u)				/* 電力会社サーバ設定済みファイル内の最大桁数	*/

#define	EEPC_EEPC_RTCER_BNK_NO_0			(0)				/* ルート証明書格納ディレクトリBank0番号	*/
#define	EEPC_EEPC_RTCER_BNK_NO_1			(1)				/* ルート証明書格納ディレクトリBank1番号	*/
#define	EEPC_RTCER_DIR_NAME					"ROOT_CER/"		/* ルート証明書格納ディレクトリ名			*/

#define	EEPC_USR_DIR_PATH					"/mnt/user/eepc/"						/* ユーザ領域のディレクトリパス							*/
#define	EEPC_RAM_DIR_PATH					"/mnt/ram/eepc/"						/* 不揮発領域のディレクトリパス							*/
#define	EEPC_USB_DIR_PATH					"/media/usb_storage/PV_SEIGYO/"			/* 電力会社URLファイルの格納先(USBストレージパス)		*/
#define	EEPC_DEF_DIR_PATH					"/usr/bin/"								/* デフォルト領域(rootfs)のディレクトリパス				*/
#define	EEPC_RTCER_BNK_DIR_PATH_PREF		"/mnt/user/eepc/rtcer_"					/* ルート証明書格納ディレクトリパスPrefix				*/
#define	EEPC_DEF_RTCER_DIR_PATH				EEPC_DEF_DIR_PATH EEPC_RTCER_DIR_NAME	/* デフォルトルート証明書ファイル格納ディレクトリパス	*/

#define	EEPC_RTCER_BNK0_DIR_PATH			EEPC_RTCER_BNK_DIR_PATH_PREF"0/"		/* ルート証明書格納ディレクトリ(Bank0)パス	*/
#define	EEPC_RTCER_BNK1_DIR_PATH			EEPC_RTCER_BNK_DIR_PATH_PREF"1/"		/* ルート証明書格納ディレクトリ(Bank1)パス	*/

#define	EEPC_USE_SRV_FILE_NAME				"eepc_UseSrv.csv"	/* 電力会社サーバ選択ファイル名 */
#define	EEPC_COM_HIST_FILE_NAME				"eepc_ComHist.csv"	/* 電力会社サーバ通信履歴ファイル名 */
#define	EEPC_SRV_FIEXD_FILE_NAME			"eepc_SrvFixed.csv"	/* 電力会社サーバ設定済みファイル名 */
#define	EEPC_STR_DATA_END					'\0'				/* 文字列データの終端(NULL文字) */

#define	EEPC_CHG_STR_TO_DEC					((uchar)10u)	/* 文字列変換対象の値(10進数)	*/
#define	EEPC_CHG_STR_TO_HEX					((uchar)16u)	/* 文字列変換対象の値(16進数)	*/

#define	EEPC_TMP_FILE_PATH_SIZE				((uchar)128u)			/* プロセス内のファイルパスサイズ	*/
#define	EEPC_TMP_BUF_SIZE					((ushort)512u)			/* プロセス内の一時バッファサイズ	*/

#define	EEPC_SRV_URL_FILE_LINE_MAX			((uchar)13u)			/* 電力会社URLファイルの最大行数	*/
#define	EEPC_SRV_URL_FILE_NAME				"SrvData.csv"			/* 電力会社URLファイル名			*/

#define	EEPC_CORP_DIV_LEN					((uchar)2u)					/* 送配電事業者コードの最大桁数	*/
#define	EEPC_CORP_DIV_MAX					((uchar)11u)				/* 送配電事業者コードの最大数	*/

#define	EEPC_SRV_MNG_FILE_SIZE				((ushort)(4u * 1024u))		/* 電力会社サーバ管理ファイルサイズ										*/
#define	EEPC_SRV_MNG_FILE_LINE_MAX			((uchar)13u)				/* 電力会社サーバ管理ファイルの最大行数									*/
#define	EEPC_SRV_MNG_VER_LEN				((uchar)7u)					/* (NULL終端込み)電力会社サーバ管理ファイルのバージョン情報サイズ		*/
#define	EEPC_SRV_MNG_CNT_LEN				((uchar)2u)					/* 対象サーバ数の最大桁数												*/
#define	EEPC_SRV_MNG_DATA_LEN_MAX			((uchar)129u)				/* 電力会社情報1つの最大桁数											*/
#define	EEPC_SRV_MNG_PORT_LEN				((uchar)5u)					/* サーバのポート番号最大桁数											*/
#define	EEPC_SRV_MNG_PORT_MAX				((ushort)65535u)			/* サーバのポート番号最大値												*/
#define	EEPC_SRV_MNG_RTCER_BNK_NO_LEN		((uchar)1u)					/* ルート証明書ファイル格納ディレクトリBankNo最大桁数					*/
#define	EEPC_SRV_MNG_FILE_NAME				"eepc_SrvMng.csv"			/* 電力会社サーバ管理ファイル名											*/
#define	EEPC_SRV_MNG_FILE_NAME_LEN			((uchar)15u)				/* 電力会社サーバ管理ファイル名サイズ									*/

#define	EEPC_FILE_LINE_MIN_USE_SRV			((uchar)1u)		/* ファイルの最低行数(電力会社サーバ選択ファイル)									*/
#define	EEPC_FILE_LINE_MIN_COM_HIST			((uchar)1u)		/* ファイルの最低行数(電力会社サーバ通信履歴ファイル)								*/
#define	EEPC_FILE_LINE_MIN_SRV_FIXED		((uchar)1u)		/* ファイルの最低行数(電力会社サーバ設定済みファイル)								*/
#define	EEPC_FILE_LINE_MIN_NTC_SET			((uchar)1u)		/* ファイルの最低行数(設定変更通知イベント情報ファイル)								*/
#define	EEPC_FILE_LINE_MIN_SRV_URL			((uchar)4u)		/* ファイルの最低行数(電力会社URLファイル)											*/

#define	EEPC_PEM_FILE_PATH					"/usr/bin/"			/* ルート証明書ファイルパス */
#define	EEPC_PEM_FILE_NAME					"reene.pem"			/* ルート証明書ファイル名 */

#define	EEPC_TRNC_FILE_MAX					((ushort)(20u * 1024u))		/* (単位:KB)伝送ファイルの最大サイズ								*/
#define	EEPC_TRNC_FILE_NAME_HEAD_LEN		((uchar)4u)					/* 伝送ファイル名先頭のフォーマットID文字数							*/
#define	EEPC_TRNC_FILE_ERR_CODE_LEN			((uchar)5u)					/* 伝送ファイル内のエラーコード文字数								*/
#define	EEPC_TRNC_FILE_PATH_EXT				".data"						/* 伝送ファイルの拡張子(拡張子のみ。ファイル名はeschのシーケンスID)	*/
#define	EEPC_TRNC_FILE_NAME_HEAD_STR		"ERR"						/* 伝送ファイル名先頭のフォーマット(エラーファイル)					*/
#define	EEPC_TRNC_FILE_NAME_HEAD_STR_LEN	((uchar)3u)					/* 伝送ファイル名先頭のフォーマット文字数(エラーファイル)			*/
#define	EEPC_TRNC_FILE_NONE_ERR				"00000"						/* 伝送ファイル名先頭のフォーマット(正常ファイル)					*/
#define	EEPC_TRNC_FILE_NONE_ERR_LEN			((uchar)5u)					/* 伝送ファイル名先頭のフォーマット(正常ファイル)					*/

#define	EEPC_NTC_EVT_PORT_LEN				((uchar)5u)						/* (設定変更通知イベント情報)ポート番号の最大桁数	*/
#define	EEPC_NTC_EVT_FLG_LEN				((uchar)2u)						/* (設定変更通知イベント情報)フラグの最大桁数		*/
#define	EEPC_NTC_SET_FILE_NAME				"eepc_notification_setting.inf"	/* 設定変更通知イベント情報ファイル名				*/

#define	EEPC_INIT_SRV_PORT					((ushort)443u)			/* (九州電力)電力会社サーバポート番号の初期値			*/
#define	EEPC_INIT_NTP_PORT					((uchar)123u)			/* (九州電力)電力会社指定のNTPサーバポート番号の初期値	*/
#define	EEPC_INIT_CORP_DIV					((uchar)9u)				/* (九州電力)電力会社の区分の初期値						*/
#define	EEPC_INIT_HTTPS_HEAD				"https://"				/* URL形成の為のヘッダ									*/
#define	EEPC_INIT_HTTPS_HEAD_LEN			((uchar)8u)				/* URL形成の為のヘッダ文字数							*/
#define	EEPC_INIT_SRV_HOST					"re-ene.kyuden.co.jp"	/* (九州電力)電力会社サーバHostの初期値					*/
#define	EEPC_INIT_SRV_REQ					"/scheduleSend/"		/* (九州電力)電力会社サーバリクエスト先の初期値			*/
#define	EEPC_INIT_NTP_FQDN					"re-ene.kyuden.co.jp"	/* (九州電力)電力会社指定のNTPサーバFQDNの初期値		*/
#define	EEPC_INIT_CORP_NAME					"九州電力"				/* (九州電力)電力会社名の初期値							*/

#define	EEPC_RTRY_CNT_FIX_SCH				((uchar)5u)			/* 固定スケジュール受信のリトライ回数		*/
#define	EEPC_RTRY_INTVL_FIX_SCH				((ushort)300u)		/* 固定スケジュール受信のリトライ間隔(秒)	*/
#define	EEPC_RTRY_CNT_UPDT_SCH				((uchar)5u)			/* 更新スケジュール受信のリトライ回数		*/
#define	EEPC_RTRY_INTVL_UPDT_SCH			((ushort)1800u)		/* 更新スケジュール受信のリトライ間隔(秒)	*/
#define	EEPC_RTRY_CNT_TIM_SYNC				((uchar)5u)			/* 時刻同期のリトライ回数					*/
#define	EEPC_RTRY_INTVL_TIM_SYNC			((uchar)60u)		/* 時刻同期のリトライ間隔(秒)				*/
#define	EEPC_RTRY_OVR_INTVL_TIM_SYNC		((ushort)1800u)		/* 時刻同期の再リトライ間隔(秒)				*/
#define	EEPC_RTRY_ID_REG_MAX				((uchar)5u)			/* ID登録確認のリトライ実施最大数			*/
#define	EEPC_RTRY_ID_REG_INTVL				((uchar)30u)		/* ID登録確認のリトライ間隔(秒)				*/

#define	EEPC_SPRIT_FILE_CSV					","				/* EEPC管理ファイル内の区切り文字(カンマ)	*/
#define	EEPC_SPRIT_URL_DOMAIN				"/"				/* URLの区切り文字(ドメイン部)				*/
#define	EEPC_SPRIT_DIV_VER					"."				/* バージョン情報の区切り文字(ドット)		*/
#define	EEPC_SPRIT_VER_POSITION				((uchar)3u)		/* バージョン情報の区切り位置				*/
#define	EEPC_SPRIT_COM_HIT_POSITION			((uchar)5u)		/* 通信履歴の区切り位置						*/

#define	EEPC_HTTP_MSG_TMP_SIZE				((uchar)128u)			/* HTTPメッセージ用一時領域サイズ									*/
#define	EEPC_HTTP_HEADER_MAX				((uchar)60u)			/* HTTPメッセージ内のヘッダ最大数									*/
#define	EEPC_HTTP_HEADER_C_LENGTH_SIZE		((uchar)84u)			/* HTTPヘッダ値(Content-Lengthサイズ)								*/
#define	EEPC_HTTP_HEADER_POST				"POST"					/* HTTPヘッダ(POST)													*/
#define	EEPC_HTTP_HEADER_VER				"HTTP/1.1"				/* HTTPヘッダ値(HTTPバージョン)										*/
#define	EEPC_HTTP_HEADER_VER_USE_PROXY		"HTTP/"					/* HTTPヘッダ値(HTTPバージョン)										*/
#define	EEPC_HTTP_HEADER_VER_LEN			((uchar)9u)				/* HTTPヘッダ文字列数(終端文字含む_HTTPバージョン)					*/
#define	EEPC_HTTP_HEADER_RES_LEN			((uchar)4u)				/* HTTPヘッダ文字列数(終端文字含む_レスポンスステータス)			*/
#define	EEPC_HTTP_HEADER_RES_MSG_LEN		((uchar)2u)				/* HTTPヘッダ文字列数(終端文字含む_レスポンスステータスメッセージ)	*/
#define	EEPC_HTTP_HEADER_HOST				"Host:"					/* HTTPヘッダ(Host)													*/
#define	EEPC_HTTP_HEADER_CONNECTION			"Connection: close"		/* HTTPヘッダ(Connection)											*/
#define	EEPC_HTTP_HEADER_C_TYPE				"Content-Type:"			/* HTTPヘッダ(Content-Type)											*/
#define	EEPC_HTTP_HEADER_C_TYPE_LEN			((uchar)14u)			/* HTTPヘッダ文字列数(終端文字含む_Content-Type)					*/
#define	EEPC_HTTP_HEADER_MULTIPART			"multipart/mixed;"		/* HTTPヘッダ値(multipart/mixed)									*/
#define	EEPC_HTTP_HEADER_MULTIPART_LEN		((uchar)16u)			/* HTTPヘッダ文字列数(終端文字含まず_multipart/mixed)				*/
#define	EEPC_HTTP_HEADER_BOUNDARY			"boundary="				/* HTTPヘッダ値(boundary)											*/
#define	EEPC_HTTP_HEADER_BOUNDARY_LEN		((uchar)9u)				/* HTTPヘッダ文字列数(終端文字含まず_boundary)						*/
#define	EEPC_HTTP_HEADER_C_LENGTH			"Content-Length:"		/* HTTPヘッダ(Content-Length)										*/
#define	EEPC_HTTP_HEADER_C_LENGTH_LEN		((uchar)16u)			/* HTTPヘッダ文字列数(終端文字含む_Content-Length)					*/
#define	EEPC_HTTP_HEADER_C_LENGTH_DATA_LEN	((uchar)11u)			/* HTTPヘッダ値文字列数(終端文字含む_Content-Lengthの値)			*/
#define	EEPC_HTTP_HEADER_APP				"application/"			/* HTTPヘッダ値(application/)										*/
#define	EEPC_HTTP_HEADER_STREAM				"octet-stream"			/* HTTPヘッダ値(octet-stream)										*/
#define	EEPC_HTTP_HEADER_PRM_C_TYPE			"content-type:"			/* HTTPヘッダ(content-type)											*/
#define	EEPC_HTTP_HEADER_ENCODED			"x-www-form-urlencoded"	/* HTTPヘッダ値(x-www-form-urlencoded)								*/
#define	EEPC_HTTP_HEADER_C_DISP				"Content-Disposition:"	/* HTTPヘッダ(Content-Disposition)									*/
#define	EEPC_HTTP_HEADER_C_DISP_LEN			((uchar)21u)			/* HTTPヘッダ文字列数(終端文字含む_Content-Disposition)				*/
#define	EEPC_HTTP_HEADER_ATTACHMENT			"attachment;"			/* HTTPヘッダ値(attachment)											*/
#define	EEPC_HTTP_HEADER_ATTACHMENT_LEN		((uchar)12u)			/* HTTPヘッダ文字列数(終端文字含む_attachment)						*/
#define	EEPC_HTTP_HEADER_FILE_NAME			"filename="				/* HTTPヘッダ値(filename)											*/
#define	EEPC_HTTP_HEADER_FILE_NAME_LEN		((uchar)9u)				/* HTTPヘッダ文字列数(終端文字含まず_filename)						*/
#define	EEPC_HTTP_HEADER_PC_ID				"power_plant_id="		/* HTTPヘッダ(power_plant_id)										*/
#define	EEPC_HTTP_HEADER_MAC_ADDR			"mac_address="			/* HTTPヘッダ(mac_address)											*/
#define	EEPC_HTTP_HEADER_SCH_DIV			"schedule_kbn="			/* HTTPヘッダ(schedule_kbn)											*/
#define	EEPC_HTTP_MSG_END_LINE				"\r\n"					/* HTTPメッセージの行端(2byte改行)									*/
#define	EEPC_HTTP_MSG_END_LINE_LEN			((uchar)2u)				/* HTTPメッセージの行端の文字数(2byte改行)							*/
#define	EEPC_HTTP_BOUNDARY					"\""					/* boundaryヘッダの区切り文字(ダブルクォーテーション)				*/

/* JET認証用 START */
#define	EEPC_TEST_SRV_FILEFULLPATH_LEN		((uchar)255u)					/* 電力会社テストサーバ用URLファイルフルパスの文字列長"/media/usb_storage/PV_SEIGYO/test_srv/eepc_SrvMng.csv"(NULL文字考慮) */
#define	EEPC_TEST_SRV_PATH					EEPC_USB_DIR_PATH"test_srv/"	/* 電力会社テストサーバ用URLファイル格納ディレクトリ（USBメモリ）へのファイルパス */
#define	EEPC_USB_MEDIA_PATH					"/media/usb_storage"			/* USBメモリルートディレクトリ */
#define	EEPC_TEST_PATH_DEVSDA				"/dev/sda1"
#define	EEPC_MNT_USER_PATH					"/mnt/user"
#define	EEPC_USR_NAND_DIR_PATH				"/mnt/user/eepc_nand/"					/* ユーザ領域のディレクトリパス */
#define	EEPC_RTCER_BNK_NAND_DIR_PATH_PREF	"/mnt/user/eepc_nand/rtcer_"			/* ルート証明書格納ディレクトリパスPrefix */
#define	EEPC_RTCER_BNK0_NAND_DIR_PATH		EEPC_RTCER_BNK_NAND_DIR_PATH_PREF"0/"	/* ルート証明書格納ディレクトリ(Bank0)パス */
#define	EEPC_RTCER_BNK1_NAND_DIR_PATH		EEPC_RTCER_BNK_NAND_DIR_PATH_PREF"1/"	/* ルート証明書格納ディレクトリ(Bank1)パス */
#define	EEPC_LINK_PATH						"/mnt/user/eepc"						/* シンボリックリンクパス */
#define	EEPC_RAM_PEM_FILE_PATH				EEPC_RAM_DIR_PATH EEPC_RTCER_DIR_NAME	/* RAM上ルート証明書格納パス */
#define	EEPC_LINK_PEM_FILE_PATH				EEPC_RAM_PEM_FILE_PATH"KYUDEN/"			/* ルート証明書格納シンボリックリンクパス */
#define	EEPC_LINK_RAM_PEM_FILE				EEPC_RAM_PEM_FILE_PATH EEPC_PEM_FILE_NAME	/* ルート証明書ファイルシンボリックリンク */
/* JET認証用 END */

/* SSLオプション設定値	*/
#define	EEPC_SSL_SET_OPT	(SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1)

/* 設定変更通知イベント情報1行のデータ桁数				*/
/* データイメージ"NNNNN.XX\n"							*/
/* ポート番号サイズ+カンマ1文字+フラグサイズ+1byte改行	*/
#define EEPC_NOTICE_EVT_DATA_LINE_LEN	(EEPC_NTC_EVT_PORT_LEN + (uchar)1u + EEPC_NTC_EVT_FLG_LEN + (uchar)1u)

/* Content-Type行のデータ桁数																						*/
/* multipart/mixedの文字数+boundaryの文字数+区切り文字本体の文字数(128byte想定)+ダブルクォーテーションの数(2byte分)	*/
#define EEPC_HTTP_HEADER_C_TYPE_DATA_LEN	(EEPC_HTTP_HEADER_MULTIPART_LEN + EEPC_HTTP_HEADER_BOUNDARY_LEN + EEPC_HTTP_MSG_TMP_SIZE + (uchar)2u)

/* Content-Disposition行のデータ桁数									*/
/* filenameの文字数+ファイル名本体の文字数(128byte想定)+NULL終端(1byte)	*/
#define EEPC_HTTP_HEADER_C_DISP_DATA_LEN	(EEPC_HTTP_HEADER_FILE_NAME_LEN + EEPC_HTTP_MSG_TMP_SIZE + (uchar)1u)

/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
/******************************************************************************
* @enum		EepcFuncResult_t
* @brief	内部処理結果(プロセス内の処理結果を識別する値)
******************************************************************************/
typedef enum
{
	EEPC_FUNC_RESULT_OK = 0,	/* 処理成功	*/
	EEPC_FUNC_RESULT_NG,		/* 処理失敗	*/
	EEPC_FUNC_RESULT_EPRM,		/* 引数異常	*/
} EepcFuncResult_t;

/******************************************************************************
* @enum		EepcLclEvtId_t
* @brief	内部イベントID(プロセス内のイベント通知を識別する値)
******************************************************************************/
typedef enum
{
	EEPC_LCL_EVT_ID_UPDATE_SRV_INF = 0,		/* 電力会社情報更新スレッドの終了	*/
	EEPC_LCL_EVT_ID_RCV_SCH,				/* スケジュール受信スレッドの終了	*/
	EEPC_LCL_EVT_ID_ID_REG,					/* ID登録確認スレッドの終了			*/
	EEPC_LCL_EVT_ID_MAX						/* 終端要素							*/
} EepcLclEvtId_t;

/******************************************************************************
* @enum		EepcThrType_t
* @brief	スレッド種別(プロセス内のスレッド種別を識別する値)
******************************************************************************/
typedef enum
{
	EEPC_THR_TYPE_UPDATE_SRV_INF = 0,		/* 電力会社情報更新スレッド	*/
	EEPC_THR_TYPE_RCV_SCH,					/* スケジュール受信スレッド	*/
	EEPC_THR_TYPE_ID_REG,					/* ID登録確認スレッド		*/
	EEPC_THR_TYPE_MAX						/* 終端要素					*/
} EepcThrType_t;

/******************************************************************************
* @enum		EepcSrvMngDataLineType_t
* @brief	電力会社サーバ管理ファイル内のデータ行種別
******************************************************************************/
typedef enum
{
	EEPC_SRV_MNG_DATA_LINE_TYPE_BNK_NO = 0,	/* 1行目:ルート証明書ファイル格納ディレクトリBankNo	*/
	EEPC_SRV_MNG_DATA_LINE_TYPE_VER,		/* 2行目:バージョン情報			*/
	EEPC_SRV_MNG_DATA_LINE_TYPE_TOTAL,		/* 3行目:電力会社の総数			*/
	EEPC_SRV_MNG_DATA_LINE_TYPE_INF_1,		/* 4行目:1件目の電力会社情報	*/
	EEPC_SRV_MNG_DATA_LINE_TYPE_INF_2,		/* 5行目:2件目の電力会社情報	*/
	EEPC_SRV_MNG_DATA_LINE_TYPE_INF_3,		/* 6行目:3件目の電力会社情報	*/
	EEPC_SRV_MNG_DATA_LINE_TYPE_INF_4,		/* 7行目:4件目の電力会社情報	*/
	EEPC_SRV_MNG_DATA_LINE_TYPE_INF_5,		/* 8行目:5件目の電力会社情報	*/
	EEPC_SRV_MNG_DATA_LINE_TYPE_INF_6,		/* 9行目:6件目の電力会社情報	*/
	EEPC_SRV_MNG_DATA_LINE_TYPE_INF_7,		/* 10行目:7件目の電力会社情報	*/
	EEPC_SRV_MNG_DATA_LINE_TYPE_INF_8,		/* 11行目:8件目の電力会社情報	*/
	EEPC_SRV_MNG_DATA_LINE_TYPE_INF_9,		/* 12行目:9件目の電力会社情報	*/
	EEPC_SRV_MNG_DATA_LINE_TYPE_INF_10,		/* 13行目:10件目の電力会社情報	*/
	EEPC_SRV_MNG_DATA_LINE_TYPE_MAX			/* 終端要素						*/
} EepcSrvMngDataLineType_t;

/******************************************************************************
* @enum		EepcCorpDataType_t
* @brief	電力会社情報種別(電力会社情報を識別する値)
******************************************************************************/
typedef enum
{
	EEPC_CORP_DATA_TYPE_DIV	= 0,	/* 送配電事業者コード					*/
	EEPC_CORP_DATA_TYPE_NAME,		/* 電力会社名							*/
	EEPC_CORP_DATA_TYPE_CORP_URL,	/* 電力会社のURL						*/
	EEPC_CORP_DATA_TYPE_CORP_PORT,	/* 電力会社のポート番号					*/
	EEPC_CORP_DATA_TYPE_NTP_FQDN,	/* 電力会社指定のNTPサーバFQDN			*/
	EEPC_CORP_DATA_TYPE_NTP_PORT,	/* 電力会社指定のNTPサーバポート番号	*/
	EEPC_CORP_DATA_TYPE_RTCER_NAME,	/* ルート証明書ファイル名				*/
	EEPC_CORP_DATA_TYPE_MAX			/* 終端要素								*/
} EepcCorpDataType_t;

/******************************************************************************
* 構造体
******************************************************************************/
/******************************************************************************
* @struct	電力会社情報用構造体
* @breaf	電力会社情報用構造体
******************************************************************************/
typedef struct
{
	uchar	corpDiv;								/* 電力会社の区分						*/
	ushort	srvPort;								/* 電力会社サーバのポート番号			*/
	ushort	ntpSrvPort;								/* 電力会社指定のNTPサーバのポート番号	*/
	schar	srvNameStr[EEPC_SRV_NAME_LEN];			/* 電力会社名							*/
	schar	srvUrlStr[EEPC_URL_LEN];				/* 電力会社サーバのURL					*/
	schar	ntpFqdnStr[EEPC_FQDN_LEN];				/* 電力会社指定のNTPサーバのFQDN		*/
	schar	rtCerFileStr[EEPC_RTCER_NAME_LEN];		/* ルート証明書ファイル名				*/
} EepcFileSrvInf_t;

/******************************************************************************
* @struct	電力会社サーバ管理ファイル用構造体
* @breaf	電力会社サーバ管理ファイル用構造体
******************************************************************************/
typedef struct
{
	uchar				srvCnt;							/* 電力会社サーバの総数								*/
	ulong				srvRtryCntFixSch;				/* 固定スケジュール受信のリトライ回数				*/
	ulong				srvRtryIntvlFixSch;				/* 固定スケジュール受信のリトライ間隔				*/
	ulong				srvRtryCntUpdtSch;				/* 更新スケジュール受信のリトライ回数				*/
	ulong				srvRtryIntvlUpdtSch;			/* 更新スケジュール受信のリトライ間隔				*/
	ulong				ntpRtryCntTimSync;				/* 時刻同期のリトライ回数							*/
	ulong				ntpRtryIntvlTimSync;			/* 時刻同期のリトライ間隔							*/
	ulong				ntpRtryOvrIntvlTimSync;			/* 時刻同期の再リトライ間隔							*/
	ushort				srvInfVerMajor;					/* 電力会社サーバ管理ファイルのメジャーバージョン	*/
	uchar				srvInfVerMiner;					/* 電力会社サーバ管理ファイルのマイナーバージョン	*/
	EepcFileSrvInf_t	srvInfList[EEPC_SRV_LIST_MAX];	/* 電力会社情報のリスト								*/
	uchar				rtCerBnkNo;						/* ルート証明書ファイル格納ディレクトリBankNo		*/
} EepcFileSrvMng_t;

/******************************************************************************
* @struct	電力会社サーバ選択ファイル用構造体
* @breaf	電力会社サーバ選択ファイル用構造体
******************************************************************************/
typedef struct
{
	EepcFileSrvInf_t	srvInf;	/* 電力会社情報	*/
} EepcFileUseSrv_t;

/******************************************************************************
* @struct	電力会社サーバ通信履歴ファイル用構造体
* @breaf	電力会社サーバ通信履歴ファイル用構造体
******************************************************************************/
typedef struct 
{
	schar resultTime[EEPC_RESULT_TIME_LEN];	/* 電力会社との通信結果及び通信時間	*/
} EepcFileComHist_t;

/******************************************************************************
* @struct	電力会社サーバ設定済みファイル用構造体
* @breaf	電力会社サーバ設定済みファイル用構造体
******************************************************************************/
typedef struct
{
	uchar	srvFixed;	/* 電力会社サーバの設定済みフラグ(M1/M2の修正計画No.139対応により、常にTRUEに変更)	*/
} EepcFileSrvFixed_t;

/******************************************************************************
* @struct	設定変更通知イベント情報ファイル用構造体
* @breaf	設定変更通知イベント情報ファイル用構造体
******************************************************************************/
typedef struct 
{
	sint	receiver;	/* イベント通知先のポート番号					*/
	ulong	chgDataFlg;	/* EEPC_EVT01_T型のメンバの変更に対応するフラグ	*/
}EepcFileEvt01_t;

/******************************************************************************
* @struct	サーバ通信情報用構造体
* @breaf	サーバ通信情報用構造体
******************************************************************************/
typedef struct 
{
	EepcComStart_t		comStart;	/* 通信開始要求用構造体							*/
	EepcFileUseSrv_t	usrSrv;		/* 電力会社サーバ選択ファイル用構造体			*/
	uchar				rtCerBnkNo;	/* ルート証明書ファイル格納ディレクトリBankNo	*/
}EepcSrvComInf_t;
/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/
EwdtTermType_t		g_eepcReqProcEndType;					/* 終了要求種別								*/
bool_t				g_eepcFileUpdateCanselFlg;				/* ファイル更新中断フラグ					*/
uchar				g_eepcUseSrvIndex;						/* 使用中電力会社のIndex値					*/
uchar				g_eepcChgEvtRegCnt;						/* 設定変更通知イベントの登録数				*/
EepcUpdtSrv_t		g_eepcUpdtSrv;							/* 電力会社情報更新用構造体					*/
EepcComInf_t		g_eepcComInf;							/* 通信先情報用構造体						*/
EepcFileSrvMng_t	g_eepcFileSrvMng;						/* 電力会社サーバ管理ファイル用構造体		*/
EepcFileUseSrv_t	g_eepcFileUseSrv;						/* 電力会社サーバ選択ファイル用構造体		*/
EepcFileComHist_t	g_eepcFileComHist;						/* 電力会社サーバ通信履歴ファイル用構造体	*/
EepcFileSrvFixed_t	g_eepcFileSrvFixed;						/* 電力会社サーバ設定済みファイル用構造体	*/
EepcFileEvt01_t		g_eepcFileEvt01[PROCESS_MAX];			/* 設定変更通知イベント情報ファイル用の配列	*/
EepcSrvComInf_t		g_eepcSrvComInfSch;						/* サーバ通信情報用構造体(スケジュール受信)	*/
EepcSrvComInf_t		g_eepcSrvComInfRegId;					/* サーバ通信情報用構造体(ID登録確認)		*/
bool_t				g_eepcThrExecFlg[EEPC_THR_TYPE_MAX];	/* サブスレッドの動作中フラグ				*/
FrmPthread_t		g_eepcThrId[EEPC_THR_TYPE_MAX];			/* サブスレッドID保持用の配列				*/
uchar				g_eepcThrWdtCnt[EEPC_THR_TYPE_MAX];		/* サブスレッドの死活監視カウンタ			*/

/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern EepcFuncResult_t EEPC_ComInitProc(void);

extern EepcFuncResult_t EEPC_ComDivideAsyncMsg(FrmMsgHead_t const *msg);
extern EepcFuncResult_t EEPC_ComDivideEventMsg(FrmMsgHead_t const *msg);
extern EepcFuncResult_t EEPC_ComDivideSyncMsg(FrmMsgHead_t const *msg);



#endif /*__EEPC_COM_H__*/
