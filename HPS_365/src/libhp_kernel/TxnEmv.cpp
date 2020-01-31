
/*
 * File Name: TxnEmv.cpp
 * Author: Tim.Ma
 * 
 * Create Date: 2019.2.28
 */

#include "ByteStream.h"
#include "Inc/ErrorCode.h"
#include "FunBase.h"
#include "StringFun.h"
#include "Inc/DefParamTypeId.h"
#include "Inc/TxnDataType.h"
#include "TxnEmv.h"
#include "TxnDef.h"
#include "TxnTag.h"
#include "TlvFun.h"
#include "dbghlp.h"
#include "Inc/TxnState.h"

#include <emvaplib.h>

static const WORD wTagList_[] = 
{
    0x9F02, 0x9F03, 0x9F26, 0x0082, 0x9F36, 0x9F1A, 0x0095,
    0x5F2A, 0x009A, 0x009C, 0x9F37, 0x004F, 0x9F10, 0x0057, 
    0x9F39, 0x9F40, 0x9F06, 0x5F34, 0x9F07, 0x9F08, 0x9F09,
    0x008A, 0x9F34, 0x9F27, 0x9F7C, 0x0084, 0x9F6E, 0x9F0D, 
    0x9F0E, 0x9F0F, 0x5F28, 0x9F5B, 0x9F33, 0x9F35, 0x9F41, 
    0x009B, 0x9F21, 0x5F25, 0x5F24
};


/**
* Function:
*      CTxnEmv's Construct Function:
*
* @param
*      pParamPtr*       : the parameters of the pool
*      pDataPackPtr*    : point of the object to pack and unpack
*      IBaseCurl*       : point of the object to communicate with host
*      CEMVApi*         : point of the object associated with 
*      IUserAction*     : point of the object to chat with user
*
* @return
*      Void.
*
* Author: Tim.Ma
*/
CTxnEmv::CTxnEmv(IN IParameter *pParamPtr, 
        IN IDataPack *pDataPackPtr,
        IN IBaseCurl *pBaseCurlPtr,
        IN CEMVApi *pEmvApi,
        IN IUserAction *pUserAction)
