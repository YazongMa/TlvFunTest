
/*
 * File Name: TxnMsr.cpp
 * Author: Tim.Ma
 * 
 * Create Date: 2019.2.28
 */

#include <unistd.h>

#include "ByteStream.h"
#include "FunBase.h"
#include "Inc/ErrorCode.h"
#include "Inc/UserActFlags.h"
#include "Inc/TxnState.h"
#include "Inc/TxnDataType.h"
#include "PackSoap/BaseParam.h"
#include "TxnEmvcl.h"
#include "TxnDef.h"
#include "TlvFun.h"
#include "TxnTag.h"
#include "StringFun.h"
#include "dbghlp.h"
#include "cJSON.h"


#ifdef PLATFORM_ANDROID
#include "IccSetting/emvcl_setting.h"
#include <kmshelper.h>
#endif

static const char *byTagList_[] =
{
    "5A", "9F02", "9F03", "9F26", "82", "9F36", "9F1A", "95",
    "5F2A", "9A", "9C", "9F37", "4F", "9F10", /*"57",*/ "9F39",
    "9F40", "9F06", "5F34", "9F07", "9F08", "9F09", "8A",
    "9F34", "9F27", "9F7C", "84", "9F6E", "9F0D", "9F0E",
    "9F0F", "5F28", "9F5B", "9F33", "9F35", "9F41", "9B",
    "9F21"
};
static const int nTagListCount_ = sizeof(byTagList_) / sizeof(byTagList_[0]);


static char* s_szAryRspTag[] = 
{
    d_TAG_TRANS_CARD_MASKED_PAN, 
    d_TAG_TRANS_CARD_HOLDER_NAME
};


ULONG OnEVENT_EMVCL_LED_SHOW(unsigned char bLedIndex, unsigned char bOnOff)
{
    return 0;
}

ULONG OnEVENT_EMVCL_SPEAKER(unsigned short usTone)
{
    return 0;
}


