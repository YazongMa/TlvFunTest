
/*
 * File Name: TxnContext.cpp
 * Author: Tim.Ma
 * 
 * Create Date: 2019.5.28
 */

#include "dbghlp.h"
#include "TxnDef.h"
#include "TxnTag.h"
#include "TlvFun.h"
#include "FunBase.h"
#include "StringFun.h"
#include "TxnContext.h"

#include "Inc/TxnState.h"
#include "Inc/TxnDataType.h"

#include <sys/time.h>

typedef struct
{
    const WORD      wTagBuf_;
    const char*     pszTagBuf_;
    const bool      bNeedToString_;
} PrintTagList;

static const PrintTagList printTagList_[] = 
{
    {0x9F12, TAG_APP_PREFER_NAME,   false},
    {0x0050, TAG_APP_PREFER_NAME,   false},
    {0x9F27, TAG_CID,               true},
    {0x009B, d_TAG_TRANS_EMV_TSI,   true},
    {0x0095, d_TAG_TRANS_EMV_TVR,   true},
    {0x008A, TAG_ARC,               true},
    {0x0084, TAG_FD_NAME,           true},
    {0x004F, TAG_FD_NAME,           true},
    {0x9F06, TAG_FD_NAME,           true},
};

static const size_t nTagBufCount_ = sizeof(printTagList_) / sizeof(printTagList_[0]);


static const PrintTagList printTagAid_[] = 
{
    {0x004F, TAG_ADF,               true},
    {0x0084, TAG_FD_NAME,           true},
    {0x9F06, TAG_APP_ID,            true},
};

static const size_t nTagAidCount_ = sizeof(printTagAid_) / sizeof(printTagAid_[0]);


static int      DemoModeProcess(IParameter* pParamPtr);
static char*    GenerateRandomStr(int nLength);
static void     GenerateRandomResponse(HEARTLAND_TRANS_PARAM* pcTransParam, int nPackSoapType);

/**
* Function:
*      CTxnContext's Construct Function:
*
* @param
*      pszPanEntryMode  : card type of current transaction, such as TAP, INSERT, SWIPE etc.
*      pParamPtr*       : the parameters of the pool
*      pDataPackPtr*    : point of the object to pack and unpack
*      IBaseCurl*       : point of the object to communicate with host
*      IPinAction*      : point of the object to get pin or key sn
*      IUserAction*     : point of the object to chat with user
*
* @return
*      Void.
*
* Author: Tim.Ma
*/
CTxnContext::CTxnContext(IN const char* pszPanEntryMode,
        IN IParameter *pParamPtr, 
        IN IDataPack *pDataPackPtr,
        IN IBaseCurl *pBaseCurlPtr,
        IN IPinAction *pPinEntryPtr,
        IN IUserAction *pUserAction)
