
/*
 * File Name: CallUserAction.cpp
 * Author: Alan.Ren
 * 
 * Create Date: 2019.05.21
 */

#include "CallUserAction.h"

#include "TxnJsonKey.h"
#include "dbghlp.h"
#include "TxnDef.h"
#include "Inc/TxnState.h"
#include "Inc/ErrorCode.h"
#include "StringFun.h"
#include <emvaplib.h>


CCallUserAction::CCallUserAction(
    IN IUserAction *pUserActionPtr,
    IN IParameter *pParamPtr):
    m_pUserAction(pUserActionPtr),
    m_pParamPtr(pParamPtr)
{ }

CCallUserAction::~CCallUserAction()
{ }


void CCallUserAction::ReqUserCallBank(OUT WORD *pwTxnResult)
{
}


void CCallUserAction::ReqUserPrintSignature(void)
{
    if (IsEnableUserAction(EnablePrintSignautre))
    {
        const char *pszJsonStr = NULL;
        cJSON *pJsonRoot = NULL;
        char szRspBuf[256] = {0};

        do
        {
            pJsonRoot = cJSON_CreateObject();
            if (NULL != pJsonRoot)
                break;

            cJSON_AddStringToObject(pJsonRoot, JK_Act_Type, JV_Act_Print_Signature);

            pszJsonStr = cJSON_Print(pJsonRoot);
            if (NULL == pszJsonStr)
                break;

            ReqUserAction(TXN_STATE_PRINT_SIGNATURE, pszJsonStr, strlen(pszJsonStr));
            WaitUserFinishAction(TXN_STATE_PRINT_SIGNATURE, szRspBuf, sizeof(szRspBuf));
        } while (0);

        if (NULL != pszJsonStr)
            cJSON_free((void *)pszJsonStr);
        if (NULL != pJsonRoot)
            cJSON_Delete(pJsonRoot);
    }
}


void CCallUserAction::ReqUserPromptLastPINInput(void)
{
    if (IsEnableUserAction(EnablePromptLastPINInput))
    {
        cJSON *pcJsonRoot = NULL;
        const char *pszJsonStr = NULL;
        char szRspBuffer[256] = {0};

        do
        {
            pcJsonRoot = cJSON_CreateObject();
            if (NULL == pcJsonRoot)
                break;

            cJSON_AddStringToObject(pcJsonRoot, JK_Act_Type,
                                    JV_Confirm_Prompt_Last_PIN_Input);

            pszJsonStr = cJSON_Print(pcJsonRoot);
            if (NULL == pszJsonStr)
                break;

            ReqUserAction(TXN_STATE_LAST_PIN_INPUT, pszJsonStr, strlen(pszJsonStr));
            WaitUserFinishAction(TXN_STATE_LAST_PIN_INPUT, szRspBuffer, sizeof(szRspBuffer));
        } while (0);

        if (NULL != pcJsonRoot)
            cJSON_Delete(pcJsonRoot);
        if (NULL != pszJsonStr)
            cJSON_free((void *)pszJsonStr);
    }
}

BOOL CCallUserAction::ReqUserConfirmConnWithHost(void)// in this case, this function always return true
{
    BOOL bResult = TRUE;

    if (IsEnableUserAction(EnableConfirmCommWithHost))
    {
        char szRspBuf[256] = {0};
        const char *pszReqStr = NULL;
        cJSON *pcRoot = NULL;
        do
        {
            pcRoot = cJSON_CreateObject();
            if (NULL == pcRoot)
                break;

            cJSON_AddStringToObject(pcRoot, JK_Act_Type, JV_Confirm_Comm_With_Host);

            pszReqStr = cJSON_Print(pcRoot);
            if (NULL == pszReqStr)
                break;

            ReqUserAction(TXN_STATE_CONFIRM_COMM_WITH_HOST, pszReqStr, strlen(pszReqStr));
            WaitUserFinishAction(TXN_STATE_CONFIRM_COMM_WITH_HOST, szRspBuf, sizeof(szRspBuf));
            if (0 == strcasecmp(szRspBuf, "true"))
            {
                bResult = TRUE;
            }
        } while (0);

        if (NULL != pcRoot)
            cJSON_Delete(pcRoot);
        if (NULL != pszReqStr)
            cJSON_free((void *)pszReqStr);
    }
    return bResult;
}

