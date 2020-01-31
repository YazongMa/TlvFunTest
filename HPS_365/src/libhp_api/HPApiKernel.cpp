
/*
 * File Name: HPApiKernel.cpp
 * Author: Alan.Ren
 * 
 * Create Date: 2019.03.14
 */

#include "KernelLayerObjs.h"
#include "HPApiContext.h"
#include "HPApiKernel.h"
#include "TlvFun.h"

#include "../libhp_kernel/dbghlp.h"
#include "../libhp_kernel/TxnJsonKey.h"
#include "Inc/DefUiIndex.h"
#include "Inc/HP_ProcStatus.h"

#define INVALID_THREAD_ID                     (-1)
#define Nop_ApiTrd_Idle_                      (10 * 1000)  // 10 milliseconds.
#define Nop_Detect_Card_                      (50 * 1000)  // 50 milliseconds.
#define Nop_WaitUserRspArrive_                (100 * 1000) // 100 milliseconds.
#define EmptyJsonRsponse_                     ("{ }")


static const size_t nMinJsonRspLen_ = 32;

static const struct 
{
    const int nTxnState;
    const int nUiIndex;
} s_cTxnStateToUiIndex_[] = 
{
    { TXN_STATE_CONNECTING, kConnectingHostUi },
    { TXN_STATE_APPROVAL, kTxnApproveUi },
    { TXN_STATE_DECLINED, kTxnDeclineUi },
    { TXN_STATE_OFFLINE_APPROVED, kTxnOfflineApproveUi },
    { TXN_STATE_OFFLINE_DECLINED, kTxnOfflineDeclineUi },
    { TXN_STATE_PREPARING_TXNDATA, kPackingSoapDataUi },
    { TXN_STATE_PACKING_SOAPDATA, kPackingSoapDataUi },
    { TXN_STATE_PLEASE_INSERT_CARD, kPlzInsertCard },
    { TXN_STATE_PLEASE_SWIPE_CARD, kPlzSwipeCard },
    { TXN_STATE_PLEASE_TAP_CARD, kPlzTapCard },
    { TXN_STATE_PIN_ENTER_START, kStartGetPinUi },
    { TXN_STATE_PIN_ENTER_FINISH, kFinishGetPinUi },
    { TXN_STATE_READING_CARD_FAIL, kReadCardFailUi },
    { TXN_STATE_MORE_CARD_WERE_DETECTED, kMoreCardsDetectedUi },
    { TXN_STATE_PLEASE_SEE_PHONE, kPlzSeePhone },
    { TXN_STATE_POLLING_CARD_SUCCESS, kDetectCardSuccessUi},
    { TXN_STATE_PLEASE_REMOVE_CARD, kPlsRemoveCardUi },
    { TXN_STATE_SAF_APPROVED, kSAFApproveUi },
    { TXN_STATE_SAF_DECLINE, kSAFDeclinedUi },
    { TXN_STATE_CONNECTIVITY_AVAILABLE, kConnectAvailable },
    { TXN_STATE_CONNECTIVITY_NOT_AVAILABLE, kConnectNotAvailable }
};

static const struct
{
    const int nTxnState;
    const int nUiIndex;
} s_cActTypeToUiIndex_[] = 
{
    { TXN_STATE_AID_SELECT, kUserSelectAidUi },
    { TXN_STATE_AID_CONFIRM, kUserConfirmAidUi },
    { TXN_STATE_TOTAL_AMOUNT_GET, kUserGetTotalAmtUi },
    { TXN_STATE_EXCEPTION_FILE_CHECK, kUserCheckExceptionFileUi },
    { TXN_STATE_CALL_YOUR_BANK, kUserCallBankUi },
    { TXN_STATE_LAST_PIN_INPUT, kLastInputPINUi },
    { TXN_STATE_PIN_ENTER, kPlsInputPINUi },
    { TXN_STATE_INTERAC_POS_SEQNBR, kTxnInteracPosSeqNbr },
    { TXN_STATE_INTERAC_ACCOUNT_TYPE, kTxnInteracAccountType },
    { TXN_STATE_SELECT_LANGUAGE, kSelectLanguage },
    { TXN_STATE_SWITCH_LANGUAGE, kSwitchLanguage }
};

//
// Kernel single instance.
//
CHPApiKernel CHPApiKernel::cHPApiKernelObj_;

#define ResetActData() { \
    m_cActReqData.m_nActType = -1; \
    m_cActRspData.m_nActType = -1; \
    bzero(m_cActReqData.m_szActData, sizeof(m_cActReqData.m_szActData)); \
    bzero(m_cActRspData.m_szActData, sizeof(m_cActRspData.m_szActData)); \
}

/*
 * Private.
 */
#define _StringAssign(jsonRoot, keyName, buffer, bufferSize) { \
    if (NULL != jsonRoot) {\
        const cJSON *pcSubItem = cJSON_GetObjectItem((jsonRoot), (keyName)); \
        if (NULL != (pcSubItem) && cJSON_String == (pcSubItem)->type) { \
            sprintf((char *)(buffer), "%.*s", ((bufferSize) - 1), pcSubItem->valuestring); \
        } \
        else { \
            TraceMsg("Error: Not found %s item or format error.", keyName); \
        } \
    }\
}

/*
 * Protected.
 */
CHPApiKernel::CHPApiKernel()
{
    cApiThreadId_ = INVALID_THREAD_ID;
    static BOOL bInitKMS = FALSE;
    if (!bInitKMS)
    {
        bInitKMS = TRUE;
        CTOS_KMS2Init();
    }
    ResetActData();
    SetupAPIThread();
}

/*
 * Public.
 */
CHPApiKernel::~CHPApiKernel()
{ }

/*
 * Private
 */
void CHPApiKernel::ProcCancelModeJsonCmd(IN const char *pszJsonCmd)
{
    if (kCancel != m_cCurCancelCmd.GetTxnType())
    {
        CJsonReqCmd cTmpCmd;
        cTmpCmd.ParseCmd(pszJsonCmd);
        if (kCancel == cTmpCmd.GetTxnType())
        {
            TraceMsg("m_cCurCancelCmd received");
            m_cCurCancelCmd.ParseCmd(pszJsonCmd);
        }
    }
}

/*
 * Public.
 */
void CHPApiKernel::PostRequest(IN const char *pszJsonCmd)
{
    TraceMsg("PostRequest--Entry, CurApiMode:%d, JsonCmd=%s",
            GetCurApiMode(), (pszJsonCmd != NULL) ? pszJsonCmd : "");
    EnterCallApi();
    do
    {
        ResetApiMode();
        if (kWaitingNewJsonCmd == GetCurApiMode() ||
            kWaitingUserInputCmd == GetCurApiMode() )
        {
            SetCurApiMode(kStartParsingJsonCmd);
            CJsonReqCmd cReqCmd;
            if (!IsValidJsonCmd(pszJsonCmd, &cReqCmd))
            {
                SetCurApiMode(GetLastApiMode());
                TraceMsg("Info: Invalid Json cmd=\r\n%s", pszJsonCmd);
                break;
            }
            if (kWaitingNewJsonCmd == GetLastApiMode())
            {
                TraceMsg("Info: Normal JSON command");
                m_cCurReqCmd = cReqCmd;
                cReqCmd.DetachJsonObj();
                SetCurApiMode(kBeforeStartProcessingJsonCmd); 
            }
            else if (kWaitingUserInputCmd == GetLastApiMode())
            {
                TraceMsg("Info: User JSON command");
                SetCurApiMode(kStartProcessingUserInputCmd);
                if (ExeUserActionJsonCmd(cReqCmd, pszJsonCmd))
                {
                    SetCurApiMode(kFinishProcessingUserInputCmd);
                    TraceMsg("Info: Processing user input command success.");
                }
                else
                {
                    SetCurApiMode(kWaitingUserInputCmd);
                    TraceMsg("Error: Processing user input command failed.");
                }
            }
        }
        else if (IsEnableCancel())
        {
            ProcCancelModeJsonCmd(pszJsonCmd);
        }
        else
        {
            TraceMsg("Info: Api Kernel busy, KeState=%d, ApiMode=%d, TrdState=%d, JsonCmd=%s",
                     GetCurKeState(), GetCurApiMode(), pthread_kill(cApiThreadId_, 0),
                     ((pszJsonCmd != NULL) ? pszJsonCmd : ""));
            break;
        }
    } while (0);
    LeaveCallApi();
    TraceMsg("PostRequest--Exit");
}

/*
 * Private.
 */
void CHPApiKernel::DefResponse(OUT char *pszBuffer, IN const int nBufSize)
{
    if (m_cCurReqCmd.IsRequestCmdValid())
    {
        const char *pszRspStr = NULL;
        CJsonRspCmd cRspCmd(&m_cCurReqCmd, this);

        int nIndex = GetUiIndex();
        cRspCmd.SetUiIndex(GetUiIndex());
        if (kFinishTxn >= GetCurKeState() && 
             kStartTxn <= GetCurKeState())
        {
            cRspCmd.SetTxnField(m_pParamPtr);
        }
        pszRspStr = cRspCmd.PackResponse();

        if (NULL != pszRspStr &&
            nBufSize > (strlen(pszRspStr) + 1))
        {
            strcpy(pszBuffer, pszRspStr);
        }
        else if (NULL != pszRspStr)
        {
            TraceMsg("Buffer is smaller than the real data: %d. Plz check the input parameters of \"GetResponse\"", strlen(pszRspStr));
        }
    }
}

