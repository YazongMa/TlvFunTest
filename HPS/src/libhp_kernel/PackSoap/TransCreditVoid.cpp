
/* 
 * File:   TransCreditVoid.cpp
 * Author: Tim.Ma
 * 
 * Created on 2018/8/2日, 14:05
 */

#include "TransCreditVoid.h"

#include "../TxnDef.h"
#include "../TlvFun.h"
#include "../StringFun.h"
#include "../FunBase.h"
#include "../Inc/ErrorCode.h"


/**
* Function:
*      TransCreditVoid's Construct Function:
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
TransCreditVoid::TransCreditVoid(IN IParameter* pcTransParam, IN HeartlandSoap* pcSoapKernel, IN CVoltageObject* pcVoltageObj)
:
CBaseSoap(pcTransParam),
m_pcSoapKernel(pcSoapKernel),
m_cFillTxnData(pcSoapKernel, pcVoltageObj),
m_nSoapTxnType(tt_CREDIT_VOID)
{
}


/**
* Function:
*      TransCreditVoid's Destruct Function:
*
* @param
*      none
*
* @return
*      none.
*
* Author: Tim.Ma
*/
TransCreditVoid::~TransCreditVoid()
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
int TransCreditVoid::DoPack(OUT CByteStream *pcPackData)
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
        TraceMsg("Unknow cardSource = %02X", transParam.bARQCType);
        break;
    }


    if (tt_CREDIT_ADD_TO_BATCH == m_nSoapTxnType)
    {
        m_cFillTxnData.SetAmt(&cHsoapRequest, &transParam);
    }
    m_cFillTxnData.SetGatewayTxnId(&cHsoapRequest, &transParam);
    m_cFillTxnData.SetUniqueDeviceId(&cHsoapRequest, &generalParam);

    nRet = m_pcSoapKernel->HeartlandSOAP_Pack(&cHsoapRequest, &nSoapLen, szRequestSoap);
    if (nRet)
    {
        TraceMsg("HeartlandSOAP_Pack Return: 0x%X", nRet);
        return nRet;
    }
    
    pcPackData->Write(szRequestSoap, nSoapLen);
    pcPackData->PushZero();
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
int TransCreditVoid::DoUnpack(const CByteStream &cData)
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
        TraceMsg("HeartlandSOAP_Pack Return: 0x%X", nRet);
        return nRet;
    }

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
WORD TransCreditVoid::GetSoapData(ST_HSOAP_RESPONSE* pcHsoapResponse, HEARTLAND_TRANS_PARAM* pcTransParam)
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
        if (memcmp(pcHsoapResponse->cGateway.szGatewayRspCode, d_GATEWAY_RSP_CODE_SUCCESS, 1) != 0)
            chTxnResult = d_TXN_DECLINED;
        TraceMsg("CreditSale AuthAmt is %s", pcHsoapResponse->cTrans.szAuthAmt);
        
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_RESULT,                 &chTxnResult, 1);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_CODE,        pcHsoapResponse->cTrans.szRspCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_TXT,         pcHsoapResponse->cTrans.szRspText);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_APPROVED_CODE,              pcHsoapResponse->cTrans.szAuthCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_AMT_AUTH,               pcHsoapResponse->cTrans.szAuthAmt);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RETRIEVAL_REFERENCE_NUMBER, pcHsoapResponse->cTrans.szRefNbr);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_CARD_TYPE,              (char *)"CREDIT CARD");
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_POS_TXN_TYPE,               (char *)"VOID");
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_CARD_BRAND,                 pcHsoapResponse->cTrans.szCardType);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANSACTION_DESCRIPTOR,     pcHsoapResponse->cTrans.szTxnDescriptor);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_TOKEN_VALUE,          pcHsoapResponse->cGateway.szTokenValue);
    }while (0);
    
    
    TraceMsg("exit");
    return 0;
}