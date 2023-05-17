/****************************************************************************/
/*	Description		:INV通信ミドル 通信処理									*/
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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>
#include <poll.h>

#include "typedef.h"
#include "task_g.h"
#include "common_g.h"
#include "frm_g.h"
#include "dlog_g.h"

#include "einv_g.h"
#include "einv_msg_g.h"
#include "einv_ram_g.h"
#include "einv_utl_g.h"
#include "einv_n_g.h"
#include "einv_n_ram_g.h"
#include "einv_n_utl_g.h"
#include "einv_n_func_g.h"
#include "einv_n_comm_g.h"
#include "einv_n_comm.h"


/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
uchar s_commSequence = 0U;									/* 通信シーケンス */
uchar s_resMsg[EINV_RESMSG_LARGE_BUF_SIZE] = {0};			/* コマンド応答バッファー */
ulong s_ErrTimeOut = 0UL;
ulong s_ReadTimeOut = 0UL;

const EinvCmdTable_t s_cmdTable[EINV_CMD_MAX] =
{
	{ EINV_CMD01,	EINV_CODE_CMD_E1_01_REQ,	EINV_CMD01_REQ_SIZE,	EINV_CMD01_RES_SIZE,	NULL,				einv_ParseCmd01Data	},
	{ EINV_CMD11,	EINV_CODE_CMD_E1_11_REQ,	0,						1,						NULL,				NULL				},
	{ EINV_CMD18,	EINV_CODE_CMD_E1_18_REQ,	EINV_CMD18_REQ_SIZE,	EINV_CMD18_RES_SIZE,	NULL,				einv_ParseCmd18Data	},
	{ EINV_CMD19,	EINV_CODE_CMD_E1_19_REQ,	EINV_CMD19_REQ_SIZE,	EINV_CMD19_RES_SIZE,	einv_MakeCmd19Data,	einv_ParseCmd19Data	},
	{ EINV_CMD60,	EINV_CODE_CMD_E1_60_REQ,	EINV_CMD60_REQ_SIZE,	EINV_CMD60_RES_SIZE,	NULL,				einv_ParseCmd60Data	},
	{ EINV_CMD62,	EINV_CODE_CMD_E1_62_REQ,	EINV_CMD62_REQ_SIZE,	EINV_CMD62_RES_SIZE,	NULL,				einv_ParseCmd62Data	},
	{ EINV_CMD63,	EINV_CODE_CMD_E1_63_REQ,	EINV_CMD63_REQ_SIZE,	EINV_CMD63_RES_SIZE,	einv_MakeCmd63Data,	einv_ParseCmd63Data	},
	{ EINV_CMD64,	EINV_CODE_CMD_E1_64_REQ,	EINV_CMD64_REQ_SIZE,	EINV_CMD64_RES_SIZE,	NULL,				einv_ParseCmd64Data },
	{ EINV_CMD65,	EINV_CODE_CMD_E1_65_REQ,	EINV_CMD65_REQ_SIZE,	EINV_CMD65_RES_SIZE,	einv_MakeCmd65Data,	einv_ParseCmd65Data },
	{ EINV_CMD68,	EINV_CODE_CMD_E1_68_REQ,	EINV_CMD68_REQ_SIZE,	EINV_CMD68_RES_SIZE,	NULL,				einv_ParseCmd68Data	},
	{ EINV_CMD69,	EINV_CODE_CMD_E1_69_REQ,	EINV_CMD69_REQ_SIZE,	EINV_CMD69_RES_SIZE,	einv_MakeCmd69Data,	einv_ParseCmd69Data	},
	{ EINV_CMD6A,	EINV_CODE_CMD_E1_6A_REQ,	EINV_CMD6A_REQ_SIZE,	EINV_CMD6A_RES_SIZE,	NULL,				einv_ParseCmd6AData	},
	{ EINV_CMD6B,	EINV_CODE_CMD_E1_6B_REQ,	EINV_CMD6B_REQ_SIZE,	EINV_CMD6B_RES_SIZE,	einv_MakeCmd6BData,	einv_ParseCmd6BData	},
	{ EINV_CMD6D,	EINV_CODE_CMD_E1_6D_REQ,	EINV_CMD6D_REQ_SIZE,	EINV_CMD6D_RES_SIZE,	einv_MakeCmd6DData,	einv_ParseCmd6DData	},
	{ EINV_CMD74,	EINV_CODE_CMD_E1_74_REQ,	EINV_CMD74_REQ_SIZE,	EINV_CMD74_RES_SIZE,	NULL,				einv_ParseCmd74Data	},
	{ EINV_CMD75,	EINV_CODE_CMD_E1_75_REQ,	EINV_CMD75_REQ_SIZE,	EINV_CMD75_RES_SIZE,	einv_MakeCmd75Data,	einv_ParseCmd75Data	},
	{ EINV_CMD81,	EINV_CODE_CMD_E1_81_REQ,	EINV_CMD81_REQ_SIZE,	EINV_CMD81_RES_SIZE,	einv_MakeCmd81Data,	einv_ParseCmd81Data	},
	{ EINV_CMD90,	EINV_CODE_CMD_E1_90_REQ,	EINV_CMD90_REQ_SIZE,	EINV_CMD90_RES_SIZE,	NULL,				einv_ParseCmd90Data	},
	{ EINV_CMD98,	EINV_CODE_CMD_E1_98_REQ,	EINV_CMD98_REQ_SIZE,	EINV_CMD98_RES_SIZE,	NULL,				NULL				}
};

ulong debugTimes = 0U;
/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description	:CMD送付処理												*/
/*--------------------------------------------------------------------------*/
/*	param		:reqMsg		:			要求メッセージ						*/
/*--------------------------------------------------------------------------*/
/*	return		:ret	応答コード											*/
/*						EINV_OK					正常終了					*/
/*						EINV_ERR_SYSCALL		システムコールエラー		*/
/*						EINV_ERR_PARAM			パラメータエラー			*/
/*						EINV_ERR_FAIL			通信異常					*/
/*--------------------------------------------------------------------------*/
/*	detail		:CMD送付処理を行う。										*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/27 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
slong EINV_SendCMD(EinvCommand_t command, EinvCmdDataT_t *cmdData)
{
	slong ret = EINV_OK;
	slong i = 0;
	bool_t breakFlag = FALSE;
	bool_t breakFlag2 = FALSE;
	EinvCmdTable_t cmdTable;
	EinvReqContent_t reqContent;
	EinvResContent_t resContent;
	uchar reqMsgBuff[EINV_REQMSG_MIDDLE_BUF_SIZE + 1] = { 0 };
	sshort phaseSeq = EINV_PHASE_1;

	DEBUG_TRCIN();

	if(cmdData == NULL)
	{
		DLOG_Error("EINV_SendCMD EINV_ERR_PARAM cmdData == NULL\n");
		DEBUG_TRCOUT();
		return EINV_ERR_PARAM;
	}
	else
	{
		/* 処理なし */
	}

	phaseSeq = EINV_PHASE_1;
	do 
	{
		switch (phaseSeq)
		{
		case EINV_PHASE_1:
			/* 応答変数初期化 */
			memset(s_resMsg, 0, EINV_RESMSG_LARGE_BUF_SIZE);

			do
			{
				/* CMD対応処理を取得 */
				if (i < EINV_CMD_MAX)
				{
					if (s_cmdTable[i].command == command)
					{
						cmdTable = s_cmdTable[i];
						breakFlag2 = TRUE;
					}
					else
					{
						/* 処理無し */
					}
					i++;
				}
				else
				{
					breakFlag2 = TRUE;
					DLOG_Error("EINV_SendCMD CMD_ID out range command = %d\n", command);
					ret = EINV_ERR_PARAM;
				}
			} while (!breakFlag2);

			if (i >= EINV_CMD_MAX)
			{
				phaseSeq = EINV_PHASE_END;
			}
			else
			{
				phaseSeq = EINV_PHASE_2;
			}
			break;
		case EINV_PHASE_2:
			if (cmdTable.reqCmdSize > EINV_REQMSG_MIDDLE_BUF_SIZE)
			{
				DLOG_Error("EINV_SendCMD reqCmdSize > EINV_REQMSG_SMALL_BUF_SIZE reqCmdSize = %d EINV_REQMSG_SMALL_BUF_SIZE = %d\n", cmdTable.reqCmdSize, EINV_REQMSG_MIDDLE_BUF_SIZE);
				ret = EINV_ERR_PARAM;
				phaseSeq = EINV_PHASE_END;
			}
			else
			{
				phaseSeq = EINV_PHASE_3;
			}
			break;
		case EINV_PHASE_3:
			reqContent.cmdCode = cmdTable.reqCmdCode;
			reqContent.addr = EINV_CODE_ADDR_E1;

			resContent.responseMsg = s_resMsg;

			/* CMD対応作成処理があれば */
			if (cmdTable.MakeCmdData != NULL)
			{
				/* INV通信を行う(付随データあり) */
				reqContent.msgDataSize = cmdTable.reqCmdSize;
				reqContent.msgData = reqMsgBuff;

				/* CMD対応作成処理を行う */
				cmdTable.MakeCmdData(cmdData, reqContent.msgDataSize, reqContent.msgData);
			}
			else
			{
				/* INV通信を行う(付随データ無し) */
				reqContent.msgDataSize = 0;
				reqContent.msgData = NULL;
			}

			phaseSeq = EINV_PHASE_4;
			break;
		case EINV_PHASE_4:
			/* INV通信を行う */
			ret = einv_n_CmdComm(&reqContent, &resContent);
			if (ret == EINV_OK)
			{
				if (resContent.responseMsgSize != cmdTable.resCmdSize)
				{
					DLOG_Error("responseMsgSize err.(%ld, %ld)\n", resContent.responseMsgSize, cmdTable.resCmdSize);
					ret = EINV_ERR_FAIL;
					phaseSeq = EINV_PHASE_END;
				}
				else
				{
					phaseSeq = EINV_PHASE_5;
				}
			}
			else
			{
				phaseSeq = EINV_PHASE_END;
			}
			break;
		case EINV_PHASE_5:
			/* CMD対応解析処理があれば */
			if (cmdTable.ParseCmdData != NULL)
			{
				/* CMD対応解析処理を行う */
				cmdTable.ParseCmdData(resContent.responseMsg);
			}
			else
			{
				/* 処理無し */
			}
			phaseSeq = EINV_PHASE_END;
			break;
		case EINV_PHASE_END:
			breakFlag = TRUE;
			break;
		default:
			DLOG_Error("phaseSeq err. phaseSeq = %d\n", phaseSeq);
			ret = EINV_ERR_SYSCALL;
			breakFlag = TRUE;
			break;
		}
	} while (!breakFlag);

	DEBUG_TRCOUT();

	return ret;
}


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description	:基数を指定して文字列をslong型に変換します．				*/
/*--------------------------------------------------------------------------*/
/*	param		:val		:数字											*/
/*				:digits		:桁数											*/
/*				:base		:基数											*/
/*--------------------------------------------------------------------------*/
/*	return		:result		数値											*/
/*--------------------------------------------------------------------------*/
/*	detail		:基数を指定して文字列をslong型に変換処理を行う。			*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/27 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
static slong einv_Strtoll(const uchar* val, uchar digits, uchar base)
{
	slong ret = 0L;
	schar tmp[EINV_DOUBLE_BUF_SIZE];

	DEBUG_TRCIN();

	if( digits > EINV_DOUBLE_BUF_DATA_LEN ){
		digits = EINV_DOUBLE_BUF_DATA_LEN;
		DLOG_Warn("einv_Strtoll(digits=%d) is maximum size over.\n", digits);
	}
	memcpy(tmp, val, digits);
	tmp[digits] ='\0';

	ret = strtoll(tmp, NULL, base);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:基数を指定して文字列をulong型に変換します．				*/
/*--------------------------------------------------------------------------*/
/*	param		:val		:数字											*/
/*				:digits		:桁数											*/
/*				:base		:基数											*/
/*--------------------------------------------------------------------------*/
/*	return		:result		数値											*/
/*--------------------------------------------------------------------------*/
/*	detail		:基数を指定して文字列をulong型に変換処理を行う。			*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/27 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
static ulong einv_Strtoul(const uchar* val, uchar digits, uchar base)
{
	ulong ret = 0UL;
	schar tmp[EINV_DOUBLE_BUF_SIZE];

	DEBUG_TRCIN();

	if( digits > EINV_DOUBLE_BUF_DATA_LEN ){
		digits = EINV_DOUBLE_BUF_DATA_LEN;
		DLOG_Warn("einv_Strtoul(digits=%d) is maximum size over.\n", digits);
	}
	memcpy(tmp, val, digits);
	tmp[digits] ='\0';

	ret = strtoul(tmp, NULL, base);

	DEBUG_TRCOUT();

	return ret;
}

/*==========================================================================*/
/*	Description	:作成IPアドレス												*/
/*--------------------------------------------------------------------------*/
/*	param		:str	:文字列												*/
/*				:ipAddr	:IPアドレス											*/
/*--------------------------------------------------------------------------*/
/*	return		:なし														*/
/*--------------------------------------------------------------------------*/
/*	detail		:作成IPアドレス処理を行う。									*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/06 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
static void einv_MakeIpAddress(const uchar *str, schar *ipAddr)
{
	slong ipAddr1 = einv_Strtoll(str, 3, EINV_CMD_RES_BASE_DEC);
	slong ipAddr2 = einv_Strtoll((str + 3), 3, EINV_CMD_RES_BASE_DEC);
	slong ipAddr3 = einv_Strtoll((str + 6), 3, EINV_CMD_RES_BASE_DEC);
	slong ipAddr4 = einv_Strtoll((str + 9), 3, EINV_CMD_RES_BASE_DEC);

	DEBUG_TRCIN();

	snprintf(ipAddr, EINV_IPADDRESS_LEN, "%ld.%ld.%ld.%ld", ipAddr1, ipAddr2, ipAddr3, ipAddr4);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description	:解析IPアドレス												*/
/*--------------------------------------------------------------------------*/
/*	param		:ipAddr	:IPアドレス											*/
/*				:str	:文字列												*/
/*--------------------------------------------------------------------------*/
/*	return		:なし														*/
/*--------------------------------------------------------------------------*/
/*	detail		:解析IPアドレス処理を行う。									*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/04/06 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
static void einv_ParseIpAddress(schar *ipAddr, uchar *str)
{
	schar *findPoint1 = ipAddr;
	schar *findPoint2 = ipAddr;
	slong ipAddr1 = 0;
	slong ipAddr2 = 0;
	slong ipAddr3 = 0;
	slong ipAddr4 = 0;
	
	DEBUG_TRCIN();

	findPoint2 = strchr(findPoint1, '.');
	ipAddr1 = einv_Strtoll((uchar *)findPoint1, (uchar)(findPoint2 - findPoint1), EINV_CMD_RES_BASE_DEC);
	findPoint1 = findPoint2;

	findPoint2 = strchr(findPoint1, '.');
	ipAddr2 = einv_Strtoll((uchar *)findPoint1, (uchar)(findPoint2 - findPoint1), EINV_CMD_RES_BASE_DEC);
	findPoint1 = findPoint2;

	findPoint2 = strchr(findPoint1, '.');
	ipAddr3 = einv_Strtoll((uchar *)findPoint1, (uchar)(findPoint2 - findPoint1), EINV_CMD_RES_BASE_DEC);
	findPoint1 = findPoint2;

	ipAddr4 = einv_Strtoll((uchar *)findPoint1, (uchar)strlen(findPoint1), EINV_CMD_RES_BASE_DEC);

	snprintf((schar*)str, EINV_CMD19_IPADDRESS_LEN + 1U, "%03ld%03ld%03ld%03ld", ipAddr1, ipAddr2, ipAddr3, ipAddr4);

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description	:INV通信処理												*/
/*--------------------------------------------------------------------------*/
/*	param		:reqContent			:送付メッセージ内容						*/
/*				:resContent			:応答メッセージ内容						*/
/*--------------------------------------------------------------------------*/
/*	return		:commResult	処理結果										*/
/*				:		EINV_OK					正常終了					*/
/*				:		EINV_ERR_SYSCALL		システムコールエラー		*/
/*				:		EINV_ERR_PARAM			パラメータエラー			*/
/*				:		EINV_ERR_FAIL			通信異常					*/
/*--------------------------------------------------------------------------*/
/*	detail		:メッセージ種別とメッセージデータ部を受け取り、メッセージ	*/
/*				:パケットを作成し通信インターフェースに通信依頼を行う。		*/
/*				:その後、それにより取得した情報からメッセージデータ部を		*/
/*				:取得し上位関数に渡す。										*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/28 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
static slong einv_n_CmdComm(EinvReqContent_t* reqContent, EinvResContent_t* resContent)
{
	/* メッセージパケット */
	uchar msgPacket[EINV_CMD_REQ_MSG_FORMAT_SIZE + EINV_REQMSG_MIDDLE_BUF_SIZE];
	uchar responseData[EINV_CMD_REQ_MSG_FORMAT_SIZE + EINV_RESMSG_LARGE_BUF_SIZE];

	/* 状態判定用変数 */
	slong commStatus = 0L;

	/* サイズ */
	slong responseDataSize = 0L;

	/* 通信リトライ回数 */
	slong retryCnt = 0L;

	/* チェックサム */
	slong cSum = 0L;

	/* カウンタ */
	slong i = 0L;
	slong j = 0L;
	slong k = 0L;
	schar buf[EINV_LONG_BUF_SIZE];

	/* 応答電文情報 */
	schar *stxPoint = NULL;
	schar *etxPoint = NULL;
	schar *crPoint  = NULL;
	slong resCSum = 0L;

	/* 送信した要求応答番号 */
	slong commSequence = 0L;

	/* 通信結果 */
	slong commResult = EINV_OK;

	/* 抜けたい処理フラグ */
	bool_t breakFlag = FALSE;
	
	/* 応答コマンドコードが期待値と一致するか比較する結果 */
	bool_t isResCmdError = FALSE;

	sshort phaseSeq = EINV_PHASE_1;

	/* 開始ログ出力 */
	DEBUG_TRCIN();

	/* 引数NULLチェック */
	if(reqContent == NULL
		|| resContent == NULL
		|| reqContent->cmdCode == NULL
		|| resContent->responseMsg == NULL)
	{
		/* 終了ログ出力 */
		DLOG_Error("reqContent or resContent or cmdCode or responseMsg is NULL. return EINV_ERR_PARAM.\n");
		DEBUG_TRCOUT();
		/* パラメータエラー */
		return EINV_ERR_PARAM;
	}
	else
	{
		/* 処理無し */
	}

	/* メッセージがNULLに対し、データサイズが指定されていた場合 */
	if(reqContent->msgData == NULL
		&& reqContent->msgDataSize > 0)
	{
		/* 終了ログ出力 */
		DLOG_Error("msgData is NULL. return EINV_ERR_PARAM.\n");
		DEBUG_TRCOUT();
		/* パラメータエラー */
		return EINV_ERR_PARAM;
	}
	else
	{
		/* 処理無し */
	}

	/* バッファの初期化 */
	memset(buf, 0, sizeof(buf));

	phaseSeq = EINV_PHASE_1;
	do 
	{
		switch (phaseSeq)
		{
		case EINV_PHASE_1:		/* 通信電文作成 */
			/* 初期化 */
			memset(msgPacket, 0, sizeof(msgPacket));

			/* STX */
			msgPacket[i++] = EINV_CODE_STX;

			/* ヘッダ部 */
			memcpy(&msgPacket[i], EINV_CODE_HEADER_RMC, strlen(EINV_CODE_HEADER_RMC));
			i = i + strlen(EINV_CODE_HEADER_RMC);

			/* アドレス */
			msgPacket[i++] = reqContent->addr;

			/* シーケンス */
			commSequence = s_commSequence;
			s_commSequence++;
			if (s_commSequence > EINV_CODE_SEQ_MAX)
			{
				s_commSequence = EINV_CODE_SEQ_MIN;
			}
			else
			{
				/* 処理無し */
			}
			snprintf(buf, sizeof(buf), "%d", commSequence & 0x0F);
			memcpy(&msgPacket[i++], buf, strlen(buf));

			/* コマンド */
			memcpy(&msgPacket[i], reqContent->cmdCode, strlen(reqContent->cmdCode));
			i = i + strlen(reqContent->cmdCode);

			/* データ部 */
			for (j = 0; j < reqContent->msgDataSize; j++)
			{
				msgPacket[i++] = *(reqContent->msgData + j);
			}

			/* ETX */
			msgPacket[i++] = EINV_CODE_ETX;

			/* チェックサム */
			for (k = 0; k < i; k++)
			{
				cSum = cSum + msgPacket[k];
			}
			snprintf(buf, sizeof(buf), "%02X", cSum & 0xFF);
			memcpy(&msgPacket[i], buf, strlen(buf));
			i = i + 2;

			/* CR */
			msgPacket[i++] = EINV_CODE_CR;

			phaseSeq = EINV_PHASE_2;
			break;
		case EINV_PHASE_2:		/* 送信リトライループ */

			/* エラー状態のリセット */
			commResult = EINV_OK;

			/* 送受信バッファクリア */
			tcflush(g_einvFd, TCIOFLUSH);

			phaseSeq = EINV_PHASE_3;
			break;
		case EINV_PHASE_3:		/* メッセージ送信 */
			/* 電文送信 */
			commStatus = einv_n_MsgWrite(g_einvFd, msgPacket, i);
			if (commStatus < 0)		/* システムコールエラー or 送信タイムアウト */
			{
				commResult = EINV_ERR_SYSCALL;
				phaseSeq = EINV_PHASE_ERR;
			}
			else
			{
				phaseSeq = EINV_PHASE_4;
			}
			break;
		case EINV_PHASE_4:		/* 転送完了を待つ */
			commResult = EINV_n_CheckComplete(g_einvFd);
			if (commResult != EINV_OK)
			{
				retryCnt++;					/* リトライ回数を加算 */
				phaseSeq = EINV_PHASE_RTY;	/* リトライ */
			}
			else
			{
				phaseSeq = EINV_PHASE_5;
			}
			break;
		case EINV_PHASE_5:		/* メッセージ受信 */
			/* 受信バッファ初期化 */
			memset(responseData, 0, sizeof(responseData));

			/* メッセージ読み込み */
			responseDataSize = einv_n_MsgRead(g_einvFd, reqContent->cmdCode, responseData, sizeof(responseData));
			if (responseDataSize == EINV_ERR_FAIL) 			/* タイムアウトが発生した場合 */
			{
				retryCnt++;					/* リトライ回数を加算 */
				phaseSeq = EINV_PHASE_RTY;	/* リトライ */
			}
			else if (responseDataSize == EINV_ERR_SYSCALL)	/* システムコールエラー */
			{
				commResult = EINV_ERR_SYSCALL;
				phaseSeq = EINV_PHASE_ERR;
			}
			else
			{
				phaseSeq = EINV_PHASE_6;
			}
			break;
		case EINV_PHASE_6:		/* 受信電文チェック */
			/* 特定コードのポインタを検索 */
			stxPoint = strchr((schar*)responseData, EINV_CODE_STX);
			etxPoint = strchr((schar*)responseData, EINV_CODE_ETX);
			crPoint = strchr((schar*)responseData, EINV_CODE_CR);

			/* 設定されていない特定コードがあった場合 */
			if ((stxPoint == NULL)
				|| (etxPoint == NULL)
				|| (crPoint == NULL))
			{
				DLOG_Error("inv response format error.\n");
				retryCnt++;					/* リトライ回数を加算 */
				phaseSeq = EINV_PHASE_RTY;	/* リトライ */
			}
			else
			{
				phaseSeq = EINV_PHASE_6_1;
			}
			break;
		case EINV_PHASE_6_1:
			/* <STX>と<ETX>の間に、最低限必要なbyte数のチェック */
			if ((etxPoint - stxPoint - 1) < EINV_CMD_BODY_LEN_MIN)		/* アドレス上、<STX><ETX>間に必要なサイズの文字列が無い為、エラーとする */
			{
				DLOG_Error("inv response format error. CMD Msg Len.\n");
				retryCnt++;					/* リトライ回数を加算 */
				phaseSeq = EINV_PHASE_RTY;	/* リトライ */
			}
			else
			{
				phaseSeq = EINV_PHASE_6_2;
			}
			break;
		case EINV_PHASE_6_2:
			/* シーケンス番号チェック */
			snprintf(buf, sizeof(buf), "%d", commSequence & 0x0F);
			if (*buf != *(stxPoint + 4))
			{
				DLOG_Error("inv response sequence error.\n");
				retryCnt++;					/* リトライ回数を加算 */
				phaseSeq = EINV_PHASE_RTY;	/* リトライ */
			}
			else
			{
				phaseSeq = EINV_PHASE_6_3;
			}
			break;
		case EINV_PHASE_6_3:
			/* 応答CMDチェック */
			isResCmdError = einv_n_IsResCmdError(reqContent->cmdCode, responseData);	/* 応答コマンド不一致 */
			if (isResCmdError)
			{
				DLOG_Error("inv response command error.\n");
				retryCnt++;					/* リトライ回数を加算 */
				phaseSeq = EINV_PHASE_RTY;	/* リトライ */
			}
			else
			{
				phaseSeq = EINV_PHASE_6_4;
			}
			break;
		case EINV_PHASE_6_4:
			/* チェックサム計算 */
			resCSum = 0;
			for (k = (stxPoint - (schar*)responseData); k < ((etxPoint - (schar*)responseData) + 1); k++)
			{
				resCSum = resCSum + responseData[k];
			}

			/* チェックサムチェック */
			snprintf(buf, sizeof(buf), "%02X", resCSum & 0xFF);
			if ((*buf != *(etxPoint + 1))
				|| (*(buf + 1) != *(etxPoint + 2)))
			{
				DLOG_Error("inv response checksum error.\n");
				retryCnt++;					/* リトライ回数を加算 */
				phaseSeq = EINV_PHASE_RTY;	/* リトライ */
			}
			else
			{
				phaseSeq = EINV_PHASE_7;
			}
			break;
		case EINV_PHASE_7:		/* メッセージデータ部取出し */
			/* メッセージデータ部取出し */
			memset(resContent->responseMsg, 0, (etxPoint - stxPoint));
			memcpy(resContent->responseMsg, (stxPoint + EINV_CMD_BODY_LEN_MIN + 1), (ulong)(etxPoint - (stxPoint + EINV_CMD_BODY_LEN_MIN + 1)));
			resContent->responseMsgSize = strlen((schar*)resContent->responseMsg);

			/* ステータス変更排他ロック */
			CMN_MutexLock(&g_einvSttsMutex);

			/* 通信未確認の場合 */
			if (g_einvStts == EINV_STTS_COMM_UNCONF)
			{

				/* 起動時通信確立 */
				g_einvStts = EINV_STTS_COMM_INIT_CONF;
			}
			/* 起動時通信エラーの場合 */
			else if (g_einvStts == EINV_STTS_COMM_INIT_ERR)
			{

				/* 起動時通信確立 */
				g_einvStts = EINV_STTS_COMM_INIT_CONF;
			}
			/* 通信エラーの場合 */
			else if (g_einvStts == EINV_STTS_COMM_ERR)
			{

				/* 通信確立 */
				g_einvStts = EINV_STTS_COMM_CONF;
			}
			else
			{
				/* 処理無し */
			}

			/* 取得正常なので初期化 */
			s_ErrTimeOut = 0;

			// ステータス変更排他アンロック
			CMN_MutexUnlock(&g_einvSttsMutex);

			phaseSeq = EINV_PHASE_END;
			break;
		case EINV_PHASE_RTY:
			if (retryCnt > EINV_COMM_RETRY_CNT)
			{
				phaseSeq = EINV_PHASE_ERR;
			}
			else
			{
				phaseSeq = EINV_PHASE_2;
			}
			break;
		case EINV_PHASE_ERR:
			/* 初回は表示 */
			if (s_ErrTimeOut == 0) {
				/* エラーログ出力 */
				DLOG_Error("inv communication interruption error.\n");
			}

			s_ErrTimeOut++;
			if (s_ErrTimeOut == EINV_CYCL_INFO_ERR_TIME) {
				s_ErrTimeOut = 0;
			}


			/* システムコールエラーが発生していない場合 */
			if (commResult != EINV_ERR_SYSCALL) {

				/* 通信異常 */
				commResult = EINV_ERR_FAIL;
			}

			/* ステータス変更排他ロック */
			CMN_MutexLock(&g_einvSttsMutex);

			/* 通信未確認 or 起動時通信確立の場合 */
			if ((g_einvStts == EINV_STTS_COMM_UNCONF)
				|| (g_einvStts == EINV_STTS_COMM_INIT_CONF))
			{

				/* 起動時通信エラー */
				g_einvStts = EINV_STTS_COMM_INIT_ERR;
			}
			// 通信確立の場合
			else if (g_einvStts == EINV_STTS_COMM_CONF)
			{

				/* 通信エラー */
				g_einvStts = EINV_STTS_COMM_ERR;
			}
			else
			{
				/* 処理無し */
			}

			/* ステータス排他アンロック */
			CMN_MutexUnlock(&g_einvSttsMutex);

			phaseSeq = EINV_PHASE_END;
			break;
		case EINV_PHASE_END:
			breakFlag = TRUE;
			break;
		default:
			DLOG_Error("phaseSeq err. phaseSeq = %d\n", phaseSeq);
			commResult = EINV_ERR_SYSCALL;
			breakFlag = TRUE;
			break;
		}
	} while (!breakFlag);

	// 終了ログ出力
	DEBUG_TRCOUT();

	/* 結果を返す */
	return commResult;
}

