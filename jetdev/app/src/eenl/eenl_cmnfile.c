/****************************************************************************/
/*	Description		:EENL ファイル入出力									*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC DX.zhao											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/12												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、EENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:無し													*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/
#include "eenl_cmn_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*==========================================================================*/
/*	Description		:ファイル書込み											*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　filename	ファイル名（フルパス）					*/
/*					 [in]　data	データポインタ								*/
/*					 [in]　size	データサイズ								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:引数エラー/-2:システムコールエラー)		*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_CmnFileWrite(schar* filename, void* data, ulong size)
{

#ifndef CROSSDEBUG
	sint	fd = -1;
	ulong	crc = 0u;
	s64		ret = -1;
	slong	retCode = -1;
#endif

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (filename == NULL) {
		DLOG_Error("fileneame parameter NULL.");
		DEBUG_TRCOUT();
		return retCode;
	} else if (data == NULL) {
		DLOG_Error("data parameter NULL.");
		DEBUG_TRCOUT();
		return retCode;
	} else {
		/* DO NOTHING */
	}

#ifndef CROSSDEBUG

	fd = CMN_OPEN(filename, O_WRONLY | O_CREAT | O_SYNC,
		(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH));
	if (fd < 0) {
		DLOG_Error("%s open error.", filename);
		retCode = -2;
	} else {
		crc = CMN_Crc32(data, size);

		ret = write(fd, &crc, sizeof(sint));
		if (ret < (s64)(sizeof(sint))) {
			DLOG_Error("%s CRC write error.", filename);
			retCode = -1;
		} else {
			ret = write(fd, data, size);
			if (ret < (s64)size) {
				DLOG_Error("%s data write error.", filename);
				retCode = -2;
			} else {
				retCode = 0;
			}
		}

		CMN_CLOSE(fd);
	}

#endif

	DEBUG_TRCOUT();
	return retCode;
}

/*==========================================================================*/
/*	Description		:ファイル読み出し										*/
/*--------------------------------------------------------------------------*/
/*	param			:[in]　filename	ファイル名（フルパス）					*/
/*					 [in]　data	データ格納用ポインタ						*/
/*					 [in]　size	データサイズ								*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:引数エラー/-2:システムコールエラー		*/
/*					 -3:CRCエラー/-4:ファイルオープンエラー)				*/
/*--------------------------------------------------------------------------*/
/*	detail			:無し													*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_CmnFileRead(schar* filename, void* data, ulong size)
{

#ifndef CROSSDEBUG
	sint	fd = -1;
	ulong	r_crc = 0u;
	ulong	c_crc = 0u;
	s64		ret = -1;
	slong	retCode = -1;
#endif

	DEBUG_TRCIN();

	/* パラメータチェック */
	if (filename == NULL) {
		DLOG_Error("fileneame parameter NULL.");
		DEBUG_TRCOUT();
		return retCode;
	} else if (data == NULL) {
		DLOG_Error("data parameter NULL.");
		DEBUG_TRCOUT();
		return retCode;
	} else {
		/* DO NOTHING */
	}

#ifndef CROSSDEBUG

	fd = CMN_OPEN(filename, O_RDONLY, 0);
	if (fd < 0) {
		/* ※オープン不可をエラーとするかは呼び出し元で判断すること。 */
		DLOG_Debug("%s open error.", filename);
		retCode = -4;
	} else {
		ret = read(fd, &r_crc, sizeof(sint));
		if (ret < (s64)sizeof(sint)) {
			DLOG_Error("%s CRC read error.", filename);
			retCode = -2;
		} else {
			ret = read(fd, data, size);
			if (ret < (s64)size) {
				DLOG_Error("%s data read error.", filename);
				retCode = -2;
			} else {
				c_crc = CMN_Crc32(data, size);

				if (r_crc != c_crc) {
					DLOG_Error("crc mismatch error.");
					retCode = -3;
				} else {
					retCode = 0;
				}
			}

		}

		CMN_CLOSE(fd);
	}

#endif

	DEBUG_TRCOUT();
	return retCode;
}

