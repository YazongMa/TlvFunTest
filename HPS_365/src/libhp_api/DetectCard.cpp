
/*
 * File Name: DetectCard.cpp
 * Author: Alan.Ren
 * 
 * Create Date: 2019.2.28
 */

#include <ctosapi.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <ctosapi.h>

#include "DetectCard.h"

#ifndef PLATFORM_ANDROID
#include <ctos_clapi.h>
#else
extern USHORT CTOS_CLTypeBActive(BYTE* baPUPI);
extern USHORT CTOS_HALTB(BYTE *baPUPI);
#endif



void CDetectCard::Detect(OUT int *pCardType)
{
    if (NULL != pCardType)
    {
        if (DetectMSR())
        {
            (*pCardType) = kMSR;
        }
        else if (DetectEMV())
        {
            (*pCardType) = kEmv;
        }
        else if (DetectEMVCL())
        {
            (*pCardType) = kEmvCL;
        }
        else
        {
            (*pCardType) = kNoneCard;
        }
    }
    else
    {
        TraceMsg("Error: pCardType pointer is NULL.");
    }
}


void CDetectCard::Detect(OUT int *pCardType, IN int nCardSet)
{
    if (NULL == pCardType)
    {
        TraceMsg("Error: pCardType pointer is NULL.");
        return ;
    }

    if (nCardSet & kMSR)
    {
        if (DetectMSR())
        {
            (*pCardType) = kMSR;
            return ;
        }
    }

    if (nCardSet & kEmv)
    {
        if (DetectEMV())
        {
            (*pCardType) = kEmv;
            return ;
        }
    }

    if (nCardSet & kEmvCL)
    {
        if (DetectEMVCL())
        {
            (*pCardType) = kEmvCL;
            return ;
        }
    }
    
    (*pCardType) = kNoneCard;
    return ;
}


void CDetectCard::ClearLastDetectInfo(void)
{
    ClearLastMSRInfo();
    ClearLastEMVCLInfo();
    ClearLastEMVInfo();
}

void CDetectCard::ClearLastMSRInfo(void)
{
    DWORD dwRet;
    BYTE byTmpBuf1[128];
    BYTE byTmpBuf2[128];
    BYTE byTmpBuf3[128];
    WORD wLen1, wLen2, wLen3;

    CTOS_SystemWait(10, d_EVENT_SC, &dwRet);
    wLen1 = wLen2 = wLen3 = sizeof(byTmpBuf1);
    CTOS_MSRRead(byTmpBuf1, &wLen1,
                 byTmpBuf2, &wLen2,
                 byTmpBuf3, &wLen3);
}

void CDetectCard::ClearLastEMVCLInfo(void)
{
    CTOS_CLPowerOff();
}

void CDetectCard::ClearLastEMVInfo(void)
{
    CTOS_SCPowerOff(d_SC_USER);
    usleep(100 * 1000);
}

BOOL CDetectCard::DetectMSR(void)
{
    DWORD dwEvt = 0x00;
    BOOL bResult = FALSE;
    DWORD dwWaitEvt = d_EVENT_MSR;
    USHORT usErr = 0x00;

    usErr = CTOS_SystemWait(1, dwWaitEvt, &dwEvt);
    if ((d_OK == usErr) &&
        (d_EVENT_MSR == dwEvt))
    {
        bResult = TRUE;
    }
    return bResult;
}

BOOL CDetectCard::DetectEMV(void) 
{
    USHORT usErr = d_OK;
    BOOL bResult = FALSE;
    BYTE byStatus = 0;

    usErr = CTOS_SCStatus(d_SC_USER, &byStatus);
    if ((d_OK == usErr) && (d_MK_SC_PRESENT == byStatus))
    {
        bResult = TRUE;
    }
    return bResult;
 }

#define DEFAULT_CTLS_CMD_BUF_LEN    (20)
static BOOL detectCardTypeA_() 
{
    BOOL bResult = FALSE;
    BYTE byCSNLen1;
    USHORT usRet;
    BYTE byCSNLen2;
    BYTE byAutoBR = 0;
    BYTE byATQA1[ DEFAULT_CTLS_CMD_BUF_LEN ] = { 0 };
    BYTE bySAK1[ DEFAULT_CTLS_CMD_BUF_LEN ] = { 0 };
    BYTE byCSN1[ DEFAULT_CTLS_CMD_BUF_LEN ] = { 0 };
    BYTE byATQA2[ DEFAULT_CTLS_CMD_BUF_LEN ] = { 0 };
    BYTE bySAK2[ DEFAULT_CTLS_CMD_BUF_LEN ] = { 0 };
    BYTE byCSN2[ DEFAULT_CTLS_CMD_BUF_LEN ] = { 0 };
    BYTE byATSBuf[ DEFAULT_CTLS_CMD_BUF_LEN ] = { 0 };
    USHORT usATSLen = sizeof(byATSBuf);

    byCSNLen1 = sizeof(byCSN1);
    usRet = CTOS_CLTypeAActiveFromIdle(0,
                                       byATQA1,
                                       bySAK1,
                                       byCSN1,
                                       &byCSNLen1);
    if (d_OK != usRet)
    {
        goto __func_End;
    }

    usRet = CTOS_CLTypeAHalt();
    if (d_OK != usRet)
    {
        goto __func_End;
    }

    byCSNLen2 = sizeof(byCSN2);
    usRet = CTOS_CLTypeAActiveFromIdle(0,
                                       byATQA2,
                                       bySAK2,
                                       byCSN2,
                                       &byCSNLen2);
    if (d_CL_ACCESS_TIMEOUT != usRet)
    {
        goto __func_End;
    }

    usRet = CTOS_CLTypeAActiveFromHalt(0,
                                       byATQA1,
                                       bySAK1,
                                       byCSN1,
                                       byCSNLen1);
    if (d_OK != usRet)
    {
        goto __func_End;
    }

    usRet = CTOS_CLRATS(byAutoBR, byATSBuf, &usATSLen);
    if (d_OK != usRet)
    {
        goto __func_End;
    }

    bResult = TRUE; // Dected card.
__func_End:
    return bResult;
}

static BOOL detectCardTypeB_(void)
{
    BOOL bResult = FALSE;
    USHORT usRet;
    BYTE byPUPI[ DEFAULT_CTLS_CMD_BUF_LEN ];

    memset(byPUPI, 0, sizeof(byPUPI));
    usRet = CTOS_CLTypeBActive(byPUPI);
    if (d_OK != usRet)
    {
        goto __func_End;
    }

    usRet = CTOS_HALTB(byPUPI);
    if (d_OK != usRet)
    {
        goto __func_End;
    }

    BYTE byPUPI2[ DEFAULT_CTLS_CMD_BUF_LEN ];
    usRet = CTOS_CLTypeBActive(byPUPI2);
    if (d_CL_ACCESS_TIMEOUT != usRet)
    {
        goto __func_End;
    }

    usRet = CTOS_CLTypeBActive(byPUPI);
    if (d_OK != usRet)
    {
        goto __func_End;
    }

    bResult = TRUE; 
__func_End:
    return bResult;
}

BOOL CDetectCard::DetectEMVCL(void)
{
    USHORT usErr = d_OK;
    BOOL bResult = FALSE;

    if (detectCardTypeA_() || detectCardTypeB_())
    {
        bResult = TRUE;
        CTOS_CLPowerOff();
    }
    return bResult;
}