/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Gu.CM												*/
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "eled_serv.h"
#include "eled_serv_g.h"
#include "eled_msg_g.h"
#include "eled_file_g.h"
#include "task_g.h"
#include "esys_g.h"
#include "dgpio_g.h"
#include "common_g.h"
#include "dlog_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
#define ELED_ORANGE_FILE_PATH	"/sys/class/leds/orange/brightness"
#define ELED_GREEN_FILE_PATH	"/sys/class/leds/green/brightness"
#define ELED_VAL_HI				"1"
#define ELED_VAL_LO				"0"
#define ELED_VAL_NONE			"none"
#define ELED_VAL_SIZE			((sint)2)
#define ELED_VAL_NONE_SIZE		((sint)5)

#define REPEAT_NONE		((ushort)0u)
#define REPEAT_ALWAYS	((ushort)0xFFFFu)

#define DELAY_NONE		((ulong)0u)
#define DELAY_ALWAYS	((ulong)0xFFFFu)

#define SEQ_INVALID		((ulong)0xFFFFu)
#define STEP_INVALID	((ulong)0xFFFFu)

#define ELED_DELAY_200_MS ((ushort)200u)
#define ELED_DELAY_400_MS ((ushort)400u)
#define ELED_DELAY_800_MS ((ushort)800u)

#define ELED_STEP_SIZE				((uchar)2u)
#define ELED_PWRRUN_STEP_SIZE		((uchar)1u)
#define ELED_SHUTDOWN_STEP_SIZE		((uchar)1u)
#define ELED_FWDOWN_STEP_SIZE		((uchar)3u)
#define ELED_FWUP_STEP_SIZE			((uchar)3u)
#define ELED_FWUP_FINI_STEP_SIZE	((uchar)4u)
#define ELED_FWUP_ERR_STEP_SIZE		((uchar)4u)
#define ELED_SEQDESC_SIZE			((uchar)1u)
#define ELED_FW_SEQDESC_SIZE		((uchar)2u)
#define ELED_MODETBL_SIZW			((uchar)15u)

sint s_ledGrFd = -1;
sint s_ledOgFd = -1;

typedef struct {
	void (*stepProc)(void);
	ulong postDelayMs;
} EledStepDesc_t;

typedef struct {
	EledStepDesc_t const *steps;
	ushort stepSize;
	ushort repeatTime;
} EledSeqDesc_t;


typedef struct {
	EledPriority_t priority;
	void (*preProc)(void);
	void (*postProc)(void);
	EledSeqDesc_t const *seqs;
	ushort seqSize;
	ushort repeatTime;
} EledModeDesc_t;

const EledStepDesc_t s_ledPwrOnStepTbl[ELED_STEP_SIZE] = {
	{eled_LedOnStepProc, 	ELED_DELAY_800_MS},
	{eled_OgLedOnStepProc,	ELED_DELAY_800_MS}
};

const EledStepDesc_t s_ledPwrRunStepTbl[ELED_PWRRUN_STEP_SIZE] = {
	{eled_LedOnStepProc,	DELAY_ALWAYS}
};

const EledStepDesc_t s_ledPwrOffStepTbl[ELED_STEP_SIZE] = {
	{eled_LedOnStepProc,	ELED_DELAY_400_MS},
	{eled_OgLedOnStepProc,	ELED_DELAY_400_MS}
};

const EledStepDesc_t s_ledTestRunStepTbl[ELED_STEP_SIZE] = {
	{eled_LedOnStepProc,	ELED_DELAY_800_MS},
	{eled_GrLedOnStepProc,	ELED_DELAY_800_MS}
};

const EledStepDesc_t s_ledShutdownStepTbl[ELED_SHUTDOWN_STEP_SIZE] = {
	{eled_GrLedOnStepProc,	DELAY_NONE}
};

