/**
 **    A Template for developing new terminal shared application
 **/

#include <string.h>
#include <stdio.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include "FileFun.h"
#include "dbghlp.h"
#include "Inc/ErrorCode.h"

#define d_COPY_FILE_BUFFER_SIZE     0x100
#define BACKUP_FILE_NAME            "./FILE.bak"


INT CFileFun::WriteFile(__const void *__restrict __ptr, size_t __size, size_t __n, FILE *__restrict __s)
{
    INT ret = 0, fd = 0, retry = 0;

    if (__s == NULL)
    {
        return -1;
    }
    ret = fwrite(__ptr, __size, __n, __s);
    if (ret < 0)
    {
        ret = d_FILE_ERR_WRITE_FAIL;
        return ret;
    }
    fflush(__s);

    fd = fileno(__s);
    if (fd != -1)
    {
        retry = 10;
        while (retry > 0)
        {
            ret = fsync(fd);
            if (ret == d_OK)
            {
                break;
            }
            retry--;
            usleep(50 * 1000);
        }
    }
    if (ret)
    {
        ret = d_FILE_ERR_SYNC_FAIL;
    }
    return ret;
}


INT CFileFun::WriteFileWithCheck(WriteFile_t *pWriteFile)
{
    FILE *pFile = NULL;
    //static USHORT usCNT = 0;
    INT ret = 0, fileNameLen = 0, offset = 0, retry = 0;
    BYTE bCmdBuf[512];

    if (pWriteFile == NULL)
    {
        return d_FILE_ERR_INVALID_PARAM;
    }
    do
    {
        memset(bCmdBuf, 0x00, sizeof(bCmdBuf));
        fileNameLen = strlen(pWriteFile->pFileName);
        if (fileNameLen > (sizeof(bCmdBuf) - (2 * fileNameLen)))
        {
            ret = d_FILE_ERR_FILENAME_TOO_LONG;
            break;
        }
        memcpy(bCmdBuf, "cp ", 3);
        offset = 3;
        memcpy(&bCmdBuf[offset], pWriteFile->pFileName, fileNameLen);
        offset += fileNameLen;
        bCmdBuf[offset++] = ' ';
        memcpy(&bCmdBuf[offset], BACKUP_FILE_NAME, strlen(BACKUP_FILE_NAME));
        offset += strlen(BACKUP_FILE_NAME);
        //printf("backfile cmd = %s\n", bCmdBuf);
        system((const char *)bCmdBuf);
        retry = 3;
        do
        {
            pFile = fopen(pWriteFile->pFileName, pWriteFile->pAccessMode);
            if (pFile != NULL)
            {
                break;
            }
            retry--;
            usleep(100 * 1000);
        }
        while (retry > 0);
        if (pFile == NULL)
        {
            ret = d_FILE_ERR_OPEN_FAIL;
            break;
        }
        ret = WriteFile(pWriteFile->pWData, pWriteFile->wSize, pWriteFile->wDataLen, pFile);
        if (ret /*|| (usCNT >= 40)*/)
        {
            memset(bCmdBuf, 0x00, sizeof(bCmdBuf));
            memcpy(bCmdBuf, "rm ", 3);
            offset = 3;
            memcpy(&bCmdBuf[offset], pWriteFile->pFileName, fileNameLen);
            offset += fileNameLen;
            //printf("remove file cmd = %s\n", bCmdBuf);
            system((const char *)bCmdBuf);
            memset(bCmdBuf, 0x00, sizeof(bCmdBuf));
            memcpy(bCmdBuf, "cp ", 3);
            offset = 3;
            memcpy(&bCmdBuf[offset], BACKUP_FILE_NAME, strlen(BACKUP_FILE_NAME));
            offset += strlen(BACKUP_FILE_NAME);
            bCmdBuf[offset++] = ' ';
            memcpy(&bCmdBuf[offset], pWriteFile->pFileName, fileNameLen);
            offset += fileNameLen;
            //printf("restore cmd = %s\n", bCmdBuf);
            system((const char *)bCmdBuf);
            //ret = d_FILE_ERR_SYNC_FAIL;
        }
        //system("ls -al");
        memset(bCmdBuf, 0x00, sizeof(bCmdBuf));
        memcpy(bCmdBuf, "rm ", 3);
        offset = 3;
        memcpy(&bCmdBuf[offset], BACKUP_FILE_NAME, strlen(BACKUP_FILE_NAME));
        //printf("remove file cmd = %s\n", bCmdBuf);
        system((const char *)bCmdBuf);
        usleep(100 * 1000);
        //system("ls - al");
    }
    while (0);
    if (pFile != NULL)
    {
        fclose(pFile);
        pFile = NULL;
    }
    //usCNT++;
    return ret;
}


