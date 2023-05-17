
/******************************************************************************/
/*    Description    :時刻管理ミドル                                           */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC Gu.CM                                               */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/04/06                                              */
/*----------------------------------------------------------------------------*/
/*    Module Name    :                                                        */
/*----------------------------------------------------------------------------*/
/*    Note           :                                                        */
/*----------------------------------------------------------------------------*/
/*    Copyright      :(c) 2020 NICHICON CORPORATION                           */
/******************************************************************************/
#ifndef __ETIM_MSG_H__
#define __ETIM_MSG_H__

#include <time.h>
#include <linux/rtc.h>
#include "common_g.h"
#include "ewdt.h"
#include "typedef.h"
#include "etim_g.h"
#include "etim_serv.h"
#include "frm_g.h"
/*----------------------------------------------------------------------------*/
/* マクロ定義                                                                  */
/*----------------------------------------------------------------------------*/
/* etim_file.c */
#define	ETIM_INI_FILEPATH	"/mnt/user/etim/etim.inf"	/* 設定ファイルパス名 */
#define	ETIM_NTPHOST_TAG	"ntphostaddr"				/* NTPサーバーアドレス設定タグ名 */
#define	ETIM_READBUF_SZ		((size_t)512u)				/* ファイル行読み込みバッファサイズ */
#define	ETIM_WHITESP		" \t"						/* 空白＋タブ */
#define	ETIM_CRLF			"\r\n"						/* <CR><LF> */
#define ETIM_FLAG_OK		((uchar)1u)
#define ETIM_FLAG_ERR		((uchar)0u)

/* etim_if.c */
#define ETIM_NODBG_PRINT	/* ログ出力抑止 */

#ifndef CROSSDEBUG
/* ハードウェアカウンタ読込用 */
#define ETIM_BWREG_HW_DIGCTL_MICROSECONDS	((ulong)0x8001C0C0u)
/* #define	ETIM_BWREG_HW_DIGCTL_HCLKCOUNT		((ulong)0x8001C020)） */
#define ETIM_BWREG_DEV			"/dev/bwreg"
/* device magic number */
#define ETIM_BWREG_IOC_MAGIC	'A'
/* read one register throught a ptr */
#define ETIM_BWREG_IOCREAD		_IOR(ETIM_BWREG_IOC_MAGIC, 2, sint)

#define RTC_RD_TIME _IOR('p', 0x09, struct rtc_time)	/* Read RTC time */
#define RTC_SET_TIME _IOW('p', 0x0a, struct rtc_time)	/* Set RTC time */

#endif

/* etim_ntp.h */
#ifndef CROSSDEBUG
#define	ETIM_CALL_SYSTEM(x)		system(x)
#else
#define	ETIM_CALL_SYSTEM(x)		0; DLOG_Info("%s\n",x)
#endif

#define	ETIM_NTPINFO_FILEPATH	"/mnt/ram/etim_ntp.dat"					/* NTPサーバー情報ファイルパス名 */
#define	ETIM_SYNCSTATE_FILEPATH	"/mnt/ram/etim_syncStt.dat"				/* 同期状態ファイルパス名 */

#define	ETIM_NTP_DEFAULT_PORT			((ushort)0u)					/* ポート番号(未使用) */
#define	ETIM_NTP_DEFAULT_THRESHOLD_1	((ulong)3u)						/* リトライ閾値1(3回) */
#define	ETIM_NTP_DEFAULT_THRESHOLD_2	((ulong)0u)						/* リトライ閾値2(0回) */
#define	ETIM_NTP_DEFAULT_INTERVAL_1		((ulong)(6 * 60 * 60))			/* リトライ間隔1(6時間) */
#define	ETIM_NTP_DEFAULT_INTERVAL_2		((ulong)0u)						/* リトライ間隔2(0秒) */

#define	ETIM_NTP_MIN_SYNC_TIME_YEAR		((sint)70)						/* 最小年(1970年) */
#define	ETIM_NTP_MIN_SYNC_TIME_MON		((sint)0)						/* 最小月(1月) */
#define	ETIM_NTP_MIN_SYNC_TIME_DAY		((sint)1)						/* 最小日(1日) */

