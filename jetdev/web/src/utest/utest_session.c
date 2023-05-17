#include <string.h>

#include "test_g.h"
#include "util_logger_g.h"
#include "session_g.h"
#include "common_g.h"
#include "config.h"
#include "stub_g.h"
#include "frm_cgiapi_g.h"

void TEST_session_generate(void)
{
	schar tmp_sessionId_ucp[17];
	SessionUserInfo_t userInfo;
	strcpy(userInfo.appVersion, "1.0");
	strcpy(userInfo.phoneType, "android");
	strcpy(userInfo.userId, "E001");
	FRM_SetSession(tmp_sessionId_ucp, &userInfo);
	LOG_debug("session id: %s", tmp_sessionId_ucp);

	/**
	 *  set cookie
	 */
	LOG_debug("Set-Cookie:auth=%s\n", tmp_sessionId_ucp);
}

void test_checkSessin(void)
{
	schar tmp_session_id_ucp[SESSION_ID_LENGTH] = "HMMJHNVCWZMRKCTF2";
	bool_t ret = FRM_CheckSession(tmp_session_id_ucp);
	LOG_debug("ret: %d", ret);
}

void TEST_GetSession(void)
{
	schar tmp_sessionId_ucp[17];
	SessionUserInfo_t userInfo_set;
	strcpy(userInfo_set.appVersion, "1.0");
	strcpy(userInfo_set.phoneType, "android");
	strcpy(userInfo_set.userId, "E001");
	FRM_SetSession(tmp_sessionId_ucp, &userInfo_set);
	LOG_debug("session id: %s", tmp_sessionId_ucp);

	SessionUserInfo_t userInfo_get;
	FRM_GetSession(tmp_sessionId_ucp, &userInfo_get);

	LOG_debug("userId: %s, phoneType: %s, appVersion: %s", userInfo_get.userId, userInfo_get.phoneType, userInfo_get.appVersion);
}

/* 设置cookie值，是无法通过CGIAPI获取到的，因为此时cookie并没有存放在Header里面 */
void TEST_Cookie(void)
{
	/* 生成sessionid */
	char tmp_sessionId[17];
	SessionUserInfo_t userInfo;
	strcpy(userInfo.appVersion, "1.0");
	strcpy(userInfo.phoneType, "android");
	strcpy(userInfo.userId, "E001");
	FRM_SetSession(tmp_sessionId, &userInfo);
	LOG_debug("set session id: %s", tmp_sessionId);
	
	/* 将sessionId放到Cookie里面 */
	FRM_CgiHeaderCookieSetString(AUTH_TOKEN, tmp_sessionId, 1200, "/", stub_getIpAddress());
	
	/* 尝试获取Cookie中session信息 */
	char tmp_auth_token_cp[17];
	FRM_CgiCookieString(AUTH_TOKEN, tmp_auth_token_cp, AUTH_TOKEN_SIZE);

	LOG_debug("get the session id: %s", tmp_auth_token_cp);
}

extern char **environ;
int TEST_print_all_env(void)
{
	int index = 0;

	while (environ[index] != NULL)
	{
		LOG_debug("%s\n", environ[index++]);
	}

	return 1;
}

int TEST_strtok(void)
{
	char *tmp_requestRaw_scp = "https://jsonweb/cgi/usr/option/123";
	char *splitChar = "?";
	char tmp_data_scp[REQUSET_URL_MAX_SIZE];
	strncpy(tmp_data_scp, strtok(tmp_requestRaw_scp, splitChar), REQUSET_URL_MAX_SIZE);
	char *tmp = strtok(NULL, splitChar);
	if (tmp != 0)
	{
		strncpy(tmp_data_scp, tmp, REQUSET_URL_MAX_SIZE);
	}
	else
	{
		LOG_info("no data!");
	}
}