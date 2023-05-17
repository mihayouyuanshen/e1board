/******************************************************************************/
/*    Description    :時刻管理ミドル                                           */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC Gu.CM                                              */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/04/06                                              */
/*----------------------------------------------------------------------------*/
/*    Module Name    :                                                        */
/*----------------------------------------------------------------------------*/
/*    Note           :                                                        */
/*----------------------------------------------------------------------------*/
/*    Copyright      :(c) 2020 NICHICON CORPORATION                           */
/******************************************************************************/
#ifndef __ETIM_RTC_H__
#define __ETIM_RTC_H__

#include "typedef.h"
/*----------------------------------------------------------------------------*/
/* マクロ定義                                                                  */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開型宣言、定義                                                          */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 定数宣言、定義                                                              */
/*--------------------------------------- ------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 変数宣言、定義                                                              */
/*----------------------------------------------------------------------------*/
typedef struct etimRtcReg {
	union {
		uchar	byte;
		struct {
			uchar	sec_l:4;
			uchar	sec_h:3;
			uchar	  dmy:1;
		} bit;
	} d00;

	union {
		uchar	byte;
		struct {
			uchar	min_l:4;
			uchar	min_h:3;
			uchar	  dmy:1;
		} bit;
	} d01;

	union {
		uchar	byte;
		struct {
			uchar  hour_l:4;
			uchar  hour_h:2;
			uchar	  dmy:2;
		} bit;
	} d02;

	union {
		uchar	byte;
		struct {
			uchar	 wday:3;
			uchar	  dmy:5;
		} bit;
	} d03;

	union {
		uchar	byte;
		struct {
			uchar  mday_l:4;
			uchar  mday_h:2;
			uchar	  dmy:2;
		} bit;
	} d04;

	union {
		uchar	byte;
		struct {
			uchar	mon_l:4;
			uchar	mon_h:1;
			uchar	  dmy:3;
		} bit;
	} d05;

	union {
		uchar	byte;
		struct {
			uchar  year_l:4;
			uchar  year_h:4;
		} bit;
	} d06;

	union {
		uchar	byte;
		struct {
			uchar	    f:7;
			uchar	  dev:1;
		} bit;
	} d07;

	union {
		uchar	byte;
		struct {
			uchar	 wm_l:4;
			uchar	 wm_h:3;
			uchar	  dmy:1;
		} bit;
	} d08;

	union {
		uchar	byte;
		struct {
			uchar    wh_l:4;
			uchar    wh_h:2;
			uchar	  dmy:2;
		} bit;
	} d09;

	union {
		uchar	byte;
		struct {
			uchar  ww_sun:1;
			uchar  ww_mon:1;
			uchar  ww_tue:1;
			uchar  ww_wed:1;
			uchar  ww_thr:1;
			uchar  ww_fri:1;
			uchar  ww_sat:1;
			uchar	  dmy:1;
		} bit;
	} d0A;

	union {
		uchar	byte;
		struct {
			uchar    dm_l:4;
			uchar    dm_h:3;
			uchar	  dmy:1;
		} bit;
	} d0B;

	union {
		uchar	byte;
		struct {
			uchar	 dh_l:4;
			uchar	 dh_h:2;
			uchar	   ad:2;
		} bit;
	} d0C;

	union {
		uchar	byte;
		struct {
			uchar	  ram:8;
		} bit;
	} d0D;

	union {
		uchar	byte;
		struct {
			uchar	   ct:3;
			uchar	 test:1;
			uchar	clen2:1;
			uchar	  t24:1;
			uchar	 dale:1;
			uchar	 wale:1;
		} bit;
	} d0E;

	union {
		uchar	byte;
		struct {
			uchar	 dafg:1;
			uchar	 wafg:1;
			uchar	 ctfg:1;
			uchar	clen1:1;
			uchar	  pon:1;
			uchar	 xstp:1;
			uchar	 vdet:1;
			uchar	  eco:1;
		} bit;
	} d0F;
} EtimRtcReg_t;



/*----------------------------------------------------------------------------*/
/* プロトタイプ宣言                                                            */
/*----------------------------------------------------------------------------*/
sint ETIM_GetRtc(struct tm *ptm);
sint ETIM_SetRtc(void);

#endif /*__ETIM_RTC_H__*/

