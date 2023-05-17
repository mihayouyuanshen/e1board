/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

	Ubiquitous ECHONET Lite Library
	common include file

	$Revision: 1.13 $	$Date: 2021/03/12 06:13:14 $

	Copyright (C) Ubiquitous AI Corporation 2001-2021. All rights reserved.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#ifndef __ENL_H__
#define __ENL_H__

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	structures
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/* message */
typedef struct _enl_message {
	unsigned char flags;	/* control information */
	unsigned char *data;	/* message data */
	int length;				/* message length */
} ENL_MESSAGE;

/* for flags of ENL_MESSAGE */
#define ENL_MESSAGE_BROADCAST		(1U << 0)		/* handled as general broadcast */
#define ENL_MESSAGE_REQUEST			(1U << 1)		/* handled as request */

/* property define */
typedef struct _enl_property_define {
	unsigned char epc;		/* property code */
	unsigned char type;		/* data type */
	unsigned char size;		/* data size */
	unsigned long min;		/* minimum value */
	unsigned long max;		/* maximum value */
	unsigned char rule;		/* access rule */
} ENL_PROPERTY_DEFINE;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	API functions
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/* initialize / terminate */
extern int ENL_initialize(void);
extern int ENL_terminate(void);

/* setting */
#if defined(RL78)
extern int ENL_set_message_length(long length);
#else
extern int ENL_set_message_length(int length);
#endif
extern int ENL_set_transaction_id(unsigned short tid);

/* instance */
extern int ENL_create_node_profile(void **instance, unsigned char *list, const ENL_PROPERTY_DEFINE *define, unsigned char mode);
extern int ENL_dispose_instance(void *instance);

/* property */
extern int ENL_get_operation_mode(void *instance, unsigned char epc, unsigned short *mode);
extern int ENL_set_operation_mode(void *instance, unsigned char epc, unsigned short mode);
extern int ENL_get_property(void *instance, unsigned char epc, unsigned char *pdc, unsigned char *edt);
extern int ENL_get_property_value(void *instance, unsigned char epc, unsigned long *value, unsigned char index);

/* message */
extern int ENL_get_next_message(ENL_MESSAGE *message, ENL_MESSAGE **next);
extern int ENL_dispose_message(ENL_MESSAGE *message);

/* in callback */
extern int ENL_get_callback_value(void *instance, unsigned long *value, unsigned char index);

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	constants for API
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/* result of API */
#define ENL_RESULT_SUCCESS					0				/* normally success */

#define ENL_RESULT_TO_CONTROLLER			1				/* message processed at device instance should be reprocessed at controller instance */

#define ENL_RESULT_NOT_INITIALIZED			(-1)			/* ENL_initialize is not called yet (for other API) */
#define ENL_RESULT_ALREADY_INITIALIZED		(-2)			/* ENL_initialize is called yet (for ENL_initialize) */

#define ENL_RESULT_INVALID_PARAMETER		(-3)			/* invalid parameter value was given */
#define ENL_RESULT_ALLOCATION_FAILED		(-4)			/* NULL received from memory allocation function */
#define ENL_RESULT_SEMAPHORE_ERROR			(-5)			/* failed to take semaphore */

#define ENL_RESULT_INVALID_INSTANCE_MODE	(-6)			/* instance modes is not matched */

#define ENL_RESULT_REGISTERED_INSTANCE		(-7)			/* an instance which has same EOJ is already registered */
#define ENL_RESULT_UNREGISTERD_INSTANCE		(-8)			/* specified instance is not registerd */

#define ENL_RESULT_INVALID_MESSAGE			(-9)			/* given message is invalid */

#define ENL_RESULT_UNSUPPORTED_PROPERTY		(-10)			/* specified EPC is not supported */
#define ENL_RESULT_UNSUPPORTED_TYPE			(-11)			/* type of specified property is not integer */

#define ENL_RESULT_INVALID_SIZE				(-12)			/* length or index over definition */
#define ENL_RESULT_EVALUATION_FAILED		(-13)			/* fail at data evaluation */

#define ENL_RESULT_TENTATIVE_NOT_READY		(-14)			/* tentative value for the property specified to use for write request is not ready */
#define ENL_RESULT_OVER_MAX_LENGTH			(-15)			/* length for request message tried to create is over maximum message length */

#define ENL_RESULT_NOT_IN_CALLBACK			(-16)			/* called in the state not in callback that the API is valid */

#define ENL_RESULT_VALUE_NOT_RECEIVED		(-17)			/* value of specified property is not received yet */

#if defined(EN_FRAME_INFO_CHECK) && EN_FRAME_INFO_CHECK
#define ENL_RESULT_MESSAGE_TOO_LONG 				(-21)
#define ENL_RESULT_FRAME_LENGTH_NOT_CORRECT			(-22)
#define ENL_RESULT_EHD1_NOT_CORRECT 				(-23)
#define ENL_RESULT_EHD2_NOT_CORRECT 				(-24)
#define ENL_RESULT_OPC_NOT_CORRECT 					(-25)
#define ENL_RESULT_DEOJ_NOT_FOUND 					(-26)
#define ENL_RESULT_DEOJ_NODE_INSTANCE_NOT_CORRECT	(-27)
#define ENL_RESULT_ESV_NOT_CORRECT 					(-31)
#define ENL_RESULT_EDT_NOT_CORRECT 					(-33)

