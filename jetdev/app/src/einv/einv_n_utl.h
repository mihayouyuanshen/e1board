/****************************************************************************/
/*	Description		:INV通信ミドル ユーティリティ							*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Dai.P												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/23												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:INV通信												*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EINV_N_UTL_H__
#define __EINV_N_UTL_H__


/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define EINV_SYSCONF_TABLE_SIZE				((uchar)8)

#define EINV_TIME_HOUR_MIN					((uchar)0u)
#define EINV_TIME_HOUR_MAX					((uchar)23u)
#define EINV_TIME_MIN_MIN					((uchar)0u)
#define EINV_TIME_MIN_MAX					((uchar)59u)
#define EINV_TIME_SECOND_MIN				((uchar)0u)
#define EINV_TIME_SECOND_MAX				((uchar)59u)
#define EINV_TIME_DAY_MIN					((uchar)1u)
#define EINV_TIME_DAY_MAX					((uchar)31u)
#define EINV_TIME_MON_MIN					((uchar)1u)
#define EINV_TIME_MON_MAX					((uchar)12u)
#define EINV_TIME_YEAR_MIN					((uchar)2000u)
#define EINV_TIME_YEAR_MAX					((uchar)2099u)

#define EINV_RUNMODE_LEN					((uchar)8u)
#define EINV_SYSTEMSTRUCTURE_LEN			((uchar)8u)

#define EINV_OPRUNMODE_LEN					((uchar)8u)

/*OPリモコンから設定可能な運転モード*/
#define EINV_OP_SELL						((uchar)'1')
#define EINV_OP_GREEN						((uchar)'2')
#define EINV_OP_BATTCHARGING				((uchar)'4')
#define EINV_OP_BATTDISCHARGE				((uchar)'5')
#define EINV_OP_CARCHARGING					((uchar)'B')
#define EINV_OP_CARDISCHARGE				((uchar)'C')
#define EINV_OP_CARBATTDISCHARGE			((uchar)'N')
#define EINV_OP_IDLE						((uchar)'6')


/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
/* システム構成関連付けテーブル */
typedef struct 
{
	uchar	cmdSysConf;
	ushort	bitSysconf;
} EinvSysconfTable_t;


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/


#endif /*__EINV_N_UTL_H__*/
