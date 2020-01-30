
/* 
 * File:   TransSAF.cpp
 * Author: Charlie.Zhao
 * 
 * Created on 2019年11月11日, 上午9:30
 */

#include "TransSAF.h"
#include "../Inc/ErrorCode.h"

TransSAF::TransSAF(IN IParameter *pcTransParam, IN HeartlandSoap *pcSoapKernel, IN CVoltageObject *pcVoltageObj)
:
CBaseSoap(pcTransParam),
m_pcSoapKernel(pcSoapKernel),
m_cFillTxnData(pcSoapKernel, pcVoltageObj),
m_nSoapTxnType(tt_CREDIT_SALE)
{
    memset(m_szTxnType, 0x00, sizeof(m_szTxnType));
    memset(m_szTxnAmount, 0x00, sizeof(m_szTxnAmount));
    memset(m_szTxnSafSequenceID, 0x00, sizeof(m_szTxnSafSequenceID));
}


TransSAF::~TransSAF()
{
}

int TransSAF::DoPack(OUT CByteStream *pcPackData)
{
    TraceMsg("%s Entry", __FUNCTION__);

    if(NULL == pcPackData)
    {
        TraceMsg("ERROR: param is NULL");
        return -1;
    }

    HEARTLAND_AMOUNT_PARAM cAmtParam;
    tagParam cTagParam = { 0 };

    InitTagParam(cTagParam, kTxnAmountData, cAmtParam);
    int nRet = m_pcParamPtr->GetParameter(&cTagParam);
    if (nRet)
    {
        TraceMsg("GetParameter Failed:%d",  nRet);
        return nRet;
    }

    if (strlen(cAmtParam.szTxnRequestData) == 0 ||
        cAmtParam.nTxnReqDataLength == 0)
    {
        TraceMsg("Invalid txnRequestData, it's empty");
        return 1;
    }

    memset(m_szTxnType, 0x00, sizeof(m_szTxnType));
    memset(m_szTxnAmount, 0x00, sizeof(m_szTxnAmount));
    memset(m_szTxnSafSequenceID, 0x00, sizeof(m_szTxnSafSequenceID));

    memcpy(m_szTxnType, cAmtParam.szTxnRequestData, 2);
    memcpy(m_szTxnAmount, cAmtParam.szTxnRequestData + 2, 20);
    memcpy(m_szTxnSafSequenceID, cAmtParam.szTxnSafSequenceID, strlen(cAmtParam.szTxnSafSequenceID));
    sprintf(m_szTxnAmount,"$%lld.%02lld", atoll(m_szTxnAmount)/100, atoll(m_szTxnAmount)%100);
    pcPackData->Write(cAmtParam.szTxnRequestData + 22, cAmtParam.nTxnReqDataLength - 22);
    pcPackData->PushZero();

    TraceMsg("m_szTxnType: %d", atoi(m_szTxnType));
    TraceMsg("m_szTxnAmount: %s", m_szTxnAmount);
    TraceMsg("%s Exit", __FUNCTION__);
    return 0;
}

int TransSAF::DoUnpack(const CByteStream &cData)
{
    TraceMsg("Info: %s Entry", __FUNCTION__);
    if (cData.GetLength() == 0)
    {
        TraceMsg("Error: %s cData invalid", __FUNCTION__);
        return -1;
    }
    
    ST_HSOAP_RESPONSE cHostResponse = { 0 };
    HEARTLAND_TRANS_PARAM transParam = { 0 };
    tagParam cTagParam = { 0 };

    int nRet = m_pcSoapKernel->HeartlandSOAP_Unpack(&cHostResponse, (char *)cData.GetBuffer());
    if (nRet)
    {
        TraceMsg("Error: HeartlandSOAP_Pack Return: 0x%X", nRet);
        return nRet;
    }

    InitTagParam(cTagParam, kHLTransParam, transParam);
    nRet = m_pcParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("Error: GetParameter Failed:%d",  nRet);
        return nRet;
    }

    GetSoapData(&cHostResponse, &transParam);
    m_pcParamPtr->SetParameter(cTagParam);

    TraceMsg("%s Exit", __FUNCTION__);
    return 0;
}