#endif //EN_FRAME_INFO_CHECK


/* 'mode' of ENL_create_node_profile / ENL_create_device_### */
#define ENL_INSTANCE_MODE_DEVICE			0				/* use as a device on own node [device mode] */
#define ENL_INSTANCE_MODE_CONTROLLER		(1U << 0)		/* use to control a device on other node [controller mode] */
#define ENL_INSTANCE_MODE_CTRL_NOT_STORE	(1U << 1)		/* not store current value (with controller mode only) */
#define ENL_INSTANCE_MODE_CTRL_ONLY_DIRECT	(1U << 2)		/* not use tentative value (with controller mode only) */
#define ENL_INSTANCE_MODE_CTRL_NOT_REGIST	(1U << 3)		/* for a 'device object' not related to 'node profile object' (with controller mode only) */

/* 'mode' of ENL_set_operation_mode */
#define ENL_ENABLE_ACCESS_RULE_SET			(1U << 0)		/* support access rule Set */
#define ENL_ENABLE_ACCESS_RULE_GET			(1U << 1)		/* support access rule Get */
#define ENL_ENABLE_ACCESS_RULE_ANNO			(1U << 2)		/* support access rule Anno */
#define ENL_ENABLE_STATUS_CHANGE_ANNO		(1U << 3)		/* enable announcement at state change */
#define ENL_ENABLE_PROPERTY_MAP_SET			(1U << 4)		/* validate in Set property map */
#define ENL_ENABLE_PROPERTY_MAP_GET			(1U << 5)		/* validata in Get property map */
#define ENL_ENABLE_PROPERTY_MAP_SC_ANNO		(1U << 7)		/* validate in state change announcement property map */
#define ENL_ENABLE_CALLBACK_WRITE			(1U << 8)		/* enable callback at write request */
#define ENL_ENABLE_CALLBACK_READ			(1U << 9)		/* enable callback at read/notification request */
#define ENL_ENABLE_CALLBACK_UPDATE			(1U << 10)		/* enable callback at update (for controller) */
#define ENL_ENABLE_REMOTE_CONTROL_SETTING	(1U << 11)		/* enable remote control setting */

/* 'reply' of ENL_create_### / ENL_create_###_single */
#define ENL_REPLY_UNNECESSARY				0				/* the message does not need a reply */
#define ENL_REPLY_NECESSARY					1				/* the message needs a reply */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	constants for callback
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#define ENL_RESULT_ACCEPT					0				/* accept request for the property */
#define ENL_RESULT_COLLECT_AND_ACCEPT		1				/* accept request for the property with collection */
#define ENL_RESULT_NO_UPDATE_AND_ACCEPT		2				/* accept request for the property , no update propery value */
#define ENL_RESULT_NOT_ACCEPT				(-1)			/* not accept request for the property */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	property types
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/* integer */
#define ENL_TYPE_CHAR						0x01
#define ENL_TYPE_SHORT						0x02
#define ENL_TYPE_LONG						0x04

#define ENL_TYPE_UNSIGNED					(1U << 7)

#define ENL_TYPE_SIGNED_CHAR				ENL_TYPE_CHAR
#define ENL_TYPE_SIGNED_SHORT				ENL_TYPE_SHORT
#define ENL_TYPE_SIGNED_LONG				ENL_TYPE_LONG
#define ENL_TYPE_UNSIGNED_CHAR				(ENL_TYPE_UNSIGNED | ENL_TYPE_CHAR)
#define ENL_TYPE_UNSIGNED_SHORT				(ENL_TYPE_UNSIGNED | ENL_TYPE_SHORT)
#define ENL_TYPE_UNSIGNED_LONG				(ENL_TYPE_UNSIGNED | ENL_TYPE_LONG)

/* other */
#define ENL_TYPE_OTHER						0xff

#if defined(USE_SIMPLE_VERSION) && USE_SIMPLE_VERSION
extern int ENL_CTRL_create_read_request_by_eoj(ENL_MESSAGE **message, unsigned char seoj[3], unsigned char deoj[3], unsigned char *list);
extern int ENL_CTRL_create_write_request_direct_by_eoj(ENL_MESSAGE **message, unsigned char seoj[3], unsigned char deoj[3], unsigned char *epc, unsigned char *pdc, unsigned char *edt, int reply);
extern int ENL_CTRL_create_notification_request_single_by_eoj(ENL_MESSAGE **message, unsigned char seoj[3], unsigned char deoj[3], unsigned char epc);
extern int ENL_dispose_message_by_eoj(ENL_MESSAGE *message);
#endif //USE_SIMPLE_VERSION

#if defined(NO_GLOBAL_VARIABLE_INIT)
extern void ENL_CMN_initialize_variables();
#endif

#endif