:
m_cPinParamFlag(0),
m_pszPanEntryMode(pszPanEntryMode),
m_nPackSoapType(kInvalidType),
m_nFirstPlainDigits(4),
m_pParamPtr(pParamPtr),
m_pPinEntryPtr(pPinEntryPtr),
m_pDataPackPtr(pDataPackPtr),
m_pBaseCurlPtr(pBaseCurlPtr),
//m_cTxnProcess(pDataPackPtr, pBaseCurlPtr),
m_cCallUserAction(pUserAction, pParamPtr)
{
    memset(m_szAcctNbr, 0, sizeof(m_szAcctNbr));
    memset(m_szTransAmount, 0, sizeof(m_szTransAmount));
    memset(m_szCardHolderName, 0, sizeof(m_szCardHolderName));
    m_SAFPackage.Empty();
    m_bNeedCheckSAF = FALSE;
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
CTxnContext::~CTxnContext()
{
    
}


WORD CTxnContext::InitParam(void)
{
    // Set IDLE state
    SetState(TXN_STATE_IDLE);

    TraceMsg("InitParam entry");

    m_nPackSoapType = kInvalidType;
    memset(m_szAcctNbr, 0, sizeof(m_szAcctNbr));
    memset(m_szTransAmount, 0, sizeof(m_szTransAmount));
    memset(m_szTransAmountOther, 0, sizeof(m_szTransAmountOther));
    memset(m_szCardHolderName, 0, sizeof(m_szCardHolderName));
    memset(m_szCardExpireDate, 0, sizeof(m_szCardExpireDate));

    tagParam cTagParam = {0};
    HEARTLAND_TRANS_PARAM transParam = { 0 };

    InitTagParam(cTagParam, kHLTransParam, transParam);
    int nRet = m_pParamPtr->SetParameter(cTagParam);
    
    TraceMsg("INFO: SetParameter <kHLTransParam> %s", nRet==0?"successful":"fail");

    HEARTLAND_GENERAL_PARAM cGeneralParam = { 0 };

    InitTagParam(cTagParam, kGeneralParam, cGeneralParam);
    m_pParamPtr->GetParameter(&cTagParam);

    m_nFirstPlainDigits = 4;
    if (strlen(cGeneralParam.szMaskedAcctMode))
    {
        m_nFirstPlainDigits = atoi(cGeneralParam.szMaskedAcctMode);
    }
    TraceMsg("INFO: First Plain Digits for account: %d", m_nFirstPlainDigits);

    m_cPinParamFlag = 0;
    TraceMsg("InitParam exit");
    return 0;
}


WORD CTxnContext::GetAcctNbr(unsigned char* pszTrack2, const int nTrack2Len)
{
    TraceMsg("GetAcctNbr entry");

    memset(m_szAcctNbr, 0, sizeof(m_szAcctNbr));
    if (!pszTrack2)
    {
        TraceMsg("ERROR: pszTrack2 is NULL");
        return 1;
    }
    
    for (int i=1, j=0; i<nTrack2Len; ++i)
    {
        if (pszTrack2[i] == 0x3D)
        {
            break;
        }
        
        m_szAcctNbr[j++] = pszTrack2[i];
    }

    TraceMsg("GetAcctNbr exit");
    return 0;
}


WORD CTxnContext::SetPinParam(void)
{
    TraceMsg("INFO: SetPinParam entry");

    tagParam cTagParam = {0};
    HEARTLAND_PINENTRY_PARAM pinParam = { 0 };

    InitTagParam(cTagParam, kPinParam, pinParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("ERROR: GetParameter <pinParam> failed wRet:%d", nRet);
        return 1;
    }
    
    pinParam.byPanLength = strlen(m_szAcctNbr);
    memset(pinParam.byPanData, 0, sizeof(pinParam.byPanData));
    memcpy(pinParam.byPanData, m_szAcctNbr, pinParam.byPanLength);

    //TraceMsg("INFO: PAN:%s, Length:%d", pinParam.byPanData, pinParam.byPanLength);
    
    nRet = m_pParamPtr->SetParameter(cTagParam);
    if (nRet != 0)
    {
        TraceMsg("ERROR: SetParameter <pinParam> failed wRet:%d", nRet);
        return 1;
    }

    m_cPinParamFlag = 1;

    TraceMsg("INFO: SetPinParam exit");
    return 0;
}


WORD CTxnContext::GetPinBolck(void)
{
    TraceMsg("INFO: GetPinBolck entry");

    CTxnContext::SetPinParam();
    if (NULL == m_pPinEntryPtr || 0 == m_cPinParamFlag)
    {
        TraceMsg("ERROR: m_pPinEntryPtr:%p, m_cPinParamFlag:%d Invalid", m_pPinEntryPtr, m_cPinParamFlag);
        return 1;
    }
    
    BYTE byKeySN[64] = { 0 };
    BYTE byKeySNSize = sizeof (byKeySN);
    WORD wRet = m_pPinEntryPtr->GetKSNString(byKeySN, &byKeySNSize);
    if (wRet != 0)
    {
        TraceMsg("ERROR: GetKSNString failed wRet:%d", wRet);
        return 1;
    }
    TraceBytes(byKeySN, byKeySNSize, "KeySN:");
    
    BYTE byPinBlock[16] = {0};
    WORD wPinBlockSize = sizeof (byPinBlock);

    wRet = m_pPinEntryPtr->GetPinBlock(byPinBlock, &wPinBlockSize);
    if (wRet != 0)
    {
        TraceMsg("ERROR: GetPinBlock failed wRet:%d", wRet);
        return 1;
    }
    TraceBytes(byPinBlock, wPinBlockSize, "PinBlock:");

    tagParam cTagParam = {0};
    HEARTLAND_TRANS_PARAM transParam = {0};

    InitTagParam(cTagParam, kHLTransParam, transParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("ERROR: GetParameter <transParam> failed wRet:%d", wRet);
        return 1;
    }

    wRet = CTlvFun::TLVFormatAndUpdateList(d_TAG_EPB,
            wPinBlockSize,
            byPinBlock,
            &transParam.usRequestDataLen,
            &transParam.ucaRequestData[0]);
    if (wRet != 0)
    {
        TraceMsg("ERROR: CTlvFun::TLVFormatAndUpdateList <d_TAG_EPB> failed wRet:%d", wRet);
        return 1;
    }

    wRet = CTlvFun::TLVFormatAndUpdateList(d_TAG_EPB_KSN,
            byKeySNSize,
            byKeySN,
            &transParam.usRequestDataLen,
            &transParam.ucaRequestData[0]);
    if (wRet != 0)
    {
        TraceMsg("ERROR: CTlvFun::TLVFormatAndUpdateList <d_TAG_EPB_KSN> failed wRet:%d", wRet);
        return 1;
    }
    
    nRet = m_pParamPtr->SetParameter(cTagParam);
    if (nRet != 0)
    {
        TraceMsg("ERROR: SetParameter <transParam> failed wRet:%d", wRet);
        return 1;
    }

    TraceMsg("INFO: GetPinBolck exit");
    return 0;    
}



WORD CTxnContext::PreProcessing(void)
{
    TraceMsg("Info: PreProcessing entry");

    int         nMinLength = 0;
    //BYTE        byDataBuf[6] = { 0 };
    CByteStream cOutData;
    BYTE        byTempAmt[20] = { 0 };
    tagParam    cTagParam = {0};

    HEARTLAND_AMOUNT_PARAM amountParam;
    HEARTLAND_TRANS_PARAM transParam = { 0 };
    
    bzero(&amountParam, sizeof(amountParam));
    InitTagParam(cTagParam, kTxnAmountData, amountParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("ERROR: GetParameter Failed:%d", nRet);
        return 1;
    }

    InitTagParam(cTagParam, kHLTransParam, transParam);
    nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("ERROR: GetParameter Failed:%d", nRet);
        return 1;
    }

    // soap type
    m_nPackSoapType = amountParam.nPackSoapType;
    if (m_nPackSoapType == kCreditReturn_SoapType ||
        m_nPackSoapType == kCreditSale_SoapType ||
        m_nPackSoapType == kCreditVoid_SoapType ||
        m_nPackSoapType == kPreAuth_SoapType ||
        m_nPackSoapType == kAuthComplete_SoapType ||
        m_nPackSoapType == kCreditPollingCard_TxnType ||
        m_nPackSoapType == kManualCreditReversal_SoapType)
    {
        transParam.bCardType = d_CREDIT_CARD_TYPE;
    }
    else if(m_nPackSoapType == kDebitSale_SoapType ||
        m_nPackSoapType == kDebitReturn_SoapType ||
        m_nPackSoapType == kDebitPollingCard_TxnType ||
        m_nPackSoapType == kManualDebitReversal_SoapType)
    {
        transParam.bCardType = d_DEBIT_CARD_TYPE;
    }

    // amount
    TraceMsg("INFO: GetParameter amountParam.ullAmount:%lld", amountParam.ullAmount);
    if (amountParam.ullAmount > 0)
    {
        cOutData.Empty();
        memset(byTempAmt, 0, sizeof(byTempAmt));
        sprintf((char *)byTempAmt, "%012lld", amountParam.ullAmount);
        CFunBase::Str2Hex((char *)byTempAmt, cOutData);
        nRet = CTlvFun::TLVFormatAndUpdateList(
                TAG_AMT_AUTH,
                6,
                cOutData.GetBuffer(),
                &transParam.usRequestDataLen,
                &transParam.ucaRequestData[0]);
        if (nRet)
        {
            TraceMsg("ERROR: TLV_FormatAndUpdateList <TAG_AMT_AUTH> Failed");
            return 1;
        }
        memset(m_szTransAmount, 0, sizeof(m_szTransAmount));
        sprintf(m_szTransAmount, "%lld.%02lld", amountParam.ullAmount/100, amountParam.ullAmount%100);
    }

    // other amount
    TraceMsg("INFO: GetParameter amountParam.ullOtherAmount:%lld", amountParam.ullOtherAmount);
    if (amountParam.ullOtherAmount > 0)
    {
        cOutData.Empty();
        memset(byTempAmt, 0, sizeof(byTempAmt));
        sprintf((char *)byTempAmt, "%012lld", amountParam.ullOtherAmount);
        CFunBase::Str2Hex((char *)byTempAmt, cOutData);
        nRet = CTlvFun::TLVFormatAndUpdateList(
                TAG_AMT_OTHER,
                6,
                cOutData.GetBuffer(),
                &transParam.usRequestDataLen,
                &transParam.ucaRequestData[0]);
        if (nRet)
        {
            TraceMsg("ERROR: TLV_FormatAndUpdateList <TAG_AMT_OTHER> Failed");
            return 1;
        }
        memset(m_szTransAmountOther, 0, sizeof(m_szTransAmountOther));
        sprintf(m_szTransAmountOther, "%lld.%02lld", amountParam.ullOtherAmount/100, amountParam.ullOtherAmount%100);
    }

    // gatewayid
    if (strlen(amountParam.szGatewayIdBuf) > 0)
    {
        nRet = CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANSACTION_IDENTITY,
                    strlen(amountParam.szGatewayIdBuf),
                    (BYTE *)amountParam.szGatewayIdBuf,
                    &transParam.usRequestDataLen,
                    &transParam.ucaRequestData[0]);
        if (nRet != 0)
        {
            TraceMsg("ERROR: TLV_FormatAndUpdateList <d_TAG_TRANSACTION_IDENTITY> Failed");
            return 1;
        }
    }

    // token value
    if (strlen(amountParam.szTokenValueBuf) > 0)
    {
        nRet = CTlvFun::TLVFormatAndUpdateList(
                d_TAG_TRANS_TOKEN_VALUE,
                strlen(amountParam.szTokenValueBuf),
                (unsigned char*)amountParam.szTokenValueBuf,
                &transParam.usRequestDataLen,
                &transParam.ucaRequestData[0]);
        if (nRet)
        {
            TraceMsg("TLV_FormatAndUpdateList <d_TAG_TRANS_TOKEN_VALUE> fail");
        }
    }

    // allow partial auth
    nRet = CTlvFun::TLVFormatAndUpdateList(
            d_TAG_TRANS_ALLOW_PARTIAL_AUTH,
            1,
            amountParam.byPartialAuthFlag==TRUE ? (unsigned char*)"1" : (unsigned char*)"0",
            &transParam.usRequestDataLen,
            &transParam.ucaRequestData[0]);
    if (nRet)
    {
        TraceMsg("TLV_FormatAndUpdateList <d_TAG_TRANS_ALLOW_PARTIAL_AUTH> fail");
    }
    

    TraceMsg("INFO: CardType:%d, TxnType:%d", transParam.bCardType, amountParam.nPackSoapType);
    nRet = m_pParamPtr->SetParameter(cTagParam);
    if (nRet != 0)
    {
        TraceMsg("ERROR: SetParameter Failed:%d", nRet);
        return nRet;
    }
    
    TraceMsg("Info: PreProcessing exit");
    return 0;
}