void CCallUserAction::ReqUserChkExpFile(IN BYTE *pPAN,
                                       IN BYTE bPANLen,
                                       OUT BOOL *isException)
{
    (*isException) = FALSE;
    if (IsEnableUserAction(EnableActExpFile))
    {
        char szPanData[64] = {0};
        const char *pszJsonStr = NULL;
        cJSON *pcJsonRoot = NULL;
        BYTE szUserInput[260] = {0};

        for (BYTE i = 0; i < bPANLen; ++i)
        {
            sprintf(&szPanData[i * 2], "%02X", pPAN[i]);
        }

        do
        {
            pcJsonRoot = cJSON_CreateObject();
            if (NULL == pcJsonRoot)
            {
                TraceMsg("Error: Call cJSON_CreateObject() failed.");
                break;
            }

            cJSON_AddStringToObject(pcJsonRoot, JK_Act_Type, JV_Act_ExceptionCheck);
            cJSON_AddStringToObject(pcJsonRoot, JK_Act_Value, (char *)szPanData);
            pszJsonStr = cJSON_Print(pcJsonRoot);
            if (NULL == pszJsonStr)
            {
                TraceMsg("Error: Call cJSON_Print failed.");
                break;
            }

            ReqUserAction(TXN_STATE_EXCEPTION_FILE_CHECK, pszJsonStr, strlen(pszJsonStr));
            WaitUserFinishAction(TXN_STATE_EXCEPTION_FILE_CHECK, szUserInput, sizeof(szUserInput));
            if (0 == strcasecmp((char *)szUserInput, "true"))
            {
                (*isException) = TRUE;
                TraceMsg("ExptionFileCheck WaitUserFinishAction:%s", szUserInput);
            }
        } while (0);
        if (NULL != pszJsonStr)
        {
            cJSON_free((void *)pszJsonStr);
        }
        if (NULL != pcJsonRoot)
        {
            cJSON_Delete(pcJsonRoot);
        }
    }
}

void CCallUserAction::ReqUserOutTotalAmt(IN BYTE *pPAN,
                                     IN BYTE bPANLen,
                                     OUT unsigned long *pAmount)
{
    tagParam cTagParam = {0};
    HEARTLAND_GENERAL_PARAM cGeneralParam = { 0 };

    InitTagParam(cTagParam, kGeneralParam, cGeneralParam);
    m_pParamPtr->GetParameter(&cTagParam);

    int nFirstPlainDigits = 4;
    if (strlen(cGeneralParam.szMaskedAcctMode))
    {
        nFirstPlainDigits = atoi(cGeneralParam.szMaskedAcctMode);
    }
    TraceMsg("INFO: First Plain Digits for account: %d", nFirstPlainDigits);


    (*pAmount) = 0;
    if (IsEnableUserAction(EnableActTotalAmt))
    {
        cJSON *pcJsonRoot = NULL;
        char szUserInput[260] = {0};
        char szPAN[32 + 1] = {0};
        const char *pszJsonStr = NULL;

        do
        {
            pcJsonRoot = cJSON_CreateObject();
            if (NULL == pcJsonRoot)
            {
                TraceMsg("Error: Call cJSON_CreateObject() failed.");
                break;
            }

            cJSON_AddStringToObject(pcJsonRoot, JK_Act_Type, JV_Act_TotalAmt);
            for (int nIndex = 0; nIndex < bPANLen; ++nIndex)
            {
                sprintf(szPAN + (nIndex * 2), "%02X", *(pPAN + nIndex));
            }
            CStringFun::MaskString(szPAN, nFirstPlainDigits, 4, '*');
            cJSON_AddStringToObject(pcJsonRoot, JK_PAN, szPAN);
            TraceMsg("Info: PAN=%s", szPAN);
            pszJsonStr = cJSON_Print(pcJsonRoot);
            if (NULL == pszJsonStr)
            {
                TraceMsg("Error: Call cJSON_Print() failed.");
                break;
            }
            ReqUserAction(TXN_STATE_TOTAL_AMOUNT_GET, pszJsonStr, strlen(pszJsonStr));
            WaitUserFinishAction(TXN_STATE_TOTAL_AMOUNT_GET, szUserInput, sizeof(szUserInput) - 1);

            // Total amount
            (*pAmount) = strtoul(szUserInput, NULL, 10);
            TraceMsg("Info: Total amount %lu", (*pAmount));
        } while (0);

        if (NULL != pszJsonStr)
        {
            cJSON_free((void *)pszJsonStr);
        }
        if (NULL != pcJsonRoot)
        {
            cJSON_Delete(pcJsonRoot);
        }
    }
}

