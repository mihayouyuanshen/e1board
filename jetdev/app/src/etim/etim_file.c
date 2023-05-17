/******************************************************************************/
/*    Description    :                                                        */
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

/*----------------------------------------------------------------------------*/
/* Include Header File                                                        */
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include "etim_file.h"
#include "etim_msg.h"
#include "common_g.h"
#include "dlog_g.h"

/*----------------------------------------------------------------------------*/
/* 公開変数定義                                                                */
/*----------------------------------------------------------------------------*/
extern schar g_etimNtpHost[ETIM_NTPHOST_BUFSZ];	/* IPv4/IPv6アドレス又はホスト名 */

/*----------------------------------------------------------------------------*/
/* 非公開変数定義                                                              */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 公開関数定義                                                                */
/*----------------------------------------------------------------------------*/
/*============================================================================*/
/*    Description    :設定ファイル読込                                         */
/*----------------------------------------------------------------------------*/
/*    param          :                                                        */
/*----------------------------------------------------------------------------*/
/*    return         :sint   処理結果                                           */
/*                    (ETIM_OK(0):正常/                                       */
/*                     ETIM_E_SYSCALL(-1):システムコールエラー/                 */
/*                     ETIM_E_PARAM(-2):引数異常（ポインタNULL）/               */
/*                     ETIM_E_OUTVAL(-3):設定値異常（範囲外）/）/               */
/*                     ETIM_E_OTHER(-4):その他エラー)                          */
/*----------------------------------------------------------------------------*/
/*    detail         :                                                        */
/*----------------------------------------------------------------------------*/
/*    history        :2023/04/03  : DHC Gu.CM : 新規作成                      */
/*============================================================================*/
sint ETIM_ReadFile(void)
{
	FILE *fp;
	schar rbuf[ETIM_READBUF_SZ + 8];
	schar *pbuf;
	schar *savep;
	sint ret;
	size_t len;
	slong cnt = 0;
	struct stat	st;
	uchar readFlag = ETIM_FLAG_ERR;
	uchar fGetBufFlag = 0;

	DEBUG_TRCIN();

	/* ファイルOPEN */
	fp = CMN_FOPEN(ETIM_INI_FILEPATH, "r");
	if (fp != NULL)
	{
		cnt = 0;
		while(1)
		{
			memset(rbuf, 0x0, sizeof(rbuf));

			/* １行読込 */
			pbuf = fgets(rbuf, sizeof(rbuf) - 1, fp);
			if (NULL != pbuf)
			{
				/* 行番号インクリメント */
				cnt++;

				/* 行の長さチェック */
				len = strnlen(rbuf, sizeof(rbuf) - 1);
				if (ETIM_READBUF_SZ <= len)
				{
					/* 長さエラー */
					DLOG_Error("line length too long.(%d)\n", cnt + 1);
					/* 改行が含まれているかどうかを判定する */
					pbuf = strpbrk(rbuf, ETIM_CRLF);
					if (NULL == pbuf)
					{
						/* 改行が無い→行の最後まで読み捨てる */
						while(0 == fGetBufFlag)
						{
							if (NULL != fgets(rbuf, ETIM_READBUF_SZ, fp))
							{
								pbuf = strpbrk(rbuf, ETIM_CRLF);
								if (NULL != pbuf)
								{
									fGetBufFlag = 1;
								}
							}
							else
							{
								fGetBufFlag = 1;
							}
						}
					}
				}
				else
				{
					/* 先頭の空白文字をスキップしてタグ名取り出し */
					pbuf = strtok_r(rbuf, ETIM_WHITESP, &savep);
					if (NULL != pbuf)
					{
					/* コメント行か改行コードならスキップ */
						if (('#' == *pbuf) || ('\n' == *pbuf) || ('\r' == *pbuf))
						{
							/* don't care */
						}
						else
						{
							/* タグ名比較 */
							/* NTPサーバーアドレス設定タグ名 */
							ret = strncmp(pbuf, ETIM_NTPHOST_TAG, strlen(ETIM_NTPHOST_TAG));
							if (0 == ret)
							{
								/* 続きから値を取り出す */
								pbuf = strtok_r(NULL, ETIM_WHITESP, &savep);
								if (NULL != pbuf)
								{
									readFlag = ETIM_FLAG_OK;
								}
								else
								{
									DLOG_Error("line has no value (EOF).(%d)\n", cnt+1);
								}

								if (readFlag == ETIM_FLAG_OK)
								{
									if (('\n' == *pbuf) || ('\r' == *pbuf))
									{
										/* 先頭が改行コードなら値なし */
										DLOG_Error("line has no value string.(%d)\n", cnt+1);
										readFlag = ETIM_FLAG_ERR;
									}
								}

								if (readFlag == ETIM_FLAG_OK)
								{
									len = strnlen(pbuf, ETIM_NTPHOST_BUFSZ);
									if (len < ETIM_NTPHOST_BUFSZ)
									{
										/* 範囲内なので値をコピー */
										/* 末尾の改行を削除 */
										if ('\n' == *(pbuf+len-1))
										{
											*(pbuf + len - 1) = '\0';
										}
										/* 最大255バイトコピー */
										strncpy( g_etimNtpHost, pbuf, ETIM_NTPHOST_BUFSZ-1 );
										/* 安全のため256バイト目をヌル */
										g_etimNtpHost[ETIM_NTPHOST_BUFSZ - 1] = '\0';
									}
									else
									{
										DLOG_Error("line value string too long.(%d)\n", cnt+1);
									}
								}
							}
							else
							{
								/* 合致するタグ名が無ければ無視 */
								DLOG_Error("unknown tag name.(%d)\n", cnt+1);
							}
						}
					}
					else
					{
						/* タグが取得できない（通常発生しない）*/
						DLOG_Error("it is null line.(%d)\n", cnt+1);
					}
				}
			}
		}
		/* リターン値設定 */
		ret = ETIM_OK;
	}
	else
	{

		memset(rbuf, 0x00, sizeof(rbuf));
		strerror_r(errno, rbuf, ETIM_ERR_BUF_SIZE);
		DLOG_Error("CMN_FOPEN() error(%d,%s).\n", errno, rbuf);

		/* ファイル有無確認 */
		ret = stat(ETIM_INI_FILEPATH, &st);
		if (0 == ret)
		{
			/* ファイル有り */
			/* 一旦ファイルを削除 */
			ret = remove(ETIM_INI_FILEPATH);
			if (0 != ret)
			{
				/* 削除失敗 */
				memset(rbuf, 0x00, sizeof(rbuf));
				strerror_r(errno, rbuf, ETIM_ERR_BUF_SIZE);
				DLOG_Error("remove() error(%d,%s).\n", errno, rbuf);
				DEBUG_TRCOUT();
				return ETIM_E_SYSCALL;
			}
		}

		/* ファイル再オープン */
		fp = CMN_FOPEN(ETIM_INI_FILEPATH, "w");
		if (NULL == fp)
		{
			/* オープン不可 */
			memset(rbuf, 0x00, sizeof(rbuf));
			strerror_r(errno, rbuf, ETIM_ERR_BUF_SIZE);
			DLOG_Error("CMN_FOPEN(w) error(%d,%s).\n", errno, rbuf);
			DEBUG_TRCOUT();
			return ETIM_E_SYSCALL;
		}

		/* デフォルト値を設定 */
		/* NTPサーバーアドレス */
		fprintf(fp, "%s\t%s\n", ETIM_NTPHOST_TAG, g_etimNtpHost);

		/* ファイルクローズ */
		CMN_FCLOSE(fp);

		/* リターン値設定 */
		return ETIM_E_SYSCALL;
	}

	/* ファイルクローズ */
	CMN_FCLOSE(fp);

	/* リターンを返す */
	DEBUG_TRCOUT();

	return ETIM_OK;
}

/*----------------------------------------------------------------------------*/
/* 非公開関数定義                                                              */
/*----------------------------------------------------------------------------*/


/* End Of File */