WORD CTxnContext::PostProcessing(void)
{
    TraceMsg("Info: PostProcessing entry");

    tagParam cTagParam = {0};
    HEARTLAND_TRANS_PARAM transParam = {0};

    InitTagParam(cTagParam, kHLTransParam, transParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter <transParam> failed wRet:%d", nRet);
        return 1;
    }

    WORD wLen = 0;
    BYTE* pbyData = NULL;

    // Amount
    TraceMsg("Info: Amount");
    CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_AMOUNT,
            strlen(m_szTransAmount),
            (unsigned char*)m_szTransAmount,
            &transParam.bResultLen,
            &transParam.bResult[0]);

#if 0
    // Card Entry Mode: SWIPE/INSERT/TAP
    TraceMsg("Info: Entry Mode");
    CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_CARD_ENTRY_MODE,
            strlen(m_pszPanEntryMode),
            (unsigned char*)m_pszPanEntryMode,
            &transParam.bResultLen,
            &transParam.bResult[0]);
#endif

    // Entry Legend
    TraceMsg("Info: Entry Legend");
    CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_CARD_ENTRY_LEGEND,
            strlen(m_pszPanEntryLegend),
            (unsigned char*)m_pszPanEntryLegend,
            &transParam.bResultLen,
            &transParam.bResult[0]);

    // DT
    TraceMsg("Info: DT");
    nRet = CTlvFun::TLVListReadTagValue(d_TAG_TRANS_RESP_DT, transParam.bResult, transParam.bResultLen, &pbyData, &wLen);
    if (nRet == 0)
    {
        while(*pbyData!='T')
            *pbyData++;
        *pbyData = ' ';

        char* pvRet = (char *)memrchr(pbyData, '.', wLen);
        *pvRet = '\0';
    }
    else
    {
        CTOS_RTC stRTC = {0};
        CTOS_RTCGet(&stRTC);
        char szTxnDate[20] = { 0 };
        sprintf(szTxnDate, "%04d-%02d-%02d %02d:%02d:%02d",
                stRTC.bYear + 2000,
                stRTC.bMonth,
                stRTC.bDay,
                stRTC.bHour,
                stRTC.bMinute,
                stRTC.bSecond);
        CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_RESP_DT,
                strlen(szTxnDate),
                (unsigned char*)szTxnDate,
                &transParam.bResultLen,
                &transParam.bResult[0]);
    }
    
    // CardType
    TraceMsg("Info: Card Type");
    nRet = CTlvFun::TLVListReadTagValue(d_TAG_TXN_CARD_TYPE, transParam.bResult, transParam.bResultLen, &pbyData, &wLen);
    if (nRet != 0 && IsInteracAID(transParam.bARQCType))
    {
        TraceMsg("WARNING: Get Card Type failed");
        CTlvFun::TLVFormatAndUpdateList(d_TAG_TXN_CARD_TYPE,
                strlen("DEBIT CARD"),
                (unsigned char*)"DEBIT CARD",
                &transParam.bResultLen,
                &transParam.bResult[0]);
    }

    // Masked PAN
    if (strlen(m_szAcctNbr) > 0)
    {
        TraceMsg("Info: MaskedPAN");
        CStringFun::MaskString(m_szAcctNbr, m_nFirstPlainDigits, 4, '*');
        CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_CARD_MASKED_PAN,
                strlen(m_szAcctNbr),
                (unsigned char*)m_szAcctNbr,
                &transParam.bResultLen,
                &transParam.bResult[0]);
    }
    
    // m_szCardHolderName
    if (strlen(m_szCardHolderName) > 0)
    {
        TraceMsg("Info: CardHolderName");
        CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_CARD_HOLDER_NAME,
                strlen(m_szCardHolderName),
                (unsigned char*)m_szCardHolderName,
                &transParam.bResultLen,
                &transParam.bResult[0]);
    }
    
    // Account Type, only for Interac Debit
    nRet = CTlvFun::TLVListReadTagValue(d_TAG_HEARTLAND_ACCOUNT_TYPE, 
                    &transParam.ucaRequestData[0],
                    transParam.usRequestDataLen,
                    &pbyData, 
                    &wLen);
    if (nRet == 0)
    {
        TraceMsg("Info: Account Type:%s", pbyData); // Chequing
        if (memcmp("CHECKING", (char *)pbyData, strlen("CHECKING")) == 0)
        {
            CTlvFun::TLVFormatAndUpdateList(d_TAG_HEARTLAND_ACCOUNT_TYPE,
                    strlen((char *)"CHEQUING"),
                    (unsigned char*)"CHEQUING",
                    &transParam.bResultLen,
                    &transParam.bResult[0]);
        }
        else if (memcmp("SAVINGS", (char *)pbyData, strlen("SAVINGS")) == 0)
        {
            CTlvFun::TLVFormatAndUpdateList(d_TAG_HEARTLAND_ACCOUNT_TYPE,
                    strlen((char *)"SAVINGS"),
                    (unsigned char*)"SAVINGS",
                    &transParam.bResultLen,
                    &transParam.bResult[0]);
        }
    }
    else if(IsInteracAID(d_MSR_ARQC))
    {
        TraceMsg("Info: Account Type default: CHEQUING"); // Chequing
        CTlvFun::TLVFormatAndUpdateList(d_TAG_HEARTLAND_ACCOUNT_TYPE,
                strlen((char *)"CHEQUING"),
                (unsigned char*)"CHEQUING",
                &transParam.bResultLen,
                &transParam.bResult[0]);
    }
    

    // pos sequence nbr
    CTlvFun::TLVFormatAndUpdateListFromAnotherList(d_TAG_HEARTLAND_POS_SEQUENCE_NBR, 
                    &transParam.bResultLen,
                    &transParam.bResult[0],
                    transParam.usRequestDataLen,
                    &transParam.ucaRequestData[0]);

    // m_szCardExpireDate
    if (strlen(m_szCardExpireDate) > 0)
    {
        TraceMsg("Info: CardExpireDate");
        CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_CARD_EXPIRE_DATE,
                strlen(m_szCardExpireDate),
                (unsigned char*)m_szCardExpireDate,
                &transParam.bResultLen,
                &transParam.bResult[0]);
    }

    WORD wErr;
    BYTE byBuffer[256] = { 0 };
    WORD wBufLen = sizeof(byBuffer);
    for (size_t index=0; index<nTagBufCount_ && (transParam.bARQCType!=d_MSR_ARQC); ++index)
    {
        wBufLen = sizeof(byBuffer);
        memset(byBuffer, 0, wBufLen);

        wErr = ReadSpecialTags(printTagList_[index].wTagBuf_, byBuffer, &wBufLen);
        if (d_OK != wErr || wBufLen == 0)
        {
            TraceMsg("Error: EMV_DataGet <%s> failed wRet:0x%04X", printTagList_[index].pszTagBuf_, wErr);
            continue;
        }

        CByteStream cFormatStr(byBuffer, wBufLen);
        if (printTagList_[index].bNeedToString_)
        {
            cFormatStr = CFunBase::Hex2Str(byBuffer, wBufLen);
        }

        wErr = CTlvFun::TLVFormatAndUpdateList((char *)printTagList_[index].pszTagBuf_,
                cFormatStr.GetLength(),
                cFormatStr.GetBuffer(),
                &transParam.bResultLen,
                &transParam.bResult[0]);

        TraceMsg("Info: Index[%d] Update Ret[%d] Tag[%s] Value[%s]", index, wErr, printTagList_[index].pszTagBuf_, CByteStream(byBuffer, wBufLen).PushZero());
    }

    nRet = m_pParamPtr->SetParameter(cTagParam);
    if (nRet != 0)
    {
        TraceMsg("SetParameter <transParam> failed wRet:%d", nRet);
    }

    if (d_TXN_OFFLINE_APPROVED==GetState() ||
        d_TXN_OFFLINE_DECLINED==GetState())
    {
        TraceMsg("INFO: Offline Transaction, and the state has been set, directly return");
        return 0;
    }

    wLen = 0;
    pbyData = NULL;
    nRet = CTlvFun::TLVListReadTagValue(d_TAG_TXN_RESULT, transParam.bResult, transParam.bResultLen, &pbyData, &wLen);
    if (nRet || NULL == pbyData)
    {
        SetState(TXN_STATE_DECLINED);
        TraceMsg("Invalid Parameter");
    }
    else if(*pbyData == d_TXN_APPROVED)
    {
        SetState(TXN_STATE_APPROVAL);
        TraceMsg("Transaction successful");
    }
    else if(*pbyData == d_TXN_OFFLINE_APPROVED)
    {
        SetState(TXN_STATE_OFFLINE_APPROVED);
        TraceMsg("Transaction offline approved");
    }
    else if(*pbyData == d_TXN_OFFLINE_DECLINED)
    {
        SetState(TXN_STATE_OFFLINE_DECLINED);
        TraceMsg("Transaction offline declined");
    }
    else if(*pbyData == d_TXN_APPROVAL_WITH_KERNEL_DECLINE)
    {
        TraceMsg("Transaction Host approved but kernel declined");
        return d_TXN_APPROVAL_WITH_KERNEL_DECLINE;
    }
    else if (*pbyData == d_TXN_SAF_APPROVED)
    {
        SetState(TXN_STATE_SAF_APPROVED);
        TraceMsg("Transaction saf approved");
    }
    else if (*pbyData == d_TXN_SAF_DECLINED)
    {
        SetState(TXN_STATE_SAF_DECLINE);
        TraceMsg("Transaction saf declined");
    }
    else if(*pbyData == d_TXN_HOST_TIMEOUT ||
        *pbyData == d_TXN_APPROVAL_WITH_KERNEL_DECLINE ||
        *pbyData == d_TXN_INTERAC_RETRY_WITH_POS_SEQNBR)
    {
        TraceMsg("Transaction Failed:%x", *pbyData);
        return *pbyData;
    }
    else
    {
        SetState(TXN_STATE_DECLINED);
        TraceMsg("Transaction Failed:%x", *pbyData);
    }

    TraceMsg("Info: PostProcessing exit");
    return 0;    
}