/**
* Function:
*      CTxnEmvcl's Construct Function:
*
* @param
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
CTxnEmvcl::CTxnEmvcl(IN IParameter *pParamPtr,
                     IN IDataPack *pDataPackPtr,
                     IN IBaseCurl *pBaseCurlPtr,
                     IN IPinAction *pPinEntryPtr,
                     IN IUserAction *pUserAction)
: 
#ifdef PLATFORM_ANDROID
CTxnContext("TAP",
#else
CTxnContext("CONTACTLESS",
#endif
        pParamPtr,
        pDataPackPtr,
        pBaseCurlPtr,
        pPinEntryPtr,
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
CTxnEmvcl::~CTxnEmvcl()
{
}


int CTxnEmvcl::InitLevel2Lib(void)
{
    return InitContactless();
}


int CTxnEmvcl::PerformTxn(void)
{
    ULONG ulRet = 0xFFFFFFFF;
    //1. Clear cache, init trans parameter
    CTxnContext::InitParam();

    //6. Get data from api layer
    CTxnContext::PreProcessing();

    //2. Init library with related parameters
    InitContactless();

    if (IsTxnSeqNbrMatched())
    {
         // copy data from polling card cache data
         ProcessMatchedTxnSeqNbr();
    }
    else
    {
        //3. Init related tags for emvcl txn
        InitTransaction();

        //4. start reading card
        SetState(TXN_STATE_READING_CARD);
        ULONG ulRet = PerformTransactionEx();
        if (OK != ulRet)
        {
            TraceMsg("ERROR: PerformTransactionEx FAIL, ulRet:0x%08lX", ulRet);
            return -1;
        }

        if (IsInteracAID(d_CTLS_ARQC))
        {
            nRet = m_cCallUserAction.ReqUserInteracReceiptWarning()
            if(nRet != d_EMVAPLIB_OK)
            {
                TraceMsg("PerformTxn User terminated the transaction since no receipt provided");
                SetState(TXN_STATE_IDLE);
                return nRet;
            }
        }
        
        //5. analysising card datas
        ulRet = AnalysisTransactionEx();
        if (ulRet != 0)
        {
            GetOfflineTxnResult();
            CTxnContext::PostProcessing();
            TraceMsg("WARNING: Offline Transaction, ulRet:0x%08lX", ulRet);
            return 0;
        }

        //7. Retrieve data from card
        ulRet = RetrieveCardData();
        if (OK != ulRet)
        {
            SetState(TXN_STATE_PREPARING_TXNDATA_FAIL);
            return -1;
        }

        ulRet = PollingCardProcess(kEmvCL);
        if (TXN_STATE_POLLING_CARD_SUCCESS == ulRet)
        {
            TraceMsg("Info: Polling card.");
            return 0;
        }
    }

    InteracSpecialProcess(false);

    //8. Chat with host
    CommWithHeartlandHost();
    if (m_bNeedCheckSAF)
    {
        ExeSAF();
    }

    //9. Complete transaction
    CompleteTransaction();

    //10. Related tags process
    int nRet = CTxnContext::PostProcessing();
    if (d_TXN_HOST_TIMEOUT == nRet ||d_TXN_APPROVAL_WITH_KERNEL_DECLINE == nRet)
    {
        SendReversal();
    }

    return 0;
}


int CTxnEmvcl::InitContactless(void)
{
    TraceMsg("INFO: CTxnEmvcl::InitContactless entry");
    EnableCancelTxn(FALSE);

    HEARTLAND_EMVCL_PARAM emvclParam = {0};
    tagParam cTagParam = {0};

    InitTagParam(cTagParam, kEmvclParam, emvclParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("ERROR: GetParameter Failed:%d", nRet);
        return nRet;
    }

#ifndef PLATFORM_ANDROID
    TraceMsg("INFO: CTxnEmvcl::InitContactless m_nDebugFlag:%d", emvclParam.m_nDebugFlag);
    if (emvclParam.m_nDebugFlag != 0)
    {
        EMVCL_SetDebug(TRUE, d_EMVCL_DEBUG_PORT_USB);
    }
    else
    {
        EMVCL_SetDebug(FALSE, d_EMVCL_DEBUG_PORT_USB);
    }
#endif

    static int s_nInitFlag = 0;
    if (s_nInitFlag == 1)
    {
        TraceMsg("ERROR: EMVCL has already initialized");
        return 0;
    }

    emvclParam.m_cInitData.stOnEvent.OnShowMessage = OnShowMsg;

#ifdef PLATFORM_ANDROID
    char* pszEmvclCfgPath = (char *)emvclParam.m_cInitData.pConfigFilename;
    emvclParam.m_cInitData.pConfigFilename = NULL;
    emvclParam.m_cInitData.bConfigFilenameLen = 0;
    ULONG ulRet = EMVCL_Initialize(&emvclParam.m_cInitData);
    if (ulRet != OK)
    {
        TraceMsg("ERROR: EMVCL_Initialize Failed:0x%08lX", ulRet);
        return ulRet;
    }

    EMVCLManage cEMVCLManage;
    cEMVCLManage.EMVCLInit(TRUE, pszEmvclCfgPath);

#else
    ULONG ulRet = EMVCL_Initialize(&emvclParam.m_cInitData);
    if (ulRet != OK)
    {
        TraceMsg("ERROR: EMVCL_Initialize Failed:0x%08lX", ulRet);
        return ulRet;
    }
#endif

#ifdef PLATFORM_ANDROID
    if (emvclParam.m_nDebugFlag != 0)
    {
        EMVCL_SetDebug(TRUE, 0xF4);
    }
    else
    {
        EMVCL_SetDebug(FALSE, 0xF4);
    }
#endif

    ulRet = EMVCL_SetParameter(&emvclParam.m_cParamData);
    if (ulRet != d_EMVCL_NO_ERROR)
    {
        TraceMsg("ERROR: EMVCL_SetParameter Failed:0x%08lX", ulRet);
        return ulRet;
    }

    EMVCL_SpecialEventRegister(d_EMVCL_EVENTID_LED_PIC_SHOW, (void *)OnEVENT_EMVCL_LED_SHOW);
    EMVCL_SpecialEventRegister(d_EMVCL_EVENTID_AUDIO_INDICATION, (void *)OnEVENT_EMVCL_SPEAKER);

    s_nInitFlag = 1;
    TraceMsg("INFO: CTxnEmvcl::InitContactless exit");
    return 0;
}

void CTxnEmvcl::InitTransaction(void)
{
    TraceMsg("INFO: CTxnEmvcl::InitTransaction entry");

    // clear host response
    ResetHostRspData();
    
    BYTE bTagNum = 0;
    BYTE byTlvBuf[256] = {0};
    WORD wTlvLen = 0;

    memset((void *)&m_cRCData, 0, sizeof(m_cRCData));
    m_cTlvList.Empty();

    tagParam cTagParam = {0};
    HEARTLAND_AMOUNT_PARAM cAmtParam;
    InitTagParam(cTagParam, kTxnAmountData, cAmtParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("ERROR: GetParameter Failed:%d", nRet);
        return;
    }

    // 9C: txnType[d_EMVCL_TXN_TYPE_PURCHASE, d_EMVCL_TXN_TYPE_CASH, d_EMVCL_TXN_TYPE_CASHBACK, d_EMVCL_TXN_TYPE_REFUND]
    BYTE bTxnType = 0xFF;
    switch (cAmtParam.nPackSoapType)
    {
    case kPreAuth_SoapType:
    case kCreditSale_SoapType:
    case kDebitSale_SoapType:
    case kDebitPollingCard_TxnType:
    case kCreditPollingCard_TxnType:
        if (cAmtParam.ullOtherAmount == 0)
            bTxnType = d_EMVCL_TXN_TYPE_PURCHASE;
        else
            bTxnType = d_EMVCL_TXN_TYPE_CASHBACK;
        break;
    case kCreditVoid_SoapType:
        bTxnType = 0x02;
        break;
    case kCreditReturn_SoapType:
    case kDebitReturn_SoapType:
        bTxnType = d_EMVCL_TXN_TYPE_REFUND;
        break;
    default:
        break;
    }
    if (0xFF == bTxnType)
    {
        TraceMsg("ERROR: The transaction type is %d, which is not correct.", cAmtParam.nPackSoapType);
        return;
    }

    WORD wRet = CTlvFun::TLVFormatAndUpdateList(TAG_TXN_TYPE, 1, &bTxnType, &wTlvLen, (unsigned char *)byTlvBuf);
    if (wRet)
    {
        TraceMsg("ERROR: CTlvFun::TLVFormatAndUpdateList <TAG_TXN_TYPE>");
        return;
    }
    bTagNum++;

    BYTE byTempAmt[20] = {0};
    static CByteStream cOutData;

    // 9F02: Amount
    cOutData.Empty();
    memset(byTempAmt, 0, sizeof(byTempAmt));
    if (cAmtParam.ullAmount != 0)
    {
        sprintf((char *)byTempAmt, "%012lld", cAmtParam.ullAmount);
    }
    else
    {
        sprintf((char *)byTempAmt, "%012d", 10);
    }
    
    CFunBase::Str2Hex((char *)byTempAmt, cOutData);
    wRet = CTlvFun::TLVFormatAndUpdateList(TAG_AMT_AUTH, 6, cOutData.GetBuffer(), &wTlvLen, (unsigned char *)byTlvBuf);
    if (wRet)
    {
        TraceMsg("ERROR: CTlvFun::TLVFormatAndUpdateList <TAG_AMT_AUTH>");
        return;
    }
    bTagNum++;

    // 9F03: CashBackAmount
    cOutData.Empty();
    memset(byTempAmt, 0, sizeof(byTempAmt));
    sprintf((char *)byTempAmt, "%012lld", cAmtParam.ullOtherAmount);
    CFunBase::Str2Hex((char *)byTempAmt, cOutData);
    wRet = CTlvFun::TLVFormatAndUpdateList(TAG_AMT_OTHER, 6, cOutData.GetBuffer(), &wTlvLen, (unsigned char *)byTlvBuf);
    if (wRet)
    {
        TraceMsg("ERROR: CTlvFun::TLVFormatAndUpdateList <TAG_AMT_OTHER>");
        return;
    }
    bTagNum++;

    cOutData.Empty();
#ifdef PLATFORM_ANDROID
    CFunBase::Str2Hex((char *)"07A000000004220307A000000004101007A000000098084007A0000000031010", cOutData);
#else
    CFunBase::Str2Hex((char *)"07A000000004101007A000000004220307A000000098084007A0000000031010", cOutData);
#endif
    wRet = CTlvFun::TLVFormatAndUpdateList(d_TAG_PREFER_AID_LIST, cOutData.GetLength(), cOutData.GetBuffer(), &wTlvLen, (unsigned char *)byTlvBuf);
    if (wRet)
    {
        TraceMsg("ERROR: CTlvFun::TLVFormatAndUpdateList <TAG_AMT_OTHER>");
        return;
    }
    bTagNum++;

    TraceMsg("INFO: bTagNum:%d, wTlvLen:%d", bTagNum, wTlvLen);
    TraceBytes(byTlvBuf, wTlvLen, "InitData:");
    ULONG ulRet = EMVCL_InitTransactionEx(bTagNum, byTlvBuf, wTlvLen);
    if (wRet)
    {
        TraceMsg("ERROR: EMVCL_InitTransactionEx failed, ulRet:0x%08lX", ulRet);
        return;
    }

    TraceMsg("INFO: CTxnEmvcl::InitTransaction exit");
}

ULONG CTxnEmvcl::PerformTransactionEx(void)
{
    TraceMsg("INFO: CTxnEmvcl::PerformTransactionEx entry");

    ULONG ulRet;
    do
    {
        if (IsEnableCancelTxn())
        {
            ulRet == d_EMVCL_TX_CANCEL;
            break;
        }

        ulRet = EMVCL_PerformTransactionEx(&m_cRCData);
        usleep(20);
    } while (ulRet == d_EMVCL_PENDING);
    TraceMsg("INFO: EMVCL_PerformTransactionEx :0x%08lX", ulRet);

    switch (ulRet)
    {
    case d_EMVCL_TX_CANCEL:
        SetState(TXN_STATE_CANCEL_READING_CARD);
        TraceMsg("ERROR: TXN CANCEL :0x%08lX", ulRet);
        return ulRet;

    case d_EMVCL_RC_FAILURE:
        SetState(TXN_STATE_READING_CARD_FAIL);
        CTOS_Sound(750, 20);
        usleep(200 * 1000);
        CTOS_Sound(750, 20);
        TraceMsg("ERROR: TXN RC FAILURE :0x%08lX, EMVCL_GetLastError:%lu", ulRet, EMVCL_GetLastError());
        return ulRet;

    case d_EMVCL_RC_MORE_CARDS:
        SetState(TXN_STATE_MORE_CARD_WERE_DETECTED);
        TraceMsg("ERROR: More card was detected :0x%08lX", ulRet);
        return ulRet;

    case d_EMVCL_RC_SEE_PHONE:
        SetState(TXN_STATE_PLEASE_SEE_PHONE);
        TraceMsg("ERROR: Please see phone");
        return ulRet;

    case d_EMVCL_RC_FALLBACK:
        SetState(TXN_STATE_PLEASE_INSERT_CARD);
        TraceMsg("ERROR: Please insert card");
        return ulRet;

    case d_EMVCL_RC_DATA:
        CTOS_Sound(2700, 20);
        TraceMsg("INFO: RC data successful");
        break;

    default:
        SetState(TXN_STATE_READING_CARD_FAIL);
        TraceMsg("ERROR: RC data fail");
        return ulRet;
    }

    BYTE byszTrack2Data[128] = {0};
    WORD wTrack2DataLen = 0;

    CStringFun::Track2ToFormatedStr(m_cRCData.baTrack2Data, m_cRCData.bTrack2Len, byszTrack2Data, &wTrack2DataLen);
    m_cRCData.bTrack2Len = wTrack2DataLen;
    m_cRCData.baTrack2Data[wTrack2DataLen] = 0x00;
    memcpy(m_cRCData.baTrack2Data, byszTrack2Data, wTrack2DataLen);

    CTxnContext::GetAcctNbr(m_cRCData.baTrack2Data, m_cRCData.bTrack2Len);
    m_cTlvList.Write((void *)m_cRCData.baChipData, m_cRCData.usChipDataLen);
    m_cTlvList.Write((void *)m_cRCData.baAdditionalData, m_cRCData.usAdditionalDataLen);
    CTxnContext::GetCardHolderNameFromICC();

    // try to get card expire date from track 2 data
    for(int i = 0; i < m_cRCData.bTrack2Len; i++)
    {
        if(m_cRCData.baTrack2Data[i] == 0x3D)
        {
            memcpy(m_szCardExpireDate, &m_cRCData.baTrack2Data[i+1], 4);
            break;
        }
    }

    // update card holder information to app layer
    UpdateCardHolderInfo();
    TraceMsg("INFO: CTxnEmvcl::PerformTransactionEx exit");
    return OK;
}

WORD CTxnEmvcl::AnalysisTransactionEx(void)
{
    unsigned int wRet = 0;
    EMVCL_RC_DATA_ANALYZE cAnalysisRCData = {0};
    EMVCL_AnalyzeTransactionEx(&m_cRCData, &cAnalysisRCData);

    TraceMsg("Info: RC Analyze, Trans Result = %d", cAnalysisRCData.usTransResult);
    TraceMsg("Info: RC Analyze, CVM Analysis = %d", cAnalysisRCData.bCVMAnalysis);
    TraceMsg("Info: RC Analyze, Visa AOSA Present = %d", cAnalysisRCData.bVisaAOSAPresent);
    TraceMsg("Info: RC Analyze, ODA Fail = %d", cAnalysisRCData.bODAFail);
    TraceMsg("Info: RC Analyze, CompleteFunRequired = %d", cAnalysisRCData.bODAFail);
    TraceMsg("Info: RC Analyze, bCompleteFunRequired = %d", cAnalysisRCData.bCompleteFunRequired);
    TraceBytes(cAnalysisRCData.baCVMResults, sizeof(cAnalysisRCData.baCVMResults), "Info: RC Analyze, CVM Result:");
    TraceBytes(cAnalysisRCData.baVisaAOSA, sizeof(cAnalysisRCData.baVisaAOSA), "Info: RC Analyze, Visa AOSA:");

    // CompleteFunRequired ? 
    m_bCompleteFunRequired = cAnalysisRCData.bCompleteFunRequired;

    // Cardholder verification
    switch (cAnalysisRCData.bCVMAnalysis)
    {
    case d_EMVCL_CVM_REQUIRED_SIGNATURE:
        TraceMsg("Info: Use Signature");
        break;

    case d_EMVCL_CVM_REQUIRED_NOCVM:
        TraceMsg("Info: Use No CVM");
        break;

    case d_EMVCL_CVM_REQUIRED_ONLPIN:
        TraceMsg("Info: Need to get online PIN");
#ifdef PLATFORM_ANDROID
    	CTOS_LCDTClearDisplay();
#endif
		SetState(TXN_STATE_PIN_ENTER_START);
		
#ifdef PLATFORM_ANDROID
    CTJNI_KMS_EnableLCD();
#endif
        // Avoid brushing off KMS interfaces due to delays
        usleep(500 * 1000);
        wRet = CTxnContext::GetPinBolck();

#ifdef PLATFORM_ANDROID
    CTOS_LCDTClearDisplay();
    CTJNI_KMS_DisableLCD();
#endif
        SetState(TXN_STATE_PIN_ENTER_FINISH);
        usleep(500 * 1000);
        break;

    default :
        TraceMsg("WARNING: Unknown CVM");
    }

    // ARQC judgement
    switch (cAnalysisRCData.usTransResult)
    {
    case d_EMVCL_OUTCOME_APPROVAL:
        SetState(TXN_STATE_OFFLINE_APPROVED);
        TraceMsg("Info: Outcome offline approval");
        return d_EMVCL_OUTCOME_APPROVAL;

    case d_EMVCL_OUTCOME_DECLINED:
        SetState(TXN_STATE_OFFLINE_DECLINED);
        TraceMsg("Info: Outcome offline declined");
        return d_EMVCL_OUTCOME_DECLINED;

    case d_EMVCL_OUTCOME_ONL:
        TraceMsg("Info: Need to go online");
        break;

    default :
        TraceMsg("WARNING: Unknown Operation");
    }

    return wRet;
}

WORD CTxnEmvcl::RetrieveCardData(void)
{
    TraceMsg("%s entry", __FUNCTION__);
    static CByteStream cOutData;

    WORD wRet;
    BYTE byTempAmt[20] = {0};
    tagParam cTagParam = {0};
    HEARTLAND_TRANS_PARAM transParam = {0};

    InitTagParam(cTagParam, kHLTransParam, transParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter Failed:%d", nRet);
        return ERROR_INVALID_PARAM;
    }

    WORD wLength = 0;
    BYTE *pbyData = NULL;

    switch (m_cRCData.bSID)
    {
    //MSD
    case d_EMVCL_SID_VISA_OLD_US:
    case d_EMVCL_SID_VISA_WAVE_2:
    case d_EMVCL_SID_VISA_WAVE_MSD:
    case d_EMVCL_SID_PAYPASS_MAG_STRIPE:
    case d_EMVCL_SID_JCB_WAVE_2:
    case d_EMVCL_SID_JCB_MSD:
    case d_EMVCL_SID_JCB_LEGACY:
    case d_EMVCL_SID_AE_MAG_STRIPE:
//    case d_EMVCL_SID_INTERAC_FLASH:
    case d_EMVCL_SID_MEPS_MCCS:
    case d_EMVCL_SID_CUP_QPBOC:
    case d_EMVCL_SID_DISCOVER_DPAS_MAG_STRIPE:
        TraceMsg("MSD");
        m_pszPanEntryLegend = "MAGNETIC STRIPE";

        transParam.bARQCType = d_MSD_ARQC;
        break;

    case d_EMVCL_SID_DISCOVER:
        CTlvFun::TLVListReadTagValue(TAG_APP_ID,
                                            m_cRCData.baChipData, m_cRCData.usChipDataLen,
                                            &pbyData, &wLength);
        if (wLength >= 7 && memcmp("\xA0\x00\x00\x03\x24\x10\x10", pbyData, 7) == 0)
        {
            TraceMsg("MSD");
            transParam.bARQCType = d_MSD_ARQC;
            break;
        }

    //emv contactless
    default:
        TraceMsg("EMV Contactless, length of all tags: %d", m_cTlvList.GetLength());
        TraceBytes(m_cTlvList.GetBuffer(), m_cTlvList.GetLength(), "EMVCL TAGS :");
        for (int i = 0; i < nTagListCount_; i++)
        {
            wRet = CTlvFun::TLVFormatAndUpdateListFromAnotherList(
                (char *)byTagList_[i],
                &transParam.usRequestDataLen,
                transParam.ucaRequestData,
                m_cTlvList.GetLength(),
                m_cTlvList.GetBuffer());
            if (wRet != OK)
            {
                TraceMsg("WARNING: Tag:%s TLVFormatAndUpdateListFromAnotherList fail", byTagList_[i]);
            }
        }

        transParam.bARQCType = d_CTLS_ARQC;
        break;
    }

    TraceMsg("INFO: Retrieve TAG_TK2 Data");
    wRet = CTlvFun::TLVFormatAndUpdateList(
        TAG_TK2,
        m_cRCData.bTrack2Len,
        m_cRCData.baTrack2Data,
        &transParam.usRequestDataLen,
        transParam.ucaRequestData);
    if (wRet)
    {
        TraceMsg("TLV_FormatAndUpdateList <TAG_TK2> Failed");
        return ERROR_GET_TAGDATA_FAIL;
    }

    BYTE bPosEntry = 0x00;
    TraceMsg("INFO: Retrieve TAG_POS_ENTRY_MODE Data");
    wRet = CTlvFun::TLVFormatAndUpdateList(
        TAG_POS_ENTRY_MODE,
        1,
        &bPosEntry,
        &transParam.usRequestDataLen,
        &transParam.ucaRequestData[0]);
    if (wRet)
    {
        TraceMsg("TLV_FormatAndUpdateList <TAG_POS_ENTRY_MODE> Failed");
        return ERROR_GET_TAGDATA_FAIL;
    }

    TraceMsg("INFO: SetParameter");
    nRet = m_pParamPtr->SetParameter(cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter Failed:%d", nRet);
        return ERROR_INVALID_PARAM;
    }


    TraceMsg("%s exit", __FUNCTION__);
    return nRet;
}

void CTxnEmvcl::CommWithHeartlandHost(void)
{
    TraceMsg("%s entry", __FUNCTION__);

    int         nRet = 0;
    WORD        wLen = 0;
    BYTE*       pbyData = NULL;
    static BOOL bRetry = TRUE;
    tagParam    cTagParam = {0};
    HEARTLAND_TRANS_PARAM transParam = { 0 };

    do
    {
        nRet = CTxnContext::CommWithHost();
        if (0 != nRet)
        {
            m_byActionCode = d_EMVCL_COMPLETE_ACTION_UNAB_ONL;

            // defined in cURL.h
            if (nRet == 0xEC06)
            {
                memcpy(m_byAuthCode, "Z3", 2);
            }
            TraceMsg("Error: Connect heartland host failed.");
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
            m_byActionCode = d_EMVCL_COMPLETE_ACTION_UNAB_ONL;
            TraceMsg("Error: Parse host response failed.");
            break;
        }
        if (0 == memcmp(m_byAuthCode, "00", 2))
        {
            m_byActionCode = d_EMVCL_COMPLETE_ACTION_ONL_APPROVED;
        }
        else if ((memcmp(m_byAuthCode, "Z3", 2) == 0) ||
                 (memcmp(m_byAuthCode, "Y3", 2) == 0))
        {
            m_byActionCode = d_EMVCL_COMPLETE_ACTION_UNAB_ONL;
        }
        else
        {
            m_byActionCode = d_EMVCL_COMPLETE_ACTION_ONL_DECLINED;
        }
    } while (0);

    TraceMsg("%s exit", __FUNCTION__);
}


void CTxnEmvcl::ResetHostRspData(void)
{
    TraceMsg("%s entry", __FUNCTION__);
    bzero(m_byAuthCode, sizeof(m_byAuthCode));
    bzero(m_byIADBuf, sizeof(m_byIADBuf));
    bzero(m_byIssuerScript, sizeof(m_byIssuerScript));
    m_bCompleteFunRequired = FALSE;
    m_byActionCode = 0x00;
    m_wRecvIADLen = m_wRecvIssuerScriptLen = 0x00;

    TraceMsg("%s exit", __FUNCTION__);
}


WORD CTxnEmvcl::CompleteTransaction()
{
    TraceMsg("%s entry", __FUNCTION__);
    if (m_bCompleteFunRequired == FALSE)
    {
        TraceMsg("INFO: It does not have to call CompleteTransaction");
        return 0;
    }

    EMVCL_RC_DATA_EX stRCDataEx = { 0 };
    EMVCL_CompleteEx(m_byActionCode, m_byAuthCode, m_wRecvIADLen, m_byIADBuf, m_wRecvIssuerScriptLen, m_byIssuerScript, &stRCDataEx);
    
    TraceMsg("%s exit", __FUNCTION__);
    return 0;
}


int CTxnEmvcl::ParseHostResponse()
{
    TraceMsg("%s entry", __FUNCTION__);
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

    TraceMsg("%s exit", __FUNCTION__);
    return 0;
}


void CTxnEmvcl::FillRspIccData_AuthData(
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


void CTxnEmvcl::FillRspIccData_IssuerScript(
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


WORD CTxnEmvcl::ReadSpecialTags(WORD wTagName, unsigned char *pszTlvValue, WORD *pwTlvValueLen)
{
    BYTE *pbyBuffer = NULL;

    // clear buffer pszTlvValue with the special length: *pwTlvValueLen
    memset(pszTlvValue, 0, *pwTlvValueLen);

    char caTagName[8] = { 0 };
    sprintf(caTagName, "%X", wTagName);
    
    // get data from m_cTlvList
    WORD wErr = CTlvFun::TLVListReadTagValue(
        caTagName,
        m_cTlvList.GetBuffer(),
        m_cTlvList.GetLength(),
        &pbyBuffer,
        pwTlvValueLen);
    if (wErr != 0)
    {
        TraceMsg("ERROR: ReadSpecialTags[%s] FAIL", caTagName);
        return 1;
    }

    memcpy(pszTlvValue, pbyBuffer, *pwTlvValueLen);
    return 0;
}

void CTxnEmvcl::OnShowMsg(unsigned char bKernel, EMVCL_USER_INTERFACE_REQ_DATA *pstUserInterfaceRequestData)
{
    if (pstUserInterfaceRequestData->bStatus == d_EMVCL_USR_REQ_STATUS_READY_TO_READ &&
        pstUserInterfaceRequestData->bMessageIdentifier != d_EMVCL_USR_REQ_MSG_SEE_PHONE)
    {
        TraceMsg("Ready to Read");
    }

    if (((pstUserInterfaceRequestData->bStatus == d_EMVCL_USR_REQ_STATUS_NOT_READY) ||
         (pstUserInterfaceRequestData->bStatus == d_EMVCL_USR_REQ_STATUS_PROCESSING_ERROR)) &&
        (pstUserInterfaceRequestData->bMessageIdentifier == d_EMVCL_USR_REQ_MSG_SEE_PHONE))
    {
        TraceMsg("Reader Not Ready!!");
    }

    if (pstUserInterfaceRequestData->bMessageIdentifier == d_EMVCL_USR_REQ_MSG_PRESENT_CARD_AGAIN)
    {
        TraceMsg("Plz Tap Card Again");
    }

    /******************************/
    if (pstUserInterfaceRequestData->bStatus == d_EMVCL_USR_REQ_STATUS_READY_TO_READ)
    {
        TraceMsg("PLZ TAP CARD");
        //EMVCL_ShowContactlessSymbol(NULL);
    }

    if (pstUserInterfaceRequestData->bStatus == d_EMVCL_USR_REQ_STATUS_CARD_READ_SUCCESSFULLY ||
        pstUserInterfaceRequestData->bMessageIdentifier == d_EMVCL_USR_REQ_MSG_CARD_READ_OK)
    {
        TraceMsg("READ CARD OK");
    }

    if (pstUserInterfaceRequestData->bMessageIdentifier == d_EMVCL_USR_REQ_MSG_PROCESSING)
    {
        TraceMsg("  PROCESSING...");
    }
    else if (pstUserInterfaceRequestData->bMessageIdentifier == d_EMVCL_USR_REQ_MSG_DECLINED)
    {
        TraceMsg("TXN Result: ");
        TraceMsg("   DECLINED ");
    }
    else if (pstUserInterfaceRequestData->bMessageIdentifier == d_EMVCL_USR_REQ_MSG_APPROVED)
    {
        TraceMsg("TXN Result: ");
        TraceMsg("   APPROVED ");
    }
    else if (pstUserInterfaceRequestData->bMessageIdentifier == d_EMVCL_USR_REQ_MSG_AUTHORISING_PLEASE_WAIT)
    {
        TraceMsg("   PLZ WAIT! ");
    }
    else if (pstUserInterfaceRequestData->bMessageIdentifier == d_EMVCL_USR_REQ_MSG_ERROR_OTHER_CARD)
    {
        TraceMsg("PLZ USE OTHER CARD!");
    }
    else if (pstUserInterfaceRequestData->bMessageIdentifier == d_EMVCL_USR_REQ_MSG_INSERT_CARD)
    {
        TraceMsg("PLZ INSERT CARD!");
    }
    else if (pstUserInterfaceRequestData->bMessageIdentifier == d_EMVCL_USR_REQ_MSG_ENTER_PIN)
    {
        TraceMsg("PLZ ENTER PIN!");
    }
    else if (pstUserInterfaceRequestData->bMessageIdentifier == d_EMVCL_USR_REQ_MSG_SEE_PHONE)
    {
        TraceMsg("PLZ SEE PHONE!");
    }
    else if (pstUserInterfaceRequestData->bMessageIdentifier == d_EMVCL_USR_REQ_MSG_PRESENT_1_CARD_ONLY)
    {
        TraceMsg("PRESENT ONE\nCARD ONLY       ");
    }
    else if (pstUserInterfaceRequestData->bMessageIdentifier == d_EMVCL_USR_REQ_MSG_NO_CARD)
    {
        TraceMsg("  NO CARD ");
    }
    else if (pstUserInterfaceRequestData->bMessageIdentifier == d_EMVCL_USR_REQ_MSG_REMOVE_CARD)
    {
//        CTOS_LCDTClearDisplay();
        TraceMsg("  REMOVE CARD ");
    }
    else if (pstUserInterfaceRequestData->bMessageIdentifier == d_EMVCL_USR_REQ_MSG_INSERT_OR_SWIPE_CARD)
    {
        TraceMsg("INSERT, SWIPE\nOR TRY ANOTHER\nCARD");
    }
}


