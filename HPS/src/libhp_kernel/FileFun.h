/*
 * File:   FileOper.h
 *
 * Created on 2018.6.26 8:57
 */

#ifndef __HEARTLAND_FILEFUN_H__
#define __HEARTLAND_FILEFUN_H__

#include <stdio.h>
#include <typedef.h>

#define PACKED  __attribute__ ((packed))

typedef struct {
    //fopen param.
    __const char *__restrict pFileName;
    __const char *__restrict pAccessMode;
    //fwrite param.
    __const void *__restrict pWData;
    size_t wSize;
    size_t wDataLen;
}PACKED WriteFile_t;

class CFileFun
{
public :
    static INT WriteFile(__const void *__restrict __ptr, size_t __size,  size_t __n, FILE *__restrict __s);
    static INT WriteFileWithCheck(WriteFile_t *pstWriteFile);
    
    static long ReadFile(const char *pcFileName, char* pszData, int* pnSize, const char* pAccessMode = "r");
    static long GetFileSize(const char caFileName[]);
    static bool CopyFile(const char caSourceFileName[], const char caDestinationFileName[]);
    static bool IsFileExist(const char *pcFileName);
    static bool CreateEmptyFile(const char *pkcFileName);
    static bool DeleteFile(const char *pcFileName);
};


#endif    /* __HEARTLAND_FILEOPER_H__ */
