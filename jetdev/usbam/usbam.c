/****************************************************************************/
/*	Description		:USBメモリオートマウントコマンドソースコード			*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Gu.CM												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/10												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023/03/10 NICHICON CORPORATION					*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mount.h>
#include <errno.h>
#include "typedef.h"
#include "dlog_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
#define	USBST_ATMNT_WAIT_USLP				(100 * 1000)									/* リトライスリープ時間(us) */
#define	USBST_ATMNT_RETRY_CNT				(uchar)(3 * 1000 * 1000 / USBST_ATMNT_WAIT_USLP)/* リトライ回数(3s分) */
#define	USBST_ATMNT_STRLEN_MAX				((uchar)64)										/* 最大文字列長 */

#define USBST_ATMNT_TELNET_STA_FILE_PATH	"/media/usb_storage/remotelogin.txt"
#define	USBST_ATMNT_TELNET_EXEC_CMD			"/usr/sbin/inetd"								/* Telnetデーモン起動コマンド */
#define	USBST_ATMNT_INETD_TERM_CMD			"killall -9 inetd"								/* inetd終了コマンド */
#define USBST_ATMNT_TELNET_TERM_CMD			"killall -9 telnetd"							/* telnetd終了コマンド */
#define	USBST_ATMNT_TESTMODE_FILE_PATH		"/mnt/ram/test_mode"							/* 製品検査モード起動判定ファイルPATH */
#define	USBST_ATMNT_FLG_OFF					((uchar)0x00)									/* フラグ設定値(OFF) */
#define	USBST_ATMNT_FLG_ON					((uchar)0x01)									/* フラグ設定値(ON) */

#define USB_ERR		((sint)-1)
#define USB_OK		((sint)0)

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:telnetデーモン起動/終了処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:uchar	onOff											*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/10 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void usbam_ctrlTelnet(uchar onOff)
{
	struct stat st;
	sint ret = 0;
	sint result = 0;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((USBST_ATMNT_FLG_OFF != onOff) && (USBST_ATMNT_FLG_ON != onOff))
	{
		return;
	}

	memset(&st, 0x00, sizeof(st));

	/* 製品検査モードファイルが存在する場合はデーモンの制御しない */
	ret = stat(USBST_ATMNT_TESTMODE_FILE_PATH, &st);
	if (0 != ret)
	{
			/* telnetデーモン起動処理 */
		if (USBST_ATMNT_FLG_ON == onOff)
		{
			/* telnetデーモン起動 */
			/* バックグラウンド実行したコマンドの処理結果はsystemで取得不可なので戻り値はチェックしない */
			system(USBST_ATMNT_TELNET_EXEC_CMD);
		}
		else	/* telnetデーモン終了処理 */
		{
			system(USBST_ATMNT_INETD_TERM_CMD);
			system(USBST_ATMNT_TELNET_TERM_CMD);
		}
	}
	else
	{
		/* do nothing */
	}

	DEBUG_TRCOUT();

	return;
}

