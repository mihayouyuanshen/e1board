/****************************************************************************/
/*	Description		:INV通信ミドル 共通										*/
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
#ifndef __EINV_N_G_H__
#define __EINV_N_G_H__


/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "typedef.h"
#include "einv_g.h"
#include "common_g.h"


/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define EINV_PHASE_1				((sshort)1)
#define EINV_PHASE_1_1				((sshort)11)
#define EINV_PHASE_1_2				((sshort)12)
#define EINV_PHASE_1_3				((sshort)13)
#define EINV_PHASE_2				((sshort)2)
#define EINV_PHASE_2_1				((sshort)21)
#define EINV_PHASE_2_2				((sshort)22)
#define EINV_PHASE_2_3				((sshort)23)
#define EINV_PHASE_3				((sshort)3)
#define EINV_PHASE_3_1				((sshort)31)
#define EINV_PHASE_3_2				((sshort)32)
#define EINV_PHASE_3_3				((sshort)33)
#define EINV_PHASE_4				((sshort)4)
#define EINV_PHASE_4_1				((sshort)41)
#define EINV_PHASE_4_2				((sshort)42)
#define EINV_PHASE_4_3				((sshort)43)
#define EINV_PHASE_5				((sshort)5)
#define EINV_PHASE_5_1				((sshort)51)
#define EINV_PHASE_5_2				((sshort)52)
#define EINV_PHASE_5_3				((sshort)53)
#define EINV_PHASE_6				((sshort)6)
#define EINV_PHASE_6_1				((sshort)61)
#define EINV_PHASE_6_2				((sshort)62)
#define EINV_PHASE_6_3				((sshort)63)
#define EINV_PHASE_6_4				((sshort)64)
#define EINV_PHASE_6_5				((sshort)65)
#define EINV_PHASE_7				((sshort)7)
#define EINV_PHASE_7_1				((sshort)71)
#define EINV_PHASE_7_2				((sshort)72)
#define EINV_PHASE_7_3				((sshort)73)
#define EINV_PHASE_8				((sshort)8)
#define EINV_PHASE_8_1				((sshort)81)
#define EINV_PHASE_8_2				((sshort)82)
#define EINV_PHASE_8_3				((sshort)83)
#define EINV_PHASE_9				((sshort)9)
#define EINV_PHASE_9_1				((sshort)91)
#define EINV_PHASE_9_2				((sshort)92)
#define EINV_PHASE_9_3				((sshort)93)
#define EINV_PHASE_END				((sshort)-1)
#define EINV_PHASE_ERR				((sshort)-11)
#define EINV_PHASE_RTY				((sshort)-101)

/*  変換値 */
#define	EINV_CMD_RES_BASE_HEX				((uchar)16U)
#define	EINV_CMD_RES_BASE_DEC				((uchar)10U)
#define	EINV_CMD_RES_32B_HEX				((uchar)8U)
#define	EINV_PEAK_CTRL_MIN					((uchar)0U)
#define	EINV_PEAK_CTRL_MAX					((uchar)100U)
#define	EINV_SOC_MIN						((ushort)0U)
#define	EINV_SOC_MAX						((ushort)1000U)
#define	EINV_SOH_WRITING_STR				((schar*)"----")		/* SOH値書き換え時 */
#define	EINV_SOH_ERR_STR					((schar*)"ERR ")		/* SOH値エラー */
#define	EINV_PANEL_PWR_MIN					((ushort)0U)			/* 太陽光発電パネル定格(PVパネル定格)[W] 最小 */
#define	EINV_PANEL_PWR_MAX					((ushort)9999U)			/* 太陽光発電パネル定格(PVパネル定格)[W] 最大 */
#define	EINV_GEN_PWR_CAP_MIN				((ushort)0U)			/* 発電電力上限[W] 最小 */
#define	EINV_GEN_PWR_CAP_MAX				((ushort)9999U)			/* 発電電力上限[W] 最大 */
#define EINV_BTCAPA_TO_CHARGE_AMOUNT		((uchar)100U)			/* [0.1kWh]⇒[Wh]変換値) */
#define	EINV_INV_RATED_PWR_MIN				((ulong)0UL)			/* 太陽光パワコン定格(INV定格)[W] 最小 */
#define	EINV_INV_RATED_PWR_MAX				((ulong)99990UL)		/* 太陽光パワコン定格(INV定格)[W] 最大 */
#define EINV_BT_RATE_ELTC_0768				((ushort)768U)			/* CMD60:蓄電池定格電力量=“0768” */
#define EINV_BT_RATE_ELTC_0958				((ushort)958U)			/* CMD60:蓄電池定格電力量=“0958” */
#define EINV_MAINTE_DATE_YEAR_MAX			((schar*)"99")			/* メンテ期最大年 */
#define EINV_MAINTE_DATE_YEAR_PLUS			((uchar)1U)
#define EINV_ASSIG1_DATE_MON				((schar*)"06")			/* V:実施指定日1の「月」 */
#define EINV_ASSIG2_DATE_MON				((schar*)"09")			/* V:実施指定日2の「月」 */
#define EINV_MAINTE_DATE_DAY_1				((schar*)"21")			/* V:実施指定日の「日」 */
#define EINV_MAINTE_DATE_DAY_2				((schar*)"22")			/* V:実施指定日の「日」 */
#define EINV_MAINTE_DATE_DAY_3				((schar*)"23")			/* V:実施指定日の「日」 */
#define EINV_MAINTE_DATE_DAY_4				((schar*)"24")			/* V:実施指定日の「日」 */
#define EINV_MAINTE_DATE_DAY_5				((schar*)"25")			/* V:実施指定日の「日」 */
#define EINV_MAINTE_DATE_DAY_6				((schar*)"26")			/* V:実施指定日の「日」 */
#define EINV_MAINTE_DATE_DAY_7				((schar*)"27")			/* V:実施指定日の「日」 */
#define EINV_MAINTE_DATE_DAY_8				((schar*)"28")			/* V:実施指定日の「日」 */
#define EINV_MAINTE_DATE_DAY_9				((schar*)"29")			/* V:実施指定日の「日」 */
#define EINV_MAINTE_DATE_DAY_0				((schar*)"30")			/* V:実施指定日の「日」 */
#define EINV_MAINTE_DATE_DAY_OTHER			((schar*)"30")			/* V:実施指定日の「日」 */
#define EINV_YEAR_2000						((ushort)2000U)			/* 2000年 */