WORD CTxnContext::ReadSpecialTags(WORD wTagName, unsigned char *pszTlvValue, WORD *pwTlvValueLen)
{
    *pwTlvValueLen = 0;
    TraceMsg("WARNING: Castles couldn't come here, it's implemented in subclasses");
    return 0;    
}


WORD CTxnContext::CommWithHost(void)
{
    TraceMsg("Info: CommWithHost entry");

    SetState(TXN_STATE_PREPARING_TXNDATA);
    if (!m_cCallUserAction.ReqUserConfirmConnWithHost())
    {
        TraceMsg("Error: User cancel communicate with host.");
        return 1;
    }

    if (NULL == m_pDataPackPtr || NULL == m_pBaseCurlPtr)
    {
        return 2;
    }
    
    // pack
    TraceMsg("Info: Call DoPack.");
    SetState(TXN_STATE_PACKING_SOAPDATA);
    CByteStream cDataRequest;
    int nRet = m_pDataPackPtr->DoPack(&cDataRequest);
    if (nRet != 0)
    {
        SetState(TXN_STATE_PACKING_SOAPDATA_FAIL);
        return 3;
    }
    SetState(TXN_STATE_PACKING_SOAPDATA_SUCCESS);
    
    m_bNeedCheckSAF = FALSE;
    
    // send to host;
    TraceMsg("Info: Call CurlProcess.");
    SetState(TXN_STATE_CONNECTING);


    if (0 == DemoModeProcess(m_pParamPtr))
    {
        TraceMsg("Info: Demo Mode Process.");
        SetState(TXN_STATE_COMPLETION);
        return 0;
    }


    CByteStream cDataResponse;
    nRet = m_pBaseCurlPtr->CurlProcess(&cDataRequest, cDataResponse);
    if (nRet != 0)
    {
        TraceMsg("CurlProcess, nRet:0x%04X, SoapType:%d", nRet, m_nPackSoapType);
        if (nRet == 0xEC06) //d_CURL_ERR_COULDNT_CONNECT
        {
    		if (m_nPackSoapType == kCreditSale_SoapType ||
				m_nPackSoapType == kDebitSale_SoapType || 
				m_nPackSoapType == kPreAuth_SoapType || 
				m_nPackSoapType == kAuthComplete_SoapType)
    		{
            	m_bNeedCheckSAF = TRUE;
            	m_SAFPackage = cDataRequest;
            	TraceMsg("Set m_bNeedCheckSAF True");
        		return 0;
			}
        }
        SetState(TXN_STATE_CONNECTING_FAIL);
        return nRet;
    }
    SetState(TXN_STATE_CONNECTING_SUCCESS);
    
    // unpack
    TraceMsg("Info: Call DoUnpack.");
    SetState(TXN_STATE_UNPACKING_SOAPDATA);
    nRet = m_pDataPackPtr->DoUnpack(cDataResponse);
    if (nRet != 0)
    {
        SetState(TXN_STATE_UNPACKING_SOAPDATA_FAIL);
        return 4;
    }
    SetState(TXN_STATE_COMPLETION);

    TraceMsg("Info: CommWithHost exit");
    return 0;  
}


