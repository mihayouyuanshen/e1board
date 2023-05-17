/****************************************************************************/
/*	Description		:INV通信ミドル ユーティリティ関数						*/
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

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#include "typedef.h"
#include "task_g.h"
#include "frm_g.h"
#include "dlog_g.h"
#include "common_g.h"

#include "einv_g.h"
#include "einv_msg_g.h"
#include "einv_ram_g.h"
#include "einv_utl_g.h"
#include "einv_que_g.h"
#include "einv_n_g.h"
#include "einv_n_comm_g.h"
#include "einv_n_ram_g.h"
#include "einv_n_utl_g.h"
#include "einv_n_utl.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
/** システム構成関連付けテーブル **/
const EinvSysconfTable_t s_EinvSysconfTable[EINV_SYSCONF_TABLE_SIZE] = {
		/*	INV文字,			バイナリ設定値	*/
		{EINV_SYS_STRUCT_0,		EINV_SYS_CON_TR_PBV},	/* 太陽光＋蓄電池＋V2H */
		{EINV_SYS_STRUCT_1,		EINV_SYS_CON_HY_PB},	/* 太陽光＋蓄電池 */
		{EINV_SYS_STRUCT_2,		EINV_SYS_CON_HY_PV},	/* 太陽光＋V2H */
		{EINV_SYS_STRUCT_3,		EINV_SYS_CON_HY_BV},	/* 蓄電池＋V2H */
		{EINV_SYS_STRUCT_4,		EINV_SYS_CON_SI_B},		/* 蓄電池 */
		{EINV_SYS_STRUCT_5,		EINV_SYS_CON_SI_V},		/* V2H */
		{EINV_SYS_STRUCT_6,		EINV_SYS_CON_P},		/* PV */
		{EINV_SYS_STRUCT_7,		EINV_SYS_CON_UNDEF}		/* システム構成未設定 */
};

/*表 2-15 システム構成と設定可能な運転モード*/
const EinvSupport_t s_einvServerSetRunmodeCheckTbl[EINV_RUNMODE_LEN][EINV_SYSTEMSTRUCTURE_LEN] =
{
	{EINV_SUPPORT,	EINV_SUPPORT,		EINV_SUPPORT		,EINV_SUPPORT		,EINV_SUPPORT		,EINV_SUPPORT		,EINV_SUPPORT		,EINV_NOTSUPPORT},
	{EINV_SUPPORT,	EINV_SUPPORT,		EINV_SUPPORT		,EINV_SUPPORT		,EINV_SUPPORT		,EINV_SUPPORT		,EINV_SUPPORT		,EINV_NOTSUPPORT},
	{EINV_SUPPORT,	EINV_SUPPORT,		EINV_NOTSUPPORT		,EINV_NOTSUPPORT	,EINV_SUPPORT		,EINV_NOTSUPPORT	,EINV_NOTSUPPORT	,EINV_NOTSUPPORT},
	{EINV_SUPPORT,	EINV_SUPPORT,		EINV_NOTSUPPORT		,EINV_NOTSUPPORT	,EINV_SUPPORT		,EINV_NOTSUPPORT	,EINV_NOTSUPPORT	,EINV_NOTSUPPORT},
	{EINV_SUPPORT,	EINV_NOTSUPPORT,	EINV_SUPPORT		,EINV_SUPPORT		,EINV_NOTSUPPORT	,EINV_SUPPORT		,EINV_NOTSUPPORT	,EINV_NOTSUPPORT},
	{EINV_SUPPORT,	EINV_NOTSUPPORT,	EINV_SUPPORT		,EINV_SUPPORT		,EINV_NOTSUPPORT	,EINV_SUPPORT		,EINV_NOTSUPPORT	,EINV_NOTSUPPORT},
	{EINV_SUPPORT,	EINV_SUPPORT,		EINV_NOTSUPPORT		,EINV_NOTSUPPORT	,EINV_NOTSUPPORT	,EINV_NOTSUPPORT	,EINV_NOTSUPPORT	,EINV_NOTSUPPORT},
	{EINV_SUPPORT,	EINV_SUPPORT,		EINV_SUPPORT		,EINV_SUPPORT		,EINV_SUPPORT		,EINV_SUPPORT		,EINV_SUPPORT		,EINV_SUPPORT   }
};

const uchar s_einvConvertOPRunmodeToCtlRunmodeTbl[EINV_OPRUNMODE_LEN] =
{
	EINV_COMMU_BOARD_MANAGE_MODE_0,
	EINV_COMMU_BOARD_MANAGE_MODE_1,
	EINV_COMMU_BOARD_MANAGE_MODE_2,
	EINV_COMMU_BOARD_MANAGE_MODE_3,
	EINV_COMMU_BOARD_MANAGE_MODE_4,
	EINV_COMMU_BOARD_MANAGE_MODE_5,
	EINV_COMMU_BOARD_MANAGE_MODE_6,
	EINV_COMMU_BOARD_MANAGE_MODE_7
};

