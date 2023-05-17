/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Xu.Y												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/11												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EEPC_G_H__
#define __EEPC_G_H__
/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#include "frm_g.h"
#include "typedef.h"

/*--------------------------------------------------------------------------*/
/*	公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/
#define	EEPC_SCH_DIV_LEN			((uchar)5u)				/* (NULL終端込み)スケジュール区分の文字列サイズ					*/
#define	EEPC_FILE_PATH_LEN			((uchar)23u)			/* (NULL終端込み)伝送ファイルのファイルパスの文字列サイズ		*/
#define	EEPC_FILE_NAME_LEN			((uchar)56u)			/* (NULL終端込み)伝送ファイルのファイル名の文字列サイズ			*/
#define	EEPC_RESULT_TIME_LEN		((uchar)21u)			/* (NULL終端込み)電力会社との通信結果及び通信時間の文字列サイズ	*/
#define	EEPC_URL_LEN				((uchar)129u)			/* (NULL終端込み)電力会社サーバのURL文字列サイズ				*/
#define	EEPC_FQDN_LEN				((uchar)129u)			/* (NULL終端込み)電力会社指定のNTPサーバのFQDN文字列サイズ		*/
#define	EEPC_RTCER_NAME_LEN			((uchar)65u)			/* (NULL終端込み)ルート証明書ファイル名の最大文字列サイズ		*/
#define	EEPC_SRV_LIST_MAX			((uchar)10u)			/* 電力会社名のリストサイズMAX									*/
#define EEPC_SRV_LIST_MIN			((uchar)1u)				/* 電力会社名のリストサイズMIN									*/
#define	EEPC_SRV_NAME_LEN			((uchar)129u)			/* (NULL終端込み)電力会社名の文字列サイズ						*/
#define	EEPC_SRV_RES_INF			((uchar)100u)			/* 電力会社サーバのHTTPレスポンス:1xx(情報)						*/
#define	EEPC_SRV_RES_SCSS			((uchar)200u)			/* 電力会社サーバのHTTPレスポンス:2xx台(成功)					*/
#define	EEPC_SRV_RES_RDRCT			((ushort)300u)			/* 電力会社サーバのHTTPレスポンス:3xx台(リダイレクション)		*/
#define	EEPC_SRV_RES_CLNT_ERR		((ushort)400u)			/* 電力会社サーバのHTTPレスポンス:4xx台(クライアントエラー)		*/
#define	EEPC_SRV_RES_SRV_ERR		((ushort)500u)			/* 電力会社サーバのHTTPレスポンス:5xx台(サーバエラー)			*/
#define	EEPC_SCH_DIV_FIX			"999"					/* スケジュール区分:固定スケジュール(年間)						*/
#define	EEPC_SCH_DIV_UPDT			"0000"					/* スケジュール区分:更新スケジュール							*/
#define	EEPC_SCH_DIV_ID				"8888"					/* スケジュール区分:ID登録確認									*/
#define	EEPC_REG_NOTICE_DISABLE		((uchar)0u)				/* 設定変更通知イベント登録時の通知不要フラグ					*/
#define	EEPC_REG_NOTICE_ENABLE		((uchar)1u)				/* 設定変更通知イベント登録時の通知必要フラグ					*/
#define	EEPC_PWR_ST_ID				((uchar)27u)			/* (NULL終端込み)発電所IDの文字列サイズ							*/
#define EEPC_PROCESS_LEN			((uchar)1u)
#define EEPC_STR_END_LEN			((uchar)1u)
/********************************************************************************************************************/
/* 設定変更イベント通知用のフラグ定数										*/
/* 注意：EEPC_EVT01_T型にメンバを追加する毎に、EEPC_EVT01_CHG_*の追加が必要	*/
/* 注意：EEPC_EVT01_CHG_*の追加を行う場合、最下位bitから詰めて定義すること	*/
#define	EEPC_EVT01_CHG_CORP_DIV		(0x00000001)	/* EEPC_EVT01_T型のメンバ:corpDivの変更通知用					*/
#define	EEPC_EVT01_CHG_NTP_PORT		(0x00000002)	/* EEPC_EVT01_T型のメンバ:ntpPortの変更通知用					*/
#define	EEPC_EVT01_CHG_RES_TIME		(0x00000004)	/* EEPC_EVT01_T型のメンバ:resultTime[ ]の変更通知用				*/
#define	EEPC_EVT01_CHG_SRV_URL		(0x00000008)	/* EEPC_EVT01_T型のメンバ:srvUrlStr[ ]の変更通知用				*/
#define	EEPC_EVT01_CHG_NTP_FQDN		(0x00000010)	/* EEPC_EVT01_T型のメンバ:ntpFqdnStr[ ]の変更通知用				*/

