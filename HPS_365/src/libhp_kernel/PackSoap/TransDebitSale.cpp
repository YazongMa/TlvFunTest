
/* 
 * File:   TransDebitSale.cpp
 * Author: Tim.Ma
 * 
 * Created on 2018/8/2日, 14:05
 */

#include "TransDebitSale.h"

#include "../TxnDef.h"
#include "../TlvFun.h"
#include "../StringFun.h"
#include "../FileFun.h"
#include "../FunBase.h"
#include "../Inc/ErrorCode.h"


/**
* Function:
*      TransDebitSale's Construct Function:
*
* @param
*      IParameter*   :  the parameters of the pool
*      HeartlandSoap*:  object handle for packing and unpacking data
*      CVoltageObject*: object handle for encrypted functions
*
* @return
*      Void.
*
* Author: Tim.Ma
*/
TransDebitSale::TransDebitSale(IN IParameter* pcTransParam, IN HeartlandSoap* pcSoapKernel, IN CVoltageObject* pcVoltageObj)
:
CBaseSoap(pcTransParam),
m_pcSoapKernel(pcSoapKernel),
m_cFillTxnData(pcSoapKernel, pcVoltageObj),
m_nSoapTxnType(tt_DEBIT_SALE)
{
}

/**
* Function:
*      TransDebitSale's Destruct Function:
*
* @param
*      none
*
* @return
*      none.
*
* Author: Tim.Ma
*/
TransDebitSale::~TransDebitSale()
{
}


/**
* Function:
*      DoPack, pack the transaction data
*
* @param
*      CByteStream* : the data packed to send to host
*
* @return
*      0:       success
*      other:   fail, please refer to libstructdefined.h
* Author: Tim.Ma
*/
int TransDebitSale::DoPack(OUT CByteStream *pcPackData)
{
    if(NULL == pcPackData)
    {
        TraceMsg("ERROR: param is NULL");
        return -1;
    }

    HEARTLAND_TRANS_PARAM transParam = { 0 };
    HEARTLAND_GENERAL_PARAM generalParam = { 0 };
    tagParam cTagParam = { 0 };

    InitTagParam(cTagParam, kHLTransParam, transParam);
    int nRet = m_pcParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter Failed:%d",  nRet);
        return nRet;
    }

    InitTagParam(cTagParam, kGeneralParam, generalParam);
    nRet = m_pcParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter Failed:%d",  nRet);
        return nRet;
    }
    
    ST_HSOAP_REQUEST cHsoapRequest = { 0 };
    
    // nTransactionType
    cHsoapRequest.nTransactionType = m_nSoapTxnType;
    
    // nTransactionInterface
    switch (transParam.bARQCType)
    {
    case d_MSR_ARQC:
    case d_ICC_ARQC:
        cHsoapRequest.nTransactionInterface = ti_SWIPE;
        break;

    case d_MSD_ARQC:
    case d_CTLS_ARQC:
        cHsoapRequest.nTransactionInterface = ti_CTLS;
        break;

    default:
        cHsoapRequest.nTransactionInterface = ti_CTLS;
        TraceMsg("Error: Unknow cardSource = %02X", transParam.bARQCType);
        break;
    }
    
    m_cFillTxnData.SetTrackData(&cHsoapRequest, &transParam, d_DEBIT_CARD_TYPE);
    m_cFillTxnData.SetAmt(&cHsoapRequest, &transParam);
    m_cFillTxnData.SetCashBackAmt(&cHsoapRequest, &transParam);
    m_cFillTxnData.SetPINBlock(&cHsoapRequest, &transParam);
    m_cFillTxnData.SetGratuityAmtInfo(&cHsoapRequest, &transParam);
    m_cFillTxnData.SetAllowDupInfo(&cHsoapRequest, &transParam);
    m_cFillTxnData.SetAllowPartialAuth(&cHsoapRequest, &transParam);
    m_cFillTxnData.SetEMVData(&cHsoapRequest, &transParam);
    m_cFillTxnData.SetTagData(&cHsoapRequest, &transParam);
    m_cFillTxnData.SetInteracAccountType(&cHsoapRequest, &transParam);
    m_cFillTxnData.SetInteracPOSSeqNbr(&cHsoapRequest, &transParam);
    m_cFillTxnData.SetUniqueDeviceId(&cHsoapRequest, &generalParam);
    m_cFillTxnData.SetTokenValue(&cHsoapRequest, &transParam, d_DEBIT_CARD_TYPE);
    
    if (tt_DEBIT_SALE == m_nSoapTxnType)
    {
        m_cFillTxnData.SetAllowPartialAuth(&cHsoapRequest, &transParam);
    }

    int  nSoapLen = 4096;
    char szRequestSoap[4096] = { 0 };

    nRet = m_pcSoapKernel->HeartlandSOAP_Pack(&cHsoapRequest, &nSoapLen, szRequestSoap);
    if (nRet)
    {
        TraceMsg("HeartlandSOAP_Pack Return: 0x%X", nRet);
        return nRet;
    }
    
    pcPackData->Write(szRequestSoap, nSoapLen);
    pcPackData->PushZero();

    if (tt_DEBIT_SALE == m_nSoapTxnType)
    {
        m_cFillTxnData.SetReversalData(&cHsoapRequest);
    }
    
    return 0;
}


