/****************************************************************************/
/*	Description		:INV通信ミドル											*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Wang.M												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/23												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:INV通信												*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EINV_G_H__
#define __EINV_G_H__

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <typedef.h>
#include "frm_g.h"
#include <common_g.h>


/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
/* 応答コード */
#define	EINV_OK								((slong)0)			/* 正常終了 */
#define	EINV_ERR_SYSCALL					((slong)-1)			/* システムコールエラー */
#define	EINV_ERR_NOT_COMM					((slong)-2)			/* 通信未確認エラー */
#define	EINV_ERR_NOT_INIT					((slong)-3)			/* 未初期化エラー */
#define	EINV_ERR_PARAM						((slong)-4)			/* パラメータエラー */
#define	EINV_ERR_FAIL						((slong)-5)			/* 処理失敗エラー */
#define	EINV_ERR_NOT_SUPPORT				((slong)-6)			/* 非サポートエラー */
#define	EINV_ERR_GENERATION					((slong)-7)			/* 世代エラー */
#define	EINV_ERR_FWUP_STATE					((slong)-8)			/* INVFW更新開始条件不成立 */
#define	EINV_ERR_SYS_CONF					((slong)-9)			/* 非対応システム構成エラー */
#define	EINV_ERR_DISCONNECT					((slong)-10)		/* 通信無応答が4分30秒経過前 */
#define	EINV_ERR_DISCONNECT_LONGTIME		((slong)-11)		/* 通信無応答が4分30秒経過 */

/* 配列要素数定義 */
#define	EINV_DATEYEAR_LEN					((uchar)3)			/* 年（“00”～“99”）*/
#define	EINV_LONGDATEYEAR_LEN				((uchar)5)			/* 年（“2000”～“2099”）*/
#define	EINV_DATEMONTH_LEN					((uchar)3)			/* 月（“01”～“12”）*/
#define	EINV_DATEDAY_LEN					((uchar)3)			/* 日（“01”～“31”）*/
#define	EINV_TIMEHOUR_LEN					((uchar)3)			/* 時（“00”～“23”）*/
#define	EINV_TIMEMINUTE_LEN					((uchar)3)			/* 分（“00”～“59”）*/
#define	EINV_MODELFUNCTIONNAME_LEN			((uchar)3)			/* モデル機能名称 */
#define	EINV_PCSERRRATING_LEN				((uchar)3)			/* PCSエラーレーティング */
#define	EINV_SYSMADENUM_LEN					((uchar)17)			/* システム製造番号 */
#define	EINV_INVMCUFW_LEN					((uchar)9)			/* INVマイコン_FW */
#define	EINV_PVMCUFW_LEN					((uchar)9)			/* PVマイコン_FW */
#define	EINV_BDCMCUFW_LEN					((uchar)9)			/* BDCマイコン_FW */
#define	EINV_JBFW_LEN						((uchar)4)			/* J.B._FW */
#define	EINV_INVMCUBOOTLOADER_LEN			((uchar)6)			/* INVマイコン_ブートローダー */
#define	EINV_PVMCUBOOTLOADER_LEN			((uchar)6)			/* PVマイコン_ブートローダー */
#define	EINV_BDCMCUBOOTLOADER_LEN			((uchar)6)			/* BDCマイコン_ブートローダー */
#define	EINV_BTMADENUM_LEN					((uchar)17)			/* 蓄電池ユニット製造番号 */
#define	EINV_PWRHOUSEID_LEN					((uchar)27)			/* 発電所ID */
#define	EINV_SUNPWRRATED_LEN				((uchar)6)			/* 外付け太陽光発電の定格 */
#define	EINV_POSTALNUM_LEN					((uchar)8)			/* 郵便番号 */
#define	EINV_IPADDRESS_LEN					((uchar)16)			/* IPアドレス */
#define	EINV_PROXYADDRESS_LEN				((uchar)13)			/* PROXYアドレス */
#define	EINV_PORTNUMBER_LEN					((uchar)6)			/* ポート番号 */
#define	EINV_COMMUBOARDERROR_LEN			((uchar)6)			/* 通信ボードエラー */
#define	EINV_PCSERR_LEN						((uchar)6)			/* PCSエラー */
#define	EINV_CARCHARANDDISCMODELNAME_LEN	((uchar)17)			/* 車両充放電器機種名 */
#define	EINV_CARCHARANDDISCMADENUM_LEN		((uchar)31)			/* 車両充放電器製造番号 */
#define	EINV_CARID_LEN						((uchar)26)			/* 車両ID */
#define	EINV_EVPCSFW_LEN					((uchar)9)			/* EVPCS_FW */
#define	EINV_MCUFW_LEN						((uchar)9)			/* MCU_FW(蓄電池) [0.01kW] */
#define	EINV_C2COMMUBORADFW_LEN				((uchar)9)			/* C2通信ボード_FW */
#define	EINV_ACTIONLOG_LEN					((ushort)3001)		/* 動作ログ */
#define	EINV_SYS_MODEL_NAME_LEN				((uchar)17)			/* システム機種名の最大文字列長 */

/* CMD60 */
/* システム構成 */
#define EINV_SYS_STRUCT_0					((uchar)'0')		/* "0":太陽光+蓄電池+V2H */
#define EINV_SYS_STRUCT_1					((uchar)'1')		/* "1":太陽光+蓄電池 */
#define EINV_SYS_STRUCT_2					((uchar)'2')		/* "2":太陽光+V2H */
#define EINV_SYS_STRUCT_3					((uchar)'3')		/* "3":蓄電池+V2H */
#define EINV_SYS_STRUCT_4					((uchar)'4')		/* "4":蓄電池 */
#define EINV_SYS_STRUCT_5					((uchar)'5')		/* "5":V2H */
#define EINV_SYS_STRUCT_6					((uchar)'6')		/* "6":太陽光 */
#define EINV_SYS_STRUCT_7					((uchar)'-')		/* "-":未設定 */

/* CMD63 */
/* PCSエラー消去命令 */
#define EINV_PCS_ERROR_REMOVE_COMMAND_1		((uchar)'1')		/* "1":消去実行 */


/* CMD64 / CMD65 */
/* 運転モード */		 										
#define EINV_OPEMODE_0						((uchar)'0')		/* "0":売電モード放電時間帯 */
#define EINV_OPEMODE_1						((uchar)'1')		/* "1":グリーンモード放電時間帯 */
#define EINV_OPEMODE_2						((uchar)'2')		/* "2":グリーンモード余剰充電時間帯 */
#define EINV_OPEMODE_3						((uchar)'3')		/* "3":自動運転モード充電時間帯 */
#define EINV_OPEMODE_4						((uchar)'4')		/* "4":自動運転モード不定時間帯 */
#define EINV_OPEMODE_5						((uchar)'5')		/* "5":EVモード */
#define EINV_OPEMODE_6						((uchar)'6')		/* "6":待機モード */
#define EINV_OPEMODE_7						((uchar)'7')		/* "7":蓄電池充電モード */
#define EINV_OPEMODE_8						((uchar)'8')		/* "8":蓄電池放電モード */
#define EINV_OPEMODE_9						((uchar)'9')		/* "9":蓄電池逆潮流(指定電力)放電モード */
#define EINV_OPEMODE_A						((uchar)'A')		/* "A":蓄電池余剰充電のみモード */
#define EINV_OPEMODE_B						((uchar)'B')		/* "B":蓄電池余剰充電・不足放電モード */
#define EINV_OPEMODE_C						((uchar)'C')		/* "C":車両充電モード */
#define EINV_OPEMODE_D						((uchar)'D')		/* "D":車両放電モード */
#define EINV_OPEMODE_E						((uchar)'E')		/* "E":拡張充電モード */
#define EINV_OPEMODE_F						((uchar)'F')		/* "F":蓄電池・車両同時放電モード */
#define EINV_OPEMODE_G						((uchar)'G')		/* "G":蓄電池・車両同時充電モード */
#define EINV_OPEMODE_H						((uchar)'H')		/* "H":エレムーブ充電モード */
#define EINV_OPEMODE_I						((uchar)'I')		/* "I":エレムーブ放電モード */
#define EINV_OPEMODE_J						((uchar)'J')		/* "J":メンテナンスモード */

