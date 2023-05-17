/****************************************************************************************************/
/*	Description	:データ型再定義ヘッダファイル														*/
/*--------------------------------------------------------------------------------------------------*/
/*	Author		:T.Norimoto																			*/
/*--------------------------------------------------------------------------------------------------*/
/*	Date		:2022/07/06																			*/
/*--------------------------------------------------------------------------------------------------*/
/*	Module Name	:-																					*/
/*--------------------------------------------------------------------------------------------------*/
/*	Note		:新規作成																			*/
/*--------------------------------------------------------------------------------------------------*/
/*	Copyright	:(c) 2022 NICHICON CORPORATION 														*/
/****************************************************************************************************/
#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

typedef unsigned char			uchar;
typedef unsigned short int		ushort;
typedef unsigned long int		ulong;
typedef unsigned long long		u64;
typedef unsigned int			uint;

typedef signed char				schar;
typedef signed short int		sshort;
typedef signed long int			slong;
typedef signed long long		s64;
typedef signed int				sint;

typedef unsigned char			bool_t;
typedef signed long int			retcode_t;

#define FALSE (0)							
#define TRUE (1)							

#endif

/*__TYPEDEF_H__*/