WORD CCallUserAction::MatchLanguage(OUT BOOL *pbNeedSelect,
                                    OUT BYTE *pbyLanguage)
{
    TraceMsg("MatchLanguage entry");
    
    BYTE byLanguage[16] = {0};
    WORD wLanguageLen = sizeof(byLanguage);
    WORD wRet = EMV_DataGet(0x5F2D, &wLanguageLen, byLanguage);
    if(d_EMVAPLIB_OK != wRet)
    {
        TraceMsg("EMV_DataGet fail, wRet:0x%04X",  wRet);
        return wRet;
    }
    
    TraceMsg("5F2D is %s",  byLanguage);
    *pbyLanguage = LANGUAGE_INVALID;//invalid language
    for(int i = 0; i < wLanguageLen; ++i)
    {
        if(byLanguage[i] == 'e' && byLanguage[i+1] == 'n')
        {
            *pbyLanguage = LANGUAGE_EN;//english
            break;
        }
        if(byLanguage[i] == 'f' && byLanguage[i+1] == 'r')
        {
            *pbyLanguage = LANGUAGE_FR;//french
            break;
        }
    }
    
    if(0 == *pbyLanguage)
    {
        *pbNeedSelect = TRUE;
    }
    else
    {
        *pbNeedSelect = FALSE;
    }
    
    TraceMsg("NeedSelect is %d, Language is %d", *pbNeedSelect, *pbyLanguage);
    TraceMsg("MatchLanguage exit");
    return OK;
}

void CCallUserAction::LanguageProcessing(void)
{
    BOOL bSelectLang;
    BYTE byLanguage;
    BYTE szUserInput[260] = {0};
    WORD wRet = MatchLanguage(&bSelectLang, &byLanguage);
    if(OK == wRet)
    {
        cJSON *pcRoot = NULL;
        const char *pszJson = NULL;
        
        if(bSelectLang)//need user select language
        {
            pcRoot = cJSON_CreateObject();
            if(NULL == pcRoot)
                TraceMsg("Error: Call cJSON_CreateObject() failed.");

            cJSON_AddStringToObject(pcRoot, JK_Act_Type, JV_Act_Select_Language);
            
            pszJson = cJSON_Print(pcRoot);
            if(NULL == pszJson)
                TraceMsg("Error: Call cJSON_Print() failed.");
            
            TraceMsg("pcRoot is %s", pszJson);
            ReqUserAction(TXN_STATE_SELECT_LANGUAGE, (const void *)pszJson, strlen(pszJson));
            WaitUserFinishAction(TXN_STATE_SELECT_LANGUAGE, szUserInput, sizeof(szUserInput));
        }
        else if(LANGUAGE_FR == byLanguage)
        {
            pcRoot = cJSON_CreateObject();
            if(NULL == pcRoot)
                TraceMsg("Error: Call cJSON_CreateObject() failed.");

            cJSON_AddStringToObject(pcRoot, JK_Act_Type, JV_Act_Switch_Language);
            
            pszJson = cJSON_Print(pcRoot);
            if(NULL == pszJson)
                TraceMsg("Error: Call cJSON_Print() failed.");
            
            TraceMsg("pcRoot is %s", pszJson);
            ReqUserAction(TXN_STATE_SWITCH_LANGUAGE, (const void *)pszJson, strlen(pszJson));
            WaitUserFinishAction(TXN_STATE_SWITCH_LANGUAGE, szUserInput, sizeof(szUserInput));
        }
        else
        {
            TraceMsg("Do not need to change the language");
        }
        
        if (NULL != pcRoot)
        {
            cJSON_Delete(pcRoot);
        }
        if (NULL != pszJson)
        {
            cJSON_free((void *)pszJson);
        }
    }
}

