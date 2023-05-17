/****************************************************************************/
/*	Description		:FW更新機能 データ操作ヘッダファイル					*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC.zhangs												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/20												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:FW更新機能の機能名は、EOTAとす							*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 DHC CORPORATION								*/
/****************************************************************************/
#ifndef	__EOTA_DATA_G_H__
#define	__EOTA_DATA_G_H__

#include "typedef.h"
#include "eota_setting_g.h"

/*--------------------------------------------------------------------------*/
/* マクロ定義																*/
/*--------------------------------------------------------------------------*/
#define EOTA_FW_SIZE_FROMAT	((schar*)",")		/* コンマ					*/
#define EOTA_DOT_SIZE		((uchar)4u)			/* コンマサイズ				*/
#define EOTA_HEX_DATA_SIZE	((uchar)4u)			/* Hexデータサイズ			*/
#define EOTA_INI_DATA		((ushort)0x20)		/* パティング				*/

/*--------------------------------------------------------------------------*/
/* 非公開型宣言、定義														*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* 定数宣言、定義															*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* 変数宣言、定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* プロトタイプ宣言															*/
/*--------------------------------------------------------------------------*/
/* 初期化 */
extern void EOTA_DataIni(void);
/* ダウンロード経由設定 */
extern void EOTA_SetDownSrc(uchar DownSrc);
/* ダウンロード経由取得 */
extern uchar EOTA_GetDownSrc(void);
/* FW 更新実施状態設定 */
extern void EOTA_SetRunState(uchar fwRunState);
/* FW 更新実施状態取得 */
extern uchar EOTA_GetRunState(void);
/* FW 更新状態設定 */
extern void EOTA_SetUpdateState(uchar UpdateState);
/* FW 更新状態取得 */
extern uchar EOTA_GetUpdateState(void);
/* FW 更新 LED フラグ設定 */
extern void EOTA_SetUpdateLed(uchar UpdateLed);
/* FW 更新 LED フラグ取得 */
extern uchar EOTA_GetUpdateLed(void);
/* ダウンロード・解析対象設定 */
extern void EOTA_SetDownAnalyTgt(uchar DownAnalyTgt);
/* ダウンロード・解析対象取得 */
extern uchar EOTA_GetDownAnalyTgt(void);
/* 解析結果設定 */
extern void EOTA_SetAnalyRes(uchar AnalyRes);
/* 解析結果取得 */
extern uchar EOTA_GetAnalyRes(void);
/* TGT BOARD設定 */
extern void EOTA_SetTgtBoard(uchar BoardTgt);
/* TGT BOARD取得 */
extern uchar EOTA_GetTgtBoard(void);
/* TGT INV設定 */
extern void EOTA_SetTgtInv(uchar InvTgt);
/* TGT INV取得 */
extern uchar EOTA_GetTgtInv(void);
/* TGT PV設定	 */
extern void EOTA_SetTgtPV(uchar PvTgt);
/* TGT PV取得	 */
extern uchar EOTA_GetTgtPV(void);
/* TGT BDC設定 */
extern void EOTA_SetTgtBDC(uchar BdcTgt);
/* TGT BDC取得 */
extern uchar EOTA_GetTgtBDC(void);
/* TGT J.B.設定 */
extern void EOTA_SetTgtJB(uchar JbTgt);
extern uchar EOTA_GetTgtJB(void);
/* 更新 No設定 */
extern void EOTA_SetUpdNO(ushort UpdNo);
/* 更新 No取得 */
extern ushort EOTA_GetUpdNO(void);
/* 削除フラグ設定 */
extern void EOTA_SetDownDelFlg(uchar DownDelFlg);
/* 削除フラグ取得 */
extern uchar EOTA_GetDownDelFlg(void);
/* 更新 Mode設定 */
extern void EOTA_SetUpdMode(uchar UpdMode);
/* 更新 Mode取得 */
extern uchar EOTA_GetUpdMode(void);
/* 更新部品設定 */
extern void EOTA_SetUpdProjectState(uchar UpdProject);
/* 更新部品取得 */
extern uchar EOTA_GetUpdProjectState(void);
/* 更新中状態設定 */
extern void EOTA_SetUpdIngState(uchar UpdIngState);
/* 更新中状態取得 */
extern uchar EOTA_GetUpdIngState(void);

