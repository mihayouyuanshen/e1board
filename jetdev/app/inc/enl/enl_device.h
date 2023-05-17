/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

	Ubiquitous ECHONET Lite Library
	include file for device usage

	$Revision: 1.3 $	$Date: 2016/02/10 04:18:38 $

	Copyright (C) Ubiquitous Corp. 2001-2012. All rights reserved.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	API functions
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/* instance */
extern int ENL_DEV_register_instance(ENL_MESSAGE **message, void *node, void *device);
extern int ENL_DEV_unregister_instance(ENL_MESSAGE **message, void *node, void *device);

/* property */
extern int ENL_DEV_set_property(ENL_MESSAGE **message, void *instance, unsigned char epc, unsigned char pdc, unsigned char *edt);
extern int ENL_DEV_set_property_value(ENL_MESSAGE **message, void *instance, unsigned char epc, unsigned long value, unsigned char index);

/* notification */
extern int ENL_DEV_create_notification(ENL_MESSAGE **message, void *instance, unsigned char eoj[3], unsigned char *list, int reply);
extern int ENL_DEV_create_notification_single(ENL_MESSAGE **message, void *instance, unsigned char eoj[3], unsigned char epc, int reply);

/* message */
extern int ENL_DEV_process_message(ENL_MESSAGE **message, void *node, unsigned char *data, int length);

/* in callback */
extern int ENL_DEV_modify_callback_value(void *instance, unsigned long value, unsigned char index);

#if defined(NO_GLOBAL_VARIABLE_INIT)
extern void ENL_DEV_initialize_variables();
#endif