/* エラーコード */
#define	EINV_ERROR_FREE						"BE000"				/* 正常コード */	
#define	EINV_ERROR_FREE_RATE				'0'					/* 正常レーティング */
#define	EINV_ERROR_OPSTOP					"BC096"				/* 運転停止 */	
#define	EINV_ERROR_OPSTOP_RATE				'G'					/* 運転停止レーティング */
#define	EINV_ERROR_RTSTART					"BC672"				/* 試運転開始 */	
#define	EINV_ERROR_RTSTART_RATE				'A'					/* 試運転開始レーティング */
#define	EINV_ERROR_RTBREAK					"bc673"				/* 試運転中断 */	
#define	EINV_ERROR_RTBREAK_RATE				'C'					/* 試運転中断レーティング */
#define	EINV_ERROR_RTSTOP					"bc674"				/* 試運転終了 */	
#define	EINV_ERROR_RTSTOP_RATE				'C'					/* 試運転終了レーティング */
#define	EINV_ERROR_COMM						"BF650"				/* INVマイコン通信不通 */	
#define	EINV_ERROR_COMM_RATE				'4'					/* INVマイコン通信不通レーティング */
#define	EINV_ERROR_INIT						"bf650"				/* INVマイコン初期化通信失敗 */	
#define	EINV_ERROR_INIT_RATE				'C'					/* INVマイコン初期化通信失敗レーティング */
/* 初期値 */
#define	EINV_BT_LOWER_LIMIT_INIT						((ushort)(30 * 10))						/* 蓄電池放電下限値設定(cmd64) */
#define	EINV_BT_UPPER_LIMIT_INIT						((ushort)(100 * 10))					/* 蓄電池充電上限値設定(cmd64) */
#define	EINV_BT_LOWER_LIMIT_18_INIT						((ushort)(100 * 10))					/* 蓄電池放電下限値設定(cmd18) */
#define	EINV_BT_UPPER_LIMIT_18_INIT						((ushort)000)							/* 蓄電池充電上限値設定(cmd18) */
#define EINV_CHARGE_PWR_SET_INIT						((ushort)9999)							/* 充電電力設定値(蓄電池) */
#define EINV_DISCHG_PWR_SET_INIT						((ushort)9999)							/* 放電電力設定値(蓄電池) */
#define EINV_V2H_DISCHG_LOW_LMT_INIT					((ushort)(30 * 10))						/* 車両放電下限値設定 */
#define EINV_V2H_CHARGE_UP_LMT_INIT						((ushort)(100 * 10))					/* 車両充電上限値設定 */
#define EINV_PWRCUT_V2H_DISC_LOW_INIT					((ushort)(10 * 10))						/* 停電時車両放電下限値設定 */
#define EINV_CAR_CHARGE_SET_INIT						((ulong)0U)								/* 車載電池充電電力設定値 */
#define EINV_CAR_DISCHG_SET_INIT						((ulong)0U)								/* 車載電池放電電力設定値 */
#define EINV_CONTRACT_PWR_SET_INIT						((ushort)9999)							/* 契約電力設定値 */
#define EINV_EQPMT_APPROVE_CAP_INIT						((ushort)0000)							/* 設備認定容量 */
#define EINV_GEN_PWR_UPPER_LIMIT_INIT					((ushort)0000)							/* 発電電力上限設定(上限クリップ) */
#define EINV_BT_CHAR_PWR_MAX_SET_INIT					((ushort)0000)							/* 蓄電池最大充電電力設定 */
#define EINV_CAR_CHAR_PWR_SET_INIT						((ushort)0000)							/* 車両最大充電電力設定 */
#define EINV_MODEL_FUNCTION_NAME_INIT					((schar*)"E_")							/* モデル機能名称 */
#define EINV_PV_PANEL_RATING_INIT						((ushort)0000)							/* PVパネル定格 */
#define EINV_POWER_HOUSE_INIT							((schar*)"--------------------------")	/* 発電所ID */
#define EINV_SELL_MODE_CHAR_START_HOUR_INIT				((schar*)"01")							/* 売電モード充電開始時刻(時) */
#define EINV_SELL_MODE_CHAR_START_MINUTE_INIT			((schar*)"00")							/* 売電モード充電開始時刻(分) */
#define EINV_SELL_MODE_CHAR_STOP_HOUR_INIT				((schar*)"05")							/* 売電モード充電停止時刻(時) */
#define EINV_SELL_MODE_CHAR_STOP_MINUTE_INIT			((schar*)"59")							/* 売電モード充電停止時刻(分) */
#define EINV_SELL_MODE_DISC_START_HOUR_INIT				((schar*)"06")							/* 売電モード放電開始時刻(時) */
#define EINV_SELL_MODE_DISC_START_MINUTE_INIT			((schar*)"00")							/* 売電モード放電開始時刻(分) */
#define EINV_SELL_MODE_DISC_STOP_HOUR_INIT				((schar*)"00")							/* 売電モード放電停止時刻(時) */
#define EINV_SELL_MODE_DISC_STOP_MINUTE_INIT			((schar*)"50")							/* 売電モード放電停止時刻(分) */
#define EINV_GREEN_MODE_CHAR_START_HOUR_INIT			((schar*)"01")							/* グリーンモード充電開始時刻(時) */
#define EINV_GREEN_MODE_CHAR_START_MINUTE_INIT			((schar*)"00")							/* グリーンモード充電開始時刻(分) */
#define EINV_GREEN_MODE_CHAR_STOP_HOUR_INIT				((schar*)"01")							/* グリーンモード充電停止時刻(時) */
#define EINV_GREEN_MODE_CHAR_STOP_MINUTE_INIT			((schar*)"01")							/* グリーンモード充電停止時刻(分) */
#define EINV_GREEN_MODE_DISC_START_HOUR_INIT			((schar*)"01")							/* グリーンモード放電開始時刻(時) */
#define EINV_GREEN_MODE_DISC_START_MINUTE_INIT			((schar*)"02")							/* グリーンモード放電開始時刻(分) */
#define EINV_GREEN_MODE_DISC_STOP_HOUR_INIT				((schar*)"00")							/* グリーンモード放電停止時刻(時) */
#define EINV_GREEN_MODE_DISC_STOP_MINUTE_INIT			((schar*)"50")							/* グリーンモード放電停止時刻(分) */
#define EINV_SUN_PWR_RATED_INIT							((schar*)"00000")						/* 外付け太陽光発電の定格 */
#define EINV_POSTAL_NUM_INIT							((schar*)"-------")						/* 郵便番号 */
#define	EINV_ECHONET_LITE_VER_INIT						((uchar)'Q')							/* ECHONET Lite_Ver */
#define EINV_IP_ADDRESS_INIT							((schar*)"192168001203")				/* IPアドレス */
#define EINV_SUBNET_MASK_INIT							((schar*)"255255255000")				/* サブネットマスク */
#define EINV_DEFAULT_GATEWAY_INIT						((schar*)"192168001001")				/* デフォルトゲートウェイ */
#define EINV_POIORITY_DNS_INIT							((schar*)"192168001001")				/* 優先DNS */
#define EINV_REPLACE_DNS_INIT							((schar*)"192168001001")				/* 代替DNS */
#define EINV_PROXY_ADDRESS_INIT							((schar*)"------------")				/* PROXYアドレス */
#define EINV_PORT_NUMBER_INIT							((schar*)"-----")						/* ポート番号 */
#define	EINV_RESERVE_INIT								((uchar)'0')							/* 予約 */
#define EINV_MENTANCE_DATE_YEAR_INIT					((schar*)"00")							/* メンテ期間開始日(年) */
#define EINV_MENTANCE_DATE_MON_INIT						((schar*)"00")							/* メンテ期間開始日(月) */
#define EINV_MENTANCE_DATE_DAY_INIT						((schar*)"00")							/* メンテ期間開始日(日) */
#define EINV_IMPLTATION_DESIGN_DAY1_YEAR_INIT			((schar*)"00")							/* 実施指定日1(年) */
#define EINV_IMPLTATION_DESIGN_DAY1_MON_INIT			((schar*)"00")							/* 実施指定日1(月) */
#define EINV_IMPLTATION_DESIGN_DAY1_DAY_INIT			((schar*)"00")							/* 実施指定日1(日) */
#define EINV_IMPLTATION_DESIGN_DAY2_YEAR_INIT			((schar*)"00")							/* 実施指定日2(年) */
#define EINV_IMPLTATION_DESIGN_DAY2_MON_INIT			((schar*)"00")							/* 実施指定日2(月) */
#define EINV_IMPLTATION_DESIGN_DAY2_DAY_INIT			((schar*)"00")							/* 実施指定日2(日) */
#define EINV_SYS_MADE_NUM_INIT							((schar*)"----------------")			/* システム製造番号 */
#define EINV_BT_RATE_ELTC_INIT							((ushort)0000)							/* 蓄電池定格電力量 */
#define EINV_PCS_RATE_OUT_INIT							((ushort)5900)							/* PCS定格出力 */
#define EINV_PCS_RATE_VOL_INIT							((ushort)0000)							/* PCS定格電圧 */
#define EINV_PCS_RATE_VOL_ALONE_INIT					((ushort)0000)							/* PCS定格電圧_自立時 */
#define EINV_SET_DATE_YEAR_INIT							((schar*)"--")							/* 設置日 */
#define EINV_SET_DATE_MON_INIT							((schar*)"--")							/* 設置日 */
#define EINV_SET_DATE_DAY_INIT							((schar*)"--")							/* 設置日 */
#define EINV_INV_MCU_FW_INIT							((schar*)"--------")					/* INVマイコン_FW */
#define EINV_PV_MCU_FW_INIT								((schar*)"--------")					/* PVマイコン_FW */
#define EINV_BDC_MCU_FW_INIT							((schar*)"--------")					/* BDCマイコン_FW */
#define EINV_JB_FW_INIT									((schar*)"---")							/* J.B._FW */
#define EINV_INV_MCU_BOOT_LOADER_INIT					((schar*)"-----")						/* INVマイコン_ブートローダー */
#define EINV_PV_MCU_BOOT_LOADER_INIT					((schar*)"-----")						/* PVマイコン_ブートローダー */
#define EINV_BDCMCUBOOTLOADER_INIT						((schar*)"-----")						/* BDCマイコン_ブートローダー */
#define EINV_BT_MADE_NUM_INIT							((schar*)"----------------")			/* 蓄電池ユニット製造番号 */
#define EINV_BT_RATED_VOL_INIT							((ushort)0000)							/* 蓄電池定格電圧 */
#define EINV_BT_MAX_CHARGE_PWR_INIT						((ulong)300)							/* 蓄電池最大充電電力値 */
#define EINV_BT_MAX_DISCHAR_PWR_INIT					((ulong)300)							/* 蓄電池最大放電電力値 */
#define EINV_BT_MAX_CHARGE_PWR_INDEP_INIT				((ulong)300)							/* 蓄電池最大充電電力値_自立時 */
#define EINV_BT_MAX_DISCHAR_PWR_INDEP_INIT				((ulong)300)							/* 蓄電池最大放電電力値_自立時 */
#define EINV_SYS_FREQUENCY_INIT							((ushort)(60 * 100))					/* 系統周波数(50Hz or 60Hz) */
#define EINV_SYS_VOLT_DETECT_LEVEL_INIT					((ushort)(115 * 10))					/* 系統過電圧検出レベル */
#define EINV_SYS_VOLT_DETECT_TIME_INIT					((ushort)1000)							/* 系統過電圧検出時間 */
#define EINV_SYS_UNDER_VOLT_DETECT_LEVEL_INIT			((ushort)(80 * 10))						/* 系統不足電圧検出レベル */
#define EINV_SYS_UNDER_VOLT_DETECT_TIME_INIT			((ushort)1000)							/* 系統不足電圧検出時間 */
#define	EINV_FRE_RISE_DETECT_LEVEL_INIT					((uchar)'2')							/* 周波数上昇検出レベル */
#define EINV_FRE_RISE_DETECT_TIME_INIT					((ushort)1000)							/* 周波数上昇検出時間 */
#define	EINV_FRE_DROP_DETECT_LEVEL_INIT					((uchar)'5')							/* 周波数低下検出レベル */
#define EINV_FRE_DROP_DETECT_TIME_INIT					((ushort)2000)							/* 周波数低下検出時間 */
#define EINV_PASS_IND_OPER_DETECT_INIT					((ushort)   3)							/* 受動的単独運転検出レベル */
#define EINV_RESCT_TIME_INIT							((ushort)(300 * 1000))					/* 復帰時間 */
#define EINV_CHANGE_TIME_INCOR_DES_INIT					((ushort)(5 * 60 * 1000))				/* 出力増減変化時間 */
#define EINV_VOL_RS_INHB_VALUE_INIT						((ushort)1090)							/* 電圧上昇抑制値 */
#define EINV_VOL_RS_INHBATT_LIMIT_INIT					((ushort)(200 * 1000))					/* 電圧上昇抑制動作待機時限 */
#define EINV_COT_POWER_CONTROL_INIT						((ushort)(95 * 10))						/* 力率一定制御 */
#define EINV_FRE_FEED_BACK_GAIN_INIT					((ushort)1000)							/* 周波数フィードバックゲイン */
#define EINV_FRE_LEV_ALLOW_INIT							((ushort)10)							/* 並列時許容周波数レベル */
#define EINV_PRE_MOMENT_YEAR_INIT						((uchar)14)								/* 現在時刻 */
#define EINV_PRE_MOMENT_MON_INIT						((uchar)1)								/* 現在時刻 */
#define EINV_PRE_MOMENT_DAY_INIT						((uchar)1)								/* 現在時刻 */
#define EINV_PRE_MOMENT_HOUR_INIT						((uchar)0)								/* 現在時刻 */
#define EINV_PRE_MOMENT_MIN_INIT						((uchar)0)								/* 現在時刻 */
#define EINV_PRE_MOMENT_SEC_INIT						((uchar)0)								/* 現在時刻 */
#define EINV_COMMU_BOARD_ERROR							EINV_ERROR_FREE							/* 通信ボードエラー */
#define EINV_CHARGE_DISCHAR_PWR_AC_INIT					((ushort)0000)							/* PCS充放電電力（AC端計測値） */
#define EINV_TTL_CHAR_ENERGY_SYS_AC_INIT				((ulong)0U)								/* 積算PCS充電電力量(AC端計測値)(系統連系時) */
#define EINV_TTL_DISCHAR_ENERGY_SYS_AC_INIT				((ulong)0U)								/* 積算PCS放電電力量(AC端計測値)(系統連系時) */
#define EINV_RSOC_INIT									((ushort)000)							/* rSOC */
#define EINV_REMAINING_BT_INIT							((ushort)0000)							/* 電池残量 */
#define EINV_BT_VOLT_INIT								((ushort)0000)							/* 電池電圧*/
#define EINV_SOH_INIT									((ushort)000)							/* SOH */
#define EINV_PV_GEN_PWR_INIT							((sshort)0000)							/* PV発電電力(本製品以外のPV) */
#define EINV_REV_PWR_FLOW_INIT							((sshort)0000)							/* 逆潮流電力 */
#define EINV_SYS_VOLT_INIT								((ushort)0000)							/* 系統電圧 */
#define EINV_PCS_ERR_INIT								EINV_ERROR_FREE							/* PCSエラー */
#define EINV_PCS_ERR_RATING_INIT						((schar*)"00")							/* PCSエラーレーティング */
#define EINV_CHAR_DISCHAR_PWR_DC_INIT					((sshort)0000)							/* DC蓄電池充放電電力2(コンバータ部) */
#define EINV_TTL_CHAR_ENERGY_DC_INIT					((ushort)0U)							/* DC積算蓄電池充電電力量2(コンバータ部) */
#define EINV_TTL_DISCHAR_ENERGY_DC_INIT					((ushort)0U)							/* DC積算蓄電池放電電力量2(コンバータ部) */
#define EINV_TTL_PV_GEN_PWR_INIT						((ulong)0U)								/* 積算PV発電電力量(本製品以外のPV) */
#define EINV_TTL_PURCHASE_PWR_INIT						((ulong)0U)								/* 積算買電電力量 */
#define EINV_TTL_SELLING_PWR_INIT						((ulong)0U)								/* 積算売電電力量 */
#define EINV_GEN_PWR_LIMIT_INIT							((ushort)9999)							/* 発電電力制限値(kW) */
#define EINV_CAR_DISCHAR_GEABLE_REMAIN_CAPACITY1_INIT	((ulong)0U)								/* 車載電池の放電可能残容量1 */
#define EINV_CAR_DISCHAR_GEABLE_REMAIN_CAPACITY3_INIT	((uchar)0U)								/* 車載電池の放電可能残容量3 */
#define EINV_CAR_CHAR_GEABLE_REMAIN_CAP_INIT			((ulong)0U)								/* 車載電池の充電可能残容量値 */
#define EINV_V2H_INSTANT_POWER_INIT						((slong)0)								/* V2H瞬時充放電電力計測値(AC端計測値) */
#define EINV_CAR_TTL_DISCHAR_POWER_INIT					((ulong)0U)								/* 車載電池の積算放電電力量計測値(AC端計測値)(トータル) */
#define EINV_CAR_TTL_CHAR_POWER_INIT					((ulong)0U)								/* 車載電池の積算充電電力量計測値(AC端計測値)(トータル) */
#define EINV_CAR_REMAIN_CAPACITY1_INIT					((ulong)0U)								/* 車載電池の電池残容量1 */
#define EINV_CAR_REMAIN_CAPACITY3_INIT					((uchar)0U)								/* 車載電池の電池残容量3 */
#define EINV_TTL_CHAR_ENERGY_ALONE_AC_INIT				((ulong)0U)								/* 積算PCS充電電力量(AC端計測値)(自立時) */
#define EINV_TTL_DISCHAR_ENERGY_ALONE_AC_INIT			((ulong)0U)								/* 積算PCS放電電力量(AC端計測値)(自立時) */
#define EINV_PCS_EFFICIENCY_CHAR_INIT					((ushort)000)							/* PCS効率(充電方向) */
#define EINV_PCS_EFFICIENCY_DISCHAR_INIT				((ushort)000)							/* PCS効率(放電方向) */
#define EINV_BT_CHAR_DISCHAR_PWR_INIT					((slong)0)								/* 蓄電池充放電電力(AC端計測値) */
#define EINV_PV_PWR2_INIT								((ushort)0U)							/* 太陽光発電電力(AC端計測値) */
#define EINV_TTL_PV_GEN_PWR2_INIT						((ulong)0U)								/* 積算太陽光発電電力量(AC端計測値) */
#define EINV_CELL_VOLT_MIN_INIT							((ushort)0U)							/* 最低セル電圧 */
#define EINV_CELL_AVERAGE_VOLT_INIT						((ushort)0U)							/* 平均セル電圧 */
#define EINV_CELL_VOLT_MAX_INIT							((ushort)0U)							/* 最高セル電圧 */
#define EINV_CELL_TEMPE_MIN_INIT						((sshort)0)								/* 最低セル温度 */
#define EINV_CELL_TEMPE_MAX_INIT						((sshort)0)								/* 最高セル温度 */
#define EINV_PV_STRING_VOLT1_INIT						((ushort)0000)							/* 太陽光発電ストリング電圧(1) */
#define EINV_PV_STRING_VOLT2_INIT						((ushort)0000)							/* 太陽光発電ストリング電圧(2) */
#define EINV_PV_STRING_VOLT3_INIT						((ushort)0000)							/* 太陽光発電ストリング電圧(3) */
#define EINV_PV_STRING_VOLT4_INIT						((ushort)0000)							/* 太陽光発電ストリング電圧(4) */
#define EINV_PV_STRING_VOLT5_INIT						((ushort)0000)							/* 太陽光発電ストリング電圧(5) */
#define EINV_V2H_OPERATION_BUTTON1_INIT					((uchar)'0')							/* V2H本体操作ボタン1 */
#define EINV_REV_CT_CURRENT_U_INIT						((ushort)0U)							/* 逆潮CTの電流値(U相) */
#define EINV_REV_CT_CURRENT_W_INIT						((ushort)0U)							/* 逆潮CTの電流値(W相) */
#define EINV_CAR_BT_DISC_CAPACITY_VALUE1_INIT			((ulong)0U)								/* 車載電池の放電可能容量値1 */
#define EINV_CAR_BT_USE_CAPACITY_VALUE1_INIT			((ulong)0U)								/* 車載電池の使用容量値1 */
#define EINV_CAR_BT_CHAR_CAPACITY_VALUE_INIT			((ulong)0U)								/* 車載電池の充電可能容量値 */
#define EINV_CAR_CHAR_AND_DISC_MODEL_NAME_INIT			((schar*)"----------------")			/* 車両充放電器機種名 */
#define EINV_CAR_CHAR_AND_DISC_MADE_NUM_INIT			((schar*)"------------------------------")/* 車両充放電器製造番号 */
#define EINV_CAR_CHAR_AND_DISC_TYPE_INIT				((uchar)21U)							/* 車両充放電器タイプ */
#define EINV_CAR_CHAR_AND_DISC_RATED_CHAR_INIT			((ulong)0U)								/* 車両充放電器定格充電能力 */
#define EINV_CAR_CHAR_AND_DISC_RATED_DISC_INIT			((ulong)0U)								/* 車両充放電器定格放電能力 */
#define EINV_CARID_INIT									((schar*)"0")							/* 車両ID */
#define EINV_EVPCS_FW_INIT								((schar*)"--------")					/* EVPCS_FW */
#define EINV_MCU_FW_INIT								((schar*)"--------")					/* MCU_FW */
#define EINV_C2_COMMU_BORAD_FW_INIT						((schar*)"--------")					/* C2通信ボード_FW */

