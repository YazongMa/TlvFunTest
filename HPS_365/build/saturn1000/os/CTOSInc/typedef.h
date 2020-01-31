/*============================================================================
 *
 *      typedef.h
 *      
 *==========================================================================*/
/*============================================================================
 * 
 * Author(s):    Peril Chen
 * Date:          
 * Description:	
 * 
 *==========================================================================*/
#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__
#include <sys/types.h>

#define IN
#define OUT
#define INOUT

#define BOOL		uint8_t						// 1 byte

#define CHAR		int8_t						// 1 byte
#define UCHAR		uint8_t						// 1 byte
#define BYTE		uint8_t						// 1 byte

#define SHORT		int16_t						// 2 byte
#define USHORT	uint16_t					// 2 byte
#define WORD		uint16_t					// 2 byte

#define INT			int32_t						// 4 byte	
#define UINT		uint32_t					// 4 byte
#define DWORD		uint32_t					// 4 byte
#define LONG		uint32_t					// 4 byte
#define ULONG		uint32_t					// 4 byte

#ifndef STR
	#define STR		uint8_t
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef NULL
#define NULL	0
#endif

#define d_OK 0
#define d_NO 1
#define d_TRUE 1
#define d_FALSE 0
#define d_ON 1
#define d_OFF 0

#endif


