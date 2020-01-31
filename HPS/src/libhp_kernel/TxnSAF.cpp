
/* 
 * File:   CTxnSAF.cpp
 * Author: Tim Ma
 *
 * Created on 2019年12月10日, 上午9:30
 */

#include "TxnSAF.h"
#include "dbghlp.h"
#include "TxnDef.h"
#include "Inc/TxnDataType.h"
#include "../libhp_api/Inc/HL_JsonKey.h"
#include "cJSON.h"

typedef struct
{
    char*   pszJsonKey;
    char*   pszJsonValue;
}Tlvtag2Json;


CTxnSAF::CTxnSAF(IN HeartlandSoap *pcSoapKernel,
            IN IBaseCurl *pcBaseCurlPtr)
:
m_pcSoapKernel(pcSoapKernel),
m_pcBaseCurlPtr(pcBaseCurlPtr),
m_nSoapTxnType(tt_CREDIT_SALE)
{
    memset(m_szTxnType, 0x00, sizeof(m_szTxnType));
    memset(m_szTxnAmount, 0x00, sizeof(m_szTxnAmount));
    memset(m_szTxnSafSequenceID, 0x00, sizeof(m_szTxnSafSequenceID));
}


CTxnSAF::~CTxnSAF()
{
}


int CTxnSAF::PerformTxn(void)
{
    TraceMsg("ERROR: Heartland Payment Kernel couldn't not come here");
    return 0;
}


int CTxnSAF::PerformTxn(IN char* pcJsonString,  OUT char* pszRspData, INOUT int* pnRspDataSize)
{
    TraceMsg("%s Entry", __FUNCTION__);

    int nRet;
    CByteStream cDataRequest;
    CByteStream cDataResponse;
    CByteStream cParsedResponse;
    const char* pszErrMsg;

    do
    {
        // 1. DoPack
        nRet = DoPack(pcJsonString, &cDataRequest);
        if (nRet != 0)
        {
            pszErrMsg = "FAILED TO PARSE REQUEST DATA";
            TraceMsg("ERROR: DoPack failed, nRet:%d, pcJsonString:\n%s\n", nRet, pcJsonString);
            break;
        }

        // 2. chat with host
        nRet = m_pcBaseCurlPtr->CurlProcess(&cDataRequest, cDataResponse);
        if (nRet != 0)
        {
            pszErrMsg = "FAILED TO CONNECT WITH HOST";
            TraceMsg("ERROR: failed to communicate with host");
            break;
        }

        // 3. unpack soap package
        nRet = DoUnpack((char *)cDataResponse.GetBuffer(), &cParsedResponse);
        if (nRet != 0)
        {
            pszErrMsg = "FAILED TO UPPACK RESPONSE OF HOST";
            TraceMsg("ERROR: failed to unpack the host's response");
            break;
        }

        if (*pnRspDataSize <= cParsedResponse.GetLength())
        {
            *pnRspDataSize = 0;
            pszErrMsg = "BUFFER TOO SMALL";
            TraceMsg("ERROR: buffer too small");
            break;
        }

        *pnRspDataSize = cParsedResponse.GetLength();
        memcpy(pszRspData, cParsedResponse.GetBuffer(), cParsedResponse.GetLength());
        pszRspData[*pnRspDataSize] = 0x00;

        TraceMsg("%s Exit", __FUNCTION__);
        return 0;
    }while(0);

    // error handle
    cJSON *pcJsonRoot = cJSON_CreateObject();
    if (pcJsonRoot == NULL)
    {
        TraceMsg("ERROR: json root tag created fail");
        return 1;
    }

    cJSON_AddStringToObject(pcJsonRoot, JK_Txn_Trans_RspInfo, pszErrMsg);
    cJSON_AddStringToObject(pcJsonRoot, JK_Txn_Trans_RspCode, "FF");

    char* pszRespJson = cJSON_Print(pcJsonRoot);
    if (pszRespJson != NULL)
    {
        if (*pnRspDataSize > strlen(pszRespJson))
        {
            *pnRspDataSize = strlen(pszRespJson);
            memcpy(pszRspData, pszRespJson, *pnRspDataSize);
            pszRspData[*pnRspDataSize] = 0x00;
        }

        cJSON_free(pszRespJson);
        pszRespJson = NULL;

    }

    cJSON_Delete(pcJsonRoot);
    pcJsonRoot = NULL;

    TraceMsg("%s Exit", __FUNCTION__);
    return 1;
}


