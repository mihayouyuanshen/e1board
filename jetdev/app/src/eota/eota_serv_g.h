/****************************************************************************/
/*	Description		:FW更新機能 モジュール間処理ヘッダファイル				*/
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
#ifndef	__EOTA_SERV_G_H__
#define	__EOTA_SERV_G_H__

#include <stdio.h>

#include "typedef.h"
#include "task_g.h"
#include "eota_g.h"

/*--------------------------------------------------------------------------*/
/* マクロ定義																*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* 非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* 定数宣言、定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* 変数宣言、定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* プロトタイプ宣言															*/
/*--------------------------------------------------------------------------*/
/* ダウンロード状態送信(スマホへ) */
extern void EOTA_SetDownLoadState(EOTA_DOWN_LOAD_STATE_T DownLoadState);
/* 進捗率送信(スマホへ) */
extern void EOTA_SetProgressRate(uchar ProgressRate);
/* 更新状態送信(スマホへ) */
extern void EOTA_SetUpdState(EOTA_UPDATE_STATE_T UpdState);
/* スマホ用情報送信(スマホへ) */
extern void EOTA_SetUpdInfoPhone(EOTA_PHONE_UPD_INFO_T * UpdInfo);

/* 見守りサーバ情報送信(見守りサーバへ) */
extern void EOTA_SetUpdInfoSub(EOTA_SUB_UPD_INFO_T * UpdInfo);

/* LED動作設定 */
extern void EOTA_Set_Led(EOTA_LED_UPD_INFO_T Led);

/* CMD65:運転/停止=停止(INVへ) */
extern void EOTA_SendCMD65Stop();
/* CMD81 の定期送信停止(INVへ) */
extern void EOTA_SendCMD81Stop();
/* CMD11 送信(INVへ) */
extern void EOTA_SendInvCMD11(EOTA_CMD11_REQ_INFO_T * Message);
/* CMD60 送信(INVへ) */
extern void EOTA_SendInvCMD60( );

#endif	/* __EOTA_SERV_G_H__ */