/* etim_rtc.h */
#define ETIM_RTC_DEV_PATH		"/dev/rtc1"				/* RTCデバイスファイル			*/
#define ETIM_RTC_SLAVE_ADDR		((ulong)0x32)			/* RTCスレーブアドレス			*/
#define	ETIM_RTC_REG_SIZE		((sint)16)				/* RTC内部レジスタサイズ(byte)	*/
#define ETIM_HW_BUF_SIZE		((uchar)4u)

#define	ETIM_RTC_VDET(r)	((r).d0F.bit.vdet)			/* 電源監視結果表示ビット	*/
#define	ETIM_RTC_XSTP(r)	((r).d0F.bit.xstp)			/* 発振停止検出フラグビット	*/
#define	ETIM_RTC_PON(r)		((r).d0F.bit.pon)			/* パワーオンリセットビット	*/
#define	ETIM_RTC_FLT(r)		((r).d0F.bit.pon | (r).d0F.bit.xstp | (r).d0F.bit.vdet)
														/* 異常ビット				*/

/* 日付・時刻値取得マクロ */
#define	ETIM_RTC_SEC(r)		((r).d00.bit.sec_h * 10 + (r).d00.bit.sec_l)	/* 秒 */
#define	ETIM_RTC_MIN(r)		((r).d01.bit.min_h * 10 + (r).d01.bit.min_l)	/* 分 */
#define	ETIM_RTC_HOUR(r)	((r).d02.bit.hour_h * 10 + (r).d02.bit.hour_l)	/* 時 */
#define	ETIM_RTC_MDAY(r)	((r).d04.bit.mday_h * 10 + (r).d04.bit.mday_l)	/* 日 */
#define	ETIM_RTC_MON(r)		((r).d05.bit.mon_h * 10 + (r).d05.bit.mon_l)	/* 月 */
#define	ETIM_RTC_YEAR(r)	((((r).d0D.bit.ram == 0) ? (1900) : (2000)) + (r).d06.bit.year_h * 10 + (r).d06.bit.year_l)
																			/* 年 */
/* 制御レジスタ1 リセット(12/24=1, CLEN2=1, other=0) */
#define	ETIM_RTC_CTRL1_RESET(r)	((r).d0E.byte = (uchar)0x30)
/* 制御レジスタ2 リセット(CLEN1=1, other=0) */
#define	ETIM_RTC_CTRL2_RESET(r)	((r).d0F.byte = (uchar)0x08)

/* other */
#ifndef ETIM_CLOCK_MONOTONIC_RAW
#define ETIM_CLOCK_MONOTONIC_RAW	4
#endif

#define ETIM_TMP_BUF_SIZE	((uchar)256u)

#define ETIM_OPEN(path, flags, mode)	open(path,flags,mode)
#define ETIM_CLOSE(fd)					{if ((fd) >= 0){close(fd);}}

#define ETIM_ERR_BUF_SIZE	((uchar)256u)

#define	ETIM_TIMER_MAX		((ulong)4294967290UL)		/* タイマー登録時間最大値 */

#define ETIM_TIMEFMT		"%Y/%m/%d %H:%M:%S"			/* 時刻->文字列変換フォーマット */

#define	ETIM_IFNAME			"eth0"						/* ネットワークIF名 */
#define	ETIM_DAYSEC_SZ		(86400)						/* 1日の秒数：3600sec * 24H */
#define	ETIM_DAYSEC(n)		((n) % ETIM_DAYSEC_SZ)		/* 1日の秒数へ変換 */
#define	ETIM_M_MSEC(n)		((n) * 1000000)				/* msec->nsec変換 */
#define	ETIM_M_USEC(n)		((n) * 1000)				/* msec->usec変換 */

