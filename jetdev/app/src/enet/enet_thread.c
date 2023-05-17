/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jingtong.li										*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/23												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:														*/
/*--------------------------------------------------------------------------*/
/*	Note			:														*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2020 NICHICON CORPORATION							*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*	Include header File														*/
/*--------------------------------------------------------------------------*/
#include <poll.h>
#include <fcntl.h>
#include "enet_cmn_g.h"
#include "enet_thread.h"
#include "enet_thread_g.h"
#include "enet_serv_g.h"
#include "enet_file_g.h"
#include "enet_g.h"
/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/
extern uchar g_enet_WlanLimitSignal;
extern EnetDeviceRunMode_t g_enet_WiFiRunMode;
/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
pthread_mutex_t s_ENET_SystemCmdMutex; /* system関数によるコマンド実行用 */

EnetIpv4Settings_t s_ENET_V4mem; /* IP監視用、変化前IP */

FrmPthread_t s_ENET_Dhcp;
FrmPthread_t s_ENET_WatchIP;
FrmPthread_t s_ENET_SendPing;
FrmPthread_t s_ENET_WlanSetting;
FrmPthread_t s_ENET_Hostapd;
FrmPthread_t s_ENET_ScanEssid;
FrmPthread_t s_ENET_Inotify;
/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:DHCPクライアントをすべて終了する						*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:DHCPクライアントをすべて終了する						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_ExecCmdKillDhcp(void)
{
	slong fncRet = ENET_ERR1;
	DEBUG_TRCIN();
	CMN_MutexLock(&s_ENET_SystemCmdMutex);
	if (system("killall udhcpc") == 0)
	{
		/* コマンド実行成功 */
		fncRet = ENET_SUCCESS;
	}
	CMN_MutexUnlock(&s_ENET_SystemCmdMutex);
	DLOG_Debug("cmd [ %s ].\n", fncRet==ENET_SUCCESS?"OK":"NG");
	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:DHCPクライアント起動をスレッド実行する					*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:DHCPクライアント起動をスレッド実行する					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_ExecDhcpThread(void)
{
	slong fncRet = 0;
	DEBUG_TRCIN();

	fncRet = FRM_PthreadCreate(&s_ENET_Dhcp, NULL, enet_ExecDhcpThreadMain, (void*) NULL);

	DEBUG_TRCOUT();
	return (fncRet);
}


/*==========================================================================*/
/*	Description		:ESSIDのスキャンをスレッド実行する						*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong cPort	結果通知するプロセスのCポート			*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:ESSIDのスキャンをスレッド実行する						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_ExecScanEssidThread(ulong cPort)
{
	slong fncRet = 0;
	DEBUG_TRCIN();

	fncRet = FRM_PthreadCreate(&s_ENET_ScanEssid, NULL, enet_ExecScanEssidThreadMain, (void*) cPort);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:hostapdをすべて終了する							*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:hostapdをすべて終了する							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_ExecCmdKillHostapd(void)
{
	slong fncRet = ENET_ERR1;
	DEBUG_TRCIN();
	CMN_MutexLock(&s_ENET_SystemCmdMutex);
	if (system("killall udhcpd;killall hostapd") == 0)
	{
		/* コマンド実行成功 */
		fncRet = ENET_SUCCESS;
	}
	CMN_MutexUnlock(&s_ENET_SystemCmdMutex);
	DLOG_Debug("cmd [ %s ].\n", fncRet==ENET_SUCCESS?"OK":"NG");	
	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:hostapd起動をスレッド実行する					*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:hostapd起動をスレッド実行する					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_ExecHostapdThread(void)
{
	slong fncRet = 0;
	DEBUG_TRCIN();
	fncRet = FRM_PthreadCreate(&s_ENET_Hostapd, NULL, enet_ExecHostapdThreadMain, (void*) NULL);
	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:無線LAN設定をスレッド実行する							*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:無線LAN設定をスレッド実行する							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_ExecWlanSettingThread(void)
{
	slong fncRet = 0;
	DEBUG_TRCIN();

	fncRet = FRM_PthreadCreate(&s_ENET_WlanSetting, NULL, enet_ExecWlanSettingThreadMain, (void*) NULL);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:PING送信をスレッド実行する								*/
/*--------------------------------------------------------------------------*/
/*	param			:ulong cPort											*/
/*					結果通知するプロセスのCポート、0のとき通知しない		*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:PING送信をスレッド実行する								*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_SendPingThread(ulong cPort)
{
	slong fncRet = 0;
	DEBUG_TRCIN();

	fncRet = FRM_PthreadCreate(&s_ENET_SendPing, NULL, enet_SendPingThreadMain, (void*) cPort);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:IP変化監視をスレッド実行する							*/
/*--------------------------------------------------------------------------*/
/*	param			:無い													*/
/*--------------------------------------------------------------------------*/
/*	return			:slong	処理結果										*/
/*						0	正常	-1	異常								*/
/*--------------------------------------------------------------------------*/
/*	detail			:IP変化監視をスレッド実行する							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
slong ENET_WatchIPThread(void)
{
	slong fncRet = 0;
	DEBUG_TRCIN();

	fncRet = FRM_PthreadCreate(&s_ENET_WatchIP, NULL, enet_WatchIPThreadMain, (void*) NULL);

	DEBUG_TRCOUT();
	return (fncRet);
}

/*==========================================================================*/
/*	Description		:プロセスを初期化する									*/
/*--------------------------------------------------------------------------*/
/*	param			:int argc	プロセス起動時の引数						*/
/*					char *argv[]	プロセス起動時の引数					*/
/*--------------------------------------------------------------------------*/
/*	return			:無い													*/
/*--------------------------------------------------------------------------*/
/*	detail			:プロセスを初期化する									*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void ENET_InitEnetThread(int argc, char *argv[])
{
	DEBUG_TRCIN();

	/* Mutexの初期化 */
	CMN_MutexInit(&s_ENET_SystemCmdMutex);
	if (ENET_GetPrefIPv4AutoFlg())
	{
		DLOG_Debug("init auto addr.\n");
		ENET_GetPrefIPv4fmtIP(ENET_KEY_IPV4_ADDR_AUTO, &s_ENET_V4mem.ip);
	}
	else
	{
		DLOG_Debug("init self addr.\n");
		ENET_GetPrefIPv4fmtIP(ENET_KEY_IPV4_ADDR_SELF, &s_ENET_V4mem.ip);
	}

	DEBUG_TRCOUT();
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*==========================================================================*/
/*	Description		:DHCPクライアント起動の前後処理を実行する				*/
/*--------------------------------------------------------------------------*/
/*	param			:void* arg	未使用										*/
/*--------------------------------------------------------------------------*/
/*	return			:void*	未使用											*/
/*--------------------------------------------------------------------------*/
/*	detail			:DHCPクライアント起動の前後処理を実行する				*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void* enet_ExecDhcpThreadMain(void* arg)
{
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	/* 排他ロック開始 */
	CMN_MutexLock(&s_ENET_SystemCmdMutex);
	system("/sbin/route del default");	/* デフォルトGWを削除 */
	snprintf(buf, ENET_BUF_STR_LEN, "sleep 3;killall udhcpc;/sbin/udhcpc -b -i %s -t %ld -T %ld -A %ld &", /* ハードウェア遅延を考慮 */
	ENET_GetPrefNetworkTypeStr(), ENET_GetPrefDhcpRetries(), ENET_GetPrefDhcpTimeout(), ENET_GetPrefDhcpTryagain());
	DLOG_Debug("cmd : %s\n", buf);
	system(buf);
	CMN_MutexUnlock(&s_ENET_SystemCmdMutex);
	enet_CancelThread(s_ENET_Dhcp);
	
	DEBUG_TRCOUT();
	return NULL;
}

/*==========================================================================*/
/*	Description		:ESSIDスキャンの前後処理を実行する						*/
/*--------------------------------------------------------------------------*/
/*	param			:void* arg	結果通知するプロセスのCポート				*/
/*--------------------------------------------------------------------------*/
/*	return			:void*	未使用											*/
/*--------------------------------------------------------------------------*/
/*	detail			:ESSIDスキャンの前後処理を実行する						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void* enet_ExecScanEssidThreadMain(void* arg)
{
	ulong cPort = (ulong) arg;
	slong fncRet = ENET_ERR1;
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	snprintf(buf, ENET_BUF_STR_LEN, "/usr/bin/iwlist2csv.sh %s %s", ENET_GetNetworkTypeStrTbl(ENET_NETWORK_WIRELESS), ENET_FILE_AP_CSV);
	DLOG_Debug("cmd : %s\n", buf);
	CMN_MutexLock(&s_ENET_SystemCmdMutex);
	if (system(buf) == 0)
	{
		/* コマンド実行成功 */
		fncRet = ENET_SUCCESS;
	}
	CMN_MutexUnlock(&s_ENET_SystemCmdMutex);
	DLOG_Debug("cmd [ %s ].\n", fncRet==ENET_SUCCESS?"OK":ENET_YELLOW"NG"ENET_RESET);

	if (cPort != 0)
	{
		/* コールバックで呼び元に通知 */
		ENET_ScanEssidLfwASyncRet(cPort);
	}
	/* 自スレッド識別情報でデタッチ */
	enet_CancelThread(s_ENET_ScanEssid);
	
	DEBUG_TRCOUT();
	return NULL;
}

