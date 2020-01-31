
/*
 * File Name: JsonRspCmd.cpp
 * Author: Alan.Ren
 * 
 * Create Date: 2019.03.15
 */

#include "JsonRspCmd.h"

#include "JsonReqCmd.h"
#include "Inc\DefUiIndex.h"
#include "Inc\HL_JsonKey.h"
#include "HPApiKernel.h"
#include "TlvFun.h"
#include "..\libhp_kernel\dbghlp.h"
#include "..\libhp_kernel\TxnTag.h"
#include "..\libhp_kernel\TxnDef.h"
#include "..\libhp_kernel\Inc\DefParamTypeId.h"
#include "HPApiContext.h"


/*
 * Public.
 */
static const struct
{
    const char *pszTlvTag;
    const char *pszJsonTag;
} tlvTag2JsonKey_[] = 
{
    {d_TAG_BATCH_IDENTITY, JK_Txn_BatchId},
    {d_TAG_TOTAL_SALE_COUNT, JK_Txn_TotalSaleCnt},
    {d_TAG_TOTAL_SALE_AMOUNT, JK_Txn_TotalSaleAmt},
    {d_TAG_TRANSACTION_IDENTITY, JK_Txn_Gateway_Id},
    {d_TAG_RETRIEVAL_REFERENCE_NUMBER, JK_Txn_RefNumber},
    {d_TAG_APPROVED_CODE, JK_Txn_AuthCode},
    {d_TAG_TRANS_RESPONSE_CODE, JK_Txn_Trans_RspCode},
    {d_TAG_TRANS_RESPONSE_TXT, JK_Txn_Trans_RspInfo},
    {d_TAG_RESPONSE_CODE, JK_Txn_Gateway_RspCode},
    {d_TAG_RESPONSE_TXT, JK_Txn_Gateway_RspInfo},
    {d_TAG_TRANS_AMOUNT, JK_Txn_Amount},
    {d_TAG_TXN_AMT_AUTH, JK_Txn_OtherAmount},
    {d_TAG_POS_TXN_TYPE, JK_Txn_Type_Receipt},
    {d_TAG_TRANS_CARD_ENTRY_MODE, JK_Txn_Card_EntryMode},
    {d_TAG_TRANS_POLLCARD_SEQUENCE_NBR, JK_Txn_SequenceId},
    {d_TAG_TRANS_RESP_DT, JK_Txn_DateTime},
    {TAG_FD_NAME, JK_Txn_EMV_AID},
    {d_TAG_TRANS_EMV_TVR, JK_Txn_EMV_TVR},
    {d_TAG_TRANS_EMV_TSI, JK_Txn_EMV_TSI},
    {TAG_ARC, JK_Txn_EMV_ARC},
    {d_TAG_CARD_BRAND, JK_Txn_CardBrand},
    {d_TAG_TXN_CARD_TYPE, JK_Txn_Card_Type},
    {d_TAG_TRANS_CARD_ENTRY_LEGEND, JK_Txn_EntryLegend},
    {TAG_FD_NAME, JK_Txn_EMV_AID},
    {TAG_APP_PREFER_NAME, JK_Txn_EMV_AppLabel},
    {d_TAG_TRANS_CARD_EXPIRE_DATE, JK_Txn_CardExpire_Date},
    {d_TAG_TRANS_CARD_HOLDER_NAME, JK_Txn_CardHolderName},
    {d_TAG_TRANS_BANK_RESPONSE_CODE, JK_Txn_BankRespCode},
    {TAG_AMT_AUTH, JK_Txn_Amount},
    {d_TAG_HEARTLAND_ACCOUNT_TYPE, JK_Txn_AccountType},
    {d_TAG_HEARTLAND_POS_SEQUENCE_NBR, JK_Txn_SequenceNbr},
    {d_TAG_TRANS_TRACE_NUMBER, JK_Txn_TraceNumber},
    {d_TAG_TRANS_RSP_DATA, JK_Txn_RspData},
    {d_TAG_TRANS_SAF_SEQUENCEID, JK_Txn_SafSequenceID},
#ifdef PLATFORM_ANDROID
    {d_TAG_TRANS_CARD_MASKED_PAN, JK_Txn_MaskedAcct},
#else
    {d_TAG_TRANS_CARD_MASKED_PAN, JK_Txn_MaskedCardNum},
#endif
};

static const struct
{
    const int nTxnEntryModeVal;
    const char *pszTxnEntryModeStr;
} txnEntryModeMap_[] = {
    {kUnknown, "Unknown"},
    {kSwipe, "SWIPE"},
#ifdef PLATFORM_ANDROID
    {kTap, "TAP"},
    {kInsert, "INSERT"},
#else
    {kTap, "CONTACTLESS"},
    {kInsert, "CONTACT"},
#endif
    {kFallback, "FALLBACK"},
    {kNone, "NONE"}
};


