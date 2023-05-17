/******************************************************************************/
/*    Description    :                                                        */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC Cao.HZ                                              */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/03/20                                              */
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
#include <fcntl.h>
#include "typedef.h"
#include "dgpio_g.h"
#include "common_g.h"
#include "dlog_g.h"

/*----------------------------------------------------------------------------*/
/* 公開変数定義                                                                */
/*----------------------------------------------------------------------------*/
#define GPIO_EXT_PATH		"/sys/class/gpio/export"
#define GPIO_UNEXT_PATH		"/sys/class/gpio/unexport"
#define GPIO_SF_PATH		"/sys/class/gpio/gpio"
#define GPIO_DIR_PATH		"/direction"
#define GPIO_VAL_PATH		"/value"
#define GPIO_EDGE_PATH		"/edge"

#define GIPI_PORT_LEN		((uchar)4)
#define GPIO_PATH_LEN		((uchar)64)

typedef enum {
	DGPIO_DIR_IN,
	DGPIO_DIR_OUT
} DGPIO_Dir_t;

typedef struct {
	uchar Index;
	DGPIO_Dir_t Dir;
	const char *edge;
} DGPIO_Desc_t;

static const DGPIO_Desc_t s_GpioDescTbl[DGPIO_PORT_MAX] = {
	{64 + 22, DGPIO_DIR_IN, "both"},		/* DGPIO_PORT_KEY_SW1 */
	{64 + 23, DGPIO_DIR_IN, "both"},		/* DGPIO_PORT_KEY_SW2 */
	{64 + 25, DGPIO_DIR_IN, "both"},		/* DGPIO_PORT_KEY_A8 */
	{64 + 26, DGPIO_DIR_IN, "both"},		/* DGPIO_PORT_KEY_D10 */
	{64 + 27, DGPIO_DIR_IN, "both"},		/* DGPIO_PORT_KEY_A14 */
	{64 + 28, DGPIO_DIR_IN, "both"},		/* DGPIO_PORT_KEY_K17 */
	{0 + 9,  DGPIO_DIR_OUT, "both"},		/* DGPIO_PORT_LED_GREEN */
	{64 + 7, DGPIO_DIR_OUT, "both"},		/* DGPIO_PORT_LED_ORANGE */
	{0 + 26, DGPIO_DIR_OUT, "both"}			/* DGPIO_PORT_RS485_DE */
};

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
/*    Description    :打开GPIO引脚                                             */
/*----------------------------------------------------------------------------*/
/*    param          :EnGpioPort_t Port GPIO引脚号                    	     */
/*----------------------------------------------------------------------------*/
/*    return         :sint                                                    */
/*                    (<0:打开失败,否则返回文件描述符)                          */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/13  : DHC Cao.HZ : 新規作成                      */
/*============================================================================*/
sint DGPIO_Open(DGPIO_Port_t Port)
{
	sint fd = -1, flags;
	size_t portLen;
	schar portNo[GIPI_PORT_LEN];
	schar path[GPIO_PATH_LEN];

	DEBUG_TRCIN();
	if(Port >= DGPIO_PORT_MAX)
	{
		return -1;
	}
	/* 配列を明示的に初期化 */
	memset(portNo, 0x00, sizeof(portNo));
	memset(path,   0x00, sizeof(path));

	/* ポート番号を文字列化 */
	snprintf(portNo, sizeof(portNo), "%d", s_GpioDescTbl[Port].Index);
	portLen = strlen(portNo);
	/* GPIOの使用開始を指定する("/sys/class/gpio/export") */
	fd = open(GPIO_EXT_PATH, O_WRONLY);
	if(fd < 0) 
	{
		DLOG_Error("GPIO export open error.\n");
		return fd;
	}
	else
	{
		/*動作なし*/
	}
	write(fd, portNo, portLen + 1);
	close(fd);

	/* 入出力を指定する("/sys/class/gpio/gpioXX/direction"(XX:PORT番号)) */
	snprintf(path, sizeof(path), "%s%s%s",
		GPIO_SF_PATH, portNo, GPIO_DIR_PATH);
	fd = open(path, O_WRONLY);
	if(fd < 0)
	{
		DLOG_Error("GPIO direction open error.\n");
		return fd;
	}
	else
	{
		/*動作なし*/
	}
	if(s_GpioDescTbl[Port].Dir == DGPIO_DIR_IN)
	{
		write(fd, "in", 3);
		flags = O_RDONLY;
	}
	else
	{
		write(fd, "out", 4);
		flags = O_WRONLY;
	}
	close(fd);

	/* Edgeを指定する("/sys/class/gpio/gpioXX/edge"(XX:PORT番号)) */
	snprintf(path, sizeof(path), "%s%s%s",
		GPIO_SF_PATH, portNo, GPIO_EDGE_PATH);
	fd = open(path, O_WRONLY);
	if(fd < 0)
	{
		DLOG_Error("GPIO edge open error.\n");
		return fd;
	}
	else
	{
		/*動作なし*/
	}
	write(fd, s_GpioDescTbl[Port].edge, strlen(s_GpioDescTbl[Port].edge));
	close(fd);

	/* 入出力値を開きハンドルを出力("/sys/class/gpio/gpioXX/value"(XX:PORT番号)) */
	snprintf(path, sizeof(path), "%s%s%s",
		GPIO_SF_PATH, portNo, GPIO_VAL_PATH);
	fd = open(path, flags);
	if(fd < 0)
	{
		DLOG_Error("GPIO value open error.\n");
		return fd;
	}
	else
	{
		/*動作なし*/
	}

	DEBUG_TRCOUT();
	return fd;
}

