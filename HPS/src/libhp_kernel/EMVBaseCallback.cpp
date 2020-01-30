
/*
 * File Name: EMVBaseCallback.cpp
 * Author: Alan.Ren
 * 
 * Create Date: 2019.3.12
 */

#include "ByteStream.h"

#include "Inc/DefParamTypeId.h"
#include "EMVBaseCallback.h"
#include "TxnDef.h"
#include "TxnTag.h"
#include "Inc/TxnState.h"
#include "TlvFun.h"
#include "StringFun.h"
#include "dbghlp.h"
#include "TxnJsonKey.h"
#include "FunBase.h"
#include "TxnBase.h"
#include "Inc/TxnDataType.h"
#include "Inc/ErrorCode.h"
#include "Inc/UserActFlags.h"
#include "cJSON.h"

#ifdef PLATFORM_ANDROID
#include <kmshelper.h>
#endif


#define HEARTLAND_KERNEL_EMVCALLBACK_VERSION        0x01


CEMVBaseCallback::CEMVBaseCallback(
    IN IParameter *pParamPtr,
    IN IPinAction *pPinAction,
    IN IUserAction *pUserAction) : m_pParamPtr(pParamPtr),
                                   m_pPinAction(pPinAction),
                                   m_cCallUserAction(pUserAction, pParamPtr)
{ }

CEMVBaseCallback::~CEMVBaseCallback()
{ }


BYTE CEMVBaseCallback::GetVersion(void)
{
    return HEARTLAND_KERNEL_EMVCALLBACK_VERSION;
}


void CEMVBaseCallback::DisplayShow(IN char *pszMsg)
{
    TraceMsg("Info: DisplayShow, Msg=%s", pszMsg);
}


void CEMVBaseCallback::ErrorMsg(IN char *pszErrMsg)
{
    TraceMsg("Error: ErrorMsg, Msg=%s", pszErrMsg);
}


void CEMVBaseCallback::EMVConfigActive(INOUT BYTE *pbyActiveIndex)
{
    TraceMsg("Error: EMVConfigActive, Index=%d", (*pbyActiveIndex));
}

BOOL CEMVBaseCallback::HashVerify(
    IN BYTE *pbyRID, IN BYTE byKeyIndex,
    IN BYTE *pbyModule, IN WORD wModuleLen,
    IN BYTE *pbyExponent, IN WORD wExponentLen,
    IN BYTE *pbyHash, IN WORD wHashLen)
{
    TraceMsg("HashVerify entry");
    TraceMsg("HashVerify exit");
    return FALSE;
}

WORD CEMVBaseCallback::TxnDataGet(OUT EMV_TXNDATA *pcTxnData)
{
    TraceMsg("TxnDataGet entry");
    CTOS_RTC stRTC = {0};
    WORD wErr = d_OK;
    tagParam    cTagParam = {0};
    HEARTLAND_AMOUNT_PARAM amountParam;
    
    bzero(&amountParam, sizeof(amountParam));
    InitTagParam(cTagParam, kTxnAmountData, amountParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter Failed:%d", nRet);
        return ERROR_CODE_GETPARAM_FAIL;
    }
    
    wErr = CTOS_RTCGet(&stRTC);
    if (d_OK != wErr)
    {
        TraceMsg("CTOS_RTCGet failed: 0x%04X", wErr);
        return wErr;
    }

    EMV_TXNDATA_2* pcTxnData2 = (EMV_TXNDATA_2 *)pcTxnData;
    sprintf((char *)pcTxnData2->TxnDate, "%02d%02d%02d",
            stRTC.bYear,
            stRTC.bMonth,
            stRTC.bDay);
    sprintf((char *)pcTxnData2->TxnTime, "%02d%02d%02d",
            stRTC.bHour,
            stRTC.bMinute,
            stRTC.bSecond);
    pcTxnData2->Version = 0x02;
    pcTxnData2->bTxnType = 0x00;
    pcTxnData2->ulAmount = amountParam.ullAmount;
    pcTxnData2->ulAmountOther = amountParam.ullOtherAmount;
    pcTxnData2->isForceOnline = FALSE;
    pcTxnData2->bPOSEntryMode = 0x05;
    
    TraceMsg("Amt:%ld, OtherAmt:%ld", pcTxnData2->ulAmount, pcTxnData2->ulAmountOther);
    TraceMsg("TxnDataGet exit");
    return d_EMVAPLIB_OK;
}

WORD CEMVBaseCallback::AppList(IN BYTE byAppNum,
                               IN char szAppLabel[][d_LABEL_STR_SIZE + 1],
                               OUT BYTE *pbySelIndex)
{
    TraceMsg("AppList--Entry");
    const WORD wErr = m_cCallUserAction.ReqUserSelectAID(
        byAppNum,
        szAppLabel,
        pbySelIndex);
    TraceMsg("AppList--Exit, ReturnCode=0x%04X", wErr);
    return wErr;
}