/* 試運転済みファイル */
#define	EINV_TESTRUN_FINISH_FILE_PATH		"/mnt/ram/EinvTestrunFin.inf"

/*merge from sunJT START*/
#define EINV_MIN_ONE_HOUR					((uchar)60u)
#define EINV_HOUR_ONE_DAY					((uchar)24u)
#define EINV_MIN_ONE_DAY_MIN				((ushort)(0u))
#define EINV_MIN_ONE_DAY_MAX				((ushort)((23u*60u) + 59u))

#define EINV_SIXHOUR		((ulong)21600000u)

#define EINV_MIN_ONE_HOUR	((uchar)60u)

/*V2Hの動作状態*/
/*更新/未更新フラッグ*/
#define EINV_NOUPDATE 		((uchar)(0u))
#define EINV_UPDATE 		((uchar)(1u))

/*必要/不必要フラッグ*/
#define EINV_NONEED 		((uchar)(0u))
#define EINV_NEED	 		((uchar)(1u))

/*CMD65/CMD75フラッグ*/
#define EINV_SENDCMD65 		((uchar)(0u))
#define EINV_SENDCMD75	 	((uchar)(1u))

/*運転/停止*/
#define EINV_OPSTOP			((uchar)'0')
#define EINV_OPSTART		((uchar)'1')