/* 非常時運転モード */
#define	EINV_EMGMODE_0						((uchar)'0')		/* "0":自動給電切換 */
#define	EINV_EMGMODE_1						((uchar)'1')		/* "1":停電時蓄電池充電 */
#define	EINV_EMGMODE_2						((uchar)'2')		/* "2":停電時車両充電 */
#define	EINV_EMGMODE_3						((uchar)'3')		/* "3":停電時エレムーブ充電 */
#define	EINV_EMGMODE_4						((uchar)'4')		/* "4":停電時エレムーブ放電 */

/* 運転／停止 */
#define	EINV_INV_STOP						((uchar)'0')		/* 停止 */
#define	EINV_INV_START						((uchar)'1')		/* 運転 */

/* 試運転時停電出力 */
#define	EINV_TEST_RUN_POWERCUT_OUT_NO		((uchar)'0')		/* 連系出力 */
#define	EINV_TEST_RUN_POWERCUT_OUT_YES		((uchar)'1')		/* 停電出力 */

/* 試運転指示フラグ */
#define	EINV_TEST_RUN_FLAG_NO				((uchar)'0')		/* 試運転なし */
#define	EINV_TEST_RUN_FLAG_YES				((uchar)'1')		/* 試運転開始 */


/* CMD68 / CMD69 */


/* CMD6A / CMD6B */
/* 停電時出力設定 */
#define	EINV_PWRCUT_HAND_OUT					((uchar)'0')				/* 停電時手動出力 */
#define	EINV_PWRCUT_VOLUNTARILY_OUT				((uchar)'1')				/* 停電時自動出力 */

/* 停電時出力開始 */
#define	EINV_PWRCUT_OUT_STOP					((uchar)'0')				/* 出力停止 */
#define	EINV_PWRCUT_OUT_START					((uchar)'1')				/* 出力開始 */

/* 売電有効無効設定 */
#define	EINV_SELL_INVALID						((uchar)'0')				/* しない */
#define	EINV_SELL_VALID							((uchar)'1')				/* する */

/* メンテ指示 */
#define	EINV_MAINTENANCE_INDICATE_STOP			((uchar)'0')				/* 終了 */
#define	EINV_MAINTENANCE_INDICATE_START			((uchar)'1')				/* 開始 */


/* CMD6D */
/* 逆潮CT接続位置設定 */
#define	EINV_CONNECT_POSITION_SET_0				((uchar)'0')		/* "0":設定なし */
#define	EINV_CONNECT_POSITION_SET_1				((uchar)'1')		/* "1":切り替えモジュール内蔵CT */
#define	EINV_CONNECT_POSITION_SET_2				((uchar)'2')		/* "2":外付けCT */


/* CMD18 / CMD19 */
/* 外付け太陽光発電有無 */
#define	EINV_EXTER_SUN_PWR_0					((uchar)'0')		/* "0":未設定 */
#define	EINV_EXTER_SUN_PWR_1					((uchar)'1')		/* "1":なし */
#define	EINV_EXTER_SUN_PWR_2					((uchar)'2')		/* "2":あり */

/* 太陽光以外の発電装置 */
#define	EINV_SUN_BEYOND_PWR_GENRAT_0			((uchar)'0')		/* "0":未設定 */
#define	EINV_SUN_BEYOND_PWR_GENRAT_1			((uchar)'1')		/* "1":なし */
#define	EINV_SUN_BEYOND_PWR_GENRAT_2			((uchar)'2')		/* "2":あり */

/* 負荷接続タイプ*/
#define	EINV_LOAD_CONNECT_TYPE_0				((uchar)'0')		/* "0":未設定 */
#define	EINV_LOAD_CONNECT_TYPE_1				((uchar)'1')		/* "1":全負荷対応型  */
#define	EINV_LOAD_CONNECT_TYPE_2				((uchar)'2')		/* "2":重要負荷対応型 */

/* 出力制御対象 */
#define	EINV_OUTPUT_CONTR_OBJECT_NO				((uchar)'0')		/* "0":なし */
#define	EINV_OUTPUT_CONTR_OBJECT_YES			((uchar)'1')		/* "1":あり */

/* FIT契約タイプ */
#define	EINV_FIT_CONTRACT_TYPE_0				((uchar)'0')		/* "0":未設定 */
#define	EINV_FIT_CONTRACT_TYPE_1				((uchar)'1')		/* "1":非FIT */
#define	EINV_FIT_CONTRACT_TYPE_2				((uchar)'2')		/* "2":FIT */

/* 自家消費タイプ */
#define	EINV_SELF_CONSUME_TYPE_0				((uchar)'0')		/* "0":未設定 */
#define	EINV_SELF_CONSUME_TYPE_1				((uchar)'1')		/* "1":自家消費なし */
#define	EINV_SELF_CONSUME_TYPE_2				((uchar)'2')		/* "2":自家消費あり */

/* 電力会社サーバ */
#define EINV_PWR_COMPANY_SERVER_0				((uchar)'0')		/* "0":未設定 */
#define EINV_PWR_COMPANY_SERVER_1				((uchar)'1')		/* "1":北海道電力 */
#define EINV_PWR_COMPANY_SERVER_2				((uchar)'2')		/* "2":東北電力 */
#define EINV_PWR_COMPANY_SERVER_3				((uchar)'3')		/* "3":東京電力 */
#define EINV_PWR_COMPANY_SERVER_4				((uchar)'4')		/* "4":北陸電力 */
#define EINV_PWR_COMPANY_SERVER_5				((uchar)'5')		/* "5":中部電力 */
#define EINV_PWR_COMPANY_SERVER_6				((uchar)'6')		/* "6":関西電力 */
#define EINV_PWR_COMPANY_SERVER_7				((uchar)'7')		/* "7":中国電力 */
#define EINV_PWR_COMPANY_SERVER_8				((uchar)'8')		/* "8":四国電力 */
#define EINV_PWR_COMPANY_SERVER_9				((uchar)'9')		/* "9":九州電力 */
#define EINV_PWR_COMPANY_SERVER_A				((uchar)'A')		/* "A":沖縄電力 */

/* V:通信ボード内管理運転モード */
#define EINV_COMMU_BOARD_MANAGE_MODE_0			((uchar)'0')		/* "0":売電モード */
#define EINV_COMMU_BOARD_MANAGE_MODE_1			((uchar)'1')		/* "1":グリーンモード */
#define EINV_COMMU_BOARD_MANAGE_MODE_2			((uchar)'2')		/* "2":蓄電池充電 */
#define EINV_COMMU_BOARD_MANAGE_MODE_3			((uchar)'3')		/* "3":蓄電池放電 */
#define EINV_COMMU_BOARD_MANAGE_MODE_4			((uchar)'4')		/* "4":車両充電 */
#define EINV_COMMU_BOARD_MANAGE_MODE_5			((uchar)'5')		/* "5":車両放電 */
#define EINV_COMMU_BOARD_MANAGE_MODE_6			((uchar)'6')		/* "6":蓄電池・車両同時放電モード */
#define EINV_COMMU_BOARD_MANAGE_MODE_7			((uchar)'7')		/* "7":待機モード */

/* 外付け太陽光発電の有無 */
#define	EINV_SUN_PWR_EXTERN_NO					((uchar)'0')		/* "0":なし */
#define	EINV_SUN_PWR_EXTERN_YES					((uchar)'1')		/* "1":あり */

/* 太陽光発電設置場所 */
#define	EINV_SUN_PWR_SET_LOCATION_0				((uchar)'0')		/* "0":未設定 */
#define	EINV_SUN_PWR_SET_LOCATION_1				((uchar)'1')		/* "1":屋外 */
#define	EINV_SUN_PWR_SET_LOCATION_2				((uchar)'2')		/* "2":その他 */

/* 蓄電池設置場所 */
#define	EINV_BT_SET_LOCATION_0					((uchar)'0')		/* "0":未設定 */
#define	EINV_BT_SET_LOCATION_1					((uchar)'1')		/* "1":屋内 */
#define	EINV_BT_SET_LOCATION_2					((uchar)'2')		/* "2":屋外 */