void CTxnContext::Set_SAF_Results(unsigned long long ullAmount, int nState)
{
    HEARTLAND_TRANS_PARAM transParam = { 0 };
    tagParam cTagParam = { 0 };

    InitTagParam(cTagParam, kHLTransParam, transParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("SetResult GetParameter failed: 0x%04X", nRet);
        return;
    }

    BYTE byResult = d_TXN_SAF_APPROVED;
    if (nState == TXN_STATE_SAF_DECLINE)
    {
        byResult = d_TXN_SAF_DECLINED;
        SetState(TXN_STATE_SAF_DECLINE);
        CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_RESPONSE_CODE, strlen("FF"), (BYTE *)"FF", &transParam.bResultLen, transParam.bResult);
        CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_RESPONSE_TXT, strlen("SAF DECLINE"), (BYTE *)"SAF DECLINE", &transParam.bResultLen, transParam.bResult);
    }
    else
    {
        byResult = d_TXN_SAF_APPROVED;
        SetState(TXN_STATE_SAF_APPROVED);
        CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_RESPONSE_CODE, strlen("00"), (BYTE *)"00", &transParam.bResultLen, transParam.bResult);
        CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_RESPONSE_TXT, strlen("SAF APPROVAL"), (BYTE *)"SAF APPROVAL", &transParam.bResultLen, transParam.bResult);
    }
    CTlvFun::TLVFormatAndUpdateList(d_TAG_TXN_RESULT, 1, &byResult, &transParam.bResultLen, transParam.bResult);
    
    CByteStream cTemp;
    cTemp.Format("%02d%020llu", m_nPackSoapType, ullAmount);
    cTemp << m_SAFPackage;
    CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_RSP_DATA, cTemp.GetLength(), cTemp.GetBuffer(), &transParam.bResultLen, transParam.bResult);

    TraceMsg("Set SAF Results: package: %s ", cTemp.PushZero());
    nRet = m_pParamPtr->SetParameter(cTagParam);
}


//  SAF modifications  //
// if not debit/credit sale, do not run this function
WORD CTxnContext::ExeSAF(void)
{
//    if (TXN_STATE_CONNECTING_FAIL != GetState())
//    {
//        // SetResult(TXN_STATE_SAF_DECLINE);
//        return 1;
//    }

    tagParam cTagParam = { 0 };
    HEARTLAND_AMOUNT_PARAM cAmountParam;
    HEARTLAND_GENERAL_PARAM cGeneralParam = { 0 };

    InitTagParam(cTagParam, kTxnAmountData, cAmountParam);
    TraceMsg("cTagParam.nType:%d", cTagParam.nType);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        Set_SAF_Results(cAmountParam.ullAmount, TXN_STATE_SAF_DECLINE);
        TraceMsg("GetParameter <transParam> failed wRet:%d", nRet);
        return 2;
    }
    
    if ((cAmountParam.nPackSoapType != kCreditSale_SoapType) && 
        (cAmountParam.nPackSoapType != kDebitSale_SoapType) &&
        (cAmountParam.nPackSoapType != kPreAuth_SoapType) &&
        (cAmountParam.nPackSoapType != kAuthComplete_SoapType))
    {
        Set_SAF_Results(cAmountParam.ullAmount, TXN_STATE_SAF_DECLINE);
        TraceMsg("Soap type:%d is not supported to performe SAF transction, SAF break", cAmountParam.nPackSoapType);
        return 2;
    }

    memset(m_szTransAmount, 0, sizeof(m_szTransAmount));
    sprintf(m_szTransAmount, "%lld.%02lld", cAmountParam.ullAmount/100, cAmountParam.ullAmount%100);

    InitTagParam(cTagParam, kGeneralParam, cGeneralParam);
    nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        Set_SAF_Results(cAmountParam.ullAmount, TXN_STATE_SAF_DECLINE);
        TraceMsg("GetParameter <transParam> failed wRet:%d", nRet);
        return 2;
    }

    // If SAF Mode
    if (0 != strcasecmp(cGeneralParam.szSAFModeFlag, "true"))
    {
        Set_SAF_Results(cAmountParam.ullAmount, TXN_STATE_SAF_DECLINE);
        TraceMsg("SAF mode is not true, return");
        return 3;
    }

    // If reach limit amount
    BYTE byDataRead[64] = { 0 };
    WORD wRet = CTOS_KMS2UserDataRead(TRUE, 0, byDataRead, 64);
    if (wRet != 0)
    {
        // Write a new file to store amount
        memset(byDataRead, 0x00, sizeof(byDataRead));
        memcpy(byDataRead, "0.00", strlen("0.00"));
        CTOS_KMS2UserDataWrite(TRUE, 0, byDataRead, 64);
        TraceMsg("Error: CTOS_KMS2UserDataRead failed, wRet:0x%04X", wRet);
    }
    double dLimitAmount = atof(cGeneralParam.szLimitAmount);
    double dCurrentAmount = atof((char*)byDataRead);
    double dTxnAmount = atof(m_szTransAmount);
    TraceMsg("dSAFApprovedAmt: %.02f, dTxnAmt: %.02f, dSAFLimitAmount: %.02f", dCurrentAmount, dTxnAmount, dLimitAmount);
    if ((dCurrentAmount + dTxnAmount) >= dLimitAmount)
    {
        Set_SAF_Results(cAmountParam.ullAmount, TXN_STATE_SAF_DECLINE);
        TraceMsg("SAF amount over limit");
        return 4;
    }

    // add txn amount to current amount and save to file
    dCurrentAmount += dTxnAmount;
    char szDataWrite[64] = { 0 };
    memset(szDataWrite, 0x00, sizeof(szDataWrite));
    sprintf(szDataWrite, "%.02f", dCurrentAmount);
    TraceMsg("szDataWrite: %s, dSAFApprovedAmt: %.02f", szDataWrite, dCurrentAmount);
    wRet = CTOS_KMS2UserDataWrite(TRUE, 0, (BYTE*)szDataWrite, 64);
    if (wRet != 0)
    {
        Set_SAF_Results(cAmountParam.ullAmount, TXN_STATE_SAF_DECLINE);
        TraceMsg("Write saf amount fail");
        return wRet;
    }
    
    // SAF approve, send package to host.
    // save m_SAFPackage to param
    TraceMsg("Saf Approved, set results");
    Set_SAF_Results(cAmountParam.ullAmount, TXN_STATE_SAF_APPROVED);

    return 0;
}