/* FW 更新実施時刻設定	 */
extern void EOTA_SetUpdTime(const schar * UpdTime);
/* FW 更新実施時刻取得	 */
extern void EOTA_GetUpdTime(schar ** UpdTime);
/* 更新中止状態設定	 */
extern void EOTA_SetUpdStop(uchar UpdateStop);
/* 更新中止状態取得	 */
extern uchar EOTA_GetUpdStop(void);
/* 前回のメッセージ設定 */
extern void EOTA_SetBefMessage(EOTA_CMD11_REQ_INFO_T * Message);
/* 前回のメッセージ取得 */
extern void EOTA_GetBefMessage(EOTA_CMD11_REQ_INFO_T * Message);

/* Invバージョン設定 */
extern void EOTA_SetInvVersion(const schar * Version);
/* Invバージョン取得 */
extern void EOTA_GetInvVersion(schar ** Version);
/* Inv BANK1のFW ファイル名設定 */
extern void EOTA_SetInvBank1File(const schar * Bank1File);
/* Inv BANK1のFW ファイル名取得 */
extern void EOTA_GetInvBank1File(schar ** Bank1File);
/* Inv BANK2のFW ファイル名設定 */
extern void EOTA_SetInvBank2File(const schar * Bank2File);
/* Inv BANK2のFW ファイル名取得 */
extern void EOTA_GetInvBank2File(schar ** Bank2File);

/* Pvバージョン設定 */
extern void EOTA_SetPvVersion(const schar * Version);
/* Pvバージョン取得 */
extern void EOTA_GetPvVersion(schar ** Version);
/* PV BANK1のFW ファイル名設定 */
extern void EOTA_SetPvBank1File(const schar * Bank1File);
/* PV BANK1のFW ファイル名取得 */
extern void EOTA_GetPvBank1File(schar ** Bank1File);
/* PV BANK2のFW ファイル名設定 */
extern void EOTA_SetPvBank2File(const schar * Bank2File);
/* PV BANK2のFW ファイル名取得 */
extern void EOTA_GetPvBank2File(schar ** Bank2File);

/* BDCバージョン設定 */
extern void EOTA_SetBdcVersion(const schar * Version);
/* BDCバージョン取得 */
extern void EOTA_GetBdcVersion(schar ** Version);
/* BDC BANK1のFW ファイル名設定 */
extern void EOTA_SetBdcBank1File(const schar * Bank1File);
/* BDC BANK1のFW ファイル名取得 */
extern void EOTA_GetBdcBank1File(schar ** Bank1File);
/* BDC BANK2のFW ファイル名設定 */
extern void EOTA_SetBdcBank2File(const schar * Bank2File);
/* BDC BANK2のFW ファイル名取得 */
extern void EOTA_GetBdcBank2File(schar ** Bank2File);

/* J.B. バージョン設定  */
extern void EOTA_SetJbVersion(const schar * Version);
/* J.B. バージョン取得 */
extern void EOTA_GetJbVersion(schar ** Version);
/* J.B. FW ファイル名設定 */
extern void EOTA_SetJbFile(const schar * File);
/* J.B. FW ファイル名取得 */
extern void EOTA_GetJbFile(schar ** File);
/* EEPROM Checksum設定 */
extern void EOTA_SetJbEepromChecksum(const schar * EepromChecksum);
/* EEPROM Checksum取得 */
extern void EOTA_GetJbEepromChecksum(schar ** EepromChecksum);
/* Flash Checksum設定 */
extern void EOTA_SetJbFalshChekusum(const schar * FalshChekusum);
/* Flash Checksum取得 */
extern void EOTA_GetJbFalshChekusum(schar ** FalshChekusum);

/* 更新ファイルフォルダ設定 */
extern void EOTA_SetUpdFileDri(const schar * Dir);
/* 更新ファイルフォルダパス取得 */
extern void EOTA_GetUpdFileDri(schar ** Dir);

