/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Gu.CM												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/05/12												*/
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
#include "etest_shell_g.h"
#include "eled_g.h"
#include "esys_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
#define ETEXT_SUB_CMD_TBL_SIZE 24

ETEST_SubCmdItem_t s_ETestEledSubCmdTbl[ETEXT_SUB_CMD_TBL_SIZE];
ETEST_CmdItem_t g_ETestEledCmdItem = {
	"eled",
	"eled",
	s_ETestEledSubCmdTbl,
	ARRAY_SIZE(s_ETestEledSubCmdTbl)
};
/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
void etest_FwOtherErrLedAtc(void);
void etest_CclFwOtherErrLedAtc(void);
void etest_FwJbErrLedAtc(void);
void etest_CclFwJbErrLedAtc(void);
void etest_FwBdcErrLedAtc(void);
void etest_CclFwBdcErrLedAtc(void);
void etest_FwPvErrLedAtc(void);
void etest_CclFwPvErrLedAtc(void);
void etest_FwInvErrLedAtc(void);
void etest_CclFwInvErrLedAtc(void);
void etest_FwE1ErrLedAtc(void);
void etest_CclFwE1ErrLedAtc(void);
void etest_FwUpdateOkLedAtc(void);
void etest_CclFwUpdateOkLedAtc(void);
void etest_FwUpdateLedAtc(void);
void etest_CclFwUpdateLedAtc(void);
void etest_FwDownloadLedAtc(void);
void etest_CclFwDownloadLedAtc(void);
void etest_E1ErrLedAtc(void);
void etest_CclE1ErrLedAtc(void);
void etest_FileSysMtFailLedAtc(void);
void etest_CclFileSysMtFailLedAtc(void);
void etest_readEepFailLedAtc(void);
void etest_CclReadEepFailLedAtc(void);

ETEST_SubCmdItem_t s_ETestEledSubCmdTbl[ETEXT_SUB_CMD_TBL_SIZE] = {
	{
		"rdeeperr", "read eeprom fail led action", etest_readEepFailLedAtc
	},
	{
		"cclrdeeperr", "cancel read eeprom fail led action", 
		etest_CclReadEepFailLedAtc
	},
	{
		"filesyserr", "file system mount fail led action", etest_FileSysMtFailLedAtc
	},
	{
		"cclfilesyserr", "cancel file system mount fail led action", 
		etest_CclFileSysMtFailLedAtc
	},
	{
		"e1bferr", "E1 error:bfxxx led action", etest_E1ErrLedAtc
	},
	{
		"ccle1bferr", "cancel E1 error:bfxxx led action", etest_CclE1ErrLedAtc
	},
	{
		"fwdown", "Fw download led action", etest_FwDownloadLedAtc
	},
	{
		"cclfwdown", "cancel Fw download led action", etest_CclFwDownloadLedAtc
	},
	{
		"fwup", "FW update led action", etest_FwUpdateLedAtc
	},
	{
		"cclfwup", "cancel FW update led action", etest_CclFwUpdateLedAtc
	},
	{
		"fwupok", "FW update seccess led action", etest_FwUpdateOkLedAtc
	},
	{
		"cclfwupok", "cancel FW update seccess led action", etest_CclFwUpdateOkLedAtc
	},
	{
		"fwe1err", "FW E1 update Fail led action", etest_FwE1ErrLedAtc
	},
	{
		"cclfwe1err", "cancel FW E1 update Fail led action", etest_CclFwE1ErrLedAtc
	},
	{
		"fwinverr", "FW INV update Fail led action", etest_FwInvErrLedAtc
	},
	{
		"cclfwinverr", "cancel FW INV update Fail led action", etest_CclFwInvErrLedAtc
	},
	{
		"fwpverr", "FW PV update Fail led action", etest_FwPvErrLedAtc
	},
	{
		"cclfwpverr", "cancel FW PV update Fail led action", etest_CclFwPvErrLedAtc
	},
	{
		"fwbdcerr", "FW BDC update Fail led action", etest_FwBdcErrLedAtc
	},
	{
		"cclfwbdcerr", "cancel FW BDC update Fail led action", etest_CclFwBdcErrLedAtc
	},
	{
		"fwjberr", "FW J.B update Fail led action", etest_FwJbErrLedAtc
	},
	{
		"cclfwjberr", "cancel FW J.B update Fail led action", etest_CclFwJbErrLedAtc
	},
	{
		"fwerr", "FW update other err led action", etest_FwOtherErrLedAtc
	},
	{
		"cclfwerr", "cancel FW update other err led action", etest_CclFwOtherErrLedAtc
	}
};

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
void etest_FwOtherErrLedAtc(void)
{
	ELED_FwLedBlankStart(ELED_FWLED_ERR_OTHER);
}

