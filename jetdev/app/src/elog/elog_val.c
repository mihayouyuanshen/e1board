/******************************************************************************/
/*    Description    :ログ管理ミドル グローバル変数                             */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC Jiang.Z                                             */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/03/21                                              */
/*----------------------------------------------------------------------------*/
/*    Module Name    :                                                        */
/*----------------------------------------------------------------------------*/
/*    Note           :                                                        */
/*----------------------------------------------------------------------------*/
/*    Copyright      :(c) 2020 NICHICON CORPORATION                           */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Include Header File                                                        */
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "typedef.h"
#include "elog_g.h"
#include "task_g.h"
#include "elog_val_g.h"
#include "dlog_g.h"
#include "elog_utl_g.h"

/*----------------------------------------------------------------------------*/
/* 公開変数定義                                                                */
/*----------------------------------------------------------------------------*/
ElogVat_t *s_elog = NULL;
#ifndef ELOG_INTERFACE

/*下記テーブルを修正する場合はelog_val.hの*/ 
/*#define ELOG_OUTPUT_LOG_PROC_NUMの設定値も合わせて修正すること*/
const ElogTaskName_t s_ElogTaskName[PROCESS_MAX]={
	{"AIN", AIN_SPORT},			/* 0 	*/
	{"ASMP", ASMP_SPORT},		/* 1 	*/
	{"AGUI", AGUI_SPORT},		/* 2 	*/
	{"DUMMY", 0xFFFFFFFF},		/* 3 	*/
	{"DUMMY", 0xFFFFFFFF},		/* 4 	*/
	{"DUMMY", 0xFFFFFFFF},		/* 5 	*/
	{"DUMMY", 0xFFFFFFFF},		/* 6 	*/
	{"DUMMY", 0xFFFFFFFF},		/* 7 	*/
	{"DUMMY", 0xFFFFFFFF},		/* 8 	*/
	{"DUMMY", 0xFFFFFFFF},		/* 9 	*/
	{"DUMMY", 0xFFFFFFFF},		/* 10	*/
	{"DUMMY", 0xFFFFFFFF},		/* 11	*/
	{"DUMMY", 0xFFFFFFFF},		/* 12	*/
	{"DUMMY", 0xFFFFFFFF},		/* 13	*/
	{"DUMMY", 0xFFFFFFFF},		/* 14	*/
	{"DUMMY", 0xFFFFFFFF},		/* 15	*/
	{"DTM", DTM_SPORT},			/* 16	*/
	{"DUMMY", 0xFFFFFFFF},		/* 17	*/
	{"DUMMY", 0xFFFFFFFF},		/* 18	*/
	{"DUMMY", 0xFFFFFFFF},		/* 19	*/
	{"DUMMY", 0xFFFFFFFF},		/* 20	*/
	{"DUMMY", 0xFFFFFFFF},		/* 21	*/
	{"DUMMY", 0xFFFFFFFF},		/* 22	*/
	{"DUMMY", 0xFFFFFFFF},		/* 23	*/
	{"DUMMY", 0xFFFFFFFF},		/* 24	*/
	{"DUMMY", 0xFFFFFFFF},		/* 25	*/
	{"DUMMY", 0xFFFFFFFF},		/* 26	*/
	{"DUMMY", 0xFFFFFFFF},		/* 27	*/
	{"DUMMY", 0xFFFFFFFF},		/* 28	*/
	{"DUMMY", 0xFFFFFFFF},		/* 29	*/
	{"DUMMY", 0xFFFFFFFF},		/* 30	*/
	{"DUMMY", 0xFFFFFFFF},		/* 31	*/
	{"DUMMY", 0xFFFFFFFF},		/* 32	*/
	{"DUMMY", 0xFFFFFFFF},		/* 33	*/
	{"DUMMY", 0xFFFFFFFF},		/* 34	*/
	{"DUMMY", 0xFFFFFFFF},		/* 35	*/
	{"DUMMY", 0xFFFFFFFF},		/* 36	*/
	{"DUMMY", 0xFFFFFFFF},		/* 37	*/
	{"DUMMY", 0xFFFFFFFF},		/* 38	*/
	{"DUMMY", 0xFFFFFFFF},		/* 39	*/
	{"DUMMY", 0xFFFFFFFF},		/* 40	*/
	{"DUMMY", 0xFFFFFFFF},		/* 41	*/
	{"DUMMY", 0xFFFFFFFF},		/* 42	*/
	{"DUMMY", 0xFFFFFFFF},		/* 43	*/
	{"DUMMY", 0xFFFFFFFF},		/* 44	*/
	{"DUMMY", 0xFFFFFFFF},		/* 45	*/
	{"DUMMY", 0xFFFFFFFF},		/* 46	*/
	{"DUMMY", 0xFFFFFFFF},		/* 47	*/
	{"DUMMY", 0xFFFFFFFF},		/* 48	*/
	{"MSMP", MSMP_SPORT},		/* 49 	*/
	{"EPCS", EINV_SPORT},		/* 50 	*/
	{"ENSV", ENSV_SPORT},		/* 51 	*/
	{"ESSV", ESSV_SPORT},		/* 52 	*/
	{"EHEM", EHEM_SPORT},		/* 53 	*/
	{"EENL", EENL_SPORT},		/* 54 	*/
	{"EMOD", EMOD_SPORT},		/* 55 	*/
	{"ELST", ELST_SPORT},		/* 56 	*/
	{"ENOT", ENOT_SPORT},		/* 57 	*/
	{"EINI", EINI_SPORT},		/* 58 	*/
	{"ELOG", ELOG_SPORT},		/* 59 	*/
	{"ETIM", ETIM_SPORT},		/* 60 	*/
	{"EWDT", EWDT_SPORT},		/* 61 	*/
	{"ELED", ELED_SPORT},		/* 62 	*/
	{"EBKL", EBKL_SPORT},		/* 63 	*/
	{"EBUZ", EBUZ_SPORT},		/* 64 	*/
	{"EEEP", EEEP_SPORT},		/* 65 	*/
	{"ETCH", ETCH_SPORT},		/* 66 	*/
	{"ECAP", ECAP_SPORT},		/* 67 	*/
	{"ENET", ENET_SPORT},		/* 68 	*/
	{"EFPG", EFPG_SPORT},		/* 69	*/
	{"EGRA", EGRA_SPORT},		/* 70	*/
	{"EHGW", EHGW_SPORT},		/* 71	*/
	{"EGUI", EGUI_SPORT},		/* 72	*/
	{"ENSS", ENSV_SUB_SPORT},	/* 73	*/
	{"ENSC", ENSC_SPORT},		/* 74	*/
	{"ESCH", ESCH_SPORT},		/* 75	*/
	{"EFUP", EFUP_SPORT},		/* 76	*/
	{"EEPC", EEPC_SPORT},		/* 77	*/
	{"EREP", EREP_SPORT},		/* 78	*/
	{"DUMMY", 0xFFFFFFFF},		/* 79	*/
	{"DUMMY", 0xFFFFFFFF},		/* 80	*/
	{"DUMMY", 0xFFFFFFFF},		/* 81	*/
	{"DUMMY", 0xFFFFFFFF},		/* 82	*/
	{"DUMMY", 0xFFFFFFFF},		/* 83	*/
	{"DUMMY", 0xFFFFFFFF},		/* 84	*/
	{"DUMMY", 0xFFFFFFFF},		/* 85	*/
	{"DUMMY", 0xFFFFFFFF},		/* 86	*/
	{"DUMMY", 0xFFFFFFFF},		/* 87	*/
	{"DUMMY", 0xFFFFFFFF},		/* 88	*/
	{"DUMMY", 0xFFFFFFFF},		/* 89	*/
	{"DUMMY", 0xFFFFFFFF},		/* 90	*/
	{"DUMMY", 0xFFFFFFFF},		/* 91	*/
	{"DUMMY", 0xFFFFFFFF},		/* 92	*/
	{"DUMMY", 0xFFFFFFFF},		/* 93	*/
	{"DUMMY", 0xFFFFFFFF},		/* 94	*/
	{"DUMMY", 0xFFFFFFFF},		/* 95	*/
	{"DUMMY", 0xFFFFFFFF},		/* 96	*/
	{"DUMMY", 0xFFFFFFFF},		/* 97	*/
	{"DUMMY", 0xFFFFFFFF},		/* 98	*/
	{"DUMMY", 0xFFFFFFFF},		/* 99	*/
	{"DUMMY", 0xFFFFFFFF},		/* 100	*/
	{"DUMMY", 0xFFFFFFFF},		/* 101	*/
	{"DUMMY", 0xFFFFFFFF},		/* 102	*/
	{"DUMMY", 0xFFFFFFFF},		/* 103	*/
	{"DUMMY", 0xFFFFFFFF},		/* 104	*/
	{"DUMMY", 0xFFFFFFFF},		/* 105	*/
	{"DUMMY", 0xFFFFFFFF},		/* 106	*/
	{"DUMMY", 0xFFFFFFFF},		/* 107	*/
	{"DUMMY", 0xFFFFFFFF},		/* 108	*/
	{"DUMMY", 0xFFFFFFFF},		/* 109	*/
	{"DUMMY", 0xFFFFFFFF},		/* 110	*/
	{"DUMMY", 0xFFFFFFFF},		/* 111	*/
	{"DUMMY", 0xFFFFFFFF},		/* 112	*/
	{"DUMMY", 0xFFFFFFFF},		/* 113	*/
	{"DUMMY", 0xFFFFFFFF},		/* 114	*/
	{"DUMMY", 0xFFFFFFFF},		/* 115	*/
	{"DUMMY", 0xFFFFFFFF},		/* 116	*/
	{"DUMMY", 0xFFFFFFFF},		/* 117	*/
	{"DUMMY", 0xFFFFFFFF},		/* 118	*/
	{"DUMMY", 0xFFFFFFFF},		/* 119	*/
	{"DUMMY", 0xFFFFFFFF},		/* 120	*/
	{"DUMMY", 0xFFFFFFFF},		/* 121	*/
	{"DUMMY", 0xFFFFFFFF},		/* 122	*/
	{"DUMMY", 0xFFFFFFFF},		/* 123	*/
	{"DUMMY", 0xFFFFFFFF},		/* 124	*/
	{"DUMMY", 0xFFFFFFFF},		/* 125	*/
	{"DUMMY", 0xFFFFFFFF},		/* 126	*/
	{"DUMMY", 0xFFFFFFFF},		/* 127	*/
	{"DUMMY", 0xFFFFFFFF},		/* 128	*/
	{"DUMMY", 0xFFFFFFFF},		/* 129	*/
	{"DUMMY", 0xFFFFFFFF},		/* 130	*/
	{"DUMMY", 0xFFFFFFFF},		/* 131	*/
	{"DUMMY", 0xFFFFFFFF},		/* 132	*/
	{"DUMMY", 0xFFFFFFFF},		/* 133	*/
	{"DUMMY", 0xFFFFFFFF},		/* 134	*/
	{"DUMMY", 0xFFFFFFFF},		/* 135	*/
	{"DUMMY", 0xFFFFFFFF},		/* 136	*/
	{"DUMMY", 0xFFFFFFFF},		/* 137	*/
	{"DUMMY", 0xFFFFFFFF},		/* 138	*/
	{"DUMMY", 0xFFFFFFFF},		/* 139	*/
	{"DUMMY", 0xFFFFFFFF},		/* 140	*/
	{"DUMMY", 0xFFFFFFFF},		/* 141	*/
	{"DUMMY", 0xFFFFFFFF},		/* 142	*/
	{"DUMMY", 0xFFFFFFFF},		/* 143	*/
	{"TEST", ETEST_SPORT},		/* 144	*/
	{"TPCS", ETEST_PCS_SPORT}	/* 145	*/
};