/* 
 * Protected.
 */
void CHPApiKernel::UserInputResponse(OUT char *pszBuffer,
                                     IN const int nBufSize)
{
    const int nActDataLength = strlen(m_cActReqData.m_szActData);

    if (0 == nActDataLength)
    {
        TraceMsg("Error: Data need to be output to user is empty");
        return ;
    }

    if (nBufSize <= nActDataLength)
    {
        TraceMsg("Error: Buffer too small");
        return ;
    }
    
    CJsonRspCmd cRspCmd(m_cActReqData.m_szActData, GetProcessingStatus());
    cRspCmd.SetUiIndex(GetUiIndex());

    const char *pszRspStr = cRspCmd.PackResponse();
    if (NULL != pszRspStr &&
        nBufSize > (strlen(pszRspStr) + 1))
    {
        strcpy(pszBuffer, pszRspStr);
    }
}

/*
 * Public.
 */
void CHPApiKernel::GetResponse(OUT char *pszBuffer,
                               IN const int nBufSize)
{
    EnterCallApi();
    bzero(pszBuffer, nBufSize);
    do
    {
        if (NULL == pszBuffer || nBufSize <= (nMinJsonRspLen_ + 1)) 
        {
            TraceMsg("Error: Buffer is NULL or size is too small.");
            break;
        }

        if (IsEnableResponse())
        {   
            if (kWaitingUserInputCmd == GetCurApiMode())
            {
                UserInputResponse(pszBuffer, nBufSize);
            }
            else if (kStartProcessingJsonCmd == GetCurApiMode() ||
                     kStartParsingJsonCmd == GetCurApiMode()    ||
                     kWaitingNewJsonCmd == GetCurApiMode())
            {
                DefResponse(pszBuffer, nBufSize);
            }
            else
            {
                strcpy(pszBuffer, EmptyJsonRsponse_);
            }
        }
        else
        {
            strcpy(pszBuffer, EmptyJsonRsponse_);
        }
    } while (0);
    LeaveCallApi();
}

/*
 * Public.
 * Static.
 */
IKernelCtrl* CHPApiKernel::GetKernelCtrlPtr(void)
{
    return dynamic_cast<IKernelCtrl *>((&cHPApiKernelObj_));
}

/*
 * Public.
 * Static.
 */
IUserAction* CHPApiKernel::GetUserActionPtr(void)
{
    return dynamic_cast<IUserAction *>(&cHPApiKernelObj_);
}

/*
 * Protected.
 */
void* CHPApiKernel::APIThreadMainEntry(IN void *pVoid)
{
    pthread_detach(pthread_self());
    CHPApiKernel *pApiKe = static_cast<CHPApiKernel *>(pVoid);

    while (NULL != pApiKe)
    {
        if (kBeforeStartProcessingJsonCmd == pApiKe->GetCurApiMode())
        {
            TraceMsg("Begin call ExeProcessingJsonCmd()");
            pApiKe->SetCurApiMode(kStartProcessingJsonCmd);
            pApiKe->ExeProcessingJsonCmd();
            TraceMsg("End call ExeProcessingJsonCmd()");
        }
        if (kStartProcessingJsonCmd == pApiKe->GetCurApiMode())
        {
            pApiKe->UpdateApiMode();
        }

        usleep(Nop_ApiTrd_Idle_);
        pApiKe->DumpKernelInfo();
    }
    return NULL;
}

/*
 * Private.
 */ 
void CHPApiKernel::ResetApiMode(void)
{
    if (kStartProcessingJsonCmd == GetCurApiMode())
    {
        if (kInitHLLibraryFailed == GetCurKeState() ||
            kNoSendInitJsonCmd == GetCurKeState() ||
            kInvalid == GetCurKeState())
        {
            SetCurKeState(kInvalid);
            SetCurApiMode(kWaitingNewJsonCmd);
            SetCurStateTimeout(0xFFFFFFFF);
            TraceMsg("Info: ApiMode return wait, keState return invalid.");
        }
        else if (kDetectCardTimeout == GetCurKeState() ||
                 kCancelDetectCard == GetCurKeState() ||
                 kFinishTxn == GetCurKeState() ||
                 kFinishCancelTxn == GetCurKeState() ||
                 kFinishUpdateData == GetCurKeState() ||
                 kIdle == GetCurKeState())
        {
            SetCurKeState(kIdle);
            SetCurApiMode(kWaitingNewJsonCmd);
            SetCurStateTimeout(0xFFFFFFFF);
            TraceMsg("Info: ApiMode return wait, keState return idle.");
            TraceMsg("Info: ApiMode %d, CurKeState=%d.", GetCurApiMode(), GetCurKeState());
        }
    }
}

/*
 * Private.
 */
BOOL CHPApiKernel::ExeUserActionJsonCmd(
    IN const CJsonReqCmd &cReqCmd,
    IN const char *pszSrcJson)
{
    TraceMsg("ExeUserActionJsonCmd--Entry");
    BOOL bResult = FALSE;
    LockDataAccess();
    if (cReqCmd.GetActType() == m_cActReqData.m_nActType)
    {
        if (NULL != pszSrcJson && 
            strlen(pszSrcJson) < (sizeof(m_cActRspData.m_szActData) - 1))
        {
            bzero(m_cActRspData.m_szActData, sizeof(m_cActRspData.m_szActData));
            strcpy(m_cActRspData.m_szActData, pszSrcJson);
            TraceMsg("Info: response is valid.");
            bResult = TRUE;
        }
        else
        {
            TraceMsg("Error: pszJson data error.");
        }
    }
    else if(cReqCmd.GetTxnType() == kCancel)
    {
        TraceMsg("Info: received cancel.");
        TraceMsg("Info: m_cActReqData.m_nActType%d", m_cActReqData.m_nActType);
        // user canceled the user action, 

        if (m_cActReqData.m_nActType == CJsonReqCmd::StringToActType(JV_Act_AidList))
        {
            cJSON* pJsonCmd = cJSON_Parse(m_cActReqData.m_szActData);
            cJSON_DeleteItemFromObject(pJsonCmd, JK_Act_Value);
            cJSON_AddStringToObject(pJsonCmd, JK_Act_Value, JV_Cancel);
            char* pszJsonStr = cJSON_Print(pJsonCmd);
            
            bzero(m_cActRspData.m_szActData, sizeof(m_cActRspData.m_szActData));
            m_cActRspData.m_nActType = m_cActReqData.m_nActType;
            strcpy(m_cActRspData.m_szActData, pszJsonStr);

            TraceMsg("Info: user action is canceled.");
            bResult = TRUE;

            if (pJsonCmd) 
                cJSON_Delete(pJsonCmd);

            if (pszJsonStr) 
                free(pszJsonStr);
        }
    }
    else
    {
        // User send invalid json command, 
        // return waiting json command mode.
        TraceMsg("Error: Act type not equal.");
        TraceMsg("Info: Current Act Type=%d, Request Act Type=%d",
                  cReqCmd.GetActType(), m_cActReqData.m_nActType);
    }
    UnlockDataAccess();
    TraceMsg("ExeUserActionJsonCmd--Exit");
    return bResult;
}


/*
 * Private
 * 
 * Description: Print kernel state and api mode.
 */
void CHPApiKernel::DumpKernelInfo(void)
{
    static int nLastApiMode_ = -1;
    static int nLastKeState_ = -1;
    static int nLastTxnState_ = -1;

    if (nLastApiMode_ != GetCurApiMode() || 
       nLastKeState_ != GetCurKeState() ||
       nLastTxnState_ != CTxnBase::GetState())
    {
        nLastApiMode_ = GetCurApiMode();
        nLastKeState_ = GetCurKeState();
        nLastTxnState_ = CTxnBase::GetState();
        TraceMsg("Info: ThreadId=%lu, ApiMode=%d, KeState=%d, TxnState=%d",
                 pthread_self(),
                 nLastApiMode_,
                 nLastKeState_,
                 nLastTxnState_);
    }
}

/*
 * Virtual protected.
 */
void CHPApiKernel::ExeUpdateData(void)
{
    TraceMsg("ExeUpdateDate--Entry");
    SetCurKeState(kProcessingUpdateData);
    LockDataAccess();
    const int nCurCmdType = m_cCurReqCmd.GetTxnType();
    UnlockDataAccess();

    if (kUpdateData == nCurCmdType)
    {
        HEARTLAND_GENERAL_PARAM cGeneralParam = {0};
        tagParam cTagParam = {0};

        InitTagParam(cTagParam, kGeneralParam, cGeneralParam);
        m_pParamPtr->GetParameter(&cTagParam);

        LockDataAccess();
        const cJSON *pcJson = m_cCurReqCmd.GetJsonObjPtr();
        _StringAssign(pcJson, JK_CFG_Voltage_Flag, cGeneralParam.szVoltageFlag, sizeof(cGeneralParam.szVoltageFlag));
        _StringAssign(pcJson, JK_CFG_Token_Flag, cGeneralParam.szTokenizationFlag, sizeof(cGeneralParam.szTokenizationFlag));
        UnlockDataAccess();

        m_pParamPtr->SetParameter(cTagParam);
        SetCurKeState(kFinishUpdateData);
    }
    else
    {
        TraceMsg("Error: Json command passing error.");
        SetCurKeState(kIdle);
    }
    TraceMsg("ExeUpdateDate--Exit");
}