const ushort EINV_SocConvtTable[105][2] = {
/*		{rSOC,	uSOC},		バイナリ設定値	*/
		{100,	100},
		{99,	100},
		{98,	100},
		{97,	100},
		{96,	100},
		{95,	99},
		{94,	98},
		{93,	97},
		{92,	96},
		{91,	95},
		{91,	94},
		{90,	93},
		{89,	92},
		{88,	91},
		{87,	90},
		{86,	89},
		{85,	88},
		{84,	87},
		{83,	86},
		{82,	85},
		{82,	84},
		{81,	83},
		{80,	82},
		{79,	81},
		{78,	80},
		{77,	79},
		{76,	78},
		{75,	77},
		{74,	76},
		{73,	75},
		{72,	74},
		{71,	73},
		{70,	72},
		{69,	71},
		{68,	70},
		{67,	69},
		{66,	68},
		{65,	67},
		{64,	66},
		{63,	65},
		{62,	64},
		{61,	63},
		{60,	62},
		{59,	61},
		{58,	60},
		{57,	59},
		{56,	58},
		{55,	57},
		{54,	56},
		{53,	55},
		{53,	54},
		{52,	53},
		{51,	52},
		{50,	51},
		{49,	50},
		{48,	49},
		{47,	48},
		{47,	47},
		{46,	46},
		{45,	45},
		{44,	44},
		{43,	43},
		{42,	42},
		{41,	41},
		{40,	40},
		{39,	39},
		{38,	38},
		{37,	37},
		{36,	36},
		{35,	35},
		{34,	34},
		{33,	33},
		{32,	32},
		{31,	31},
		{30,	30},
		{29,	29},
		{28,	28},
		{27,	27},
		{26,	26},
		{25,	25},
		{24,	24},
		{23,	23},
		{22,	22},
		{21,	21},
		{20,	20},
		{19,	19},
		{18,	18},
		{17,	17},
		{16,	16},
		{15,	15},
		{14,	14},
		{13,	13},
		{12,	12},
		{11,	11},
		{10,	10},
		{9,		9},
		{8,		8},
		{7,		7},
		{6,		6},
		{5,		5},
		{4,		4},
		{3,		3},
		{2,		2},
		{1,		1},
		{0,		0}
};

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description	:送信完了確認処理											*/
/*--------------------------------------------------------------------------*/
/*	param		:fd		:ファイルディスクリプタ								*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	応答コード											*/
/*						EINV_OK					正常終了					*/
/*						EINV_ERR_SYSCALL		システムコールエラー		*/
/*						EINV_ERR_FAIL			通信異常					*/
/*--------------------------------------------------------------------------*/
/*	detail		:送信完了確認処理を行う。									*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/30 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_n_CheckComplete( sint fd )
{
	ulong	cnt = 0UL;
	slong	ret = 0L;
	slong	funcRet = EINV_OK ;
	slong	len1 = 0L;
	slong	len2 = 0L;
	bool_t	breakFlag = FALSE;

	DEBUG_TRCIN();

	while(!breakFlag)
	{
		/* 出力バッファにあるバイト数確認 */
		ret = ioctl( fd, TIOCOUTQ, &len1 );
		if( ret != 0 )
		{
			/* システムコールエラー */
			DLOG_Error( "ioctl() error %ld\n", ret );
			funcRet = EINV_ERR_SYSCALL ;
			breakFlag = TRUE;
		}
		else if( len1 == 0 )
		{
			/* 送信完了 */
			funcRet = EINV_OK ;
			breakFlag = TRUE;
		}
		else if( len1 == len2 )
		{
			cnt++ ;
			if( cnt >= EINV_COMM_RETRY_WRITE ){
				/* 100msec間送信バッファから送信出来ていない */
				DLOG_Error( "ioctl() retryCntOver \n" );
				funcRet = EINV_ERR_FAIL ;
				breakFlag = TRUE;
			}
		}
		else
		{
			/* リトライ数初期化 */
			cnt = 0 ;
		}
		/* 前回値保存 */
		len2 = len1 ;

		/* 1msecスリープ */
		EINV_Usleep(EINV_COMM_RETRY_INTERVAL);
	}

	DEBUG_TRCOUT();

	return funcRet ;
}

/*==========================================================================*/
/*	Description	:システム構成変換（INV定義文字 ⇒ ビットアサイン）処理		*/
/*--------------------------------------------------------------------------*/
/*	param		:cmdSysConf		:システム構成(INV定義文字)					*/
/*				:bitSysconf		:システム構成(ビットアサイン)				*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	応答コード											*/
/*						EINV_OK					正常終了					*/
/*						EINV_ERR_PARAM			パラメータエラー			*/
/*						EINV_ERR_FAIL			通信異常					*/
/*--------------------------------------------------------------------------*/
/*	detail		:システム構成変換処理を行う。								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/24 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_GetSysConfBitAssign(uchar cmdSysConf, ushort* bitSysconf)
{
	slong ret = EINV_OK;
	uchar i = 0U;
	bool_t breakFlag = FALSE;

	/* 開始ログ出力 */
	DEBUG_TRCIN();

	/* NULLチェック */
	if (bitSysconf == NULL) {
		DLOG_Error("EINV_ERR_PARAM.\n");
		DEBUG_TRCOUT();
		return EINV_ERR_PARAM;
	}
	else
	{
		/* 処理無し */
	}

	/* 設定されたINV定義文字でシステム構成を検索 */
	while (!breakFlag)
	{
		if (i >= EINV_SYSCONF_TABLE_SIZE)
		{
			breakFlag = TRUE;
		}
		else
		{
			/* INV定義文字が一致した場合 */
			if (cmdSysConf == s_EinvSysconfTable[i].cmdSysConf)
			{
				/* 引数のアドレスにシステム構成ビットを設定 */
				*bitSysconf = s_EinvSysconfTable[i].bitSysconf;
				breakFlag = TRUE;
			}
			else
			{
				/* 処理無し */
			}
			i++;
		}
	}

	/* 見つからなかった場合 */
	if (i == EINV_SYSCONF_TABLE_SIZE)
	{
		DLOG_Error("Unexpected cmdSysConf.\n");
		ret = EINV_ERR_FAIL;
	}
	else
	{
		/* 処理無し */
	}

	/* 終了ログ出力 */
	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:システム構成変換（ビットアサイン ⇒ PCS定義文字）処理		*/
