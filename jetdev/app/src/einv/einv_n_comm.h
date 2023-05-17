/****************************************************************************/
/*	Description		:INV通信ミドル 通信処理									*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC MH.Song											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/23												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:INV通信												*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef __EINV_N_COMM_H__
#define __EINV_N_COMM_H__


/*--------------------------------------------------------------------------*/
/*	マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define	EINV_CYCL_READ_ERR_TIME		((ushort)3*60*5)		/* 周期情報受信タイムアウトの表示間隔( 毎秒3回表示されるので、3*60秒*5分毎 ) */
#define	EINV_CYCL_INFO_ERR_TIME		((ushort)60*5)			/* 周期情報取得異常の表示間隔( 毎秒1回表示されるので、60秒*5分毎 ) */


/*--------------------------------------------------------------------------*/
/*	非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/
typedef struct _einv_cmd_table_t
{
	EinvCommand_t command;
	schar *reqCmdCode;
	ushort reqCmdSize;
	ushort resCmdSize;
	void (*MakeCmdData)(EinvCmdDataT_t *cmdData, slong reqSize, uchar *reqData);
	void (*ParseCmdData)(const uchar *resData);
} EinvCmdTable_t;

/* 送付メッセージ内容 */
typedef struct _einv_req_content_t
{
	schar *cmdCode;				/* コマンドコード */
	schar addr;					/* ユニットアドレス */
	uchar *msgData;				/* メッセージデータ */
	slong msgDataSize;			/* メッセージデータサイズ */
} EinvReqContent_t;

/* 応答メッセージ内容 */
typedef struct _einv_res_content_t
{
	uchar *responseMsg;			/* 応答メッセージデータ */
	slong responseMsgSize;		/* 応答メッセージデータサイズ */
} EinvResContent_t;


/*--------------------------------------------------------------------------*/
/*	定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	変数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	プロトタイプ宣言														*/
/*--------------------------------------------------------------------------*/
static slong einv_Strtoll(const uchar* val, uchar digits, uchar base);
static ulong einv_Strtoul(const uchar* val, uchar digits, uchar base);
static void einv_MakeIpAddress(const uchar *str, schar *ipAddr);
static void einv_ParseIpAddress(schar *ipAddr, uchar *str);
static slong einv_n_CmdComm(EinvReqContent_t* reqContent, EinvResContent_t* resContent);
static slong einv_n_MsgWrite(sint fd, const void *buf, slong size);
static slong einv_n_MsgRead(sint fd, schar *cmdCode, uchar *buf, slong size);
static bool_t einv_n_IsResCmdError( const schar *cmdCode, uchar* pBuf );
static void einv_ParseCmd01Data(const uchar *resData);
static void einv_ParseCmd18Data(const uchar *resData);
static void einv_MakeCmd19Data(EinvCmdDataT_t *cmdData, slong reqSize, uchar *reqData);
static void einv_ParseCmd19Data(const uchar *resData);
static void einv_ParseCmd60Data(const uchar *resData);
static void einv_ParseCmd62Data(const uchar *resData);
static void einv_MakeCmd63Data(EinvCmdDataT_t *cmdData, slong reqSize, uchar *reqData);
static void einv_ParseCmd63Data(const uchar *resData);
static void einv_ParseCmd64Data(const uchar *resData);
static void einv_MakeCmd65Data(EinvCmdDataT_t *cmdData, slong reqSize, uchar *reqData);
static void einv_ParseCmd65Data(const uchar *resData);
static void einv_ParseCmd68Data(const uchar *resData);
static void einv_MakeCmd69Data(EinvCmdDataT_t *cmdData, slong reqSize, uchar *reqData);
static void einv_ParseCmd69Data(const uchar *resData);
static void einv_ParseCmd6AData(const uchar *resData);
static void einv_MakeCmd6BData(EinvCmdDataT_t *cmdData, slong reqSize, uchar *reqData);
static void einv_ParseCmd6BData(const uchar *resData);
static void einv_MakeCmd6DData(EinvCmdDataT_t *cmdData, slong reqSize, uchar *reqData);
static void einv_ParseCmd6DData(const uchar *resData);
static void einv_ParseCmd74Data(const uchar *resData);
static void einv_MakeCmd75Data(EinvCmdDataT_t *cmdData, slong reqSize, uchar *reqData);
static void einv_ParseCmd75Data(const uchar *resData);
static void einv_MakeCmd81Data(EinvCmdDataT_t *cmdData, slong reqSize, uchar *reqData);
static void einv_ParseCmd81Data(const uchar *resData);
static void einv_ParseCmd90Data(const uchar *resData);

#endif /*__EINV_N_COMM_H__*/