/* 以下、設定変更イベント通知用のフラグ定数を組み合わせた定数	*/
/* 設定変更通知対象のフラグにてALL:ONを示す値					*/
/* 注意：EEPC_EVT01_CHG_*の追加毎に、加算を行うこと				*/
#define	EEPC_EVT01_CHG_ALL	(EEPC_EVT01_CHG_CORP_DIV | EEPC_EVT01_CHG_NTP_PORT | EEPC_EVT01_CHG_RES_TIME | EEPC_EVT01_CHG_SRV_URL | EEPC_EVT01_CHG_NTP_FQDN)
/********************************************************************************************************************/

/******************************************************************************
* @enum		EepcFncId_t
* @brief	ファンクションID
******************************************************************************/
typedef enum
{
	EEPC_FNC_ID_ASYNC_REQ_COM_START = 0,	/* 通信開始要求					*/
	EEPC_FNC_ID_ASYNC_UPDATE_SRV_INF,		/* 電力会社情報更新				*/
	EEPC_FNC_ID_ASYNC_CANCEL_SRV_INF,		/* 電力会社情報更新中断			*/
	EEPC_FNC_ID_SYNC_GET_SRV_INF,			/* 電力会社情報取得				*/
	EEPC_FNC_ID_SYNC_SET_SRV_INF,			/* 電力会社情報保存				*/
	EEPC_FNC_ID_SYNC_REG_EEPC_CHG_EVT,		/* 設定変更通知イベント登録		*/
	EEPC_FNC_ID_SYNC_CLR_EEPC_CHG_EVT,		/* 設定変更通知イベント登録解除	*/
	EEPC_FNC_ID_SYNC_GET_COM_INF			/* 通信先情報取得				*/
} EepcFncId_t;

/******************************************************************************
* @enum		EepcRtnCode_t
* @brief	EEPCリターンコード番号(同期/非同期の戻り値)
******************************************************************************/
typedef enum
{
	EEPC_RTN_CODE_OK	= 0,				/* 正常終了			*/
	EEPC_RTN_CODE_ESYS	= -1,				/* システム異常		*/
	EEPC_RTN_CODE_EPRM	= -2				/* パラメータ異常	*/
} EepcRtnCode_t;

/******************************************************************************
* @enum		EepcEvtCode_t
* @brief	EEPCイベントリザルト番号(イベントメッセージの戻り値)
******************************************************************************/
typedef enum
{
	EEPC_EVT_CODE_COM_OK		= 0,		/* 通信OK													*/
	EEPC_EVT_CODE_COM_NG		= -1,		/* 通信NG													*/
	EEPC_EVT_CODE_AUTH_ERR		= -2,		/* (H1PC_未使用)認証NG										*/
	EEPC_EVT_CODE_CONFLICT_SCH	= -3,		/* スケジュール受信の二重要求								*/
	EEPC_EVT_CODE_CONFLICT_ID	= -4,		/* ID登録確認の二重要求										*/
	EEPC_EVT_CODE_SYSERR		= -5,		/* システム異常(スレッド異常など)							*/
	EEPC_EVT_CODE_PROC_END		= -6,		/* プロセス終了要求を受信済み								*/
	EEPC_EVT_CODE_FILE_UPDATE	= -7		/* (H1PC_未使用)電力会社サーバ管理ファイル更新の二重要求	*/
} EepcEvtCode_t;

/******************************************************************************
* @enum		EepcEvtId_t
* @brief	EEPCイベントID(内部イベントを識別する値)
******************************************************************************/
typedef enum
{
	EEPC_EVT_ID_RES_SRV_COM	= 0,			/* 通信終了応答用			*/
	EEPC_EVT_ID_SRV_INF_UPDATE,				/* 電力会社情報更新終了用	*/
	EEPC_EVT_ID_NTC_CHG_SET					/* 設定変更通知用			*/
} EepcEvtId_t;

/******************************************************************************
* @enum		EepcCorpDiv_t
* @brief	電力会社区分(電力会社を識別する値)
******************************************************************************/
typedef enum
{
	EEPC_CORP_DIV_NONE	= 0,				/* 未設定		*/
	EEPC_CORP_DIV_HOKKAIDO,					/* 北海道電力	*/
	EEPC_CORP_DIV_TOHOKU,					/* 東北電力		*/
	EEPC_CORP_DIV_TOKYO,					/* 東京電力		*/
	EEPC_CORP_DIV_HOKURIKU,					/* 北陸電力		*/
	EEPC_CORP_DIV_CHUBU,					/* 中部電力		*/
	EEPC_CORP_DIV_KANSAI,					/* 関西電力		*/
	EEPC_CORP_DIV_CHUGOKU,					/* 中国電力		*/
	EEPC_CORP_DIV_SHIKOKU,					/* 四国電力		*/
	EEPC_CORP_DIV_KYUSYU,					/* 九州電力		*/
	EEPC_CORP_DIV_OKINAWA,					/* 沖縄電力		*/
	EEPC_CORP_DIV_OTHER						/* その他		*/
} EepcCorpDiv_t;