/*
 * Protected.
 */
void CHPApiKernel::ExeProcessingJsonCmd(void)
{
    const int nTxnType = m_cCurReqCmd.GetTxnType();
    const cJSON *pcJsonCmd = m_cCurReqCmd.GetJsonObjPtr();

    if (NULL != pcJsonCmd)
    {
        switch (nTxnType)
        {
        case kWriteData:
            ExeWriteDataCmd();
            break;

        case kSale:
        case kVoid:
        case kPreAuth:
        case kReturn:
        case kAuthComplete:
        case kSettlement:
        case kReversal:
        case kPollingCard:
        case kSAF:
        case kCheckConnectivity:
            ExeCurReqTxnCmd();
            break;

        case kGetAppVer:
        case kGetKernelVer:
            break;

        case kUpdateData:
            ExeUpdateData();
            break;

        default:
            break;
        }
        TraceMsg("Info: TxnType=%d, Name=%s", nTxnType,
                 CJsonReqCmd::TxnTypeToString(nTxnType));
    }

    if (kCancelDetectCard == GetCurKeState())
    {
        TraceMsg("Info: User cancel detect card.");
        SetCurStateTimeout(0x0);
    }
    else if (kDetectCardTimeout == GetCurKeState())
    {
        TraceMsg("Info: Detected card timeout");
    }
    else
    {
        TraceMsg("Info: Other state, state=%d", GetCurKeState());
    }
}

/*
 * Protected.
 */
void CHPApiKernel::SetupAPIThread(void)
{
    LockDataAccess();
    if (INVALID_THREAD_ID == cApiThreadId_)
    {
        const int nRet = pthread_create(&cApiThreadId_,
                                        NULL,
                                        APIThreadMainEntry,
                                        static_cast<void *>(this));
        if (0 != nRet)
        {
            cApiThreadId_ = INVALID_THREAD_ID;
            TraceMsg("Error: pthread_create failed, ErrorInfo=%s", strerror(errno));
        }
    }
    UnlockDataAccess();
}

/*
 * Protected.
 */
BOOL CHPApiKernel::IsValidJsonCmd(IN const char *pszJsonCmd, OUT CJsonReqCmd *pcReqJsonCmd)
{
    BOOL bResult = FALSE;
    const int nCurKeState = GetCurKeState();
    const int nApiMode = GetCurApiMode();

    if (nApiMode != kStartParsingJsonCmd)
    {
        return FALSE;
    }

    do
    {
        if (!pcReqJsonCmd->ParseCmd(pszJsonCmd))
            break;

        if (!pcReqJsonCmd->IsRequestCmdValid())
            break;

        // Must be initialize first.
        if (kWriteData != pcReqJsonCmd->GetTxnType() &&
            (kInvalid == nCurKeState || kInitHLLibraryFailed == nCurKeState))
        { 
            TraceMsg("Error: payment sdk init failed");
            SetCurKeState(kNoSendInitJsonCmd);
            break; 
        }
        
        bResult = TRUE;
    } while (0);
    
    return bResult;
}

/*
 * Protected.
 */
int CHPApiKernel::GetUiIndex(void)
{
    int nUiIndex = kInvalidUiIndex;
    const int nKeState = GetCurKeState();

    switch (nKeState)
    {
    case kIdle:
        nUiIndex = kIdleUi;
        break;

    case kProcessingUpdateData:
        nUiIndex = kParsingCmdUi;
        break;

    case kStartDetectingCard:
        nUiIndex = kDetectingCardUi;
        break;

    case kDetectCardTimeout:
        nUiIndex = kDetectedCardTimeoutUi;
        break;

    // case kDetectedCard:
    //     nUiIndex = kDetectCardSuccessUi;
    //     break;
    
    case kProcessingTxn:
    case kFinishTxn:
        nUiIndex = GetUiIndexByTxnState();
        break;
        
    case kUserActionType:
        nUiIndex = GetUiIndexByActType();
        break;

    default:
        break;
    }

    return nUiIndex;
}


/*
 * Protected.
 */
int CHPApiKernel::GetUiIndexByTxnState(void)
{
    return GetUiIndexByTxnState(CTxnBase::GetState());
}


/*
 * Protected.
 */
int CHPApiKernel::GetUiIndexByTxnState(IN const int &nTxnState)
{
    int nUiIndex = kInvalidUiIndex;
    const int nMax = sizeof(s_cTxnStateToUiIndex_)/sizeof(s_cTxnStateToUiIndex_[0]);

    for (int i = 0; i < nMax; ++i)
    {
        if (nTxnState == s_cTxnStateToUiIndex_[i].nTxnState)
        {
            nUiIndex = s_cTxnStateToUiIndex_[i].nUiIndex;
            if (nTxnState == TXN_STATE_APPROVAL ||
                nTxnState == TXN_STATE_OFFLINE_APPROVED)
            {
                AddProcessingStatus(ProcStatus_TxnApproval);
            }
            else if (nTxnState == TXN_STATE_DECLINED ||
                nTxnState == TXN_STATE_OFFLINE_DECLINED)
            {
                AddProcessingStatus(ProcStatus_TxnDecline);
            }
            else if (nTxnState == TXN_STATE_READING_CARD_FAIL)
            {
                AddProcessingStatus(ProcStatus_ReadCardDataFailed);
            }
            break;
        }
    }
    static int nLastTxnState = TXN_STATE_INVALID;
    if (nLastTxnState != nTxnState)
    {
        TraceMsg("Info: nTxnState:%d-->UiIndex=%x", nTxnState, nUiIndex);
        nLastTxnState = nTxnState;
    }
    return nUiIndex;
}


/*
 * Protected.
 */
int CHPApiKernel::GetUiIndexByActType(void)
{
    int nUiIndex = kInvalidUiIndex;
    const int nMax = sizeof(s_cActTypeToUiIndex_) / sizeof(s_cActTypeToUiIndex_[0]);
    for (int i = 0; i < nMax; ++i)
    {
        if (m_cActReqData.m_nActType == s_cActTypeToUiIndex_[i].nTxnState)
        {
            nUiIndex = s_cActTypeToUiIndex_[i].nUiIndex;
            break;
        }
    }
    
    static int nLastActType = TXN_STATE_INVALID;
    if (nLastActType != m_cActReqData.m_nActType)
    {
        TraceMsg("Info: nActType:%d-->UiIndex=%x", m_cActReqData.m_nActType, nUiIndex);
        nLastActType = m_cActReqData.m_nActType;
    }

    return nUiIndex;    
}

/*
 * Virtual, Protected.
 * Execute initialize command.
 */
void CHPApiKernel::ExeWriteDataCmd(void)
{
    TraceMsg("Info: ExeWriteDataCmd entry");
    SetCurKeState(kStartInitHLLibrary);
    const cJSON *pcJson = m_cCurReqCmd.GetJsonObjPtr();
    if (Init(pcJson))
    {
        // Initialize API kernel success.
        SetCurKeState(kIdle);
        AddProcessingStatus(ProcStatus_InitSuccess);
        TraceMsg("Info: Init OK");
    }
    else
    {
        // Initialize API kernel failed.
        SetCurKeState(kInitHLLibraryFailed);
        TraceMsg("Info: Init fail");
    }
    TraceMsg("Info: ExeWriteDataCmd exit");
}

/*
 * Private.
 */