void CTxnEmvcl::SendReversal(void)
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

    CTxnContext::CommWithHost();
    CTxnContext::PostProcessing();

    // update result
    HEARTLAND_TRANS_PARAM transParam = { 0 };
    InitTagParam(cTagParam, kHLTransParam, transParam);
    nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter Failed:%d",  nRet);
        return ;
    }

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


bool CTxnEmvcl::IsInteracOfEMVCL(void)
{
    WORD    wLen = 0;
    BYTE*   pbyData = NULL;
    WORD    wRet = 1;
    
    do
    {
        wRet = CTlvFun::TLVListReadTagValue(TAG_APP_ID, m_cRCData.baChipData, m_cRCData.usChipDataLen, &pbyData, &wLen);
        if (!wRet)
        {
            break;
        }

        wRet = CTlvFun::TLVListReadTagValue(TAG_APP_ID, m_cRCData.baAdditionalData, m_cRCData.usAdditionalDataLen, &pbyData, &wLen);
        if (!wRet)
        {
            break;
        }

        wRet = CTlvFun::TLVListReadTagValue(TAG_ADF, m_cRCData.baChipData, m_cRCData.usChipDataLen, &pbyData, &wLen);
        if (!wRet)
        {
            break;
        }

        wRet = CTlvFun::TLVListReadTagValue(TAG_ADF, m_cRCData.baAdditionalData, m_cRCData.usAdditionalDataLen, &pbyData, &wLen);
        if (!wRet)
        {
            break;
        }

        TraceMsg("CTlvFun::TLVListReadTagValue Read AID failed");
        return false;
    }while (0);

    TraceBytes(pbyData, wLen, "CTlvFun::TLVListReadTagValue AID: ");
    if (wLen < 5)
    {
        return false;
    }

    if (memcmp(pbyData, "\xA0\x00\x00\x02\x77", 5) != 0)
    {
        return false;
    }

    return true;
}

