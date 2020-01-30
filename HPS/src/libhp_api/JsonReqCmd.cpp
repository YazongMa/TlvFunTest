
/* 
 * File Name: JsonReqCmd.cpp
 * Author: Alan.Ren
 * 
 * Create Date: 2019.03.15
 */

#include "JsonReqCmd.h"

#include "../libhp_kernel/dbghlp.h"
#include "../libhp_kernel/TxnJsonKey.h"
#include "../libhp_kernel/Inc/TxnState.h"

#define FreeJsonObjPtr(jsonPtr) { \
    if (NULL != (jsonPtr)) { \
        cJSON_Delete((jsonPtr)); \
        (jsonPtr) = NULL; \
    } \
}

#if 0
#define SaleCmd_Sup_Keys             "txnCardType\0txnAmt\0txnSequenceID\0\0" 
#define VoidCmd_Sup_Keys             "txnGatewayTxnID\0\0"
#define PreAuthCmd_Sup_Keys          "txnAmt\0\0"
#define ReturnCmd_Sup_Keys           "txnAmt\0txnCardType\0\0"
#define AuthCompleteCmd_Sup_Keys     "txnAmt\0txnGatewayTxnID\0\0"
#define SettlementCmd_Sup_Keys       "txnAmt\0\0"

static const struct
{
    int nTxnType;
    const char *pszCmdSupKeys;
} cmdOptsBuf_[] = {
    {kSale, SaleCmd_Sup_Keys},
    {kVoid, VoidCmd_Sup_Keys},
    {kPreAuth, PreAuthCmd_Sup_Keys},
    {kAuthComplete, AuthCompleteCmd_Sup_Keys},
    {kReturn, ReturnCmd_Sup_Keys},
    {kSettlement, SettlementCmd_Sup_Keys}
};
#endif 

static const struct
{
    const char* pszActTypeName;
    const int nActType;
} cActMap_[] = {
    { JV_Act_AidList, TXN_STATE_AID_SELECT }, 
    { JV_Act_AidConfim, TXN_STATE_AID_CONFIRM },
    { JV_Act_TotalAmt, TXN_STATE_TOTAL_AMOUNT_GET },
    { JV_Act_ExceptionCheck, TXN_STATE_EXCEPTION_FILE_CHECK }, 
    { JV_Act_CallUBank, TXN_STATE_CALL_YOUR_BANK },
    { JV_Confirm_Comm_With_Host, TXN_STATE_CONFIRM_COMM_WITH_HOST },
    { JV_Confirm_Prompt_Last_PIN_Input, TXN_STATE_LAST_PIN_INPUT },
    { JV_Act_Print_Signature, TXN_STATE_PRINT_SIGNATURE },
    { JV_Act_InteracPosSeqNbr, TXN_STATE_INTERAC_POS_SEQNBR },
    { JV_Act_InteracAccountType, TXN_STATE_INTERAC_ACCOUNT_TYPE },
    { JV_Act_Select_Language, TXN_STATE_SELECT_LANGUAGE },
    { JV_Act_Switch_Language, TXN_STATE_SWITCH_LANGUAGE },
    { JV_Act_Interac_Receipt_Warning, TXN_STATE_INTERAC_RECEIPT_WARNINIG },
};

//
// Private.
//
static const struct
{
    const int nTxnType;
    const char *pszString;
} cStringTypeMap_[] = {
    {kCmdUnsetTxnType, JV_Invalid_Value},
    {kWriteData, JV_WriteData},
    {kSale, JV_Sale},
    {kVoid, JV_Void},
    {kPreAuth, JV_PreAuth},
    {kReturn, JV_Return},
    {kAuthComplete, JV_AuthComplete},
    {kCancel, JV_Cancel},
    {kSettlement, JV_Settlement},
    {kGetAppVer, JV_GetAppVer},
    {kGetKernelVer, JV_GetKernelVer},
    {kReversal, JV_Reversal},
    {kUpdateData, JV_UpdateData},
    {kPollingCard, JV_PollingCard},
    {kSAF, JV_SAF},
    {kCheckConnectivity, JV_CheckConnectivity}
};
static const char *NullString_ = "";
static const size_t nMaxCount_ =
    sizeof(cStringTypeMap_) / sizeof(cStringTypeMap_[0]);