/* V2Hスタンド設置場所 */
#define	EINV_V2H_STAND_SET_LOCATION_0			((uchar)'0')		/* "0":未設定 */
#define	EINV_V2H_STAND_SET_LOCATION_1			((uchar)'1')		/* "1":車庫 */
#define	EINV_V2H_STAND_SET_LOCATION_2			((uchar)'2')		/* "2":その他 */

/* 電気給湯器 */
#define	EINV_ELECT_WATER_HEATING_NO				((uchar)'0')		/* "0":なし */
#define	EINV_ELECT_WATER_HEATING_YES			((uchar)'1')		/* "1":あり */

/* OPリモコン有無 */
#define	EINV_OP_REMOTE_CONTROL_NO				((uchar)'0')		/* "0":なし */
#define	EINV_OP_REMOTE_CONTROL_YES				((uchar)'1')		/* "1":あり */

/* IPアドレス設定 */
#define	EINV_IP_ADDRESS_SET_AUTO				((uchar)'0')		/* "0":自動設定 */
#define	EINV_IP_ADDRESS_SET_HAND				((uchar)'1')		/* "1":手動設定 */

/* PROXY設定 */
#define	EINV_PROXY_SET_OFF						((uchar)'0')		/* "0":OFF */
#define	EINV_PROXY_SETL_ON						((uchar)'1')		/* "1":ON */

/* 試運転_蓄電池充電 */
#define	EINV_TESTRUN_BT_CHAR_0					((uchar)'0')		/* "0":－ */
#define	EINV_TESTRUN_BT_CHAR_1					((uchar)'1')		/* "1":未実施 */
#define	EINV_TESTRUN_BT_CHAR_2					((uchar)'2')		/* "2":OK */

/* 試運転_蓄電池放電 */
#define	EINV_TESTRUN_BT_DISC_0					((uchar)'0')		/* "0":－ */
#define	EINV_TESTRUN_BT_DISC_1					((uchar)'1')		/* "1":未実施 */
#define	EINV_TESTRUN_BT_DISC_2					((uchar)'2')		/* "2":OK */

/* 試運転_買電(E1-ACCT) */
#define	EINV_TESTRUN_BUY_E1_0					((uchar)'0')		/* "0":－ */
#define	EINV_TESTRUN_BUY_E1_1					((uchar)'1')		/* "1":未実施 */
#define	EINV_TESTRUN_BUY_E1_2					((uchar)'2')		/* "2":OK */

/* 試運転_買電(C2-ACCT) */
#define	EINV_TESTRUN_BUY_C2_0					((uchar)'0')		/* "0":－ */
#define	EINV_TESTRUN_BUY_C2_1					((uchar)'1')		/* "1":未実施 */
#define	EINV_TESTRUN_BUY_C2_2					((uchar)'2')		/* "2":OK */

/* 試運転_車両充電 */
#define	EINV_TESTRUN_CAR_CHAR_0					((uchar)'0')		/* "0":－ */
#define	EINV_TESTRUN_CAR_CHAR_1					((uchar)'1')		/* "1":未実施 */
#define	EINV_TESTRUN_CAR_CHAR_2					((uchar)'2')		/* "2":OK */

/* 試運転_車両放電 */
#define	EINV_TESTRUN_CAR_DISC_0					((uchar)'0')		/* "0":－ */
#define	EINV_TESTRUN_CAR_DISC_1					((uchar)'1')		/* "1":未実施 */
#define	EINV_TESTRUN_CAR_DISC_2					((uchar)'2')		/* "2":OK */

/* 試運転_内部太陽光発電の動作 */
#define	EINV_TESTRUN_INTER_SUN_PWR_GERAT_0		((uchar)'0')		/* "0":－ */
#define	EINV_TESTRUN_INTER_SUN_PWR_GERAT_1		((uchar)'1')		/* "1":未実施 */
#define	EINV_TESTRUN_INTER_SUN_PWR_GERAT_2		((uchar)'2')		/* "2":OK */

/* 試運転_外付け太陽光発電の動作 */
#define	EINV_TESTRUN_EXTERN_SUN_PWR_GERAT_0		((uchar)'0')		/* "0":－ */
#define	EINV_TESTRUN_EXTERN_SUN_PWR_GERAT_1		((uchar)'1')		/* "1":未実施 */
#define	EINV_TESTRUN_EXTERN_SUN_PWR_GERAT_2		((uchar)'2')		/* "2":OK */

/* 試運転_切り替えモジュール動作(停電) */
#define	EINV_TESTRUN_SWITCH_PWRCUT_MODE_0		((uchar)'0')		/* "0":－ */
#define	EINV_TESTRUN_SWITCH_PWRCUT_MODE_1		((uchar)'1')		/* "1":未実施 */
#define	EINV_TESTRUN_SWITCH_PWRCUT_MODE_2		((uchar)'2')		/* "2":OK */

/* 試運転_停電時E1放電 */
#define	EINV_TESTRUN_PUTCUT_E1_DISC_0			((uchar)'0')		/* "0":－ */
#define	EINV_TESTRUN_PUTCUT_E1_DISC_1			((uchar)'1')		/* "1":未実施 */
#define	EINV_TESTRUN_PUTCUT_E1_DISC_2			((uchar)'2')		/* "2":OK */

/* 試運転_切り替えモジュール動作(復帰) */
#define	EINV_TESTRUN_PUTCUT_SWITCH_RESET_MODE_0	((uchar)'0')		/* "0":－ */
#define	EINV_TESTRUN_PUTCUT_SWITCH_RESET_MODE_1	((uchar)'1')		/* "1":未実施 */
#define	EINV_TESTRUN_PUTCUT_SWITCH_RESET_MODE_2	((uchar)'2')		/* "2":OK */

/* クリップ動作 */
#define	EINV_CLIP_ACTION_0						((uchar)'0')		/* "0":なし */
#define	EINV_CLIP_ACTION_1						((uchar)'1')		/* "1":上限クリップ */
#define	EINV_CLIP_ACTION_2						((uchar)'2')		/* "2":常時クリップ */

/* 実行モード */
#define	EINV_EXECUTE_MODE_USUAL					((uchar)'0')		/* "0":通常 */
#define	EINV_EXECUTE_MODE_FORCE					((uchar)'1')		/* "1":強制 */

/* メンテ結果 */
#define	EINV_MAINTAIN_RESULT_NOT_IMPLMT			((uchar)'0')		/* "0":未実施 */
#define	EINV_MAINTAIN_RESULT_IMPLMT				((uchar)'1')		/* "1":実施済み */


/* CMD74 / CMD75 */
/* 車両操作優先 */
#define	EINV_CAR_OPERT_PRIORITY_NO				((uchar)'0')			/* 車両操作優先しない */
#define	EINV_CAR_OPERT_PRIORITY_YES				((uchar)'1')			/* 車両操作優先 */

/* 車両充放電器本体操作 */
#define	EINV_BODY_OPRAT_NO						((uchar)'0')			/* 操作不可 */
#define	EINV_BODY_OPRAT_YES						((uchar)'1')			/* 操作可 */

/* 停電時放電動作 */
#define	EINV_PWRCUT_DISCHAR_NO					((uchar)'0')			/* 放電不可 */
#define	EINV_PWRCUT_DISCHAR_YES					((uchar)'1')			/* 放電可 */

/* 停電時放電優先設定 */
#define	EINV_PWRCUT_DISC_PRIORITY_BT			((uchar)'0')			/* 蓄電池 */
#define	EINV_PWRCUT_DISC_PRIORITY_CAR			((uchar)'1')			/* 車両 */

/* 停電時充電優先設定 */
#define	EINV_PWRCUT_CHAR_PRIORITY_BT			((uchar)'0')			/* 蓄電池 */
#define	EINV_PWRCUT_CHAR_PRIORITY_CAR			((uchar)'1')			/* 車両 */

/* コネクタロック／解除 */
#define	EINV_CONNECT_UNLOCK						((uchar)'0')			/* アンロック */
#define	EINV_CONNECT_LOCK						((uchar)'1')			/* ロック */

/* 車両放電有無 */
#define	EINV_CAR_DISC_NO						((uchar)'0')			/* 放電しない */
#define	EINV_CAR_DISC_YES						((uchar)'1')			/* 放電する */