/*==========================================================================*/
/*	Description		:Write gpio												*/
/*--------------------------------------------------------------------------*/
/*	param			:slong ファイルディスクリプタ							*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/17 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
slong DGPIO_Read(sint fd, DGPIO_Val_t *gpioVal)
{
	schar c;
	ssize_t num;
	slong ret = -1;

	DEBUG_TRCIN();
	num = read(fd, &c, 1);
	lseek(fd, 0, SEEK_SET);
	if(num < 0)
	{
		DLOG_Error("GPIO value read error.\n");
	}
	else
	{
		if('0' == c)
		{
			*gpioVal = DGPIO_VAL_LO;
		}
		else
		{
			*gpioVal = DGPIO_VAL_HI;
		}
		ret = 0;
	}
	DEBUG_TRCOUT();
	return ret;
}

/*============================================================================*/
/*    Description    :写GPIO引脚值                                             */
/*----------------------------------------------------------------------------*/
/*    param          :sint fd 文件描述符                                       */
/*                   :EnGpioVal_t gpioVal 设置GPIO引脚的值                     */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/13  : DHC Cao.HZ : 新規作成                      */
/*============================================================================*/
void DGPIO_Write(sint fd, DGPIO_Val_t gpioVal)
{
	ssize_t	num;

	DEBUG_TRCIN();

	if(DGPIO_VAL_LO == gpioVal)
	{
		num = write(fd, "0", 2);
	}
	else
	{
		num = write(fd, "1", 2);
	}
	if(num < 0)
	{
		DLOG_Error("GPIO value write error.\n");
	}

	DEBUG_TRCOUT();
}
/*============================================================================*/
/*    Description    :关闭GPIO引脚                                             */
/*----------------------------------------------------------------------------*/
/*    param          :sint fd 文件描述符                                       */
/*                   :EnGpioVal_t gpioVal GPIO引脚号                           */
/*----------------------------------------------------------------------------*/
/*    return         :                                                        */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/03/13  : DHC Cao.HZ : 新規作成                      */
/*============================================================================*/
void DGPIO_Close(sint fd, DGPIO_Port_t Port)
{
	sint fd1 = 0;
	size_t portLen;
	schar portNo[GIPI_PORT_LEN];
	ssize_t num;

	DEBUG_TRCIN();
	if (fd < 0)
	{
		return;
	}
	/* 入出力値クローズ */
	close(fd);
	/* ポート番号を文字列化 */
	snprintf(portNo, sizeof(portNo), "%d", s_GpioDescTbl[Port].Index);
	portLen = strlen(portNo);
	/* GPIOの使用終了を指定する(/sys/class/gpio/unexport) */
	fd1 = open(GPIO_UNEXT_PATH, O_WRONLY);
	if (fd1 < 0)
	{
		DLOG_Error("GPIO unexport open error.\n");
		return;
	}
	num = write(fd1, portNo, portLen + 1);
	if(num < 0)
	{
		DLOG_Error("GPIO unexport write error.\n");
		return;
	}
	close(fd1);

	DEBUG_TRCOUT();
}

/* End Of File */