const EledStepDesc_t s_ledErrStepTbl[ELED_STEP_SIZE] = {
	{eled_LedOnStepProc,	ELED_DELAY_400_MS},
	{eled_GrLedOnStepProc,	ELED_DELAY_400_MS}
};

const EledStepDesc_t s_ledFwDownStepTbl[ELED_FWDOWN_STEP_SIZE] = {
	{eled_OgLedOnStepProc,	ELED_DELAY_400_MS},
	{eled_GrLedOnStepProc,	ELED_DELAY_400_MS},
	{ELED_LedOffStepProc,	ELED_DELAY_200_MS}
};

const EledStepDesc_t s_ledFwUpStepTbl[ELED_FWUP_STEP_SIZE] = {
	{eled_OgLedOnStepProc,	ELED_DELAY_200_MS},
	{eled_GrLedOnStepProc,	ELED_DELAY_200_MS},
	{ELED_LedOffStepProc,	ELED_DELAY_200_MS}
};

const EledStepDesc_t s_ledFwUpFiniStepTbl[ELED_FWUP_FINI_STEP_SIZE] = {
	{eled_GrLedOnStepProc,	ELED_DELAY_200_MS},
	{ELED_LedOffStepProc,	ELED_DELAY_200_MS},
	{eled_GrLedOnStepProc,	1000},
	{ELED_LedOffStepProc,	ELED_DELAY_200_MS}
};

const EledStepDesc_t s_ledFwUpErrStepTbl[ELED_FWUP_ERR_STEP_SIZE] = {
	{eled_LedOnStepProc,	ELED_DELAY_200_MS},
	{ELED_LedOffStepProc,	ELED_DELAY_200_MS},
	{eled_LedOnStepProc,	1000},
	{ELED_LedOffStepProc,	ELED_DELAY_200_MS}
};

const EledSeqDesc_t s_ledPwrOnSeqTbl[ELED_SEQDESC_SIZE] = {
	{s_ledPwrOnStepTbl,		ARRAY_SIZE(s_ledPwrOnStepTbl),	REPEAT_ALWAYS}
};

const EledSeqDesc_t s_ledPwrRunSeqTbl[ELED_SEQDESC_SIZE] = {
	{s_ledPwrRunStepTbl,	ARRAY_SIZE(s_ledPwrRunStepTbl),	REPEAT_NONE}
};

const EledSeqDesc_t s_ledPwrOffSeqTbl[ELED_SEQDESC_SIZE] = {
	{s_ledPwrOffStepTbl,	ARRAY_SIZE(s_ledPwrOffStepTbl),	REPEAT_ALWAYS}
};

const EledSeqDesc_t s_ledTestRunSeqTbl[ELED_SEQDESC_SIZE] = {
	{s_ledTestRunStepTbl,	ARRAY_SIZE(s_ledTestRunStepTbl),REPEAT_ALWAYS}
};

const EledSeqDesc_t s_ledShutdownSeqTbl[ELED_SEQDESC_SIZE] = {
	{s_ledShutdownStepTbl,	ARRAY_SIZE(s_ledShutdownStepTbl), REPEAT_NONE}
};

const EledSeqDesc_t s_ledErrSeqTbl[ELED_SEQDESC_SIZE] = {
	{s_ledErrStepTbl,		ARRAY_SIZE(s_ledErrStepTbl),	REPEAT_ALWAYS}
};

const EledSeqDesc_t s_ledFwDownSeqTbl[ELED_SEQDESC_SIZE] = {
	{s_ledFwDownStepTbl,	ARRAY_SIZE(s_ledFwDownStepTbl),	REPEAT_ALWAYS}
};

const EledSeqDesc_t s_ledFwUpSeqTbl[ELED_SEQDESC_SIZE] = {
	{s_ledFwUpStepTbl,		ARRAY_SIZE(s_ledFwUpStepTbl),	REPEAT_ALWAYS}
};