/*蓄電池充電完了通知*/
#define EINV_UNFINISH		((uchar)'0')
#define EINV_FINISH			((uchar)'1')

/*CMD75 LOCK UNLOCK*/
#define EINV_UNLOCK			((uchar)'0')
#define EINV_LOCK			((uchar)'1')

/*蓄電池運転状態保存変数*/
#define EINV_BATTSTOP		((uchar)'0')
#define EINV_BATTRUNNING	((uchar)'1')

/*V2H本体操作可能不可能*/
#define EINV_IMPOSSIBLE		((uchar)'0')
#define EINV_POSSIBLE		((uchar)'1')

/*V2H本体操作ボタンステータス*/
#define EINV_OFF			((uchar)'0')
#define EINV_ON				((uchar)'1')

/*充放電不可フラグ*/
#define EINV_NONCHARGEDISCHARGE_FLAG_CLR	((uchar)'0')
#define EINV_NONCHARGEDISCHARGE_FLAG_SET	((uchar)'1')

/*外部通信機から設定可能な運転モード設定元*/
#define EINV_EXTERNAL_SMARTPHONE			((uchar)'0')
#define EINV_EXTERNAL_OP					((uchar)'1')
#define EINV_EXTERNAL_ENL					((uchar)'3')
#define EINV_EXTERNAL_SERVER_CLIMATEALARM	((uchar)'4')
#define EINV_EXTERNAL_SERVER_SV				((uchar)'5')
#define EINV_EXTERNAL_SERVER_ATTENTIONINFO	((uchar)'6')
#define EINV_EXTERNAL_SERVER_DRCTL			((uchar)'7')
#define EINV_EXTERNAL_SERVER_AIAUTOCTL		((uchar)'8')

