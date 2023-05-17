/****************************************************************************/
/*	Description		:Timer框架												*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC yufeng.yang										*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/14												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:frm_timer												*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "task_g.h"
#include "dlog_g.h"
#include "frm_timer.h"
#include "frm_msg_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
/* Timer模块描述数据 */
const ModDesc_t s_frmTimerModDesc = {
	MOD_TIMER, 0u, NULL, NULL, NULL
};

/* Timer模块综合描述数据 */
ModDescEx_t s_frmTimerModDescEx = {
	&s_frmTimerModDesc,
	0,
	0,
	NULL,
	0u,
	1u,
	{},
#ifdef ENABLE_STATISTIC
	0,
	0
#endif
};

/* Timer内存池 */
FrmTimerMemory_t s_frmTimerMemoryPool[FRM_TIMER_MEMORY_MAX_CNT];
/* タイマー登録リスト先頭 */
FrmTimerReg_t *s_frmTimerTimerListHead;
/* タイマー登録リスト排他制御用 */
pthread_mutex_t s_frmTimerTimerMutex;
/* アラーム登録リスト先頭 */
FrmAlarmReg_t *s_frmTimerAlarmListHead;
/* アラーム登録リスト排他制御用 */
pthread_mutex_t s_frmTimerFrmAlarmMutex;
/* Timer线程私有数据Key */
pthread_key_t s_frmTimerThPrivKey;
/* Timer线程ID */
pthread_t s_frmTimerTid;
/* Timer最大负载 */
ulong s_frmTimerMaxLoad;
/* Timer当前负载 */
ulong s_frmTimerCurLoad;
/* Timer负载测量计数 */
ulong s_frmTimerLoadMeasureCnt;

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description		:初始化Timer框架										*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	Other:异常										*/
/*--------------------------------------------------------------------------*/
/*	detail			:初始化Timer框架										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
slong FRM_TimerInit()
{
	DEBUG_TRCIN();
	s_frmTimerThPrivKey = FRM_GetPriKey();
	s_frmTimerMaxLoad = 0u;
	s_frmTimerCurLoad = 0u;
	s_frmTimerLoadMeasureCnt = 0u;
	s_frmTimerTimerListHead = NULL;
	s_frmTimerAlarmListHead = NULL;
	pthread_mutex_init(&s_frmTimerTimerMutex, NULL);
	pthread_mutex_init(&s_frmTimerFrmAlarmMutex, NULL);
	memset(s_frmTimerMemoryPool, 0x00u, sizeof(s_frmTimerMemoryPool));
	pthread_create(&s_frmTimerTid, NULL, frm_WatchThread,
					&s_frmTimerModDescEx);
	DEBUG_TRCOUT();
	return FRM_SUCCESS;
}

/*==========================================================================*/
/*	Description		:设定Timer												*/
/*--------------------------------------------------------------------------*/
/*	param			:ushort timerId:Timer ID								*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong msec:延时毫秒数									*/
/*--------------------------------------------------------------------------*/
/*	param			:const FrmTimerOpt *opt:可选参数						*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	Other:异常										*/
/*--------------------------------------------------------------------------*/
/*	detail			:设定Timer，Timeout后会通过消息通知设定者				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
slong FRM_TimerSetOneShot(ushort timerId, ulong msec, const FrmTimerOpt *opt)
{
	ModDescEx_t *modDescEx;
	struct timespec timesSpec;
	FrmTimerReg_t *plist;
	FrmTimerReg_t *pprev;
	ulong ssec;
	ulong snsec;
	sint ret;
	ulong srcModId;
	uchar nodeIsFound = 0;

	DEBUG_TRCIN();

	modDescEx = (ModDescEx_t *)pthread_getspecific(s_frmTimerThPrivKey);
	if (!modDescEx)
	{
		DLOG_Error("Null pointer");
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}

	srcModId = modDescEx->modDesc->modId;
	/* システム時刻取得（起動時間） */
	ret = clock_gettime(CLOCK_MONOTONIC_RAW, &timesSpec);
	if (ret != 0) {
		DLOG_Error("clock_gettime() error.\n");
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}

	/* 排他ロック */
	pthread_mutex_lock(&s_frmTimerTimerMutex);

	/* 登録リスト確認 */
	pprev = NULL;
	plist = s_frmTimerTimerListHead;
	while ((!nodeIsFound) && (plist != NULL)) {

		/* 同登録有り */
		if ((srcModId == plist->destModId) &&
			 (timerId == plist->id)) {
			nodeIsFound = 1;
		}
		else
		{
			pprev = plist;
			plist = plist->next;
		}
	}

	/* 新規リスト追加 */
	if (plist == NULL) {
		/* メモリ確保 */
		plist = (FrmTimerReg_t *)frm_TimerMalloc(sizeof(FrmTimerReg_t));
		if (plist == NULL) {
			DLOG_Error("frm_TimerMalloc() error.\n");
			DEBUG_TRCOUT();
			/* 排他アンロック */
			pthread_mutex_unlock(&s_frmTimerTimerMutex);
			return FRM_ERROR;
		}
		else
		{
			/* 処理無し */
		}

		/* 初期化 */
		memset(plist, 0x00, sizeof(FrmTimerReg_t));

		/* リスト接続 */
		if (s_frmTimerTimerListHead == NULL) {
			s_frmTimerTimerListHead = plist;
		}
		else
		{
			/* 処理無し */
		}
		if (pprev != NULL) {
			pprev->next = plist;
		}
		else
		{
			/* 処理無し */
		}
		plist->prev = pprev;
	}
	else
	{
		/* 処理無し */
	}

	/* 登録時刻計算 */
	ssec = timesSpec.tv_sec + (msec / 1000);
	snsec = timesSpec.tv_nsec + FRM_TIMER_M_NSEC(msec % 1000);
	if (snsec > FRM_TIMER_M_NSEC(1000)) {
		ssec += snsec / FRM_TIMER_M_NSEC(1000);
		snsec = snsec % FRM_TIMER_M_NSEC(1000);
	}

	/* 情報を設定 */
	plist->destModId = srcModId;
	plist->id = timerId;
	plist->expectTime.tv_sec = ssec;
	plist->expectTime.tv_nsec = snsec;
	if (opt)
	{
		memcpy(&plist->opt, opt, sizeof(plist->opt));
	}
	else
	{
		/* 処理無し */
	}

	/* 排他アンロック */
	pthread_mutex_unlock(&s_frmTimerTimerMutex);

	DEBUG_TRCOUT();
	return FRM_SUCCESS;
}

