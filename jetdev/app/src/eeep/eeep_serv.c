/****************************************************************************/
/*	Description		:EEPROM管理プロセス内部関数ソースコード						*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Cao.HZ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/23												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
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
#include "common_g.h"
#include "typedef.h"
#include "task_g.h"
#include "lfw_g.h"
#include "dlog_g.h"
#include "eeep_serv.h"
#include "eeep_serv_g.h"
#include "eeep_cmn_g.h"
#include "eled_g.h"
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
uchar s_eeep_RomData[EEEP_BANK_SZ];	/* EEPROMデータ読み出しバッファ */
schar s_eeep_readFlag = EEEP_READ_OK;
pthread_mutex_t s_eeep_rwMutex;
EEEP_WrCmdTbl_t g_eeep_wrcmd_tbl[EEEP_WRITE_OP_MAX] = {
	{EEEP_WRITE_OP_BK0FW,		EEEP_OFS_FW_BANK0_VER,		EEEP_FW_LEN},
	{EEEP_WRITE_OP_BK1FW,		EEEP_OFS_FW_BANK1_VER,		EEEP_FW_LEN},
	{EEEP_WRITE_OP_WIREMAC,		EEEP_OFS_MAC_ADDR_WIRE,		EEEP_MAC_LEN},
	{EEEP_WRITE_OP_WLESSMAC,	EEEP_OFS_MAC_ADDR_WIRELESS,	EEEP_MAC_LEN}
};
/*----------------------------------------------------------------------------*/
/* 公開関数定義                                                                */
/*----------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:EEPROM管理プロセス起動時処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:void													*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void eeep_Init(void)
{
	slong ret = 0;
	FILE *fp = NULL;
	
	CMN_MutexInit(&s_eeep_rwMutex);
	/* グローバル変数初期化 */
	memset(s_eeep_RomData, 0xFF, EEEP_BANK_SZ);

	system("echo 42 > /sys/class/gpio/export;echo out > /sys/class/gpio/gpio42/direction");	

	fp = fopen(EEEP_DEV_PATH, "r+b");
	if(NULL == fp)
	{
		return;
	}
	ret = eeep_Check2Bank(fp);
	if(EEEP_2B_NG == ret)
	{
		DLOG_Error("eeep_Init 2 bank ng [to eeep_Recover()].\n");
		eeep_Recover(fp);
		eeep_Check2Bank(fp);
	}
	else if(EEEP_B0_OK == ret)
	{
		eeep_CopyBank(fp, 1, 0);
	}
	else if(EEEP_B1_OK == ret)
	{
		eeep_CopyBank(fp, 0, 1);
	}
	else
	{
		DLOG_Debug("eeep_Init [OK].\n");
	}
	fclose(fp);
}

/*==========================================================================*/
/*	Description		:データ書き込み											*/
/*--------------------------------------------------------------------------*/
/*	param			:uchar offset		Bank先頭からのオフセット
					:uchar*	data		書き込みデータバッファ
					:uchar	data_len	書き込みデータ長					*/
/*--------------------------------------------------------------------------*/
/*	return			:slong ret 処理結果										*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
slong eeep_WriteData(uchar offset, uchar* data, ushort data_len)
{
	slong ret = EEEP_R_OK;
	ushort cnt;
	uchar crc_buff[EEEP_CRC_LEN] = {0};
	ulong crc = 0;
	ushort bankOffset = 0;
	size_t retCnt = 0;
	FILE *fp = NULL;
	
	fp = fopen(EEEP_DEV_PATH, "r+b");
	if(NULL == fp)
	{
		return EEEP_R_EPRM;
	}

	CMN_MutexLock(&s_eeep_rwMutex);
	memcpy(&s_eeep_RomData[offset], data, data_len);
	crc = CMN_Crc32(s_eeep_RomData, EEEP_ALL_DATA_LEN);
	CMN_Crc32L2Buf(crc, crc_buff);
	CMN_MutexUnlock(&s_eeep_rwMutex);

	for(cnt=0u; cnt<EEEP_BANK_NUM; cnt++)
	{
		/* オフセット算出 */
		bankOffset = cnt * EEEP_BANK_SZ;
		eeep_WriteFile(fp, bankOffset + offset, data, data_len);
		eeep_WriteFile(fp, bankOffset + EEEP_OFS_CRC, crc_buff, EEEP_CRC_LEN);
	}
	ret = eeep_Check2Bank(fp);
	if(EEEP_R_OK != ret)
	{
		/*send EEPROM write failed event log.*/
		DLOG_Error("send EEPROM write failed event log.");
	}
	else
	{
		/*do nothing*/
	}
	fclose(fp);
	return ret;
}

/*==========================================================================*/
/*	Description		:データ読み出し											*/
/*--------------------------------------------------------------------------*/
/*	param			:uchar offset Bank先頭からのオフセット
					:uchar* data 読み出しデータ格納バッファ
					:uchar data_len 読み出しデータ長						*/