/*==========================================================================*/
/*	Description	:電文の送信処理												*/
/*--------------------------------------------------------------------------*/
/*	param		:fd		:ファイルディスクリプタ								*/
/*				:buf	:送信電文											*/
/*				:size	:送信電文サイズ										*/
/*--------------------------------------------------------------------------*/
/*	return		:total	実際に送信した電文のサイズ							*/
/*				:		0以上の数値				送信した電文のサイズ		*/
/*				:		EINV_ERR_SYSCALL(-1)	システムコールエラー		*/
/*				:		EINV_ERR_FAIL(-5)		処理失敗(タイムアウト)		*/
/*--------------------------------------------------------------------------*/
/*	detail		:電文の送信処理を行う。										*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/29 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
static slong einv_n_MsgWrite(sint fd, const void *buf, slong size)
{
	slong	cc = 0L;
	slong	total = 0L;
	slong	ret = EINV_OK;
	uchar	retry = 0U;
	void	*top = NULL;

	DEBUG_TRCIN();

	/* 引数チェック */
	if(buf == NULL
	|| size <= 0)
	{
		DLOG_Error("einv_n_MsgWrite：buf is NULL. return EINV_ERR_FAIL.\n");
		DEBUG_TRCOUT();
		return EINV_ERR_FAIL;
	}
	else
	{
		/* 処理無し */
	}

	/* すべて書き込むまで繰り返し */
	top = (void*)buf;
	while (size > 0) {
		cc = write(fd, buf, size);

		if (cc < 0)	/* エラーが発生した場合 */
		{		
			DLOG_Error("write() error ret=%ld, return EINV_ERR_SYSCALL.\n", cc);
			ret = EINV_ERR_SYSCALL;
			size = 0;
		} 
		else if ( cc == 0 )	/* 未送信の場合 */
		{	
			if ( retry > EINV_COMM_RETRY_WRITE ) {
				DLOG_Error("write() error retry=%d. return EINV_ERR_FAIL.\n", retry);
				ret = EINV_ERR_FAIL;
				size = 0;
			}
			else
			{
				EINV_Usleep(EINV_COMM_RETRY_INTERVAL);	/* 1msec */
				retry++;
			}
		} 
		else	/* 正常送信の場合 */
		{			
			/* 書き込んだ分だけ進む */
			total = total + cc;
			buf = ((const schar *)buf) + cc;
			size = size - cc;
		}
	}

	EINV_Dump(top, total);
	if (ret != EINV_OK)
	{
		total = ret;
	} 
	else
	{
	}

	DEBUG_TRCOUT();

	return total;
}

/*==========================================================================*/
/*	Description	:電文の受信処理												*/
/*--------------------------------------------------------------------------*/
/*	param		:fd			:ファイルディスクリプタ							*/
/*				:buf		:受信電文バッファ								*/
/*				:size		:受信電文バッファサイズ							*/
/*				:cmdCode	:コマンドコード									*/
/*--------------------------------------------------------------------------*/
/*	return		:total	実際に受信した電文のサイズ							*/
/*				:		0以上の数値				受信した電文のサイズ		*/
/*				:		EINV_ERR_SYSCALL(-1)	システムコールエラー		*/
/*				:		EINV_ERR_FAIL(-5)		処理失敗(タイムアウト)		*/
/*--------------------------------------------------------------------------*/
/*	detail		:P電文はエンドコード(CR)が読み込まれるまで繰り返す。		*/
/*				:100ms以内に受信が終わらなかった場合、タイムアウトし		*/
/*				:タイムアウトエラーを返す。									*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/29 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
static slong einv_n_MsgRead(sint fd, schar *cmdCode, uchar *buf, slong size)
{
	slong pollResult = 0;
	slong cc = 0;
	slong total = 0;
	slong ret = EINV_OK;
	bool_t breakFlag = FALSE;
	schar* scPoint = NULL;
	schar* ecPoint = NULL;
	struct pollfd fds[1];
	uchar readBuf[EINV_RESMSG_MIDDLE_BUF_SIZE + 1];
	slong cmdFlag = 0;			/* 1:CMD62, 2:CMD11, 0:その他 */
	slong readTimeOutTime = 0;
	sshort phaseSeq = EINV_PHASE_1;
	uchar *top = NULL;

	struct timespec	startClock;
	struct timespec	checkClock;
	slong elapsedTime;

	DEBUG_TRCIN();

	/* 引数チェック */
	if ((buf == NULL)
		|| (size <= 0))
	{
		DLOG_Error("buf is NULL. return EINV_ERR_FAIL.\n");
		DEBUG_TRCOUT();
		return EINV_ERR_FAIL;
	}
	else
	{
		/* 処理無し */
	}

	/* pollfd構造体設定 */
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	fds[0].revents = 0;

	/* 読み込み開始時間 */
	top = buf;
	memset(&startClock, 0x0, sizeof(startClock));
	elapsedTime = EINV_PassTime(NULL, &startClock);
	if (elapsedTime != 0) {
		DLOG_Error("read start time get error. return EINV_ERR_SYSCALL.\n");
		ret = EINV_ERR_SYSCALL;
	}
	else
	{
		phaseSeq = EINV_PHASE_1;
		while (!breakFlag) {
			switch (phaseSeq)
			{
			case EINV_PHASE_1:		/* タイムアウト処理別判断 */
				cmdFlag = strcmp(cmdCode, EINV_CODE_CMD_E1_62_REQ);
				if (cmdFlag == 0)
				{
					readTimeOutTime = EINV_COMM_TIMEOUT_G62_COMM;
					cmdFlag = 1;
				}
				else
				{
					cmdFlag = strcmp(cmdCode, EINV_CODE_CMD_E1_11_REQ);
					if (cmdFlag == 0)
					{
						cmdFlag = 2;
					}
					else
					{
						readTimeOutTime = EINV_COMM_TIMEOUT_READ;
						cmdFlag = 0;
					}
				}
				phaseSeq = EINV_PHASE_2;
				break;
			case EINV_PHASE_2:		/* すべて読み込むまで繰り返し */
				switch (cmdFlag)
				{
				case 0:
					pollResult = poll(fds, 1, EINV_COMM_TIMEOUT_COMM);
					phaseSeq = EINV_PHASE_2_1;
					break;
				case 1:
					pollResult = poll(fds, 1, EINV_COMM_TIMEOUT_G62_COMM);
					phaseSeq = EINV_PHASE_2_1;
					break;
				default:
					phaseSeq = EINV_PHASE_3;
					break;
				}
				break;
			case EINV_PHASE_2_1:		/* 受信確認 */
				if (pollResult < 0)	/* システムコールエラーが発生した場合 */
				{
					DLOG_Error("poll() error ret=%d. return EINV_ERR_SYSCALL.\n", pollResult);
					ret = EINV_ERR_SYSCALL;
					phaseSeq = EINV_PHASE_END;
				}
				else if (pollResult == 0)	/* タイムアウトが発生した場合 */
				{
					/* 発生初回は表示 */
					if (s_ReadTimeOut == 0) {
						DLOG_Error("poll() timeout. return EINV_ERR_FAIL.\n");
					}

					s_ReadTimeOut++;
					/* 通信エラー時毎秒3回表示されるので、5分毎(3*60秒*5分)に表示されるようにする */
					if (s_ReadTimeOut == EINV_CYCL_READ_ERR_TIME) {
						s_ReadTimeOut = 0;
					}
					else
					{
						/* 処理無し */
					}

					ret = EINV_ERR_FAIL;
					phaseSeq = EINV_PHASE_END;
				}
				else
				{
					phaseSeq = EINV_PHASE_3;
				}
				break;
			case EINV_PHASE_3:		/* 受信有り*/
				s_ReadTimeOut = 0;

				/* 情報読み込み */
				memset(readBuf, 0, sizeof(readBuf));
				cc = read(fd, readBuf, sizeof(readBuf) - 1);	/* システムコールエラーが発生した場合 */
				if (cc < 0)
				{
					DLOG_Error("read() error ret=%d. return EINV_ERR_SYSCALL.\n", pollResult);
					ret = EINV_ERR_SYSCALL;
					phaseSeq = EINV_PHASE_END;
				}
				else
				{
					if (scPoint == NULL)
					{
						phaseSeq = EINV_PHASE_3_1;
					}
					else
					{
						phaseSeq = EINV_PHASE_3_2;
					}
				}
				break;
			case EINV_PHASE_3_1:		/* STX捜索 */
				scPoint = strchr((schar*)readBuf, EINV_CODE_STX);
				if (scPoint != NULL)
				{
					cc = cc - ((uchar*)scPoint - readBuf);
					if (cc >= 0)
					{
						phaseSeq = EINV_PHASE_3_2;
					}
					else
					{
						DLOG_Error("STX position error. scPoint = %p readBuf = %p cc = &ld return EINV_ERR_SYSCALL.\n", scPoint, readBuf, cc);
						ret = EINV_ERR_SYSCALL;
						phaseSeq = EINV_PHASE_END;
					}
				}
				else
				{
					phaseSeq = EINV_PHASE_2;
				}
				break;
			case EINV_PHASE_3_2:		/* 読み込んだ分だけ進む */
				size = size - cc;
				if (size <= 0)
				{
					DLOG_Error("size <= 0\n");
					ret = EINV_ERR_SYSCALL;
					phaseSeq = EINV_PHASE_END;
				}
				else
				{
					if (scPoint != (schar*)top)
					{
						memcpy(top, scPoint, cc);
						scPoint = (schar*)top;
					}
					else
					{
						memcpy(buf, readBuf, cc);
					}
					total = total + cc;
					buf = buf + cc;

					/* エンドコードを検索 */
					ecPoint = strchr(scPoint, EINV_CODE_CR);
					if (ecPoint != NULL)
					{
						/* エンドコードがあった場合、読み込み処理を終了する */
						total = ecPoint - scPoint + 1;
						switch (cmdFlag)
						{
						case 0:
						case 1:
							phaseSeq = EINV_PHASE_4;
							break;
						default:
							phaseSeq = EINV_PHASE_END;
							break;
						}
					}
					else
					{
						phaseSeq = EINV_PHASE_2;
					}
				}
				break;
			case EINV_PHASE_4:		/* 経過時間チェック */
				memset(&checkClock, 0x0, sizeof(checkClock));
				elapsedTime = EINV_PassTime(&startClock, &checkClock);
				if (elapsedTime < 0)
				{
					DLOG_Error("read elaps time get error. return EINV_ERR_SYSCALL.\n");
					ret = EINV_ERR_SYSCALL;
					phaseSeq = EINV_PHASE_END;
				}
				else
				{
					elapsedTime = elapsedTime * 10;	/* [10msec->msec] */

					/* 最大読み込み時間を超過していた場合 */
					if (elapsedTime > readTimeOutTime)
					{
						DLOG_Error("read elaps time timeout. return EINV_ERR_FAIL.\n");
						ret = EINV_ERR_FAIL;	/* タイムアウトとする */
						phaseSeq = EINV_PHASE_END;
					}
					else
					{
						phaseSeq = EINV_PHASE_END;
					}
				}
				break;
			case EINV_PHASE_END:
				breakFlag = TRUE;
				break;
			default:
				DLOG_Error("phaseSeq err. phaseSeq = %d\n", phaseSeq);
				ret = EINV_ERR_SYSCALL;
				breakFlag = TRUE;
				break;
			}
		}
	}

	EINV_Dump(scPoint, total);
	if (ret != EINV_OK)
	{
		total = ret;
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT();

	return total;
}