/* Copy From Global Payment*/
static const struct
{
    const char*     pszBINBegin;
    const char*     pszBINEnd;
    HL_CARD_BRAND   eCardBrand;
}
s_BinRange[] = 
{
    { "5100000000", "5599999999", CARD_BRAND_MASTERCARD },
    { "2221000000", "2720999999", CARD_BRAND_MASTERCARD },
    { "4000000000", "4999999999", CARD_BRAND_VISA },
    { "3400000000", "3499999999", CARD_BRAND_AMEX },
    { "3700000000", "3799999999", CARD_BRAND_AMEX },
    { "3528000000", "3589999999", CARD_BRAND_JCB },
    { "3000000000", "3059999999", CARD_BRAND_DISCOVER },
    { "3095000000", "3095999999", CARD_BRAND_DISCOVER },
    { "3600000000", "3699999999", CARD_BRAND_DISCOVER },
    { "3800000000", "3899999999", CARD_BRAND_DISCOVER },
    { "6011000000", "6011999999", CARD_BRAND_DISCOVER },
    { "6440000000", "6505999999", CARD_BRAND_DISCOVER },
    { "6400000000", "6599999999", CARD_BRAND_DISCOVER },
    { "3000000000", "3059999999", CARD_BRAND_DISCOVER },
    { "3800000000", "3999999999", CARD_BRAND_DISCOVER },
    { "6011000000", "6011039999", CARD_BRAND_DISCOVER },
    { "6011050000", "6011099999", CARD_BRAND_DISCOVER },
    { "6011200000", "6011499999", CARD_BRAND_DISCOVER },
    { "6011740000", "6011749999", CARD_BRAND_DISCOVER },
    { "6011770000", "6011799999", CARD_BRAND_DISCOVER },
    { "6011860000", "6011999999", CARD_BRAND_DISCOVER },
    { "6506010000", "6506099999", CARD_BRAND_DISCOVER },
    { "6506110000", "6599999999", CARD_BRAND_DISCOVER }
};


static const WORD s_BinRangeSize = sizeof(s_BinRange) / sizeof(s_BinRange[0]);


WORD CTxnContext::GetCardBrandFromBinRange(void)
{
    if (strlen(m_szAcctNbr) < 10)
    {
        TraceMsg("Error: Unknown AccNbr:%s", m_szAcctNbr);
        return CARD_BRAND_UNKNOWN;
    }

    char szFirst10[11] = { 0 };
    memcpy(szFirst10, m_szAcctNbr, 10);

    for(WORD w = 0; w < s_BinRangeSize; ++w)
    {
        if (strcmp(szFirst10, s_BinRange[w].pszBINBegin) >=0 &&
            strcmp(szFirst10, s_BinRange[w].pszBINEnd) <=0)
            return s_BinRange[w].eCardBrand;
    }

    return CARD_BRAND_UNKNOWN;
}


BOOL CTxnContext::IsInteracAID(const BYTE& bARQCType)
{
    BYTE byBuffer[256];
    WORD wBufLen, wErr;

    TraceMsg("%s Entry", __FUNCTION__);
    for (size_t index=0; index < nTagAidCount_ && (bARQCType != d_MSR_ARQC); ++index)
    {
        wBufLen = sizeof(byBuffer);
        memset(byBuffer, 0, wBufLen);

        wErr = ReadSpecialTags(printTagAid_[index].wTagBuf_, byBuffer, &wBufLen);
        if (d_OK != wErr || wBufLen == 0)
        {
            TraceMsg("Error: ReadSpecialTags <%s> failed wRet:0x%04X", printTagList_[index].pszTagBuf_, wErr);
            continue;
        }

        // TraceBytes(byBuffer, wBufLen, "AID:");
        if (wBufLen>=5 && memcmp(byBuffer, "\xA0\x00\x00\x02\x77", 5)==0)
        {
            TraceMsg("INFO: ReadSpecialTags <%s> OK, it indicates interac card", printTagList_[index].pszTagBuf_);
            return TRUE;
        }
    }

    TraceMsg("%s Exit", __FUNCTION__);
    return FALSE;
}


BOOL CTxnContext::IsTxnSeqNbrMatched(void)
{
    TraceMsg("Info: %s Entry", __FUNCTION__);

    tagParam cTagParam = { 0 };
    HEARTLAND_AMOUNT_PARAM amountParam;

    InitTagParam(cTagParam, kTxnAmountData, amountParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet)
    {
        TraceMsg("Error: Call IsTxnSeqNbrMatched get HEARTLAND_AMOUNT_PARAM failed.");
        return FALSE;
    }

    HEARTLAND_POLLINGCARD_CACHE pollData = { 0 };
    InitTagParam(cTagParam, kPollingCardCache, pollData);
    nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet)
    {
        TraceMsg("Error: Call IsTxnSeqNbrMatched get HEARTLAND_POLLINGCARD_CACHE failed.");
        return FALSE;
    }

    if (pollData.ulSequenceNbr!=amountParam.ulSeqeueceId || amountParam.ulSeqeueceId==0 || pollData.ulSequenceNbr==0)
    {
        TraceMsg("Error: pollData.ulSequenceNbr:%lu, amountParam.ulSeqeueceId:%lu, mismatch condition", pollData.ulSequenceNbr, amountParam.ulSeqeueceId);
        return FALSE;
    }

    TraceMsg("Info: pollData.ulSequenceNbr:%lu, amountParam.ulSeqeueceId:%lu", pollData.ulSequenceNbr, amountParam.ulSeqeueceId);
    TraceMsg("Info: %s Exit, and Sequence Nbr is matched ", __FUNCTION__);
    return TRUE;
}

WORD CTxnContext::PollingCardProcess(unsigned short usCardEntryMode)
{
    TraceMsg("Info: %s Entry", __FUNCTION__);

    if (m_nPackSoapType != kDebitPollingCard_TxnType &&
        m_nPackSoapType != kCreditPollingCard_TxnType)
    {
        TraceMsg("Info: %s Exit, it's not pollingcard transaction", __FUNCTION__);
        return 0;
    }

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

    // txnSequenceID 
    memset(szTemp, 0, sizeof(szTemp));

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    unsigned long ulSeqNbr = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    sprintf(szTemp, "%ld", ulSeqNbr);
    wRet = CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_POLLCARD_SEQUENCE_NBR,
            strlen(szTemp),
            (BYTE *)szTemp,
            &transParam.bResultLen,
            &transParam.bResult[0]);
    if (wRet != 0)
    {
        TraceMsg("txnSequenceID is abnormal to set, %s Exit", __FUNCTION__);
    }
    m_pParamPtr->SetParameter(cTagParam);

    // save cache card data
    HEARTLAND_POLLINGCARD_CACHE cCacheCardData = {0};
    InitTagParam(cTagParam, kPollingCardCache, cCacheCardData);

    cCacheCardData.usCardEntryMode = usCardEntryMode;
    cCacheCardData.ulSequenceNbr = ulSeqNbr;
    cCacheCardData.usRequestDataLen = transParam.usRequestDataLen;
    memcpy(cCacheCardData.ucaRequestData, transParam.ucaRequestData, transParam.usRequestDataLen);
    memcpy(cCacheCardData.ucaCardNumber, m_szAcctNbr, strlen(m_szAcctNbr));
    TraceMsg("%s, cCacheCardData.ucaCardNumber to set: %s", __FUNCTION__, cCacheCardData.ucaCardNumber);

    CTlvFun::TLVFormatAndUpdateListFromAnotherList(d_TAG_TRANS_CARD_MASKED_PAN, 
                                &cCacheCardData.usRequestDataLen, &cCacheCardData.ucaRequestData[0],
                                transParam.bResultLen, &transParam.bResult[0]);

    CTlvFun::TLVFormatAndUpdateListFromAnotherList(d_TAG_TRANS_CARD_HOLDER_NAME, 
                                &cCacheCardData.usRequestDataLen, &cCacheCardData.ucaRequestData[0],
                                transParam.bResultLen, &transParam.bResult[0]);
    
    m_pParamPtr->SetParameter(cTagParam);

    // txnUiIndex ==> 16
    SetState(TXN_STATE_POLLING_CARD_SUCCESS);

    TraceMsg("Info: %s Exit", __FUNCTION__);
    return TXN_STATE_POLLING_CARD_SUCCESS;

}