void CTxnEmvcl::ProcessMatchedTxnSeqNbr(void)
{
    TraceMsg("Info: %s Entry", __FUNCTION__);

    unsigned long ulRet = 0xFFFFFFFF;
    tagParam cTagParam = { 0 };
    HEARTLAND_TRANS_PARAM transParam = { 0 };
    HEARTLAND_AMOUNT_PARAM amountParam ;
    HEARTLAND_POLLINGCARD_CACHE pollData = { 0 };

    InitTagParam(cTagParam, kPollingCardCache, pollData);
    ulRet = m_pParamPtr->GetParameter(&cTagParam);
    TraceMsg("Info: Call IsTxnSeqNbrMatched get HEARTLAND_POLLINGCARD_CACHE %s.", ulRet==0?"OK":"FAIL");

    InitTagParam(cTagParam, kTxnAmountData, amountParam);
    ulRet = m_pParamPtr->GetParameter(&cTagParam);
    TraceMsg("Info: Call IsTxnSeqNbrMatched get HEARTLAND_AMOUNT_PARAM %s.", ulRet==0?"OK":"FAIL");

    InitTagParam(cTagParam, kHLTransParam, transParam);
    ulRet = m_pParamPtr->GetParameter(&cTagParam);
    TraceMsg("Info: Call IsTxnSeqNbrMatched get HEARTLAND_TRANS_PARAM %s.", ulRet==0?"OK":"FAIL");

    transParam.bARQCType = d_CTLS_ARQC;
    CopyDataFromCache((char**)byTagList_, nTagListCount_, 
            &transParam.usRequestDataLen, &transParam.ucaRequestData[0],
            pollData.usRequestDataLen, &pollData.ucaRequestData[0]);
            
    CopyDataFromCache(s_szAryRspTag, sizeof(s_szAryRspTag) / sizeof(s_szAryRspTag[0]), 
            &transParam.bResultLen, &transParam.bResult[0],
            pollData.usRequestDataLen, &pollData.ucaRequestData[0]);

    CByteStream cOutData;
    BYTE        byTempAmt[20] = { 0 };
    sprintf((char *)byTempAmt, "%012lld", amountParam.ullAmount);
    CFunBase::Str2Hex((char *)byTempAmt, cOutData);
    ulRet = CTlvFun::TLVFormatAndUpdateList(
            d_TAG_TRANS_QUICKCHIP_ACTURAL_AMT,
            6,
            cOutData.GetBuffer(),
            &transParam.usRequestDataLen,
            &transParam.ucaRequestData[0]);
    TraceMsg("Info: Call IsTxnSeqNbrMatched TLVFormatAndUpdateList <d_TAG_TRANS_QUICKCHIP_ACTURAL_AMT> %s.", ulRet==0?"OK":"FAIL");

    TraceBytes(transParam.ucaRequestData, transParam.usRequestDataLen, "transParam:");
    TraceBytes(pollData.ucaRequestData, pollData.usRequestDataLen, "pollData:");
    
    InitTagParam(cTagParam, kHLTransParam, transParam);
    ulRet = m_pParamPtr->SetParameter(cTagParam);
    TraceMsg("Info: Call IsTxnSeqNbrMatched set HEARTLAND_TRANS_PARAM %s.", ulRet==0?"OK":"FAIL");
    
    InitTagParam(cTagParam, kPollingCardCache, pollData);
    memset((void *)&pollData, 0, sizeof(pollData));
    ulRet = m_pParamPtr->SetParameter(cTagParam);
    TraceMsg("Info: Call IsTxnSeqNbrMatched set HEARTLAND_POLLINGCARD_CACHE %s.", ulRet==0?"OK":"FAIL");

    TraceMsg("Info: %s Exit", __FUNCTION__);
}

    
void CTxnEmvcl::GetOfflineTxnResult(void)
{
    TraceMsg("GetOfflineTxnResult Entry");

    BYTE    byTxnResult = d_TXN_OFFLINE_DECLINED;
    tagParam cTagParam = { 0 };
    HEARTLAND_TRANS_PARAM transParam = { 0 };

    InitTagParam(cTagParam, kHLTransParam, transParam);
    unsigned long ulRet = m_pParamPtr->GetParameter(&cTagParam);
    TraceMsg("Info: Call IsTxnSeqNbrMatched get HEARTLAND_TRANS_PARAM %s.", ulRet==0?"OK":"FAIL");

    unsigned int uState = GetState();
    if (uState == TXN_STATE_OFFLINE_APPROVED)
    {
        byTxnResult = d_TXN_OFFLINE_APPROVED;
    }
    else if(uState == TXN_STATE_OFFLINE_DECLINED)
    {
        byTxnResult = d_TXN_OFFLINE_DECLINED;
    }
    
    CTlvFun::TLVFormatAndUpdateList(d_TAG_TXN_RESULT, 1, &byTxnResult, &transParam.bResultLen, transParam.bResult);
    ulRet = m_pParamPtr->SetParameter(cTagParam);
    TraceMsg("TxnResult byTxnResult:%d, ulRet:%ld", byTxnResult, ulRet);

    TraceMsg("GetOfflineTxnResult exit");
}