int CTxnSAF::DoPack(IN char* pcJsonString, OUT CByteStream *pcPackData)
{
    TraceMsg("%s Entry", __FUNCTION__);

    if(NULL == pcJsonString || NULL == pcPackData)
    {
        TraceMsg("ERROR: param is NULL");
        return 1;
    }

    memset(m_szTxnType, 0x00, sizeof(m_szTxnType));
    memset(m_szTxnAmount, 0x00, sizeof(m_szTxnAmount));
    memset(m_szTxnSafSequenceID, 0x00, sizeof(m_szTxnSafSequenceID));

    cJSON *pcSubItem = NULL;
    cJSON *pcTxnType = NULL;
    cJSON *pcJsonRoot = NULL;
    int nTxnReqDataLength = 0;

    //txnSafSequenceID
    pcJsonRoot = cJSON_Parse(pcJsonString);
    if (pcJsonRoot == NULL)
    {
        TraceMsg("ERROR: failed to parse json request data");
        return 1;
    }

    pcSubItem = cJSON_GetObjectItem(pcJsonRoot, JK_Txn_SafSequenceID);
    if (NULL != pcSubItem && cJSON_String == pcSubItem->type)
    {
        if (strlen(pcSubItem->valuestring) != 0)
        {
            const int nMaxBufSize_ = sizeof(m_szTxnSafSequenceID) - 1;
            sprintf(m_szTxnSafSequenceID, "%.*s", nMaxBufSize_, pcSubItem->valuestring);
        }
        else
        {
            TraceMsg("WARNING: %s is empty", JK_Txn_SafSequenceID);
        }
    }

    //txnType
    pcTxnType = cJSON_GetObjectItem(pcJsonRoot, JK_Txn_Type);
    if (NULL != pcTxnType && cJSON_String == pcTxnType->type && 0 == strcmp(pcTxnType->valuestring, JV_AuthComplete))
    {
        ST_HSOAP_REQUEST cHsoapRequest = { 0 };
        cHsoapRequest.nTransactionType = tt_CREDIT_ADD_TO_BATCH;
        cHsoapRequest.nTransactionInterface = ti_CTLS;

        //txnGatewayTxnID
        cJSON* pcGatewayId = cJSON_GetObjectItem(pcJsonRoot, JK_Txn_Gateway_Id);
        if (NULL != pcGatewayId && cJSON_String == pcGatewayId->type && 0 != strlen(pcGatewayId->valuestring))
        {
            strcpy(cHsoapRequest.szGatewayTxnId, pcGatewayId->valuestring);
        }

        //txnAmt
        cJSON* pcAuthAmt = cJSON_GetObjectItem(pcJsonRoot, JK_Txn_Amount);
        if (NULL != pcAuthAmt && cJSON_String == pcAuthAmt->type && 0 != strlen(pcAuthAmt->valuestring))
        {
            strcpy(cHsoapRequest.szAmt, pcAuthAmt->valuestring);
        }

        int  nSoapLen = 2048;
        char szRequestSoap[2048] = { 0 };
        int nRet = m_pcSoapKernel->HeartlandSOAP_Pack(&cHsoapRequest, &nSoapLen, szRequestSoap);
        if (nRet)
        {
            TraceMsg("HeartlandSOAP_Pack Return: 0x%X", nRet);
            cJSON_Delete(pcJsonRoot);
            return nRet;
        }

        pcPackData->Write(szRequestSoap, nSoapLen);
        pcPackData->PushZero();

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
        TraceMsg("%s Exit", __FUNCTION__);
        return 0;
    }


    //txnRequestData
    pcSubItem = cJSON_GetObjectItem(pcJsonRoot, JK_Txn_ReqData);
    if (NULL != pcSubItem && cJSON_String == pcSubItem->type && 0 != strlen(pcSubItem->valuestring))
    {
        nTxnReqDataLength = strlen(pcSubItem->valuestring);
    }
    else
    {
        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
        TraceMsg("Info: TxnRequestData is invalid.");
        return 1;
    }

    if (nTxnReqDataLength <= 22)
    {
        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
        TraceMsg("Info: TxnRequestData is invalid.");
        return 1;
    }

    memcpy(m_szTxnType, pcSubItem->valuestring, 2);
    memcpy(m_szTxnAmount, pcSubItem->valuestring + 2, 20);
    sprintf(m_szTxnAmount,"$%lld.%02lld", atoll(m_szTxnAmount)/100, atoll(m_szTxnAmount)%100);

    pcPackData->Write(pcSubItem->valuestring + 22, nTxnReqDataLength - 22);
    pcPackData->PushZero();

    cJSON_Delete(pcJsonRoot);
    pcJsonRoot = NULL;
    TraceMsg("%s Exit", __FUNCTION__);
    return 0;
}