WORD CEMVBaseCallback::AppSelectedConfirm(IN BOOL bRequiredByCard,
                                          IN BYTE *pbyLabel,
                                          IN BYTE byLabelLen)
{
#ifdef PLATFORM_ANDROID
    return d_EMVAPLIB_OK;
#endif

//#define TEST_AIDLIST
#ifndef TEST_AIDLIST
    TraceMsg("AppSelectedConfirm entry");
    TraceMsg("AppSelectedConfirm AID:%s", pbyLabel);
    TraceBytes(pbyLabel, byLabelLen, "AppSelectedConfirm AID:");
    const WORD wErr = m_cCallUserAction.ReqUserConfirmSelectedAID(pbyLabel,
                                                             byLabelLen);
    TraceMsg("AppSelectedConfirm exit, Ret=0x%04X", wErr);
#else
    TraceMsg("AppList--Entry");
    BYTE byAppNum = 5;
    BYTE bySelIndex = 0;
    char (*pszAppLabel)[d_LABEL_STR_SIZE + 1] = new char[byAppNum][d_LABEL_STR_SIZE + 1];
    memset(pszAppLabel, 0, sizeof(char) * (d_LABEL_STR_SIZE + 1) * byAppNum);

    for (int i=0; i<byAppNum; ++i)
    {
        sprintf(pszAppLabel[i], "AID Test %03d", i);
    }
    const WORD wErr = m_cCallUserAction.ReqUserSelectAID(
        byAppNum,
        pszAppLabel,
        &bySelIndex);
    TraceMsg("AppList--Exit, ReturnCode=0x%04X", wErr);
#endif

    return wErr;
}

BOOL CEMVBaseCallback::TerminalDataGet(IN WORD wTag,
                                       INOUT WORD *pwValueLen,
                                       OUT BYTE *pbyValue)
{
    TraceMsg("TerminalDataGet entry");
    TraceMsg("TerminalDataGet exit");
    return FALSE;
}

BOOL CEMVBaseCallback::CAPKGet(IN BYTE *pRID,
                               IN BYTE bKeyIndex,
                               OUT BYTE *pModulus,
                               OUT WORD *pModulusLen,
                               OUT BYTE *pExponent,
                               OUT WORD *pExponentLen)
{
    TraceMsg("CAPKGet entry");
    TraceMsg("RID:%s, KeyIndex:%d", pRID, bKeyIndex);
    TraceBytes(pRID, 10, "RID:");
    TraceMsg("CAPKGet exit");
    return FALSE;
}

void CEMVBaseCallback::GetPINNotify(IN BYTE bPINType,
                                    IN WORD bRemainingCounter,
                                    OUT BOOL *pIsInternalPINPad,
                                    OUT DEFAULT_GETPIN_FUNC_PARA *pGetPINFuncPara)
{
    TraceMsg("GetPINNotify entry");
    TraceMsg("PIN Type:%s, RemainingCounter:%d", 
                bPINType == d_NOTIFY_ONLINE_PIN ? "Online PIN" : "Offline PIN",
                bRemainingCounter);
    
    // Last ping input
    if (1 == bRemainingCounter && bPINType == d_NOTIFY_OFFLINE_PIN)
    {
        m_cCallUserAction.ReqUserPromptLastPINInput();
    }

    tagParam cTagParam = { 0 };
    HEARTLAND_PINENTRY_PARAM pinParam = { 0 };

    InitTagParam(cTagParam, kPinParam, pinParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("CTOS_RTCGet failed: 0x%04X", nRet);
        return;
    }
    
    memset(pGetPINFuncPara, 0, sizeof(DEFAULT_GETPIN_FUNC_PARA));
    pGetPINFuncPara->Version = 0x00;
    pGetPINFuncPara->usLineLeft_X = pinParam.cKms2PinObj.Control.AsteriskPositionX;
    pGetPINFuncPara->usLineRight_X = pinParam.cKms2PinObj.Control.AsteriskPositionX + pinParam.cKms2PinObj.PIN_Info.PINDigitMaxLength;
    pGetPINFuncPara->usLinePosition_Y = pinParam.cKms2PinObj.Control.AsteriskPositionY;
    
    pGetPINFuncPara->ulTimeout = pinParam.cKms2PinObj.Control.Timeout;
    pGetPINFuncPara->bPINDigitMaxLength = pinParam.cKms2PinObj.PIN_Info.PINDigitMaxLength;
    pGetPINFuncPara->bPINDigitMinLength = pinParam.cKms2PinObj.PIN_Info.PINDigitMinLength;
    pGetPINFuncPara->IsReverseLine = FALSE;
    pGetPINFuncPara->IsRightToLeft = FALSE;

    if (bPINType == d_NOTIFY_ONLINE_PIN)
    {
        *pIsInternalPINPad = FALSE;
        
        pGetPINFuncPara->ONLINEPIN_PARA.CipherKeyIndex = pinParam.cKms2PinObj.Protection.CipherKeyIndex;
        pGetPINFuncPara->ONLINEPIN_PARA.CipherKeySet = pinParam.cKms2PinObj.Protection.CipherKeySet;
        pGetPINFuncPara->ONLINEPIN_PARA.bPANLen = pinParam.byPanLength;

        memcpy(pGetPINFuncPara->ONLINEPIN_PARA.baPAN, pinParam.byPanData, pinParam.byPanLength);
    }
    else if(bPINType == d_NOTIFY_OFFLINE_PIN)
    {
        *pIsInternalPINPad = TRUE;
#ifdef PLATFORM_ANDROID
        CTOS_LCDTClearDisplay();
#endif
        CTxnBase::SetState(TXN_STATE_PIN_ENTER_START);

#ifdef PLATFORM_ANDROID
        CTJNI_KMS_EnableLCD();
#endif
        // Avoid brushing off KMS interfaces due to delays
        usleep(500 * 1000);
    }
    TraceMsg("GetPINNotify exit");
}

