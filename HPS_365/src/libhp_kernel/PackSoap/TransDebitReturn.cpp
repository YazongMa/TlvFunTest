
/* 
 * File:   TransDebitReturn.cpp
 * Author: Tim.Ma
 * 
 * Created on 2018/8/2日, 14:05
 */

#include "TransDebitReturn.h"

#include "../TxnDef.h"
#include "../TlvFun.h"
#include "../StringFun.h"
#include "../FunBase.h"
#include "../Inc/ErrorCode.h"


/**
* Function:
*      TransDebitReturn's Construct Function:
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
TransDebitReturn::TransDebitReturn(IN IParameter* pcTransParam, IN HeartlandSoap* pcSoapKernel, IN CVoltageObject* pcVoltageObj)
:
TransDebitSale(pcTransParam, pcSoapKernel, pcVoltageObj)
{
    m_nSoapTxnType = tt_DEBIT_RETURN;
}


/**
* Function:
*      TransDebitReturn's Destruct Function:
*
* @param
*      none
*
* @return
*      none.
*
* Author: Tim.Ma
*/
TransDebitReturn::~TransDebitReturn()
{
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
WORD TransDebitReturn::GetSoapData(ST_HSOAP_RESPONSE* pcHsoapResponse, HEARTLAND_TRANS_PARAM* pcTransParam)
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
        else if (memcmp(pcHsoapResponse->cTrans.szRspCode, d_TXN_RSP_CODE_SUCCESS, 2) != 0)
            chTxnResult = d_TXN_DECLINED;

        TraceMsg("CreditSale GetwayTxnId is %s", pcHsoapResponse->cGateway.szGatewayTxnId);
        TraceMsg("CreditSale AuthAmt is %s", pcHsoapResponse->cTrans.szAuthAmt);
        
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_RESULT,                   &chTxnResult, 1);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_CODE,          pcHsoapResponse->cTrans.szRspCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_TXT,           pcHsoapResponse->cTrans.szRspText);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_APPROVED_CODE,                pcHsoapResponse->cTrans.szAuthCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_AMT_AUTH,                 pcHsoapResponse->cTrans.szAuthAmt);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RETRIEVAL_REFERENCE_NUMBER,   pcHsoapResponse->cTrans.szRefNbr);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_CARD_BRAND,                   pcHsoapResponse->cTrans.szCardType);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANSACTION_DESCRIPTOR,       pcHsoapResponse->cTrans.szTxnDescriptor);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_CARD_TYPE,                (char *)"DEBIT CARD");
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_POS_TXN_TYPE,                 (char *)"RETURN");
    }while (0);
    
    
    TraceMsg("exit");
    return 0;
}