const EledSeqDesc_t s_ledFwUpFiniSeqTbl[ELED_FW_SEQDESC_SIZE] = {
	{s_ledFwUpFiniStepTbl,		2u,	3u},
	{&s_ledFwUpFiniStepTbl[2],	2u,	1u}
};

const EledSeqDesc_t s_ledFwE1ErrSeqTbl[ELED_FW_SEQDESC_SIZE] = {
	{s_ledFwUpErrStepTbl,		2u,	2u},
	{&s_ledFwUpErrStepTbl[2],	2u,	1u}
};

const EledSeqDesc_t s_ledFwInvErrSeqTbl[ELED_FW_SEQDESC_SIZE] = {
	{s_ledFwUpErrStepTbl,		2u,	3u},
	{&s_ledFwUpErrStepTbl[2],	2u,	1u}
};

const EledSeqDesc_t s_ledFwPvErrSeqTbl[ELED_FW_SEQDESC_SIZE] = {
	{s_ledFwUpErrStepTbl,		2u,	4u},
	{&s_ledFwUpErrStepTbl[2],	2u,	1u}
};

const EledSeqDesc_t s_ledFwBdcErrSeqTbl[ELED_FW_SEQDESC_SIZE] = {
	{s_ledFwUpErrStepTbl,		2u,	5u},
	{&s_ledFwUpErrStepTbl[2],	2u,	1u}
};


const EledSeqDesc_t s_ledFwJbErrSeqTbl[ELED_FW_SEQDESC_SIZE] = {
	{s_ledFwUpErrStepTbl,		2u,	6u},
	{&s_ledFwUpErrStepTbl[2],	2u,	1u}
};

const EledSeqDesc_t s_ledFwOthErrSeqTbl[ELED_FW_SEQDESC_SIZE] = {
	{s_ledFwUpErrStepTbl,		2u,	7u},
	{&s_ledFwUpErrStepTbl[2],	2u,	1u}
};

const EledModeDesc_t s_modeTbl[ELED_MODETBL_SIZW] = {
	{
		ELED_PRIORITY_POWER,	eled_ChgModePreProc,	NULL,	s_ledPwrOnSeqTbl,
		ARRAY_SIZE(s_ledPwrOnSeqTbl),	REPEAT_ALWAYS
	},
	{
		ELED_PRIORITY_POWER,	eled_ChgModePreProc,	NULL,	s_ledPwrRunSeqTbl,
		ARRAY_SIZE(s_ledPwrRunSeqTbl),		REPEAT_ALWAYS
	},
	{
		ELED_PRIORITY_POWER,	eled_ChgModePreProc,	NULL,	s_ledPwrOffSeqTbl,
		ARRAY_SIZE(s_ledPwrOffSeqTbl),		REPEAT_ALWAYS
	},
	{
		ELED_PRIORITY_TESTRUN,	eled_ChgModePreProc,	NULL,	s_ledTestRunSeqTbl,
		ARRAY_SIZE(s_ledTestRunSeqTbl),	REPEAT_ALWAYS
	},
	{
		ELED_PRIORITY_SHUTDOWN,	eled_ChgModePreProc,	NULL,	s_ledShutdownSeqTbl,
		ARRAY_SIZE(s_ledShutdownSeqTbl),	REPEAT_ALWAYS
	},
	{
		ELED_PRIORITY_ERROR,	eled_ChgModePreProc,	NULL,	s_ledErrSeqTbl,
		ARRAY_SIZE(s_ledErrSeqTbl),	REPEAT_ALWAYS
	},
	{
		ELED_PRIORITY_FW,		eled_ChgModePreProc,	NULL,	s_ledFwDownSeqTbl,
		ARRAY_SIZE(s_ledFwDownSeqTbl),		REPEAT_ALWAYS
	},
	{
		ELED_PRIORITY_FW,		eled_ChgModePreProc,	NULL,	s_ledFwUpSeqTbl,
		ARRAY_SIZE(s_ledFwUpSeqTbl),		REPEAT_ALWAYS
	},
	{
		ELED_PRIORITY_FW,		eled_ChgModePreProc,	NULL,	s_ledFwUpFiniSeqTbl,
		ARRAY_SIZE(s_ledFwUpFiniSeqTbl),	REPEAT_ALWAYS
	},
	{
		ELED_PRIORITY_FW,		eled_ChgModePreProc,	NULL,	s_ledFwE1ErrSeqTbl,
		ARRAY_SIZE(s_ledFwE1ErrSeqTbl),		REPEAT_ALWAYS
	},
	{
		ELED_PRIORITY_FW,		eled_ChgModePreProc,	NULL,	s_ledFwInvErrSeqTbl,
		ARRAY_SIZE(s_ledFwInvErrSeqTbl),	REPEAT_ALWAYS
	},
	{
		ELED_PRIORITY_FW,		eled_ChgModePreProc,	NULL,	s_ledFwPvErrSeqTbl,
		ARRAY_SIZE(s_ledFwPvErrSeqTbl),		REPEAT_ALWAYS
	},
	{
		ELED_PRIORITY_FW,		eled_ChgModePreProc,	NULL,	s_ledFwBdcErrSeqTbl,
		ARRAY_SIZE(s_ledFwBdcErrSeqTbl),	REPEAT_ALWAYS
	},
	{
		ELED_PRIORITY_FW,		eled_ChgModePreProc,	NULL,	s_ledFwJbErrSeqTbl,
		ARRAY_SIZE(s_ledFwJbErrSeqTbl),		REPEAT_ALWAYS
	},
	{
		ELED_PRIORITY_FW,		eled_ChgModePreProc,	NULL,	s_ledFwOthErrSeqTbl,
		ARRAY_SIZE(s_ledFwOthErrSeqTbl),	REPEAT_ALWAYS
	}
};