#define	ETIM_PINGCMD			"ping -q -c 1 "			/* 疎通確認コマンド */
#define	ETIM_PINGKILL			"killall ping "			/* ping 強制終了コマンド */
#define	ETIM_PINGWAIT_DIV		((ulong)500u)					/* ping 応答待ち時間[msec] */
#define	ETIM_PINGTOUT			((ulong)6000u)					/* ping 応答タイムアウト[msec] */
#define	ETIM_NTPCMD				"ntpd -nq -p "			/* NTPコマンド */
#define	ETIM_CMDRED				" >& /dev/null"			/* コマンド出力nullリダイレクト */
#define	ETIM_CMDLINE_BUFSIZE	((ulong)1024u)			/* NTPコマンドラインバッファ */

#define	ETIM_WDT_TIMEOUT		(10)					/* WDT要求応答のタイムアウト値 */

#define	ETIM_TIME_LIMIT_HOUR	((uchar)23u)			/* 時分秒(時)限界値 */
#define	ETIM_TIME_LIMIT_MIN		((uchar)59u)			/* 時分秒(分)限界値 */
#define	ETIM_TIME_LIMIT_SEC		((uchar)60u)			/* 時分秒(秒)限界値 */

/*----------------------------------------------------------------------------*/
/* 非公開型宣言、定義                                                          */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 定数宣言、定義                                                              */
/*--------------------------------------- ------------------------------------*/
/* IF関数からetimプロセスへのファンクション番号 */
typedef enum {
	ETIM_MSGID_SETTIME_REQ,			/* 時刻設定 */
	ETIM_MSGID_PING_NTP,			/* NTPサーバー疎通確認 */
	ETIM_MSGID_STARTSYNC_REQ,		/* 時刻同期開始 */
	ETIM_MSGID_START,
	ETIM_MSGID_NTP_SYNC_TREG
} EtimMsgFuntion_t;

/* スレッド管理番号 */
typedef enum {
	ETIM_TH_NTPEX = 0,				/* NTP同期スレッド */
	ETIM_TH_SIZE					/* 終端（配列サイズ） */
} EtimTh_t;

/* NTP時刻配列管理番号 */
typedef enum {
	ETIM_NTPTIME_BASE = 0,			/* 基準時刻用 */
	ETIM_NTPTIME_SCHE,				/* 予定時刻用 */
	ETIM_NTPTIME_SIZE				/* 終端（配列サイズ） */
} EtimNtpTimeArry_t;

/*----------------------------------------------------------------------------*/
/* 変数宣言、定義                                                              */
/*----------------------------------------------------------------------------*/
/* 応答メッセージデータ */
typedef struct {
	LfwHead_t header;			/* メッセージ構造体ヘッダ */
} EtimMsgRspData_t;

/* 時刻設定要求データ */
typedef struct {
	LfwHead_t header;			/* メッセージ構造体ヘッダ */
	struct tm time;				/* 設定時刻データ */
} EtimSetTime_t;

/* 時刻同期開始要求データ */
typedef struct {
	LfwHead_t header;		/* メッセージ構造体ヘッダ */
	EtimNtpInfo_t ntpInfo;	/* NTPサーバー情報 */
} EtimStartNtpSync_t;

/* NTP時刻同期状態取得応答データ */
typedef struct {
	LfwHead_t		header;		/* メッセージ構造体ヘッダ */
	EtimNtpState_t	state;		/* NTP時刻同期状態		  */
} EtimMsgGetstResData_t;

/* 各応答データ共用体 */
typedef union {
	LfwHead_t header;
	EtimMsgGetstResData_t getstRes;
} EtimRes_t;

/* スレッド管理データ */
typedef struct {
	pthread_t id;		/* スレッドID */
	void *(*start)();	/* スレッド関数ポインタ */
	uchar live;			/* スレッド死活監視 */
} EtimThdata_t;

typedef struct {
	FrmMsgHead_t header;
	sint fact;
} EtimStart_t;

typedef struct {
	LfwHead_t header;
	EtimNtpState_t trigger;
}NtpStateMsg_t;

typedef enum {
	ETIM_RETRY_TIMER_ID,
	ETIM_NTP_SYNC_TIMER_ID,
	ETIM_NTP_SYNC_JUDGE_TIMER_ID
} etimTimerId_t;

/*----------------------------------------------------------------------------*/
/* プロトタイプ宣言                                                            */
/*----------------------------------------------------------------------------*/

#endif /*__ETIM_MSG_H__*/