:
m_pEmvApi(pEmvApi),
#ifdef PLATFORM_ANDROID
CTxnContext("INSERT",
#else
CTxnContext("CONTACT",
#endif
        pParamPtr,
        pDataPackPtr,
        pBaseCurlPtr,
        NULL,
        pUserAction)
{
    m_pszPanEntryLegend = "CHIP READ";
}


/**
* Function:
*      Destruct Function:
*
* @param
*      none
*
* @return
*      none.
*
* Author: Tim.Ma
*/
CTxnEmv::~CTxnEmv()
{
    
}


int CTxnEmv::InitLevel2Lib(void)
{
    return InitEMV();
}


WORD CTxnEmv::InitEMV(void)
{
    // Clear cache
    bFallbackFlag_ = FALSE;
    m_wEMVCompletionResponse = 0;

    CTxnContext::InitParam();

    EnableCancelTxn(FALSE);

    static int nFlag = 0;
    
    HEARTLAND_EMV_PARAM cEmvParam = { 0 };
    tagParam cTagParam = { 0 };

    InitTagParam(cTagParam, kEmvParam, cEmvParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter Failed:%d",  nRet);
        return nRet;
    }

#ifndef PLATFORM_ANDROID
    TraceMsg("INFO: CTxnEmv::InitEMV m_nDebugFlag:%d", cEmvParam.m_nDebugFlag);
    if (cEmvParam.m_nDebugFlag == 1)
    {
        m_pEmvApi->SetDebug(TRUE, 0xFF);
    }
    else
    {
        m_pEmvApi->SetDebug(FALSE, 0xFF);
    }
#endif

    if (nFlag != 0)
    {
        TraceMsg("EMV has already initialized");
        return 0;
    }
    
    nRet = m_pEmvApi->InitEMV(cEmvParam.m_pszConfigFile, cEmvParam.m_nDebugFlag);
    if (0 != nRet)
    {
        TraceMsg("InitEMV Failed:0x%04X, ConfigFile:%s",  nRet, cEmvParam.m_pszConfigFile);
        return nRet;
    }
    
    nFlag = 1;
    return 0;
}


WORD CTxnEmv::SelectTxnApp(void)
{
    int nRet;
    TraceMsg("SelectTxnApp entry");
    
    nRet = m_pEmvApi->SelectTxnApp();
    if (d_SC_BAD_TS == nRet || 
        d_SC_COMM_ERROR == nRet ||
        d_SC_MUTE == nRet ||
        d_EMVAPLIB_ERR_NO_AP_FOUND == nRet)
    {
        if(CheckAPPInCard((BYTE*)"\xA0\x00\x00\x02\x77"))// not interac, need do fallback
        { 
            SetState(TXN_STATE_PLEASE_SWIPE_CARD);
            bFallbackFlag_ = TRUE;
            TraceMsg("PerformEMVTxn perform fallback, nRet:0x%04X",  nRet);
        }
        return nRet;
    }
    else if (0xEA01 == nRet)
    {
        TraceMsg("PerformEMVTxn perform fallback, nRet:0x%04X",  nRet);
        SetState(TXN_STATE_IDLE);
    }
    else if (d_EMVAPLIB_OK != nRet)
    {
        TraceMsg("PerformEMVTxn perform fallback, nRet:0x%04X",  nRet);
        SetState(TXN_STATE_DECLINED);
        return nRet;
    }
    
    // Get Tag57
    BYTE byTagValue[128] = { 0 };
    USHORT usTagLen = sizeof(byTagValue);
    WORD wRet = EMV_DataGet(0x57, &usTagLen, byTagValue);
    if (wRet != d_OK)
    {
        TraceMsg("EMV_DataGet <0x57> failed wRet:0x%04X", wRet);
        return wRet;
    }
    BYTE byszTrack2Data[128] = {0};
    WORD wTrack2DataLen = 0;
    CStringFun::Track2ToFormatedStr(byTagValue, usTagLen, byszTrack2Data, &wTrack2DataLen);
    
    CTxnContext::GetAcctNbr(byszTrack2Data, wTrack2DataLen);
    CTxnContext::GetCardHolderNameFromICC();
    CTxnContext::SetPinParam();

    CTxnEmv::SetCardInformation();

    memset(m_byAppLabel, 0, sizeof(m_byAppLabel));
    wRet = EMV_DataGet(0x50, &usTagLen, byTagValue);
    if (wRet == d_OK)
    {
        TraceMsg("Tag[50]: %s", byTagValue);
        memcpy(m_byAppLabel, byTagValue, usTagLen);
    }

    return OK;
}


int CTxnEmv::PerformTxn(void)
{
    int nRet;

    nRet = InitEMV();
    if (nRet != 0)
    {
        TraceMsg("InitEMV Failed:%d",  nRet);
        return nRet;
    }

    // Add transaction sequence counter
    nRet = EMV_TxnPreDataSet((BYTE*)"\x9F\x41\x04\x00\x00\x00\x11", 7);
    TraceMsg("EMV_TxnPreDataSet %4x", nRet);
    
    nRet = CTxnContext::PreProcessing();
    if (nRet != 0)
    {
        TraceMsg("PreProcessing Failed:%d",  nRet);
        return nRet;
    }

    if (m_nPackSoapType == kDebitPollingCard_TxnType ||
        m_nPackSoapType == kCreditPollingCard_TxnType)
    {
        PollingCardProcess(kEmv);
        TraceMsg("Info: %s Exit, it's pollingcard transaction", __FUNCTION__);
        return 0;
    }
    
    nRet = m_pEmvApi->SelectTxnApp();
    if (d_SC_BAD_TS == nRet || 
        d_SC_COMM_ERROR == nRet ||
        d_SC_MUTE == nRet ||
        d_EMVAPLIB_ERR_NO_AP_FOUND == nRet)
    {
        if(CheckAPPInCard((BYTE*)"\xA0\x00\x00\x02\x77"))// not interac, need do fallback
        { 
            SetState(TXN_STATE_PLEASE_SWIPE_CARD);
            bFallbackFlag_ = TRUE;
        }
        TraceMsg("PerformEMVTxn SelectTxnApp, nRet:0x%04X",  nRet);
        return nRet;
    }
    else if (0xEA01 == nRet)
    {
        TraceMsg("PerformEMVTxn SelectTxnApp, nRet:0x%04X",  nRet);
        SetState(TXN_STATE_IDLE);
    }
    else if (d_EMVAPLIB_OK != nRet)
    {
        TraceMsg("PerformEMVTxn SelectTxnApp, nRet:0x%04X",  nRet);
        SetState(TXN_STATE_DECLINED);
        return nRet;
    }
    
    // Get Tag57
    BYTE byTagValue[128] = { 0 };
    USHORT usTagLen = sizeof(byTagValue);
    WORD wRet = EMV_DataGet(0x57, &usTagLen, byTagValue);
    if (wRet != d_OK)
    {
        TraceMsg("EMV_DataGet <0x57> failed wRet:0x%04X", wRet);
        return wRet;
    }
    BYTE byszTrack2Data[128] = {0};
    WORD wTrack2DataLen = 0;
    CStringFun::Track2ToFormatedStr(byTagValue, usTagLen, byszTrack2Data, &wTrack2DataLen);
    
    CTxnContext::GetAcctNbr(byszTrack2Data, wTrack2DataLen);
    CTxnContext::GetCardHolderNameFromICC();
    CTxnContext::SetPinParam();

    CTxnEmv::SetCardInformation();

    memset(m_byAppLabel, 0, sizeof(m_byAppLabel));
    wRet = EMV_DataGet(0x50, &usTagLen, byTagValue);
    if (wRet == d_OK)
    {
        TraceMsg("Tag[50]: %s", byTagValue);
        memcpy(m_byAppLabel, byTagValue, usTagLen);
    }

    // perform emv transaction
    nRet = m_pEmvApi->PerformEMVTxn();
    if (nRet == d_EMVAPLIB_OK)
    {
        InteracSpecialProcess(true);
        GetTxnResult(false);
    }
    else if(nRet == d_EMVAPLIB_ERR_EVENT_ONLINE)
    {
        InteracSpecialProcess(true);
    
        TxnOnline();
        GetTxnResult(true);
        if (CTxnContext::m_bNeedCheckSAF)
        {
            ExeSAF();
        }
    }
    else if (nRet != 0)
    {
        TraceMsg("PerformEMVTxn Failed:0x%04X",  nRet);
        SetState(TXN_STATE_DECLINED);
        return nRet;
    }
    
    nRet = CTxnContext::PostProcessing();
    if (d_TXN_HOST_TIMEOUT == nRet ||d_TXN_APPROVAL_WITH_KERNEL_DECLINE == nRet)
    {
        SendReversal();
    }
    
    return nRet;
}


WORD CTxnEmv::CompleteEMVTxn(void)
{
    TraceMsg("Info: %s Entry", __FUNCTION__);
    EMV_ONLINE_RESPONSE_DATA cOnlineRspData = { 0 };

    cOnlineRspData.bAction = m_byActionCode;
    cOnlineRspData.pAuthorizationCode = m_byAuthCode;
    cOnlineRspData.pIssuerAuthenticationData = m_byIADBuf;
    cOnlineRspData.IssuerAuthenticationDataLen = m_wRecvIADLen;
    cOnlineRspData.pIssuerScript = m_byIssuerScript;
    cOnlineRspData.IssuerScriptLen = m_wRecvIssuerScriptLen;

    m_wEMVCompletionResponse = m_pEmvApi->CompleteEMVTxn(&cOnlineRspData);
    
    TraceMsg("Info: %s m_wEMVCompletionResponse:%X Exit", __FUNCTION__, m_wEMVCompletionResponse);
    return m_wEMVCompletionResponse;
}


void CTxnEmv::RegisterEMVSpecEvt(IN IEMVSpecialEvt *pcEvt, unsigned int uEvtMask)
{
    return m_pEmvApi->RegisterEMVSpecEvt(pcEvt, uEvtMask);
}




WORD CTxnEmv::ReadSpecialTags(WORD wTagName, unsigned char *pszTlvValue, WORD *pwTlvValueLen)
{
    BYTE *pbyBuffer = NULL;

    // clear buffer pszTlvValue with the special length: *pwTlvValueLen
    memset(pszTlvValue, 0, *pwTlvValueLen);

    // get data from EMV pool
    WORD wErr = EMV_DataGet(wTagName, pwTlvValueLen, pszTlvValue);
    if (wErr != 0)
    {
        if (wTagName != 0x9F12)
        {
            *pwTlvValueLen = 0;
            TraceMsg("ERROR: ReadSpecialTags[%X] FAIL", wTagName);
            return 1;
        }

        if (strlen((char *)m_byAppLabel) != 0)
        {
            *pwTlvValueLen = strlen((char *)m_byAppLabel);
            memcpy(pszTlvValue, m_byAppLabel, *pwTlvValueLen);

            return 0;
        } 

        TraceMsg("WARNING: ReadSpecialTags[%X] FAIL, Searching from Tag[50]", wTagName);
        wTagName = 0x50;
        wErr = EMV_DataGet(wTagName, pwTlvValueLen, pszTlvValue);
        if (wErr != 0)
        {
            *pwTlvValueLen = 0;
            TraceMsg("ERROR: ReadSpecialTags[%X] FAIL", wTagName);
            return 1;
        }
    }
    
    return 0;
}


void CTxnEmv::SendReversal(void)
{
    TraceMsg("Info: %s Entry", __FUNCTION__);

    tagParam    cTagParam = {0};
    HEARTLAND_AMOUNT_PARAM amountParam;

    InitTagParam(cTagParam, kTxnAmountData, amountParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter Failed:%d", nRet);
        return ;
    }

    amountParam.nPackSoapType = kReversal_SoapType;
    m_pParamPtr->SetParameter(cTagParam);
    

    // update TVR and CVR
    HEARTLAND_TRANS_PARAM transParam = { 0 };
    InitTagParam(cTagParam, kHLTransParam, transParam);
    nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter Failed:%d",  nRet);
        return ;
    }

    WORD    wDataBufLen = 0;
    BYTE    byTag[8] = {0};
    BYTE    byDataBuf[32] = {0}; 
    static const WORD wTagList_Reversal[] = { 0x0095, 0x9F10 };
    for (int i = 0; i < 2; i++)
    {    
        wDataBufLen = sizeof(byDataBuf);
        nRet = EMV_DataGet(wTagList_Reversal[i], &wDataBufLen, byDataBuf);
        if (nRet != d_EMVAPLIB_OK)
        {
            TraceMsg("FillReqIccData 0x%04X get failed", wTagList_Reversal[i])
            continue;
        }
        
        memset(byTag, 0x00, sizeof(byTag));
        if (wTagList_Reversal[i] > 0xFF)
        {
            sprintf((char *)byTag, "%02X", (BYTE)(wTagList_Reversal[i] >> 8));
            sprintf((char *)&byTag[2], "%02X", (BYTE)wTagList_Reversal[i]);
        }
        else
        {
            sprintf((char *)byTag, "%02X", (BYTE)wTagList_Reversal[i]);
        }
        
        nRet = CTlvFun::TLVFormatAndUpdateList(
                (char *)byTag,
                wDataBufLen,
                byDataBuf,
                &transParam.usRequestDataLen, 
                transParam.ucaRequestData);
        if(nRet != OK)
        {
            TraceMsg("FillReqIccData WARNING: Tag:%s TLVFormatAndUpdateListFromAnotherList fail", (char *)byTag);
        }
    }

    m_pParamPtr->SetParameter(cTagParam);

    CTxnContext::CommWithHost();
    CTxnContext::PostProcessing();

    // set result of the original transaction to declined
    SetState(TXN_STATE_DECLINED);

    m_pParamPtr->GetParameter(&cTagParam);
    CTlvFun::TLVFormatAndUpdateList(
                    (char *)d_TAG_TRANS_RESPONSE_TXT,
                    strlen("DECLINED"),
                    (BYTE *)"DECLINED",
                    &transParam.bResultLen,
                    &transParam.bResult[0]);
    m_pParamPtr->SetParameter(cTagParam);

    TraceMsg("Info: %s Exit", __FUNCTION__);
}