schar s_elogRmcSerialNo[ELOG_SERIES_LEN + 1];

/* グローバル変数 */
extern pthread_mutex_t s_elogSysCmdMutx;			/* system()実行排他用 */

/*----------------------------------------------------------------------------*/
/* 非公開変数定義                                                              */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 公開関数定義                                                                */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 非公開関数定義                                                              */
/*----------------------------------------------------------------------------*/


/*============================================================================*/
/*    Description    :変数を初期化する                                         */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/21  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_ValInit()
{
	slong ret = ELOG_OK;
	slong i;
	uchar data[ELOG_SERIES_LEN];
	DEBUG_TRCIN();

	ret = ELOG_Shmget();
	if(ret != ELOG_OK)
	{
		/*Do nothing*/
	}
	else
	{
		for(i=0;i<PROCESS_MAX;i++)
		{
			if(s_ElogTaskName[i].sport != 0xFFFFFFFF)
			{
				/*出力の対象なら*/
				/*出力Onをデフォルト設定*/
				s_elog->logEnabled[i] = TRUE;
			}
			else
			{
				s_elog->logEnabled[i] = FALSE;
			}
		}

		for(i=0;i<PROCESS_MAX;i++)
		{
			s_elog->logLevel[i] = ELOG_LV_ERROR;
		}

		memset(s_elogRmcSerialNo, 0x00, sizeof(s_elogRmcSerialNo));
		EEEP_GetBoardSeries(data);
		snprintf(s_elogRmcSerialNo, sizeof(s_elogRmcSerialNo), "%s", data);
		CMN_MutexInit(&s_elogSysCmdMutx);
	}
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :logenabledメンバに値をセットする                          */
/*----------------------------------------------------------------------------*/
/*    param          :bool_t    enable                                        */
/*----------------------------------------------------------------------------*/
/*                   :slong    msgid                                          */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/21  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_ValSetEnable(bool_t enable, slong msgid)
{
	DEBUG_TRCIN();
	if(msgid >= LFW_SPORT_MIN && msgid < (LFW_SPORT_MIN + PROCESS_MAX))
	{
		if(s_elog != NULL)
		{
			s_elog->logEnabled[msgid-LFW_SPORT_MIN] = enable;
		}
	}
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :全プロセスのlogenabledメンバに値をセットする               */
/*----------------------------------------------------------------------------*/
/*    param          :bool_t	enable                                        */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/21  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_ValSetEnableAll(bool_t enable)
{
	slong i;
	DEBUG_TRCIN();
	if(s_elog != NULL)
	{
		for(i=0;i<PROCESS_MAX;i++)
		{
			s_elog->logEnabled[i] = enable;
		}
	}
	DEBUG_TRCOUT();
}

#endif /* not ELOG_INTERFACE */

/*============================================================================*/
/*    Description    :logenabledメンバから値を取得する                          */
/*----------------------------------------------------------------------------*/
/*    param          :slong    msgid                                          */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:無効設定値/TRUE:有効)                             */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/21  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
bool_t ELOG_ValGetEnable(slong msgid)
{
	bool_t ret = TRUE;
	DEBUG_TRCIN();
	if(msgid >= LFW_SPORT_MIN && msgid < (LFW_SPORT_MIN + PROCESS_MAX))
	{
		if(s_elog != NULL)
		{
			ret = (s_elog->logEnabled[msgid-LFW_SPORT_MIN]);
		}
		else
		{
			ret = FALSE;
		}
	}
	else
	{
		ret = FALSE;
	}
	DEBUG_TRCOUT();
	return ret;
}

#ifndef ELOG_INTERFACE

/*============================================================================*/
/*    Description    :logenabledメンバから値を取得する(全ポートOR)               */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (bool_t    有効/無効設定値)                              */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/21  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
bool_t ELOG_ValGetEnableAll(void)
{
	slong i;
	bool_t ret = FALSE;
	DEBUG_TRCIN();
	if(s_elog != NULL)
	{
		for(i=0;i<PROCESS_MAX;i++)
		{
			if(s_elog->logEnabled[i] == TRUE)
			{
				ret = TRUE;
			}
		}
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :loglevelメンバに値をセットする                            */
/*----------------------------------------------------------------------------*/
/*    param          : ElogLv_t    level                                     */
/*----------------------------------------------------------------------------*/
/*                   : slong    msgid                                         */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/21  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_ValSetLevel(ElogLv_t level, slong msgid)
{
	DEBUG_TRCIN();
	if(msgid >= LFW_SPORT_MIN && msgid < (LFW_SPORT_MIN + PROCESS_MAX))
	{
		if(s_elog != NULL)
		{
			s_elog->logLevel[msgid-LFW_SPORT_MIN] = level;
		}
	}
	DEBUG_TRCOUT();
}

#endif /* not ELOG_INTERFACE */

/*============================================================================*/
/*    Description    :loglevelメンバから値を取得する                            */
/*----------------------------------------------------------------------------*/
/*    param          :slong    msgid                                          */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ElogLv_t    ログレベル                                  */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/21  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
ElogLv_t ELOG_ValGetLevel(slong msgid)
{
	slong ret;
	DEBUG_TRCIN();
	if(msgid >= LFW_SPORT_MIN && msgid < (LFW_SPORT_MIN + PROCESS_MAX))
	{
		if(s_elog != NULL)
		{
			ret = s_elog->logLevel[msgid-LFW_SPORT_MIN];
		}
		else
		{
			ret = ELOG_LV_FATAL;
		}
	}
	else
	{
		ret = ELOG_LV_FATAL;
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :USBの使用状態をセットする                                 */
/*----------------------------------------------------------------------------*/
/*    param          :bool_t    usbInProgress                                 */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    (false:異常発生無/TRUE:異常発生有)                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/21  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_ValSetUsbInProgress(bool_t usbInProgress)
{
	DEBUG_TRCIN();
	if(s_elog != NULL)
	{
		s_elog->usbInProgress = usbInProgress;
	}
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :loglevelメンバから値を取得する                            */
/*----------------------------------------------------------------------------*/
/*    param          : slong    msgid                                         */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ElogLv_t    ログレベル                                  */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/21  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
bool_t ELOG_ValGetUsbInProgress(void)
{
	bool_t ret;
	DEBUG_TRCIN();
	if(s_elog != NULL)
	{
		ret = s_elog->usbInProgress;
	}
	else
	{
		ret = FALSE;
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :変数を表示                                               */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :void                                                    */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/21  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
void ELOG_ValPrintVariable(void)
{
	slong i;
	DEBUG_TRCIN();
	if(s_elog != NULL)
	{
		for(i=0;i<PROCESS_MAX;i++)
		{
			/*DLOG_Error("s_elog.logEnabled[%03d]=[%d]\n",
			(int)i,(int)s_elog->logEnabled[i]);*/
		}
		for(i=0;i<PROCESS_MAX;i++)
		{
			/*DLOG_Error("s_elog.logLevel[%03d]=[%d]\n",
			(int)i,(int)s_elog->logLevel[i]);*/
		}
		DLOG_Error("s_elog.usbInProgress=[%d]\n",(int)s_elog->usbInProgress);
	}
	DEBUG_TRCOUT();
}

/*============================================================================*/
/*    Description    :共有メモリ取得                                           */
/*----------------------------------------------------------------------------*/
/*    param          :void                                                    */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*                    ELOG_OK(0)    正常終了                                   */
/*                    ELOG_E_SYSCAL(-2)    システムコールエラー                 */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/21  : DHC Jang.Z : 新規作成                      */
/*============================================================================*/
slong ELOG_Shmget(void)
{
	int id;
	slong ret = ELOG_OK;
	DEBUG_TRCIN();
	if(s_elog != NULL)
	{
		ret = ELOG_OK;
	}
	else
	{
		#ifndef ELOG_INTERFACE
			id = shmget((key_t)ELOG_SPORT, sizeof(ElogVat_t), IPC_CREAT|0666);
		#else
			id = shmget((key_t)ELOG_SPORT, sizeof(ElogVat_t), 0666);
		#endif

		if(id < 0)
		{
			ret = ELOG_E_SYSCAL;
		}
		else
		{
			s_elog = shmat(id, NULL, 0);
			if(s_elog == (void*)-1)
			{
				s_elog = NULL;
				ret = ELOG_E_SYSCAL;
			}
			else
			{
				/*Do nothing*/
			}
		}
	}
	return ret;
}


/* End Of File */