/******************************************************************************
* 構造体
******************************************************************************/
/******************************************************************************
* @struct	時刻情報用構造体
* @breaf	時刻情報用構造体
******************************************************************************/
typedef struct
{
	uchar	hour;							/* 時(0～23)				*/
	uchar	min;							/* 分(0～59)				*/
	uchar	sec;							/* 秒(0～60) ※閏秒を考慮	*/
	uchar	reserved;						/* 予備						*/
} EepcTimInf_t;

/******************************************************************************
* @struct	電力会社情報管理用構造体
* @breaf	電力会社情報管理用構造体
******************************************************************************/
typedef struct
{
	uchar	totalSrv;												/* 使用可能な電力会社の総数		*/
	uchar	useSrvIndex;											/* 使用中の電力会社のIndex値	*/
	schar	srvNameStrList[EEPC_SRV_LIST_MAX][EEPC_SRV_NAME_LEN];	/* 電力会社名のリスト			*/
	uchar	corpDiv[EEPC_SRV_LIST_MAX];								/* 電力会社区分のリスト			*/
} EepcSrvMng_t;

/******************************************************************************
* @struct	通信先情報用構造体
* @breaf	通信先情報用構造体
******************************************************************************/
typedef struct
{
	uchar	corpDiv;							/* 電力会社の区分						*/
	ulong	srvRtryCntFixSch;					/* 固定スケジュール受信のリトライ回数	*/
	ulong	srvRtryIntvlFixSch;					/* 固定スケジュール受信のリトライ間隔	*/
	ulong	srvRtryCntUpdtSch;					/* 更新スケジュール受信のリトライ回数	*/
	ulong	srvRtryIntvlUpdtSch;				/* 更新スケジュール受信のリトライ間隔	*/
	ulong	ntpPort;							/* 電力会社指定のNTPサーバのポート番号	*/
	ulong	ntpRtryCntTimSync;					/* 時刻同期のリトライ回数				*/
	ulong	ntpRtryIntvlTimSync;				/* 時刻同期のリトライ間隔				*/
	ulong	ntpRtryOvrIntvlTimSync;				/* 時刻同期の再リトライ間隔				*/
	schar	resultTime[EEPC_RESULT_TIME_LEN];	/* 電力会社との通信結果及び通信時間		*/
	schar	srvUrlStr[EEPC_URL_LEN];			/* 電力会社サーバのURL					*/
	schar	ntpFqdnStr[EEPC_FQDN_LEN];			/* 電力会社指定のNTPサーバのFQDN		*/
} EepcComInf_t;

/******************************************************************************
* @struct	同期時刻情報用構造体
* @breaf	同期時刻情報用構造体
******************************************************************************/
typedef struct
{
	EepcTimInf_t	ntpRtryStrtTimSync;		/* 時刻同期のリトライ開始時間	*/
	EepcTimInf_t	ntpRtryEndTimSync;		/* 時刻同期のリトライ終了時間	*/
} EepcSyncTimInf_t;

/******************************************************************************
* @struct	EEPCプロセス応答用構造体
* @breaf	EEPCプロセス応答用構造体
******************************************************************************/
typedef struct
{
	FrmMsgHead_t	header;					/* メッセージ構造ヘッダ	*/
	sint			result;					/* リターンコード		*/
} EepcPrcRes_t;

/******************************************************************************
* @struct	通信開始要求用構造体
* @breaf	通信開始要求用構造体
******************************************************************************/
typedef struct
{
	FrmMsgHead_t	header;							/* メッセージ構造ヘッダ						*/
	ulong			reqID;							/* 要求元への成否通知に利用する為の返送ID	*/
	uchar			seqID;							/* シーケンスID								*/
	schar			schDivStr[EEPC_SCH_DIV_LEN];	/* スケジュール区分							*/
	schar			pwrStID[EEPC_PWR_ST_ID];		/* 発電所ID									*/
} EepcComStart_t;

/******************************************************************************
* @struct	電力会社情報更新用構造体
* @breaf	電力会社情報更新用構造体
******************************************************************************/
typedef struct
{
	FrmMsgHead_t	header;					/* メッセージ構造ヘッダ						*/
	ulong			reqID;					/* 要求元への成否通知に利用する為の返送ID	*/
	uchar			seqID;					/* シーケンスID								*/
} EepcUpdtSrv_t;