/*==========================================================================*/
/*	Description		:取消Timer												*/
/*--------------------------------------------------------------------------*/
/*	param			:ushort timerId:Timer ID								*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	Other:异常										*/
/*--------------------------------------------------------------------------*/
/*	detail			:取消Timer												*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
slong FRM_TimerCancelOneShot(ushort timerId)
{
	FrmTimerReg_t *plist;
	ModDescEx_t *modDescEx;
	ulong srcModId;
	uchar nodeIsFound = 0u;

	DEBUG_TRCIN();
	modDescEx = (ModDescEx_t *)pthread_getspecific(s_frmTimerThPrivKey);
	if (!modDescEx)
	{
		DLOG_Error("Null pointer");
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}
	srcModId = modDescEx->modDesc->modId;
	/* 排他ロック */
	pthread_mutex_lock(&s_frmTimerTimerMutex);

	/* 登録リスト確認 */
	plist = s_frmTimerTimerListHead;
	while ((!nodeIsFound) && (plist != NULL)) {

		/* 同登録有り */
		if (srcModId == plist->destModId &&
			 (timerId == plist->id)) {
			nodeIsFound = 1;
		}
		else
		{
			plist = plist->next;
		}
	}

	/* リスト削除 */
	if (plist != NULL) {

		/* このリストが先頭 */
		if (plist == s_frmTimerTimerListHead) {
			/* 削除するので先頭を次へ移動 */
			s_frmTimerTimerListHead = plist->next;
		}
		else
		{
			/* 処理無し */
		}

		/* リストメンテ */
		if (plist->next != NULL) {
			plist->next->prev = plist->prev;
		}
		else
		{
			/* 処理無し */
		}

		if (plist->prev != NULL) {
			plist->prev->next = plist->next;
		}
		else
		{
			/* 処理無し */
		}

		/* メモリ開放 */
		frm_TimerFree(plist);
	}
	else
	{
		/* 処理無し */
	}

	/* 排他アンロック */
	pthread_mutex_unlock(&s_frmTimerTimerMutex);

	DEBUG_TRCOUT();
	return FRM_SUCCESS;
}

