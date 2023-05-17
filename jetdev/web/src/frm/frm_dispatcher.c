/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Zhang.WY											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/07												*/
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
#include <stdlib.h>
#include "frm_dispatcher_g.h"
#include "typedef.h"
#include "common_g.h"
#include "string.h"
#include "util_logger_g.h"
#include "ctrl_g.h"

/*--------------------------------------------------------------------------*/
/*	公開変数定義															*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*	非公開変数定義															*/
/*--------------------------------------------------------------------------*/
CtlNodes_t DISPATCHERNodes_t = {0};

/*--------------------------------------------------------------------------*/
/*	公開関数定義															*/
/*--------------------------------------------------------------------------*/

void FRM_DispAddController(schar *moduleName, void (*controllerHandler)(const RequestData_t *request_data))
{
	LOG_entry("FRM_DispAddController");

	if ((NULL == moduleName) || (NULL == controllerHandler))
	{
		MlogErrorParameterNull();
	}
	else
	{
		int i = 0;
		bool_t repeatCtrlName = FALSE;
		if (DISPATCHERNodes_t.nodeLength < CONTRLLER_MAX_CNT)
		{
			/* look over the array to make sure no controller has the same name */
			while ((i < DISPATCHERNodes_t.nodeLength) & !repeatCtrlName)
			{
				char *controllName = DISPATCHERNodes_t.controllers[i].controllerName_cp;
				/* if exists the same name controller */
				if (0 == strncmp(controllName, moduleName, CONTROLLER_NAME_SIZE))
				{
					repeatCtrlName = TRUE;
				}

				i++;
			}

			if (repeatCtrlName)
			{
				LOG_error("controller name(%s) define repeat!");
			}
			else
			{
				ControllerNode_t *node = &DISPATCHERNodes_t.controllers[DISPATCHERNodes_t.nodeLength];
				node->controllerName_cp = moduleName;
				node->controllerHandler = controllerHandler;

				DISPATCHERNodes_t.nodeLength++;
			}
		}
		else
		{
			LOG_error("count has more then MAX COUNT: %d", CONTRLLER_MAX_CNT);
		}
	}

	LOG_leave("FRM_DispAddController");
}

sint FRM_Dispatch(RequestData_t *requestdata)
{
	sint ret = RET_ERROR_CODE;
	if (NULL == requestdata)
	{
		MlogErrorParameterNull();
		LOG_leave("main_buildRequestInfo");
	}
	else
	{
		sint i = 0;
		schar tmpPahtInfo[REQUSET_URL_MAX_SIZE];
		bool_t findModule = FALSE;
		strncpy(tmpPahtInfo, requestdata->Path_info_SCP, REQUSET_URL_MAX_SIZE);
		schar *path_head = strtok(tmpPahtInfo, "/");
		LOG_debug("head info: %s", path_head);
		while (i < DISPATCHERNodes_t.nodeLength && !findModule)
		{
			ControllerNode_t *node = &DISPATCHERNodes_t.controllers[i];

			if (0 == strncmp(node->controllerName_cp, path_head, PATHINFO_HEAD_MAX_SIZE))
			{
				node->controllerHandler(requestdata);
				findModule = TRUE;
				ret = RET_SUCCESS_CODE;
			}
			else
			{
				i++;
			}
		}

		LOG_leave("main_buildRequestInfo");
	}
	return ret;
}

/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/

/*End Of File*/
