
/* 
 * Copyright (c) Castles Technology Co., Ltd. 
 * 
 * File:   paramwrite.cpp
 * Author: Amber
 *
 * Created: 2018/07/13
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "paramwrite.h"


#define TAG_FOUND       0
#define TAG_NOT_FOUND   1
#define DATA_NULL   0x00000011

#define TAG_LEN     2
#define TAG_DATA_LEN   600
#define TLV_DATA_LEN   1000

//#define RID_LEN         5

ParamWrite::ParamWrite()
{
}

ParamWrite::~ParamWrite()
{
}

/**
 * Str2Hex
 *
 * Function:
 *           change Hex data to string for debug.
 *
 * @param:
 *           [IN]  pbyData  ASCII string.
 *           [IN]  nSize
 *           [OUT] cOut  Hex data.           
 *
 * @return:
 *           TRUE - If successful
 *           FALSE - Otherwise
 **/
BOOL ParamWrite::Hex2Str(BYTE* pbyStrData, BYTE* pbyData, int nSize)
{
    char szBuf[8 + 1] = {0};
    
    if (NULL == pbyData || nSize < 0)
    {
        printf("File:%s, Line:%d, Func:%s ERROR:Bad Parameter \n", __FILE__, __LINE__, __func__);
        return FALSE;
    }
    
    for(int i=0; i<nSize; i++)
    {
        memset(szBuf, 0, sizeof(szBuf));
        sprintf(szBuf, "%02X", pbyData[i]);
        memcpy(pbyStrData + i*2, szBuf, 2);
    }
    
    return TRUE;
}

/**
 * Str2Hex
 *
 * Function:
 *           change string to Hex.
 *
 * @param:
 *           [IN]   pStr  ASCII string.
 *           [OUT]  cOut  Hex data.           
 *
 * @return:
 *           TRUE - If successful
 *           FALSE - Otherwise
 **/
BOOL ParamWrite::Str2Hex(const char* pStr, BYTE* cOut)
{
    int nLen = 0;
    char szTemp[4] = {0};
    
    if (NULL == pStr)
    {
        printf("File:%s, Line:%d, Func:%s ERROR:NULL String \n", __FILE__, __LINE__, __func__);
        return FALSE;
    }
     
    nLen = strlen(pStr);
    for(int i=0; i<nLen/2; i++)
    {
        szTemp[0] = pStr[i*2];
        szTemp[1] = pStr[i*2+1];
        cOut[i] = (BYTE)strtol(szTemp, NULL, 16);
    }
   
    return TRUE;
}