/*緊急時運転モード*/
#define	EINV_POWEROFFAUTOPOWERCTL			((uchar)'0')
#define	EINV_POWEROFFBATTERYCHARGING		((uchar)'1')
#define	EINV_POWEROFFCARCHARGING			((uchar)'2')
#define	EINV_POWEROFFELECHARGING			((uchar)'3')
#define	EINV_POWEROFFELEDISCHARGE			((uchar)'4')

/*システム通常稼働中のSUB状態*/
/*自動運転モード*/
#define EINV_AUTOSELL						((uchar)'0')
#define EINV_AUTOGREEN						((uchar)'1')

/*:同時放電モード優先度*/
#define EINV_ENLBATTERY						((uchar)'0')
#define EINV_ENLCAR							((uchar)'1')

/*運転モード設定元*/
#define EINV_SYSTEM							((uchar)'0')
#define EINV_SMARTPHONEOP					((uchar)'1')
#define EINV_V2H							((uchar)'2')
#define EINV_ENL							((uchar)'3')
#define EINV_CLIMATEALARM					((uchar)'4')
#define EINV_SV								((uchar)'5')
#define EINV_ATTENTIONINFO					((uchar)'6')
#define EINV_DRCTL							((uchar)'7')
#define EINV_AIAUTOCTL						((uchar)'8')