/*==========================================================================*/
/*	Description		:hostapd起動の前後処理を実行する					*/
/*--------------------------------------------------------------------------*/
/*	param			:void* arg	未使用										*/
/*--------------------------------------------------------------------------*/
/*	return			:void*	未使用											*/
/*--------------------------------------------------------------------------*/
/*	detail			:hostapd起動の前後処理を実行する					*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void* enet_ExecHostapdThreadMain(void* arg)
{
	schar buf[ENET_BUF_STR_LEN] = { };
	DEBUG_TRCIN();

	enet_RmInsMod();
	snprintf(buf, ENET_BUF_STR_LEN, "hostapd %s -B;udhcpd &", ENET_FILE_WPACONF);
	CMN_MutexLock(&s_ENET_SystemCmdMutex);
	system(buf);
	CMN_MutexUnlock(&s_ENET_SystemCmdMutex);

	enet_CancelThread(s_ENET_Hostapd);
	DEBUG_TRCOUT();
	return NULL;
}

/*==========================================================================*/
/*	Description		:無線LAN設定の前後処理を実行する						*/
/*--------------------------------------------------------------------------*/
/*	param			:void* arg	未使用										*/
/*--------------------------------------------------------------------------*/
/*	return			:void*	未使用											*/
/*--------------------------------------------------------------------------*/
/*	detail			:無線LAN設定の前後処理を実行する						*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void* enet_ExecWlanSettingThreadMain(void* arg)
{
	EnetWlanSettings_t wlan;
	DEBUG_TRCIN();
	ENET_GetPrefWlanSetting(&wlan);
	if (ENET_InitPrefHostapd(&wlan))
	{
		ENET_ExecHostapdThread();
	}
	else
	{
		/* hostapd用の設定ファイル初期化(生成)失敗 */
		DLOG_Error("Err! ENET_InitPrefHostapd NG.\n");
	}
	enet_CancelThread(s_ENET_WlanSetting);
	DEBUG_TRCOUT();
	return NULL;
}