/*==========================================================================*/
/*	Description		:设定Alarm												*/
/*--------------------------------------------------------------------------*/
/*	param			:ushort alarmId:Alarm ID								*/
/*--------------------------------------------------------------------------*/
/*	param			:const FrmTime_t *pTime:设定时刻						*/
/*--------------------------------------------------------------------------*/
/*	param			:const FrmTimerOpt *opt:可选参数						*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	Other:异常										*/
/*--------------------------------------------------------------------------*/
/*	detail			:设定Alarm，到达设定时刻后会通过消息通知设定者				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
slong FRM_TimerSetAlarm(ushort alarmId,
						const FrmTime_t *pTime, const FrmTimerOpt *opt)
{
	struct timespec timesSpec;
	FrmAlarmReg_t *plist;
	FrmAlarmReg_t *pprev;
	slong ret;
	time_t timeT;
	struct tm timeTm;
	ModDescEx_t *modDescEx;
	ulong srcModId;
	uchar nodeIsFound = 0u;

	DEBUG_TRCIN();
	modDescEx = (ModDescEx_t *)pthread_getspecific(s_frmTimerThPrivKey);
	/* 引数チェック */
	if ((!pTime) || (!modDescEx))
	{
		DLOG_Error("Null pointer");
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}
	srcModId = modDescEx->modDesc->modId;

	/* 時刻データ設定 */
	ret = frm_FrmTime2Tm(pTime, &timeTm);
	if (ret != FRM_SUCCESS) {
		DEBUG_TRCOUT();
		return ret;
	}
	else
	{
		/* 処理無し */
	}

	/* 範囲確認 */
	ret = frm_CheckDateRange(&timeTm);
	if (ret != FRM_SUCCESS) {
		DEBUG_TRCOUT();
		return ret;
	}
	else
	{
		/* 処理無し */
	}

	/* 登録時刻計算 */
	memset(&timesSpec, 0x00, sizeof(timesSpec));
	timeT = mktime(&timeTm);
	if (-1 == timeT) {
		DLOG_Error("mktime() error.\n");
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}
	timesSpec.tv_sec = timeT;

	/* 排他ロック */
	pthread_mutex_lock(&s_frmTimerFrmAlarmMutex);

	/* 登録リスト確認 */
	pprev = NULL;
	plist = s_frmTimerAlarmListHead;
	while ((!nodeIsFound) && (plist != NULL)) {

		/* 同登録有り */
		if ((srcModId == plist->destModId) &&
			 (alarmId == plist->id)) {
			nodeIsFound = 1;
		}
		else
		{
			pprev = plist;
			plist = plist->next;
		}
	}

	/* 新規リスト追加 */
	if (plist == NULL) {
		/* メモリ確保 */
		plist = (FrmAlarmReg_t *)frm_TimerMalloc(sizeof(FrmAlarmReg_t));
		if (plist == NULL) {
			DLOG_Error("frm_TimerMalloc() error.\n");
			DEBUG_TRCOUT();
			/* 排他アンロック */
			pthread_mutex_unlock(&s_frmTimerFrmAlarmMutex);
			return FRM_ERROR;
		}

		/* 初期化 */
		memset(plist, 0x00, sizeof(FrmAlarmReg_t));

		/* リスト接続 */
		if (s_frmTimerAlarmListHead == NULL) {
			s_frmTimerAlarmListHead = plist;
		}
		if (pprev != NULL) {
			pprev->next = plist;
		}
		plist->prev = pprev;
	}
	else
	{
		/* 処理無し */
	}

	/* 情報を設定 */
	plist->destModId = srcModId;
	plist->id = alarmId;
	plist->expectTime.tv_sec = timesSpec.tv_sec;
	plist->expectTime.tv_nsec = timesSpec.tv_nsec;
	if (opt)
	{
		memcpy(&plist->opt, opt, sizeof(plist->opt));
	}
	else
	{
		/* 処理無し */
	}

	/* 排他アンロック */
	pthread_mutex_unlock(&s_frmTimerFrmAlarmMutex);

	DEBUG_TRCOUT();
	return FRM_SUCCESS;
}