/*ENL運転モード*/
#define EINV_ENLDISCHARGE					((uchar)'1')
#define EINV_ENLCHARGE						((uchar)'2')
#define EINV_ENLIDLE						((uchar)'3')
#define EINV_ENLAUTO						((uchar)'4')

/* メンテ状態 */
#define EINV_MAINTIDLE						((uchar)0)
#define EINV_MAINTRUN						((uchar)1)
#define EINV_MAINTBREAK						((uchar)2)
#define EINV_MAINTFIN						((uchar)3)

/*dummy data*/
#define EINV_INVALIDVALUE					((uchar)255u)
#define EINV_UCHARMAXVALUE					((uchar)0xFF)
#define EINV_USHORTMAXVALUE					((ushort)0xFFFF)
#define EINV_ULONGMAXVALUE					((ulong)0xFFFFFFFF)
/*merge from sunJT END*/

/* PCSエラーレーティングインデックス */
#define EINV_PCSERRRAT_0					((ushort)0)
#define EINV_PCSERRRAT_1					((ushort)1)

/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef struct _einv_cmd_date_t {
	schar	year[EINV_DATEYEAR_LEN];				/* 年（“00”～“99”）*/
	schar	month[EINV_DATEMONTH_LEN];				/* 月（“01”～“12”）*/
	schar	day[EINV_DATEDAY_LEN];					/* 日（“01”～“31”）*/
}EinvDateStr_t;