/* 放電優先設定 */
#define	EINV_DISCHAR_PRIORIT_BT					((uchar)'0')			/* 蓄電池放電 */
#define	EINV_DISCHAR_PRIORIT_CAR				((uchar)'1')			/* 車両放電 */

/* 充電優先設定 */
#define	EINV_CHAR_PRIORIT_BT					((uchar)'0')			/* 蓄電池充電 */
#define	EINV_CHAR_PRIORIT_CAR					((uchar)'1')			/* 車両充電 */


/* CMD81 */
/* 車両充放電制限 */
#define	EINV_CAR_CHAR_OR_DISC_LIMITED_0			((uchar)'0')			/* "0";制限なし */
#define	EINV_CAR_CHAR_OR_DISC_LIMITED_1			((uchar)'1')			/* "1":充電制限 */
#define EINV_CAR_CHAR_OR_DISC_LIMITED_2			((uchar)'2')			/* "2":放電制限 */
#define EINV_CAR_CHAR_OR_DISC_LIMITED_3			((uchar)'3')			/* "3":充放電制限 */

/* PCS運転状態 */
#define	EINV_PCS_OPESTATE_STOP					((uchar)'0')			/* 停止中 */
#define	EINV_PCS_OPESTATE_RUN					((uchar)'1')			/* 運転中 */

/* 蓄電池運転動作状態 */
#define	EINV_BATTERY_OPERATION_STOP				((uchar)'0')			/* 停止中 */
#define	EINV_BATTERY_OPERATION_RUN				((uchar)'1')			/* 運転中 */

/* 停電 */
#define	EINV_PWRCUT_0							((uchar)'0')			/* "0":それ以外 */
#define	EINV_PWRCUT_1							((uchar)'1')			/* "1":停電  */
#define	EINV_PWRCUT_2							((uchar)'2')			/* "2":試運転時停電 */

/* 出力抑制状態 */
#define	EINV_SUPPRESS_STATE_0					((uchar)'0')			/* "0":抑制なし */
#define	EINV_SUPPRESS_STATE_1					((uchar)'1')			/* "1":電圧上昇中 */
#define	EINV_SUPPRESS_STATE_2					((uchar)'2')			/* "2":出力制御中 */

/* コネクタロック状態 */
#define	EINV_UNLOCK_STATE						((uchar)'0')			/* 解除状態 */
#define	EINV_LOCKED_STATE						((uchar)'1')			/* ロック状態 */

/* 車両充放電器運転動作状態 */
#define EINV_OPRATSTATE_0						((uchar)'0')			/* "0":起動処理 */
#define EINV_OPRATSTATE_1						((uchar)'1')			/* "1":未ロック停止 */
#define EINV_OPRATSTATE_2						((uchar)'2')			/* "2":ロック停止 */
#define EINV_OPRATSTATE_3						((uchar)'3')			/* "3":充放電準備 */
#define EINV_OPRATSTATE_4						((uchar)'4')			/* "4":ロック中 */
#define EINV_OPRATSTATE_5						((uchar)'5')			/* "5":充電 */
#define EINV_OPRATSTATE_6						((uchar)'6')			/* "6":放電 */
#define EINV_OPRATSTATE_7						((uchar)'7')			/* "7":グリーン */
#define EINV_OPRATSTATE_8						((uchar)'8')			/* "8":余剰電力充電 */
#define EINV_OPRATSTATE_9						((uchar)'9')			/* "9":指定電力充電 */
#define EINV_OPRATSTATE_A						((uchar)'A')			/* "A":指定買電電力充放電 */
#define EINV_OPRATSTATE_B						((uchar)'B')			/* "B":自立放電 */
#define EINV_OPRATSTATE_C						((uchar)'C')			/* "C":充放電スタンバイ */
#define EINV_OPRATSTATE_D						((uchar)'D')			/* "D":充放電終了準備 */
#define EINV_OPRATSTATE_E						((uchar)'E')			/* "E":エラー停止  */

/* 車両接続充放電可否状態 */
#define EINV_STATUS_0XFF						((uchar)0xFFU)			/* "0xFF":不定 */
#define EINV_STATUS_0X30						((uchar)0x30U)			/* "0x30":車両未接続 */
#define EINV_STATUS_0X40						((uchar)0x40U)			/* "0x40":車両接続・充電不可・放電不可 */
#define EINV_STATUS_0X41						((uchar)0x41U)			/* "0x41":車両接続・充電可・放電不可 */
#define EINV_STATUS_0X42						((uchar)0x42U)			/* "0x42":車両接続・充電不可・放電可 */
#define EINV_STATUS_0X43						((uchar)0x43U)			/* "0x43":車両接続・充電可・放電可 */

/* 試運転状態通知 CMD81戻り値 */ 
#define	EINV_TESTRUN_OFF						((uchar)'0')			/* OFF */
#define	EINV_TESTRUN_ON							((uchar)'1')			/* ON */

/* 試運転状態通知 試運転可否 */ 
#define	EINV_TESTRUN_UNAVAILABLE				((uchar)'0')			/* 試運転不可能 */
#define	EINV_TESTRUN_AVAILABLE					((uchar)'1')			/* 試運転可能 */

/* LAN試運転指示 */ 
#define	EINV_TESTRUN_START						((uchar)'1')			/* 試運転開始 */
#define	EINV_TESTRUN_FIN						((uchar)'2')			/* 試運転終了 */
#define	EINV_TESTRUN_PROABORT					((uchar)'3')			/* 試運転中止 */

/* V2H本体操作ボタン2 */
#define	EINV_V2H_OPERATION_BUTTON2_OFF			((uchar)'0')			/* OFF */
#define	EINV_V2H_OPERATION_BUTTON2_ON			((uchar)'1')			/* ON */

/* V2H本体操作ボタン3 */
#define	EINV_V2H_OPERATION_BUTTON3_OFF			((uchar)'0')			/* OFF */
#define	EINV_V2H_OPERATION_BUTTON3_ON			((uchar)'1')			/* ON */

/* 接続中の車種またはメーカー(特定車両判定) */
#define	EINV_TYPE_OR_MANUFACTURER_0				((uchar)'0')			/* "0":その他 */
#define	EINV_TYPE_OR_MANUFACTURER_1				((uchar)'1')			/* "1":T社プリウスPHV */
#define	EINV_TYPE_OR_MANUFACTURER_2				((uchar)'2')			/* "2":N社EV */
#define	EINV_TYPE_OR_MANUFACTURER_3				((uchar)'3')			/* "3":FCV車両 */

/* 充電不可フラグ */
#define	EINV_CHAR_FLAG_NOHAVE					((uchar)'0')			/* なし */
#define	EINV_CHAR_FLAG_NO						((uchar)'1')			/* 不可 */

/* 放電不可フラグ */
#define	EINV_DISCHAR_FLAG_NOHAVE				((uchar)'0')			/* なし */
#define	EINV_DISCHAR_FLAG_NO					((uchar)'1')			/* 不可 */

/* 蓄電池充電完了通知 */
#define	EINV_CHAR_COPLT_NOTICE_NO				((uchar)'0')			/* 未完了 */
#define	EINV_CHAR_COPLT_NOTICE_YES				((uchar)'1')			/* 完了 */

/* 蓄電池放電完了通知 */
#define	EINV_DISCHAR_COPLT_NOTICE_NO			((uchar)'0')			/* 未完了 */
#define	EINV_DISCHAR_COPLT_NOTICE_YES			((uchar)'1')			/* 完了 */

/* メンテ電池状態 */
#define	EINV_BTSTATE_0							((uchar)'0')			/* "0":通常 */
#define	EINV_BTSTATE_1							((uchar)'1')			/* "1":メンテナンス中 */
#define	EINV_BTSTATE_2							((uchar)'2')			/* "2":正常終了 */
#define	EINV_BTSTATE_3							((uchar)'3')			/* "3":失敗1 */
#define	EINV_BTSTATE_4							((uchar)'4')			/* "4":失敗2 */
#define	EINV_BTSTATE_5							((uchar)'5')			/* "5":失敗3 */


/* 管理用パラメータ */
/* V:試運転要否 */
#define EINV_NONEED_TEST_RUN				((uchar)0)					/* 0:不要 */
#define EINV_NEED_TEST_RUN					((uchar)1)					/* 1:必要 */

