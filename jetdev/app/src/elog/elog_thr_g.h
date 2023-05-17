/****************************************************************************/
/*	Description		:ログ管理ミドル スレッド関連ヘッダ						*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Jiang.Z											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/05/09												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __ELOG_THR_G_H__
#define __ELOG_THR_G_H__


#include "elog_g.h"
#include <pthread.h>
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define ELOG_ALLPERMS 					(0777)
/* USB保存		: 00000001 */
#define	ELOG_THTYPE_USB					(0x00000001)
/* 圧縮			: 00000010 */
#define	ELOG_THTYPE_CPRS				(0x00000002)
/* ログリスト	: 00001000 */
#define	ELOG_THTYPE_LLST				(0x00000008)
/* ログ削除		: 00010000 */
#define	ELOG_THTYPE_LDEL				(0x00000010)
#define ELOG_FILE_NAME_MAX				(uchar)(32u)

/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
/***********************************************************
* @enum		ElogThstate_t
* @brief	スレッド実行状態
************************************************************/
typedef enum{
	ELOG_THSTATE_IDLE = 0,		/* スレッド未実行       */
	ELOG_THSTATE_START,			/* スレッド処理開始     */
	ELOG_THSTATE_RUN,			/* スレッド実行中       */
	ELOG_THSTATE_ABORTED,		/* スレッド処理外部中断 */
	ELOG_THSTATE_INCOMPLETE,	/* スレッド処理異常終了 */
	ELOG_THSTATE_COMPLETE		/* スレッド処理正常終了 */
} ElogThstate_t;

/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/
/***********************************************************
* @struct	ElogUsbSave_t
* @brief	USB保存スレッドデータ
************************************************************/
typedef struct ElogUsbSave{
	ElogFileList_t *filelist;
	ElogFileList_t *dirList;
	ulong tTime;
	ElogFncUsbSave_t msgData;
}ElogUsbSave_t;

/***********************************************************
* @struct	ElogTgzSave_t
* @brief	圧縮スレッドデータ
************************************************************/
typedef struct ElogTgzSave{
	ElogType_t type;					/* ログ種別				*/
	FrmMsgHead_t header;				/* メッセージヘッダ		*/
	schar fileName[ELOG_FILE_NAME_MAX];	/* 圧縮ファイル名		*/
	bool_t del;							/* 削除要否				*/
	bool_t branched;
} ElogTgzSave_t;
/***********************************************************
* @struct	ElogThdata_t
* @brief	スレッド管理データ
************************************************************/
typedef struct ElogThdata{
	pthread_t id;				/* スレッドID		 */
	void* (*start)();			/* スレッド関数		 */
	ElogThstate_t state;		/* ステータス		 */
	void* data;					/* スレッド利用データ */
	pthread_mutex_t sMutex;		/* ステータス排他用	 */
	pthread_mutex_t dMutex;		/* データ排他用		 */
} ElogThdata_t;
extern ElogThdata_t s_elogThrUSB;
extern ElogThdata_t s_elogThrCompress;
extern ElogThdata_t s_elogThrNandDel;
extern ElogThdata_t s_elogThrGetLogList;
extern ElogThdata_t s_elogThrDeleteLog;

/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern void ELOG_ThrInit(void);
extern slong ELOG_ThrStart(ElogThdata_t *thData);
extern slong  elog_thr_check(ElogThdata_t *thData);
extern slong  elog_mutex_check(pthread_mutex_t *pmutex);
extern slong  ELOG_ThrGetState(ElogThdata_t *thData, ElogThstate_t *state);
extern slong  ELOG_ThrState(ElogThdata_t *thData, ElogThstate_t state);
extern void ELOG_thrSendUsbInProgress(bool_t first);
extern void ELOG_SetUsbAbort(bool_t abort);
extern bool_t ELOG_GetUsbAbort(void);
extern void ELOG_SendCompressEvent(
		ulong port, ElogCompress_t state, ElogType_t type,
		schar* savepath, bool_t branched);
extern void ELOG_SetNandDeleteAbort(bool_t f);
extern bool_t ELOG_GetNandDeleteAbort(void);
extern slong ELOG_AbortAndWaitNandDelete(bool_t wait);
extern slong ELOG_CheckSubThreadRun(ulong mask);
extern void	elog_sendGetLogListEvent(ulong port, ElogGetLogList_t state);
extern void	elog_sendDelLogEvent(ulong port, ElogDeleteLog_t state);

#endif /* __ELOG_THR_G_H__ */