/*==========================================================================*/
/*	Description		:解析CMD01データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:resData	:応答データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:解析CMD01データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_ParseCmd01Data(const uchar *resData)
{
	DEBUG_TRCIN( );
	
	memset(&g_einvCmd01_r, 0, sizeof(g_einvCmd01_r));

	memcpy(&g_einvCmd01_r.modelFunctionName, &resData[EINV_CMD01_RES_MODELFUNCTIONNAME], EINV_CMD01_MODELFUNCTIONNAME_LEN);		/* モデル機能名称 */

	DLOG_Debug("g_einvCmd01_r.modelFunctionName = %s\n", g_einvCmd01_r.modelFunctionName);

	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:解析CMD60データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:resData	:応答データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:解析CMD60データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_ParseCmd60Data(const uchar *resData)
{
	DEBUG_TRCIN( );
		
	memset(&g_einvCmd60_r, 0, sizeof(g_einvCmd60_r));

	memcpy(&g_einvCmd60_r.sysMadeNum, &resData[EINV_CMD60_RES_SYSMADENUM		],EINV_CMD60_SYSMADENUM_LEN);																		/* システム製造番号 */
	g_einvCmd60_r.btRateEltc			= (ushort)einv_Strtoul(&resData[EINV_CMD60_RES_BTRATEELTC		], EINV_CMD60_BTRATEELTC_LEN		, EINV_CMD_RES_BASE_DEC);				/* 蓄電池定格電力量 */
	g_einvCmd60_r.pcsRateOut			= (ushort)einv_Strtoul(&resData[EINV_CMD60_RES_PCSRATEOUT		], EINV_CMD60_PCSRATEOUT_LEN		, EINV_CMD_RES_BASE_DEC);				/* PCS定格出力 */
	g_einvCmd60_r.pcsRateVol			= (ushort)einv_Strtoul(&resData[EINV_CMD60_RES_PCSRATEVOL		], EINV_CMD60_PCSRATEVOL_LEN		, EINV_CMD_RES_BASE_DEC);				/* PCS定格電圧 */
	g_einvCmd60_r.pcsRateVolAlone		= (ushort)einv_Strtoul(&resData[EINV_CMD60_RES_PCSRATEVOLALONE	], EINV_CMD60_PCSRATEVOLALONE_LEN		, EINV_CMD_RES_BASE_DEC);			/* PCS定格電圧_自立時 */
	memcpy(g_einvCmd60_r.setDate.year, &resData[EINV_CMD60_RES_SETDATEYEAR		], EINV_CMD60_SETDATEYEAR_LEN);																		/* 設置日(年) */
	memcpy(g_einvCmd60_r.setDate.month, &resData[EINV_CMD60_RES_SETDATEMONTH	], EINV_CMD60_SETDATEMONTH_LEN);																	/* 設置日(月) */
	memcpy(g_einvCmd60_r.setDate.day, &resData[EINV_CMD60_RES_SETDATEDAY		], EINV_CMD60_SETDATEDAY_LEN);																		/* 設置日(日) */
	memcpy(&g_einvCmd60_r.invMcuFw, &resData[EINV_CMD60_RES_INVMCUFW			],EINV_CMD60_INVMCUFW_LEN);																			/* INVマイコン_FW */
	memcpy(&g_einvCmd60_r.pvMcuFw, &resData[EINV_CMD60_RES_PVMCUFW				],EINV_CMD60_PVMCUFW_LEN);																			/* PVマイコン_FW */
	memcpy(&g_einvCmd60_r.bdcMcuFw, &resData[EINV_CMD60_RES_BDCMCUFW			],EINV_CMD60_BDCMCUFW_LEN);																			/* BDCマイコン_FW */
	memcpy(&g_einvCmd60_r.jbFw, &resData[EINV_CMD60_RES_JBFW					],EINV_CMD60_JBFW_LEN);																				/* J.B._FW */
	memcpy(&g_einvCmd60_r.invMcuBootLoader, &resData[EINV_CMD60_RES_INVMCUBOOTLOADER		],EINV_CMD60_INVMCUBOOTLOADER_LEN);														/* INVマイコン_ブートローダー */
	memcpy(&g_einvCmd60_r.pvMcuBootLoader, &resData[EINV_CMD60_RES_PVMCUBOOTLOADER			],EINV_CMD60_PVMCUBOOTLOADER_LEN);														/* PVマイコン_ブートローダー */
	memcpy(&g_einvCmd60_r.bdcMcuBootLoader, &resData[EINV_CMD60_RES_BDCMCUBOOTLOADER		],EINV_CMD60_BDCMCUBOOTLOADER_LEN);														/* BDCマイコン_ブートローダー */
	g_einvCmd60_r.sysStruct				= resData[EINV_CMD60_RES_SYSSTRUCT			];																								/* システム構成 */
	memcpy(&g_einvCmd60_r.btMadeNum, &resData[EINV_CMD60_RES_BTMADENUM			],EINV_CMD60_BTMADENUM_LEN);																		/* 蓄電池ユニット製造番号 */
	g_einvCmd60_r.btRatedVol			= (ushort)einv_Strtoul(&resData[EINV_CMD60_RES_BTRATEDVOL			], EINV_CMD60_BTRATEDVOL_LEN		, EINV_CMD_RES_BASE_DEC);			/* 蓄電池定格電圧 */
	g_einvCmd60_r.btMaxChargePwr		= (ulong)einv_Strtoul(&resData[EINV_CMD60_RES_BTMAXCHARGEPWR		], EINV_CMD60_BTMAXCHARGEPWR_LEN		, EINV_CMD_RES_BASE_DEC);		/* 蓄電池最大充電電力値 */
	g_einvCmd60_r.btMaxDischarPwr		= (ulong)einv_Strtoul(&resData[EINV_CMD60_RES_BTMAXDISCHARPWR		], EINV_CMD60_BTMAXDISCHARPWR_LEN		, EINV_CMD_RES_BASE_DEC);		/* 蓄電池最大放電電力値 */
	g_einvCmd60_r.btMaxChargePwrIndep	= (ulong)einv_Strtoul(&resData[EINV_CMD60_RES_BTMAXCHARGEPWRINDEP	], EINV_CMD60_BTMAXCHARGEPWRINDEP_LEN		, EINV_CMD_RES_BASE_DEC);	/* 蓄電池最大充電電力値_自立時 */
	g_einvCmd60_r.btMaxDischarPwrIndep	= (ulong)einv_Strtoul(&resData[EINV_CMD60_RES_BTMAXDISCHARPWRINDEP	], EINV_CMD60_BTMAXDISCHARPWRINDEP_LEN		, EINV_CMD_RES_BASE_DEC);	/* 蓄電池最大放電電力値_自立時 */
	g_einvCmd60_r.sysFrequency			= (uchar)einv_Strtoul(&resData[EINV_CMD60_RES_SYSFREQUENCY			], EINV_CMD60_SYSFREQUENCY_LEN		, EINV_CMD_RES_BASE_DEC);			/* 系統周波数(50Hz or 60Hz) */


	DLOG_Debug("g_einvCmd60_r.sysMadeNum = %s\n",				g_einvCmd60_r.sysMadeNum);
	DLOG_Debug("g_einvCmd60_r.btRateEltc = %d\n",				g_einvCmd60_r.btRateEltc);
	DLOG_Debug("g_einvCmd60_r.pcsRateOut = %d\n",				g_einvCmd60_r.pcsRateOut);
	DLOG_Debug("g_einvCmd60_r.pcsRateVol = %d\n",				g_einvCmd60_r.pcsRateVol);
	DLOG_Debug("g_einvCmd60_r.pcsRateVolAlone = %d\n",			g_einvCmd60_r.pcsRateVolAlone);
	DLOG_Debug("g_einvCmd60_r.setDate.year = %s\n",				g_einvCmd60_r.setDate.year);
	DLOG_Debug("g_einvCmd60_r.setDate.month = %s\n",			g_einvCmd60_r.setDate.month);
	DLOG_Debug("g_einvCmd60_r.setDate.day = %s\n",				g_einvCmd60_r.setDate.day);
	DLOG_Debug("g_einvCmd60_r.invMcuFw = %s\n",					g_einvCmd60_r.invMcuFw);
	DLOG_Debug("g_einvCmd60_r.pvMcuFw = %s\n",					g_einvCmd60_r.pvMcuFw);
	DLOG_Debug("g_einvCmd60_r.bdcMcuFw = %s\n",					g_einvCmd60_r.bdcMcuFw);
	DLOG_Debug("g_einvCmd60_r.jbFw = %s\n",						g_einvCmd60_r.jbFw);
	DLOG_Debug("g_einvCmd60_r.invMcuBootLoader = %s\n",			g_einvCmd60_r.invMcuBootLoader);
	DLOG_Debug("g_einvCmd60_r.pvMcuBootLoader = %s\n",			g_einvCmd60_r.pvMcuBootLoader);
	DLOG_Debug("g_einvCmd60_r.bdcMcuBootLoader = %s\n",			g_einvCmd60_r.bdcMcuBootLoader);
	DLOG_Debug("g_einvCmd60_r.sysStruct = %d\n",				g_einvCmd60_r.sysStruct);
	DLOG_Debug("g_einvCmd60_r.btMadeNum = %s\n",				g_einvCmd60_r.btMadeNum);
	DLOG_Debug("g_einvCmd60_r.btRatedVol = %d\n",				g_einvCmd60_r.btRatedVol);
	DLOG_Debug("g_einvCmd60_r.btMaxChargePwr = %ld\n",			g_einvCmd60_r.btMaxChargePwr);
	DLOG_Debug("g_einvCmd60_r.btMaxDischarPwr = %ld\n",			g_einvCmd60_r.btMaxDischarPwr);
	DLOG_Debug("g_einvCmd60_r.btMaxChargePwrIndep = %ld\n",		g_einvCmd60_r.btMaxChargePwrIndep);
	DLOG_Debug("g_einvCmd60_r.btMaxDischarPwrIndep = %ld\n",	g_einvCmd60_r.btMaxDischarPwrIndep);
	DLOG_Debug("g_einvCmd60_r.sysFrequency = %d\n",				g_einvCmd60_r.sysFrequency);

	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:解析CMD62データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:resData	:応答データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:解析CMD62データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/13 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_ParseCmd62Data(const uchar *resData)
{
	DEBUG_TRCIN( );
	
	memset(&g_einvCmd62_r, 0, sizeof(g_einvCmd62_r));

	memcpy(&g_einvCmd62_r.actionLog, &resData[EINV_CMD62_RES_ACTIONLOG], EINV_CMD62_ACTIONLOG_LEN);		/* 動作ログ */

	DLOG_Debug("g_einvCmd62_r.actionLog = %s\n", g_einvCmd62_r.actionLog);

	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:作成CMD63データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:cmdData	:cmdデータ									*/
/*					:reqSize	:要求サイズ									*/
/*					:reqData	:要求データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:作成CMD63データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_MakeCmd63Data(EinvCmdDataT_t *cmdData, slong reqSize, uchar *reqData)
{
	EinvCmd63T_t *cmd63 = &cmdData->cmd63_t;
	schar buf[EINV_LONG_BUF_SIZE] = { 0 };
	ushort cnt = 0U;

	DEBUG_TRCIN( );

	memset(reqData, EINV_CODE_NO_CHANGE, reqSize);
	memset(buf   , 0                  , sizeof(buf));

	/* PCSエラー消去命令 */
	reqData[cnt] = cmd63->pcsErrorRemoveCommand;

	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:解析CMD63データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:resData	:応答データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:解析CMD63データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_ParseCmd63Data(const uchar *resData)
{
	DEBUG_TRCIN( );

	memset(&g_einvCmd63_r, 0, sizeof(g_einvCmd63_r));

	g_einvCmd63_r.response = (schar)einv_Strtoll(&resData[EINV_CMD63_RES_RESPONSE], EINV_CMD63_RESPONSE_LEN, EINV_CMD_RES_BASE_DEC);		/* ACK/NAK */

	DLOG_Debug("g_einvCmd63_r.response = %d", g_einvCmd63_r.response);

	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:解析CMD64データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:resData	:応答データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:解析CMD64データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_ParseCmd64Data(const uchar *resData)
{
	DEBUG_TRCIN( );
	
	memset(&g_einvCmd64_r, 0, sizeof(g_einvCmd64_r));

	g_einvCmd64_r.opeMode				= resData[EINV_CMD64_RES_OPEMODE	];																							/* 運転モード */
	g_einvCmd64_r.emergencyMode			= resData[EINV_CMD64_RES_EMGMODE	];																							/* 非常時運転モード */
	g_einvCmd64_r.startOrStop			= resData[EINV_CMD64_RES_STARTORSTOP];																							/* 運転／停止 */
	g_einvCmd64_r.btLowerLimit			= (ushort)einv_Strtoul(&resData[EINV_CMD64_RES_BTLOWERLIMIT		], EINV_CMD64_BTLOWERLIMIT_LEN		, EINV_CMD_RES_BASE_DEC);	/* 蓄電池放電下限値設定 */
	g_einvCmd64_r.btUpperLimit			= (ushort)einv_Strtoul(&resData[EINV_CMD64_RES_BTUPPERLIMIT		], EINV_CMD64_BTUPPERLIMIT_LEN		, EINV_CMD_RES_BASE_DEC);	/* 蓄電池充電上限値設定 */
	g_einvCmd64_r.testrunOutAtPwrCut	= resData[EINV_CMD64_RES_TESTRUNOUT	];																							/* 試運転時停電出力 */
	g_einvCmd64_r.testrunFlag			= resData[EINV_CMD64_RES_TESTRUNFLAG];																							/* 試運転指示フラグ */
	g_einvCmd64_r.chargePwrSet			= (ushort)einv_Strtoul(&resData[EINV_CMD64_RES_CHARGEPWRSET		], EINV_CMD64_CHARGEPWRSET_LEN		, EINV_CMD_RES_BASE_DEC);	/* 充電電力設定値(蓄電池) */
	g_einvCmd64_r.dischargePwrSet		= (ushort)einv_Strtoul(&resData[EINV_CMD64_RES_DISCHARGEPWRSET	], EINV_CMD64_DISCHARGEPWRSET_LEN	, EINV_CMD_RES_BASE_DEC);	/* 放電電力設定値(蓄電池) */
		
	DLOG_Debug("g_einvCmd64_r.opeMode = %d\n",					g_einvCmd64_r.opeMode);
	DLOG_Debug("g_einvCmd64_r.emergencyMode = %d\n",			g_einvCmd64_r.emergencyMode);
	DLOG_Debug("g_einvCmd64_r.startOrStop = %d\n",				g_einvCmd64_r.startOrStop);
	DLOG_Debug("g_einvCmd64_r.btLowerLimit = %d\n",				g_einvCmd64_r.btLowerLimit);
	DLOG_Debug("g_einvCmd64_r.btUpperLimit = %d\n",				g_einvCmd64_r.btUpperLimit);
	DLOG_Debug("g_einvCmd64_r.testrunOutAtPwrCut = %d\n",		g_einvCmd64_r.testrunOutAtPwrCut);
	DLOG_Debug("g_einvCmd64_r.testrunFlag = %d\n",				g_einvCmd64_r.testrunFlag);
	DLOG_Debug("g_einvCmd64_r.chargePwrSet = %d\n",				g_einvCmd64_r.chargePwrSet);
	DLOG_Debug("g_einvCmd64_r.dischargePwrSet = %d\n",			g_einvCmd64_r.dischargePwrSet);

	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:作成CMD65データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:cmdData	:cmdデータ									*/
/*					:reqSize	:要求サイズ									*/
/*					:reqData	:要求データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:作成CMD65データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_MakeCmd65Data(EinvCmdDataT_t *cmdData, slong reqSize, uchar *reqData)
{
	EinvCmd65T_t *cmd65 = &cmdData->cmd65_t;
	schar buf[EINV_LONG_BUF_SIZE] = { 0 };
	ulong cnt = 0U;

	DEBUG_TRCIN( );

	memset(reqData, EINV_CODE_NO_CHANGE, reqSize);
	memset(buf   , 0                  , sizeof(buf));

	/* 運転モード */
	if((cmd65->changeFlag & EINV_SET_CMD65_OPE_MODE) == EINV_SET_CMD65_OPE_MODE)
	{
		reqData[cnt] = cmd65->opeMode;
	} 
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD65_OPEMODE_LEN;

	/* 非常時運転モード */
	if((cmd65->changeFlag & EINV_SET_CMD65_EMG_MODE) == EINV_SET_CMD65_EMG_MODE)
	{
		reqData[cnt] = cmd65->emergencyMode;
	} 
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD65_EMGMODE_LEN;

	/* 運転／停止 */
	if((cmd65->changeFlag & EINV_SET_CMD65_START_OR_STOP) == EINV_SET_CMD65_START_OR_STOP)
	{
		reqData[cnt] = cmd65->startOrStop;
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD65_STARTORSTOP_LEN;

	/* 蓄電池放電下限値設定 */
	if((cmd65->changeFlag & EINV_SET_CMD65_BT_LOWER_LIMIT) == EINV_SET_CMD65_BT_LOWER_LIMIT)
	{
		snprintf(buf, sizeof(buf), "%03u",(cmd65->btLowerLimit) / 10);
		memcpy(&reqData[cnt], buf, EINV_CMD65_BTLOWERLIMIT_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD65_BTLOWERLIMIT_LEN;

	/* 蓄電池充電上限値設定 */
	if((cmd65->changeFlag & EINV_SET_CMD65_BT_UPPER_LIMIT) == EINV_SET_CMD65_BT_UPPER_LIMIT)
	{
		snprintf(buf, sizeof(buf), "%03u",(cmd65->btUpperLimit) / 10);
		memcpy(&reqData[cnt], buf, EINV_CMD65_BTUPPERLIMIT_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD65_BTUPPERLIMIT_LEN;

	/* 試運転時停電出力 */
	if((cmd65->changeFlag & EINV_SET_CMD65_TEST_RUN_OUT) == EINV_SET_CMD65_TEST_RUN_OUT)
	{
		reqData[cnt] = cmd65->testrunOutAtPwrCut;
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD65_TESTRUNOUT_LEN;

	/* 試運転指示フラグ */
	if((cmd65->changeFlag & EINV_SET_CMD65_TEST_RUN_FLAG) == EINV_SET_CMD65_TEST_RUN_FLAG)
	{
		reqData[cnt] = cmd65->testrunFlag;
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD65_TESTRUNFLAG_LEN;

	/* 充電電力設定値(蓄電池) */
	if((cmd65->changeFlag & EINV_SET_CMD65_CHR_PWR_SET) == EINV_SET_CMD65_CHR_PWR_SET)
	{
		snprintf(buf, sizeof(buf), "%04u",cmd65->chargePwrSet);
		memcpy(&reqData[cnt], buf, EINV_CMD65_CHARGEPWRSET_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD65_CHARGEPWRSET_LEN;

	/* 放電電力設定値(蓄電池) */
	if((cmd65->changeFlag & EINV_SET_CMD65_DISCHR_PWR_SET) == EINV_SET_CMD65_DISCHR_PWR_SET)
	{
		snprintf(buf, sizeof(buf), "%04u",cmd65->dischargePwrSet);
		memcpy(&reqData[cnt], buf, EINV_CMD65_DISCHARGEPWRSET_LEN);
	}
	else
	{
		/* 処理無し */
	}
	 
	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:解析CMD65データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:resData	:応答データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:解析CMD65データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/27 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_ParseCmd65Data(const uchar *resData)
{
	DEBUG_TRCIN( );

	memset(&g_einvCmd65_r, 0, sizeof(g_einvCmd65_r));

	g_einvCmd65_r.response = (schar)einv_Strtoll(&resData[EINV_CMD65_RES_RESPONSE], EINV_CMD65_RESPONSE_LEN, EINV_CMD_RES_BASE_DEC);		/* ACK/NAK */

	DLOG_Debug("g_einvCmd65_r.response = %d", g_einvCmd65_r.response);

	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:解析CMD68データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:resData	:応答データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:解析CMD68データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/29 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_ParseCmd68Data(const uchar *resData)
{
	DEBUG_TRCIN( );

	memset(&g_einvCmd68_r, 0, sizeof(g_einvCmd68_r));

	g_einvCmd68_r.sysVoltDetectLevel		= (ushort)einv_Strtoul(&resData[EINV_CMD68_RES_SYSVOLTDETECTLEVEL		], EINV_CMD68_SYSVOLTDETECTLEVEL_LEN		, EINV_CMD_RES_BASE_DEC);		/* 系統過電圧検出レベル */
	g_einvCmd68_r.sysVoltDetectTime			= (ushort)einv_Strtoul(&resData[EINV_CMD68_RES_SYSVOLTDETECTTIME		], EINV_CMD68_SYSVOLTDETECTTIME_LEN			, EINV_CMD_RES_BASE_DEC);		/* 系統過電圧検出時間 */
	g_einvCmd68_r.sysUnderVoltDetectLevel	= (ushort)einv_Strtoul(&resData[EINV_CMD68_RES_SYSUNDERVOLTDETECTLEVEL	], EINV_CMD68_SYSUNDERVOLTDETECTLEVEL_LEN	, EINV_CMD_RES_BASE_DEC);		/* 系統不足電圧検出レベル */
	g_einvCmd68_r.sysUnderVoltDetectTime	= (ushort)einv_Strtoul(&resData[EINV_CMD68_RES_SYSUNDERVOLTDETECTTIME	], EINV_CMD68_SYSUNDERVOLTDETECTTIME_LEN	, EINV_CMD_RES_BASE_DEC);		/* 系統不足電圧検出時間 */
	g_einvCmd68_r.freRiseDetectLevel		= (uchar)einv_Strtoul(&resData[EINV_CMD68_RES_FRERISEDETECTLEVEL		], EINV_CMD68_FRERISEDETECTLEVEL_LEN		, EINV_CMD_RES_BASE_DEC);		/* 周波数上昇検出レベル */
	g_einvCmd68_r.freRiseDetectTime			= (ushort)einv_Strtoul(&resData[EINV_CMD68_RES_FRERISEDETECTTIME		], EINV_CMD68_FRERISEDETECTTIME_LEN			, EINV_CMD_RES_BASE_DEC);		/* 周波数上昇検出時間 */
	g_einvCmd68_r.freDropDetectLevel		= (uchar)einv_Strtoul(&resData[EINV_CMD68_RES_FREDROPDETECTLEVEL		], EINV_CMD68_FREDROPDETECTLEVEL_LEN		, EINV_CMD_RES_BASE_DEC);		/* 周波数低下検出レベル */
	g_einvCmd68_r.freDropDetectTime			= (ushort)einv_Strtoul(&resData[EINV_CMD68_RES_FREDROPDETECTTIME		], EINV_CMD68_FREDROPDETECTTIME_LEN			, EINV_CMD_RES_BASE_DEC);		/* 周波数低下検出時間 */
	g_einvCmd68_r.passIndOperDetect			= (ushort)einv_Strtoul(&resData[EINV_CMD68_RES_PASSINDOPERDETECT		], EINV_CMD68_PASSINDOPERDETECT_LEN			, EINV_CMD_RES_BASE_DEC);		/* 受動的単独運転検出レベル */
	g_einvCmd68_r.resctTime					= (ushort)einv_Strtoul(&resData[EINV_CMD68_RES_RESCTTIME				], EINV_CMD68_RESCTTIME_LEN					, EINV_CMD_RES_BASE_DEC);		/* 復帰時間 */
	g_einvCmd68_r.changeTimeIncorDes		= (ushort)einv_Strtoul(&resData[EINV_CMD68_RES_CHANGETIMEINCORDES		], EINV_CMD68_CHANGETIMEINCORDES_LEN		, EINV_CMD_RES_BASE_DEC);		/* 出力増減変化時間 */
	g_einvCmd68_r.volRsInhbValue			= (ushort)einv_Strtoul(&resData[EINV_CMD68_RES_VOLRSINHBVALUE			], EINV_CMD68_VOLRSINHBVALUE_LEN			, EINV_CMD_RES_BASE_DEC);		/* 電圧上昇抑制値 */
	g_einvCmd68_r.volRsInhbattLimit			= (ushort)einv_Strtoul(&resData[EINV_CMD68_RES_VOLRSINHBATTLIMIT		], EINV_CMD68_VOLRSINHBATTLIMIT_LEN			, EINV_CMD_RES_BASE_DEC);		/* 電圧上昇抑制動作待機時限 */
	g_einvCmd68_r.cotPowerControl			= (ushort)einv_Strtoul(&resData[EINV_CMD68_RES_COTPOWERCONTROL			], EINV_CMD68_COTPOWERCONTROL_LEN			, EINV_CMD_RES_BASE_DEC);		/* 力率一定制御 */
	g_einvCmd68_r.freFeedbackGain			= (ushort)einv_Strtoul(&resData[EINV_CMD68_RES_FREFEEDBACKGAIN			], EINV_CMD68_FREFEEDBACKGAIN_LEN			, EINV_CMD_RES_BASE_DEC);		/* 周波数フィードバックゲイン */
	g_einvCmd68_r.freLevAllow				= (ushort)einv_Strtoul(&resData[EINV_CMD68_RES_FRELEVALLOW				], EINV_CMD68_FRELEVALLOW_LEN				, EINV_CMD_RES_BASE_DEC);		/* 並列時許容周波数レベル */

	DLOG_Debug("g_einvCmd68_r.sysVoltDetectLevel = %d\n",			g_einvCmd68_r.sysVoltDetectLevel);
	DLOG_Debug("g_einvCmd68_r.sysVoltDetectTime = %d\n",			g_einvCmd68_r.sysVoltDetectTime);
	DLOG_Debug("g_einvCmd68_r.sysUnderVoltDetectLevel = %d\n",		g_einvCmd68_r.sysUnderVoltDetectLevel);
	DLOG_Debug("g_einvCmd68_r.sysUnderVoltDetectTime = %d\n",		g_einvCmd68_r.sysUnderVoltDetectTime);
	DLOG_Debug("g_einvCmd68_r.freRiseDetectLevel = %d\n",			g_einvCmd68_r.freRiseDetectLevel);
	DLOG_Debug("g_einvCmd68_r.freRiseDetectTime = %d\n",			g_einvCmd68_r.freRiseDetectTime);
	DLOG_Debug("g_einvCmd68_r.freDropDetectLevel = %d\n",			g_einvCmd68_r.freDropDetectLevel);
	DLOG_Debug("g_einvCmd68_r.freDropDetectTime = %d\n",			g_einvCmd68_r.freDropDetectTime);
	DLOG_Debug("g_einvCmd68_r.passIndOperDetect = %d\n",			g_einvCmd68_r.passIndOperDetect);
	DLOG_Debug("g_einvCmd68_r.resctTime = %d\n",					g_einvCmd68_r.resctTime);
	DLOG_Debug("g_einvCmd68_r.changeTimeIncorDes = %d\n",			g_einvCmd68_r.changeTimeIncorDes);
	DLOG_Debug("g_einvCmd68_r.volRsInhbValue = %d\n",				g_einvCmd68_r.volRsInhbValue);
	DLOG_Debug("g_einvCmd68_r.volRsInhbattLimit = %d\n",			g_einvCmd68_r.volRsInhbattLimit);
	DLOG_Debug("g_einvCmd68_r.cotPowerControl = %d\n",				g_einvCmd68_r.cotPowerControl);
	DLOG_Debug("g_einvCmd68_r.freFeedbackGain = %d\n",				g_einvCmd68_r.freFeedbackGain);
	DLOG_Debug("g_einvCmd68_r.freLevAllow = %d\n",					g_einvCmd68_r.freLevAllow);
	
	DEBUG_TRCOUT( );	
}

/*==========================================================================*/
/*	Description		:作成CMD69データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:cmdData	:cmdデータ									*/
/*					:reqSize	:要求サイズ									*/
/*					:reqData	:要求データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:作成CMD69データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/29 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_MakeCmd69Data(EinvCmdDataT_t *cmdData, slong reqSize, uchar *reqData)
{
	EinvCmd69T_t *cmd69 = &cmdData->cmd69_t;
	schar buf[EINV_LONG_BUF_SIZE] = { 0 };
	ulong cnt = 0UL;

	DEBUG_TRCIN( );

	memset(reqData, EINV_CODE_NO_CHANGE, reqSize);
	memset(buf   , 0                  , sizeof(buf));

	/* 系統過電圧検出レベル */
	if((cmd69->changeFlag & EINV_SET_CMD69_SYS_VOLTDETECT_LEVEL) == EINV_SET_CMD69_SYS_VOLTDETECT_LEVEL)
	{
		snprintf(buf, sizeof(buf), "% 4u",cmd69->sysVoltDetectLevel);
		memcpy(&reqData[cnt], buf, EINV_CMD69_SYSVOLTDETECTLEVEL_LEN);
	} 
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD69_SYSVOLTDETECTLEVEL_LEN;

	/* 系統過電圧検出時間 */
	if((cmd69->changeFlag & EINV_SET_CMD69_SYS_VOLTDETECT_TIME) == EINV_SET_CMD69_SYS_VOLTDETECT_TIME)
	{
		snprintf(buf, sizeof(buf), "% 4u",cmd69->sysVoltDetectTime);
		memcpy(&reqData[cnt], buf, EINV_CMD69_SYSVOLTDETECTTIME_LEN);
	} 
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD69_SYSVOLTDETECTTIME_LEN;

	/* 系統不足電圧検出レベル */
	if((cmd69->changeFlag & EINV_SET_CMD69_SYS_UNDERVOLT_DETECT_LEVEL) == EINV_SET_CMD69_SYS_UNDERVOLT_DETECT_LEVEL)
	{
		snprintf(buf, sizeof(buf), "% 4u",cmd69->sysUnderVoltDetectLevel);
		memcpy(&reqData[cnt], buf, EINV_CMD69_SYSUNDERVOLTDETECTLEVEL_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD69_SYSUNDERVOLTDETECTLEVEL_LEN;

	/* 系統不足電圧検出時間 */
	if((cmd69->changeFlag & EINV_SET_CMD69_SYS_UNDERVOLT_DETECT_TIME) == EINV_SET_CMD69_SYS_UNDERVOLT_DETECT_TIME)
	{
		snprintf(buf, sizeof(buf), "% 4u",cmd69->sysUnderVoltDetectTime);
		memcpy(&reqData[cnt], buf, EINV_CMD69_SYSUNDERVOLTDETECTTIME_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD69_SYSUNDERVOLTDETECTTIME_LEN;

	/* 周波数上昇検出レベル */
	if((cmd69->changeFlag & EINV_SET_CMD69_FRE_RISEDETECT_LEVEL) == EINV_SET_CMD69_FRE_RISEDETECT_LEVEL)
	{
		snprintf(buf, sizeof(buf), "% 4u",cmd69->freRiseDetectLevel);
		memcpy(&reqData[cnt], buf, EINV_CMD69_FRERISEDETECTLEVEL_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD69_FRERISEDETECTLEVEL_LEN;

	/* 周波数上昇検出時間 */
	if((cmd69->changeFlag & EINV_SET_CMD69_FRE_RISEDETECT_TIME) == EINV_SET_CMD69_FRE_RISEDETECT_TIME)
	{
		snprintf(buf, sizeof(buf), "% 4u",cmd69->freRiseDetectTime);
		memcpy(&reqData[cnt], buf, EINV_CMD69_FRERISEDETECTTIME_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD69_FRERISEDETECTTIME_LEN;

	/* 周波数低下検出レベル */
	if((cmd69->changeFlag & EINV_SET_CMD69_FRE_DROPDETECT_LEVEL) == EINV_SET_CMD69_FRE_DROPDETECT_LEVEL)
	{
		snprintf(buf, sizeof(buf), "% 4u",cmd69->freDropDetectLevel);
		memcpy(&reqData[cnt], buf, EINV_CMD69_FREDROPDETECTLEVEL_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD69_FREDROPDETECTLEVEL_LEN;

	/* 周波数低下検出時間 */
	if((cmd69->changeFlag & EINV_SET_CMD69_FRE_DROPDETECT_TIME) == EINV_SET_CMD69_FRE_DROPDETECT_TIME)
	{
		snprintf(buf, sizeof(buf), "% 4u",cmd69->freDropDetectTime);
		memcpy(&reqData[cnt], buf, EINV_CMD69_FREDROPDETECTTIME_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD69_FREDROPDETECTTIME_LEN;

	/* 受動的単独運転検出レベル */
	if((cmd69->changeFlag & EINV_SET_CMD69_PASS_INDOPER_DETECT) == EINV_SET_CMD69_PASS_INDOPER_DETECT)
	{
		snprintf(buf, sizeof(buf), "% 4u",cmd69->passIndOperDetect);
		memcpy(&reqData[cnt], buf, EINV_CMD69_PASSINDOPERDETECT_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD69_PASSINDOPERDETECT_LEN;

	/* 復帰時間 */
	if((cmd69->changeFlag & EINV_SET_CMD69_RESCT_TIME) == EINV_SET_CMD69_RESCT_TIME)
	{
		snprintf(buf, sizeof(buf), "% 4u",cmd69->resctTime);
		memcpy(&reqData[cnt], buf, EINV_CMD69_RESCTTIME_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD69_RESCTTIME_LEN;

	/* 出力増減変化時間 */
	if((cmd69->changeFlag & EINV_SET_CMD69_CHANGETIME_INC_OR_DES) == EINV_SET_CMD69_CHANGETIME_INC_OR_DES)
	{
		snprintf(buf, sizeof(buf), "% 4u",cmd69->changeTimeIncorDes);
		memcpy(&reqData[cnt], buf, EINV_CMD69_CHANGETIMEINCORDES_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD69_CHANGETIMEINCORDES_LEN;

	/* 電圧上昇抑制値 */
	if((cmd69->changeFlag & EINV_SET_CMD69_VOLRS_INHB_VALUE) == EINV_SET_CMD69_VOLRS_INHB_VALUE)
	{
		snprintf(buf, sizeof(buf), "% 4u",cmd69->volRsInhbValue);
		memcpy(&reqData[cnt], buf, EINV_CMD69_VOLRSINHBVALUE_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD69_VOLRSINHBVALUE_LEN;

	/* 電圧上昇抑制動作待機時限 */
	if((cmd69->changeFlag & EINV_SET_CMD69_VOLRS_INHBAT_TLIMIT) == EINV_SET_CMD69_VOLRS_INHBAT_TLIMIT)
	{
		snprintf(buf, sizeof(buf), "% 4u",cmd69->volRsInhbattLimit);
		memcpy(&reqData[cnt], buf, EINV_CMD69_VOLRSINHBATTLIMIT_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD69_VOLRSINHBATTLIMIT_LEN;

	/* 力率一定制御 */
	if((cmd69->changeFlag & EINV_SET_CMD69_COTPOWER_CONTROL) == EINV_SET_CMD69_COTPOWER_CONTROL)
	{
		snprintf(buf, sizeof(buf), "% 4u",cmd69->cotPowerControl);
		memcpy(&reqData[cnt], buf, EINV_CMD69_COTPOWERCONTROL_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD69_COTPOWERCONTROL_LEN;

	/* 周波数フィードバックゲイン */
	if((cmd69->changeFlag & EINV_SET_CMD69_FRE_FEEDBACK_GAIN) == EINV_SET_CMD69_FRE_FEEDBACK_GAIN)
	{
		snprintf(buf, sizeof(buf), "% 4u",cmd69->freFeedbackGain);
		memcpy(&reqData[cnt], buf, EINV_CMD69_FREFEEDBACKGAIN_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD69_FREFEEDBACKGAIN_LEN;

	/* 並列時許容周波数レベル */
	if((cmd69->changeFlag & EINV_SET_CMD69_FRE_LEV_ALLOW) == EINV_SET_CMD69_FRE_LEV_ALLOW)
	{
		snprintf(buf, sizeof(buf), "% 4u",cmd69->freLevAllow);
		memcpy(&reqData[cnt], buf, EINV_CMD69_FRELEVALLOW_LEN);
	}
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:解析CMD69データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:resData	:応答データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:解析CMD69データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/29 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_ParseCmd69Data(const uchar *resData)
{
	DEBUG_TRCIN( );

	memset(&g_einvCmd69_r, 0, sizeof(g_einvCmd69_r));

	g_einvCmd69_r.response = (schar)einv_Strtoll(&resData[EINV_CMD69_RES_RESPONSE], EINV_CMD69_RESPONSE_LEN, EINV_CMD_RES_BASE_DEC);		/* ACK/NAK */

	DLOG_Debug("g_einvCmd69_r.response = %d", g_einvCmd69_r.response);
	
	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:解析CMD6Aデータ										*/
/*--------------------------------------------------------------------------*/
/*	param			:resData	:応答データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:解析CMD6Aデータ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/30 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_ParseCmd6AData(const uchar *resData)
{
	DEBUG_TRCIN( );

	memset(&g_einvCmd6A_r, 0, sizeof(g_einvCmd6A_r));

	g_einvCmd6A_r.contaractPwrSet			= (ushort)einv_Strtoul(&resData[EINV_CMD6A_RES_CONTARACTPWRSET		], EINV_CMD6A_CONTARACTPWRSET_LEN	, EINV_CMD_RES_BASE_DEC);		/* 契約電力設定値 */
	g_einvCmd6A_r.pwrcutOutSet				= resData[EINV_CMD6A_RES_PWRCUTOUTSET				];																					/* 停電時出力設定 */
	g_einvCmd6A_r.pwrcutOutStart			= resData[EINV_CMD6A_RES_PWRCUTOUTSTART				];																					/* 停電時出力開始 */
	g_einvCmd6A_r.sellValidOrInvalid		= resData[EINV_CMD6A_RES_SELLVALIDORINVALID			];																					/* 売電有効無効設定 */
	g_einvCmd6A_r.maintenanceIndicate		= resData[EINV_CMD6A_RES_MAINTENANCEINDICATE		];																					/* メンテ指示 */


	DLOG_Debug("g_einvCmd6A_r.contaractPwrSet = %d\n",			g_einvCmd6A_r.contaractPwrSet);
	DLOG_Debug("g_einvCmd6A_r.pwrcutOutSet = %d\n",				g_einvCmd6A_r.pwrcutOutSet);
	DLOG_Debug("g_einvCmd6A_r.pwrcutOutStart = %d\n",			g_einvCmd6A_r.pwrcutOutStart);
	DLOG_Debug("g_einvCmd6A_r.sellValidOrInvalid = %d\n",		g_einvCmd6A_r.sellValidOrInvalid);
	DLOG_Debug("g_einvCmd6A_r.maintenanceIndicate = %d\n",		g_einvCmd6A_r.maintenanceIndicate);

	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:作成CMD6Bデータ										*/
/*--------------------------------------------------------------------------*/
/*	param			:cmdData	:cmdデータ									*/
/*					:reqSize	:要求サイズ									*/
/*					:reqData	:要求データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:作成CMD6Bデータ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/30 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_MakeCmd6BData(EinvCmdDataT_t *cmdData, slong reqSize, uchar *reqData)
{
	EinvCmd6BT_t *cmd6B = &cmdData->cmd6B_t;
	schar buf[EINV_LONG_BUF_SIZE] = { 0 };
	ulong cnt = 0UL;

	DEBUG_TRCIN( );

	memset(reqData, EINV_CODE_NO_CHANGE, reqSize);
	memset(buf   , 0                  , sizeof(buf));

	/* 契約電力設定値 */
	if((cmd6B->changeFlag & EINV_SET_CMD6B_CONTARACT_PWR_SET) == EINV_SET_CMD6B_CONTARACT_PWR_SET)
	{
		snprintf(buf, sizeof(buf), "% 4u",cmd6B->contractPwrSet);
		memcpy(&reqData[cnt], buf, EINV_CMD6B_CONTARACTPWRSET_LEN);
	} 
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD6B_CONTARACTPWRSET_LEN;

	/* 停電時出力設定 */
	if((cmd6B->changeFlag & EINV_SET_CMD6B_PWRCUT_OUT_SET) == EINV_SET_CMD6B_PWRCUT_OUT_SET)
	{
		reqData[cnt] = cmd6B->pwrcutOutSet;
	} 
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD6B_PWRCUTOUTSET_LEN;

	/* 停電時出力開始 */
	if((cmd6B->changeFlag & EINV_SET_CMD6B_PWRCUT_OUT_START) == EINV_SET_CMD6B_PWRCUT_OUT_START)
	{
		reqData[cnt] = cmd6B->pwrcutOutStart;
	} 
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD6B_PWRCUTOUTSTART_LEN;

	/* 売電有効無効設定 */
	if((cmd6B->changeFlag & EINV_SET_CMD6B_SELL_VALID_OR_INVALID) == EINV_SET_CMD6B_SELL_VALID_OR_INVALID)
	{
		reqData[cnt] = cmd6B->sellValidOrInvalid;
	} 
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD6B_SELLVALIDORINVALID_LEN;

	/* メンテ指示 */
	if((cmd6B->changeFlag & EINV_SET_CMD6B_MAINTENANCE_INDICATE) == EINV_SET_CMD6B_MAINTENANCE_INDICATE)
	{
		reqData[cnt] = cmd6B->maintenanceIndicate;
	} 
	else
	{
		/* 処理無し */
	}

	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:解析CMD6Bデータ										*/
/*--------------------------------------------------------------------------*/
/*	param			:resData	:応答データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:解析CMD6Bデータ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/30 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_ParseCmd6BData(const uchar *resData)
{
	DEBUG_TRCIN( );

	memset(&g_einvCmd6B_r, 0, sizeof(g_einvCmd6B_r));

	g_einvCmd6B_r.response = (schar)einv_Strtoll(&resData[EINV_CMD6B_RES_RESPONSE], EINV_CMD6B_RESPONSE_LEN, EINV_CMD_RES_BASE_DEC);		/* ACK/NAK */

	DLOG_Debug("g_einvCmd6B_r.response = %d", g_einvCmd6B_r.response);

	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:作成CMD6Dデータ										*/
/*--------------------------------------------------------------------------*/
/*	param			:cmdData	:cmdデータ									*/
/*					:reqSize	:要求サイズ									*/
/*					:reqData	:要求データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:作成CMD6Dデータ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_MakeCmd6DData(EinvCmdDataT_t *cmdData, slong reqSize, uchar *reqData)
{
	EinvCmd6DT_t *cmd6D = &cmdData->cmd6D_t;
	schar buf[EINV_LONG_BUF_SIZE] = { 0 };
	ulong cnt = 0UL;

	DEBUG_TRCIN( );

	memset(reqData, EINV_CODE_NO_CHANGE, reqSize);
	memset(buf   , 0                  , sizeof(buf));

	/* 設置日(年) */
	memcpy(&reqData[cnt], cmd6D->setDate.year, EINV_CMD6D_SETDATEYEAR_LEN);
	cnt = cnt + EINV_CMD6D_SETDATEYEAR_LEN;

	/* 設置日(月) */
	memcpy(&reqData[cnt], cmd6D->setDate.month, EINV_CMD6D_SETDATEMONTH_LEN);
	cnt = cnt + EINV_CMD6D_SETDATEMONTH_LEN;

	/* 設置日(日) */
	memcpy(&reqData[cnt], cmd6D->setDate.day, EINV_CMD6D_SETDATEDAY_LEN);
	cnt = cnt + EINV_CMD6D_SETDATEDAY_LEN;

	/* システム製造番号 */
	memcpy(&reqData[cnt], cmd6D->sysMadeNum, EINV_CMD6D_SYSMADENUM_LEN);
	cnt = cnt + EINV_CMD6D_SYSMADENUM_LEN;

	/* システム構成 */
	reqData[cnt] = cmd6D->sysStruct;
	cnt = cnt + EINV_CMD6D_SYSSTRUCT_LEN;

	/* 逆潮CT接続位置設定 */
	reqData[cnt] = cmd6D->revCtConctPositionSet;

	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:解析CMD6Dデータ										*/
/*--------------------------------------------------------------------------*/
/*	param			:resData	:応答データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:解析CMD6Dデータ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_ParseCmd6DData(const uchar *resData)
{
	DEBUG_TRCIN( );

	memset(&g_einvCmd6D_r, 0, sizeof(g_einvCmd6D_r));

	g_einvCmd6D_r.response = (schar)einv_Strtoll(&resData[EINV_CMD6D_RES_RESPONSE], EINV_CMD6D_RESPONSE_LEN, EINV_CMD_RES_BASE_DEC);		/* ACK/NAK */

	DLOG_Debug("g_einvCmd6D_r.response = %d", g_einvCmd6D_r.response);

	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:解析CMD18データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:resData	:応答データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:解析CMD18データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/07 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_ParseCmd18Data(const uchar *resData)
{
	DEBUG_TRCIN( );

	memset(&g_einvCmd18_r, 0, sizeof(g_einvCmd18_r));

	g_einvCmd18_r.exterSunPwrOrnot				= resData[EINV_CMD18_RES_EXTERSUNPWRORNOT			];																						/* 外付け太陽光発電有無 */
	g_einvCmd18_r.sunBeyondPwrGenratDevice		= resData[EINV_CMD18_RES_SUNBEYONDPWRGENRATDEVICE	];																						/* 太陽光以外の発電装置 */
	g_einvCmd18_r.loadConnectType				= resData[EINV_CMD18_RES_LOADCONNECTTYPE			];																						/* 負荷接続タイプ */
	g_einvCmd18_r.pvPanelRating					= (ushort)einv_Strtoul(&resData[EINV_CMD18_RES_PVPANELRATING	], EINV_CMD6A_CONTARACTPWRSET_LEN	, EINV_CMD_RES_BASE_HEX	);				/* PVパネル定格 */
	g_einvCmd18_r.outputContrObject				= resData[EINV_CMD18_RES_OUTPUTCONTROBJECT			];																						/* 出力制御対象 */
	g_einvCmd18_r.eqpmtApproveCap				= (ushort)einv_Strtoul(&resData[EINV_CMD18_RES_EQPMTAPPROVECAP	], EINV_CMD18_EQPMTAPPROVECAP_LEN	, EINV_CMD_RES_BASE_DEC	);				/* 設備認定容量 */
	g_einvCmd18_r.genPwrUpperLimit				= (ushort)einv_Strtoul(&resData[EINV_CMD18_RES_GENPWRUPPERLIMIT	], EINV_CMD18_GENPWRUPPERLIMIT_LEN	, EINV_CMD_RES_BASE_DEC	);				/* 発電電力上限設定(上限クリップ) */
	g_einvCmd18_r.fitContractType				= resData[EINV_CMD18_RES_FITCONTRACTTYPE			];																						/* FIT契約タイプ */
	g_einvCmd18_r.selfConsumeType				= resData[EINV_CMD18_RES_SELFCONSUMETYPE			];																						/* 自家消費タイプ */
	memcpy(&g_einvCmd18_r.pwrHouseId, &resData[EINV_CMD18_RES_PWRHOUSEID], EINV_CMD18_PWRHOUSEID_LEN			);																			/* 発電所ID */
	g_einvCmd18_r.pwrCompanyServer				= resData[EINV_CMD18_RES_PWRCOMPANYSERVER			];																						/* 電力会社サーバ */
	g_einvCmd18_r.commuBoardManageMode			= resData[EINV_CMD18_RES_COMMUBOARDMANAGEMODE		];																						/* V:通信ボード内管理運転モード */
	g_einvCmd18_r.emergencyMode					= resData[EINV_CMD18_RES_EMERGENCYMODE				];																						/* 非常時運転モード */
	memcpy(g_einvCmd18_r.sellModeCharStartTime.hour, &resData[EINV_CMD18_RES_SELLMODECHARSTARTTIMEHOUR		], EINV_CMD18_SELLMODECHARSTARTTIMEHOUR_LEN		);								/* 売電モード充電開始時刻(時) */
	memcpy(g_einvCmd18_r.sellModeCharStartTime.minute, &resData[EINV_CMD18_RES_SELLMODECHARSTARTTIMEMINUTE	], EINV_CMD18_SELLMODECHARSTARTTIMEMINUTE_LEN	);								/* 売電モード充電開始時刻(分) */
	memcpy(g_einvCmd18_r.sellModeCharStopTime.hour, &resData[EINV_CMD18_RES_SELLMODECHARSTOPTIMEHOUR		], EINV_CMD18_SELLMODECHARSTOPTIMEHOUR_LEN		);								/* 売電モード充電停止時刻(時) */
	memcpy(g_einvCmd18_r.sellModeCharStopTime.minute, &resData[EINV_CMD18_RES_SELLMODECHARSTOPTIMEMINUTE	], EINV_CMD18_SELLMODECHARSTOPTIMEMINUTE_LEN	);								/* 売電モード充電停止時刻(分) */
	memcpy(g_einvCmd18_r.sellModeDisCStartTime.hour, &resData[EINV_CMD18_RES_SELLMODEDISCSTARTTIMEHOUR		], EINV_CMD18_SELLMODEDISCSTARTTIMEHOUR_LEN		);								/* 売電モード放電開始時刻(時) */
	memcpy(g_einvCmd18_r.sellModeDisCStartTime.minute, &resData[EINV_CMD18_RES_SELLMODEDISCSTARTTIMEMINUTE	], EINV_CMD18_SELLMODEDISCSTARTTIMEMINUTE_LEN	);								/* 売電モード放電開始時刻(分) */
	memcpy(g_einvCmd18_r.sellModeDisCStopTime.hour, &resData[EINV_CMD18_RES_SELLMODEDISCSTOPTIMEHOUR		], EINV_CMD18_SELLMODEDISCSTOPTIMEHOUR_LEN		);								/* 売電モード放電停止時刻(時) */
	memcpy(g_einvCmd18_r.sellModeDisCStopTime.minute, &resData[EINV_CMD18_RES_SELLMODEDISCSTOPTIMEMINUTE	], EINV_CMD18_SELLMODEDISCSTOPTIMEMINUTE_LEN	);								/* 売電モード放電停止時刻(分) */
	memcpy(g_einvCmd18_r.greenModeCharStartTime.hour, &resData[EINV_CMD18_RES_GREENMODECHARSTARTTIMEHOUR	], EINV_CMD18_GREENMODECHARSTARTTIMEHOUR_LEN	);								/* グリーンモード充電開始時刻(時) */
	memcpy(g_einvCmd18_r.greenModeCharStartTime.minute, &resData[EINV_CMD18_RES_GREENMODECHARSTARTTIMEMINUTE], EINV_CMD18_GREENMODECHARSTARTTIMEMINUTE_LEN	);								/* グリーンモード充電開始時刻(分) */
	memcpy(g_einvCmd18_r.greenModeCharStopTime.hour, &resData[EINV_CMD18_RES_GREENMODECHARSTOPTIMEHOUR		], EINV_CMD18_GREENMODECHARSTOPTIMEHOUR_LEN		);								/* グリーンモード充電停止時刻(時) */
	memcpy(g_einvCmd18_r.greenModeCharStopTime.minute, &resData[EINV_CMD18_RES_GREENMODECHARSTOPTIMEMINUTE	], EINV_CMD18_GREENMODECHARSTOPTIMEMINUTE_LEN	);								/* グリーンモード充電停止時刻(分) */
	memcpy(g_einvCmd18_r.greenModeDisCStartTime.hour, &resData[EINV_CMD18_RES_GREENMODEDISCSTARTTIMEHOUR	], EINV_CMD18_GREENMODEDISCSTARTTIMEHOUR_LEN	);								/* グリーンモード放電開始時刻(時) */
	memcpy(g_einvCmd18_r.greenModeDisCStartTime.minute, &resData[EINV_CMD18_RES_GREENMODEDISCSTARTTIMEMINUTE], EINV_CMD18_GREENMODEDISCSTARTTIMEMINUTE_LEN	);								/* グリーンモード放電開始時刻(分) */
	memcpy(g_einvCmd18_r.greenModeDisCStopTime.hour, &resData[EINV_CMD18_RES_GREENMODEDISCSTOPTIMEHOUR		], EINV_CMD18_GREENMODEDISCSTOPTIMEHOUR_LEN		);								/* グリーンモード放電停止時刻(時) */
	memcpy(g_einvCmd18_r.greenModeDisCStopTime.minute, &resData[EINV_CMD18_RES_GREENMODEDISCSTOPTIMEMINUTE	], EINV_CMD18_GREENMODEDISCSTOPTIMEMINUTE_LEN	);								/* グリーンモード放電停止時刻(分) */
	memcpy(&g_einvCmd18_r.sunPwrRated, &resData[EINV_CMD18_RES_SUNPWRRATED						], EINV_CMD18_SUNPWRRATED_LEN			);													/* 外付け太陽光発電の定格 */
	g_einvCmd18_r.sunPwrExtern					= resData[EINV_CMD18_RES_SUNPWREXTERN				];																						/* 外付け太陽光発電の有無 */
	g_einvCmd18_r.btCharUpperLimit				= (ushort)einv_Strtoul(&resData[EINV_CMD18_RES_BTCHARUPPERLIMIT	], EINV_CMD18_BTCHARUPPERLIMIT_LEN	, EINV_CMD_RES_BASE_DEC	);				/* 蓄電池充電上限設定値 */
	g_einvCmd18_r.btDiscLowerLimit				= (ushort)einv_Strtoul(&resData[EINV_CMD18_RES_BTDISCLOWERLIMIT	], EINV_CMD18_BTDISCLOWERLIMIT_LEN	, EINV_CMD_RES_BASE_DEC	);				/* 蓄電池放電下限設定値 */
	g_einvCmd18_r.carOpertPriority				= resData[EINV_CMD18_RES_CAROPERTPRIORITY			];																						/* 車両操作優先 */
	g_einvCmd18_r.carCharordiscBodyOprat		= resData[EINV_CMD18_RES_CARCHARORDISCBODYOPRAT		];																						/* 車両充放電器本体操作 */
	g_einvCmd18_r.pwrcutDischar					= resData[EINV_CMD18_RES_PWRCUTDISCHAR				];																						/* 停電時放電動作 */
	g_einvCmd18_r.pwrcutDiscPrioritySet			= resData[EINV_CMD18_RES_PWRCUTDISCPRIORITYSET		];																						/* 停電時放電優先設定 */
	g_einvCmd18_r.pwrcutCharPrioritySet			= resData[EINV_CMD18_RES_PWRCUTCHARPRIORITYSET		];																						/* 停電時充電優先設定 */
	g_einvCmd18_r.carDiscLowerLimitSet			= (ushort)einv_Strtoul(&resData[EINV_CMD18_RES_CARDISCLOWERLIMITSET		], EINV_CMD18_CARDISCLOWERLIMITSET_LEN	, EINV_CMD_RES_BASE_DEC);	/* 車両放電下限値設定 */
	g_einvCmd18_r.carCharUpperLimitSet			= (ushort)einv_Strtoul(&resData[EINV_CMD18_RES_CARCHARUPPERLIMITSET		], EINV_CMD18_CARCHARUPPERLIMITSET_LEN	, EINV_CMD_RES_BASE_DEC);	/* 車両充電上限値設定 */
	g_einvCmd18_r.carDiscOrnot					= resData[EINV_CMD18_RES_CARDISCORNOT				];																						/* 車両放電有無 */
	g_einvCmd18_r.discharPrioritySet			= resData[EINV_CMD18_RES_DISCHARPRIORITYSET			];																						/* 放電優先設定 */
	g_einvCmd18_r.pwrcutDiscLowerLimitSet		= (ushort)einv_Strtoul(&resData[EINV_CMD18_RES_PWRCUTDISCLOWERLIMITSET	], EINV_CMD18_PWRCUTDISCLOWERLIMITSET_LEN	, EINV_CMD_RES_BASE_DEC);/* 停電時車両放電下限値設定 */
	g_einvCmd18_r.chargePrioritySet				= resData[EINV_CMD18_RES_CHARGEPRIORITYSET			];																						/* 充電優先設定 */
	g_einvCmd18_r.sunPwrSetLocation				= resData[EINV_CMD18_RES_SUNPWRSETLOCATION			];																						/* 太陽光発電設置場所 */
	g_einvCmd18_r.btSetLocation					= resData[EINV_CMD18_RES_BTSETLOCATION				];																						/* 蓄電池設置場所 */
	g_einvCmd18_r.v2hStandSetLocation			= resData[EINV_CMD18_RES_V2HSTANDSETLOCATION		];																						/* V2Hスタンド設置場所 */
	g_einvCmd18_r.electWaterHeating				= resData[EINV_CMD18_RES_ELECTWATERHEATING			];																						/* 電気給湯器 */
	memcpy(&g_einvCmd18_r.postalNum, &resData[EINV_CMD18_RES_POSTALNUM], EINV_CMD18_POSTALNUM_LEN			);																				/* 郵便番号 */
	g_einvCmd18_r.echonetLiteVer				= resData[EINV_CMD18_RES_ECHONETLITEVER				];																						/* ECHONET Lite_Ver */
	g_einvCmd18_r.opRemoteControlOrnot			= resData[EINV_CMD18_RES_OPREMOTECONTROLORNOT		];																						/* OPリモコン有無 */
	g_einvCmd18_r.btCharPwrMaxValueSet			= (ushort)einv_Strtoul(&resData[EINV_CMD18_RES_BTCHARPWRMAXVALUESET		], EINV_CMD18_BTCHARPWRMAXVALUESET_LEN	, EINV_CMD_RES_BASE_DEC);	/* 蓄電池最大充電電力設定 */
	g_einvCmd18_r.carCharPwrValueSet			= (ushort)einv_Strtoul(&resData[EINV_CMD18_RES_CARCHARPWRVALUESET		], EINV_CMD18_CARCHARPWRVALUESET_LEN	, EINV_CMD_RES_BASE_DEC);	/* 車両最大充電電力設定 */
	g_einvCmd18_r.pwrcutOutSet					= resData[EINV_CMD18_RES_PWRCUTOUTSET				];																						/* 停電時出力設定 */
	g_einvCmd18_r.ipAddrSet						= resData[EINV_CMD18_RES_IPADDRSET					];																						/* IPアドレス設定 */
	einv_MakeIpAddress(&resData[EINV_CMD18_RES_IPADDRESS		], g_einvCmd18_r.ipAddress			);																						/* IPアドレス */
	einv_MakeIpAddress(&resData[EINV_CMD18_RES_SUBNETMASK		], g_einvCmd18_r.subnetMask			);																						/* サブネットマスク */
	einv_MakeIpAddress(&resData[EINV_CMD18_RES_DEFAULTGATEWAY	], g_einvCmd18_r.defaultGateway		);																						/* デフォルトゲートウェイ */
	einv_MakeIpAddress(&resData[EINV_CMD18_RES_POIORITYDNS		], g_einvCmd18_r.poiorityDns		);																						/* 優先DNS */
	einv_MakeIpAddress(&resData[EINV_CMD18_RES_REPLACEDNS		], g_einvCmd18_r.replaceDns			);																						/* 代替DNS */
	g_einvCmd18_r.proxySet						= resData[EINV_CMD18_RES_PROXYSET					];																						/* PROXY設定 */
	memcpy(&g_einvCmd18_r.proxyAddress, &resData[EINV_CMD18_RES_PROXYADDRESS], EINV_CMD18_PROXYADDRESS_LEN	);																				/* PROXYアドレス */
	memcpy(&g_einvCmd18_r.portNumber, &resData[EINV_CMD18_RES_PORTNUMBER	], EINV_CMD18_PORTNUMBER_LEN	);																				/* ポート番号 */
	g_einvCmd18_r.testrunBtChar					= resData[EINV_CMD18_RES_TESTRUNBTCHAR				];																						/* 試運転_蓄電池充電 */
	g_einvCmd18_r.testrunBtDisc					= resData[EINV_CMD18_RES_TESTRUNBTDISC				];																						/* 試運転_蓄電池放電 */
	g_einvCmd18_r.testrunBuyE1					= resData[EINV_CMD18_RES_TESTRUNBUYE1				];																						/* 試運転_買電(E1-ACCT) */
	g_einvCmd18_r.testrunBuyC2					= resData[EINV_CMD18_RES_TESTRUNBUYC2				];																						/* 試運転_買電(C2-ACCT) */
	g_einvCmd18_r.testrunCarChar				= resData[EINV_CMD18_RES_TESTRUNCARCHAR				];																						/* 試運転_車両充電 */
	g_einvCmd18_r.testrunCarDisc				= resData[EINV_CMD18_RES_TESTRUNCARDISC				];																						/* 試運転_車両放電 */
	g_einvCmd18_r.testrunInterSunPwrGerat		= resData[EINV_CMD18_RES_TESTRUNINTERSUNPWRGERAT	];																						/* 試運転_内部太陽光発電の動作 */
	g_einvCmd18_r.testrunExternSunPwrGerat		= resData[EINV_CMD18_RES_TESTRUNEXTERNSUNPWRGERAT	];																						/* 試運転_外付け太陽光発電の動作 */
	g_einvCmd18_r.testrunSwitchPwrCutMode		= resData[EINV_CMD18_RES_TESTRUNSWITCHPWRCUTMODE	];																						/* 試運転_切り替えモジュール動作(停電) */
	g_einvCmd18_r.testrunPutCutE1Disc			= resData[EINV_CMD18_RES_TESTRUNPUTCUTE1DISC		];																						/* 試運転_停電時E1放電 */
	g_einvCmd18_r.testrunSwitchResetMode		= resData[EINV_CMD18_RES_TESTRUNSWITCHRESETMODE		];																						/* 試運転_切り替えモジュール動作(復帰) */
	g_einvCmd18_r.reserve						= (uchar)einv_Strtoul(&resData[EINV_CMD18_RES_RESERVE	], EINV_CMD18_RESERVE_LEN	, EINV_CMD_RES_BASE_DEC		);							/* 予約 */
	g_einvCmd18_r.clipAction					= resData[EINV_CMD18_RES_CLIPACTION					];																						/* クリップ動作 */
	memcpy(&g_einvCmd18_r.mentanceDate.year, &resData[EINV_CMD18_RES_MENTANCEDATEYEAR	], EINV_CMD18_MENTANCEDATEYEAR_LEN	);																/* メンテ期間開始日(年) */
	memcpy(&g_einvCmd18_r.mentanceDate.month, &resData[EINV_CMD18_RES_MENTANCEDATEMONTH	], EINV_CMD18_MENTANCEDATEYEAR_LEN	);																/* メンテ期間開始日(月) */
	memcpy(&g_einvCmd18_r.mentanceDate.day, &resData[EINV_CMD18_RES_MENTANCEDATEDAY		], EINV_CMD18_MENTANCEDATEDAY_LEN	);																/* メンテ期間開始日(日) */
	memcpy(&g_einvCmd18_r.impltationDesignDay1.year, &resData[EINV_CMD18_RES_IMPLTATIONDESIGNDAY1YEAR	], EINV_CMD18_IMPLTATIONDESIGNDAY1YEAR_LEN	);										/* 実施指定日1(年) */
	memcpy(&g_einvCmd18_r.impltationDesignDay1.month, &resData[EINV_CMD18_RES_IMPLTATIONDESIGNDAY1MONTH	], EINV_CMD18_IMPLTATIONDESIGNDAY1MONTH_LEN	);										/* 実施指定日1(月) */
	memcpy(&g_einvCmd18_r.impltationDesignDay1.day, &resData[EINV_CMD18_RES_IMPLTATIONDESIGNDAY1DAY		], EINV_CMD18_IMPLTATIONDESIGNDAY1DAY_LEN	);										/* 実施指定日1(日) */
	memcpy(&g_einvCmd18_r.impltationDesignDay2.year, &resData[EINV_CMD18_RES_IMPLTATIONDESIGNDAY2YEAR	], EINV_CMD18_IMPLTATIONDESIGNDAY2YEAR_LEN	);										/* 実施指定日2(年) */
	memcpy(&g_einvCmd18_r.impltationDesignDay2.month, &resData[EINV_CMD18_RES_IMPLTATIONDESIGNDAY2MONTH	], EINV_CMD18_IMPLTATIONDESIGNDAY2MONTH_LEN	);										/* 実施指定日2(月) */
	memcpy(&g_einvCmd18_r.impltationDesignDay2.day, &resData[EINV_CMD18_RES_IMPLTATIONDESIGNDAY2DAY		], EINV_CMD18_IMPLTATIONDESIGNDAY2DAY_LEN	);										/* 実施指定日2(日) */
	g_einvCmd18_r.executeMode					= resData[EINV_CMD18_RES_EXECUTEMODE				];																						/* 実行モード */
	g_einvCmd18_r.maintainResult				= resData[EINV_CMD18_RES_MAINTAINRESULT				];																						/* メンテ結果 */


	DLOG_Debug("g_einvCmd18_r.exterSunPwrOrnot = %d\n",				g_einvCmd18_r.exterSunPwrOrnot);
	DLOG_Debug("g_einvCmd18_r.sunBeyondPwrGenratDevice = %d\n",		g_einvCmd18_r.sunBeyondPwrGenratDevice);
	DLOG_Debug("g_einvCmd18_r.loadConnectType = %d\n",				g_einvCmd18_r.loadConnectType);
	DLOG_Debug("g_einvCmd18_r.pvPanelRating = %d\n",				g_einvCmd18_r.pvPanelRating);
	DLOG_Debug("g_einvCmd18_r.outputContrObject = %d\n",			g_einvCmd18_r.outputContrObject);
	DLOG_Debug("g_einvCmd18_r.eqpmtApproveCap = %d\n",				g_einvCmd18_r.eqpmtApproveCap);
	DLOG_Debug("g_einvCmd18_r.genPwrUpperLimit = %d\n",				g_einvCmd18_r.genPwrUpperLimit);
	DLOG_Debug("g_einvCmd18_r.fitContractType = %d\n",				g_einvCmd18_r.fitContractType);
	DLOG_Debug("g_einvCmd18_r.selfConsumeType = %d\n",				g_einvCmd18_r.selfConsumeType);
	DLOG_Debug("g_einvCmd18_r.pwrHouseId = %s\n",					g_einvCmd18_r.pwrHouseId);
	DLOG_Debug("g_einvCmd18_r.pwrCompanyServer = %d\n",				g_einvCmd18_r.pwrCompanyServer);
	DLOG_Debug("g_einvCmd18_r.commuBoardManageMode = %d\n",			g_einvCmd18_r.commuBoardManageMode);
	DLOG_Debug("g_einvCmd18_r.emergencyMode = %d\n",				g_einvCmd18_r.emergencyMode);
	DLOG_Debug("g_einvCmd18_r.sellModeCharStartTime.hour = %s\n",	g_einvCmd18_r.sellModeCharStartTime.hour);
	DLOG_Debug("g_einvCmd18_r.sellModeCharStartTime.minute = %s\n",	g_einvCmd18_r.sellModeCharStartTime.minute);
	DLOG_Debug("g_einvCmd18_r.sellModeCharStopTime.hour = %s\n",	g_einvCmd18_r.sellModeCharStopTime.hour);
	DLOG_Debug("g_einvCmd18_r.sellModeCharStopTime.minute = %s\n",	g_einvCmd18_r.sellModeCharStopTime.minute);
	DLOG_Debug("g_einvCmd18_r.sellModeDisCStartTime.hour = %s\n",	g_einvCmd18_r.sellModeDisCStartTime.hour);
	DLOG_Debug("g_einvCmd18_r.sellModeDisCStartTime.minute = %s\n",	g_einvCmd18_r.sellModeDisCStartTime.minute);
	DLOG_Debug("g_einvCmd18_r.sellModeDisCStopTime.hour = %s\n",	g_einvCmd18_r.sellModeDisCStopTime.hour);
	DLOG_Debug("g_einvCmd18_r.sellModeDisCStopTime.minute = %s\n",	g_einvCmd18_r.sellModeDisCStopTime.minute);
	DLOG_Debug("g_einvCmd18_r.greenModeCharStartTime.hour = %s\n",	g_einvCmd18_r.greenModeCharStartTime.hour);
	DLOG_Debug("g_einvCmd18_r.greenModeCharStartTime.minute = %s\n",g_einvCmd18_r.greenModeCharStartTime.minute);
	DLOG_Debug("g_einvCmd18_r.greenModeCharStopTime.hour = %s\n",	g_einvCmd18_r.greenModeCharStopTime.hour);
	DLOG_Debug("g_einvCmd18_r.greenModeCharStopTime.minute = %s\n",	g_einvCmd18_r.greenModeCharStopTime.minute);
	DLOG_Debug("g_einvCmd18_r.greenModeDisCStartTime.hour = %s\n",	g_einvCmd18_r.greenModeDisCStartTime.hour);
	DLOG_Debug("g_einvCmd18_r.greenModeDisCStartTime.minute = %s\n",g_einvCmd18_r.greenModeDisCStartTime.minute);
	DLOG_Debug("g_einvCmd18_r.greenModeDisCStopTime.hour = %s\n",	g_einvCmd18_r.greenModeDisCStopTime.hour);
	DLOG_Debug("g_einvCmd18_r.greenModeDisCStopTime.minute = %s\n",	g_einvCmd18_r.greenModeDisCStopTime.minute);
	DLOG_Debug("g_einvCmd18_r.sunPwrRated = %s\n",					g_einvCmd18_r.sunPwrRated);
	DLOG_Debug("g_einvCmd18_r.sunPwrExtern = %d\n",					g_einvCmd18_r.sunPwrExtern);
	DLOG_Debug("g_einvCmd18_r.btCharUpperLimit = %d\n",				g_einvCmd18_r.btCharUpperLimit);
	DLOG_Debug("g_einvCmd18_r.btDiscLowerLimit = %d\n",				g_einvCmd18_r.btDiscLowerLimit);
	DLOG_Debug("g_einvCmd18_r.carOpertPriority = %d\n",				g_einvCmd18_r.carOpertPriority);
	DLOG_Debug("g_einvCmd18_r.carCharordiscBodyOprat = %d\n",		g_einvCmd18_r.carCharordiscBodyOprat);
	DLOG_Debug("g_einvCmd18_r.pwrcutDischar = %d\n",				g_einvCmd18_r.pwrcutDischar);
	DLOG_Debug("g_einvCmd18_r.pwrcutDiscPrioritySet = %d\n",		g_einvCmd18_r.pwrcutDiscPrioritySet);
	DLOG_Debug("g_einvCmd18_r.pwrcutCharPrioritySet = %d\n",		g_einvCmd18_r.pwrcutCharPrioritySet);
	DLOG_Debug("g_einvCmd18_r.carDiscLowerLimitSet = %d\n",			g_einvCmd18_r.carDiscLowerLimitSet);
	DLOG_Debug("g_einvCmd18_r.carCharUpperLimitSet = %d\n",			g_einvCmd18_r.carCharUpperLimitSet);
	DLOG_Debug("g_einvCmd18_r.carDiscOrnot = %d\n",					g_einvCmd18_r.carDiscOrnot);
	DLOG_Debug("g_einvCmd18_r.discharPrioritySet = %d\n",			g_einvCmd18_r.discharPrioritySet);
	DLOG_Debug("g_einvCmd18_r.pwrcutDiscLowerLimitSet = %d\n",		g_einvCmd18_r.pwrcutDiscLowerLimitSet);
	DLOG_Debug("g_einvCmd18_r.chargePrioritySet = %d\n",			g_einvCmd18_r.chargePrioritySet);
	DLOG_Debug("g_einvCmd18_r.sunPwrSetLocation = %d\n",			g_einvCmd18_r.sunPwrSetLocation);
	DLOG_Debug("g_einvCmd18_r.btSetLocation = %d\n",				g_einvCmd18_r.btSetLocation);
	DLOG_Debug("g_einvCmd18_r.v2hStandSetLocation = %d\n",			g_einvCmd18_r.v2hStandSetLocation);
	DLOG_Debug("g_einvCmd18_r.electWaterHeating = %d\n",			g_einvCmd18_r.electWaterHeating);
	DLOG_Debug("g_einvCmd18_r.postalNum = %s\n",					g_einvCmd18_r.postalNum);
	DLOG_Debug("g_einvCmd18_r.echonetLiteVer = %d\n",				g_einvCmd18_r.echonetLiteVer);
	DLOG_Debug("g_einvCmd18_r.opRemoteControlOrnot = %d\n",			g_einvCmd18_r.opRemoteControlOrnot);
	DLOG_Debug("g_einvCmd18_r.btCharPwrMaxValueSet = %d\n",			g_einvCmd18_r.btCharPwrMaxValueSet);
	DLOG_Debug("g_einvCmd18_r.carCharPwrValueSet = %d\n",			g_einvCmd18_r.carCharPwrValueSet);
	DLOG_Debug("g_einvCmd18_r.pwrcutOutSet = %d\n",					g_einvCmd18_r.pwrcutOutSet);
	DLOG_Debug("g_einvCmd18_r.ipAddrSet = %d\n",					g_einvCmd18_r.ipAddrSet);
	DLOG_Debug("g_einvCmd18_r.ipAddress = %s\n",					g_einvCmd18_r.ipAddress);
	DLOG_Debug("g_einvCmd18_r.subnetMask = %s\n",					g_einvCmd18_r.subnetMask);
	DLOG_Debug("g_einvCmd18_r.defaultGateway = %s\n",				g_einvCmd18_r.defaultGateway);
	DLOG_Debug("g_einvCmd18_r.poiorityDns = %s\n",					g_einvCmd18_r.poiorityDns);
	DLOG_Debug("g_einvCmd18_r.replaceDns = %s\n",					g_einvCmd18_r.replaceDns);
	DLOG_Debug("g_einvCmd18_r.proxySet = %d\n",						g_einvCmd18_r.proxySet);
	DLOG_Debug("g_einvCmd18_r.proxyAddress = %s\n",					g_einvCmd18_r.proxyAddress);
	DLOG_Debug("g_einvCmd18_r.portNumber = %s\n",					g_einvCmd18_r.portNumber);
	DLOG_Debug("g_einvCmd18_r.testrunBtChar = %d\n",				g_einvCmd18_r.testrunBtChar);
	DLOG_Debug("g_einvCmd18_r.testrunBtDisc = %d\n",				g_einvCmd18_r.testrunBtDisc);
	DLOG_Debug("g_einvCmd18_r.testrunBuyE1 = %d\n",					g_einvCmd18_r.testrunBuyE1);
	DLOG_Debug("g_einvCmd18_r.testrunBuyC2 = %d\n",					g_einvCmd18_r.testrunBuyC2);
	DLOG_Debug("g_einvCmd18_r.testrunCarChar = %d\n",				g_einvCmd18_r.testrunCarChar);
	DLOG_Debug("g_einvCmd18_r.testrunCarDisc = %d\n",				g_einvCmd18_r.testrunCarDisc);
	DLOG_Debug("g_einvCmd18_r.testrunInterSunPwrGerat = %d\n",		g_einvCmd18_r.testrunInterSunPwrGerat);
	DLOG_Debug("g_einvCmd18_r.testrunExternSunPwrGerat = %d\n",		g_einvCmd18_r.testrunExternSunPwrGerat);
	DLOG_Debug("g_einvCmd18_r.testrunSwitchPwrCutMode = %d\n",		g_einvCmd18_r.testrunSwitchPwrCutMode);
	DLOG_Debug("g_einvCmd18_r.testrunPutCutE1Disc = %d\n",			g_einvCmd18_r.testrunPutCutE1Disc);
	DLOG_Debug("g_einvCmd18_r.testrunSwitchResetMode = %d\n",		g_einvCmd18_r.testrunSwitchResetMode);
	DLOG_Debug("g_einvCmd18_r.reserve = %d\n",						g_einvCmd18_r.reserve);
	DLOG_Debug("g_einvCmd18_r.clipAction = %d\n",					g_einvCmd18_r.clipAction);
	DLOG_Debug("g_einvCmd18_r.mentanceDate.year = %s\n",			g_einvCmd18_r.mentanceDate.year);
	DLOG_Debug("g_einvCmd18_r.mentanceDate.month = %s\n",			g_einvCmd18_r.mentanceDate.month);
	DLOG_Debug("g_einvCmd18_r.mentanceDate.day = %s\n",				g_einvCmd18_r.mentanceDate.day);
	DLOG_Debug("g_einvCmd18_r.impltationDesignDay1.year = %s\n",	g_einvCmd18_r.impltationDesignDay1.year);
	DLOG_Debug("g_einvCmd18_r.impltationDesignDay1.month = %s\n",	g_einvCmd18_r.impltationDesignDay1.month);
	DLOG_Debug("g_einvCmd18_r.impltationDesignDay1.day = %s\n",		g_einvCmd18_r.impltationDesignDay1.day);
	DLOG_Debug("g_einvCmd18_r.impltationDesignDay2.year = %s\n",	g_einvCmd18_r.impltationDesignDay2.year);
	DLOG_Debug("g_einvCmd18_r.impltationDesignDay2.month = %s\n",	g_einvCmd18_r.impltationDesignDay2.month);
	DLOG_Debug("g_einvCmd18_r.impltationDesignDay2.day = %s\n",		g_einvCmd18_r.impltationDesignDay2.day);
	DLOG_Debug("g_einvCmd18_r.executeMode = %d\n",					g_einvCmd18_r.executeMode);
	DLOG_Debug("g_einvCmd18_r.maintainResult = %d\n",				g_einvCmd18_r.maintainResult);

	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:作成CMD19データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:cmdData	:cmdデータ									*/
/*					:reqSize	:要求サイズ									*/
/*					:reqData	:要求データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:作成CMD19データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_MakeCmd19Data(EinvCmdDataT_t *cmdData, slong reqSize, uchar *reqData)
{
	EinvCmd19T_t *cmd19 = &cmdData->cmd19_t;
	schar buf[EINV_LONG_BUF_SIZE] = { 0 };
	ulong cnt = 0UL;

	DEBUG_TRCIN( );

	memset(reqData, EINV_CODE_NO_CHANGE, reqSize);
	memset(buf   , 0                  , sizeof(buf));

	/* 外付け太陽光発電有無 */
	reqData[cnt] = cmd19->exterSunPwrOrnot;
	cnt = cnt + EINV_CMD19_EXTERSUNPWRORNOT_LEN;

	/* 太陽光以外の発電装置 */
	reqData[cnt] = cmd19->sunBeyondPwrGenratDevice;
	cnt = cnt + EINV_CMD19_SUNBEYONDPWRGENRATDEVICE_LEN;

	/* 負荷接続タイプ */
	reqData[cnt] = cmd19->loadConnectType;
	cnt = cnt + EINV_CMD19_LOADCONNECTTYPE_LEN;

	/* PVパネル定格 */
	snprintf(buf, sizeof(buf), "%04X",cmd19->pvPanelRating);
	memcpy(&reqData[cnt], buf, EINV_CMD19_PVPANELRATING_LEN);
	cnt = cnt + EINV_CMD19_PVPANELRATING_LEN;

	/* 出力制御対象 */
	reqData[cnt] = cmd19->outputContrObject;
	cnt = cnt + EINV_CMD19_OUTPUTCONTROBJECT_LEN;

	/* 設備認定容量 */
	snprintf(buf, sizeof(buf), "%04u",cmd19->eqpmtApproveCap);
	memcpy(&reqData[cnt], buf, EINV_CMD19_EQPMTAPPROVECAP_LEN);
	cnt = cnt + EINV_CMD19_EQPMTAPPROVECAP_LEN;

	/* 発電電力上限設定(上限クリップ) */
	snprintf(buf, sizeof(buf), "%04u",cmd19->genPwrUpperLimit);
	memcpy(&reqData[cnt], buf, EINV_CMD19_GENPWRUPPERLIMIT_LEN);
	cnt = cnt + EINV_CMD19_GENPWRUPPERLIMIT_LEN;

	/* FIT契約タイプ */
	reqData[cnt] = cmd19->fitContractType;
	cnt = cnt + EINV_CMD19_FITCONTRACTTYPE_LEN;

	/* 自家消費タイプ */
	reqData[cnt] = cmd19->selfConsumeType;
	cnt = cnt + EINV_CMD19_SELFCONSUMETYPE_LEN;

	/* 発電所ID */
	memcpy(&reqData[cnt], &cmd19->pwrHouseId, EINV_CMD19_PWRHOUSEID_LEN);
	cnt = cnt + EINV_CMD19_PWRHOUSEID_LEN;

	/* 電力会社サーバ */
	reqData[cnt] = cmd19->pwrCompanyServer;
	cnt = cnt + EINV_CMD19_PWRCOMPANYSERVER_LEN;

	/* V:通信ボード内管理運転モード */
	reqData[cnt] = cmd19->commuBoardManageMode;
	cnt = cnt + EINV_CMD19_COMMUBOARDMANAGEMODE_LEN;

	/* 非常時運転モード */
	reqData[cnt] = cmd19->emergencyMode;
	cnt = cnt + EINV_CMD19_EMERGENCYMODE_LEN;

	/* 売電モード充電開始時刻(時) */
	memcpy(&reqData[cnt], &cmd19->sellModeCharStartTime.hour, EINV_CMD19_SELLMODECHARSTARTTIMEHOUR_LEN);
	cnt = cnt + EINV_CMD19_SELLMODECHARSTARTTIMEHOUR_LEN;

	/* 売電モード充電開始時刻(分) */
	memcpy(&reqData[cnt], &cmd19->sellModeCharStartTime.minute, EINV_CMD19_SELLMODECHARSTARTTIMEMINUTE_LEN);
	cnt = cnt + EINV_CMD19_SELLMODECHARSTARTTIMEMINUTE_LEN;

	/* 売電モード充電停止時刻(時) */
	memcpy(&reqData[cnt], &cmd19->sellModeCharStopTime.hour, EINV_CMD19_SELLMODECHARSTOPTIMEHOUR_LEN);
	cnt = cnt + EINV_CMD19_SELLMODECHARSTOPTIMEHOUR_LEN;

	/* 売電モード充電停止時刻(分) */
	memcpy(&reqData[cnt], &cmd19->sellModeCharStopTime.minute, EINV_CMD19_SELLMODECHARSTOPTIMEMINUTE_LEN);
	cnt = cnt + EINV_CMD19_SELLMODECHARSTOPTIMEMINUTE_LEN;

	/* 売電モード放電開始時刻(時) */
	memcpy(&reqData[cnt], &cmd19->sellModeDisCStartTime.hour, EINV_CMD19_SELLMODEDISCSTARTTIMEHOUR_LEN);
	cnt = cnt + EINV_CMD19_SELLMODEDISCSTARTTIMEHOUR_LEN;

	/* 売電モード放電開始時刻(分) */
	memcpy(&reqData[cnt], &cmd19->sellModeDisCStartTime.minute, EINV_CMD19_SELLMODEDISCSTARTTIMEMINUTE_LEN);
	cnt = cnt + EINV_CMD19_SELLMODEDISCSTARTTIMEMINUTE_LEN;

	/* 売電モード放電停止時刻(時) */
	memcpy(&reqData[cnt], &cmd19->sellModeDisCStopTime.hour, EINV_CMD19_SELLMODEDISCSTOPTIMEHOUR_LEN);
	cnt = cnt + EINV_CMD19_SELLMODEDISCSTOPTIMEHOUR_LEN;

	/* 売電モード放電停止時刻(分) */
	memcpy(&reqData[cnt], &cmd19->sellModeDisCStopTime.minute, EINV_CMD19_SELLMODEDISCSTOPTIMEMINUTE_LEN);
	cnt = cnt + EINV_CMD19_SELLMODEDISCSTOPTIMEMINUTE_LEN;

	/* グリーンモード充電開始時刻(時) */
	memcpy(&reqData[cnt], &cmd19->greenModeCharStartTime.hour, EINV_CMD19_GREENMODECHARSTARTTIMEHOUR_LEN);
	cnt = cnt + EINV_CMD19_GREENMODECHARSTARTTIMEHOUR_LEN;

	/* グリーンモード充電開始時刻(分) */
	memcpy(&reqData[cnt], &cmd19->greenModeCharStartTime.minute, EINV_CMD19_GREENMODECHARSTARTTIMEMINUTE_LEN);
	cnt = cnt + EINV_CMD19_GREENMODECHARSTARTTIMEMINUTE_LEN;

	/* グリーンモード充電停止時刻(時) */
	memcpy(&reqData[cnt], &cmd19->greenModeCharStopTime.hour, EINV_CMD19_GREENMODECHARSTOPTIMEHOUR_LEN);
	cnt = cnt + EINV_CMD19_GREENMODECHARSTOPTIMEHOUR_LEN;

	/* グリーンモード充電停止時刻(分) */
	memcpy(&reqData[cnt], &cmd19->greenModeCharStopTime.minute, EINV_CMD19_GREENMODECHARSTOPTIMEMINUTE_LEN);
	cnt = cnt + EINV_CMD19_GREENMODECHARSTOPTIMEMINUTE_LEN;

	/* グリーンモード放電開始時刻(時) */
	memcpy(&reqData[cnt], &cmd19->greenModeDisCStartTime.hour, EINV_CMD19_GREENMODEDISCSTARTTIMEHOUR_LEN);
	cnt = cnt + EINV_CMD19_GREENMODEDISCSTARTTIMEHOUR_LEN;

	/* グリーンモード放電開始時刻(分) */
	memcpy(&reqData[cnt], &cmd19->greenModeDisCStartTime.minute, EINV_CMD19_GREENMODEDISCSTARTTIMEMINUTE_LEN);
	cnt = cnt + EINV_CMD19_GREENMODEDISCSTARTTIMEMINUTE_LEN;

	/* グリーンモード放電停止時刻(時) */
	memcpy(&reqData[cnt], &cmd19->greenModeDisCStopTime.hour, EINV_CMD19_GREENMODEDISCSTOPTIMEHOUR_LEN);
	cnt = cnt + EINV_CMD19_GREENMODEDISCSTOPTIMEHOUR_LEN;

	/* グリーンモード放電停止時刻(分) */
	memcpy(&reqData[cnt], &cmd19->greenModeDisCStopTime.minute, EINV_CMD19_GREENMODEDISCSTOPTIMEMINUTE_LEN);
	cnt = cnt + EINV_CMD19_GREENMODEDISCSTOPTIMEMINUTE_LEN;

	/* 外付け太陽光発電の定格 */
	memcpy(&reqData[cnt], cmd19->sunPwrRated, EINV_CMD19_SUNPWRRATED_LEN);
	cnt = cnt + EINV_CMD19_SUNPWRRATED_LEN;

	/* 外付け太陽光発電の有無 */
	reqData[cnt] = cmd19->sunPwrExtern;
	cnt = cnt + EINV_CMD19_SUNPWREXTERN_LEN;

	/* 蓄電池充電上限設定値 */
	snprintf(buf, sizeof(buf), "%03u",cmd19->greenModeDisCStopTime);
	memcpy(&reqData[cnt], buf, EINV_CMD19_BTCHARUPPERLIMIT_LEN);
	cnt = cnt + EINV_CMD19_BTCHARUPPERLIMIT_LEN;

	/* 蓄電池放電下限設定値 */
	snprintf(buf, sizeof(buf), "%03u",cmd19->greenModeDisCStopTime);
	memcpy(&reqData[cnt], buf, EINV_CMD19_BTDISCLOWERLIMIT_LEN);
	cnt = cnt + EINV_CMD19_BTDISCLOWERLIMIT_LEN;

	/* 車両操作優先 */
	reqData[cnt] = cmd19->carOpertPriority;
	cnt = cnt + EINV_CMD19_CAROPERTPRIORITY_LEN;

	/* 車両充放電器本体操作 */
	reqData[cnt] = cmd19->carCharordiscBodyOprat;
	cnt = cnt + EINV_CMD19_CARCHARORDISCBODYOPRAT_LEN;

	/* 停電時放電動作 */
	reqData[cnt] = cmd19->pwrcutDischar;
	cnt = cnt + EINV_CMD19_PWRCUTDISCHAR_LEN;

	/* 停電時放電優先設定 */
	reqData[cnt] = cmd19->pwrcutDiscPrioritySet;
	cnt = cnt + EINV_CMD19_PWRCUTDISCPRIORITYSET_LEN;

	/* 停電時充電優先設定 */
	reqData[cnt] = cmd19->pwrcutCharPrioritySet;
	cnt = cnt + EINV_CMD19_PWRCUTCHARPRIORITYSET_LEN;

	/* 車両放電下限値設定 */
	snprintf(buf, sizeof(buf), "%03u",cmd19->carDiscLowerLimitSet);
	memcpy(&reqData[cnt], buf, EINV_CMD19_CARDISCLOWERLIMITSET_LEN);
	cnt = cnt + EINV_CMD19_CARDISCLOWERLIMITSET_LEN;

	/* 車両充電上限値設定 */
	snprintf(buf, sizeof(buf), "%03u",cmd19->carCharUpperLimitSet);
	memcpy(&reqData[cnt], buf, EINV_CMD19_CARCHARUPPERLIMITSET_LEN);
	cnt = cnt + EINV_CMD19_CARCHARUPPERLIMITSET_LEN;

	/* 車両放電有無 */
	reqData[cnt] = cmd19->carDiscOrnot;
	cnt = cnt + EINV_CMD19_CARDISCORNOT_LEN;

	/* 放電優先設定 */
	reqData[cnt] = cmd19->discharPrioritySet;
	cnt = cnt + EINV_CMD19_DISCHARPRIORITYSET_LEN;

	/* 停電時車両放電下限値設定 */
	snprintf(buf, sizeof(buf), "%03u",cmd19->pwrcutDiscLowerLimitSet);
	memcpy(&reqData[cnt], buf, EINV_CMD19_PWRCUTDISCLOWERLIMITSET_LEN);
	cnt = cnt + EINV_CMD19_PWRCUTDISCLOWERLIMITSET_LEN;

	/* 充電優先設定 */
	reqData[cnt] = cmd19->chargePrioritySet;
	cnt = cnt + EINV_CMD19_CHARGEPRIORITYSET_LEN;

	/* 太陽光発電設置場所 */
	reqData[cnt] = cmd19->sunPwrSetLocation;
	cnt = cnt + EINV_CMD19_SUNPWRSETLOCATION_LEN;

	/* 蓄電池設置場所 */
	reqData[cnt] = cmd19->btSetLocation;
	cnt = cnt + EINV_CMD19_BTSETLOCATION_LEN;

	/* V2Hスタンド設置場所 */
	reqData[cnt] = cmd19->v2hStandSetLocation;
	cnt = cnt + EINV_CMD19_V2HSTANDSETLOCATION_LEN;

	/* 電気給湯器 */
	reqData[cnt] = cmd19->electWaterHeating;
	cnt = cnt + EINV_CMD19_ELECTWATERHEATING_LEN;

	/* 郵便番号 */
	memcpy(&reqData[cnt], &cmd19->postalNum, EINV_CMD19_POSTALNUM_LEN);
	cnt = cnt + EINV_CMD19_POSTALNUM_LEN;

	/* ECHONET Lite_Ver */
	reqData[cnt] = cmd19->echonetLiteVer;
	cnt = cnt + EINV_CMD19_ECHONETLITEVER_LEN;

	/* OPリモコン有無 */
	reqData[cnt] = cmd19->opRemoteControlOrnot;
	cnt = cnt + EINV_CMD19_OPREMOTECONTROLORNOT_LEN;

	/* 蓄電池最大充電電力設定 */
	snprintf(buf, sizeof(buf), "%04u",cmd19->btCharPwrMaxValueSet);
	memcpy(&reqData[cnt], buf, EINV_CMD19_BTCHARPWRMAXVALUESET_LEN);
	cnt = cnt + EINV_CMD19_BTCHARPWRMAXVALUESET_LEN;

	/* 車両最大充電電力設定 */
	snprintf(buf, sizeof(buf), "%04u",cmd19->carCharPwrValueSet);
	memcpy(&reqData[cnt], buf, EINV_CMD19_CARCHARPWRVALUESET_LEN);
	cnt = cnt + EINV_CMD19_CARCHARPWRVALUESET_LEN;

	/* 停電時出力設定 */
	reqData[cnt] = cmd19->pwrcutOutSet;
	cnt = cnt + EINV_CMD19_PWRCUTOUTSET_LEN;

	/* IPアドレス設定 */
	reqData[cnt] = cmd19->ipAddrSet;
	cnt = cnt + EINV_CMD19_IPADDRSET_LEN;

	/* IPアドレス */
	einv_ParseIpAddress(cmd19->ipAddress, &reqData[cnt]);
	cnt = cnt + EINV_CMD19_IPADDRESS_LEN;

	/* サブネットマスク */
	einv_ParseIpAddress(cmd19->subnetMask, &reqData[cnt]);
	cnt = cnt + EINV_CMD19_SUBNETMASK_LEN;

	/* デフォルトゲートウェイ */
	einv_ParseIpAddress(cmd19->defaultGateway, &reqData[cnt]);
	cnt = cnt + EINV_CMD19_DEFAULTGATEWAY_LEN;

	/* 優先DNS */
	einv_ParseIpAddress(cmd19->poiorityDns, &reqData[cnt]);
	cnt = cnt + EINV_CMD19_POIORITYDNS_LEN;

	/* 代替DNS */
	einv_ParseIpAddress(cmd19->replaceDns, &reqData[cnt]);
	cnt = cnt + EINV_CMD19_REPLACEDNS_LEN;

	/* PROXY設定 */
	reqData[cnt] = cmd19->proxySet;
	cnt = cnt + EINV_CMD19_PROXYSET_LEN;

	/* PROXYアドレス */
	memcpy(&reqData[cnt], cmd19->proxyAddress, EINV_CMD19_PROXYADDRESS_LEN);
	cnt = cnt + EINV_CMD19_PROXYADDRESS_LEN;

	/* ポート番号 */
	memcpy(&reqData[cnt], cmd19->portNumber, EINV_CMD19_PORTNUMBER_LEN);
	cnt = cnt + EINV_CMD19_PORTNUMBER_LEN;

	/* 試運転_蓄電池充電 */
	reqData[cnt] = cmd19->testrunBtChar;
	cnt = cnt + EINV_CMD19_TESTRUNBTCHAR_LEN;

	/* 試運転_蓄電池放電 */
	reqData[cnt] = cmd19->testrunBtDisc;
	cnt = cnt + EINV_CMD19_TESTRUNBTDISC_LEN;

	/* 試運転_買電(E1-ACCT) */
	reqData[cnt] = cmd19->testrunBuyE1;
	cnt = cnt + EINV_CMD19_TESTRUNBUYE1_LEN;

	/* 試運転_買電(C2-ACCT) */
	reqData[cnt] = cmd19->testrunBuyC2;
	cnt = cnt + EINV_CMD19_TESTRUNBUYC2_LEN;

	/* 試運転_車両充電 */
	reqData[cnt] = cmd19->testrunCarChar;
	cnt = cnt + EINV_CMD19_TESTRUNCARCHAR_LEN;

	/* 試運転_車両放電 */
	reqData[cnt] = cmd19->testrunCarDisc;
	cnt = cnt + EINV_CMD19_TESTRUNCARDISC_LEN;

	/* 試運転_内部太陽光発電の動作 */
	reqData[cnt] = cmd19->testrunInterSunPwrGerat;
	cnt = cnt + EINV_CMD19_TESTRUNINTERSUNPWRGERAT_LEN;

	/* 試運転_外付け太陽光発電の動作 */
	reqData[cnt] = cmd19->testrunExternSunPwrGerat;
	cnt = cnt + EINV_CMD19_TESTRUNEXTERNSUNPWRGERAT_LEN;

	/* 試運転_切り替えモジュール動作(停電) */
	reqData[cnt] = cmd19->testrunSwitchPwrCutMode;
	cnt = cnt + EINV_CMD19_TESTRUNSWITCHPWRCUTMODE_LEN;

	/* 試運転_停電時E1放電 */
	reqData[cnt] = cmd19->testrunPutCutE1Disc;
	cnt = cnt + EINV_CMD19_TESTRUNPUTCUTE1DISC_LEN;

	/* 試運転_切り替えモジュール動作(復帰) */
	reqData[cnt] = cmd19->testrunSwitchResetMode;
	cnt = cnt + EINV_CMD19_TESTRUNSWITCHRESETMODE_LEN;

	/* 予約 */
	snprintf(buf, sizeof(buf), "% 2u",cmd19->reserve);
	memcpy(&reqData[cnt], buf, EINV_CMD19_RESERVE_LEN);
	cnt = cnt + EINV_CMD19_RESERVE_LEN;

	/* クリップ動作 */
	reqData[cnt] = cmd19->clipAction;
	cnt = cnt + EINV_CMD19_CLIPACTION_LEN;

	/* メンテ期間開始日(年) */
	memcpy(&reqData[cnt], &cmd19->mentanceDate.year, EINV_CMD19_MENTANCEDATEYEAR_LEN);
	cnt = cnt + EINV_CMD19_MENTANCEDATEYEAR_LEN;

	/* メンテ期間開始日(月) */
	memcpy(&reqData[cnt], &cmd19->mentanceDate.month, EINV_CMD19_MENTANCEDATEMONTH_LEN);
	cnt = cnt + EINV_CMD19_MENTANCEDATEMONTH_LEN;

	/* メンテ期間開始日(日) */
	memcpy(&reqData[cnt], &cmd19->mentanceDate.day, EINV_CMD19_MENTANCEDATEDAY_LEN);
	cnt = cnt + EINV_CMD19_MENTANCEDATEDAY_LEN;

	/* 実施指定日1(年) */
	memcpy(&reqData[cnt], &cmd19->impltationDesignDay1.year, EINV_CMD19_IMPLTATIONDESIGNDAY1YEAR_LEN);
	cnt = cnt + EINV_CMD19_IMPLTATIONDESIGNDAY1YEAR_LEN;

	/* 実施指定日1(月) */
	memcpy(&reqData[cnt], &cmd19->impltationDesignDay1.month, EINV_CMD19_IMPLTATIONDESIGNDAY1MONTH_LEN);
	cnt = cnt + EINV_CMD19_IMPLTATIONDESIGNDAY1MONTH_LEN;

	/* 実施指定日1(日) */
	memcpy(&reqData[cnt], &cmd19->impltationDesignDay1.day, EINV_CMD19_IMPLTATIONDESIGNDAY1DAY_LEN);
	cnt = cnt + EINV_CMD19_IMPLTATIONDESIGNDAY1DAY_LEN;

	/* 実施指定日2(年) */
	memcpy(&reqData[cnt], &cmd19->impltationDesignDay2.year, EINV_CMD19_IMPLTATIONDESIGNDAY2YEAR_LEN);
	cnt = cnt + EINV_CMD19_IMPLTATIONDESIGNDAY2YEAR_LEN;

	/* 実施指定日2(月) */
	memcpy(&reqData[cnt], &cmd19->impltationDesignDay2.month, EINV_CMD19_IMPLTATIONDESIGNDAY2MONTH_LEN);
	cnt = cnt + EINV_CMD19_IMPLTATIONDESIGNDAY2MONTH_LEN;
	
	/* 実施指定日2(日) */
	memcpy(&reqData[cnt], &cmd19->impltationDesignDay2.day, EINV_CMD19_IMPLTATIONDESIGNDAY2DAY_LEN);
	cnt = cnt + EINV_CMD19_IMPLTATIONDESIGNDAY2DAY_LEN;

	/* 実行モード */
	reqData[cnt] = cmd19->executeMode;
	cnt = cnt + EINV_CMD19_EXECUTEMODE_LEN;
	
	/* メンテ結果 */
	reqData[cnt] = cmd19->maintainResult;

	DEBUG_TRCOUT( );
}


/*==========================================================================*/
/*	Description		:解析CMD19データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:resData	:応答データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:解析CMD19データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/10 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_ParseCmd19Data(const uchar *resData)
{
	DEBUG_TRCIN( );

	memset(&g_einvCmd19_r, 0, sizeof(g_einvCmd19_r));

	g_einvCmd19_r.response = (schar)einv_Strtoll(&resData[EINV_CMD19_RES_RESPONSE], EINV_CMD19_RESPONSE_LEN, EINV_CMD_RES_BASE_DEC);		/* ACK/NAK */

	DLOG_Debug("g_einvCmd19_r.response = %d", g_einvCmd19_r.response);

	DEBUG_TRCOUT( );
}


/*==========================================================================*/
/*	Description		:解析CMD74データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:resData	:応答データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:解析CMD74データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/03 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_ParseCmd74Data(const uchar *resData)
{
	DEBUG_TRCIN( );
	
	memset(&g_einvCmd74_r, 0, sizeof(g_einvCmd74_r));

	g_einvCmd74_r.carOpertPriority			= resData[EINV_CMD74_RES_CAROPERTPRIORITY			];																							/* 車両操作優先 */
	g_einvCmd74_r.carCharordiscBodyOprat	= resData[EINV_CMD74_RES_CARCHARORDISCBODYOPRAT		];																							/* 車両充放電器本体操作 */
	g_einvCmd74_r.pwrcutDischar				= resData[EINV_CMD74_RES_PWRCUTDISCHAR				];																							/* 停電時放電動作 */
	g_einvCmd74_r.pwrcutDiscPrioritySet		= resData[EINV_CMD74_RES_PWRCUTDISCPRIORITYSET		];																							/* 停電時放電優先設定 */
	g_einvCmd74_r.pwrcutCharPrioritySet		= resData[EINV_CMD74_RES_PWRCUTCHARPRIORITYSET		];																							/* 停電時充電優先設定 */
	g_einvCmd74_r.connectLockOrUnlock		= resData[EINV_CMD74_RES_CONNECTLOCKORUNLOCK		];																							/* コネクタロック／解除 */
	g_einvCmd74_r.carDiscLowerLimitSet		= (ushort )einv_Strtoul(&resData[EINV_CMD74_RES_CARDISCLOWERLIMITSET	], EINV_CMD74_CARDISCLOWERLIMITSET_LEN	, EINV_CMD_RES_BASE_DEC);		/* 車両放電下限値設定 */
	g_einvCmd74_r.carCharUpperLimitSet		= (ushort )einv_Strtoul(&resData[EINV_CMD74_RES_CARCHARUPPERLIMITSET	], EINV_CMD74_CARCHARUPPERLIMITSET_LEN	, EINV_CMD_RES_BASE_DEC);		/* 車両充電上限値設定 */
	g_einvCmd74_r.carDiscOrnot				= resData[EINV_CMD74_RES_CARDISCORNOT				];																							/* 車両放電有無 */
	g_einvCmd74_r.discharPrioritySet		= resData[EINV_CMD74_RES_DISCHARPRIORITYSET			];																							/* 放電優先設定 */
	g_einvCmd74_r.pwrcutDiscLowerLimitSet	= (ushort)einv_Strtoul(&resData[EINV_CMD74_RES_PWRCUTDISCLOWERLIMITSET	], EINV_CMD74_PWRCUTDISCLOWERLIMITSET_LEN	, EINV_CMD_RES_BASE_DEC);	/* 停電時車両放電下限値設定 */
	g_einvCmd74_r.chargePrioritySet			= resData[EINV_CMD74_RES_CHARGEPRIORITYSET			];																							/* 充電優先設定 */
	g_einvCmd74_r.carCharPwrValueSet		= (ushort)einv_Strtoul(&resData[EINV_CMD74_RES_BTCHARPWRVALUESET		], EINV_CMD74_BTCHARPWRVALUESET_LEN	, EINV_CMD_RES_BASE_HEX		);		/* 車載電池充電電力設定値 */
	g_einvCmd74_r.carDischarPwrValueSet		= (ushort)einv_Strtoul(&resData[EINV_CMD74_RES_BTDISCHARPWRVALUESET		], EINV_CMD74_BTDISCHARPWRVALUESET_LEN	, EINV_CMD_RES_BASE_HEX	);		/* 車載電池放電電力設定値 */
		
	DLOG_Debug("g_einvCmd74_r.carOpertPriority = %d\n",				g_einvCmd74_r.carOpertPriority);
	DLOG_Debug("g_einvCmd74_r.carCharordiscBodyOprat = %d\n",		g_einvCmd74_r.carCharordiscBodyOprat);
	DLOG_Debug("g_einvCmd74_r.pwrcutDischar = %d\n",				g_einvCmd74_r.pwrcutDischar);
	DLOG_Debug("g_einvCmd74_r.pwrcutDiscPrioritySet = %d\n",		g_einvCmd74_r.pwrcutDiscPrioritySet);
	DLOG_Debug("g_einvCmd74_r.pwrcutCharPrioritySet = %d\n",		g_einvCmd74_r.pwrcutCharPrioritySet);
	DLOG_Debug("g_einvCmd74_r.connectLockOrUnlock = %d\n",			g_einvCmd74_r.connectLockOrUnlock);
	DLOG_Debug("g_einvCmd74_r.carDiscLowerLimitSet = %d\n",			g_einvCmd74_r.carDiscLowerLimitSet);
	DLOG_Debug("g_einvCmd74_r.carCharUpperLimitSet = %d\n",			g_einvCmd74_r.carCharUpperLimitSet);
	DLOG_Debug("g_einvCmd74_r.carDiscOrnot = %d\n",					g_einvCmd74_r.carDiscOrnot);
	DLOG_Debug("g_einvCmd74_r.discharPrioritySet = %d\n",			g_einvCmd74_r.discharPrioritySet);
	DLOG_Debug("g_einvCmd74_r.pwrcutDiscLowerLimitSet = %d\n",		g_einvCmd74_r.pwrcutDiscLowerLimitSet);
	DLOG_Debug("g_einvCmd74_r.chargePrioritySet = %d\n",			g_einvCmd74_r.chargePrioritySet);
	DLOG_Debug("g_einvCmd74_r.carCharPwrValueSet = %d\n",			g_einvCmd74_r.carCharPwrValueSet);
	DLOG_Debug("g_einvCmd74_r.carDischarPwrValueSet = %d\n",		g_einvCmd74_r.carDischarPwrValueSet);

	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:作成CMD75データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:cmdData	:cmdデータ									*/
/*					:reqSize	:要求サイズ									*/
/*					:reqData	:要求データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:作成CMD75データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/03 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_MakeCmd75Data(EinvCmdDataT_t *cmdData, slong reqSize, uchar *reqData)
{
	EinvCmd75T_t *cmd75 = &cmdData->cmd75_t;
	schar buf[EINV_LONG_BUF_SIZE] = { 0 };
	ulong cnt = 0UL;

	DEBUG_TRCIN( );

	memset(reqData, EINV_CODE_NO_CHANGE, reqSize);
	memset(buf   , 0                  , sizeof(buf));

	/* 車両操作優先 */
	if((cmd75->changeFlag & EINV_SET_CMD75_CAR_OPERT_PRIORITY) == EINV_SET_CMD75_CAR_OPERT_PRIORITY)
	{
		reqData[cnt] = cmd75->carOpertPriority;
	} 
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD75_CAROPERTPRIORITY_LEN;

	/* 車両充放電器本体操作 */
	if((cmd75->changeFlag & EINV_SET_CMD75_CAR_CHARORDISC_BODY_OPRAT) == EINV_SET_CMD75_CAR_CHARORDISC_BODY_OPRAT)
	{
		reqData[cnt] = cmd75->carCharordiscBodyOprat;
	} 
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD75_CARCHARORDISCBODYOPRAT_LEN;

	/* 停電時放電動作 */
	if((cmd75->changeFlag & EINV_SET_CMD75_PWRCUT_DISCHAR) == EINV_SET_CMD75_PWRCUT_DISCHAR)
	{
		reqData[cnt] = cmd75->pwrcutDischar;
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD75_PWRCUTDISCHAR_LEN;

	/* 停電時放電優先設定 */
	if((cmd75->changeFlag & EINV_SET_CMD75_PWRCUT_DISC_PRIORITY_SET) == EINV_SET_CMD75_PWRCUT_DISC_PRIORITY_SET)
	{
		reqData[cnt] = cmd75->pwrcutDiscPrioritySet;
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD75_PWRCUTDISCPRIORITYSET_LEN;

	/* 停電時充電優先設定 */
	if((cmd75->changeFlag & EINV_SET_CMD75_PWRCUT_CHAR_PRIORITY_SET) == EINV_SET_CMD75_PWRCUT_CHAR_PRIORITY_SET)
	{
		reqData[cnt] = cmd75->pwrcutCharPrioritySet;
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD75_PWRCUTCHARPRIORITYSET_LEN;

	/* コネクタロック／解除 */
	if((cmd75->changeFlag & EINV_SET_CMD75_CONNECT_LOCK_OR_UNLOCK) == EINV_SET_CMD75_CONNECT_LOCK_OR_UNLOCK)
	{
		reqData[cnt] = cmd75->connectLockOrUnlock;
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD75_CONNECTLOCKORUNLOCK_LEN;

	/* 車両放電下限値設定 */
	if((cmd75->changeFlag & EINV_SET_CMD75_CAR_DISC_LOWERLIMIT_SET) == EINV_SET_CMD75_CAR_DISC_LOWERLIMIT_SET)
	{
		snprintf(buf, sizeof(buf), "%03u",cmd75->carDiscLowerLimitSet);
		memcpy(&reqData[cnt], buf, EINV_CMD75_CARDISCLOWERLIMITSET_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD75_CARDISCLOWERLIMITSET_LEN;

	/* 車両充電上限値設定 */
	if((cmd75->changeFlag & EINV_SET_CMD75_CAR_CHAR_UPPERLIMIT_SET) == EINV_SET_CMD75_CAR_CHAR_UPPERLIMIT_SET)
	{
		snprintf(buf, sizeof(buf), "%03u",cmd75->carCharUpperLimitSet);
		memcpy(&reqData[cnt], buf, EINV_CMD75_CARCHARUPPERLIMITSET_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD75_CARCHARUPPERLIMITSET_LEN;

	/* 車両放電有無 */
	if((cmd75->changeFlag & EINV_SET_CMD75_CAR_DISC_ORNOT) == EINV_SET_CMD75_CAR_DISC_ORNOT)
	{
		reqData[cnt] = cmd75->carDiscOrnot;
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD75_CARDISCORNOT_LEN;

	/* 放電優先設定 */
	if((cmd75->changeFlag & EINV_SET_CMD75_DISCHAR_PRIORITY_SET) == EINV_SET_CMD75_DISCHAR_PRIORITY_SET)
	{
		reqData[cnt] = cmd75->discharPrioritySet;
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD75_DISCHARPRIORITYSET_LEN;

	/* 停電時車両放電下限値設定 */
	if((cmd75->changeFlag & EINV_SET_CMD75_PWRCUT_DISC_LOWERLIMIT_SET) == EINV_SET_CMD75_PWRCUT_DISC_LOWERLIMIT_SET)
	{
		snprintf(buf, sizeof(buf), "%03u",cmd75->pwrcutCarDiscLowLmtSet);
		memcpy(&reqData[cnt], buf, EINV_CMD75_PWRCUTDISCLOWERLIMITSET_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD75_PWRCUTDISCLOWERLIMITSET_LEN;

	/* 充電優先設定 */
	if((cmd75->changeFlag & EINV_SET_CMD75_CHARGE_PRIORITY_SET) == EINV_SET_CMD75_CHARGE_PRIORITY_SET)
	{
		reqData[cnt] = cmd75->chargePrioritySet;
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD75_CHARGEPRIORITYSET_LEN;

	/* 車載電池充電電力設定値 */
	if((cmd75->changeFlag & EINV_SET_CMD75_BTCHAR_PWR_VALUE_SET) == EINV_SET_CMD75_BTCHAR_PWR_VALUE_SET)
	{
		snprintf(buf, sizeof(buf), "%04u",cmd75->carCharPwrValueSet);
		memcpy(&reqData[cnt], buf, EINV_CMD75_BTCHARPWRVALUESET_LEN);
	}
	else
	{
		/* 処理無し */
	}
	cnt = cnt + EINV_CMD75_BTCHARPWRVALUESET_LEN;

	/* 車載電池放電電力設定値 */
	if((cmd75->changeFlag & EINV_SET_CMD75_BTDISCHAR_PWR_VALUE_SET) == EINV_SET_CMD75_BTDISCHAR_PWR_VALUE_SET)
	{
		snprintf(buf, sizeof(buf), "%04u",cmd75->carDiscPwrValueSet);
		memcpy(&reqData[cnt], buf, EINV_CMD75_BTDISCHARPWRVALUESET_LEN);
	}
	else
	{
		/* 処理無し */
	}
	
	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:解析CMD75データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:resData	:応答データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:解析CMD75データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/03 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_ParseCmd75Data(const uchar *resData)
{
	DEBUG_TRCIN( );

	memset(&g_einvCmd75_r, 0, sizeof(g_einvCmd75_r));

	g_einvCmd75_r.response = (schar)einv_Strtoll(&resData[EINV_CMD75_RES_RESPONSE], EINV_CMD75_RESPONSE_LEN, EINV_CMD_RES_BASE_DEC);		/* ACK/NAK */

	DLOG_Debug("g_einvCmd75_r.response = %d", g_einvCmd75_r.response);

	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:解析CMD81データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:resData	:応答データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:解析CMD81データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/06 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_ParseCmd81Data(const uchar *resData)
{
	DEBUG_TRCIN( );

	memset(&g_einvCmd81_r, 0, sizeof(g_einvCmd81_r));

	g_einvCmd81_r.chargeDischargePwrAC			= (sshort)einv_Strtoll(&resData[EINV_CMD81_RES_CHARGEDISCHARGEPWRAC		], EINV_CMD81_CHARGEDISCHARGEPWRAC_LEN		, EINV_CMD_RES_BASE_HEX);		/* PCS充放電電力（AC端計測値） */
	g_einvCmd81_r.opeState						= resData[EINV_CMD81_RES_OPESTATE	];																												/* PCS運転状態 */
	g_einvCmd81_r.ttlChargeEnergySysAC			= (ulong)einv_Strtoul(&resData[EINV_CMD81_RES_TTLCHARGEENERGYSYSAC		], EINV_CMD81_TTLCHARGEENERGYSYSAC_LEN		, EINV_CMD_RES_BASE_HEX);		/* 積算PCS充電電力量(AC端計測値)(系統連系時) */
	g_einvCmd81_r.ttlDischargeEnergySysAC		= (ulong)einv_Strtoul(&resData[EINV_CMD81_RES_TTLDISCHARGEENERGYSYSAC	], EINV_CMD81_TTLDISCHARGEENERGYSYSAC_LEN	, EINV_CMD_RES_BASE_HEX);		/* 積算PCS放電電力量(AC端計測値)(系統連系時) */
	g_einvCmd81_r.rsoc							= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_RSOC						], EINV_CMD81_RSOC_LEN		, EINV_CMD_RES_BASE_DEC); 						/* rSOC */
	g_einvCmd81_r.remainingBt					= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_REMAININGBT				], EINV_CMD81_REMAININGBT_LEN		, EINV_CMD_RES_BASE_DEC);				/* 電池残量 */
	g_einvCmd81_r.btVolt						= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_BTVOLT					], EINV_CMD81_BTVOLT_LEN	, EINV_CMD_RES_BASE_DEC);						/* 電池電圧*/
	g_einvCmd81_r.soh							= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_SOH						], EINV_CMD81_SOH_LEN		, EINV_CMD_RES_BASE_DEC);						/* SOH */
	g_einvCmd81_r.btOperation					= resData[EINV_CMD81_RES_BTOPERATION];																												/* 蓄電池運転動作状態 */
	g_einvCmd81_r.pwrCut						= resData[EINV_CMD81_RES_PWRCUT		];																												/* 停電 */
	g_einvCmd81_r.pvGenPwr						= (sshort)einv_Strtoll(&resData[EINV_CMD81_RES_PVGENPWR					], EINV_CMD81_PVGENPWR_LEN		, EINV_CMD_RES_BASE_HEX);					/* PV発電電力(本製品以外のPV) */
	g_einvCmd81_r.revPwrFlow					= (sshort)einv_Strtoll(&resData[EINV_CMD81_RES_REVPWRFLOW				], EINV_CMD81_REVPWRFLOW_LEN	, EINV_CMD_RES_BASE_HEX);					/* 逆潮流電力 */
	g_einvCmd81_r.sysVolt						= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_SYSVOLT					], EINV_CMD81_SYSVOLT_LEN		, EINV_CMD_RES_BASE_DEC);					/* 系統電圧 */
	memcpy(&g_einvCmd81_r.pcsErr, &resData[EINV_CMD81_RES_PCSERR], EINV_CMD81_PCSERR_LEN);																											/* PCSエラー */
	memcpy(&g_einvCmd81_r.pcsErrRating, &resData[EINV_CMD81_RES_PCSERRRATING], EINV_CMD81_PCSERRRATING_LEN);																						/* PCSエラーレーティング */
	g_einvCmd81_r.opeMode						= resData[EINV_CMD81_RES_OPEMODE];																													/* 運転モード */
	g_einvCmd81_r.chargeDischargePwrDC			= (sshort)einv_Strtoll(&resData[EINV_CMD81_RES_CHARGEDISCHARGEPWRDC		], EINV_CMD81_CHARGEDISCHARGEPWRDC_LEN	, EINV_CMD_RES_BASE_HEX);			/* DC蓄電池充放電電力2(コンバータ部) */
	g_einvCmd81_r.ttlChargeEnergyDC				= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_TTLCHARGEENERGYDC		], EINV_CMD81_TTLCHARGEENERGYDC_LEN		, EINV_CMD_RES_BASE_HEX);			/* DC積算蓄電池充電電力量2(コンバータ部) */
	g_einvCmd81_r.ttlDischargeEnergyDC			= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_TTLDISCHARGEENERGYDC		], EINV_CMD81_TTLDISCHARGEENERGYDC_LEN	, EINV_CMD_RES_BASE_HEX);			/* DC積算蓄電池放電電力量2(コンバータ部) */
	g_einvCmd81_r.ttlPvGenPwr					= (ulong)einv_Strtoul(&resData[EINV_CMD81_RES_TTLPVGENPWR				], EINV_CMD81_TTLPVGENPWR_LEN		, EINV_CMD_RES_BASE_HEX);				/* 積算PV発電電力量(本製品以外のPV) */
	g_einvCmd81_r.ttlPurchasePwr				= (ulong)einv_Strtoul(&resData[EINV_CMD81_RES_TTLPURCHASEPWR			], EINV_CMD81_TTLPURCHASEPWR_LEN	, EINV_CMD_RES_BASE_HEX);				/* 積算買電電力量 */
	g_einvCmd81_r.ttlSellingPwr					= (ulong)einv_Strtoul(&resData[EINV_CMD81_RES_TTLSELLINGPWR				], EINV_CMD81_TTLSELLINGPWR_LEN		, EINV_CMD_RES_BASE_HEX);				/* 積算売電電力量 */
	g_einvCmd81_r.genPwrLimit					= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_GENPWRLIMIT				], EINV_CMD81_GENPWRLIMIT_LEN		, EINV_CMD_RES_BASE_DEC);				/* 発電電力制限値(kW) */
	g_einvCmd81_r.suppressState					= resData[EINV_CMD81_RES_SUPPRESSSTATE];																											/* 出力抑制状態 */
	g_einvCmd81_r.carConnectorLockState			= resData[EINV_CMD81_RES_CARCONNECTORLOCKSTATE];																									/* コネクタロック状態 */
	g_einvCmd81_r.carCharDiscOpratState			= resData[EINV_CMD81_RES_CARCHARDISCOPRATSTATE];																									/* 車両充放電器運転動作状態 */
	g_einvCmd81_r.carDischargeableRemainingCapacity1		= (ulong)einv_Strtoul(&resData[EINV_CMD81_RES_CARDISCHARGEABLEREMAININGCAPACITY1	], EINV_CMD81_CARDISCHARGEABLEREMAININGCAPACITY1_LEN	, EINV_CMD_RES_BASE_HEX);	/* 車載電池の放電可能残容量1 */
	g_einvCmd81_r.carDischargeableRemainingCapacity3		= (uchar)einv_Strtoul(&resData[EINV_CMD81_RES_CARDISCHARGEABLEREMAININGCAPACITY3	], EINV_CMD81_CARDISCHARGEABLEREMAININGCAPACITY3_LEN	, EINV_CMD_RES_BASE_HEX);	/* 車載電池の放電可能残容量3 */
	g_einvCmd81_r.carChargeableRemainCap		= (ulong)einv_Strtoul(&resData[EINV_CMD81_RES_CARCHARGEABLEREMAINCAP	], EINV_CMD81_CARCHARGEABLEREMAINCAP_LEN	, EINV_CMD_RES_BASE_HEX);		/* 車載電池の充電可能残容量値 */
	g_einvCmd81_r.carAvailabilityStatus			= (uchar)einv_Strtoul(&resData[EINV_CMD81_RES_CARAVAILABILITYSTATUS		], EINV_CMD81_CARAVAILABILITYSTATUS_LEN	, EINV_CMD_RES_BASE_HEX);			/* 車両接続充放電可否状態 */
	g_einvCmd81_r.v2hInstantPower				= (slong)einv_Strtoll(&resData[EINV_CMD81_RES_V2HINSTANTPOWER			], EINV_CMD81_V2HINSTANTPOWER_LEN		, EINV_CMD_RES_BASE_HEX);			/* V2H瞬時充放電電力計測値(AC端計測値) */
	g_einvCmd81_r.carTtlDischargePower			= (ulong)einv_Strtoul(&resData[EINV_CMD81_RES_CARTTLDISCHARGEPOWER		], EINV_CMD81_CARTTLDISCHARGEPOWER_LEN	, EINV_CMD_RES_BASE_HEX);			/* 車載電池の積算放電電力量計測値(AC端計測値)(トータル) */
	g_einvCmd81_r.carTtlChargePower				= (ulong)einv_Strtoul(&resData[EINV_CMD81_RES_CARTTLCHARGEPOWER			], EINV_CMD81_CARTTLCHARGEPOWER_LEN		, EINV_CMD_RES_BASE_HEX);			/* 車載電池の積算充電電力量計測値(AC端計測値)(トータル) */
	g_einvCmd81_r.emergencyMode					= resData[EINV_CMD81_RES_EMERGENCYMODE];																											/* 非常時運転モード */
	g_einvCmd81_r.carRemainingCapacity1			= (ulong)einv_Strtoul(&resData[EINV_CMD81_RES_CARREMAININGCAPACITY1		], EINV_CMD81_CARREMAININGCAPACITY1_LEN	, EINV_CMD_RES_BASE_HEX);			/* 車載電池の電池残容量1 */
	g_einvCmd81_r.carRemainingCapacity3			= (uchar)einv_Strtoul(&resData[EINV_CMD81_RES_CARREMAININGCAPACITY3		], EINV_CMD81_CARREMAININGCAPACITY3_LEN	, EINV_CMD_RES_BASE_HEX);			/* 車載電池の電池残容量3 */
	g_einvCmd81_r.ttlChargeEnergyAloneAC		= (ulong)einv_Strtoul(&resData[EINV_CMD81_RES_TTLCHARGEENERGYALONEAC	], EINV_CMD81_TTLCHARGEENERGYALONEAC_LEN	, EINV_CMD_RES_BASE_HEX);		/* 積算PCS充電電力量(AC端計測値)(自立時) */
	g_einvCmd81_r.ttlDischargeEnergyAloneAC		= (ulong)einv_Strtoul(&resData[EINV_CMD81_RES_TTLDISCHARGEENERGYALONEAC	], EINV_CMD81_TTLDISCHARGEENERGYALONEAC_LEN	, EINV_CMD_RES_BASE_HEX);		/* 積算PCS放電電力量(AC端計測値)(自立時) */
	g_einvCmd81_r.pcsEfficiencyChar				= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_PCSEFFICIENCYCHAR		], EINV_CMD81_PCSEFFICIENCYCHAR_LEN		, EINV_CMD_RES_BASE_DEC);			/* PCS効率(充電方向) */
	g_einvCmd81_r.pcsEfficiencyDischar			= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_PCSEFFICIENCYDISCHAR		], EINV_CMD81_PCSEFFICIENCYDISCHAR_LEN	, EINV_CMD_RES_BASE_DEC);			/* PCS効率(放電方向) */
	g_einvCmd81_r.btCharDischarPwr				= (slong)einv_Strtoll(&resData[EINV_CMD81_RES_BTCHARDISCHARPWR			], EINV_CMD81_BTCHARDISCHARPWR_LEN		, EINV_CMD_RES_BASE_HEX);			/* 蓄電池充放電電力(AC端計測値) */
	g_einvCmd81_r.pvPwr2						= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_PVPWR2					], EINV_CMD81_PVPWR2_LEN	, EINV_CMD_RES_BASE_HEX);						/* 太陽光発電電力(AC端計測値) */
	g_einvCmd81_r.ttlPvGenPwr2					= (ulong)einv_Strtoul(&resData[EINV_CMD81_RES_TTLPVGENPWR2				], EINV_CMD81_TTLPVGENPWR2_LEN		, EINV_CMD_RES_BASE_HEX);				/* 積算太陽光発電電力量(AC端計測値) */
	g_einvCmd81_r.cellVoltMIN					= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_CELLVOLTMIN				], EINV_CMD81_CELLVOLTMIN_LEN		, EINV_CMD_RES_BASE_HEX);				/* 最低セル電圧 */
	g_einvCmd81_r.cellAverageVolt				= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_CELLAVERAGEVOLT			], EINV_CMD81_CELLAVERAGEVOLT_LEN	, EINV_CMD_RES_BASE_HEX);				/* 平均セル電圧 */
	g_einvCmd81_r.cellVoltMAX					= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_CELLVOLTMAX				], EINV_CMD81_CELLVOLTMAX_LEN	, EINV_CMD_RES_BASE_HEX);					/* 最高セル電圧 */
	g_einvCmd81_r.cellTempeMIN					= (sshort)einv_Strtoll(&resData[EINV_CMD81_RES_CELLTEMPEMIN				], EINV_CMD81_CELLTEMPEMIN_LEN	, EINV_CMD_RES_BASE_HEX);					/* 最低セル温度 */
	g_einvCmd81_r.cellTempeMAX					= (sshort)einv_Strtoll(&resData[EINV_CMD81_RES_CELLTEMPEMAX				], EINV_CMD81_CELLTEMPEMAX_LEN	, EINV_CMD_RES_BASE_HEX);					/* 最高セル温度 */
	g_einvCmd81_r.pvStringVolt1					= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_PVSTRINGVOLT1			], EINV_CMD81_PVSTRINGVOLT1_LEN	, EINV_CMD_RES_BASE_DEC);					/* 太陽光発電ストリング電圧(1) */
	g_einvCmd81_r.pvStringVolt2					= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_PVSTRINGVOLT2			], EINV_CMD81_PVSTRINGVOLT2_LEN	, EINV_CMD_RES_BASE_DEC);					/* 太陽光発電ストリング電圧(2) */
	g_einvCmd81_r.pvStringVolt3					= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_PVSTRINGVOLT3			], EINV_CMD81_PVSTRINGVOLT3_LEN	, EINV_CMD_RES_BASE_DEC);					/* 太陽光発電ストリング電圧(3) */
	g_einvCmd81_r.pvStringVolt4					= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_PVSTRINGVOLT4			], EINV_CMD81_PVSTRINGVOLT4_LEN	, EINV_CMD_RES_BASE_DEC);					/* 太陽光発電ストリング電圧(4) */
	g_einvCmd81_r.pvStringVolt5					= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_PVSTRINGVOLT5			], EINV_CMD81_PVSTRINGVOLT5_LEN	, EINV_CMD_RES_BASE_DEC);					/* 太陽光発電ストリング電圧(5) */
	g_einvCmd81_r.testRunoutinfrom				= resData[EINV_CMD81_RES_TESTRUNOUTINFROM];																											/* 試運転状態通知 */
	g_einvCmd81_r.v2hOperationButton1			= (uchar)einv_Strtoul(&resData[EINV_CMD81_RES_V2HOPERATIONBUTTON1		], EINV_CMD81_V2HOPERATIONBUTTON1_LEN	, EINV_CMD_RES_BASE_DEC);			/* V2H本体操作ボタン1 */
	g_einvCmd81_r.v2hOperationButton2			= resData[EINV_CMD81_RES_V2HOPERATIONBUTTON2];																										/* V2H本体操作ボタン2 */
	g_einvCmd81_r.v2hOperationButton3			= resData[EINV_CMD81_RES_V2HOPERATIONBUTTON3];																										/* V2H本体操作ボタン3 */
	g_einvCmd81_r.connectCarTypeOrManufacturer	= resData[EINV_CMD81_RES_CONNECTCARTYPEORMANUFACTURER];																								/* 接続中の車種またはメーカー(特定車両判定) */
	g_einvCmd81_r.notChargeFlag					= resData[EINV_CMD81_RES_NOTCHARGEFLAG];																											/* 充電不可フラグ */
	g_einvCmd81_r.notDisChargeFlag				= resData[EINV_CMD81_RES_NOTDISCHARGEFLAG];																											/* 放電不可フラグ */
	g_einvCmd81_r.revCtCurrentU					= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_REVCTCURRENTU			], EINV_CMD81_REVCTCURRENTU_LEN	, EINV_CMD_RES_BASE_HEX);					/* 逆潮CTの電流値(U相) */
	g_einvCmd81_r.revCtCurrentW					= (ushort)einv_Strtoul(&resData[EINV_CMD81_RES_REVCTCURRENTW			], EINV_CMD81_REVCTCURRENTW_LEN	, EINV_CMD_RES_BASE_HEX);					/* 逆潮CTの電流値(W相) */
	g_einvCmd81_r.carbtDiscCapacityValue1		= (ulong)einv_Strtoul(&resData[EINV_CMD81_RES_CARBTDISCCAPACITYVALUE1	], EINV_CMD81_CARBTDISCCAPACITYVALUE1_LEN	, EINV_CMD_RES_BASE_HEX);		/* 車載電池の放電可能容量値1 */
	g_einvCmd81_r.carbtUseCapacityValue1		= (ulong)einv_Strtoul(&resData[EINV_CMD81_RES_CARBTUSECAPACITYVALUE1	], EINV_CMD81_CARBTUSECAPACITYVALUE1_LEN	, EINV_CMD_RES_BASE_HEX);		/* 車載電池の使用容量値1 */
	g_einvCmd81_r.carbtCharCapacityValue		= (ulong)einv_Strtoul(&resData[EINV_CMD81_RES_CARBTCHARCAPACITYVALUE	], EINV_CMD81_CARBTCHARCAPACITYVALUE_LEN	, EINV_CMD_RES_BASE_HEX);		/* 車載電池の充電可能容量値 */
	g_einvCmd81_r.btCharCopltNotice				= resData[EINV_CMD81_RES_BTCHARCOPLTNOTICE];																										/* 蓄電池充電完了通知 */
	g_einvCmd81_r.btDischarCopltNotice			= resData[EINV_CMD81_RES_BTDISCHARCOPLTNOTICE];																										/* 蓄電池放電完了通知 */
	g_einvCmd81_r.maintenanceBtState			= resData[EINV_CMD81_RES_MAINTENANCEBTSTATE];																										/* メンテ電池状態 */


	if (debugTimes == 0U)
	{
		DLOG_Debug("g_einvCmd81_r.chargeDischargePwrAC = %d\n",					g_einvCmd81_r.chargeDischargePwrAC);
		DLOG_Debug("g_einvCmd81_r.opeState = %d\n",								g_einvCmd81_r.opeState);
		DLOG_Debug("g_einvCmd81_r.ttlChargeEnergySysAC = %ld\n",				g_einvCmd81_r.ttlChargeEnergySysAC);
		DLOG_Debug("g_einvCmd81_r.ttlDischargeEnergySysAC = %ld\n",				g_einvCmd81_r.ttlDischargeEnergySysAC);
		DLOG_Debug("g_einvCmd81_r.rsoc = %d\n",									g_einvCmd81_r.rsoc);
		DLOG_Debug("g_einvCmd81_r.remainingBt = %d\n",							g_einvCmd81_r.remainingBt);
		DLOG_Debug("g_einvCmd81_r.btVolt = %d\n",								g_einvCmd81_r.btVolt);
		DLOG_Debug("g_einvCmd81_r.soh = %d\n",									g_einvCmd81_r.soh);
		DLOG_Debug("g_einvCmd81_r.btOperation = %d\n",							g_einvCmd81_r.btOperation);
		DLOG_Debug("g_einvCmd81_r.pwrCut = %d\n",								g_einvCmd81_r.pwrCut);
		DLOG_Debug("g_einvCmd81_r.pvGenPwr = %d\n",								g_einvCmd81_r.pvGenPwr);
		DLOG_Debug("g_einvCmd81_r.revPwrFlow = %d\n",							g_einvCmd81_r.revPwrFlow);
		DLOG_Debug("g_einvCmd81_r.sysVolt = %d\n",								g_einvCmd81_r.sysVolt);
		DLOG_Debug("g_einvCmd81_r.pcsErr = %s\n",								g_einvCmd81_r.pcsErr);
		DLOG_Debug("g_einvCmd81_r.pcsErrRating = %s\n",							g_einvCmd81_r.pcsErrRating);
		DLOG_Debug("g_einvCmd81_r.opeMode = %d\n",								g_einvCmd81_r.opeMode);
		DLOG_Debug("g_einvCmd81_r.chargeDischargePwrDC = %d\n",					g_einvCmd81_r.chargeDischargePwrDC);
		DLOG_Debug("g_einvCmd81_r.ttlChargeEnergyDC = %d\n",					g_einvCmd81_r.ttlChargeEnergyDC);
		DLOG_Debug("g_einvCmd81_r.ttlDischargeEnergyDC = %d\n",					g_einvCmd81_r.ttlDischargeEnergyDC);
		DLOG_Debug("g_einvCmd81_r.ttlPvGenPwr = %ld\n",							g_einvCmd81_r.ttlPvGenPwr);
		DLOG_Debug("g_einvCmd81_r.ttlPurchasePwr = %ld\n",						g_einvCmd81_r.ttlPurchasePwr);
		DLOG_Debug("g_einvCmd81_r.ttlSellingPwr = %ld\n",						g_einvCmd81_r.ttlSellingPwr);
		DLOG_Debug("g_einvCmd81_r.genPwrLimit = %d\n",							g_einvCmd81_r.genPwrLimit);
		DLOG_Debug("g_einvCmd81_r.suppressState = %d\n",						g_einvCmd81_r.suppressState);
		DLOG_Debug("g_einvCmd81_r.carConnectorLockState = %d\n",				g_einvCmd81_r.carConnectorLockState);
		DLOG_Debug("g_einvCmd81_r.carCharDiscOpratState = %d\n",				g_einvCmd81_r.carCharDiscOpratState);
		DLOG_Debug("g_einvCmd81_r.carDischargeableRemainingCapacity1 = %ld\n",	g_einvCmd81_r.carDischargeableRemainingCapacity1);
		DLOG_Debug("g_einvCmd81_r.carDischargeableRemainingCapacity3 = %d\n",	g_einvCmd81_r.carDischargeableRemainingCapacity3);
		DLOG_Debug("g_einvCmd81_r.carChargeableRemainCap = %ld\n",				g_einvCmd81_r.carChargeableRemainCap);
		DLOG_Debug("g_einvCmd81_r.carAvailabilityStatus = %x\n",				g_einvCmd81_r.carAvailabilityStatus);
		DLOG_Debug("g_einvCmd81_r.v2hInstantPower = %ld\n",						g_einvCmd81_r.v2hInstantPower);
		DLOG_Debug("g_einvCmd81_r.carTtlDischargePower = %ld\n",				g_einvCmd81_r.carTtlDischargePower);
		DLOG_Debug("g_einvCmd81_r.carTtlChargePower = %ld\n",					g_einvCmd81_r.carTtlChargePower);
		DLOG_Debug("g_einvCmd81_r.emergencyMode = %d\n",						g_einvCmd81_r.emergencyMode);
		DLOG_Debug("g_einvCmd81_r.carRemainingCapacity1 = %ld\n",				g_einvCmd81_r.carRemainingCapacity1);
		DLOG_Debug("g_einvCmd81_r.carRemainingCapacity3 = %d\n",				g_einvCmd81_r.carRemainingCapacity3);
		DLOG_Debug("g_einvCmd81_r.ttlChargeEnergyAloneAC = %ld\n",				g_einvCmd81_r.ttlChargeEnergyAloneAC);
		DLOG_Debug("g_einvCmd81_r.ttlDischargeEnergyAloneAC = %ld\n",			g_einvCmd81_r.ttlDischargeEnergyAloneAC);
		DLOG_Debug("g_einvCmd81_r.pcsEfficiencyChar = %d\n",					g_einvCmd81_r.pcsEfficiencyChar);
		DLOG_Debug("g_einvCmd81_r.pcsEfficiencyDischar = %d\n",					g_einvCmd81_r.pcsEfficiencyDischar);
		DLOG_Debug("g_einvCmd81_r.btCharDischarPwr = %ld\n",					g_einvCmd81_r.btCharDischarPwr);
		DLOG_Debug("g_einvCmd81_r.pvPwr2 = %d\n",								g_einvCmd81_r.pvPwr2);
		DLOG_Debug("g_einvCmd81_r.ttlPvGenPwr2 = %ld\n",						g_einvCmd81_r.ttlPvGenPwr2);
		DLOG_Debug("g_einvCmd81_r.cellVoltMIN = %d\n",							g_einvCmd81_r.cellVoltMIN);
		DLOG_Debug("g_einvCmd81_r.cellAverageVolt = %d\n",						g_einvCmd81_r.cellAverageVolt);
		DLOG_Debug("g_einvCmd81_r.cellVoltMAX = %d\n",							g_einvCmd81_r.cellVoltMAX);
		DLOG_Debug("g_einvCmd81_r.cellTempeMIN = %d\n",							g_einvCmd81_r.cellTempeMIN);
		DLOG_Debug("g_einvCmd81_r.cellTempeMAX = %d\n",							g_einvCmd81_r.cellTempeMAX);
		DLOG_Debug("g_einvCmd81_r.pvStringVolt1 = %d\n",						g_einvCmd81_r.pvStringVolt1);
		DLOG_Debug("g_einvCmd81_r.pvStringVolt2 = %d\n",						g_einvCmd81_r.pvStringVolt2);
		DLOG_Debug("g_einvCmd81_r.pvStringVolt3 = %d\n",						g_einvCmd81_r.pvStringVolt3);
		DLOG_Debug("g_einvCmd81_r.pvStringVolt4 = %d\n",						g_einvCmd81_r.pvStringVolt4);
		DLOG_Debug("g_einvCmd81_r.pvStringVolt5 = %d\n",						g_einvCmd81_r.pvStringVolt5);
		DLOG_Debug("g_einvCmd81_r.testRunoutinfrom = %d\n",						g_einvCmd81_r.testRunoutinfrom);
		DLOG_Debug("g_einvCmd81_r.v2hOperationButton1 = %d\n",					g_einvCmd81_r.v2hOperationButton1);
		DLOG_Debug("g_einvCmd81_r.v2hOperationButton2 = %d\n",					g_einvCmd81_r.v2hOperationButton2);
		DLOG_Debug("g_einvCmd81_r.v2hOperationButton3 = %d\n",					g_einvCmd81_r.v2hOperationButton3);
		DLOG_Debug("g_einvCmd81_r.connectCarTypeOrManufacturer = %d\n",			g_einvCmd81_r.connectCarTypeOrManufacturer);
		DLOG_Debug("g_einvCmd81_r.notChargeFlag = %d\n",						g_einvCmd81_r.notChargeFlag);
		DLOG_Debug("g_einvCmd81_r.notDisChargeFlag = %d\n",						g_einvCmd81_r.notDisChargeFlag);
		DLOG_Debug("g_einvCmd81_r.revCtCurrentU = %d\n",						g_einvCmd81_r.revCtCurrentU);
		DLOG_Debug("g_einvCmd81_r.revCtCurrentW = %d\n",						g_einvCmd81_r.revCtCurrentW);
		DLOG_Debug("g_einvCmd81_r.carbtDiscCapacityValue1 = %ld\n",				g_einvCmd81_r.carbtDiscCapacityValue1);
		DLOG_Debug("g_einvCmd81_r.carbtUseCapacityValue1 = %ld\n",				g_einvCmd81_r.carbtUseCapacityValue1);
		DLOG_Debug("g_einvCmd81_r.carbtCharCapacityValue = %ld\n",				g_einvCmd81_r.carbtCharCapacityValue);
		DLOG_Debug("g_einvCmd81_r.btCharCopltNotice = %d\n",					g_einvCmd81_r.btCharCopltNotice);
		DLOG_Debug("g_einvCmd81_r.btDischarCopltNotice = %d\n",					g_einvCmd81_r.btDischarCopltNotice);
		DLOG_Debug("g_einvCmd81_r.maintenanceBtState = %d\n",					g_einvCmd81_r.maintenanceBtState);
	} 
	else if (debugTimes >= 60U)
	{
		debugTimes = 1U;
	}
	else
	{
		/* 処理無し */
	}
	debugTimes++;

	/* PV発電電力(本製品以外のPV) */
	if(g_einvCmd81_r.pvGenPwr < 100)
	{		
		memset(g_arrayPvGenPwr,	0, sizeof(g_arrayPvGenPwr));
		g_indexPvGenPwr = 0;
	}
	else
	{
		g_arrayPvGenPwr[g_indexPvGenPwr] = g_einvCmd81_r.pvGenPwr;
		g_indexPvGenPwr++;

		if(g_indexPvGenPwr > 9)
		{
			g_indexPvGenPwr = 0;
		}
	}

	/* 太陽光発電電力(AC端計測値) */
	if(g_einvCmd81_r.pvPwr2 < 100)
	{		
		memset(g_arrayPvPwr2,	0, sizeof(g_arrayPvPwr2));
		g_indexPvPwr2 = 0;
	}
	else
	{
		g_arrayPvPwr2[g_indexPvPwr2] = g_einvCmd81_r.pvPwr2;
		g_indexPvPwr2++;

		if(g_indexPvPwr2 > 9)
		{
			g_indexPvPwr2 = 0;
		}
	}
	
	/* 太陽光+PV発電電力平均値sum */
	if((g_einvCmd81_r.pvPwr2 + g_einvCmd81_r.pvGenPwr) < 100)
	{
		g_pvPwrAvgSum = 0;
	}

	/* 逆潮流電力 */
	if( (g_einvCmd81_r.revPwrFlow < 100) && (g_einvCmd81_r.revPwrFlow > -100) )
	{		
		memset(g_arrayRevPwrFlow,	0, sizeof(g_arrayRevPwrFlow));
		g_indexRevPwrFlow = 0;
	}
	else
	{
		g_arrayRevPwrFlow[g_indexRevPwrFlow] = g_einvCmd81_r.revPwrFlow;
		g_indexRevPwrFlow++;

		if(g_indexRevPwrFlow > 9)
		{
			g_indexRevPwrFlow = 0;
		}
	}

	/* 蓄電池充放電電力(AC端計測値) */
	if( (EINV_BATTERY_OPERATION_STOP == g_einvCmd81_r.btOperation)
		|| ( (g_einvCmd81_r.btCharDischarPwr < 100) && (g_einvCmd81_r.btCharDischarPwr > -100) ) )
	{		
		memset(g_arrayBtChargeDischPwr,	0, sizeof(g_arrayBtChargeDischPwr));
		g_indexBtChargeDischPwr = 0;
	}
	else
	{
		g_arrayBtChargeDischPwr[g_indexBtChargeDischPwr] = g_einvCmd81_r.btCharDischarPwr;
		g_indexBtChargeDischPwr++;

		if(g_indexBtChargeDischPwr > 9)
		{
			g_indexBtChargeDischPwr = 0;
		}
	}
	
	/* V2H瞬時充放電電力計測値(AC端計測値) */	
	if( (EINV_UNLOCK_STATE == g_einvCmd81_r.carConnectorLockState)
		|| (EINV_OPRATSTATE_2 == g_einvCmd81_r.carCharDiscOpratState)
		|| (EINV_OPRATSTATE_3 == g_einvCmd81_r.carCharDiscOpratState)
		|| (EINV_OPRATSTATE_E == g_einvCmd81_r.carCharDiscOpratState)
		|| ( (g_einvCmd81_r.v2hInstantPower < 100) && (g_einvCmd81_r.v2hInstantPower > -100) ) )
	{		
		memset(g_arrayV2hInstantPower,	0, sizeof(g_arrayV2hInstantPower));
		g_indexV2hInstantPower = 0;
	}
	else
	{
		g_arrayV2hInstantPower[g_indexV2hInstantPower] = g_einvCmd81_r.v2hInstantPower;
		g_indexV2hInstantPower++;

		if(g_indexV2hInstantPower > 9)
		{
			g_indexV2hInstantPower = 0;
		}
	}
	DEBUG_TRCOUT( );
}


/*==========================================================================*/
/*	Description		:作成CMD81データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:cmdData	:cmdデータ									*/
/*					:reqSize	:要求サイズ									*/
/*					:reqData	:要求データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:作成CMD81データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/06 : DHC Song.mh : 新規作成					*/
/*==========================================================================*/
static void einv_MakeCmd81Data(EinvCmdDataT_t *cmdData, slong reqSize, uchar *reqData)
{
	EinvCmd81T_t *cmd81 = &cmdData->cmd81_t;
	schar buf[EINV_LONG_BUF_SIZE] = { 0 };
	ulong cnt = 0UL;

	DEBUG_TRCIN( );

	memset(reqData, EINV_CODE_NO_CHANGE, reqSize);
	memset(buf   , 0                  , sizeof(buf));
	
	/* 現在時刻(年) */
	snprintf(buf, sizeof(buf), "%02u",cmd81->preMoment.year);
	memcpy(&reqData[cnt], buf, EINV_CMD81_PREMOMENTYEAR_LEN);
	cnt = cnt + EINV_CMD81_PREMOMENTYEAR_LEN;

	/* 現在時刻(月) */
	snprintf(buf, sizeof(buf), "%02u",cmd81->preMoment.month);
	memcpy(&reqData[cnt], buf, EINV_CMD81_PREMOMENTMONTH_LEN);
	cnt = cnt + EINV_CMD81_PREMOMENTMONTH_LEN;

	/* 現在時刻(日) */
	snprintf(buf, sizeof(buf), "%02u",cmd81->preMoment.day);
	memcpy(&reqData[cnt], buf, EINV_CMD81_PREMOMENTDAY_LEN);
	cnt = cnt + EINV_CMD81_PREMOMENTDAY_LEN;

	/* 現在時刻(時) */
	snprintf(buf, sizeof(buf), "%02u",cmd81->preMoment.hour);
	memcpy(&reqData[cnt], buf, EINV_CMD81_PREMOMENTHOUR_LEN);
	cnt = cnt + EINV_CMD81_PREMOMENTHOUR_LEN;

	/* 現在時刻(分) */
	snprintf(buf, sizeof(buf), "%02u",cmd81->preMoment.minute);
	memcpy(&reqData[cnt], buf, EINV_CMD81_PREMOMENTMINUTE_LEN);
	cnt = cnt + EINV_CMD81_PREMOMENTMINUTE_LEN;

	/* 現在時刻(秒) */
	snprintf(buf, sizeof(buf), "%02u",cmd81->preMoment.second);
	memcpy(&reqData[cnt], buf, EINV_CMD81_PREMOMENTSECOND_LEN);
	cnt = cnt + EINV_CMD81_PREMOMENTSECOND_LEN;

	/* 発電電力制限値(kW) */
	snprintf(buf, sizeof(buf), "%04u",cmd81->genPwrLimit);
	memcpy(&reqData[cnt], buf, EINV_CMD81_GENPWRLIMIT_LEN);
	cnt = cnt + EINV_CMD81_GENPWRLIMIT_LEN;

	/* 通信ボードエラー */
	memcpy(&reqData[cnt], &cmd81->commuBoardError, EINV_CMD81_COMMUBOARDERROR_LEN);
	cnt = cnt + EINV_CMD81_COMMUBOARDERROR_LEN;

	/* 車両充放電制限 */
	reqData[cnt] = cmd81->carCharDischar;

	DEBUG_TRCOUT( );
}

/*==========================================================================*/
/*	Description		:解析CMD90データ										*/
/*--------------------------------------------------------------------------*/
/*	param			:resData	:応答データ									*/
/*--------------------------------------------------------------------------*/
/*	return			:なし													*/
/*--------------------------------------------------------------------------*/
/*	detail			:解析CMD90データ処理を行う								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/06 : DHC Song.MH : 新規作成					*/
/*==========================================================================*/
static void einv_ParseCmd90Data(const uchar *resData)
{
	DEBUG_TRCIN( );

	memset(&g_einvCmd90_r, 0, sizeof(g_einvCmd90_r));

	memcpy(&g_einvCmd90_r.carCharAndDiscModelName, &resData[EINV_CMD90_RES_CARCHARANDDISCMODELNAME],EINV_CMD90_CARCHARANDDISCMODELNAME_LEN);															/* 車両充放電器機種名 */
	memcpy(&g_einvCmd90_r.carCharAndDiscMadeNum, &resData[EINV_CMD90_RES_CARCHARANDDISCMADENUM],EINV_CMD90_CARCHARANDDISCMADENUM_LEN);																	/* 車両充放電器製造番号 */
	g_einvCmd90_r.carCharAndDiscType			= (uchar)einv_Strtoul(&resData[EINV_CMD90_RES_CARCHARANDDISCTYPE			], EINV_CMD90_CARCHARANDDISCTYPE_LEN		, EINV_CMD_RES_BASE_DEC);		/* 車両充放電器タイプ */
	g_einvCmd90_r.carCharAndDiscRatedChar		= (ulong)einv_Strtoul(&resData[EINV_CMD90_RES_CARCHARANDDISCRATEDCHAR		], EINV_CMD90_CARCHARANDDISCRATEDCHAR_LEN	, EINV_CMD_RES_BASE_HEX);		/* 車両充放電器定格充電能力 */
	g_einvCmd90_r.carCharAndDiscRatedDisc		= (ulong)einv_Strtoul(&resData[EINV_CMD90_RES_CARCHARANDDISCRATEDDISC		], EINV_CMD90_CARCHARANDDISCRATEDDISC_LEN	, EINV_CMD_RES_BASE_HEX);		/* 車両充放電器定格放電能力 */
	memcpy(&g_einvCmd90_r.carId, &resData[EINV_CMD90_RES_CARID],EINV_CMD90_CARID_LEN);																													/* 車両ID */
	memcpy(&g_einvCmd90_r.evpcsFw, &resData[EINV_CMD90_RES_EVPCSFW],EINV_CMD90_EVPCSFW_LEN);																											/* EVPCS_FW */
	memcpy(&g_einvCmd90_r.mcuFw, &resData[EINV_CMD90_RES_MCUFW],EINV_CMD90_MCUFW_LEN);																													/* MCU_FW */
	memcpy(&g_einvCmd90_r.c2CommuBoradFw, &resData[EINV_CMD90_RES_C2COMMUBORADFW],EINV_CMD90_C2COMMUBORADFW_LEN);																						/* C2通信ボード_FW */

	DLOG_Debug("g_einvCmd90_r.carCharAndDiscModelName = %s\n",				g_einvCmd90_r.carCharAndDiscModelName);
	DLOG_Debug("g_einvCmd90_r.carCharAndDiscMadeNum = %s\n",				g_einvCmd90_r.carCharAndDiscMadeNum);
	DLOG_Debug("g_einvCmd90_r.carCharAndDiscType = %u\n",					g_einvCmd90_r.carCharAndDiscType);
	DLOG_Debug("g_einvCmd90_r.carCharAndDiscRatedChar = %ld\n",				g_einvCmd90_r.carCharAndDiscRatedChar);
	DLOG_Debug("g_einvCmd90_r.carCharAndDiscRatedDisc = %ld\n",				g_einvCmd90_r.carCharAndDiscRatedDisc);
	DLOG_Debug("g_einvCmd90_r.carId = %s\n",								g_einvCmd90_r.carId);
	DLOG_Debug("g_einvCmd90_r.evpcsFw = %s\n",								g_einvCmd90_r.evpcsFw);
	DLOG_Debug("g_einvCmd90_r.mcuFw = %s\n",								g_einvCmd90_r.mcuFw);
	DLOG_Debug("g_einvCmd90_r.c2CommuBoradFw = %s\n",						g_einvCmd90_r.c2CommuBoradFw);

	DEBUG_TRCOUT( );
}
/*==========================================================================*/
/*	Description	:応答コマンドコードが期待値と一致するか比較する。			*/
/*--------------------------------------------------------------------------*/
/*	param		:cmdCode	：送信コマンドコード							*/
/*				:pBuf		：受信データバッファ							*/
/*--------------------------------------------------------------------------*/
/*	return		:ret		受信データサイズチェック結果					*/
/*				:			FALSE				一致						*/
/*				:			TRUE				不一致						*/
/*--------------------------------------------------------------------------*/
/*	detail		:応答コマンドコードが期待値と一致するか比較処理を行う。		*/
/*--------------------------------------------------------------------------*/
/*	history		：2023/03/31 : DHC Dai.P : 新規作成							*/
/*==========================================================================*/
static bool_t einv_n_IsResCmdError( const schar *cmdCode, uchar* pBuf )
{
	bool_t	ret  = FALSE;
	sint	cmp  = 0;
	schar	resCmdCodeExp[EINV_CMD_REQ_CODE_LEN];			/* 応答コマンド(期待値)	*/
	schar	resCmdCodeVal[EINV_CMD_REQ_CODE_LEN];			/* 応答コマンド(取得値)	*/

	/* バッファ初期化 */
	memset( resCmdCodeExp, 0, sizeof(resCmdCodeExp) );
	memset( resCmdCodeVal, 0, sizeof(resCmdCodeVal) );

	/* 期待値生成		*/
	resCmdCodeExp[0] = (schar)tolower( (sint)cmdCode[0] );
	memcpy( &resCmdCodeExp[1], &cmdCode[1], sizeof(resCmdCodeExp) - 1 );

	/* 取得値取り出し	*/
	strncpy( resCmdCodeVal, (schar*)&pBuf[EINV_CMD_CODE_OFFSET], EINV_CMD_REQ_CODE_LEN - 1 );

	/* 応答コマンドを確認	*/
	cmp = strncmp( resCmdCodeExp, resCmdCodeVal, EINV_CMD_REQ_CODE_LEN - 1);
	if ( cmp != 0 ){
		/* 不一致	*/
		DLOG_Error( "pcs response command error.(exp=%s, val=%s)\n", resCmdCodeExp, resCmdCodeVal );
		ret = TRUE;
	}
	else
	{
		/* 処理無し */
	}

	return ret;

}


/*End Of File*/