int CTxnSAF::DoUnpack(IN char* pszHostString, OUT CByteStream *pcRespData)
{
    TraceMsg("Info: %s Entry", __FUNCTION__);

    if (pszHostString == NULL || pcRespData == NULL)
    {
        TraceMsg("Error: %s cData invalid", __FUNCTION__);
        return 1;
    }
    
    ST_HSOAP_RESPONSE cHostResponse = { 0 };
    int nRet = m_pcSoapKernel->HeartlandSOAP_Unpack(&cHostResponse, pszHostString);
    if (nRet)
    {
        TraceMsg("Error: HeartlandSOAP_Pack Return: 0x%X", nRet);
        return 1;
    }

    // if approved, update the SAFTotalAmount
    if (memcmp(cHostResponse.cTrans.szRspCode, d_TXN_RSP_CODE_SUCCESS, 2) == 0)
    {
        UpdateKMSUserData();
    }

    Tlvtag2Json cRespJson[] =
    {
        {(char*)JK_Txn_Gateway_RspCode,    cHostResponse.cGateway.szGatewayRspCode},
        {(char*)JK_Txn_Gateway_RspInfo,    cHostResponse.cGateway.szGatewayRspMsg},
        {(char*)JK_Txn_DateTime,           cHostResponse.cGateway.szRspDT},
        {(char*)JK_Txn_Gateway_Id,         cHostResponse.cGateway.szGatewayTxnId},
        {(char*)JK_Txn_Trans_RspCode,      cHostResponse.cTrans.szRspCode},
        {(char*)JK_Txn_Trans_RspInfo,      cHostResponse.cTrans.szRspText},
        {(char*)JK_Txn_AuthCode,           cHostResponse.cTrans.szAuthCode},
        {(char*)JK_Txn_RefNumber,          cHostResponse.cTrans.szRefNbr},
        {(char*)JK_Txn_CardBrand,          cHostResponse.cTrans.szCardType},
        {(char*)JK_Txn_SafSequenceID,      m_szTxnSafSequenceID},
        {NULL,                      NULL},
    };


    // fill json data from the structure: cHostResponse
    cJSON *pcJsonRoot = cJSON_CreateObject();
    if (NULL == pcJsonRoot)
    {
        if (memcmp(cHostResponse.cTrans.szRspCode, d_TXN_RSP_CODE_SUCCESS, 2) == 0)
        {
            TraceMsg("ERROR: json root tag created fail, but transaction is approved");
            return 0;
        }

        TraceMsg("ERROR: json root tag created fail, transaction is declined either");
        return 1;
    }

    for(int i=0; cRespJson[i].pszJsonKey!=NULL && cRespJson[i].pszJsonValue!=NULL; ++i)
    {
        if (strlen(cRespJson[i].pszJsonValue))
        {
            cJSON_AddStringToObject(pcJsonRoot, cRespJson[i].pszJsonKey, cRespJson[i].pszJsonValue);
            TraceMsg("INFO: JsonKey:%s, JsonValue:%s", cRespJson[i].pszJsonKey, cRespJson[i].pszJsonValue);
        }
    }

    if (strlen(cHostResponse.cTrans.szAuthAmt))
    {
        TraceMsg("INFO: From the HOST");
        cJSON_AddStringToObject(pcJsonRoot, JK_Txn_Amount, cHostResponse.cTrans.szAuthAmt);
    }
    else if(strlen(m_szTxnAmount))
    {
        TraceMsg("INFO: From the request JSON package");
        cJSON_AddStringToObject(pcJsonRoot, JK_Txn_Amount, &m_szTxnAmount[1]);
    }


    char* pszRespJson = cJSON_Print(pcJsonRoot);
    pcRespData->Write(pszRespJson, strlen(pszRespJson));
    pcRespData->PushZero();

    cJSON_Delete(pcJsonRoot);
    pcJsonRoot = NULL;

    cJSON_free(pszRespJson);
    pszRespJson = NULL;
    TraceMsg("%s Exit", __FUNCTION__);
    return 0;
}


