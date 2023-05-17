/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Cao.HZ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/19												*/
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
#include "eeep_g.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "typedef.h"
#include "common_g.h"
#include "dlog_g.h"
#include "etest_shell_g.h"
/*----------------------------------------------------------------------------*/
/* 公開変数定義                                                                */
/*----------------------------------------------------------------------------*/
#define EDAT_CMD_TBL_SIZE ((uchar)2)
void eeepSetTest(slong argc, schar *agrv[]);
void eeepGetTest(slong argc, schar *agrv[]);

ETEST_SubCmdItem_t s_ETestEeepSubCmdTbl[EDAT_CMD_TBL_SIZE] = {
	{"eepset", "eepset", eeepSetTest},
	{"eepget", "eepset", eeepGetTest}
};

ETEST_CmdItem_t g_ETestEeepCmdItem = {
	"eeep",
	"eeep",
	s_ETestEeepSubCmdTbl,
	ARRAY_SIZE(s_ETestEeepSubCmdTbl)
};
/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
void eeepSetTest(slong argc, schar *agrv[])
{
	uchar wireMac[6] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
	uchar wirelessMac[6] = {0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa};
	printf("[Set BANK0 FW Version 230522-00]\n");
	EEEP_SetBK0FWVer("230522-00");
	printf("[Set BANK0 FW Version 230522-01]\n");
	EEEP_SetBK1FWVer("230522-01");
	printf("[Set WireMac Address aa:bb:cc:dd:ee:ff]\n");
	EEEP_SetWireMac(wireMac);
	printf("[Set WirelessMac Address ff:ee:dd:cc:bb:aa]\n");
	EEEP_SetWirelessMac(wirelessMac);
	fflush(stdout);
}

void eeepGetTest(slong argc, schar *agrv[])
{
	uchar data[16] = {0};

	EEEP_GetMountFailTimes(data);
	printf("[READ EEPROM MountFailTimes] : [%d]\n", data[0]);

	EEEP_GetBK0FailTimes(data);
	printf("[READ EEPROM EEEP_GetBK0FailTimes] : [%d]\n", data[0]);

	EEEP_GetBK1FailTimes(data);
	printf("[READ EEPROM EEEP_GetBK1FailTimes] : [%d]\n", data[0]);

	EEEP_GetBK0FWVer(data);
	printf("[READ EEPROM FW BK0 VER] : [%9s]\n", data);

	EEEP_GetBK1FWVer(data);
	printf("[READ EEPROM FW BK1 VER] : [%9s]\n", data);
	
	EEEP_GetBoardName(data);
	printf("[READ EEPROM MODEL INFO] : [%16s]\n", data);
	
	EEEP_GetBoardSeries(data);
	printf("[READ EEPROM MODEL SERIES] : [%16s]\n", data);

	EEEP_GetWireMac(data);
	printf("[READ EEPROM WIRELAN MAC] : [%02x:%02x:%02x:%02x:%02x:%02x]\n", data[0], data[1], data[2], data[3], data[4], data[5]);

	EEEP_GetWirelessMac(data);
	printf("[READ EEPROM WIRELESSLAN MAC] : [%02x:%02x:%02x:%02x:%02x:%02x]\n", data[0], data[1], data[2], data[3], data[4], data[5]);

	fflush(stdout);
}
/*End Of File*/