BOOL CHPApiKernel::Init(IN const cJSON *pcJson)
{
    TraceMsg("Init--Entry");
    static char szEmvConfigFile[260] = { 0 };
    static char szEmvclConfigFile[260] = { 0 };

    BOOL bInitRet = FALSE;
    tagParam cParamVar;
    HEARTLAND_CURL_PARAM curlParam;
    HEARTLAND_EMVCL_PARAM emvclParam;
    HEARTLAND_EMV_PARAM emvParam;
    HEARTLAND_PINENTRY_PARAM cKMSParam;
    HEARTLAND_GENERAL_PARAM cGeneralParam;
    ST_HSOAP_INIT cSoapInit;
    CTxnBase* pBase = NULL;
    int nRet;

    do
    {
        if (NULL == pcJson || NULL == m_pParamPtr)
        {
            TraceMsg("Error: Invalid Parameter");
            break;
        }

        // Curl Parameter
        TraceMsg("Info: Curl Parameter");
        bzero(&cParamVar, sizeof(tagParam));
        bzero(&curlParam, sizeof(curlParam));
        _StringAssign(pcJson, JK_CFG_Http_Method, curlParam.szHttpMethod, sizeof(curlParam.szHttpMethod));
        _StringAssign(pcJson, JK_CFG_CertFile, curlParam.szCertFile, sizeof(curlParam.szCertFile));
        _StringAssign(pcJson, JK_CFG_Cert_Path, curlParam.szCertPath, sizeof(curlParam.szCertPath));
        _StringAssign(pcJson, JK_CFG_Host_Url, curlParam.szHostUrl, sizeof(curlParam.szHostUrl));

        InitTagParam(cParamVar, kCurlParam, curlParam);
        nRet = m_pParamPtr->SetParameter(cParamVar);
        if (nRet)
        {
            TraceMsg("Error: Call SetParameter set kCurParam failed.");
            break;
        }

        char szDebugFlag[16] = { 0 };
        // emvclParam
        TraceMsg("Info: emvclParam Parameter");
        bzero(&cParamVar, sizeof(tagParam));
        bzero(&emvclParam, sizeof(emvclParam));
        bzero(szEmvclConfigFile, sizeof(szEmvclConfigFile));
        _StringAssign(pcJson, JK_CFG_EMVCL_Config, szEmvclConfigFile, sizeof(szEmvclConfigFile));
        _StringAssign(pcJson, JK_CFG_EMVCL_Debug_Flag, szDebugFlag, sizeof(szDebugFlag));
        emvclParam.m_cInitData.pConfigFilename = (unsigned char*)szEmvclConfigFile;
        emvclParam.m_cInitData.bConfigFilenameLen = strlen(szEmvclConfigFile);
        emvclParam.m_cInitData.stOnEvent.OnCancelTransaction = NULL;
        emvclParam.m_cInitData.stOnEvent.OnShowMessage = NULL;

        emvclParam.m_cParamData.uiNoP = 1;
        emvclParam.m_cParamData.uiIndex[0] = d_EMVCL_PARA_INDEX_SALE_TIMEOUT;
        emvclParam.m_cParamData.uiLen[0] = 2;
        emvclParam.m_cParamData.baData[0][0] = 0x3A;
        emvclParam.m_cParamData.baData[0][1] = 0x98;
        if (strcasecmp(szDebugFlag, "true") == 0)
            emvclParam.m_nDebugFlag = 1;

        InitTagParam(cParamVar, kEmvclParam, emvclParam);
        nRet = m_pParamPtr->SetParameter(cParamVar);
        if (nRet)
        {
            TraceMsg("Error: Call SetParameter set kEmvclParam failed.");
            break;
        }
        
        // emvParam
        TraceMsg("Info: emvParam Parameter");
        bzero(&cParamVar, sizeof(tagParam));
        bzero(&emvParam, sizeof(emvParam));
        bzero(szEmvConfigFile, sizeof(szEmvConfigFile));
        bzero(&szDebugFlag, sizeof(szDebugFlag));
        _StringAssign(pcJson, JK_CFG_EMV_Config, szEmvConfigFile, sizeof(szEmvConfigFile));
        _StringAssign(pcJson, JK_CFG_EMV_Debug_Flag, szDebugFlag, sizeof(szDebugFlag));
        emvParam.m_pszConfigFile = szEmvConfigFile;
        if (strcasecmp(szDebugFlag, "true") == 0)
            emvParam.m_nDebugFlag = 1;

        InitTagParam(cParamVar, kEmvParam, emvParam);
        nRet = m_pParamPtr->SetParameter(cParamVar);
        if (nRet)
        {
            TraceMsg("Error: Call SetParameter set kEmvParam failed.");
            break;
        }
        
        // general parameters
        TraceMsg("Info: general Parameter");
        char szUserActionBitMap[16] = { 0 };
        bzero(&cParamVar, sizeof(tagParam));
        bzero(&cGeneralParam, sizeof(cGeneralParam));
        _StringAssign(pcJson, JK_CFG_ARQC_TIMEOUT, cGeneralParam.szARQCTimeout, sizeof(cGeneralParam.szARQCTimeout));
        _StringAssign(pcJson, JK_CFG_PIN_Entry_Timeout, cGeneralParam.szPINEntryTimeout, sizeof(cGeneralParam.szPINEntryTimeout));
        _StringAssign(pcJson, JK_CFG_Poll_Card_Timeout, cGeneralParam.szPollCardTimeout, sizeof(cGeneralParam.szPollCardTimeout));
        _StringAssign(pcJson, JK_CFG_AID_Select_Timeout, cGeneralParam.szAIDSelectTimeout, sizeof(cGeneralParam.szAIDSelectTimeout));
        _StringAssign(pcJson, JK_CFG_Pullout_Card_Timeout, cGeneralParam.szPulloutCardTimeout, sizeof(cGeneralParam.szPulloutCardTimeout));
        _StringAssign(pcJson, JK_CFG_Voltage_Flag, cGeneralParam.szVoltageFlag, sizeof(cGeneralParam.szVoltageFlag));
        _StringAssign(pcJson, JK_CFG_BB_Param_File, cGeneralParam.szBBParamFile, sizeof(cGeneralParam.szBBParamFile));
        _StringAssign(pcJson, JK_CFG_Token_Flag, cGeneralParam.szTokenizationFlag, sizeof(cGeneralParam.szTokenizationFlag));
        _StringAssign(pcJson, JK_CFG_UniqueDeviceId, cGeneralParam.szTxnUniqueDebiceId, sizeof(cGeneralParam.szTxnUniqueDebiceId));
        _StringAssign(pcJson, JK_CFG_Txn_Descriptor, cGeneralParam.szTxnDescriptor, sizeof(cGeneralParam.szTxnDescriptor));
        _StringAssign(pcJson, JK_CFG_Cert_Name, cGeneralParam.szCertName, sizeof(cGeneralParam.szCertName));
        _StringAssign(pcJson, JK_CFG_Strategy_Flag, cGeneralParam.szStrategyFlag, sizeof(cGeneralParam.szStrategyFlag));
        _StringAssign(pcJson, JK_CFG_SAF_MODE, cGeneralParam.szSAFModeFlag, sizeof(cGeneralParam.szSAFModeFlag));
        _StringAssign(pcJson, JK_CFG_SAF_LIMIT_AMOUNT, cGeneralParam.szLimitAmount, sizeof(cGeneralParam.szLimitAmount));
        _StringAssign(pcJson, JK_CFG_MaskedAcctMode, cGeneralParam.szMaskedAcctMode, sizeof(cGeneralParam.szMaskedAcctMode));
        _StringAssign(pcJson, JK_CFG_UserAct_Bitmap, szUserActionBitMap, sizeof(szUserActionBitMap));
        cGeneralParam.dwUserActionBitMap = strtol(szUserActionBitMap, NULL, 16);

        bzero(&szUserActionBitMap, sizeof(szUserActionBitMap));
        _StringAssign(pcJson, JK_CFG_PaymentDebugFlag, szUserActionBitMap, sizeof(szUserActionBitMap));
        if (strcasecmp(szUserActionBitMap, "false") == 0)
        {
            SetPaymentDebugFlag(false);
        }

        TraceMsg("Info: cGeneralParam.szVoltageFlag:%s, %p", cGeneralParam.szVoltageFlag, &cGeneralParam);
        InitTagParam(cParamVar, kGeneralParam, cGeneralParam);
        nRet = m_pParamPtr->SetParameter(cParamVar);
        if (nRet)
        {
            TraceMsg("Error: Call SetParameter set cGeneralParam failed.");
            break;
        }

        int nPinTimeout = atoi(cGeneralParam.szPINEntryTimeout);
        
        // KMS Param
        TraceMsg("Info: KMS Parameter");
        bzero(&cParamVar, sizeof(tagParam));
        bzero(&cKMSParam, sizeof(cKMSParam));
        cKMSParam.cKms2PinObj.Version = 0x00;
        cKMSParam.cKms2PinObj.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
        cKMSParam.cKms2PinObj.PIN_Info.PINDigitMinLength = 4;
        cKMSParam.cKms2PinObj.PIN_Info.PINDigitMaxLength = 12;

        cKMSParam.cKms2PinObj.Protection.CipherKeySet = HEARTLAND_PRIVATE_KEY_SET_1;
        cKMSParam.cKms2PinObj.Protection.CipherKeyIndex = HEARTLAND_PRIVATE_KEY_IDX_1;
        cKMSParam.cKms2PinObj.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
        cKMSParam.cKms2PinObj.Protection.SK_Length = 0;

        cKMSParam.cKms2PinObj.DUKPT_PARA.IsUseCurrentKey = FALSE;    

        cKMSParam.cKms2PinObj.Control.Timeout = (nPinTimeout==0 ? 30 : nPinTimeout);
        cKMSParam.cKms2PinObj.Control.AsteriskPositionX = 7;
        cKMSParam.cKms2PinObj.Control.AsteriskPositionY = 5;
        cKMSParam.cKms2PinObj.Control.NULLPIN = TRUE;
        cKMSParam.cKms2PinObj.Control.piTestCancel = NULL;
    
        InitTagParam(cParamVar, kPinParam, cKMSParam);
        nRet = m_pParamPtr->SetParameter(cParamVar);
        if (nRet)
        {
            TraceMsg("Error: Call SetParameter set kPinParam failed.");
            break;
        }
        
        // soap kernel init
        TraceMsg("Info: soap kernel Parameter");
        bzero(&cParamVar, sizeof(tagParam));
        bzero(&cSoapInit, sizeof(cSoapInit));
        _StringAssign(pcJson, JK_CFG_POS_Gateway_Url, cSoapInit.szPosGatewayUrl, sizeof(cSoapInit.szPosGatewayUrl));
        _StringAssign(pcJson, JK_CFG_License_ID, cSoapInit.szLicenseId, sizeof(cSoapInit.szLicenseId));
        _StringAssign(pcJson, JK_CFG_Site_ID, cSoapInit.szSiteId, sizeof(cSoapInit.szSiteId));
        _StringAssign(pcJson, JK_CFG_Device_ID, cSoapInit.szDeviceId, sizeof(cSoapInit.szDeviceId));
        _StringAssign(pcJson, JK_CFG_User_Name, cSoapInit.szUserName, sizeof(cSoapInit.szUserName));
        _StringAssign(pcJson, JK_CFG_PWD, cSoapInit.szPassword, sizeof(cSoapInit.szPassword));
        _StringAssign(pcJson, JK_CFG_Developer_ID, cSoapInit.szDeveloperID, sizeof(cSoapInit.szDeveloperID));
        _StringAssign(pcJson, JK_CFG_Version_Nbr, cSoapInit.szVersionNbr, sizeof(cSoapInit.szVersionNbr));

        HeartlandSoap* pSoapKernel = HLKE_GetSoapKernelPtr();
        nRet = pSoapKernel->HeartlandSOAP_Init(&cSoapInit); 
        if (nRet)
        {
            TraceMsg("Error: Call HeartlandSOAP_Init failed, nRet:%d", nRet);
            break;
        }

#ifdef PLATFORM_ANDROID
       pBase = HLKE_GetEMVTxnPtr();
        pBase->InitLevel2Lib();
        if (nRet)
        {
            TraceMsg("Error: Call InitLevel2Lib failed, nRet:%d", nRet);
            break;
        }

        pBase = HLKE_GetEMVCLTxnPtr();
        pBase->InitLevel2Lib();
        if (nRet)
        {
            TraceMsg("Error: Call InitLevel2Lib failed, nRet:%d", nRet);
            break;
        }
#endif

        // Finally, Success.
        TraceMsg("Info: Finally, Success");
        bInitRet = TRUE;
    } while (0);
    
    TraceMsg("Init--Ext, Ret=%d", bInitRet);
    return bInitRet;
}