/*==========================================================================*/
/*	Description		:ファイルの初期設定										*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:EENL_FileWrite/EENL_FileReadを利用する前に呼び出すこと	*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_FileInit(void)
{
	struct stat st;
	slong ret = -1;
	slong ret2 = 0;


	DEBUG_TRCIN();

	/*ファイルの存在確認 */
	ret = stat(EENL_INF_FPATH3, &st);
	/*最新のファイルサイズと比較する */
	if ((ret == 0) && (st.st_size != EENL_INF3_SIZE2)) {
		if (st.st_size == EENL_INF3_SIZE) {
			/*過去のファイルのサイズと同じ場合はファイル内容を引き継ぐ */
			DLOG_Debug("different size inf file found. file size : [%ld].",
				(s64)st.st_size);

			/*ファイルを読み込み、追加となった分だけ新規に設定する */
			EENL_FileRead();

			/*マルチ入力PCS 設置場所(0x81) 蓄電池・太陽光と共通であるため、蓄電池の設置場所を設定する */
			g_eenlMpcs0x81 = g_eenlBattery0x81;

			ret2 = EENL_FileWrite();
			if (ret2 != 0) {
				/*失敗した場合、グローバル変数に初期状態は入っているので処理続行 */
				DLOG_Error("inf file make error!");

			} else {
				/* DO NOTHING */
			}
		} else {
			/*最新とも過去のサイズとも異なる場合は初期化する */
			DLOG_Debug("different size inf file found. initialize file[%s].",
				EENL_INF_FPATH3);
			ret = -1;
		}
	}

	if (ret != 0) {
		/* ファイルがない場合 */

		/* ディレクトリの作成 */
		ret = mkdir(EENL_INF_DIR, 0777);
		if (ret != 0) {
			/* 既にディレクトリがある場合も失敗をするので、次の処理へ */
		} else {
			/* DO NOTHING */
		}

		/* 初期状態のファイルを作成 */
		/* グローバル変数を初期状態に設定 */
		/* 蓄電池 設置場所(0x81) */
		g_eenlBattery0x81 = EENL_LOCATION_INSIDE;
		/* 蓄電池 充電量設定値(0xE7) */
		g_eenlBattery0xE7 = 0xFFFFFFFF;
		/* 蓄電池 放電量設定値(0xE8) */
		g_eenlBattery0xE8 = 0xFFFFFFFF;
		/* 蓄電池 AC充電量設定値(0xAA) */
		g_eenlBattery0xAA = 0xFFFFFFFF;
		/* 蓄電池 AC放電量設定値(0xAB) */
		g_eenlBattery0xAB = 0xFFFFFFFF;
		/* 太陽光 設置場所(0x81) */
		g_eenlSolar0x81 = EENL_LOCATION_INSIDE;
		/* 電気自動車充放電器クラスの設置場所(EPC:0x81) 2017/06/21 M1対応 */
		g_eenlV2h0x81 = EENL_LOCATION_GARAGE;
		/* マルチ入力PCS 設置場所(0x81) */
		g_eenlMpcs0x81 = EENL_LOCATION_INSIDE;

		ret = EENL_FileWrite();
		if (ret != 0) {
			/*失敗した場合、グローバル変数に初期状態は入っているので処理続行 */
			DLOG_Error("inf file make error!");
		} else {
			/* DO NOTHING */
		}

	} else {
		DLOG_Debug("[%s] is found!", EENL_INF_FPATH3);
	}

#if EENL_VPP_RAMFILE
	/*RAMファイル読み込み(ファイル存在確認は呼び出し先で実施) */
	ret = EENL_CmnFileRead(EENL_TESTRUNSTATE_FILE,
		&g_eenlTestRunState, sizeof(g_eenlTestRunState));
	if (ret == 0) {
		DLOG_Debug("read ram file. test run state = %d.",
			g_eenlTestRunState);
	} else {
		/*読み込めなくても処理は続行 */
		g_eenlTestRunState = EENL_TESTRUN_STOP;
		DLOG_Debug("ram file NOT found. test run state = %d.",
			g_eenlTestRunState);
	}
#endif /* EENL_VPP_RAMFILE */

	DEBUG_TRCOUT();
	return 0;
}