WORD CCallUserAction::ReqUserConfirmSelectedAID(IN BYTE *pbyLabel,
                                           IN BYTE byLen)
{
    WORD wErr = d_EMVAPLIB_OK;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonStr = NULL;
    BYTE szUserInput[260] = {0};
    
    LanguageProcessing();

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        
        if (NULL == pcJsonRoot)
        {
            wErr = ERROR_CODE_AIDCONFIRM_FAIL;
            TraceMsg("Error: Call cJSON_CreateObject() failed.");
            break;
        }

        cJSON_AddStringToObject(pcJsonRoot, JK_Act_Type, JV_Act_AidConfim);
        cJSON_AddStringToObject(pcJsonRoot, JK_Act_Value, (char *)pbyLabel);

        pszJsonStr = cJSON_Print(pcJsonRoot);
        if (pszJsonStr == NULL)
        {
            wErr = ERROR_CODE_AIDCONFIRM_FAIL;
            TraceMsg("Error: Call cJSON_Print() failed.");
            break;
        }

        ReqUserAction(TXN_STATE_AID_CONFIRM, (const void *)pszJsonStr, strlen(pszJsonStr));
        WaitUserFinishAction(TXN_STATE_AID_CONFIRM, szUserInput, sizeof(szUserInput));
        if (0 != strcasecmp((char *)szUserInput, "true"))
        {
            wErr = ERROR_CODE_AIDCONFIRM_REFUSE;
            TraceMsg("Error: AppSelectedConfirm WaitUserFinishAction:%s", szUserInput);
            break;
        }

        TraceMsg("Info: User confirm.");
    } while (0);

    if (NULL != pcJsonRoot)
    {
        cJSON_Delete(pcJsonRoot);
    }
    if (NULL != pszJsonStr)
    {
        cJSON_free((void *)pszJsonStr);
    }
    return wErr;
}

WORD CCallUserAction::ReqUserSelectAID(IN BYTE byAppNum,
                                       IN char szAppLabel[][17],
                                       OUT BYTE *pbySelIndex)
{
    WORD wErr = ERROR_CODE_AIDLIST_FAIL;
    cJSON *pcJsonRoot = NULL;
    cJSON *pcJsonArray = NULL;
    BYTE szUserInput[260] = { 0 };
    const char *pszJsonStr  = NULL;
    
    LanguageProcessing();

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
        {
            TraceMsg("Error: Call cJSON_CreateObject() failed.");
            break;
        }

        cJSON_AddStringToObject(pcJsonRoot, JK_Act_Type, JV_Act_AidList);

#ifndef PLATFORM_ANDROID
        cJSON_AddItemToObject(pcJsonRoot, JK_Act_Value, pcJsonArray = cJSON_CreateArray());
#else
        cJSON_AddItemToObject(pcJsonRoot, JK_Act_ListValue, pcJsonArray = cJSON_CreateArray());
#endif

        for (BYTE byIdx = 0; byIdx < byAppNum; ++byIdx)
        {
            TraceMsg("AppList AppLabel:%s", szAppLabel[byIdx]);
            cJSON_AddItemToArray(pcJsonArray, cJSON_CreateString(szAppLabel[byIdx]));
        }

        pszJsonStr = cJSON_Print(pcJsonRoot);
        if (NULL == pszJsonStr)
        {
            TraceMsg("Error: Call cJSON_Print() failed.");
            break;
        }

        ReqUserAction(TXN_STATE_AID_SELECT, (const void *)pszJsonStr, strlen(pszJsonStr));
        WaitUserFinishAction(TXN_STATE_AID_SELECT, (void *)szUserInput, sizeof(szUserInput));
        TraceMsg("Info: User input %s", szUserInput);

        if (0 != memcmp(szUserInput, "##", 2))
        {
            for (BYTE byIndex = 0; byIndex < byAppNum; ++byIndex)
            {
                if (0 == strcasecmp((char *)szUserInput, szAppLabel[byIndex]))
                {
                    (*pbySelIndex) = byIndex;
                    break;
                }
            }
            byAppNum = 0;
        }
        else
        {
            int nSelIndex = -1;
            int nScanRet = -1;
            nScanRet = sscanf((char *)szUserInput, "##%d", &nSelIndex);
            if (1 == nScanRet && -1 != nSelIndex &&
                nSelIndex > 0 && nSelIndex <= byAppNum)
            {
                (*pbySelIndex) = static_cast<BYTE>(nSelIndex);
                (*pbySelIndex) -= 1;
                TraceMsg("Info: Other select: %d", (*pbySelIndex));
            }
        }
        TraceMsg("Info: User selected aid.");
    } while (0);

    if (NULL != pcJsonRoot)
        cJSON_Delete(pcJsonRoot);
    if (NULL != pszJsonStr)
        cJSON_free((void *)pszJsonStr);

    if ((*pbySelIndex) >= 0 && (*pbySelIndex) < byAppNum) 
    {
        wErr = d_EMVAPLIB_OK;
    }
    return wErr;
}