int CJsonReqCmd::StringToActType(IN const char *pszStr)
{
    int nActCode = -1;
    const size_t nActCount = sizeof(cActMap_)/sizeof(cActMap_[0]);

    for (size_t nIndex = 0; nIndex < nActCount; ++nIndex)
    {
        if (0 == strcmp(pszStr, cActMap_[nIndex].pszActTypeName))
        {
            nActCode = cActMap_[nIndex].nActType;
            break;
        }
    }
    return nActCode;
}

/*
 * Public.
 */
CJsonReqCmd::CJsonReqCmd(): m_pReqJsonCmd(NULL),
m_bIsRequestCmdValid(FALSE)
{
    ResetCmdProp();
}

/*
 * Public.
 */
CJsonReqCmd::~CJsonReqCmd()
{
    ResetCmdProp();
}

/*
 * Public.
 */
void CJsonReqCmd::ClearReqCmd(void)
{
    ResetCmdProp();
}

/*
 * Public.
 */
BOOL CJsonReqCmd::ParseCmd(
    IN const char *pszJsonCmd)
{
    BOOL bResult = FALSE;
    cJSON *pcJsonRoot = NULL;

    do
    {
        // Reset parameters
        ResetCmdProp();
        
        //Parse new Json command
        pcJsonRoot = cJSON_Parse(pszJsonCmd);
        if (NULL == pcJsonRoot)
        {
            TraceMsg("Error: Invalid json data");
            break;
        }

        // Check json commad is valid or not.
        // Return the result of check.
        if (CheckRequestCmd(pcJsonRoot))
        {
            TraceMsg("Error: Invalid json cmd");
            m_bIsRequestCmdValid = TRUE;
            m_pReqJsonCmd = pcJsonRoot;
            bResult = TRUE;
        }
        else
        {
            FreeJsonObjPtr(pcJsonRoot);
        }
    } while (0);
    return bResult;
}

/*
 * Private.
 */
void CJsonReqCmd::FreeResource(void)
{
    FreeJsonObjPtr(m_pReqJsonCmd);
}

/*
 * Private
 */
void CJsonReqCmd::ResetCmdProp(void)
{
    FreeJsonObjPtr(m_pReqJsonCmd);
    m_nAmt = 0ll;
    m_nOtherAmt = 0ll;
    m_nCardType = kCmdUnsetCardType;
    m_nTxnType = kCmdUnsetTxnType;
    m_bIsRequestCmdValid = FALSE;
    m_nActType = -1;
    memset(m_szTxnGatewayID, 0, sizeof(m_szTxnGatewayID));
    memset(m_szTxnTokenValue, 0, sizeof(m_szTxnTokenValue));
}

/*
 * Public.
 */
