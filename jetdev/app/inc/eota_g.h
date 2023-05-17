/****************************************************************************/
/*	Description		:FW更新機能 公開ヘッダファイル							*/
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
#ifndef __EOTA_H__
#define __EOTA_H__

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "typedef.h"
#include "frm_g.h"

/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
/* リモコン起動時コマンド実行結果 */
#define	EOTA_OK						((uchar)0u)
											/* 正常終了						*/
#define	EOTA_NG						((schar)-99)
											/* 予期せぬエラー				*/
#define	EOTA_ERR_SYSCALL			((schar)-1)
											/* システムコールエラー			*/
#define	EOTA_ERR_PARAM				((schar)-2)
											/* パラメータエラー				*/
#define	EOTA_PROCESS_ING			((schar)-3)
											/* FW更新実施中エラー			*/
#define	EOTA_UPD_STOP				((schar)-5)
											/* FW更新中止					*/

#define	EOTA_UPD_FILE_INFO_SIZE		((ushort)1024)
											/* FW 更新ファイル情報サイズ	*/

/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
/* 起動モード */
typedef enum _eota_start_mode_t
{
	EOTA_START_MODE_USB = 0,				/* 0:USB 経由					*/
	EOTA_START_MODE_SUB,					/* 1:見守りサーバー経由			*/
	EOTA_START_MODE_PHONE,					/* 2:スマホ経由					*/
	EOTA_START_MODE_SW						/* 3:タクト SW経由				*/
} EOTA_START_MODE_T;

/* 更新 Mode */
typedef enum _eota_upd_mode_e
{
	EOTA_UPD_MODE_U = 0,			 		/* 0:U							*/
	EOTA_UPD_MODE_F,						/* 1:F							*/
	EOTA_UPD_MODE_C,				 		/* 2:C							*/
} EOTA_UPD_MODE_E;

/* 更新時点 */
typedef enum _eota_update_time_t
{
	EOTA_UPD_AT_ONCE = 0,				 	/* 0:今すぐ更新					*/
	EOTA_UPD_RESERVATION,				 	/* 1:更新を予約					*/
	EOTA_UPD_LATER,						 	/* 2:後で更新					*/
} EOTA_UPDATE_TIME_T;

/* FW ダウンロード処理状態(スマホ用) */
typedef enum _eota_down_load_state_t
{
	EOTA_DL_STATE_START = 0,				/* 0:正常開始					*/
	EOTA_DL_STATE_END,						/* 1:完了						*/
	EOTA_DL_STATE_BUSY,						/* 2:busy 状態					*/
	EOTA_DL_STATE_NET_DL,					/* 3:ネットワーク DL中			*/

	EOTA_DL_STATE_USB_NO,					/* 4:USB なし					*/
	EOTA_DL_STATE_ERR,						/* 5:エラー						*/
	EOTA_DL_STATE_UPD_NO,					/* 6:更新対象なし				*/
	EOTA_DL_FW_FILE_NO,						/* 7:FW 対象なし				*/
} EOTA_DOWN_LOAD_STATE_T;

/* FW 更新処理状態(スマホ用) */
typedef enum _eota_update_state_t
{
	EOTA_UPD_STATE_USB_START = 0,		 	/* 0:開始						*/
	EOTA_UPD_STATE_END,					 	/* 1:完了						*/
	EOTA_UPD_STATE_BUSY,				 	/* 2:busy 状態					*/
	EOTA_UPD_STATE_ERR,				 	 	/* 3:エラー						*/
} EOTA_UPDATE_STATE_T;

/* update_setup.txtチェックエラー(見守りサーバ用) */
typedef enum _eota_update_setup_err_t
{
	EOTA_DL_FW_FILE_NO_SUB = 1,				/* 1:FW 対象なし				*/
	EOTA_DL_ERR_SUB = 1,					/* 2:エラー						*/
} EOTA_UPDATE_SETUP_ERR_T;