/*==========================================================================*/
/*	Description		:ファイルへの保存										*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:EENL_FileInitが事前に実行済みであること				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/
slong EENL_FileWrite(void)
{
	FILE* fp = NULL;
	slong retCode = -1;

	DEBUG_TRCIN();

	/*ファイルを開く */
	fp = CMN_FOPEN(EENL_INF_FPATH3, "w");
	if (fp == NULL) {
		/*ファイルが開けない場合 */
		retCode = -1;	/* error */
	} else {
		/*ファイルに書き込み */

		/* 蓄電池 設置場所(0x81) */
		/* 蓄電池 充電量設定値(0xE7) */
		/* 蓄電池 放電量設定値(0xE8) */
		/* 太陽光 設置場所(0x81) */
		/* 電気自動車充放電器クラスの設置場所(EPC:0x81) 2017/06/21 M1対応 */
		/* 蓄電池クラスのAC充電量設定値(EPC:0xAA) 2017/06/21 M1対応 */
		/* 蓄電池クラスのAC充電量設定値(EPC:0xAB) 2017/06/21 M1対応 */
		/* マルチ入力PCS 設置場所(0x81) */
		fprintf(fp, "%02x\t%08lx\t%08lx\t%02x\t%02x\t%08lx\t%08lx\t%02x",
			g_eenlBattery0x81,
			g_eenlBattery0xE7, g_eenlBattery0xE8,
			g_eenlSolar0x81,
			g_eenlV2h0x81,
			g_eenlBattery0xAA,
			g_eenlBattery0xAB,
			g_eenlMpcs0x81
		);

		fflush(fp);

		/* ファイルを閉じる */
		CMN_FCLOSE(fp);

		retCode = 0;
	}

	DEBUG_TRCOUT();
	return retCode;
}

/*==========================================================================*/
/*	Description		:ファイルからの読み込み									*/
/*--------------------------------------------------------------------------*/
/*	param			:無し													*/
/*--------------------------------------------------------------------------*/
/*	return			:処理結果												*/
/*					 (0:正常終了/-1:異常)									*/
/*--------------------------------------------------------------------------*/
/*	detail			:EENL_FileInitが事前に実行済みであること				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/12 : DHC DX.zhao : 新規作成					*/
/*==========================================================================*/

slong EENL_FileRead(void)
{
	FILE* fp = NULL;
	ulong work1 = 0u;
	ulong work2 = 0u;
	ulong work5 = 0u;
	ulong work6 = 0u;
	slong retCode = -1;

	DEBUG_TRCIN();

	/* ファイルを開く */
	fp = CMN_FOPEN(EENL_INF_FPATH3, "r");
	if (fp == NULL) {
		/*ファイルが開けない場合 */
		retCode = -1;	/*error */
	} else {
		/*ファイルから読み込み */

		/* 蓄電池 設置場所(0x81) */
		/* 蓄電池 充電量設定値(0xE7) */
		/* 蓄電池 放電量設定値(0xE8) */
		/* 太陽光 設置場所(0x81) */
		/* 電気自動車充放電器クラスの設置場所(EPC:0x81) */
		/* 蓄電池クラスのAC充電量設定値(EPC:0xAA) */
		/* 蓄電池クラスのAC充電量設定値(EPC:0xAB) */
		/* マルチ入力PCS 設置場所(0x81) */
		fscanf(fp, "%02lx\t%08lx\t%08lx\t%02lx\n%02lx\t%08lx\t%08lx\t%02lx",
			&work1,
			&g_eenlBattery0xE7, &g_eenlBattery0xE8,
			&work2,
			&work5,
			&g_eenlBattery0xAA,
			&g_eenlBattery0xAB,
			&work6
		);

		g_eenlBattery0x81 = work1 & 0x00FF;
		g_eenlSolar0x81 = work2 & 0x00FF;
		/* 電気自動車充放電器クラスの設置場所(EPC:0x81) */
		g_eenlV2h0x81 = work5 & 0x00FF;
		/* マルチ入力PCS 設置場所(0x81) */
		g_eenlMpcs0x81 = work6 & 0x00FF;

		/* ファイルを閉じる */
		CMN_FCLOSE(fp);

		retCode = 0;
	}

	DEBUG_TRCOUT();
	return retCode;
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