/*==========================================================================*/
/*	Description		:取消Alarm												*/
/*--------------------------------------------------------------------------*/
/*	param			:ushort alarmId:Alarm ID								*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	Other:异常										*/
/*--------------------------------------------------------------------------*/
/*	detail			:取消Alarm												*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
slong FRM_TimerCancelAlarm(ushort alarmId)
{
	FrmAlarmReg_t *plist;
	ModDescEx_t *modDescEx;
	ulong srcModId;
	uchar nodeIsFound = 0u;

	DEBUG_TRCIN();
	/* 引数チェック */
	modDescEx = (ModDescEx_t *)pthread_getspecific(s_frmTimerThPrivKey);
	if (!modDescEx)
	{
		DLOG_Error("Null pointer");
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}
	srcModId = modDescEx->modDesc->modId;

	/* 排他ロック */
	pthread_mutex_lock(&s_frmTimerFrmAlarmMutex);

	/* 登録リスト確認 */
	plist = s_frmTimerAlarmListHead;
	while ((!nodeIsFound) && (plist != NULL)) {

		/* 同登録有り */
		if ((srcModId == plist->destModId) &&
			 (alarmId == plist->id)) {
			nodeIsFound = 1;
		}
		else
		{
			plist = plist->next;
		}
	}

	/* リスト削除 */
	if (plist != NULL) {

		/* このリストが先頭 */
		if (plist == s_frmTimerAlarmListHead) {
			/* 削除するので先頭を次に設定 */
			s_frmTimerAlarmListHead = plist->next;
		}
		else
		{
			/* 処理無し */
		}

		/* リストメンテ */
		if (plist->next != NULL) {
			plist->next->prev = plist->prev;
		}
		else
		{
			/* 処理無し */
		}

		if (plist->prev != NULL) {
			plist->prev->next = plist->next;
		}
		else
		{
			/* 処理無し */
		}

		/* メモリ開放 */
		frm_TimerFree(plist);
	}
	else
	{
		/* 処理無し */
	}

	/* 排他アンロック */
	pthread_mutex_unlock(&s_frmTimerFrmAlarmMutex);

	DEBUG_TRCOUT();
	return FRM_SUCCESS;
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:监控Timer												*/
/*--------------------------------------------------------------------------*/
/*	param			:struct timespec *ptms:当前时间						*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	Other:异常										*/
/*--------------------------------------------------------------------------*/
/*	detail			:监控Timer列表中的Timer，如果Timeout，通知设定者			*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
static slong frm_WatchTimer(struct timespec *ptms)
{
	FrmTimerReg_t *plist;
	FrmTimerReg_t *pprev;
	FrmTimeout_t result;
	DEBUG_TRCIN();

	/* 引数確認 */
	if (ptms == NULL) {
		DLOG_Error("parameter NULL\n");
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}

	/* タイマー登録リスト排他ロック */
	pthread_mutex_lock(&s_frmTimerTimerMutex);

	/* タイマー監視ループ */
	pprev = NULL;
	plist = s_frmTimerTimerListHead;
	while (NULL != plist) {
		/* タイマー値比較 */
		if ((plist->expectTime.tv_sec < ptms->tv_sec) ||
			((plist->expectTime.tv_sec == ptms->tv_sec) &&
			(plist->expectTime.tv_nsec <= ptms->tv_nsec)))
		{
			/* タイムアップイベント送信 */
			memset(&result, 0x00, sizeof(FrmTimeout_t));
			result.header.msgId = LFW_MSGID_TIMER_TIMEOUT;
			result.timerId = plist->id;
			memcpy(&result.opt, &plist->opt, sizeof(result.opt));

			FRM_MsgSendEvent(plist->destModId, sizeof(FrmTimeout_t), &result);

			/* 登録リストメンテナンス */

			/* 次リストのprevへ現リストのprevを接続 */
			if (plist->next != NULL) {
				plist->next->prev = plist->prev;
			}
			else
			{
				/* 処理無し */
			}

			/* 前リストのnextへ現リストのnextを接続 */
			if (pprev != NULL) {
				pprev->next = plist->next;
			}
			else {
				/* 前リストが無い＝先頭へ現リストのnextを格納 */
				s_frmTimerTimerListHead = plist->next;
			}

			/* 現リストを開放 */
			frm_TimerFree(plist);

			/* 前リストのnextを次の検索対象とする */
			if (pprev != NULL) {
				plist = pprev->next;
			}
			else {
				/* 前リストが無い＝更新した先頭を次の検索対象とする */
				plist = s_frmTimerTimerListHead;
			}

		}
		else {
			/* 現リストを前リストへ */
			pprev = plist;
			/* 現リストの次を検索対照とする */
			plist = plist->next;
		}
	}

	/* タイマー登録リスト排他アンロック */
	pthread_mutex_unlock(&s_frmTimerTimerMutex);
	DEBUG_TRCOUT();
	return FRM_SUCCESS;
}

