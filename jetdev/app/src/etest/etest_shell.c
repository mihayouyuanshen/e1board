/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC yufeng.yang										*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/05/05												*/
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
#include "etest_shell_g.h"
#include "frm_g.h"
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
#define ETEST_CMDBUF_SIZE	((ushort)128u)
#define MAX_ARG_NUM			((uchar)6u)
schar s_cmdBuf[ETEST_CMDBUF_SIZE];

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
static void ETest_getUserInput(void);
static void sampleTest(slong argc, schar *argv[]);

ETEST_SubCmdItem_t g_ETest_subCmdTbl[] = {
	{"sample", "print hello world", sampleTest}
};

ETEST_CmdItem_t g_ETest_CmdItem = {
	"etest",
	"etest",
	g_ETest_subCmdTbl,
	ARRAY_SIZE(g_ETest_subCmdTbl)
};

extern ETEST_CmdItem_t g_ETestEdatCmdItem;
extern ETEST_CmdItem_t g_ETest_ElogCmdItem;
extern ETEST_CmdItem_t g_ETest_EtimCmdItem;
extern ETEST_CmdItem_t g_ETestEsystCmdItem;
extern ETEST_CmdItem_t g_ETestEeepCmdItem;
extern ETEST_CmdItem_t g_ETestEledCmdItem;

const ETEST_CmdItem_t *s_cmds[] = {
	&g_ETest_CmdItem,
	&g_ETest_ElogCmdItem,
	&g_ETestEdatCmdItem,
	&g_ETest_EtimCmdItem,
	&g_ETestEsystCmdItem,
	&g_ETestEeepCmdItem,
	&g_ETestEledCmdItem
};

static void sampleTest(slong argc, schar *argv[])
{
	printf("hello world\n");
}

static void ETest_CmdShowHelp(void)
{
	slong i;
	printf("command\t\tmodule\n");
	printf("=======\t\t======\n");
	printf("help\t\tshow help for module\n");
	for (i = 0; i < ARRAY_SIZE(s_cmds); ++i)
	{
		printf("%s\t\t%s\n", s_cmds[i]->cmd, s_cmds[i]->modName);
	}
}

static void ETest_SubCmdShowHelp(ETEST_CmdItem_t const *cmdItem)
{
	slong i;
	printf("Module: %s\n", cmdItem->modName);
	printf("subcmd\t\thelp text\n");
	printf("======\t\t===========\n");
	printf("help\t\tshow help for sub command\n");
	printf("exit\t\treturn to module\n");
	for (i = 0; i < cmdItem->subCmdNum; ++i)
	{
		printf("%s\t\t%s\n", cmdItem->subCmds[i].subCmd, cmdItem->subCmds[i].help);
	}
}

static void ETest_subCmdParser(ETEST_CmdItem_t const *cmdItem)
{
	slong i;
	slong loopFlag = 1;
	slong loopFlag2;
	slong parseLoopFlag;
	slong findFlag;
	slong argc = 0;
	schar *argv[MAX_ARG_NUM + 1];
	schar *str;

	ETest_SubCmdShowHelp(cmdItem);
	while(loopFlag)
	{
		printf("subcmd(%s):\\> ", cmdItem->modName);
		ETest_getUserInput();

		str = s_cmdBuf;
		parseLoopFlag = 1;
		argc = 0;
		/* printf("s_cmdBuf = %s\n", s_cmdBuf); */
		while (parseLoopFlag)
		{
			/* Skip leading space */
			while ((' ' == *str) || ('\t' == *str))
			{
				++str;
			}
			if (*str && (argc < MAX_ARG_NUM))
			{
				argv[argc++] = str;
				loopFlag2 = 1;
				while (('\0' != *str) && loopFlag2)
				{
					if ((' ' != *str) && ('\t' != *str) && ('\n' != *str) && ('\r' != *str))
					{
						str++;
					}
					else
					{
						loopFlag2 = 0;
					}
				}
				if ('\0' != *str)
				{
					*str = '\0';
					str++;
				}
			}
			else
			{
				argv[argc] = NULL;
				parseLoopFlag = 0;
			}
		}
		/*
		printf("argc = %d\n", argc);
		for (int x = 0; x < argc; ++x)
		{
			printf("argv[%d] = %s\n", x, argv[x]);
		}
		*/
		if (0 == strncmp(argv[0], "help", 5))
		{
			ETest_SubCmdShowHelp(cmdItem);
		}
		else if (0 == strncmp(argv[0], "exit", 5))
		{
			loopFlag = 0;
		}
		else
		{
			i = 0;
			findFlag = 0;
			while((i < cmdItem->subCmdNum) && (0 == findFlag))
			{
				if (0 == strcmp(argv[0], cmdItem->subCmds[i].subCmd))
				{
					findFlag = 1;
					if (cmdItem->subCmds[i].mainProc)
					{
						cmdItem->subCmds[i].mainProc(argc, argv);
					}
				}
				else
				{
					i++;
				}
			}
			if (0 == findFlag)
			{
				printf("command not support: %s\n", argv[0]);
			}
		}
	}
}

static void ETest_cmdParser(schar *cmd)
{
	slong i;
	slong findFlag = 0;

	if (0 == strncmp(cmd, "help", 5))
	{
		ETest_CmdShowHelp();
	}
	else
	{
		i = 0;
		while((i < ARRAY_SIZE(s_cmds)) && (0 == findFlag))
		{
			if (0 == strcmp(cmd, s_cmds[i]->cmd))
			{
				findFlag = 1;
				ETest_subCmdParser(s_cmds[i]);
			}
			else
			{
				i++;
			}
		}
		if (0 == findFlag)
		{
			printf("command not support: %s\n", cmd);
		}
	}
}

static void ETest_getUserInput(void)
{
	slong loopFlag;
	schar *str;

	memset(s_cmdBuf, 0, ETEST_CMDBUF_SIZE);
	fgets(s_cmdBuf, sizeof(s_cmdBuf) - 1, stdin);
	str = s_cmdBuf;
	loopFlag = 1;
	while(('\0' != *str) && loopFlag)
	{
		if (('\r' == *str) || ('\n' == *str))
		{
			*str = '\0';
			loopFlag = 0;
		}
		else
		{
			str++;
		}
	}
}

static void *ETest_cmdParserThread(void * param)
{
	ETest_CmdShowHelp();
	while(1)
	{
		printf("cmd:\\> ");
		ETest_getUserInput();
		ETest_cmdParser(s_cmdBuf);
	}
}

void ETEST_startTestShell(void)
{
	FrmPthread_t tid;
	printf("etest_start\n");
	FRM_PthreadCreate(&tid, NULL, ETest_cmdParserThread, NULL);
}

/*End Of File*/
