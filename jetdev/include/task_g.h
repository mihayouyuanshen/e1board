/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC yufeng.yang										*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/14												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __TASK_G_H__
#define __TASK_G_H__
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/

/*****************************************************************************/
/*																			 */
/*				   システム共通 											 */
/*																			 */
/*****************************************************************************/
#define		LFW_SPORT_MIN		 	40000		/* 最小タスク Server用Port番号*/
#define		LFW_SPORT_MAX	 		49999		/* 最大タスク Server用Port番号*/

#define		LFW_CPORT_MIN	 		50000		/* 最小タスク Client用Port番号*/
#define		LFW_CPORT_MAX	 		59999		/* 最大タスク Client用Port番号*/

/*****************************************************************************/
/*																			 */
/*				   Base System Software(1 - 15) 							 */
/*																			 */
/*****************************************************************************/
												/* 初期化					 */
#define		AIN_SPORT		LFW_SPORT_MIN + 0	/*	 Server用Port番号		 */
#define		AIN_CPORT		LFW_CPORT_MIN + 0	/*	 client用Port番号		 */
#define		AIN_PRI			80 					/*	 タスク優先度			 */
												/* サンプルアプリ			 */
#define		ASMP_SPORT		LFW_SPORT_MIN + 1	/*	 Server用Port番号		 */
#define		ASMP_CPORT		LFW_CPORT_MIN + 1	/*	 Client用Port番号		 */
#define		ASMP_PRI		30 	 				/*	 タスク優先度			 */
												/* Qtアプリケーション		 */
#define		AGUI_SPORT		LFW_SPORT_MIN + 2	/*	 Server用Port番号		 */
#define		AGUI_CPORT		LFW_CPORT_MIN + 2	/*	 Client用Port番号		 */
#define		AGUI_PRI		30 	 				/*	 タスク優先度			 */
												/* 初期設定					 */
#define		AINI_SPORT		LFW_SPORT_MIN + 3	/*	 Server用Port番号		 */
#define		AINI_CPORT		LFW_CPORT_MIN + 3	/*	 client用Port番号		 */
#define		AINI_PRI		30 					/*	 タスク優先度			 */
												/* Webブラウザ				 */
#define		AWEB_SPORT		LFW_SPORT_MIN + 4	/*	 Server用Port番号		 */
#define		AWEB_CPORT		LFW_CPORT_MIN + 4	/*	 client用Port番号		 */
#define		AWEB_PRI		30 					/*	 タスク優先度			 */
												/* LCD表示アプリ			 */
#define		ALCD_SPORT		LFW_SPORT_MIN + 5	/*	 Server用Port番号		 */
#define		ALCD_CPORT		LFW_CPORT_MIN + 5	/*	 client用Port番号		 */
#define		ALCD_PRI		30 					/*	 タスク優先度			 */
												/* Web Server	 			*/
#define		WEBSVR_SPORT	LFW_SPORT_MIN + 6	/*	 Server用Port番号		 */
#define		WEBSVR_CPORT	LFW_CPORT_MIN + 6	/*	 client用Port番号		 */
#define		WEBSVR_PRI		30 					/*	 タスク優先度			 */

/*****************************************************************************/
/*																			 */
/*				   デバイスドライバ(16 - 48)								 */
/*																			 */
/*****************************************************************************/
												/* タイマドライバ			 */
#define		DTM_SPORT		LFW_SPORT_MIN + 16	/*	 Server用Port番号		 */
#define		DTM_CPORT		LFW_CPORT_MIN + 16	/*	 Client用Port番号		 */
#define		DTM_PRI			75	 					/*	 タスク優先度			 */
/*****************************************************************************/
/*																			 */
/*				   ミドルウェア(49-143)										 */
/*																			 */
/*****************************************************************************/
												/*   サンプルミドル			 */
#define		MSMP_SPORT		LFW_SPORT_MIN + 49	/*	 Server用Port番号		 */
#define		MSMP_CPORT		LFW_CPORT_MIN + 49	/*	 Client用Port番号		 */
#define		MSMP_PRI		50 	 				/*	 タスク優先度			 */
												/*   PCS通信ミドル			 */
#define		EINV_SPORT		LFW_SPORT_MIN + 50	/*	 Server用Port番号		 */
#define		EINV_CPORT		LFW_CPORT_MIN + 50	/*	 Client用Port番号		 */
#define		EINV_PRI		50 	 				/*	 タスク優先度			 */
												/*   N社見守りサーバーミドル */
#define		ENSV_SPORT		LFW_SPORT_MIN + 51	/*	 Server用Port番号		 */
#define		ENSV_CPORT		LFW_CPORT_MIN + 51	/*	 Client用Port番号		 */
#define		ENSV_PRI		50 	 				/*	 タスク優先度			 */
												/*   S社見守りサーバーミドル */