/*==========================================================================*/
/*	Description		:PINGを送信する											*/
/*--------------------------------------------------------------------------*/
/*	param			:EnetIpv4_t* sendAddr	PING送信アドレス				*/
/*					slong timeoutMs	PING送信タイムアウト(ミリ秒)			*/
/*					ulong cPort	結果通知するプロセスのCポート				*/
/*--------------------------------------------------------------------------*/
/*	return			:無い													*/
/*--------------------------------------------------------------------------*/
/*	detail			:PINGを送信する											*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void enet_SendPing(EnetIpv4_t* sendAddr, slong timeoutMs, ulong cPort)
{
	slong fncRet = 0;
	sint skfd = 0;
	slong pollResult = 0;
	struct pollfd fds[1];
	struct icmphdr hdr;
	struct sockaddr_in addr;

	schar buf[ENET_BUF_STR_LEN]; /* 受信パケットバッファ */
	struct icmphdr *icmphdrptr;
	struct iphdr *iphdrptr;
	DEBUG_TRCIN();

	DLOG_Debug("sendAddr : ", (*sendAddr));
	DLOG_Debug("timeout : %d[ms]\n", timeoutMs);

	if (ENET_GetPrefNetworkType() == ENET_NETWORK_NONE)
	{
		DLOG_Debug("network is disabled : router err clear.\n");
		ENET_SetRmcErrCode(ENET_ERRCODE_UNSET_ROUTER_COMM); /* ネットワークなしの場合、ルータPING疎通成功(エラー解除)を通知 */
		ENET_ChkRouterConnectionLfwASyncRet(cPort, ENET_SUCCESS);
		return;
	}
	if (sendAddr->addr32bit == 0)
	{
		DLOG_Debug("sendAddr is NOT ip.\n");
		ENET_SetRmcErrCode(ENET_ERRCODE_SET_ROUTER_COMM); /* ルータPING疎通エラーを通知(送り先IPなし) */
		ENET_ChkRouterConnectionLfwASyncRet(cPort, ENET_ERR1);

		if (ENET_GetPrefIPv4AutoFlg())
		{
			ENET_ExecDhcpThread();
		}
		return;
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = sendAddr->addr32bit;

#ifndef CROSSDEBUG
	/* RAWソケット */
	skfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
#endif
	if (skfd < 0)
	{
		DLOG_Error("Err! socket is NULL.\n");
		return;
	}

	/* ICMP 送信処理 */
	memset(&hdr, 0, sizeof(hdr));
	/* ICMPヘッダ作成 */
	hdr.type = ICMP_ECHO; /* エコー要求 */
	hdr.code = 0;
	hdr.checksum = 0;
	hdr.un.echo.id = 0;
	hdr.un.echo.sequence = 0;
	/* ICMPヘッダのチェックサム計算 */
	hdr.checksum = ENET_IcmpChksum((ushort*) &hdr, sizeof(hdr));
	fncRet = sendto(skfd, (schar*) &hdr, sizeof(hdr), 0, (struct sockaddr*) &addr, sizeof(addr));
	if (fncRet < 1)
	{
		DLOG_Error("Err! sendto : %d\n", fncRet);
		close(skfd);
		return;
	}

	/* ICMP 受信待機 */
	/* pollfd構造体設定（fileDescriptor/requestedEvents/returnedEvents） */
	fds[0].fd = skfd;
	/* 読み出し可能データあり */
	fds[0].events = POLLIN;
	fds[0].revents = 0;
	/* 受信確認 */
	pollResult = poll(fds, 1, timeoutMs);
	/* タイムアウト(0)orシステムコールエラー(負数)が発生 */
	if (pollResult <= 0)
	{
		DLOG_Error("Err! poll Result : %d\n", pollResult);
		/* ルータPING疎通エラーを通知(タイムアウト) */
		ENET_SetRmcErrCode(ENET_ERRCODE_SET_ROUTER_COMM);
		ENET_ChkRouterConnectionLfwASyncRet(cPort, ENET_ERR1);
#if 0
		if (ENET_GetPrefIPv4AutoFlg()) {
			/* DHCPを再起動しrouting table再構築 */
			enet_ExecCmdDhcp(ENET_GetPrefDhcpRetries(), ENET_GetPrefDhcpTimeout(), ENET_GetPrefDhcpTryagain());
		}
#endif
		close(skfd);
		return;
	}

	/* ICMP 受信処理 */
	memset(buf, 0, sizeof(buf));
	/* 相手ホストからのICMPを受信 */
	fncRet = recv(skfd, buf, sizeof(buf), 0);
	if (fncRet < 1)
	{
		DLOG_Error("Err! recv : ", fncRet);
		close(skfd);
		return;
	}
	/* 受信データからIPヘッダ部分へのポインタを取得 */
	iphdrptr = (struct iphdr *) buf;
	/* 受信データからICMPヘッダ部分へのポインタを取得 */
	icmphdrptr = (struct icmphdr *) (buf + (iphdrptr->ihl * 4));
	/* ICMPヘッダからICMPの種類を判定 */
	switch (icmphdrptr->type)
	{
		/* 応答受信 */
		case ICMP_ECHOREPLY:
			DLOG_Debug("received ICMP ECHO REPLY\n");
			/* ルータPING疎通成功(エラー解除)を通知(正常応答) */
			ENET_SetRmcErrCode(ENET_ERRCODE_UNSET_ROUTER_COMM);
			ENET_ChkRouterConnectionLfwASyncRet(cPort, ENET_SUCCESS);
			break;
		/* 疎通失敗 */
		default:
		 	/* 応答以外の種類 */
			DLOG_Error("Err! received ICMP type-code : %d-%d\n", icmphdrptr->type, icmphdrptr->code);
			/* ルータPING疎通エラーを通知(疎通失敗受信) */
			ENET_SetRmcErrCode(ENET_ERRCODE_SET_ROUTER_COMM);
			ENET_ChkRouterConnectionLfwASyncRet(cPort, ENET_ERR1);

			break;
	}
	close(skfd);

	DEBUG_TRCOUT();
	return;
}