WORD CEMVBaseCallback::OnlinePINBlockGet(OUT ONLINE_PIN_DATA *pOnlinePINData)
{
    TraceMsg("OnlinePINBlockGet entry");

    int nRet = 1;
    HEARTLAND_TRANS_PARAM transParam = { 0 };
    tagParam cTagParam = { 0 };

    static BYTE byKSNString[64];
    static BYTE chKSNStringSize;

    static BYTE byPinBlock[64];
    static WORD wPinBlockSize;

    TraceMsg("Info: Need to get online PIN");
#ifdef PLATFORM_ANDROID
	CTOS_LCDTClearDisplay();
#endif

    CTxnBase::SetState(TXN_STATE_PIN_ENTER_START);
    // m_cCallUserAction.ReqUserInputEMVPIN(TRUE);

#ifdef PLATFORM_ANDROID
    CTJNI_KMS_EnableLCD();
#endif
    
    // Avoid brushing off KMS interfaces due to delays
    usleep(500 * 1000);
    
    do
    {
        chKSNStringSize = sizeof(byKSNString);
        memset(byKSNString, 0, chKSNStringSize);
        nRet = m_pPinAction->GetKSNString(byKSNString, &chKSNStringSize);
        if(nRet != 0)
        {
           TraceMsg("OnlinePINBlockGet GetKSNString fail! wRet is:0x%04X\n",nRet);
           break;
        }
    
        wPinBlockSize = sizeof(byPinBlock);
        memset(byPinBlock, 0, wPinBlockSize);
        pOnlinePINData->isOnlinePINRequired = TRUE;
        pOnlinePINData->bPINLen = 0;
        pOnlinePINData->pPIN = byPinBlock;

        nRet = m_pPinAction->GetPinBlock(byPinBlock, &wPinBlockSize);
        if(nRet == d_KMS2_GET_PIN_NULL_PIN)
        {
           TraceMsg("OnlinePINBlockGet: d_KMS2_GET_PIN_NULL_PIN! wRet is:0x%04X\n",nRet);
           nRet = d_EMVAPLIB_ERR_PIN_BY_PASS;
           break;
        }
        else if(nRet != 0)
        {
           TraceMsg("OnlinePINBlockGet GetPinBlock fail! wRet is:0x%04X\n",nRet);
           break;
        }
        pOnlinePINData->bPINLen = wPinBlockSize;
        pOnlinePINData->pPIN = byPinBlock;


        InitTagParam(cTagParam, kHLTransParam, transParam);
        nRet = m_pParamPtr->GetParameter(&cTagParam);
        if (nRet != 0)
        {
           TraceMsg("OnlinePINBlockGet GetParameter fail! wRet is:0x%04X\n",nRet);
           break;
        }

        nRet = CTlvFun::TLVFormatAndUpdateList(d_TAG_EPB, 
                pOnlinePINData->bPINLen, 
                pOnlinePINData->pPIN, 
                &transParam.usRequestDataLen, 
                &transParam.ucaRequestData[0]);
        if(nRet != 0)
        {
           TraceMsg("OnlinePINBlockGet TLVFormatAndUpdateList d_TAG_EPB fail! wRet is:0x%04X\n",nRet);
           break;
        }

        nRet = CTlvFun::TLVFormatAndUpdateList(d_TAG_EPB_KSN, 
                chKSNStringSize, 
                byKSNString, 
                &transParam.usRequestDataLen, 
                &transParam.ucaRequestData[0]);
        if(nRet != 0)
        {
           TraceMsg("OnlinePINBlockGet TLVFormatAndUpdateList d_TAG_EPB_KSN fail! wRet is:0x%04X\n",nRet);
           break;
        }

        nRet = m_pParamPtr->SetParameter(cTagParam);
        if (nRet != 0)
        {
           TraceMsg("OnlinePINBlockGet SetParameter fail! wRet is:0x%04X\n",nRet);
           break;
        }
    }while(0);

    // m_cCallUserAction.ReqUserInputEMVPIN(FALSE);
#ifdef PLATFORM_ANDROID
    CTOS_LCDTClearDisplay();
    CTJNI_KMS_DisableLCD();
#endif

    CTxnBase::SetState(TXN_STATE_PIN_ENTER_FINISH);
    usleep(500 * 1000);
    
    TraceMsg("OnlinePINBlockGet wRet : 0x%04X", nRet);
    return nRet;
}

