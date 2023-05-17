#include <string.h>
#
#include <sys/time.h>

#include "common_g.h"
#include "stub_g.h"
#include "config.h"
#include "test_g.h"
#include "util_logger_g.h"

static u64 get_timestamp(void);

schar stub_GetOutputFlag(char *outputFlag)
{
	LOG_entry("sub_GetOutputFlag");
	sint tmp_ret_SC = RET_SUCCESS_CODE;

	strncpy(outputFlag, "1", 1);
	LOG_leave("sub_GetOutputFlag");
	return tmp_ret_SC;
}

schar stub_Writelog(char *endpointInfo, char *pictureId, char *opInfo)
{
	LOG_debug("timestamp: %lld, endpointInfo: %s, picId: %s, opInfo: %s", get_timestamp(), endpointInfo, pictureId, opInfo);
}

schar *stub_getIpAddress()
{
	return "192.168.88.128";
}

static u64 get_timestamp(void)
{
	long long tmp;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	tmp = tv.tv_sec;
	tmp = tmp * 1000;
	tmp = tmp + (tv.tv_usec / 1000);

	return tmp;
}
typedef struct
{
	RequestName requestName;
	char *URL;
	char *Method;
	char *Data;
} NameRequestMapping;

const NameRequestMapping nameRequestMappings[17] = {
	{OUTPUTCTRL, "/api/servicesMenu/outputCtrl", "GET", ""},
	{SPLASH, "/api/start/splash?noticeToken=XXX&&custAplId=&&serviceAplId=yyy&&hostModel=abc&&appVer=1001", "GET", ""},
	{SPLASHINFO, "/api/start/splashInfo", "GET", ""},
	{STORAGESTATUS, "/api/polling/storageStatus", "GET", ""},
	{HOME, "/api/home?timeStamp=1200", "GET", ""},
	{MODESET, "/api/modeSet", "GET", ""},
	{HISTORY_SEARCH, "/api/history_search", "GET", ""},
	{TRADEHIST, "/api/tradeHist?dd=21&&mm=04&&yyyy=2023", "GET", ""},
	{RECORDS, "/api/records?dd=21&&mm=04&&yyyy=2023", "GET", ""},
	{VEHICLERECORDS, "/api/vehicleRecords?dd=21&&mm=04&&yyyy=2023", "GET", ""},
	{POWERFLOW, "/api/home/powerFlow", "GET", ""},
	{NOTICE, "/api/notice?maxNum=100", "GET", ""},
	{CONFIG, "/api/config", "GET", ""},
	{TIMESET, "/api/timeSet", "POST", "{\"data\":{\"cTime\":\"2023\"}}"},
	{ERRORDISP, "/api/errorDisp/errClear", "POST", ""},
	{EXTCTPOSCONFIRMINS, "/api/extCTPosConfirmIns", "POST", "{\"data\":{\"againstCtConnSet\":\"01\"}}"},
	{SWITCHMODULECTUSE, "/api/switchModuleCTUse", "POST", "{\"data\":{\"againstCtConnSet\":\"02\"}}"}
};

void stub_getRequestURL(RequestName requestName, schar *requestRaw, RequestData_t *request_data)
{
	int arrayLength = sizeof(nameRequestMappings) / sizeof(NameRequestMapping);
	int i;
	for (int i = 0; i < arrayLength; i++)
	{
		if (nameRequestMappings[i].requestName == requestName)
		{
			strncpy(requestRaw, nameRequestMappings[i].URL, REQUSET_URL_MAX_SIZE);
			strncpy(request_data->Method_SCP, nameRequestMappings[i].Method, REQUSET_URL_MAX_SIZE);
			strncpy(request_data->Data_SCP, nameRequestMappings[i].Data, REQUSET_URL_MAX_SIZE);
			break;
		}
	}
}