void CTxnEmv::TxnOnline(void)
{
    TraceMsg("TxnOnline entry");
    if (m_cCallUserAction.ReqUserConfirmConnWithHost())
    {
        FillReqIccData();
        CommWithHeartlandHost();
        DumpHostRsp();
        CompleteEMVTxn();
    }
    else
    {
        m_byActionCode = d_ONLINE_ACTION_UNABLE;
        TraceMsg("Info: User cancel communicate with host.");
    }
    TraceMsg("TxnOnline exit");
}


void CTxnEmv::CommWithHeartlandHost(void)
{
    TraceMsg("Info: %s Entry", __FUNCTION__);

    int         nRet = 0;
    WORD        wLen = 0;
    BYTE*       pbyData = NULL;
    static BOOL bRetry = TRUE;
    tagParam    cTagParam = {0};
    HEARTLAND_TRANS_PARAM transParam = { 0 };

    TraceMsg("Info: bRetry: %d, %X", bRetry, &bRetry);
    do
    {
        ResetHostRspData();
        nRet = CTxnContext::CommWithHost();
        if (0 != nRet)
        {
            m_byActionCode = d_ONLINE_ACTION_UNABLE;

            // defined in cURL.h
            if (nRet == 0xEC06)
            {
                memcpy(m_byAuthCode, "Z3", 2);
            }
            TraceMsg("Error: Connect heartland host failed. nRet:0x%04X", nRet);
            break;
        }

        InitTagParam(cTagParam, kHLTransParam, transParam);
        nRet = m_pParamPtr->GetParameter(&cTagParam);
        if (nRet != 0)
        {
            TraceMsg("GetParameter Failed:%d",  nRet);
            break;
        }

        nRet = CTlvFun::TLVListReadTagValue(d_TAG_TXN_RESULT, transParam.bResult, transParam.bResultLen, &pbyData, &wLen);
        if (nRet || NULL == pbyData)
        {
            TraceMsg("Invalid d_TAG_TXN_RESULT");
        }
        else if (*pbyData==d_TXN_INTERAC_RETRY_WITH_POS_SEQNBR && bRetry)
        {
            bRetry = FALSE;
            ResetInteracPosSeqNbr();
            CommWithHeartlandHost();
        }

        // Parse host response
        nRet = ParseHostResponse();
        if (0 != nRet)
        {
            m_byActionCode = d_ONLINE_ACTION_UNABLE;
            TraceMsg("Error: Parse host response failed.");
            break;
        }
        if (0 == memcmp(m_byAuthCode, "00", 2))
        {
            m_byActionCode = d_ONLINE_ACTION_APPROVAL;
        }
        else if ((memcmp(m_byAuthCode, "Z3", 2) == 0) ||
                 (memcmp(m_byAuthCode, "Y3", 2) == 0))
        {
            m_byActionCode = d_ONLINE_ACTION_UNABLE;
        }
        else
        {
            m_byActionCode = d_ONLINE_ACTION_DECLINE;
        }
    } while (0);
    TraceMsg("Info: %s Exit", __FUNCTION__);
}


