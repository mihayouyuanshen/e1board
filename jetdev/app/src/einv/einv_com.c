/****************************************************************************/
/*	Description		:INV通信ミドル INV通信スレッド							*/
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

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h> 
#include <termios.h> 
#include <unistd.h> 

#include "typedef.h"
#include "task_g.h"
#include "frm_g.h"
#include "common_g.h"
#include "dlog_g.h"

#include "einv_g.h"
#include "einv_ram_g.h"
#include "einv_msg_g.h"
#include "einv_que_g.h"
#include "einv_spec_g.h"
#include "einv_com_g.h"
#include "einv_com.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
 

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description	:INV通信スレッドstartRoutine								*/
/*--------------------------------------------------------------------------*/
/*	param		:arg		:NULL指定										*/
/*--------------------------------------------------------------------------*/
/*	return		:NULL														*/
/*--------------------------------------------------------------------------*/
/*	detail		:INV通信スレッドstartRoutine処理を行う。					*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/27 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
void * EINV_CommThreadStartRoutine(void *arg)
{
	DEBUG_TRCIN();

	/* 通信スレッド初期処理 */
	einv_CommThreadInit();

	/* INV通信スレッドループ */
	einv_CommThreadLoop();

	DEBUG_TRCOUT();

	return NULL;
}


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description	:通信スレッド初期処理										*/
/*--------------------------------------------------------------------------*/
/*	param		:fact		:起動要因										*/
/*--------------------------------------------------------------------------*/
/*	return		:EINV_OK(0)				正常終了							*/
/*				:EPSC_ERR_SYSCALL(-1)	システムコールエラー				*/
/*--------------------------------------------------------------------------*/
/*	detail		:通信スレッド初期処理を行う。								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/27 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong einv_CommThreadInit(void)
{

	struct termios	tio;				/* シリアル通信設定 */
	speed_t 		baudRate = B115200;
	sint			ionum = 0;
	slong			ret = EINV_OK;

	DEBUG_TRCIN();

	g_einvFd = open(SERIAL_PORT, O_RDWR);				/* デバイスをオープンする */
	if (0 > g_einvFd)
	{
		DLOG_Error("serial open error. %d %m\n", errno);
		ret = EINV_ERR_SYSCALL;
	}
	else
	{
		/* 処理無し */
	}

	/* 新しいポートの設定 */
	memset(&tio , 0x00 , sizeof(tio));					/* オールクリア */
	tio.c_cflag = (CS8 | CREAD | CLOCAL);				/* 8bit,フロー制御なし,stop=1(default) */

	if (ret == EINV_OK)
	{
		/* 入力ボーレートの設定 */
		ionum = cfsetispeed(&tio, baudRate);
		if (0 > ionum)
		{
			DLOG_Error("UART rx baudrate setting error. %d %m\n", errno);
			ret = EINV_ERR_SYSCALL;
		}
		else
		{
			/* 処理無し */
		}
	} 
	else
	{
		/* 処理無し */
	}

	if (ret == EINV_OK)
	{
		/* 出力ボーレートの設定 */
		ionum = cfsetospeed(&tio, baudRate);
		if (0 > ionum)
		{
			DLOG_Error("UART tx baudrate setting error. %d %m\n", errno);
			ret = EINV_ERR_SYSCALL;
		}
		else
		{
			/* 処理無し */
		}
	} 
	else
	{
		/* 処理無し */
	}

	if (ret == EINV_OK)
	{
		/* デバイスに設定を行う */
		ionum = tcsetattr(g_einvFd, TCSANOW, &tio);
		if (0 > ionum)
		{
			DLOG_Error("UART device setting error. %d %m\n", errno);
			ret = EINV_ERR_SYSCALL;
		}
		else
		{
			/* 処理無し */
		}
	} 
	else
	{
		/* 処理無し */
	}

	if (ret == EINV_OK)
	{
		/* ポートの設定を有効にする */
		ionum = ioctl(g_einvFd, TCSETS, &tio);
		if (0 > ionum)
		{
			DLOG_Error("UART port control error. %d %m\n", errno);
			ret = EINV_ERR_SYSCALL;
		}
		else
		{
			/* 処理無し */
		}
	} 
	else
	{
		/* 処理無し */
	}

	if (ret != EINV_OK && g_einvFd >= 0)
	{
		close(g_einvFd);
	} 
	else
	{
		/* 処理無し */
	}

	if (ret == EINV_OK)
	{
		/* INV依存レイヤーのINV通信スレッド初期処理 */
		ret = EINV_InitCommThread();
		if(ret != EINV_OK)
		{
			DLOG_Error("EINV_initCommThread() error.(%ld)\n", ret);
		} 
		else
		{
			/* 処理無し */
		}
	} 
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();
	return ret;
}

/*==========================================================================*/
/*	Description	:INV通信スレッドループ										*/
/*--------------------------------------------------------------------------*/
/*	param		:なし														*/
/*--------------------------------------------------------------------------*/
/*	return		:0		:処理結果											*/
/*--------------------------------------------------------------------------*/
/*	detail		:INV通信スレッドループを行う。								*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/27 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
static slong einv_CommThreadLoop(void)
{
	EinvMsgPar_t reqMsg;

	DEBUG_TRCIN();

	while(1){

		/* 通信要求キューから要素を取得 */
		EINV_DeqCommReqQue(&reqMsg);

		/* INV通信IF処理 */
		EINV_ProcInvCommunication(&reqMsg);
	}

	DEBUG_TRCOUT();

	return 0;
}


/*End Of File*/
