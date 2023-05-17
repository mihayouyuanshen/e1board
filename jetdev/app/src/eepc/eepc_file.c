/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Xu.Y												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/07												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include Header File														*/
/*--------------------------------------------------------------------------*/
#include "eepc_file.h"
#include "dlog_g.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <ctype.h>
#include "common_g.h"
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
static EepcFuncResult_t eepc_FileOpen(schar *filePath, schar *fielAttr, FILE *fp);
static EepcFuncResult_t eepc_FileWrite(schar *writeData, ulong dataSize, FILE *fp);
static EepcFuncResult_t eepc_FileClose(FILE *fp);
static EepcFuncResult_t eepc_FileChgLineEnd(schar *chgData, uint chgDataSize);
static EepcFuncResult_t eepc_FileChkFrmtBankNo(schar *tmpFileData, uchar *bankNo, ulong tmpFileDataSize);
static EepcFuncResult_t eepc_FileFncChkFrmtVer(schar *tmpFileData, ushort *srvInfVerMajor, uchar *srvInfVerMiner, ulong tmpFileDataSize);
static EepcFuncResult_t eepc_FileChkFrmtSrvCnt(schar *tmpFileData, uchar *srvCnt, ulong tmpFileDataSize);
static EepcFuncResult_t eepc_FileChkFrmtCorpDiv(schar *tmpFileData, uchar *corpDiv, ulong maxDiv, ulong tmpFileDataSize);
static EepcFuncResult_t eepc_FileChkFrmtSrvInfStr(schar *tmpFileData, schar *setStr, uint maxSize, ulong tmpFileDataSize);
static EepcFuncResult_t eepc_FileChkFrmtSrvInfPort(schar *tmpFileData, ushort *setPort, ulong maxPort, ulong tmpFileDataSize);
static EepcFuncResult_t eepc_FileChkFrmtSrvInf(schar *tmpFileData, EepcFileSrvInf_t *srvInf, ulong tmpFileDataSize);
static EepcFuncResult_t eepc_FileChkFrmtSrvMngFile(schar *tmpFileData, ulong fileLineCnt, EepcFileSrvMng_t *srvMng, ulong tmpFileDataSize);
static EepcFuncResult_t eepc_FileStartChkFrmtSrvMngFile(FILE *fp);
static EepcFuncResult_t eepc_FileChkFileExist(schar *dirPath, schar *fileName);
static EepcFuncResult_t eepc_FileChkRtCerFileExist(EepcFileSrvMng_t *srvMng, schar *dirPath);
static EepcFuncResult_t eepc_FileCpyFile(schar *fileName, schar *srcDirPath, schar *dstDirPath);
static EepcFuncResult_t eepc_FileFncCpyRtCerFile(EepcFileSrvMng_t *srvMng, schar *srcDirPath, schar *dstDirPath);
static EepcFuncResult_t eepc_FileFncWriteSrvMngFile(EepcFileSrvMng_t *srvMng);
static EepcFuncResult_t eepc_FileUpdateUseSrv(EepcFileUseSrv_t *useSrv, uchar index);
static EepcFuncResult_t eepc_FileWriteUseSrvFile(EepcFileUseSrv_t *useSrv);
static EepcFuncResult_t eepc_FileUpdateChgData(EepcEvt01_t *chgData, ulong chgFlg);
static EepcFuncResult_t eepc_FileEvtChgSettingAll(ulong eventType);
static EepcFuncResult_t eepc_FileEvtChgSetting(sint receiver, ulong eventType);
static EepcFuncResult_t eepc_FileCrtInitUseSrvFile(void);
static EepcFuncResult_t eepc_FileErrInitGlobalSrvMng(EepcFileSrvMng_t *srvMng);
static EepcFuncResult_t eepc_FileCrtInitSrvMngFile(void);
static EepcFuncResult_t eepc_FileChkFrmtUseSrvFile(schar *tmpFileData, ulong fileLineCnt, EepcFileUseSrv_t *ursSrv, ulong tmpFileDataSize);
static EepcFuncResult_t eepc_FileInitGlobalUseSrv(EepcFileUseSrv_t *useSrv);
static EepcFuncResult_t eepc_FileStartChkFrmtUseSrvFile(FILE *filePtr);
static EepcFuncResult_t eepc_FileChkFrmtSrvFixed(schar *tmpFileData, uchar *srvFixed, ulong tmpFileDataSize);
static EepcFuncResult_t eepc_FncChkFrmtSrvFixedFile(schar *tmpFileData, ulong fileLineCnt, EepcFileSrvFixed_t *srvFixed, ulong tmpFileDataSize);
static EepcFuncResult_t eepc_FileInitGlobalSrvFixed(EepcFileSrvFixed_t *srvFixed);
static EepcFuncResult_t eepc_FileStartChkFrmtSrvFixedFile(FILE *filePtr);
static EepcFuncResult_t eepc_FileCrtInitSrvFixed_File(void);
static EepcFuncResult_t eepc_FileChkSrvMngFile(void);
static EepcFuncResult_t eepc_FileChkUseSrvFile(void);
static void eepc_FileFncGetUrlVer(schar *urlFilePath, ushort *verMajor, uchar *verMinor);
static bool_t eepc_FileFncChkVerUpdate(ushort baseMajor, uchar baseMinor, ushort chkMajor, uchar chkMinor);
static EepcFuncResult_t eepc_FileFncGetUpdFilePath(schar *dirPathBuf, ulong dirPathBufSz, schar *fnameBuf, ulong fnameBufSz);
static EepcFuncResult_t eepc_FileFncChkFrmtSrvUrlFile(schar *tmpFileData, ulong fileLineCnt, ulong tmpFileDataSize);
static EepcFuncResult_t eepc_FileStartChkFrmtSrvUrlFile(FILE *filePtr);
/*==========================================================================*/
/*	Description		:	ファイルチェック処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:	なし												*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ファイルチェック処理								*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/10 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcFuncResult_t EEPC_FileFncChkAllFiles(void)
{
	EepcFuncResult_t	result		= EEPC_FUNC_RESULT_OK;	/* 処理結果										*/
	sint				retSrvMng	= EEPC_FUNC_RESULT_OK;	/* 電力会社サーバ管理ファイルチェック結果		*/
	sint				retUseSrv	= EEPC_FUNC_RESULT_OK;	/* 電力会社サーバ選択ファイルチェック結果		*/

	DEBUG_TRCIN();

	/* 電力会社サーバ管理ファイルチェック処理	*/
	retSrvMng = eepc_FileChkSrvMngFile();

	/* 電力会社サーバ選択ファイルチェック処理	*/
	retUseSrv = eepc_FileChkUseSrvFile();

	/* いずれかの処理が失敗している場合	*/
	if ((EEPC_FUNC_RESULT_OK != retSrvMng) || (EEPC_FUNC_RESULT_OK != retUseSrv))
	{
		result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
	}
	else
	{
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	テストサーバ接続設定読み込みとユーザー領域パス設定	*/
/*--------------------------------------------------------------------------*/
/*	param			:	なし												*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	テストサーバ接続設定読み込みとユーザー領域パス設定	*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/07 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcFuncResult_t EEPC_FileReadTestSrvConnect(void)
{
	EepcFuncResult_t	result	= EEPC_FUNC_RESULT_OK;
	sint				ret		= 0;
	struct stat			stat_buf;
	schar				testFilePathName[EEPC_TEST_SRV_FILEFULLPATH_LEN];
	schar				cmd[EEPC_TEST_CMD_LEN];

	DEBUG_TRCIN();

	memset(&stat_buf, 0x00, sizeof(stat_buf));
	memset(testFilePathName, 0x00, sizeof(testFilePathName));
	memset(cmd, 0x00, sizeof(cmd));

	sprintf(testFilePathName, "%s%s", EEPC_TEST_SRV_PATH, EEPC_SRV_MNG_FILE_NAME);

	/* 前回のディレクトリが残っている場合 */
	ret = stat(EEPC_USR_DIR_PATH, &stat_buf);
	if (EEPC_FUNC_RESULT_OK == ret)
	{
		/* ディレクトリ削除	*/
		sprintf( cmd, "rm -rf %s", EEPC_USR_DIR_PATH);
		ret = system(cmd);
		if (EEPC_FUNC_RESULT_OK == ret)
		{
			DLOG_Info("rm success. path=%s\n", EEPC_USR_DIR_PATH);
		}
		else
		{
			DLOG_Error("rm error. path=%s, ret=%d\n", EEPC_USR_DIR_PATH, ret);
		}
	}
	else
	{
		/* DO NOTHING */
	}

	/* ルート証明書シンボリックリンク用フォルダ作成 */
	sprintf(cmd, "mkdir -p %s", EEPC_LINK_PEM_FILE_PATH);
	ret = system(cmd);
	DLOG_Info("mkdir(%s) ret=%d\n", cmd, ret);

	/* mountコマンド実行 */
	sprintf(cmd, "mount -t vfat %s %s", EEPC_TEST_PATH_DEVSDA, EEPC_USB_MEDIA_PATH);
	ret = system(cmd);
	if (EEPC_FUNC_RESULT_OK == ret)
	{
		/* /media/usb_storage/PV_SEIGYO/test_srv/eepc_SrvMng.csvチェック */
		ret = stat(testFilePathName, &stat_buf);
	}
	else
	{
		DLOG_Error("usb media path not exist(%s)", EEPC_USB_MEDIA_PATH);
	}

	if (EEPC_FUNC_RESULT_OK == ret)
	{
		/* /media/usb_storage/PV_SEIGYO/test_srv/eepc_UseSrv.csvチェック */
		sprintf(testFilePathName, "%s%s", EEPC_TEST_SRV_PATH, EEPC_USE_SRV_FILE_NAME);
		ret = stat(testFilePathName, &stat_buf);
	}
	else
	{
		/* DO NOTHING */
	}

	if (EEPC_FUNC_RESULT_OK == ret)
	{
		/* 接続先情報コピー */
		/* /media/usb_storage/PV_SEIGYO/test_srv/eepc_SrvMng.csv -> /mnt/ram/eepc */
		/* /media/usb_storage/PV_SEIGYO/test_srv/eepc_UseSrv.csv -> /mnt/ram/eepc */
		/* /media/usb_storage/PV_SEIGYO/test_srv/ROOT_CER/reene.pem -> /mnt/ram/eepc */
		sprintf( cmd, "cp -r %s* %s", EEPC_TEST_SRV_PATH, EEPC_RAM_DIR_PATH);
		ret = system(cmd);
	}
	else
	{
		DLOG_Error("no test_srv_path=%s", testFilePathName);
	}

	if (EEPC_FUNC_RESULT_OK == ret)
	{
		/* RAMへのシンボリックリンクコマンド生成 */
		/* /mnt/ram/eepc/ -> /mnt/user/ */
		sprintf( cmd, "ln -nfs %s %s", EEPC_RAM_DIR_PATH, EEPC_MNT_USER_PATH);
		ret = system(cmd);
		if (EEPC_FUNC_RESULT_OK == ret)
		{
			/* /mnt/ram/eepc/ROOT_CER/reene.pem -> /mnt/ram/eepc/ROOT_CER/KYUDEN/ */
			sprintf( cmd, "ln -nfs %s %s", EEPC_LINK_RAM_PEM_FILE, EEPC_LINK_PEM_FILE_PATH);
			ret = system(cmd);
			if (EEPC_FUNC_RESULT_OK != ret)
			{
				DLOG_Error("link error. ret=%d (%s)", ret, cmd);
			}
			else
			{
				/* DO NOTHING */
			}
		}
		else
		{
			/* DO NOTHING */
		}
		
	}
	else
	{
		DLOG_Error("cp srv file error. ret=%d (%s)", ret, cmd);
	}

	/* テストサーバ接続用ファイルがない場合 */
	if (EEPC_FUNC_RESULT_OK != ret)
	{
		/* 不揮発領域へのシンボリックリンクコマンド生成 */
		/* /mnt/user/eepc_nand/ -> /mnt/user/eepc */
		sprintf( cmd, "ln -nfs %s %s", EEPC_USR_NAND_DIR_PATH, EEPC_LINK_PATH);
		ret = system(cmd);
		if (EEPC_FUNC_RESULT_OK == ret)
		{
			/* /usr/bin/reene.pem -> /mnt/ram/eepc/ROOT_CER/KYUDEN/ */
			sprintf( cmd, "ln -nfs %s%s %s", EEPC_PEM_FILE_PATH, EEPC_PEM_FILE_NAME, EEPC_LINK_PEM_FILE_PATH);
			ret = system(cmd);
			if (EEPC_FUNC_RESULT_OK != ret)
			{
				DLOG_Error("link error. ret=%d (%s)\n", ret, cmd);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
		}
		else
		{
			/* DO NOTHING */
		}
	}
	else
	{
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社サーバ選択ファイル詳細チェック処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcFileUseSrv_t *useSrv	電力会社サーバ選択ファイル用構造体	*/
/*					:	bool_t *chgFlg				電力会社サーバ情報変化フラグ		*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK			処理成功				*/
/*					:	EEPC_FUNC_RESULT_NG			処理失敗				*/
/*					:	EEPC_FUNC_RESULT_EPRM		引数異常				*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社サーバ選択ファイル詳細チェック処理			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcFuncResult_t EEPC_FileDetailChkUseSrvFile(EepcFileUseSrv_t *useSrv, bool_t *chgFlg)
{
	EepcFuncResult_t	result	= EEPC_FUNC_RESULT_OK;	/* 処理結果			*/
	sint				cmpRet	= 0;					/* メモリ比較結果	*/
	ulong				loopCnt	= 0u;					/* ループカウンタ	*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == useSrv) || (NULL == chgFlg))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_OK;
	}
	else
	{
		/* 電力会社サーバの総数をチェック */
		if (!g_eepcFileSrvMng.srvCnt)
		{
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* 電力会社サーバ管理ファイル用mutex取得 */
			FRM_MutexLock(g_eepcMutexFileSrvMng);

			*chgFlg = FALSE;
			/* 更に、電力会社サーバ管理ファイルのデータと一致する事をチェック */
			for (loopCnt = 0; loopCnt < g_eepcFileSrvMng.srvCnt; loopCnt++)
			{
				cmpRet = memcmp(&(useSrv->srvInf),
					&(g_eepcFileSrvMng.srvInfList[loopCnt]),
					sizeof(EepcFileSrvInf_t));

				/* 電力会社管理ファイルのデータと一致する場合 */
				if (!cmpRet)
				{
					/* 使用中電力会社のIndex値を更新する */
					g_eepcUseSrvIndex = (uchar)loopCnt;
					*chgFlg = FALSE;	// 変更なし
					/* 一致するデータを確認出来た為、ループを抜ける */
					loopCnt = g_eepcFileSrvMng.srvCnt;
				}
				/* 全一致しない場合でも送配電事業社コードと電力会社名が一致する場合は同一と判断 */
				else if (g_eepcFileSrvMng.srvInfList[loopCnt].corpDiv == useSrv->srvInf.corpDiv)
				{
					sint tmpCmp = strncmp(g_eepcFileSrvMng.srvInfList[loopCnt].srvNameStr,
						useSrv->srvInf.srvNameStr, EEPC_SRV_NAME_LEN);
					if (!tmpCmp)
					{
						/* 使用中電力会社のIndex値を更新する */
						g_eepcUseSrvIndex = (uchar)loopCnt;
						*chgFlg = TRUE;	// 変更あり
						cmpRet = 0;
						/* 一致するデータを確認出来た為、ループを抜ける */
						loopCnt = g_eepcFileSrvMng.srvCnt;
					}
					else
					{
						/* DO NOTHING */
					}
				}
				else
				{
					// 何もしない。PGrelief対応。
				}
			}

			/* 電力会社サーバ管理ファイル用mutex解放 */
			FRM_MutexUnLock(g_eepcMutexFileSrvMng);

			/* 一致するデータが無い場合	*/
			if (cmpRet)
			{
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社情報保存処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcSetSrv_t *setSrv	選択した電力会社情報の格納先*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社情報保存処理								*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/10 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcFuncResult_t EEPC_FileSetUsrSrvInf(EepcSetSrv_t *setSrv)
{
	EepcFuncResult_t	result									= EEPC_FUNC_RESULT_OK;	/* 処理結果												*/
	EepcFuncResult_t	openResult								= EEPC_FUNC_RESULT_OK;	/* ファイルオープン結果									*/
	EepcFuncResult_t	closeResult								= EEPC_FUNC_RESULT_OK;	/* ファイルクローズ結果									*/
	EepcRtnCode_t		ifResult								= EEPC_RTN_CODE_OK;		/* プロセス間IF向け処理結果								*/
	ulong				chgSettingFlg							= 0u;					/* 設定変更通知イベント用のフラグ						*/
	uchar				beforeIndex								= 0u;					/* 変更前の使用中電力会社のIndex値						*/
	schar				tmpFilePath[EEPC_TMP_FILE_PATH_SIZE]	= {EEPC_STR_DATA_END};	/* ファイルパスの一時領域								*/
	FILE				*filePtrSrvFixed						= NULL;					/* 電力会社サーバ設定済みファイルのファイルポインタ		*/
	EepcPrcRes_t		ret;															/* EEPCプロセス応答用構造体								*/
	EepcFileUseSrv_t	tmpUsrSrv;														/* 選択ファイル用構造体の一時領域						*/

	DEBUG_TRCIN();

	memset(&ret, 0, sizeof(EepcPrcRes_t));
	memset(&tmpUsrSrv, 0, sizeof(EepcFileUseSrv_t));

	/* パラメータチェック */
	if (NULL == setSrv)
	{
		DLOG_Error("prm err");
		DEBUG_TRCOUT();
		return EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* Do NOTHING */
	}

	/* パラメータチェック */
	if (g_eepcFileSrvMng.srvCnt <= setSrv->useSrvIndex)
	{
		DLOG_Error("prm err");
		result		= EEPC_FUNC_RESULT_EPRM;
		ifResult	= EEPC_RTN_CODE_EPRM;
	}
	else
	{
		/* 変更前の使用中電力会社のIndex値を保持 */
		beforeIndex = g_eepcUseSrvIndex;
		/* 変更前の選択ファイル用構造体を保持 */
		memcpy(&tmpUsrSrv, &g_eepcFileUseSrv, sizeof(EepcFileUseSrv_t));
		/* グローバル変数(電力会社サーバ選択ファイル用構造体)を更新 */
		result = eepc_FileUpdateUseSrv(&g_eepcFileUseSrv, setSrv->useSrvIndex);
		if (EEPC_FUNC_RESULT_OK != result)
		{
			result		= EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
			ifResult	= EEPC_RTN_CODE_ESYS;
		}
		else
		{
			/* グローバル変数(通信先情報用構造体)を更新 */
			result = EEPC_FileUpdateComInf(&g_eepcComInf);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				result		= EEPC_FUNC_RESULT_NG;	/* 処理失敗 */
				ifResult	= EEPC_RTN_CODE_ESYS;
			}
			else
			{
				/* 電力会社サーバ選択ファイル書き込み処理 */
				result = eepc_FileWriteUseSrvFile(&g_eepcFileUseSrv);
				/* 書き込み失敗時 */
				if (EEPC_FUNC_RESULT_OK != result)
				{
					result		= EEPC_FUNC_RESULT_NG;	/* 処理失敗 */
					ifResult	= EEPC_RTN_CODE_ESYS;

					/* 電力会社サーバ選択ファイルチェック処理にて、	*/
					/* グローバル変数をロールバックする				*/
					result = eepc_FileChkUseSrvFile();
					/* 通信先情報用構造体もロールバックする */
					result = EEPC_FileUpdateComInf(&g_eepcComInf);
					if (EEPC_FUNC_RESULT_OK != result)
					{
						result		= EEPC_FUNC_RESULT_NG;	/* 処理失敗 */
						ifResult	= EEPC_RTN_CODE_ESYS;
					}
					else
					{
						/* DO NOTHING */
					}
					/* 変更前の使用中電力会社のIndex値にロールバック */
					g_eepcUseSrvIndex = beforeIndex;
				}
				else
				{
					/* 設定済みフラグ:TRUE	*/
					g_eepcFileSrvFixed.srvFixed = TRUE;

					/* 選択ファイル構造体の変更をチェック */
					/* 電力会社の区分 */
					if (tmpUsrSrv.srvInf.corpDiv
						!= g_eepcFileUseSrv.srvInf.corpDiv)
					{
						chgSettingFlg += EEPC_EVT01_CHG_CORP_DIV;
					}
					else
					{
						/* DO NOTHING */
					}

					/* 電力会社指定のNTPサーバのポート番号 */
					if (tmpUsrSrv.srvInf.ntpSrvPort
						!= g_eepcFileUseSrv.srvInf.ntpSrvPort)
					{
						chgSettingFlg += EEPC_EVT01_CHG_NTP_PORT;
					}
					else
					{
						/* DO NOTHING */
					}

					/* 電力会社サーバのURL */
					sint tmpUrlFlg = memcmp(tmpUsrSrv.srvInf.srvUrlStr,
						g_eepcFileUseSrv.srvInf.srvUrlStr, EEPC_URL_LEN);
					if (tmpUrlFlg)
					{
						chgSettingFlg += EEPC_EVT01_CHG_SRV_URL;
					}
					else
					{
						/* DO NOTHING */
					}

					/* 電力会社指定のNTPサーバのFQDN */
					sint tmpNTPFlg = memcmp(tmpUsrSrv.srvInf.ntpFqdnStr,
						g_eepcFileUseSrv.srvInf.ntpFqdnStr, EEPC_FQDN_LEN);
					if (tmpNTPFlg)
					{
						chgSettingFlg += EEPC_EVT01_CHG_NTP_FQDN;
					}
					else
					{
						/* DO NOTHING */
					}

					/* 設定変更通知イベント送信処理(全プロセス) */
					result = eepc_FileEvtChgSettingAll(chgSettingFlg);
					if (result != EEPC_FUNC_RESULT_OK)
					{
						result		= EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
						ifResult	= EEPC_RTN_CODE_ESYS;
					}
					else
					{
						/* DO NOTHING */
					}
				}
			}

			/* 設定済みフラグとファイル作成の要否をチェック */
			if (TRUE == g_eepcFileSrvFixed.srvFixed)
			{
				/* 電力会社サーバ設定済みファイルのファイルパスを生成 */
				sprintf(tmpFilePath, "%s%s",
					EEPC_USR_DIR_PATH, EEPC_SRV_FIEXD_FILE_NAME);
				
				/* ファイルオープン処理 */
				openResult = eepc_FileOpen(tmpFilePath, "r", filePtrSrvFixed);
				if (EEPC_FUNC_RESULT_OK != openResult)
				{
					result		= EEPC_FUNC_RESULT_NG;
					ifResult	= EEPC_RTN_CODE_ESYS;
				}
				else
				{
					/* DO NOTHING */
				}
			}
			else
			{
				/* ファイルがオープンできる場合、	*/
				/* 設定済み情報は書き込んである為、	*/
				/* ファイル更新を行う必要が無い		*/
				/* ファイルクローズ処理 */
				closeResult = eepc_FileClose(filePtrSrvFixed);
				/* ファイルクローズ処理結果チェック */
				if (EEPC_FUNC_RESULT_OK != openResult)
				{
					DLOG_Error("eepc_FileClose err %d", closeResult);
					result		= EEPC_FUNC_RESULT_NG;
					ifResult	= EEPC_RTN_CODE_ESYS;
				}
				else
				{
					/* DO NOTHING */
				}
			}
		}
	}

	/* 呼び出し元がeepcの場合、メッセージ送信は行わない */
	if (EEPC_CPORT != setSrv->header.srcModId)
	{
		/* メッセージ生成	*/
		ret.header.attr		= FRM_CALL_RET;
		ret.header.msgSize	= sizeof(EepcPrcRes_t);
		ret.header.retCode	= ifResult;

		slong tmpAsync = FRM_MsgSendAsync(setSrv->header.srcModId, sizeof(EepcPrcRes_t), (void *)&ret);
		if (tmpAsync)
		{
			DLOG_Error("FRM_MsgSendAsync error");
			result = EEPC_FUNC_RESULT_NG;	/* 処理失敗 */
		}
		else
		{
			/* DO nOTHINg */
		}
		
	}
	else
	{
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	通信先情報用構造体更新処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcComInf_t *comInf	通信先情報用構造体			*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	通信先情報用構造体更新処理							*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/10 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcFuncResult_t EEPC_FileUpdateComInf(EepcComInf_t *comInf)
{
	EepcFuncResult_t result = EEPC_FUNC_RESULT_OK;	/* 処理結果 */

	DEBUG_TRCIN();
	
	/* パラメータチェック	*/
	if(NULL == comInf)
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* 通信先情報用構造体を更新	*/
		/* 電力会社の区分	*/
		comInf->corpDiv = g_eepcFileUseSrv.srvInf.corpDiv;

		/* 電力会社サーバ管理ファイル用mutex取得 */
		FRM_MutexLock(g_eepcMutexFileSrvMng);

		/* 固定スケジュール受信のリトライ回数 */
		comInf->srvRtryCntFixSch = g_eepcFileSrvMng.srvRtryCntFixSch;

		/* 固定スケジュール受信のリトライ間隔 */
		comInf->srvRtryIntvlFixSch = g_eepcFileSrvMng.srvRtryIntvlFixSch;

		/* 更新スケジュール受信のリトライ回数 */
		comInf->srvRtryCntUpdtSch = g_eepcFileSrvMng.srvRtryCntUpdtSch;

		/* 更新スケジュール受信のリトライ間隔 */
		comInf->srvRtryIntvlUpdtSch = g_eepcFileSrvMng.srvRtryIntvlUpdtSch;

		/* 電力会社指定のNTPサーバのポート番号 */
		comInf->ntpPort = g_eepcFileUseSrv.srvInf.ntpSrvPort;

		/* 時刻同期のリトライ回数 */
		comInf->ntpRtryCntTimSync = g_eepcFileSrvMng.ntpRtryCntTimSync;

		/* 時刻同期のリトライ間隔 */
		comInf->ntpRtryIntvlTimSync = g_eepcFileSrvMng.ntpRtryIntvlTimSync;

		/* 時刻同期の再リトライ間隔 */
		comInf->ntpRtryOvrIntvlTimSync = g_eepcFileSrvMng.ntpRtryOvrIntvlTimSync;

		/* 電力会社サーバ管理ファイル用mutex解放 */
		FRM_MutexUnLock(g_eepcMutexFileSrvMng);

		/* 電力会社サーバ通信履歴ファイル用mutex取得 */
		FRM_MutexLock(g_eepcMutexFileComHist);

		/* 電力会社との通信結果及び通信時間 */
		memcpy(comInf->resultTime, g_eepcFileComHist.resultTime, EEPC_RESULT_TIME_LEN);

		/* 電力会社サーバ通信履歴ファイル用mutex解放 */
		FRM_MutexUnLock(g_eepcMutexFileComHist);

		/* 電力会社サーバのURL */
		memcpy(comInf->srvUrlStr, g_eepcFileUseSrv.srvInf.srvUrlStr, EEPC_URL_LEN);

		/* 電力会社指定のNTPサーバのFQDN */
		memcpy(comInf->ntpFqdnStr, g_eepcFileUseSrv.srvInf.ntpFqdnStr, EEPC_FQDN_LEN);

		DLOG_Debug("comInf->srvRtryCntFixSch:%ld\n", comInf->srvRtryCntFixSch);
		DLOG_Debug("comInf->srvRtryIntvlFixSch:%ld\n", comInf->srvRtryIntvlFixSch);
		DLOG_Debug("comInf->srvRtryCntUpdtSch:%ld\n", comInf->srvRtryCntUpdtSch);
		DLOG_Debug("comInf->srvRtryIntvlUpdtSch:%ld\n", comInf->srvRtryIntvlUpdtSch);
		DLOG_Debug("comInf->ntpRtryCntTimSync:%ld\n", comInf->ntpRtryCntTimSync);
		DLOG_Debug("comInf->ntpRtryIntvlTimSync:%ld\n", comInf->ntpRtryIntvlTimSync);
		DLOG_Debug("comInf->ntpRtryOvrIntvlTimSync:%ld\n", comInf->ntpRtryOvrIntvlTimSync);
		DLOG_Debug("comInf->srvUrlStr:%s\n", comInf->srvUrlStr);
		DLOG_Debug("comInf->ntpFqdnStr:%s\n", comInf->ntpFqdnStr);
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	設定変更通知イベント登録データ作成処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcRegChg_t *regChg	設定変更通知登録情報		*/
/*					:	schar *writeFilePath	書き込みファイルパス		*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	設定変更通知イベント登録データ作成処理				*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/11 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcFuncResult_t EEPC_FileCrtRegChgEvtData(EepcRegChg_t *regChg, schar *writeFilePath)
{
	EepcFuncResult_t	result											= EEPC_FUNC_RESULT_OK;	/* 処理結果						*/
	sint				loopCnt											= 0;					/* ループカウンタ				*/
	sint				loopMax											= 0;					/* ループ上限					*/
	sint				closeResult										= 0;					/* ファイルクローズ結果			*/
	sint				evtSendResult									= 0;					/* 設定変更通知送信結果			*/
	sint				writeResult										= 0;					/* ファイル書き込み結果			*/
	ulong				writeFileSize									= 0;					/* ファイルの書き込みサイズ		*/
	bool_t				dataSetFlg										= FALSE;				/* データ設定フラグ				*/
	schar				writeDataLine[EEPC_NOTICE_EVT_DATA_LINE_LEN]	= {EEPC_STR_DATA_END};	/* 1行分のファイル書き込み領域	*/
	FILE				*writeFilePtr									= NULL;					/* ファイルポインタ				*/
	schar				writeDataALL[EEPC_NOTICE_EVT_DATA_LINE_LEN * PROCESS_MAX];				/* ファイル書き込み領域全体		*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == regChg) || (NULL == writeFilePath))
	{
		DLOG_Debug("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		memset(writeDataALL, 0, (EEPC_NOTICE_EVT_DATA_LINE_LEN * PROCESS_MAX));

		/* ファイルオープン処理 */
		result = eepc_FileOpen(writeFilePath, "w", writeFilePtr);
		/* ファイルオープン結果チェック */
		if (EEPC_FUNC_RESULT_OK != result)
		{
			DLOG_Error("eepc_FileOpen err %d", result);
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* ループ上限を取得する */
			loopMax = g_eepcChgEvtRegCnt;
			/* 登録数がプロセス最大数に満たない場合 */
			if (loopMax < PROCESS_MAX)
			{
				/* ループ上限は、現在の登録済み件数+1とする */
				loopMax++;
			}
			else
			{
				/* DO NOTHING */
			}

			/* 設定変更通知イベント情報登録ループ */
			for (loopCnt = 0; loopCnt < loopMax; loopCnt++)
			{
				memset(writeDataLine, 0, EEPC_NOTICE_EVT_DATA_LINE_LEN);

				/* ループ位置のポート番号チェック(新規登録を行う場合) */
				if (!g_eepcFileEvt01[loopCnt].receiver)
				{
					/* データ未設定の場合、新規登録 */
					if (dataSetFlg == FALSE)
					{
						/* グローバル変数(設定変更通知イベント情報ファイル用構造体)を更新 */
						g_eepcFileEvt01[loopCnt].receiver = regChg->receiver;
						g_eepcFileEvt01[loopCnt].chgDataFlg = regChg->eventType;
						/* 新規登録の場合は、登録済み件数を加算する */
						g_eepcChgEvtRegCnt++;
						/* データ設定フラグ:ON */
						dataSetFlg = TRUE;
					}
					else
					{
						loopCnt = loopMax;
					}
				}
				/* ループ位置のポート番号チェック(登録済みのプロセスの更新を行う場合) */
				else if (g_eepcFileEvt01[loopCnt].receiver == regChg->receiver)
				{
					/* グローバル変数(設定変更通知イベント情報ファイル用構造体)を更新 */
					g_eepcFileEvt01[loopCnt].chgDataFlg = regChg->eventType;
					/* 登録済みの場合は、登録済み件数の加算を行わない */
					/* データ設定フラグ:ON */
					dataSetFlg = TRUE;
				}
				/* ループ位置のポート番号チェック(登録対象にならない場合)	*/
				else
				{
					/* 登録情報は変更しない	*/
				}

				if (loopCnt < loopMax)
				{
					/* 1行分のデータを生成する	*/
					sprintf(writeDataLine,
						"%d,%02x\n",
						g_eepcFileEvt01[loopCnt].receiver,
						(unsigned int)g_eepcFileEvt01[loopCnt].chgDataFlg);
					/* ファイル全体の領域に結合する	*/
					strcat(writeDataALL, &(writeDataLine[0]));
				}
				else
				{
					/* DO NOTHING */
				}
			}

			/* ファイルサイズを取得する */
			writeFileSize = (ulong)strnlen(writeDataALL,
				(EEPC_NOTICE_EVT_DATA_LINE_LEN * PROCESS_MAX));
			
			/* fwriteによるファイル書き込み */
			writeResult = eepc_FileWrite(writeDataALL, writeFileSize, writeFilePtr);

			if (EEPC_FUNC_RESULT_OK != writeResult)
			{
				DLOG_Error("eepc_FileWrite err %d", writeResult);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}

			/* ファイルクローズ処理 */
			closeResult = eepc_FileClose(writeFilePtr);

			/* ファイルクローズ処理結果チェック */
			if (EEPC_FUNC_RESULT_OK != closeResult)
			{
				DLOG_Error("eepc_FileClose err %d", closeResult);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
		}

		if (FALSE == dataSetFlg)
		{
			result = EEPC_FUNC_RESULT_NG;	/* 処理失敗 */
		}
		else
		{
			/* 通知要否フラグチェック */
			if (EEPC_REG_NOTICE_ENABLE == regChg->noticerflg)
			{
				/* 設定変更通知イベント送信処理(1プロセス) */
				evtSendResult = eepc_FileEvtChgSetting(regChg->receiver, regChg->eventType);
				/* 設定変更通知イベント送信処理結果チェック	*/
				if (EEPC_FUNC_RESULT_OK != evtSendResult)
				{
					DLOG_Debug("eepc_FileEvtChgSetting err %d", evtSendResult);
					result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
				}
				else
				{
					/* DO NOTHING */
				}
			}
			else
			{
				/* DO NOTHING */
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	設定変更通知イベント登録解除データ作成処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcClrChg_t *clrChg	設定変更通知登録解除情報	*/
/*					:	schar *writeFilePath	書き込みファイルパス		*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	設定変更通知イベント登録解除データ作成処理			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/11 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcFuncResult_t EEPC_FileCrtClrChgEvtData(EepcClrChg_t *clrChg, schar *writeFilePath)
{
	EepcFuncResult_t	result								= EEPC_FUNC_RESULT_OK;	/* 処理結果					*/
	ulong				loopCnt								= 0u;					/* ループカウンタ			*/
	sint				writeResult							= 0;					/* 書き込み結果				*/
	ulong				writeFileSize						= 0u;					/* ファイルの書き込みサイズ	*/
	schar				writeFileData[EEPC_TMP_BUF_SIZE]	= {EEPC_STR_DATA_END};	/* ファイルの書き込み領域	*/
	FILE				*writeFilePtr						= NULL;					/* ファイルポインタ			*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == clrChg) || (NULL == writeFilePath))
	{
		DLOG_Debug("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* ファイルオープン処理 */
		result = eepc_FileOpen(writeFilePath, "w", writeFilePtr);
		/* ファイルオープン結果チェック */
		if (EEPC_FUNC_RESULT_OK != result)
		{
			DLOG_Error("eepc_FileOpen err %d", result);
			result = EEPC_FUNC_RESULT_NG;	/* 処理失敗 */
		}
		else
		{
			/* 設定変更通知イベント情報登録解除ループ */
			for (loopCnt = 0; loopCnt < PROCESS_MAX ; loopCnt++)
			{
				/* ループ位置のポート番号チェック */
				if (g_eepcFileEvt01[loopCnt].receiver == clrChg->receiver)
				{
					/* グローバル変数(設定変更通知イベント情報ファイル用構造体)を更新 */
					g_eepcFileEvt01[loopCnt].chgDataFlg = 0u;
				}
				else
				{
					/* DO NOTHING */
				}

				/* 生成した値を基に、ファイルに書き出す文字列とする */
				sprintf(writeFileData,
					"%d,%x\n",
					g_eepcFileEvt01[loopCnt].receiver,
					(uint)g_eepcFileEvt01[loopCnt].chgDataFlg);
				
				/* ファイルサイズを取得する */
				writeFileSize = strnlen(writeFileData, sizeof(writeFileData));

				/* eepc_FileWriteによるファイル書き込み */
				writeResult = eepc_FileWrite(writeFileData, writeFileSize, writeFilePtr);
				if (EEPC_FUNC_RESULT_OK != writeResult)
				{
					DLOG_Error("eepc_FileWrite err %d", writeResult);
					result = EEPC_FUNC_RESULT_NG;	/* 処理失敗 */
					loopCnt = PROCESS_MAX;
				}
				else
				{
					/* DO NOTHING */
				}
			}
			
			/* ファイルクローズ処理	*/
			result = eepc_FileClose(writeFilePtr);
			/* ファイルクローズ処理結果チェック	*/
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Error("eepc_FileClose err %d", result);
				result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
			}
			else
			{
				/* DO NOTHING */
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社サーバ管理情報更新処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:	void												*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社サーバ管理情報更新処理						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/12 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
EepcFuncResult_t EEPC_FileUpdateSrvMngFile(void)
{
	EepcFuncResult_t	result										= EEPC_FUNC_RESULT_OK;	/* 処理結果								*/
	EepcFuncResult_t	closeResult									= EEPC_FUNC_RESULT_OK;	/* ファイルクローズ結果					*/
	uchar				loopCnt										= 0u;					/* ループカウンタ						*/
	schar				tmpPath[EEPC_TMP_FILE_PATH_SIZE]			= {EEPC_STR_DATA_END};	/* パスの一時領域						*/
	FILE				*filePtr									= NULL;					/* ファイルポインタ						*/
	uchar				updRtCerBankNo								= EEPC_RTCER_BNK_NO_0;	/* 更新後のルート証明書格納BankNo		*/
	schar				*dstRtCerDirPath							= NULL;					/* ルート証明書コピー先ディレクトリPATH	*/
	schar				tmpDirPath[EEPC_TMP_FILE_PATH_SIZE / 2u]	= {EEPC_STR_DATA_END};	/* ディレクトリPATH格納領域				*/
	schar				tmpFname[EEPC_TMP_FILE_PATH_SIZE / 2u]		= {EEPC_STR_DATA_END};	/* ファイル名格納領域					*/

	DEBUG_TRCIN();

	/* 更新対象を取得	*/
	result = eepc_FileFncGetUpdFilePath(tmpDirPath, (ulong)sizeof(tmpDirPath), tmpFname, (ulong)sizeof(tmpFname));
	if (EEPC_FUNC_RESULT_OK != result)
	{
		DLOG_Error("cannot get target URL File");
		result = EEPC_FUNC_RESULT_NG;
	}
	else if (EEPC_STR_DATA_END != tmpDirPath[0])
	{
		/* 更新対象のファイルパスを生成															*/
		/*	tmpDirPath[]とtmpFname[]を連結して、1つのファイルのフルパスをtmpPath[]に格納する 	*/
		/*	tmpPath[]が格納するデータの最大長は、USBメモリ内の電力会社URLファイルが対象になる	*/
		/*		"/media/usb_storage/eepc_SrvMng.csv"	(34byte)								*/
		/*			※tmpPath[]のサイズは、拡張性を考慮し、余裕を持たせる						*/
		/*	出力先のtmpPath[]のサイズを超過しない様に、tmpDirPath[]とtmpFname[]の合計サイズが	*/
		/*	tmpPath[]のサイズと同値になる様に、tmpDirPath[]とtmpFname[]のサイズは半分とする		*/
		/*		格納するデータの最大長は以下													*/
		/*			tmpDirPath[]	"/media/usb_storage/"	(19byte)							*/
		/*			tmpFname[]		"eepc_SrvMng.csv"		(15byte)							*/
		/*			※上記から、ファイルパスとファイル名の文字列長に大きな差は無い為、			*/
		/*			　それぞれの拡張性を考慮し、tmpPath[]のサイズの半分としている				*/
		sprintf(tmpPath, "%s%s", tmpDirPath, tmpFname);
		DLOG_Debug("Update URL File is(%s)", tmpPath);

		/* ファイルオープン処理 */
		result = eepc_FileOpen(tmpPath, "r", filePtr);
		/* ファイルオープン結果チェック */
		if (EEPC_FUNC_RESULT_OK != result)
		{
			DLOG_Error("eepc_FileOpen() err %s", tmpPath);
			result = EEPC_FUNC_RESULT_NG;
		}
		else if (TRUE == g_eepcFileUpdateCanselFlg)
		{
			/* ファイル更新中断を検出した場合 */
			DLOG_Error("rcv update cancel.");

			/* ファイルクローズ処理 */
			closeResult = eepc_FileClose(filePtr);
			if (EEPC_FUNC_RESULT_OK != closeResult)
			{
				DLOG_Error("eepc_FileClose() err %d", closeResult);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
		}
		else
		{
			/* 電力会社サーバ管理ファイル用mutex取得 */
			FRM_MutexLock(g_eepcMutexFileSrvMng);

			for (loopCnt = 0u; loopCnt < EEPC_SRV_LIST_MAX; loopCnt++)
			{
				memset(&(g_eepcFileSrvMng.srvInfList[loopCnt]), 0u, sizeof(EepcFileSrvInf_t));
			}

			/* 電力会社サーバ管理ファイル用mutex解放 */
			FRM_MutexUnLock(g_eepcMutexFileSrvMng);

			/* 電力会社URLファイルフォーマットチェック開始処理 */
			result = eepc_FileStartChkFrmtSrvUrlFile(filePtr);
			/* フォーマットチェック開始処理結果チェック(フォーマット異常) */
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Debug("eepc_FileStartChkFrmtSrvUrlFile() fmt err %d", result);

				/* 電力会社サーバ管理ファイル用構造体をロールバック */
				result = eepc_FileChkSrvMngFile();
				if (result != EEPC_FUNC_RESULT_OK)
				{
					DLOG_Debug("eepc_FileChkSrvMngFile() err %d", result);
					result = EEPC_FUNC_RESULT_NG;
				}
				else
				{
					/* DO NOTHING */
				}
			}
			/* ファイル更新中断フラグチェック */
			else if (TRUE == g_eepcFileUpdateCanselFlg)
			{
				/* ファイル更新中断を検出した場合 */
				DLOG_Debug("rcv update cancel.");

				/* 電力会社サーバ管理ファイル用構造体をロールバック */
				result = eepc_FileChkSrvMngFile();
				if (result != EEPC_FUNC_RESULT_OK)
				{
					DLOG_Debug("eepc_FileChkSrvMngFile() err %d", result);
					result = EEPC_FUNC_RESULT_NG;
				}
				else
				{
					/* DO NOTHING */
				}
			}
			else
			{
				/* ルート証明書ファイルコピー */
				/* 現在のBankと逆のBankを設定 */
				if (EEPC_RTCER_BNK_NO_0 == g_eepcFileSrvMng.rtCerBnkNo)
				{
					updRtCerBankNo = EEPC_RTCER_BNK_NO_1;
					dstRtCerDirPath = EEPC_RTCER_BNK1_DIR_PATH;
				}
				else
				{
					updRtCerBankNo = EEPC_RTCER_BNK_NO_0;
					dstRtCerDirPath = EEPC_RTCER_BNK0_DIR_PATH;
				}

				/* ルート証明書格納ディレクトリPATH生成	*/
				memset(tmpPath, 0x00, sizeof(tmpPath));
				snprintf(tmpPath, sizeof(tmpPath), "%s%s", tmpDirPath, EEPC_RTCER_DIR_NAME);

				/* コピー実行 */
				result = eepc_FileFncCpyRtCerFile(&g_eepcFileSrvMng, tmpPath, dstRtCerDirPath);
				if (EEPC_FUNC_RESULT_OK == result)
				{
					/* 電力会社サーバ管理ファイル用mutex取得 */
					FRM_MutexLock(g_eepcMutexFileSrvMng);

					/* 電力会社サーバ管理ファイル書き込み処理 */
					g_eepcFileSrvMng.rtCerBnkNo = updRtCerBankNo;
					result = eepc_FileFncWriteSrvMngFile(&g_eepcFileSrvMng);

					/* 電力会社サーバ管理ファイル用mutex解放 */
					FRM_MutexUnLock(g_eepcMutexFileSrvMng);
				}
				else
				{
					/* 電力会社サーバ管理ファイル用構造体をロールバック */
					result = eepc_FileChkSrvMngFile();
					if (EEPC_FUNC_RESULT_OK != result)
					{
						DLOG_Debug("eepc_FileChkSrvMngFile() err %d", result);
						result = EEPC_FUNC_RESULT_NG;
					}
					else
					{
						/* DO NOTHING */
					}
				}
			}

			/* ファイルクローズ処理 */
			closeResult = eepc_FileClose(filePtr);
			if (EEPC_FUNC_RESULT_OK != closeResult)
			{
				DLOG_Error("eepc_FileClose() err %d", closeResult);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
		}
	}
	else
	{
		/* 更新対象なし	*/
		DLOG_Debug("Current URL File is Newest");
	}

	DEBUG_TRCOUT();

	return result;
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:	ファイルオープン処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *filePath			ファイルパス				*/
/*					:	schar *fielAttr			ファイル属性				*/
/*					:	FILE *fp				ファイルポインタ			*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ファイルオープン処理								*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/06 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileOpen(schar *filePath, schar *fielAttr, FILE *fp)
{
	EepcFuncResult_t result = EEPC_FUNC_RESULT_OK;

	DEBUG_TRCIN();

	if ((NULL == filePath) || (NULL == fielAttr) || (NULL == fp))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* CMNを使用したファイルオープン */
		fp = CMN_FOPEN(filePath, fielAttr);
		if (NULL == (fp))
		{
			DLOG_Error("%s file open failed\n", filePath);
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	ファイルライト処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *writeData		ファイルパス				*/
/*					:	ulong dataSize			書き込みデータ				*/
/*					:	FILE *fp				ファイルサイズ				*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ファイルライト処理									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileWrite(schar *writeData, ulong dataSize, FILE *fp)
{
	EepcFuncResult_t	result = EEPC_FUNC_RESULT_OK;	/* 処理結果 */
	ulong				writeRes = 0;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == writeData) || (!dataSize))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;	/* 引数異常 */
	}
	else
	{
		/* ファイルに出力	*/
		writeRes = fwrite(writeData, EEPC_FILE_WRITE_NUMBER, dataSize, fp);
		if (dataSize != writeRes)
		{
			DLOG_Error("fwrite err %ld", writeRes);
			result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	ファイルクローズ処理								*/
/*--------------------------------------------------------------------------*/
/*	param			:	FILE *fp				ファイルポインタ			*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ファイルクローズ処理								*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/06 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileClose(FILE *fp)
{
	EepcFuncResult_t result = EEPC_FUNC_RESULT_OK;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if(NULL == fp)
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* CMNを使用したファイルクローズ */
		CMN_FCLOSE(fp);
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	行端変換処理										*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *chgData			変換元のデータ				*/
/*					:	uint chgDataSize		変換元データの文字列サイズ	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	行端変換処理										*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/31 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileChgLineEnd(schar *chgData, uint chgDataSize)
{
	EepcFuncResult_t	result		= EEPC_FUNC_RESULT_OK;	/* 処理結果					*/
	ulong				getDataLen	= 0u;					/* 読み出したデータサイズ	*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == chgData) || (!chgDataSize))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* パラメータから、実際のデータサイズを取得 */
		getDataLen = (ulong)strnlen(chgData, chgDataSize);
		/* 取得サイズが1文字以上であれば、		*/
		/* 終端の1byteをチェックし、変換する	*/
		if (EEPC_LINE_END1_LEN < getDataLen)
		{
			schar tmpChar = *(chgData + (getDataLen - EEPC_LINE_END1_LEN));
			if ((EEPC_FILE_END_SYMR == tmpChar) || (EEPC_FILE_END_SYMN == tmpChar))
			{
				*(chgData + (getDataLen - EEPC_LINE_END1_LEN)) = EEPC_STR_DATA_END;
			}
			else
			{
				/* DO NOTHING */
			}
		}
		else
		{
			/* DO NOTHING */
		}

		/* 取得サイズが2文字以上であれば、			*/
		/* 終端より2byte目をチェックし、変換する	*/
		if (EEPC_LINE_END2_LEN < getDataLen)
		{
			schar tmpChar = *(chgData + (getDataLen - EEPC_LINE_END2_LEN));
			if ((EEPC_FILE_END_SYMR == tmpChar) || (EEPC_FILE_END_SYMN == tmpChar))
			{
				*(chgData + (getDataLen - EEPC_LINE_END2_LEN)) = EEPC_STR_DATA_END;
			}
			else
			{
				/* DO NOTHING */
			}
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	フォーマットチェック処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *tmpFileData		ファイル内の1行分のデータ			*/
/*					:	uchar *bankNo			SSL通信情報							*/
/*					:	ulong tmpFileDataSize	ファイル内の1行分のデータサイズ		*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ルート証明書ファイル格納ディレクトリBankNo			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/31 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileChkFrmtBankNo(schar *tmpFileData, uchar *bankNo, ulong tmpFileDataSize)
{
	EepcFuncResult_t	result		= EEPC_FUNC_RESULT_OK;	/* 処理結果			*/
	uchar				tmpBankNo	= 0u;					/* BankNo			*/
	ulong				dataLen		= 0u;					/* データの文字列長	*/
	sint				tmpIsdRtn	= 0;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if((NULL == tmpFileData) || (NULL == bankNo) || (!tmpFileDataSize))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;	/* 引数異常 */
	}
	/* ファイルフォーマット(ルート証明書格納ディレクトリBankNo)チェック */
	else
	{
		/* BankNoの文字数をチェック */
		dataLen = (ulong)strnlen(tmpFileData, tmpFileDataSize);
		if((!dataLen) || (EEPC_SRV_MNG_BNK_NO_DATA_LEN < dataLen))
		{
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* BankNoの不正チェック */
			tmpIsdRtn = isdigit((sint)(*tmpFileData));
			if (!tmpIsdRtn)
			{
				DLOG_Error("bnk no err");
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}

			if (result == EEPC_FUNC_RESULT_OK)
			{
				/* 対象サーバ数は値管理の為、文字列を値に変換 */
				tmpBankNo = (uchar)atoi(tmpFileData);
				/* 値の範囲ををチェック */
				if (tmpBankNo == EEPC_RTCER_BNK_NO_0)
				{
					/* BankNoとして設定 */
					*bankNo = tmpBankNo;
				}
				/* 値が不適切の場合、フォーマットエラー */
				else
				{
					DLOG_Error("fmt chk bnk no err");
					result = EEPC_FUNC_RESULT_NG;
				}
			}
			else
			{
				/* DO NOTHING */
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	フォーマットチェック処理(バージョン情報)			*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *tmpFileData		ファイル内の1行分のデータ		*/
/*					:	ushort *srvInfVerMajor	メジャーバージョン				*/
/*					:	uchar *srvInfVerMiner	マイナーバージョン				*/
/*					:	ulong tmpFileDataSize	ファイル内の1行分のデータサイズ	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	フォーマットチェック処理(バージョン情報)			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/31 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileFncChkFrmtVer(schar *tmpFileData, ushort *srvInfVerMajor, uchar *srvInfVerMiner, ulong tmpFileDataSize)
{
	EepcFuncResult_t	result											= EEPC_FUNC_RESULT_OK;	/* 処理結果						*/
	ulong				dataLen											= 0u;					/* データの文字列長				*/
	ulong				loopCnt											= 0u;					/* ループカウンタ				*/
	schar				majorStr[EEPC_MAJOR_VER_LEN + EEPC_STR_END_LEN]	= {0};					/* メジャーバージョンの文字列	*/
	schar				minerStr[EEPC_MINER_VER_LEN + EEPC_STR_END_LEN]	= {0};					/* マイナーバージョンの文字列	*/
	sint				tmpIsdRtn										= 0;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == tmpFileData) || (NULL == srvInfVerMajor) ||
		(NULL == srvInfVerMiner) || (!tmpFileDataSize))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* ファイルフォーマット(バージョン情報)チェック	*/
		/* バージョン情報は文字列管理の為、文字数を取得	*/
		dataLen = (ulong)strnlen(tmpFileData, tmpFileDataSize);
		/* バージョン情報の文字数をチェック */
		ulong tmpLen = EEPC_SRV_MNG_VER_LEN - EEPC_STR_END_LEN;
		if (dataLen != tmpLen)
		{
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* バージョン情報の不正チェック */
			for (loopCnt = 0u; loopCnt < dataLen; loopCnt++)
			{
				/* メジャーバージョンとマイナーバージョンの区切りをチェック */
				if (EEPC_SPRIT_VER_POSITION == loopCnt)
				{
					sint cmpRtn = strncmp((tmpFileData + loopCnt), EEPC_SPRIT_DIV_VER, 1);
					if (cmpRtn)
					{
						DLOG_Error("fmt chk split err");
						/* 区切り文字が見つからない場合、処理異常としてループを抜ける	*/
						result = EEPC_FUNC_RESULT_NG;
						loopCnt = dataLen;
					}
					else
					{
						/* DO NOTHING */
					}
				}
				else
				{
					/* 現在参照する文字が値であることをチェックする	*/
					tmpIsdRtn = isdigit((sint)*(tmpFileData + loopCnt));
					if (!tmpIsdRtn)
					{
						DLOG_Error("fmt chk ver err");
						/* 一つでも間違いであれば、処理異常としてループを抜ける	*/
						result = EEPC_FUNC_RESULT_NG;
						loopCnt = dataLen;
					}
					else
					{
						/* DO NOTHING */
					}
				}
			}

			if (EEPC_FUNC_RESULT_OK == result)
			{
				/* メジャーバージョンの文字列を取得 */
				strncpy(majorStr, tmpFileData, EEPC_MAJOR_VER_LEN);
				/* マイナーバージョンの文字列を取得 */
				strncpy(minerStr, (tmpFileData + (EEPC_MAJOR_VER_LEN + EEPC_STR_END_LEN)), EEPC_MINER_VER_LEN);
				/* バージョン情報として設定 */
				*srvInfVerMajor = (ushort)atoi(majorStr);
				*srvInfVerMiner = (uchar)atoi(minerStr);
			}
			else
			{
				/* DO NOTHING */
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	フォーマットチェック処理(対象サーバ数)				*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *tmpFileData		ファイル内の1行分のデータ	*/
/*					:	uchar *srvCnt			電力会社サーバの総数		*/
/*					:	ulong tmpFileDataSize	ファイル内の1行分のデータサイズ	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	フォーマットチェック処理(対象サーバ数)				*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/31 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileChkFrmtSrvCnt(schar *tmpFileData, uchar *srvCnt, ulong tmpFileDataSize)
{
	EepcFuncResult_t	result		= EEPC_FUNC_RESULT_OK;	/* 処理結果			*/
	sint				tmpSrvCnt	= 0;					/* 対象サーバ数		*/
	ulong				loopCnt		= 0u;					/* ループカウンタ	*/
	ulong				dataLen		= 0u;					/* データの文字列長	*/
	sint				tmpIsdRtn	= 0;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == tmpFileData) || (NULL == srvCnt) || (!tmpFileDataSize))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* ファイルフォーマット(対象サーバ数)チェック */
		/* 対象サーバ数の文字数を取得 */
		dataLen = (ulong)strnlen(tmpFileData, tmpFileDataSize);
		/* 対象サーバ数の文字数をチェック */
		if ((!dataLen) || (EEPC_SRV_MNG_CNT_LEN < dataLen))
		{
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* 対象サーバ数の不正チェック */
			for (loopCnt = 0; loopCnt < dataLen; loopCnt++)
			{
				/* 現在参照する文字が値であることをチェックする */
				tmpIsdRtn = isdigit((sint)*(tmpFileData + loopCnt));
				if (!tmpIsdRtn)
				{
					DLOG_Error("srv cnt err");
					/* 一つでも間違いであれば、処理異常としてループを抜ける */
					result = EEPC_FUNC_RESULT_NG;
					loopCnt = dataLen;
				}
				else
				{
					/* DO NOTHING */
				}
			}

			if (EEPC_FUNC_RESULT_OK == result)
			{
				/* 対象サーバ数は値管理の為、文字列を値に変換 */
				tmpSrvCnt = atoi(tmpFileData);
				/* ファイル3行目(対象サーバ数)をチェック */
				if ((EEPC_SRV_LIST_MIN <= tmpSrvCnt) && (tmpSrvCnt <= EEPC_SRV_LIST_MAX))
				{
					/* 電力会社サーバの総数として設定 */
					*srvCnt = (uchar)tmpSrvCnt;
				}
				/* 値が不適切の場合、フォーマットエラー */
				else
				{
					DLOG_Error("fmt chk srv cnt err");
					result = EEPC_FUNC_RESULT_NG;
				}
			}
			else
			{
				/* DO NOTHING */
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	フォーマットチェック処理(送配電事業者コード)		*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *tmpFileData		ファイル内の1行分のデータ	*/
/*					:	uchar *corpDiv			設定先の電力会社の区分		*/
/*					:	ulong maxDiv			電力会社の区分(最大値)		*/
/*					:	ulong tmpFileDataSize	ファイル内の1行分のデータサイズ	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	フォーマットチェック処理(送配電事業者コード)		*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileChkFrmtCorpDiv(schar *tmpFileData, uchar *corpDiv, ulong maxDiv, ulong tmpFileDataSize)
{
	EepcFuncResult_t	result		= EEPC_FUNC_RESULT_OK;	/* 処理結果				*/
	uchar				tmpCorpDiv	= 0u;					/* 送配電事業者コード	*/
	ulong				dataLen		= 0u;					/* データの文字列長		*/
	ulong				loopCnt		= 0u;					/* ループカウンタ		*/
	sint				tmpIsdRtn	= 0;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if((NULL == tmpFileData) || (NULL == corpDiv) ||
		(EEPC_CORP_DIV_MAX < maxDiv) || (!tmpFileDataSize))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* データの文字列長を取得 */
		dataLen = (ulong)strnlen(tmpFileData, tmpFileDataSize);
		if((!dataLen) || (EEPC_CORP_DIV_LEN < dataLen))
		{
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* ファイルフォーマット(送配電事業者コード)チェック */
			/* 送配電事業者コードの不正チェック */
			for(loopCnt = 0u; loopCnt < dataLen; loopCnt++)
			{
				/* 現在参照する文字が値であることをチェックする */
				tmpIsdRtn = isdigit((sint)*(tmpFileData + loopCnt));
				if(!tmpIsdRtn)
				{
					DLOG_Error("corp div err");
					/* 一つでも間違いであれば、処理異常としてループを抜ける */
					result = EEPC_FUNC_RESULT_NG;
					loopCnt = dataLen;
				}
				else
				{
					/* DO NOTHING */
				}
			}

			if (EEPC_FUNC_RESULT_OK == result)
			{
				/* 文字列データから送配電事業者コードの値に変換	*/
				tmpCorpDiv = (uchar)atoi(tmpFileData);
				/* 設定する送配電事業者コードの最大値チェック */
				if (tmpCorpDiv <= EEPC_CORP_DIV_MAX)
				{
					/* 送配電事業者コードとして設定	*/
					*corpDiv = tmpCorpDiv;
				}
				/* 値が不適切の場合、フォーマットエラー */
				else
				{
					DLOG_Error("fmt chk corp div err");
					result = EEPC_FUNC_RESULT_NG;
				}
			}
			else
			{
				/* DO NOTHING */
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	フォーマットチェック処理(文字列情報)				*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *tmpFileData		ファイル内の1行分のデータ	*/
/*					:	schar *setStr			設定先の文字列				*/
/*					:	uint maxSize			設定先の文字列サイズ(最大値)*/
/*					:	ulong tmpFileDataSize	ファイル内の1行分のデータサイズ	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	フォーマットチェック処理(文字列情報)				*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileChkFrmtSrvInfStr(schar *tmpFileData, schar *setStr, uint maxSize, ulong tmpFileDataSize)
{
	EepcFuncResult_t	result		= EEPC_FUNC_RESULT_OK;	/* 処理結果		*/
	uint				crntSize	= 0u;					/* 設定サイズ 	*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL ==tmpFileData) || (NULL ==setStr) || (!maxSize) || (!tmpFileDataSize))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;	/* 引数異常 */
	}
	else
	{
		/* 設定サイズの取得 */
		crntSize = (uint)strnlen(tmpFileData, tmpFileDataSize);

		/* 設定するサイズの最大値チェック */
		if((!crntSize) || (maxSize < crntSize))
		{
			DLOG_Error("fmt chk srv inf string err");
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* 前回設定した値をクリア */
			memset(setStr, 0, maxSize);
			/* 設定値を基に文字列をコピー */
			strncpy(setStr, tmpFileData, crntSize);
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	フォーマットチェック処理(ポート番号)				*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *tmpFileData		ファイル内の1行分のデータ	*/
/*					:	ushort *setPort			設定先のポート番号			*/
/*					:	ulong maxPort			サーバのポート番号最大値	*/
/*					:	ulong tmpFileDataSize	ファイル内の1行分のデータサイズ	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	フォーマットチェック処理(ポート番号)				*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileChkFrmtSrvInfPort(schar *tmpFileData, ushort *setPort, ulong maxPort, ulong tmpFileDataSize)
{
	EepcFuncResult_t	result		= EEPC_FUNC_RESULT_OK;	/* 処理結果				*/
	ulong				dataLen		= 0u;					/* データの文字列長		*/
	ulong				loopCnt		= 0u;					/* ループカウンタ		*/
	ulong				tmpSrvPort	= 0u;					/* ポート番号の一時領域	*/
	sint				tmpIsdRtn	= 0;

	DEBUG_TRCIN();

	/* パラメータチェック	*/
	if ((NULL ==tmpFileData) || (NULL ==setPort) || (!maxPort) || (!tmpFileDataSize))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;	/* 引数異常	*/
	}
	else
	{
		/* データの文字列長を取得	*/
		dataLen = (ulong)strnlen(tmpFileData, tmpFileDataSize);
		/* データの文字列長をチェック	*/
		if ((!dataLen) || (EEPC_SRV_MNG_PORT_LEN < dataLen))
		{
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* ファイルフォーマット(ポート番号)チェック	*/
			/* ポート番号の不正チェック	*/
			for (loopCnt = 0u; loopCnt < dataLen; loopCnt++)
			{
				/* 現在参照する文字が値であることをチェックする	*/
				tmpIsdRtn = isdigit((sint)*(tmpFileData + loopCnt));
				if (!tmpIsdRtn)
				{
					DLOG_Error("port err");
					/* 一つでも間違いであれば、処理異常としてループを抜ける	*/
					result = EEPC_FUNC_RESULT_NG;
					loopCnt = dataLen;
				}
				else
				{
					/* DO NOTHING */
				}
			}

			if (EEPC_FUNC_RESULT_OK == result)
			{
				/* 文字列データからポート番号の値に変換 */
				tmpSrvPort = (ulong)atoi(tmpFileData);
				/* 設定するポートの最大値チェック */
				if (tmpSrvPort <= maxPort)
				{
					*setPort = (ushort)tmpSrvPort;
					
				}
				else
				{
					DLOG_Error("fmt chk srv inf port err");
					/* 値が不適切の場合、フォーマットエラー	*/
					result = EEPC_FUNC_RESULT_NG;
				}
			}
			else
			{
				/* DO NOTHING */
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	フォーマットチェック処理(電力会社情報)				*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *tmpFileData			ファイル内の1行分のデータ*/
/*					:	EepcFileSrvInf_t *srvInf	電力会社情報			*/
/*					:	ulong tmpFileDataSize		ファイル内の1行分のデータサイズ	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	フォーマットチェック処理(電力会社情報)				*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileChkFrmtSrvInf(schar *tmpFileData, EepcFileSrvInf_t *srvInf, ulong tmpFileDataSize)
{
	EepcFuncResult_t	result								= EEPC_FUNC_RESULT_OK;		/* 処理結果								*/
	ulong				loopCnt								= 0u;						/* ループカウンタ						*/
	ulong				offsetSize							= 0u;						/* ファイル内データのオフセットサイズ	*/
	schar				*spritPtr							= NULL;						/* 区切り文字確認箇所					*/
	ulong				dataSizeList[EEPC_CORP_DATA_TYPE_MAX];							/* 電力会社情報毎のデータサイズリスト	*/
	schar				srvInfList[EEPC_CORP_DATA_TYPE_MAX][EEPC_SRV_MNG_DATA_LEN_MAX];	/* 電力会社情報毎のデータリスト			*/

	DEBUG_TRCIN();

	for (loopCnt = 0u; loopCnt < EEPC_CORP_DATA_TYPE_MAX; loopCnt++)
	{
		dataSizeList[loopCnt] = 0u;
		memset(srvInfList[loopCnt], 0, EEPC_SRV_MNG_DATA_LEN_MAX);
	}

	/* パラメータチェック */
	if ((NULL == tmpFileData) || (NULL == srvInf) || (!tmpFileDataSize))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* ルート証明書ファイル名を初期化 */
		memset(srvInf->rtCerFileStr, EEPC_STR_DATA_END, EEPC_RTCER_NAME_LEN);
		/* 電力会社情報毎のデータ取得ループ	*/
		for (loopCnt = 0; loopCnt < EEPC_CORP_DATA_TYPE_MAX; loopCnt++)
		{
			/* 区切り文字を探す	*/
			spritPtr = strstr((tmpFileData + offsetSize), EEPC_SPRIT_FILE_CSV);
			/* 区切り文字が見つからない場合	*/
			if ((NULL == spritPtr) && (loopCnt < EEPC_CORP_DATA_TYPE_RTCER_NAME))
			{
				result = EEPC_FUNC_RESULT_NG;
				loopCnt = EEPC_CORP_DATA_TYPE_MAX;
			}
			else
			{
				/* DO NOTHING */
			}

			/* 行端は区切り文字が無い為、アドレスの取得方法を変える	*/
			if (EEPC_CORP_DATA_TYPE_RTCER_NAME == loopCnt)
			{
				/* 行端のオフセットを取得	*/
				spritPtr = tmpFileData + strnlen(tmpFileData, tmpFileDataSize);
			}
			else
			{
				/* DO NOTHING */
			}

			/* 電力会社情報毎のデータサイズを取得	*/
			dataSizeList[loopCnt] = (ulong)(spritPtr - (tmpFileData + offsetSize));
			/* データサイズをチェック	*/
			if (EEPC_SRV_MNG_DATA_LEN_MAX < dataSizeList[loopCnt])
			{
				dataSizeList[loopCnt] = EEPC_SRV_MNG_DATA_LEN_MAX;
			}
			else
			{
				/* DO NOTHING */
			}

			/* 電力会社毎のデータを取得	*/
			strncpy(srvInfList[loopCnt], (tmpFileData + offsetSize), dataSizeList[loopCnt]);
			/* 取得したデータのNULL文字チェック	*/
			if (EEPC_STR_DATA_END == srvInfList[loopCnt][0])
			{
				result = EEPC_FUNC_RESULT_NG;
				loopCnt = EEPC_CORP_DATA_TYPE_MAX;
			}
			else
			{
				/* DO NOTHING */
			}

			/* オフセットを更新	*/
			offsetSize += (dataSizeList[loopCnt] + EEPC_STR_END_LEN);
		}

		DLOG_Info("loopCnt:%ld", loopCnt);
		/* データ取得を正常に終えている場合	*/
		if (EEPC_FUNC_RESULT_OK == result)
		{
			/* 送配電事業者コードのチェック	*/
			/* フォーマットチェック処理(送配電事業者コード) */
			result = eepc_FileChkFrmtCorpDiv(
				srvInfList[EEPC_CORP_DATA_TYPE_DIV],
				&(srvInf->corpDiv),
				EEPC_CORP_DIV_MAX,
				EEPC_SRV_MNG_DATA_LEN_MAX);
		}
		else
		{
			/* DO NOTHING */
		}

		if (EEPC_FUNC_RESULT_OK == result)
		{
			/* 電力会社名のチェック */
			/* フォーマットチェック処理(文字列情報) */
			result = eepc_FileChkFrmtSrvInfStr(
				srvInfList[EEPC_CORP_DATA_TYPE_NAME],
				srvInf->srvNameStr,
				(EEPC_SRV_NAME_LEN - EEPC_STR_END_LEN),
				EEPC_SRV_MNG_DATA_LEN_MAX);

		}
		else
		{
			/* DO NOTHING */
		}

		if (EEPC_FUNC_RESULT_OK == result)
		{
			/* 電力会社URLのチェック */
			/* フォーマットチェック処理(文字列情報) */
			result = eepc_FileChkFrmtSrvInfStr(
				srvInfList[EEPC_CORP_DATA_TYPE_CORP_URL],
				srvInf->srvUrlStr,
				(EEPC_SRV_NAME_LEN - EEPC_STR_END_LEN),
				EEPC_SRV_MNG_DATA_LEN_MAX);
		}
		else
		{
			/* DO NOTHING */
		}

		if (EEPC_FUNC_RESULT_OK == result)
		{
			/* 電力会社サーバのポート番号のチェック	*/
			/* フォーマットチェック処理(ポート番号)	*/
			result = eepc_FileChkFrmtSrvInfPort(
				srvInfList[EEPC_CORP_DATA_TYPE_CORP_PORT],
				&(srvInf->srvPort),
				EEPC_SRV_MNG_PORT_MAX,
				EEPC_SRV_MNG_DATA_LEN_MAX);
		}
		else
		{
			/* DO NOTHING */
		}

		if (EEPC_FUNC_RESULT_OK == result)
		{
			/* 電力会社指定のNTPサーバFQDN	*/
			/* フォーマットチェック処理(文字列情報)	*/
			result = eepc_FileChkFrmtSrvInfStr(
				srvInfList[EEPC_CORP_DATA_TYPE_NTP_FQDN],
				srvInf->ntpFqdnStr,
				(EEPC_SRV_NAME_LEN - EEPC_STR_END_LEN),
				EEPC_SRV_MNG_DATA_LEN_MAX);
		}
		else
		{
			/* DO NOTHING */
		}

		if (EEPC_FUNC_RESULT_OK == result)
		{
			/* 電力会社指定のNTPサーバのポート番号	*/
			/* フォーマットチェック処理(ポート番号)	*/
			result = eepc_FileChkFrmtSrvInfPort(
				srvInfList[EEPC_CORP_DATA_TYPE_NTP_PORT],
				&(srvInf->ntpSrvPort),
				EEPC_SRV_MNG_PORT_MAX,
				EEPC_SRV_MNG_DATA_LEN_MAX);
		}
		else
		{
			/* DO NOTHING */
		}

		if (EEPC_FUNC_RESULT_OK == result)
		{
			/* ルート証明書ファイル名	*/
			/* フォーマットチェック処理(文字列情報)	*/
			result = eepc_FileChkFrmtSrvInfStr(
				srvInfList[EEPC_CORP_DATA_TYPE_RTCER_NAME],
				srvInf->rtCerFileStr,
				(EEPC_SRV_NAME_LEN - EEPC_STR_END_LEN),
				EEPC_SRV_MNG_DATA_LEN_MAX);
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社サーバ管理ファイルフォーマットチェック処理	*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *tmpFileData			ファイル内の1行分のデータ	*/
/*					:	ulong fileLineCnt			参照中のファイル行数	*/
/*					:	EepcFileSrvMng_t *srvMng	電力会社サーバ管理ファイル用構造体	*/
/*					:	ulong tmpFileDataSize		ファイル内の1行分のデータサイズ	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社サーバ管理ファイルフォーマットチェック処理	*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/31 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileChkFrmtSrvMngFile(schar *tmpFileData, ulong fileLineCnt, EepcFileSrvMng_t *srvMng, ulong tmpFileDataSize)
{
	EepcFuncResult_t result	= EEPC_FUNC_RESULT_OK;	/* 処理結果 */

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == tmpFileData) || (EEPC_SRV_MNG_FILE_LINE_MAX <= fileLineCnt) ||
		(NULL == srvMng) || (!tmpFileDataSize))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	/* データフォーマットチェック */
	else
	{
		/* ファイル内の参照行数により、フォーマットチェック方法を変える */
		/* 1行目 ルート証明書ファイル格納ディレクトリBankNo */
		if (EEPC_SRV_MNG_BANKNO_LINE == fileLineCnt)
		{
			/* フォーマットチェック処理(ルート証明書ファイル格納ディレクトリBankNo) */
			result = eepc_FileChkFrmtBankNo(tmpFileData,
										&(srvMng->rtCerBnkNo),
										tmpFileDataSize);
		}
		/* 2行目 バージョン情報 */
		else if (EEPC_SRV_MNG_VER_LINE == fileLineCnt)
		{
			/* フォーマットチェック処理(バージョン情報) */
			result = eepc_FileFncChkFrmtVer(
				tmpFileData,
				&(srvMng->srvInfVerMajor),
				&(srvMng->srvInfVerMiner),
				tmpFileDataSize);
		}
		/* 3行目 対象サーバ数 */
		else if (EEPC_SRV_MNG_CNT_LINE == fileLineCnt)
		{
			/* フォーマットチェック処理(対象サーバ数) */
			result = eepc_FileChkFrmtSrvCnt(
				tmpFileData,
				&(srvMng->srvCnt),
				tmpFileDataSize);
		}
		/* 4行目 ～ 13行目:電力会社1社の情報 */
		else if (EEPC_SRV_MNG_INF_LINE <= fileLineCnt)
		{
			/* フォーマットチェック処理(電力会社情報) */
			result = eepc_FileChkFrmtSrvInf(
				tmpFileData,
				&(srvMng->srvInfList[fileLineCnt - EEPC_SRV_MNG_INF_LINE]),
				tmpFileDataSize);
		}
		else
		{
			/* ファイル範囲外エラー */
			DLOG_Warn("fileLineCnt Over %ld", fileLineCnt);
			result = EEPC_FUNC_RESULT_NG;
		}

		if (EEPC_FUNC_RESULT_OK != result)
		{
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社サーバ管理ファイルフォーマットチェック開始処理*/
/*--------------------------------------------------------------------------*/
/*	param			:	FILE *fp				ファイルポインタ			*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社サーバ管理ファイルフォーマットチェック開始処理*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/31 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileStartChkFrmtSrvMngFile(FILE *fp)
{
	EepcFuncResult_t	result							= EEPC_FUNC_RESULT_OK;	/* 処理結果								*/
	ulong				fileLineCnt						= 0u;					/* ファイル内行数のカウンタ				*/
	ulong				loopCnt							= 0u;					/* ループカウンタ						*/
	schar				tmpFileData[EEPC_TMP_BUF_SIZE]	= {"\0"};				/* ファイルデータの一時領域				*/
	schar				*tmpBuf							= NULL;
	bool_t 				loopFlag						= TRUE;

	DEBUG_TRCIN();

	if (NULL == fp)
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* 電力会社サーバ管理ファイル用mutex取得 */
		FRM_MutexLock(g_eepcMutexFileSrvMng);

		/* フォーマットチェック毎に、ファイルポインタを先頭に戻す */
		g_eepcFileSrvMng.srvCnt			= 0u;	/* 電力会社サーバの総数								*/
		g_eepcFileSrvMng.srvInfVerMajor	= 0u;	/* 電力会社サーバ管理ファイルのメジャーバージョン	*/
		g_eepcFileSrvMng.srvInfVerMiner	= 0u;	/* 電力会社サーバ管理ファイルのマイナーバージョン	*/
		g_eepcFileSrvMng.rtCerBnkNo		= 0u;	/* ルート証明書ファイル格納ディレクトリBankNo		*/
		memset(&(g_eepcFileSrvMng.srvInfList[0]), 0,
				sizeof(g_eepcFileSrvMng.srvInfList));	/* 電力会社情報のリスト	*/

		while (loopFlag)
		{
			tmpBuf = fgets(&(tmpFileData[0]), EEPC_TMP_BUF_SIZE, fp);
			if (NULL == tmpBuf)
			{
				result = EEPC_FUNC_RESULT_NG;
				loopFlag = FALSE;
			}
			else
			{
				/* DO NOTHING */
			}

			if (EEPC_FUNC_RESULT_OK == result)
			{
				result = eepc_FileChgLineEnd(tmpFileData, sizeof(tmpFileData));
				if (EEPC_FUNC_RESULT_OK != result)
				{
					result = EEPC_FUNC_RESULT_NG;
					loopFlag = FALSE;
				}
				else
				{
					result = eepc_FileChkFrmtSrvMngFile(tmpFileData,
														fileLineCnt, 
														&g_eepcFileSrvMng,
														sizeof(tmpFileData));
					if (EEPC_FUNC_RESULT_OK == result)
					{
						memset(tmpFileData, 0, sizeof(tmpFileData));
						fileLineCnt++;
					}
					else
					{
						DLOG_Error("fileLineCnt:%ld", fileLineCnt);
						result = EEPC_FUNC_RESULT_NG;
						loopFlag = FALSE;
					}
				}
			}
			else
			{
				/* DO NOTHING */
			}
			
		}

		if (EEPC_FUNC_RESULT_OK != result)
		{
			DLOG_Error("srv mng fmt NG");
		}
		else
		{
			/* CT用ログ出力	*/
			DLOG_Info("srv mng fmt OK");
			DLOG_Info("srvCnt:%d", g_eepcFileSrvMng.srvCnt);
			DLOG_Info("srvRtryCntFixSch:%ld", g_eepcFileSrvMng.srvRtryCntFixSch);
			DLOG_Info("srvRtryIntvlFixSch:%ld", g_eepcFileSrvMng.srvRtryIntvlFixSch);
			DLOG_Info("srvRtryCntUpdtSch:%ld", g_eepcFileSrvMng.srvRtryCntUpdtSch);
			DLOG_Info("srvRtryIntvlUpdtSch:%ld", g_eepcFileSrvMng.srvRtryIntvlUpdtSch);
			DLOG_Info("ntpRtryCntTimSync:%ld", g_eepcFileSrvMng.ntpRtryCntTimSync);
			DLOG_Info("ntpRtryIntvlTimSync:%ld", g_eepcFileSrvMng.ntpRtryIntvlTimSync);
			DLOG_Info("ntpRtryOvrIntvlTimSync:%ld", g_eepcFileSrvMng.ntpRtryOvrIntvlTimSync);
			DLOG_Info("srvInfVerMajor:%d", g_eepcFileSrvMng.srvInfVerMajor);
			DLOG_Info("srvInfVerMiner:%d", g_eepcFileSrvMng.srvInfVerMiner);

			for (loopCnt = 0; loopCnt < g_eepcFileSrvMng.srvCnt; loopCnt++)
			{
				DLOG_Info("srvInfList[%ld].corpDiv:%d\n", loopCnt,
					g_eepcFileSrvMng.srvInfList[loopCnt].corpDiv);
				DLOG_Info("srvInfList[%ld].srvPort:%d\n", loopCnt,
					g_eepcFileSrvMng.srvInfList[loopCnt].srvPort);
				DLOG_Info("srvInfList[%ld].ntpSrvPort:%d\n", loopCnt,
					g_eepcFileSrvMng.srvInfList[loopCnt].ntpSrvPort);
				DLOG_Info("srvInfList[%ld].srvNameStr:%s\n", loopCnt,
					&(g_eepcFileSrvMng.srvInfList[loopCnt].srvNameStr[0]));
				DLOG_Info("srvInfList[%ld].srvUrlStr:%s\n", loopCnt,
					&(g_eepcFileSrvMng.srvInfList[loopCnt].srvUrlStr[0]));
				DLOG_Info("srvInfList[%ld].ntpFqdnStr:%s\n", loopCnt,
					&(g_eepcFileSrvMng.srvInfList[loopCnt].ntpFqdnStr[0]));
				DLOG_Info("srvInfList[%ld].rtCerFileStr:%s\n", loopCnt,
					&(g_eepcFileSrvMng.srvInfList[loopCnt].rtCerFileStr[0]));
			}
		}
		/* 電力会社サーバ管理ファイル用mutex解放 */
		FRM_MutexUnLock(g_eepcMutexFileSrvMng);
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	ファイル存在チェック処理							*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *dirPath			チェック対象のディレクトリパ	*/
/*					:	schar *fileName			チェック対象のファイル名	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ファイル存在チェック処理							*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileChkFileExist(schar *dirPath, schar *fileName)
{
	EepcFuncResult_t	result								= EEPC_FUNC_RESULT_OK;	/* 処理結果					*/
	schar				filePath[EEPC_TMP_FILE_PATH_SIZE]	= {EEPC_STR_DATA_END};	/* 検索対象ファイルのPATH	*/
	sint				statRet								= 0;
	struct stat			statInfo;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if((NULL == dirPath) || (NULL == fileName))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* ファイルPATH生成 */
		memset(filePath, 0x00, sizeof(filePath));
		snprintf(filePath, sizeof(filePath), "%s%s", dirPath, fileName);
		/* ファイル存在チェック */
		memset(&statInfo, 0x00, sizeof(statInfo));
		statRet = stat(filePath, &statInfo);
		if(statRet)
		{
			DLOG_Error("cannot find %s", filePath);
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	ルート証明書ファイル存在チェック処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcFileSrvMng_t *srvMng	電力会社サーバ管理情報	*/
/*					:	schar *dirPath				ルート証明書格納ディレクトリのパス	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ルート証明書ファイル存在チェック処理				*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileChkRtCerFileExist(EepcFileSrvMng_t *srvMng, schar *dirPath)
{
	EepcFuncResult_t	result	= EEPC_FUNC_RESULT_OK;	/* 処理結果					*/
	sint 				cnt		= 0;					/* ループカウンタ			*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == srvMng) || (NULL == dirPath))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* 管理情報にあるルート証明書ファイルが指定ディレクトリ内に存在するかチェック	*/
		for (cnt = 0; cnt < srvMng->srvCnt; cnt++)
		{
			/* ファイル存在チェック処理	*/
			result = eepc_FileChkFileExist(dirPath,
				srvMng->srvInfList[cnt].rtCerFileStr);
			/* チェック結果を確認	*/
			if (result != EEPC_FUNC_RESULT_OK)
			{
				result = EEPC_FUNC_RESULT_NG;
				cnt = srvMng->srvCnt;
			}
			else
			{
				/* DO NOTHING */
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	ファイルコピー処理									*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *fileName			コピーファイル名			*/
/*					:	schar *srcDirPath		コピー元ディレクトリのパス	*/
/*					:	schar *dstDirPath		コピー先ディレクトリのパス	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ファイルコピー処理									*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileCpyFile(schar *fileName, schar *srcDirPath, schar *dstDirPath)
{
	EepcFuncResult_t	result									= EEPC_FUNC_RESULT_OK;	/* 処理結果				*/
	schar				srcFilePath[EEPC_TMP_FILE_PATH_SIZE]	= {EEPC_STR_DATA_END};	/* コピー元ファイルPATH	*/
	schar				dstFilePath[EEPC_TMP_FILE_PATH_SIZE]	= {EEPC_STR_DATA_END};	/* コピー先ファイルPATH	*/
	schar				systemStr[EEPC_TMP_BUF_SIZE]			= {EEPC_STR_DATA_END};	/* コピーコマンド文字列	*/
	FILE				*fp										= NULL;					/* ファイルポインタ		*/
	sint				ret										= 0;

	DEBUG_TRCOUT();

	/* パラメータチェック */
	if ((NULL == fileName) || (NULL == srcDirPath) || (NULL == dstDirPath))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* コピー元ファイル名生成	*/
		memset(srcFilePath, 0x00, sizeof(srcFilePath));
		snprintf(srcFilePath, sizeof(srcFilePath), "%s%s", srcDirPath, fileName);
		/* 指定ディレクトリへコピー	*/
		memset(systemStr, 0x00, sizeof(systemStr));
		snprintf(systemStr, sizeof(systemStr), "/bin/cp %s %s", srcFilePath, dstDirPath);
		ret = system(systemStr);
		if (ret)
		{
			DLOG_Error("system(%s) fail", systemStr);
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* 共通関数のファイルOPEN & CLOSEでfsyncさせる	*/
			memset(dstFilePath, 0x00, sizeof(dstFilePath));
			snprintf(dstFilePath, sizeof(dstFilePath), "%s%s", dstDirPath, fileName);
			result = eepc_FileOpen(dstFilePath, "r", fp);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Error("eepc_FileOpen(%s) fail", dstFilePath);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				eepc_FileClose(fp);
				/* コピー元とコピー先の内容が同一かチェック	*/
				memset(systemStr, 0x00, sizeof(systemStr));
				snprintf(systemStr, sizeof(systemStr), "cmp %s %s", srcFilePath, dstFilePath);
				ret = system(systemStr);
				if (ret)
				{
					DLOG_Error("system(%s) fail", systemStr);
					result = EEPC_FUNC_RESULT_NG;
				}
				else
				{
					/* DO NOTHING */
				}
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	ルート証明書ファイルコピー処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcFileSrvMng_t *srvMng	電力会社サーバ管理情報	*/
/*					:	schar *srcDirPath			コピー元ディレクトリのパス	*/
/*					:	schar *dstDirPath			コピー先ディレクトリのパス	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	ルート証明書ファイルコピー処理						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileFncCpyRtCerFile(EepcFileSrvMng_t *srvMng, schar *srcDirPath, schar *dstDirPath)
{
	EepcFuncResult_t	result							= EEPC_FUNC_RESULT_OK;	/* 処理結果				*/
	sint				cnt								= 0;					/* ループカウンタ		*/
	schar				systemStr[EEPC_TMP_BUF_SIZE]	= {EEPC_STR_DATA_END};	/* コピーコマンド文字列	*/
	sint				ret								= 0;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == srvMng) || (NULL == srcDirPath) || (NULL == dstDirPath))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* コピー時間測定開始位置	*/
		DLOG_Info("eepc_lcl_fnc_cpyRtCerFile() cpy start");
		DLOG_Info("root cer file cpy(%s to %s)", srcDirPath, dstDirPath);
		/* コピー先ディレクトリ内を一旦クリア	*/
		memset(systemStr, 0x00, sizeof(systemStr));
		snprintf(systemStr, sizeof(systemStr), "rm -rf %s*", dstDirPath);
		ret = system(systemStr);
		if (ret)
		{
			DLOG_Error("system(%s) fail", systemStr);
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* 管理情報にあるルート証明書ファイルを指定ディレクトリへコピー	*/
			for (cnt = 0; cnt < srvMng->srvCnt; cnt++)
			{
				/* コピー実行	*/
				result = eepc_FileCpyFile(srvMng->srvInfList[cnt].rtCerFileStr, srcDirPath, dstDirPath);
				if(result != EEPC_FUNC_RESULT_OK)
				{
					DLOG_Error("eepc_lcl_fnc_cpyFile err");
					cnt = srvMng->srvCnt;
				}
				else
				{
					/* DO NOTHING */
				}
			}
		}

		/* コピー時間測定終了位置	*/
		DLOG_Info("eepc_lcl_fnc_cpyRtCerFile() cpy end");
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社サーバ管理ファイル書き込み処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcFileSrvMng_t *srvMng	電力会社サーバ管理ファイル用構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社サーバ管理ファイル書き込み処理				*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileFncWriteSrvMngFile(EepcFileSrvMng_t *srvMng)
{
	EepcFuncResult_t	result									= EEPC_FUNC_RESULT_OK;	/* 処理結果						*/
	EepcFuncResult_t	closeResult								= EEPC_FUNC_RESULT_OK;	/* ファイルクローズ結果			*/
	EepcFuncResult_t	writeResult								= EEPC_FUNC_RESULT_OK;	/* 書き込み結果					*/
	ulong				tmpStrLen								= 0u;
	ulong				loopCnt									= 0u;					/* ループカウンタ				*/
	ulong				writeFileSize							= 0u;					/* ファイルの書き込みサイズ		*/
	schar				writeFilePath[EEPC_TMP_FILE_PATH_SIZE]	= {EEPC_STR_DATA_END};	/* 書き込みファイルパス			*/
	schar				writeDataLine[EEPC_TMP_BUF_SIZE]		= {EEPC_STR_DATA_END};	/* ファイル1行の書き込み領域	*/
	FILE				*writeFilePtr							= NULL;					/* ファイルポインタ				*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == srvMng)
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* 電力会社サーバ管理ファイルのファイルパスを生成 */
		sprintf(writeFilePath, "%s%s",
			EEPC_USR_DIR_PATH, EEPC_SRV_MNG_FILE_NAME);
		/* ファイルオープン処理	*/
		result = eepc_FileOpen(writeFilePath, "w", writeFilePtr);
		/* ファイルオープン結果チェック */
		if(EEPC_FUNC_RESULT_OK != result)
		{
			DLOG_Error("eepc_FileOpen err %d", result);
			result = EEPC_FUNC_RESULT_NG;	/* 処理失敗 */
		}
		else
		{
			/* 電力会社管理ファイルの出力ループ	*/
			for(loopCnt = 0u; loopCnt < EEPC_SRV_MNG_FILE_LINE_MAX; loopCnt++)
			{
				/* ファイル1行分の領域をクリア	*/
				memset(writeDataLine, 0, EEPC_TMP_BUF_SIZE);
				/* ファイル行数で書き込み内容が変わる	*/
				switch (loopCnt)
				{
						/* 1行目:ルート証明書ファイル格納ディレクトリBankNo	*/
				case EEPC_SRV_MNG_DATA_LINE_TYPE_BNK_NO:
					/* ファイル1行分のデータを生成	*/
					sprintf(writeDataLine, "%d\n", srvMng->rtCerBnkNo);
					break;
				/* 2行目:バージョン情報	*/
				case EEPC_SRV_MNG_DATA_LINE_TYPE_VER:
					/* ファイル1行分のデータを生成	*/
					sprintf(writeDataLine, "%03d.%02d\n",
						srvMng->srvInfVerMajor, srvMng->srvInfVerMiner);
					break;
				/* 3行目:電力会社の総数	*/
				case EEPC_SRV_MNG_DATA_LINE_TYPE_TOTAL:
					/* ファイル1行分のデータを生成	*/
					sprintf(writeDataLine, "%d\n", srvMng->srvCnt);
					break;
				/* 4～13行目:1～10件目の電力会社情報	*/
				case EEPC_SRV_MNG_DATA_LINE_TYPE_INF_1:
				case EEPC_SRV_MNG_DATA_LINE_TYPE_INF_2:
				case EEPC_SRV_MNG_DATA_LINE_TYPE_INF_3:
				case EEPC_SRV_MNG_DATA_LINE_TYPE_INF_4:
				case EEPC_SRV_MNG_DATA_LINE_TYPE_INF_5:
				case EEPC_SRV_MNG_DATA_LINE_TYPE_INF_6:
				case EEPC_SRV_MNG_DATA_LINE_TYPE_INF_7:
				case EEPC_SRV_MNG_DATA_LINE_TYPE_INF_8:
				case EEPC_SRV_MNG_DATA_LINE_TYPE_INF_9:
				case EEPC_SRV_MNG_DATA_LINE_TYPE_INF_10:
					/* 出力するデータがある場合	*/
					tmpStrLen = (ulong)strnlen(srvMng->srvInfList[loopCnt - EEPC_SRV_MNG_INF_LINE].srvNameStr,
										sizeof(srvMng->srvInfList[loopCnt - EEPC_SRV_MNG_INF_LINE].srvNameStr));
					if (tmpStrLen)
					{
						/* ファイル1行分のデータを生成	*/
						sprintf(writeDataLine, "%02d,%s,%s,%d,%s,%d,%s\n",
							srvMng->srvInfList[loopCnt - EEPC_SRV_MNG_INF_LINE].corpDiv,
							&(srvMng->srvInfList[loopCnt - EEPC_SRV_MNG_INF_LINE].srvNameStr[0]),
							&(srvMng->srvInfList[loopCnt - EEPC_SRV_MNG_INF_LINE].srvUrlStr[0]),
							srvMng->srvInfList[loopCnt - EEPC_SRV_MNG_INF_LINE].srvPort,
							&(srvMng->srvInfList[loopCnt - EEPC_SRV_MNG_INF_LINE].ntpFqdnStr[0]),
							srvMng->srvInfList[loopCnt - EEPC_SRV_MNG_INF_LINE].ntpSrvPort,
							&(srvMng->srvInfList[loopCnt - EEPC_SRV_MNG_INF_LINE].rtCerFileStr[0]));
					}
					else
					{
						/* DO NOTHING */
					}
					break;
					/* 無効な行数を検出	*/
				default:
					DLOG_Error("srv mng data create err");
					break;
				}

				/* ファイルサイズを取得する	*/
				writeFileSize = (ulong)strnlen(writeDataLine, sizeof(writeDataLine));
				if (writeFileSize)
				{
					/* fwriteによるファイル書き込み	*/
					writeResult = eepc_FileWrite(writeDataLine, writeFileSize, writeFilePtr);
				}
				else
				{
					/* DO NOTHING */
				}

				if (EEPC_FUNC_RESULT_OK != writeResult)
				{
					DLOG_Error("fwrite err %d", writeResult);
					result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
					loopCnt = EEPC_SRV_MNG_FILE_LINE_MAX;
				}
				else
				{
					/* DO NOTHING */
				}
			}

			/* ファイルクローズ処理	*/
			closeResult = eepc_FileClose(writeFilePtr);
			/* ファイルクローズ処理結果チェック	*/
			if(closeResult != EEPC_FUNC_RESULT_OK)
			{
				DLOG_Error("eepc_com_file_close err %d", result);
				result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
			}
			else
			{
				/* DO NOTHING */
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社サーバ選択ファイル用構造体更新処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcFileUseSrv_t *useSrv	電力会社サーバ選択ファイル用構造体	*/
/*					:	uchar index					電力会社サーバ管理ファイルを参照するIndex値	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社サーバ選択ファイル用構造体更新処理			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileUpdateUseSrv(EepcFileUseSrv_t *useSrv, uchar index)
{
	EepcFuncResult_t result = EEPC_FUNC_RESULT_OK;	/* 処理結果 */

	DEBUG_TRCIN();

	/* パラメータチェック */
	if((useSrv == NULL) || (g_eepcFileSrvMng.srvCnt <= index))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* 電力会社サーバ管理ファイル用mutex取得 */
		FRM_MutexLock(g_eepcMutexFileSrvMng);

		/* 電力会社サーバ選択ファイル構造体の情報は、 */
		/* 電力会社サーバ管理ファイル用構造体から取得する */
		memcpy(&(useSrv->srvInf), &(g_eepcFileSrvMng.srvInfList[index]),
			sizeof(EepcFileSrvInf_t));

		g_eepcUseSrvIndex = index;	/* 使用中電力会社のIndex値 */

		/* 電力会社サーバ管理ファイル用mutex解放 */
		FRM_MutexUnLock(g_eepcMutexFileSrvMng);
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社サーバ選択ファイル書き込み処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcFileUseSrv_t *useSrv	電力会社サーバ選択ファイル用構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社サーバ選択ファイル書き込み処理				*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileWriteUseSrvFile(EepcFileUseSrv_t *useSrv)
{
	EepcFuncResult_t	result									= EEPC_FUNC_RESULT_OK;	/* 処理結果					*/
	ulong				writeFileSize							= 0u;					/* ファイルの書き込みサイズ	*/
	schar				writeFilePath[EEPC_TMP_FILE_PATH_SIZE]	= {EEPC_STR_DATA_END};	/* 書き込みファイルパス		*/
	schar				writeFileData[EEPC_TMP_BUF_SIZE]		= {EEPC_STR_DATA_END};	/* ファイルに書き込むデータ	*/
	FILE				*filePtr								= NULL;					/* ファイルポインタ	*/

	DEBUG_TRCIN();

	/* パラメータチェック	*/
	if (NULL == useSrv)
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* 電力会社サーバ選択ファイルのファイルパスを生成	*/
		sprintf(writeFilePath, "%s%s",
			EEPC_USR_DIR_PATH, EEPC_USE_SRV_FILE_NAME);

		sprintf(writeFileData, "%02d,%s,%s,%d,%s,%d,%s\n",
			useSrv->srvInf.corpDiv, useSrv->srvInf.srvNameStr,
			useSrv->srvInf.srvUrlStr, useSrv->srvInf.srvPort,
			useSrv->srvInf.ntpFqdnStr, useSrv->srvInf.ntpSrvPort,
			useSrv->srvInf.rtCerFileStr);

		/* ファイルサイズを取得する	*/
		writeFileSize = (ulong)strnlen(writeFileData, sizeof(writeFileData));

		/* ファイルオープン処理	*/
		result = eepc_FileOpen(writeFilePath, "w", filePtr);
		/* ファイルオープン結果チェック	*/
		if (EEPC_FUNC_RESULT_OK != result)
		{
			DLOG_Error("eepc_FileOpen failed %d", result);
			result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
		}
		else
		{
			result = eepc_FileWrite(writeFileData, writeFileSize, filePtr);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Error("eepc_FileWrite failed %d", result);
				result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
			}
			else
			{
				/* DO NOTHING */
			}

			result = eepc_FileClose(filePtr);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Error("eepc_FileClose failed %d", result);
				result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
			}
			else
			{
				/* DO NOTHING */
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	設定変更通知用構造体更新処理						*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcEvt01_t	*chgData	設定変更通知用構造体		*/
/*					:	ulong		chgFlg		設定変更通知対象のフラグ	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	設定変更通知用構造体更新処理						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/07 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileUpdateChgData(EepcEvt01_t *chgData, ulong chgFlg)
{
	EepcFuncResult_t result = EEPC_FUNC_RESULT_OK;	/* 処理結果	*/

	DEBUG_TRCIN();

	/* パラメータチェック	*/
	if((chgData == NULL) || (EEPC_EVT01_CHG_ALL < chgFlg))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* メッセージ生成 */
		chgData->header.msgId = EEPC_EVT_ID_NTC_CHG_SET;

		/* 変更有無のフラグ */
		chgData->chgDataFlg = chgFlg;

		/* 電力会社の区分 */
		chgData->corpDiv = g_eepcFileUseSrv.srvInf.corpDiv;

		/* 電力会社指定のNTPサーバのポート番号 */
		chgData->ntpPort = g_eepcFileUseSrv.srvInf.ntpSrvPort;

		/* 電力会社サーバ通信履歴ファイル用mutex取得 */
		FRM_MutexLock(g_eepcMutexFileComHist);

		/* 電力会社との通信結果及び通信時間 */
		memcpy(chgData->resultTime,
			g_eepcFileComHist.resultTime,
			EEPC_RESULT_TIME_LEN);

		/* 電力会社サーバ通信履歴ファイル用mutex解放 */
		FRM_MutexUnLock(g_eepcMutexFileComHist);

		/* 電力会社サーバのURL	*/
		memcpy(chgData->srvUrlStr, g_eepcFileUseSrv.srvInf.srvUrlStr, EEPC_URL_LEN);

		/* 電力会社指定のNTPサーバのFQDN */
		memcpy(chgData->ntpFqdnStr, g_eepcFileUseSrv.srvInf.ntpFqdnStr, EEPC_FQDN_LEN);
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	設定変更通知イベント送信処理(全プロセス)			*/
/*--------------------------------------------------------------------------*/
/*	param			:	ulong eventType			設定変更通知対象のフラグ	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	設定変更通知イベント送信処理(全プロセス)			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/07 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileEvtChgSettingAll(ulong eventType)
{
	EepcFuncResult_t		result		= EEPC_FUNC_RESULT_OK;	/* 処理結果				*/
	uchar					loopCnt		= 0u;					/* ループカウンタ		*/
	EepcEvt01_t				chgData;							/* 設定変更通知用構造体	*/

	DEBUG_TRCIN();

	memset(&chgData, 0, sizeof(EepcEvt01_t));
	/* パラメータチェック */
	if (EEPC_EVT01_CHG_ALL < eventType)
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* DO NOTHING */
	}

	if (EEPC_FUNC_RESULT_OK != result)
	{
		result = EEPC_FUNC_RESULT_NG;
	}
	else
	{
		result = eepc_FileUpdateChgData(&chgData, eventType);
		if (EEPC_FUNC_RESULT_OK != result)
		{
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* DO NOTHING */
		}
	}

	if ((EEPC_FUNC_RESULT_OK != result) || (g_eepcChgEvtRegCnt))
	{
		result = EEPC_FUNC_RESULT_NG;
	}
	else
	{
		/* 設定変更通知イベント送信ループ */
		for (loopCnt = 0u; loopCnt < g_eepcChgEvtRegCnt; loopCnt++)
		{
			/* フラグ値チェック */
			ulong tmpFlg = g_eepcFileEvt01[loopCnt].chgDataFlg & eventType;
			if (tmpFlg)
			{
				DLOG_Debug("send chg setting %d %02lX",
					g_eepcFileEvt01[loopCnt].receiver,
					g_eepcFileEvt01[loopCnt].chgDataFlg);

				/* イベントメッセージ送信 */
				slong tmpEvtRet = FRM_MsgSendEvent((ulong)g_eepcFileEvt01[loopCnt].receiver,
													sizeof(chgData), &chgData);
				if (tmpEvtRet)
				{
					/* 送信失敗を出力	*/
					DLOG_Error("chg evt send err %d", loopCnt);
					result = EEPC_FUNC_RESULT_NG;
				}
				else
				{
					/* DO NOTHING */
				}
			}
			else
			{
				/* DO NOTHING */
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	設定変更通知イベント送信処理(1プロセス)				*/
/*--------------------------------------------------------------------------*/
/*	param			:	sint	receiver		設定変更通知対象のフラグ	*/
/*					:	ulong	eventType		設定変更通知対象のフラグ	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	設定変更通知イベント送信処理(1プロセス)				*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/11 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileEvtChgSetting(sint receiver, ulong eventType)
{
	EepcFuncResult_t	result	= EEPC_FUNC_RESULT_OK;	/* 処理結果				*/
	sint				receMax	= 0;
	EepcEvt01_t			chgData;						/* 設定変更通知用構造体	*/

	DEBUG_TRCIN();
	
	/* パラメータチェック */
	receMax = LFW_SPORT_MIN + PROCESS_MAX - EEPC_STR_END_LEN;
	if(!((LFW_SPORT_MIN <= receiver) &&
		(receiver <= receMax)) ||
		(EEPC_EVT01_CHG_ALL < eventType))
	{
		DLOG_Debug("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* 設定変更通知用構造体更新処理 */
		result = eepc_FileUpdateChgData(&chgData, eventType);
		if (EEPC_FUNC_RESULT_OK == result)
		{
			DLOG_Debug("send chg setting %d %02lx", receiver, eventType);

			/* 非同期メッセージ送信 */
			slong tmpAsyncRtn = FRM_MsgSendAsync((ulong)receiver,
				sizeof(EepcEvt01_t), (void *)&chgData);
			if (tmpAsyncRtn)
			{
				/* 送信失敗を出力 */
				DLOG_Error("chg evt send err");
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
		}
		else
		{
			result = EEPC_FUNC_RESULT_NG;
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社サーバ選択ファイル初期値生成処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:	なし												*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社サーバ選択ファイル初期値生成処理			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileCrtInitUseSrvFile(void)
{
	EepcFuncResult_t	result			= EEPC_FUNC_RESULT_OK;	/* 処理結果	*/
	EepcFuncResult_t	noticeRet		= EEPC_FUNC_RESULT_OK;	/* 設定変更通知結果	*/
	ulong				chgSettingFlg	= 0u;					/* 設定変更通知イベント用のフラグ	*/

	DEBUG_TRCIN();

	/* 電力会社サーバ選択ファイル用構造体更新処理				*/
	/* 初期値を生成する為、index値はリスト先頭を示す0を指定する	*/
	result = eepc_FileUpdateUseSrv(&g_eepcFileUseSrv, 0);
	if (EEPC_FUNC_RESULT_OK != result)
	{
		result = EEPC_FUNC_RESULT_NG;
	}
	else
	{
		/* 電力会社サーバ選択ファイル書き込み処理	*/
		result = eepc_FileWriteUseSrvFile(&g_eepcFileUseSrv);
		if (EEPC_FUNC_RESULT_OK != result)
		{
			result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
		}
		else
		{
			/* DO NOTHING */
		}

		/* 設定変更通知イベントのフラグを設定	*/
		chgSettingFlg += (EEPC_EVT01_CHG_CORP_DIV | EEPC_EVT01_CHG_NTP_PORT |
			EEPC_EVT01_CHG_SRV_URL  | EEPC_EVT01_CHG_NTP_FQDN);

		/* 設定変更通知イベント送信処理(全プロセス)	*/
		noticeRet = eepc_FileEvtChgSettingAll(chgSettingFlg);
		if (EEPC_FUNC_RESULT_OK != noticeRet)
		{
			DLOG_Error("eepc_FileEvtChgSettingAll() err");
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	グローバル変数初期化処理(電力会社サーバ管理ファイル用)*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcFileSrvMng_t *srvMng電力会社サーバ管理ファイル用構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	グローバル変数初期化処理(電力会社サーバ管理ファイル用)*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileErrInitGlobalSrvMng(EepcFileSrvMng_t *srvMng)
{
	EepcFuncResult_t result = EEPC_FUNC_RESULT_OK;	/* 処理結果 */

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == srvMng)
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* 電力会社サーバ管理ファイル用構造体を初期化 */
		/* 電力会社サーバの総数 */
		srvMng->srvCnt = 1u;

		/* 固定スケジュール受信のリトライ回数 */
		srvMng->srvRtryCntFixSch = EEPC_RTRY_CNT_FIX_SCH;

		/* 固定スケジュール受信のリトライ間隔 */
		srvMng->srvRtryIntvlFixSch = EEPC_RTRY_INTVL_FIX_SCH;

		/* 更新スケジュール受信のリトライ回数 */
		srvMng->srvRtryCntUpdtSch = EEPC_RTRY_CNT_UPDT_SCH;

		/* 更新スケジュール受信のリトライ間隔 */
		srvMng->srvRtryIntvlUpdtSch = EEPC_RTRY_INTVL_UPDT_SCH;

		/* 時刻同期のリトライ回数 */
		srvMng->ntpRtryCntTimSync = EEPC_RTRY_CNT_TIM_SYNC;

		/* 時刻同期のリトライ間隔 */
		srvMng->ntpRtryIntvlTimSync = EEPC_RTRY_INTVL_TIM_SYNC;

		/* 時刻同期の再リトライ間隔 */
		srvMng->ntpRtryOvrIntvlTimSync = EEPC_RTRY_OVR_INTVL_TIM_SYNC;

		/* 電力会社サーバのポート番号 */
		srvMng->srvInfList[0].srvPort = 0u;

		/* NTPサーバのポート番号 */
		srvMng->srvInfList[0].ntpSrvPort = 0u;

		/* メジャーバージョン情報 */
		srvMng->srvInfVerMajor = 0u;

		/* マイナーバージョン情報 */
		srvMng->srvInfVerMiner = 0u;

		/* 電力会社の区分 */
		srvMng->srvInfList[0].corpDiv = EEPC_CORP_DIV_OTHER;

		/* 電力会社名 */
		sprintf(srvMng->srvInfList[0].srvNameStr, "%s", "-----");

		/* 電力会社サーバのURL */
		sprintf(srvMng->srvInfList[0].srvUrlStr, "%s", "-----");

		/* NTPサーバのFQDN */
		sprintf(srvMng->srvInfList[0].ntpFqdnStr, "%s", "-----");

		/* ルート証明書ファイル格納ディレクトリBankNo */
		srvMng->rtCerBnkNo = 0u;

		/* 初期化した管理ファイルの情報を以って、選択ファイルの情報を更新 */
		result = eepc_FileCrtInitUseSrvFile();
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社サーバ管理ファイル初期値生成処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:	なし												*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社サーバ管理ファイル初期値生成処理			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileCrtInitSrvMngFile(void)
{
	EepcFuncResult_t	result									= EEPC_FUNC_RESULT_OK;	/* 処理結果					*/
	schar				tmpFilePath[EEPC_TMP_FILE_PATH_SIZE]	= {EEPC_STR_DATA_END};	/* ファイルパスの一時領域	*/
	FILE				*filePtr								= NULL;					/* ファイルポインタ			*/

	DEBUG_TRCIN();

	/* 電力会社サーバ管理ファイル用mutex取得 */
	FRM_MutexLock(g_eepcMutexFileSrvMng);

	/* デフォルト電力会社サーバ管理ファイルのパスを生成 */
	snprintf(tmpFilePath, sizeof(tmpFilePath), "%s%s",
		EEPC_DEF_DIR_PATH, EEPC_SRV_MNG_FILE_NAME);
	
	/* ファイルオープン */
	result = eepc_FileOpen(tmpFilePath, "r", filePtr);
	if (EEPC_FUNC_RESULT_OK != result)
	{
		DLOG_Error("eepc_FileOpen() err %s", tmpFilePath);
	}
	else
	{
		/* 電力会社サーバ管理ファイルフォーマットチェック開始処理 */
		result = eepc_FileStartChkFrmtSrvMngFile(filePtr);
		/* ファイルクローズ	*/
		sint tmpRet = eepc_FileClose(filePtr);
		if (EEPC_FUNC_RESULT_OK != tmpRet)
		{
			DLOG_Error("eepc_com_file_close() err");
		}
		else
		{
			/* DO NOTHING */
		}

		/* フォーマットチェック開始処理結果チェック(フォーマット異常) */
		if (EEPC_FUNC_RESULT_OK != result)
		{
			DLOG_Error("srv mng fmt err %d", result);
		}
		else
		{
			/* ルート証明書をコピー */
			result = eepc_FileFncCpyRtCerFile(&g_eepcFileSrvMng, EEPC_DEF_RTCER_DIR_PATH, EEPC_RTCER_BNK0_DIR_PATH);
			if (EEPC_FUNC_RESULT_OK == result)
			{
				/* 読み出したファイルで書き込み */
				result = eepc_FileFncWriteSrvMngFile(&g_eepcFileSrvMng);
			}
			else
			{
				DLOG_Error("eepc_lcl_fnc_cpyRtCerFile() err");
			}
		}
	}

	/* 一連の処理結果を確認 */
	if (EEPC_FUNC_RESULT_OK != result)
	{
		/* デフォルトを復元できない場合の対処 */
		DLOG_Error("eepc_SrvMng.csv Init Fail");
		/* グローバル変数初期化処理(電力会社サーバ管理ファイル_エラー用) */
		result = eepc_FileErrInitGlobalSrvMng(&g_eepcFileSrvMng);
		if(EEPC_FUNC_RESULT_OK != result)
		{
			result = EEPC_FUNC_RESULT_NG;	/* 処理失敗 */
		}
		else
		{
			/* 使用中電力会社のIndex値を初期化 */
			g_eepcUseSrvIndex = 0u;
		}
	}

	/* 電力会社サーバ管理ファイル用mutex解放 */
	FRM_MutexUnLock(g_eepcMutexFileSrvMng);

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社サーバ選択ファイルフォーマットチェック処理	*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *tmpFileData			ファイル内の1行分のデータ	*/
/*					:	ulong fileLineCnt			参照中のファイル行数	*/
/*					:	EepcFileUseSrv_t *ursSrv	電力会社サーバ選択ファイル用構造体	*/
/*					:	ulong tmpFileDataSize		ファイル内の1行分のデータサイズ	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社サーバ選択ファイルフォーマットチェック処理	*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/04 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileChkFrmtUseSrvFile(schar *tmpFileData, ulong fileLineCnt, EepcFileUseSrv_t *ursSrv, ulong tmpFileDataSize)
{
	EepcFuncResult_t	result			= EEPC_FUNC_RESULT_OK;	/* 処理結果	*/
	bool_t				chgFlg			= FALSE;
	ulong				chgSettingFlg	= 0u;			/* 設定変更通知イベント用のフラグ	*/

	DEBUG_TRCIN();

	/* パラメータチェック	*/
	if((NULL == tmpFileData) || (fileLineCnt) || (NULL == ursSrv) ||
		(!tmpFileDataSize))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;	/* 引数異常	*/
	}
	else
	{
		/* DO NOTHING */
	}

	if (EEPC_FUNC_RESULT_OK == result)
	{
		/* ファイルフォーマットチェック	*/
		/* ファイル内の参照行数により、フォーマットチェック方法を変える	*/
		/* 1行目 電力会社情報	*/
		if (fileLineCnt)
		{
			/* ファイル範囲外エラー	*/
			DLOG_Error("fileLineCnt Over %ld", fileLineCnt);
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* Do NOTHING */
		}
	}
	else
	{
		/* DO NOTHING */
	}

	if (EEPC_FUNC_RESULT_OK == result)
	{
		/* フォーマットチェック(電力会社情報)	*/
		result = eepc_FileChkFrmtSrvInf(
			tmpFileData,
			&(ursSrv->srvInf),
			tmpFileDataSize);

		/* フォーマットチェックに加えて、データの正当性をチェック	*/
		if (EEPC_FUNC_RESULT_OK != result)
		{
			DLOG_Error("eepc_FileChkFrmtSrvInf err");
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* DO NOTHING */
		}
	}
	else
	{
		/* DO NOTHING */
	}

	if (EEPC_FUNC_RESULT_OK == result)
	{
		/* 電力会社サーバ選択ファイル詳細チェック処理	*/
		result = EEPC_FileDetailChkUseSrvFile(ursSrv, &chgFlg);
		if (EEPC_FUNC_RESULT_OK != result)
		{
			DLOG_Error("EEPC_FileDetailChkUseSrvFile() err");
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* DO NOTHING */
		}
	}
	else
	{
		/* DO NOTHING */
	}

	if ((EEPC_FUNC_RESULT_OK == result) && (TRUE == chgFlg))
	{
		/* 同じ電力会社だが、情報更新されたので選択ファイル更新	*/
		result = eepc_FileUpdateUseSrv(&g_eepcFileUseSrv, g_eepcUseSrvIndex);
		if (EEPC_FUNC_RESULT_OK != result)
		{
			DLOG_Error("eepc_FileUpdateUseSrv() err");
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			result = eepc_FileWriteUseSrvFile(&g_eepcFileUseSrv);
			/* 電力会社情報が更新されたので、設定変更通知が必要	*/
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Error("eepc_FileWriteUseSrvFile() err");
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* 設定変更通知イベントのフラグを設定		*/
				/* chgFlgがTRUEの場合、送配電事業者コードと						*/
				/* 電力会社名が変化なしの為、EEPC_EVT01_CHG_CORP_DIVは設定しない*/
				chgSettingFlg += (EEPC_EVT01_CHG_NTP_PORT |
					EEPC_EVT01_CHG_SRV_URL |
					EEPC_EVT01_CHG_NTP_FQDN);

				/* 設定変更通知イベント送信処理(全プロセス)	*/
				result = eepc_FileEvtChgSettingAll(chgSettingFlg);

				if (EEPC_FUNC_RESULT_OK != result)
				{
					result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
				}
				else
				{
					/* DO NOTHING */
				}
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	グローバル変数初期化処理(電力会社サーバ選択ファイル用)*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcFileUseSrv_t *useSrv	電力会社サーバ選択ファイル用構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	グローバル変数初期化処理(電力会社サーバ選択ファイル用)*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/04 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileInitGlobalUseSrv(EepcFileUseSrv_t *useSrv)
{
	EepcFuncResult_t result	= EEPC_FUNC_RESULT_OK;	/* 処理結果	*/

	DEBUG_TRCIN();

	/* パラメータチェック	*/
	if(NULL == useSrv)
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* 電力会社サーバ選択ファイル用構造体更新処理	*/
		/* 初期値を生成する為、index値はリスト先頭を示す0を指定する	*/
		result = eepc_FileUpdateUseSrv(useSrv, 0u);
		
		if(EEPC_FUNC_RESULT_OK != result)
		{
			result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社サーバ選択ファイルフォーマットチェック開始処理*/
/*--------------------------------------------------------------------------*/
/*	param			:	FILE *filePtr			ファイルポインタ			*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社サーバ選択ファイルフォーマットチェック開始処理*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/04 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileStartChkFrmtUseSrvFile(FILE *filePtr)
{
	EepcFuncResult_t	result							= EEPC_FUNC_RESULT_OK;	/* 処理結果					*/
	EepcFuncResult_t	initGlobalResult				= EEPC_FUNC_RESULT_OK;	/* グローバル領域初期化結果	*/
	ulong				fileLineCnt						= 0u;					/* ファイル内行数のカウンタ	*/
	schar				tmpFileData[EEPC_TMP_BUF_SIZE]	= {EEPC_STR_DATA_END};	/* ファイルデータの一時領域	*/
	schar				*tmpBuf							= NULL;
	bool_t				loopFlag						= TRUE;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == filePtr)
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;	/* 引数異常 */
	}
	else
	{
		/* フォーマットチェック前に、結果の格納先を初期化 */
		memset(&g_eepcFileUseSrv, 0, sizeof(EepcFileUseSrv_t));

		/* ファイルから1行ずつ読み出し、フォーマットチェックを行う */
		while (loopFlag)
		{
			tmpBuf = fgets(tmpFileData, EEPC_TMP_BUF_SIZE, filePtr);
			if (NULL == tmpBuf)
			{
				DLOG_Error("fgets err");
				loopFlag = FALSE;
			}
			else
			{
				/* 行端変換処理 */
				result = eepc_FileChgLineEnd(tmpFileData, sizeof(tmpFileData));
				/* 処理結果に異常がある場合、フォーマットチェックを中断する */
				if (EEPC_FUNC_RESULT_OK != result)
				{
					result = EEPC_FUNC_RESULT_NG;
					loopFlag = FALSE;
				}
				else
				{
					/* 電力会社サーバ選択ファイルフォーマットチェック処理 */
					result = eepc_FileChkFrmtUseSrvFile(
						tmpFileData,
						fileLineCnt,
						&g_eepcFileUseSrv,
						sizeof(tmpFileData));
					/* 処理結果が正常ならば、次の行のフォーマットチェックを行う */
					if (EEPC_FUNC_RESULT_OK == result)
					{
						memset(tmpFileData, 0, EEPC_TMP_BUF_SIZE);
						fileLineCnt++;
					}
					/* 処理結果に異常がある場合、フォーマットチェックを中断する */
					else
					{
						result = EEPC_FUNC_RESULT_NG;	/* 処理失敗 */
						loopFlag = FALSE;
					}
				}
			}
		}

		/* 最低読み出し行数のチェック */
		if (fileLineCnt < EEPC_FILE_LINE_MIN_USE_SRV)
		{
			result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
		}
		else
		{
			/* フォーマットチェック結果の確認 */
			if (EEPC_FUNC_RESULT_OK != result)
			{
				/* グローバル変数初期化処理(電力会社サーバ選択ファイル用) */
				initGlobalResult = eepc_FileInitGlobalUseSrv(&g_eepcFileUseSrv);
				if (EEPC_FUNC_RESULT_OK != initGlobalResult)
				{
					result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
				}
				else
				{
					/* DO NOTHING */
				}
			}
			else
			{
				/* CT用ログ出力	*/
				DLOG_Info("use srv fmt OK.\n");
				DLOG_Info("corpDiv:%d\n", g_eepcFileUseSrv.srvInf.corpDiv);
				DLOG_Info("srvPort:%d\n", g_eepcFileUseSrv.srvInf.srvPort);
				DLOG_Info("ntpSrvPort:%d\n", g_eepcFileUseSrv.srvInf.ntpSrvPort);
				DLOG_Info("srvNameStr:%s\n", &(g_eepcFileUseSrv.srvInf.srvNameStr[0]));
				DLOG_Info("srvUrlStr:%s\n", &(g_eepcFileUseSrv.srvInf.srvUrlStr[0]));
				DLOG_Info("ntpFqdnStr:%s\n", &(g_eepcFileUseSrv.srvInf.ntpFqdnStr[0]));
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	フォーマットチェック処理(電力会社サーバ設定済み情報)*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *tmpFileData		ファイル内の1行分のデータ	*/
/*					:	uchar *srvFixed			設定先の領域				*/
/*					:	ulong tmpFileDataSize	ファイル内の1行分のデータサイズ	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	フォーマットチェック処理(電力会社サーバ設定済み情報)*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/04 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileChkFrmtSrvFixed(schar *tmpFileData, uchar *srvFixed, ulong tmpFileDataSize)
{
	EepcFuncResult_t	result	= EEPC_FUNC_RESULT_OK;	/* 処理結果	*/
	ulong				dataLen	= 0;					/* データの文字列長	*/
	schar				*tmpStr	= NULL;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == tmpFileData) || (NULL == srvFixed) || (!tmpFileDataSize))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;	/* 引数異常 */
	}
	/* ファイルフォーマット(電力会社サーバ設定済み情報)チェック */
	else
	{
		/* データの文字列長を取得 */
		dataLen = (ulong)strnlen(tmpFileData, tmpFileDataSize);
		/* データの文字列長をチェック	*/
		if((!dataLen) || (EEPC_SRV_FIEXD_MAX_LEN < dataLen))
		{
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* 設定済み情報の値をチェック */
			tmpStr = strstr(tmpFileData, "1");
			if(NULL != tmpStr)
			{
				*srvFixed = (uchar)atoi(tmpFileData);
			}
			/* 値が不適切の場合、フォーマットエラー	*/
			else
			{
				DLOG_Error("fmt chk srv fixed err");
				result = EEPC_FUNC_RESULT_NG;
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社サーバ設定済みファイルフォーマットチェック処理*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *tmpFileData				ファイル内の1行分のデータ	*/
/*					:	ulong fileLineCnt				参照中のファイル行数	*/
/*					:	EepcFileSrvFixed_t *srvFixed	電力会社サーバ設定済みファイル用構造体	*/
/*					:	ulong tmpFileDataSize			ファイル内の1行分のデータサイズ	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社サーバ設定済みファイルフォーマットチェック処理*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/04 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FncChkFrmtSrvFixedFile(schar *tmpFileData, ulong fileLineCnt, EepcFileSrvFixed_t *srvFixed, ulong tmpFileDataSize)
{
	EepcFuncResult_t result = EEPC_FUNC_RESULT_OK;	/* 処理結果	*/

	DEBUG_TRCIN();

	/* パラメータチェック	*/
	if ((NULL == tmpFileData) || (NULL == srvFixed) ||
		(fileLineCnt) || (!tmpFileDataSize))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;	/* 引数異常	*/
	}
	/* ファイルフォーマットチェック	*/
	else{
		/* ファイル内の参照行数により、フォーマットチェック方法を変える	*/
		/* 1行目 電力会社サーバ設定済み情報	*/
		if (!fileLineCnt)
		{
			/* フォーマットチェック処理(電力会社サーバ設定済み情報)	*/
			result = eepc_FileChkFrmtSrvFixed(
				tmpFileData,
				&(srvFixed->srvFixed),
				tmpFileDataSize);
		}
		else
		{
			/* ファイル範囲外エラー	*/
			DLOG_Error("fileLineCnt Over %ld", fileLineCnt);
			result = EEPC_FUNC_RESULT_NG;
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	グローバル変数初期化処理(電力会社サーバ設定済みファイル用)*/
/*--------------------------------------------------------------------------*/
/*	param			:	EepcFileSrvFixed_t *srvFixed	電力会社サーバ設定済みファイル用構造体	*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	グローバル変数初期化処理(電力会社サーバ設定済みファイル用)*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileInitGlobalSrvFixed(EepcFileSrvFixed_t *srvFixed)
{
	EepcFuncResult_t result = EEPC_FUNC_RESULT_OK;	/* 処理結果 */

	DEBUG_TRCIN();

	/* パラメータチェック	*/
	if(NULL == srvFixed)
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* 電力会社サーバ設定済みファイル用構造体を初期化 */
		srvFixed->srvFixed = TRUE;	/* 電力会社サーバ設定済み情報 */
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社サーバ設定済みファイルフォーマットチェック開始処理*/
/*--------------------------------------------------------------------------*/
/*	param			:	FILE *filePtr			ファイルポインタ			*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社サーバ設定済みファイルフォーマットチェック開始処理*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/04 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileStartChkFrmtSrvFixedFile(FILE *filePtr)
{
	EepcFuncResult_t	result							= EEPC_FUNC_RESULT_OK;	/* 処理結果					*/
	EepcFuncResult_t	initGlobalResult				= EEPC_FUNC_RESULT_OK;	/* グローバル領域初期化結果	*/
	ulong				fileLineCnt						= 0u;					/* ファイル内行数のカウンタ	*/
	schar				tmpFileData[EEPC_TMP_BUF_SIZE]	= {EEPC_STR_DATA_END};	/* ファイルデータの一時領域	*/
	schar				*tmpBuf							= NULL;
	bool_t				loopFlag						= TRUE;

	DEBUG_TRCIN();

	/* パラメータチェック	*/
	if (NULL == filePtr)
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;	/* 引数異常	*/
	}
	else
	{
		/* ファイルから1行ずつ読み出し、フォーマットチェックを行う	*/
		while (loopFlag)
		{
			tmpBuf = fgets(tmpFileData, EEPC_TMP_BUF_SIZE, filePtr);
			if (NULL == tmpBuf)
			{
				DLOG_Error("fgets err");
				loopFlag = FALSE;
			}
			else
			{
				/* 行端変換処理	*/
				result = eepc_FileChgLineEnd(tmpFileData, sizeof(tmpFileData));
				/* 処理結果に異常がある場合、フォーマットチェックを中断する	*/
				if (EEPC_FUNC_RESULT_OK != result)
				{
					result = EEPC_FUNC_RESULT_NG;
					loopFlag = FALSE;
				}
				else
				{
					/* 電力会社サーバ設定済みファイルフォーマットチェック処理	*/
					result = eepc_FncChkFrmtSrvFixedFile(
						tmpFileData,
						fileLineCnt,
						&g_eepcFileSrvFixed,
						sizeof(tmpFileData));
					/* 処理結果が正常ならば、次の行のフォーマットチェックを行う	*/
					if (EEPC_FUNC_RESULT_OK == result)
					{
						memset(tmpFileData, 0, EEPC_TMP_BUF_SIZE);
						fileLineCnt++;
					}
					/* 処理結果に異常がある場合、フォーマットチェックを中断する	*/
					else
					{
						result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
						loopFlag = FALSE;
					}
				}
			}
		}

		/* 最低読み出し行数のチェック	*/
		if(fileLineCnt < EEPC_FILE_LINE_MIN_SRV_FIXED)
		{
			result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
		}
		else
		{
			/* DO NOTHING */
		}

		/* フォーマットチェック結果の確認	*/
		if(EEPC_FUNC_RESULT_OK != result)
		{
			/* グローバル変数初期化処理(電力会社サーバ設定済みファイル用)	*/
			initGlobalResult = eepc_FileInitGlobalSrvFixed(&g_eepcFileSrvFixed);
			if(EEPC_FUNC_RESULT_OK != initGlobalResult)
			{
				result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
			}
			else
			{
				/* DO NOTHING */
			}
		}
		else
		{
			/* CT用ログ出力	*/
			DLOG_Info("srv fixed fmt OK\n");
			DLOG_Info("srvFixed:%d\n",g_eepcFileSrvFixed.srvFixed);
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社サーバ設定済みファイル初期値生成処理		*/
/*--------------------------------------------------------------------------*/
/*	param			:	なし												*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社サーバ設定済みファイル初期値生成処理		*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileCrtInitSrvFixed_File(void)
{
	EepcFuncResult_t	result									= EEPC_FUNC_RESULT_OK;	/* 処理結果					*/
	ulong				initFileSize							= 0u;					/* ファイルの初期値サイズ	*/
	schar				initFileData[EEPC_TMP_BUF_SIZE]			= {EEPC_STR_DATA_END};	/* ファイルの初期値格納先	*/
	schar				initFilePath[EEPC_TMP_FILE_PATH_SIZE]	= {EEPC_STR_DATA_END};	/* 初期値ファイル生成パス	*/
	FILE				*filePtr								= NULL;

	DEBUG_TRCIN();

	/* 電力会社サーバ設定済みファイルのファイルパスを生成 */
	sprintf(initFilePath,
		"%s%s",
		EEPC_USR_DIR_PATH,
		EEPC_SRV_FIEXD_FILE_NAME);

	/* グローバル変数初期化処理(電力会社サーバ設定済みファイル用) */
	result = eepc_FileInitGlobalSrvFixed(&g_eepcFileSrvFixed);

	if(EEPC_FUNC_RESULT_OK != result)
	{
		result = EEPC_FUNC_RESULT_NG;	/* 処理失敗 */
	}
	else
	{
		/* 生成した値を基に、ファイルに書き出す文字列とする */
		sprintf(&(initFileData[0]), "%d\n", g_eepcFileSrvFixed.srvFixed);

		/* ファイルサイズを取得する */
		initFileSize = (ulong)strnlen(&(initFileData[0]), sizeof(initFileData));

		/* ファイルオープン処理 */
		result = eepc_FileOpen(initFilePath, "w", filePtr);
		/* ファイルオープン結果チェック */
		if (EEPC_FUNC_RESULT_OK != result)
		{
			DLOG_Error("eepc_FileOpen failed %d", result);
			result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
		}
		else
		{
			result = eepc_FileWrite(initFileData, initFileSize, filePtr);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Error("eepc_FileWrite failed %d", result);
				result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
			}
			else
			{
				/* DO NOTHING */
			}

			result = eepc_FileClose(filePtr);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Error("eepc_FileClose failed %d", result);
				result = EEPC_FUNC_RESULT_NG;	/* 処理失敗	*/
			}
			else
			{
				/* DO NOTHING */
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社サーバ管理ファイルチェック処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:	なし												*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社サーバ管理ファイルチェック処理				*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/03/31 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileChkSrvMngFile(void)
{
	EepcFuncResult_t	result									= EEPC_FUNC_RESULT_OK;	/* 処理結果					*/
	bool_t				fmtErrFlg								= FALSE;				/* フォーマットエラーフラグ	*/
	schar				tmpFilePath[EEPC_TMP_FILE_PATH_SIZE]	= {EEPC_STR_DATA_END};	/* ファイルパスの一時領域	*/
	FILE				*filePtr								= NULL;					/* ファイルポインタ			*/
	schar				*rtCerDirPath							= NULL;					/* ルート証明書格納ディレクトリPATH	*/

	DEBUG_TRCIN();

	/* 電力会社サーバ管理ファイルのファイルパスを生成 */
	snprintf(tmpFilePath, sizeof(tmpFilePath), "%s%s",
		EEPC_USR_DIR_PATH, EEPC_SRV_MNG_FILE_NAME);

	/* ファイルオープン処理 */
	result = eepc_FileOpen(tmpFilePath, "r", filePtr);
	/* ファイルオープン結果チェック(処理失敗) */
	if (EEPC_FUNC_RESULT_OK != result)
	{
		DLOG_Error("eepc_FileOpen() err %s", tmpFilePath);
		result = EEPC_FUNC_RESULT_NG;
		fmtErrFlg = TRUE;
	}
	else
	{
		/* ファイルオープン結果チェック(処理成功) */
		/* 電力会社サーバ管理ファイルフォーマットチェック開始処理 */
		result = eepc_FileStartChkFrmtSrvMngFile(filePtr);
		/* ファイルクローズ処理	*/
		sint tmpRet = eepc_FileClose(filePtr);
		if (EEPC_FUNC_RESULT_OK != tmpRet)
		{
			DLOG_Error("eepc_com_file_close() err");
		}
		else
		{
			/* DO NOTHING */
		}

		/* フォーマットチェック開始処理結果チェック(フォーマット異常)	*/
		if (EEPC_FUNC_RESULT_OK != result)
		{
			DLOG_Error("srv mng fmt err %d", result);
			fmtErrFlg = TRUE;
		}
		else
		{
			/* ルート証明書のファイル名を確認する	*/
			if(EEPC_STR_DATA_END != g_eepcFileSrvMng.srvInfList[0].rtCerFileStr[0])
			{
				/* ルート証明書ファイル名を持つ場合は	*/
				/* ルート証明書ファイルの有無をチェック	*/
				if (EEPC_EEPC_RTCER_BNK_NO_0 == g_eepcFileSrvMng.rtCerBnkNo)
				{
					rtCerDirPath = EEPC_RTCER_BNK0_DIR_PATH;
				}
				else
				{
					rtCerDirPath = EEPC_RTCER_BNK1_DIR_PATH;
				}
				
				result = eepc_FileChkRtCerFileExist(&g_eepcFileSrvMng, rtCerDirPath);
				if(EEPC_FUNC_RESULT_OK != result)
				{
					DLOG_Error("root cer not exist");
					/* フォーマットエラー扱いとする	*/
					fmtErrFlg = TRUE;
				}
				else
				{
					/* DO NOTHING */
				}
			}
			else
			{
				/* DO NOTHING */
			}
		}
	}

	/* ファイルチェック処理に異常がある場合	*/
	if (TRUE == fmtErrFlg)
	{
		/* 電力会社サーバ管理ファイル初期値生成処理	*/
		result = eepc_FileCrtInitSrvMngFile();
		if (EEPC_FUNC_RESULT_OK != result)
		{
			DLOG_Error("eepc_FileCrtInitSrvMngFile() err %d", result);
			result = EEPC_FUNC_RESULT_NG;
		}
		else
		{
			/* DO NOTHING */
		}
	}
	else
	{
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社サーバ選択ファイルチェック処理				*/
/*--------------------------------------------------------------------------*/
/*	param			:	なし												*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社サーバ選択ファイルチェック処理				*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/03 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileChkUseSrvFile(void)
{
	EepcFuncResult_t	result									= EEPC_FUNC_RESULT_OK;	/* 処理結果(下位関数用)									*/
	EepcFuncResult_t	resultUseSrv 							= EEPC_FUNC_RESULT_OK;	/* 処理結果(電力会社サーバ選択ファイル用)				*/
	EepcFuncResult_t	resultSrvFixed 							= EEPC_FUNC_RESULT_OK;	/* 処理結果(電力会社サーバ設定済みファイル用)			*/
	bool_t				fmtErrFlgUseSrv							= FALSE;				/* フォーマットエラー(電力会社サーバ選択ファイル用)		*/
	bool_t				fmtErrFlgSrvFixed						= FALSE;				/* フォーマットエラー(電力会社サーバ設定済みファイル用)	*/
	schar				tmpFilePath[EEPC_TMP_FILE_PATH_SIZE]	= {EEPC_STR_DATA_END};	/* ファイルパスの一時領域								*/
	FILE				*filePtrUseSrv							= NULL;					/* 電力会社サーバ選択ファイルのファイルポインタ			*/
	FILE				*filePtrSrvFixed						= NULL;					/* 電力会社サーバ設定済みファイルのファイルポインタ		*/
	bool_t				chgFlg									= FALSE;				/* 選択ファイル詳細チェックによる情報更新の有無			*/

	DEBUG_TRCIN();

	/* 電力会社サーバ選択ファイルのファイルパスを生成 */
	sprintf(&(tmpFilePath[0]), "%s%s", EEPC_USR_DIR_PATH, EEPC_USE_SRV_FILE_NAME);
	/* ファイルオープン処理 */
	resultUseSrv = eepc_FileOpen(tmpFilePath, "r", filePtrUseSrv);

	/* 電力会社サーバ設定済みファイルのファイルパスを生成 */
	memset(tmpFilePath, 0, EEPC_TMP_FILE_PATH_SIZE);
	sprintf(tmpFilePath, "%s%s", EEPC_USR_DIR_PATH, EEPC_SRV_FIEXD_FILE_NAME);
	/* ファイルオープン処理 */
	resultSrvFixed = eepc_FileOpen(tmpFilePath, "r", filePtrSrvFixed);

	/* ファイルオープン結果チェック */
	/* (2点のファイルポインタで確認する為、処理結果はチェック対象外とする) */
	/* 電力会社サーバ選択ファイルがない */
	/* 電力会社サーバ設定済みファイルがない */
	if ((NULL == filePtrUseSrv) && (NULL == filePtrSrvFixed))
	{
		/* 本条件は、リモコンの初回起動に相当する為、 */
		/* 選択ファイルや設定済みファイルの生成は行わない */

		/* 電力会社サーバ選択ファイル用構造体の初期値を設定(九州電力) */
		/* 電力会社の区分 */
		g_eepcFileUseSrv.srvInf.corpDiv = EEPC_INIT_CORP_DIV;

		/* 電力会社サーバのポート番号 */
		g_eepcFileUseSrv.srvInf.srvPort = EEPC_INIT_SRV_PORT;

		/* 電力会社指定のNTPサーバのポート番号 */
		g_eepcFileUseSrv.srvInf.ntpSrvPort = EEPC_INIT_NTP_PORT;

		/* 電力会社名 */
		sprintf(
			g_eepcFileUseSrv.srvInf.srvNameStr,
			"%s",
			EEPC_INIT_CORP_NAME);

		/* 電力会社サーバのURL */
		sprintf(
			g_eepcFileUseSrv.srvInf.srvUrlStr,
			"%s%s%s",
			EEPC_INIT_HTTPS_HEAD,
			EEPC_INIT_SRV_HOST,
			EEPC_INIT_SRV_REQ);

		/* 電力会社指定のNTPサーバのFQDN */
		sprintf(
			g_eepcFileUseSrv.srvInf.ntpFqdnStr,
			"%s",
			EEPC_INIT_NTP_FQDN);

		/* 電力会社サーバ選択ファイル詳細チェック処理 */
		result = EEPC_FileDetailChkUseSrvFile(&g_eepcFileUseSrv, &chgFlg);

		/* 詳細チェックで初期値(九州電力)が該当しない場合 */
		if (EEPC_FUNC_RESULT_OK != result)
		{
			/* 電力会社サーバ選択ファイル用構造体更新処理				*/
			/* 初期値を生成する為、index値はリスト先頭を示す0を指定する	*/
			result = eepc_FileUpdateUseSrv(&g_eepcFileUseSrv, 0);
		}
		else
		{
			/* 詳細チェックで初期値(九州電力)に情報更新がある場合	*/
			if (TRUE == chgFlg)
			{
				/* 電力会社サーバ選択ファイル用構造体更新処理						*/
				/* 初期値を更新する為、index値は詳細チェックで更新した値を指定する	*/
				result = eepc_FileUpdateUseSrv(&g_eepcFileUseSrv, g_eepcUseSrvIndex);
			}
			else
			{
				/* DO NOTHING */
			}
		}
		

		/* 電力会社サーバ設定済みファイル用構造体の初期値を設定	*/
		g_eepcFileSrvFixed.srvFixed = TRUE;

		DLOG_Info("Init_corpDiv %d \n", g_eepcFileUseSrv.srvInf.corpDiv);
		DLOG_Info("Init_srvPort %d \n", g_eepcFileUseSrv.srvInf.srvPort);
		DLOG_Info("Init_ntpSrvPort %d \n", g_eepcFileUseSrv.srvInf.ntpSrvPort);
		DLOG_Info("Init_srvNameStr %s \n", g_eepcFileUseSrv.srvInf.srvNameStr);
		DLOG_Info("Init_srvUrlStr %s \n", g_eepcFileUseSrv.srvInf.srvUrlStr);
		DLOG_Info("Init_ntpFqdnStr %s \n", g_eepcFileUseSrv.srvInf.ntpFqdnStr);
		DLOG_Info("Init_rtCerFileStr %s \n", g_eepcFileUseSrv.srvInf.rtCerFileStr);
	}
	/* 電力会社サーバ選択ファイルと						*/
	/* 電力会社サーバ設定済みファイルのどちらかがある	*/
	else
	{
		/* 電力会社サーバ選択ファイルがある */
		if (NULL != filePtrUseSrv)
		{
			/* 電力会社サーバ選択ファイルフォーマットチェック開始処理 */
			resultUseSrv = eepc_FileStartChkFrmtUseSrvFile(filePtrUseSrv);

			/* フォーマットチェック開始処理結果チェック(フォーマット異常) */
			if (EEPC_FUNC_RESULT_OK !=  resultUseSrv)
			{
				DLOG_Error("use srv file fmt err %d", result);
				fmtErrFlgUseSrv = TRUE;
			}
			else
			{
				/* DO NOTHING */
			}

			/* ファイルクローズ処理	*/
			resultUseSrv = eepc_FileClose(filePtrUseSrv);
			/* ファイルクローズ処理結果チェック	*/
			if (EEPC_FUNC_RESULT_OK != resultUseSrv){
				DLOG_Error("eepc_com_file_close() err %d", result);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
		}
		else
		{
			/* DO NOTHING */
		}

		/* 電力会社サーバ設定済みファイルがある */
		if (NULL != filePtrSrvFixed)
		{
			/* 電力会社サーバ設定済みファイルフォーマットチェック開始処理 */
			resultSrvFixed = eepc_FileStartChkFrmtSrvFixedFile(filePtrSrvFixed);
			/* フォーマットチェック開始処理結果チェック(フォーマット異常) */
			if (EEPC_FUNC_RESULT_OK != resultSrvFixed)
			{
				DLOG_Error("srv fixed file fmt err %d", resultSrvFixed);
				fmtErrFlgSrvFixed = TRUE;
			}
			else
			{
				/* DO NOTHING */
			}

			/* ファイルクローズ処理	*/
			resultSrvFixed = eepc_FileClose(filePtrSrvFixed);
			/* ファイルクローズ処理結果チェック	*/
			if (resultSrvFixed != EEPC_FUNC_RESULT_OK){
				DLOG_Error("eepc_com_file_close() err %d", resultSrvFixed);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
		}
		else
		{
			/* DO NOTHING */
		}

		/* 電力会社サーバ選択ファイルがない場合、						*/
		/* 又は、電力会社サーバ選択ファイルがフォーマットエラーの場合	*/
		if ((NULL == filePtrUseSrv) || (TRUE == fmtErrFlgUseSrv))
		{
			/* 電力会社サーバ選択ファイルの初期値を生成	*/
			resultUseSrv = eepc_FileCrtInitUseSrvFile();
			if (EEPC_FUNC_RESULT_OK != resultUseSrv)
			{
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
		}
		else
		{
			/* DO NOTHING */
		}

		/* 電力会社サーバ設定済みファイルがない場合、						*/
		/* 又は、電力会社サーバ設定済みファイルのフォーマットエラーの場合	*/
		if ((NULL == filePtrSrvFixed) || (TRUE == fmtErrFlgSrvFixed))
		{
			/* 電力会社サーバ設定済みファイルの初期値を生成	*/
			resultSrvFixed = eepc_FileCrtInitSrvFixed_File();
			if (EEPC_FUNC_RESULT_OK != resultSrvFixed)
			{
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}


/*==========================================================================*/
/*	Description		:	対象URLファイルのバージョン取得						*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *urlFilePath		対象URLファイルのPATH		*/
/*					:	ushort *verMajor		Majorバージョン格納先		*/
/*					:	uchar *verMinor			Minorバージョン格納先		*/
/*--------------------------------------------------------------------------*/
/*	return			:	なし												*/
/*--------------------------------------------------------------------------*/
/*	detail			:	対象URLファイルのバージョン取得						*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/13 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static void eepc_FileFncGetUrlVer(schar *urlFilePath, ushort *verMajor, uchar *verMinor)
{
	FILE*				fp								= NULL;
	EepcFuncResult_t	ret								= EEPC_FUNC_RESULT_OK;
	schar				*str_ret						= NULL;
	schar				tmpFileData[EEPC_TMP_BUF_SIZE]	= {EEPC_STR_DATA_END};

	DEBUG_TRCIN();
	
	if ((NULL == urlFilePath) || (NULL == verMajor) || (NULL == verMinor))
	{
		DEBUG_TRCOUT();

		return;
	}
	else
	{
		/* DO NOTHING */
	}

	/* 初期化	*/
	*verMajor = 0u;
	*verMinor = 0u;

	/* 指定ファイルオープン	*/
	ret = eepc_FileOpen(urlFilePath, "r", fp);
	if (EEPC_FUNC_RESULT_OK != ret)
	{
		/* ファイルが無い場合はバージョン000.00扱い	*/
	}
	else
	{
		/* 1行目(コメント行 or BankNo)読み飛ばし	*/
		memset(tmpFileData, 0x00, sizeof(tmpFileData));
		str_ret = fgets(tmpFileData, EEPC_TMP_BUF_SIZE, fp);
		if (str_ret)
		{
			/* バージョン文字列取得	*/
			memset(tmpFileData, 0x00, sizeof(tmpFileData));
			str_ret = fgets(tmpFileData, sizeof(tmpFileData), fp);
			if (str_ret)
			{
				/* バージョン(数値)取得	*/
				ret = eepc_FileChgLineEnd(tmpFileData, sizeof(tmpFileData));
				if (EEPC_FUNC_RESULT_OK == ret)
				{
					ret = eepc_FileFncChkFrmtVer(tmpFileData, verMajor, verMinor, sizeof(tmpFileData));
					if (EEPC_FUNC_RESULT_OK != ret)
					{
						DLOG_Error("getUrlVer Fail(%s)", urlFilePath);
					}
					else
					{
						/* DO NOTHING */
					}
				}
				else
				{
					/* DO NOTHING */
				}
			}
			else
			{
				/* DO NOTHING */
			}
		}
		else
		{
			/* DO NOTHING */
		}

		/* ファイルクローズ	*/
		ret = eepc_FileClose(fp);
		if (EEPC_FUNC_RESULT_OK != ret)
		{
			DLOG_Error("eepc_FileClose() err");
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:	URLバージョン更新判定								*/
/*--------------------------------------------------------------------------*/
/*	param			:	ushort	baseMajor	比較元Majorバージョン			*/
/*	param			:	uchar	baseMinor	比較元Minorバージョン			*/
/*	param			:	ushort	chkMajor	比較対象Majorバージョン			*/
/*	param			:	uchar	chkMinor	比較対象Minorバージョン			*/
/*--------------------------------------------------------------------------*/
/*	return			:	TRUE				比較対象が最新					*/
/*	return			:	FALSE				比較元が最新					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	URLバージョン更新判定								*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/13 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static bool_t eepc_FileFncChkVerUpdate(ushort baseMajor, uchar baseMinor, ushort chkMajor, uchar chkMinor)
{
	bool_t result = FALSE;

	DEBUG_TRCIN();

	if ((baseMajor < chkMajor) ||
		((chkMajor == baseMajor) && (baseMinor < chkMinor)))
	{
		result = TRUE;
	}
	else
	{
		/* DO NOTHING */
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	更新対象とするURLファイルのPATH取得					*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *dirPathBuf		ディレクトリPATH格納先バッファアドレス	*/
/*					:	ulong dirPathBufSz		ディレクトリPATH格納先バッファサイズ	*/
/*					:	schar *fnameBuf			ファイル名格納先バッファアドレス		*/
/*					:	ulong fnameBufSz		ファイル名格納先バッファサイズ			*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	更新対象とするURLファイルのPATH取得					*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/13 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileFncGetUpdFilePath(schar *dirPathBuf, ulong dirPathBufSz, schar *fnameBuf, ulong fnameBufSz)
{
	EepcFuncResult_t	result									= EEPC_FUNC_RESULT_OK;
	schar				tmpFilePath[EEPC_TMP_FILE_PATH_SIZE]	= {'\0'};
	ushort				defVerMajor								= 0u;
	uchar				defVerMiner								= 0u;
	ushort				usbVerMajor								= 0u;
	uchar				usbVerMiner								= 0u;
	ushort				targetVerMajor							= 0u;
	uchar				targetVerMiner							= 0u;
	bool_t				verRet									= FALSE;

	DEBUG_TRCIN();
	
	/* パラメータチェック */
	if ((NULL == dirPathBuf) || (!dirPathBufSz) ||
		(NULL == fnameBuf) || (!fnameBufSz))
	{
		result = EEPC_FUNC_RESULT_NG;
	}
	else
	{
		/* 更新対象を初期化(「現状(アプリケーション領域)」が最新の状態とする) */
		targetVerMajor = g_eepcFileSrvMng.srvInfVerMajor;
		targetVerMiner = g_eepcFileSrvMng.srvInfVerMiner;
		dirPathBuf[0] 	= EEPC_STR_DATA_END;
		fnameBuf[0] 	= EEPC_STR_DATA_END;

		/* 「現状(アプリケーション領域)」 / 「デフォルト(rootfs領域)」 / 「USBメモリ」 にあるURLファイルの内、最も新しいものを決定 */
		/* 「rootfs」にあるURLファイルのバージョンを取得 */
		memset(tmpFilePath, 0x00, sizeof(tmpFilePath));
		snprintf(tmpFilePath, sizeof(tmpFilePath), "%s%s", EEPC_DEF_DIR_PATH, EEPC_SRV_MNG_FILE_NAME);
		eepc_FileFncGetUrlVer(tmpFilePath, &defVerMajor, &defVerMiner);

		/* USBメモリ内にあるURLファイルのバージョンを取得 */
		memset(tmpFilePath, 0x00, sizeof(tmpFilePath));
		snprintf(tmpFilePath, sizeof(tmpFilePath), "%s%s", EEPC_USB_DIR_PATH, EEPC_SRV_URL_FILE_NAME);
		eepc_FileFncGetUrlVer(tmpFilePath, &usbVerMajor, &usbVerMiner);

		/* 「現状」と「デフォルト」のURLファイルバージョンを比較 */
		verRet = eepc_FileFncChkVerUpdate(targetVerMajor, targetVerMiner, defVerMajor, defVerMiner);
		if (TRUE == verRet)
		{
			/*	デフォルトが新しい場合は更新 */
			targetVerMajor = defVerMajor;
			targetVerMiner = defVerMiner;
			snprintf(dirPathBuf, dirPathBufSz, "%s", EEPC_DEF_DIR_PATH);
			snprintf(fnameBuf, fnameBufSz, "%s", EEPC_SRV_MNG_FILE_NAME);
		}
		else
		{
			/* DO NOTHING */
		}

		/* 上記で新しい方とUSB内 URLファイルのバージョンを比較 */
		verRet = eepc_FileFncChkVerUpdate(targetVerMajor, targetVerMiner, usbVerMajor, usbVerMiner);
		if (TRUE == verRet)
		{
			/*	USB内が新しい場合は更新	*/
			snprintf(dirPathBuf, dirPathBufSz, "%s", EEPC_USB_DIR_PATH);
			snprintf(fnameBuf, fnameBufSz, "%s", EEPC_SRV_URL_FILE_NAME);
		}
		else
		{
			/* DO NOTHING */
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社URLファイルフォーマットチェック処理			*/
/*--------------------------------------------------------------------------*/
/*	param			:	schar *tmpFileData		ファイル内の1行分のデータ			*/
/*	param			:	ulong fileLineCnt		参照中のファイル行数				*/
/*	param			:	ulong tmpFileDataSize	ファイル内の1行分のデータサイズ		*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社URLファイルフォーマットチェック処理			*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/13 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileFncChkFrmtSrvUrlFile(schar *tmpFileData, ulong fileLineCnt, ulong tmpFileDataSize)
{
	EepcFuncResult_t	result			= EEPC_FUNC_RESULT_OK;	/* 処理結果				*/
	ushort				srvUrlVerMajor	= 0u;					/* メジャーバージョン	*/
	uchar				srvUrlVerMiner	= 0u;					/* マイナーバージョン	*/

	DEBUG_TRCIN();

	/* パラメータチェック */
	if ((NULL == tmpFileData) || (!tmpFileDataSize) ||
		(EEPC_SRV_URL_FILE_LINE_MAX <= fileLineCnt))
	{
		DLOG_Error("prm err");
		result = EEPC_FUNC_RESULT_EPRM;	/* 引数異常 */
	}
	/* ファイルフォーマットチェック */
	else
	{
		/* 電力会社サーバ管理ファイル用mutex取得 */
		FRM_MutexLock(g_eepcMutexFileSrvMng);

		/* ファイル内の参照行数により、フォーマットチェック方法を変える */
		/* 1行目 コメント行	*/
		if (EEPC_SRV_URL_COM_LINE_ID == fileLineCnt)
		{
			/* コメント行はチェックの対象外 */
		}
		/* 2行目 バージョン情報	*/
		else if (EEPC_SRV_URL_VER_LINE_ID == fileLineCnt)
		{
			/* フォーマットチェック処理(バージョン情報) */
			result = eepc_FileFncChkFrmtVer(
				tmpFileData,
				&srvUrlVerMajor,
				&srvUrlVerMiner,
				tmpFileDataSize);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Debug("eepc_FileFncChkFrmtVer() err %d", result);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* バージョン情報の比較						*/
				/* バージョンが新しい場合、更新に対応する	*/
				if ((g_eepcFileSrvMng.srvInfVerMajor < srvUrlVerMajor) ||
					((srvUrlVerMajor == g_eepcFileSrvMng.srvInfVerMajor) &&
					(g_eepcFileSrvMng.srvInfVerMiner < srvUrlVerMiner)))
				{
					/* 電力会社URLファイルバージョンを取得 */
					g_eepcFileSrvMng.srvInfVerMajor = srvUrlVerMajor;
					g_eepcFileSrvMng.srvInfVerMiner = srvUrlVerMiner;
				}
				/* バージョンが同一か、古い場合 */
				else
				{
					DLOG_Debug("SrvData.csv Ver Old or Same.");
					result = EEPC_FUNC_RESULT_NG;
				}
			}
		}
		/* 3行目 対象サーバ数 */
		else if (EEPC_SRV_URL_CNT_LINE_ID == fileLineCnt)
		{
			/* フォーマットチェック処理(対象サーバ数) */
			result = eepc_FileChkFrmtSrvCnt(
				tmpFileData,
				&(g_eepcFileSrvMng.srvCnt),
				tmpFileDataSize);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Debug("eepc_FileChkFrmtSrvCnt() err %d", result);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
		}
		/* 4行目 ～ 13行目:電力会社1社の情報 */
		else if ((EEPC_SRV_URL_INF_LINE_ID <= fileLineCnt) &&
				(fileLineCnt < EEPC_SRV_URL_FILE_LINE_MAX))
		{
			/* フォーマットチェック処理(電力会社情報) */
			result = eepc_FileChkFrmtSrvInf(
				tmpFileData,
				&(g_eepcFileSrvMng.srvInfList[fileLineCnt - EEPC_SRV_URL_INF_LINE_ID]),
				tmpFileDataSize);
			if (EEPC_FUNC_RESULT_OK != result)
			{
				DLOG_Debug("eepc_FileChkFrmtSrvInf() err %d", result);
				result = EEPC_FUNC_RESULT_NG;
			}
			else
			{
				/* DO NOTHING */
			}
		}
		else
		{
			DLOG_Debug("fileLineCnt Over! %ld", fileLineCnt);
			result = EEPC_FUNC_RESULT_NG;
		}

		/* 電力会社サーバ管理ファイル用mutex解放 */
		FRM_MutexUnLock(g_eepcMutexFileSrvMng);
	}

	DEBUG_TRCOUT();

	return result;
}

/*==========================================================================*/
/*	Description		:	電力会社URLファイルフォーマットチェック開始処理		*/
/*--------------------------------------------------------------------------*/
/*	param			:	FILE *filePtr			ファイルポインタ			*/
/*--------------------------------------------------------------------------*/
/*	return			:	EEPC_FUNC_RESULT_OK		処理成功					*/
/*					:	EEPC_FUNC_RESULT_NG		処理失敗					*/
/*					:	EEPC_FUNC_RESULT_EPRM	引数異常					*/
/*--------------------------------------------------------------------------*/
/*	detail			:	電力会社URLファイルフォーマットチェック開始処理		*/
/*--------------------------------------------------------------------------*/
/*	history			:	2023/04/13 : DHC Xu.Y : 新規作成					*/
/*==========================================================================*/
static EepcFuncResult_t eepc_FileStartChkFrmtSrvUrlFile(FILE *filePtr)
{
	EepcFuncResult_t	result							= EEPC_FUNC_RESULT_OK;	/* 処理結果					*/
	ulong				fileLineCnt						= 0u;					/* ファイル内行数のカウンタ	*/
	schar				tmpFileData[EEPC_TMP_BUF_SIZE]	= {EEPC_STR_DATA_END};	/* ファイルデータの一時領域	*/
	bool_t				loopFlg							= TRUE;
	schar				*tmpStr							= NULL;

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (NULL == filePtr)
	{
		DLOG_Debug("prm err");
		result = EEPC_FUNC_RESULT_EPRM;
	}
	else
	{
		/* ファイルから1行ずつ読み出し、フォーマットチェックを行う */
		while (loopFlg)
		{
			tmpStr = fgets(tmpFileData, EEPC_TMP_BUF_SIZE, filePtr);
			if (NULL == tmpStr)
			{
				DLOG_Error("fgets err");
				loopFlg = FALSE;
			}
			else
			{
				/* 行端変換処理 */
				result = eepc_FileChgLineEnd(tmpFileData, sizeof(tmpFileData));
				/* 処理結果に異常がある場合、フォーマットチェックを中断する */
				if (EEPC_FUNC_RESULT_OK != result)
				{
					result = EEPC_FUNC_RESULT_NG;
					loopFlg = FALSE;
				}
				else
				{
					/* 電力会社URLファイルフォーマットチェック処理 */
					result = eepc_FileFncChkFrmtSrvUrlFile(
						tmpFileData,
						fileLineCnt,
						sizeof(tmpFileData));
					
					/* 処理結果が正常ならば、次の行のフォーマットチェックを行う */
					if (EEPC_FUNC_RESULT_OK == result)
					{
						memset(tmpFileData, 0, EEPC_TMP_BUF_SIZE);
						fileLineCnt++;
					}
					/* 処理結果に異常がある場合、フォーマットチェックを中断する */
					else
					{
						result = EEPC_FUNC_RESULT_NG;
						loopFlg = FALSE;
					}
				}
			}
		}
	}

	DEBUG_TRCOUT();

	return result;
}

/*End Of File*/