#define ELED_WORKMODE_INVALID ELED_WORKMODE_MAX

eledPriMemoryPool_t s_ledPriMemory[ELED_WORKMODE_MAX];
EledWorkMode_t s_currentMode;
ushort s_currentSeq;
ushort s_currentStep;
ushort s_seqRepeatCnt;
ushort s_modeRepeatCnt;
EledPriority_t s_curPriority;
eledPriNode_t *s_LedPriListHead = NULL;

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:LED闪烁模式初始化										*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_ServInit(void)
{
	DEBUG_TRCIN();

	memset(s_ledPriMemory, 0 , sizeof(s_ledPriMemory));
	s_currentMode = ELED_WORKMODE_INVALID;
	s_curPriority = ELED_PRIORITY_POWER;

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:LED闪烁模式切换处理									*/
/*--------------------------------------------------------------------------*/
/*	param			:EledWorkMode_t targetMode LED灯闪烁模式				*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_ProcChgMode(void)
{
	DEBUG_TRCIN();

	if (s_currentMode != s_LedPriListHead->mode)
	{
		if (ELED_WORKMODE_INVALID != s_currentMode)
		{
			if (NULL != s_modeTbl[s_currentMode].postProc)
			{
				s_modeTbl[s_currentMode].postProc();
			}
		}
		else
		{
			/* 処理無し */
		}

		FRM_TimerCancelOneShot(ELED_TIMEID_TRIGGER);
		s_currentMode = s_LedPriListHead->mode;
		s_curPriority = s_modeTbl[s_currentMode].priority;

#if 0
		ELED_FileRead(ELED_FILEPATH_CTRLTBL, (schar *)&s_currentMode, sizeof(s_currentMode));