/*
 * Private.
 */
BOOL CHPApiKernel::IsNeedDetectCard(void)
{
    BOOL bRet = FALSE;
    const int nCmdType = m_cCurReqCmd.GetTxnType();

    if ((kSale == nCmdType ||
        kPreAuth == nCmdType ||
        kPollingCard == nCmdType ||
        kReturn == nCmdType) && (strlen(m_cCurReqCmd.GetTxnTokenValue()) == 0))
    {
        bRet = TRUE;
    }
    
    return bRet;
}

/*
 * Private.
 */
BOOL CHPApiKernel::IsEnableCancelDetectCard(void)
{
    return TRUE;
}

/*
 * Private.
 */
unsigned long CHPApiKernel::GetDetectCardTimeout(void)
{
    static unsigned long ulDetectCardTimeout = 0;
    
    if (ulDetectCardTimeout == 0)
    {
        tagParam cTagParam = { 0 };
        HEARTLAND_GENERAL_PARAM cGeneralParam = { 0 };

        InitTagParam(cTagParam, kGeneralParam, cGeneralParam);
        m_pParamPtr->GetParameter(&cTagParam);
        
        ulDetectCardTimeout = strtoul(cGeneralParam.szPollCardTimeout, NULL, 10);
        ulDetectCardTimeout *= 1000;
        if (ulDetectCardTimeout == 0)
        {
            ulDetectCardTimeout = 0xFFFFFFFF;
        }
    }
    TraceMsg("ulDetectCardTimeout: %lu", ulDetectCardTimeout);
    
    return ulDetectCardTimeout;
}

/*
 * Private.
 */
void CHPApiKernel::ExeClearLastDetectInfo(void)
{
    CDetectCard *pDetectObj = HLKE_GetDetectedCardPtr();
    if (NULL != pDetectObj)
    {
        pDetectObj->ClearLastDetectInfo();
    }
}

/*
 * Private.
 */
BOOL CHPApiKernel::ExeDetectCard(OUT int *pnCardType)
{
    BOOL bResult = FALSE;
    (*pnCardType) = kNoneCard;

    int nCardType = IsTxnSeqNbrMatched();
    if (nCardType != 0)
    {
        *pnCardType = nCardType;
        return TRUE;
    }

    do
    {
        SetCurKeState(kStartDetectingCard);
        if (IsEnableCancelDetectCard())
        {
            EnableCancel(TRUE);
        }
        const unsigned long ulDetectTime = CTOS_TickGet();
        const unsigned long ulTimeoutValue = GetDetectCardTimeout() / 10;
        CDetectCard *pDetectCard = HLKE_GetDetectedCardPtr();
        int nDetectedCardType = -1;

        while (TRUE)
        {
            pDetectCard->Detect(&nDetectedCardType);
            if (kMSR == nDetectedCardType ||
                kEmv == nDetectedCardType ||
                kEmvCL == nDetectedCardType)
            {
                (*pnCardType) = nDetectedCardType;
                SetCurKeState(kDetectedCard);
                AddProcessingStatus(ProcStatus_PollingCardSuccess);
                break;
            }
            
            // TraceMsg("m_cCurCancelCmd kCancel:%d, TxnType:%d", kCancel, m_cCurCancelCmd.GetTxnType());
            if (kCancel == m_cCurCancelCmd.GetTxnType())
            {
                // User cancel Detected card.
                SetCurKeState(kCancelDetectCard);
                AddProcessingStatus(ProcStatus_CancelPollingCard);
                TraceMsg("Info: Recv user cancel cmd");
                goto __func_End;
            }

            if ((CTOS_TickGet() - ulDetectTime) > ulTimeoutValue)
            {
                // Detected card timeout.
                TraceMsg("Info: Detected card timeout.");
                SetCurKeState(kDetectCardTimeout);
                AddProcessingStatus(ProcStatus_PollingCardTimeout);
                TraceMsg("Info: Detect card timeout");
                goto __func_End;
            }
            usleep(Nop_Detect_Card_);
        }

        bResult = TRUE;
    } while (0);

__func_End:
    if (IsEnableCancelDetectCard())
    {
        EnableCancel(FALSE);
    }
    return bResult;
}


/*
 * Private.
 */
BOOL CHPApiKernel::ExeDetectCard(OUT int *pnCardType, IN const int nCurKeState, IN const int nCardSet)
{
    BOOL bResult = FALSE;
    (*pnCardType) = kNoneCard;

    do
    {
        SetCurKeState(nCurKeState);
        if (IsEnableCancelDetectCard())
        {
            EnableCancel(TRUE);
        }
        const unsigned long ulDetectTime = CTOS_TickGet();
        const unsigned long ulTimeoutValue = GetDetectCardTimeout() / 10;
        CDetectCard *pDetectCard = HLKE_GetDetectedCardPtr();
        int nDetectedCardType = -1;

        while (TRUE)
        {
            pDetectCard->Detect(&nDetectedCardType, nCardSet);
            if (kMSR == nDetectedCardType ||
                kEmv == nDetectedCardType ||
                kEmvCL == nDetectedCardType)
            {
                (*pnCardType) = nDetectedCardType;
                SetCurKeState(kDetectedCard);
                break;
            }
            
            // TraceMsg("m_cCurCancelCmd kCancel:%d, TxnType:%d", kCancel, m_cCurCancelCmd.GetTxnType());
            if (kCancel == m_cCurCancelCmd.GetTxnType())
            {
                // User cancel Detected card.
                SetCurKeState(kCancelDetectCard);
                TraceMsg("Info: Recv user cancel cmd");
                goto __func_End;
            }

            if ((CTOS_TickGet() - ulDetectTime) > ulTimeoutValue)
            {
                // Detected card timeout.
                TraceMsg("Info: Detected card timeout.");
                SetCurKeState(kDetectCardTimeout);
                TraceMsg("Info: Detect card timeout");
                goto __func_End;
            }
            usleep(Nop_Detect_Card_);
        }

        bResult = TRUE;
    } while (0);

__func_End:
    if (IsEnableCancelDetectCard())
    {
        EnableCancel(FALSE);
    }
    return bResult;
}


/*
 * Private.
 */