void CTxnSAF::UpdateKMSUserData()
{
    TraceMsg("Info: %s Entry", __FUNCTION__);

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

        return ;
    }

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

    TraceMsg("%s Exit", __FUNCTION__);
}

#if 0
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
WORD CTxnSAF::GetSoapData(ST_HSOAP_RESPONSE* pcHsoapResponse, OUT CByteStream *pcRespData)
{
    TraceMsg("entry");
    
    do
    {
        if (NULL == pcHsoapResponse)
        {
            TraceMsg("ERROR: pcHsoapResponse is NULL");
            return ERROR_INVALID_PARAM;
        }
/*
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RESPONSE_CODE,                pcHsoapResponse->cGateway.szGatewayRspCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RESPONSE_TXT,                 pcHsoapResponse->cGateway.szGatewayRspMsg);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESP_DT,                pcHsoapResponse->cGateway.szRspDT);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_LICENSE_IDENTITY,             pcHsoapResponse->cGateway.szLicenseId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_SITE_IDENTITY,                pcHsoapResponse->cGateway.szSiteId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_DEVICE_IDENTITY,              pcHsoapResponse->cGateway.szDeviceId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANSACTION_IDENTITY,         pcHsoapResponse->cGateway.szGatewayTxnId);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_UNIQUE_DEVICE_ID,             pcHsoapResponse->cGateway.szUniqueDeviceId);
*/
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
                sprintf((char*)byData, "%.02f", dSAFTotalAmount);

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
//            m_cFillTxnData.SetTxnResult(pcTransParam, TAG_AMT_AUTH,                   pcHsoapResponse->cTrans.szAuthAmt);
        }
        else
        {
//            m_cFillTxnData.SetTxnResult(pcTransParam, TAG_AMT_AUTH,                   m_szTxnAmount);
        }

/*
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_RESULT,                   &chTxnResult, 1);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_CODE,          pcHsoapResponse->cTrans.szRspCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_RESPONSE_TXT,           pcHsoapResponse->cTrans.szRspText);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_APPROVED_CODE,                pcHsoapResponse->cTrans.szAuthCode);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_RETRIEVAL_REFERENCE_NUMBER,   pcHsoapResponse->cTrans.szRefNbr);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_CARD_BRAND,                   pcHsoapResponse->cTrans.szCardType);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANSACTION_DESCRIPTOR,       pcHsoapResponse->cTrans.szTxnDescriptor);
        m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_EMV_ISSUER_RESP,        pcHsoapResponse->cTrans.szEMVIssuerResp);
*/
        int nTxnSoapType = atoi(m_szTxnType);
        if (nTxnSoapType == kCreditSale_SoapType)
        {
//            m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_CARD_TYPE,                (char*)"CREDIT CARD");
//            m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TRANS_TOKEN_VALUE,            pcHsoapResponse->cGateway.szTokenValue);
        }
        else
        {
//            m_cFillTxnData.SetTxnResult(pcTransParam, d_TAG_TXN_CARD_TYPE,                (char*)"DEBIT CARD");
        }
    } while (0);

    TraceMsg("exit");
    return 0;
}
#endif