/*==========================================================================*/
/*	Description		:USBメモリマウント処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:schar *temp_dev_SC デバイス名							*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/10 : DHC Du.CM : 新規作成						*/
/*==========================================================================*/
void usbam_mount(schar *dev)
{
	struct stat st;
	schar devPath[USBST_ATMNT_STRLEN_MAX];
	schar mpt[USBST_ATMNT_STRLEN_MAX];
	schar cmd[USBST_ATMNT_STRLEN_MAX];
	sint ret = 0;
	sint telnStartRes = 0;
	uchar retry = 0;
	uchar result = 0;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == dev)
	{
		return;
	}

	memset(&st, 0x00, sizeof(st));

	/* マウントポイント文字列生成 */
	memset(mpt, 0x00, sizeof(mpt));
	snprintf(mpt, sizeof(mpt), "/media/%s", dev);

	/* 既にリンクが存在する場合はマウントしない */
	ret = stat("/media/usb_storage", &st);
	if (0 == ret)
	{
		/* マウントポイントクリア */
		memset(cmd, 0x00, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "umount -l %s", mpt);
		system(cmd);

		memset(cmd, 0x00, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "rm -rf %s", mpt);
		system(cmd);
	}
	else
	{
		/* do nothing */
	}

	/* マウントポイント作成 */
	memset(cmd, 0x00, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "mkdir -p %s", mpt);
	system(cmd);

		/* マウント */
	retry = USBST_ATMNT_RETRY_CNT;
	memset(devPath, 0x00, sizeof(devPath));
	snprintf(devPath, sizeof(devPath), "/dev/%s", dev);
	do
	{
		ret = mount(devPath, mpt, "vfat", MS_SYNCHRONOUS, NULL);
		/* マウント成功 */
		if (0 == ret)
		{
			result = 0;
		}
		else if ((0 > ret) && (errno == ENOMEDIUM))		/* USBメモリ応答待ち */
		{
			usleep(USBST_ATMNT_WAIT_USLP);
			retry--;
			result = ret;
		}
		else	/* 上記以外は異常 */
		{
			result = -1;
		}
	} while((retry > 0) && (result > 0));

	/* マウント成功時はシンボリックリンク作成 */
	if (0 == ret)
	{
		memset(cmd, 0x00, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "ln -s %s /media/usb_storage", mpt);
		system(cmd);
			/* Telnetデーモン起動 */
		telnStartRes = stat(USBST_ATMNT_TELNET_STA_FILE_PATH, &st);
		if (0 == telnStartRes)
		{
			usbam_ctrlTelnet(USBST_ATMNT_FLG_ON);
		}
	}
	else	/* マウント失敗時はマウントポイント削除 */
	{
		memset(cmd, 0x00, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "rm -rf %s", mpt);
		system(cmd);
	}

	DEBUG_TRCOUT();

	return;
}
/*==========================================================================*/
/*	Description		:USBメモリアンマウント処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:schar *temp_dev_SC デバイス名							*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/09 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
void usbam_unmount(schar *dev)
{
	schar mpt[USBST_ATMNT_STRLEN_MAX];
	schar cmd[USBST_ATMNT_STRLEN_MAX];

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == dev)
	{
		return;
	}

	/* シンボリックリンク削除 */
	system("rm -r /media/usb_storage");

	/* Telnetデーモン停止 */
	usbam_ctrlTelnet(USBST_ATMNT_FLG_OFF);

	/* マウントポイントPATH生成 */
	memset(mpt, 0x00, sizeof(mpt));
	snprintf(mpt, sizeof(mpt), "/media/%s", dev);

	/* アンマウント */
	memset(cmd, 0x00, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "umount -l %s", mpt);
	system(cmd);

	/* マウントポイント削除 */
	memset(cmd, 0x00, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "rm -rf %s", mpt);
	system(cmd);

	DEBUG_TRCOUT();

	return;
}
/*==========================================================================*/
/*	Description		:USBメモリマウント/アンマウントコマンド					*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:sint													*/
/*					(0:異常発生無/-1:異常発生有)							*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/10 : DHC Gu.CM : 新規作成						*/
/*==========================================================================*/
sint main(sint argc, schar *argv[])
{
	sint ret = USB_OK;
	sint op = 0;

	DEBUG_TRCIN();

	/* コマンド引数チェック */
	if (argc < 3)
	{
		ret = USB_ERR;
	}
	else
	{
		/* マウント / アンマウント処理実行 */
		op = atoi(argv[2]);
		switch(op)
		{
		case 0:	/* マウント */ 
			usbam_mount(argv[1]);
			break;
		case 1:	/* アンマウント */
			usbam_unmount(argv[1]);
			break;
		default:
			break;
		}
	}

	DEBUG_TRCOUT();

	return ret;
}

/*End Of File*/