typedef struct{
	EinvDateStr_t mainteStartDate;					/* V:メンテ期間開始日 */
	EinvDateStr_t runAssignDate1;					/* V:実施指定日1 */
	EinvDateStr_t runAssignDate2;					/* V:実施指定日2 */
} EinvMaintenanceDates_t;

/*merge from sunJT START*/
typedef struct _einv_time_t{
	uchar	hour;			// 時刻（時）00-23
	uchar	min;			// 時刻（分）00-59
}EinvTime_t;

typedef struct _einv_sys_struct_t{
	uchar	sysStructAscii;			// システム構成 ASCII CODE
	uchar	sysStructNum;			// システム構成 NUM
}EinvSysStruct_t;

typedef struct _einv_update_t{
	ulong CTLRUNMODE:1;					/*1:update 0:not update*/
	ulong AUTORUNMODE:1;
	ulong SELLCHARGSTARTTIME:1;
	ulong SELLCHARGSTOPTIME:1;
	ulong SELLDISCHARGSTARTTIME:1;
	ulong SELLDISCHARGSTOPTIME:1;
	ulong GREENCHARGSTARTTIME:1;
	ulong GREENCHARGSTOPTIME:1;
	ulong GREENDISCHARGSTARTTIME:1;
	ulong GREENDISCHARGSTOPTIME:1;
	ulong RUNMODESETFROM:1;
	ulong POWEROFFRUNMODE:1;
	ulong BATTCLASSRUNMODE:1;
	ulong V2HCLASSRUNMODE:1;
	ulong DISCHARGEPRIORITY:1;
	ulong ENLRUNMODEFROM:1;
	ulong MAINTAIN:1;
	ulong OPERATIONSTARTSTOP:1;
	ulong MAINSTATUS:1;
	ulong SYSSTRUCT:1;
	ulong TESTRUNAVAILABLE:1;
	ulong TESTRUNREQUEST:1;
	ulong TESTRUNSTATUS:1;
	ulong TESTRUNNEED:1;
}EinvUpdate_t;