/*==========================================================================*/
/*	Description		:PING送信の前後処理を実行する							*/
/*--------------------------------------------------------------------------*/
/*	param			:void* arg	結果通知するプロセスのCポート				*/
/*--------------------------------------------------------------------------*/
/*	return			:void*	未使用											*/
/*--------------------------------------------------------------------------*/
/*	detail			:PING送信の前後処理を実行する							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void* enet_SendPingThreadMain(void* arg)
{
	ulong cPort = (ulong) arg;
	EnetIpv4_t gateway = { };
	DEBUG_TRCIN();

	/* 取得失敗時も処理を実施 */
	ENET_GetDefaultGW(ENET_GetPrefNetworkTypeStr(), &gateway);
	/* gatewayにping */
	enet_SendPing(&gateway, ENET_GetPrefPingTimeout(), cPort);
	FRM_TimerSetOneShot(ENET_ONE_SHOT_ID_SEND_PING, ENET_GetPrefSendPingInterval(), NULL);
	/* 自スレッド識別情報でデタッチ */
	enet_CancelThread(s_ENET_SendPing);

	DEBUG_TRCOUT();
	return NULL;
}

/*==========================================================================*/
/*	Description		:IP変化監視の前後処理を実行する							*/
/*--------------------------------------------------------------------------*/
/*	param			:void* arg	未使用										*/
/*--------------------------------------------------------------------------*/
/*	return			:void*	未使用											*/
/*--------------------------------------------------------------------------*/
/*	detail			:IP変化監視の前後処理を実行する							*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/22 : DHC jingtong.li : 新規作成				*/
/*==========================================================================*/
void* enet_WatchIPThreadMain(void* arg)
{
	EnetIpv4Settings_t v4now = { };								/* 実際に取得したIP */
	EnetIpv4Settings_t v4pref = { };							/* 設定ファイルから取得したIP */
	EnetNetworkType_t nowType = ENET_GetPrefNetworkType();		/* 設定ファイルのネットワーク */
	schar* nowIfaceName = ENET_GetNetworkTypeStrTbl(nowType);	/* 現在のネットワークIF名 */
	DEBUG_TRCIN();
	/* 取得失敗時も処理を実施 */
	ENET_GetIpAddr(nowIfaceName, (void*) &v4now, ENET_IPV4);
	DLOG_Debug("before : ", s_ENET_V4mem.ip);
	DLOG_Debug("now    : ", v4now.ip);
	if (v4now.autoFlg)
	{
		/* 前回記憶したアドレスと現在のアドレスが違う */
		if (s_ENET_V4mem.ip.addr32bit != v4now.ip.addr32bit)
		{
			/* IP変化通知 */
			DLOG_Debug("auto "ENET_YELLOW"ipChanged!\n"ENET_RESET);
			ENET_SendChgEvnt(ENET_CHGEVNT_IP_ADDRESS);
			/* アドレス更新 */
			s_ENET_V4mem.ip.addr32bit = v4now.ip.addr32bit;
			ENET_SetPrefIPv4fmtIP(ENET_KEY_IPV4_ADDR_AUTO, &v4now.ip);
		}
	}
	else
	{
		/* 前回記憶したアドレスと現在のアドレスが違う */
		if (s_ENET_V4mem.ip.addr32bit != v4now.ip.addr32bit)
		{
			DLOG_Debug("self "ENET_YELLOW"ipChanged!\n"ENET_RESET); /* IP変化通知 */
			ENET_SendChgEvnt(ENET_CHGEVNT_IP_ADDRESS);
			s_ENET_V4mem.ip.addr32bit = v4now.ip.addr32bit; /* アドレス更新 */
		}
		if (ENET_GetPrefIPv4fmtIP(ENET_KEY_IPV4_ADDR_SELF, &v4pref.ip))
		{
			DLOG_Debug("setting: ", v4pref.ip);
			if (v4pref.ip.addr32bit != v4now.ip.addr32bit /* 設定ファイルのアドレスと現在のアドレスが違う */
			&& nowType != ENET_NETWORK_NONE) /* ネットワークが無効以外(=有線or無線) */
			{
				DLOG_Error("Err! unmatch self ip. RecoverStart...\n");
				/* アドレス復元(異常状態からの回復) */
				ENET_UpdateNetworkType();
			}
		}
		/* 手動＋無線＋電波強度0＝再取得(WEPの電波取得失敗) */
		if (nowType == ENET_NETWORK_WIRELESS
		&& ENET_GetWlanStatus() == 0)
		{
			DLOG_Debug("wlan acquireStart...\n");
			ENET_UpdateNetworkType();
			/* 設定更新のため、ルータ疎通エラーとする */
			ENET_SetRmcErrCode(ENET_ERRCODE_SET_ROUTER_COMM);
			/* 無線回復のため最短周期 */
			FRM_TimerSetOneShot(ENET_ONE_SHOT_ID_IP_WATCH, ENET_IP_WATCH_INTERVAL_MIN, NULL);
		}
	}
	/* IP未取得かつ無線の場合、再接続(抜き差し対応) */
	if (v4now.ip.addr32bit == 0
	&& nowType == ENET_NETWORK_WIRELESS)
	{
		DLOG_Debug("wlan RecoverStart...\n");
		ENET_UpdateNetworkType();
		/* 設定更新のため、ルータ疎通エラーとする */
		ENET_SetRmcErrCode(ENET_ERRCODE_SET_ROUTER_COMM);
		/* DHCPを考慮してデフォルト値 */
		FRM_TimerSetOneShot(ENET_ONE_SHOT_ID_IP_WATCH, ENET_IP_WATCH_INTERVAL, NULL);
	}
	else
	{
		/* 通常巡回 */
		FRM_TimerSetOneShot(ENET_ONE_SHOT_ID_IP_WATCH, ENET_GetPrefIpWatchInterval(), NULL);
	}
	/* 自スレッド識別情報でデタッチ */
	enet_CancelThread(s_ENET_WatchIP);

	DEBUG_TRCOUT();
	return NULL;
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/18 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void enet_CancelThread(FrmPthread_t thread)
{
	EnetThdCancel_t thdmsg;
	DEBUG_TRCIN();
	thdmsg.header.msgId = ENET_THREAD_CANCEL;
	thdmsg.thread = thread;

	FRM_MsgSendAsync(ENET_SPORT, sizeof(thdmsg), &thdmsg);
	DEBUG_TRCOUT();
}

/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/27 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void ENET_CreateInotifyThread(void)
{
	slong ret;
	DEBUG_TRCIN();
	ret = FRM_PthreadCreate(&s_ENET_Inotify, NULL, enet_Inotify, NULL);
	if(0 == ret)
	{
		DLOG_Error("FRM_PthreadCreate(enet_Inotify)\n");
	}
	else
	{
		/* 動作なし*/
	}
	DEBUG_TRCOUT();
}
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/04/28 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void* enet_Inotify(void* arg)
{
	const schar *event_str[EVENT_NUM] =
	{
		"IN_ACCESS",
		"IN_MODIFY",
		"IN_ATTRIB",
		"IN_CLOSE_WRITE",
		"IN_CLOSE_NOWRITE",
		"IN_OPEN",
		"IN_MOVED_FROM",
		"IN_MOVED_TO",
		"IN_CREATE",
		"IN_DELETE",
		"IN_DELETE_SELF",
		"IN_MOVE_SELF"
	};

	sint fd = -1;
	schar buf[BUFSIZ];
	sint wd1 = -1;
	ssize_t length;
	ssize_t nread;
	slong i = 0;
	struct inotify_event *event;
	bool_t loopFlag = TRUE;

	DLOG_Debug("enet_Inotify start run");
	fd = inotify_init();
	if(fd < 0)
	{
		DLOG_Error("inotify_init error.");
		return NULL;
	}
	buf[sizeof(buf) - 1] = 0;

	while (loopFlag)
	{
		wd1 = inotify_add_watch(fd, ENET_FILE_WLAN_LIMIT, IN_ALL_EVENTS);

		length = read(fd, buf, sizeof(buf) - 1);
		nread = 0;

		while(length > 0)
		{
			event = (struct inotify_event *)&buf[nread];

			for(i = 0; i< EVENT_NUM; i++)
			{
				if( (event->mask >> i) & 1 )
				{
					if(event->len > 0)
					{
						DLOG_Debug("%s --- %s\n", event->name, event_str[i]);
					}
					else if(event->len == 0)
					{
						if(event->wd == wd1)
						{
							if(0 == strcmp("IN_CLOSE_WRITE", event_str[i]))
							{
								ENET_ReadWlanLimitFile();
								if(1 == g_enet_WlanLimitSignal)
								{
									FRM_TimerCancelOneShot(ENET_ONE_SHOT_ID_WLANLMT);
									ENET_ExecCmdKillHostapd();
									DLOG_Debug("modify WLANLimit.txt value = 0, wifi shut down");
								}
							}
						}
					}
				}
			}
			nread = nread + sizeof(struct inotify_event) + event->len;
			length = length - sizeof(struct inotify_event) - event->len;
		}
	}
	close(fd);
	return;
}
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/12 : DHC Cao.HZ : 新規作成						*/
/*==========================================================================*/
void enet_RmInsMod(void)
{
	CMN_MutexLock(&s_ENET_SystemCmdMutex);
	system("rmmod /lib/modules/extra/wilc-sdio;insmod /lib/modules/extra/wilc-sdio.ko;ifconfig wlan0 192.168.10.1 up");
	CMN_MutexUnlock(&s_ENET_SystemCmdMutex);
}
/*End Of File*/
