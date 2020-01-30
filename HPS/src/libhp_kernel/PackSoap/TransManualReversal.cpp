
/* 
 * File:   TransManualReversal.cpp
 * Author: Tim.Ma
 * 
 * Created on 2019/9/10, 13:42
 */


#include <unistd.h>
#include <errno.h>

#include "../Inc/ErrorCode.h"
#include "../FileFun.h"
#include "../TxnDef.h"
#include "../TxnTag.h"
#include "../dbghlp.h"
#include "../Inc/TxnDataType.h"
#include "TransManualReversal.h"

TransManualReversal::TransManualReversal(IN IParameter *pcTransParam, IN HeartlandSoap *pcSoapKernel, IN int nCardType)
:
m_pcParamPtr(pcTransParam),
m_pcSoapKernel(pcSoapKernel),
m_cFillTxnData(pcSoapKernel, NULL),
m_nCardType(nCardType),
CBaseSoap(pcTransParam)
{
}


/**
* ~TransManualReversal
*  
* Function:
*          Destruct. 
* 
* @param:
*      None.
* 
* @return:
*      none
**/
TransManualReversal::~TransManualReversal(void)
{
}


int TransManualReversal::DoPack(OUT CByteStream *pcPackData)
{
    char szRequestData[4096] = { 0 };
    int nLength = sizeof(szRequestData);
    ST_HSOAP_REQUEST cSoapRequest = { 0 };
    
    // fill request data;
    FillRequestData(&cSoapRequest);
    long lRet = m_pcSoapKernel->HeartlandSOAP_Pack(&cSoapRequest, &nLength, szRequestData);
    if (lRet != 0)
    {
        TraceMsg("HeartlandSOAP_Pack : %ld", lRet);
        return lRet;
    }
    
    pcPackData->Empty();
    pcPackData->Write((BYTE *)szRequestData, nLength);
    pcPackData->PushZero();
    
    return 0;
}


int TransManualReversal::DoUnpack(const CByteStream &cData)
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


void TransManualReversal::FillRequestData(ST_HSOAP_REQUEST* pcSoapRequest)
{
    tagParam cParamVar = {0};
    HEARTLAND_AMOUNT_PARAM cAmtParam;
    int iOffset = 0;
    unsigned char tmpAmtSting[16] = { 0 };

    InitTagParam(cParamVar, kTxnAmountData, cAmtParam);
    int nRet = m_pcParamPtr->GetParameter(&cParamVar);
    if (nRet)
    {
        TraceMsg("Error: Call FillRequestData get kHLTransParam failed.");
        return ;
    }

    // transaction type， please refer to JsonCmdProp.h in libhp_api
    if (m_nCardType == 2)
    {
        pcSoapRequest->nTransactionType = tt_DEBIT_REVERSAL;
    }
    else
    {
        pcSoapRequest->nTransactionType = tt_CREDIT_REVERSAL;
    }

    // GatewayTxnId
    sprintf(pcSoapRequest->szGatewayTxnId, "%.*s", sizeof(pcSoapRequest->szGatewayTxnId) - 1, cAmtParam.szGatewayIdBuf);

    // original amount
    sprintf((char *)tmpAmtSting, "%012lld", cAmtParam.ullAmount);
    memcpy(&pcSoapRequest->szAmt[iOffset], tmpAmtSting, strlen((char *)tmpAmtSting) - 2);
    iOffset += (strlen((char *)tmpAmtSting) - 2);
    pcSoapRequest->szAmt[iOffset++] = '.';
    memcpy(&pcSoapRequest->szAmt[iOffset], &tmpAmtSting[strlen((char *)tmpAmtSting) - 2], 2);

    // auth amount
    iOffset = 0;
    sprintf((char *)tmpAmtSting, "%012lld", cAmtParam.ullOtherAmount);
    memcpy(&pcSoapRequest->szAuthAmt[iOffset], tmpAmtSting, strlen((char *)tmpAmtSting) - 2);
    iOffset += (strlen((char *)tmpAmtSting) - 2);
    pcSoapRequest->szAuthAmt[iOffset++] = '.';
    memcpy(&pcSoapRequest->szAuthAmt[iOffset], &tmpAmtSting[strlen((char *)tmpAmtSting) - 2], 2);
    
    // clear transParam
    HEARTLAND_TRANS_PARAM transParam = { 0 };

    InitTagParam(cParamVar, kHLTransParam, transParam);
    nRet = m_pcParamPtr->SetParameter(cParamVar);
    if (nRet != 0)
    {
        TraceMsg("SetParameter Failed:%d",  nRet);
    }
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
*      OK -  Successful
*      Other - Please refer to ErrorCode.h
**/
WORD TransManualReversal::GetSoapData(ST_HSOAP_RESPONSE* pcHsoapResponse, HEARTLAND_TRANS_PARAM* pcTransParam)
{
    TraceMsg("entry");
    
    do
    {
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RESPONSE_CODE,              pcHsoapResponse->cGateway.szGatewayRspCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RESPONSE_TXT,               pcHsoapResponse->cGateway.szGatewayRspMsg);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESP_DT,              pcHsoapResponse->cGateway.szRspDT);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_LICENSE_IDENTITY,           pcHsoapResponse->cGateway.szLicenseId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_SITE_IDENTITY,              pcHsoapResponse->cGateway.szSiteId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_DEVICE_IDENTITY,            pcHsoapResponse->cGateway.szDeviceId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANSACTION_IDENTITY,       pcHsoapResponse->cGateway.szGatewayTxnId);
        
        char chTxnResult = d_TXN_APPROVED;
        if (memcmp(pcHsoapResponse->cTrans.szRspCode, d_TXN_RSP_CODE_SUCCESS, 2) != 0)
            chTxnResult = d_TXN_DECLINED;
        
        TraceMsg("CreditSale GetwayTxnId is %s", pcHsoapResponse->cGateway.szGatewayTxnId);
        
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_RESULT,                 &chTxnResult, 1);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_CODE,        pcHsoapResponse->cTrans.szRspCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_TXT,         pcHsoapResponse->cTrans.szRspText);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_APPROVED_CODE,              pcHsoapResponse->cTrans.szAuthCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RETRIEVAL_REFERENCE_NUMBER, pcHsoapResponse->cTrans.szRefNbr);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_CARD_TYPE,              pcHsoapResponse->cTrans.szCardType);
    }while (0);
    
    TraceMsg("exit");
    return 0;
}