typedef struct _einv_v2hstatus_t{
	uchar connecterStatus;
	uchar button1;
	uchar button2;
	uchar button3;
	uchar v2hOperatePossible;
} EinvV2hStatus_t;

typedef struct _einv_manualrunstatus_t{
	uchar chargeStatus;
	uchar disChargeStatus;
	uchar battStatus;
	uchar v2hRunStatus;
	uchar nonChargeable;
	uchar nonDisChargeable;
} Einv_ManualRunStatus_t;

typedef struct _einv_timescope_t{
	EinvTime_t start;
	EinvTime_t end;
} EinvTimeScope_t;
/*merge from sunJT END*/
/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
typedef enum {
	EINV_INVALID = 0u,
	EINV_VALID,
} EinvValidity_t;

typedef enum {
	EINV_NOTSUPPORT = 0u,
	EINV_SUPPORT,
} EinvSupport_t;

typedef enum {
	EINV_ENLBATTERYSET = 0u,
	EINV_ENLV2HSET,
} EinvEnlSetFrom_t;

typedef enum {
	EINV_SELLCHARGINGSTART = 0u,						/*売電モード充電開始*/
	EINV_SELLCHARGINGSTOP,								/*売電モード充電停止*/
	EINV_SELLDISCHARGSTART,								/*売電モード放電開始*/
	EINV_SELLDISCHARGSTOP,								/*売電モード放電停止*/
	EINV_GREENCHARGINGSTART,							/*グリーンモード充電開始*/
	EINV_GREENCHARGINGSTOP,								/*グリーンモード充電停止*/
	EINV_GREENDISCHARGSTART,							/*グリーンモード放電開始*/
	EINV_GREENDISCHARGSTOP,								/*グリーンモード放電停止*/
	EINV_SIXHOURCONTINUE,								/*IDLE状態持続6時間*/
	EINV_EXTERNALSET,									/*外部通信機より設置変更*/
	EINV_FEEDBACK,										/*FEEDBACKより設置変更*/
} EinvTimerEvent_t;

typedef enum {
	EINV_INITSET = 0u,									/*初回設定*/
	EINV_NORMALSET,										/*初回以降設定*/
} EinvEnlSetFromStatus_t;

/*MAIN状態*/
typedef enum {
	EINV_SYS = 0u,										/*通常稼働中*/
	EINV_MAINTAIN,										/*メンテ*/
	EINV_TEST,											/*試運転*/
	EINV_BACKUP,										/*復旧*/
} EinvMainStatus_t;

typedef enum {
	EINV_CLR = 0u,
	EINV_SET,
} EinvFlag_t;

typedef enum {
	EINV_TESTRUNSTART = 0u,
	EINV_TESTRUNSTOP,
	EINV_TESTRUNABORT,
} EinvCmdParaSetOption_t;

typedef enum {
	EINV_TESTRUN_IDLE = 0u,//試運転IDLE
	EINV_TESTRUN_DO,	  //試運転中
	EINV_TESTRUN_FINISH,  //試運転後処理
	EINV_TESTRUN_ABORT,	  //試運転中止
} TestRunSubFsmState_t;

/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/


#endif /*__EINV_N_G_H__*/