void CTxnContext::GetCardHolderNameFromICC()
{
    TraceMsg("Info: %s Entry", __FUNCTION__);

    WORD wError = 0xFFFF;
    BYTE byDataBuf[1024] = {0};
    WORD wDataBufLen = 0;
    
    memset(m_szCardHolderName, 0, sizeof(m_szCardHolderName));
    do
    {
        
        wDataBufLen = sizeof(byDataBuf);
        memset(byDataBuf, 0, wDataBufLen);
        wError = ReadSpecialTags(0x5F20, byDataBuf, &wDataBufLen);
        if (wError == 0)
        {
            //TraceBytes(byDataBuf, wDataBufLen, "CardHoldername:");
            break;
        }
        TraceMsg("Get 0x5F20 fail! wError is:%4X\n",wError);
        
        wDataBufLen = sizeof(byDataBuf);
        memset(byDataBuf, 0, wDataBufLen);
        wError = ReadSpecialTags(0x9F0B, byDataBuf, &wDataBufLen);
        if (wError == 0)
        {
            //TraceBytes(byDataBuf, wDataBufLen, "CardHoldername:");
            break;
        }
    }while(0);
    

    BOOL bFlag = TRUE;
    while(wDataBufLen > 0 && wError == 0)
    {
        if ((byDataBuf[wDataBufLen - 1] >= '0' || byDataBuf[wDataBufLen - 1] <= '9') && bFlag == TRUE)
        {
            --wDataBufLen;
        }

        if (byDataBuf[wDataBufLen - 1] == ' ')
        {
            bFlag = FALSE;
            --wDataBufLen;
        }
        else if(bFlag == FALSE)
        {
            break;
        }
    }

    byDataBuf[wDataBufLen + 1] = '\0';
    memcpy(m_szCardHolderName, byDataBuf, wDataBufLen);
    TraceMsg("CardHolder length:%d, name:%s", wDataBufLen, byDataBuf);
    TraceMsg("Info: %s Exit", __FUNCTION__);
}


void CTxnContext::SendReversal(void)
{
    TraceMsg("Info: %s could not come here", __FUNCTION__);
}


void CTxnContext::CopyDataFromCache(char* pAryTag[], int nTagCount, 
        unsigned short *pusTlvListDstLen,  unsigned char ucaTlvListDst[],
        unsigned short usTlvListSrcLen,  unsigned char ucaTlvListSrc[])
{
    int nFlag = 0;
    int nRet = 0;
    for (int i=0; i<nTagCount && pAryTag[i]!=NULL; ++i)
    {
        nRet = CTlvFun::TLVFormatAndUpdateListFromAnotherList(pAryTag[i], pusTlvListDstLen, ucaTlvListDst, usTlvListSrcLen, ucaTlvListSrc);
        if (nRet != 0)
        {
            TraceMsg("WARNING: Tag[%s] update fail", pAryTag[i]);
        }

        if (strcmp("57", pAryTag[i]) == 0)
        {
            nFlag = 1;
        }
    }

    if (nFlag == 1)
    {
        return;
    }

    nRet = CTlvFun::TLVFormatAndUpdateListFromAnotherList((char *)"57", pusTlvListDstLen, ucaTlvListDst, usTlvListSrcLen, ucaTlvListSrc);
    if (nRet != 0)
    {
        TraceMsg("WARNING: Tag[57] update fail");
    }
}


void CTxnContext::InteracSpecialProcess(bool bNeedAccount)
{    
    TraceMsg("Info: %s Entry", __FUNCTION__);

    tagParam cTagParam;
    HEARTLAND_TRANS_PARAM transParam = { 0 };
    InitTagParam(cTagParam, kHLTransParam, transParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("ERROR: GetParameter Failed:%d", nRet);
        return;
    }

    if (!IsInteracAID(transParam.bARQCType))
    {
        return;
    }

    char szRspBuf[256] = {0};

    // 1. request the account type
    if(bNeedAccount)
    {
        memset(szRspBuf, 0, sizeof(szRspBuf));
        m_cCallUserAction.ReqUserInputAccountType((BYTE *)szRspBuf);
        if (strlen(szRspBuf) > 0)
        {
            TraceMsg("Info: account type:%s", szRspBuf);
            nRet = CTlvFun::TLVFormatAndUpdateList(
                    d_TAG_HEARTLAND_ACCOUNT_TYPE,
                    strlen(szRspBuf),
                    (unsigned char*)szRspBuf,
                    &transParam.usRequestDataLen,
                    &transParam.ucaRequestData[0]);
            if (nRet)
            {
                TraceMsg("TLV_FormatAndUpdateList <d_TAG_HEARTLAND_ACCOUNT_TYPE> fail");
            }
        }
        else
        {
            TraceMsg("Info: Account Type empty");
        }
    }

    // 2. request a pos sequence number
    memset(szRspBuf, 0, sizeof(szRspBuf));
    m_cCallUserAction.ReqUserInputPosSeqNbr((BYTE *)szRspBuf);
    if (strlen(szRspBuf) > 0)
    {
        TraceMsg("Info: POS Sequence Number:%s", szRspBuf);
        nRet = CTlvFun::TLVFormatAndUpdateList(
                d_TAG_HEARTLAND_POS_SEQUENCE_NBR,
                strlen(szRspBuf),
                (unsigned char*)szRspBuf,
                &transParam.usRequestDataLen,
                &transParam.ucaRequestData[0]);
        if (nRet)
        {
            TraceMsg("TLV_FormatAndUpdateList <d_TAG_HEARTLAND_POS_SEQUENCE_NBR> fail");
        }
    }
    else
    {
        TraceMsg("Info: POS Sequence Numbe empty");
    }
    m_pParamPtr->SetParameter(cTagParam);
    
    TraceMsg("Info: %s Exit", __FUNCTION__);
}


void CTxnContext::ResetInteracPosSeqNbr()
{
    TraceMsg("Info: %s Entry", __FUNCTION__);

    tagParam    cTagParam = {0};

    // update POS Sequence Number
    HEARTLAND_TRANS_PARAM transParam = { 0 };
    InitTagParam(cTagParam, kHLTransParam, transParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter Failed:%d",  nRet);
        return ;
    }

    WORD wLen = 0;
    BYTE* pbyData = NULL;
    nRet = CTlvFun::TLVListReadTagValue(d_TAG_TRANS_TRACE_NUMBER, 
            transParam.bResult, 
            transParam.bResultLen, 
            &pbyData, 
            &wLen);
    if (nRet != 0)
    {
        TraceMsg("ERROR: TLVListReadTagValue read d_TAG_TRANS_TRACE_NUMBER Failed:%d",  nRet);
        return ;
    }
                            
    // pos sequence nbr
    nRet = CTlvFun::TLVFormatAndUpdateList(d_TAG_HEARTLAND_POS_SEQUENCE_NBR,
            wLen,
            pbyData,
            &transParam.usRequestDataLen,
            &transParam.ucaRequestData[0]);
    if (nRet != 0)
    {
        TraceMsg("ERROR: TLVFormatAndUpdateList write d_TAG_HEARTLAND_POS_SEQUENCE_NBR Failed:%d",  nRet);
        return ;
    }

    memset(transParam.bResult, 0, transParam.bResultLen);
    transParam.bResultLen = 0;

    nRet = m_pParamPtr->SetParameter(cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter Failed:%d",  nRet);
        return ;
    }

    TraceMsg("Info: %s Exit", __FUNCTION__);
}


