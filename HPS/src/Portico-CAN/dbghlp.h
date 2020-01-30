
/*
 * Date: 2017.3.10
 *
 * Author: Allan Ren
 *
 * Des: For Debug help functions
 *
 */
 
#ifndef _H_DEBUG_HELP_4A75E685_586D_4b91_92A7_63DCE4A906BF_H_
#define _H_DEBUG_HELP_4A75E685_586D_4b91_92A7_63DCE4A906BF_H_

#include <ctosapi.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>


/*
 * GetTickCount
 * 
 * Represents monotonic time, in seconds
 */
static unsigned long 
GetTickCount( void )
{
    struct timespec ts;
    
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}


static int
pwcThreadGetTaskId( void )
{
    return pthread_self();
}


#ifndef _DISABLE_THIS_FILE_DBGHLP_

#define TraceBytes(pByte, ByteLen, fmt, ...) { const int nLineNumber = __LINE__; \
        const char *pszSrcFile = __FILE__;\
        printf("%s %d : "fmt, pszSrcFile, \
        nLineNumber, ##__VA_ARGS__);\
        int i=0;\
        for (; i < ByteLen; ++i)\
        { printf("%02X", pByte[i]);}\
        printf("\r\n");\
      }


#define TraceMsg(fmt,...) { const int nLineNumber = __LINE__;\
	const char *pszSrcFile = __FILE__;\
	printf("%s %d : "fmt"\r\n", pszSrcFile, \
	nLineNumber, ##__VA_ARGS__); }

#define Continue(fmt,...) { const int nLineNumber = __LINE__;\
	const char *pszSrcFile = __FILE__;\
	BYTE key;\
	printf("%s %d : "fmt"\r\n", pszSrcFile, nLineNumber, ##__VA_ARGS__);\
	CTOS_KBDGet(&key);\
	if (key == d_KBD_00)\
	{ exit(0); }\
}

#else

#define TraceMsg(fmt, ...)

#define Trace(fmt, ...)

#define TraceExit(fmt,ExitCode,...) { return ExitCode; }

#define Continue(fmt,...)

#endif

#endif 
