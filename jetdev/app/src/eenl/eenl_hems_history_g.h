/****************************************************************************/
/*	Description		:ECHONET Lite application for hems history funciton 	*/
/*--------------------------------------------------------------------------*/
/*	Author			:DHC jiayu.li											*/
/*--------------------------------------------------------------------------*/
/*	Date			:2023/04/17												*/
/*--------------------------------------------------------------------------*/
/*	Module Name		:ECHONET Lite通信の機能名は、EENLとする					*/
/*--------------------------------------------------------------------------*/
/*	Note			:無し													*/
/*--------------------------------------------------------------------------*/
/*	Copyright		:(c) 2023 NICHICON CORPORATION							*/
/****************************************************************************/
#ifndef	__EENL_HEMS_HISTORY_G_H__
#define	__EENL_HEMS_HISTORY_G_H__

extern slong EENL_GetHemsHistory(EenlGetHemsHistory_t* fmsg);

extern slong EENL_ReadHemsHistory(EenlHemsHistory_t* hemsHist);
extern slong EENL_WriteHemsHistory(EenlHemsHistory_t* hemsHist);
extern slong EENL_CheckHemsHistValue(uchar value);
extern void EENL_ExtUpdateHemsHistory(uchar value);
extern slong EENL_UpdateHemsHistory(uchar value, uchar check);

#endif	/* __EENL_HEMS_HISTORY_G_H__ */
