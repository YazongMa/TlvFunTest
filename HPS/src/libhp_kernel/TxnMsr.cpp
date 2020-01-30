
/*
 * File Name: TxnMsr.h
 * Author: Tim.Ma
 * 
 * Create Date: 2019.2.28
 */

#include <ctosapi.h>
#include <string.h>

#include "ByteStream.h"
#include "FunBase.h"
#include "Inc/ErrorCode.h"

#include "PackSoap/BaseSoap.h"
#include "Inc/TxnState.h"
#include "StringFun.h"
#include "TxnDef.h"
#include "TlvFun.h"
#include "TxnTag.h"
#include "TxnMsr.h"
#include "dbghlp.h"
#include "CurlObject.h"
#include "Inc/TxnDataType.h"
#include "Inc/UserActFlags.h"
#include "cURL.h"


#ifdef PLATFORM_ANDROID
#include <kmshelper.h>
#endif


/**
* Function:
*      CTxnMsr's Construct Function:
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
CTxnMsr::CTxnMsr(IN IParameter *pParamPtr,
                 IN IDataPack *pDataPackPtr,
                 IN IBaseCurl *pBaseCurlPtr,
                 IN IPinAction *pPinEntryPtr,
                 IN IUserAction *pUserAction)
:
CTxnContext("SWIPE",
        pParamPtr,
        pDataPackPtr,
        pBaseCurlPtr,
        pPinEntryPtr,
        pUserAction)
{
    m_pszPanEntryLegend = "MAG STRIPE";
    ClearAllTrackData();
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
CTxnMsr::~CTxnMsr() { }

#define ResetTrackData(track_name) {\
    bzero(&track_name, sizeof(track_name)); \
    track_name.byErr = d_MSR_NO_DATA; \
}

#define IsTrackDataValid(track_name) ( (d_MSR_SUCCESS == track_name.byErr) && \
(0 != track_name.wDataLen) )


static char* s_szAryReqTag[] = 
{
    TAG_TK2, 
    d_TAG_EPB,
    d_TAG_EPB_KSN
};


static char* s_szAryRspTag[] = 
{
    d_TAG_TRANS_CARD_MASKED_PAN, 
    d_TAG_TRANS_CARD_HOLDER_NAME
};


int CTxnMsr::PerformTxn(void)
{
    int nRet = 1;

    //1. Init related tags for emvcl txn
    InitTransaction();

    //4. Get data from api layer
    CTxnContext::PreProcessing();
    
    SetState(TXN_STATE_READING_CARD);

    if (IsTxnSeqNbrMatched())
    {
        // copy data from polling card cache data
        if (0 != ProcessMatchedTxnSeqNbr())
        {
            SetState(TXN_STATE_DECLINED);
            TraceMsg("Error: ProcessMatchedTxnSeqNbr failed.");
            return -1;
        }
    }
    else
    {
        //2. start reading card
        ReadTrackData();
        // DumpTrackData();

        //3. check card data
        if (!IsTrackDataValid(m_cTrack2))
        {
            SetState(TXN_STATE_READING_CARD_FAIL);
            TraceMsg("Error: Track2 Data:%s, %d, %d is invalid",
                    m_cTrack2.byDataBuf, m_cTrack2.byErr, m_cTrack2.wDataLen);
            return -1;
        }
        
        if (false == CheckTrackData())
        {
            if (TXN_STATE_PLEASE_INSERT_CARD != GetState())
            {
                SetState(TXN_STATE_DECLINED);
            }
            TraceMsg("Error: CheckTrackData failed.");
            return -1;
        }
        
        //5. Retrieve data from card
        if (OK != RetrieveCardData())
        {
        	SetState(TXN_STATE_PREPARING_TXNDATA_FAIL);
            TraceMsg("Error: Get Txn data failed.");
            return -1;
        }

        nRet = PollingCardProcess(kMSR);
        if (TXN_STATE_POLLING_CARD_SUCCESS == nRet)
        {
            TraceMsg("Info: Polling card.");
            return 0;
        }
    }

    //7. Chat with host
    CTxnContext::CommWithHost();
    TraceMsg("m_bNeedCheckSAF: %d", CTxnContext::m_bNeedCheckSAF);
    if (CTxnContext::m_bNeedCheckSAF)
    {
        ExeSAF();
    }

    //8. Related tags process
    nRet = CTxnContext::PostProcessing();
    if (d_TXN_HOST_TIMEOUT == nRet)
    {
        SendReversal();
    }

    return 0;
}

void CTxnMsr::InitTransaction(void)
{
    TraceMsg("INFO: InitTransaction entry");

    CTxnContext::InitParam();

    EnableCancelTxn(FALSE);
    ClearAllTrackData();

    TraceMsg("INFO: InitTransaction exit");
}

void CTxnMsr::ClearAllTrackData(void)
{
    // Clear cache
    ResetTrackData(m_cTrack1);
    ResetTrackData(m_cTrack2);
    ResetTrackData(m_cTrack3);
}

void CTxnMsr::ReadTrackData(void)
{
    WORD wErr;
    WORD wBuf1Len, wBuf2Len, wBuf3Len;

    wBuf1Len = wBuf2Len = wBuf3Len = sizeof (m_cTrack1.byDataBuf);
    wErr = CTOS_MSRRead(m_cTrack1.byDataBuf, &wBuf1Len,
            m_cTrack2.byDataBuf, &wBuf2Len,
            m_cTrack3.byDataBuf, &wBuf3Len);
    if (d_OK != wErr)
    {
        return;
    }

    m_cTrack1.wDataLen = wBuf1Len;
    m_cTrack2.wDataLen = wBuf2Len;
    m_cTrack3.wDataLen = wBuf3Len;
    CTOS_MSRGetLastErr(&m_cTrack1.byErr,
            &m_cTrack2.byErr,
            &m_cTrack3.byErr);

    // format pszTrack2 data
    BYTE byszTrackData[128] = {0};
    WORD wTrackDataLen = 0;
    CStringFun::Track2ToFormatedStr(m_cTrack2.byDataBuf, m_cTrack2.wDataLen, byszTrackData, &wTrackDataLen);

    m_cTrack2.wDataLen = wTrackDataLen;
    m_cTrack2.byDataBuf[wTrackDataLen] = 0x00;
    memcpy(m_cTrack2.byDataBuf, byszTrackData, wTrackDataLen);

    CTxnContext::GetAcctNbr(m_cTrack2.byDataBuf, m_cTrack2.wDataLen);

    // try to get card expire date from track 2 data
    for(int i = 0; i < m_cTrack2.wDataLen; i++)
    {
        if(m_cTrack2.byDataBuf[i] == 0x3D)
        {
            memcpy(m_szCardExpireDate, &m_cTrack2.byDataBuf[i+1], 4);
            break;
        }
    }


    // try to get card holder name from track 1 data
    memset(byszTrackData, 0, sizeof(byszTrackData));
    memcpy(byszTrackData, m_cTrack1.byDataBuf, wBuf1Len);
    if (m_cTrack1.byErr == d_MSR_SUCCESS)
    {
//        TraceBytes(byszTrackData, wBuf1Len, "Track 1 data is");
        BOOL bSign = false;
        for(int i = 0, j=0; i < wBuf1Len; i++)
        {
            if(byszTrackData[i] == 0x5e)//^
                bSign = ~bSign;
            
            if(bSign)
                if(byszTrackData[i] != 0x5e)
                    m_szCardHolderName[j++] = byszTrackData[i];
            else if(j>0)
                break;
        }
    }

    // update card holder information to app layer
    UpdateCardHolderInfo();
}

void CTxnMsr::DumpTrackData(void)
{
    if (IsTrackDataValid(m_cTrack1))
    {
        TraceBytes(m_cTrack1.byDataBuf, m_cTrack1.wDataLen, "Info: Track1 Data=");
    }

    if (IsTrackDataValid(m_cTrack2))
    {
        TraceBytes(m_cTrack2.byDataBuf, m_cTrack2.wDataLen, "Info: Track2 Data=");
    }

    if (IsTrackDataValid(m_cTrack3))
    {
        TraceBytes(m_cTrack3.byDataBuf, m_cTrack3.wDataLen, "Info: Track3 Data=");
    }
}

WORD CTxnMsr::RetrieveCardData(void)
{
    TraceMsg("%s Entry", __FUNCTION__);
    static CByteStream      cOutData;

    int                     nRet;
    tagParam                cTagParam = {0};

    HEARTLAND_TRANS_PARAM   transParam = {0};
    InitTagParam(cTagParam, kHLTransParam, transParam);
    nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter Failed:%d", nRet);
        return ERROR_INVALID_PARAM;
    }

    // track2 data
    nRet = CTlvFun::TLVFormatAndUpdateList(
            TAG_TK2,
            m_cTrack2.wDataLen,
            m_cTrack2.byDataBuf,
            &transParam.usRequestDataLen,
            &transParam.ucaRequestData[0]);
    if (nRet)
    {
        TraceMsg("TLV_FormatAndUpdateList <TAG_TK2> TAG_AMT_OTHER");
        return ERROR_GET_TAGDATA_FAIL;
    }

    // ARQC type
    transParam.bARQCType = d_MSR_ARQC;

    InitTagParam(cTagParam, kHLTransParam, transParam);
    nRet = m_pParamPtr->SetParameter(cTagParam);
    if (nRet != 0)
    {
        TraceMsg("SetParameter Failed:%d", nRet);
        return nRet;
    }

    TraceMsg("%s m_nPackSoapType:%d, kDebitSale_SoapType:%d, kDebitReturn_SoapType:%d", 
            __FUNCTION__, m_nPackSoapType, kDebitSale_SoapType, kDebitReturn_SoapType);
    if (m_nPackSoapType == kDebitSale_SoapType ||
        m_nPackSoapType == kDebitReturn_SoapType)
    {
#ifdef PLATFORM_ANDROID
        CTOS_LCDTClearDisplay();
#endif
        SetState(TXN_STATE_PIN_ENTER_START);

#ifdef PLATFORM_ANDROID
    	CTJNI_KMS_EnableLCD();
#endif
        // Avoid brushing off KMS interfaces due to delays
        usleep(500 * 1000);

        nRet = CTxnContext::GetPinBolck();
		
#ifdef PLATFORM_ANDROID
    	CTOS_LCDTClearDisplay();
    	CTJNI_KMS_DisableLCD();
#endif
        SetState(TXN_STATE_PIN_ENTER_FINISH);
        usleep(500 * 1000);
    }
    
    TraceMsg("%s Exit, nRet:%d", __FUNCTION__, nRet);
    return nRet;
}


bool CTxnMsr::CheckTrackData(void)
{
    bool bRet = true;
    int  nRet = -1;
    BYTE bPosEntry = 0x80;

    tagParam                cTagParam = {0};
    HEARTLAND_TRANS_PARAM   transParam = {0};
    InitTagParam(cTagParam, kHLTransParam, transParam);
    nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter Failed:%d", nRet);
        return ERROR_INVALID_PARAM;
    }

    do
    {
        if (Track1IsEmv((char *)m_cTrack1.byDataBuf, m_cTrack1.wDataLen) ||
            Track2IsEmv((char *)m_cTrack2.byDataBuf, m_cTrack2.wDataLen))
        {
            if (bFallbackFlag_ == FALSE)
            {
                bRet = false; 
                SetState(TXN_STATE_PLEASE_INSERT_CARD);

                TraceMsg("Error: It is an emv card");
                break;
            }

            m_pszPanEntryMode = "FALLBACK";
            bFallbackFlag_ = FALSE;
            bRet = true; 

            if (GetCardBrandFromBinRange() != CARD_BRAND_DISCOVER)
            {
                nRet = CTlvFun::TLVFormatAndUpdateList(TAG_POS_ENTRY_MODE, 1, &bPosEntry,
                    &transParam.usRequestDataLen, &transParam.ucaRequestData[0]);
                TraceMsg("TLV_FormatAndUpdateList <TAG_POS_ENTRY_MODE> %s", nRet==0 ? "success" : "fail");
            }
            break;
        }
    } while(0);

    nRet = m_pParamPtr->SetParameter(cTagParam);
    TraceMsg("SetParameter %s", nRet==0 ? "success" : "fail");

    return bRet;
}


// This was helpful:
// http://blog.opensecurityresearch.com/2012/02/deconstructing-credit-cards-data.html

bool CTxnMsr::Track1IsEmv(const char *pszTrack1, size_t nTrack1Len)
{
    int delim_cnt = 0;
    int i;
    for (i = 0; i < nTrack1Len; i++)
    {
        if (pszTrack1[i] == '^') delim_cnt++;
        if (delim_cnt == 2)
        {
            // after finding the second delimeter, we should have enough
            // space for YYMMSSS, where YYMM is the expiry year/month and
            // SSS is the service code.
            if (nTrack1Len - i >= 5)
            {
                if (pszTrack1[i + 5] == '2' || pszTrack1[i + 5] == '6')
                {
                    TraceMsg("track 1 service code indicates an EMV card\n");
                    return true;
                }
            }
            // not enough data after delimiter to contain service code
            TraceMsg("track 1 after the delimiter is too short to contain a service code\n");
            return false;
        }
    }
    TraceMsg("pszTrack1 did not contain enough delimiter to detect a service code\n");
    return false;
}

bool CTxnMsr::Track2IsEmv(const char *pszTrack2, size_t nTrack2Len)
{
    // Skip start sentinel / end sentinel / CRC.
    pszTrack2++;
    nTrack2Len -= 3;
    int i;
    // parse and mask the PAN. First find the equals sign. Everything
    // before it is the PAN.
    for (i = 0; i < nTrack2Len; i++)
        if (pszTrack2[i] == '=') break;
    if (i < nTrack2Len)
    {
        // we found it, just make sure track has enough characters for
        // mask. If offsets check out, we've also found the expiry date
        // and (importantly) service code. Service code is used to check
        // if this is an EMV card or not.
        if (nTrack2Len - i >= 7)
        {
            if (pszTrack2[i + 5] == '2' || pszTrack2[i + 5] == '6')
            {
                TraceMsg("track 2 service code indicates an EMV card\n");
                return true;
            }
        }
        TraceMsg("track 2 after the delimiter is too short to contain a service code\n");
        return false;
    }
    TraceMsg("track 2 did not contain a delimiter\n");
    return false;
}


void CTxnMsr::SendReversal(void)
{
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
}


WORD CTxnMsr::ProcessMatchedTxnSeqNbr(void)
{
    TraceMsg("%s Entry", __FUNCTION__);
    int nRet = 0;

    // TAG_TK2
    // d_TAG_EPB
    // d_TAG_EPB_KSN
    tagParam cTagParam = { 0 };
    HEARTLAND_TRANS_PARAM transParam;
    HEARTLAND_POLLINGCARD_CACHE pollData = { 0 };

    InitTagParam(cTagParam, kPollingCardCache, pollData);
    nRet = m_pParamPtr->GetParameter(&cTagParam);
    TraceMsg("Error: Call ProcessMatchedTxnSeqNbr get HEARTLAND_POLLINGCARD_CACHE %s.", nRet==0?"OK":"FAIL");

    InitTagParam(cTagParam, kHLTransParam, transParam);
    nRet = m_pParamPtr->GetParameter(&cTagParam);
    TraceMsg("Error: Call ProcessMatchedTxnSeqNbr get HEARTLAND_TRANS_PARAM %s.", nRet==0?"OK":"FAIL");

    transParam.bARQCType = d_MSR_ARQC;
    CopyDataFromCache(s_szAryReqTag, sizeof(s_szAryReqTag) / sizeof(s_szAryReqTag[0]), 
            &transParam.usRequestDataLen, &transParam.ucaRequestData[0], 
            pollData.usRequestDataLen, &pollData.ucaRequestData[0]);

    CopyDataFromCache(s_szAryRspTag, sizeof(s_szAryRspTag) / sizeof(s_szAryRspTag[0]), 
            &transParam.bResultLen, &transParam.bResult[0], 
            pollData.usRequestDataLen, &pollData.ucaRequestData[0]);
    m_pParamPtr->SetParameter(cTagParam);
    
    memcpy(m_szAcctNbr, pollData.ucaCardNumber, strlen((char *)pollData.ucaCardNumber));
    InitTagParam(cTagParam, kPollingCardCache, pollData);
    memset((void *)&pollData, 0, sizeof(pollData));
    m_pParamPtr->SetParameter(cTagParam);
    
    TraceMsg("Info: Call ProcessMatchedTxnSeqNbr get m_szAcctNbr:%s, %s.", m_szAcctNbr, pollData.ucaCardNumber);
    // enter PIN
    if (m_nPackSoapType == kDebitSale_SoapType ||
        m_nPackSoapType == kDebitReturn_SoapType)
    {
        SetState(TXN_STATE_PIN_ENTER_START);
        // m_cCallUserAction.ReqUserInputEMVPIN(TRUE);
        // Avoid brushing off KMS interfaces due to delays
        usleep(500 * 1000);
        
        nRet = CTxnContext::GetPinBolck();
        // m_cCallUserAction.ReqUserInputEMVPIN(FALSE);
        SetState(TXN_STATE_PIN_ENTER_FINISH);
        usleep(500 * 1000);
    }

    TraceMsg("%s Exit, nRet:%d", __FUNCTION__, nRet);
    return nRet;
}