BOOL CCallUserAction::IsEnableUserAction(IN const DWORD &dwFlag)
{
    BOOL bEnable = FALSE;
    tagParam cParamObj = {0};
    HEARTLAND_GENERAL_PARAM cParam = {0};
    int nRe;

    if (NULL != m_pParamPtr)
    {
        InitTagParam(cParamObj, kGeneralParam, cParam);
        nRe = m_pParamPtr->GetParameter(&cParamObj);
        if (0 == nRe)
        {
            bEnable = (cParam.dwUserActionBitMap & dwFlag) ? TRUE : FALSE;
        }
    }
    return bEnable;
}

void CCallUserAction::ReqUserAction(IN const unsigned int &nActType,
                                        IN const void *pVoidData,
                                        IN const size_t &nDateLen)
{
    if (NULL != m_pUserAction)
    {
        m_pUserAction->RequsetUserAction(nActType, pVoidData, nDateLen);
    }
}

void CCallUserAction::WaitUserFinishAction(IN const unsigned int &nActType,
                                       OUT void *pBuffer,
                                       IN const size_t &nBufSize)
{
    if (NULL != m_pUserAction)
    {
        m_pUserAction->WaitUserResponse(nActType, pBuffer, nBufSize - 1);
        cJSON *pcJsonRoot = NULL;
        const cJSON *pcSubItem = NULL;

        do
        {
            pcJsonRoot = cJSON_Parse((char *)pBuffer);
            if (pcJsonRoot == NULL)
            {
                TraceMsg("Error: Call cJSON_Parse() failed.");
                break;
            }

            bzero(pBuffer, nBufSize);
            pcSubItem = cJSON_GetObjectItem(pcJsonRoot, JK_Act_Value);
            if (NULL != pcSubItem)
            {
                strcpy((char *)pBuffer, pcSubItem->valuestring);
                TraceMsg("Info: Act Value=%s", (char *)pBuffer);
            }

            cJSON_Delete(pcJsonRoot);
            pcJsonRoot = NULL;
        } while (0);
    }
}


void CCallUserAction::ReqUserInputEMVPIN(BOOL bStart)
{
    char szRspBuf[256] = {0};
    if (bStart == TRUE)
    {
        ReqUserAction(TXN_STATE_PIN_ENTER, NULL, 0);
    }
    else
    {
        WaitUserFinishAction(TXN_STATE_PIN_ENTER, szRspBuf, sizeof(szRspBuf));
    }
}


void CCallUserAction::ReqUserInputPosSeqNbr(OUT BYTE* pbyPosSeqNbr)
{
    WORD wErr = d_EMVAPLIB_OK;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonStr = NULL;
    BYTE szUserInput[260] = {0};

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
        {
            wErr = ERROR_CODE_INTERAC_POS_SEQUENCE_NBR;
            TraceMsg("Error: Call cJSON_CreateObject() failed.");
            break;
        }

        cJSON_AddStringToObject(pcJsonRoot, JK_Act_Type, JV_Act_InteracPosSeqNbr);

        pszJsonStr = cJSON_Print(pcJsonRoot);
        if (pszJsonStr == NULL)
        {
            wErr = ERROR_CODE_INTERAC_POS_SEQUENCE_NBR;
            TraceMsg("Error: Call cJSON_Print() failed.");
            break;
        }

        ReqUserAction(TXN_STATE_INTERAC_POS_SEQNBR, (const void *)pszJsonStr, strlen(pszJsonStr));
        WaitUserFinishAction(TXN_STATE_INTERAC_POS_SEQNBR, szUserInput, sizeof(szUserInput));
        if (0 != strlen((char *)szUserInput))
        {
            strcpy((char *)pbyPosSeqNbr, (char *)szUserInput);
            TraceMsg("Info: Interac Request POS sequence number, WaitUserFinishAction:%s", szUserInput);
            break;
        }
    } while (0);

    if (NULL != pcJsonRoot)
    {
        cJSON_Delete(pcJsonRoot);
    }
    if (NULL != pszJsonStr)
    {
        cJSON_free((void *)pszJsonStr);
    }
}