int CHPApiKernel::GetPackSoapType(void)
{
    int nPackSoapType = kInvalidType;
    static const struct
    {
        const int nTxnCmdType;
        const int nCmdSetCardType;
        const int nPackSoapType;
    } soapPackBuf_[] = 
    {
        {kSale, kCmdSetDebit, kDebitSale_SoapType},
        {kSale, kCmdSetCredit, kCreditSale_SoapType},
        {kVoid, kCmdUnsetCardType, kCreditVoid_SoapType},
        {kVoid, kCmdSetCredit, kCreditVoid_SoapType},
        {kVoid, kCmdSetDebit, kCreditVoid_SoapType},
        {kReturn, kCmdSetCredit, kCreditReturn_SoapType},
        {kReturn, kCmdSetDebit, kDebitReturn_SoapType},
        {kSettlement, kCmdUnsetCardType, kBatchClose_SoapType},
        {kSettlement, kCmdSetCredit, kBatchClose_SoapType},
        {kSettlement, kCmdSetDebit, kBatchClose_SoapType},
        {kPreAuth, kCmdUnsetCardType, kPreAuth_SoapType},
        {kPreAuth, kCmdSetCredit, kPreAuth_SoapType},
        {kAuthComplete, kCmdUnsetCardType, kAuthComplete_SoapType},
        {kAuthComplete, kCmdSetCredit, kAuthComplete_SoapType},
        {kReversal, kCmdUnsetCardType, kManualCreditReversal_SoapType},
        {kReversal, kCmdSetCredit, kManualCreditReversal_SoapType},
        {kReversal, kCmdSetDebit, kManualDebitReversal_SoapType},
        {kSAF, kCmdUnsetCardType, kSAF_SoapType},
        {kSAF, kCmdSetCredit, kSAF_SoapType},
        {kSAF, kCmdSetDebit, kSAF_SoapType},
        {kCheckConnectivity, kCmdUnsetCardType, kCheckConnectivity_SoapType},
        {kCheckConnectivity, kCmdSetCredit, kCheckConnectivity_SoapType},
        {kCheckConnectivity, kCmdSetDebit, kCheckConnectivity_SoapType},
        {kPollingCard, kCmdSetCredit, kCreditPollingCard_TxnType},
        {kPollingCard, kCmdUnsetCardType, kCreditPollingCard_TxnType},
        {kPollingCard, kCmdSetDebit, kDebitPollingCard_TxnType}
    };

    const int nCmdType = m_cCurReqCmd.GetTxnType();
    const int nCmdSetCardType = m_cCurReqCmd.GetCardType();
    const size_t nMaxType = sizeof(soapPackBuf_)/sizeof(soapPackBuf_[0]);

    for (size_t i=0; i < nMaxType; ++i)
    {
        if (nCmdType == soapPackBuf_[i].nTxnCmdType &&
            nCmdSetCardType == soapPackBuf_[i].nCmdSetCardType)
            {
                nPackSoapType = soapPackBuf_[i].nPackSoapType;
                break;
            }
    }
    return nPackSoapType;
}

/*
 * Private.
 */
void CHPApiKernel::SetTxnParam(IN const cJSON *pcJsonParam)
{
    tagParam cTagParam;
    HEARTLAND_AMOUNT_PARAM cAmtParam;
    const cJSON *pcSubItem = NULL;

    bzero(&cTagParam, sizeof(cTagParam));
    bzero(&cAmtParam, sizeof(cAmtParam));
    cAmtParam.nPackSoapType = GetPackSoapType();
    cAmtParam.ullAmount = m_cCurReqCmd.GetAmt();
    cAmtParam.ullOtherAmount = m_cCurReqCmd.GetOtherAmt();
    strcpy(cAmtParam.szTokenValueBuf, m_cCurReqCmd.GetTxnTokenValue());

    TraceMsg("Info: SoapType:%d", cAmtParam.nPackSoapType);
    TraceMsg("Info: nTxnType:%d", m_cCurReqCmd.GetTxnType());

    // Sequence Number
    pcSubItem = cJSON_GetObjectItem(pcJsonParam, JK_Txn_SequenceId);
    if (NULL != pcSubItem && cJSON_Number == pcSubItem->type)
    {
        cAmtParam.ulSeqeueceId = pcSubItem->valuedouble;
    }
    else if(pcSubItem)
    {
        TraceMsg("Error: SequenceId is null or not a number:%d, %f, %d, %s", 
                 pcSubItem->type, pcSubItem->valuedouble, pcSubItem->valueint, pcSubItem->valuestring);
    }
    else
    {
        TraceMsg("Error: txnSequenceID is empty");
    }

    // Online Flag, only for 365 project
    cAmtParam.byOnlineFlag = TRUE;
    pcSubItem = cJSON_GetObjectItem(pcJsonParam, JK_CFG_OnlineFlag);
    if (NULL != pcSubItem && cJSON_String == pcSubItem->type)
    {
        if (strcasecmp(pcSubItem->valuestring, "false") == 0)
            cAmtParam.byOnlineFlag = FALSE;
    }

    pcSubItem = cJSON_GetObjectItem(pcJsonParam, JK_Txn_SafSequenceID);
    if (NULL != pcSubItem && cJSON_String == pcSubItem->type)
    {
        if (strlen(pcSubItem->valuestring) != 0)
        {
            static const int nMaxBufSize_ = sizeof(cAmtParam.szTxnSafSequenceID) - 1;
            sprintf(cAmtParam.szTxnSafSequenceID, "%.*s",
                    nMaxBufSize_, pcSubItem->valuestring);
        }
    }

    cAmtParam.byPartialAuthFlag = TRUE;
    pcSubItem = cJSON_GetObjectItem(pcJsonParam, JK_CFG_AllowPartialAuthFlag);
    if (NULL != pcSubItem && cJSON_String == pcSubItem->type)
    {
        if (strcasecmp(pcSubItem->valuestring, "false") == 0)
            cAmtParam.byPartialAuthFlag = FALSE;
    }
    

    // Gateway Id
    pcSubItem = cJSON_GetObjectItem(pcJsonParam, JK_Txn_Gateway_Id);
    if (NULL != pcSubItem &&
        cJSON_String == pcSubItem->type &&
        0 != strlen(pcSubItem->valuestring))
    {
        static const int nMaxBufSize_ = sizeof(cAmtParam.szGatewayIdBuf) - 1;
        sprintf(cAmtParam.szGatewayIdBuf, "%.*s",
                nMaxBufSize_, pcSubItem->valuestring);
    }

    const int nTxnType = m_cCurReqCmd.GetTxnType();
    if (nTxnType == kSAF)
    {
        //txnRequestData
        pcSubItem = cJSON_GetObjectItem(pcJsonParam, JK_Txn_ReqData);
        if (NULL != pcSubItem && cJSON_String == pcSubItem->type && 0 != strlen(pcSubItem->valuestring))
        {
            cAmtParam.nTxnReqDataLength = strlen(pcSubItem->valuestring);
            memcpy(cAmtParam.szTxnRequestData, pcSubItem->valuestring, cAmtParam.nTxnReqDataLength);
            cAmtParam.szTxnRequestData[cAmtParam.nTxnReqDataLength] = 0x00;
        }
        else
        {
            TraceMsg("Info: TxnRequestData is null or empty.");
        }
    }
    
    InitTagParam(cTagParam, kTxnAmountData, cAmtParam);
    if (NULL != m_pParamPtr)
    {
        int nRet = m_pParamPtr->SetParameter(cTagParam);
        if (nRet)
        {
            TraceMsg("Error: Call SetParameter set kTxnAmountData failed.");
        }
    }
    else
    {
        TraceMsg("Error: m_pParamPtr is NULL.");
    }
}

/*
 * Private.
 */
void CHPApiKernel::WaitScCardRemove(void)
{
    while (TRUE)
    {
        BYTE byPresent; 

        CTOS_SCStatus(d_SC_USER, &byPresent);
        if (d_MK_SC_PRESENT == byPresent)
        {
            usleep(500 * 1000);
            continue;
        }
        break;
    }
}

/*
 * Private.
 */