/*--------------------------------------------------------------------------*/
/*	param		:bitSysconf		:システム構成(ビットアサイン)				*/
/*				:cmdSysConf		:システム構成(INV定義文字)					*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	応答コード											*/
/*						EINV_OK					正常終了					*/
/*						EINV_ERR_PARAM			パラメータエラー			*/
/*						EINV_ERR_FAIL			通信異常					*/
/*--------------------------------------------------------------------------*/
/*	detail		:システム構成変換処理を行う。								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/24 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_GetSysConfCharacter(ushort bitSysconf, uchar* cmdSysConf)
{
	slong ret = EINV_OK;
	uchar i = 0U;
	bool_t breakFlag = FALSE;

	/* 開始ログ出力 */
	DEBUG_TRCIN();

	/* NULLチェック */
	if (cmdSysConf == NULL)
	{
		DLOG_Error("EINV_ERR_PARAM.\n");
		DEBUG_TRCOUT();
		return EINV_ERR_PARAM;
	}
	else
	{
		/* 処理無し */
	}

	/* 設定されたビットアサインでシステム構成文字を検索 */
	while (!breakFlag)
	{
		if (i >= EINV_SYSCONF_TABLE_SIZE)
		{
			breakFlag = TRUE;
		}
		else
		{
			/* ビットアサインが一致した場合 */
			if (bitSysconf == s_EinvSysconfTable[i].bitSysconf)
			{
				/* 引数のアドレスにシステム構成文字を設定 */
				*cmdSysConf = s_EinvSysconfTable[i].cmdSysConf;
				breakFlag = TRUE;
			}
			else
			{
				/* 処理無し */
			}
			i++;
		}
	}

	/* 見つからなかった場合 */
	if (i == EINV_SYSCONF_TABLE_SIZE)
	{
		DLOG_Error("Unexpected cmdSysConf.\n");
		ret = EINV_ERR_FAIL;
	}
	else
	{
		/* 処理無し */
	}

	/* 終了ログ出力 */
	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:2桁年を4桁に変更処理										*/
/*--------------------------------------------------------------------------*/
/*	param		:year2		:2桁年											*/
/*				:year4		:4桁年											*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	応答コード											*/
/*						EINV_OK					正常終了					*/
/*						EINV_ERR_PARAM			パラメータエラー			*/
/*--------------------------------------------------------------------------*/
/*	detail		:2桁年を4桁に変更処理行う("24"->"2024")。					*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/24 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_n_StrYear2To4(schar* year2, schar* year4)
{
	slong ret = EINV_OK;

	DEBUG_TRCIN();

	if (year2 == NULL || year4 == NULL)
	{
		DLOG_Error("EINV_ERR_PARAM.\n");
		DEBUG_TRCOUT();
		return EINV_ERR_PARAM;
	}
	else
	{
		/* 処理無し */
	}

	snprintf(year4, EINV_LONGDATEYEAR_LEN, "20%s", year2);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:4桁年を2桁に変更処理										*/
/*--------------------------------------------------------------------------*/
/*	param		:year4		:4桁年											*/
/*				:year2		:2桁年											*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	応答コード											*/
/*						EINV_OK					正常終了					*/
/*						EINV_ERR_PARAM			パラメータエラー			*/
/*--------------------------------------------------------------------------*/
/*	detail		:4桁年を2桁に変更処理行う("2024"->"24")。					*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/24 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_n_StrYear4To2(schar* year4, schar* year2)
{
	slong ret = EINV_OK;

	DEBUG_TRCIN();

	if (year2 == NULL || year4 == NULL)
	{
		DLOG_Error("EINV_ERR_PARAM.\n");
		DEBUG_TRCOUT();
		return EINV_ERR_PARAM;
	}
	else
	{
		/* 処理無し */
	}

	EINV_n_StrNCopy(year2, year4 + 2, sizeof(year2));

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:2桁年を4桁に変更処理										*/
/*--------------------------------------------------------------------------*/
/*	param		:year2		:2桁年											*/
/*				:year4		:4桁年											*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	応答コード											*/
/*						EINV_OK					正常終了					*/
/*						EINV_ERR_PARAM			パラメータエラー			*/
/*--------------------------------------------------------------------------*/
/*	detail		:2桁年を4桁に変更処理行う(24->2024)。						*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/24 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_n_NumYear2To4(uchar year2, ushort* year4)
{
	slong ret = EINV_OK;

	DEBUG_TRCIN();

	if (year4 == NULL)
	{
		DLOG_Error("EINV_ERR_PARAM.\n");
		DEBUG_TRCOUT();
		return EINV_ERR_PARAM;
	}
	else
	{
		/* 処理無し */
	}

	*year4 = (ushort)(year2 + EINV_YEAR_2000);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:4桁年を2桁に変更処理										*/