void CCallUserAction::ReqUserInputAccountType(OUT BYTE* pbyAccountType)
{
    WORD wErr = d_EMVAPLIB_OK;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonStr = NULL;
    BYTE szUserInput[260] = {0};

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
        {
            wErr = ERROR_CODE_INTERAC_ACCOUNT_TYPE;
            TraceMsg("Error: Call cJSON_CreateObject() failed.");
            break;
        }

        cJSON_AddStringToObject(pcJsonRoot, JK_Act_Type, JV_Act_InteracAccountType);

        pszJsonStr = cJSON_Print(pcJsonRoot);
        if (pszJsonStr == NULL)
        {
            wErr = ERROR_CODE_INTERAC_ACCOUNT_TYPE;
            TraceMsg("Error: Call cJSON_Print() failed.");
            break;
        }

        ReqUserAction(TXN_STATE_INTERAC_ACCOUNT_TYPE, (const void *)pszJsonStr, strlen(pszJsonStr));
        WaitUserFinishAction(TXN_STATE_INTERAC_ACCOUNT_TYPE, szUserInput, sizeof(szUserInput));
        if (0 != strlen((char *)szUserInput))
        {
            strcpy((char *)pbyAccountType, (char *)szUserInput);
            TraceMsg("Info: Interac Request account type, WaitUserFinishAction:%s", szUserInput);
            break;
        }
    } while (0);

    if (NULL != pcJsonRoot)
    {
        cJSON_Delete(pcJsonRoot);
    }
    if (NULL != pszJsonStr)
    {
        cJSON_free((void *)pszJsonStr);
    }
}

#if 0
WORD CCallUserAction::ReqUserInteracReceiptWarning(void)
{
    TraceMsg("Info: %s Entry", __FUNCTION__);
    WORD wErr = d_EMVAPLIB_OK;
    if (IsEnableUserAction(EnableInteracReceiptWarning))
    {
        TraceMsg("Info: Enable InteracReceiptWarning");

        const char *pszJsonStr = NULL;
        cJSON *pJsonRoot = NULL;
        char szRspBuf[256] = {0};

        do
        {
            TraceMsg("Info: - 1");
            pJsonRoot = cJSON_CreateObject();
            if (NULL != pJsonRoot)
                break;

            TraceMsg("Info: - 2");
            cJSON_AddStringToObject(pJsonRoot, JK_Act_Type, JV_Act_Interac_Receipt_Warning);

            pszJsonStr = cJSON_Print(pJsonRoot);
            if (NULL == pszJsonStr)
                break;

            TraceMsg("Info: - 3");
            ReqUserAction(TXN_STATE_INTERAC_RECEIPT_WARNINIG, pszJsonStr, strlen(pszJsonStr));
            WaitUserFinishAction(TXN_STATE_INTERAC_RECEIPT_WARNINIG, szRspBuf, sizeof(szRspBuf));

            TraceMsg("Info: - 4");
            if (0 != strcasecmp((char *)szRspBuf, "true"))
            {
                wErr = ERROR_CODE_INTERAC_RECEIPT_WARNING;
                TraceMsg("Error: Interac Receipt Warning WaitUserFinishAction:%s", szRspBuf);
                break;
            }
            TraceMsg("Info: - 5");
        } while (0);

        if (NULL != pszJsonStr)
            cJSON_free((void *)pszJsonStr);
        if (NULL != pJsonRoot)
            cJSON_Delete(pJsonRoot);
    }

    TraceMsg("Info: %s Exit wErr:0x%04X", __FUNCTION__, wErr);
    return wErr;
}
#else
WORD CCallUserAction::ReqUserInteracReceiptWarning(void)
{
    TraceMsg("Info: %s Entry", __FUNCTION__);
    WORD wErr = d_EMVAPLIB_OK;
    if (IsEnableUserAction(EnableInteracReceiptWarning))
    {
        TraceMsg("Info: Enable InteracReceiptWarning");

        cJSON *pJsonRoot = NULL;
        char szRspBuf[256] = {0};

        do
        {
            TraceMsg("Info: - 1");
            ReqUserAction(TXN_STATE_INTERAC_RECEIPT_WARNINIG, "{ }", 3);
            WaitUserFinishAction(TXN_STATE_INTERAC_RECEIPT_WARNINIG, szRspBuf, sizeof(szRspBuf));

            TraceMsg("Info: - 2");
            if (0 != strcasecmp((char *)szRspBuf, "true"))
            {
                wErr = ERROR_CODE_INTERAC_RECEIPT_WARNING;
                TraceMsg("Error: Interac Receipt Warning WaitUserFinishAction:%s", szRspBuf);
                break;
            }
            TraceMsg("Info: - 3");
        } while (0);
    }

    TraceMsg("Info: %s Exit wErr:0x%04X", __FUNCTION__, wErr);
    return wErr;
}
#endif