#define		ESSV_SPORT		LFW_SPORT_MIN + 52	/*	 Server用Port番号		 */
#define		ESSV_CPORT		LFW_CPORT_MIN + 52	/*	 Client用Port番号		 */
#define		ESSV_PRI		50 	 				/*	 タスク優先度			 */
												/*   HEMS通信ミドル			 */
#define		EHEM_SPORT		LFW_SPORT_MIN + 53	/*	 Server用Port番号		 */
#define		EHEM_CPORT		LFW_CPORT_MIN + 53	/*	 Client用Port番号		 */
#define		EHEM_PRI		50 	 				/*	 タスク優先度			 */
												/*   EchonetLite通信ミドル	 */
#define		EENL_SPORT		LFW_SPORT_MIN + 54	/*	 Server用Port番号		 */
#define		EENL_CPORT		LFW_CPORT_MIN + 54	/*	 Client用Port番号		 */
#define		EENL_PRI		50 	 				/*	 タスク優先度			 */
												/*   モード仕向け管理ミドル	 */
#define		EMOD_SPORT		LFW_SPORT_MIN + 55	/*	 Server用Port番号		 */
#define		EMOD_CPORT		LFW_CPORT_MIN + 55	/*	 Client用Port番号		 */
#define		EMOD_PRI		50 	 				/*	 タスク優先度			 */
												/*   サービスエラーリストミドル	 */
#define		ELST_SPORT		LFW_SPORT_MIN + 56	/*	 Server用Port番号		 */
#define		ELST_CPORT		LFW_CPORT_MIN + 56	/*	 Client用Port番号		 */
#define		ELST_PRI		50 	 				/*	 タスク優先度			 */
												/*   お知らせ管理ミドル		 */
#define		ENOT_SPORT		LFW_SPORT_MIN + 57	/*	 Server用Port番号		 */
#define		ENOT_CPORT		LFW_CPORT_MIN + 57	/*	 Client用Port番号		 */
#define		ENOT_PRI		50 	 				/*	 タスク優先度			 */
												/*   初期化ミドル			 */
#define		EINI_SPORT		LFW_SPORT_MIN + 58	/*	 Server用Port番号		 */
#define		EINI_CPORT		LFW_CPORT_MIN + 58	/*	 Client用Port番号		 */
#define		EINI_PRI		50 	 				/*	 タスク優先度			 */
												/*   ログ管理ミドル			 */
#define		ELOG_SPORT		LFW_SPORT_MIN + 59	/*	 Server用Port番号		 */
#define		ELOG_CPORT		LFW_CPORT_MIN + 59	/*	 Client用Port番号		 */
#define		ELOG_PRI		50 	 				/*	 タスク優先度			 */
												/*   時刻管理ミドル			 */
#define		ETIM_SPORT		LFW_SPORT_MIN + 60	/*	 Server用Port番号		 */
#define		ETIM_CPORT		LFW_CPORT_MIN + 60	/*	 Client用Port番号		 */
#define		ETIM_PRI		50 	 				/*	 タスク優先度			 */
												/*   ウォッチドッグミドル	 */
#define		EWDT_SPORT		LFW_SPORT_MIN + 61	/*	 Server用Port番号		 */
#define		EWDT_CPORT		LFW_CPORT_MIN + 61	/*	 Client用Port番号		 */
#define		EWDT_PRI		50 	 				/*	 タスク優先度			 */
												/*   LED管理ミドル			 */
#define		ELED_SPORT		LFW_SPORT_MIN + 62	/*	 Server用Port番号		 */
#define		ELED_CPORT		LFW_CPORT_MIN + 62	/*	 Client用Port番号		 */
#define		ELED_PRI		50 	 				/*	 タスク優先度			 */
												/*   LCD管理ミドル			 */
#define		EBKL_SPORT		LFW_SPORT_MIN + 63	/*	 Server用Port番号		 */
#define		EBKL_CPORT		LFW_CPORT_MIN + 63	/*	 Client用Port番号		 */
#define		EBKL_PRI		50 	 				/*	 タスク優先度			 */
												/*   ブザー管理ミドル		 */
#define		EBUZ_SPORT		LFW_SPORT_MIN + 64	/*	 Server用Port番号		 */
#define		EBUZ_CPORT		LFW_CPORT_MIN + 64	/*	 Client用Port番号		 */
#define		EBZU_PRI		50 	 				/*	 タスク優先度			 */
												/*   EEPROM管理ミドル		 */
#define		EEEP_SPORT		LFW_SPORT_MIN + 65	/*	 Server用Port番号		 */
#define		EEEP_CPORT		LFW_CPORT_MIN + 65	/*	 Client用Port番号		 */
#define		EEEP_PRI		50 	 				/*	 タスク優先度			 */
												/*   タッチパネルミドル		 */