/* V:システム機種名 */
#define EINV_SYS_MODEL_NAME_INIT			((schar*)"----------------")
#define EINV_SYS_MODEL_NAME_E1M1			((schar*)"ESS-E1M1")
#define EINV_SYS_MODEL_NAME_E1L1			((schar*)"ESS-E1L1")
#define EINV_SYS_MODEL_NAME_E1				((schar*)"ES-E1")

/* ENLクラスの生成 */
/* システム構成ビット */
#define	EINV_SYS_CON_PV						((ushort)0x0001)			/* 太陽光を含むシステム構成 */
#define	EINV_SYS_CON_BT						((ushort)0x0002)			/* 蓄電池を含むシステム構成 */
#define	EINV_SYS_CON_V2H					((ushort)0x0004)			/* V2Hを含むシステム構成 */
/* システム構成 */
#define	EINV_SYS_CON_TR_PBV		(EINV_SYS_CON_PV|EINV_SYS_CON_BT|EINV_SYS_CON_V2H)	/* 太陽光＋蓄電池＋V2H */
#define	EINV_SYS_CON_HY_PB		(EINV_SYS_CON_PV|EINV_SYS_CON_BT)					/* 太陽光＋蓄電池 */
#define	EINV_SYS_CON_HY_PV		(EINV_SYS_CON_PV|EINV_SYS_CON_V2H)					/* 太陽光＋V2H */
#define	EINV_SYS_CON_HY_BV		(EINV_SYS_CON_BT|EINV_SYS_CON_V2H)					/* 蓄電池＋V2H */
#define	EINV_SYS_CON_SI_B		(EINV_SYS_CON_BT)									/* 蓄電池 */
#define	EINV_SYS_CON_SI_V		(EINV_SYS_CON_V2H)									/* V2H */
#define	EINV_SYS_CON_P			(EINV_SYS_CON_PV)									/* PV */
#define	EINV_SYS_CON_UNDEF		((ushort)0x0000)									/* システム構成未設定 */


/* 運転モード設定変更フラグ */
#define	EINV_SET_RUN_MODE_SET_FROM	((ulong)(0x00000001))	/* 運転モード設定元 */
#define	EINV_SET_OPE_MODE			((ulong)(0x00000002))	/* 運転モード */
#define	EINV_SET_EMG_MODE			((ulong)(0x00000004))	/* 非常時運転モード */
#define	EINV_SET_TIME_ZONE			((ulong)(0x00000008))	/* 充放電時間帯 */
#define	EINV_SET_START_OR_STOP		((ulong)(0x00000010))	/* 運転/停止 */
#define	EINV_SET_TESTRUN_OUT		((ulong)(0x00000020))	/* 試運転時停電出力 */
#define	EINV_SET_BT_CHARGE_STYLE	((ulong)(0x00000040))	/* 蓄電池充電方式(ENL) */
#define	EINV_SET_BT_DISC_STYLE		((ulong)(0x00000080))	/* 蓄電池放電方式(ENL) */
#define	EINV_SET_BT_CHARGE_PWR		((ulong)(0x00000100))	/* 蓄電池充電電力設定値(ENL) */
#define	EINV_SET_BT_DISC_PWR		((ulong)(0x00000200))	/* 蓄電池放電電力設定値(ENL) */
#define	EINV_SET_BT_CHARGE_PWR_MAX	((ulong)(0x00000400))	/* ユーザ設定の蓄電池最大充電電力設定値 */
#define	EINV_SET_BT_AC_CHARGE_QUATY	((ulong)(0x00000800))	/* 蓄電池AC充電量設定値 */
#define	EINV_SET_BT_AC_DISC_QUATY	((ulong)(0x00001000))	/* 蓄電池AC放電量設定値 */
#define	EINV_SET_BT_DC_CHARGE_QUATY	((ulong)(0x00002000))	/* 蓄電池DC充電量設定値 */
#define	EINV_SET_BT_DC_DISC_QUATY	((ulong)(0x00004000))	/* 蓄電池DC放電量設定値 */
#define	EINV_SET_USER_UPPER_LIMIT	((ulong)(0x00008000))	/* ユーザ設定の蓄電池充電上限値設定 */
#define	EINV_SET_OPEMODE_ALL		((ulong)(0x0000FFFF))	/* 以上 */

/* 整定値設定変更フラグ */
#define	EINV_SET_SYS_VOLT_DETECT_LEVEL	((ulong)(0x00000001))/* 系統過電圧検出レベル */
#define	EINV_SET_SYS_VOLT_DETECT_TIME	((ulong)(0x00000002))/* 系統過電圧検出時間 */
#define	EINV_SET_SYS_UNDER_VOLT_DETECT_LEVEL ((ulong)(0x00000004))/*系統不足電圧検出レベル*/
#define	EINV_SET_SYS_UNDER_VOLT_DETECT_TIME	((ulong)(0x00000008))/* 系統不足電圧検出時間 */
#define	EINV_SET_FRE_RISE_DETECT_LEVEL	((ulong)(0x00000010))/* 周波数上昇検出レベル */
#define	EINV_SET_FRE_RISE_DETECT_TIME	((ulong)(0x00000020))/* 周波数上昇検出時間 */
#define	EINV_SET_FRE_DROP_DETECT_LEVEL	((ulong)(0x00000040))/* 周波数低下検出レベル */
#define	EINV_SET_FRE_DROP_DETECT_TIME	((ulong)(0x00000080))/* 周波数低下検出時間 */
#define	EINV_SET_PASS_IND_OPER_DETECT	((ulong)(0x00000100))/* 受動的単独運転検出レベル */
#define	EINV_SET_RESCT_TIME				((ulong)(0x00000200))/* 復帰時間 */
#define	EINV_SET_CHANGE_TIME_INCOR_DES	((ulong)(0x00000400))/* 出力増減変化時間 */
#define	EINV_SET_VOL_RS_INHB_VALUE		((ulong)(0x00000800))/* 電圧上昇抑制値 */
#define	EINV_SET_VOL_RS_INHBATT_LIMIT	((ulong)(0x00001000))/* 電圧上昇抑制動作待機時限 */
#define	EINV_SET_COT_POWER_CONTROL		((ulong)(0x00002000))/* 力率一定制御 */
#define	EINV_SET_FRE_FEEDBACK_GAIN		((ulong)(0x00004000))/* 周波数フィードバックゲイン */
#define	EINV_SET_FRE_LEV_ALLOW			((ulong)(0x00008000))/* 並列時許容周波数レベル */
#define	EINV_SET_SPDATA_ALL				((ulong)(0x0000FFFF))/* 以上 */

/* 特殊設定値情報設定変更フラグ */
#define	EINV_SET_INSTALLATION_DATE		((ulong)(0x00000001))	/* 設置日 */
#define	EINV_SET_SYS_MADE_NUM			((ulong)(0x00000002))	/* システム製造番号 */
#define	EINV_SET_SYS_CONF				((ulong)(0x00000004))	/* システム構成 */
#define	EINV_SET_REVCT_CONNECT_POSITION	((ulong)(0x00000008))	/* 逆潮CT接続位置設定 */
#define	EINV_SET_UNIQUE_INFO_ALL		((ulong)(0x0000000F))	/* 以上 */

