/******************************************************************************/
/*    Description    :時刻管理ミドル                                           */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC Gu.CM                                              */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/04/03                                              */
/*----------------------------------------------------------------------------*/
/*    Module Name    :                                                        */
/*----------------------------------------------------------------------------*/
/*    Note           :                                                        */
/*----------------------------------------------------------------------------*/
/*    Copyright      :(c) 2020 NICHICON CORPORATION                           */
/******************************************************************************/
#ifndef __ETIM_G_H__
#define __ETIM_G_H__

#include "typedef.h"
#include "frm_g.h"
/*----------------------------------------------------------------------------*/
/* マクロ定義                                                                  */
/*----------------------------------------------------------------------------*/
#define	ETIM_DATESTR_SIZE		((uchar)20u)			/* 末尾の’\0’を含む日時文字列長 */
#define	ETIM_NTPHOST_BUFSZ		((uchar)129u)			/* NTPサーバーアドレス変数の配列サイズ */

#define	ETIM_NTPRTY_INFINITE	((ulong)0xFFFFFFFF)		/* リトライ回数閾値無し(無限リトライ) */
#define ETIM_RESERVED_BUF_SIZE	((uchar)3u)

/*----------------------------------------------------------------------------*/
/* 非公開型宣言、定義                                                          */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 定数宣言、定義                                                              */
/*--------------------------------------- ------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 変数宣言、定義                                                              */
/*----------------------------------------------------------------------------*/
/* ETIM関数リターンコード */
typedef enum {
	ETIM_OK = 0,					/* 正常終了 */
	ETIM_E_SYSCALL = -1, 			/* システムコールエラー */
	ETIM_E_PARAM = -2,				/* 引数異常 */
	ETIM_E_OUTVAL = -3,				/* 設定値異常 */
	ETIM_E_OTHER = -4				/* その他エラー */
} EtimRetCodeE_t;

/* 時刻設定・取得データ */
typedef struct {
	sint tmSec;					/* 秒（0～60）※60は閏秒のため */
	sint tmMin;					/* 分（0～59）*/
	sint tmHour;					/* 時（0～23）*/
	sint tmMday;					/* 日（1～31）*/
	sint tmMon;					/* 月（1～12）*/
	sint tmYear;					/* 年（1900～2037）※西暦年 */
} EtimTm_t;

/* 時分秒 */
typedef struct {
	uchar hour;						/* 時(0～23) */
	uchar min;						/* 分(0～59) */
	uchar sec;						/* 秒(0～60) ※閏秒を考慮 */
	uchar reserved;					/* 予備 */
} EtimHms_t;

/* NTPサーバー接続先情報 */
typedef struct {
	ushort ntpserver;				/* 接続先NTPサーバー */
	ushort portNum;					/* ポート番号(1～65,535) */
	ulong retryThreshold1;			/* リトライ回数閾値1 */
	ulong retryInterval1;			/* リトライ間隔(秒) */
	ulong retrThreshold2;			/* リトライ回数閾値1超過のリトライを繰り返す回数 */
	ulong retryInterval2;			/* リトライ回数閾値1超過後のリトライ間隔(秒) */
	EtimHms_t starSynctime;			/* 定時同期開始時刻 */
	EtimHms_t endSynctime;			/* 定時同期終了時刻 */
	schar addr[ETIM_NTPHOST_BUFSZ];	/* 接続先サーバーアドレス */
	uchar reserved[ETIM_RESERVED_BUF_SIZE];				/* 予備 */
} EtimNtpInfo_t;

/* ワンショットタイマー、アラーム通知イベントデータ */
typedef struct {
	LfwHead_t header;			/* メッセージ構造体ヘッダ */
	ulong id;						/* ワンショットタイマー・アラームID */
} EtimEvData_t;

/* NTP時刻同期状態 */
typedef enum {
	ETIM_STATE_NTP_SYNC_NG	= 0,	/* NTP时刻同步失败超过24小时,可选遥控器可设置 */
	ETIM_STATE_NTP_SYNC_OK,			/* NTP時刻同期OK，24小时以内 */
	ETIM_STATE_NTP_SYNC_RETRY,		/* NTP時刻同期失败，24小时以内 */
	ETIM_STATE_NTP_SYNC_INIT		/* NTP时间同步初始状态,可选遥控器不可设置 */
} EtimNtpState_t;

/*----------------------------------------------------------------------------*/
/* プロトタイプ宣言                                                            */
/*----------------------------------------------------------------------------*/
extern void ETIM_Init(void);
extern void ETIM_MsgProc(FrmMsgHead_t const *msg);
extern void ETIM_Fin(void);
extern slong ETIM_Start(slong fact);
extern slong ETIM_Stop(void);

extern sint ETIM_GetTime(EtimTm_t *pTime);
extern sint ETIM_GetTimeStr(schar *pTimeStr);
extern sint ETIM_GetTimeEpoch(sint *pSec);
extern sint ETIM_GetTimeEpochNsec(slong *pSec);
extern sint ETIM_SyncSetTime(const EtimTm_t *pTime);
extern sint ETIM_SyncSetTimeStr(const schar *pTimeStr);
extern sint ETIM_SyncPingNtpsvr(void);
extern sint ETIM_ReadHwCount(ulong *res);
extern sint ETIM_AsyncStartSyncNtpsvr(EtimNtpInfo_t* ntpInfo);
extern sint ETIM_SyncGetSyncState(EtimNtpState_t* state);

#endif /*__ETIM_G_H__*/