/******************************************************************************
* @struct	電力会社情報保存用構造体
* @breaf	電力会社情報保存用構造体
******************************************************************************/
typedef struct
{
	FrmMsgHead_t	header;					/* メッセージ構造ヘッダ			*/
	uchar			useSrvIndex;			/* 使用中の電力会社のIndex値	*/
} EepcSetSrv_t;

/******************************************************************************
* @struct	設定変更通知イベント登録用構造体
* @breaf	設定変更通知イベント登録用構造体
******************************************************************************/
typedef struct
{
	FrmMsgHead_t	header;					/* メッセージ構造ヘッダ				*/
	sint			receiver;				/* イベント通知先のポート番号		*/
	ulong			eventType;				/* 設定変更通知イベント種別			*/
	uchar			noticerflg;				/* イベント登録後の通知要否フラグ	*/
} EepcRegChg_t;

/******************************************************************************
* @struct	設定変更通知イベント登録解除用構造体
* @breaf	設定変更通知イベント登録解除用構造体
******************************************************************************/
typedef struct
{
	FrmMsgHead_t	header;					/* メッセージ構造ヘッダ			*/
	sint			receiver;				/* イベント通知先のポート番号	*/
} EepcClrChg_t;

/******************************************************************************
* @struct	通信終了応答用構造体
* @breaf	通信終了応答用構造体
******************************************************************************/
typedef struct
{
	FrmMsgHead_t	header;								/* メッセージ構造ヘッダ				*/
	EepcEvtCode_t	comResult;							/* 電力会社サーバとの通信結果		*/
	uchar			seqID;								/* シーケンスID						*/
	ulong			resSts;								/* HTTPS通信のレスポンスステータス	*/
	schar			reqSchDiv[EEPC_SCH_DIV_LEN];		/* 通信開始時のスケジュール区分		*/
	schar			filePathStr[EEPC_FILE_PATH_LEN];	/* 伝送ファイルのファイルパス		*/
	schar			fileNameStr[EEPC_FILE_NAME_LEN];	/* 伝送ファイル名					*/
} EepcEvt00_t;

/******************************************************************************
* @struct	設定変更通知用構造体
* @breaf	設定変更通知用構造体
* @breaf	メンバ修正する場合、define値：EEPC_EVT01_CHG_*の定義も修正すること
******************************************************************************/
typedef struct
{
	FrmMsgHead_t	header;								/* メッセージ構造ヘッダ					 */
	ulong			chgDataFlg;							/* corpDiv～ntpFqdnStrの変更有無のフラグ */
	uchar			corpDiv;							/* 電力会社の区分						 */
	ulong			ntpPort;							/* 電力会社指定のNTPサーバのポート番号	 */
	schar			resultTime[EEPC_RESULT_TIME_LEN];	/* 電力会社との通信結果及び通信時間		 */
	schar			srvUrlStr[EEPC_URL_LEN];			/* 電力会社サーバのURL					 */
	schar			ntpFqdnStr[EEPC_FQDN_LEN];			/* 電力会社指定のNTPサーバのFQDN		 */
} EepcEvt01_t;
/*--------------------------------------------------------------------------*/
/*	変数宣言																*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
extern void EEPC_Init(void);
extern void EEPC_MsgProc(FrmMsgHead_t const *msg);
extern void EEPC_Fin(void);

/******************************************************************************
* 非同期ファンクション
******************************************************************************/
/* 通信開始要求IF	*/
extern EepcRtnCode_t EEPC_IfAsyncReqComStart(const ulong reqId, const uchar seqId,
	const schar *schDivStr, const schar *pwrStId);
extern EepcRtnCode_t EEPC_IfAsyncUpdateSrvInf(const ulong reqId, const uchar seqId);
extern EepcRtnCode_t EEPC_IfAsyncCancelSrvInf(const ulong reqId, const uchar seqId);

/******************************************************************************
* 同期ファンクション
******************************************************************************/
/* 電力会社情報取得IF */
extern EepcRtnCode_t EEPC_IfSyncGetSrvInf(EepcSrvMng_t *srvMng);
/* 電力会社情報保存IF */
extern EepcRtnCode_t EEPC_IfSyncSetSrvInf(const uchar useSrvIndex);
/* 設定変更通知イベント登録IF */
extern EepcRtnCode_t EEPC_IFSyncRegEepcChgEvt(const sint receiver, const ulong eventType, const uchar noticerflg);
/* 設定変更通知イベント登録解除IF */
extern EepcRtnCode_t EEPC_IfSyncClrEepcChgEvt(const sint receiver);
/* 通信先情報取得IF */
extern EepcRtnCode_t EEPC_IfSyncGetComInf(EepcComInf_t *comInf);

/* 同期時刻情報取得IF */
extern EepcRtnCode_t EEPC_IfLibGetSyncTimInf(uchar chkSum, EepcSyncTimInf_t *timInf);


#endif /*__EEPC_G_H__*/
