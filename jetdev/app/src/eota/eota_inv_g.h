/****************************************************************************/
/*	Description		:INV・PV・BDC・J.B.転送ヘッダファイル					*/
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
#ifndef	__EOTA_INV_G_H__
#define	__EOTA_INV_G_H__

#include "typedef.h"

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
/* 一回応答タイムアウト */
extern void EOTA_TimeOutRes();
/* 通信タイムアウト */
extern void EOTA_TimeOutInv();
/* メッセージ送信処理 */
extern uchar EOTA_SendInvMessage();
/* メッセージ受信処理 */
extern uchar EOTA_RevInvMessage(EOTA_CMD11_RES_INFO_T * Message);

#endif	/* __EOTA_INV_G_H__ */