/*==========================================================================*/
/*	Description		:监控Alarm												*/
/*--------------------------------------------------------------------------*/
/*	param			:struct timespec *ptms:当前时间						*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	Other:异常										*/
/*--------------------------------------------------------------------------*/
/*	detail			:监控Alarm列表中的Alarm，如果到达设定时刻，通知设定者		*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
static slong frm_WatchAlarm(struct timespec *ptms)
{
	FrmAlarmReg_t *plist;
	FrmAlarmReg_t *pprev;
	FrmTimeout_t result;
	DEBUG_TRCIN();

	/* 引数確認 */
	if (ptms == NULL) {
		DLOG_Error("parameter NULL\n");
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}

	/* アラーム登録リスト排他ロック */
	pthread_mutex_lock(&s_frmTimerFrmAlarmMutex);

	/* アラーム監視ループ */
	pprev = NULL;
	plist = s_frmTimerAlarmListHead;
	while (NULL != plist) {
		/* アラーム値比較 */
		if ((plist->expectTime.tv_sec < ptms->tv_sec) ||
			((plist->expectTime.tv_sec == ptms->tv_sec) &&
			(plist->expectTime.tv_nsec <= ptms->tv_nsec))) {

			/* タイムアップイベント送信 */
			memset(&result, 0x00, sizeof(FrmTimeout_t));
			result.header.msgId = LFW_MSGID_ALARM_TIMEOUT;
			result.timerId = plist->id;
			memcpy(&result.opt, &plist->opt, sizeof(result.opt));

			FRM_MsgSendEvent(plist->destModId, sizeof(FrmTimeout_t), &result);

			/* 登録リストメンテナンス */

			/* 次リストのprevへ現リストのprevを接続 */
			if (plist->next != NULL) {
				plist->next->prev = plist->prev;
			}
			else
			{
				/* 処理無し */
			}

			/* 前リストのnextへ現リストのnextを接続 */
			if (pprev != NULL) {
				pprev->next = plist->next;
			}
			else {
				/* 前リストが無い＝先頭へ現リストのnextを格納 */
				s_frmTimerAlarmListHead = plist->next;
			}

			/* 現リストを開放 */
			frm_TimerFree(plist);

			/* 前リストのnextを次の検索対象とする */
			if (pprev != NULL) {
				plist = pprev->next;
			}
			else {
				/* 前リストが無い＝更新した先頭を次の検索対象とする */
				plist = s_frmTimerAlarmListHead;
			}

		}
		else {
			/* 現リストを前リストへ */
			pprev = plist;
			/* 現リストの次を検索対照とする */
			plist = plist->next;
		}
	}

	/* アラーム登録リスト排他アンロック */
	pthread_mutex_unlock(&s_frmTimerFrmAlarmMutex);
	DEBUG_TRCOUT();
	return FRM_SUCCESS;
}

