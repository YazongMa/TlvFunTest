
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
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>


#ifdef PLATFORM_ANDROID
#include <android/log.h>
#include "FunBase.h"
#define LOG_TAG     "CASTLESPAY_ANDROID"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

inline bool g_bPaymentDebugFlag = true;

inline void SetPaymentDebugFlag(bool bPaymentDebugFlag)
{
    g_bPaymentDebugFlag = bPaymentDebugFlag;
}

inline void PrintHex(
    const char *pszFileName,
    const int nFileNo,
    const char *pszFunc,
    const char *pszStr,
    const int nSize,
    const char *pszText)
{
   if (pszStr == NULL || nSize <= 0 || g_bPaymentDebugFlag == false)
       return;

   if (nSize > 4096)
   {
       LOGE("%s-%05d %s buffer is too large", pszFileName, nFileNo, pszFunc);
       return;
   }

   char szStrTmp[4096 + 1] = { 0 };

   CFunBase::Hex2Str((BYTE *)szStrTmp, (BYTE *)pszStr, nSize);

   LOGI("%s-%05d %s %s:", pszFileName, nFileNo, pszFunc, pszText);
   LOGI("%s", szStrTmp);
}


inline void PrintAscii(
    const char *pszFileName,
    const int nFileNo,
    const char *pszFunc,
    const char* fmt, ...)
{
   if (pszFunc == NULL || g_bPaymentDebugFlag == false)
       return;

    char szBuf[4096 + 1] = { 0 };
    va_list vp;
    va_start(vp, fmt);
    vsprintf(szBuf, fmt, vp);
    va_end(vp);

    int nLeft = 0;
    int nOffSet = 0;
    char szTmp[513] = { 0 };

    do
    {
        memset(szTmp, 0, sizeof(szTmp));

        nLeft = strlen(&szBuf[nOffSet]);
        if (nLeft > 512)
        {
            memcpy(szTmp, &szBuf[nOffSet], 512);
            nOffSet += 512;
        }
        else
        {
            memcpy(szTmp, &szBuf[nOffSet], nLeft);
            nOffSet += nLeft;
        }

        LOGI("%s-%05d %s: %s", pszFileName, nFileNo, pszFunc, szTmp);
    } while(nOffSet % 512 == 0);
}
#endif


#ifndef PLATFORM_ANDROID
#define SetPaymentDebugFlag(x)
#define TraceBytes(pByte, ByteLen, fmt, ...) \
{ \
    printf("%s %d : "fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
    int i=0; \
    for (; i < ByteLen; ++i) \
    { \
        printf("%02X", pByte[i]); \
    } \
    printf("\r\n"); \
}

#define TraceMsg(fmt,...) \
{ \
	printf("%s %d : " fmt "\r\n", __FILE__, __LINE__, ##__VA_ARGS__); \
}
#else
#define TraceBytes(pByte, ByteLen, fmt, ...)                                                \
    {                                                                                       \
        PrintHex(__FILE__, __LINE__, __FUNCTION__, (char*)pByte, (int)ByteLen, (char*)fmt); \
    }

#define TraceMsg(fmt,...)                                                                   \
    {                                                                                       \
        PrintAscii(__FILE__, __LINE__, __FUNCTION__, fmt, ## __VA_ARGS__);                  \
    }
#endif

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

#endif 