#define FreeJsonPrintStr(strPtr) {\
    if (NULL != (strPtr)) { \
        cJSON_free((void *)(strPtr)); \
        (strPtr) = NULL; \
    } \
}

/*
 * Private.
 */
unsigned short CJsonRspCmd::_PackTransResult(cJSON *jsonRoot, char *tag, BYTE *tlvBuff, unsigned short tlvBuffSize)      
{                                                                           
    BYTE*   pbyTlvValue = NULL;                                             
    unsigned short    wTlvValueLen = 0;                                               
    unsigned short    wError = CTlvFun::TLVListReadTagValue(tag, tlvBuff,             
                        tlvBuffSize, &pbyTlvValue, &wTlvValueLen);          
    if (wError == 0)                                                        
    {                                                                       
        static char szTmpBuff[2048] = { 0 };                                 
        memset(szTmpBuff, 0, sizeof(szTmpBuff));                            
        memcpy(szTmpBuff, pbyTlvValue, wTlvValueLen);                       
        AddItemToObject(jsonRoot, tag, szTmpBuff);                          
    }
    return wError;
}

/*
 * Public.
 */
CJsonRspCmd::CJsonRspCmd(IN CJsonReqCmd *pcReqCmdObj):
m_pcReqCmdObj(pcReqCmdObj),
m_pszRspJsonStr(NULL),
m_nUiIndex(-1),
m_pcParamPtr(NULL),
m_pszUserJsonData(NULL),
m_pcApiContext(NULL)
{ 
}

/*
 * Public.
 */
CJsonRspCmd::CJsonRspCmd(IN CJsonReqCmd *pcReqCmdObj,
                         IN CHPApiContext *pcApiContext):
                         m_pcReqCmdObj(pcReqCmdObj),
                         m_pcApiContext(pcApiContext),
                         m_pszRspJsonStr(NULL),
                         m_nUiIndex(-1),
                         m_pcParamPtr(NULL),
                         m_pszUserJsonData(NULL)
{
}

/*
 * Public.
 */
CJsonRspCmd::CJsonRspCmd(IN CJsonReqCmd *pcReqCmdObj, IN unsigned long dwStatus):
m_pcReqCmdObj(pcReqCmdObj),
m_pszRspJsonStr(NULL),
m_nUiIndex(-1),
m_pcParamPtr(NULL),
m_pszUserJsonData(NULL),
m_dwStatus(dwStatus),
m_pcApiContext(NULL)
{

}

/*
 * Public.
 */
CJsonRspCmd::CJsonRspCmd(IN const char *pszValue, IN unsigned long dwStatus):
m_pcReqCmdObj(NULL),
m_pszRspJsonStr(NULL),
m_nUiIndex(-1),
m_pcParamPtr(NULL),
m_pszUserJsonData(pszValue),
m_dwStatus(dwStatus)
{ 
}

/* 
 * Public.
 */
CJsonRspCmd::~CJsonRspCmd()
{
    m_pcReqCmdObj = NULL;
    FreeJsonPrintStr(m_pszRspJsonStr);
}

/*
 * Public.
 */
const char *CJsonRspCmd::PackResponse(void)
{
    // Check the rquest command is valid or not.
    if(m_pszUserJsonData != NULL)
    {
        DoPackUserInputResponse();
    }
    else if (m_pcReqCmdObj && m_pcReqCmdObj->IsRequestCmdValid())
    {
        DoPackResponse();
    }
    else
    {
        TraceMsg("Error: Request JSON command is invalid.");
    }
    return m_pszRspJsonStr;
}


/*
 * Protected.
 */
void CJsonRspCmd::DoExtensionPack(IN cJSON *pcRoot)
{
    tagParam cTagParam = { 0 };
    HEARTLAND_TRANS_PARAM cTxnParam = { 0 };
    
    InitTagParam(cTagParam, kHLTransParam, cTxnParam);
    m_pcParamPtr->GetParameter(&cTagParam);

    const int nMax = sizeof(tlvTag2JsonKey_) / sizeof(tlvTag2JsonKey_[0]);
    unsigned short wError = 0;
    for(int i=0; i < nMax; ++i)
    {
        wError = _PackTransResult(pcRoot, (char *)tlvTag2JsonKey_[i].pszTlvTag, cTxnParam.bResult, cTxnParam.bResultLen);
        if(!memcmp(tlvTag2JsonKey_[i].pszTlvTag, TAG_APP_PREFER_NAME, sizeof(TAG_APP_PREFER_NAME))) // 9F12
        {
            if(wError == 0)// get 9F12 success
            {
                ++i; //skip TAG_APP_LABEL
            }
        }
    }
}