#define		ETCH_SPORT		LFW_SPORT_MIN + 66	/*	 Server用Port番号		 */
#define		ETCH_CPORT		LFW_CPORT_MIN + 66	/*	 Client用Port番号		 */
#define		ETCH_PRI		50 	 				/*	 タスク優先度			 */
												/*   画面キャプチャミドル	 */
#define		ECAP_SPORT		LFW_SPORT_MIN + 67	/*	 Server用Port番号		 */
#define		ECAP_CPORT		LFW_CPORT_MIN + 67	/*	 Client用Port番号		 */
#define		ECAP_PRI		50 	 				/*	 タスク優先度			 */
												/*   ネットワークミドル		 */
#define		ENET_SPORT		LFW_SPORT_MIN + 68	/*	 Server用Port番号		 */
#define		ENET_CPORT		LFW_CPORT_MIN + 68	/*	 Client用Port番号		 */
#define		ENET_PRI		50 	 				/*	 タスク優先度			 */
												/*   リモコンUpgradeミドル	 */
#define		EFPG_SPORT		LFW_SPORT_MIN + 69	/*	 Server用Port番号		 */
#define		EFPG_CPORT		LFW_CPORT_MIN + 69	/*	 Client用Port番号		 */
#define		EFPG_PRI		50 	 				/*	 タスク優先度			 */
												/*   グラフ管理ミドル		 */
#define		EGRA_SPORT		LFW_SPORT_MIN + 70	/*	 Server用Port番号		 */
#define		EGRA_CPORT		LFW_CPORT_MIN + 70	/*	 Client用Port番号		 */
#define		EGRA_PRI		50 	 				/*	 タスク優先度			 */
												/*   HGW通信ミドル			 */
#define		EHGW_SPORT		LFW_SPORT_MIN + 71	/*	 Server用Port番号		 */
#define		EHGW_CPORT		LFW_CPORT_MIN + 71	/*	 Client用Port番号		 */
#define		EHGW_PRI		50 	 				/*	 タスク優先度			 */
												/*   ホーム画面情報取得		 */
#define		EGUI_SPORT		LFW_SPORT_MIN + 72	/*	 Server用Port番号		 */
#define		EGUI_CPORT		LFW_CPORT_MIN + 72	/*	 Client用Port番号		 */
#define		EGUI_PRI		50 	 				/*	 タスク優先度			 */
												/*   N社見守りサーバーサブ	 */
#define		ENSV_SUB_SPORT	LFW_SPORT_MIN + 73	/*	 Server用Port番号		 */
#define		ENSV_SUB_CPORT	LFW_CPORT_MIN + 73	/*	 Client用Port番号		 */
#define		ENSV_SUB_PRI	50 	 				/*	 タスク優先度			 */
												/*   N社見守りサーバー通信	 */
#define		ENSC_SPORT		LFW_SPORT_MIN + 74	/*	 Server用Port番号		 */
#define		ENSC_CPORT		LFW_CPORT_MIN + 74	/*	 Client用Port番号		 */
#define		ENSC_PRI		50 	 				/*	 タスク優先度			 */
												/*	 出力抑制制御ミドル		 */
#define		ESCH_SPORT		LFW_SPORT_MIN + 75	/*	 Server用Port番号		 */
#define		ESCH_CPORT		LFW_CPORT_MIN + 75	/*	 Client用Port番号		 */
#define		ESCH_PRI		50 	 				/*	 タスク優先度			 */
												/*	 ファームUpgradeミドル	 */
#define		EFUP_SPORT		LFW_SPORT_MIN + 76	/*	 Server用Port番号		 */
#define		EFUP_CPORT		LFW_CPORT_MIN + 76	/*	 Client用Port番号		 */
#define		EFUP_PRI		50 	 				/*	 タスク優先度			 */
												/*	 電力会社サーバ通信ミドル*/
#define		EEPC_SPORT		LFW_SPORT_MIN + 77	/*	 Server用Port番号		 */
#define		EEPC_CPORT		LFW_CPORT_MIN + 77	/*	 Client用Port番号		 */
#define		EEPC_PRI		50 	 				/*	 タスク優先度			 */
												/*	 設定値復元ミドル		 */
#define		EREP_SPORT		LFW_SPORT_MIN + 78	/*	 Server用Port番号		 */
#define		EREP_CPORT		LFW_CPORT_MIN + 78	/*	 Client用Port番号		 */
#define		EREP_PRI		50 	 				/*	 タスク優先度			 */
												/*	 KEY入力制御ミドル		 */
#define		EKEY_SPORT		LFW_SPORT_MIN + 79	/*	 Server用Port番号		 */
#define		EKEY_CPORT		LFW_CPORT_MIN + 79	/*	 Client用Port番号		 */
#define		EKEY_PRI		50 	 				/*	 タスク優先度			 */
												/*	 LCD制御ミドル			 */