/* Inv BANK1 の ファイルサイズ設定 */
extern void EOTA_SetInvBank1FileSize(slong Bank1FileSize);
/* Inv BANK1 の ファイルサイズ取得 */
extern slong EOTA_GetInvBank1FileSize();
/* Inv BANK2 の ファイルサイズ設定 */
extern void EOTA_SetInvBank2FileSize(slong Bank2FileSize);
/* Inv BANK2 の ファイルサイズ取得 */
extern slong EOTA_GetInvBank2FileSize();
/* PV BANK1 の ファイルサイズ設定 */
extern void EOTA_SetPVBank1FileSize(slong Bank1FileSize);
/* PV BANK1 の ファイルサイズ取得 */
extern slong EOTA_GetPVBank1FileSize();
/* PV BANK2 の ファイルサイズ設定 */
extern void EOTA_SetPVBank2FileSize(slong Bank2FileSize);
/* PV BANK2 の ファイルサイズ取得 */
extern slong EOTA_GetPVBank2FileSize();
/* BDC BANK1 の ファイルサイズ設定 */
extern void EOTA_SetBdcBank1FileSize(slong Bank1FileSize);
/* BDC BANK1 の ファイルサイズ取得 */
extern slong EOTA_GetBdcBank1FileSize();
/* BDC BANK2 の ファイルサイズ設定 */
extern void EOTA_SetBdcBank2FileSize(slong Bank2FileSize);
/* BDC BANK2 の ファイルサイズ取得 */
extern slong EOTA_GetBdcBank2FileSize();
/* J.B. ファイルサイズ設定 */
extern void EOTA_SetJbFileSize(slong FileSize);
/* J.B. ファイルサイズ取得 */
extern slong EOTA_GetJbFileSize();
/* 転送中Tgt設定 */
extern void EOTA_SetUpdTgt(uchar UpdTgt);
/* 転送中Tgt取得 */
extern uchar EOTA_GetUpdTgt();
/* 転送中ファイルoffset設定 */
extern void EOTA_SetUpdFileOffset(slong UpdFileOffset);
/* 転送中ファイルoffset取得 */
extern slong EOTA_GetUpdFileOffset();

/* 送信情報保存 */
extern void EOTA_SetSendData(EOTA_UPDATE_SETUP_T * update_setup);
/* 更新Sec取得 */
extern slong EOTA_GetModeFUpdTime();

/* タイマー設定 */
/* Description	:一回応答タイム状態設定 */
extern void EOTA_SetInvTimeRes(uchar InvTimeRes);
/* 一回応答タイム状態取得 */
extern uchar EOTA_GetInvTimeRes(void);
/* 通信タイムアウト状態設定 */
extern void EOTA_SetInvTimeOut(uchar InvTimeOut);
/* 通信タイムアウト状態取得 */
extern uchar EOTA_GetInvTimeOut(void);

/* メッセージ作成 */
/* CMD11の転送メッセージ作成(転送 FW 問い合わせ)*/
extern uchar EOTA_SetCMD11Inquiry(uchar Tgt, EOTA_CMD11_REQ_INFO_T * Mssage);
/* CMD11のFW 転送開始メッセージ作成 */
extern uchar EOTA_SetCMD11Start(uchar Tgt, uchar BankNo,
						EOTA_CMD11_REQ_INFO_T * Mssage);
/* CMD11のFW 転送メッセージ作成 */
extern uchar EOTA_SetCMD11Send(ulong   DataNo,
						uchar * Data,
						size_t Len,
						EOTA_CMD11_REQ_INFO_T * Mssage);
/* CMD11のFW 転送完了メッセージ作成 */
extern uchar EOTA_SetCMD11End(EOTA_CMD11_REQ_INFO_T * Mssage);
/* CMD11のFW 転送完了終了メッセージ作成 */
extern uchar EOTA_SetCMD11Stop(EOTA_CMD11_REQ_INFO_T * Mssage);
/* CMD11のBusy 確認メッセージ作成 */
extern uchar EOTA_SetCMD11Busy(EOTA_CMD11_REQ_INFO_T * Mssage);
/* CMD60メッセージ作成 */
extern void EOTA_SetCMD60(/*, Message */);

#endif	/* __EOTA_DATA_G_H__ */