/*--------------------------------------------------------------------------*/
/*	return			:slong ret 処理結果										*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
slong eeep_ReadData(uchar offset, uchar* data, ushort data_len)
{
	slong ret;

	CMN_MutexLock(&s_eeep_rwMutex);
	if(eeep_CrcCheck(s_eeep_RomData))
	{
		/* データ読み出し */
		memcpy(data, &s_eeep_RomData[offset], data_len);
		ret = EEEP_R_OK;
		if(EEER_READ_NG == s_eeep_readFlag)
		{
			/*CLEAR ERROR CODE*/
			s_eeep_readFlag = EEEP_READ_OK;
			DLOG_Debug("CLEAR ERROR CODE.\n");
		}
		else
		{
			DLOG_Debug("Read [OK].\n");
		}
	}
	else
	{
		ret = EEEP_R_EFAIL;
		if(EEEP_READ_OK == s_eeep_readFlag)
		{
			/*ERROR CODE*/
			s_eeep_readFlag = EEER_READ_NG;
			DLOG_Error("ERROR CODE.\n");
		}
		else
		{
			DLOG_Error("Read [NG].\n");
		}
	}
	CMN_MutexUnlock(&s_eeep_rwMutex);

	return ret;
}
/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
slong eeep_Check2Bank(FILE *fp)
{
	slong ret;
	bool_t bank_ok[EEEP_BANK_NUM];
	uchar tmpBank[EEEP_BANK_NUM][EEEP_BANK_SZ] = {0};
	/* Bankデータ読み出し&チェック */
	eeep_ReadFile(fp, 0, tmpBank[0], EEEP_BANK_SZ);
	bank_ok[0] = eeep_CrcCheck(&tmpBank[0]);
	eeep_ReadFile(fp, EEEP_BANK_SZ, tmpBank[1], EEEP_BANK_SZ);
	bank_ok[1] = eeep_CrcCheck(&tmpBank[1]);
	CMN_MutexLock(&s_eeep_rwMutex);
	/* 両方のBankが正常の場合 */
	if(bank_ok[0] && bank_ok[1])
	{
		memcpy(s_eeep_RomData, &tmpBank[0], EEEP_BANK_SZ);
		DLOG_Debug("[BANK 0]:[OK] [BANK 1]:[OK]\n");
		ret = EEEP_R_OK;
	}
	else if(bank_ok[0] == TRUE && bank_ok[1] == FALSE)
	{
		memcpy(s_eeep_RomData, &tmpBank[0], EEEP_BANK_SZ);
		DLOG_Error("[BANK 0]:[OK] [BANK 1]:[NG]\n");
		ret = EEEP_B0_OK;
	}
	else if(bank_ok[0] == FALSE && bank_ok[1] == TRUE)
	{
		memcpy(s_eeep_RomData, &tmpBank[1], EEEP_BANK_SZ);
		DLOG_Error("[BANK 0]:[NG] [BANK 1]:[OK]\n");
		ret = EEEP_B1_OK;
	}
	else
	{
		DLOG_Error("[BANK 0]:[NG] [BANK 1]:[NG]\n");
		ret = EEEP_2B_NG;
	}
	CMN_MutexUnlock(&s_eeep_rwMutex);
	return ret;
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/26 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void eeep_Recover(FILE *fp)
{
	uchar datBuf[EEEP_BANK_SZ] = {0xFF};
	slong fileSize = 0;
	size_t rdcnt = 0;
	size_t wrcnt = 0;
	FILE *fptmp = NULL;

	fptmp = fopen(EEEP_RECOVER_FILEPATH, "rb");
	if(NULL == fptmp)
	{
		DLOG_Error("NO INITIAL FILE.\n");
		return;
	}
	fseek(fptmp, 0, SEEK_END);
	fileSize = ftell(fptmp);
	if(EEEP_BANK_SZ != fileSize)
	{
		DLOG_Error("INITIAL FILE SIZE ERROR.\n");
		return;
	}
	eeep_ReadFile(fptmp, 0, datBuf, EEEP_BANK_SZ);
	eeep_WriteFile(fp, 0, datBuf, EEEP_BANK_SZ);
	eeep_WriteFile(fp, EEEP_BANK_SZ, datBuf, EEEP_BANK_SZ);
	fclose(fptmp);
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/25 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
size_t eeep_ReadFile(FILE *fp, uchar ofs, uchar* buf, ushort len)
{
	fseek(fp, ofs, SEEK_SET);
	return fread(buf, 1, len, fp);
}
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/26 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
size_t eeep_WriteFile(FILE *fp, uchar ofs, uchar* buf, ushort len)
{
	fseek(fp, ofs, SEEK_SET);
	return fwrite(buf, 1, len, fp);
}
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/26 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
bool_t eeep_CrcCheck(uchar* buf)
{
	ulong crc = 0;
	crc = CMN_Crc32(buf, EEEP_ALL_DATA_LEN);
	if(crc == CMN_Crc32Buf2L(buf + EEEP_OFS_CRC))
	{
		DLOG_Debug("ram crc check [OK].\n");
		return TRUE;
	}
	else
	{
		DLOG_Error("ram crc check [NG].\n");
		return FALSE;
	}
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/27 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
slong eeep_CopyBank(FILE *fp, uchar dest, uchar src)
{
	slong ret = EEEP_R_EFAIL;
	size_t cpyCnt = 0;
	uchar tmpBuf[EEEP_BANK_SZ] = {0};

	eeep_ReadFile(fp, src * EEEP_BANK_SZ, tmpBuf, EEEP_BANK_SZ);
	cpyCnt = eeep_WriteFile(fp, dest * EEEP_BANK_SZ, tmpBuf, EEEP_BANK_SZ);
	if(EEEP_BANK_SZ == cpyCnt)
	{
		ret = EEEP_R_OK;
		DLOG_Debug("Bank [%d] copy to Bank[%d] success.\n", src, dest);
	}
	else
	{
		DLOG_Error("Bank [%d] copy to Bank[%d] failed.\n", src, dest);
	}
	return ret;
}

/* End Of File */