long CFileFun::ReadFile(const char *pcFileName, char* pszData, int* pnSize, const char* pAccessMode)
{
    unsigned long ulRet;
    
    FILE *pFile = fopen(pcFileName, pAccessMode);
    if (NULL == pFile)
    {
        TraceMsg("Open reversal for read %s failed, errno:%d, errmsg:%s", pcFileName, errno, strerror(errno));
        return errno;
    }

    // Obtain file size:
    fseek(pFile, 0, SEEK_END);
    ULONG ulSize = ftell(pFile);
    rewind(pFile);
    if (ulSize > *pnSize)
    {
        TraceMsg("The size of the reversal file is larger than the size of the buffer (%ld > %d)", ulSize, *pnSize);
        fclose(pFile);
        return errno;
    }
    TraceMsg("file size: %ld\n", ulSize);

    // Read file
    size_t byteRead = fread(pszData, sizeof(char), ulSize, pFile);
    fclose(pFile);
    TraceMsg("byteRead: %d, Readed Reversal:\n%s\n", byteRead, pszData);

    if (byteRead != ulSize)
    {
        TraceMsg("Read reversal file failed, file length = %ld, read length = %d", ulSize, byteRead);
        return errno;
    }
    
    pszData[byteRead] = '\0';
    *pnSize = byteRead;
    
    return 0;
}

long CFileFun::GetFileSize(const char caFileName[])
{
    FILE *pFile = NULL;
    long int liFileSize = -1;

    TraceMsg("entry")

    do
    {
        pFile = fopen(caFileName, "r");

        if (pFile == NULL)
        {
            TraceMsg("\x1B[1;31m" "pFile == NULL" "\x1B[m");
            break;
        }

        fseek(pFile, 0, SEEK_END);
        liFileSize = ftell(pFile);
    }
    while (0);

    if (pFile)
    {
        fclose(pFile);
    }
    TraceMsg("%s-Exit: %ld", __func__, liFileSize);

    return liFileSize;
}


bool CFileFun::CopyFile(const char caSourceFileName[], const char caDestinationFileName[])
{
    bool bRet = true;
    FILE *pstSourceFile = NULL;
    FILE *pstDestinationFile = NULL;
    do
    {
        if (caSourceFileName == NULL)
        {
            bRet = false;
            break;
        }
        if (caDestinationFileName == NULL)
        {
            bRet = false;
            break;
        }
        if (IsFileExist(caSourceFileName) == false)
        {
            TraceMsg("caSourceFileName[%s] doesn't exist", caSourceFileName);
            bRet = false;
            break;
        }

        pstSourceFile = fopen(caSourceFileName, "rb");
        if (pstSourceFile == NULL)
        {
            TraceMsg("pstSourceFile == NULL");
            bRet = false;
            break;
        }

        pstDestinationFile = fopen(caDestinationFileName, "wb");
        if (pstDestinationFile == NULL)
        {
            TraceMsg("pstDestinationFile == NULL");
            bRet = false;
            break;
        }

        unsigned char ucaDataBuffer[d_COPY_FILE_BUFFER_SIZE] = {0};

        while (1)
        {
            long lBytesRead = fread(ucaDataBuffer, sizeof(unsigned char), d_COPY_FILE_BUFFER_SIZE, pstSourceFile);
            WriteFile(ucaDataBuffer, sizeof(unsigned char), lBytesRead, pstDestinationFile);
            if (feof(pstSourceFile))
            {
                break;
            }
        }
    }
    while (0);
    if (pstSourceFile != NULL)
    {
        fclose(pstSourceFile);
    }
    if (pstDestinationFile != NULL)
    {
        fclose(pstDestinationFile);
    }
    return bRet;
}


bool CFileFun::IsFileExist(const char *pcFileName)
{
    if (pcFileName == NULL)
    {
        TraceMsg("\x1B[1;31m" "Error: In %s() input pkcFileName is NULL" "\x1B[m", __func__);
        return false;
    }
    struct stat buf;
    return stat(pcFileName, &buf) == 0;
}


bool CFileFun::CreateEmptyFile(const char *pkcFileName)
{
    if (pkcFileName == NULL)
    {
        TraceMsg("\x1B[1;31m" "Error: In %s() input pkcFileName is NULL" "\x1B[m", __func__);
        return false;
    }

    return creat(pkcFileName, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) != -1;
}


bool CFileFun::DeleteFile(const char *pcFileName)
{
    if (pcFileName == NULL)
    {
        TraceMsg("\x1B[1;31m" "Error: In %s() input pkcFileName is NULL" "\x1B[m", __func__);
        return false;
    }

    if (IsFileExist(pcFileName) == false)
    {
        return true;
    }
    return unlink(pcFileName) == 0;
}