void CTxnEmv::ResetHostRspData(void)
{
    bzero(m_byAuthCode, sizeof(m_byAuthCode));
    bzero(m_byIADBuf, sizeof(m_byIADBuf));
    bzero(m_byIssuerScript, sizeof(m_byIssuerScript));
    m_byActionCode = 0x00;
    m_wRecvIADLen = m_wRecvIssuerScriptLen = 0x00;
	m_wEMVCompletionResponse = 0;
}


int CTxnEmv::ParseHostResponse()
{
    HEARTLAND_TRANS_PARAM transParam = {0};
    tagParam cTagParam = {0};

    InitTagParam(cTagParam, kHLTransParam, transParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    { 
        TraceMsg("FillRspIccData GetParameter failed: 0x%04X", nRet);
        return 1;
    }
    WORD wLen = 0;
    BYTE *pbyData = NULL;
    nRet = CTlvFun::TLVListReadTagValue(d_TAG_TRANS_RESPONSE_CODE,
                                        transParam.bResult,
                                        transParam.bResultLen,
                                        &pbyData, &wLen);
    if (nRet != 0)
    {
        TraceMsg("ERROR: FillRspIccData_ActionAndAuthCode CTlvFun::TLVListReadTagValue <d_TAG_TRANS_RESPONSE_CODE> fail");
        return 1;
    }
    if (NULL != pbyData)
    {
        memcpy(m_byAuthCode, pbyData, 2);
        TraceMsg("m_byAuthCode is set");
    }
    else
    {
        TraceMsg("m_byAuthCode is not set");
        return 1;
    }
    
    
    nRet = CTlvFun::TLVListReadTagValue(d_TAG_TRANS_EMV_ISSUER_RESP, 
                                    transParam.bResult, 
                                    transParam.bResultLen, 
                                    &pbyData, &wLen);
    if (nRet == 0)
    {
        BYTE    byEMVRespInfo[512] = { 0 };
        int     nEMVRespInfoLength=0;
        char    chTmp[3] = {0};
        for (int i=0; i < wLen; i+=2)
        {
            chTmp[0] = *(pbyData + i);
            chTmp[1] = *(pbyData + i + 1);
            chTmp[2] = '\0'; 
            byEMVRespInfo[nEMVRespInfoLength++] = static_cast<BYTE>(strtol(chTmp, NULL, 16));
        }
        TraceBytes(byEMVRespInfo, nEMVRespInfoLength, "Info: EMVRespInfo=\n");
        
        FillRspIccData_ARC(byEMVRespInfo, nEMVRespInfoLength);
        FillRspIccData_AuthData(byEMVRespInfo, nEMVRespInfoLength);
        FillRspIccData_IssuerScript(byEMVRespInfo, nEMVRespInfoLength);
    }

    nRet = CTlvFun::TLVListReadTagValue(d_TAG_CARD_BRAND, 
                                    transParam.bResult, 
                                    transParam.bResultLen, 
                                    &pbyData, &wLen);
    if (0 != nRet)
    {
        return 0;
    }

    // AMEX ARC
    if (0 == memcmp(pbyData, "Amex", wLen))
    {
        if (m_wRecvIADLen > 2)
        {
            memcpy(m_byAuthCode, &m_byIADBuf[m_wRecvIADLen - 2], 2);
        }
        else if (memcmp(m_byAuthCode, "51", 2) == 0)
        {
            memcpy(m_byAuthCode, "05", 2);
        }
    }

    // DISC ARC
    if (0 == memcmp(pbyData, "Disc", wLen))
    {
        if (memcmp(m_byAuthCode, "55", 2) == 0 ||
            memcmp(m_byAuthCode, "63", 2) == 0)
        {
            memcpy(m_byAuthCode, "05", 2);
        }
    }

    return 0;
}


void CTxnEmv::DumpHostRsp(void)
{
    TraceMsg("Info: bAction=%d", m_byActionCode);
    TraceBytes(m_byAuthCode, sizeof(m_byAuthCode) - 1, "Info: ARC=");
    TraceBytes(m_byIADBuf, m_wRecvIADLen, "Info: IAD=");
    TraceBytes(m_byIssuerScript, m_wRecvIssuerScriptLen, "Info: IssueScript=");
}


void CTxnEmv::FillReqIccData(void)
{
    TraceMsg("FillReqIccData entry");

    HEARTLAND_TRANS_PARAM transParam = { 0 };
    tagParam cTagParam = { 0 };

    InitTagParam(cTagParam, kHLTransParam, transParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("FillReqIccData GetParameter failed: 0x%04X", nRet);
        return;
    }

    WORD    wRet; 
    WORD    wDataBufLen = 0;
    BYTE    byTag[8] = {0};
    BYTE    byDataBuf[1024] = {0}; 
    const int nMax = sizeof(wTagList_) / sizeof(wTagList_[0]);
    for (int i = 0; i < nMax; i++)
    {    
        wDataBufLen = sizeof(byDataBuf);
        wRet = EMV_DataGet(wTagList_[i], &wDataBufLen, byDataBuf);
        // if (0x5F34 == wTagList_[i] && d_EMVAPLIB_OK != wRet)
        // {
        //     wDataBufLen = 1;
        //     bzero(byDataBuf, sizeof(byDataBuf));
        //     byDataBuf[0] = 0xFF;
        // }
        // else 
        if (wRet != d_EMVAPLIB_OK)
        {
            TraceMsg("FillReqIccData 0x%04X get failed", wTagList_[i])
            continue;
        }
        
        memset(byTag, 0x00, sizeof(byTag));
        if (wTagList_[i] > 0xFF)
        {
            sprintf((char *)byTag, "%02X", (BYTE)(wTagList_[i] >> 8));
            sprintf((char *)&byTag[2], "%02X", (BYTE)wTagList_[i]);
        }
        else
        {
            sprintf((char *)byTag, "%02X", (BYTE)wTagList_[i]);
        }
        
        // Format Track2 data
        if (wTagList_[i] == 0x57)
        {
            BYTE byFormatStr[64] = { 0 };
            WORD wFormatStrLength = sizeof(byFormatStr);
            CStringFun::Track2ToFormatedStr(byDataBuf, wDataBufLen, byFormatStr, &wFormatStrLength);
            
            wDataBufLen = wFormatStrLength;
            memcpy(byDataBuf, byFormatStr, wDataBufLen);
        }
        
        wRet = CTlvFun::TLVFormatAndUpdateList(
                (char *)byTag,
                wDataBufLen,
                byDataBuf,
                &transParam.usRequestDataLen, 
                transParam.ucaRequestData);
        if(wRet != OK)
        {
            TraceMsg("FillReqIccData WARNING: Tag:%s TLVFormatAndUpdateListFromAnotherList fail", (char *)byTag);
        }
    }

    transParam.bARQCType = d_ICC_ARQC;
    nRet = m_pParamPtr->SetParameter(cTagParam);
    if (nRet != 0)
    {
        TraceMsg("FillReqIccData SetParameter failed: 0x%04X", nRet);
        return;
    }
    TraceMsg("FillReqIccData exit");
}


void CTxnEmv::FillRspIccData_ARC(IN BYTE *pbySrcData, IN WORD wSrcDataLength)
{
    WORD    wLen = 0;
    BYTE*   pbyData = NULL;
    int nRet = CTlvFun::TLVListReadTagValue(TAG_ARC, 
                                    pbySrcData, wSrcDataLength, 
                                    &pbyData, &wLen);
    if (nRet != 0)
    {
        TraceMsg("ERROR: FillRspIccData_ARC CTlvFun::TLVListReadTagValue <TAG_ARC> fail");
        return;
    }

    memcpy(m_byAuthCode, pbyData, wLen);
}


void CTxnEmv::FillRspIccData_AuthData(
        IN BYTE *pbySrcData, 
        IN WORD wSrcDataLength)
{
    WORD    wLen = 0;
    BYTE*   pbyData = NULL;
    int nRet = CTlvFun::TLVListReadTagValue(TAG_ISSUER_AUTH_DATA, 
                                    pbySrcData, wSrcDataLength, 
                                    &pbyData, &wLen);
    if (nRet != 0)
    {
        TraceMsg("ERROR: FillRspIccData_AuthData CTlvFun::TLVListReadTagValue <TAG_ISSUER_AUTH_DATA> fail");
        return;
    }

    m_wRecvIADLen = wLen;
    memcpy(m_byIADBuf, pbyData, wLen);
}


void CTxnEmv::FillRspIccData_IssuerScript(
        IN BYTE *pbySrcData,
        IN WORD wSrcDataLength)
{
    WORD    wLen = 0;
    BYTE*   pbyData = NULL;
    int     nRet;
    
    BYTE byTag[2] = { 0 };
    CByteStream cIssuerScript;
    
    do
    {
        byTag[0] = 0x71;
        nRet = CTlvFun::TLVListReadTagPkt(byTag, pbySrcData, wSrcDataLength, &pbyData, &wLen);
        if (nRet == 0)
        {
            cIssuerScript.Write(pbyData, wLen);
        }
        else
        {
            TraceMsg("ERROR: FillRspIccData_IssuerScript CTlvFun::TLVListReadTagValue <TAG_ISSUER_SCRIPT_1> fail");
        }
        
        byTag[0] = 0x72;
        nRet = CTlvFun::TLVListReadTagPkt(byTag, pbySrcData, wSrcDataLength, &pbyData, &wLen);
        if (nRet == 0)
        {
            cIssuerScript.Write(pbyData, wLen);
        }
        else
        {
            TraceMsg("ERROR: FillRspIccData_IssuerScript CTlvFun::TLVListReadTagValue <TAG_ISSUER_SCRIPT_2> fail");
        }
    }while (0);

    if (cIssuerScript.IsEmpty())
    {
        return ;
    }

    m_wRecvIssuerScriptLen = cIssuerScript.GetLength();
    memcpy(m_byIssuerScript, cIssuerScript.GetBuffer(), m_wRecvIssuerScriptLen);
}


void CTxnEmv::GetTxnResult(const bool bOnline)
{
    BYTE bResult = m_pEmvApi->GetTxnResult();

    TraceMsg("Info : TxnResult: %d, bOnline:%d", bResult, bOnline?1:0);

    HEARTLAND_TRANS_PARAM transParam = { 0 };
    tagParam cTagParam = { 0 };

    InitTagParam(cTagParam, kHLTransParam, transParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("FillReqIccData GetParameter failed: 0x%04X", nRet);
        return;
    }

    WORD    wLen = 0;
    BYTE*   pbyData = NULL;
    BYTE    byTxnResult = d_TXN_DECLINED;

    if (bResult == d_TXN_RESULT_APPROVAL) 
        byTxnResult = d_TXN_APPROVED;

    if (bOnline == true)    
    {
        nRet = CTlvFun::TLVListReadTagValue(d_TAG_TXN_RESULT, transParam.bResult, transParam.bResultLen, &pbyData, &wLen);
        if (nRet != 0)
        {
            TraceMsg("FillReqIccData TLVListReadTagValue failed: 0x%04X", nRet);
        }
        else if(d_TXN_APPROVED == *pbyData && d_TXN_RESULT_APPROVAL == bResult)
        {
            byTxnResult = d_TXN_APPROVED;
            TraceMsg("Info : Host APPROVAL, and EMV Kernel APPROVAL");
        }
        else if(d_TXN_APPROVED == *pbyData && d_TXN_RESULT_DECLINE == bResult)
        {
            byTxnResult = d_TXN_APPROVAL_WITH_KERNEL_DECLINE;
            TraceMsg("Info : Host APPROVAL, but EMV Kernel DECLINE");
        }
        else if(d_TXN_APPROVED == *pbyData && m_wEMVCompletionResponse != d_EMVAPLIB_OK)
        {
            byTxnResult = d_TXN_APPROVAL_WITH_KERNEL_DECLINE;
            TraceMsg("Info : Host APPROVAL, but EMV Kernel DECLINE");
        }
        else if(*pbyData == d_TXN_HOST_TIMEOUT || 
            *pbyData == d_TXN_APPROVAL_WITH_KERNEL_DECLINE ||
            *pbyData == d_TXN_INTERAC_RETRY_WITH_POS_SEQNBR)
        {
            byTxnResult = *pbyData;
            TraceMsg("Info: Transaction Failed:%x", *pbyData);
        }
        else
        {
            byTxnResult = d_TXN_DECLINED;
            TraceMsg("Info : Host DECLINED, %x", *pbyData);
        }
    }
    else if (bResult == d_TXN_RESULT_APPROVAL) 
    {
        byTxnResult = d_TXN_OFFLINE_APPROVED;
    }
    else if (bResult == d_TXN_RESULT_DECLINE) 
    {
        byTxnResult = d_TXN_OFFLINE_DECLINED;
    }
    
    CTlvFun::TLVFormatAndUpdateList(d_TAG_TXN_RESULT, 1, &byTxnResult, &transParam.bResultLen, transParam.bResult);
    nRet = m_pParamPtr->SetParameter(cTagParam);
    TraceMsg("TxnResult byTxnResult:%d, nRet:%d", byTxnResult, nRet);
    
    TraceMsg("TxnResult exit");

    return ;
}


void CTxnEmv::SetCardInformation(void)
{
    TraceMsg("Info: %s Entry", __FUNCTION__);
    
    WORD wRet = 0;
    char szTemp[64] = { 0 };
    tagParam cTagParam = { 0 };
    HEARTLAND_TRANS_PARAM transParam = { 0 };
    InitTagParam(cTagParam, kHLTransParam, transParam);

    m_pParamPtr->GetParameter(&cTagParam);

    // txnCardNum
    if (strlen(m_szAcctNbr) != 0)
    {
        memset(szTemp, 0, sizeof(szTemp));
        strcpy(szTemp, m_szAcctNbr);
        CStringFun::MaskString(szTemp, m_nFirstPlainDigits, 4, '*');
        wRet = CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_CARD_MASKED_PAN,
                strlen(szTemp),
                (BYTE *)szTemp,
                &transParam.bResultLen,
                &transParam.bResult[0]);
        if (wRet != 0)
        {
            TraceMsg("txnCardNum is abnormal to set, %s Exit", __FUNCTION__);
        }
    }
    TraceMsg("%s, txnCardNum to set: %s wRet:%d Exit", __FUNCTION__, szTemp, wRet);

    // txnCardholderName
    if (strlen(m_szCardHolderName) != 0)
    {
        wRet = CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_CARD_HOLDER_NAME,
            strlen(m_szCardHolderName),
            (BYTE *)m_szCardHolderName,
            &transParam.bResultLen,
            &transParam.bResult[0]);
        if (wRet != 0)
        {
            TraceMsg("txnCardholderName is abnormal to set, %s Exit", __FUNCTION__);
        }
    }
    TraceMsg("%s, txnCardHoldername to set: %s wRet:%d Exit", __FUNCTION__, m_szCardHolderName, wRet);
    m_pParamPtr->SetParameter(cTagParam);

    TraceMsg("Info: %s Exit", __FUNCTION__);
}