BOOL CJsonReqCmd::CheckRequestCmd(
    IN const cJSON *pcReqJsonCmd) 
{ 
    BOOL bChkRet = FALSE;
    cJSON *pcSubItem = NULL;
    const char *pszSupKeys = NULL;
    const char *pszSubPtr = NULL;
    // const size_t nCmdOptCount = sizeof(cmdOptsBuf_)/sizeof(cmdOptsBuf_[0]);

    do
    {
        if (NULL == pcReqJsonCmd)
            break;

        // TxnType
        pcSubItem = cJSON_GetObjectItem(pcReqJsonCmd, JK_Txn_Type);
        if (NULL != pcSubItem)
        {
            m_nTxnType = StringToTxnType(pcSubItem->valuestring);
        }

        // Card type
        pcSubItem = cJSON_GetObjectItem(pcReqJsonCmd, JK_Txn_Card_Type);
        if (NULL != pcSubItem)
        {
            m_nCardType = StringToCardType(pcSubItem->valuestring);
        }
        if (kCmdUnsetCardType == m_nCardType)
        {
            TraceMsg("Info: Unset card type.");
        }
        
        // Amount
        pcSubItem = cJSON_GetObjectItem(pcReqJsonCmd, JK_Txn_Amount);
        if (NULL != pcSubItem)
        {
            m_nAmt = strtoull(pcSubItem->valuestring, NULL, 10);
        }
        else
        {
            TraceMsg("Info: Invalid amount.");
        }

        // Other Amount
        pcSubItem = cJSON_GetObjectItem(pcReqJsonCmd, JK_Txn_OtherAmount);
        if (NULL != pcSubItem)
        {
            m_nOtherAmt = strtoull(pcSubItem->valuestring, NULL, 10);
        }
        else if(pcSubItem = cJSON_GetObjectItem(pcReqJsonCmd, JK_Txn_CashbackAmount))
        {
            m_nOtherAmt = strtoull(pcSubItem->valuestring, NULL, 10);
        }
        else
        {
            TraceMsg("Info: Invalid other amount.");
        }

        // GatewayID
        pcSubItem = cJSON_GetObjectItem(pcReqJsonCmd, JK_Txn_Gateway_Id);
        if (NULL != pcSubItem)
        {
            strcpy(m_szTxnGatewayID, pcSubItem->valuestring);
        }
        else
        {
            TraceMsg("Info: Invalid TxnGatewayID.");
        }

        // TokenValue
        pcSubItem = cJSON_GetObjectItem(pcReqJsonCmd, JK_Txn_Tokenization_Value);
        if (NULL != pcSubItem)
        {
            strcpy(m_szTxnTokenValue, pcSubItem->valuestring);
            TraceMsg("Info: TxnTokenValue:%s", m_szTxnTokenValue);
        }

        // ActType
        pcSubItem = cJSON_GetObjectItem(pcReqJsonCmd, JK_Act_Type);
        if (NULL != pcSubItem)
        {
            m_nActType = StringToActType(pcSubItem->valuestring);
        }
        else
        {
            TraceMsg("Info: Invalid act type.");
        }        
        if (kCmdUnsetCardType == m_nTxnType && 
            -1 == m_nActType)
        {
            TraceMsg("Info: Invalid txn type.");
            break;
        }
        
        bChkRet = TRUE;
    } while (0);
    return bChkRet;
}

/*
 * Public.
 */
BOOL CJsonReqCmd::IsRequestCmdValid(void) const
{
    return m_bIsRequestCmdValid;
}

/*
 * Public.
 */
int CJsonReqCmd::StringToTxnType(
    IN const char *pszTxnTypeStr)
{
    int nTxnType = kCmdUnsetCardType;
    for (size_t nIndex = 0; nIndex < nMaxCount_; ++nIndex)
    {
        if (0 == strcmp(pszTxnTypeStr, cStringTypeMap_[nIndex].pszString))
        {
            nTxnType = cStringTypeMap_[nIndex].nTxnType;
            break;
        }
    }
    return nTxnType;
}

/*
 * Public.
 */
const char *CJsonReqCmd::TxnTypeToString(
    IN const int &nType)
{
    const char *pszStr = NullString_;
    for (size_t nIndex=0; nIndex < nMaxCount_; ++nIndex)
    {
        if (nType == cStringTypeMap_[nIndex].nTxnType)
        {
            pszStr = cStringTypeMap_[nIndex].pszString;
            break;
        }
    }
    return pszStr;
}

/*
 * Public.
 */
static const struct
{
    int nCardType;
    const char *pszString;
} cardTypeMap_[] = {
    {kCmdSetCredit, JV_Credit},
    {kCmdSetDebit, JV_Debit}
};
static const size_t cardTypeMapSize_ =
    sizeof(cardTypeMap_) / sizeof(cardTypeMap_[0]);
int CJsonReqCmd::StringToCardType(IN const char *pszStr)
{
    int nCardType = kCmdUnsetCardType;

    for (size_t nIndex=0; nIndex < cardTypeMapSize_; ++nIndex)
    {
        if (0 == strcmp(pszStr, cardTypeMap_[nIndex].pszString))
        {
            nCardType = cardTypeMap_[nIndex].nCardType;
            break;
        }
    }
    return nCardType;
}

/*
 * Public.
 */
const char *CJsonReqCmd::CardTypeToString(
    IN const int &nCardType)
{
    const char *pszString = NullString_;

    for (size_t nIndex=0; nIndex < cardTypeMapSize_; ++nIndex)
    {
        if (nCardType == cardTypeMap_[nIndex].nCardType)
        {
            pszString = cardTypeMap_[nIndex].pszString;
            break;
        }
    }
    return pszString;
}

/*
 * Public.
 */
const cJSON* CJsonReqCmd::GetJsonObjPtr(void) const
{
    return m_pReqJsonCmd;
}