/**
* GetSoapData
*
* Function:
*      Parse pcHsoapResponse into m_pcTransParam->bResult
*
* @param:
*      [IN]pcHsoapResponse -  The response soap structure point.
*
* @return:
*      OK -  Successful
*      Other - Please refer to ErrorCode.h
**/
WORD TransSAF::GetSoapData(ST_HSOAP_RESPONSE* pcHsoapResponse, HEARTLAND_TRANS_PARAM* pcTransParam)
{
    TraceMsg("entry");
    
    do
    {
        if (NULL == pcHsoapResponse)
        {
            TraceMsg("ERROR: pcHsoapResponse is NULL");
            return ERROR_INVALID_PARAM;
        }
        
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RESPONSE_CODE,                pcHsoapResponse->cGateway.szGatewayRspCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RESPONSE_TXT,                 pcHsoapResponse->cGateway.szGatewayRspMsg);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESP_DT,                pcHsoapResponse->cGateway.szRspDT);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_LICENSE_IDENTITY,             pcHsoapResponse->cGateway.szLicenseId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_SITE_IDENTITY,                pcHsoapResponse->cGateway.szSiteId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_DEVICE_IDENTITY,              pcHsoapResponse->cGateway.szDeviceId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANSACTION_IDENTITY,         pcHsoapResponse->cGateway.szGatewayTxnId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_UNIQUE_DEVICE_ID,             pcHsoapResponse->cGateway.szUniqueDeviceId);

        if (strlen(m_szTxnSafSequenceID) != 0)
        {
            m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_SAF_SEQUENCEID,     m_szTxnSafSequenceID);
        }
        
        char chTxnResult = d_TXN_APPROVED;
        if (memcmp(pcHsoapResponse->cTrans.szRspCode, d_TXN_RSP_CODE_SUCCESS, 2) != 0)
            chTxnResult = d_TXN_DECLINED;
        else
        {
            //Call CTOS_KMS2UserDataWrite API to update SAFTotalAmount
            BYTE byData[64];
            memset(byData, 0x00, sizeof(byData));
            WORD wRet = CTOS_KMS2UserDataRead(TRUE, 0, byData, 64);
            if (wRet != 0)
            {
                TraceMsg("ERROR: CTOS_KMS2UserDataRead Failed");
                // Write a new file to store amount
                memset(byData, 0x00, sizeof(byData));
                memcpy(byData, "0.00", strlen("0.00"));
                wRet = CTOS_KMS2UserDataWrite(TRUE, 0, byData, 64);
                if (wRet != 0)
                {
                    TraceMsg("ERROR: CTOS_KMS2UserDataWrite Failed");
                }
            }
            else
            {
                // substract txn amount from SAF Total amount and save to file
                double dSAFTotalAmount = atof((const char*)byData);
                double dTxnAmount = atof(&m_szTxnAmount[1]);

                dSAFTotalAmount -= dTxnAmount;
                memset(byData, 0x00, sizeof(byData));

                if (dSAFTotalAmount < 0.001)
                {
                    memcpy(byData, "0.00", strlen("0.00"));
                }
                else
                {
                    sprintf((char*)byData, "%.02f", dSAFTotalAmount);
                }
                TraceMsg("byData: %s, dSAFTotalAmount: %.02f", byData, dSAFTotalAmount);

                wRet = CTOS_KMS2UserDataWrite(TRUE, 0, byData, 64);
                if (wRet != 0)
                {
                    TraceMsg("ERROR: CTOS_KMS2UserDataWrite Failed");
                }
            }
        }
        
        TraceMsg("CreditSale GetwayTxnId is %s", pcHsoapResponse->cGateway.szGatewayTxnId);
        TraceMsg("CreditSale EMVIssuerResp is %s", pcHsoapResponse->cTrans.szEMVIssuerResp);

        if (strlen(pcHsoapResponse->cTrans.szAuthAmt))
        {
            m_cFillTxnData.SetTxnResult(pcTransParam, TAG_AMT_AUTH,                   pcHsoapResponse->cTrans.szAuthAmt);
        }
        else
        {
            m_cFillTxnData.SetTxnResult(pcTransParam, TAG_AMT_AUTH,                   m_szTxnAmount);
        }
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_RESULT,                   &chTxnResult, 1);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_CODE,          pcHsoapResponse->cTrans.szRspCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_TXT,           pcHsoapResponse->cTrans.szRspText);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_APPROVED_CODE,                pcHsoapResponse->cTrans.szAuthCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RETRIEVAL_REFERENCE_NUMBER,   pcHsoapResponse->cTrans.szRefNbr);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_CARD_BRAND,                   pcHsoapResponse->cTrans.szCardType);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANSACTION_DESCRIPTOR,       pcHsoapResponse->cTrans.szTxnDescriptor);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_EMV_ISSUER_RESP,        pcHsoapResponse->cTrans.szEMVIssuerResp);
        int nTxnSoapType = atoi(m_szTxnType);
        if (nTxnSoapType == kCreditSale_SoapType)
        {
            m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_CARD_TYPE,                (char*)"CREDIT CARD");
            m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_TOKEN_VALUE,            pcHsoapResponse->cGateway.szTokenValue);
        }
        else
        {
            m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_CARD_TYPE,                (char*)"DEBIT CARD");
        }
    } while (0);

    TraceMsg("exit");
    return 0;
}