/* FW更新情報(見守りサーバ用) */
typedef enum _eota_update_state_sub_t
{
	EOTA_UPDATE_FIRM_START,					/* 0:FW更新開始					*/
	EOTA_CHECK_UPDATE_SETUP,				/* 1:update_setup.txtチェック	*/
	EOTA_CB_SAME_VERSION,					/* 2:CB same Version			*/
	EOTA_INV_SAME_VERSION,					/* 3:INV same Version			*/
	EOTA_PV_SAME_VERSION,					/* 4:PV same Version			*/
	EOTA_JB_SAME_VERSION,					/* 5:J.B. same Version			*/
	EOTA_BCD_SAME_VERSION,					/* 6:BCD same Version			*/

	EOTA_ECP_UPD_CANCEL_OK,					/* 7:ECP Update Cancel:OK		*/
	EOTA_ECP_UPD_CANCEL_ERR,				/* 8:ECP Update Cancel:ERROR	*/
	EOTA_ECP_UPD_CANCEL_CANCEL,				/* 9:ECP Update Cancel:CANCEL	*/

	EOTA_USER_UPD_WAITING,					/* 10:User Update Waiting		*/
	EOTA_USER_UPD_START,					/* 11:User Update Start			*/
	EOTA_USER_UPD_CANCEL,					/* 12:User Update Cancel		*/
	EOTA_AUTO_UPD_START,					/* 13:Auto Update Start			*/
	EOTA_AUTO_UPD_CANCEL,					/* 14:Auto Update Cancel		*/
	EOTA_BAD_UPDATE_CONDITION,				/* 15:Bad Update Condition 		*/

	EOTA_WAIT_12H,							/* 16:Wait１２H					*/

	EOTA_CB_UPDATE_SUCCES,					/* 17:CB Update:SUCCESS			*/
	EOTA_CB_UPDATE_FAIL,					/* 18:CB Update:FAIL			*/

	EOTA_INV_UPDATE_SUCCES,					/* 19:INV Update:SUCCESS		*/
	EOTA_INV_UPDATE_FAIL,					/* 20:INV Update:FAIL			*/

	EOTA_PV_UPDATE_SUCCES,					/* 21:PV Update:SUCCESS			*/
	EOTA_PV_UPDATE_FAIL,					/* 22:PV Update:FAIL			*/

	EOTA_JB_UPDATE_SUCCES,					/* 23:JB Update:SUCCESS			*/
	EOTA_JB_UPDATE_FAIL,					/* 24:JB Update:FAIL			*/

	EOTA_BDC_UPDATE_SUCCES,					/* 25:BDC Update:SUCCESS		*/
	EOTA_BDC_UPDATE_FAIL,					/* 26:BDC Update:FAIL			*/

	EOTA_UPDATE_FIRM_END,					/* 27:updateFirmware End(xx)	*/
} EOTA_UPDATE_STATE_SUB_T;

/* FW更新LED動作(LED用) */
typedef enum _eota_led_upd_info_t
{
	EOTA_DL_ANA_SW = 0,						/* 0:ダウンロード・解析			*/
	EOTA_FW_UPD_STATE,						/* 1:FW 更新状態				*/
	EOTA_ALL_FW_SUCESS,						/* 2:全ての FW 更新成功			*/
	EOTA_BOARD_UPD_FAIL,					/* 3:通信ボード更新失敗			*/
	EOTA_INV_UPD_FAIL,						/* 4:INV マイコン更新失敗		*/
	EOTA_PV_UPD_FAIL,						/* 5:INV マイコン更新失敗		*/
	EOTA_BDC_UPD_FAIL,						/* 6:INV マイコン更新失敗		*/
	EOTA_JB_UPD_FAIL,						/* 7:INV マイコン更新失敗		*/
	EOTA_OTHER_FAIL,						/* 8:INV マイコン更新失敗		*/
} EOTA_LED_UPD_INFO_T;

/* FW更新情報(スマホ用) */
typedef struct _eota_phone_upd_info_t
{
	ushort	UpdNo;							/* FW 更新 No					*/
	uchar	UpdMode;						/* FW 更新 Mode					*/
	schar	UpdFileInfo[EOTA_UPD_FILE_INFO_SIZE];
											/* FW 更新ファイル情報			*/
} EOTA_PHONE_UPD_INFO_T;

/* FW更新情報(見守りサーバ用) */
typedef struct _eota_sub_upd_info_t
{
	EOTA_UPDATE_STATE_SUB_T	UpdState;		/* 更新状態						*/
	uchar	Err;							/* エラー情報					*/
	ushort	UpdNo;							/* FW 更新 No					*/
} EOTA_SUB_UPD_INFO_T;

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
/* FW更新開始 */
extern slong EOTA_Start(uchar tmp_bootState);
/* FW更新初期化 */
extern void EOTA_Init(void);
/* FW更新プロセス処理 */
extern void EOTA_Proc(void);
/* FW更新終了 */
extern void EOTA_Fin(void);
/* FW更新停止 */
extern slong EOTA_Stop(void);
/* FW更新受信メッセージ処理 */
extern void EOTA_MsgProc(FrmMsgHead_t const * Msg);

/* FW更新処理開始 */
/* 見守り・スマホアプリ・タクト SWから */
/* 戻り値：(EOTA_OK 正常終了/EOTA_PROCESS_ING FW更新実施中) */
extern schar ETOA_FWStart(EOTA_START_MODE_T fw_start_mode);

/* ユーザ更新時点(スマホアプリから) */
extern void ETOA_FWUseTime(EOTA_UPDATE_TIME_T UpdPoint, schar * Time);
/* FW更新処理中止(スマホアプリから)  */
extern void ETOA_FWStop();

/* INVから */
/* CMD11 受信 */
extern void ETOA_DMD11Response();
/* CMD60 受信 */
extern void ETOA_DMD60Response();
/* CMD64 受信 */
extern void ETOA_DMD64Response(uchar startOrStop);				/* startOrStop 運転／停止 "0":停止, "1":運転 */
/* CMD65 受信 */
extern void ETOA_DMD65Response(char ret);						/* ret 0:OK 0以外：NG*/


#endif /*__EOTA_H__*/