/**
* Function:
*      DoUnpack, unpack the transaction data that received from the host
*
* @param
*      CByteStream& : the response data unpacked from host
*
* @return
*      0:       success
*      other:   fail, please refer to libstructdefined.h
* Author: Tim.Ma
*/
int TransDebitSale::DoUnpack(const CByteStream &cData)
{
    if (cData.GetLength() == 0)
    {
        return -1;
    }
    
    ST_HSOAP_RESPONSE cHostResponse = { 0 };
    int nRet = m_pcSoapKernel->HeartlandSOAP_Unpack(&cHostResponse, (char *)cData.GetBuffer());
    if (nRet)
    {
        TraceMsg("HeartlandSOAP_Pack Return: 0x%X", nRet);
        return nRet;
    }
    
    HEARTLAND_TRANS_PARAM transParam = { 0 };
    tagParam cTagParam = { 0 };

    InitTagParam(cTagParam, kHLTransParam, transParam);
    nRet = m_pcParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter Failed:%d",  nRet);
        return nRet;
    }

    GetSoapData(&cHostResponse, &transParam);
    m_pcParamPtr->SetParameter(cTagParam);
    
    return 0;
}


/**
* GetSoapData
*
* Function:
*      Parse pcHsoapResponse into m_&transParam->bResult
*
* @param:
*      [IN]pcHsoapResponse -  The response soap structure point.
*
* @return:
*      0 -  Successful
**/
WORD TransDebitSale::GetSoapData(ST_HSOAP_RESPONSE* pcHsoapResponse, HEARTLAND_TRANS_PARAM* pcTransParam)
{
    TraceMsg("entry");
    
    do
    {
        if (NULL == pcHsoapResponse)
        {
            TraceMsg("ERROR: pcHsoapResponse is NULL");
            return ERROR_INVALID_PARAM;
        }
        
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RESPONSE_CODE,              pcHsoapResponse->cGateway.szGatewayRspCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RESPONSE_TXT,               pcHsoapResponse->cGateway.szGatewayRspMsg);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESP_DT,              pcHsoapResponse->cGateway.szRspDT);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_LICENSE_IDENTITY,           pcHsoapResponse->cGateway.szLicenseId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_SITE_IDENTITY,              pcHsoapResponse->cGateway.szSiteId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_DEVICE_IDENTITY,            pcHsoapResponse->cGateway.szDeviceId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANSACTION_IDENTITY,       pcHsoapResponse->cGateway.szGatewayTxnId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_UNIQUE_DEVICE_ID,           pcHsoapResponse->cGateway.szUniqueDeviceId);
        
        char chTxnResult = d_TXN_APPROVED;
        if (memcmp(pcHsoapResponse->cGateway.szGatewayRspCode, d_TXN_RSP_CODE_REVERSAL, 2) == 0 ||
            memcmp(pcHsoapResponse->cTrans.szRspCode, d_TXN_RSP_CODE_NOREPLY, 2) == 0)
            chTxnResult = d_TXN_HOST_TIMEOUT;
        else if (memcmp(pcHsoapResponse->cTrans.szRspCode, d_TXN_RSP_CODE_SUCCESS, 2) != 0 &&
            memcmp(pcHsoapResponse->cTrans.szBankResponseCode, d_TXN_INTERAC_INVALID_POS_SEQNBR, 3)==0 && 
            strlen(pcHsoapResponse->cTrans.szTraceNumber)==12)
            chTxnResult = d_TXN_INTERAC_RETRY_WITH_POS_SEQNBR;
        else if (memcmp(pcHsoapResponse->cTrans.szRspCode, d_TXN_RSP_CODE_SUCCESS, 2) != 0)
            chTxnResult = d_TXN_DECLINED;

        TraceMsg("DebitSale GetwayTxnId is %s", pcHsoapResponse->cGateway.szGatewayTxnId);
        TraceMsg("DebitSale EMVIssuerResp is %s", pcHsoapResponse->cTrans.szEMVIssuerResp);
        TraceMsg("DebitSale AuthAmt is %s", pcHsoapResponse->cTrans.szAuthAmt);
        
        TraceMsg("DebitSale szTransactionCode is %s", pcHsoapResponse->cTrans.szTransactionCode);
        TraceMsg("DebitSale szTransmissionNumber is %s", pcHsoapResponse->cTrans.szTransmissionNumber);
        TraceMsg("DebitSale szBankResponseCode is %s", pcHsoapResponse->cTrans.szBankResponseCode);
        TraceMsg("DebitSale szMacKey is %s", pcHsoapResponse->cTrans.szMacKey);
        TraceMsg("DebitSale szPinKey is %s", pcHsoapResponse->cTrans.szPinKey);
        TraceMsg("DebitSale szFieldKey is %s", pcHsoapResponse->cTrans.szFieldKey);
        TraceMsg("DebitSale szTraceNumber is %s", pcHsoapResponse->cTrans.szTraceNumber);
        TraceMsg("DebitSale szMessageAuthenticationCode is %s", pcHsoapResponse->cTrans.szMessageAuthenticationCode);
        TraceMsg("DebitSale chTxnResult is %X", chTxnResult);
        
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_RESULT,                 &chTxnResult, 1);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_CODE,        pcHsoapResponse->cTrans.szRspCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_TXT,         pcHsoapResponse->cTrans.szRspText);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_APPROVED_CODE,              pcHsoapResponse->cTrans.szAuthCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_AMT_AUTH,               pcHsoapResponse->cTrans.szAuthAmt);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RETRIEVAL_REFERENCE_NUMBER, pcHsoapResponse->cTrans.szRefNbr);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_CARD_BRAND,                 pcHsoapResponse->cTrans.szCardType);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANSACTION_DESCRIPTOR,     pcHsoapResponse->cTrans.szTxnDescriptor);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_EMV_ISSUER_RESP,      pcHsoapResponse->cTrans.szEMVIssuerResp);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_CARD_TYPE,              (char *)"DEBIT CARD");
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_POS_TXN_TYPE,               (char *)"PURCHASE");
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_TRACE_NUMBER,         pcHsoapResponse->cTrans.szTraceNumber);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_BANK_RESPONSE_CODE,   pcHsoapResponse->cTrans.szBankResponseCode);

    }while (0);
    
    
    TraceMsg("exit");
    return 0;
}