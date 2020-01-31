
/* 
 * File:   TransReversal.cpp
 * Author: Tim.Ma
 * 
 * Created on 2019/3/28, 13:42
 */


#include <unistd.h>
#include <errno.h>

#include "../Inc/ErrorCode.h"
#include "../FileFun.h"
#include "../TxnDef.h"
#include "../TxnTag.h"
#include "../dbghlp.h"
#include "../Inc/TxnDataType.h"
#include "TransReversal.h"


/**
* Function:
*      TransReversal's Construct Function:
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
TransReversal::TransReversal(IN IParameter *pcTransParam, IN HeartlandSoap *pcSoapKernel)
:
m_pcParamPtr(pcTransParam),
m_pcSoapKernel(pcSoapKernel),
m_cFillTxnData(pcSoapKernel, NULL),
CBaseSoap(pcTransParam)
{
}


/**
* Function:
*      TransReversal's Destruct Function:
*
* @param
*      none
*
* @return
*      none.
*
* Author: Tim.Ma
*/
TransReversal::~TransReversal(void)
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
int TransReversal::DoPack(OUT CByteStream *pcPackData)
{
    bool bRet = CFileFun::IsFileExist(HEARTLAND_REVERSAL_FILE_NAME);
    if (bRet == false)
    {
        return 0;
    }
    
    char szRequestData[8192] = { 0 };
    int nLength = sizeof(szRequestData);
    long lRet = CFileFun::ReadFile(HEARTLAND_REVERSAL_FILE_NAME, szRequestData, &nLength, "rb");
    ST_HSOAP_REQUEST cSoapRequest = { 0 };

    if (lRet != 0)
    {
        TraceMsg("ReadFile : %ld", lRet);
        return 0;
    }
    
    memcpy((void *)&cSoapRequest, szRequestData, sizeof(ST_HSOAP_REQUEST));
    memset(szRequestData, 0, sizeof(szRequestData));
    
    // fill request data;
    UpdateTagValues(&cSoapRequest);
    FillRequestData(&cSoapRequest);
    FillTokenData(&cSoapRequest);
    lRet = m_pcSoapKernel->HeartlandSOAP_Pack(&cSoapRequest, &nLength, szRequestData);
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
int TransReversal::DoUnpack(const CByteStream &cData)
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
* Function:
*      FillRequestData, fill the special data to reversal package
*
* @param
*      ST_HSOAP_REQUEST*： fill to this val
*
* @return
*      [none]

* Author: Tim.Ma
*/
void TransReversal::FillRequestData(ST_HSOAP_REQUEST* pcSoapRequest)
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
* Function:
*      FillTokenData, fill the token data to reversal package
*
* @param
*      ST_HSOAP_REQUEST*： fill to this val
*
* @return
*      [none]

* Author: Tim.Ma
*/
void TransReversal::FillTokenData(ST_HSOAP_REQUEST* pcSoapRequest)
{
    TraceMsg("FillTokenData entry");
    tagParam cParamVar = {0};
    HEARTLAND_AMOUNT_PARAM cAmtParam;
    int nRet, iOffset = 0;
    unsigned char tmpAmtSting[16] = { 0 };

    InitTagParam(cParamVar, kTxnAmountData, cAmtParam);
    nRet = m_pcParamPtr->GetParameter(&cParamVar);
    if (nRet)
    {
        TraceMsg("Error: Call FillRequestData get kHLTransParam failed.");
        return ;
    }

    //d_TAG_TRANS_TOKEN_VALUE
    if (strlen(cAmtParam.szTokenValueBuf) == 0)
    {
        return ;
    }

    TraceMsg("TokenData: %s", cAmtParam.szTokenValueBuf);
    m_cFillTxnData.SetTokenValue(pcSoapRequest, cAmtParam.szTokenValueBuf, d_CREDIT_CARD_TYPE);
    
    return ;
}


/**
* Function:
*      UpdateTagValues, this function used to update tag 95,9B for reversal, it's only for emv transaction
*
* @param
*      ST_HSOAP_REQUEST*： this val will be updated
*
* @return
*      [none]

* Author: Tim.Ma
*/
void TransReversal::UpdateTagValues(ST_HSOAP_REQUEST* pcSoapRequest)
{
    TraceMsg("UpdateTagValues entry");

    tagParam                cTagParam = { 0 };
    HEARTLAND_TRANS_PARAM   transParam = { 0 };
    InitTagParam(cTagParam, kHLTransParam, transParam);
    int nRet = m_pcParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter Failed:%d",  nRet);
        return ;
    }

    memset(pcSoapRequest->cTagData.szTagValues, 0, sizeof(pcSoapRequest->cTagData.szTagValues));
    m_cFillTxnData.SetTagData(pcSoapRequest, &transParam);
    TraceMsg("UpdateTagValues exit");
    return ;
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
WORD TransReversal::GetSoapData(ST_HSOAP_RESPONSE* pcHsoapResponse, HEARTLAND_TRANS_PARAM* pcTransParam)
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
        TraceMsg("CreditSale AuthAmt is %s", pcHsoapResponse->cTrans.szAuthAmt);
        
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_RESULT,                 &chTxnResult, 1);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_CODE,        pcHsoapResponse->cTrans.szRspCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_TXT,         pcHsoapResponse->cTrans.szRspText);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_APPROVED_CODE,              pcHsoapResponse->cTrans.szAuthCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_AMT_AUTH,               pcHsoapResponse->cTrans.szAuthAmt);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RETRIEVAL_REFERENCE_NUMBER, pcHsoapResponse->cTrans.szRefNbr);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_CARD_TYPE,              pcHsoapResponse->cTrans.szCardType);
    }while (0);
    
    TraceMsg("exit");
    return 0;
}