/* V2H電力情報設定変更フラグ */
#define	EINV_SET_V2H_OPE_PRIORITY		((ulong)(0x00000001))	/* 車両操作優先 */
#define	EINV_SET_V2H_MAINUNIT_OPE		((ulong)(0x00000002))	/* 車両充放電器本体操作 */
#define	EINV_SET_V2H_EMG_DISCHARGE		((ulong)(0x00000004))	/* 非常時放電動作 */
#define	EINV_SET_V2H_EMG_DISC_PRIORITY	((ulong)(0x00000008))	/* 非常時放電優先設定 */
#define	EINV_SET_V2H_EMG_CHARGE_PRIORITY ((ulong)(0x00000010))	/* 非常時充電優先設定 */
#define	EINV_SET_V2H_EMG_DISC_LOWER_LIMIT ((ulong)(0x00000020))/* 非常時車両放電下限値 <- 非常時放電下限値設定(車両)として流用する */
#define	EINV_SET_V2H_CONNECTOR_LOCK		((ulong)(0x00000040))	/* コネクタロック／解除 */
#define	EINV_SET_V2H_CHARGE_UPPER_LIMIT	((ulong)(0x00000080))	/* 車両充電上限値設定 */
#define	EINV_SET_V2H_DISC_SETTING_FLAG	((ulong)(0x00000100))	/* 車両放電有無 */
#define	EINV_SET_V2H_DISC_PRIORITY		((ulong)(0x00000200))	/* 放電優先設定 */
#define	EINV_SET_V2H_CHARGE_PRIORITY	((ulong)(0x00000400))	/* 充電優先設定 */
#define	EINV_SET_V2H_CHARGE_STYLE		((ulong)(0x00000800))	/* v2h充電方式(ENL) */
#define	EINV_SET_V2H_DISC_STYLE			((ulong)(0x00001000))	/* v2h放電方式(ENL) */
#define	EINV_SET_V2H_CHARGE_PWR			((ulong)(0x00002000))	/* v2h充電電力設定値(ENL) */
#define	EINV_SET_V2H_DISC_PWR			((ulong)(0x00004000))	/* v2h放電電力設定値(ENL) */
#define	EINV_SET_V2H_CHARGE_PWR_MAX	((ulong)(0x00008000))/* ユーザ設定の車両最大充電電力設定値[kW] */
#define	EINV_SET_V2H_AC_CHARGE_QUATY	((ulong)(0x00010000))	/* v2h AC充電量設定値 */
#define	EINV_SET_V2H_AC_DISC_QUATY		((ulong)(0x00020000))	/* v2h AC放電量設定値 */
#define	EINV_SET_V2H_SETVALUE_ALL		((ulong)(0x0003FFFF))	/* 以上 */

/* 太陽光発電情報設定変更フラグ */
#define	EINV_SET_FIT_TYPE					((ulong)(0x00000001))/* FIT契約タイプ */
#define	EINV_SET_SELFCONSUMPTION_TYPE		((ulong)(0x00000002))/* 自家消費タイプ */
#define	EINV_SET_FACILITY_CERTIFIED_CAPACITY ((ulong)(0x00000004))/*設備認定容量[W]*/
#define	EINV_SET_PVINFO_SET_ALL				((ulong)(0x00000007))/* 以上 */



/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
/* 日付情報構造体(年4桁) */
typedef struct {
	schar	year[EINV_LONGDATEYEAR_LEN];			/* 年（“2000”～“2099”）*/
	schar	month[EINV_DATEMONTH_LEN];				/* 月（“01”～“12”）*/
	schar	day[EINV_DATEDAY_LEN];					/* 日（“01”～“31”）*/
}EinvLongDateStr_t;

/* 時刻情報構造体 */
typedef struct {
	schar	hour[EINV_TIMEHOUR_LEN];				/* 時（“00”～“23”）*/
	schar	minute[EINV_TIMEMINUTE_LEN];			/* 分（“00”～“59”）*/
}EinvTimeStr_t;

/* 動作情報構造体(81) */
typedef struct {
	ushort 	uSOC;								/* V:uSOC[1%] */
	sshort	chargeDischargePwrAC;				/* PCS充放電電力（AC端計測値）[W] */
	uchar	opeState;							/* PCS運転状態 */
	ulong	ttlChargeEnergySysAC;				/* 積算PCS充電電力量(AC端計測値)(系統連系時) */
	ulong	ttlDischargeEnergySysAC;			/* 積算PCS放電電力量(AC端計測値)(系統連系時) */
	ushort	rsoc;								/* rSOC[1%] */
	ushort	remainingBt;						/* 電池残量[0.1kWh] */
	ushort	btVolt;								/* 電池電圧[0.1V] */
	ushort	soh;								/* SOH[1%] */
	uchar	btOperation;						/* 蓄電池運転動作状態 */
	uchar	pwrCut;								/* 停電 */
	sshort	pvGenPwr;							/* PV発電電力(本製品以外のPV) */
	sshort	revPwrFlow;							/* 逆潮流電力 */
	ushort	sysVolt;							/* 系統電圧 */
	schar	pcsErr[EINV_PCSERR_LEN];			/* PCSエラー */
	schar	pcsErrRating[EINV_PCSERRRATING_LEN];/* PCSエラーレーティング */
	uchar	opeMode;							/* 運転モード */
	sshort	chargeDischargePwrDC;				/* DC蓄電池充放電電力2(コンバータ部) */
	ushort	ttlChargeEnergyDC;					/* DC積算蓄電池充電電力量2(コンバータ部) */
	ushort	ttlDischargeEnergyDC;				/* DC積算蓄電池放電電力量2(コンバータ部) */
	ulong	ttlPvGenPwr;						/* 積算PV発電電力量(本製品以外のPV) */
	ulong	ttlPurchasePwr;						/* 積算買電電力量 */
	ulong	ttlSellingPwr;						/* 積算売電電力量 */
	ushort	genPwrLimit;						/* 発電電力制限値(kW) */
	uchar	suppressState;						/* 出力抑制状態 */
	uchar	carConnectorLockState;				/* コネクタロック状態 */
	uchar	carCharDiscOpratState;				/* 車両充放電器運転動作状態 */
	ulong	carDischargeableRemainingCapacity1;	/* 車載電池の放電可能残容量1 */
	uchar	carDischargeableRemainingCapacity3;	/* 車載電池の放電可能残容量3 */
	ulong	carChargeableRemainCap;				/* 車載電池の充電可能残容量値 */
	uchar	carAvailabilityStatus;				/* 車両接続充放電可否状態 */
	slong	v2hInstantPower;					/* V2H瞬時充放電電力計測値(AC端計測値) */
	ulong	carTtlDischargePower;		/* 車載電池の積算放電電力量計測値(AC端計測値)(トータル) */
	ulong	carTtlChargePower;			/* 車載電池の積算充電電力量計測値(AC端計測値)(トータル) */
	uchar	emergencyMode;						/* 非常時運転モード */
	ulong	carRemainingCapacity1;				/* 車載電池の電池残容量1[wh] */
	uchar	carRemainingCapacity3;				/* 車載電池の電池残容量3[%] */
	ulong	ttlChargeEnergyAloneAC;				/* 積算PCS充電電力量(AC端計測値)(自立時) */
	ulong	ttlDischargeEnergyAloneAC;			/* 積算PCS放電電力量(AC端計測値)(自立時) */
	ushort	pcsEfficiencyChar;					/* PCS効率(充電方向) */
	ushort	pcsEfficiencyDischar;				/* PCS効率(放電方向) */
	slong	btCharDischarPwr;					/* 蓄電池充放電電力(AC端計測値) */
	ushort	pvPwr2;								/* 太陽光発電電力(AC端計測値) */
	ulong	ttlPvGenPwr2;						/* 積算太陽光発電電力量(AC端計測値) */
	ushort	cellVoltMIN;						/* 最低セル電圧 */
	ushort	cellAverageVolt;					/* 平均セル電圧 */
	ushort	cellVoltMAX;						/* 最高セル電圧 */
	sshort	cellTempeMIN;						/* 最低セル温度 */
	sshort	cellTempeMAX;						/* 最高セル温度 */
	ushort	pvStringVolt1;						/* 太陽光発電ストリング電圧(1) */
	ushort	pvStringVolt2;						/* 太陽光発電ストリング電圧(2) */
	ushort	pvStringVolt3;						/* 太陽光発電ストリング電圧(3) */
	ushort	pvStringVolt4;						/* 太陽光発電ストリング電圧(4) */
	ushort	pvStringVolt5;						/* 太陽光発電ストリング電圧(5) */
	uchar	testRunoutinfrom;					/* 試運転状態通知 */
	uchar	v2hOperationButton1;				/* V2H本体操作ボタン1 */
	uchar	v2hOperationButton2;				/* V2H本体操作ボタン2 */
	uchar	v2hOperationButton3;				/* V2H本体操作ボタン3 */
	uchar	connectCarTypeOrManufacturer;		/* 接続中の車種またはメーカー(特定車両判定) */
	uchar	notChargeFlag;						/* 充電不可フラグ */
	uchar	notDisChargeFlag;					/* 放電不可フラグ */
	ushort	revCtCurrentU;						/* 逆潮CTの電流値(U相) */
	ushort	revCtCurrentW;						/* 逆潮CTの電流値(W相) */
	ulong	carbtDiscCapacityValue1;			/* 車載電池の放電可能容量値1 */
	ulong	carbtUseCapacityValue1;				/* 車載電池の使用容量値1 */
	ulong	carbtCharCapacityValue;				/* 車載電池の充電可能容量値 */
	uchar	btCharCopltNotice;					/* 蓄電池充電完了通知 */
	uchar	btDischarCopltNotice;				/* 蓄電池放電完了通知 */
	uchar	maintenanceBtState;					/* メンテ電池状態 */
}EinvOpesttsInfo_t;

