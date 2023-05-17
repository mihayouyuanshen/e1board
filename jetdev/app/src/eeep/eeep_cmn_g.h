/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Cao.HZ												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/22												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EEEP_CMN_G_H__
#define __EEEP_CMN_G_H__
/*--------------------------------------------------------------------------*/
/*	Include header File														*/
/*--------------------------------------------------------------------------*/
#include "typedef.h"
#include "frm_g.h"
#include "eeep_g.h"

#define DATA_MAX_SIZE (ushort)(16)
#define EEEP_MSGID_START (ulong)(99)
/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef enum{
	EEEP_WRITE_OP_BK0FW = 0u,
	EEEP_WRITE_OP_BK1FW,
	EEEP_WRITE_OP_WIREMAC,
	EEEP_WRITE_OP_WLESSMAC,
	EEEP_WRITE_OP_MAX
}EEEP_WriteOpId_t;

typedef struct{
	EEEP_WriteOpId_t opID;
	uchar offset;
	ushort len;
}EEEP_WrCmdTbl_t;

typedef struct{
	FrmMsgHead_t header;
	uchar data[DATA_MAX_SIZE];
}EEEP_SetMsg_t;

typedef struct{
	FrmMsgHead_t header;
	slong fact;
}EEEP_Start_t;

#endif /*__EEEP_CMN_G_H__*/