/*==========================================================================*/
/*	Description		:监控线程入口											*/
/*--------------------------------------------------------------------------*/
/*	param			:void *arg:Timer模块管理信息							*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	Other:异常										*/
/*--------------------------------------------------------------------------*/
/*	detail			:监控线程入口函数，监控Timer和Alarm						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
static void *frm_WatchThread(void *arg)
{
	struct timespec req;		/* スリープ要求時間 */
	struct timespec rem;		/* スリープ残り時間 */
	struct timespec timesSpec;	/* スレッド処理前時間 */
	struct timespec tmr;		/* 現在のシステム時刻 */
	struct timespec tme;		/* スレッド処理後時間 */
	sint ret;
	schar ebuf[256];
	uchar sleepDone = 0u;

	DEBUG_TRCIN();
	/* 引数確認 */
	if (arg == NULL) {
		DLOG_Error("parameter NULL\n");
		DEBUG_TRCOUT();
		return NULL;
	}
	else
	{
		/* 処理無し */
	}
	pthread_setspecific(s_frmTimerThPrivKey, arg);
	/* スレッドメインループ */
	while(1) {
		/***** タイマー監視 *****/
		/* システム時刻取得（起動時間）*/
		ret = clock_gettime(CLOCK_MONOTONIC_RAW, &timesSpec);
		if (ret != 0) {
			DLOG_Error("clock_gettime(CLOCK_MONOTONIC_RAW) error.\n");
			timesSpec.tv_sec = -1;
		}
		else
		{
			ret = frm_WatchTimer(&timesSpec);
			if (ret != FRM_SUCCESS) {
				DLOG_Error("frm_WatchTimer() error.\n");
			}
			else
			{
				/* 処理無し */
			}
		}

		/***** アラーム監視 *****/

		/* システム時刻取得（通常時間）*/
		ret = clock_gettime(CLOCK_REALTIME, &tmr);
		if (ret != 0) {
			DLOG_Error("clock_gettime(CLOCK_REALTIME) error.\n");
		}
		else
		{
			ret = frm_WatchAlarm(&tmr);
			if (ret != FRM_SUCCESS) {
				DLOG_Error("frm_WatchAlarm() error.\n");
			}
			else
			{
				/* 処理無し */
			}
		}

		memset(&req, 0x00, sizeof(req));
		memset(&rem, 0x00, sizeof(rem));

		if (timesSpec.tv_sec == -1) {

			/* 起動時に時刻取得できてない場合10msecを指定 */
			req.tv_nsec = FRM_TIMER_M_NSEC(FRM_TIMER_INTERVAL_MS);

		}
		else {
			/* スレッド処理にかかった時間を10msecから引いてスリープする */

			/* スレッド処理後時間取得（起動時間）*/
			ret = clock_gettime(CLOCK_MONOTONIC_RAW, &tme);
			if (ret != 0) {
				DLOG_Error("clock_gettime(CLOCK_MONOTONIC_RAW)-2 error.\n");

				/* 時刻取得できない場合10msecを指定 */
				req.tv_nsec = FRM_TIMER_M_NSEC(FRM_TIMER_INTERVAL_MS);
			}
			else
			{
				/* 10msecからの残り時間を計算 */
				req.tv_nsec = FRM_TIMER_M_NSEC(FRM_TIMER_INTERVAL_MS) -
					((FRM_TIMER_M_NSEC(1000) * (tme.tv_sec - timesSpec.tv_sec))
					 + (tme.tv_nsec - timesSpec.tv_nsec));

				if (req.tv_nsec < FRM_TIMER_M_NSEC(1)) {
					/* 計算結果が1msecより小さい場合、1msecを指定 */
					req.tv_nsec = FRM_TIMER_M_NSEC(1);
				}
				else
				{
					/* 処理無し */
				}
			}
		}

		/* スリープ処理 */
		sleepDone = 0u;
		while(!sleepDone) {

			ret = nanosleep(&req, &rem);
			if (ret == 0) {
				/* nanosleep()が正常終了 */
				sleepDone = 1u;
			}
			else
			{
				if (errno == EINTR) {
					/* nanosleep()がシグナルで停止したので残り時間分再sleep */
					req.tv_sec = rem.tv_sec;
					req.tv_nsec = rem.tv_nsec;
					DLOG_Error("nanosleep() stop by signal.(%d)\n", errno);
				}
				else {
					/* nanosleep()がエラー */
					memset(ebuf, 0x00, sizeof(ebuf));
					strerror_r(errno, ebuf, 256);
					DLOG_Error("nanosleep() error.(%d,%s)\n", errno, ebuf);
					sleepDone = 1;
				}
			}
		}

	} /* スレッドメインループEnd */
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:时刻转换												*/
/*--------------------------------------------------------------------------*/
/*	param			:const FrmTime_t *pTime:Alarm设定时刻					*/
/*--------------------------------------------------------------------------*/
/*	param			:struct tm *ptm:出力tm结构								*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	Other:异常										*/
/*--------------------------------------------------------------------------*/
/*	detail			:Alarm设定时刻转换为tm结构								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
static slong frm_FrmTime2Tm(const FrmTime_t *pTime, struct tm *ptm)
{
	DEBUG_TRCIN();
	if ((!pTime) || (!ptm))
	{
		DLOG_Error("Null pointer");
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}
	ptm->tm_year = pTime->year - 1900;
	ptm->tm_mon = pTime->mon - 1;
	ptm->tm_mday = pTime->day;
	ptm->tm_hour = pTime->hour;
	ptm->tm_min = pTime->min;
	ptm->tm_sec = pTime->sec;

	DEBUG_TRCOUT();
	return FRM_SUCCESS;
}

/*==========================================================================*/
/*	Description		:时刻范围合法性判定										*/
/*--------------------------------------------------------------------------*/
/*	param			:const struct tm *ptm:时刻								*/
/*--------------------------------------------------------------------------*/
/*	return			:0:正常	Other:异常										*/
/*--------------------------------------------------------------------------*/
/*	detail			:时刻范围合法性判定										*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
static slong frm_CheckDateRange(const struct tm *ptm)
{
	DEBUG_TRCIN();
	/* 引数チェック */
	if (ptm == NULL) {
		DLOG_Error("parameter NULL.\n");
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}
	/* 年 */
	if ((ptm->tm_year < 70) || (ptm->tm_year > 138)) {
		DLOG_Error("year out of range.(%d)\n", ptm->tm_year);
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}
	/* 月 */
	if ((ptm->tm_mon < 0) || (ptm->tm_mon > 11)) {
		DLOG_Error("month out of range.(%d)\n", ptm->tm_mon);
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}
	/* 日 */
	if ((ptm->tm_mday < 1) || (ptm->tm_mday > 31)) {
		DLOG_Error("day out of range.(%d)\n", ptm->tm_mday);
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}
	/* 時 */
	if ((ptm->tm_hour < 0) || (ptm->tm_hour > 23)) {
		DLOG_Error("hour out of range.(%d)\n", ptm->tm_hour);
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}
	/* 分 */
	if ((ptm->tm_min < 0) || (ptm->tm_min > 59)) {
		DLOG_Error("min out of range.(%d)\n", ptm->tm_min);
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}
	/* 秒 */
	if ((ptm->tm_sec < 0) || (ptm->tm_sec > 60)) {
		DLOG_Error("sec out of range.(%d)\n", ptm->tm_sec);
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}
	/* 2038/1/1まで許容 */
	if (ptm->tm_year == 138) {
		if ((ptm->tm_mon > 0) || (ptm->tm_mday > 1)) {
			DLOG_Error("over 2038/01/01 23:59:59. out of range.\n");
			DEBUG_TRCOUT();
			return FRM_ERROR;
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
	/* 日本時間で 1970/01/01 09:00:00 以前は不可 */
	if ((ptm->tm_year) == 70 &&
		(ptm->tm_mon) == 0 &&
		(ptm->tm_mday) == 1 &&
		(ptm->tm_hour < 9)) {
		DLOG_Error("under 1970/01/01 09:00:00. out of range.\n");
		DEBUG_TRCOUT();
		return FRM_ERROR;
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();
	return FRM_SUCCESS;
}

/*==========================================================================*/
/*	Description		:显示内存池使用情况										*/
/*--------------------------------------------------------------------------*/
/*	param			:void													*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:显示内存池使用情况，供Debug使用						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
#ifdef __DEBUG__
static void frm_TimerDisplayMemoryPool(void)
{
	DEBUG_TRCIN();
	ulong i;
	for (i = 0u; i < FRM_TIMER_MEMORY_MAX_CNT; i++)
	{
		DLOG_Debug("pool[%d].use=%d", i, s_frmTimerMemoryPool[i].isUsing);
	}
}
#endif

/*==========================================================================*/
/*	Description		:申请Timer内存											*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong size:内存大小									*/
/*--------------------------------------------------------------------------*/
/*	return			:void *													*/
/*--------------------------------------------------------------------------*/
/*	detail			:从内存池申请Timer内存									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
static void *frm_TimerMalloc(ulong size)
{
	ulong i;
	void *ptr = NULL;
	DEBUG_TRCIN();

	if (size <= sizeof(FrmTimerRegEntity_t))
	{
		for (i = 0u; (i < FRM_TIMER_MEMORY_MAX_CNT) && (NULL == ptr); i++)
		{
			if (!s_frmTimerMemoryPool[i].isUsing)
			{
				s_frmTimerMemoryPool[i].isUsing = 1u;
				ptr = &s_frmTimerMemoryPool[i];
				s_frmTimerCurLoad++;
				if (s_frmTimerMaxLoad < s_frmTimerCurLoad)
				{
					s_frmTimerMaxLoad = s_frmTimerCurLoad;
				}
				else
				{
					/* 処理無し */
				}
				s_frmTimerLoadMeasureCnt++;
				s_frmTimerLoadMeasureCnt = s_frmTimerLoadMeasureCnt % 
											FRM_TIMER_LOAD_MEASURE_DIV;
				if (0 == s_frmTimerLoadMeasureCnt)
				{
					DLOG_Debug("MemoryPool Load:Current[%lu] Max[%lu]",
								s_frmTimerCurLoad, s_frmTimerMaxLoad);
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
		}
	}
	else
	{
		/* 処理無し */
	}

#ifdef __DEBUG__
	frm_TimerDisplayMemoryPool();
#endif
	DEBUG_TRCOUT();
	return ptr;
}

/*==========================================================================*/
/*	Description		:释放Timer内存											*/
/*--------------------------------------------------------------------------*/
/*	param			:void *ptr:Timer内存指针								*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:释放Timer内存											*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/14 : DHC Chen.H : 新規作成					*/
/*==========================================================================*/
static void frm_TimerFree(void *ptr)
{
	FrmTimerMemory_t *pMemory = (FrmTimerMemory_t *)ptr;
	DEBUG_TRCIN();

	if (pMemory)
	{
		pMemory->isUsing = 0u;
		s_frmTimerCurLoad--;
	}
	else
	{
		/* 処理無し */
	}
#ifdef __DEBUG__
	frm_TimerDisplayMemoryPool();
#endif
	DEBUG_TRCOUT();
}

/*End Of File*/