/* 表示用シグナル処理構造体 */
typedef struct {
	sshort	pvGenPwrAvg;						/* PV発電電力(本製品以外のPV)平均値[W] */
	ushort	pvPwr2Avg;							/* 太陽光発電電力(AC端計測値)平均値[W] */
	sshort	revPwrFlowAvg;						/* 逆潮流電力平均値[W] */
	slong	v2hInstantPowerAvg;					/* V2H瞬時充放電電力計測値(AC端計測値)平均値[W] */
	slong	btChargeDischPwrAvg;				/* 蓄電池充放電電力(AC端計測値)平均値[W] */
	slong	homeLoadPwr;						/* 家庭負荷電力[W] */
	slong	pvPwrAvgSum;						/* 太陽光発電電力平均値sum[W] */
}EinvCalcAvg_t;

/* 充放電時間帯情報構造体 */
typedef struct {
	EinvTimeStr_t		chargeStartTime;	/* 充電開始時刻 */
	EinvTimeStr_t		chargeStopTime;		/* 充電停止時刻 */
	EinvTimeStr_t		dischargeStartTime;	/* 放電開始時刻 */
	EinvTimeStr_t		dischargeStopTime;	/* 放電停止時刻 */
}EinvChargeTimeZone_t;

/* 運転モード蓄電池電力取得用構造体 */
typedef struct {
	uchar	runModeSetFrom;			/* V:運転モード設定元 */
	uchar	splPowerRunFlag;		/* V:指定電力運転フラグ */
	uchar	opeMode;				/* 運転モード */
	uchar	emergencyMode;			/* 非常時運転モード */
	EinvChargeTimeZone_t	chargeTimeZone;	/* 充放電時間帯 */
	uchar	startOrStop;			/* 運転/停止 */
	uchar	testrunOutAtPwrCut;		/* 試運転時停電出力 */
	uchar	beforeAutoOpeMode;		/* 前回の自動運転モード */
	uchar 	btChargeStyle;			/* 蓄電池充電方式(ENL) */
	uchar 	btDischargeStyle;		/* 蓄電池放電方式(ENL) */
	ushort	btChargePwrSetValue;	/* 蓄電池充電電力設定値[0.01KW](ENL) */
	ushort	btDiscPwrSetValue;		/* 蓄電池放電電力設定値[0.01KW](ENL) */
	ulong	btChargePwrSetValueMax;/* ユーザ設定の蓄電池最大充電電力設定値[kW] */
	ushort 	btACChargeQuatySetValue;/* 蓄電池AC充電量設定値 */
	ushort 	btACDiscQuatySetValue;	/* 蓄電池AC放電量設定値 */
	ushort 	btDCChargeQuatySetValue;/* 蓄電池DC充電量設定値 */
	ushort 	btDCDiscQuatySetValue;	/* 蓄電池DC放電量設定値 */
	ushort	userUpperLimit;			/* ユーザ表示の蓄電池充電上限値設定 */
	ushort 	userLowerLimit;			/* ユーザ表示の蓄電池放電下限値設定 */
}EinvOpemodeGet_t;

/* 運転モード蓄電池電力設定用構造体 */
typedef struct {
	ulong	changeFlag;						/* 変更フラグ */
	uchar	runModeSetFrom;					/*V:運転モード設定元*/
	uchar	opeMode;						/* 運転モード */
	uchar	emergencyMode;					/* 非常時運転モード */
	EinvChargeTimeZone_t	chargeTimeZone;	/* 充放電時間帯 */
	uchar	startOrStop;					/* 運転/停止 */
	uchar	testrunOutAtPwrCut;				/* 試運転時停電出力 */
	uchar 	btChargeStyle;					/* 蓄電池充電方式(ENL) */
	uchar 	btDischargeStyle;				/* 蓄電池放電方式(ENL) */
	ushort	btChargePwrSetValue;			/* 蓄電池充電電力設定値[0.01KW](ENL) */
	ushort	btDiscPwrSetValue;				/* 蓄電池放電電力設定値[0.01KW](ENL) */
	ulong	btChargePwrSetValueMax;			/* ユーザ設定の蓄電池最大充電電力設定値[kW] */
	ushort 	btACChargeQuatySetValue;		/* 蓄電池AC充電量設定値 */
	ushort 	btACDiscQuatySetValue;			/* 蓄電池AC放電量設定値 */
	ushort 	btDCChargeQuatySetValue;		/* 蓄電池DC充電量設定値 */
	ushort 	btDCDiscQuatySetValue;			/* 蓄電池DC放電量設定値 */
	ushort	userUpperLimit;					/* ユーザ設定の蓄電池充電上限値設定 */
	ushort 	userLowerLimit;					/* ユーザ設定の蓄電池放電下限値設定 */
}EinvOpemodeSet_t;

/* 整定値取得用構造体 */
typedef struct {
	ushort	sysVoltDetectLevel;		/* 系統過電圧検出レベル */
	ushort	sysVoltDetectTime;		/* 系統過電圧検出時間 */
	ushort	sysUnderVoltDetectLevel;/* 系統不足電圧検出レベル */
	ushort	sysUnderVoltDetectTime;	/* 系統不足電圧検出時間 */
	uchar	freRiseDetectLevel;		/* 周波数上昇検出レベル */
	ushort	freRiseDetectTime;		/* 周波数上昇検出時間 */
	uchar	freDropDetectLevel;		/* 周波数低下検出レベル */
	ushort	freDropDetectTime;		/* 周波数低下検出時間 */
	ushort	passIndOperDetect;		/* 受動的単独運転検出レベル */
	ushort	resctTime;				/* 復帰時間 */
	ushort	changeTimeIncorDes;		/* 出力増減変化時間 */
	ushort	volRsInhbValue;			/* 電圧上昇抑制値 */
	ushort	volRsInhbattLimit;		/* 電圧上昇抑制動作待機時限 */
	ushort	cotPowerControl;		/* 力率一定制御 */
	ushort	freFeedbackGain;		/* 周波数フィードバックゲイン */
	ushort	freLevAllow;			/* 並列時許容周波数レベル */
}EinvSpdataGet_t;

/* 整定値設定用構造体(68) */
typedef struct {
	ulong	changeFlag;				/* 変更フラグ */
	ushort	sysVoltDetectLevel;		/* 系統過電圧検出レベル */
	ushort	sysVoltDetectTime;		/* 系統過電圧検出時間 */
	ushort	sysUnderVoltDetectLevel;/* 系統不足電圧検出レベル */
	ushort	sysUnderVoltDetectTime;	/* 系統不足電圧検出時間 */
	uchar	freRiseDetectLevel;		/* 周波数上昇検出レベル */
	ushort	freRiseDetectTime;		/* 周波数上昇検出時間 */
	uchar	freDropDetectLevel;		/* 周波数低下検出レベル */
	ushort	freDropDetectTime;		/* 周波数低下検出時間 */
	ushort	passIndOperDetect;		/* 受動的単独運転検出レベル */
	ushort	resctTime;				/* 復帰時間 */
	ushort	changeTimeIncorDes;		/* 出力増減変化時間 */
	ushort	volRsInhbValue;			/* 電圧上昇抑制値 */
	ushort	volRsInhbattLimit;		/* 電圧上昇抑制動作待機時限 */
	ushort	cotPowerControl;		/* 力率一定制御 */
	ushort	freFeedbackGain;		/* 周波数フィードバックゲイン */
	ushort	freLevAllow;			/* 並列時許容周波数レベル */
}EinvSpdataSetT;

