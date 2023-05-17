/****************************************************************************/
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC Sun.YF												*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/03/08												*/
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ctrl_g.h"
#include "srv_user_g.h"
#include "util_logger_g.h"
#include "ref_cjson_g.h"

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
/*	Description		:														*/
/*--------------------------------------------------------------------------*/
/*	param			:														*/
/*--------------------------------------------------------------------------*/
/*	return			:														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/03/08 : DHC Sun.YF : 新規作成						*/
/*==========================================================================*/
schar SRV_USER_GetSingle(const UserDataInfo_t *tmp_dataRequest_P, UserDataInfo_t *tmp_dataResponse_P)
{
	schar tmp_ret_SC = 0;
	
	if((NULL == tmp_dataRequest_P) || (NULL == tmp_dataResponse_P))
	{
		LOG_debug("Error(SRV_USER_GetSingle): Param is NULL.\n" );
		return -1;
	}
	else
	{
		LOG_debug("HELLO WORLD!\n");
		LOG_debug("SRV_USER_GetSingle by:%s/%d.\n", tmp_dataRequest_P->Name_SC, tmp_dataRequest_P->Age_UI);

		memset(tmp_dataResponse_P, 0, sizeof(UserDataInfo_t));
		strncpy(tmp_dataResponse_P->Name_SC, tmp_dataRequest_P->Name_SC, CTRL_USER_NAME_LEN);
		tmp_dataResponse_P->Age_UI = tmp_dataRequest_P->Age_UI;
		strncpy(tmp_dataResponse_P->Role_SC, "admin", CTRL_USER_ROLE_LEN);
		tmp_dataResponse_P->Grade_UI = 1;
	}
	return tmp_ret_SC;
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
/*	history			:2023/03/08 : DHC Sun.YF : 新規作成						*/
/*==========================================================================*/
schar SRV_USER_GetList(const UserDataListQuery_t *tmp_listQuery_P,
								UserDataInfoList_t *tmp_dataResponse_P)
{
	schar tmp_ret_SC = 0;
	schar tmp_loop_SC = 0;
	schar* tmp_userDataName[CTRL_USER_RESPONSE_LIST_MAX_NUM] = {
				"aaa","bbb","ccc","ddd","eee",
				"fff","ggg","hhh","iii","jjj",
			};
	
	if((NULL == tmp_listQuery_P) || (NULL == tmp_dataResponse_P))
	{
		LOG_debug("Error(SRV_USER_GetList): Param is NULL.\n" );
		return -1;
	}
	else
	{
		LOG_debug("HELLO WORLD!\n");
		LOG_debug("SRV_USER_GetList by:%d/%d.\n",
					tmp_listQuery_P->AgeMin_UI, tmp_listQuery_P->AgeMax_UI);

		memset(tmp_dataResponse_P, 0, sizeof(UserDataInfoList_t));
		tmp_dataResponse_P->ListCurrentNum = 8;
		for(tmp_loop_SC = 0; tmp_loop_SC < 8; tmp_loop_SC++)
		{			
			strncpy(tmp_dataResponse_P->DataList[tmp_loop_SC].Name_SC,
					tmp_userDataName[tmp_loop_SC], CTRL_USER_NAME_LEN);
			tmp_dataResponse_P->DataList[tmp_loop_SC].Age_UI = tmp_loop_SC + 28;
			strncpy(tmp_dataResponse_P->DataList[tmp_loop_SC].Role_SC,
					"admin", CTRL_USER_ROLE_LEN);
			tmp_dataResponse_P->DataList[tmp_loop_SC].Grade_UI = tmp_loop_SC + 1;
		}
	}
	return tmp_ret_SC;
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
/*	history			:2023/03/08 : DHC Sun.YF : 新規作成						*/
/*==========================================================================*/
schar SRV_USER_ModifySingle(const UserDataInfo_t *tmp_dataRequest_P,
										UserDataInfo_t *tmp_dataResponse_P)
{
	schar tmp_ret_SC = 0;
	
	if((NULL == tmp_dataRequest_P) || (NULL == tmp_dataResponse_P))
	{
		LOG_debug("Error(SRV_USER_ModifySingle): Param is NULL.\n" );
		return -1;
	}
	else
	{
		LOG_debug("HELLO WORLD!\n");
		LOG_debug("SRV_USER_ModifySingle by:%s/%d.\n",
					tmp_dataRequest_P->Name_SC, tmp_dataRequest_P->Age_UI);

		memset(tmp_dataResponse_P, 0, sizeof(UserDataInfo_t));
		strncpy(tmp_dataResponse_P->Name_SC, tmp_dataRequest_P->Name_SC, CTRL_USER_NAME_LEN);
		tmp_dataResponse_P->Age_UI = tmp_dataRequest_P->Age_UI;
		strncpy(tmp_dataResponse_P->Role_SC, "admin", CTRL_USER_ROLE_LEN);
		tmp_dataResponse_P->Grade_UI = 1;
	}
	return tmp_ret_SC;
}


/*--------------------------------------------------------------------------*/
/*	非公開関数定義															*/
/*--------------------------------------------------------------------------*/


/*End Of File*/