#define		ELCD_SPORT		LFW_SPORT_MIN + 80	/*	 Server用Port番号		 */
#define		ELCD_CPORT		LFW_CPORT_MIN + 80	/*	 Client用Port番号		 */
#define		ELCD_PRI		50 	 				/*	 タスク優先度			 */
												/*	 FW更新ミドル			 */
#define		EOTA_SPORT		LFW_SPORT_MIN + 81	/*	 Server用Port番号		 */
#define		EOTA_CPORT		LFW_CPORT_MIN + 81	/*	 Client用Port番号		 */
#define		EOTA_PRI		50 	 				/*	 タスク優先度			 */
												/*	 Remconミドル			 */
#define		ERMC_SPORT		LFW_SPORT_MIN + 82	/*	 Server用Port番号		 */
#define		ERMC_CPORT		LFW_CPORT_MIN + 82	/*	 Client用Port番号		 */
#define		ERMC_PRI		50 	 				/*	 タスク優先度			 */
												/*	 SYSミドル			 */
#define		ESYS_SPORT		LFW_SPORT_MIN + 83	/*	 Server用Port番号		 */
#define		ESYS_CPORT		LFW_CPORT_MIN + 83	/*	 Client用Port番号		 */
#define		ESYS_PRI		50 	 				/*	 タスク優先度			 */
												/*	 UPNPミドル			 */
#define		EUPNP_SPORT		LFW_SPORT_MIN + 84	/*	 Server用Port番号		 */
#define		EUPNP_CPORT		LFW_CPORT_MIN + 84	/*	 Client用Port番号		 */
#define		EUPNP_PRI		50 	 				/*	 タスク優先度			 */
												/*	 WDTミドル			 */
#define		WDT_SPORT		LFW_SPORT_MIN + 85	/*	 Server用Port番号		 */
#define		WDT_CPORT		LFW_CPORT_MIN + 85	/*	 Client用Port番号		 */
#define		WDT_PRI			50 	 				/*	 タスク優先度			 */
												/*	 見守りサーバーミドル	 */
#define		EWTCH_SPORT		LFW_SPORT_MIN + 86	/*	 Server用Port番号		 */
#define		EWTCH_CPORT		LFW_CPORT_MIN + 86	/*	 Client用Port番号		 */
#define		EWTCH_PRI		50 	 				/*	 タスク優先度			 */
												/*	 TIMERミドル			 */
#define		ETIMER_SPORT	LFW_SPORT_MIN + 87	/*	 Server用Port番号		 */
#define		ETIMER_CPORT	LFW_CPORT_MIN + 87	/*	 Client用Port番号		 */
#define		ETIMER_PRI		50 	 				/*	 タスク優先度			 */
												/*	 EMDTRミドル			 */
#define		EMDTR_SPORT		LFW_SPORT_MIN + 88	/*	 Server用Port番号		 */
#define		EMDTR_CPORT		LFW_CPORT_MIN + 88	/*	 Client用Port番号		 */
#define		EMDTR_PRI		50 	 				/*	 タスク優先度			 */
												/*	 EDATミドル			 */
#define		EDAT_SPORT		LFW_SPORT_MIN + 89	/*	 Server用Port番号		 */
#define		EDAT_CPORT		LFW_CPORT_MIN + 89	/*	 Client用Port番号		 */
#define		EDAT_PRI		50 	 				/*	 タスク優先度			 */

/*****************************************************************************/
/*																			 */
/*				   メンテナンス & ツール(144-160)							 */
/*																			 */
/*****************************************************************************/
												/* テストタスク				 */
#define		ETEST_SPORT		LFW_SPORT_MIN + 144	/*	 Server用Port番号		 */
#define		ETEST_CPORT		LFW_CPORT_MIN + 144	/*	 Client用Port番号		 */
#define		ETEST_PRI		65 	 				/*	 タスク優先度			 */
#define		ETEST_PCS_SPORT	LFW_SPORT_MIN + 145	/*	 Server用Port番号		 */
#define		ETEST_PCS_CPORT	LFW_CPORT_MIN + 145	/*	 Client用Port番号		 */
#define		ETEST_PRI		65 	 				/*	 タスク優先度			 */
												/* Field test task			 */
/*****************************************************************************/
/*																			 */
/*				   システムアプリケーション(161 - 191)						 */
/*																			 */
/*****************************************************************************/

#define		PROCESS_MAX		146					/* 最大プロセス数			*/

#define		CONVERT_SPORT2CPORT(x)	(x + (LFW_CPORT_MIN-LFW_SPORT_MIN))
#define		CONVERT_CPORT2SPORT(x)	(x - (LFW_CPORT_MIN-LFW_SPORT_MIN))


/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/


#endif /*__TASK_G_H__*/
