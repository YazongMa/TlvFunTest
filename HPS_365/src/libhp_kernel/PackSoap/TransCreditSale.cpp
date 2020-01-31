
/* 
 * File:   TransCreditSale.cpp
 * Author: Tim.Ma
 * 
 * Created on 2018/8/2日, 14:05
 */

#include "TransCreditSale.h"

#include "../TxnDef.h"
#include "../TlvFun.h"
#include "../StringFun.h"
#include "../FileFun.h"
#include "../FunBase.h"
#include "../Inc/ErrorCode.h"

/**
* Function:
*      TransCreditSale's Construct Function:
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
TransCreditSale::TransCreditSale(IN IParameter* pcTransParam, IN HeartlandSoap* pcSoapKernel, IN CVoltageObject* pcVoltageObj)
:
CBaseSoap(pcTransParam),
m_pcSoapKernel(pcSoapKernel),
m_cFillTxnData(pcSoapKernel, pcVoltageObj),
m_nSoapTxnType(tt_CREDIT_SALE)
{
}

/**
* Function:
*      TransCreditSale's Destruct Function:
*
* @param
*      none
*
* @return
*      none.
*
* Author: Tim.Ma
*/
TransCreditSale::~TransCreditSale()
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
int TransCreditSale::DoPack(OUT CByteStream *pcPackData)
{
    if(NULL == pcPackData)
    {
        TraceMsg("ERROR: param is NULL");
        return -1;
    }

    HEARTLAND_TRANS_PARAM transParam = { 0 };
    HEARTLAND_GENERAL_PARAM generalParam = { 0 };
    ST_HSOAP_REQUEST cHsoapRequest = { 0 };
    tagParam cTagParam = { 0 };
    int  nSoapLen = 4096;
    char szRequestSoap[4096] = { 0 };

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
        TraceMsg("Unknow cardSource:%02X, interface:%d", transParam.bARQCType, cHsoapRequest.nTransactionInterface);
        break;
    }
    
    m_cFillTxnData.SetTrackData(&cHsoapRequest, &transParam, d_CREDIT_CARD_TYPE);
    m_cFillTxnData.SetAmt(&cHsoapRequest, &transParam);
    m_cFillTxnData.SetGratuityAmtInfo(&cHsoapRequest, &transParam);
    m_cFillTxnData.SetAllowDupInfo(&cHsoapRequest, &transParam);
    m_cFillTxnData.SetEMVData(&cHsoapRequest, &transParam);
    m_cFillTxnData.SetTagData(&cHsoapRequest, &transParam);
    m_cFillTxnData.SetUniqueDeviceId(&cHsoapRequest, &generalParam);
    m_cFillTxnData.SetTokenRequest(&cHsoapRequest, GetTokenFlag());
    m_cFillTxnData.SetTokenValue(&cHsoapRequest, &transParam, d_CREDIT_CARD_TYPE);

    if (tt_CREDIT_SALE == m_nSoapTxnType || tt_CREDIT_AUTH == m_nSoapTxnType)
    {
        m_cFillTxnData.SetAllowPartialAuth(&cHsoapRequest, &transParam);
        m_cFillTxnData.SetTransactionDescriptor(&cHsoapRequest, &generalParam);
    }

    nRet = m_pcSoapKernel->HeartlandSOAP_Pack(&cHsoapRequest, &nSoapLen, szRequestSoap);
    if (nRet)
    {
        TraceMsg("HeartlandSOAP_Pack Return: 0x%X", nRet);
        return nRet;
    }
    
    pcPackData->Write(szRequestSoap, nSoapLen);
    pcPackData->PushZero();

    if (tt_CREDIT_SALE == m_nSoapTxnType ||
        tt_CREDIT_AUTH == m_nSoapTxnType)
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
int TransCreditSale::DoUnpack(const CByteStream &cData)
{
    if (cData.GetLength() == 0)
    {
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
WORD TransCreditSale::GetSoapData(ST_HSOAP_RESPONSE* pcHsoapResponse, HEARTLAND_TRANS_PARAM* pcTransParam)
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
        
        char chTxnResult = d_TXN_APPROVED;        
        if (memcmp(pcHsoapResponse->cGateway.szGatewayRspCode, d_TXN_RSP_CODE_REVERSAL, 2) == 0 ||
            memcmp(pcHsoapResponse->cTrans.szRspCode, d_TXN_RSP_CODE_NOREPLY, 2) == 0)
            chTxnResult = d_TXN_HOST_TIMEOUT;
        else if (memcmp(pcHsoapResponse->cTrans.szRspCode, d_TXN_RSP_CODE_SUCCESS, 2) != 0)
            chTxnResult = d_TXN_DECLINED;
        
        TraceMsg("CreditSale GetwayTxnId is %s", pcHsoapResponse->cGateway.szGatewayTxnId);
        TraceMsg("CreditSale EMVIssuerResp is %s", pcHsoapResponse->cTrans.szEMVIssuerResp);
        TraceMsg("CreditSale AuthAmt is %s", pcHsoapResponse->cTrans.szAuthAmt);
        TraceMsg("CreditSale TxnResult is %d", chTxnResult);
        
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_RESULT,                   &chTxnResult, 1);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_CODE,          pcHsoapResponse->cTrans.szRspCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_TXT,           pcHsoapResponse->cTrans.szRspText);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_APPROVED_CODE,                pcHsoapResponse->cTrans.szAuthCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_AMT_AUTH,                 pcHsoapResponse->cTrans.szAuthAmt);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RETRIEVAL_REFERENCE_NUMBER,   pcHsoapResponse->cTrans.szRefNbr);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_CARD_TYPE,                (char *)"CREDIT CARD");
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_POS_TXN_TYPE,                 (char *)"PURCHASE");
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_CARD_BRAND,                   pcHsoapResponse->cTrans.szCardType);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANSACTION_DESCRIPTOR,       pcHsoapResponse->cTrans.szTxnDescriptor);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_EMV_ISSUER_RESP,        pcHsoapResponse->cTrans.szEMVIssuerResp);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_TOKEN_VALUE,            pcHsoapResponse->cGateway.szTokenValue);
    } while (0);

    TraceMsg("exit");
    return 0;
}


/**
* GetTokenFlag
*
* Function:
*      GetTokenFlag, check if use token 
*
* @param:
*      [none]
*
* @return:
*      0 -  don't use token
*      1 -  use token
**/
int TransCreditSale::GetTokenFlag(void)
{
    HEARTLAND_GENERAL_PARAM tokenParam = { 0 };
    tagParam cTagParam = { 0 };

    InitTagParam(cTagParam, kGeneralParam, tokenParam);
    int nRet = m_pcParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        printf("GetParameter Failed:%d\n",  nRet);
        return 0;
    }
    
    if (0 == strcasecmp((char *)tokenParam.szTokenizationFlag, "true"))
    {
        return 1;
    }

    return 0;
}
