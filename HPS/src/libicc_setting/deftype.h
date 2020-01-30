
/**
 * Copyright(c) Castles Technology Co,. Ltd
 * 
 * File Name: deftype.h
 * Author: Alan.Ren
 * 
 * Description: Basic date types.
 * Create Date: 2018.06.06
 **/

#ifndef _H_H_DEF_TYPE_20180611_H_H_
#define _H_H_DEF_TYPE_20180611_H_H_

#include <string.h>
#include <pthread.h>


#define IN
#define OUT
#define INOUT

#define CHAR	char
#define UCHAR	unsigned char
#define BYTE	unsigned char

#define SHORT	short			// 2byte
#define USHORT	unsigned short		// 2byte
#define WORD	unsigned short		// 2byte
#define PWORD   unsigned short *
#define INT	int		        // 4byte	
#define PINT	int *		        // 4byte	
#define UINT	unsigned int		// 4byte
#define DWORD	unsigned int		// 4byte
#define PDWORD  unsigned int *
#define BOOL	unsigned char		// 1byte
#define LONG	long			// 4byte
#define ULONG	unsigned long		// 4byte
#define PCSTR   const char *
#define PSTR    char *
#define PCBYTE  const unsigned char *
#define PBYTE   unsigned char *

typedef unsigned int    UINT_PTR, *PUINT_PTR;
typedef long            LONG_PTR, *PLONG_PTR;
typedef UINT_PTR        WPARAM;
typedef LONG_PTR        LPARAM;

#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

#endif

