
/* 
 * File:   TransBatchClose.cpp
 * Author: Tinker Han
 * 
 * Created on 2018年8月7日, 上午10:43
 */

#include "../ByteStream.h"
#include "../TxnDef.h"
#include "../TlvFun.h"
#include "../TxnTag.h"  
#include "../Inc/ErrorCode.h"
#include "TransBatchClose.h"


/**
* Function:
*      TransBatchClose's Construct Function:
*
* @param
*      IParameter*   :  the parameters of the pool
*      HeartlandSoap*:  object handle for packing and unpacking data
*
* @return
*      Void.
*
* Author: Tim.Ma
*/
TransBatchClose::TransBatchClose(IN IParameter* pcTransParam, IN HeartlandSoap* pcSoapKernel)
:
m_pcSoapKernel(pcSoapKernel),
m_cFillTxnData(pcSoapKernel, NULL),
m_nSoapTxnType(tt_BATCH_CLOSE),
CBaseSoap(pcTransParam)
{
}


/**
* Function:
*      TransBatchClose's Destruct Function:
*
* @param
*      none
*
* @return
*      none.
*
* Author: Tim.Ma
*/
TransBatchClose::~TransBatchClose()
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
int TransBatchClose::DoPack(OUT CByteStream *pcPackData)
{
    HEARTLAND_GENERAL_PARAM generalParam = { 0 };
    ST_HSOAP_REQUEST cHsoapRequest = { 0 };
    cHsoapRequest.nTransactionType = m_nSoapTxnType;   
    tagParam cTagParam = { 0 };
    int  nSoapLen = 4096;
    char szRequestSoap[4096] = { 0 };

    InitTagParam(cTagParam, kGeneralParam, generalParam);
    int nRet = m_pcParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter Failed:%d",  nRet);
        return nRet;
    }

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
int TransBatchClose::DoUnpack(const CByteStream &cData)
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
 *      parse pcHsoapResponse into m_pcTransParam->bResult
 *
 * @param:
 *      [IN]pcHsoapResponse: the response soap structure point
 *
 * @return:
 *      OK  -   GetSoapData successful
 *      other - GetSoapData failed, refer to ErrorCode.h
 */
WORD TransBatchClose::GetSoapData(ST_HSOAP_RESPONSE* pcHsoapResponse, HEARTLAND_TRANS_PARAM* pcTransParam)
{
    TraceMsg("GetSoapData--Entry");
    
    do
    {
        if (NULL == pcHsoapResponse)
        {
            TraceMsg("ERROR: pcHsoapResponse is NULL");
            return ERROR_INVALID_PARAM;
        }
        
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RESPONSE_CODE,            pcHsoapResponse->cGateway.szGatewayRspCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RESPONSE_TXT,             pcHsoapResponse->cGateway.szGatewayRspMsg);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESP_DT,            pcHsoapResponse->cGateway.szRspDT);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_LICENSE_IDENTITY,         pcHsoapResponse->cGateway.szLicenseId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_SITE_IDENTITY,            pcHsoapResponse->cGateway.szSiteId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_DEVICE_IDENTITY,          pcHsoapResponse->cGateway.szDeviceId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANSACTION_IDENTITY,     pcHsoapResponse->cGateway.szGatewayTxnId);

        char chTxnResult = d_TXN_APPROVED;
        if (memcmp(pcHsoapResponse->cGateway.szGatewayRspCode, d_GATEWAY_RSP_CODE_SUCCESS, 1) != 0)
            chTxnResult = d_TXN_DECLINED;
        
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_RESULT,               &chTxnResult, 1);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_CODE,      pcHsoapResponse->cTrans.szRspCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_TXT,       pcHsoapResponse->cTrans.szRspText);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TOTAL_SALE_COUNT,         pcHsoapResponse->cTrans.szTxnCnt);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TOTAL_SALE_AMOUNT,        pcHsoapResponse->cTrans.szTotalAmt);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_BATCH_NUMBER,             pcHsoapResponse->cTrans.szBatchSeqNbr);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_BATCH_IDENTITY,           pcHsoapResponse->cTrans.szBatchId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_UNIQUE_DEVICE_ID,         pcHsoapResponse->cGateway.szUniqueDeviceId);
        
    }while (0);
    
    TraceMsg("GetSoapData--Exit");
    return 0;
}