int CHPApiKernel::ExeTxnOp(IN const int &nDetectedCardType)
{
    TraceMsg("ExtTxnOp--Entry");
    int nLoop = 0;
    int nRet = -1;
    int nFallback = 0;
    int nCardType = nDetectedCardType;
    CTxnBase *pTxnPtr = NULL;
    // const cJSON *pcJsonParam = m_cCurReqCmd.GetJsonObjPtr();
    SetCurKeState(kStartTxn); // Set begin txn state.

    int nStrategyFlag = 0;
    int nRePollingCard = 0;

    tagParam cTagParam = { 0 };
    HEARTLAND_GENERAL_PARAM cGeneralParam = { 0 };

    InitTagParam(cTagParam, kGeneralParam, cGeneralParam);
    m_pParamPtr->GetParameter(&cTagParam);
    if(strcasecmp(cGeneralParam.szStrategyFlag, "cooler") == 0)
    {
        nStrategyFlag = 1;
    }

    do
    {
        nLoop = 0;
        
        // Init Txn param
        // TraceMsg("Info: SetTxnParam Beg");
        // SetTxnParam(pcJsonParam);
        // TraceMsg("Info: SetTxnParam End");

        // Execute Txn Operation
        pTxnPtr = GetTxnPtr(nCardType);
        if (NULL == pTxnPtr)
        {
            TraceMsg("Error: NULL");
            break;
        }

        SetCurKeState(kProcessingTxn);

        TraceMsg("Info: PerformTxn Beg");
        nRet = pTxnPtr->PerformTxn();
        TraceMsg("Info: PerformTxn End");
        ShowResult();
        TraceMsg("Info: ShowResult End");
        
        unsigned int uCardSet = 0;
        unsigned int uTxnState = CTxnBase::GetState();
        if (uTxnState == TXN_STATE_PLEASE_INSERT_CARD)
        {
            nFallback = 1;
            uCardSet |= kEmv;
            uCardSet |= kMSR;
			uCardSet |= kEmvCL;
            ExeClearLastDetectInfo();
            nLoop = ExeDetectCard(&nCardType, kProcessingTxn, uCardSet);
        }

        if (uTxnState == TXN_STATE_PLEASE_SWIPE_CARD)
        {
            nFallback = 1;
            uCardSet |= kMSR;
            uCardSet |= kEmv;
			uCardSet |= kEmvCL;

            ExeClearLastDetectInfo();
            WaitScCardRemove();
            nLoop = ExeDetectCard(&nCardType, kProcessingTxn, uCardSet);
        }
		else if(uTxnState == TXN_STATE_PLEASE_SEE_PHONE && pTxnPtr->IsInteracOfEMVCL())
        {
            uCardSet |= kEmvCL;
            ExeClearLastDetectInfo();
            sleep(2);//display SEE PHONE UI 2 second
            CTxnBase::SetState(TXN_STATE_PLEASE_TAP_CARD);
            nLoop = ExeDetectCard(&nCardType, kProcessingTxn, uCardSet);
        }
        else if ((uTxnState==TXN_STATE_DECLINED || uTxnState==TXN_STATE_READING_CARD_FAIL)
            && nStrategyFlag==1
            && nRePollingCard<2)
        {
            uCardSet |= kMSR;
            uCardSet |= kEmv;
            uCardSet |= kEmvCL;
            nRePollingCard++;
            ExeClearLastDetectInfo();

            // pause 2s
            sleep(2);

            if (nCardType==kEmv)
            {
                CTxnBase::SetState(TXN_STATE_PLEASE_REMOVE_CARD);
                WaitScCardRemove();
            }
            nLoop = ExeDetectCard(&nCardType, kStartDetectingCard, uCardSet);
        }        
		UpdateTxnEntryMode(nCardType);


    } while (nLoop);

    if (nFallback == 1)
    {
        UpdateTxnEntryMode(kMSR_Fallback);
    }

    // Set Finish Txn State.
    SetCurKeState(kFinishTxn);
    TraceMsg("ExtTxnOp--Exit");
    return nRet;
}

void CHPApiKernel::UpdateTxnEntryMode(IN const int &nCardType)
{
    switch (nCardType)
    {
    case kMSR:
        SetTxnEntryMode(kSwipe);
        break;

    case kEmv:
        SetTxnEntryMode(kInsert);
        break;

    case kEmvCL:
        SetTxnEntryMode(kTap);
        break;

    case kNoneCard: 
        SetTxnEntryMode(kNone);
        break;

    case kMSR_Fallback: 
        SetTxnEntryMode(kFallback);
        break;

    default:
        break;
    }
}

void CHPApiKernel::ExeCurReqTxnCmd(void)
{
    TraceMsg("Info: ExeCurReqTxnCmd--Begin");
    int nCardType = kNoneCard;
    BOOL bDetectResult = FALSE;
    ResetProcessingStatus();
    ExeClearLastDetectInfo();
    ResetTxnEntryMode();

    const cJSON *pcJsonParam = m_cCurReqCmd.GetJsonObjPtr();
    do
    {
        if (!pcJsonParam)
        {
            TraceMsg("Info: request json package is empty");
            break;
        }
        
        // Init Txn param
        TraceMsg("Info: SetTxnParam Beg");
        SetTxnParam(pcJsonParam);
        TraceMsg("Info: SetTxnParam End");

        if (IsNeedDetectCard())
        {
            AddProcessingStatus(ProcStatus_StartPollingCard);
            bDetectResult = ExeDetectCard(&nCardType);
            if (!bDetectResult)
                break;
        }

        UpdateTxnEntryMode(nCardType);
        TraceMsg("Info: Finish Detected Card.");

        AddProcessingStatus(ProcStatus_StartTxn);
        const int nTxnRet = ExeTxnOp(nCardType);
        AddProcessingStatus(ProcStatus_FinishTxn);
        
        if (TXN_STATE_APPROVAL == GetUiIndexByTxnState())
        {
            AddProcessingStatus(ProcStatus_TxnApproval);
        }
        else if (TXN_STATE_DECLINED == GetUiIndexByTxnState())
        {
            AddProcessingStatus(ProcStatus_TxnDecline);
        }
        TraceMsg("Info: ExeTxnOp return Ret=%d", nTxnRet);
    } while (0);

    TraceMsg("Info: ExeCurReqTxnCmd--End");
}

/*
 * Protected.
 */
void CHPApiKernel::UpdateApiMode(void)
{
    if (CTOS_TickGet() > GetCurStateTimeout())
    {
        if (kInitHLLibraryFailed == GetCurKeState() ||
            kNoSendInitJsonCmd == GetCurKeState() ||
            kInvalid == GetCurKeState())
        {
            SetCurKeState(kInvalid);
            SetCurApiMode(kWaitingNewJsonCmd);
            SetCurStateTimeout(0xFFFFFFFF);
            TraceMsg("Info: ApiMode return wait, keState return invalid.");
        }
        else if (kDetectCardTimeout == GetCurKeState() ||
                 kCancelDetectCard == GetCurKeState() ||
                 kFinishTxn == GetCurKeState() ||
                 kFinishCancelTxn == GetCurKeState() ||
                 kIdle == GetCurKeState() || 
                 kFinishUpdateData == GetCurKeState())
        {
            SetCurKeState(kIdle);
            SetCurApiMode(kWaitingNewJsonCmd);
            SetCurStateTimeout(0xFFFFFFFF);
            TraceMsg("Info: ApiMode return wait, keState return idle.");
            TraceMsg("Info: ApiMode %d, CurKeState=%d.", GetCurApiMode(), GetCurKeState());
        }
        else
        {
            DumpKernelInfo();
        }
    }
}
    

/*
 * Private.
 */
#define _ShowTransResult(tag, tlvBuff, tlvBuffSize, data, length, error)        \
    {                                                                           \
        error = CTlvFun::TLVListReadTagValue(tag, tlvBuff, tlvBuffSize,         \
                &data, &length);                                                \
        if (error == 0)                                                         \
        {                                                                       \
            char szTmpBuff[256] = { 0 };                                        \
            memset(szTmpBuff, 0, sizeof(szTmpBuff));                            \
            memcpy(szTmpBuff, data, length);                                    \
            TraceMsg("Error: _ShowTransResult error, Buf=%s", szTmpBuff);       \
        }                                                                       \
    }


/*
 * Private.
 */
void CHPApiKernel::ShowResult(void)
{
#if 0
    tagParam cTagParam = { 0 };
    HEARTLAND_TRANS_PARAM cTxnParam = { 0 };
    
    InitTagParam(cTagParam, kHLTransParam, cTxnParam);
    m_pParamPtr->GetParameter(&cTagParam);
    
    BYTE*   pbyTlvValue = NULL;
    WORD    wTlvValueLen = 0;
    WORD    wError;
    return ;

    _ShowTransResult(d_TAG_TOTAL_SALE_COUNT, cTxnParam.bResult, cTxnParam.bResultLen, pbyTlvValue, wTlvValueLen, wError)
    _ShowTransResult(d_TAG_TOTAL_SALE_AMOUNT, cTxnParam.bResult, cTxnParam.bResultLen, pbyTlvValue, wTlvValueLen, wError)
    _ShowTransResult(d_TAG_BATCH_IDENTITY, cTxnParam.bResult, cTxnParam.bResultLen, pbyTlvValue, wTlvValueLen, wError)
    _ShowTransResult(d_TAG_TRANSACTION_IDENTITY, cTxnParam.bResult, cTxnParam.bResultLen, pbyTlvValue, wTlvValueLen, wError)
    _ShowTransResult(d_TAG_RETRIEVAL_REFERENCE_NUMBER, cTxnParam.bResult, cTxnParam.bResultLen, pbyTlvValue, wTlvValueLen, wError)
    _ShowTransResult(d_TAG_APPROVED_CODE, cTxnParam.bResult, cTxnParam.bResultLen, pbyTlvValue, wTlvValueLen, wError)
    _ShowTransResult(d_TAG_TRANS_TOKEN_VALUE, cTxnParam.bResult, cTxnParam.bResultLen, pbyTlvValue, wTlvValueLen, wError)
    _ShowTransResult(d_TAG_TXN_RESULT, cTxnParam.bResult, cTxnParam.bResultLen, pbyTlvValue, wTlvValueLen, wError)
    _ShowTransResult(d_TAG_TRANS_RESPONSE_CODE, cTxnParam.bResult, cTxnParam.bResultLen, pbyTlvValue, wTlvValueLen, wError)
    _ShowTransResult(d_TAG_TRANS_RESPONSE_TXT, cTxnParam.bResult, cTxnParam.bResultLen, pbyTlvValue, wTlvValueLen, wError)
    _ShowTransResult(d_TAG_RESPONSE_CODE, cTxnParam.bResult, cTxnParam.bResultLen, pbyTlvValue, wTlvValueLen, wError)
    _ShowTransResult(d_TAG_RESPONSE_TXT, cTxnParam.bResult, cTxnParam.bResultLen, pbyTlvValue, wTlvValueLen, wError)
#endif
}


/*
 * private.
 */