/*
 * Public.
 */
void CJsonRspCmd::DoPackResponse(void)
{
    cJSON *pcJsonRoot = NULL;
    char szVal[64] = {0};

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;

        // Start packing response.
        cJSON_AddStringToObject(pcJsonRoot, JK_Txn_Type,
                                CJsonReqCmd::TxnTypeToString(m_pcReqCmdObj->GetTxnType()));

        // Ui Index.
        cJSON_AddNumberToObject(pcJsonRoot, JK_Txn_Ui_Index, m_nUiIndex);

        // txnStatus
        if (NULL == m_pcApiContext)
        {
            sprintf(szVal, "%08X", m_dwStatus);
            cJSON_AddStringToObject(pcJsonRoot, JK_Txn_Processing_Status, szVal);
        }
        else
        {
            sprintf(szVal, "%08X", m_pcApiContext->GetProcessingStatus());
            cJSON_AddStringToObject(pcJsonRoot, JK_Txn_Processing_Status, szVal);

            const int nTxnEntryMode = m_pcApiContext->GetTxnEntryMode(); 
            if (kUnknown <  nTxnEntryMode &&
                nTxnEntryMode <= kNone )
            {
                const int nSize = sizeof(txnEntryModeMap_)/sizeof(txnEntryModeMap_[0]);

                for (int nIndex = 0; nIndex < nSize; ++nIndex)
                {
                    if (nTxnEntryMode == txnEntryModeMap_[nIndex].nTxnEntryModeVal)
                    {
                        cJSON_AddStringToObject(pcJsonRoot, JK_Txn_Card_EntryMode,
                                                txnEntryModeMap_[nIndex].pszTxnEntryModeStr);
                        break;
                    }
                }
            }
        }

        // Pack extension data.
        if (NULL != m_pcParamPtr)
        {
            DoExtensionPack(pcJsonRoot);
        }

        // Generate print str.
        FreeJsonPrintStr(m_pszRspJsonStr);
        m_pszRspJsonStr = cJSON_Print(pcJsonRoot);

        // Free Json Data.
        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);
}


/*
 * Public.
 */
void CJsonRspCmd::DoPackUserInputResponse(void)
{
    cJSON *pcJsonRoot = NULL;

    do
    {
        // Parse User Action Data.
        pcJsonRoot = cJSON_Parse(m_pszUserJsonData);
        if (NULL == pcJsonRoot)
        {
            TraceMsg("Null object. m_pszUserJsonData:%s", m_pszUserJsonData);
            break;
        } 

        // Ui Index.
        cJSON_AddNumberToObject(pcJsonRoot, JK_Txn_Ui_Index, m_nUiIndex);

        // txnStatus
        char szVal[64] = {0};
        sprintf(szVal, "%08X", m_dwStatus);
        cJSON_AddStringToObject(pcJsonRoot, JK_Txn_Processing_Status, szVal);

        // Generate print str.
        FreeJsonPrintStr(m_pszRspJsonStr);
        m_pszRspJsonStr = cJSON_Print(pcJsonRoot);

        // Free Json Data.
        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);
}
/*
 * Public.
 */
void CJsonRspCmd::SetUiIndex(IN const int &nIndex)
{
    m_nUiIndex = nIndex;
}

/*
 * Public.
 */
void CJsonRspCmd::SetTxnField(IN IParameter* pcParamObj)
{
    m_pcParamPtr = pcParamObj;
}

/*
 * Add sub item.
 */
void CJsonRspCmd::AddItemToObject(IN cJSON *pcJsonRoot,
                                  IN const char *pszTag,
                                  IN const char *pszValue)
{
    if (NULL == pszValue || 0 == strlen(pszValue))
    {
        return;
    }

    const char* pszJsonKey = NULL;
    const int nMax = sizeof(tlvTag2JsonKey_) / sizeof(tlvTag2JsonKey_[0]);
    for(int i=0; i < nMax; ++i)
    {
        if (strcmp(pszTag, tlvTag2JsonKey_[i].pszTlvTag) == 0)
        {
            pszJsonKey = tlvTag2JsonKey_[i].pszJsonTag;
            if (strcmp(d_TAG_TRANS_POLLCARD_SEQUENCE_NBR, pszTag) == 0)
            {
                cJSON_AddNumberToObject(pcJsonRoot, pszJsonKey, atoll(pszValue));
            }
            else
            {
                cJSON_AddStringToObject(pcJsonRoot, pszJsonKey, pszValue);
            }
            break;
        }
    }
}