#endif

		if (NULL != s_modeTbl[s_currentMode].preProc)
		{
			s_modeTbl[s_currentMode].preProc();
		}
		s_modeRepeatCnt = 0u;
		eled_SendTriggerMsg(ELED_MSGID_FIRST_SEQ);
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:设置LED闪烁模式										*/
/*--------------------------------------------------------------------------*/
/*	param			:EledWorkMode_t targetMode LED灯闪烁模式				*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_SetLedMode(EledWorkMode_t targetMode)
{
	DEBUG_TRCIN();
	DLOG_Debug("ELED_SetLedMode:%d", targetMode);
	eled_LedPriListInsert(targetMode, s_modeTbl[targetMode].priority);
	ELED_ProcChgMode();
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:执行LED闪烁模式前顺序									*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_ProcDoFirstSeq(void)
{
	DEBUG_TRCIN();

	if ((NULL == s_modeTbl[s_currentMode].seqs) || (s_modeTbl[s_currentMode].seqSize < 1))
	{
		/* 没有sequence, 不执行 */
	}
	else
	{
		s_currentSeq = 0u;
		s_seqRepeatCnt = 1u;
		/* 执行前一步 */
		eled_SendTriggerMsg(ELED_MSGID_FIRST_STEP);
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:执行LED闪烁模式后顺序									*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_ProcDoNextSeq(void)
{
	s_currentSeq++;
	s_seqRepeatCnt = 1u;

	DEBUG_TRCIN();

	if (s_currentSeq >= s_modeTbl[s_currentMode].seqSize)
	{
		if (REPEAT_NONE == s_modeTbl[s_currentMode].repeatTime)
		{
			/* 不重复模式, 不执行 */
		}
		else if (REPEAT_ALWAYS == s_modeTbl[s_currentMode].repeatTime)
		{
			eled_SendTriggerMsg(ELED_MSGID_FIRST_SEQ);
		}
		else if (s_modeRepeatCnt < s_modeTbl[s_currentMode].repeatTime)
		{
			eled_SendTriggerMsg(ELED_MSGID_FIRST_SEQ);
			s_modeRepeatCnt++;
		}
		else
		{
			/* 不执行 */
		}
	}
	else
	{
		eled_SendTriggerMsg(ELED_MSGID_FIRST_STEP);
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:执行LED闪烁顺序中前步骤								*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_ProcDoFirstStep(void)
{
	s_currentStep = 0u;

	DEBUG_TRCIN();

	if ((NULL == s_modeTbl[s_currentMode].seqs[s_currentSeq].steps) || 
		(s_modeTbl[s_currentMode].seqs[s_currentSeq].stepSize < 1))
	{
		/* 执行下一个sequence */
		eled_SendTriggerMsg(ELED_MSGID_NEXT_SEQ);
	}
	else 
	{
		eled_ProcDoCurrentStep();
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:执行LED闪烁顺序中后步骤								*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_ProcDoNextStep(void)
{
	s_currentStep++;

	DEBUG_TRCIN();

	if (s_currentStep >= s_modeTbl[s_currentMode].seqs[s_currentSeq].stepSize)
	{
		if (REPEAT_NONE == s_modeTbl[s_currentMode].seqs[s_currentSeq].repeatTime)
		{
			/* 执行下一个sequence */
			eled_SendTriggerMsg(ELED_MSGID_NEXT_SEQ);
		}
		else if (REPEAT_ALWAYS == s_modeTbl[s_currentMode].seqs[s_currentSeq].repeatTime)
		{
			/* 执行前一步 */
			eled_SendTriggerMsg(ELED_MSGID_FIRST_STEP);
		}
		else if (s_seqRepeatCnt < s_modeTbl[s_currentMode].seqs[s_currentSeq].repeatTime)
		{
			/* 执行前一步 */
			eled_SendTriggerMsg(ELED_MSGID_FIRST_STEP);
			s_seqRepeatCnt++;
		}
		else
		{
			/* 执行下一个sequence */
			eled_SendTriggerMsg(ELED_MSGID_NEXT_SEQ);
		}
	}
	else
	{
		eled_ProcDoCurrentStep();
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:LED模块启动处理函数									*/
/*--------------------------------------------------------------------------*/
/*	param			:FrmMsgHead_t const *msg 启动参数						*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_ProcStart(FrmMsgHead_t const *msg)
{
	EledStart_t const * startMsg = (EledStart_t const *)msg;
	slong ret;
	EledWorkMode_t workMode;

	DEBUG_TRCIN();;

	if (ESYS_START_FACT_NORMAL == startMsg->fact)
	{	/* 起動時処理 */
		ELED_LedOffStepProc();
	}
	else
	{
		ret = ELED_FileRead(ELED_FILEPATH_CTRLTBL, (schar *)&workMode, sizeof(workMode));
		if (0 == ret)
		{
			ELED_SetLedMode(workMode);
		}
		else
		{
			ELED_LedOffStepProc();
		}
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:打开GPIO引脚											*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_OpenGpioStart(void)
{
	DEBUG_TRCIN();

#ifdef ELED_USE_GPIO_LEDS
	s_ledOgFd = open(ELED_ORANGE_FILE_PATH, O_WRONLY);
	s_ledGrFd = open(ELED_GREEN_FILE_PATH, O_WRONLY);
#else
	s_ledOgFd = DGPIO_Open(DGPIO_PORT_LED_ORANGE);
	s_ledGrFd = DGPIO_Open(DGPIO_PORT_LED_GREEN);
#endif

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:关闭GPIO引脚											*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_CloseGpioStop(void)
{
	DEBUG_TRCIN();

	if (-1 != s_ledOgFd)
	{
#ifdef ELED_USE_GPIO_LEDS
		close(s_ledOgFd);
#else
		DGPIO_Close(s_ledOgFd, DGPIO_PORT_LED_ORANGE);
#endif
		s_ledOgFd = -1;
	}

	if (-1 != s_ledGrFd)
	{
#ifdef ELED_USE_GPIO_LEDS
		close(s_ledGrFd);
#else
		DGPIO_Close(s_ledGrFd, DGPIO_PORT_LED_GREEN);
#endif
		s_ledGrFd = -1;
	}

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:设置橙灯和绿灯灭灯										*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_LedOffStepProc(void)
{
	DEBUG_TRCIN();

	eled_setLedOff(s_ledOgFd);
	eled_setLedOff(s_ledGrFd);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:WDT応答												*/
/*--------------------------------------------------------------------------*/
/*	param			:FrmMsgHead_t const *msg								*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_EwdtRes(FrmMsgHead_t const *msg)
{
	/* TODO */
}

/*==========================================================================*/
/*	Description		:終了応答												*/
/*--------------------------------------------------------------------------*/
/*	param			:FrmMsgHead_t const *msg								*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_EwdtTermReg(FrmMsgHead_t const *msg)
{
	/* TODO */
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
/*	history			:2023/05/12 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void ELED_LedBlinkStop(EledWorkMode_t mode)
{
	DEBUG_TRCIN();

	DLOG_Debug("ELED_LedBlinkStop=%d", mode);
	eled_PriListErase(mode);
	ELED_ProcChgMode();

	DEBUG_TRCOUT();
}
/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:LED灯开始闪烁											*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void eled_ProcDoCurrentStep(void)
{
	DEBUG_TRCIN();

	if (NULL != s_modeTbl[s_currentMode].seqs[s_currentSeq].steps[s_currentStep].stepProc)
	{
		s_modeTbl[s_currentMode].seqs[s_currentSeq].steps[s_currentStep].stepProc();
	}

	if (DELAY_NONE == s_modeTbl[s_currentMode].seqs[s_currentSeq].steps[s_currentStep].postDelayMs)
	{
		/* 执行下一步 */
		eled_SendTriggerMsg(ELED_MSGID_NEXT_STEP);
	}
	else if (DELAY_ALWAYS == s_modeTbl[s_currentMode].seqs[s_currentSeq].steps[s_currentStep].postDelayMs)
	{
		/* 不执行 */
	}
	else
	{
		/* 开始定时 */
		FRM_TimerSetOneShot(ELED_TIMEID_TRIGGER, s_modeTbl[s_currentMode].seqs[s_currentSeq].steps[s_currentStep].postDelayMs, NULL);
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:消息发送												*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void eled_SendTriggerMsg(ushort msgId)
{
	EledTrigger_t msg;
	msg.header.msgId = msgId;

	DEBUG_TRCIN();

	FRM_MsgSendAsync(ELED_SPORT, sizeof(msg), &msg);

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:LED灯文件写操作										*/
/*--------------------------------------------------------------------------*/
/*	param			:sint fd	文件描述符									*/
/*					:schar *val	向文件写入的数								*/
/*					:sint len	数据长度									*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void eled_FileWrite(sint fd, schar *val, sint len)
{
	lseek(fd, 0, SEEK_SET);
	write(fd, val, len);
}

/*==========================================================================*/
/*	Description		:设置LED灯												*/
/*--------------------------------------------------------------------------*/
/*	param			:sint ledFd	文件描述符									*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void eled_setLedOn(sint ledFd)
{
	if(-1 != ledFd)
	{
#ifdef ELED_USE_GPIO_LEDS
		eled_FileWrite(ledFd, ELED_VAL_HI, ELED_VAL_SIZE);
#else
		DGPIO_Write(ledFd, DGPIO_VAL_HI);
#endif
	}
}

/*==========================================================================*/
/*	Description		:设置LED灯灭											*/
/*--------------------------------------------------------------------------*/
/*	param			:sint ledFd	文件描述符									*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void eled_setLedOff(sint ledFd)
{
	if(-1 != ledFd)
	{
#ifdef ELED_USE_GPIO_LEDS
		eled_FileWrite(ledFd, ELED_VAL_LO, ELED_VAL_SIZE);
#else
		DGPIO_Write(ledFd, DGPIO_VAL_LO);
#endif
	}
}

/*==========================================================================*/
/*	Description		:设置橙灯和绿灯亮灯										*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void eled_LedOnStepProc(void)
{
	DEBUG_TRCIN();

	eled_setLedOn(s_ledOgFd);
	eled_setLedOn(s_ledGrFd);

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:绿灯亮灯、橙灯灭灯										*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void eled_GrLedOnStepProc(void)
{
	DEBUG_TRCIN();

	eled_setLedOff(s_ledOgFd);
	eled_setLedOn(s_ledGrFd);

	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:绿灯灭灯、橙灯亮灯										*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void eled_OgLedOnStepProc(void)
{
	DEBUG_TRCIN();

	eled_setLedOn(s_ledOgFd);
	eled_setLedOff(s_ledGrFd);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:LED闪烁模式切换前处理									*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/23 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void eled_ChgModePreProc(void)
{
	DEBUG_TRCIN();

	ELED_LedOffStepProc();

	DEBUG_TRCOUT();
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
/*	history			:2023/05/11 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void *eled_PriMalloc(slong size)
{
	DEBUG_TRCIN();

	slong i;
	void *ptr =  NULL;

	if(size <= sizeof(eledPriNode_t))
	{
		for(i = 0; (i < ELED_MODETBL_SIZW) && (NULL == ptr); i++)
		{
			if(!s_ledPriMemory[i].isUsing)
			{
				s_ledPriMemory[i].isUsing = 1;
				ptr = &s_ledPriMemory[i];
			}
			else
			{
			}
		}
	}
	else
	{
		/* do nothing */
	}

	DEBUG_TRCOUT();

	return ptr;
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
/*	history			:2023/05/11 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void eled_PriFree(void *ptr)
{
	eledPriMemoryPool_t *pMemory = (eledPriMemoryPool_t *)ptr;

	DEBUG_TRCIN();

	if (pMemory)
	{
		pMemory->isUsing = 0;
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();
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
/*	history			:2023/05/12 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
bool_t eled_PriListFind(EledWorkMode_t mode)
{
	DEBUG_TRCIN();
	bool_t findFlag = FALSE;
	eledPriNode_t *curNode = s_LedPriListHead;

	while ((NULL != curNode) && (!findFlag))
	{
		if(mode == curNode->mode)
		{
			findFlag = TRUE;
		}
		else
		{
			curNode = curNode->next;
		}
	}

	DEBUG_TRCOUT();

	return findFlag;
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
/*	history			:2023/05/12 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void eled_PrtList(void)
{
	eledPriNode_t *curNode = s_LedPriListHead;
	DEBUG_TRCIN();

	DLOG_Debug("----------Print List Start----------");
	while (NULL != curNode)
	{
		DLOG_Debug("----------mode=%d----------", curNode->mode);
		curNode = curNode->next;
	}
	DLOG_Debug("----------Print List End----------");
	DEBUG_TRCOUT();
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
/*	history			:2023/05/11 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void eled_LedPriListInsert(EledWorkMode_t mode, EledPriority_t pri)
{
	eledPriNode_t *curNode;
	eledPriNode_t *instNode;
	bool_t res;
	schar insertFlag = 0;

	DEBUG_TRCIN();
	res = eled_PriListFind(mode);

	DLOG_Debug("----------Before Insert List----------");
	eled_PrtList();
	if(!res)
	{
		instNode = eled_PriMalloc(sizeof(eledPriNode_t));
		if(NULL != instNode)
		{
			instNode->pri = pri;
			instNode->mode = mode;
			instNode->prev = NULL;
			instNode->next = NULL;

			if(NULL == s_LedPriListHead)
			{
				s_LedPriListHead = instNode;
				s_LedPriListHead->next = NULL;
				s_LedPriListHead->prev = NULL;
			}
			else
			{
				curNode = s_LedPriListHead;
				if(pri >= curNode->pri)
				{
					instNode->next = curNode;
					curNode->prev = instNode;
					s_LedPriListHead = instNode;
				}
				else
				{
					while ((NULL != curNode->next) && (1 != insertFlag))
					{
						curNode = curNode->next;
						if(pri >= curNode->pri)
						{
							instNode->prev = curNode->prev;
							instNode->next = curNode;
							curNode->prev->next = instNode;
							curNode->prev = instNode;
							insertFlag = 1;
						}
					}

					if(1 != insertFlag)
					{
						curNode->next = instNode;
						instNode->prev = curNode;
					}
				}
			}
		}
	}
	else
	{
		/* do nothing */
	}
	DLOG_Debug("----------After Insert List----------");
	eled_PrtList();

	DEBUG_TRCOUT();
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
/*	history			:2023/05/11 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void eled_PriListErase(EledWorkMode_t mode)
{
	eledPriNode_t *curNode = s_LedPriListHead;
	uchar eraseFlag = 0;
	bool_t res;
	DEBUG_TRCIN();

	DLOG_Debug("----------Before Delete List----------");
	eled_PrtList();
	res = eled_PriListFind(mode);
	if (res)
	{
		if(mode == curNode->mode)
		{
			s_LedPriListHead = curNode->next;
			eled_PriFree((void *)curNode);
		}
		else
		{
			curNode = curNode->next;
			while((NULL != curNode) && (1 != eraseFlag))
			{
				if(mode == curNode->mode)
				{
					curNode->prev->next = curNode->next;
					if (NULL != curNode->next)
					{
						curNode->next->prev = curNode->prev;
					}
					eled_PriFree((void *)curNode);
					eraseFlag = 1;
				}
				else
				{
					curNode = curNode->next;
				}
			}
		}
	}
	DLOG_Debug("----------After Delete List----------");
	eled_PrtList();

	DEBUG_TRCIN();
}

/*End Of File*/