CTxnBase* CHPApiKernel::GetTxnPtr(IN const int nCardType)
{
    CTxnBase* pTxnObj = NULL;
    switch(nCardType)
    {
        case kNoneCard :
            pTxnObj = HLKE_GetNoneCardTxnPtr();
            break;
        
        case kEmv :
            pTxnObj = HLKE_GetEMVTxnPtr();
            break;
        
        case kEmvCL :
            pTxnObj = HLKE_GetEMVCLTxnPtr();
            break;
        
        case kMSR :
            pTxnObj = HLKE_GetMSRTxnPtr();
            break;
        
        default:
            TraceMsg("Error: Invalid cardtype.");
            break;
    }
    return pTxnObj;
}

/*
 * Public.
 */
void CHPApiKernel::RequsetUserAction(IN const unsigned int &nActionType,
                                     IN const void *pData,
                                     IN const size_t &nDataSize)
{
    TraceMsg("RequsetUserAction entry.");
    switch (nActionType)
    {
    case TXN_STATE_AID_SELECT:
    case TXN_STATE_AID_CONFIRM:
    case TXN_STATE_TOTAL_AMOUNT_GET:
    case TXN_STATE_EXCEPTION_FILE_CHECK:
    case TXN_STATE_CALL_YOUR_BANK:
    case TXN_STATE_CONFIRM_COMM_WITH_HOST:
    case TXN_STATE_LAST_PIN_INPUT:
    case TXN_STATE_PRINT_SIGNATURE:
    case TXN_STATE_INTERAC_POS_SEQNBR:
    case TXN_STATE_INTERAC_ACCOUNT_TYPE:
    case TXN_STATE_SELECT_LANGUAGE:
    case TXN_STATE_SWITCH_LANGUAGE:
        {
            NormalActionRequest(nActionType, pData, nDataSize);
            SetCurApiMode(kWaitingUserInputCmd);
        }
        break;

    default:
        DefActionRequest(nActionType, pData, nDataSize);
        break;
    }

    EnableCancel(TRUE);
    TraceMsg("RequsetUserAction exit.");
}

/*
 * Public.
 */
void CHPApiKernel::WaitUserResponse(IN const unsigned int &nActionType,
                                    OUT void *pBuffer,
                                    IN const size_t &nBufferSize)
{
    TraceMsg("WaitUserResponse Entry.");
    switch (nActionType)
    {
    case TXN_STATE_AID_SELECT:
    case TXN_STATE_AID_CONFIRM:
    case TXN_STATE_TOTAL_AMOUNT_GET:
    case TXN_STATE_EXCEPTION_FILE_CHECK:
    case TXN_STATE_CALL_YOUR_BANK:
    case TXN_STATE_CONFIRM_COMM_WITH_HOST:
    case TXN_STATE_LAST_PIN_INPUT: 
    case TXN_STATE_PRINT_SIGNATURE:
    case TXN_STATE_INTERAC_POS_SEQNBR:
    case TXN_STATE_INTERAC_ACCOUNT_TYPE:
    case TXN_STATE_SELECT_LANGUAGE:
    case TXN_STATE_SWITCH_LANGUAGE:
        {
            NormalActionResponse(nActionType, pBuffer, nBufferSize);
        }
        break;

    default:
        DefActionResponse(nActionType, pBuffer, nBufferSize);
        break;
    }
    
    EnableCancel(FALSE);
    TraceMsg("WaitUserResponse Exit.");
}

/*
 * Private.
 */
void CHPApiKernel::NormalActionRequest(IN const unsigned int &nActionType,
                                       IN const void *pBuffer,
                                       IN const size_t &nBufferSize)
{
    TraceMsg("NormalActionRequest---Begin");

    LockDataAccess();
    bzero((void *)&m_cActReqData, sizeof(m_cActReqData));
    memcpy(m_cActReqData.m_szActData, pBuffer, nBufferSize);
    m_cActReqData.m_nActType = nActionType;
    UnlockDataAccess();

    SetCurKeState(kUserActionType);
    TraceMsg("NormalActionRequest--Exit");
}

/*
 * Private.
 */
BOOL CHPApiKernel::IsActRspArrive(void)
{
    return (kFinishProcessingUserInputCmd == GetCurApiMode());
}

/*
 * Private.
 */
void CHPApiKernel::NormalActionResponse(IN const unsigned int &nActionType,
                                        OUT void *pBuffer,
                                        IN const size_t &nBufferSize)
{
    TraceMsg("NormalActionResponse--Entry");
    unsigned int dwCount = 0;
    unsigned int dwTimeout = GetAIDSelectTimeout();
    unsigned long ulStart = GetTickCount();
    while (!IsActRspArrive())
    {
        if ((GetTickCount()-ulStart>dwTimeout) && 
            nActionType==CJsonReqCmd::StringToActType(JV_Act_AidList))
        {
            TraceMsg("Info: AID Select Timeout");
            break;
        }
        usleep(Nop_WaitUserRspArrive_);


        if (dwCount++ % 10 == 0)
        {
            TraceMsg("Info: Wait user response.");
        }
    }
    LockDataAccess();
    bzero(pBuffer, nBufferSize);
    if (strlen(m_cActRspData.m_szActData) <= (nBufferSize - 1))
    {
        memcpy(pBuffer, m_cActRspData.m_szActData, strlen(m_cActRspData.m_szActData));
    }
    UnlockDataAccess();

    SetCurApiMode(kStartProcessingJsonCmd);
    
    LockDataAccess();
    ResetActData();
    UnlockDataAccess();

    SetCurKeState(kProcessingTxn);
    TraceMsg("NormalActionResponse--Exit");
}

/*
 * Private.
 */
void CHPApiKernel::DefActionRequest(IN const unsigned int &nActType,
                                    IN const void *pVoid,
                                    IN const size_t &nSize)
{
    TraceMsg("DefActionRequest--Entry");

    LockDataAccess();
    ResetActData();
    m_cActReqData.m_nActType = nActType;
    UnlockDataAccess();

    SetCurKeState(kUserActionType);
    TraceMsg("DefActionRequest--Exit");
}

/*
 * Private.
 */
void CHPApiKernel::DefActionResponse(IN const unsigned int &nActType,
                                     OUT void *pBuffer,
                                     IN const size_t &nBufferSize)
{
    TraceMsg("DefActionResponse--Entry");

    LockDataAccess();
    ResetActData();
    UnlockDataAccess();

    SetCurKeState(kProcessingTxn);
    TraceMsg("DefActionResponse--Exit");
}


unsigned short CHPApiKernel::IsTxnSeqNbrMatched(void)
{
    TraceMsg("Info: %s Entry", __FUNCTION__);

    tagParam cTagParam = { 0 };
    HEARTLAND_AMOUNT_PARAM amountParam;

    InitTagParam(cTagParam, kTxnAmountData, amountParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet)
    {
        TraceMsg("Error: Call IsTxnSeqNbrMatched get HEARTLAND_AMOUNT_PARAM failed.");
        return 0;
    }

    HEARTLAND_POLLINGCARD_CACHE pollData = { 0 };
    InitTagParam(cTagParam, kPollingCardCache, pollData);
    nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet)
    {
        TraceMsg("Error: Call IsTxnSeqNbrMatched get HEARTLAND_POLLINGCARD_CACHE failed.");
        return 0;
    }

    if (pollData.ulSequenceNbr != amountParam.ulSeqeueceId 
        || amountParam.ulSeqeueceId == 0 
        || pollData.ulSequenceNbr == 0 
        || pollData.usCardEntryMode == kNoneCard 
        || pollData.usCardEntryMode == kMSR_Fallback)
    {
        TraceMsg("Error: pollData.ulSequenceNbr:%lu, amountParam.ulSeqeueceId:%lu, pollData.usCardEntryMode:%d mismatch condition", 
                pollData.ulSequenceNbr, amountParam.ulSeqeueceId, pollData.usCardEntryMode);
        return 0;
    }

    TraceMsg("Info: pollData.ulSequenceNbr:%lu, amountParam.ulSeqeueceId:%lu", pollData.ulSequenceNbr, amountParam.ulSeqeueceId);
    TraceMsg("Info: %s Exit, and Sequence Nbr is matched ", __FUNCTION__);
    return pollData.usCardEntryMode;
}


DWORD CHPApiKernel::GetAIDSelectTimeout(void)
{
    static DWORD dwTimeout = 0;

    if (dwTimeout != 0)
    {
        TraceMsg("Info: AID Select Timeout:%u", dwTimeout);
        return dwTimeout;
    }

    
    tagParam cTagParam = { 0 };
    HEARTLAND_GENERAL_PARAM cGeneralParam = { 0 };

    InitTagParam(cTagParam, kGeneralParam, cGeneralParam);
    m_pParamPtr->GetParameter(&cTagParam);

    if (strlen(cGeneralParam.szAIDSelectTimeout)==0 || atol(cGeneralParam.szAIDSelectTimeout)==0)
    {
        dwTimeout = 0xFFFFFFFF;
    }
    else
    {
        dwTimeout = atol(cGeneralParam.szAIDSelectTimeout);
        dwTimeout *= 1000;
    }
    
    TraceMsg("Info: AID Select Timeout:%u", dwTimeout);
    return dwTimeout;
}