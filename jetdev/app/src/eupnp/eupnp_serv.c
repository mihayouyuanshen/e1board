/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC yufeng.yang										*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/28												*/
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
#include <frm_g.h>
#include <upnp/upnp.h>
#include <upnp/upnpdebug.h>
#include "dlog_g.h"
#include "eupnp_serv.h"

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
static UpnpDevice_Handle s_DeviceHandle = -1;
static sint s_DefaultAdvrExpire = 100;

static sint eupnp_HandleSubscriptionRequest(const UpnpSubscriptionRequest *sr_event);
static sint eupnp_CallbackEventHandler(Upnp_EventType EventType,
									   const void *Event, void *Cookie);
/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:void													*/
/*--------------------------------------------------------------------------*/
/*	return			:slong													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC yufeng.yang : 新規作成				*/
/*==========================================================================*/
FrmPthread_t eupnp_g_tid;
slong eupnp_ServStart(void)
{
	slong ret = RET_SUCCESS_CODE;
	sint result;

	DEBUG_TRCIN();

	slong ret_thread = FRM_PthreadCreate(&eupnp_g_tid, NULL, eupnp_DeviceStart, NULL);
	if (ret_thread != 0)
	{
		ret = RET_ERROR_CODE;
	}

	DEBUG_TRCOUT();

	return ret;
}
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:void													*/
/*--------------------------------------------------------------------------*/
/*	return			:void													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC yufeng.yang : 新規作成				*/
/*==========================================================================*/
void eupnp_ServStop(void)
{
	DEBUG_TRCIN();
	FRM_PthreadCancel(&eupnp_g_tid);
	DEBUG_TRCOUT();
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:const UpnpSubscriptionRequest *sr_event				*/
/*--------------------------------------------------------------------------*/
/*	return			:sint													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC yufeng.yang : 新規作成				*/
/*==========================================================================*/
static sint eupnp_HandleSubscriptionRequest(const UpnpSubscriptionRequest *sr_event)
{
	/* TODO */
	DEBUG_TRCIN();
	DEBUG_TRCOUT();
	return 1;
}
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:Upnp_EventType EventType,
					 const void *Event,
					 void *Cookie											*/
/*--------------------------------------------------------------------------*/
/*	return			:sint													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC yufeng.yang : 新規作成				*/
/*==========================================================================*/
static sint eupnp_CallbackEventHandler(Upnp_EventType EventType, const void *Event, void *Cookie)
{
	(void)Cookie;
	DEBUG_TRCIN();
	DLOG_Info("callback, event = %d\n", EventType);
	switch (EventType)
	{
	case UPNP_EVENT_SUBSCRIPTION_REQUEST:
		eupnp_HandleSubscriptionRequest((UpnpSubscriptionRequest *)Event);
		break;
	case UPNP_CONTROL_GET_VAR_REQUEST:
		break;
	case UPNP_CONTROL_ACTION_REQUEST:
		break;
		/* ignore these cases, since this is not a control point */
	case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE:
	case UPNP_DISCOVERY_SEARCH_RESULT:
	case UPNP_DISCOVERY_SEARCH_TIMEOUT:
	case UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE:
	case UPNP_CONTROL_ACTION_COMPLETE:
	case UPNP_CONTROL_GET_VAR_COMPLETE:
	case UPNP_EVENT_RECEIVED:
	case UPNP_EVENT_RENEWAL_COMPLETE:
	case UPNP_EVENT_SUBSCRIBE_COMPLETE:
	case UPNP_EVENT_UNSUBSCRIBE_COMPLETE:
		break;
	default:
		DLOG_Error("Error in eupnp_CallbackEventHandler: unknown event type %d", EventType);
	}
	DEBUG_TRCOUT();

	return 0;
}
/*==========================================================================*/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:void													*/
/*--------------------------------------------------------------------------*/
/*	return			:sint													*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/28 : DHC yufeng.yang : 新規作成				*/
/*==========================================================================*/
// static sint eupnp_DeviceStart(void)
// {
// 	sint ret = UPNP_E_SUCCESS;
// 	const char *desc_doc_pathname;

// 	/*
// 	UpnpSetLogFileNames(NULL, NULL);
// 	UpnpSetLogLevel(UPNP_ALL);
// 	UpnpInitLog();
// 	*/

// 	DEBUG_TRCIN();
// 	ret = UpnpInit2(NULL, 0);
// 	if (ret != UPNP_E_SUCCESS) {
// 		DLOG_Error("Error with UpnpInit2 -- %d", ret);
// 		UpnpFinish();
// 		return ret;
// 	}

// 	desc_doc_pathname = "./web/tvdevicedesc.xml";

// 	ret = UpnpRegisterRootDevice2(UPNPREG_FILENAME_DESC,
// 		desc_doc_pathname,
// 		0,
// 		1,
// 		eupnp_CallbackEventHandler,
// 		&s_DeviceHandle,
// 		&s_DeviceHandle);
// 	if (ret != UPNP_E_SUCCESS) {
// 		DLOG_Error("Error registering the rootdevice : %d", ret);
// 		UpnpFinish();
// 		return ret;
// 	} else {
// 		ret = UpnpSendAdvertisement(s_DeviceHandle, s_DefaultAdvrExpire);
// 		if (ret != UPNP_E_SUCCESS) {
// 			DLOG_Error("Error sending advertisements : %d", ret);
// 			UpnpFinish();
// 			return ret;
// 		}
// 	}
// 	DEBUG_TRCOUT();

// 	return UPNP_E_SUCCESS;
// }

/*End Of File*/