void CTxnContext::UpdateCardHolderInfo(void)
{
    TraceMsg("Info: UpdateCardHolderInfo entry");

    tagParam cTagParam = {0};
    HEARTLAND_TRANS_PARAM transParam = {0};

    InitTagParam(cTagParam, kHLTransParam, transParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter <transParam> failed wRet:%d", nRet);
        return;
    }

    // Masked PAN
    if (strlen(m_szAcctNbr) > 0)
    {
        TraceMsg("Info: MaskedPAN");

        char szAcctNbr[20] = { 0 };
        memcpy(szAcctNbr, m_szAcctNbr, strlen(m_szAcctNbr));

        CStringFun::MaskString(szAcctNbr, m_nFirstPlainDigits, 4, '*');
        CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_CARD_MASKED_PAN,
                strlen(szAcctNbr),
                (unsigned char*)szAcctNbr,
                &transParam.bResultLen,
                &transParam.bResult[0]);
    }

    // m_szCardHolderName
    if (strlen(m_szCardHolderName) > 0)
    {
        TraceMsg("Info: CardHolderName");

        CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_CARD_HOLDER_NAME,
                strlen(m_szCardHolderName),
                (unsigned char*)m_szCardHolderName,
                &transParam.bResultLen,
                &transParam.bResult[0]);
    }

    // m_szCardExpireDate
    if (strlen(m_szCardExpireDate) > 0)
    {
        TraceMsg("Info: CardExpireDate");

        CTlvFun::TLVFormatAndUpdateList(d_TAG_TRANS_CARD_EXPIRE_DATE,
                strlen(m_szCardExpireDate),
                (unsigned char*)m_szCardExpireDate,
                &transParam.bResultLen,
                &transParam.bResult[0]);
    }

    nRet = m_pParamPtr->SetParameter(cTagParam);
    if (nRet != 0)
    {
        TraceMsg("SetParameter <transParam> failed wRet:%d", nRet);
    }

    TraceMsg("Info: UpdateCardHolderInfo exit");
    return;
}


int DemoModeProcess(IParameter* pParamPtr)
{
    int nRet = 0;

    tagParam cTagParam = {0};
    HEARTLAND_AMOUNT_PARAM amountParam;
    HEARTLAND_TRANS_PARAM transParam = { 0 };
    InitTagParam(cTagParam, kTxnAmountData, amountParam);

    nRet = pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("ERROR: GetParameter Failed:%d", nRet);
        return 1;
    }

    if (TRUE == amountParam.byOnlineFlag)
    {
        TraceMsg("ERROR: GetParameter Failed:%d", nRet);
        return 1;
    }


    InitTagParam(cTagParam, kHLTransParam, transParam);
    nRet = pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("ERROR: GetParameter Failed:%d", nRet);
    }

    GenerateRandomResponse(&transParam, amountParam.nPackSoapType);

    nRet = pParamPtr->SetParameter(cTagParam);
    if (nRet != 0)
    {
        TraceMsg("ERROR: GetParameter Failed:%d", nRet);
    }

    return 0;
}


static const struct
{
    const char*     pszTlvTag;
    const char*     pszValue;
    int             nValueSize;
} GenerateTlv_[] =
{
    {d_TAG_LICENSE_IDENTITY,            NULL,       6},
    {d_TAG_SITE_IDENTITY,               NULL,       6},
    {d_TAG_DEVICE_IDENTITY,             NULL,       6},
    {d_TAG_TRANSACTION_IDENTITY,        NULL,       10},
    {d_TAG_APPROVED_CODE,               NULL,       6},
    {d_TAG_RETRIEVAL_REFERENCE_NUMBER,  NULL,       12},
    {d_TAG_RESPONSE_CODE,               "0",        0},
    {d_TAG_RESPONSE_TXT,                "Success",  0},
    {d_TAG_TRANS_RESPONSE_CODE,         "00",       2},
    {d_TAG_TRANS_RESPONSE_TXT,          "APPROVAL", 8},
    {d_TAG_TXN_RESULT,                  "\x05",     1},
    {d_TAG_BATCH_IDENTITY,              NULL,       6},
    {d_TAG_TOTAL_SALE_COUNT,            NULL,       2},
    {d_TAG_TOTAL_SALE_AMOUNT,           NULL,       4},
    {d_TAG_BATCH_NUMBER,                NULL,       6},
};



void GenerateRandomResponse(HEARTLAND_TRANS_PARAM* pcTransParam, int nPackSoapType)
{
    if( pcTransParam == NULL )
    {
        TraceMsg("ERROR: Invalid Parameter");
        return;
    }

    WORD wRet = 0;
    static int nSize = sizeof(GenerateTlv_) / sizeof(GenerateTlv_[0]);
    for (int i=0; i<nSize; ++i)
    {
        if (nPackSoapType!=kBatchClose_SoapType &&
            strcmp(GenerateTlv_[i].pszTlvTag, d_TAG_BATCH_IDENTITY) == 0)
        {
            break;
        }
        const char* pszTmpValue = GenerateTlv_[i].pszValue;
        if (pszTmpValue == NULL)
        {
            pszTmpValue = GenerateRandomStr(GenerateTlv_[i].nValueSize);
        }

        wRet = CTlvFun::TLVFormatAndUpdateList((char *)GenerateTlv_[i].pszTlvTag,
                strlen(pszTmpValue),
                (unsigned char*)pszTmpValue,
                &pcTransParam->bResultLen,
                &pcTransParam->bResult[0]);
        if (wRet != 0)
        {
            TraceMsg("ERROR: CTlvFun::TLVFormatAndUpdateList <%s> failed wRet:%d", GenerateTlv_[i].pszTlvTag, wRet);
        }
    }
}


char* GenerateRandomStr(int nLength)
{
    static char s_szTmpString[32] = { 0 };
    memset(s_szTmpString, 0, sizeof(s_szTmpString));

    struct timeval tv = { 0 };
    gettimeofday(&tv, NULL);
    srand((unsigned int)(tv.tv_sec * 1000 + tv.tv_usec));
    int a = rand() % 1000000 + 100000;;

    if (nLength  == 2)
    {
        a = rand() % 100;
        sprintf(s_szTmpString, "%02d", a);
    }
    else if (nLength  == 3)
    {
        a = rand() % 1000;
        sprintf(s_szTmpString, "%03d", a);
    }
    else if (nLength  == 4)
    {
        a = rand() % 10000;
        sprintf(s_szTmpString, "%04d", a);
    }
    else if (nLength == 6)
    {
        sprintf(s_szTmpString, "%06d", a);
    }
    else if(nLength  == 10)
    {
        sprintf(s_szTmpString, "1146%06d", a);
    }
    else if(nLength  == 12)
    {
        sprintf(s_szTmpString, "907814%06d", a);
    }
    usleep(150 * 1000);
    return s_szTmpString;
}