WORD CEMVBaseCallback::OfflinePINBlockGet(void)
{    
    TraceMsg("OfflinePINBlockGet entry");
    return d_EMVAPLIB_OK;
}


void CEMVBaseCallback::OfflinePINVerifyResult(IN WORD wResult)
{
    TraceMsg("OfflinePINVerifyResult entry");
    // m_cCallUserAction.ReqUserInputEMVPIN(FALSE);
    CTxnBase::SetState(TXN_STATE_PIN_ENTER_FINISH);
    usleep(500 * 1000);
    TraceMsg("OfflinePINVerifyResult exit");
}

/*
 * Protected.
 */
void CEMVBaseCallback::TxnOnline(IN ONLINE_PIN_DATA *pOnlinePINData,
                                 OUT EMV_ONLINE_RESPONSE_DATA *pOnlineResponseData)
{
    TraceMsg("TxnOnline entry");
    TraceMsg("TxnOnline exit");
}

void CEMVBaseCallback::TxnIssueScriptResult(IN BYTE *pScriptResult,
        IN WORD pScriptResultLen)
{
    TraceMsg("TxnIssueScriptResult entry");
    TraceMsg("TxnIssueScriptResult exit");
}

/*
 * Protected.
 */
void CEMVBaseCallback::TxnResult(IN BYTE bResult,
                                 IN BOOL IsSignatureRequired)
{
    TraceMsg("TxnResult entry");
    TraceMsg("Info: TxnResult:%d, IsSignatureRequired:%d", bResult,
             IsSignatureRequired);
    if (IsSignatureRequired)
    {
        m_cCallUserAction.ReqUserPrintSignature();
    }
    TraceMsg("TxnResult exit");
}


void CEMVBaseCallback::TotalAmountGet(IN BYTE *pPAN,
                                      IN BYTE bPANLen,
                                      OUT ULONG *pAmount)
{
    TraceMsg("TotalAmountGet--Entry");
    m_cCallUserAction.ReqUserOutTotalAmt(pPAN, bPANLen, (unsigned long*)pAmount);
    TraceMsg("TotalAmountGet--Exit");
}

void CEMVBaseCallback::ExptionFileCheck(IN BYTE *pPAN,
                                        IN BYTE bPANLen,
                                        OUT BOOL *isException)
{
    TraceMsg("ExptionFileCheck entry");
    m_cCallUserAction.ReqUserChkExpFile(pPAN, bPANLen, isException);
    TraceMsg("ExptionFileCheck exit");
}

BOOL CEMVBaseCallback::CAPKRevocationChk(IN BYTE *pbRID,
                                         IN BYTE bCAPKIdx,
                                         IN BYTE *pbSerialNumuber)
{
    TraceMsg("CAPKRevocationChk entry");
    TraceMsg("CAPKRevocationChk exit");
    return FALSE;
}


unsigned short CEMVBaseCallback::OnAppListEx(IN BYTE bAppNum,
                                    IN EMV_APP_LIST_EX_DATA *pstAppListExData,
                                    OUT BYTE *pbAppSelectedIndex)
{
    TraceMsg("OnAppListEx entry");

    char (*pszAppLabel)[d_LABEL_STR_SIZE + 1] = new char[bAppNum][d_LABEL_STR_SIZE + 1];
    memset(pszAppLabel, 0, sizeof(char) * bAppNum * d_LABEL_STR_SIZE + 1);

    for(BYTE i=0; i<bAppNum; ++i)
    {
        strcpy(pszAppLabel[i], pstAppListExData[i].cAppLabel);
        TraceMsg("AppLabel[%d], Name[%s]", i, pstAppListExData[i].cAppLabel);
    }

    const WORD wErr = m_cCallUserAction.ReqUserSelectAID(
        bAppNum,
        pszAppLabel,
        pbAppSelectedIndex);

    delete [] pszAppLabel;

    TraceMsg("OnAppListEx wErr:%d exit", wErr);
    return wErr;
}