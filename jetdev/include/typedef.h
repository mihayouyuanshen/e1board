/******************************************************************************/
/*    Description    :データ型再定義ヘッダファイル                              */
/*----------------------------------------------------------------------------*/
/*    Author         :DHC yufeng.yang                                         */
/*----------------------------------------------------------------------------*/
/*    Date           :2023/03/08                                              */
/*----------------------------------------------------------------------------*/
/*    Module Name    :                                                        */
/*----------------------------------------------------------------------------*/
/*    Note           :                                                        */
/*----------------------------------------------------------------------------*/
/*    Copyright      :(c) 2020 NICHICON CORPORATION                           */
/******************************************************************************/
#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

typedef unsigned char			uchar;
typedef char					schar;
typedef unsigned short int		ushort;
typedef signed short int		sshort;
typedef unsigned int			uint;
typedef signed int				sint;
typedef unsigned long int		ulong;
typedef signed long int			slong;
typedef unsigned long long		u64;
typedef signed long long		s64;


typedef unsigned char			bool_t;
typedef signed int				retcode_t;

#define FALSE	(0)
#define TRUE	(1)

#ifndef NULL
#define NULL	((void *)0)
#endif

#endif /*__TYPEDEF_H__*/