/*--------------------------------------------------------------------------*/
/*	param		:year4		:4桁年											*/
/*				:year2		:2桁年											*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	応答コード											*/
/*						EINV_OK					正常終了					*/
/*						EINV_ERR_PARAM			パラメータエラー			*/
/*--------------------------------------------------------------------------*/
/*	detail		:4桁年を2桁に変更処理行う(2024->24)。						*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/24 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_n_NumYear4To2(ushort year4, uchar* year2)
{
	slong ret = EINV_OK;

	DEBUG_TRCIN();

	if (year2 == NULL)
	{
		DLOG_Error("EINV_ERR_PARAM.\n");
		DEBUG_TRCOUT();
		return EINV_ERR_PARAM;
	}
	else
	{
		/* 処理無し */
	}

	*year2 = (uchar)(year4 - EINV_YEAR_2000);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:	車両充放電器運転動作状態値チェック					*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	V2HOpStatus:運転モード設定値						*/
/*	return			:	EinvValidity_t:EINV_VALID:設定可能					*/
/*						EINV_INVALID:設定不可能								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	CMD81からV2Hの動作状態設定範囲チェック				*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/19 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
EinvValidity_t EINV_n_CheckV2HOpStatus(uchar V2HOpStatus)
{
	EinvValidity_t ret = EINV_INVALID;

	DEBUG_TRCIN();

	if ((V2HOpStatus >= EINV_OPRATSTATE_0) && (V2HOpStatus <= EINV_OPRATSTATE_E))
	{
		ret = EINV_VALID;
	}
	else
	{
		ret = EINV_INVALID;
	}

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	見守りサーバから運転モード設定可能範囲チェック		*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	ctlRunMode:運転モード設定値							*/
/*	param	in		:	sysStructure:システム構成							*/
/*	return			:	EinvValidity_t:EINV_VALID:設定可能					*/
/*						EINV_INVALID:設定不可能								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	見守りサーバから運転モード設定範囲チェック			*/
/*					:	システム構成より見守りサーバから運転モード設定可能性*/
/*					:	チェック											*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/19 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
EinvValidity_t EINV_n_CheckSetRunmodeFromServer(uchar ctlRunMode)
{
	EinvValidity_t ret = EINV_INVALID;

	DEBUG_TRCIN();

	if ((ctlRunMode >= EINV_COMMU_BOARD_MANAGE_MODE_0) && (ctlRunMode <= EINV_COMMU_BOARD_MANAGE_MODE_7))
	{
		ret = EINV_VALID;
	}
	else
	{
		ret = EINV_INVALID;
	}

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	システム構成可能範囲チェック						*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	sysStructure:システム構成							*/
/*	return			:	EinvValidity_t:EINV_VALID:設定可能					*/
/*						EINV_INVALID:設定不可能								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	システム構成可能範囲チェック						*/
/*					:														*/
/*					:														*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/19 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
EinvValidity_t EINV_n_CheckSysStructure(uchar sysStructure)
{
	EinvValidity_t ret = EINV_INVALID;

	DEBUG_TRCIN();

	if (((sysStructure >= EINV_SYS_STRUCT_0) && (sysStructure <= EINV_SYS_STRUCT_6)) || (sysStructure == EINV_SYS_STRUCT_7))
	{
		ret = EINV_VALID;
	}
	else
	{
		ret = EINV_INVALID;
	}

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	システム構成ASCII CODE->数字						*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	sysStructure:システム構成ASCII CODE					*/
/*	return			:	sysStrucnum:システム構成NUM							*/
/*--------------------------------------------------------------------------*/
/*	detail			:	システム構成ASCII CODE->数字						*/
/*					:														*/
/*					:														*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/27 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
uchar EINV_n_ConvertSysStructureFromAsciiToNum(uchar sysStructure)
{
	uchar sysStrucnum = EINV_INVALIDVALUE;

	DEBUG_TRCIN();

	if ((sysStructure >= EINV_SYS_STRUCT_0) && (sysStructure <= EINV_SYS_STRUCT_6))
	{
		sysStrucnum = sysStructure - EINV_SYS_STRUCT_0;
	}
	else if (sysStructure == EINV_SYS_STRUCT_7)
	{
		sysStrucnum = 7u;
	}
	else {}
	
	DEBUG_TRCOUT();

	return sysStrucnum;
}
/*==========================================================================*/
/*	Description		:	見守りサーバから運転モード設定可能範囲チェック		*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	ctlRunMode:運転モード設定値							*/
/*	param	in		:	sysStructure:システム構成							*/
/*	return			:	EinvSupport_t:	EINV_SUPPORT:設定可能				*/
/*									   	EINV_NOTSUPPORT:設定不可能			*/
/*--------------------------------------------------------------------------*/
/*	detail			:	見守りサーバから運転モード設定可能チェック			*/
/*					:	システム構成より見守りサーバから運転モード設定		*/
/*					:	可能性チェック										*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/19 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
EinvSupport_t EINV_n_CheckSetRunmodeSupportOrNotFromServer(uchar ctlRunMode, uchar sysStructure)
{
	EinvSupport_t ret = EINV_NOTSUPPORT;
	uchar ctlRunModeIndex = 0u;

	DEBUG_TRCIN();

	if ((sysStructure >= EINV_SYS_STRUCT_0) && (sysStructure <= EINV_SYS_STRUCT_6))
	{
		sysStructure = sysStructure - EINV_SYS_STRUCT_0;
	}
	else if (sysStructure == EINV_SYS_STRUCT_7)
	{
		sysStructure = 7u;
	}
	else
	{
		/* 処理無し */
	}

	ctlRunModeIndex = ctlRunMode - EINV_SYS_STRUCT_0;

	ret = s_einvServerSetRunmodeCheckTbl[ctlRunModeIndex][sysStructure];

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	スマホアプリから運転モード設定可能範囲チェック		*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	ctlRunMode:運転モード設定値							*/
/*	return			:	EinvValidity_t:EINV_VALID:設定可能					*/
/*						EINV_INVALID:設定不可能								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	スマホアプリから運転モード設定可能範囲チェック		*/
/*					:														*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/04 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
EinvValidity_t EINV_n_CheckSetRunModeFromSmartPhone(uchar ctlRunMode)
{

	EinvValidity_t ret = EINV_INVALID;

	DEBUG_TRCIN();

	if ((ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_0) ||
		(ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_1) ||
		(ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_2) ||
		(ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_3) ||
		(ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_4) ||
		(ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_5) ||
		(ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_6) ||
		(ctlRunMode == EINV_COMMU_BOARD_MANAGE_MODE_7)
		)
	{
		ret = EINV_VALID;
	}
	else
	{
		ret = EINV_INVALID;
	}

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	OPリモコンから運転モード設定可能範囲チェック		*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	ctlRunMode:運転モード設定値							*/
/*	return			:	EinvValidity_t:EINV_VALID:設定可能					*/
/*						EINV_INVALID:設定不可能								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	OPリモコンから運転モード設定可能範囲チェック		*/
/*					:														*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/11 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
EinvValidity_t EINV_n_CheckSetRunModeFromOpRemote(uchar ctlRunMode)
{

	EinvValidity_t ret = EINV_INVALID;

	DEBUG_TRCIN();

	if ((ctlRunMode == EINV_OP_SELL) ||
		(ctlRunMode == EINV_OP_GREEN) ||
		(ctlRunMode == EINV_OP_BATTCHARGING) ||
		(ctlRunMode == EINV_OP_BATTDISCHARGE) ||
		(ctlRunMode == EINV_OP_CARCHARGING) ||
		(ctlRunMode == EINV_OP_CARDISCHARGE) ||
		(ctlRunMode == EINV_OP_CARBATTDISCHARGE) ||
		(ctlRunMode == EINV_OP_IDLE)
		)
	{
		ret = EINV_VALID;
	}
	else
	{
		ret = EINV_INVALID;
	}

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	OPリモコンから運転モード設定値変換					*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	opRunMode:opリモコンからモード設定値				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	設定値フォーマット変更								*/
/*	return			:	設定値フォーマット変更結果							*/
/*					:														*/
/*					:														*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/12 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
uchar EINV_n_ConvertOpRemoteRunModeValue(uchar opRunMode)
{
	uchar opRunModeIndex;
	uchar ctlrunmode;

	DEBUG_TRCIN();

	if ((opRunMode >= EINV_OP_SELL) && (opRunMode <= EINV_OP_GREEN))
	{
		opRunModeIndex = opRunMode - '1';
		ctlrunmode = s_einvConvertOPRunmodeToCtlRunmodeTbl[opRunModeIndex];
	}
	else if ((opRunMode >= EINV_OP_BATTCHARGING) && (opRunMode <= EINV_OP_BATTDISCHARGE))
	{
		opRunModeIndex = opRunMode - '2';
		ctlrunmode = s_einvConvertOPRunmodeToCtlRunmodeTbl[opRunModeIndex];
	}
	else if ((opRunMode >= EINV_OP_CARCHARGING) && (opRunMode <= EINV_OP_CARDISCHARGE))
	{
		opRunModeIndex = opRunMode - 'B' + 4u;
		ctlrunmode = s_einvConvertOPRunmodeToCtlRunmodeTbl[opRunModeIndex];
	}
	else if (opRunMode == EINV_OP_IDLE)
	{
		opRunModeIndex = opRunMode - '6' + 7u;
		ctlrunmode = s_einvConvertOPRunmodeToCtlRunmodeTbl[opRunModeIndex];
	}
	else
	{
		opRunModeIndex = opRunMode - 'N' + 6u;
		ctlrunmode = s_einvConvertOPRunmodeToCtlRunmodeTbl[opRunModeIndex];
	}

	DEBUG_TRCOUT();

	return ctlrunmode;
}

/*==========================================================================*/
/*	Description		:	外部通信機から運転モード設定元可能範囲チェック		*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	ctlRunModeSetFrom:運転モード設定元					*/
/*	return			:	ret:EINV_VALID:設定可能E	INV_INVALID:設定不可能	*/
/*--------------------------------------------------------------------------*/
/*	detail			:	外部通信機から運転モード設定元可能範囲チェック		*/
/*					:														*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/12 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
EinvValidity_t EINV_n_CheckRunModeSetFromExternal(uchar ctlRunModeSetFrom)
{

	EinvValidity_t ret = EINV_INVALID;

	DEBUG_TRCIN();

	if ((ctlRunModeSetFrom == EINV_EXTERNAL_SMARTPHONE) ||
		(ctlRunModeSetFrom == EINV_EXTERNAL_OP) ||
		(ctlRunModeSetFrom == EINV_EXTERNAL_ENL) ||
		(ctlRunModeSetFrom == EINV_EXTERNAL_SERVER_CLIMATEALARM) ||
		(ctlRunModeSetFrom == EINV_EXTERNAL_SERVER_SV) ||
		(ctlRunModeSetFrom == EINV_EXTERNAL_SERVER_ATTENTIONINFO) ||
		(ctlRunModeSetFrom == EINV_EXTERNAL_SERVER_DRCTL) ||
		(ctlRunModeSetFrom == EINV_EXTERNAL_SERVER_AIAUTOCTL)
		)
	{
		ret = EINV_VALID;
	}
	else
	{
		ret = EINV_INVALID;
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:	ENLからENL運転モード設定可能範囲チェック			*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	enlRunMode:ENL運転モード設定値						*/
/*	return			:	EINV_VALID:設定可能E								*/
/*						INV_INVALID:設定不可能								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ENLからENL運転モード設定可能範囲チェック			*/
/*					:														*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/04 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
EinvValidity_t EINV_n_CheckRunModeExternalEnl(uchar enlRunMode)
{

	EinvValidity_t ret = EINV_INVALID;
	DEBUG_TRCIN();

	if ((enlRunMode == EINV_ENLDISCHARGE) ||
		(enlRunMode == EINV_ENLCHARGE) ||
		(enlRunMode == EINV_ENLIDLE) ||
		(enlRunMode == EINV_ENLAUTO)
		)
	{
		ret = EINV_VALID;
	}
	else
	{
		ret = EINV_INVALID;
	}

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	時刻値変換処理	ASCII->NUM							*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	timeAscii:ASCII時刻									*/
/*	param	in		:	timeNum:NUMBER時刻									*/
/*	return			:	EINV_OK				 0								*/
/*					:	EINV_ERR_PARAM		-4								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	hour/min ASCII CODEからNumberへ変換					*/
/*						epcs.h定義した										*/
/*						受信構造体明確後									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/18 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
slong EINV_n_ConvertTimeAsciiToValue(const EinvTimeStr_t* timeAscii, EinvTime_t* timeNum)
{
	uchar i, j, m, n;
	slong ret = EINV_OK;

	DEBUG_TRCIN();

	i = timeAscii->hour[0] - '0';	/*時十桁*/
	j = timeAscii->hour[1] - '0';	/*時個桁*/
	m = timeAscii->minute[0] - '0';	/*分十桁*/
	n = timeAscii->minute[1] - '0';	/*分個桁*/

	if (((i >= 0u) && (i <= 2u)) &&
		((j >= 0u) && (j <= 9u)) &&
		((m >= 0u) && (m <= 5u)) &&
		((n >= 0u) && (n <= 9u))
		)
	{
		if ((i == 2u) && (j > 3u))
		{
			ret = EINV_ERR_PARAM;
		}
		else
		{
			timeNum->hour = i * 10u + j;
			timeNum->min = m * 10u + n;
			ret = EINV_OK;
		}
	}
	else
	{
		ret = EINV_ERR_PARAM;
	}

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	時刻値変換処理	NUM->ASCII							*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	timeAscii:ASCII時刻									*/
/*	param	in		:	timeNum:NUMBER時刻									*/
/*	return			:	EINV_OK				 0								*/
/*					:	EINV_ERR_PARAM		-4								*/
/*--------------------------------------------------------------------------*/
/*	detail			:	hour/min NumberからへASCII CODE変換					*/
/*						epcs.h定義した										*/
/*																			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/20 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
slong EINV_n_ConvertTimeValueToAscii(const EinvTime_t* timeNum, EinvTimeStr_t* timeAscii)
{
	uchar i, j, m, n;
	slong ret = EINV_OK;

	DEBUG_TRCIN();

	if ((timeNum->hour >= 0u) &&
		(timeNum->hour <= 23u) &&
		(timeNum->min >= 0u) &&
		(timeNum->min <= 59u)
		)
	{
		i = timeNum->hour / 10u;
		j = timeNum->hour - i * 10u;
		m = timeNum->min / 10u;
		n = timeNum->min - m * 10u;

		timeAscii->hour[0] = i + '0';
		timeAscii->hour[1] = j + '0';
		timeAscii->hour[2] = '\0';
		timeAscii->minute[0] = m + '0';
		timeAscii->minute[1] = n + '0';
		timeAscii->minute[2] = '\0';
		ret = EINV_OK;
	}
	else
	{
		ret = EINV_ERR_PARAM;
	}

	DEBUG_TRCOUT();

	return ret;

}
/*==========================================================================*/
/*	Description		:	非常時運転モード設定値範囲チェックする				*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	powerOffRunMode:非常時運転モード					*/
/*	return			:	EINV_VALID:'0'-'4'									*/
/*					:	EINV_INVALID:'0'-'4'以外							*/
/*--------------------------------------------------------------------------*/
/*	detail			:	非常時運転モード設定値範囲チェックする				*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/12 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
EinvValidity_t EINV_n_CheckPowerOffRunMode(uchar powerOffRunMode)
{
	EinvValidity_t result = EINV_VALID;

	DEBUG_TRCIN();

	if ((powerOffRunMode >= EINV_POWEROFFAUTOPOWERCTL) &&
		(powerOffRunMode <= EINV_POWEROFFELEDISCHARGE))
	{
		result = EINV_VALID;
	}
	else
	{
		result = EINV_INVALID;
	}

	DEBUG_TRCOUT();

	return result;
}
/*==========================================================================*/
/*	Description		:	運転開始停止設定値範囲チェックする					*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	opStartStop:運転開始停止							*/
/*	return			:	EINV_VALID:EINV_OPSTOP/EINV_OPSTART					*/
/*					:	EINV_INVALID:EINV_OPSTOP/EINV_OPSTART以外			*/
/*--------------------------------------------------------------------------*/
/*	detail			:	運転開始停止設定値範囲チェックする					*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/12 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
EinvValidity_t EINV_n_CheckOpStartStop(uchar opStartStop)
{
	EinvValidity_t result = EINV_VALID;

	DEBUG_TRCIN();

	if ((opStartStop == EINV_OPSTOP) ||
		(opStartStop == EINV_OPSTART)
		)
	{
		result = EINV_VALID;
	}
	else
	{
		result = EINV_INVALID;
	}

	DEBUG_TRCOUT();
	return result;
}
/*==========================================================================*/
/*	Description		:	自動運転切替時刻値チェック処理						*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	time:開始終了時刻									*/
/*	return			:	EINV_VALID:			設置値範囲正常					*/
/*					:	EINV_INVALID:		設定値範囲異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	チェック時刻値										*/
/*					:	時:	  0-23以外->パラメータ異常						*/
/*					:	分:	  0-59以外->パラメータ異常						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/18 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
slong EINV_n_CheckTimeValid(const EinvTime_t* time)
{
	slong ret = EINV_INVALID;

	DEBUG_TRCIN();

	if ((time->hour >= EINV_TIME_HOUR_MIN) &&
		(time->hour <= EINV_TIME_HOUR_MAX) &&
		(time->min >= EINV_TIME_MIN_MIN) &&
		(time->min <= EINV_TIME_MIN_MAX)
		)
	{
		ret = EINV_VALID;
	}
	else
	{
		ret = EINV_INVALID;
	}

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	チェックSYSTIME処理									*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	time:				SYSTIMEシステム時間				*/
/*	return			:	EINV_VALID:			設置値範囲正常					*/
/*					:	EINV_INVALID:		設定値範囲異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	チェック時刻値										*/
/*					:	時:	  0-23以外->パラメータ異常						*/
/*					:	分:	  0-59以外->パラメータ異常						*/
/*					:	秒:	  0-59以外->パラメータ異常						*/
/*					:	月:	  1-12以外->パラメータ異常						*/
/*					:	年:	  2000-2099以外->パラメータ異常					*/
/*					:	日:	  1-31以外->パラメータ異常						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
slong EINV_n_CheckSysTimeValid(const EtimTm_t* time)
{
	slong ret = EINV_INVALID;

	DEBUG_TRCIN();

	if ((time->tmHour >= EINV_TIME_HOUR_MIN) &&
		(time->tmHour <= EINV_TIME_HOUR_MAX) &&
		(time->tmMin >= EINV_TIME_MIN_MIN) &&
		(time->tmMin <= EINV_TIME_MIN_MAX) &&
		(time->tmSec >= EINV_TIME_SECOND_MIN) &&
		(time->tmSec <= EINV_TIME_SECOND_MAX) &&
		(time->tmMday >= EINV_TIME_DAY_MIN) &&
		(time->tmMday <= EINV_TIME_DAY_MAX) &&
		(time->tmMon >= EINV_TIME_MON_MIN) &&
		(time->tmMon <= EINV_TIME_MON_MAX) &&
		(time->tmYear >= EINV_TIME_YEAR_MIN) &&
		(time->tmYear <= EINV_TIME_YEAR_MAX)
		)
	{
		ret = EINV_VALID;
	}
	else
	{
		ret = EINV_INVALID;
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:	Button ON->OFF判定									*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	buttonstatus:カレントbutton ON/OFF状態				*/
/*	param	in/out	:	buttonprestatus:直近button ON/OFF状態				*/
/*--------------------------------------------------------------------------*/
/*	return			:	1:EINV_SET		Button ON->OFF判定成立				*/
/*						0:EINV_CLR											*/
/*--------------------------------------------------------------------------*/
/*	detail			:	Button ON->OFF判定		エッジ判定					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC Sun.JT : 新規作成						*/
/*==========================================================================*/
EinvFlag_t EINV_n_CheckButtonStatus(uchar buttonStatus, uchar* buttonPrestatus)
{
	EinvFlag_t ret = EINV_CLR;

	DEBUG_TRCIN();

	if (*buttonPrestatus != buttonStatus)
	{
		if (*buttonPrestatus == EINV_ON)
		{
			/*ON->OFFエッジ検出*/
			ret = EINV_SET;
		}
		else
		{
			ret = EINV_CLR;
		}
	}
	else
	{
		ret = EINV_CLR;
	}

	/*カレントbutton ON/OFF状態保存*/
	*buttonPrestatus = buttonStatus;

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:	現在時刻は自動運転モード確定時間帯にある判断		*/
/*--------------------------------------------------------------------------*/
/*	param	in		:	time:現在時刻										*/
/*	param	in		:	timeScope:指定時間帯								*/
/*	return			:	EINV_SET:カレント時間が時間帯内						*/
/*					:	EINV_CLR:カレント時間が時間帯外						*/
/*--------------------------------------------------------------------------*/
/*	detail			:	現在時刻は自動運転モード確定時間帯にあるか判断		*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/19 : DHC Sun.JT : 新規作成					*/
/*==========================================================================*/
slong EINV_n_CheckTimeScope(const EinvTime_t* time, const EinvTimeScope_t* timeScope)
{

	slong ret = EINV_CLR;

	uint c, s, e;

	DEBUG_TRCIN();

	c = time->hour * EINV_MIN_ONE_HOUR + time->min;							/*カレント時刻*/
	s = timeScope->start.hour * EINV_MIN_ONE_HOUR + timeScope->start.min;	/*開始時刻*/
	e = timeScope->end.hour * EINV_MIN_ONE_HOUR + timeScope->end.min;	  	/*終了時刻*/

	if (e >= s)
	{
		if ((c >= s) &&
			(c <= e)
			)
		{
			/*s-e*/
			ret = EINV_SET;
		}
		else
		{
			ret = EINV_CLR;
		}
	}
	else
	{	/*(e < s)*/


		e = e + EINV_MIN_ONE_DAY_MIN - s;

		if (c >= s)
		{
			c = c - s;
		}
		else
		{
			c = c + EINV_MIN_ONE_DAY_MIN - s;
		}

		s = 0u;

		if ((c >= s) &&
			(c <= e)
			)
		{
			/*s-e*/
			ret = EINV_SET;
		}
		else
		{
			ret = EINV_CLR;
		}
	}

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description		:	変換テーブルに従い、rSOCからuSOCへの変換を行う。	*/
/*--------------------------------------------------------------------------*/
/*	param			:	ushort	RSOC	CMD64でINVマイコンから取得した蓄電池充電上限・放電下限値 */
/*					:	ushort*	DEST	変換後にユーザー表示用を支えるuSOC	*/
/*--------------------------------------------------------------------------*/
/*	return			:	slong	ret		EINV_OK				 0：変換テーブルの中で検索できた	*/
/*										EINV_ERR_SYSCALL	-1：変換テーブルの中で検索できなかった*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/30 : Chen.chongyang : 新規作成					*/
/*==========================================================================*/
slong EINV_n_SocConvtRtoU(ushort* DEST, const ushort RSOC)
{
	ushort rSoc = RSOC;
	slong ret = 0;
	uchar i = 0;
	bool_t	breakFlag = FALSE;
	uchar range;

	range = sizeof(EINV_SocConvtTable)/sizeof(EINV_SocConvtTable[0]);

	DEBUG_TRCIN();

	while (!breakFlag)
	{
		if(i >= range)
		{			
			breakFlag = TRUE;
			ret = EINV_ERR_FAIL;
		}
		else if( EINV_SocConvtTable[i][0] == rSoc )	/* 変換テーブル中の最初の列でrSOCを検索した場合 */
		{
			*DEST = EINV_SocConvtTable[i][1];		/* 検索したrSOCに対応する2番目の列のuSOCを返す */
			DLOG_Debug("EINV_SocConvtRtoU() return: uSOC=%d\n", *DEST);

			breakFlag = TRUE;
			ret = EINV_OK;
		}
		else
		{
			/* 処理無し */
		}

		/* 4つのリレーショナル・マッピング（relational mapping）を追加したため、rSocの値の重複を引き起こしている。
			rSocの設定値は大きいから順に検索時、同じrSoc設定値が存在する場合は、rSoc値ごとに対応するuSoc設定値の内で最も大きい値を取得する。 */
		i = i + 1;
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description		:	変換テーブルの通り、uSOCからrSOCへの変換を行う。CMD65でINVマイコンへ送信する。*/
/*--------------------------------------------------------------------------*/
/*	param			:	ushort 	USOC	ユーザー側に設定されている蓄電池充電上限・放電下限値*/
/*					:	ushort* DEST	変換後にCMD65でINVマイコンへ送信するrSOC			*/
/*--------------------------------------------------------------------------*/
/*	return			:	slong	ret		EINV_OK				 0：変換テーブルの中で検索できた	*/
/*										EINV_ERR_SYSCALL	-1：変換テーブルの中で検索できなかった*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/30 : DHC Chen.chongyang : 新規作成				*/
/*==========================================================================*/
slong EINV_n_SocConvtUtoR(ushort* DEST, const ushort USOC)
{
	ushort uSoc = USOC;
	slong ret = 0;
	uchar i = 0;
	bool_t	breakFlag = FALSE;

	i = sizeof(EINV_SocConvtTable)/sizeof(EINV_SocConvtTable[0]);

	DEBUG_TRCIN();
	
	while (!breakFlag)
	{
		i = i - 1;
		if(i < 0)
		{
			breakFlag = TRUE;
			ret = EINV_ERR_FAIL;
		}
		else if(EINV_SocConvtTable[i][1] == uSoc)	/* 変換テーブル中の2番目の列でuSOCを検索できた場合 */
		{
			*DEST = EINV_SocConvtTable[i][0];		/* 検索したuSOCに対応する最初の列でのrSOCを返す  */
			DLOG_Debug("EINV_SocConvtUtoR() return: rSOC=%d\n", *DEST);

			breakFlag = TRUE;
			ret = EINV_OK;
		}
		else
		{
			/* 処理無し */
		}
	}


	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description	:文字列コピー処理											*/
/*--------------------------------------------------------------------------*/
/*	param		:dest		:コピー先										*/
/*				:src		:コピー元										*/
/*				:destSize	:コピー先サイズ									*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	コピー先副本										*/
/*--------------------------------------------------------------------------*/
/*	detail		:文字列コピー処理を行う。									*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/05/06 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
schar* EINV_n_StrNCopy(schar* dest, const schar* src, size_t destSize)
{
	schar* ret = NULL;

	DEBUG_TRCIN();

	memset(dest, 0, destSize);
	ret = strncpy(dest, src, destSize - 1);

	DEBUG_TRCOUT();

	return ret;
}


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*End Of File*/