/* 登録情報取得用構造体 (60)*/
typedef struct {
	uchar	needTestRun;							/* V:試運転要否 */
	schar 	sysModeName[EINV_SYS_MODEL_NAME_LEN];	/* システム機種名 */
	schar	sysMadeNum[EINV_SYSMADENUM_LEN];		/* システム製造番号 */
	EinvLongDateStr_t		installationDate;		/* 設置日 */
	uchar	sysConf;					/* システム構成 */
	ushort	btRatedCapacity;			/* 蓄電池定格容量[0.1kWh] */
	ushort	btRatedVolt;				/* 蓄電池定格電圧[0.1V] */
	ulong	chargePwrValueMax;			/* 蓄電池最大充電電力値[0.01kW] */
	ulong	dischargePwrValueMax;		/* 蓄電池最大放電電力値[0.01kW] */
	ulong	chargePowerValueMaxCut;		/* 蓄電池最大充電電力値_自立時[0.01kW] */
	ulong	dischargePowerValueMaxCut;	/* 蓄電池最大放電電力値_自立時[0.01kW] */
}EinvRegInfoGet_t;

/* 特殊設定値情報構造体 (6D)*/
typedef struct {
	ulong				changeFlag;						/* 変更フラグ */
	EinvLongDateStr_t	installationDate;				/* 設置日 */
	schar				sysMadeNum[EINV_SYSMADENUM_LEN];/* システム製造番号 */
	uchar				sysConf;						/* システム構成 */
	uchar				revCtConctPositionSet;			/* 逆潮CT接続位置設定 */
}EinvUniqueInfoSet_t;

/* V2H電力情報取得用構造体 */
typedef struct {
	uchar	v2hOpePriority;			/*車両操作優先*/
	uchar	v2hMainUnitOpe;			/* 車両充放電器本体操作 */
	uchar	v2hEmgDischarge;		/* 非常時放電動作 */
	uchar	v2hEmgDischargePriority;/* 非常時放電優先設定 */
	uchar	v2hEmgChargePriority;	/* 非常時充電優先設定 */
	ushort 	v2hEmgDischrLowerLimit;	/* 非常時車両放電下限値 <- 非常時放電下限値設定(車両)として流用する */
	uchar	v2hConnectorLock;		/* コネクタロック／解除 */
	ushort	v2hChargeUpperLimit;	/* 車両充電上限値設定 */
	uchar	v2hDischargeSettingFlag;/* 車両放電有無 */
	uchar	v2hDischargePriority;	/* 放電優先設定 */
	uchar	v2hChargePriority;		/* 充電優先設定 */
	uchar 	v2hChargeStyle;			/* v2h充電方式(ENL) */
	uchar 	v2hDischargeStyle;		/* v2h放電方式(ENL) */
	ushort	v2hChargePwrSetValue;	/* v2h充電電力設定値(ENL) */
	ushort	v2hDischargePwrSetValue;/* v2h放電電力設定値(ENL) */
	ulong	v2hChargePwrSetValueMax;/* ユーザ設定の車両最大充電電力設定値[kW] */
	ushort 	v2hACChargeQuatySetValue;/* v2h AC充電量設定値 */
	ushort 	v2hACDiscQuatySetValue;	/* v2h AC放電量設定値 */	
}EinvV2HSetvalueGet_t;

/* V2H電力情報設定用構造体 */
typedef struct {
	ulong	changeFlag;				/* 変更フラグ */
	uchar	v2hOpePriority;			/*車両操作優先*/
	uchar	v2hMainUnitOpe;			/* 車両充放電器本体操作 */
	uchar	v2hEmgDischarge;		/* 非常時放電動作 */
	uchar	v2hEmgDischargePriority;/* 非常時放電優先設定 */
	uchar	v2hEmgChargePriority;	/* 非常時充電優先設定 */
	ushort 	v2hEmgDischrLowerLimit;	/* 非常時車両放電下限値 <- 非常時放電下限値設定(車両)として流用する */
	uchar	v2hConnectorLock;		/* コネクタロック／解除 */
	ushort	v2hChargeUpperLimit;	/* 車両充電上限値設定 */
	uchar	v2hDischargeSettingFlag;/* 車両放電有無 */
	uchar	v2hDischargePriority;	/* 放電優先設定 */
	uchar	v2hChargePriority;		/* 充電優先設定 */	
	uchar 	v2hChargeStyle;			/* v2h充電方式(ENL) */
	uchar 	v2hDischargeStyle;		/* v2h放電方式(ENL) */
	ushort	v2hChargePwrSetValue;	/* v2h充電電力設定値(ENL) */
	ushort	v2hDischargePwrSetValue;/* v2h放電電力設定値(ENL) */
	ulong	v2hChargePwrSetValueMax;/* ユーザ設定の車両最大充電電力設定値[kW] */
	ushort 	v2hACChargeQuatySetValue;/* v2h AC充電量設定値 */
	ushort 	v2hACDiscQuatySetValue;	/* v2h AC放電量設定値 */
}EinvV2HSetvalueSet_t;

/* 太陽光発電情報取得構造体 */
typedef struct {
	uchar	fitType;					/* FIT契約タイプ */
	uchar	selfConsumptionType;		/* 自家消費タイプ */
	ushort	facilityCertifiedCapacity;	/* 設備認定容量[W] */
} EinvPvinfoGet_t;

/* 太陽光発電情報設定構造体 */
typedef struct {
	uchar	changeFlg;					/* 変更フラグ */
	uchar	fitType;					/* FIT契約タイプ */
	uchar	selfConsumptionType;		/* 自家消費タイプ */
	ushort	facilityCertifiedCapacity;	/* 設備認定容量[W] */
} EinvPvinfoSet_t;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
/* CMD81:停電の動作 */
typedef enum _pwr_cut_operation
{
	OPERATION_PWRCUT,
	OPERATION_PWRON
} PowerCutOperation_t;

/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern void EINV_Init(void);
extern void EINV_MsgProc(const FrmMsgHead_t *msg);
extern void EINV_Fin(void);
extern slong EINV_Start(ulong fact);
extern slong EINV_Stop(void);

/* 動作情報(81) */
extern slong EINV_GetOpeSttsInfo(EinvOpesttsInfo_t *opeSttsInfo);

/* CMD41表示用シグナル処理値 */
extern slong EINV_GetCalcAvgValue(EinvCalcAvg_t *calcAvgValueGet);

/* 運転モード蓄電池電力 */
extern slong EINV_GetOpeMode(EinvOpemodeGet_t *opeModeGet);
extern slong EINV_SetOpeMode(const EinvOpemodeSet_t *opeModeSet);
extern slong EINV_ReqOpeMode(void);

/* 非常時運転モード */
extern slong EINV_GetEmergencyMode(EinvOpemodeGet_t *opeModeGet);
extern slong EINV_SetEmergencyMode(const EinvOpemodeSet_t *opeModeSet);
extern slong EINV_ReqEmergencyMode(void);

/* 整定値 */
extern slong EINV_GetSpData(EinvSpdataGet_t *spDataGet);
extern slong EINV_SetSpData(const EinvSpdataSetT *spDataSet);
extern slong EINV_ReqSpData(void);

/* 登録情報(60) */
extern slong EINV_GetRegInfo(EinvRegInfoGet_t *regInfoGet);
extern slong EINV_ReqRegInfo(void);

/* 特殊設定値情報(6D) */
extern slong EINV_SetUniqueInfo(const EinvUniqueInfoSet_t *uniqueInfoSet);

/* V2H電力情報(75) */
extern slong EINV_GetV2hValue(EinvV2HSetvalueGet_t *v2hValueGet);
extern slong EINV_SetV2hValue(const EinvV2HSetvalueSet_t *v2hValueSet);
extern slong EINV_ReqV2hValue(void);

/* 太陽光発電情報 */
extern slong EINV_GetPvInfo(EinvPvinfoGet_t *pvInfoGet);
extern slong EINV_SetPvInfo(const EinvPvinfoSet_t *pvInfoSet);
extern slong EINV_ReqPvInfo(void);

/* 試運転開始/停止指示 */
extern slong EINV_SetTestRunStartStop(uchar StartStop);
/* 試運転可否取得 */
extern uchar EINV_GetTestRunAvailable();
#endif /*__EINV_G_H__*/