void etest_CclFwOtherErrLedAtc(void)
{
	ELED_FwLedBlankStop(ELED_FWLED_ERR_OTHER);
}

void etest_FwJbErrLedAtc(void)
{
	ELED_FwLedBlankStart(ELED_FWLED_ERR_JB);
}

void etest_CclFwJbErrLedAtc(void)
{
	ELED_FwLedBlankStop(ELED_FWLED_ERR_JB);
}

void etest_FwBdcErrLedAtc(void)
{
	ELED_FwLedBlankStart(ELED_FWLED_ERR_BDC);
}

void etest_CclFwBdcErrLedAtc(void)
{
	ELED_FwLedBlankStop(ELED_FWLED_ERR_BDC);
}

void etest_FwPvErrLedAtc(void)
{
	ELED_FwLedBlankStart(ELED_FWLED_ERR_PV);
}

void etest_CclFwPvErrLedAtc(void)
{
	ELED_FwLedBlankStop(ELED_FWLED_ERR_PV);
}

void etest_FwInvErrLedAtc(void)
{
	ELED_FwLedBlankStart(ELED_FWLED_ERR_INV);
}

void etest_CclFwInvErrLedAtc(void)
{
	ELED_FwLedBlankStop(ELED_FWLED_ERR_INV);
}

void etest_FwE1ErrLedAtc(void)
{
	ELED_FwLedBlankStart(ELED_FWLED_ERR_E1BOARD);
}

void etest_CclFwE1ErrLedAtc(void)
{
	ELED_FwLedBlankStop(ELED_FWLED_ERR_E1BOARD);
}

void etest_FwUpdateOkLedAtc(void)
{
	ELED_FwLedBlankStart(ELED_FWLED_FINISH);
}

void etest_CclFwUpdateOkLedAtc(void)
{
	ELED_FwLedBlankStop(ELED_FWLED_FINISH);
}

void etest_FwUpdateLedAtc(void)
{
	ELED_FwLedBlankStart(ELED_FWLED_UPDATE);
}

void etest_CclFwUpdateLedAtc(void)
{
	ELED_FwLedBlankStop(ELED_FWLED_UPDATE);
}

void etest_FwDownloadLedAtc(void)
{
	ELED_FwLedBlankStart(ELED_FWLED_DOWNLOAD);
}

void etest_CclFwDownloadLedAtc(void)
{
	ELED_FwLedBlankStop(ELED_FWLED_DOWNLOAD);
}

void etest_E1ErrLedAtc(void)
{
	ELED_PwrLedBlankStart(ELED_PWRLED_ERROR);
}

void etest_CclE1ErrLedAtc(void)
{
	ELED_PwrLedBlankStop(ELED_PWRLED_ERROR);
}

void etest_FileSysMtFailLedAtc(void)
{
	ELED_PwrLedBlankStart(ELED_PWRLED_ERROR);
}

void etest_CclFileSysMtFailLedAtc(void)
{
	ELED_PwrLedBlankStop(ELED_PWRLED_ERROR);
}

void etest_readEepFailLedAtc(void)
{
	//ELED_PwrLedBlankStart(ELED_PWRLED_ERROR);
	ESYS_NotifyReadEEPromErr();
}

void etest_CclReadEepFailLedAtc(void)
{
	//ELED_PwrLedBlankStop(ELED_PWRLED_ERROR);
	ESYS_ClearReadEEPromErr();
}
/*End Of File*/
