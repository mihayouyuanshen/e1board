/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Cao.HZ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/24												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include header File														*/
/*--------------------------------------------------------------------------*/
#include "eeep_g.h"
#include "eeep_if.h"
#include "eeep_serv_g.h"
#include "eeep_cmn_g.h"
#include "dlog_g.h"
#include <string.h>

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
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
slong eeep_preRead(uchar offset, uchar* data, ushort data_len)
{
	if(NULL == data)
	{
		return EEEP_R_EPRM;
	}
	else
	{
		return eeep_ReadData(offset, data, data_len);
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
/*	history			:2023/04/25 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
slong eeep_preWrite(EEEP_WriteOpId_t op, uchar* data, ushort data_len)
{
	EEEP_SetMsg_t msgSet;
	if(data == NULL)
	{
		return;
	}
	memset(&msgSet, 0, sizeof(msgSet));
	msgSet.header.msgId = op;
	memcpy(&msgSet.data, data, data_len);
	FRM_MsgSendAsync(EEEP_SPORT, sizeof(msgSet), &msgSet);
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
/*	history			:2023/04/24 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
slong EEEP_GetMountFailTimes(uchar *buf)
{
	return eeep_preRead(EEEP_OFS_MOUNT_FAILED_TIMES, buf, EEEP_BYTE_LEN);
}
slong EEEP_GetBK0FailTimes(uchar *buf)
{
	return eeep_preRead(EEEP_OFS_BANK0_FAILED_TIMES, buf, EEEP_BYTE_LEN);
}
slong EEEP_GetBK1FailTimes(uchar *buf)
{
	return eeep_preRead(EEEP_OFS_BANK1_FAILED_TIMES, buf, EEEP_BYTE_LEN);
}
slong EEEP_GetBoardName(uchar *buf)
{
	return eeep_preRead(EEEP_OFS_MODEL_INFO, buf, EEEP_BOARD_LEN);
}
slong EEEP_GetBoardSeries(uchar *buf)
{
	return eeep_preRead(EEEP_OFS_MODEL_SERIES, buf, EEEP_BOARD_LEN);
}
slong EEEP_GetBK0FWVer(uchar *buf)
{
	return eeep_preRead(EEEP_OFS_FW_BANK0_VER, buf, EEEP_FW_LEN);
}
slong EEEP_GetBK1FWVer(uchar *buf)
{
	return eeep_preRead(EEEP_OFS_FW_BANK1_VER, buf, EEEP_FW_LEN);
}
slong EEEP_GetWireMac(uchar *buf)
{
	return eeep_preRead(EEEP_OFS_MAC_ADDR_WIRE, buf, EEEP_MAC_LEN);
}
slong EEEP_GetWirelessMac(uchar *buf)
{
	return eeep_preRead(EEEP_OFS_MAC_ADDR_WIRELESS, buf, EEEP_MAC_LEN);
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
/*	history			:2023/04/24 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void EEEP_SetBK0FWVer(uchar *buf)
{
	eeep_preWrite(EEEP_WRITE_OP_BK0FW, buf, EEEP_FW_LEN);
}

void EEEP_SetBK1FWVer(uchar *buf)
{
	eeep_preWrite(EEEP_WRITE_OP_BK1FW, buf, EEEP_FW_LEN);
}

void EEEP_SetWireMac(uchar *buf)
{
	eeep_preWrite(EEEP_WRITE_OP_WIREMAC, buf, EEEP_MAC_LEN);
}

void EEEP_SetWirelessMac(uchar *buf)
{
	eeep_preWrite(EEEP_WRITE_OP_WLESSMAC, buf, EEEP_MAC_LEN);
}

/*End Of File*/