WORD CTxnEmv::CheckAPPInCard(BYTE* pbyRID)
{
    TraceMsg("Info: %s Entry", __FUNCTION__);
    
    BYTE bySAPDU[32];
    BYTE byRAPDU[512];
    BYTE bySLen;
    USHORT usRLen;        

    memset(bySAPDU, 0, sizeof(bySAPDU));
    memset(byRAPDU, 0, sizeof(byRAPDU));

    bySAPDU[0] = 0x00; 
    bySAPDU[1] = 0xA4; 
    bySAPDU[2] = 0x04; 
    bySAPDU[3] = 0x00; 
    bySAPDU[4] = 0x05;
    memcpy(&bySAPDU[5], pbyRID, 5);
    bySAPDU[10] = 0x00;

    bySLen = 11;
    usRLen = sizeof(byRAPDU);

    WORD wRet = CTOS_SCSendAPDU(d_SC_USER, bySAPDU, bySLen, byRAPDU, &usRLen);
    if(wRet == d_OK) 
    {

        if(usRLen > 2 && (byRAPDU[usRLen - 2] == 0x90 && byRAPDU[usRLen - 1] == 0x00))
        {
            TraceMsg("Info: application in card");
            return 0;    //app in card
        }
        else
        {
            TraceMsg("Info: application not in card");
            return 1;    //app not in card
        }
    }
    else
    {
        TraceMsg("Info: CTOS_SCSendAPDU fail, return code is %x", wRet);
        return 2;        //send APDU fail
    }
    
    TraceMsg("Info: %s Exit", __FUNCTION__);
}

void CTxnEmv::ReActivateEMVKernel()
{
 	BYTE baATR[64], bATRLen, CardType;
	bATRLen = sizeof(baATR);
	WORD rtn = CTOS_SCResetEMV(d_SC_USER, d_SC_5V, baATR, &bATRLen, &CardType);
	TraceMsg("  CTOS_SCResetEMV, Rtn:%04X", rtn);
	if(rtn != d_OK)
	{
		bATRLen = sizeof(baATR);
		rtn = CTOS_SCResetISO(d_SC_USER, d_SC_5V, baATR, &bATRLen, &CardType);
		TraceMsg("  CTOS_SCResetISO, Rtn:%04X", rtn);
	}
	
	if(rtn != d_OK)
	{
		TraceMsg("SC Reset Fail, Rtn:%04X", rtn);
	}   
}