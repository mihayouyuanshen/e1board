/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Cao.HZ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/25												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EEEP_SERV_G_H__
#define __EEEP_SERV_G_H__
/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "eeep_cmn_g.h"
#include "typedef.h"

#define EEEP_OFS_BOOTFAILED_REASON (uchar)(0x06u)			/* 起動失敗要因 */
#define EEEP_OFS_MOUNT_FAILED_TIMES (uchar)(0x0Cu)			/* マウント失敗回数 */
#define EEEP_OFS_BANK0_FAILED_TIMES (uchar)(0X0Du)			/* BANK0 累積失敗回数 */
#define EEEP_OFS_BANK1_FAILED_TIMES (uchar)(0X0Eu)			/* BANK1 累積失敗回数 */
#define EEEP_OFS_MODEL_INFO (uchar)(0x20u)					/* 通信ボード機種名 */
#define EEEP_OFS_MODEL_SERIES (uchar)(0x30u)					/* 通信ボード製造番号 */
#define EEEP_OFS_FW_BANK0_VER (uchar)(0X40u)					/* 通信ボード FW(BANK0) */
#define EEEP_OFS_FW_BANK1_VER (uchar)(0X50u)					/* 通信ボード FW(BANK1) */
#define EEEP_OFS_MAC_ADDR_WIRE (uchar)(0x60u)				/* MAC アドレス(有線) */
#define EEEP_OFS_MAC_ADDR_WIRELESS (uchar)(0x66u)			/* MAC アドレス(無線) */
#define EEEP_OFS_CRC (uchar)(0x7Cu)							/* チェックサムオフセット */

#define EEEP_2B_NG (slong)(3)
#define EEEP_B1_OK (slong)(2)
#define EEEP_B0_OK (slong)(1)
#define EEEP_R_OK (slong)(0)								/* 正常終了 */
#define EEEP_R_EPRM (slong)(-1)								/* パラメータ異常 */
#define EEEP_R_ESYS (slong)(-2)								/* システム異常 */
#define EEEP_R_EFAIL (slong)(-3)							/* 書き込み/取得失敗 */

#define EEEP_ROM_SZ (ushort)(256u)							/* EEPROMサイズ(byte) */
#define EEEP_BANK_NUM (ushort)(2u)							/* Bank数 */
#define EEEP_BANK_SZ (ushort)(128u)							/* Bankサイズ(byte) */
#define EEEP_CRC_LEN (ushort)(4u)							/* CRCデータ長(byte) */
#define EEEP_ALL_DATA_LEN (ushort)(124u)						/* CRC算出対象データ長(byte) */
#define EEEP_BYTE_LEN (ushort)(1u)
#define EEEP_BOARD_LEN (ushort)(16u)
#define EEEP_MAC_LEN (ushort)(6u)
#define EEEP_FW_LEN (ushort)(9u)
/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
extern EEEP_WrCmdTbl_t g_eeep_wrcmd_tbl[EEEP_WRITE_OP_MAX];

/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern void eeep_Init(void);
extern slong eeep_WriteData(uchar offset, uchar* data, ushort data_len);
extern slong eeep_ReadData(uchar offset, uchar* data, ushort data_len);
#endif /*__EEEP_SERV_G_H__*/
